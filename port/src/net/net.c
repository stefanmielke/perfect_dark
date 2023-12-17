#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "platform.h"
#include "net/netenet.h"
#include "net/net.h"
#include "net/netbuf.h"
#include "net/netmsg.h"
#include "types.h"
#include "constants.h"
#include "data.h"
#include "bss.h"
#include "game/chraction.h"
#include "game/bot.h"
#include "game/botact.h"
#include "config.h"
#include "system.h"
#include "utils.h"

s32 g_NetMode = NETMODE_NONE;

u32 g_NetTick = 0;

u32 g_NetInterpTicks = 6;
u32 g_NetExterpTicks = 6;

s32 g_NetSimPacketLoss = 20;

s32 g_NetMaxClients = NET_MAX_CLIENTS;
s32 g_NetNumClients = 0;
struct netclient g_NetClients[NET_MAX_CLIENTS + 1]; // last is an extra temporary client
struct netclient *g_NetLocalClient = &g_NetClients[NET_MAX_CLIENTS];

static u8 g_NetMsgBuf[NET_BUFSIZE];
struct netbuf g_NetMsg = { .data = g_NetMsgBuf, .size = sizeof(g_NetMsgBuf) };

static u8 g_NetMsgRelBuf[NET_BUFSIZE];
struct netbuf g_NetMsgRel = { .data = g_NetMsgRelBuf, .size = sizeof(g_NetMsgRelBuf) };

static s32 g_NetInit = false;
static ENetHost *g_NetHost;
static ENetAddress g_NetLocalAddr;
static ENetAddress g_NetRemoteAddr;

static s32 netParseAddr(ENetAddress *out, const char *str)
{
	char tmp[256] = { 0 };

	if (!str || !str[0]) {
		return false;
	}

	strncpy(tmp, str, sizeof(tmp) - 1);

	char *host = tmp;
	char *port = NULL;

	if (tmp[0] == '[') {
		// ipv6 with port: [ADDR]:PORT
		host = tmp + 1; // skip [
		port = strrchr(host, ']'); // find ]
		if (port) {
			if (port[1] != ':' || !isdigit(port[2])) {
				return false;
			}
			*port = '\0'; // terminate ip
			port += 2; // skip ]:
		}
	} else if (strchr(host, '.')) {
		// ipv4 or hostname
		port = strrchr(host, ':');
		if (port) {
			if (!isdigit(port[1])) {
				return false;
			}
			*port = '\0'; // terminate address
			++port; // skip :
		}
	}

	if (!host[0]) {
		return false;
	}

	const s32 portval = port ? atoi(port) : NET_DEFAULT_PORT;
	if (portval < 0 || portval > 0xFFFF) {
		return false;
	}

	memset(out, 0, sizeof(*out));
	out->port = portval;

	if (isdigit(host[0]) || strchr(host, ':')) {
		// we stripped off the :PORT at this point and domain names can't start with a digit
		// so this must be an IP address
		return (enet_address_set_ip(out, host) == 0);
	}

	// must be a domain name; do a lookup
	return (enet_address_set_hostname(out, host) == 0);
}

static const char *netFormatAddr(const ENetAddress *addr)
{
	static char str[256];
	char tmp[256];
	if (addr && enet_address_get_ip(addr, tmp, sizeof(tmp) - 1) == 0) {
		if (tmp[0]) {
			if (strchr(tmp, ':')) {
				// ipv6
				snprintf(str, sizeof(str) - 1, "[%s]:%u", tmp, addr->port);
			} else {
				// ipv4
				snprintf(str, sizeof(str) - 1, "%s:%u", tmp, addr->port);
			}
			return str;
		}
	}
	return NULL;
}

static inline const char *netFormatPeerAddr(const ENetPeer *peer)
{
	return netFormatAddr(&peer->address);
}

const char *netFormatClientAddr(const struct netclient *cl)
{
	return cl->peer ? netFormatPeerAddr(cl->peer) : "<local>";
}

static inline void netClientReset(struct netclient *cl)
{
	memset(cl, 0, sizeof(*cl));
	cl->out.data = cl->out_data;
	cl->out.size = sizeof(cl->out_data);
	cl->id = cl - g_NetClients;
}

