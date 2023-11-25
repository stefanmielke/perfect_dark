#include <string>
#include <SDL.h>
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_sdlrenderer2.h"
#include "imgui/backends/imgui_impl_sdl2.h"

extern "C"
{

#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include <gbiex.h>
#include "data.h"
#include "bss.h"
#include "constants.h"
#include "game/stagetable.h"
#include "game/chr.h"
#include "game/chraction.h"
#include "game/bg.h"
#include "game/player.h"
#include "game/propobj.h"
#include "game/prop.h"
#include "game/sight.h"
#include "game/game_1531a0.h"
#include "lib/memp.h"
#include "lib/mema.h"
#include "lib/vi.h"
#include "lib/lib_17ce0.h"
#include "video.h"
#include "input.h"
#include "system.h"
#include "utils.h"
#include "romdata.h"

#undef bool
#undef true
#undef false

s32 g_DebuggerActive = FALSE;
s32 g_DebuggerTimePasses = TRUE;
s32 g_DebuggerFrameAdvance = 0;

u32 memaGetSize(void);

}

static SDL_Window *dbgwin = nullptr;
static SDL_Renderer *dbgren = nullptr;

static char xstr[64] = "0";
static char ystr[64] = "0";
static char zstr[64] = "0";
static char gotostr[64] = "";

static bool advanceHeld = false;

static s32 propFilter = 0;
static s32 objFilter = 0;

static const char *propTypes[9];
static const char *objTypes[0x3c];

static bool expandLatch = false;
static bool expandValue = false;

static bool showPropInfo = false;
static bool showActiveProps = true;

static void *focusPtr = nullptr;

static struct prop *lookatprop = nullptr;
static struct hitthing lookathit;
static bool lookathitset;

static inline const char *fmtCoord(const coord& crd)
{
	static char tmp[128];
	snprintf(tmp, sizeof(tmp), "(%.2f, %.2f, %.2f)", crd.x, crd.y, crd.z);
	return tmp;
}

#define DEF_ACT_NAME(x) case x: return #x;

static inline const char *fmtActionType(s8 at)
{
	switch (at) {
		DEF_ACT_NAME(ACT_INIT            )
		DEF_ACT_NAME(ACT_STAND           )
		DEF_ACT_NAME(ACT_KNEEL           )
		DEF_ACT_NAME(ACT_ANIM            )
		DEF_ACT_NAME(ACT_DIE             )
		DEF_ACT_NAME(ACT_DEAD            )
		DEF_ACT_NAME(ACT_ARGH            )
		DEF_ACT_NAME(ACT_PREARGH         )
		DEF_ACT_NAME(ACT_ATTACK          )
		DEF_ACT_NAME(ACT_ATTACKWALK      )
		DEF_ACT_NAME(ACT_ATTACKROLL      )
		DEF_ACT_NAME(ACT_SIDESTEP        )
		DEF_ACT_NAME(ACT_JUMPOUT         )
		DEF_ACT_NAME(ACT_RUNPOS          )
		DEF_ACT_NAME(ACT_PATROL          )
		DEF_ACT_NAME(ACT_GOPOS           )
		DEF_ACT_NAME(ACT_SURRENDER       )
		DEF_ACT_NAME(ACT_LOOKATTARGET    )
		DEF_ACT_NAME(ACT_SURPRISED       )
		DEF_ACT_NAME(ACT_STARTALARM      )
		DEF_ACT_NAME(ACT_THROWGRENADE    )
		DEF_ACT_NAME(ACT_TURNDIR         )
		DEF_ACT_NAME(ACT_TEST            )
		DEF_ACT_NAME(ACT_BONDINTRO       )
		DEF_ACT_NAME(ACT_BONDDIE         )
		DEF_ACT_NAME(ACT_BONDMULTI       )
		DEF_ACT_NAME(ACT_NULL            )
		DEF_ACT_NAME(ACT_BOT_ATTACKSTAND )
		DEF_ACT_NAME(ACT_BOT_ATTACKKNEEL )
		DEF_ACT_NAME(ACT_BOT_ATTACKSTRAFE)
		DEF_ACT_NAME(ACT_DRUGGEDDROP     )
		DEF_ACT_NAME(ACT_DRUGGEDKO       )
		DEF_ACT_NAME(ACT_DRUGGEDCOMINGUP )
		DEF_ACT_NAME(ACT_ATTACKAMOUNT    )
		DEF_ACT_NAME(ACT_ROBOTATTACK     )
		DEF_ACT_NAME(ACT_SKJUMP          )
		DEF_ACT_NAME(ACT_PUNCH           )
		DEF_ACT_NAME(ACT_CUTFIRE         )
		default: return "ACT";
	}
}

