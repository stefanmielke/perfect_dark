#include <string.h>
#include "types.h"
#include "data.h"
#include "bss.h"
#include "lib/main.h"
#include "game/mplayer/mplayer.h"
#include "game/chr.h"
#include "game/chraction.h"
#include "system.h"
#include "net/net.h"
#include "net/netbuf.h"
#include "net/netmsg.h"

extern s32 g_StageNum;
extern u64 g_RngSeed;
extern u64 g_Rng2Seed;

/* utils */

static inline u32 netbufWriteRooms(struct netbuf *buf, const s16 *rooms, const s32 num) {
	for (s32 i = 0; i < num; ++i) {
		netbufWriteS16(buf, rooms[i]);
	}
	return buf->error;
}

static inline u32 netbufReadRooms(struct netbuf *buf, s16 *rooms, const s32 num) {
	for (s32 i = 0; i < num; ++i) {
		rooms[i] = netbufReadS16(buf);
	}
	return buf->error;
}

static inline u32 netbufWritePlayerMove(struct netbuf *buf, const struct netplayermove *in)
{
	netbufWriteU32(buf, in->tick);
	netbufWriteU32(buf, in->ucmd);
	netbufWriteF32(buf, in->leanofs);
	netbufWriteF32(buf, in->crouchofs);
	netbufWriteF32(buf, in->movespeed[0]);
	netbufWriteF32(buf, in->movespeed[1]);
	netbufWriteF32(buf, in->lookspeed[0]);
	netbufWriteF32(buf, in->lookspeed[1]);
	netbufWriteF32(buf, in->angles[0]);
	netbufWriteF32(buf, in->angles[1]);
	netbufWriteCoord(buf, &in->pos);
	netbufWriteRooms(buf, in->rooms, ARRAYCOUNT(in->rooms));
	return buf->error;
}

static inline u32 netbufReadPlayerMove(struct netbuf *buf, struct netplayermove *in)
{
	in->tick = netbufReadU32(buf);
	in->ucmd = netbufReadU32(buf);
	in->leanofs = netbufReadF32(buf);
	in->crouchofs = netbufReadF32(buf);
	in->movespeed[0] = netbufReadF32(buf);
	in->movespeed[1] = netbufReadF32(buf);
	in->lookspeed[0] = netbufReadF32(buf);
	in->lookspeed[1] = netbufReadF32(buf);
	in->angles[0] = netbufReadF32(buf);
	in->angles[1] = netbufReadF32(buf);
	netbufReadCoord(buf, &in->pos);
	netbufReadRooms(buf, in->rooms, ARRAYCOUNT(in->rooms));
	return buf->error;
}

/* client -> server */

u32 netmsgClcAuthWrite(struct netbuf *dst)
{
	netbufWriteU8(dst, CLC_AUTH);
	netbufWriteU8(dst, 1); // TODO: number of local players
	netbufWriteU8(dst, g_PlayerConfigsArray[0].base.mpbodynum);
	netbufWriteU8(dst, g_PlayerConfigsArray[0].base.mpheadnum);
	netbufWriteStr(dst, g_PlayerConfigsArray[0].base.name);
	return dst->error;
}

u32 netmsgClcAuthRead(struct netbuf *src, struct netclient *srccl)
{
	if (srccl->state != CLSTATE_AUTH) {
		sysLogPrintf(LOG_WARNING, "NET: CLC_AUTH from %s, who is not in CLSTATE_AUTH", netFormatClientAddr(srccl));
		return 1;
	}

	const u8 players = netbufReadU8(src);
	const u8 bodynum = netbufReadU8(src);
	const u8 headnum = netbufReadU8(src);
	char *name = netbufReadStr(src);
	if (src->error) {
		sysLogPrintf(LOG_WARNING, "NET: malformed CLC_AUTH from %s", netFormatClientAddr(srccl));
		return 1;
	}

	strncpy(srccl->settings.name, name, sizeof(srccl->settings.name) - 1);
	srccl->settings.bodynum = bodynum;
	srccl->settings.headnum = headnum;
	srccl->state = CLSTATE_LOBBY;

	sysLogPrintf(LOG_NOTE, "NET: CLC_AUTH from %s (%s), responding", netFormatClientAddr(srccl), srccl->settings.name);

	netbufStartWrite(&srccl->out);
	netmsgSvcAuthWrite(&srccl->out, srccl);
	netSend(srccl, NULL, true);

	return 0;
}