static inline void netClientResetAll(void)
{
	g_NetMaxClients = NET_MAX_CLIENTS;
	g_NetNumClients = 1; // always at least one client, which is us
	for (u32 i = 0; i < NET_MAX_CLIENTS + 1; ++i) {
		netClientReset(&g_NetClients[i]);
	}
}

static inline void netClientRecordMove(struct netclient *cl, const struct player *pl)
{
	// make space in the move stack
	memmove(cl->outmove + 1, cl->outmove, sizeof(cl->outmove) - sizeof(*cl->outmove));

	struct netplayermove *move = &cl->outmove[0];

	move->tick = g_NetTick;
	move->crouchofs = pl->crouchoffset;
	move->leanofs = pl->swaytarget / 75.f;
	move->movespeed[0] = pl->speedforwards;
	move->movespeed[1] = pl->speedsideways;
	move->lookspeed[0] = pl->speedtheta;
	move->lookspeed[1] = pl->speedverta;
	move->angles[0] = pl->vv_theta;
	move->angles[1] = pl->vv_verta;
  move->pos = (pl->prop) ? pl->prop->pos : pl->cam_pos;
	move->rooms[0] = pl->cam_room;
	move->rooms[1] = pl->floorroom;
	move->ucmd = pl->ucmd;

	if (pl->crouchpos == CROUCHPOS_DUCK) {
		move->ucmd |= UCMD_DUCK;
	} else if (pl->crouchpos == CROUCHPOS_SQUAT) {
		move->ucmd |= UCMD_SQUAT;
	}

	if (pl->bondactivateorreload & JO_ACTION_ACTIVATE) {
		move->ucmd |= UCMD_ACTIVATE;
	}
	if (pl->bondactivateorreload & JO_ACTION_RELOAD) {
		move->ucmd |= UCMD_RELOAD;
	}

	if (cl != g_NetLocalClient && !cl->forcetick && (move->ucmd & UCMD_FL_FORCEMASK)) {
		cl->forcetick = move->tick;
		sysLogPrintf(LOG_NOTE, "NET: forcing client %u to move at tick %u", cl->id, cl->forcetick);
	}
}

void netInit(void)
{
	if (enet_initialize() < 0) {
		sysLogPrintf(LOG_ERROR, "NET: could not init ENet, disabling networking");
		return;
	}

	g_NetInit = true;
}

s32 netStartServer(u16 port, s32 maxclients)
{
	if (g_NetMode || !g_NetInit) {
		return -1;
	}

	memset(&g_NetLocalAddr, 0, sizeof(g_NetLocalAddr));
	g_NetLocalAddr.port = port;
	g_NetHost = enet_host_create(&g_NetLocalAddr, maxclients, NETCHAN_COUNT, 0, 0, 0);
	if (!g_NetHost) {
		sysLogPrintf(LOG_ERROR, "NET: could not create ENet host");
		return -2;
	}

	netClientResetAll();

	// the server's local client is client 0
	g_NetLocalClient = &g_NetClients[0];
	g_NetLocalClient->state = CLSTATE_LOBBY; // local client doesn't need auth
	g_NetLocalClient->settings.bodynum = g_PlayerConfigsArray[0].base.mpbodynum;
	g_NetLocalClient->settings.headnum = g_PlayerConfigsArray[0].base.mpheadnum;
	memcpy(g_NetLocalClient->settings.name, g_PlayerConfigsArray[0].base.name, sizeof(g_NetLocalClient->settings.name));

	g_NetMode = NETMODE_SERVER;

	g_NetTick = 0;

	sysLogPrintf(LOG_NOTE, "NET: created server on port %u", port);

	return 0;
}

void netServerStageStart(void)
{
	if (g_NetMode != NETMODE_SERVER) {
		return;
	}

	g_NetLocalClient->state = CLSTATE_GAME;

	netbufStartWrite(&g_NetMsgRel);
	netmsgSvcStageStartWrite(&g_NetMsgRel);
	netSend(NULL, &g_NetMsgRel, true);
}

void netServerStageEnd(void)
{
	if (g_NetMode != NETMODE_SERVER) {
		return;
	}

	g_NetLocalClient->state = CLSTATE_LOBBY;
	netbufStartWrite(&g_NetMsgRel);
	netmsgSvcStageEndWrite(&g_NetMsgRel);
	netSend(NULL, &g_NetMsgRel, true);
}