static inline const char *fmtPropType(u8 at)
{
	switch (at) {
		DEF_ACT_NAME(PROPTYPE_OBJ)
		DEF_ACT_NAME(PROPTYPE_DOOR)
		DEF_ACT_NAME(PROPTYPE_CHR)
		DEF_ACT_NAME(PROPTYPE_WEAPON)
		DEF_ACT_NAME(PROPTYPE_EYESPY)
		DEF_ACT_NAME(PROPTYPE_PLAYER)
		DEF_ACT_NAME(PROPTYPE_EXPLOSION)
		DEF_ACT_NAME(PROPTYPE_SMOKE)
		default: return "Prop";
	}
}

static inline const char *fmtObjType(u8 at)
{
	switch (at) {
		DEF_ACT_NAME(OBJTYPE_DOOR              )
		DEF_ACT_NAME(OBJTYPE_DOORSCALE         )
		DEF_ACT_NAME(OBJTYPE_BASIC             )
		DEF_ACT_NAME(OBJTYPE_KEY               )
		DEF_ACT_NAME(OBJTYPE_ALARM             )
		DEF_ACT_NAME(OBJTYPE_CCTV              )
		DEF_ACT_NAME(OBJTYPE_AMMOCRATE         )
		DEF_ACT_NAME(OBJTYPE_WEAPON            )
		DEF_ACT_NAME(OBJTYPE_CHR               )
		DEF_ACT_NAME(OBJTYPE_SINGLEMONITOR     )
		DEF_ACT_NAME(OBJTYPE_MULTIMONITOR      )
		DEF_ACT_NAME(OBJTYPE_HANGINGMONITORS   )
		DEF_ACT_NAME(OBJTYPE_AUTOGUN           )
		DEF_ACT_NAME(OBJTYPE_LINKGUNS          )
		DEF_ACT_NAME(OBJTYPE_DEBRIS            )
		DEF_ACT_NAME(OBJTYPE_10                )
		DEF_ACT_NAME(OBJTYPE_HAT               )
		DEF_ACT_NAME(OBJTYPE_GRENADEPROB       )
		DEF_ACT_NAME(OBJTYPE_LINKLIFTDOOR      )
		DEF_ACT_NAME(OBJTYPE_MULTIAMMOCRATE    )
		DEF_ACT_NAME(OBJTYPE_SHIELD            )
		DEF_ACT_NAME(OBJTYPE_TAG               )
		DEF_ACT_NAME(OBJTYPE_BEGINOBJECTIVE    )
		DEF_ACT_NAME(OBJTYPE_ENDOBJECTIVE      )
		DEF_ACT_NAME(OBJECTIVETYPE_DESTROYOBJ  )
		DEF_ACT_NAME(OBJECTIVETYPE_COMPFLAGS   )
		DEF_ACT_NAME(OBJECTIVETYPE_FAILFLAGS   )
		DEF_ACT_NAME(OBJECTIVETYPE_COLLECTOBJ  )
		DEF_ACT_NAME(OBJECTIVETYPE_THROWOBJ    )
		DEF_ACT_NAME(OBJECTIVETYPE_HOLOGRAPH   )
		DEF_ACT_NAME(OBJECTIVETYPE_1F          )
		DEF_ACT_NAME(OBJECTIVETYPE_ENTERROOM   )
		DEF_ACT_NAME(OBJECTIVETYPE_THROWINROOM )
		DEF_ACT_NAME(OBJTYPE_22                )
		DEF_ACT_NAME(OBJTYPE_BRIEFING          )
		DEF_ACT_NAME(OBJTYPE_GASBOTTLE         )
		DEF_ACT_NAME(OBJTYPE_RENAMEOBJ         )
		DEF_ACT_NAME(OBJTYPE_PADLOCKEDDOOR     )
		DEF_ACT_NAME(OBJTYPE_TRUCK             )
		DEF_ACT_NAME(OBJTYPE_HELI              )
		DEF_ACT_NAME(OBJTYPE_29                )
		DEF_ACT_NAME(OBJTYPE_GLASS             )
		DEF_ACT_NAME(OBJTYPE_SAFE              )
		DEF_ACT_NAME(OBJTYPE_SAFEITEM          )
		DEF_ACT_NAME(OBJTYPE_TANK              )
		DEF_ACT_NAME(OBJTYPE_CAMERAPOS         )
		DEF_ACT_NAME(OBJTYPE_TINTEDGLASS       )
		DEF_ACT_NAME(OBJTYPE_LIFT              )
		DEF_ACT_NAME(OBJTYPE_CONDITIONALSCENERY)
		DEF_ACT_NAME(OBJTYPE_BLOCKEDPATH       )
		DEF_ACT_NAME(OBJTYPE_HOVERBIKE         )
		DEF_ACT_NAME(OBJTYPE_END               )
		DEF_ACT_NAME(OBJTYPE_HOVERPROP         )
		DEF_ACT_NAME(OBJTYPE_FAN               )
		DEF_ACT_NAME(OBJTYPE_HOVERCAR          )
		DEF_ACT_NAME(OBJTYPE_PADEFFECT         )
		DEF_ACT_NAME(OBJTYPE_CHOPPER           )
		DEF_ACT_NAME(OBJTYPE_MINE              )
		DEF_ACT_NAME(OBJTYPE_ESCASTEP          )
		default: return "Obj";
	}
}