u32 netmsgClcMoveWrite(struct netbuf *dst)
{
	netbufWriteU8(dst, CLC_MOVE);
	netbufWriteU32(dst, g_NetLocalClient->inmove[0].tick);
	netbufWritePlayerMove(dst, &g_NetLocalClient->outmove[0]);
	return dst->error;
}

u32 netmsgClcMoveRead(struct netbuf *src, struct netclient *srccl)
{
	if (srccl->state != CLSTATE_GAME) {
		sysLogPrintf(LOG_WARNING, "NET: CLC_MOVE from %s, who is not in CLSTATE_GAME", netFormatClientAddr(srccl));
		return src->error;
	}

	struct netplayermove newmove;
	srccl->outmoveack = netbufReadU32(src);
	netbufReadPlayerMove(src, &newmove);

	if (!src->error) {
		// enforce teleports and such
		if (srccl->forcetick && srccl->player) {
			if (srccl->outmoveack >= srccl->forcetick) {
				// client has acknowledged our last sent move, clear the force flags
				srccl->forcetick = 0;
				srccl->player->ucmd &= ~UCMD_FL_FORCEMASK;
				sysLogPrintf(LOG_NOTE, "NET: client %u successfully forcemoved", srccl->id);
			} else {
				// client hasn't teleported yet, discard the new position from the input command
				if (srccl->player->ucmd & UCMD_FL_FORCEPOS) {
					newmove.pos = srccl->player->prop->pos;
				}
				if (srccl->player->ucmd & UCMD_FL_FORCEANGLE) {
					newmove.angles[0] = srccl->player->vv_theta;
					newmove.angles[1] = srccl->player->vv_verta;
				}
			}
		}
		// make space in the move stack
		memmove(srccl->inmove + 1, srccl->inmove, sizeof(srccl->inmove) - sizeof(*srccl->inmove));
		srccl->inmove[0] = newmove;
		srccl->lerpticks = 0;
	}

	return src->error;
}

/* server -> client */

u32 netmsgSvcAuthWrite(struct netbuf *dst, struct netclient *authcl)
{
	netbufWriteU8(dst, SVC_AUTH);
	netbufWriteU8(dst, authcl - g_NetClients);
	netbufWriteU8(dst, g_NetMaxClients);
	netbufWriteU32(dst, g_NetTick);
	return dst->error;
}

u32 netmsgSvcAuthRead(struct netbuf *src, struct netclient *srccl)
{
	if (g_NetLocalClient->state != CLSTATE_AUTH) {
		sysLogPrintf(LOG_WARNING, "NET: SVC_AUTH from server but we're not in AUTH state");
		return 1;
	}

	const u8 id = netbufReadU8(src);
	const u8 maxclients = netbufReadU8(src);
	g_NetTick = netbufReadU32(src);
	if (g_NetLocalClient->in.error || id == 0xFF || id == 0 || maxclients == 0) {
		sysLogPrintf(LOG_WARNING, "NET: malformed SVC_AUTH from server");
		return 1;
	}

	sysLogPrintf(LOG_NOTE, "NET: SVC_AUTH from server, our ID is %u", id);

	// there's at least one client, which is us, and we know maxclients as well
	g_NetMaxClients = maxclients;
	g_NetNumClients = 1;

	// we now know our proper ID, so move to the appropriate client slot and reset the old one
	g_NetLocalClient = &g_NetClients[id];
	g_NetClients[id] = g_NetClients[NET_MAX_CLIENTS];
	g_NetLocalClient->out.data = g_NetLocalClient->out_data;
	g_NetLocalClient->id = id;

	// clear out the old slot
	g_NetClients[NET_MAX_CLIENTS].id = NET_MAX_CLIENTS;
	g_NetClients[NET_MAX_CLIENTS].state = 0;
	g_NetClients[NET_MAX_CLIENTS].peer = NULL;

	// the server's client probably is in the lobby state by now
	g_NetClients[0].state = CLSTATE_LOBBY;

	g_NetLocalClient->state = CLSTATE_LOBBY;

	return src->error;
}