s32 netStartClient(const char *addr)
{
	if (g_NetMode || !g_NetInit) {
		return -1;
	}

	if (!netParseAddr(&g_NetRemoteAddr, addr)) {
		sysLogPrintf(LOG_ERROR, "NET: `%s` is not a valid address", addr);
		return -2;
	}

	memset(&g_NetLocalAddr, 0, sizeof(g_NetLocalAddr));
	g_NetHost = enet_host_create(&g_NetLocalAddr, 1, NETCHAN_COUNT, 0, 0, 0);
	if (!g_NetHost) {
		sysLogPrintf(LOG_ERROR, "NET: could not create ENet host");
		return -3;
	}

	// we'll use the whole array to store what we know of other clients
	netClientResetAll();

	// for now use last client struct
	g_NetLocalClient = &g_NetClients[NET_MAX_CLIENTS];

	sysLogPrintf(LOG_NOTE, "NET: connecting to %s...", addr);

	g_NetLocalClient->peer = enet_host_connect(g_NetHost, &g_NetRemoteAddr, NETCHAN_COUNT, NET_PROTOCOL_VER);
	if (!g_NetLocalClient->peer) {
		sysLogPrintf(LOG_WARNING, "NET: could not connect to %s", addr);
		enet_host_destroy(g_NetHost);
		g_NetHost = NULL;
		return -4;
	}

	g_NetLocalClient->state = CLSTATE_CONNECTING;
	g_NetLocalClient->settings.bodynum = g_PlayerConfigsArray[0].base.mpbodynum;
	g_NetLocalClient->settings.headnum = g_PlayerConfigsArray[0].base.mpheadnum;
	memcpy(g_NetLocalClient->settings.name, g_PlayerConfigsArray[0].base.name, sizeof(g_NetLocalClient->settings.name));

	g_NetMode = NETMODE_CLIENT;

	g_NetTick = 0;

	sysLogPrintf(LOG_NOTE, "NET: waiting for response from %s...", addr);

	return 0;
}

s32 netDisconnect(void)
{
	if (!g_NetMode) {
		return -1;
	}

	for (s32 i = 0; i < NET_MAX_CLIENTS + 1; ++i) {
		if (g_NetClients[i].peer) {
			enet_peer_disconnect_now(g_NetClients[i].peer, DISCONNECT_SHUTDOWN);
		}
		netClientReset(&g_NetClients[i]);
	}

	g_NetLocalClient = &g_NetClients[NET_MAX_CLIENTS];

	// flush pending packets
	enet_host_flush(g_NetHost);

	// service for a bit just to ensure disconnect gets to peer(s)
	enet_host_service(g_NetHost, NULL, 10);

	enet_host_destroy(g_NetHost);

	g_NetHost = NULL;
	g_NetMode = NETMODE_NONE;

	sysLogPrintf(LOG_NOTE, "NET: disconnected");

	return 0;
}

static void netServerEvConnect(ENetPeer *peer, const u32 data)
{
	const char *addrstr = netFormatPeerAddr(peer);

	sysLogPrintf(LOG_NOTE, "NET: connection attempt from %s", addrstr);

	if (data != NET_PROTOCOL_VER) {
		sysLogPrintf(LOG_NOTE, "NET: %s rejected: protocol mismatch", addrstr);
		enet_peer_disconnect(peer, DISCONNECT_VERSION);
		return;
	}

	struct netclient *cl = NULL;

	// id 0 is the local client
	for (s32 i = 1; i < g_NetMaxClients; ++i) {
		if (!g_NetClients[i].state) {
			cl = &g_NetClients[i];
			break;
		}
	}

	if (!cl) {
		sysLogPrintf(LOG_NOTE, "NET: %s rejected: server is full", addrstr);
		enet_peer_disconnect(peer, DISCONNECT_FULL);
		return;
	}

	netClientReset(cl);
	cl->state = CLSTATE_AUTH; // skip CLSTATE_CONNECTING, since we already know it connected
	cl->peer = peer;
	enet_peer_set_data(peer, cl);

	++g_NetNumClients;
}

static void netServerEvDisconnect(struct netclient *cl)
{
	sysLogPrintf(LOG_NOTE, "NET: disconnect event from %s", netFormatClientAddr(cl));

	if (cl->peer) {
		enet_peer_reset(cl->peer);
	}

	netClientReset(cl);

	--g_NetNumClients;
}

