#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <PR/ultratypes.h>
#include "platform.h"
#include "data.h"
#include "types.h"
#include "game/mainmenu.h"
#include "game/menu.h"
#include "game/gamefile.h"
#include "video.h"
#include "input.h"
#include "config.h"
#include "net/net.h"

extern MenuItemHandlerResult menuhandlerMainMenuCombatSimulator(s32 operation, struct menuitem *item, union handlerdata *data);

static s32 g_NetMenuMaxPlayers = NET_MAX_CLIENTS;
static s32 g_NetMenuPort = NET_DEFAULT_PORT;
static char g_NetJoinAddr[256] = "127.0.0.1:27100";
static s32 g_NetJoinAddrPtr = 0;

/* host */

static MenuItemHandlerResult menuhandlerHostMaxPlayers(s32 operation, struct menuitem *item, union handlerdata *data)
{
	switch (operation) {
	case MENUOP_GETSLIDER:
		data->slider.value = g_NetMenuMaxPlayers;
		break;
	case MENUOP_SET:
		if (data->slider.value) {
			g_NetMenuMaxPlayers = data->slider.value;
		}
		break;
	}

	return 0;
}

static MenuItemHandlerResult menuhandlerHostStart(s32 operation, struct menuitem *item, union handlerdata *data)
{
	if (operation == MENUOP_SET) {
		if (netStartServer(g_NetMenuPort, g_NetMenuMaxPlayers) == 0) {
			menuhandlerMainMenuCombatSimulator(MENUOP_SET, NULL, NULL);
		}
	}

	return 0;
}

struct menuitem g_NetHostMenuItems[] = {
	{
		MENUITEMTYPE_SLIDER,
		0,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Max Players",
		NET_MAX_CLIENTS,
		menuhandlerHostMaxPlayers,
	},
	{
		MENUITEMTYPE_SELECTABLE,
		0,
		0,
		L_MPMENU_036, // "Start Game"
		0,
		menuhandlerHostStart,
	},
	{
		MENUITEMTYPE_SEPARATOR,
		0,
		0,
		0,
		0,
		NULL,
	},
	{
		MENUITEMTYPE_SELECTABLE,
		0,
		MENUITEMFLAG_SELECTABLE_CLOSESDIALOG,
		L_OPTIONS_213, // "Back"
		0,
		NULL,
	},
	{ MENUITEMTYPE_END },
};

struct menudialogdef g_NetHostMenuDialog = {
	MENUDIALOGTYPE_DEFAULT,
	(uintptr_t)"Host Network Game",
	g_NetHostMenuItems,
	NULL,
	MENUDIALOGFLAG_LITERAL_TEXT,
	NULL,
};

/* join */

static const char *menutextJoinAddress(struct menuitem *item)
{
	static char tmp[256 + 1];
	if (item && item->flags & MENUITEMFLAG_SELECTABLE_CENTRE) {
		// in centered dialog
		if (g_NetMode == NETMODE_NONE) {
			snprintf(tmp, sizeof(tmp), "%s_\n", g_NetJoinAddr);
		} else if (g_NetLocalClient->state == CLSTATE_CONNECTING) {
			snprintf(tmp, sizeof(tmp), "Connecting to %s...\n", g_NetJoinAddr);
		} else if (g_NetLocalClient->state == CLSTATE_AUTH) {
			snprintf(tmp, sizeof(tmp), "Authenticating with %s...\n", g_NetJoinAddr);
		} else if (g_NetLocalClient->state == CLSTATE_LOBBY) {
			snprintf(tmp, sizeof(tmp), "Waiting for host...\n");
		}
	} else {
		// label
		snprintf(tmp, sizeof(tmp), "%s\n", g_NetJoinAddr);
	}
	return tmp;
}

static MenuItemHandlerResult menuhandlerJoining(s32 operation, struct menuitem *item, union handlerdata *data)
{
	if (inputKeyPressed(VK_ESCAPE)) {
		netDisconnect();
		menuPopDialog();
	}

	return 0;
}

struct menuitem g_NetJoiningMenuItems[] = {
	{
		MENUITEMTYPE_LABEL,
		0,
		MENUITEMFLAG_SELECTABLE_CENTRE,
		(uintptr_t)&menutextJoinAddress,
		0,
		NULL,
	},
	{
		MENUITEMTYPE_SEPARATOR,
		0,
		0,
		0,
		0,
		NULL,
	},
	{
		MENUITEMTYPE_SELECTABLE,
		0,
		MENUITEMFLAG_SELECTABLE_CENTRE | MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"ESC to abort\n",
		0,
		menuhandlerJoining,
	},
	{ MENUITEMTYPE_END },
};

struct menudialogdef g_NetJoiningDialog = {
	MENUDIALOGTYPE_SUCCESS,
	(uintptr_t)"Joining Game...",
	g_NetJoiningMenuItems,
	NULL,
	MENUDIALOGFLAG_LITERAL_TEXT | MENUDIALOGFLAG_IGNOREBACK | MENUDIALOGFLAG_STARTSELECTS,
	NULL,
};

static MenuItemHandlerResult menuhandlerEnterJoinAddress(s32 operation, struct menuitem *item, union handlerdata *data);

struct menuitem g_NetJoinAddressMenuItems[] = {
	{
		MENUITEMTYPE_LABEL,
		0,
		MENUITEMFLAG_SELECTABLE_CENTRE,
		(uintptr_t)&menutextJoinAddress,
		0,
		NULL,
	},
	{
		MENUITEMTYPE_SEPARATOR,
		0,
		0,
		0,
		0,
		NULL,
	},
	{
		MENUITEMTYPE_SELECTABLE,
		0,
		MENUITEMFLAG_SELECTABLE_CENTRE | MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"ESC to return\n",
		0,
		menuhandlerEnterJoinAddress,
	},
	{ MENUITEMTYPE_END },
};