#undef DEF_ACT_NAME

extern "C" void debuggerInit(void)
{
	SDL_Window *parent = (SDL_Window *)videoGetWindowHandle();

	int px = 0;
	int py = 0;
	int pw = 0;
	int ph = 0;
	// in windowed mode put the debugger to the right of the window
	if ((SDL_GetWindowFlags(parent) & SDL_WINDOW_FULLSCREEN_DESKTOP) == 0) {
		SDL_GetWindowPosition(parent, &px, &py);
		SDL_GetWindowSize(parent, &pw, &ph);
	}

	const Uint32 winflags = SDL_WINDOW_RESIZABLE;
	dbgwin = SDL_CreateWindow("Debugger", px + pw + 4, py, 320, ph, winflags);
	if (!dbgwin) {
		sysLogPrintf(LOG_ERROR, "DBG: could not create SDL window: %s", SDL_GetError());
		return;
	}

	// can't trust any hardware accelerated windows to live in harmony with our main window, so force SW
	dbgren = SDL_CreateRenderer(dbgwin, -1, SDL_RENDERER_SOFTWARE);
	if (!dbgren) {
		sysLogPrintf(LOG_ERROR, "DBG: could not create SDL renderer: %s", SDL_GetError());
		return;
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.IniFilename = nullptr;

	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForSDLRenderer(dbgwin, dbgren);
	ImGui_ImplSDLRenderer2_Init(dbgren);

	propTypes[0] = "NONE";
	for (s32 i = 1; i < ARRAYCOUNT(propTypes); ++i) {
		propTypes[i] = fmtPropType(i);
	}

	objTypes[0] = "NONE";
	for (s32 i = 1; i < ARRAYCOUNT(objTypes); ++i) {
		objTypes[i] = fmtObjType(i);
	}

	g_DebuggerActive = TRUE;
}

static inline void debuggerHotKey(const s32 key, const bool pressed)
{
	switch (key) {
		case SDLK_F1:
			if (pressed)
				g_DebuggerTimePasses ^= 1;
			break;
		case SDLK_F2:
			if (pressed)
				g_DebuggerFrameAdvance = 1;
			break;
		case SDLK_F3:
			if (pressed)
				g_DebuggerFrameAdvance = 5;
			break;
		case SDLK_F4:
			advanceHeld = pressed;
			break;
		default:
			break;
	}
}

extern "C" void debuggerProcessEvent(void *data)
{
	if (g_DebuggerActive) {
		SDL_Event *e = (SDL_Event *)data;
		if (e->type == SDL_KEYDOWN || e->type == SDL_KEYUP) {
			debuggerHotKey(e->key.keysym.sym, e->type == SDL_KEYDOWN);
		} else if (e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP) {
			if (SDL_GetMouseFocus() != dbgwin || SDL_GetWindowID(dbgwin) != e->button.windowID) {
				return;
			}
		} else if (e->type == SDL_MOUSEMOTION) {
			if (SDL_GetMouseFocus() != dbgwin || SDL_GetWindowID(dbgwin) != e->motion.windowID) {
				return;
			}
		} else if (e->type == SDL_MOUSEWHEEL) {
			if (SDL_GetMouseFocus() != dbgwin || SDL_GetWindowID(dbgwin) != e->wheel.windowID) {
				return;
			}
		}
		ImGui_ImplSDL2_ProcessEvent(e);
	}
}

extern "C" void debuggerStartFrame(void)
{
	ImGui_ImplSDLRenderer2_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
}

static inline bool debuggerGetSurfaceInfo(struct hitthing *hit)
{
	struct coord endpos;
	endpos.x = g_Vars.currentplayer->cam_pos.x + g_Vars.currentplayer->cam_look.x * 10000.f;
	endpos.y = g_Vars.currentplayer->cam_pos.y + g_Vars.currentplayer->cam_look.y * 10000.f;
	endpos.z = g_Vars.currentplayer->cam_pos.z + g_Vars.currentplayer->cam_look.z * 10000.f;

	// get what rooms the LOS goes through
	RoomNum outrooms[17], tmprooms[8], srcrooms[2];
	srcrooms[0] = g_Vars.currentplayer->cam_room;
	srcrooms[1] = -1;
	outrooms[16] = -1;
	portal00018148(&g_Vars.currentplayer->cam_pos, &endpos, srcrooms, tmprooms, outrooms, 16);

	// no prop was hit; check for bg hits
	for (s32 i = 0; outrooms[i] != -1; ++i) {
		if (bgTestHitInRoom(&g_Vars.currentplayer->cam_pos, &endpos, outrooms[i], hit)) {
			return true;
		}
	}

	return false;
}

extern "C" void debuggerUpdatePropInfo(s32 clear)
{
	if (showPropInfo) {
		if (clear) {
			lookatprop = nullptr;
			lookathitset = false;
		} else {
			lookatprop = propFindAimingAt(HAND_RIGHT, false, FINDPROPCONTEXT_QUERY);
			if (!lookatprop) {
				// if we aren't looking at a prop, try to get the surface we're looking at
				lookathitset = debuggerGetSurfaceInfo(&lookathit);
			} else {
				lookathitset = false;
			}
		}
	} else {
		lookatprop = nullptr;
		lookathitset = false;
	}
}

static inline void debuggerTeleportPlayer(struct coord& crd)
{
	sysLogPrintf(LOG_NOTE, "DBG: teleporting player 1 to (%.2f, %.2f, %.2f)", crd.x, crd.y, crd.z);

	if (!g_Vars.players[0] || !g_Vars.players[0]->prop) {
		sysLogPrintf(LOG_NOTE, "DBG: no player 1 or player 1 has no prop");
		return;
	}

	struct chrdata *playerchr = g_Vars.players[0]->prop->chr;
	if (!playerchr) {
		sysLogPrintf(LOG_NOTE, "DBG: player 1 has no chr");
		return;
	}

	RoomNum inrooms[9] = { -1 };
	RoomNum aboverooms[9] = { -1 };

	bgFindRoomsByPos(&crd, inrooms, aboverooms, 8, NULL);

	RoomNum *rooms = NULL;
	if (inrooms[0] != -1) {
		rooms = inrooms;
	} else if (aboverooms[0] != -1) {
		rooms = aboverooms;
	}

	if (rooms) {
		const u32 old = playerchr->hidden;
		playerchr->hidden |= CHRHFLAG_WARPONSCREEN;
		chrMoveToPos(playerchr, &crd, rooms, g_Vars.players[0]->vv_theta, true);
		if ((old & CHRHFLAG_WARPONSCREEN) == 0) {
			playerchr->hidden &= ~CHRHFLAG_WARPONSCREEN;
		}
	} else {
		sysLogPrintf(LOG_NOTE, "DBG: could not find any rooms at teleport destination");
	}
}

static inline void debuggerKillChr(struct chrdata *chr)
{
	struct coord dmgdir = { 0.f, 0.f, 0.f };
	chrDamageByImpact(chr, 10000.f, &dmgdir, nullptr, nullptr, HITPART_GENERAL);
}

static inline void debuggerKillPlayer(void)
{
	sysLogPrintf(LOG_NOTE, "DBG: killing player 1");

	if (!g_Vars.players[0] || !g_Vars.players[0]->prop) {
		sysLogPrintf(LOG_NOTE, "DBG: no player 1 or player 1 has no prop");
		return;
	}

	struct chrdata *playerchr = g_Vars.players[0]->prop->chr;
	if (!playerchr) {
		sysLogPrintf(LOG_NOTE, "DBG: player 1 has no chr");
		return;
	}

	debuggerKillChr(playerchr);
}

static inline void debuggerHealPlayer(void)
{
	sysLogPrintf(LOG_NOTE, "DBG: healing player 1");

	if (!g_Vars.players[0] || !g_Vars.players[0]->prop) {
		sysLogPrintf(LOG_NOTE, "DBG: no player 1 or player 1 has no prop");
		return;
	}

	player *prev = g_Vars.currentplayer;
	g_Vars.currentplayer = g_Vars.players[0];
	playerDisplayHealth();
	g_Vars.currentplayer->bondhealth = 1.f;
	g_Vars.currentplayer = prev;
}

static inline void describeHand(hand *hand)
{
	ImGui::Text("Gun num:  %02x", hand->gset.weaponnum);
	ImGui::Text("Gun func: %02x", hand->gset.weaponfunc);
	ImGui::Text("Aim pos: %s", fmtCoord(hand->aimpos));
	ImGui::Text("Hit pos: %s", fmtCoord(hand->hitpos));
}

static inline std::string& describeRoomList(RoomNum *rooms, s32 maxRooms, std::string& out)
{
	for (s32 i = 0; i < maxRooms && rooms[i] >= 0; ++i) {
		out += strFmt("%03x ", rooms[i]);
	}
	return out;
}

static inline void describeProjectile(projectile *prj)
{
	ImGui::Text("Drop type: %04x", (u16)prj->droptype);
	ImGui::Text("Flags: %08x", prj->flags);
	ImGui::Text("Vel: %s", fmtCoord(prj->speed));
}

static inline void describeObj(defaultobj *obj)
{
	ImGui::Text("Type: %s (%02x)", fmtObjType(obj->type), obj->type);
	ImGui::Text("Model: %04x", (u16)obj->modelnum);
	ImGui::Text("Pad: %04x", (u16)obj->pad);
	ImGui::Text("Flags 1: %08x", obj->flags);
	ImGui::Text("Flags 2: %08x", obj->flags2);
	ImGui::Text("Flags 3: %08x", obj->flags3);
	ImGui::Text("Hidden 1: %08x", obj->hidden);
	ImGui::Text("Hidden 2: %02x", obj->hidden2);

	if ((obj->hidden & OBJHFLAG_PROJECTILE) && obj->projectile) {
		if (ImGui::TreeNode(strFmt("Projectile (%p)", obj->projectile))) {
			describeProjectile(obj->projectile);
			ImGui::TreePop();
		}
	}
}

static inline void describeWeapon(weaponobj *wpn)
{
	ImGui::Text("Gun num:  %02x", wpn->weaponnum);
	ImGui::Text("Gun func: %02x", wpn->gunfunc);
	ImGui::Text("Fade out timer (60hz): %d", wpn->fadeouttimer60);
	ImGui::Text("Timer (240hz): %d", wpn->timer240);
}

static inline void describeProp(prop *prop);

static inline void describeChr(chrdata *chr)
{
	ImGui::Text("Number: %04x", (u16)chr->chrnum);
	ImGui::Text("Flags 1: %08x", chr->flags);
	ImGui::Text("Flags 2: %08x", chr->flags2);
	ImGui::Text("Hidden 1: %08x", chr->hidden);
	ImGui::Text("Hidden 2: %04x", chr->hidden2);
	ImGui::Text("Chr flags: %08x", chr->chrflags);
	ImGui::Text("Damage: %.3f", chr->damage);
	ImGui::Text("Shield: %.3f", chr->cshield);
	ImGui::Text("Body: %04x", (u16)chr->bodynum);
	ImGui::Text("Head: %02x", (u8)chr->headnum);
	ImGui::Text("Team: %02x", chr->team);
	ImGui::Text("Tude: %02x", chr->tude);
	ImGui::Text("Action: %s (%02x)", fmtActionType(chr->actiontype), (u8)chr->actiontype);

	ImGui::Separator();

	if (chr->weapons_held[HAND_RIGHT]) {
		if (ImGui::TreeNode(strFmt("Right hand prop (%p)", chr->weapons_held[HAND_RIGHT]))) {
			describeProp(chr->weapons_held[HAND_RIGHT]);
			ImGui::TreePop();
		}
	}
	if (chr->weapons_held[HAND_LEFT]) {
		if (ImGui::TreeNode(strFmt("Left hand prop (%p)", chr->weapons_held[HAND_LEFT]))) {
			describeProp(chr->weapons_held[HAND_LEFT]);
			ImGui::TreePop();
		}
	}
	if (chr->weapons_held[2]) {
		if (ImGui::TreeNode(strFmt("Hat prop (%p)", chr->weapons_held[2]))) {
			describeProp(chr->weapons_held[2]);
			ImGui::TreePop();
		}
	}

	ImGui::SeparatorText("Controls");

	if (ImGui::Button("Kill")) {
		debuggerKillChr(chr);
	}

	ImGui::SameLine();

	if (ImGui::Button("Heal")) {
		chr->damage = 0.f;
	}

	ImGui::SameLine();

	if (ImGui::Button("Sleep")) {
		chr->sleep = 127;
	}

	ImGui::SameLine();

	if (ImGui::Button("Wake")) {
		chr->sleep = 0;
	}

	// line break

	if (ImGui::Button("Drop guns")) {
		chrDropItemsForOwnerReap(chr);
	}
}

static inline void describeProp(prop *prop)
{
	ImGui::Text(prop->active ? "Active" : "Inactive");
	ImGui::Text("Type: %s (%02x)", fmtPropType(prop->type), prop->type);
	ImGui::Text("Flags: %02x", prop->flags);
	ImGui::Text("Pos: %s", fmtCoord(prop->pos));

	std::string roomstr = "";
	ImGui::Text("Rooms: %s", describeRoomList(prop->rooms, ARRAYCOUNT(prop->rooms), roomstr).c_str());

	if (prop->obj) {
		if (expandLatch) {
			ImGui::SetNextItemOpen(expandValue);
		}
		if (ImGui::TreeNode(strFmt("%s (%p)", fmtObjType(prop->obj->type), prop->obj))) {
			describeObj(prop->obj);
			ImGui::TreePop();
		}
	}

	if ((prop->type == PROPTYPE_CHR || prop->type == PROPTYPE_PLAYER || prop->type == PROPTYPE_EYESPY) && prop->chr) {
		if (expandLatch) {
			ImGui::SetNextItemOpen(expandValue);
		}
		if (ImGui::TreeNode(strFmt("Chr (%p)", prop->chr))) {
			describeChr(prop->chr);
			ImGui::TreePop();
		}
	} else if (prop->type == PROPTYPE_WEAPON && prop->weapon) {
		if (expandLatch) {
			ImGui::SetNextItemOpen(expandValue);
		}
		if (ImGui::TreeNode(strFmt("Weapon (%p)", prop->weapon))) {
			describeWeapon(prop->weapon);
			ImGui::TreePop();
		}
	}
}

static inline void describePlayer(player *plr)
{
	ImGui::Text("Cam pos: %s", fmtCoord(plr->cam_pos));
	ImGui::Text("Cam look: %s", fmtCoord(plr->cam_look));
	ImGui::Text("Cam room: %03x", (u32)plr->cam_room);

	ImGui::Separator();

	ImGui::Text("Health: %.3f", plr->bondhealth);
	ImGui::Text("Shield: %.3f", plr->apparentarmour);

	ImGui::Separator();

	if (plr->prop) {
		if (ImGui::TreeNode(strFmt("Prop (%p)", plr->prop))) {
			describeProp(plr->prop);
			ImGui::TreePop();
		}
	}

	ImGui::Separator();

	if (ImGui::TreeNode("Right hand")) {
		describeHand(&plr->hands[HAND_RIGHT]);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Left hand")) {
		describeHand(&plr->hands[HAND_LEFT]);
		ImGui::TreePop();
	}
}

extern "C" void debuggerFrame(void)
{
	if (!g_DebuggerActive) {
		return;
	}

	if (advanceHeld) {
		g_DebuggerFrameAdvance = 1;
	}

	expandLatch = false;

	ImGuiIO& io = ImGui::GetIO();

	ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
	ImGui::SetNextWindowSize(io.DisplaySize);
	ImGui::Begin("Debugger", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);

	if (ImGui::CollapsingHeader("Debug")) {
		ImGui::SeparatorText("Player");

		ImGui::PushItemWidth(56.f);
		ImGui::InputText("X", xstr, IM_ARRAYSIZE(xstr) - 1, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_CharsDecimal);
		ImGui::SameLine();
		ImGui::InputText("Y", ystr, IM_ARRAYSIZE(ystr) - 1, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_CharsDecimal);
		ImGui::SameLine();
		ImGui::InputText("Z", zstr, IM_ARRAYSIZE(zstr) - 1, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_CharsDecimal);
		ImGui::SameLine();
		ImGui::PopItemWidth();
		if (ImGui::Button("Warp")) {
			struct coord crd;
			crd.x = std::atof(xstr);
			crd.y = std::atof(ystr);
			crd.z = std::atof(zstr);
			debuggerTeleportPlayer(crd);
		}

		if (ImGui::Button("Kill")) {
			debuggerKillPlayer();
		}
		ImGui::SameLine();
		if (ImGui::Button("Heal")) {
			debuggerHealPlayer();
		}

		ImGui::SeparatorText("Time");

		bool timePasses = (g_DebuggerTimePasses != 0);
		if (ImGui::Checkbox("Time passes", &timePasses)) {
			g_DebuggerTimePasses = (s32)timePasses;
		}

		if (ImGui::Button("+1 frame")) {
			g_DebuggerFrameAdvance = 1;
		}
		ImGui::SameLine();
		if (ImGui::Button("+5 frames")) {
			g_DebuggerFrameAdvance = 5;
		}
		ImGui::SameLine();
		if (ImGui::Button("+10 frames")) {
			g_DebuggerFrameAdvance = 10;
		}

		ImGui::SeparatorText("Other");
	}

	if (ImGui::CollapsingHeader("Time")) {
		ImGui::Text("Level Frame#: %d", g_Vars.lvframenum);
		ImGui::Text("Level Tick (60hz): %d", g_Vars.lvframe60);
		ImGui::Text("Level Tick (240hz): %d", g_Vars.lvframe240);
		ImGui::Separator();
		ImGui::Text("Frame Time (60hz): %.2f", g_Vars.diffframe60f);
		ImGui::Text("Frame Time (240hz): %.2f", g_Vars.diffframe240f);
		ImGui::Separator();
		ImGui::Text("Lost Time (60hz): %d", g_Vars.lostframetime60t);
		ImGui::Text("Lost Time (240hz): %d", g_Vars.lostframetime240t);
	}

	if (ImGui::CollapsingHeader("Players")) {
		for (s32 i = 0; i < ARRAYCOUNT(g_Vars.players); ++i) {
			auto plr = g_Vars.players[i];
			if (plr) {
				if (ImGui::TreeNode(strFmt("Player %d (%p)###Player%p", i + 1, plr, plr))) {
					describePlayer(plr);
					ImGui::TreePop();
				}
			}
		}
	}

	if (g_Vars.activeprops && g_Vars.activeprops != g_Vars.activepropstail) {
		if (ImGui::CollapsingHeader("Props")) {
			ImGui::Text("Visible: %d", g_Vars.numonscreenprops);

			ImGui::Separator();

			ImGui::Checkbox("Active props only", &showActiveProps);

			ImGui::Combo("Filter prop", &propFilter, propTypes, ARRAYCOUNT(propTypes));
			ImGui::Combo("Filter obj", &objFilter, objTypes, ARRAYCOUNT(objTypes));

			if (ImGui::Button("Expand all")) {
				expandLatch = true;
				expandValue = true;
			}
			ImGui::SameLine();
			if (ImGui::Button("Close all")) {
				expandLatch = true;
				expandValue = false;
			}

			ImGui::Separator();

			auto showProp = [&](struct prop *prop) {
				if (prop && (!propFilter || propFilter == prop->type)) {
					if (!objFilter || (prop->obj && prop->obj->type == objFilter)) {
						const bool focus = (focusPtr && ((void *)prop == focusPtr || (void *)prop->obj == focusPtr));
						if (expandLatch) {
							ImGui::SetNextItemOpen(expandValue);
						} else if (focus) {
							ImGui::SetNextItemOpen(true);
							focusPtr = nullptr;
						}
						if (ImGui::TreeNode(strFmt("%s (%p)###Prop%p", fmtPropType(prop->type), prop, prop))) {
							if (focus) {
								ImGui::SetScrollHereY();
							}
							describeProp(prop);
							ImGui::TreePop();
						}
					}
				}
			};

			if (showActiveProps) {
				// show the active props list
				struct prop *prop;
				struct prop *next;
				bool done;
				prop = g_Vars.activeprops;
				do {
					next = prop->next;
					done = next == g_Vars.pausedprops;
					showProp(prop);
					prop = next;
				} while (!done);
			} else {
				// show all props
				for (s32 i = 0; i < g_Vars.maxprops; ++i) {
					struct prop *prop = &g_Vars.props[i];
					showProp(prop);
				}
			}
		}
	}

	if (g_ChrSlots && g_NumChrSlots) {
		if (ImGui::CollapsingHeader("Characters")) {
			ImGui::Text("Count: %d/%d", g_NumChrs, g_NumChrSlots);
			ImGui::Separator();
			for (s32 i = 0; i < g_NumChrSlots; ++i) {
				if (g_ChrSlots[i].chrnum >= 0) {
					if (ImGui::TreeNode(strFmt("Slot %3d: %04x (%p)", i, g_ChrSlots[i].chrnum, &g_ChrSlots[i]))) {
						describeChr(&g_ChrSlots[i]);
						if (g_ChrSlots[i].prop && ImGui::TreeNode(strFmt("Prop (%p)##chr", g_ChrSlots[i].prop))) {
							describeProp(g_ChrSlots[i].prop);
							ImGui::TreePop();
						}
						ImGui::TreePop();
					}
				}
			}
		}
	}

	if (ImGui::CollapsingHeader("Stage")) {
		ImGui::Text("Number: %02x", g_Vars.stagenum);
		ImGui::Text("Index:  %02x", g_StageIndex);
		ImGui::Text("Setup:  %s", romdataFileGetName(g_Stages[g_StageIndex].setupfileid));
		ImGui::Text("Room count: %d", g_Vars.roomcount);
		if (g_Rooms && ImGui::TreeNode("Rooms")) {
			ImGui::Separator();
			for (s32 i = 1; i < g_Vars.roomcount; ++i) {
				if (ImGui::TreeNode(strFmt("Room %03x", i))) {
					ImGui::Text("Loaded: %d", g_Rooms[i].loaded240);
					ImGui::Text("Flags: %04x", g_Rooms[i].flags);
					ImGui::Text("Portals: %d", g_Rooms[i].numportals);
					ImGui::Text("Centre: %s", fmtCoord(g_Rooms[i].centre));
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}
	}

	if (ImGui::CollapsingHeader("Memory")) {
		ImGui::Text("Mema size: %uk", memaGetSize() / 1024);
		ImGui::Spacing();
		if (ImGui::TreeNode("Memp state")) {
			if (ImGui::BeginTable("MemaTab", 3)) {
				ImGui::TableNextColumn();
				ImGui::Text("Pool");
				ImGui::TableNextColumn();
				ImGui::Text("Onboard");
				ImGui::TableNextColumn();
				ImGui::Text("Expansion");
				for (u8 i = MEMPOOL_STAGE; i <= MEMPOOL_PERMANENT; i += 2) {
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::Text((i == MEMPOOL_STAGE) ? "Stage" : "Permanent");
					ImGui::TableNextColumn();
					ImGui::Text("%uk/%uk", mempGetPoolFree(i, MEMBANK_ONBOARD) / 1024, mempGetPoolSize(i, MEMBANK_ONBOARD) / 1024);
					ImGui::TableNextColumn();
					ImGui::Text("%uk/%uk", mempGetPoolFree(i, MEMBANK_EXPANSION) / 1024, mempGetPoolSize(i, MEMBANK_EXPANSION) / 1024);
				}
				ImGui::EndTable();
			}
			ImGui::TreePop();
		}
	}

	if ((showPropInfo = ImGui::CollapsingHeader("Looking at"))) {
		if (lookatprop) {
			ImGui::Text("Looking at prop");
			describeProp(lookatprop);
		} else if (lookathitset) {
			ImGui::Text("Looking at surface");
			ImGui::Text("Hit pos: %s", fmtCoord(lookathit.pos));
			ImGui::Text("Texture: %04x", lookathit.texturenum);
		} else {
			ImGui::Text("Looking at nothing");
		}
	}

	ImGui::End();
}

extern "C" void debuggerEndFrame(void)
{
	ImGui::Render();
	ImGuiIO& io = ImGui::GetIO();
	SDL_RenderSetScale(dbgren, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
	SDL_SetRenderDrawColor(dbgren, 0, 0, 0, 255);
	SDL_RenderClear(dbgren);
	ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
	SDL_RenderPresent(dbgren);
}