static void netServerEvReceive(struct netclient *cl)
{
	u32 rc = 0;
	u8 msgid = 0;

	while (!rc && netbufReadLeft(&cl->in) > 0) {
		msgid = netbufReadU8(&cl->in);
		switch (msgid) {
			case CLC_NOP: rc = 0; break;
			case CLC_AUTH: rc = netmsgClcAuthRead(&cl->in, cl); break;
			case CLC_CHAT: rc = 0; break;
			case CLC_MOVE: rc = netmsgClcMoveRead(&cl->in, cl); break;
			default:
				rc = 1;
				break;
		}
	}

	if (rc) {
		sysLogPrintf(LOG_WARNING, "NET: malformed or unknown message 0x%02x from client %s", msgid, netFormatClientAddr(cl));
	}
}

static void netClientEvConnect(const u32 data)
{
	sysLogPrintf(LOG_NOTE, "NET: connected to server, sending CLC_AUTH");

	g_NetLocalClient->state = CLSTATE_AUTH;

	// send auth request
	netbufStartWrite(&g_NetMsgRel);
	netmsgClcAuthWrite(&g_NetMsgRel);
	netSend(g_NetLocalClient, &g_NetMsgRel, true);
}

static void netClientEvDisconnect(const u32 reason)
{
	sysLogPrintf(LOG_NOTE, "NET: kicked from server, reason: %u", reason);
	netDisconnect();
}

static void netClientEvReceive(struct netclient *cl)
{
	u32 rc = 0;
	u8 msgid = 0;

	while (!rc && netbufReadLeft(&cl->in) > 0) {
		msgid = netbufReadU8(&cl->in);
		switch (msgid) {
			case SVC_NOP: rc = 0; break;
			case SVC_AUTH: rc = netmsgSvcAuthRead(&cl->in, cl); break;
			case SVC_CHAT: rc = 0; break;
			case SVC_STAGE_START: rc = netmsgSvcStageStartRead(&cl->in, cl); break;
			case SVC_STAGE_END: rc = netmsgSvcStageEndRead(&cl->in, cl); break;
			case SVC_PLAYER_MOVE: rc = netmsgSvcPlayerMoveRead(&cl->in, cl); break;
			default:
				rc = 1;
				break;
		}
	}

	if (rc) {
		sysLogPrintf(LOG_WARNING, "NET: malformed or unknown message 0x%02x from server", msgid);
	}
}

void netStartFrame(void)
{
	if (!g_NetMode) {
		return;
	}

	++g_NetTick;

	const bool isClient = (g_NetMode == NETMODE_CLIENT);
	s32 polled = false;
	ENetEvent ev = { .type = ENET_EVENT_TYPE_NONE };
	while (!polled) {
		if (enet_host_check_events(g_NetHost, &ev) <= 0) {
			if (enet_host_service(g_NetHost, &ev, 1) <= 0) {
				break;
			}
			polled = true;
		}

		switch (ev.type) {
			case ENET_EVENT_TYPE_CONNECT:
				if (isClient) {
					netClientEvConnect(ev.data);
				} else if (ev.peer) {
					netServerEvConnect(ev.peer, ev.data);
				}
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
			case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
				if (isClient) {
					netClientEvDisconnect(ev.type == ENET_EVENT_TYPE_DISCONNECT_TIMEOUT ? DISCONNECT_TIMEOUT : ev.data);
				} else if (ev.peer) {
					struct netclient *cl = enet_peer_get_data(ev.peer);
					if (cl) {
						netServerEvDisconnect(cl);
					} else {
						sysLogPrintf(LOG_WARNING, "NET: disconnect from %s without attached client", netFormatPeerAddr(ev.peer));
					}
				}
				break;
			case ENET_EVENT_TYPE_RECEIVE:
				if (ev.peer) {
					struct netclient *cl = (g_NetMode == NETMODE_CLIENT) ? g_NetLocalClient : enet_peer_get_data(ev.peer);
					if (cl && cl->state) {
						if (ev.packet->data && ev.packet->dataLength) {
							netbufStartReadData(&cl->in, ev.packet->data, ev.packet->dataLength);
							if (isClient) {
								netClientEvReceive(cl);
							} else {
								netServerEvReceive(cl);
							}
							netbufReset(&cl->in);
						}
					} else if (!isClient) {
						sysLogPrintf(LOG_WARNING, "NET: receive from %s without attached client", netFormatPeerAddr(ev.peer));
					}
				}
				enet_packet_dispose(ev.packet);
				break;
			default:
				break;
		}
	}

	netbufStartWrite(&g_NetMsg);
	netbufStartWrite(&g_NetMsgRel);
}