struct menudialogdef g_NetJoinAddressDialog = {
	MENUDIALOGTYPE_SUCCESS,
	(uintptr_t)"Enter Address",
	g_NetJoinAddressMenuItems,
	NULL,
	MENUDIALOGFLAG_LITERAL_TEXT | MENUDIALOGFLAG_IGNOREBACK | MENUDIALOGFLAG_STARTSELECTS,
	NULL,
};

static MenuItemHandlerResult menuhandlerEnterJoinAddress(s32 operation, struct menuitem *item, union handlerdata *data)
{
	if (!menuIsDialogOpen(&g_NetJoinAddressDialog)) {
		return 0;
	}

	const s32 key = inputGetLastKey();
	char chr = '\0';
	switch (key) {
		case 0:
			return 0;
		case VK_ESCAPE:
			menuPopDialog();
			break;
		case VK_BACKSPACE:
			if (g_NetJoinAddrPtr) {
				g_NetJoinAddr[--g_NetJoinAddrPtr] = '\0';
			} else {
				g_NetJoinAddr[0] = '\0';
			}
			break;
		case VK_A ... VK_Z: chr = 'a' + key - VK_A; break;
		case VK_1 ... VK_9: chr = '1' + key - VK_1; break;
		case VK_0: chr = '0'; break;
		case VK_PERIOD: chr = '.'; break;
		case VK_SEMICOLON: chr = ':'; break;
		case VK_LEFTBRACKET: chr = '['; break;
		case VK_RIGHTBRACKET: chr = ']'; break;
		case VK_MINUS: chr = '-'; break;
		default:
			break;
	}

	if (chr && g_NetJoinAddrPtr < sizeof(g_NetJoinAddr) - 1) {
		g_NetJoinAddr[g_NetJoinAddrPtr++] = chr;
		g_NetJoinAddr[g_NetJoinAddrPtr + 1] = '\0';
	}

	inputClearLastKey();

	return 0;
}

static MenuItemHandlerResult menuhandlerJoinAddress(s32 operation, struct menuitem *item, union handlerdata *data)
{
	if (operation == MENUOP_SET) {
		inputClearLastKey();
		g_NetJoinAddrPtr = strlen(g_NetJoinAddr);
		menuPushDialog(&g_NetJoinAddressDialog);
	}

	return 0;
}

static MenuItemHandlerResult menuhandlerJoinStart(s32 operation, struct menuitem *item, union handlerdata *data)
{
	if (operation == MENUOP_SET) {
		if (netStartClient(g_NetJoinAddr) == 0) {
			menuPushDialog(&g_NetJoiningDialog);
		}
	}

	return 0;
}

struct menuitem g_NetJoinMenuItems[] = {
	{
		MENUITEMTYPE_SELECTABLE,
		0,
		MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Address:   \n",
		(uintptr_t)&menutextJoinAddress,
		menuhandlerJoinAddress,
	},
	{
		MENUITEMTYPE_SELECTABLE,
		0,
		0,
		L_MPMENU_036, // "Start Game"
		0,
		menuhandlerJoinStart,
	},
	{
		MENUITEMTYPE_SEPARATOR,
		0,
		0,
		0,
		0,
		NULL,
	},
	{
		MENUITEMTYPE_SELECTABLE,
		0,
		MENUITEMFLAG_SELECTABLE_CLOSESDIALOG,
		L_OPTIONS_213, // "Back"
		0,
		NULL,
	},
	{ MENUITEMTYPE_END },
};

struct menudialogdef g_NetJoinMenuDialog = {
	MENUDIALOGTYPE_DEFAULT,
	(uintptr_t)"Join Network Game",
	g_NetJoinMenuItems,
	NULL,
	MENUDIALOGFLAG_LITERAL_TEXT | MENUDIALOGFLAG_STARTSELECTS | MENUDIALOGFLAG_IGNOREBACK,
	NULL,
};

/* main */

struct menuitem g_NetMenuItems[] = {
	{
		MENUITEMTYPE_SELECTABLE,
		0,
		MENUITEMFLAG_SELECTABLE_OPENSDIALOG | MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Host Game\n",
		0,
		(void *)&g_NetHostMenuDialog,
	},
	{
		MENUITEMTYPE_SELECTABLE,
		0,
		MENUITEMFLAG_SELECTABLE_OPENSDIALOG | MENUITEMFLAG_LITERAL_TEXT,
		(uintptr_t)"Join Game\n",
		0,
		(void *)&g_NetJoinMenuDialog,
	},
	{
		MENUITEMTYPE_SEPARATOR,
		0,
		0,
		0,
		0,
		NULL,
	},
	{
		MENUITEMTYPE_SELECTABLE,
		0,
		MENUITEMFLAG_SELECTABLE_CLOSESDIALOG,
		L_OPTIONS_213, // "Back"
		0,
		NULL,
	},
	{ MENUITEMTYPE_END },
};

struct menudialogdef g_NetMenuDialog = {
	MENUDIALOGTYPE_DEFAULT,
	(uintptr_t)"Network Game",
	g_NetMenuItems,
	NULL,
	MENUDIALOGFLAG_MPLOCKABLE | MENUDIALOGFLAG_LITERAL_TEXT,
	NULL,
};