u32 netmsgSvcStageStartWrite(struct netbuf *dst)
{
	netbufWriteU8(dst, SVC_STAGE_START);

	netbufWriteU32(dst, g_NetTick);

	netbufWriteU64(dst, g_RngSeed);
	netbufWriteU64(dst, g_Rng2Seed);

	netbufWriteU8(dst, g_StageNum);

	// game settings
	netbufWriteU8(dst, 0); // 0 for combat sim TODO: coop/anti
	netbufWriteU8(dst, g_MpSetup.scenario);
	netbufWriteU8(dst, g_MpSetup.scorelimit);
	netbufWriteU8(dst, g_MpSetup.timelimit);
	netbufWriteU16(dst, g_MpSetup.teamscorelimit);
	netbufWriteU16(dst, g_MpSetup.chrslots);
	netbufWriteU32(dst, g_MpSetup.options);
	netbufWriteData(dst, g_MpSetup.weapons, sizeof(g_MpSetup.weapons));

	// who the fuck is in the game
	netbufWriteU8(dst, g_NetNumClients);
	for (s32 i = 0; i < g_NetMaxClients; ++i) {
		struct netclient *ncl = &g_NetClients[i];
		if (ncl->state) {
			netbufWriteU8(dst, ncl->id);
			netbufWriteU8(dst, ncl->settings.bodynum);
			netbufWriteU8(dst, ncl->settings.headnum);
			netbufWriteStr(dst, ncl->settings.name);
			ncl->state = CLSTATE_GAME;
		}
	}

	return dst->error;
}

u32 netmsgSvcStageStartRead(struct netbuf *src, struct netclient *srccl)
{
	if (srccl->state != CLSTATE_LOBBY) {
		sysLogPrintf(LOG_WARNING, "NET: SVC_STAGE from server but we're not in LOBBY state");
		return 1;
	}

	g_NetTick = netbufReadU32(src);

	g_RngSeed = netbufReadU64(src);
	g_Rng2Seed = netbufReadU64(src);

	const u8 stagenum = netbufReadU8(src);
	const u8 mode = netbufReadU8(src); // TODO: coop, anti

	g_MpSetup.stagenum = stagenum;
	g_MpSetup.scenario = netbufReadU8(src);
	g_MpSetup.scorelimit = netbufReadU8(src);
	g_MpSetup.timelimit = netbufReadU8(src);
	g_MpSetup.teamscorelimit = netbufReadU16(src);
	g_MpSetup.chrslots = netbufReadU16(src);
	g_MpSetup.options = netbufReadU32(src);
	netbufReadData(src, g_MpSetup.weapons, sizeof(g_MpSetup.weapons));
	strcpy(g_MpSetup.name, "server");

	if (src->error) {
		sysLogPrintf(LOG_WARNING, "NET: malformed SVC_STAGE from server");
		return 1;
	}

	// read players
	const u8 numplayers = netbufReadU8(src);
	if (src->error || !numplayers || numplayers > g_NetMaxClients + 1) {
		sysLogPrintf(LOG_WARNING, "NET: malformed SVC_STAGE from server");
		return 2;
	}

	for (u8 i = 0; i < numplayers; ++i) {
		const u8 id = netbufReadU8(src);
		struct netclient *ncl = &g_NetClients[id];
		if (ncl != g_NetLocalClient) {
			ncl->id = id;
			ncl->settings.bodynum = netbufReadU8(src);
			ncl->settings.headnum = netbufReadU8(src);
			char *name = netbufReadStr(src);
			if (name) {
				strncpy(ncl->settings.name, name, sizeof(ncl->settings.name) - 1);
			} else {
				sysLogPrintf(LOG_WARNING, "NET: malformed SVC_STAGE from server");
				return 3;
			}
		} else {
			// skip our own settings
			netbufReadU8(src);
			netbufReadU8(src);
			netbufReadStr(src);
		}
		ncl->state = CLSTATE_GAME;
		ncl->player = NULL;
	}

	if (src->error) {
		return src->error;
	}

	g_NetNumClients = numplayers;

	sysLogPrintf(LOG_NOTE, "NET: SVC_STAGE from server: going to stage 0x%02x with %u players", g_MpSetup.stagenum, numplayers);

	mpStartMatch();
	menuStop();

	return 0;
}