void netEndFrame(void)
{
	if (!g_NetMode) {
		return;
	}

	if (g_NetLocalClient->state == CLSTATE_GAME && g_NetLocalClient->player && g_NetLocalClient->player->prop) {
		if (g_NetMode == NETMODE_CLIENT) {
			netClientRecordMove(g_NetLocalClient, g_NetLocalClient->player);
			netmsgClcMoveWrite(&g_NetMsg);
		} else {
			for (s32 i = 0; i < g_NetMaxClients; ++i) {
				struct netclient *cl = &g_NetClients[i];
				if (cl->state >= CLSTATE_GAME && cl->player) {
					netClientRecordMove(cl, cl->player);
					netmsgSvcPlayerMoveWrite(cl->forcetick ? &g_NetMsgRel : &g_NetMsg, cl);
				}
			}
		}
	}

	if (g_NetMsgRel.wp) {
		if (g_NetMsgRel.error) {
			sysLogPrintf(LOG_WARNING, "NET: reliable out buffer overflow");
		}
		netSend(NULL, &g_NetMsgRel, true);
	}

	if (g_NetMsg.wp) {
		if (g_NetMsg.error) {
			sysLogPrintf(LOG_WARNING, "NET: unreliable out buffer overflow");
		}
		netSend(NULL, &g_NetMsg, false);
	}

	enet_host_flush(g_NetHost);
}

u32 netSend(struct netclient *dstcl, struct netbuf *buf, const s32 reliable)
{
	if (g_NetMode == NETMODE_CLIENT) {
		dstcl = g_NetLocalClient;
	}

	if (buf == NULL) {
		if (dstcl) {
			buf = &dstcl->out;
		} else {
			buf = reliable ? &g_NetMsgRel : &g_NetMsg;
		}
	}

	if (reliable || !g_NetSimPacketLoss || (rand() % g_NetSimPacketLoss) == 0) {
		const u32 flags = (reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
		ENetPacket *p = enet_packet_create(buf->data, buf->wp, flags);
		if (!p) {
			sysLogPrintf(LOG_ERROR, "NET: could not alloc %u bytes for packet", buf->wp);
			return 0;
		}

		if (dstcl == NULL) {
			enet_host_broadcast(g_NetHost, NETCHAN_DEFAULT, p);
		} else {
			enet_peer_send(dstcl->peer, NETCHAN_DEFAULT, p);
		}
	}

	const u32 ret = buf->wp;

	netbufStartWrite(buf);

	return ret;
}

void netPlayersAllocate(void)
{
	for (s32 i = 0; i < g_NetMaxClients; ++i) {
		struct netclient *cl = &g_NetClients[i];
		if (cl->state < CLSTATE_LOBBY) {
			continue;
		}

		if (cl == g_NetLocalClient) {
			// we always put the local player at index 0, even client-side
			cl->playernum = 0;
		} else {
			// which means that clientside we have to put the server's player into our slot
			cl->playernum = (g_NetMode == NETMODE_CLIENT && i == 0) ? g_NetLocalClient->id : cl->id;
			// disable controls for the remote pawns and set their settings
			// TODO: backup the player configs or something
			struct mpplayerconfig *cfg = &g_PlayerConfigsArray[cl->playernum];
			cfg->controlmode = CONTROLMODE_NA;
			cfg->base.mpbodynum = cl->settings.bodynum;
			cfg->base.mpheadnum = cl->settings.headnum;
			memcpy(cfg->base.name, cl->settings.name, MAX_PLAYERNAME);
		}

		cl->player = g_Vars.players[cl->playernum];
		cl->config = &g_PlayerConfigsArray[cl->playernum];
		cl->config->client = cl;
		cl->player->client = cl;
		cl->player->isremote = (cl != g_NetLocalClient);
	}
}