u32 netmsgSvcStageEndWrite(struct netbuf *dst)
{
	netbufWriteU8(dst, SVC_STAGE_END);

	for (s32 i = 0; i < g_NetMaxClients; ++i) {
		struct netclient *ncl = &g_NetClients[i];
		if (ncl->state) {
			ncl->state = CLSTATE_LOBBY;
			ncl->player = NULL;
			ncl->config = NULL;
		}
	}

	return dst->error;
}

u32 netmsgSvcStageEndRead(struct netbuf *src, struct netclient *srccl)
{
	for (s32 i = 0; i < g_NetMaxClients; ++i) {
		struct netclient *ncl = &g_NetClients[i];
		if (ncl->state) {
			ncl->state = CLSTATE_LOBBY;
			ncl->player = NULL;
			ncl->config = NULL;
		}
	}

	mainEndStage();

	return src->error;
}

u32 netmsgSvcPlayerMoveWrite(struct netbuf *dst, struct netclient *movecl)
{
	if (movecl->state < CLSTATE_GAME || !movecl->player || !movecl->player->prop) {
		return dst->error;
	}

	netbufWriteU8(dst, SVC_PLAYER_MOVE);
	netbufWriteU8(dst, movecl->id);
	netbufWriteU32(dst, movecl->inmove[0].tick);
	netbufWritePlayerMove(dst, &movecl->outmove[0]);

	return dst->error;
}

u32 netmsgSvcPlayerMoveRead(struct netbuf *src, struct netclient *srccl)
{
	if (srccl->state != CLSTATE_GAME) {
		sysLogPrintf(LOG_WARNING, "NET: SVC_PLAYER_MOVE from server but we're not in GAME state");
		return 1;
	}

	u8 id = 0;
	u32 outmoveack = 0;
	struct netplayermove newmove;

	id = netbufReadU8(src);
	outmoveack = netbufReadU32(src);
	netbufReadPlayerMove(src, &newmove);

	if (src->error) {
		return src->error;
	}

	struct netclient *movecl = &g_NetClients[id];

	// make space in the move stack
	memmove(movecl->inmove + 1, movecl->inmove, sizeof(movecl->inmove) - sizeof(*movecl->inmove));
	movecl->inmove[0] = newmove;
	movecl->outmoveack = outmoveack;
	movecl->lerpticks = 0;

	if (movecl == g_NetLocalClient && (newmove.ucmd & UCMD_FL_FORCEMASK)) {
		// server wants to teleport us
		if (movecl->player && movecl->player->prop) {
			RoomNum newrooms[8] = { newmove.rooms[0], -1 };
			chrSetPos(movecl->player->prop->chr, &newmove.pos, newrooms, newmove.angles[0], (newmove.ucmd & UCMD_FL_FORCEGROUND) != 0);
			sysLogPrintf(LOG_NOTE, "NET: server is forcing us to move");
		}
	}

	return src->error;
}
