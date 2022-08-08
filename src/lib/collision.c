#include <ultra64.h>
#include "constants.h"
#include "game/prop.h"
#include "game/game_1531a0.h"
#include "game/bg.h"
#include "bss.h"
#include "lib/dma.h"
#include "lib/memp.h"
#include "lib/mtx.h"
#include "lib/lib_17ce0.h"
#include "lib/anim.h"
#include "lib/collision.h"
#include "lib/lib_2f490.h"
#include "lib/libc/ll.h"
#include "data.h"
#include "types.h"

struct debugtri {
	s16 vertices[3][3];
	u8 unk12;
};

union filedataptr g_TileFileData;
s32 g_TileNumRooms;
u32 *g_TileRooms;
s32 var8009a8ac;
f32 var8009a8b0;
s32 var8009a8b4;
struct coord var8009a8b8;
s32 var8009a8c4;
struct coord var8009a8c8;
struct prop *g_CdObstacle;
s32 var8009a8d8;
s32 var8009a8dc;
struct coord g_CdPos;
s32 var8009a8ec;
f32 var8009a8f0;
bool var8009a8f4;
struct coord var8009a8f8;
s32 var8009a904;
struct coord var8009a908;
s32 var8009a914;
struct geoblock var8009a918;
struct geo *var8009a964;
s32 var8009a968;
s32 var8009a96c;

s32 var8005f030 = 0;
s32 var8005f034 = 0;
s32 var8005f038 = 0;

f32 cd00024e40(void)
{
	return var8009a8f0;
}

void cd00024e4c(struct coord *a, struct coord *b, u32 line, char *file)
{
	a->x = var8009a8b8.x;
	a->y = var8009a8b8.y;
	a->z = var8009a8b8.z;

	b->x = var8009a8c8.x;
	b->y = var8009a8c8.y;
	b->z = var8009a8c8.z;
}

f32 cd00024e98(void)
{
	return var8009a8b0;
}

s32 cd00024ea4(void)
{
	return var8009a8ac;
}

struct prop *cdGetObstacle(void)
{
	return g_CdObstacle;
}

void cdGetPos(struct coord *pos, u32 line, char *file)
{
	pos->x = g_CdPos.x;
	pos->y = g_CdPos.y;
	pos->z = g_CdPos.z;
}

void cd00024ee8(struct coord *arg0)
{
	cd00025928(var8009a964, arg0);
}

u32 cdGetTileFlags(void)
{
	u32 flags = 0;

	switch (var8009a964->type) {
	case GEOTYPE_TILE_I:
		flags = var8009a964->flags;
		break;
	case GEOTYPE_TILE_F:
		flags = var8009a964->flags;
		break;
	case GEOTYPE_BLOCK:
		flags = GEOFLAG_COLLISIONS | GEOFLAG_0008 | GEOFLAG_OPAQUE;
		break;
	case GEOTYPE_CYL:
		flags = var8009a964->flags;
		break;
	}

	return flags;
}

void cd00024f6c(void)
{
	var8009a8b4 = 0;
	var8009a8ac = 0;
	g_CdObstacle = NULL;
	var8009a8d8 = 0;
	var8009a8ec = 0;
	var8009a8f4 = false;
	var8005f034 = 0;
	var8005f038 = 0;
}

void cd00024fb0(struct coord *arg0, struct coord *arg1, struct prop *prop)
{
	var8009a8b8.x = arg0->x;
	var8009a8b8.y = arg0->y;
	var8009a8b8.z = arg0->z;

	var8009a8c8.x = arg1->x;
	var8009a8c8.y = arg1->y;
	var8009a8c8.z = arg1->z;

	var8009a8b4 = 1;
	var8009a8ac = 0;
	g_CdObstacle = prop;
	var8009a8d8 = 0;
	var8009a8ec = 0;
	var8009a8f4 = false;
	var8005f034 = 0;
	var8005f038 = 0;
}

void cd00025038(struct coord *arg0, struct coord *arg1, struct prop *prop, f32 arg3)
{
	var8009a8b0 = arg3;

	var8009a8b8.x = arg0->x;
	var8009a8b8.y = arg0->y;
	var8009a8b8.z = arg0->z;

	var8009a8c8.x = arg1->x;
	var8009a8c8.y = arg1->y;
	var8009a8c8.z = arg1->z;

	var8009a8b4 = 1;
	var8009a8ac = 1;
	g_CdObstacle = prop;
	var8009a8d8 = 0;
	var8009a8ec = 0;
	var8009a8f4 = 0;
	var8005f034 = 0;
	var8005f038 = 0;
}

void cd000250cc(struct coord *arg0, struct coord *arg1, f32 width)
{
	struct widthxz sp34;
	struct xz sp2c;
	struct xz sp24;
	struct xz sp1c;

	sp34.width = width;
	sp34.x = arg0->x;
	sp34.z = arg0->z;

	sp1c.x = arg1->x;
	sp1c.z = arg1->z;

	sp2c.x = var8009a8b8.x;
	sp2c.z = var8009a8b8.z;

	sp24.x = var8009a8c8.x;
	sp24.z = var8009a8c8.z;

	var8009a8b0 = func0f1579cc(&sp34, &sp2c, &sp24, &sp1c);
	var8009a8ac = 1;
}

void cd00025168(struct prop *prop)
{
	var8009a8b4 = 0;
	var8009a8ac = 0;
	g_CdObstacle = prop;
	var8009a8d8 = 0;
	var8009a8ec = 0;
	var8009a8f4 = false;
	var8005f034 = 0;
	var8005f038 = 0;
}

void cd000251ac(struct coord *arg0, struct coord *arg1, struct coord *arg2, struct prop *prop)
{
	var8009a8b8.x = arg0->x;
	var8009a8b8.y = arg0->y;
	var8009a8b8.z = arg0->z;

	var8009a8c8.x = arg1->x;
	var8009a8c8.y = arg1->y;
	var8009a8c8.z = arg1->z;

	g_CdPos.x = arg2->x;
	g_CdPos.y = arg2->y;
	g_CdPos.z = arg2->z;

	var8009a8b4 = 1;
	var8009a8ac = 0;
	g_CdObstacle = prop;
	var8009a8d8 = 1;
	var8009a8ec = 0;
	var8009a8f4 = 0;
	var8005f034 = 0;
	var8005f038 = 0;
}

void cd00025254(struct coord *arg0, struct coord *arg1, struct coord *pos, struct prop *prop, f32 arg4, struct geo *geo)
{
	var8009a8b8.x = arg0->x;
	var8009a8b8.y = arg0->y;
	var8009a8b8.z = arg0->z;

	var8009a8c8.x = arg1->x;
	var8009a8c8.y = arg1->y;
	var8009a8c8.z = arg1->z;

	g_CdPos.x = pos->x;
	g_CdPos.y = pos->y;
	g_CdPos.z = pos->z;

	var8009a8b4 = 1;
	var8009a8ac = 0;
	g_CdObstacle = prop;
	var8009a8d8 = 1;
	var8009a8f0 = arg4;
	var8009a8ec = 1;
	var8009a8f4 = false;
	var8005f034 = 0;
	var8009a964 = geo;
	var8005f038 = 1;
}

void cd00025314(struct coord *arg0, struct coord *arg1)
{
	var8009a8f8.x = arg0->x;
	var8009a8f8.y = arg0->y;
	var8009a8f8.z = arg0->z;

	var8009a908.x = arg1->x;
	var8009a908.y = arg1->y;
	var8009a908.z = arg1->z;

	var8009a8f4 = true;
}

bool cd00025364(struct coord *arg0, struct coord *arg1)
{
	if (var8009a8f4) {
		arg0->x = var8009a8f8.x;
		arg0->y = var8009a8f8.y;
		arg0->z = var8009a8f8.z;

		arg1->x = var8009a908.x;
		arg1->y = var8009a908.y;
		arg1->z = var8009a908.z;
	}

	return var8009a8f4;
}

void cd000253c4(struct geoblock *block)
{
	var8009a918 = *block;
	var8005f034 = true;
}

s32 cd00025410(f32 arg0, f32 arg1, f32 arg2, f32 arg3)
{
	f32 f0 = arg0 * arg3;
	f32 f2 = arg1 * arg2;

	if (f2 < f0) {
		return 1;
	}

	if (f2 > f0) {
		return -1;
	}

	if (arg0 * arg2 < 0.0f || arg1 * arg3 < 0.0f) {
		return -1;
	}

	if (arg0 * arg0 + arg1 * arg1 < arg2 * arg2 + arg3 * arg3) {
		return 1;
	}

	return 0;
}

s32 cd000254d8(struct coord *arg0, struct coord *arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, s32 *arg6)
{
	f32 sp54;
	f32 sp50;
	f32 sp4c;
	f32 sp48;
	s32 sp44;
	s32 sp40;
	s32 sp3c;
	s32 sp38;
	s32 sp34;
	s32 sp30;
	bool result = false;

	sp54 = arg0->x - arg2;
	sp50 = arg0->z - arg3;

	sp3c = cd00025410(arg4 - arg2, arg5 - arg3, sp54, sp50);
	sp44 = cd00025410(arg4 - arg2, arg5 - arg3, arg1->x - arg2, arg1->z - arg3);
	sp38 = sp3c * sp44;

	if (sp38 <= 0) {
		sp4c = arg1->x - arg0->x;
		sp48 = arg1->z - arg0->z;

		sp34 = cd00025410(sp4c, sp48, -sp54, -sp50);
		sp40 = cd00025410(sp4c, sp48, arg4 - arg0->x, arg5 - arg0->z);
		sp30 = sp34 * sp40;

		if (sp30 <= 0) {
			result = true;
		}
	}

	if (*arg6 && (result || sp3c <= 0)) {
		*arg6 = 0;
	}

	return result;
}

f32 cd00025654(f32 x1, f32 z1, f32 x2, f32 z2, f32 x3, f32 z3)
{
	u32 stack[8];
	f32 result;

	result = sqrtf((x2 - x1) * (x2 - x1) + (z2 - z1) * (z2 - z1));

	if (result == 0.0f) {
		return sqrtf((x3 - x2) * (x3 - x2) + (z3 - z2) * (z3 - z2));
	}

	return ((x3 - x1) * (z2 - z1) + -(x2 - x1) * (z3 - z1)) / result;
}

f32 cd00025724(f32 x1, f32 z1, f32 x2, f32 z2)
{
	x2 -= x1;
	z2 -= z1;

	return sqrtf(x2 * x2 + z2 * z2);
}

bool cd00025774(f32 x1, f32 z1, f32 x2, f32 z2, f32 x3, f32 z3)
{
	f32 f0;
	f32 f2;
	f32 f16;
	f32 f18;

	x3 -= x1;
	z3 -= z1;

	f0 = x2 - x1;
	f2 = z2 - z1;

	f16 = x3 * f0 + z3 * f2;
	f18 = f0 * f0 + f2 * f2;

	return (f18 < f16 && f16 < 0) || (f16 > 0 && f16 < f18);
}

void cd00025848(f32 tilex, f32 tilez, f32 tilewidth, f32 posx, f32 posz, f32 *x1, f32 *z1, f32 *x2, f32 *z2)
{
	posx -= tilex;
	posz -= tilez;

	if (posx != 0 || posz != 0) {
		f32 dist = sqrtf(posx * posx + posz * posz);

		if (dist > 0) {
			dist = tilewidth / dist;
			posx *= dist;
			posz *= dist;
		}
	}

	*x1 = tilex + posx + posz;
	*z1 = tilez + posz - posx;
	*x2 = tilex + posx - posz;
	*z2 = tilez + posz + posx;
}

void cd00025928(struct geo *geo, struct coord *arg1)
{
	if (geo->type == GEOTYPE_TILE_I) {
		struct geotilei *tile0 = (struct geotilei *) geo;
		s32 sp38[3];
		s32 sp2c[3];
		s32 sp20[3];

		sp38[0] = tile0->vertices[1][0] - tile0->vertices[0][0];
		sp38[1] = tile0->vertices[1][1] - tile0->vertices[0][1];
		sp38[2] = tile0->vertices[1][2] - tile0->vertices[0][2];

		sp2c[0] = tile0->vertices[2][0] - tile0->vertices[0][0];
		sp2c[1] = tile0->vertices[2][1] - tile0->vertices[0][1];
		sp2c[2] = tile0->vertices[2][2] - tile0->vertices[0][2];

		sp20[0] = sp38[1] * sp2c[2] - sp38[2] * sp2c[1];
		sp20[1] = sp38[2] * sp2c[0] - sp38[0] * sp2c[2];
		sp20[2] = sp38[0] * sp2c[1] - sp38[1] * sp2c[0];

		arg1->x = sp20[0];
		arg1->y = sp20[1];
		arg1->z = sp20[2];
	} else if (geo->type == GEOTYPE_TILE_F) {
		struct geotilef *tile1 = (struct geotilef *) geo;
		f32 sp10[3];
		f32 sp04[3];

		sp10[0] = tile1->vertices[1].x - tile1->vertices[0].x;
		sp10[1] = tile1->vertices[1].y - tile1->vertices[0].y;
		sp10[2] = tile1->vertices[1].z - tile1->vertices[0].z;

		sp04[0] = tile1->vertices[2].x - tile1->vertices[0].x;
		sp04[1] = tile1->vertices[2].y - tile1->vertices[0].y;
		sp04[2] = tile1->vertices[2].z - tile1->vertices[0].z;

		arg1->x = sp10[1] * sp04[2] - sp10[2] * sp04[1];
		arg1->y = sp10[2] * sp04[0] - sp10[0] * sp04[2];
		arg1->z = sp10[0] * sp04[1] - sp10[1] * sp04[0];
	} else if (geo->type == GEOTYPE_BLOCK) {
		arg1->x = 0;
		arg1->y = 1;
		arg1->z = 0;
	} else if (geo->type == GEOTYPE_CYL) {
		arg1->x = 0;
		arg1->y = 1;
		arg1->z = 0;
	}
}

void cdGetFloorCol(struct geo *geo, u16 *floorcol)
{
	if (geo == NULL) {
		*floorcol = 0xfff;
		return;
	}

	if (geo->type == GEOTYPE_TILE_I) {
		struct geotilei *tile0 = (struct geotilei *) geo;
		*floorcol = tile0->floorcol;
		return;
	}

	if (geo->type == GEOTYPE_TILE_F) {
		struct geotilef *tile1 = (struct geotilef *) geo;
		*floorcol = tile1->floorcol;
		return;
	}

	if (geo->type == GEOTYPE_BLOCK) {
		*floorcol = 0xfff;
		return;
	}

	if (geo->type == GEOTYPE_CYL) {
		*floorcol = 0xfff;
	}
}

void cdGetFloorType(struct geo *geo, u8 *floortype)
{
	bool water = false;

	if (geo && (geo->flags & GEOFLAG_UNDERWATER)) {
		water = true;
	}

	if (geo == NULL) {
		*floortype = 0xff;
		return;
	}

	if (water) {
		*floortype = FLOORTYPE_WATER;
		return;
	}

	if (geo->type == GEOTYPE_TILE_I) {
		struct geotilei *tile0 = (struct geotilei *) geo;
		*floortype = tile0->floortype;
		return;
	}

	if (geo->type == GEOTYPE_TILE_F) {
		struct geotilef *tile1 = (struct geotilef *) geo;
		*floortype = tile1->floortype;
		return;
	}

	if (geo->type == GEOTYPE_BLOCK) {
		*floortype = 0xff;
		return;
	}

	if (geo->type == GEOTYPE_CYL) {
		*floortype = 0xff;
	}
}

f32 cdFindGroundInIntTileAtVertex(struct geotilei *tile, f32 x, f32 z, s32 vertexindex)
{
	struct coord sp7c;
	struct coord sp70;
	s64 sp68;
	s64 sp60;
	s64 sp58;
	s64 tmp;
	f32 ground;
	s32 next;

	if (vertexindex == 0) {
		vertexindex = 1;
	}

	next = (vertexindex + 1) % tile->header.numvertices;

	if (next == 0) {
		next = 1;
	}

	sp7c.x = tile->vertices[vertexindex][0] - tile->vertices[0][0];
	sp7c.y = tile->vertices[vertexindex][1] - tile->vertices[0][1];
	sp7c.z = tile->vertices[vertexindex][2] - tile->vertices[0][2];

	sp70.x = tile->vertices[next][0] - tile->vertices[0][0];
	sp70.y = tile->vertices[next][1] - tile->vertices[0][1];
	sp70.z = tile->vertices[next][2] - tile->vertices[0][2];

	sp58 = sp7c.f[1] * sp70.f[2] - sp7c.f[2] * sp70.f[1];
	sp60 = sp7c.f[2] * sp70.f[0] - sp7c.f[0] * sp70.f[2];
	sp68 = sp7c.f[0] * sp70.f[1] - sp7c.f[1] * sp70.f[0];

	tmp = sp58 * tile->vertices[0][0]
		+ sp60 * tile->vertices[0][1]
		+ sp68 * tile->vertices[0][2];

	if (sp60 == 0) {
		return *(s16 *)(tile->ymax + (u32)tile);
	}

	ground = (tmp - (f64)x * sp58 - (f64)z * sp68) / sp60;

	if (ground > *(s16 *)(tile->ymax + (u32)tile)) {
		ground = *(s16 *)(tile->ymax + (u32)tile);
	} else if (ground < *(s16 *)(tile->ymin + (u32)tile)) {
		ground = *(s16 *)(tile->ymin + (u32)tile);
	}

	return ground;
}

#if VERSION < VERSION_NTSC_1_0
f32 cdFindGroundInTileType0AtVertex1(struct geotilei *tile, f32 x, f32 z)
{
	return cdFindGroundInIntTileAtVertex(tile, x, z, 1);
}
#endif

GLOBAL_ASM(
glabel cdFindGroundInIntTile
/*    25f90:	27bdffd8 */ 	addiu	$sp,$sp,-40
/*    25f94:	f7b40010 */ 	sdc1	$f20,0x10($sp)
/*    25f98:	4485a000 */ 	mtc1	$a1,$f20
/*    25f9c:	afbf0024 */ 	sw	$ra,0x24($sp)
/*    25fa0:	f7b60018 */ 	sdc1	$f22,0x18($sp)
/*    25fa4:	90850001 */ 	lbu	$a1,0x1($a0)
/*    25fa8:	4486b000 */ 	mtc1	$a2,$f22
/*    25fac:	24070001 */ 	addiu	$a3,$zero,0x1
/*    25fb0:	28a10004 */ 	slti	$at,$a1,0x4
/*    25fb4:	1420003c */ 	bnez	$at,.L000260a8
/*    25fb8:	2403ffff */ 	addiu	$v1,$zero,-1
/*    25fbc:	28a10002 */ 	slti	$at,$a1,0x2
/*    25fc0:	14200039 */ 	bnez	$at,.L000260a8
/*    25fc4:	00077880 */ 	sll	$t7,$a3,0x2
/*    25fc8:	848e0012 */ 	lh	$t6,0x12($a0)
/*    25fcc:	8498000e */ 	lh	$t8,0xe($a0)
/*    25fd0:	01e77823 */ 	subu	$t7,$t7,$a3
/*    25fd4:	448e2000 */ 	mtc1	$t6,$f4
/*    25fd8:	44983000 */ 	mtc1	$t8,$f6
/*    25fdc:	000f7840 */ 	sll	$t7,$t7,0x1
/*    25fe0:	44807000 */ 	mtc1	$zero,$f14
/*    25fe4:	008f1021 */ 	addu	$v0,$a0,$t7
/*    25fe8:	46802420 */ 	cvt.s.w	$f16,$f4
/*    25fec:	468034a0 */ 	cvt.s.w	$f18,$f6
.L00025ff0:
/*    25ff0:	84590012 */ 	lh	$t9,0x12($v0)
/*    25ff4:	8448000e */ 	lh	$t0,0xe($v0)
/*    25ff8:	44994000 */ 	mtc1	$t9,$f8
/*    25ffc:	44885000 */ 	mtc1	$t0,$f10
/*    26000:	46804020 */ 	cvt.s.w	$f0,$f8
/*    26004:	468050a0 */ 	cvt.s.w	$f2,$f10
/*    26008:	46008101 */ 	sub.s	$f4,$f16,$f0
/*    2600c:	4602a181 */ 	sub.s	$f6,$f20,$f2
/*    26010:	4600b281 */ 	sub.s	$f10,$f22,$f0
/*    26014:	46062202 */ 	mul.s	$f8,$f4,$f6
/*    26018:	46029101 */ 	sub.s	$f4,$f18,$f2
/*    2601c:	46045182 */ 	mul.s	$f6,$f10,$f4
/*    26020:	46064301 */ 	sub.s	$f12,$f8,$f6
/*    26024:	460e6032 */ 	c.eq.s	$f12,$f14
/*    26028:	00000000 */ 	nop
/*    2602c:	4503001b */ 	bc1tl	.L0002609c
/*    26030:	24e70001 */ 	addiu	$a3,$a3,0x1
/*    26034:	04610008 */ 	bgez	$v1,.L00026058
/*    26038:	00000000 */ 	nop
/*    2603c:	460c703c */ 	c.lt.s	$f14,$f12
/*    26040:	00001825 */ 	or	$v1,$zero,$zero
/*    26044:	45020015 */ 	bc1fl	.L0002609c
/*    26048:	24e70001 */ 	addiu	$a3,$a3,0x1
/*    2604c:	24030001 */ 	addiu	$v1,$zero,0x1
/*    26050:	10000012 */ 	b	.L0002609c
/*    26054:	24e70001 */ 	addiu	$a3,$a3,0x1
.L00026058:
/*    26058:	10600007 */ 	beqz	$v1,.L00026078
/*    2605c:	00000000 */ 	nop
/*    26060:	460e603c */ 	c.lt.s	$f12,$f14
/*    26064:	00000000 */ 	nop
/*    26068:	45000003 */ 	bc1f	.L00026078
/*    2606c:	00000000 */ 	nop
/*    26070:	1000000d */ 	b	.L000260a8
/*    26074:	24e7ffff */ 	addiu	$a3,$a3,-1
.L00026078:
/*    26078:	54600008 */ 	bnezl	$v1,.L0002609c
/*    2607c:	24e70001 */ 	addiu	$a3,$a3,0x1
/*    26080:	460c703c */ 	c.lt.s	$f14,$f12
/*    26084:	00000000 */ 	nop
/*    26088:	45020004 */ 	bc1fl	.L0002609c
/*    2608c:	24e70001 */ 	addiu	$a3,$a3,0x1
/*    26090:	10000005 */ 	b	.L000260a8
/*    26094:	24e7ffff */ 	addiu	$a3,$a3,-1
/*    26098:	24e70001 */ 	addiu	$a3,$a3,0x1
.L0002609c:
/*    2609c:	00e5082a */ 	slt	$at,$a3,$a1
/*    260a0:	1420ffd3 */ 	bnez	$at,.L00025ff0
/*    260a4:	24420006 */ 	addiu	$v0,$v0,0x6
.L000260a8:
/*    260a8:	4405a000 */ 	mfc1	$a1,$f20
/*    260ac:	4406b000 */ 	mfc1	$a2,$f22
/*    260b0:	0c00971d */ 	jal	cdFindGroundInIntTileAtVertex
/*    260b4:	00000000 */ 	nop
/*    260b8:	8fbf0024 */ 	lw	$ra,0x24($sp)
/*    260bc:	d7b40010 */ 	ldc1	$f20,0x10($sp)
/*    260c0:	d7b60018 */ 	ldc1	$f22,0x18($sp)
/*    260c4:	03e00008 */ 	jr	$ra
/*    260c8:	27bd0028 */ 	addiu	$sp,$sp,0x28
);

// Mismatch: regalloc, and operators for second mult are swapped
//f32 cdFindGroundInIntTile(struct geotilei *tile, f32 x, f32 z)
//{
//	s32 i = 1;
//	s32 ival = -1;
//	f32 tmpx;
//	f32 tmpz;
//	f32 fval;
//
//	if (tile->header.numvertices >= 4) {
//		while (i < tile->header.numvertices) { \
//			tmpz = tile->vertices[i][2];
//			tmpx = tile->vertices[i][0];
//
//			fval =
//				(tile->vertices[0][2] - tmpz) * (x - tmpx) -
//				(tile->vertices[0][0] - tmpx) * (z - tmpz);
//
//			if (fval != 0) {
//				if (ival < 0) {
//					ival = (fval > 0);
//				} else if (ival != 0 && fval < 0) {
//					i--;
//					break;
//				} else if (ival == 0 && fval > 0) {
//					i--;
//					break;
//				}
//			}
//
//			i++;
//		}
//	}
//
//	return cdFindGroundInIntTileAtVertex(tile, x, z, i);
//}

f32 cdFindGroundInFltTile(struct geotilef *tile, f32 x, f32 z)
{
	struct coord sp24;
	struct coord sp18;
	struct coord sp0c;
	f32 tmp;
	f32 ground;

	sp24.x = tile->vertices[1].x - tile->vertices[0].x;
	sp24.y = tile->vertices[1].y - tile->vertices[0].y;
	sp24.z = tile->vertices[1].z - tile->vertices[0].z;

	sp18.x = tile->vertices[2].x - tile->vertices[0].x;
	sp18.y = tile->vertices[2].y - tile->vertices[0].y;
	sp18.z = tile->vertices[2].z - tile->vertices[0].z;

	sp0c.x = sp24.f[1] * sp18.f[2] - sp24.f[2] * sp18.f[1];
	sp0c.y = sp24.f[2] * sp18.f[0] - sp24.f[0] * sp18.f[2];
	sp0c.z = sp24.f[0] * sp18.f[1] - sp24.f[1] * sp18.f[0];

	tmp = sp0c.f[0] * tile->vertices[0].f[0]
		+ sp0c.f[1] * tile->vertices[0].f[1]
		+ sp0c.f[2] * tile->vertices[0].f[2];

	if (sp0c.f[1] == 0) {
		return tile->vertices[tile->ymax].y;
	}

	ground = (tmp - (f64)x * (f64)sp0c.f[0] - (f64)z * (f64)sp0c.f[2]) / (f64)sp0c.f[1];

	if (ground > tile->vertices[tile->ymax].y) {
		ground = tile->vertices[tile->ymax].y;
	} else if (ground < tile->vertices[tile->ymin].y) {
		ground = tile->vertices[tile->ymin].y;
	}

	return ground;
}

bool cdIs2dPointInIntTile(struct geotilei *tile, f32 x, f32 z)
{
	s32 result = -1;
	s32 numvertices = tile->header.numvertices;
	s32 i;

	for (i = 0; i < numvertices; i++) {
		s32 next = (i + 1) % numvertices;

		f32 value = ((f32)tile->vertices[next][2] - (f32)tile->vertices[i][2]) * (x - tile->vertices[i][0])
			- ((f32)tile->vertices[next][0] - (f32)tile->vertices[i][0]) * (z - tile->vertices[i][2]);

		if (value != 0) {
			if (i == 0 || result < 0) {
				result = (value > 0);
			} else {
				if (result != 0 && value < 0) {
					return false;
				}

				if (result == 0 && value > 0) {
					return false;
				}
			}
		}
	}

	if (result < 0) {
		return false;
	}

	return true;
}

bool cdIs2dPointInFltTile(struct geotilef *tile, f32 x, f32 z)
{
	s32 result = -1;
	s32 numvertices = tile->header.numvertices;
	s32 i;

	for (i = 0; i < numvertices; i++) {
		s32 next = (i + 1) % numvertices;

		f32 value = (tile->vertices[next].z - tile->vertices[i].z) * (x - tile->vertices[i].x)
			- (tile->vertices[next].x - tile->vertices[i].x) * (z - tile->vertices[i].z);

		if (value != 0) {
			if (i == 0 || result < 0) {
				result = (value > 0);
			} else {
				if (result != 0 && value < 0) {
					return false;
				}

				if (result == 0 && value > 0) {
					return false;
				}
			}
		}
	}

	if (result < 0) {
		return false;
	}

	return true;
}

bool cdIs2dPointInBlock(struct geoblock *tile, f32 x, f32 z)
{
	s32 result = -1;
	s32 numvertices = tile->header.numvertices;
	s32 i;

	for (i = 0; i < numvertices; i++) {
		s32 next = (i + 1) % numvertices;

		f32 value = (tile->vertices[next][1] - tile->vertices[i][1]) * (x - tile->vertices[i][0])
			- (tile->vertices[next][0] - tile->vertices[i][0]) * (z - tile->vertices[i][1]);

		if (value != 0) {
			if (i == 0 || result < 0) {
				result = (value > 0);
			} else {
				if (result != 0 && value < 0) {
					return false;
				}

				if (result == 0 && value > 0) {
					return false;
				}
			}
		}
	}

	if (result < 0) {
		return false;
	}

	return true;
}

bool cdIs2dPointInCyl(struct geocyl *cyl, f32 x, f32 z)
{
	f32 xdiff = x - cyl->x;
	f32 zdiff = z - cyl->z;

	return xdiff * xdiff + zdiff * zdiff <= cyl->radius * cyl->radius;
}

bool func000266a4(f32 x, f32 z, struct geo *geo)
{
	if (geo == NULL) {
		return false;
	}

	if (geo->type == GEOTYPE_BLOCK) {
		return cdIs2dPointInBlock((struct geoblock *) geo, x, z);
	}

	if (geo->type == GEOTYPE_CYL) {
		return cdIs2dPointInCyl((struct geocyl *) geo, x, z);
	}

	return false;
}

/**
 * For a lift or escalator step, find the props which are riding on it.
 */
void cdGetPropsOnPlatform(struct prop *platform, s16 *propnums, s32 maxlen)
{
	u8 *start;
	u8 *end;
	s16 roompropnums[257];
	struct prop *prop;
	s16 *roompropnumptr;
	struct geo *geo;
	s32 len = 0;

	if (propUpdateGeometry(platform, &start, &end)) {
		roomGetProps(platform->rooms, roompropnums, 256);
		roompropnumptr = roompropnums;

		while (*roompropnumptr >= 0) {
			prop = &g_Vars.props[*roompropnumptr];

			if (prop != platform) {
				geo = (struct geo *) start;

				while (geo < (struct geo *) end) {
					if (geo->type == GEOTYPE_TILE_I) {
						struct geotilei *tile = (struct geotilei *) geo;
						geo = (struct geo *)((u32)geo + sizeof(struct geotilei) + sizeof(tile->vertices[0]) * (tile->header.numvertices - ARRAYCOUNT(tile->vertices)));
					} else if (geo->type == GEOTYPE_TILE_F) {
						struct geotilef *type1 = (struct geotilef *) geo;
						struct coord *pos = &prop->pos;

						if ((geo->flags & (GEOFLAG_0001 | GEOFLAG_0002))
								&& pos->x >= type1->vertices[type1->xmin].x
								&& pos->x <= type1->vertices[type1->xmax].x
								&& pos->z >= type1->vertices[type1->zmin].z
								&& pos->z <= type1->vertices[type1->zmax].z
								&& pos->y >= type1->vertices[type1->ymin].y
								&& cdIs2dPointInFltTile(type1, pos->x, pos->z)
								&& pos->y >= cdFindGroundInFltTile(type1, pos->x, pos->z)) {
							break;
						}

						geo = (struct geo *)((u32)geo + sizeof(struct geotilef) + sizeof(struct coord) * (type1->header.numvertices - ARRAYCOUNT(type1->vertices)));
					} else if (geo->type == GEOTYPE_BLOCK) {
						geo = (struct geo *)((u32)geo + sizeof(struct geoblock));
					} else if (geo->type == GEOTYPE_CYL) {
						geo = (struct geo *)((u32)geo + sizeof(struct geocyl));
					}
				}

				if (geo < (struct geo *) end) {
					if (len < maxlen - 2) {
						propnums[len] = *roompropnumptr;
						len++;
					} else {
						break;
					}
				}
			}

			roompropnumptr++;
		}
	}

	propnums[len] = -1;
}

#if VERSION < VERSION_NTSC_1_0
void cdSetPropYBounds(struct prop *prop, f32 ymax, f32 ymin)
{
	u8 *start;
	u8 *end;

	if (propUpdateGeometry(prop, &start, &end)) {
		struct geo *geo = (struct geo *) start;

		while (geo < (struct geo *) end) {
			if (geo->type == GEOTYPE_TILE_I) {
				struct geotilei *tile = (struct geotilei *) geo;
				geo = (struct geo *)((u32)geo + sizeof(struct geotilei) + sizeof(tile->vertices[0]) * (tile->header.numvertices - ARRAYCOUNT(tile->vertices)));
			} else if (geo->type == GEOTYPE_TILE_F) {
				struct geotilef *type1 = (struct geotilef *) geo;
				geo = (struct geo *)((u32)geo + sizeof(struct geotilef) + sizeof(struct coord) * (type1->header.numvertices - ARRAYCOUNT(type1->vertices)));
			} else if (geo->type == GEOTYPE_BLOCK) {
				struct geoblock *block = (struct geoblock *) geo;
				block->ymax = ymax;
				block->ymin = ymin;
				geo = (struct geo *)((u32)geo + sizeof(struct geoblock));
			} else if (geo->type == GEOTYPE_CYL) {
				struct geocyl *cyl = (struct geocyl *) geo;
				cyl->ymax = ymax;
				cyl->ymin = ymin;
				geo = (struct geo *)((u32)geo + sizeof(struct geocyl));
			}
		}
	}
}
#endif

bool cd00026a04(struct coord *pos, u8 *start, u8 *end, u16 flags, s32 room, struct geo **tileptr, s32 *roomptr, f32 *groundptr, bool arg8)
{
	bool result = false;
	struct geo *geo = (struct geo *) start;

	if (room);

	while (geo < (struct geo *) end) {
		if (geo->type == GEOTYPE_TILE_I) {
			struct geotilei *tile = (struct geotilei *) geo;

			if ((geo->flags & flags)
					&& pos->x >= *(s16 *)(tile->xmin + (u32)tile)
					&& pos->x <= *(s16 *)(tile->xmax + (u32)tile)
					&& pos->z >= *(s16 *)(tile->zmin + (u32)tile)
					&& pos->z <= *(s16 *)(tile->zmax + (u32)tile)) {
				if ((!arg8 && pos->y >= *(s16 *)(tile->ymin + (u32)tile))
						|| (arg8 && pos->y <= *(s16 *)(tile->ymax + (u32)tile))) {
					if (cdIs2dPointInIntTile(tile, pos->x, pos->z)) {
						f32 ground = cdFindGroundInIntTile(tile, pos->x, pos->z);

						if ((!arg8 && ground <= pos->y && ground > *groundptr)
								|| (arg8 && ground >= pos->y && ground < *groundptr)) {
							*groundptr = ground;
							*tileptr = geo;
							*roomptr = room;
							result = true;
						}
					}
				}
			}

			geo = (struct geo *)((u32)geo + sizeof(struct geotilei) + sizeof(tile->vertices[0]) * (tile->header.numvertices - ARRAYCOUNT(tile->vertices)));
		} else if (geo->type == GEOTYPE_TILE_F) {
			struct geotilef *type1 = (struct geotilef *) geo;

			if ((geo->flags & flags)
					&& pos->x >= type1->vertices[type1->xmin].x
					&& pos->x <= type1->vertices[type1->xmax].x
					&& pos->z >= type1->vertices[type1->zmin].z
					&& pos->z <= type1->vertices[type1->zmax].z) {
				if ((!arg8 && pos->y >= type1->vertices[type1->ymin].y)
						|| (arg8 && pos->y <= type1->vertices[type1->ymax].y)) {
					if (cdIs2dPointInFltTile(type1, pos->x, pos->z)) {
						f32 ground = cdFindGroundInFltTile(type1, pos->x, pos->z);

						if ((!arg8 && pos->y >= ground && ground > *groundptr)
								|| (arg8 && pos->y <= ground && ground < *groundptr)) {
							*groundptr = ground;
							*tileptr = geo;
							*roomptr = room;
							result = true;
						}
					}
				}
			}

			geo = (struct geo *)((u32)geo + sizeof(struct geotilef) + sizeof(struct coord) * (type1->header.numvertices - ARRAYCOUNT(type1->vertices)));
		} else if (geo->type == GEOTYPE_BLOCK) {
			geo = (struct geo *)((u32)geo + sizeof(struct geoblock));
		} else if (geo->type == GEOTYPE_CYL) {
			geo = (struct geo *)((u32)geo + sizeof(struct geocyl));
		}
	}

	return result;
}

void cd00026e7c(struct coord *pos, s16 *rooms, u16 arg2, struct geo **tileptr, s16 *arg4, f32 *arg5, struct prop **arg6, s32 arg7)
{
	s16 *roomptr;
	s32 roomnum;
	u8 *start;
	u8 *end;
	f32 sp274;
	struct geo *sp270 = NULL;
	s32 sp26c = 0;
	struct prop *sp268 = NULL;
	s16 *propnumptr;
	s16 propnums[256];

	if (arg7) {
		sp274 = 4294967296;
	} else {
		sp274 = -4294967296;
	}

	roomptr = rooms;
	roomnum = rooms[0];

	while (roomnum != -1) {
		if (roomnum < g_TileNumRooms) {
			start = g_TileFileData.u8 + g_TileRooms[roomnum];
			end = g_TileFileData.u8 + g_TileRooms[roomnum + 1];

			cd00026a04(pos, start, end, arg2, roomnum, &sp270, &sp26c, &sp274, arg7);
		}

		roomptr++;
		roomnum = *roomptr;
	}

	roomGetProps(rooms, propnums, 256);
	propnumptr = propnums;

	while (*propnumptr >= 0) {
		struct prop *prop = &g_Vars.props[*propnumptr];

		if (propUpdateGeometry(prop, &start, &end)
				&& cd00026a04(pos, start, end, arg2, prop->rooms[0], &sp270, &sp26c, &sp274, arg7)) {
			sp268 = prop;
		}

		propnumptr++;
	}

	*tileptr = sp270;
	*arg4 = sp26c;
	*arg5 = sp274;

	if (arg6 != NULL) {
		*arg6 = sp268;
	}
}

bool cd0002709c(struct geotilei *tile, f32 x, f32 z, f32 width, struct prop *prop, struct collisionthing *thing)
{
	bool result = false;

	if (cdIs2dPointInIntTile(tile, x, z)) {
		thing->geo = &tile->header;
		thing->vertexindex = 0;
		thing->prop = prop;
		result = true;
	} else {
		s32 numvertices = tile->header.numvertices;
		s32 i;

		for (i = 0; i < numvertices; i++) {
			s32 next = (i + 1) % numvertices;
			f32 value = cd00025654(tile->vertices[i][0], tile->vertices[i][2], tile->vertices[next][0], tile->vertices[next][2], x, z);

			if (value < 0) {
				value = -value;
			}

			if (value <= width
					&& (cd00025724(tile->vertices[i][0], tile->vertices[i][2], x, z) <= width
						|| cd00025724(tile->vertices[next][0], tile->vertices[next][2], x, z) <= width
						|| cd00025774(tile->vertices[i][0], tile->vertices[i][2], tile->vertices[next][0], tile->vertices[next][2], x, z))) {
				thing->geo = &tile->header;
				thing->vertexindex = i;
				thing->prop = prop;
				result = true;
				break;
			}
		}
	}

	return result;
}

bool cd000272f8(struct geotilef *tile, f32 x, f32 z, f32 radius, struct prop *prop, struct collisionthing *thing)
{
	bool result = false;

	if (cdIs2dPointInFltTile(tile, x, z)) {
		thing->geo = &tile->header;
		thing->vertexindex = 0;
		thing->prop = prop;
		result = true;
	} else {
		s32 numvertices = tile->header.numvertices;
		s32 i;

		for (i = 0; i < numvertices; i++) {
			s32 next = (i + 1) % numvertices;
			f32 value = cd00025654(tile->vertices[i].x, tile->vertices[i].z, tile->vertices[next].x, tile->vertices[next].z, x, z);

			if (value < 0) {
				value = -value;
			}

			if (value <= radius
					&& (cd00025724(tile->vertices[i].x, tile->vertices[i].z, x, z) <= radius
						|| cd00025724(tile->vertices[next].x, tile->vertices[next].z, x, z) <= radius
						|| cd00025774(tile->vertices[i].x, tile->vertices[i].z, tile->vertices[next].x, tile->vertices[next].z, x, z))) {
				thing->geo = &tile->header;
				thing->vertexindex = i;
				thing->prop = prop;
				result = true;
				break;
			}
		}
	}

	return result;
}

s32 cd000274e0(struct geoblock *tile, f32 x, f32 z, f32 width, struct prop *prop, struct collisionthing *thing)
{
	bool result = false;

	if (cdIs2dPointInBlock(tile, x, z)) {
		if (thing) {
			thing->geo = &tile->header;
			thing->vertexindex = 0;
			thing->prop = prop;
		}

		result = true;
	} else {
		s32 numvertices = tile->header.numvertices;
		s32 i;

		for (i = 0; i < numvertices; i++) {
			s32 next = (i + 1) % numvertices;
			f32 value = cd00025654(tile->vertices[i][0], tile->vertices[i][1],
					tile->vertices[next][0], tile->vertices[next][1],
					x, z);

			if (value < 0) {
				value = -value;
			}

			if (value <= width
					&& (cd00025724(tile->vertices[i][0], tile->vertices[i][1], x, z) <= width
						|| cd00025724(tile->vertices[next][0], tile->vertices[next][1], x, z) <= width
						|| cd00025774(tile->vertices[i][0], tile->vertices[i][1], tile->vertices[next][0], tile->vertices[next][1], x, z))) {
				if (thing) {
					thing->geo = &tile->header;
					thing->vertexindex = i;
					thing->prop = prop;
				}

				result = true;
				break;
			}
		}
	}

	return result;
}

bool cd000276c8(struct geocyl *cyl, f32 x, f32 z, f32 width, struct prop *prop, struct collisionthing *thing)
{
	bool result = false;

	f32 sumx = x - cyl->x;
	f32 sumz = z - cyl->z;
	f32 sumwidth = cyl->radius + width;

	if (sumx * sumx + sumz * sumz <= sumwidth * sumwidth) {
		result = true;

		if (thing) {
			thing->geo = &cyl->header;
			thing->vertexindex = 0;
			thing->prop = prop;
		}
	}

	return result;
}

void cd00027738(struct coord *pos, f32 radius, u8 *start, u8 *end, u16 flags,
		bool checkvertical, f32 arg6, f32 arg7, struct prop *prop,
		struct collisionthing *things, s32 maxthings, s32 *thingnum, s32 roomnum)
{
	struct geo *geo = (struct geo *) start;
	s32 result;

	while (geo < (struct geo *) end) {
		if (geo->type == GEOTYPE_TILE_I) {
			struct geotilei *tile = (struct geotilei *) geo;

			if ((geo->flags & flags)
					&& pos->x >= *(s16 *)(tile->xmin + (u32)tile) - radius
					&& pos->x <= *(s16 *)(tile->xmax + (u32)tile) + radius
					&& pos->z >= *(s16 *)(tile->zmin + (u32)tile) - radius
					&& pos->z <= *(s16 *)(tile->zmax + (u32)tile) + radius
					&& (!checkvertical || (pos->y + arg6 >= *(s16 *)(tile->ymin + (u32)tile)
							&& pos->y + arg7 <= *(s16 *)(tile->ymax + (u32)tile)))) {
				if (geo->flags & GEOFLAG_0080) {
					result = cd00028200(tile, pos, radius, pos->y + arg7, pos->y + arg6);
				} else {
					result = 1;
				}

				if (result != 0) {
					if (cd0002709c(tile, pos->x, pos->z, radius, prop, &things[*thingnum])) {
						things[*thingnum].room = roomnum;
						*thingnum = *thingnum + 1;

						if (*thingnum >= maxthings) {
							break;
						}
					}
				}
			}

			geo = (struct geo *)((u32)geo + tile->header.numvertices * 6 + 0xe);
		} else if (geo->type == GEOTYPE_TILE_F) {
			struct geotilef *tile = (struct geotilef *) geo;
			s32 tmp = 0x40;

			if ((geo->flags & flags)
					&& pos->x >= *(f32 *)((u32)tile + tile->xmin * 0xc + 0x10) - radius
					&& pos->x <= *(f32 *)((u32)tile + tile->xmax * 0xc + 0x10) + radius
					&& pos->z >= *(f32 *)((u32)tile + tile->zmin * 0xc + 0x18) - radius
					&& pos->z <= *(f32 *)((u32)tile + tile->zmax * 0xc + 0x18) + radius
					&& (!checkvertical || (pos->y + arg6 >= *(f32*)((u32)tile + tile->ymin * 0xc + 0x14)
							&& pos->y + arg7 <= *(f32 *)((u32)tile + tile->ymax * 0xc + 0x14)))) {
				result = cd000272f8(tile, pos->x, pos->z, radius, prop, &things[*thingnum]);

				if (result != 0) {
					things[*thingnum].room = roomnum;
					*thingnum = *thingnum + 1;

					if (*thingnum >= maxthings) {
						break;
					}
				}
			}

			geo = (struct geo *)((u32)geo + (tile->header.numvertices - tmp) * 0xc + 0x310);
		} else if (geo->type == GEOTYPE_BLOCK) {
			struct geoblock *block = (struct geoblock *) geo;

			if ((flags & (GEOFLAG_COLLISIONS | GEOFLAG_0008 | GEOFLAG_OPAQUE))
					&& (!checkvertical || (pos->y + arg6 >= block->ymin && pos->y + arg7 <= block->ymax))) {
				result = cd000274e0(block, pos->x, pos->z, radius, prop, &things[*thingnum]);

				if (result) {
					things[*thingnum].room = roomnum;
					*thingnum = *thingnum + 1;

					if (*thingnum >= maxthings) {
						break;
					}
				}
			}

			geo = (struct geo *)((u32)geo + 0x4c);
		} else if (geo->type == GEOTYPE_CYL) {
			struct geocyl *cyl = (struct geocyl *) geo;

			if ((flags & geo->flags)
					&& (!checkvertical || (pos->y + arg6 >= cyl->ymin && pos->y + arg7 <= cyl->ymax))) {
				result = cd000276c8(cyl, pos->x, pos->z, radius, prop, &things[*thingnum]);

				if (result) {
					things[*thingnum].room = roomnum;
					*thingnum = *thingnum + 1;

					if (*thingnum >= maxthings) {
						break;
					}
				}
			}

			geo = (struct geo *)((u32)geo + 0x18);
		}
	}
}

void cd00027d1c(struct coord *pos, f32 radius, s16 *rooms, u32 types, u16 arg4, u32 arg5, f32 arg6, f32 arg7, struct collisionthing *arg8, s32 arg9)
{
	s16 *roomptr;
	s32 roomnum;
	u8 *start;
	u8 *end;
	s32 sp294 = 0;
	s16 *propnumptr;
	s16 propnums[256];

	// Check BG
	if (types & CDTYPE_BG) {
		roomptr = rooms;
		roomnum = rooms[0];

		while (roomnum != -1) {
			if (roomnum < g_TileNumRooms) {
				start = g_TileFileData.u8 + g_TileRooms[roomnum];
				end = g_TileFileData.u8 + g_TileRooms[roomnum + 1];

				cd00027738(pos, radius, start, end, arg4, arg5, arg6, arg7, NULL, arg8, arg9, &sp294, roomnum);

				if (sp294 >= arg9) {
					goto end;
				}
			}

			roomptr++;
			roomnum = *roomptr;
		}
	}

	// Check props
	roomGetProps(rooms, propnums, 256);
	propnumptr = propnums;

	while (*propnumptr >= 0) {
		struct prop *prop = &g_Vars.props[*propnumptr];

		if (propIsOfCdType(prop, types) && propUpdateGeometry(prop, &start, &end)) {
			cd00027738(pos, radius, start, end, arg4, arg5, arg6, arg7, prop, arg8, arg9, &sp294, prop->rooms[0]);

			if (sp294 >= arg9) {
				break;
			}
		}

		propnumptr++;
	}

end:
	arg8[sp294].geo = NULL;
}

void cd00027f78(struct geotilei *tile, f32 arg1, f32 arg2, f32 arg3, struct prop *prop, struct collisionthing *things, s32 maxthings, s32 *thingindex)
{
	s32 i;
	s32 numvertices = tile->header.numvertices;

	for (i = 0; i < numvertices; i++) {
		s32 next = (i + 1) % numvertices;

		if (tile->vertices[i][0] != tile->vertices[next][0] || tile->vertices[i][2] != tile->vertices[next][2]) {
			f32 f0 = cd00025654(tile->vertices[i][0], tile->vertices[i][2], tile->vertices[next][0], tile->vertices[next][2], arg1, arg2);

			if (f0 < 0.0f) {
				f0 = -f0;
			}

			if (f0 <= arg3
					&& (cd00025724(tile->vertices[i][0], tile->vertices[i][2], arg1, arg2) <= arg3
						|| cd00025724(tile->vertices[next][0], tile->vertices[next][2], arg1, arg2) <= arg3
						|| cd00025774(tile->vertices[i][0], tile->vertices[i][2], tile->vertices[next][0], tile->vertices[next][2], arg1, arg2))) {
				if (*thingindex < maxthings) {
					things[*thingindex].geo = &tile->header;
					things[*thingindex].vertexindex = i;
					things[*thingindex].prop = prop;
					*thingindex += 1;
				} else {
					break;
				}
			}
		}
	}
}

s32 cd00028200(struct geotilei *tile, struct coord *pos, f32 width, f32 y1, f32 y2)
{
	s32 count;
	s32 i;
	s32 y1count;
	s32 y2count;
	s32 numverts;

	if (!g_Vars.enableslopes && (tile->header.flags & GEOFLAG_0100)) {
		return 0;
	}

	numverts = tile->header.numvertices;
	y2count = 0;
	y1count = 0;
	count = 0;

	for (i = 0; i < numverts; i++) {
		s32 next = i + 1;
		s32 last = numverts - 1;
		f32 posval;
		f32 thisvals[2];
		f32 nextvals[2];
		f32 somefloat;
		f32 somefloat2;
		s32 xdiff;
		s32 zdiff;

		if (i == last) {
			next = 0;
		}

		xdiff = tile->vertices[next][0] - tile->vertices[i][0];
		zdiff = tile->vertices[next][2] - tile->vertices[i][2];

		if (xdiff < 0) {
			xdiff = -xdiff;
		}

		if (zdiff < 0) {
			zdiff = -zdiff;
		}

		if (xdiff || zdiff) {
			thisvals[1] = tile->vertices[i][1];
			nextvals[1] = tile->vertices[next][1];

			if (zdiff < xdiff) {
				thisvals[0] = tile->vertices[i][0];
				nextvals[0] = tile->vertices[next][0];
				posval = pos->x;
			} else {
				thisvals[0] = tile->vertices[i][2];
				nextvals[0] = tile->vertices[next][2];
				posval = pos->z;
			}

			somefloat = (posval - thisvals[0]) / (nextvals[0] - thisvals[0]);

			if (somefloat <= 1.0f && somefloat >= 0.0f) {
				somefloat2 = thisvals[1] + (nextvals[1] - thisvals[1]) * somefloat;

				if (somefloat2 >= y2 - 1.0f) {
					y2count++;

					if (y1count != 0) {
						count++;
					}
				} else if (somefloat2 <= y1 + 1.0f) {
					y1count++;

					if (y2count != 0) {
						count++;
					}
				} else {
					count++;
				}
			}
		}
	}

	return count;
}

void cd0002840c(struct geotilef *tile, f32 arg1, f32 arg2, f32 arg3, struct prop *prop, struct collisionthing *things, s32 maxthings, s32 *thingindex)
{
	s32 i;
	s32 numvertices = tile->header.numvertices;

	for (i = 0; i < numvertices; i++) {
		s32 next = (i + 1) % numvertices;

		if (tile->vertices[i].x != tile->vertices[next].x || tile->vertices[i].z != tile->vertices[next].z) {
			f32 f0 = cd00025654(tile->vertices[i].x, tile->vertices[i].z, tile->vertices[next].x, tile->vertices[next].z, arg1, arg2);

			if (f0 < 0.0f) {
				f0 = -f0;
			}

			if (f0 <= arg3
					&& (cd00025724(tile->vertices[i].x, tile->vertices[i].z, arg1, arg2) <= arg3
						|| cd00025724(tile->vertices[next].x, tile->vertices[next].z, arg1, arg2) <= arg3
						|| cd00025774(tile->vertices[i].x, tile->vertices[i].z, tile->vertices[next].x, tile->vertices[next].z, arg1, arg2))) {
				if (*thingindex < maxthings) {
					things[*thingindex].geo = &tile->header;
					things[*thingindex].vertexindex = i;
					things[*thingindex].prop = prop;
					*thingindex += 1;
				} else {
					break;
				}
			}
		}
	}
}

void cd00028638(struct geoblock *block, f32 arg1, f32 arg2, f32 arg3, struct prop *prop, struct collisionthing *things, s32 maxthings, s32 *thingindex)
{
	s32 i;
	s32 numvertices = block->header.numvertices;

	for (i = 0; i < numvertices; i++) {
		s32 next = (i + 1) % numvertices;

		if (block->vertices[i][0] != block->vertices[next][0] || block->vertices[i][1] != block->vertices[next][1]) {
			f32 f0 = cd00025654(block->vertices[i][0], block->vertices[i][1], block->vertices[next][0], block->vertices[next][1], arg1, arg2);

			if (f0 < 0.0f) {
				f0 = -f0;
			}

			if (f0 <= arg3
					&& (cd00025724(block->vertices[i][0], block->vertices[i][1], arg1, arg2) <= arg3
						|| cd00025724(block->vertices[next][0], block->vertices[next][1], arg1, arg2) <= arg3
						|| cd00025774(block->vertices[i][0], block->vertices[i][1], block->vertices[next][0], block->vertices[next][1], arg1, arg2))) {
				if (*thingindex < maxthings) {
					things[*thingindex].geo = &block->header;
					things[*thingindex].vertexindex = i;
					things[*thingindex].prop = prop;
					*thingindex += 1;
				} else {
					break;
				}
			}
		}
	}
}

void cd0002885c(struct geocyl *cyl, f32 x, f32 z, f32 arg3, struct prop *prop, struct collisionthing *things, s32 maxthings, s32 *thingindex)
{
	f32 xdiff = x - cyl->x;
	f32 zdiff = z - cyl->z;
	f32 f16 = arg3 + cyl->radius;

	if (xdiff * xdiff + zdiff * zdiff <= f16 * f16) {
		if (*thingindex < maxthings) {
			things[*thingindex].geo = &cyl->header;
			things[*thingindex].vertexindex = 0;
			things[*thingindex].prop = prop;
			*thingindex += 1;
		}
	}
}

void cd00028914(u8 *start, u8 *end, struct coord *pos, f32 radius, u16 flags,
		bool checkvertical, f32 arg6, f32 arg7, struct prop *prop,
		struct collisionthing *things, s32 maxthings, s32 *thingindex)
{
	struct geo *geo = (struct geo *) start;

	while (geo < (struct geo *) end) {
		if (geo->type == GEOTYPE_TILE_I) {
			struct geotilei *tile = (struct geotilei *) geo;

			if (geo->flags & flags) {
				if (pos->x >= *(s16 *)(tile->xmin + (u32)tile) - radius
						&& pos->x <= *(s16 *)(tile->xmax + (u32)tile) + radius
						&& pos->z >= *(s16 *)(tile->zmin + (u32)tile) - radius
						&& pos->z <= *(s16 *)(tile->zmax + (u32)tile) + radius
						&& (!checkvertical || (pos->y + arg6 >= *(s16 *)(tile->ymin + (u32)tile)
								&& pos->y + arg7 <= *(s16 *)(tile->ymax + (u32)tile)))) {
					bool pass;

					if (geo->flags & GEOFLAG_0080) {
						pass = cd00028200(tile, pos, radius, pos->y + arg7, pos->y + arg6);
					} else {
						pass = true;
					}

					if (pass) {
						cd00027f78(tile, pos->x, pos->z, radius, prop, things, maxthings, thingindex);
					}
				}
			}

			geo = (struct geo *)((u32)geo + tile->header.numvertices * 6 + 0xe);
		} else if (geo->type == GEOTYPE_TILE_F) {
			struct geotilef *tile = (struct geotilef *) geo;

			if ((geo->flags & flags)
					&& pos->x >= tile->vertices[tile->xmin].x - radius
					&& pos->x <= tile->vertices[tile->xmax].x + radius
					&& pos->z >= tile->vertices[tile->zmin].z - radius
					&& pos->z <= tile->vertices[tile->zmax].z + radius
					&& (!checkvertical || (pos->y + arg6 >= tile->vertices[tile->ymin].y
							&& pos->y + arg7 <= tile->vertices[tile->ymax].y))) {
				cd0002840c(tile, pos->x, pos->z, radius, prop, things, maxthings, thingindex);
			}

			geo = (struct geo *)((u32)geo + (u32)(tile->header.numvertices - 0x40) * 0xc + 0x310);
		} else if (geo->type == GEOTYPE_BLOCK) {
			struct geoblock *block = (struct geoblock *) geo;

			if ((flags & (GEOFLAG_COLLISIONS | GEOFLAG_0008 | GEOFLAG_OPAQUE))
					&& (!checkvertical || (pos->y + arg6 >= block->ymin && pos->y + arg7 <= block->ymax))) {
				cd00028638(block, pos->x, pos->z, radius, prop, things, maxthings, thingindex);
			}

			geo = (struct geo *)((u32)geo + sizeof(struct geoblock));
		} else if (geo->type == GEOTYPE_CYL) {
			struct geocyl *cyl = (struct geocyl *) geo;

			if ((flags & geo->flags)
					&& (!checkvertical || (pos->y + arg6 >= cyl->ymin && pos->y + arg7 <= cyl->ymax))) {
				cd0002885c(cyl, pos->x, pos->z, radius, prop, things, maxthings, thingindex);
			}

			geo = (struct geo *)((u32)geo + sizeof(struct geocyl));
		}
	}
}

void cd00028df0(struct coord *pos, f32 width, s16 *rooms, u32 types, u16 arg4, u32 arg5, f32 ymax, f32 ymin, struct collisionthing *arg8, s32 arg9)
{
	s16 *roomptr;
	s32 roomnum;
	u8 *start;
	u8 *end;
	s32 sp294 = 0;
	s16 *propnumptr;
	s16 propnums[256];

	// Check BG
	if (types & CDTYPE_BG) {
		roomptr = rooms;
		roomnum = rooms[0];

		while (roomnum != -1) {
			if (roomnum < g_TileNumRooms) {
				start = g_TileFileData.u8 + g_TileRooms[roomnum];
				end = g_TileFileData.u8 + g_TileRooms[roomnum + 1];

				cd00028914(start, end, pos, width, arg4, arg5, ymax, ymin, NULL, arg8, arg9, &sp294);
			}

			roomptr++;
			roomnum = *roomptr;
		}
	}

	// Check props
	roomGetProps(rooms, propnums, 256);
	propnumptr = propnums;

	while (*propnumptr >= 0) {
		struct prop *prop = &g_Vars.props[*propnumptr];

		if (propIsOfCdType(prop, types) && propUpdateGeometry(prop, &start, &end)) {
			cd00028914(start, end, pos, width, arg4, arg5, ymax, ymin, prop, arg8, arg9, &sp294);
		}

		propnumptr++;
	}

	arg8[sp294].geo = NULL;
}

void cd0002901c(struct coord *pos, struct coord *dist, f32 width, struct collisionthing *things)
{
	s32 i;
	struct widthxz spf8;
	struct xz spf0;
	struct xz spe8;
	struct xz spe0;
	f32 bestvalue = 0.0f;
	s32 bestindex = -1;
	struct collisionthing *bestthing;
	f32 value;
	s32 curr;
	s32 next;
	struct coord vtx1;
	struct coord vtx2;
	struct geo *geo;

	for (i = 0; (geo = things[i].geo) != NULL; i++) {
		if (1);
		if (geo->type == GEOTYPE_TILE_I) {
			struct geotilei *tile = (struct geotilei *) geo;

			spf8.width = width;
			spf8.x = pos->x;
			spf8.z = pos->z;

			spe0.x = dist->x;
			spe0.z = dist->z;

			curr = things[i].vertexindex;
			next = (curr + 1) % tile->header.numvertices;

			spf0.x = tile->vertices[curr][0];
			spf0.z = tile->vertices[curr][2];

			spe8.x = tile->vertices[next][0];
			spe8.z = tile->vertices[next][2];

			value = func0f1579cc(&spf8, &spf0, &spe8, &spe0);

			if (bestindex < 0 || value < bestvalue) {
				bestvalue = value;
				bestindex = i;
			}
		} else if (geo->type == GEOTYPE_TILE_F) {
			struct geotilef *tile = (struct geotilef *) geo;

			spf8.width = width;
			spf8.x = pos->x;
			spf8.z = pos->z;

			spe0.x = dist->x;
			spe0.z = dist->z;

			curr = things[i].vertexindex;
			next = (curr + 1) % tile->header.numvertices;

			spf0.x = tile->vertices[curr].x;
			spf0.z = tile->vertices[curr].z;

			spe8.x = tile->vertices[next].x;
			spe8.z = tile->vertices[next].z;

			value = func0f1579cc(&spf8, &spf0, &spe8, &spe0);

			if (bestindex < 0 || value < bestvalue) {
				bestvalue = value;
				bestindex = i;
			}
		} else if (geo->type == GEOTYPE_BLOCK) {
			struct geoblock *block = (struct geoblock *) geo;

			spf8.width = width;
			spf8.x = pos->x;
			spf8.z = pos->z;

			spe0.x = dist->x;
			spe0.z = dist->z;

			curr = things[i].vertexindex;
			next = (curr + 1) % block->header.numvertices;

			spf0.x = block->vertices[curr][0];
			spf0.z = block->vertices[curr][1];

			spe8.x = block->vertices[next][0];
			spe8.z = block->vertices[next][1];

			value = func0f1579cc(&spf8, &spf0, &spe8, &spe0);

			if (bestindex < 0 || value < bestvalue) {
				bestvalue = value;
				bestindex = i;
			}
		} else if (geo->type == GEOTYPE_CYL) {
			struct geocyl *cyl = (struct geocyl *) geo;

			spf8.width = cyl->radius + width;
			spf8.x = pos->x;
			spf8.z = pos->z;

			spe0.x = dist->x;
			spe0.z = dist->z;

			spf0.x = cyl->x;
			spf0.z = cyl->z;

			spe8.x = cyl->x;
			spe8.z = cyl->z;

			value = func0f1579cc(&spf8, &spf0, &spe8, &spe0);

			if (bestindex < 0 || value < bestvalue) {
				bestvalue = value;
				bestindex = i;
			}
		}
	}

	if (things[bestindex].geo->type == GEOTYPE_TILE_I) {
		struct geotilei *tile = (struct geotilei *) things[bestindex].geo;
		s32 curr = things[bestindex].vertexindex;
		s32 next = (curr + 1) % tile->header.numvertices;

		vtx1.x = tile->vertices[curr][0];
		vtx1.y = tile->vertices[curr][1];
		vtx1.z = tile->vertices[curr][2];

		vtx2.x = tile->vertices[next][0];
		vtx2.y = tile->vertices[next][1];
		vtx2.z = tile->vertices[next][2];
	} else if (things[bestindex].geo->type == GEOTYPE_TILE_F) {
		struct geotilef *tile = (struct geotilef *) things[bestindex].geo;
		s32 curr = things[bestindex].vertexindex;
		s32 next = (curr + 1) % tile->header.numvertices;

		vtx1.x = tile->vertices[curr].x;
		vtx1.y = tile->vertices[curr].y;
		vtx1.z = tile->vertices[curr].z;

		vtx2.x = tile->vertices[next].x;
		vtx2.y = tile->vertices[next].y;
		vtx2.z = tile->vertices[next].z;
	} else if (things[bestindex].geo->type == GEOTYPE_BLOCK) {
		struct geoblock *block = (struct geoblock *) things[bestindex].geo;
		s32 curr = things[bestindex].vertexindex;
		s32 next = (curr + 1) % block->header.numvertices;

		vtx1.x = block->vertices[curr][0];
		vtx1.y = pos->y;
		vtx1.z = block->vertices[curr][1];

		vtx2.x = block->vertices[next][0];
		vtx2.y = pos->y;
		vtx2.z = block->vertices[next][1];
	} else if (things[bestindex].geo->type == GEOTYPE_CYL) {
		struct geocyl *cyl = (struct geocyl *) things[bestindex].geo;

		cd00025848(cyl->x, cyl->z, cyl->radius, pos->x, pos->z, &vtx1.x, &vtx1.z, &vtx2.x, &vtx2.z);

		vtx1.y = pos->y;
		vtx2.y = pos->y;
	}

	cd00025038(&vtx1, &vtx2, things[bestindex].prop, bestvalue);
}

#if VERSION >= VERSION_NTSC_1_0
// NTSC beta mismatches due to swapped s5/s6 registers
// thing should be s6; next should be s5
f32 cd000296a0(struct collisionthing *things, struct coord *pos, struct collisionthing **thingptr, f32 width)
{
	struct collisionthing *thing;
	s32 i;
	f32 curground = -4294967296;
	bool hasground = false;
	bool anyintile = false;
	bool hasflag0100 = false;
#if VERSION >= VERSION_NTSC_1_0
	bool hasdie = false;
	bool hasgroundfromearlier;
	bool isdie;
#else
	u32 unused1;
#endif
	f32 nextvalue;
	f32 spe4;
	f32 f30;
	f32 x;
	f32 z;
	f32 spd4;
	f32 f14;
	f32 spb4;
	f32 thisvalue;
	u32 unused2;
	s32 next;
	s32 numvertices;
	f32 spb8;
#if VERSION >= VERSION_NTSC_1_0
	u32 unused3[8];
#else
	u32 unused3[7];
#endif
	f32 sp94;
	u32 unused4[6];
	f32 sp78;
	f32 sp74;
	f32 nextx;
	f32 nextz;
	f32 ground;
	u32 unused5[5];
	f32 thisx;
	f32 thisz;

	*thingptr = NULL;

	for (thing = things; thing->geo != NULL; thing++) {
		if (thing->geo->type == GEOTYPE_TILE_I) {
			struct geotilei *tile = (struct geotilei *) thing->geo;

			if (tile->header.flags & GEOFLAG_DIE) {
				thing->intile = false;
			} else {
				if (tile->header.flags & GEOFLAG_0100) {
					hasflag0100 = true;
				}

				thing->intile = cdIs2dPointInIntTile(tile, pos->x, pos->z);

				if (thing->intile) {
					anyintile = true;
				}
			}
		} else if (thing->geo->type == GEOTYPE_TILE_F) {
			struct geotilef *tile = (struct geotilef *) thing->geo;

			thing->intile = cdIs2dPointInFltTile(tile, pos->x, pos->z);

			if (thing->intile) {
				anyintile = true;
			}
		}
	}

	if (anyintile) {
		for (thing = things; thing->geo != NULL; thing++) {
			if (thing->intile) {
				if (thing->geo->type == GEOTYPE_TILE_I) {
					struct geotilei *tile = (struct geotilei *) thing->geo;

					if ((tile->header.flags & GEOFLAG_STEP) == 0) {
						ground = cdFindGroundInIntTile((void *)thing->geo, pos->x, pos->z);

						if (ground >= curground && ground < pos->y) {
							curground = ground;
							*thingptr = thing;
							hasground = true;
						}
					}
				} else if (thing->geo->type == GEOTYPE_TILE_F) {
					struct geotilef *tile = (struct geotilef *) thing->geo;

					ground = cdFindGroundInFltTile((void *)thing->geo, pos->x, pos->z);

					if (ground >= curground && ground < pos->y) {
						curground = ground;
						*thingptr = thing;
						hasground = true;
					}
				}
			}
		}

		for (thing = things; thing->geo != NULL; thing++) {
			if (thing->intile) {
				if (thing->geo->type == GEOTYPE_TILE_I) {
					struct geotilei *tile = (struct geotilei *) thing->geo;

					if (tile->header.flags & GEOFLAG_STEP) {
						ground = cdFindGroundInIntTile((void *)thing->geo, pos->x, pos->z);

						if (ground >= curground && (ground < pos->y || !hasground)) {
							curground = ground;
							*thingptr = thing;
							hasground = true;
						}
					}
				}
			}
		}
	}

#if VERSION >= VERSION_NTSC_1_0
	hasgroundfromearlier = hasground;
#endif

	if (!hasground || hasflag0100) {
		spe4 = 4294967296.0f;

		for (thing = things; thing->geo != NULL; thing++) {
#if VERSION >= VERSION_NTSC_1_0
			if (thing->intile == false
					&& (!hasgroundfromearlier || (thing->geo->type == GEOTYPE_TILE_I && (thing->geo->flags & GEOFLAG_0100))))
#else
			if (thing->intile == false
					&& (!hasground || (thing->geo->type == GEOTYPE_TILE_I && (thing->geo->flags & GEOFLAG_0100))))
#endif
			{
				if (thing->geo->type == GEOTYPE_TILE_I) {
					struct geotilei *tile = (struct geotilei *) thing->geo;
					numvertices = tile->header.numvertices;
#if VERSION >= VERSION_NTSC_1_0
					isdie = (tile->header.flags & GEOFLAG_DIE) != 0;
#endif

#if VERSION >= VERSION_NTSC_1_0
					if (!isdie || !hasground)
#endif
					{
						for (i = 0; i < numvertices; i++) {
							thisx = tile->vertices[i][0];
							thisz = tile->vertices[i][2];

							next = (i + 1) % numvertices;

							nextx = tile->vertices[next][0];
							nextz = tile->vertices[next][2];

							spd4 = cd00025654(thisx, thisz, nextx, nextz, pos->x, pos->z);
							f30 = spd4;

							if (f30 < 0.0f) {
								f30 = -f30;
							}

#if VERSION >= VERSION_NTSC_1_0
							if (f30 < spe4 || hasdie)
#else
							if (f30 < spe4)
#endif
							{
								if (cd00025774(thisx, thisz, nextx, nextz, pos->x, pos->z)) {
									spb8 = nextx - thisx;
									spb4 = nextz - thisz;
									f14 = spd4 / sqrtf(spb8 * spb8 + spb4 * spb4);
									x = pos->x + f14 * -spb4;
									z = pos->z + f14 * spb8;

									ground = cdFindGroundInIntTileAtVertex(tile, x, z, i);

									if (ground < pos->y || (thing->geo->flags & GEOFLAG_STEP)) {
										curground = ground;
										*thingptr = thing;
										spe4 = f30;
#if VERSION >= VERSION_NTSC_1_0
										hasground = true;
										hasdie = isdie;
#endif
									}
								} else {
									thisvalue = cd00025724(thisx, thisz, pos->x, pos->z);
									nextvalue = cd00025724(nextx, nextz, pos->x, pos->z);

									if (thisvalue < nextvalue) {
#if VERSION >= VERSION_NTSC_1_0
										if (thisvalue < spe4 || hasdie)
#else
										if (thisvalue < spe4)
#endif
										{
											x = tile->vertices[i][0];
											z = tile->vertices[i][2];
											ground = cdFindGroundInIntTileAtVertex(tile, x, z, i);

											if (ground < pos->y || (thing->geo->flags & GEOFLAG_STEP)) {
												curground = ground;
												*thingptr = thing;
												spe4 = thisvalue;
#if VERSION >= VERSION_NTSC_1_0
												hasground = true;
												hasdie = isdie;
#endif
											}
										}
									} else {
#if VERSION >= VERSION_NTSC_1_0
										if (nextvalue < spe4 || hasdie)
#else
										if (nextvalue < spe4)
#endif
										{
											x = tile->vertices[next][0];
											z = tile->vertices[next][2];
											ground = cdFindGroundInIntTileAtVertex(tile, x, z, i);

											if (ground < pos->y || (thing->geo->flags & GEOFLAG_STEP)) {
												curground = ground;
												*thingptr = thing;
												spe4 = nextvalue;
#if VERSION >= VERSION_NTSC_1_0
												hasground = true;
												hasdie = isdie;
#endif
											}
										}
									}
								}
							}
						}
					}
				} else if (thing->geo->type == GEOTYPE_TILE_F) {
					struct geotilef *tile = (struct geotilef *) thing->geo;
					s32 numvertices = tile->header.numvertices;
					s32 i;

					for (i = 0; i < numvertices; i++) {
						thisx = tile->vertices[i].x;
						thisz = tile->vertices[i].z;

						next = (i + 1) % numvertices;

						nextx = tile->vertices[next].x;
						nextz = tile->vertices[next].z;

						sp94 = cd00025654(thisx, thisz, nextx, nextz, pos->x, pos->z);
						f30 = sp94;

						if (f30 < 0.0f) {
							f30 = -f30;
						}

						if (f30 < spe4) {
							if (cd00025774(thisx, thisz, nextx, nextz, pos->x, pos->z)) {
								sp78 = nextx - thisx;
								sp74 = nextz - thisz;
								f14 = sp94 / sqrtf(sp78 * sp78 + sp74 * sp74);
								x = pos->x + f14 * -sp74;
								z = pos->z + f14 * sp78;

								ground = cdFindGroundInFltTile(tile, x, z);

								if (ground < pos->y) {
									curground = ground;
									*thingptr = thing;
									spe4 = f30;
#if VERSION >= VERSION_NTSC_1_0
									hasground = true;
									hasdie = false;
#endif
								}
							} else {
								thisvalue = cd00025724(thisx, thisz, pos->x, pos->z);
								nextvalue = cd00025724(nextx, nextz, pos->x, pos->z);

								if (thisvalue < nextvalue) {
									if (thisvalue < spe4) {
										x = tile->vertices[i].x;
										z = tile->vertices[i].z;
										ground = cdFindGroundInFltTile(tile, x, z);

										if (ground < pos->y) {
											curground = ground;
											*thingptr = thing;
											spe4 = thisvalue;
#if VERSION >= VERSION_NTSC_1_0
											hasground = true;
											hasdie = false;
#endif
										}
									}
								} else {
									if (nextvalue < spe4) {
										x = tile->vertices[next].x;
										z = tile->vertices[next].z;
										ground = cdFindGroundInFltTile(tile, x, z);

										if (ground < pos->y) {
											curground = ground;
											*thingptr = thing;
											spe4 = nextvalue;
#if VERSION >= VERSION_NTSC_1_0
											hasground = true;
											hasdie = false;
#endif
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return curground;
}
#else
GLOBAL_ASM(
glabel cd000296a0
/*    2ab9c:	27bdfef8 */ 	addiu	$sp,$sp,-264
/*    2aba0:	3c01cf80 */ 	lui	$at,0xcf80
/*    2aba4:	44812000 */ 	mtc1	$at,$f4
/*    2aba8:	afbf0074 */ 	sw	$ra,0x74($sp)
/*    2abac:	afbe0070 */ 	sw	$s8,0x70($sp)
/*    2abb0:	afb7006c */ 	sw	$s7,0x6c($sp)
/*    2abb4:	afb60068 */ 	sw	$s6,0x68($sp)
/*    2abb8:	afb50064 */ 	sw	$s5,0x64($sp)
/*    2abbc:	afb40060 */ 	sw	$s4,0x60($sp)
/*    2abc0:	afb3005c */ 	sw	$s3,0x5c($sp)
/*    2abc4:	afb20058 */ 	sw	$s2,0x58($sp)
/*    2abc8:	afb10054 */ 	sw	$s1,0x54($sp)
/*    2abcc:	afb00050 */ 	sw	$s0,0x50($sp)
/*    2abd0:	f7be0048 */ 	sdc1	$f30,0x48($sp)
/*    2abd4:	f7bc0040 */ 	sdc1	$f28,0x40($sp)
/*    2abd8:	f7ba0038 */ 	sdc1	$f26,0x38($sp)
/*    2abdc:	f7b80030 */ 	sdc1	$f24,0x30($sp)
/*    2abe0:	f7b60028 */ 	sdc1	$f22,0x28($sp)
/*    2abe4:	f7b40020 */ 	sdc1	$f20,0x20($sp)
/*    2abe8:	afa70114 */ 	sw	$a3,0x114($sp)
/*    2abec:	afa000f8 */ 	sw	$zero,0xf8($sp)
/*    2abf0:	e7a400fc */ 	swc1	$f4,0xfc($sp)
/*    2abf4:	acc00000 */ 	sw	$zero,0x0($a2)
/*    2abf8:	8c8e0000 */ 	lw	$t6,0x0($a0)
/*    2abfc:	00a09025 */ 	or	$s2,$a1,$zero
/*    2ac00:	00809825 */ 	or	$s3,$a0,$zero
/*    2ac04:	00c0f025 */ 	or	$s8,$a2,$zero
/*    2ac08:	00008025 */ 	or	$s0,$zero,$zero
/*    2ac0c:	00008825 */ 	or	$s1,$zero,$zero
/*    2ac10:	11c00025 */ 	beqz	$t6,.NB0002aca8
/*    2ac14:	0080b025 */ 	or	$s6,$a0,$zero
/*    2ac18:	8ec70000 */ 	lw	$a3,0x0($s6)
/*    2ac1c:	90e20000 */ 	lbu	$v0,0x0($a3)
.NB0002ac20:
/*    2ac20:	24010001 */ 	addiu	$at,$zero,0x1
/*    2ac24:	14400013 */ 	bnez	$v0,.NB0002ac74
/*    2ac28:	00000000 */ 	sll	$zero,$zero,0x0
/*    2ac2c:	94e20002 */ 	lhu	$v0,0x2($a3)
/*    2ac30:	00e02025 */ 	or	$a0,$a3,$zero
/*    2ac34:	304f4000 */ 	andi	$t7,$v0,0x4000
/*    2ac38:	51e00004 */ 	beqzl	$t7,.NB0002ac4c
/*    2ac3c:	30580100 */ 	andi	$t8,$v0,0x100
/*    2ac40:	10000015 */ 	beqz	$zero,.NB0002ac98
/*    2ac44:	aec00004 */ 	sw	$zero,0x4($s6)
/*    2ac48:	30580100 */ 	andi	$t8,$v0,0x100
.NB0002ac4c:
/*    2ac4c:	53000003 */ 	beqzl	$t8,.NB0002ac5c
/*    2ac50:	8e450000 */ 	lw	$a1,0x0($s2)
/*    2ac54:	24110001 */ 	addiu	$s1,$zero,0x1
/*    2ac58:	8e450000 */ 	lw	$a1,0x0($s2)
.NB0002ac5c:
/*    2ac5c:	0c009da6 */ 	jal	cdIs2dPointInIntTile
/*    2ac60:	8e460008 */ 	lw	$a2,0x8($s2)
/*    2ac64:	1040000c */ 	beqz	$v0,.NB0002ac98
/*    2ac68:	aec20004 */ 	sw	$v0,0x4($s6)
/*    2ac6c:	1000000a */ 	beqz	$zero,.NB0002ac98
/*    2ac70:	24100001 */ 	addiu	$s0,$zero,0x1
.NB0002ac74:
/*    2ac74:	54410009 */ 	bnel	$v0,$at,.NB0002ac9c
/*    2ac78:	8ec70014 */ 	lw	$a3,0x14($s6)
/*    2ac7c:	00e02025 */ 	or	$a0,$a3,$zero
/*    2ac80:	8e450000 */ 	lw	$a1,0x0($s2)
/*    2ac84:	0c009dfd */ 	jal	cdIs2dPointInFltTile
/*    2ac88:	8e460008 */ 	lw	$a2,0x8($s2)
/*    2ac8c:	10400002 */ 	beqz	$v0,.NB0002ac98
/*    2ac90:	aec20004 */ 	sw	$v0,0x4($s6)
/*    2ac94:	24100001 */ 	addiu	$s0,$zero,0x1
.NB0002ac98:
/*    2ac98:	8ec70014 */ 	lw	$a3,0x14($s6)
.NB0002ac9c:
/*    2ac9c:	26d60014 */ 	addiu	$s6,$s6,0x14
/*    2aca0:	54e0ffdf */ 	bnezl	$a3,.NB0002ac20
/*    2aca4:	90e20000 */ 	lbu	$v0,0x0($a3)
.NB0002aca8:
/*    2aca8:	52000065 */ 	beqzl	$s0,.NB0002ae40
/*    2acac:	8faa00f8 */ 	lw	$t2,0xf8($sp)
/*    2acb0:	8e620000 */ 	lw	$v0,0x0($s3)
/*    2acb4:	0260b025 */ 	or	$s6,$s3,$zero
/*    2acb8:	1040003a */ 	beqz	$v0,.NB0002ada4
/*    2acbc:	00000000 */ 	sll	$zero,$zero,0x0
/*    2acc0:	8ed90004 */ 	lw	$t9,0x4($s6)
.NB0002acc4:
/*    2acc4:	53200032 */ 	beqzl	$t9,.NB0002ad90
/*    2acc8:	8ecc0014 */ 	lw	$t4,0x14($s6)
/*    2accc:	8ec70000 */ 	lw	$a3,0x0($s6)
/*    2acd0:	24010001 */ 	addiu	$at,$zero,0x1
/*    2acd4:	90e20000 */ 	lbu	$v0,0x0($a3)
/*    2acd8:	14400018 */ 	bnez	$v0,.NB0002ad3c
/*    2acdc:	00000000 */ 	sll	$zero,$zero,0x0
/*    2ace0:	94e80002 */ 	lhu	$t0,0x2($a3)
/*    2ace4:	31092000 */ 	andi	$t1,$t0,0x2000
/*    2ace8:	55200029 */ 	bnezl	$t1,.NB0002ad90
/*    2acec:	8ecc0014 */ 	lw	$t4,0x14($s6)
/*    2acf0:	00e02025 */ 	or	$a0,$a3,$zero
/*    2acf4:	8e450000 */ 	lw	$a1,0x0($s2)
/*    2acf8:	0c009ce8 */ 	jal	cdFindGroundInIntTile
/*    2acfc:	8e460008 */ 	lw	$a2,0x8($s2)
/*    2ad00:	c7a600fc */ 	lwc1	$f6,0xfc($sp)
/*    2ad04:	4600303e */ 	c.le.s	$f6,$f0
/*    2ad08:	00000000 */ 	sll	$zero,$zero,0x0
/*    2ad0c:	45020020 */ 	bc1fl	.NB0002ad90
/*    2ad10:	8ecc0014 */ 	lw	$t4,0x14($s6)
/*    2ad14:	c6480004 */ 	lwc1	$f8,0x4($s2)
/*    2ad18:	240a0001 */ 	addiu	$t2,$zero,0x1
/*    2ad1c:	4608003c */ 	c.lt.s	$f0,$f8
/*    2ad20:	00000000 */ 	sll	$zero,$zero,0x0
/*    2ad24:	4502001a */ 	bc1fl	.NB0002ad90
/*    2ad28:	8ecc0014 */ 	lw	$t4,0x14($s6)
/*    2ad2c:	e7a000fc */ 	swc1	$f0,0xfc($sp)
/*    2ad30:	afd60000 */ 	sw	$s6,0x0($s8)
/*    2ad34:	10000015 */ 	beqz	$zero,.NB0002ad8c
/*    2ad38:	afaa00f8 */ 	sw	$t2,0xf8($sp)
.NB0002ad3c:
/*    2ad3c:	54410014 */ 	bnel	$v0,$at,.NB0002ad90
/*    2ad40:	8ecc0014 */ 	lw	$t4,0x14($s6)
/*    2ad44:	00e02025 */ 	or	$a0,$a3,$zero
/*    2ad48:	8e450000 */ 	lw	$a1,0x0($s2)
/*    2ad4c:	0c009d37 */ 	jal	cdFindGroundInFltTile
/*    2ad50:	8e460008 */ 	lw	$a2,0x8($s2)
/*    2ad54:	c7aa00fc */ 	lwc1	$f10,0xfc($sp)
/*    2ad58:	4600503e */ 	c.le.s	$f10,$f0
/*    2ad5c:	00000000 */ 	sll	$zero,$zero,0x0
/*    2ad60:	4502000b */ 	bc1fl	.NB0002ad90
/*    2ad64:	8ecc0014 */ 	lw	$t4,0x14($s6)
/*    2ad68:	c6500004 */ 	lwc1	$f16,0x4($s2)
/*    2ad6c:	240b0001 */ 	addiu	$t3,$zero,0x1
/*    2ad70:	4610003c */ 	c.lt.s	$f0,$f16
/*    2ad74:	00000000 */ 	sll	$zero,$zero,0x0
/*    2ad78:	45020005 */ 	bc1fl	.NB0002ad90
/*    2ad7c:	8ecc0014 */ 	lw	$t4,0x14($s6)
/*    2ad80:	e7a000fc */ 	swc1	$f0,0xfc($sp)
/*    2ad84:	afd60000 */ 	sw	$s6,0x0($s8)
/*    2ad88:	afab00f8 */ 	sw	$t3,0xf8($sp)
.NB0002ad8c:
/*    2ad8c:	8ecc0014 */ 	lw	$t4,0x14($s6)
.NB0002ad90:
/*    2ad90:	26d60014 */ 	addiu	$s6,$s6,0x14
/*    2ad94:	5580ffcb */ 	bnezl	$t4,.NB0002acc4
/*    2ad98:	8ed90004 */ 	lw	$t9,0x4($s6)
/*    2ad9c:	0260b025 */ 	or	$s6,$s3,$zero
/*    2ada0:	8e620000 */ 	lw	$v0,0x0($s3)
.NB0002ada4:
/*    2ada4:	50400026 */ 	beqzl	$v0,.NB0002ae40
/*    2ada8:	8faa00f8 */ 	lw	$t2,0xf8($sp)
/*    2adac:	8ecd0004 */ 	lw	$t5,0x4($s6)
.NB0002adb0:
/*    2adb0:	51a0001f */ 	beqzl	$t5,.NB0002ae30
/*    2adb4:	8ec90014 */ 	lw	$t1,0x14($s6)
/*    2adb8:	8ec70000 */ 	lw	$a3,0x0($s6)
/*    2adbc:	90ee0000 */ 	lbu	$t6,0x0($a3)
/*    2adc0:	55c0001b */ 	bnezl	$t6,.NB0002ae30
/*    2adc4:	8ec90014 */ 	lw	$t1,0x14($s6)
/*    2adc8:	94ef0002 */ 	lhu	$t7,0x2($a3)
/*    2adcc:	31f82000 */ 	andi	$t8,$t7,0x2000
/*    2add0:	53000017 */ 	beqzl	$t8,.NB0002ae30
/*    2add4:	8ec90014 */ 	lw	$t1,0x14($s6)
/*    2add8:	00e02025 */ 	or	$a0,$a3,$zero
/*    2addc:	8e450000 */ 	lw	$a1,0x0($s2)
/*    2ade0:	0c009ce8 */ 	jal	cdFindGroundInIntTile
/*    2ade4:	8e460008 */ 	lw	$a2,0x8($s2)
/*    2ade8:	c7b200fc */ 	lwc1	$f18,0xfc($sp)
/*    2adec:	4600903e */ 	c.le.s	$f18,$f0
/*    2adf0:	00000000 */ 	sll	$zero,$zero,0x0
/*    2adf4:	4502000e */ 	bc1fl	.NB0002ae30
/*    2adf8:	8ec90014 */ 	lw	$t1,0x14($s6)
/*    2adfc:	c6440004 */ 	lwc1	$f4,0x4($s2)
/*    2ae00:	4604003c */ 	c.lt.s	$f0,$f4
/*    2ae04:	00000000 */ 	sll	$zero,$zero,0x0
/*    2ae08:	45030005 */ 	bc1tl	.NB0002ae20
/*    2ae0c:	e7a000fc */ 	swc1	$f0,0xfc($sp)
/*    2ae10:	8fb900f8 */ 	lw	$t9,0xf8($sp)
/*    2ae14:	57200006 */ 	bnezl	$t9,.NB0002ae30
/*    2ae18:	8ec90014 */ 	lw	$t1,0x14($s6)
/*    2ae1c:	e7a000fc */ 	swc1	$f0,0xfc($sp)
.NB0002ae20:
/*    2ae20:	afd60000 */ 	sw	$s6,0x0($s8)
/*    2ae24:	24080001 */ 	addiu	$t0,$zero,0x1
/*    2ae28:	afa800f8 */ 	sw	$t0,0xf8($sp)
/*    2ae2c:	8ec90014 */ 	lw	$t1,0x14($s6)
.NB0002ae30:
/*    2ae30:	26d60014 */ 	addiu	$s6,$s6,0x14
/*    2ae34:	5520ffde */ 	bnezl	$t1,.NB0002adb0
/*    2ae38:	8ecd0004 */ 	lw	$t5,0x4($s6)
/*    2ae3c:	8faa00f8 */ 	lw	$t2,0xf8($sp)
.NB0002ae40:
/*    2ae40:	3c014f80 */ 	lui	$at,0x4f80
/*    2ae44:	51400004 */ 	beqzl	$t2,.NB0002ae58
/*    2ae48:	44813000 */ 	mtc1	$at,$f6
/*    2ae4c:	5220016b */ 	beqzl	$s1,.NB0002b3fc
/*    2ae50:	8fbf0074 */ 	lw	$ra,0x74($sp)
/*    2ae54:	44813000 */ 	mtc1	$at,$f6
.NB0002ae58:
/*    2ae58:	0260b025 */ 	or	$s6,$s3,$zero
/*    2ae5c:	e7a600e4 */ 	swc1	$f6,0xe4($sp)
/*    2ae60:	8e6b0000 */ 	lw	$t3,0x0($s3)
/*    2ae64:	51600165 */ 	beqzl	$t3,.NB0002b3fc
/*    2ae68:	8fbf0074 */ 	lw	$ra,0x74($sp)
/*    2ae6c:	8ecc0004 */ 	lw	$t4,0x4($s6)
.NB0002ae70:
/*    2ae70:	8fad00f8 */ 	lw	$t5,0xf8($sp)
/*    2ae74:	5580015d */ 	bnezl	$t4,.NB0002b3ec
/*    2ae78:	8ec80014 */ 	lw	$t0,0x14($s6)
/*    2ae7c:	11a00008 */ 	beqz	$t5,.NB0002aea0
/*    2ae80:	8ec70000 */ 	lw	$a3,0x0($s6)
/*    2ae84:	90ee0000 */ 	lbu	$t6,0x0($a3)
/*    2ae88:	55c00158 */ 	bnezl	$t6,.NB0002b3ec
/*    2ae8c:	8ec80014 */ 	lw	$t0,0x14($s6)
/*    2ae90:	94ef0002 */ 	lhu	$t7,0x2($a3)
/*    2ae94:	31f80100 */ 	andi	$t8,$t7,0x100
/*    2ae98:	53000154 */ 	beqzl	$t8,.NB0002b3ec
/*    2ae9c:	8ec80014 */ 	lw	$t0,0x14($s6)
.NB0002aea0:
/*    2aea0:	90e20000 */ 	lbu	$v0,0x0($a3)
/*    2aea4:	544000ba */ 	bnezl	$v0,.NB0002b190
/*    2aea8:	24010001 */ 	addiu	$at,$zero,0x1
/*    2aeac:	90f70001 */ 	lbu	$s7,0x1($a3)
/*    2aeb0:	00e0a025 */ 	or	$s4,$a3,$zero
/*    2aeb4:	00008825 */ 	or	$s1,$zero,$zero
/*    2aeb8:	1ae0014b */ 	blez	$s7,.NB0002b3e8
/*    2aebc:	00e09825 */ 	or	$s3,$a3,$zero
.NB0002aec0:
/*    2aec0:	26350001 */ 	addiu	$s5,$s1,0x1
/*    2aec4:	02b7001a */ 	div	$zero,$s5,$s7
/*    2aec8:	00004810 */ 	mfhi	$t1
/*    2aecc:	00095080 */ 	sll	$t2,$t1,0x2
/*    2aed0:	01495023 */ 	subu	$t2,$t2,$t1
/*    2aed4:	000a5040 */ 	sll	$t2,$t2,0x1
/*    2aed8:	028a8021 */ 	addu	$s0,$s4,$t2
/*    2aedc:	860b000e */ 	lh	$t3,0xe($s0)
/*    2aee0:	860c0012 */ 	lh	$t4,0x12($s0)
/*    2aee4:	8679000e */ 	lh	$t9,0xe($s3)
/*    2aee8:	86680012 */ 	lh	$t0,0x12($s3)
/*    2aeec:	448b8000 */ 	mtc1	$t3,$f16
/*    2aef0:	448c9000 */ 	mtc1	$t4,$f18
/*    2aef4:	44994000 */ 	mtc1	$t9,$f8
/*    2aef8:	44885000 */ 	mtc1	$t0,$f10
/*    2aefc:	468086a0 */ 	cvt.s.w	$f26,$f16
/*    2af00:	c6440000 */ 	lwc1	$f4,0x0($s2)
/*    2af04:	e7a40010 */ 	swc1	$f4,0x10($sp)
/*    2af08:	46809720 */ 	cvt.s.w	$f28,$f18
/*    2af0c:	c6460008 */ 	lwc1	$f6,0x8($s2)
/*    2af10:	4406d000 */ 	mfc1	$a2,$f26
/*    2af14:	e7a60014 */ 	swc1	$f6,0x14($sp)
/*    2af18:	468045a0 */ 	cvt.s.w	$f22,$f8
/*    2af1c:	4407e000 */ 	mfc1	$a3,$f28
/*    2af20:	46805620 */ 	cvt.s.w	$f24,$f10
/*    2af24:	16e00002 */ 	bnez	$s7,.NB0002af30
/*    2af28:	00000000 */ 	sll	$zero,$zero,0x0
/*    2af2c:	0007000d */ 	break	0x7
.NB0002af30:
/*    2af30:	2401ffff */ 	addiu	$at,$zero,-1
/*    2af34:	16e10004 */ 	bne	$s7,$at,.NB0002af48
/*    2af38:	3c018000 */ 	lui	$at,0x8000
/*    2af3c:	16a10002 */ 	bne	$s5,$at,.NB0002af48
/*    2af40:	00000000 */ 	sll	$zero,$zero,0x0
/*    2af44:	0006000d */ 	break	0x6
.NB0002af48:
/*    2af48:	4600c386 */ 	mov.s	$f14,$f24
/*    2af4c:	0c009a8d */ 	jal	cd00025654
/*    2af50:	4600b306 */ 	mov.s	$f12,$f22
/*    2af54:	44804000 */ 	mtc1	$zero,$f8
/*    2af58:	e7a000d4 */ 	swc1	$f0,0xd4($sp)
/*    2af5c:	46000786 */ 	mov.s	$f30,$f0
/*    2af60:	4608003c */ 	c.lt.s	$f0,$f8
/*    2af64:	c7aa00e4 */ 	lwc1	$f10,0xe4($sp)
/*    2af68:	45020003 */ 	bc1fl	.NB0002af78
/*    2af6c:	460af03c */ 	c.lt.s	$f30,$f10
/*    2af70:	46000787 */ 	neg.s	$f30,$f0
/*    2af74:	460af03c */ 	c.lt.s	$f30,$f10
.NB0002af78:
/*    2af78:	00000000 */ 	sll	$zero,$zero,0x0
/*    2af7c:	4502007f */ 	bc1fl	.NB0002b17c
/*    2af80:	02a08825 */ 	or	$s1,$s5,$zero
/*    2af84:	c6500000 */ 	lwc1	$f16,0x0($s2)
/*    2af88:	4406d000 */ 	mfc1	$a2,$f26
/*    2af8c:	4407e000 */ 	mfc1	$a3,$f28
/*    2af90:	e7b00010 */ 	swc1	$f16,0x10($sp)
/*    2af94:	c6520008 */ 	lwc1	$f18,0x8($s2)
/*    2af98:	4600b306 */ 	mov.s	$f12,$f22
/*    2af9c:	4600c386 */ 	mov.s	$f14,$f24
/*    2afa0:	0c009ad5 */ 	jal	cd00025774
/*    2afa4:	e7b20014 */ 	swc1	$f18,0x14($sp)
/*    2afa8:	50400028 */ 	beqzl	$v0,.NB0002b04c
/*    2afac:	4600b306 */ 	mov.s	$f12,$f22
/*    2afb0:	4616d001 */ 	sub.s	$f0,$f26,$f22
/*    2afb4:	4618e501 */ 	sub.s	$f20,$f28,$f24
/*    2afb8:	46000102 */ 	mul.s	$f4,$f0,$f0
/*    2afbc:	e7a000b8 */ 	swc1	$f0,0xb8($sp)
/*    2afc0:	4614a182 */ 	mul.s	$f6,$f20,$f20
/*    2afc4:	0c012e84 */ 	jal	sqrtf
/*    2afc8:	46062300 */ 	add.s	$f12,$f4,$f6
/*    2afcc:	c7a800d4 */ 	lwc1	$f8,0xd4($sp)
/*    2afd0:	4600a407 */ 	neg.s	$f16,$f20
/*    2afd4:	c7a600b8 */ 	lwc1	$f6,0xb8($sp)
/*    2afd8:	46004383 */ 	div.s	$f14,$f8,$f0
/*    2afdc:	c64a0000 */ 	lwc1	$f10,0x0($s2)
/*    2afe0:	c6440008 */ 	lwc1	$f4,0x8($s2)
/*    2afe4:	02802025 */ 	or	$a0,$s4,$zero
/*    2afe8:	02203825 */ 	or	$a3,$s1,$zero
/*    2afec:	46107482 */ 	mul.s	$f18,$f14,$f16
/*    2aff0:	00000000 */ 	sll	$zero,$zero,0x0
/*    2aff4:	46067202 */ 	mul.s	$f8,$f14,$f6
/*    2aff8:	46125080 */ 	add.s	$f2,$f10,$f18
/*    2affc:	46082300 */ 	add.s	$f12,$f4,$f8
/*    2b000:	44051000 */ 	mfc1	$a1,$f2
/*    2b004:	44066000 */ 	mfc1	$a2,$f12
/*    2b008:	0c009c15 */ 	jal	cdFindGroundInIntTileAtVertex
/*    2b00c:	00000000 */ 	sll	$zero,$zero,0x0
/*    2b010:	c6500004 */ 	lwc1	$f16,0x4($s2)
/*    2b014:	4610003c */ 	c.lt.s	$f0,$f16
/*    2b018:	00000000 */ 	sll	$zero,$zero,0x0
/*    2b01c:	45030007 */ 	bc1tl	.NB0002b03c
/*    2b020:	e7a000fc */ 	swc1	$f0,0xfc($sp)
/*    2b024:	8ecd0000 */ 	lw	$t5,0x0($s6)
/*    2b028:	95ae0002 */ 	lhu	$t6,0x2($t5)
/*    2b02c:	31cf2000 */ 	andi	$t7,$t6,0x2000
/*    2b030:	51e00052 */ 	beqzl	$t7,.NB0002b17c
/*    2b034:	02a08825 */ 	or	$s1,$s5,$zero
/*    2b038:	e7a000fc */ 	swc1	$f0,0xfc($sp)
.NB0002b03c:
/*    2b03c:	afd60000 */ 	sw	$s6,0x0($s8)
/*    2b040:	1000004d */ 	beqz	$zero,.NB0002b178
/*    2b044:	e7be00e4 */ 	swc1	$f30,0xe4($sp)
/*    2b048:	4600b306 */ 	mov.s	$f12,$f22
.NB0002b04c:
/*    2b04c:	4600c386 */ 	mov.s	$f14,$f24
/*    2b050:	8e460000 */ 	lw	$a2,0x0($s2)
/*    2b054:	0c009ac1 */ 	jal	cd00025724
/*    2b058:	8e470008 */ 	lw	$a3,0x8($s2)
/*    2b05c:	46000506 */ 	mov.s	$f20,$f0
/*    2b060:	4600d306 */ 	mov.s	$f12,$f26
/*    2b064:	4600e386 */ 	mov.s	$f14,$f28
/*    2b068:	8e460000 */ 	lw	$a2,0x0($s2)
/*    2b06c:	0c009ac1 */ 	jal	cd00025724
/*    2b070:	8e470008 */ 	lw	$a3,0x8($s2)
/*    2b074:	4600a03c */ 	c.lt.s	$f20,$f0
/*    2b078:	46000586 */ 	mov.s	$f22,$f0
/*    2b07c:	45020021 */ 	bc1fl	.NB0002b104
/*    2b080:	c7a800e4 */ 	lwc1	$f8,0xe4($sp)
/*    2b084:	c7aa00e4 */ 	lwc1	$f10,0xe4($sp)
/*    2b088:	460aa03c */ 	c.lt.s	$f20,$f10
/*    2b08c:	00000000 */ 	sll	$zero,$zero,0x0
/*    2b090:	4502003a */ 	bc1fl	.NB0002b17c
/*    2b094:	02a08825 */ 	or	$s1,$s5,$zero
/*    2b098:	8678000e */ 	lh	$t8,0xe($s3)
/*    2b09c:	86790012 */ 	lh	$t9,0x12($s3)
/*    2b0a0:	02802025 */ 	or	$a0,$s4,$zero
/*    2b0a4:	44989000 */ 	mtc1	$t8,$f18
/*    2b0a8:	44993000 */ 	mtc1	$t9,$f6
/*    2b0ac:	02203825 */ 	or	$a3,$s1,$zero
/*    2b0b0:	468090a0 */ 	cvt.s.w	$f2,$f18
/*    2b0b4:	46803320 */ 	cvt.s.w	$f12,$f6
/*    2b0b8:	44051000 */ 	mfc1	$a1,$f2
/*    2b0bc:	44066000 */ 	mfc1	$a2,$f12
/*    2b0c0:	0c009c15 */ 	jal	cdFindGroundInIntTileAtVertex
/*    2b0c4:	00000000 */ 	sll	$zero,$zero,0x0
/*    2b0c8:	c6440004 */ 	lwc1	$f4,0x4($s2)
/*    2b0cc:	4604003c */ 	c.lt.s	$f0,$f4
/*    2b0d0:	00000000 */ 	sll	$zero,$zero,0x0
/*    2b0d4:	45030007 */ 	bc1tl	.NB0002b0f4
/*    2b0d8:	e7a000fc */ 	swc1	$f0,0xfc($sp)
/*    2b0dc:	8ec80000 */ 	lw	$t0,0x0($s6)
/*    2b0e0:	95090002 */ 	lhu	$t1,0x2($t0)
/*    2b0e4:	312a2000 */ 	andi	$t2,$t1,0x2000
/*    2b0e8:	51400024 */ 	beqzl	$t2,.NB0002b17c
/*    2b0ec:	02a08825 */ 	or	$s1,$s5,$zero
/*    2b0f0:	e7a000fc */ 	swc1	$f0,0xfc($sp)
.NB0002b0f4:
/*    2b0f4:	afd60000 */ 	sw	$s6,0x0($s8)
/*    2b0f8:	1000001f */ 	beqz	$zero,.NB0002b178
/*    2b0fc:	e7b400e4 */ 	swc1	$f20,0xe4($sp)
/*    2b100:	c7a800e4 */ 	lwc1	$f8,0xe4($sp)
.NB0002b104:
/*    2b104:	4608003c */ 	c.lt.s	$f0,$f8
/*    2b108:	00000000 */ 	sll	$zero,$zero,0x0
/*    2b10c:	4502001b */ 	bc1fl	.NB0002b17c
/*    2b110:	02a08825 */ 	or	$s1,$s5,$zero
/*    2b114:	860b000e */ 	lh	$t3,0xe($s0)
/*    2b118:	860c0012 */ 	lh	$t4,0x12($s0)
/*    2b11c:	02802025 */ 	or	$a0,$s4,$zero
/*    2b120:	448b8000 */ 	mtc1	$t3,$f16
/*    2b124:	448c5000 */ 	mtc1	$t4,$f10
/*    2b128:	02203825 */ 	or	$a3,$s1,$zero
/*    2b12c:	468080a0 */ 	cvt.s.w	$f2,$f16
/*    2b130:	46805320 */ 	cvt.s.w	$f12,$f10
/*    2b134:	44051000 */ 	mfc1	$a1,$f2
/*    2b138:	44066000 */ 	mfc1	$a2,$f12
/*    2b13c:	0c009c15 */ 	jal	cdFindGroundInIntTileAtVertex
/*    2b140:	00000000 */ 	sll	$zero,$zero,0x0
/*    2b144:	c6520004 */ 	lwc1	$f18,0x4($s2)
/*    2b148:	4612003c */ 	c.lt.s	$f0,$f18
/*    2b14c:	00000000 */ 	sll	$zero,$zero,0x0
/*    2b150:	45030007 */ 	bc1tl	.NB0002b170
/*    2b154:	e7a000fc */ 	swc1	$f0,0xfc($sp)
/*    2b158:	8ecd0000 */ 	lw	$t5,0x0($s6)
/*    2b15c:	95ae0002 */ 	lhu	$t6,0x2($t5)
/*    2b160:	31cf2000 */ 	andi	$t7,$t6,0x2000
/*    2b164:	51e00005 */ 	beqzl	$t7,.NB0002b17c
/*    2b168:	02a08825 */ 	or	$s1,$s5,$zero
/*    2b16c:	e7a000fc */ 	swc1	$f0,0xfc($sp)
.NB0002b170:
/*    2b170:	afd60000 */ 	sw	$s6,0x0($s8)
/*    2b174:	e7b600e4 */ 	swc1	$f22,0xe4($sp)
.NB0002b178:
/*    2b178:	02a08825 */ 	or	$s1,$s5,$zero
.NB0002b17c:
/*    2b17c:	16b7ff50 */ 	bne	$s5,$s7,.NB0002aec0
/*    2b180:	26730006 */ 	addiu	$s3,$s3,0x6
/*    2b184:	10000099 */ 	beqz	$zero,.NB0002b3ec
/*    2b188:	8ec80014 */ 	lw	$t0,0x14($s6)
/*    2b18c:	24010001 */ 	addiu	$at,$zero,0x1
.NB0002b190:
/*    2b190:	54410096 */ 	bnel	$v0,$at,.NB0002b3ec
/*    2b194:	8ec80014 */ 	lw	$t0,0x14($s6)
/*    2b198:	90f50001 */ 	lbu	$s5,0x1($a3)
/*    2b19c:	00e09825 */ 	or	$s3,$a3,$zero
/*    2b1a0:	00001025 */ 	or	$v0,$zero,$zero
/*    2b1a4:	1aa00090 */ 	blez	$s5,.NB0002b3e8
/*    2b1a8:	00e08825 */ 	or	$s1,$a3,$zero
.NB0002b1ac:
/*    2b1ac:	24540001 */ 	addiu	$s4,$v0,0x1
/*    2b1b0:	0295001a */ 	div	$zero,$s4,$s5
/*    2b1b4:	0000c010 */ 	mfhi	$t8
/*    2b1b8:	0018c880 */ 	sll	$t9,$t8,0x2
/*    2b1bc:	0338c823 */ 	subu	$t9,$t9,$t8
/*    2b1c0:	0019c880 */ 	sll	$t9,$t9,0x2
/*    2b1c4:	02798021 */ 	addu	$s0,$s3,$t9
/*    2b1c8:	c6460000 */ 	lwc1	$f6,0x0($s2)
/*    2b1cc:	c61a0010 */ 	lwc1	$f26,0x10($s0)
/*    2b1d0:	c61c0018 */ 	lwc1	$f28,0x18($s0)
/*    2b1d4:	c6360010 */ 	lwc1	$f22,0x10($s1)
/*    2b1d8:	c6380018 */ 	lwc1	$f24,0x18($s1)
/*    2b1dc:	e7a60010 */ 	swc1	$f6,0x10($sp)
/*    2b1e0:	c6440008 */ 	lwc1	$f4,0x8($s2)
/*    2b1e4:	16a00002 */ 	bnez	$s5,.NB0002b1f0
/*    2b1e8:	00000000 */ 	sll	$zero,$zero,0x0
/*    2b1ec:	0007000d */ 	break	0x7
.NB0002b1f0:
/*    2b1f0:	2401ffff */ 	addiu	$at,$zero,-1
/*    2b1f4:	16a10004 */ 	bne	$s5,$at,.NB0002b208
/*    2b1f8:	3c018000 */ 	lui	$at,0x8000
/*    2b1fc:	16810002 */ 	bne	$s4,$at,.NB0002b208
/*    2b200:	00000000 */ 	sll	$zero,$zero,0x0
/*    2b204:	0006000d */ 	break	0x6
.NB0002b208:
/*    2b208:	4406d000 */ 	mfc1	$a2,$f26
/*    2b20c:	4407e000 */ 	mfc1	$a3,$f28
/*    2b210:	4600b306 */ 	mov.s	$f12,$f22
/*    2b214:	4600c386 */ 	mov.s	$f14,$f24
/*    2b218:	0c009a8d */ 	jal	cd00025654
/*    2b21c:	e7a40014 */ 	swc1	$f4,0x14($sp)
/*    2b220:	44804000 */ 	mtc1	$zero,$f8
/*    2b224:	e7a00098 */ 	swc1	$f0,0x98($sp)
/*    2b228:	46000786 */ 	mov.s	$f30,$f0
/*    2b22c:	4608003c */ 	c.lt.s	$f0,$f8
/*    2b230:	c7b000e4 */ 	lwc1	$f16,0xe4($sp)
/*    2b234:	45020003 */ 	bc1fl	.NB0002b244
/*    2b238:	4610f03c */ 	c.lt.s	$f30,$f16
/*    2b23c:	46000787 */ 	neg.s	$f30,$f0
/*    2b240:	4610f03c */ 	c.lt.s	$f30,$f16
.NB0002b244:
/*    2b244:	00000000 */ 	sll	$zero,$zero,0x0
/*    2b248:	45020065 */ 	bc1fl	.NB0002b3e0
/*    2b24c:	02801025 */ 	or	$v0,$s4,$zero
/*    2b250:	c64a0000 */ 	lwc1	$f10,0x0($s2)
/*    2b254:	4406d000 */ 	mfc1	$a2,$f26
/*    2b258:	4407e000 */ 	mfc1	$a3,$f28
/*    2b25c:	e7aa0010 */ 	swc1	$f10,0x10($sp)
/*    2b260:	c6520008 */ 	lwc1	$f18,0x8($s2)
/*    2b264:	4600b306 */ 	mov.s	$f12,$f22
/*    2b268:	4600c386 */ 	mov.s	$f14,$f24
/*    2b26c:	0c009ad5 */ 	jal	cd00025774
/*    2b270:	e7b20014 */ 	swc1	$f18,0x14($sp)
/*    2b274:	50400022 */ 	beqzl	$v0,.NB0002b300
/*    2b278:	4600b306 */ 	mov.s	$f12,$f22
/*    2b27c:	4616d001 */ 	sub.s	$f0,$f26,$f22
/*    2b280:	4618e501 */ 	sub.s	$f20,$f28,$f24
/*    2b284:	46000182 */ 	mul.s	$f6,$f0,$f0
/*    2b288:	e7a0007c */ 	swc1	$f0,0x7c($sp)
/*    2b28c:	4614a102 */ 	mul.s	$f4,$f20,$f20
/*    2b290:	0c012e84 */ 	jal	sqrtf
/*    2b294:	46043300 */ 	add.s	$f12,$f6,$f4
/*    2b298:	c7a80098 */ 	lwc1	$f8,0x98($sp)
/*    2b29c:	4600a287 */ 	neg.s	$f10,$f20
/*    2b2a0:	c7a4007c */ 	lwc1	$f4,0x7c($sp)
/*    2b2a4:	46004383 */ 	div.s	$f14,$f8,$f0
/*    2b2a8:	c6500000 */ 	lwc1	$f16,0x0($s2)
/*    2b2ac:	c6460008 */ 	lwc1	$f6,0x8($s2)
/*    2b2b0:	02602025 */ 	or	$a0,$s3,$zero
/*    2b2b4:	460a7482 */ 	mul.s	$f18,$f14,$f10
/*    2b2b8:	00000000 */ 	sll	$zero,$zero,0x0
/*    2b2bc:	46047202 */ 	mul.s	$f8,$f14,$f4
/*    2b2c0:	46128080 */ 	add.s	$f2,$f16,$f18
/*    2b2c4:	46083300 */ 	add.s	$f12,$f6,$f8
/*    2b2c8:	44051000 */ 	mfc1	$a1,$f2
/*    2b2cc:	44066000 */ 	mfc1	$a2,$f12
/*    2b2d0:	0c009d37 */ 	jal	cdFindGroundInFltTile
/*    2b2d4:	00000000 */ 	sll	$zero,$zero,0x0
/*    2b2d8:	c64a0004 */ 	lwc1	$f10,0x4($s2)
/*    2b2dc:	460a003c */ 	c.lt.s	$f0,$f10
/*    2b2e0:	00000000 */ 	sll	$zero,$zero,0x0
/*    2b2e4:	4502003e */ 	bc1fl	.NB0002b3e0
/*    2b2e8:	02801025 */ 	or	$v0,$s4,$zero
/*    2b2ec:	e7a000fc */ 	swc1	$f0,0xfc($sp)
/*    2b2f0:	afd60000 */ 	sw	$s6,0x0($s8)
/*    2b2f4:	10000039 */ 	beqz	$zero,.NB0002b3dc
/*    2b2f8:	e7be00e4 */ 	swc1	$f30,0xe4($sp)
/*    2b2fc:	4600b306 */ 	mov.s	$f12,$f22
.NB0002b300:
/*    2b300:	4600c386 */ 	mov.s	$f14,$f24
/*    2b304:	8e460000 */ 	lw	$a2,0x0($s2)
/*    2b308:	0c009ac1 */ 	jal	cd00025724
/*    2b30c:	8e470008 */ 	lw	$a3,0x8($s2)
/*    2b310:	46000506 */ 	mov.s	$f20,$f0
/*    2b314:	4600d306 */ 	mov.s	$f12,$f26
/*    2b318:	4600e386 */ 	mov.s	$f14,$f28
/*    2b31c:	8e460000 */ 	lw	$a2,0x0($s2)
/*    2b320:	0c009ac1 */ 	jal	cd00025724
/*    2b324:	8e470008 */ 	lw	$a3,0x8($s2)
/*    2b328:	4600a03c */ 	c.lt.s	$f20,$f0
/*    2b32c:	46000586 */ 	mov.s	$f22,$f0
/*    2b330:	45020017 */ 	bc1fl	.NB0002b390
/*    2b334:	c7a400e4 */ 	lwc1	$f4,0xe4($sp)
/*    2b338:	c7b000e4 */ 	lwc1	$f16,0xe4($sp)
/*    2b33c:	4610a03c */ 	c.lt.s	$f20,$f16
/*    2b340:	00000000 */ 	sll	$zero,$zero,0x0
/*    2b344:	45020026 */ 	bc1fl	.NB0002b3e0
/*    2b348:	02801025 */ 	or	$v0,$s4,$zero
/*    2b34c:	c6220010 */ 	lwc1	$f2,0x10($s1)
/*    2b350:	c62c0018 */ 	lwc1	$f12,0x18($s1)
/*    2b354:	02602025 */ 	or	$a0,$s3,$zero
/*    2b358:	44051000 */ 	mfc1	$a1,$f2
/*    2b35c:	44066000 */ 	mfc1	$a2,$f12
/*    2b360:	0c009d37 */ 	jal	cdFindGroundInFltTile
/*    2b364:	00000000 */ 	sll	$zero,$zero,0x0
/*    2b368:	c6520004 */ 	lwc1	$f18,0x4($s2)
/*    2b36c:	4612003c */ 	c.lt.s	$f0,$f18
/*    2b370:	00000000 */ 	sll	$zero,$zero,0x0
/*    2b374:	4502001a */ 	bc1fl	.NB0002b3e0
/*    2b378:	02801025 */ 	or	$v0,$s4,$zero
/*    2b37c:	e7a000fc */ 	swc1	$f0,0xfc($sp)
/*    2b380:	afd60000 */ 	sw	$s6,0x0($s8)
/*    2b384:	10000015 */ 	beqz	$zero,.NB0002b3dc
/*    2b388:	e7b400e4 */ 	swc1	$f20,0xe4($sp)
/*    2b38c:	c7a400e4 */ 	lwc1	$f4,0xe4($sp)
.NB0002b390:
/*    2b390:	4604003c */ 	c.lt.s	$f0,$f4
/*    2b394:	00000000 */ 	sll	$zero,$zero,0x0
/*    2b398:	45020011 */ 	bc1fl	.NB0002b3e0
/*    2b39c:	02801025 */ 	or	$v0,$s4,$zero
/*    2b3a0:	c6020010 */ 	lwc1	$f2,0x10($s0)
/*    2b3a4:	c60c0018 */ 	lwc1	$f12,0x18($s0)
/*    2b3a8:	02602025 */ 	or	$a0,$s3,$zero
/*    2b3ac:	44051000 */ 	mfc1	$a1,$f2
/*    2b3b0:	44066000 */ 	mfc1	$a2,$f12
/*    2b3b4:	0c009d37 */ 	jal	cdFindGroundInFltTile
/*    2b3b8:	00000000 */ 	sll	$zero,$zero,0x0
/*    2b3bc:	c6460004 */ 	lwc1	$f6,0x4($s2)
/*    2b3c0:	4606003c */ 	c.lt.s	$f0,$f6
/*    2b3c4:	00000000 */ 	sll	$zero,$zero,0x0
/*    2b3c8:	45020005 */ 	bc1fl	.NB0002b3e0
/*    2b3cc:	02801025 */ 	or	$v0,$s4,$zero
/*    2b3d0:	e7a000fc */ 	swc1	$f0,0xfc($sp)
/*    2b3d4:	afd60000 */ 	sw	$s6,0x0($s8)
/*    2b3d8:	e7b600e4 */ 	swc1	$f22,0xe4($sp)
.NB0002b3dc:
/*    2b3dc:	02801025 */ 	or	$v0,$s4,$zero
.NB0002b3e0:
/*    2b3e0:	1695ff72 */ 	bne	$s4,$s5,.NB0002b1ac
/*    2b3e4:	2631000c */ 	addiu	$s1,$s1,0xc
.NB0002b3e8:
/*    2b3e8:	8ec80014 */ 	lw	$t0,0x14($s6)
.NB0002b3ec:
/*    2b3ec:	26d60014 */ 	addiu	$s6,$s6,0x14
/*    2b3f0:	5500fe9f */ 	bnezl	$t0,.NB0002ae70
/*    2b3f4:	8ecc0004 */ 	lw	$t4,0x4($s6)
/*    2b3f8:	8fbf0074 */ 	lw	$ra,0x74($sp)
.NB0002b3fc:
/*    2b3fc:	c7a000fc */ 	lwc1	$f0,0xfc($sp)
/*    2b400:	d7b40020 */ 	ldc1	$f20,0x20($sp)
/*    2b404:	d7b60028 */ 	ldc1	$f22,0x28($sp)
/*    2b408:	d7b80030 */ 	ldc1	$f24,0x30($sp)
/*    2b40c:	d7ba0038 */ 	ldc1	$f26,0x38($sp)
/*    2b410:	d7bc0040 */ 	ldc1	$f28,0x40($sp)
/*    2b414:	d7be0048 */ 	ldc1	$f30,0x48($sp)
/*    2b418:	8fb00050 */ 	lw	$s0,0x50($sp)
/*    2b41c:	8fb10054 */ 	lw	$s1,0x54($sp)
/*    2b420:	8fb20058 */ 	lw	$s2,0x58($sp)
/*    2b424:	8fb3005c */ 	lw	$s3,0x5c($sp)
/*    2b428:	8fb40060 */ 	lw	$s4,0x60($sp)
/*    2b42c:	8fb50064 */ 	lw	$s5,0x64($sp)
/*    2b430:	8fb60068 */ 	lw	$s6,0x68($sp)
/*    2b434:	8fb7006c */ 	lw	$s7,0x6c($sp)
/*    2b438:	8fbe0070 */ 	lw	$s8,0x70($sp)
/*    2b43c:	03e00008 */ 	jr	$ra
/*    2b440:	27bd0108 */ 	addiu	$sp,$sp,0x108
);
#endif

bool cd00029ffc(struct coord *pos, f32 width, f32 foreheadheight, f32 inversefeettoeyesheight, s16 *rooms, u16 arg5, struct coord *laddernormal)
{
	u32 stack[5];
	struct collisionthing thing;

	cd00027d1c(pos, width, rooms, CDTYPE_BG,
			arg5, 1, foreheadheight, inversefeettoeyesheight,
			&thing, 1);

	if (thing.geo) {
		struct geotilei *tile = (struct geotilei *) thing.geo;
		struct coord dist;

		cd00025928(thing.geo, laddernormal);

		dist.x = pos->x - tile->vertices[0][0];
		dist.y = pos->y - tile->vertices[0][1];
		dist.z = pos->z - tile->vertices[0][2];

		if (dist.f[0] * laddernormal->f[0] + dist.f[1] * laddernormal->f[1] + dist.f[2] * laddernormal->f[2] < 0) {
			laddernormal->x = -laddernormal->x;
			laddernormal->y = -laddernormal->y;
			laddernormal->z = -laddernormal->z;
		}

		return true;
	}

	return false;
}

bool cd0002a13c(struct coord *pos, f32 radius, f32 arg2, f32 arg3, s16 *rooms, u16 arg5)
{
	u32 stack[5];
	struct collisionthing thing;

	cd00027d1c(pos, radius, rooms, CDTYPE_BG, arg5, 1, arg2, arg3, &thing, 1);

	if (thing.geo) {
		return true;
	}

	return false;
}

f32 cdFindGroundY(struct coord *pos, f32 radius, s16 *rooms, u16 *floorcol,
		u8 *floortype, u16 *floorflags, s16 *floorroom, s32 *inlift, struct prop **lift)
{
	struct collisionthing cdthings[21];
	struct collisionthing *sp72 = NULL;
	f32 ground;
	struct geo *geo = NULL;

	cd00027d1c(pos, radius, rooms, CDTYPE_ALL, 3, 0, 0, 0, cdthings, 20);
	ground = cd000296a0(cdthings, pos, &sp72, radius);

	if (sp72) {
		geo = sp72->geo;
	}

	if (floorcol) {
		cdGetFloorCol(geo, floorcol);
	}

	if (floortype) {
		cdGetFloorType(geo, floortype);
	}

	if (floorflags && geo) {
		*floorflags = geo->flags;
	}

	if (floorroom) {
		if (sp72) {
			*floorroom = sp72->room;
		} else {
			*floorroom = -1;
		}
	}

	if (inlift) {
		if (geo && geo->type == GEOTYPE_TILE_F && (geo->flags & GEOFLAG_LIFTFLOOR)) {
			*inlift = true;
			*lift = sp72->prop;

			if (*lift && (*lift)->obj->modelnum == MODEL_ESCA_STEP && floortype) {
				*floortype = FLOORTYPE_METAL;
			}
		} else {
			*inlift = false;
			*lift = NULL;
		}
	}

	return ground;
}

/**
 * This function must come immediately after cdFindGroundY.
 *
 * A piracy check looks for this function, then backtracks two instructions to
 * nop the jr ra at the end of cdFindGroundY, causing it to flow into this
 * function and return 0.
 */
f32 cd0002a324(void)
{
	return 0;
}

f32 cdFindGroundYSimple(struct coord *pos, f32 width, s16 *rooms, u16 *floorcol, u8 *floortype)
{
	return cdFindGroundY(pos, width, rooms, floorcol, floortype, NULL, NULL, NULL, NULL);
}

f32 cd0002a36c(struct coord *coord, s16 *rooms, u16 *floorcol, u8 *floortype)
{
	struct geo *geo;
	s16 sp30[2];
	f32 sp2c;
	f32 result = -4294967296;

	cd00026e7c(coord, rooms, 3, &geo, &sp30[1], &sp2c, NULL, 0);

	if (geo) {
		result = sp2c;
	}

	if (floorcol) {
		cdGetFloorCol(geo, floorcol);
	}

	if (floortype) {
		cdGetFloorType(geo, floortype);
	}

	return result;
}

s32 cdFindRoom(struct coord *pos, s16 *nearrooms)
{
	struct geo *geo;
	s16 room;
	f32 sp2c;

	cd00026e7c(pos, nearrooms, 3, &geo, &room, &sp2c, 0, 0);

	return room;
}

#if VERSION >= VERSION_NTSC_1_0
s16 cd0002a440(struct coord *pos, s16 *rooms, f32 *arg2, u16 *floorcolptr, u16 *flagsptr)
#else
s16 cd0002a440(struct coord *pos, s16 *rooms, f32 *arg2, u16 *floorcolptr)
#endif
{
	struct geo *geo;
	s16 room;
	f32 sp2c;

	cd00026e7c(pos, rooms, 3, &geo, &room, &sp2c, NULL, 0);

	if (geo != NULL) {
		*arg2 = sp2c;
	}

	if (floorcolptr != NULL) {
		cdGetFloorCol(geo, floorcolptr);
	}

#if VERSION >= VERSION_NTSC_1_0
	if (flagsptr != NULL && geo != NULL) {
		*flagsptr = geo->flags;
	}
#endif

	return room;
}

#if VERSION >= VERSION_NTSC_1_0
s16 cd0002a4d0(struct coord *pos, s16 *rooms, f32 *arg2, u16 *floorcolptr, u16 *flagsptr)
#else
s16 cd0002a4d0(struct coord *pos, s16 *rooms, f32 *arg2, u16 *floorcolptr)
#endif
{
	struct geo *geo;
	s16 sp32;
	f32 sp2c;

	cd00026e7c(pos, rooms, 3, &geo, &sp32, &sp2c, NULL, 1);

	if (geo != NULL) {
		*arg2 = sp2c;
	}

	if (floorcolptr != NULL) {
		cdGetFloorCol(geo, floorcolptr);
	}

#if VERSION >= VERSION_NTSC_1_0
	if (flagsptr != NULL && geo != NULL) {
		*flagsptr = geo->flags;
	}
#endif

	return sp32;
}

s16 cd0002a564(struct coord *pos, s16 *rooms, f32 *arg2, u16 *floorcol, struct coord *arg4, struct prop **propptr)
{
	struct geo *geo;
	s16 sp32;
	f32 sp2c;

	cd00026e7c(pos, rooms, 3, &geo, &sp32, &sp2c, propptr, 0);

	if (geo) {
		*arg2 = sp2c;
		cd00025928(geo, arg4);
	}

	if (floorcol) {
		cdGetFloorCol(geo, floorcol);
	}

	return sp32;
}

s16 cd0002a5e4(struct coord *pos, s16 *rooms, f32 *arg2, u16 *floorcol, u16 *flagsptr, struct coord *arg5)
{
	struct geo *geo;
	s16 sp32;
	f32 sp2c;

	cd00026e7c(pos, rooms, 3, &geo, &sp32, &sp2c, NULL, 1);

	if (geo) {
		*arg2 = sp2c;
		cd00025928(geo, arg5);
	}

	if (floorcol) {
		cdGetFloorCol(geo, floorcol);
	}

	if (flagsptr != NULL && geo != NULL) {
		*flagsptr = geo->flags;
	}

	return sp32;
}

/**
 * Tests if a cylinder volume fits in the given position.
 */
s32 cdTestVolume(struct coord *pos, f32 width, s16 *rooms, s32 types, s32 arg4, f32 ymax, f32 ymin)
{
	struct collisionthing cdthings[2];
	bool result = true;

	cd00027d1c(pos, width, rooms, types, 4, arg4, ymax, ymin, cdthings, 1);

	if (cdthings[0].geo) {
		result = false;
		cd00025168(cdthings[0].prop);
	}

	return result;
}

GLOBAL_ASM(
glabel cd0002a6fc
/*    2a6fc:	27bdff58 */ 	addiu	$sp,$sp,-168
/*    2a700:	44866000 */ 	mtc1	$a2,$f12
/*    2a704:	afb00030 */ 	sw	$s0,0x30($sp)
/*    2a708:	00808025 */ 	or	$s0,$a0,$zero
/*    2a70c:	8fb800bc */ 	lw	$t8,0xbc($sp)
/*    2a710:	c7a400c0 */ 	lwc1	$f4,0xc0($sp)
/*    2a714:	c7a600c4 */ 	lwc1	$f6,0xc4($sp)
/*    2a718:	afa500ac */ 	sw	$a1,0xac($sp)
/*    2a71c:	00a02025 */ 	or	$a0,$a1,$zero
/*    2a720:	afbf0034 */ 	sw	$ra,0x34($sp)
/*    2a724:	afa700b4 */ 	sw	$a3,0xb4($sp)
/*    2a728:	240e0001 */ 	addiu	$t6,$zero,0x1
/*    2a72c:	00e03025 */ 	or	$a2,$a3,$zero
/*    2a730:	27b90080 */ 	addiu	$t9,$sp,0x80
/*    2a734:	240f0004 */ 	addiu	$t7,$zero,0x4
/*    2a738:	24080001 */ 	addiu	$t0,$zero,0x1
/*    2a73c:	44056000 */ 	mfc1	$a1,$f12
/*    2a740:	afae007c */ 	sw	$t6,0x7c($sp)
/*    2a744:	afa80024 */ 	sw	$t0,0x24($sp)
/*    2a748:	afaf0010 */ 	sw	$t7,0x10($sp)
/*    2a74c:	afb90020 */ 	sw	$t9,0x20($sp)
/*    2a750:	8fa700b8 */ 	lw	$a3,0xb8($sp)
/*    2a754:	afb80014 */ 	sw	$t8,0x14($sp)
/*    2a758:	e7a40018 */ 	swc1	$f4,0x18($sp)
/*    2a75c:	0c009f47 */ 	jal	cd00027d1c
/*    2a760:	e7a6001c */ 	swc1	$f6,0x1c($sp)
/*    2a764:	8fa90080 */ 	lw	$t1,0x80($sp)
/*    2a768:	5120009d */ 	beqzl	$t1,.L0002a9e0
/*    2a76c:	8fbf0034 */ 	lw	$ra,0x34($sp)
/*    2a770:	afa0007c */ 	sw	$zero,0x7c($sp)
/*    2a774:	91220000 */ 	lbu	$v0,0x0($t1)
/*    2a778:	54400037 */ 	bnezl	$v0,.L0002a858
/*    2a77c:	24010001 */ 	addiu	$at,$zero,0x1
/*    2a780:	8fa50088 */ 	lw	$a1,0x88($sp)
/*    2a784:	912b0001 */ 	lbu	$t3,0x1($t1)
/*    2a788:	24070006 */ 	addiu	$a3,$zero,0x6
/*    2a78c:	24aa0001 */ 	addiu	$t2,$a1,0x1
/*    2a790:	014b001a */ 	div	$zero,$t2,$t3
/*    2a794:	00003010 */ 	mfhi	$a2
/*    2a798:	01201025 */ 	or	$v0,$t1,$zero
/*    2a79c:	15600002 */ 	bnez	$t3,.L0002a7a8
/*    2a7a0:	00000000 */ 	nop
/*    2a7a4:	0007000d */ 	break	0x7
.L0002a7a8:
/*    2a7a8:	2401ffff */ 	addiu	$at,$zero,-1
/*    2a7ac:	15610004 */ 	bne	$t3,$at,.L0002a7c0
/*    2a7b0:	3c018000 */ 	lui	$at,0x8000
/*    2a7b4:	15410002 */ 	bne	$t2,$at,.L0002a7c0
/*    2a7b8:	00000000 */ 	nop
/*    2a7bc:	0006000d */ 	break	0x6
.L0002a7c0:
/*    2a7c0:	00a70019 */ 	multu	$a1,$a3
/*    2a7c4:	00006012 */ 	mflo	$t4
/*    2a7c8:	004c1821 */ 	addu	$v1,$v0,$t4
/*    2a7cc:	846d000e */ 	lh	$t5,0xe($v1)
/*    2a7d0:	00c70019 */ 	multu	$a2,$a3
/*    2a7d4:	448d4000 */ 	mtc1	$t5,$f8
/*    2a7d8:	00000000 */ 	nop
/*    2a7dc:	468042a0 */ 	cvt.s.w	$f10,$f8
/*    2a7e0:	0000c012 */ 	mflo	$t8
/*    2a7e4:	e7aa0070 */ 	swc1	$f10,0x70($sp)
/*    2a7e8:	846e0010 */ 	lh	$t6,0x10($v1)
/*    2a7ec:	00582021 */ 	addu	$a0,$v0,$t8
/*    2a7f0:	448e8000 */ 	mtc1	$t6,$f16
/*    2a7f4:	00000000 */ 	nop
/*    2a7f8:	468084a0 */ 	cvt.s.w	$f18,$f16
/*    2a7fc:	e7b20074 */ 	swc1	$f18,0x74($sp)
/*    2a800:	846f0012 */ 	lh	$t7,0x12($v1)
/*    2a804:	448f2000 */ 	mtc1	$t7,$f4
/*    2a808:	00000000 */ 	nop
/*    2a80c:	468021a0 */ 	cvt.s.w	$f6,$f4
/*    2a810:	e7a60078 */ 	swc1	$f6,0x78($sp)
/*    2a814:	8499000e */ 	lh	$t9,0xe($a0)
/*    2a818:	44994000 */ 	mtc1	$t9,$f8
/*    2a81c:	00000000 */ 	nop
/*    2a820:	468042a0 */ 	cvt.s.w	$f10,$f8
/*    2a824:	e7aa0064 */ 	swc1	$f10,0x64($sp)
/*    2a828:	84880010 */ 	lh	$t0,0x10($a0)
/*    2a82c:	44888000 */ 	mtc1	$t0,$f16
/*    2a830:	00000000 */ 	nop
/*    2a834:	468084a0 */ 	cvt.s.w	$f18,$f16
/*    2a838:	e7b20068 */ 	swc1	$f18,0x68($sp)
/*    2a83c:	84890012 */ 	lh	$t1,0x12($a0)
/*    2a840:	44892000 */ 	mtc1	$t1,$f4
/*    2a844:	00000000 */ 	nop
/*    2a848:	468021a0 */ 	cvt.s.w	$f6,$f4
/*    2a84c:	1000005f */ 	b	.L0002a9cc
/*    2a850:	e7a6006c */ 	swc1	$f6,0x6c($sp)
/*    2a854:	24010001 */ 	addiu	$at,$zero,0x1
.L0002a858:
/*    2a858:	54410025 */ 	bnel	$v0,$at,.L0002a8f0
/*    2a85c:	24010002 */ 	addiu	$at,$zero,0x2
/*    2a860:	8fa20080 */ 	lw	$v0,0x80($sp)
/*    2a864:	8fa50088 */ 	lw	$a1,0x88($sp)
/*    2a868:	2407000c */ 	addiu	$a3,$zero,0xc
/*    2a86c:	904b0001 */ 	lbu	$t3,0x1($v0)
/*    2a870:	24aa0001 */ 	addiu	$t2,$a1,0x1
/*    2a874:	014b001a */ 	div	$zero,$t2,$t3
/*    2a878:	00003010 */ 	mfhi	$a2
/*    2a87c:	15600002 */ 	bnez	$t3,.L0002a888
/*    2a880:	00000000 */ 	nop
/*    2a884:	0007000d */ 	break	0x7
.L0002a888:
/*    2a888:	2401ffff */ 	addiu	$at,$zero,-1
/*    2a88c:	15610004 */ 	bne	$t3,$at,.L0002a8a0
/*    2a890:	3c018000 */ 	lui	$at,0x8000
/*    2a894:	15410002 */ 	bne	$t2,$at,.L0002a8a0
/*    2a898:	00000000 */ 	nop
/*    2a89c:	0006000d */ 	break	0x6
.L0002a8a0:
/*    2a8a0:	00a70019 */ 	multu	$a1,$a3
/*    2a8a4:	00006012 */ 	mflo	$t4
/*    2a8a8:	004c1821 */ 	addu	$v1,$v0,$t4
/*    2a8ac:	c4680010 */ 	lwc1	$f8,0x10($v1)
/*    2a8b0:	00c70019 */ 	multu	$a2,$a3
/*    2a8b4:	e7a80070 */ 	swc1	$f8,0x70($sp)
/*    2a8b8:	c46a0014 */ 	lwc1	$f10,0x14($v1)
/*    2a8bc:	e7aa0074 */ 	swc1	$f10,0x74($sp)
/*    2a8c0:	c4700018 */ 	lwc1	$f16,0x18($v1)
/*    2a8c4:	00006812 */ 	mflo	$t5
/*    2a8c8:	004d2021 */ 	addu	$a0,$v0,$t5
/*    2a8cc:	e7b00078 */ 	swc1	$f16,0x78($sp)
/*    2a8d0:	c4920010 */ 	lwc1	$f18,0x10($a0)
/*    2a8d4:	e7b20064 */ 	swc1	$f18,0x64($sp)
/*    2a8d8:	c4840014 */ 	lwc1	$f4,0x14($a0)
/*    2a8dc:	e7a40068 */ 	swc1	$f4,0x68($sp)
/*    2a8e0:	c4860018 */ 	lwc1	$f6,0x18($a0)
/*    2a8e4:	10000039 */ 	b	.L0002a9cc
/*    2a8e8:	e7a6006c */ 	swc1	$f6,0x6c($sp)
/*    2a8ec:	24010002 */ 	addiu	$at,$zero,0x2
.L0002a8f0:
/*    2a8f0:	14410020 */ 	bne	$v0,$at,.L0002a974
/*    2a8f4:	8fa30088 */ 	lw	$v1,0x88($sp)
/*    2a8f8:	8fa20080 */ 	lw	$v0,0x80($sp)
/*    2a8fc:	246e0001 */ 	addiu	$t6,$v1,0x1
/*    2a900:	0003c0c0 */ 	sll	$t8,$v1,0x3
/*    2a904:	904f0001 */ 	lbu	$t7,0x1($v0)
/*    2a908:	00582021 */ 	addu	$a0,$v0,$t8
/*    2a90c:	c488000c */ 	lwc1	$f8,0xc($a0)
/*    2a910:	01cf001a */ 	div	$zero,$t6,$t7
/*    2a914:	e7a80070 */ 	swc1	$f8,0x70($sp)
/*    2a918:	c60a0004 */ 	lwc1	$f10,0x4($s0)
/*    2a91c:	00003010 */ 	mfhi	$a2
/*    2a920:	0006c8c0 */ 	sll	$t9,$a2,0x3
/*    2a924:	e7aa0074 */ 	swc1	$f10,0x74($sp)
/*    2a928:	c4900010 */ 	lwc1	$f16,0x10($a0)
/*    2a92c:	00592821 */ 	addu	$a1,$v0,$t9
/*    2a930:	15e00002 */ 	bnez	$t7,.L0002a93c
/*    2a934:	00000000 */ 	nop
/*    2a938:	0007000d */ 	break	0x7
.L0002a93c:
/*    2a93c:	2401ffff */ 	addiu	$at,$zero,-1
/*    2a940:	15e10004 */ 	bne	$t7,$at,.L0002a954
/*    2a944:	3c018000 */ 	lui	$at,0x8000
/*    2a948:	15c10002 */ 	bne	$t6,$at,.L0002a954
/*    2a94c:	00000000 */ 	nop
/*    2a950:	0006000d */ 	break	0x6
.L0002a954:
/*    2a954:	e7b00078 */ 	swc1	$f16,0x78($sp)
/*    2a958:	c4b2000c */ 	lwc1	$f18,0xc($a1)
/*    2a95c:	e7b20064 */ 	swc1	$f18,0x64($sp)
/*    2a960:	c6040004 */ 	lwc1	$f4,0x4($s0)
/*    2a964:	e7a40068 */ 	swc1	$f4,0x68($sp)
/*    2a968:	c4a60010 */ 	lwc1	$f6,0x10($a1)
/*    2a96c:	10000017 */ 	b	.L0002a9cc
/*    2a970:	e7a6006c */ 	swc1	$f6,0x6c($sp)
.L0002a974:
/*    2a974:	24010003 */ 	addiu	$at,$zero,0x3
/*    2a978:	14410014 */ 	bne	$v0,$at,.L0002a9cc
/*    2a97c:	27a80070 */ 	addiu	$t0,$sp,0x70
/*    2a980:	8fa20080 */ 	lw	$v0,0x80($sp)
/*    2a984:	c6080008 */ 	lwc1	$f8,0x8($s0)
/*    2a988:	8e070000 */ 	lw	$a3,0x0($s0)
/*    2a98c:	c44c000c */ 	lwc1	$f12,0xc($v0)
/*    2a990:	c44e0010 */ 	lwc1	$f14,0x10($v0)
/*    2a994:	8c460014 */ 	lw	$a2,0x14($v0)
/*    2a998:	27a90078 */ 	addiu	$t1,$sp,0x78
/*    2a99c:	27aa0064 */ 	addiu	$t2,$sp,0x64
/*    2a9a0:	27ab006c */ 	addiu	$t3,$sp,0x6c
/*    2a9a4:	afab0020 */ 	sw	$t3,0x20($sp)
/*    2a9a8:	afaa001c */ 	sw	$t2,0x1c($sp)
/*    2a9ac:	afa90018 */ 	sw	$t1,0x18($sp)
/*    2a9b0:	afa80014 */ 	sw	$t0,0x14($sp)
/*    2a9b4:	0c009612 */ 	jal	cd00025848
/*    2a9b8:	e7a80010 */ 	swc1	$f8,0x10($sp)
/*    2a9bc:	c60a0004 */ 	lwc1	$f10,0x4($s0)
/*    2a9c0:	e7aa0074 */ 	swc1	$f10,0x74($sp)
/*    2a9c4:	c6100004 */ 	lwc1	$f16,0x4($s0)
/*    2a9c8:	e7b00068 */ 	swc1	$f16,0x68($sp)
.L0002a9cc:
/*    2a9cc:	27a40070 */ 	addiu	$a0,$sp,0x70
/*    2a9d0:	27a50064 */ 	addiu	$a1,$sp,0x64
/*    2a9d4:	0c0093ec */ 	jal	cd00024fb0
/*    2a9d8:	8fa6008c */ 	lw	$a2,0x8c($sp)
/*    2a9dc:	8fbf0034 */ 	lw	$ra,0x34($sp)
.L0002a9e0:
/*    2a9e0:	8fa2007c */ 	lw	$v0,0x7c($sp)
/*    2a9e4:	8fb00030 */ 	lw	$s0,0x30($sp)
/*    2a9e8:	03e00008 */ 	jr	$ra
/*    2a9ec:	27bd00a8 */ 	addiu	$sp,$sp,0xa8
);

// Mismatch: Goal copies geo from t1 to v0 in the type 0 block
//s32 cd0002a6fc(struct coord *pos, struct coord *pos2, f32 width, s16 *rooms, s32 types, bool arg5, f32 arg6, f32 arg7)
//{
//	u32 stack[5];
//	struct collisionthing thing; // 80
//	s32 cdresult;
//	struct coord sp70;
//	struct coord sp64;
//
//	cdresult = CDRESULT_NOCOLLISION;
//
//	cd00027d1c(pos2, width, rooms, types, 4, arg5, arg6, arg7, &thing, 1);
//
//	// 768
//	if (thing.geo != NULL) {
//		cdresult = CDRESULT_COLLISION;
//
//		// 778
//		if (thing.geo->type == GEOTYPE_TILE_I) {
//			struct geotilei *tile = (struct geotilei *) thing.geo;
//			s32 this = thing.vertexindex;
//			s32 next = (this + 1) % thing.geo->numvertices;
//
//			sp70.x = tile->vertices[this][0];
//			sp70.y = tile->vertices[this][1];
//			sp70.z = tile->vertices[this][2];
//
//			sp64.x = tile->vertices[next][0];
//			sp64.y = tile->vertices[next][1];
//			sp64.z = tile->vertices[next][2];
//		} else /*854*/ if (thing.geo->type == GEOTYPE_TILE_F) {
//			struct geotilef *type1 = (struct geotilef *) thing.geo;
//			s32 this = thing.vertexindex;
//			s32 next = (this + 1) % thing.geo->numvertices;
//
//			sp70.x = type1->vertices[this].x;
//			sp70.y = type1->vertices[this].y;
//			sp70.z = type1->vertices[this].z;
//
//			sp64.x = type1->vertices[next].x;
//			sp64.y = type1->vertices[next].y;
//			sp64.z = type1->vertices[next].z;
//		} else if (thing.geo->type == GEOTYPE_BLOCK) {
//			struct geoblock *block = (struct geoblock *) thing.geo;
//			s32 this = thing.vertexindex;
//			s32 next = (this + 1) % thing.geo->numvertices;
//
//			sp70.x = block->vertices[this][0];
//			sp70.y = pos->y;
//			sp70.z = block->vertices[this][1];
//
//			sp64.x = block->vertices[next][0];
//			sp64.y = pos->y;
//			sp64.z = block->vertices[next][1];
//		} else if (thing.geo->type == GEOTYPE_CYL) {
//			struct geocyl *cyl = (struct geocyl *) thing.geo;
//
//			if (1);
//
//			cd00025848(cyl->x, cyl->z, cyl->width, pos->x, pos->z, &sp70.x, &sp70.z, &sp64.x, &sp64.z);
//
//			sp70.y = pos->y;
//			sp64.y = pos->y;
//		}
//
//		cd00024fb0(&sp70, &sp64, thing.prop);
//	}
//
//	return cdresult;
//}

s32 cdTestAToB1(struct coord *origpos, struct coord *dstpos, f32 width, s16 *dstrooms, s32 types, s32 arg5, f32 ymax, f32 ymin)
{
	struct collisionthing things[21];
	struct coord dist;
	s32 result = CDRESULT_NOCOLLISION;

	cd00028df0(dstpos, width, dstrooms, types, 4, arg5, ymax, ymin, things, 20);

	if (things[0].geo) {
		result = CDRESULT_COLLISION;

		dist.x = dstpos->x - origpos->x;
		dist.y = dstpos->y - origpos->y;
		dist.z = dstpos->z - origpos->z;

		cd0002901c(origpos, &dist, width, things);
	}

	return result;
}

bool cd0002aac0(struct coord *arg0, struct coord *arg1, struct coord *arg2, struct geotilei *tile, struct coord *arg4, struct coord *arg5)
{
	s32 i;
	u8 numvertices = tile->header.numvertices;

	for (i = 2; i < numvertices; i++) {
		if (func0002f490((struct vec3s16 *)&tile->vertices[0][0],
					(struct vec3s16 *)&tile->vertices[i - 1][0],
					(struct vec3s16 *)&tile->vertices[i][0],
					0, arg0, arg1, arg2, arg4, arg5)) {
			return true;
		}
	}

	return false;
}

bool cd0002ab98(struct coord *arg0, struct coord *arg1, struct coord *arg2, struct geotilef *tile, struct coord *arg4, struct coord *arg5)
{
	s32 i;
	u8 numvertices = tile->header.numvertices;

	for (i = 2; i < numvertices; i++) {
		if (func0002f560(&tile->vertices[0], &tile->vertices[i - 1], &tile->vertices[i],
					0, arg0, arg1, arg2, arg4, arg5)) {
			return true;
		}
	}

	return false;
}

bool cd0002ac70(struct coord *arg0, struct coord *arg1, struct coord *arg2, struct geotilei *tile,
		struct coord *arg4, struct coord *arg5, struct coord *arg6, bool arg7, f32 arg8, f32 arg9)
{
	bool result = false;
	s32 i;
	f32 f0;
	s32 numvertices = tile->header.numvertices;
	s32 next;
	s32 spb8 = 1;
	f32 f22 = 1.0f;
	s32 spb0;
	f32 spac;
	f32 spa8;
	f32 ymax = *(s16 *)(tile->ymax + (u32)tile);
	f32 ymin = *(s16 *)(tile->ymin + (u32)tile);
	f32 spa0[2];
	f32 sp98[2];
	f32 sp90[2];
	f32 sp88[2];

	if (!arg7
			|| (arg0->y + arg8 >= ymin && arg1->y + arg9 <= ymax)
			|| (arg0->y + arg9 <= ymax && arg1->y + arg8 >= ymin)) {
		for (i = 0; i < numvertices; i++) {
			next = (i + 1) % numvertices;

			if (cd000254d8(arg0, arg1, tile->vertices[i][0], tile->vertices[i][2], tile->vertices[next][0], tile->vertices[next][2], &spb8)) {
				spa0[0] = arg0->x;
				spa0[1] = arg0->z;
				sp98[0] = arg1->x;
				sp98[1] = arg1->z;
				sp90[0] = tile->vertices[i][0];
				sp90[1] = tile->vertices[i][2];
				sp88[0] = tile->vertices[next][0];
				sp88[1] = tile->vertices[next][2];

				f0 = func0f1577f0(spa0, sp98, sp90, sp88);

				if (f0 < f22) {
					if (arg7) {
						spa8 = (arg1->y - arg0->y) * f0 + arg0->y;
						spac = spa8 + arg8;
						spa8 = spa8 + arg9;
					}

					if (!arg7 || (!(spa8 >= ymax) && !(spac <= ymin))) {
						result = true;
						f22 = f0;
						spb0 = i;
					}
				}
			}
		}

		if (result) {
			arg4->x = arg0->x + arg2->f[0] * f22;
			arg4->y = arg0->y + arg2->f[1] * f22;
			arg4->z = arg0->z + arg2->f[2] * f22;

			if (arg5 != NULL && arg6 != NULL) {
				arg5->x = tile->vertices[spb0][0];
				arg5->y = arg4->y;
				arg5->z = tile->vertices[spb0][2];

				arg6->x = tile->vertices[(spb0 + 1) % numvertices][0];
				arg6->y = arg4->y;
				arg6->z = tile->vertices[(spb0 + 1) % numvertices][2];
			}
		} else if (!result && spb8) {
			result = true;

			arg4->x = arg0->x;
			arg4->y = arg0->y;
			arg4->z = arg0->z;

			if (arg5 != NULL && arg6 != NULL) {
				arg5->x = arg0->x;
				arg5->y = arg0->y;
				arg5->z = arg0->z;

				arg6->x = arg0->x;
				arg6->y = arg0->y;
				arg6->z = arg0->z;
			}
		}
	}

	return result;
}

bool cd0002b128(struct coord *arg0, struct coord *arg1, struct coord *arg2, struct geotilef *tile,
		struct coord *arg4, struct coord *arg5, struct coord *arg6, bool arg7, f32 arg8, f32 arg9)
{
	bool result = false;
	s32 i;
	f32 f0;
	s32 numvertices = tile->header.numvertices;
	s32 next;
	s32 spb8 = 1;
	f32 f22 = 1.0f;
	s32 spb0;
	f32 spac;
	f32 spa8;
	f32 ymax = tile->vertices[tile->ymax].y;
	f32 ymin = tile->vertices[tile->ymin].y;
	f32 spa0[2];
	f32 sp98[2];
	f32 sp90[2];
	f32 sp88[2];

	if (!arg7
			|| (arg0->y + arg8 >= ymin && arg1->y + arg9 <= ymax)
			|| (arg0->y + arg9 <= ymax && arg1->y + arg8 >= ymin)) {
		for (i = 0; i < numvertices; i++) {
			next = (i + 1) % numvertices;

			if (cd000254d8(arg0, arg1, tile->vertices[i].x, tile->vertices[i].z, tile->vertices[next].x, tile->vertices[next].z, &spb8)) {
				spa0[0] = arg0->x;
				spa0[1] = arg0->z;
				sp98[0] = arg1->x;
				sp98[1] = arg1->z;
				sp90[0] = tile->vertices[i].x;
				sp90[1] = tile->vertices[i].z;
				sp88[0] = tile->vertices[next].x;
				sp88[1] = tile->vertices[next].z;

				f0 = func0f1577f0(spa0, sp98, sp90, sp88);

				if (f0 < f22) {
					if (arg7) {
						spa8 = (arg1->y - arg0->y) * f0 + arg0->y;
						spac = spa8 + arg8;
						spa8 = spa8 + arg9;
					}

					if (!arg7 || (!(spa8 >= ymax) && !(spac <= ymin))) {
						result = true;
						f22 = f0;
						spb0 = i;
					}
				}
			}
		}

		if (result) {
			arg4->x = arg0->x + arg2->f[0] * f22;
			arg4->y = arg0->y + arg2->f[1] * f22;
			arg4->z = arg0->z + arg2->f[2] * f22;

			if (arg5 != NULL && arg6 != NULL) {
				arg5->x = tile->vertices[spb0].x;
				arg5->y = arg4->y;
				arg5->z = tile->vertices[spb0].z;

				arg6->x = tile->vertices[(spb0 + 1) % numvertices].x;
				arg6->y = arg4->y;
				arg6->z = tile->vertices[(spb0 + 1) % numvertices].z;
			}
		} else if (!result && spb8) {
			result = true;

			arg4->x = arg0->x;
			arg4->y = arg0->y;
			arg4->z = arg0->z;

			if (arg5 != NULL && arg6 != NULL) {
				arg5->x = arg0->x;
				arg5->y = arg0->y;
				arg5->z = arg0->z;

				arg6->x = arg0->x;
				arg6->y = arg0->y;
				arg6->z = arg0->z;
			}
		}
	}

	return result;
}

bool cd0002b560(struct coord *arg0, struct coord *arg1, struct coord *arg2, struct geoblock *block,
		struct coord *arg4, struct coord *arg5, struct coord *arg6, bool arg7, f32 arg8, f32 arg9)
{
	bool result = false;
	s32 i;
	f32 f0;
	s32 numvertices = block->header.numvertices;
	s32 next;
	s32 spb8 = 1;
	f32 f22 = 1.0f;
	s32 spb0;
	f32 spac;
	f32 spa8;
	f32 spa0[2];
	f32 sp98[2];
	f32 sp90[2];
	f32 sp88[2];

	if (!arg7
			|| (arg0->y + arg8 >= block->ymin && arg1->y + arg9 <= block->ymax)
			|| (arg0->y + arg9 <= block->ymax && arg1->y + arg8 >= block->ymin)) {
		for (i = 0; i < numvertices; i++) {
			next = (i + 1) % numvertices;

			if (cd000254d8(arg0, arg1, block->vertices[i][0], block->vertices[i][1], block->vertices[next][0], block->vertices[next][1], &spb8)) {
				spa0[0] = arg0->x;
				spa0[1] = arg0->z;
				sp98[0] = arg1->x;
				sp98[1] = arg1->z;
				sp90[0] = block->vertices[i][0];
				sp90[1] = block->vertices[i][1];
				sp88[0] = block->vertices[next][0];
				sp88[1] = block->vertices[next][1];

				f0 = func0f1577f0(spa0, sp98, sp90, sp88);

				if (f0 < f22) {
					if (arg7) {
						spa8 = (arg1->y - arg0->y) * f0 + arg0->y;
						spac = spa8 + arg8;
						spa8 = spa8 + arg9;
					}

					if (!arg7 || (!(spa8 >= block->ymax) && !(spac <= block->ymin))) {
						result = true;
						f22 = f0;
						spb0 = i;
					}
				}
			}
		}

		if (result) {
			arg4->x = arg0->x + arg2->f[0] * f22;
			arg4->y = arg0->y + arg2->f[1] * f22;
			arg4->z = arg0->z + arg2->f[2] * f22;

			if (arg5 != NULL && arg6 != NULL) {
				arg5->x = block->vertices[spb0][0];
				arg5->y = arg4->y;
				arg5->z = block->vertices[spb0][1];

				arg6->x = block->vertices[(spb0 + 1) % numvertices][0];
				arg6->y = arg4->y;
				arg6->z = block->vertices[(spb0 + 1) % numvertices][1];
			}
		} else if (!result && spb8) {
			result = true;

			arg4->x = arg0->x;
			arg4->y = arg0->y;
			arg4->z = arg0->z;

			if (arg5 != NULL && arg6 != NULL) {
				arg5->x = arg0->x;
				arg5->y = arg0->y;
				arg5->z = arg0->z;

				arg6->x = arg0->x;
				arg6->y = arg0->y;
				arg6->z = arg0->z;
			}
		}
	}

	return result;
}

bool cd0002b954(struct coord *arg0, struct coord *arg1, struct coord *arg2, struct geocyl *cyl,
		struct coord *arg4, struct coord *arg5, struct coord *arg6, bool arg7, f32 arg8, f32 arg9)
{
	bool result = false;
	f32 mult;
	f32 sp74;
	f32 x = cyl->x;
	f32 z = cyl->z;
	f32 radius = cyl->radius;

	if (!arg7
			|| (arg0->y + arg8 >= cyl->ymin && arg1->y + arg9 <= cyl->ymax)
			|| (arg0->y + arg9 <= cyl->ymax && arg1->y + arg8 >= cyl->ymin)) {
		sp74 = cd00025654(arg0->x, arg0->z, arg1->x, arg1->z, x, z);

		if (sp74 < 0.0f) {
			sp74 = -sp74;
		}

		if (sp74 < radius
				&& (cd00025724(arg0->x, arg0->z, x, z) < radius
					|| cd00025724(arg1->x, arg1->z, x, z) < radius
					|| cd00025774(arg0->x, arg0->z, arg1->x, arg1->z, x, z))) {
			f32 xdiff = arg1->x - arg0->x;
			f32 zdiff = arg1->z - arg0->z;
			f32 sqdist;
			f32 dist;
			u32 stack;
			f32 sp50;
			f32 sp4c;
			f32 sp48;

			sp50 = sqrtf(xdiff * xdiff + zdiff * zdiff);

			if (sp50 > 0.0f) {
				xdiff = x - arg0->x;
				zdiff = z - arg0->z;

				sqdist = xdiff * xdiff + zdiff * zdiff;

				if (sp74 * sp74 <= sqdist) {
					dist = sqrtf(sqdist - sp74 * sp74) - sqrtf(radius * radius - sp74 * sp74);
				} else {
					dist = 0.0f;
				}

				mult = dist / sp50;
			} else {
				mult = 0.0f;
			}

			if (mult < 1.0f) {
				if (arg7) {
					sp48 = (arg1->y - arg0->y) * mult + arg0->y;
					sp4c = sp48 + arg8;
					sp48 = sp48 + arg9;
				}

				if (!arg7 || (!(sp48 >= cyl->ymax) && !(sp4c <= cyl->ymin))) {
					result = true;

					arg4->x = arg0->x + arg2->f[0] * mult;
					arg4->y = arg0->y + arg2->f[1] * mult;
					arg4->z = arg0->z + arg2->f[2] * mult;

					if (arg5 != NULL && arg6 != NULL) {
						cd00025848(x, z, radius, arg0->x, arg0->z, &arg5->x, &arg5->z, &arg6->x, &arg6->z);

						arg5->y = arg4->y;
						arg6->y = arg4->y;
					}
				}
			}
		}
	}

	return result;
}

bool cd0002bd04(u8 *start, u8 *end, struct coord *arg2, struct coord *arg3, struct coord *arg4, u16 flags, bool arg6, s32 arg7, f32 arg8, f32 arg9)
{
	struct geo *geo = (struct geo *) start;

	while (geo < (struct geo *) end) {
		if (geo->type == GEOTYPE_TILE_I) {
			struct geotilei *tile = (struct geotilei *) geo;
			struct coord min;
			struct coord max;
			struct coord spc4;
			struct coord spb8;

			if (tile->header.flags & flags) {
				min.x = *(s16 *)(tile->xmin + (u32)tile);

				if ((!(arg2->x < min.x)) || !(arg3->x < min.x)) {
					max.x = *(s16 *)(tile->xmax + (u32)tile);

					if ((!(arg2->x > max.x)) || !(arg3->x > max.x)) {
						min.z = *(s16 *)(tile->zmin + (u32)tile);

						if ((!(arg2->z < min.z)) || !(arg3->z < min.z)) {
							max.z = *(s16 *)(tile->zmax + (u32)tile);

							if ((!(arg2->z > max.z)) || !(arg3->z > max.z)) {
								if (1);
								if (arg6) {
									min.y = *(s16 *)(tile->ymin + (u32)tile);
									max.y = *(s16 *)(tile->ymax + (u32)tile);

									if ((!(arg2->y < min.y) || !(arg3->y < min.y))
											&& (!(arg2->y > max.y) || !(arg3->y > max.y))
											&& bgTestLineIntersectsBbox(arg2, arg4, &min, &max)
											&& cd0002aac0(arg2, arg3, arg4, tile, &spc4, &spb8)) {
										return false;
									}
								} else if (cd0002ac70(arg2, arg3, arg4, tile, &spc4, 0, 0, arg7, arg8, arg9)) {
									return false;
								}
							}
						}
					}
				}
			}

			geo = (struct geo *)((u32)geo + tile->header.numvertices * 6 + 0xe);
		} else if (geo->type == GEOTYPE_TILE_F) {
			struct geotilef *tile = (struct geotilef *) geo;
			struct coord min;
			struct coord max;
			struct coord sp90;
			struct coord sp84;

			if (tile->header.flags & flags) {
				min.x = tile->vertices[tile->xmin].x;
				max.x = tile->vertices[tile->xmax].x;
				min.z = tile->vertices[tile->zmin].z;
				max.z = tile->vertices[tile->zmax].z;

				if (((!(arg2->x < min.x)) || !(arg3->x < min.x))
						&& (!(arg2->x > max.x) || !(arg3->x > max.x))
						&& ((!(arg2->z < min.z)) || !(arg3->z < min.z))
						&& (!(arg2->z > max.z) || !(arg3->z > max.z))) {
					if (arg6) {
						min.y = tile->vertices[tile->ymin].y;
						max.y = tile->vertices[tile->ymax].y;

						if ((!(arg2->y < min.y) || !(arg3->y < min.y))
								&& (!(arg2->y > max.y) || !(arg3->y > max.y))
								&& bgTestLineIntersectsBbox(arg2, arg4, &min, &max)
								&& cd0002ab98(arg2, arg3, arg4, tile, &sp90, &sp84)) {
							return false;
						}
					} else if (cd0002b128(arg2, arg3, arg4, tile, &sp90, 0, 0, arg7, arg8, arg9)) {
						return false;
					}
				}
			}

			geo = (struct geo *)((u32)geo + (u32)(tile->header.numvertices - 0x40) * 0xc + 0x310);
		} else if (geo->type == GEOTYPE_BLOCK) {
			struct coord sp78;
			struct geoblock *block = (struct geoblock *) geo;

			if ((flags & (GEOFLAG_COLLISIONS | GEOFLAG_0008 | GEOFLAG_OPAQUE))
					&& cd0002b560(arg2, arg3, arg4, block, &sp78, 0, 0, arg7, arg8, arg9)) {
				return false;
			}

			geo = (struct geo *)((u32)geo + sizeof(struct geoblock));
		} else if (geo->type == GEOTYPE_CYL) {
			struct coord sp68;
			struct geocyl *cyl = (struct geocyl *) geo;

			if ((flags & cyl->header.flags)
					&& cd0002b954(arg2, arg3, arg4, cyl, &sp68, 0, 0, arg7, arg8, arg9)) {
				return false;
			}

			geo = (struct geo *)((u32)geo + sizeof(struct geocyl));
		}
	}

	return true;
}

void cd0002c328(struct geotilei *tile, struct coord *arg1, struct coord *arg2, struct coord *arg3, struct coord *arg4)
{
	struct coord sp3c;
	u32 stack[2];
	u8 numvertices;
	f32 max = 0.0f;
	f32 min = 0.0f;
	f32 dist;
	s32 i;

	if (arg2->x != 0.0f || arg2->z != 0.0f) {
		sp3c.x = arg2->z;
		sp3c.y = 0.0f;
		sp3c.z = -arg2->x;

		dist = sqrtf(sp3c.f[0] * sp3c.f[0] + sp3c.f[2] * sp3c.f[2]);

		if (dist > 0.0f) {
			sp3c.x *= 1.0f / dist;
			sp3c.z *= 1.0f / dist;
		} else {
			sp3c.x = 0.0f;
			sp3c.y = 0.0f;
			sp3c.z = 1.0f;
		}
	} else {
		sp3c.x = 0.0f;
		sp3c.y = 0.0f;
		sp3c.z = 1.0f;
	}

	numvertices = tile->header.numvertices;

	for (i = 0; i < numvertices; i++) {
		f32 xdiff = tile->vertices[i][0] - arg1->x;
		f32 zdiff = tile->vertices[i][2] - arg1->z;
		f32 f0 = xdiff * sp3c.f[0] + zdiff * sp3c.f[2];

		if (f0 > max) {
			max = f0;
		} else if (f0 < min) {
			min = f0;
		}
	}

	arg3->x = arg1->x + sp3c.f[0] * max;
	arg3->y = arg1->y;
	arg3->z = arg1->z + sp3c.f[2] * max;

	arg4->x = arg1->x + sp3c.f[0] * min;
	arg4->y = arg1->y;
	arg4->z = arg1->z + sp3c.f[2] * min;
}

void cd0002c528(struct geotilef *tile, struct coord *arg1, struct coord *arg2, struct coord *arg3, struct coord *arg4)
{
	struct coord sp3c;
	u32 stack[2];
	u8 numvertices;
	f32 max = 0.0f;
	f32 min = 0.0f;
	f32 dist;
	s32 i;

	if (arg2->x != 0.0f || arg2->z != 0.0f) {
		sp3c.x = arg2->z;
		sp3c.y = 0.0f;
		sp3c.z = -arg2->x;

		dist = sqrtf(sp3c.f[0] * sp3c.f[0] + sp3c.f[2] * sp3c.f[2]);

		if (dist > 0.0f) {
			sp3c.x *= 1.0f / dist;
			sp3c.z *= 1.0f / dist;
		} else {
			sp3c.x = 0.0f;
			sp3c.y = 0.0f;
			sp3c.z = 1.0f;
		}
	} else {
		sp3c.x = 0.0f;
		sp3c.y = 0.0f;
		sp3c.z = 1.0f;
	}

	numvertices = tile->header.numvertices;

	for (i = 0; i < numvertices; i++) {
		f32 xdiff = tile->vertices[i].x - arg1->x;
		f32 zdiff = tile->vertices[i].z - arg1->z;
		f32 f0 = xdiff * sp3c.f[0] + zdiff * sp3c.f[2];

		if (f0 > max) {
			max = f0;
		} else if (f0 < min) {
			min = f0;
		}
	}

	arg3->x = arg1->x + sp3c.f[0] * max;
	arg3->y = arg1->y;
	arg3->z = arg1->z + sp3c.f[2] * max;

	arg4->x = arg1->x + sp3c.f[0] * min;
	arg4->y = arg1->y;
	arg4->z = arg1->z + sp3c.f[2] * min;
}

bool cd0002c714(u8 *start, u8 *end, struct coord *arg2, struct coord *arg3, struct coord *arg4,
		u16 flags, bool arg6, s32 arg7, f32 ymax, f32 ymin, f32 *arg10, struct coord *arg11,
		struct coord *arg12, struct coord *arg13, struct geo **geoptr, s32 roomnum)
{
	struct geo *geo;
	f32 x;
	f32 y;
	f32 z;
	f32 sum;
	bool ok;
	bool result = false;

	geo = (struct geo *) start;

	while (geo < (struct geo *) end) {
		if (geo->type == GEOTYPE_TILE_I) {
			struct geotilei *tile = (struct geotilei *) geo;
			struct coord min;
			struct coord max;
			struct coord sp12c;
			struct coord sp120;
			struct coord sp114;
			struct coord sp108;

			if (geo->flags & GEOFLAG_0080) {
				ok = cd00028200(tile, arg2, 0, arg2->y + ymin, arg2->y + ymax);
			} else {
				ok = true;
			}

			if (ok && (geo->flags & flags)) {
				min.x = *(s16 *)(tile->xmin + (u32)tile);

				if (!(arg2->x < min.x) || !(arg3->x < min.x)) {
					max.x = *(s16 *)(tile->xmax + (u32)tile);

					if (!(arg2->x > max.x) || !(arg3->x > max.x)) {
						min.z = *(s16 *)(tile->zmin + (u32)tile);

						if (!(arg2->z < min.z) || !(arg3->z < min.z)) {
							max.z = *(s16 *)(tile->zmax + (u32)tile);

							if (!(arg2->z > max.z) || !(arg3->z > max.z)) {
								if (arg6) {
									min.y = *(s16 *)(tile->ymin + (u32)tile);
									max.y = *(s16 *)(tile->ymax + (u32)tile);

									if ((!(arg2->y < min.y) || !(arg3->y < min.y))
											&& (!(arg2->y > max.y) || !(arg3->y > max.y))
											&& bgTestLineIntersectsBbox(arg2, arg4, &min, &max)
											&& cd0002aac0(arg2, arg3, arg4, tile, &sp12c, &sp120)) {
										x = sp12c.x - arg2->x;
										y = sp12c.y - arg2->y;
										z = sp12c.z - arg2->z;

										sum = x * x + y * y + z * z;

										if (sum < *arg10) {
											result = true;
											*arg10 = sum;

											arg11->x = sp12c.x;
											arg11->y = sp12c.y;
											arg11->z = sp12c.z;

											cd0002c328(tile, &sp12c, &sp120, arg12, arg13);

											*geoptr = geo;
										}
									}
								} else if (cd0002ac70(arg2, arg3, arg4, tile, &sp12c, &sp114, &sp108, arg7, ymax, ymin)) {
									x = sp12c.x - arg2->x;
									y = sp12c.y - arg2->y;
									z = sp12c.z - arg2->z;

									sum = x * x + y * y + z * z;

									if (sum < *arg10) {
										result = true;
										*arg10 = sum;

										arg11->x = sp12c.x;
										arg11->y = sp12c.y;
										arg11->z = sp12c.z;

										arg12->x = sp114.x;
										arg12->y = sp114.y;
										arg12->z = sp114.z;

										arg13->x = sp108.x;
										arg13->y = sp108.y;
										arg13->z = sp108.z;

										*geoptr = geo;
									}
								}
							}
						}
					}
				}
			}

			geo = (struct geo *)((u32)geo + (u32)(tile->header.numvertices * 6) + 0xe);
		} else if (geo->type == GEOTYPE_TILE_F) {
			struct geotilef *tile = (struct geotilef *) geo;
			struct coord min;
			struct coord max;
			struct coord spe0;
			struct coord spd4;
			struct coord spc8;
			struct coord spbc;

			if (geo->flags & flags) {
				min.x = tile->vertices[tile->xmin].x;
				max.x = tile->vertices[tile->xmax].x;
				min.z = tile->vertices[tile->zmin].z;
				max.z = tile->vertices[tile->zmax].z;

				if ((!(arg2->x < min.x) || !(arg3->x < min.x))
						&& (!(arg2->x > max.x) || !(arg3->x > max.x))
						&& (!(arg2->z < min.z) || !(arg3->z < min.z))
						&& (!(arg2->z > max.z) || !(arg3->z > max.z))) {
					if (arg6) {
						min.y = tile->vertices[tile->ymin].y;
						max.y = tile->vertices[tile->ymax].y;

						if ((!(arg2->y < min.y) || !(arg3->y < min.y))
								&& (!(arg2->y > max.y) || !(arg3->y > max.y))
								&& bgTestLineIntersectsBbox(arg2, arg4, &min, &max)
								&& cd0002ab98(arg2, arg3, arg4, tile, &spe0, &spd4)) {
							x = spe0.x - arg2->x;
							y = spe0.y - arg2->y;
							z = spe0.z - arg2->z;

							sum = x * x + y * y + z * z;

							if (sum < *arg10) {
								result = true;
								*arg10 = sum;

								arg11->x = spe0.x;
								arg11->y = spe0.y;
								arg11->z = spe0.z;

								cd0002c528(tile, &spe0, &spd4, arg12, arg13);

								*geoptr = geo;
							}
						}
					} else if (cd0002b128(arg2, arg3, arg4, tile, &spe0, &spc8, &spbc, arg7, ymax, ymin)) {
						x = spe0.x - arg2->x;
						y = spe0.y - arg2->y;
						z = spe0.z - arg2->z;

						sum = x * x + y * y + z * z;

						if (sum < *arg10) {
							result = true;
							*arg10 = sum;

							arg11->x = spe0.x;
							arg11->y = spe0.y;
							arg11->z = spe0.z;

							arg12->x = spc8.x;
							arg12->y = spc8.y;
							arg12->z = spc8.z;

							arg13->x = spbc.x;
							arg13->y = spbc.y;
							arg13->z = spbc.z;

							*geoptr = geo;
						}
					}
				}
			}

			geo = (struct geo *)((u32)geo + (u32)(tile->header.numvertices - 0x40) * 0xc + 0x310);
		} else if (geo->type == GEOTYPE_BLOCK) {
			struct coord spb0;
			struct coord spa4;
			struct coord sp98;

			if ((flags & (GEOFLAG_COLLISIONS | GEOFLAG_0008 | GEOFLAG_OPAQUE))
					&& (cd0002b560(arg2, arg3, arg4, (struct geoblock *)geo, &spb0, &spa4, &sp98, arg7, ymax, ymin))) {
				x = spb0.x - arg2->x;
				y = spb0.y - arg2->y;
				z = spb0.z - arg2->z;

				sum = x * x + y * y + z * z;

				if (sum < *arg10) {
					result = true;
					*arg10 = sum;

					arg11->x = spb0.x;
					arg11->y = spb0.y;
					arg11->z = spb0.z;

					arg12->x = spa4.x;
					arg12->y = spa4.y;
					arg12->z = spa4.z;

					arg13->x = sp98.x;
					arg13->y = sp98.y;
					arg13->z = sp98.z;

					*geoptr = geo;
				}
			}

			geo = (struct geo *)((u32)geo + sizeof(struct geoblock));
		} else if (geo->type == GEOTYPE_CYL) {
			struct geocyl *cyl = (struct geocyl *) geo;
			struct coord sp88;
			struct coord sp7c;
			struct coord sp70;

			if ((flags & geo->flags)
					&& cd0002b954(arg2, arg3, arg4, cyl, &sp88, &sp7c, &sp70, arg7, ymax, ymin)) {
				x = sp88.x - arg2->x;
				y = sp88.y - arg2->y;
				z = sp88.z - arg2->z;

				sum = x * x + y * y + z * z;

				if (sum < *arg10) {
					result = true;
					*arg10 = sum;

					arg11->x = sp88.x;
					arg11->y = sp88.y;
					arg11->z = sp88.z;

					arg12->x = sp7c.x;
					arg12->y = sp7c.y;
					arg12->z = sp7c.z;

					arg13->x = sp70.x;
					arg13->y = sp70.y;
					arg13->z = sp70.z;

					*geoptr = geo;
				}
			}

			geo = (struct geo *)((u32)geo + sizeof(struct geocyl));
		}
	}

	return !result;
}

bool cd0002d15c(struct coord *pos, struct coord *coord2, s16 *rooms, u32 types, u16 arg4, s32 arg5, s32 arg6, f32 arg7, f32 ymax)
{
	s32 roomnum;
	s16 *roomptr;
	u8 *start;
	u8 *end;
	struct coord sp27c;
	s16 *propnumptr;
	s16 propnums[256];

	sp27c.x = coord2->x - pos->x;
	sp27c.y = coord2->y - pos->y;
	sp27c.z = coord2->z - pos->z;

	if (types & CDTYPE_BG) {
		roomptr = rooms;
		roomnum = rooms[0];

		while (roomnum != -1) {
			if (roomnum < g_TileNumRooms) {
				start = g_TileFileData.u8 + g_TileRooms[roomnum];
				end = g_TileFileData.u8 + g_TileRooms[roomnum + 1];

				if (cd0002bd04(start, end, pos, coord2, &sp27c, arg4, arg5, arg6, arg7, ymax) == 0) {
					cd00025168(NULL);
					return false;
				}
			}

			roomptr++;
			roomnum = *roomptr;
		}
	}

	roomGetProps(rooms, propnums, 256);

	propnumptr = propnums;

	while (*propnumptr >= 0) {
		struct prop *prop = &g_Vars.props[*propnumptr];

		if (propIsOfCdType(prop, types)
				&& propUpdateGeometry(prop, &start, &end)
				&& cd0002bd04(start, end, pos, coord2, &sp27c, arg4, arg5, arg6, arg7, ymax) == 0) {
			cd00025168(prop);
			return false;
		}

		propnumptr++;
	}

	return true;
}

GLOBAL_ASM(
glabel cd0002d3b0
/*    2d3b0:	27bdfd20 */ 	addiu	$sp,$sp,-736
/*    2d3b4:	3c014f80 */ 	lui	$at,0x4f80
/*    2d3b8:	44812000 */ 	mtc1	$at,$f4
/*    2d3bc:	afbf007c */ 	sw	$ra,0x7c($sp)
/*    2d3c0:	afbe0078 */ 	sw	$s8,0x78($sp)
/*    2d3c4:	afb70074 */ 	sw	$s7,0x74($sp)
/*    2d3c8:	afb60070 */ 	sw	$s6,0x70($sp)
/*    2d3cc:	afb5006c */ 	sw	$s5,0x6c($sp)
/*    2d3d0:	afb40068 */ 	sw	$s4,0x68($sp)
/*    2d3d4:	afb30064 */ 	sw	$s3,0x64($sp)
/*    2d3d8:	afb20060 */ 	sw	$s2,0x60($sp)
/*    2d3dc:	afb1005c */ 	sw	$s1,0x5c($sp)
/*    2d3e0:	afb00058 */ 	sw	$s0,0x58($sp)
/*    2d3e4:	f7b60050 */ 	sdc1	$f22,0x50($sp)
/*    2d3e8:	f7b40048 */ 	sdc1	$f20,0x48($sp)
/*    2d3ec:	afa402e0 */ 	sw	$a0,0x2e0($sp)
/*    2d3f0:	afa502e4 */ 	sw	$a1,0x2e4($sp)
/*    2d3f4:	afa702ec */ 	sw	$a3,0x2ec($sp)
/*    2d3f8:	afa002c0 */ 	sw	$zero,0x2c0($sp)
/*    2d3fc:	e7a40298 */ 	swc1	$f4,0x298($sp)
/*    2d400:	c4880000 */ 	lwc1	$f8,0x0($a0)
/*    2d404:	c4a60000 */ 	lwc1	$f6,0x0($a1)
/*    2d408:	30f90020 */ 	andi	$t9,$a3,0x20
/*    2d40c:	00c0f025 */ 	or	$s8,$a2,$zero
/*    2d410:	46083281 */ 	sub.s	$f10,$f6,$f8
/*    2d414:	e7aa02c4 */ 	swc1	$f10,0x2c4($sp)
/*    2d418:	c4920004 */ 	lwc1	$f18,0x4($a0)
/*    2d41c:	c4b00004 */ 	lwc1	$f16,0x4($a1)
/*    2d420:	46128101 */ 	sub.s	$f4,$f16,$f18
/*    2d424:	e7a402c8 */ 	swc1	$f4,0x2c8($sp)
/*    2d428:	c4880008 */ 	lwc1	$f8,0x8($a0)
/*    2d42c:	c4a60008 */ 	lwc1	$f6,0x8($a1)
/*    2d430:	46083281 */ 	sub.s	$f10,$f6,$f8
/*    2d434:	13200043 */ 	beqz	$t9,.L0002d544
/*    2d438:	e7aa02cc */ 	swc1	$f10,0x2cc($sp)
/*    2d43c:	84c80000 */ 	lh	$t0,0x0($a2)
/*    2d440:	2411ffff */ 	addiu	$s1,$zero,-1
/*    2d444:	00c08025 */ 	or	$s0,$a2,$zero
/*    2d448:	1111003e */ 	beq	$t0,$s1,.L0002d544
/*    2d44c:	c7b60300 */ 	lwc1	$f22,0x300($sp)
/*    2d450:	3c17800a */ 	lui	$s7,%hi(g_TileFileData)
/*    2d454:	3c16800a */ 	lui	$s6,%hi(g_TileRooms)
/*    2d458:	3c15800a */ 	lui	$s5,%hi(g_TileNumRooms)
/*    2d45c:	26b5a8a4 */ 	addiu	$s5,$s5,%lo(g_TileNumRooms)
/*    2d460:	26d6a8a8 */ 	addiu	$s6,$s6,%lo(g_TileRooms)
/*    2d464:	26f7a8a0 */ 	addiu	$s7,$s7,%lo(g_TileFileData)
/*    2d468:	c7b402fc */ 	lwc1	$f20,0x2fc($sp)
/*    2d46c:	27b4029c */ 	addiu	$s4,$sp,0x29c
/*    2d470:	27b302a8 */ 	addiu	$s3,$sp,0x2a8
/*    2d474:	27b202b4 */ 	addiu	$s2,$sp,0x2b4
/*    2d478:	8ea90000 */ 	lw	$t1,0x0($s5)
.L0002d47c:
/*    2d47c:	00085080 */ 	sll	$t2,$t0,0x2
/*    2d480:	27b802c4 */ 	addiu	$t8,$sp,0x2c4
/*    2d484:	0109082a */ 	slt	$at,$t0,$t1
/*    2d488:	5020002b */ 	beqzl	$at,.L0002d538
/*    2d48c:	86080002 */ 	lh	$t0,0x2($s0)
/*    2d490:	8ecb0000 */ 	lw	$t3,0x0($s6)
/*    2d494:	8ee30000 */ 	lw	$v1,0x0($s7)
/*    2d498:	97b902f2 */ 	lhu	$t9,0x2f2($sp)
/*    2d49c:	014b1021 */ 	addu	$v0,$t2,$t3
/*    2d4a0:	8c4c0000 */ 	lw	$t4,0x0($v0)
/*    2d4a4:	8faa02f8 */ 	lw	$t2,0x2f8($sp)
/*    2d4a8:	8fa902f4 */ 	lw	$t1,0x2f4($sp)
/*    2d4ac:	01832021 */ 	addu	$a0,$t4,$v1
/*    2d4b0:	afa402d4 */ 	sw	$a0,0x2d4($sp)
/*    2d4b4:	8c4e0004 */ 	lw	$t6,0x4($v0)
/*    2d4b8:	27ac0294 */ 	addiu	$t4,$sp,0x294
/*    2d4bc:	27ab0298 */ 	addiu	$t3,$sp,0x298
/*    2d4c0:	01c32821 */ 	addu	$a1,$t6,$v1
/*    2d4c4:	afa502d0 */ 	sw	$a1,0x2d0($sp)
/*    2d4c8:	afab0028 */ 	sw	$t3,0x28($sp)
/*    2d4cc:	afac0038 */ 	sw	$t4,0x38($sp)
/*    2d4d0:	afa8003c */ 	sw	$t0,0x3c($sp)
/*    2d4d4:	afb40034 */ 	sw	$s4,0x34($sp)
/*    2d4d8:	afb30030 */ 	sw	$s3,0x30($sp)
/*    2d4dc:	afb2002c */ 	sw	$s2,0x2c($sp)
/*    2d4e0:	e7b60024 */ 	swc1	$f22,0x24($sp)
/*    2d4e4:	e7b40020 */ 	swc1	$f20,0x20($sp)
/*    2d4e8:	afb80010 */ 	sw	$t8,0x10($sp)
/*    2d4ec:	8fa602e0 */ 	lw	$a2,0x2e0($sp)
/*    2d4f0:	8fa702e4 */ 	lw	$a3,0x2e4($sp)
/*    2d4f4:	afb90014 */ 	sw	$t9,0x14($sp)
/*    2d4f8:	afaa001c */ 	sw	$t2,0x1c($sp)
/*    2d4fc:	0c00b1c5 */ 	jal	cd0002c714
/*    2d500:	afa90018 */ 	sw	$t1,0x18($sp)
/*    2d504:	1440000b */ 	bnez	$v0,.L0002d534
/*    2d508:	240d0001 */ 	addiu	$t5,$zero,0x1
/*    2d50c:	c7b00298 */ 	lwc1	$f16,0x298($sp)
/*    2d510:	8fae0294 */ 	lw	$t6,0x294($sp)
/*    2d514:	afad02c0 */ 	sw	$t5,0x2c0($sp)
/*    2d518:	02602025 */ 	or	$a0,$s3,$zero
/*    2d51c:	02802825 */ 	or	$a1,$s4,$zero
/*    2d520:	02403025 */ 	or	$a2,$s2,$zero
/*    2d524:	00003825 */ 	or	$a3,$zero,$zero
/*    2d528:	e7b00010 */ 	swc1	$f16,0x10($sp)
/*    2d52c:	0c009495 */ 	jal	cd00025254
/*    2d530:	afae0014 */ 	sw	$t6,0x14($sp)
.L0002d534:
/*    2d534:	86080002 */ 	lh	$t0,0x2($s0)
.L0002d538:
/*    2d538:	26100002 */ 	addiu	$s0,$s0,0x2
/*    2d53c:	5511ffcf */ 	bnel	$t0,$s1,.L0002d47c
/*    2d540:	8ea90000 */ 	lw	$t1,0x0($s5)
.L0002d544:
/*    2d544:	27b00090 */ 	addiu	$s0,$sp,0x90
/*    2d548:	27b202b4 */ 	addiu	$s2,$sp,0x2b4
/*    2d54c:	27b302a8 */ 	addiu	$s3,$sp,0x2a8
/*    2d550:	27b4029c */ 	addiu	$s4,$sp,0x29c
/*    2d554:	c7b402fc */ 	lwc1	$f20,0x2fc($sp)
/*    2d558:	c7b60300 */ 	lwc1	$f22,0x300($sp)
/*    2d55c:	02002825 */ 	or	$a1,$s0,$zero
/*    2d560:	03c02025 */ 	or	$a0,$s8,$zero
/*    2d564:	0fc197e0 */ 	jal	roomGetProps
/*    2d568:	24060100 */ 	addiu	$a2,$zero,0x100
/*    2d56c:	87af0090 */ 	lh	$t7,0x90($sp)
/*    2d570:	02008825 */ 	or	$s1,$s0,$zero
/*    2d574:	27b80090 */ 	addiu	$t8,$sp,0x90
/*    2d578:	05e0003b */ 	bltz	$t7,.L0002d668
/*    2d57c:	27be02d0 */ 	addiu	$s8,$sp,0x2d0
/*    2d580:	3c16800a */ 	lui	$s6,%hi(g_Vars)
/*    2d584:	26d69fc0 */ 	addiu	$s6,$s6,%lo(g_Vars)
/*    2d588:	87020000 */ 	lh	$v0,0x0($t8)
/*    2d58c:	27b702d4 */ 	addiu	$s7,$sp,0x2d4
/*    2d590:	24150048 */ 	addiu	$s5,$zero,0x48
.L0002d594:
/*    2d594:	00550019 */ 	multu	$v0,$s5
/*    2d598:	8ec90338 */ 	lw	$t1,0x338($s6)
/*    2d59c:	8fa502ec */ 	lw	$a1,0x2ec($sp)
/*    2d5a0:	0000c812 */ 	mflo	$t9
/*    2d5a4:	03298021 */ 	addu	$s0,$t9,$t1
/*    2d5a8:	0fc19553 */ 	jal	propIsOfCdType
/*    2d5ac:	02002025 */ 	or	$a0,$s0,$zero
/*    2d5b0:	10400029 */ 	beqz	$v0,.L0002d658
/*    2d5b4:	02002025 */ 	or	$a0,$s0,$zero
/*    2d5b8:	02e02825 */ 	or	$a1,$s7,$zero
/*    2d5bc:	0fc198a4 */ 	jal	propUpdateGeometry
/*    2d5c0:	03c03025 */ 	or	$a2,$s8,$zero
/*    2d5c4:	10400024 */ 	beqz	$v0,.L0002d658
/*    2d5c8:	8fa402d4 */ 	lw	$a0,0x2d4($sp)
/*    2d5cc:	97ab02f2 */ 	lhu	$t3,0x2f2($sp)
/*    2d5d0:	8fac02f4 */ 	lw	$t4,0x2f4($sp)
/*    2d5d4:	8fad02f8 */ 	lw	$t5,0x2f8($sp)
/*    2d5d8:	27aa02c4 */ 	addiu	$t2,$sp,0x2c4
/*    2d5dc:	27ae0298 */ 	addiu	$t6,$sp,0x298
/*    2d5e0:	27af0294 */ 	addiu	$t7,$sp,0x294
/*    2d5e4:	2418fc19 */ 	addiu	$t8,$zero,-999
/*    2d5e8:	afb8003c */ 	sw	$t8,0x3c($sp)
/*    2d5ec:	afaf0038 */ 	sw	$t7,0x38($sp)
/*    2d5f0:	afae0028 */ 	sw	$t6,0x28($sp)
/*    2d5f4:	afaa0010 */ 	sw	$t2,0x10($sp)
/*    2d5f8:	8fa502d0 */ 	lw	$a1,0x2d0($sp)
/*    2d5fc:	8fa602e0 */ 	lw	$a2,0x2e0($sp)
/*    2d600:	8fa702e4 */ 	lw	$a3,0x2e4($sp)
/*    2d604:	e7b40020 */ 	swc1	$f20,0x20($sp)
/*    2d608:	e7b60024 */ 	swc1	$f22,0x24($sp)
/*    2d60c:	afb2002c */ 	sw	$s2,0x2c($sp)
/*    2d610:	afb30030 */ 	sw	$s3,0x30($sp)
/*    2d614:	afb40034 */ 	sw	$s4,0x34($sp)
/*    2d618:	afab0014 */ 	sw	$t3,0x14($sp)
/*    2d61c:	afac0018 */ 	sw	$t4,0x18($sp)
/*    2d620:	0c00b1c5 */ 	jal	cd0002c714
/*    2d624:	afad001c */ 	sw	$t5,0x1c($sp)
/*    2d628:	1440000b */ 	bnez	$v0,.L0002d658
/*    2d62c:	24190001 */ 	addiu	$t9,$zero,0x1
/*    2d630:	c7b20298 */ 	lwc1	$f18,0x298($sp)
/*    2d634:	8fa90294 */ 	lw	$t1,0x294($sp)
/*    2d638:	afb902c0 */ 	sw	$t9,0x2c0($sp)
/*    2d63c:	02602025 */ 	or	$a0,$s3,$zero
/*    2d640:	02802825 */ 	or	$a1,$s4,$zero
/*    2d644:	02403025 */ 	or	$a2,$s2,$zero
/*    2d648:	02003825 */ 	or	$a3,$s0,$zero
/*    2d64c:	e7b20010 */ 	swc1	$f18,0x10($sp)
/*    2d650:	0c009495 */ 	jal	cd00025254
/*    2d654:	afa90014 */ 	sw	$t1,0x14($sp)
.L0002d658:
/*    2d658:	86220002 */ 	lh	$v0,0x2($s1)
/*    2d65c:	26310002 */ 	addiu	$s1,$s1,0x2
/*    2d660:	0441ffcc */ 	bgez	$v0,.L0002d594
/*    2d664:	00000000 */ 	nop
.L0002d668:
/*    2d668:	8fa202c0 */ 	lw	$v0,0x2c0($sp)
/*    2d66c:	8fbf007c */ 	lw	$ra,0x7c($sp)
/*    2d670:	d7b40048 */ 	ldc1	$f20,0x48($sp)
/*    2d674:	d7b60050 */ 	ldc1	$f22,0x50($sp)
/*    2d678:	8fb00058 */ 	lw	$s0,0x58($sp)
/*    2d67c:	8fb1005c */ 	lw	$s1,0x5c($sp)
/*    2d680:	8fb20060 */ 	lw	$s2,0x60($sp)
/*    2d684:	8fb30064 */ 	lw	$s3,0x64($sp)
/*    2d688:	8fb40068 */ 	lw	$s4,0x68($sp)
/*    2d68c:	8fb5006c */ 	lw	$s5,0x6c($sp)
/*    2d690:	8fb60070 */ 	lw	$s6,0x70($sp)
/*    2d694:	8fb70074 */ 	lw	$s7,0x74($sp)
/*    2d698:	8fbe0078 */ 	lw	$s8,0x78($sp)
/*    2d69c:	2c4a0001 */ 	sltiu	$t2,$v0,0x1
/*    2d6a0:	01401025 */ 	or	$v0,$t2,$zero
/*    2d6a4:	03e00008 */ 	jr	$ra
/*    2d6a8:	27bd02e0 */ 	addiu	$sp,$sp,0x2e0
);

// Mismatch: Calculation of g_TileRooms[roomnum] is different
// Other functions and the below use t3 as the offset, t2 as the base, then t2 + t3
// But goal for this function uses t2 as the offset, t3 as the base, then t2 + t3
//s32 cd0002d3b0(struct coord *arg0, struct coord *arg1, s16 *rooms, s32 types, u16 arg4, s32 arg5, s32 arg6, f32 ymax, f32 ymin)
//{
//	s32 roomnum;
//	s16 *roomptr;
//	u8 *start;
//	u8 *end;
//	struct coord sp2c4;
//	bool sp2c0 = false;
//	u32 sp2b4[3];
//	struct coord sp2a8;
//	struct coord sp29c;
//	f32 sp298 = 4294967296;
//	struct geo *sp294;
//	s16 *propnumptr;
//	s16 propnums[256];
//
//	sp2c4.x = arg1->x - arg0->x;
//	sp2c4.y = arg1->y - arg0->y;
//	sp2c4.z = arg1->z - arg0->z;
//
//	if (types & CDTYPE_BG) {
//		roomptr = rooms;
//		roomnum = rooms[0];
//
//		while (roomnum != -1) {
//			if (roomnum < g_TileNumRooms) {
//				start = g_TileFileData.u8 + g_TileRooms[roomnum];
//				end = g_TileFileData.u8 + g_TileRooms[roomnum + 1];
//
//				if (!cd0002c714(start, end, arg0, arg1, &sp2c4, arg4, arg5, arg6, ymax, ymin, &sp298, sp2b4, &sp2a8, &sp29c, &sp294, roomnum)) {
//					sp2c0 = true;
//					cd00025254(&sp2a8, &sp29c, sp2b4, NULL, sp298, sp294);
//				}
//			}
//
//			roomptr++;
//			roomnum = *roomptr;
//		}
//	}
//
//	roomGetProps(rooms, propnums, 256);
//	propnumptr = propnums;
//
//	while (*propnumptr >= 0) {
//		struct prop *prop = &g_Vars.props[*propnumptr];
//
//		if (propIsOfCdType(prop, types)
//				&& propUpdateGeometry(prop, &start, &end)
//				&& !cd0002c714(start, end, arg0, arg1, &sp2c4, arg4, arg5, arg6, ymax, ymin, &sp298, sp2b4, &sp2a8, &sp29c, &sp294, -999)) {
//			sp2c0 = true;
//			cd00025254(&sp2a8, &sp29c, sp2b4, prop, sp298, sp294);
//		}
//
//		propnumptr++;
//	}
//
//	return !sp2c0;
//}

bool cd0002d6ac(struct coord *pos, s16 *rooms, struct coord *targetpos, u32 types, u32 arg4, f32 arg5, f32 arg6)
{
	s16 sp44[21];
	s16 sp34[8];

	portal00018148(pos, targetpos, rooms, sp34, sp44, 20);

	return cd0002d15c(pos, targetpos, sp44, types, 4, 0, arg4, arg5, arg6);
}

s32 cdTestAToB2(struct coord *pos, s16 *rooms, struct coord *coord2, s16 *rooms2, u32 types, s32 arg5, f32 arg6, f32 arg7)
{
	s32 result;
	s16 sp44[20];
	s16 sp34[8];

	func0f065d1c(pos, rooms, coord2, sp34, sp44, 20);

	if (arrayIntersects(sp34, rooms2)) {
		result = cd0002d15c(pos, coord2, sp44, types, 4, 0, arg5, arg6, arg7);
	} else {
		result = 0;
	}

	return result;
}

bool cd0002d7c0(struct coord *pos, s16 *rooms, struct coord *arg2, u32 arg3, u32 arg4, f32 ymax, f32 ymin)
{
	s16 sp44[21];
	s16 sp34[8];

	portal00018148(pos, arg2, rooms, sp34, sp44, 20);

	return cd0002d3b0(pos, arg2, sp44, arg3, 4, 0, arg4, ymax, ymin);
}

s32 cd0002d840(struct coord *arg0, s16 *arg1, struct coord *arg2, s16 *arg3, u32 types, s32 arg5, f32 ymax, f32 ymin)
{
	s16 rooms[21];

	portal00018148(arg0, arg2, arg1, arg3, rooms, 20);

	return cd0002d15c(arg0, arg2, rooms, types, 4, 0, arg5, ymax, ymin);
}

s32 cd0002d8b8(struct coord *pos, s16 *rooms, struct coord *pos2, s16 *rooms2, s32 types, bool arg5, f32 ymax, f32 ymin)
{
	s16 sp44[21];
	s16 sp34[8];
	s32 result;

	func0f065d1c(pos, rooms, pos2, sp34, sp44, 20);

	result = cd0002d3b0(pos, pos2, sp44, types, 4, 0, arg5, ymax, ymin);

	if (result != CDRESULT_COLLISION && !arrayIntersects(sp34, rooms2)) {
		cd00024f6c();
		result = CDRESULT_ERROR;
	}

	return result;
}

s32 cdTestAToB3(struct coord *arg0, s16 *arg1, struct coord *arg2, s16 *arg3, f32 width, s32 types, s32 arg6, f32 ymax, f32 ymin)
{
	u32 stack[7];
	s16 sp5c[8];
	s16 sp4c[8];
	struct coord sp40;
	s32 result;

	func0f065d1c(arg0, arg1, arg2, sp4c, sp5c, 20);

	result = cd0002d3b0(arg0, arg2, sp5c, types, 4, 0, arg6, ymax, ymin);

	if (result == CDRESULT_COLLISION) {
		sp40.x = arg2->x - arg0->x;
		sp40.y = arg2->y - arg0->y;
		sp40.z = arg2->z - arg0->z;

		cd000250cc(arg0, &sp40, width);
	} else if (!arrayIntersects(sp4c, arg3)) {
		cd00024f6c();
		result = -1;
	}

	return result;
}

s32 cd0002da50(struct coord *arg0, s16 *arg1, struct coord *arg2, s16 *arg3, u32 types, s32 arg5, f32 ymax, f32 ymin)
{
	s16 rooms[21];

	portal00018148(arg0, arg2, arg1, arg3, rooms, 20);

	return cd0002d3b0(arg0, arg2, rooms, types, 4, 0, arg5, ymax, ymin);
}

s32 cd0002dac8(struct coord *arg0, s16 *arg1, struct coord *arg2, s16 *arg3, f32 width, u32 types, s32 arg6, f32 ymax, f32 ymin)
{
	s16 rooms[21];
	struct coord sp40;
	s32 result;

	portal00018148(arg0, arg2, arg1, arg3, rooms, 20);

	result = cd0002d3b0(arg0, arg2, rooms, types, 4, 0, arg6, ymax, ymin);

	if (result == 0) {
		sp40.x = arg2->x - arg0->x;
		sp40.y = arg2->y - arg0->y;
		sp40.z = arg2->z - arg0->z;

		cd000250cc(arg0, &sp40, width);
	}

	return result;
}

bool cd0002db98(struct coord *viewpos, s16 *rooms, struct coord *targetpos, u32 types, u16 arg4)
{
	s16 sp44[21];
	s16 sp34[8];

	portal00018148(viewpos, targetpos, rooms, sp34, sp44, 20);

	return cd0002d15c(viewpos, targetpos, sp44, types, arg4, 1, 1, 0, 0);
}

bool cd0002dc18(struct coord *coord, s16 *rooms, struct coord *coord2, s32 arg3)
{
	return cd0002db98(coord, rooms, coord2, arg3, 0x1c);
}

bool cdHasLineOfSight(struct coord *coord, s16 *rooms, struct coord *coord2, s16 *rooms2, s32 arg4, u16 arg5)
{
	bool result;
	s16 sp44[20];
	s16 sp34[8];

	func0f065d1c(coord, rooms, coord2, sp34, sp44, 20);

	if (arrayIntersects(sp34, rooms2)) {
		result = cd0002d15c(coord, coord2, sp44, arg4, arg5, 1, 1, 0, 0);
	} else {
		result = false;
	}

	return result;
}

bool cd0002dcd0(struct coord *arg0, s16 *rooms1, struct coord *arg2, s16 *rooms2, u32 arg4)
{
	return cdHasLineOfSight(arg0, rooms1, arg2, rooms2, arg4, 0x1c);
}

bool cd0002dcfc(struct coord *pos, s16 *rooms, struct coord *pos2, s16 *rooms2, s16 *rooms3, u32 types, u16 arg6)
{
	bool result;
	s16 sp34[20];

	func0f065d1c(pos, rooms, pos2, rooms3, sp34, 20);

	if (arrayIntersects(rooms3, rooms2)) {
		result = cd0002d15c(pos, pos2, sp34, types, arg6, 1, 1, 0, 0);
	} else {
		result = false;
	}

	return result;
}

s32 cdTestAToB4(struct coord *pos, s16 *rooms, struct coord *pos2, u32 types, u16 arg4)
{
	s16 sp44[21];
	s16 sp34[8];

	portal00018148(pos, pos2, rooms, sp34, sp44, 20);

	return cd0002d3b0(pos, pos2, sp44, types, arg4, 1, 1, 0, 0);
}

s32 cd0002de10(struct coord *pos, s16 *rooms, struct coord *pos2, u32 types)
{
	return cdTestAToB4(pos, rooms, pos2, types, 0x1c);
}

s32 cd0002de34(struct coord *arg0, s16 *arg1, struct coord *arg2, s16 *arg3, u32 types, u16 arg5)
{
	s16 rooms[21];

	portal00018148(arg0, arg2, arg1, arg3, rooms, 20);

	return cd0002d15c(arg0, arg2, rooms, types, arg5, 1, 1, 0, 0);
}

s32 cd0002deac(struct coord *arg0, s16 *arg1, struct coord *arg2, s16 *arg3, u32 types)
{
	return cd0002de34(arg0, arg1, arg2, arg3, types, 0x1c);
}

bool cd0002ded8(struct coord *arg0, struct coord *arg1, struct prop *prop)
{
	u8 *start;
	u8 *end;
	struct coord sp7c;
	bool result = false;
	struct coord sp6c;
	struct coord sp60;
	struct coord sp54;
	f32 sp50 = 4294967296;
	struct geo *geo;

	sp7c.x = arg1->x - arg0->x;
	sp7c.y = arg1->y - arg0->y;
	sp7c.z = arg1->z - arg0->z;

	if (propUpdateGeometry(prop, &start, &end)) {
		if (!cd0002c714(start, end, arg0, arg1, &sp7c, 0x1c, 1, 1, 0, 0, &sp50, &sp6c, &sp60, &sp54, &geo, -999)) {
			result = true;
			cd00025254(&sp60, &sp54, &sp6c, prop, sp50, geo);
		}
	}

	return !result;
}

GLOBAL_ASM(
glabel cd0002dffc
/*    2dffc:	27bdff78 */ 	addiu	$sp,$sp,-136
/*    2e000:	afbf003c */ 	sw	$ra,0x3c($sp)
/*    2e004:	afb50038 */ 	sw	$s5,0x38($sp)
/*    2e008:	afb40034 */ 	sw	$s4,0x34($sp)
/*    2e00c:	afb30030 */ 	sw	$s3,0x30($sp)
/*    2e010:	afb2002c */ 	sw	$s2,0x2c($sp)
/*    2e014:	afb10028 */ 	sw	$s1,0x28($sp)
/*    2e018:	afb00024 */ 	sw	$s0,0x24($sp)
/*    2e01c:	f7b60018 */ 	sdc1	$f22,0x18($sp)
/*    2e020:	f7b40010 */ 	sdc1	$f20,0x10($sp)
/*    2e024:	90900001 */ 	lbu	$s0,0x1($a0)
/*    2e028:	00809025 */ 	or	$s2,$a0,$zero
/*    2e02c:	00a0a825 */ 	or	$s5,$a1,$zero
/*    2e030:	90b10001 */ 	lbu	$s1,0x1($a1)
/*    2e034:	1a000084 */ 	blez	$s0,.L0002e248
/*    2e038:	00003025 */ 	or	$a2,$zero,$zero
/*    2e03c:	44802000 */ 	mtc1	$zero,$f4
/*    2e040:	00809825 */ 	or	$s3,$a0,$zero
/*    2e044:	d7b40040 */ 	ldc1	$f20,0x40($sp)
/*    2e048:	460025a1 */ 	cvt.d.s	$f22,$f4
.L0002e04c:
/*    2e04c:	24d40001 */ 	addiu	$s4,$a2,0x1
/*    2e050:	0290001a */ 	div	$zero,$s4,$s0
/*    2e054:	00002810 */ 	mfhi	$a1
/*    2e058:	000570c0 */ 	sll	$t6,$a1,0x3
/*    2e05c:	024e1021 */ 	addu	$v0,$s2,$t6
/*    2e060:	c4460010 */ 	lwc1	$f6,0x10($v0)
/*    2e064:	c6620010 */ 	lwc1	$f2,0x10($s3)
/*    2e068:	c44a000c */ 	lwc1	$f10,0xc($v0)
/*    2e06c:	46003221 */ 	cvt.d.s	$f8,$f6
/*    2e070:	46001021 */ 	cvt.d.s	$f0,$f2
/*    2e074:	c672000c */ 	lwc1	$f18,0xc($s3)
/*    2e078:	46204301 */ 	sub.d	$f12,$f8,$f0
/*    2e07c:	16000002 */ 	bnez	$s0,.L0002e088
/*    2e080:	00000000 */ 	nop
/*    2e084:	0007000d */ 	break	0x7
.L0002e088:
/*    2e088:	2401ffff */ 	addiu	$at,$zero,-1
/*    2e08c:	16010004 */ 	bne	$s0,$at,.L0002e0a0
/*    2e090:	3c018000 */ 	lui	$at,0x8000
/*    2e094:	16810002 */ 	bne	$s4,$at,.L0002e0a0
/*    2e098:	00000000 */ 	nop
/*    2e09c:	0006000d */ 	break	0x6
.L0002e0a0:
/*    2e0a0:	46366032 */ 	c.eq.d	$f12,$f22
/*    2e0a4:	24af0001 */ 	addiu	$t7,$a1,0x1
/*    2e0a8:	46005121 */ 	cvt.d.s	$f4,$f10
/*    2e0ac:	46009421 */ 	cvt.d.s	$f16,$f18
/*    2e0b0:	4500000d */ 	bc1f	.L0002e0e8
/*    2e0b4:	46248381 */ 	sub.d	$f14,$f16,$f4
/*    2e0b8:	46367032 */ 	c.eq.d	$f14,$f22
/*    2e0bc:	00000000 */ 	nop
/*    2e0c0:	45000009 */ 	bc1f	.L0002e0e8
/*    2e0c4:	00000000 */ 	nop
/*    2e0c8:	44059000 */ 	mfc1	$a1,$f18
/*    2e0cc:	44061000 */ 	mfc1	$a2,$f2
/*    2e0d0:	0c009948 */ 	jal	cdIs2dPointInBlock
/*    2e0d4:	02a02025 */ 	or	$a0,$s5,$zero
/*    2e0d8:	50400058 */ 	beqzl	$v0,.L0002e23c
/*    2e0dc:	02803025 */ 	or	$a2,$s4,$zero
/*    2e0e0:	1000005a */ 	b	.L0002e24c
/*    2e0e4:	00001025 */ 	or	$v0,$zero,$zero
.L0002e0e8:
/*    2e0e8:	01f0001a */ 	div	$zero,$t7,$s0
/*    2e0ec:	462e0182 */ 	mul.d	$f6,$f0,$f14
/*    2e0f0:	00001010 */ 	mfhi	$v0
/*    2e0f4:	00401825 */ 	or	$v1,$v0,$zero
/*    2e0f8:	00002025 */ 	or	$a0,$zero,$zero
/*    2e0fc:	462c8202 */ 	mul.d	$f8,$f16,$f12
/*    2e100:	46283080 */ 	add.d	$f2,$f6,$f8
/*    2e104:	16000002 */ 	bnez	$s0,.L0002e110
/*    2e108:	00000000 */ 	nop
/*    2e10c:	0007000d */ 	break	0x7
.L0002e110:
/*    2e110:	2401ffff */ 	addiu	$at,$zero,-1
/*    2e114:	16010004 */ 	bne	$s0,$at,.L0002e128
/*    2e118:	3c018000 */ 	lui	$at,0x8000
/*    2e11c:	15e10002 */ 	bne	$t7,$at,.L0002e128
/*    2e120:	00000000 */ 	nop
/*    2e124:	0006000d */ 	break	0x6
.L0002e128:
/*    2e128:	10c2001d */ 	beq	$a2,$v0,.L0002e1a0
/*    2e12c:	00000000 */ 	nop
/*    2e130:	0003c0c0 */ 	sll	$t8,$v1,0x3
.L0002e134:
/*    2e134:	02581021 */ 	addu	$v0,$s2,$t8
/*    2e138:	c44a0010 */ 	lwc1	$f10,0x10($v0)
/*    2e13c:	c448000c */ 	lwc1	$f8,0xc($v0)
/*    2e140:	46005121 */ 	cvt.d.s	$f4,$f10
/*    2e144:	460042a1 */ 	cvt.d.s	$f10,$f8
/*    2e148:	462e2182 */ 	mul.d	$f6,$f4,$f14
/*    2e14c:	00000000 */ 	nop
/*    2e150:	462c5102 */ 	mul.d	$f4,$f10,$f12
/*    2e154:	46243500 */ 	add.d	$f20,$f6,$f4
/*    2e158:	4622a032 */ 	c.eq.d	$f20,$f2
/*    2e15c:	00000000 */ 	nop
/*    2e160:	4500000f */ 	bc1f	.L0002e1a0
/*    2e164:	00000000 */ 	nop
/*    2e168:	24790001 */ 	addiu	$t9,$v1,0x1
/*    2e16c:	0330001a */ 	div	$zero,$t9,$s0
/*    2e170:	00001810 */ 	mfhi	$v1
/*    2e174:	16000002 */ 	bnez	$s0,.L0002e180
/*    2e178:	00000000 */ 	nop
/*    2e17c:	0007000d */ 	break	0x7
.L0002e180:
/*    2e180:	2401ffff */ 	addiu	$at,$zero,-1
/*    2e184:	16010004 */ 	bne	$s0,$at,.L0002e198
/*    2e188:	3c018000 */ 	lui	$at,0x8000
/*    2e18c:	17210002 */ 	bne	$t9,$at,.L0002e198
/*    2e190:	00000000 */ 	nop
/*    2e194:	0006000d */ 	break	0x6
.L0002e198:
/*    2e198:	5466ffe6 */ 	bnel	$v1,$a2,.L0002e134
/*    2e19c:	0003c0c0 */ 	sll	$t8,$v1,0x3
.L0002e1a0:
/*    2e1a0:	1a200021 */ 	blez	$s1,.L0002e228
/*    2e1a4:	00000000 */ 	nop
/*    2e1a8:	02a01025 */ 	or	$v0,$s5,$zero
.L0002e1ac:
/*    2e1ac:	c4480010 */ 	lwc1	$f8,0x10($v0)
/*    2e1b0:	c444000c */ 	lwc1	$f4,0xc($v0)
/*    2e1b4:	4622a032 */ 	c.eq.d	$f20,$f2
/*    2e1b8:	460042a1 */ 	cvt.d.s	$f10,$f8
/*    2e1bc:	46002221 */ 	cvt.d.s	$f8,$f4
/*    2e1c0:	462e5182 */ 	mul.d	$f6,$f10,$f14
/*    2e1c4:	00000000 */ 	nop
/*    2e1c8:	462c4282 */ 	mul.d	$f10,$f8,$f12
/*    2e1cc:	45000003 */ 	bc1f	.L0002e1dc
/*    2e1d0:	462a3000 */ 	add.d	$f0,$f6,$f10
/*    2e1d4:	46201101 */ 	sub.d	$f4,$f2,$f0
/*    2e1d8:	46222500 */ 	add.d	$f20,$f4,$f2
.L0002e1dc:
/*    2e1dc:	4622003c */ 	c.lt.d	$f0,$f2
/*    2e1e0:	00000000 */ 	nop
/*    2e1e4:	45020006 */ 	bc1fl	.L0002e200
/*    2e1e8:	4620103c */ 	c.lt.d	$f2,$f0
/*    2e1ec:	4622a03c */ 	c.lt.d	$f20,$f2
/*    2e1f0:	00000000 */ 	nop
/*    2e1f4:	4501000c */ 	bc1t	.L0002e228
/*    2e1f8:	00000000 */ 	nop
/*    2e1fc:	4620103c */ 	c.lt.d	$f2,$f0
.L0002e200:
/*    2e200:	00000000 */ 	nop
/*    2e204:	45020006 */ 	bc1fl	.L0002e220
/*    2e208:	24840001 */ 	addiu	$a0,$a0,0x1
/*    2e20c:	4634103c */ 	c.lt.d	$f2,$f20
/*    2e210:	00000000 */ 	nop
/*    2e214:	45010004 */ 	bc1t	.L0002e228
/*    2e218:	00000000 */ 	nop
/*    2e21c:	24840001 */ 	addiu	$a0,$a0,0x1
.L0002e220:
/*    2e220:	1491ffe2 */ 	bne	$a0,$s1,.L0002e1ac
/*    2e224:	24420008 */ 	addiu	$v0,$v0,0x8
.L0002e228:
/*    2e228:	54910004 */ 	bnel	$a0,$s1,.L0002e23c
/*    2e22c:	02803025 */ 	or	$a2,$s4,$zero
/*    2e230:	10000006 */ 	b	.L0002e24c
/*    2e234:	24020001 */ 	addiu	$v0,$zero,0x1
/*    2e238:	02803025 */ 	or	$a2,$s4,$zero
.L0002e23c:
/*    2e23c:	1690ff83 */ 	bne	$s4,$s0,.L0002e04c
/*    2e240:	26730008 */ 	addiu	$s3,$s3,0x8
/*    2e244:	f7b40040 */ 	sdc1	$f20,0x40($sp)
.L0002e248:
/*    2e248:	00001025 */ 	or	$v0,$zero,$zero
.L0002e24c:
/*    2e24c:	8fbf003c */ 	lw	$ra,0x3c($sp)
/*    2e250:	d7b40010 */ 	ldc1	$f20,0x10($sp)
/*    2e254:	d7b60018 */ 	ldc1	$f22,0x18($sp)
/*    2e258:	8fb00024 */ 	lw	$s0,0x24($sp)
/*    2e25c:	8fb10028 */ 	lw	$s1,0x28($sp)
/*    2e260:	8fb2002c */ 	lw	$s2,0x2c($sp)
/*    2e264:	8fb30030 */ 	lw	$s3,0x30($sp)
/*    2e268:	8fb40034 */ 	lw	$s4,0x34($sp)
/*    2e26c:	8fb50038 */ 	lw	$s5,0x38($sp)
/*    2e270:	03e00008 */ 	jr	$ra
/*    2e274:	27bd0088 */ 	addiu	$sp,$sp,0x88
);

// Mismatch: Float regalloc, likely related to the zero variable
//bool cd0002dffc(struct geoblock *arg0, struct geoblock *arg1)
//{
//	u32 stack[4];
//	f32 zero = 0;
//	s32 numvertices0 = arg0->header.numvertices;
//	s32 numvertices1 = arg1->header.numvertices;
//	s32 i;
//
//	for (i = 0; i < numvertices0; i++) {
//		s32 next = (i + 1) % numvertices0;
//		f64 diff1;
//		f64 diff2;
//
//		diff1 = arg0->vertices[next][1] - (f64)arg0->vertices[i][1];
//		diff2 = arg0->vertices[i][0] - (f64)arg0->vertices[next][0];
//
//		if (diff1 == zero && diff2 == zero) {
//			if (cdIs2dPointInBlock(arg1, arg0->vertices[i][0], arg0->vertices[i][1])) {
//				return false;
//			}
//		} else {
//			f64 sum1 = arg0->vertices[i][0] * diff1 + arg0->vertices[i][1] * diff2;
//			f64 sum2;
//			s32 j = (next + 1) % numvertices0;
//			s32 k;
//
//			while (j != i) {
//				sum2 = arg0->vertices[j][0] * diff1 + arg0->vertices[j][1] * diff2;
//
//				if (sum2 != sum1) {
//					break;
//				}
//
//				j = (j + 1) % numvertices0;
//			}
//
//			for (k = 0; k < numvertices1; k++) {
//				f64 sum3 = arg1->vertices[k][0] * diff1 + arg1->vertices[k][1] * diff2;
//
//				if (sum2 == sum1) {
//					sum2 = sum1 - sum3 + sum1;
//				}
//
//				if ((sum3 < sum1 && sum2 < sum1) || (sum3 > sum1 && sum2 > sum1)) {
//					break;
//				}
//			}
//
//			if (k == numvertices1) {
//				return true;
//			}
//		}
//	}
//
//	return false;
//}

s32 cd0002e278(u8 *start, u8 *end, struct geoblock *ref, u16 flags)
{
	struct geo *geo = (struct geo *) start;

	while (geo < (struct geo *) end) {
		if (geo->type == GEOTYPE_TILE_I) {
			struct geotilei *tile = (struct geotilei *) geo;
			geo = (struct geo *)((u32)geo + tile->header.numvertices * 6 + 0xe);
		} else if (geo->type == GEOTYPE_TILE_F) {
			struct geotilef *type1 = (struct geotilef *) geo;
			geo = (struct geo *)((u32)geo + type1->header.numvertices * 0xc + 0x10);
		} else if (geo->type == GEOTYPE_BLOCK) {
			struct geoblock *tile2 = (struct geoblock *) geo;

			if ((flags & (GEOFLAG_COLLISIONS | GEOFLAG_0008 | GEOFLAG_OPAQUE))
					&& tile2->ymax >= ref->ymin
					&& tile2->ymin <= ref->ymax) {
				// Tiles are overlapping vertically
				s32 i;

				for (i = 0; i < ref->header.numvertices; i++) {
					if (cdIs2dPointInBlock(tile2, ref->vertices[i][0], ref->vertices[i][1])) {
						return false;
					}
				}

				for (i = 0; i < tile2->header.numvertices; i++) {
					if (cdIs2dPointInBlock(ref, tile2->vertices[i][0], tile2->vertices[i][1])) {
						return false;
					}
				}

				if (!cd0002dffc(ref, tile2) && !cd0002dffc(tile2, ref)) {
					return false;
				}
			}

			geo = (struct geo *)((u32)geo + 0x4c);
		} else if (geo->type == GEOTYPE_CYL) {
			struct geocyl *cyl = (struct geocyl *) geo;

			if ((flags & geo->flags)
					&& cyl->ymax >= ref->ymin
					&& cyl->ymin <= ref->ymax
					&& cd000274e0(ref, cyl->x, cyl->z, cyl->radius, NULL, NULL)) {
				return false;
			}

			geo = (struct geo *)((u32)geo + 0x18);
		}
	}

	return true;
}

s32 cd0002e4c4(struct geoblock *geo, s16 *rooms, u32 types)
{
	s32 result = CDRESULT_NOCOLLISION;
	s32 roomnum;
	u8 *start;
	u8 *end;
	s16 *roomptr;
	s16 propnums[256];
	s16 *propnumptr;

	// Check BG
	if (types & CDTYPE_BG) {
		roomptr = rooms;
		roomnum = rooms[0];

		while (roomnum != -1) {
			if (roomnum < g_TileNumRooms) {
				start = g_TileFileData.u8 + g_TileRooms[roomnum];
				end = g_TileFileData.u8 + g_TileRooms[roomnum + 1];

				result = cd0002e278(start, end, geo, GEOFLAG_COLLISIONS);

				if (result == CDRESULT_COLLISION) {
					cd00025168(NULL);
					break;
				}
			}

			roomptr++;
			roomnum = *roomptr;
		}
	}

	// Check props
	if (result != CDRESULT_COLLISION) {
		roomGetProps(rooms, propnums, 256);
		propnumptr = propnums;

		while (*propnumptr >= 0) {
			struct prop *prop = &g_Vars.props[*propnumptr];

			if (propIsOfCdType(prop, types) && propUpdateGeometry(prop, &start, &end)) {
				result = cd0002e278(start, end, geo, GEOFLAG_COLLISIONS);

				if (result == CDRESULT_COLLISION) {
					cd00025168(prop);
					break;
				}
			}

			propnumptr++;
		}
	}

	return result;
}

bool cd0002e680(struct geotilei *tile, s32 numvertices, struct coord *arg2, struct coord *arg3, struct prop *prop, struct geoblock *block)
{
	bool result = false;
	s32 i;
	s32 next;
	s32 curr;
	struct coord sp84;
	struct coord sp78;
	struct coord sp6c;

	for (i = 0; i < numvertices; i++) {
		if (var8005f030) {
			s32 remaining = numvertices - i;
			next = (remaining + numvertices - 2) % numvertices;
			curr = remaining - 1;
		} else {
			next = (i + 1) % numvertices;
			curr = i;
		}

		if (cd0002ac70((struct coord *)((u32)arg2 + curr * sizeof(struct coord)),
					(struct coord *)((u32)arg2 + next * sizeof(struct coord)),
					(struct coord *)((u32)arg3 + curr * sizeof(struct coord)),
					tile, &sp6c, &sp84, &sp78, 0, 0.0f, 0.0f)) {
			cd000251ac(&sp84, &sp78, &sp6c, prop);
			cd00025314((struct coord *)((u32)arg2 + curr * sizeof(struct coord)), (struct coord *)((u32)arg2 + next * sizeof(struct coord)));
			cd000253c4(block);
			result = true;
			break;
		}
	}

	return result;
}

bool cd0002e82c(struct geotilef *tile, s32 numvertices, struct coord *arg2, struct coord *arg3, struct prop *prop, struct geoblock *block)
{
	bool result = false;
	s32 i;
	s32 next;
	s32 curr;
	struct coord sp84;
	struct coord sp78;
	struct coord sp6c;

	for (i = 0; i < numvertices; i++) {
		if (var8005f030) {
			s32 remaining = numvertices - i;
			next = (remaining + numvertices - 2) % numvertices;
			curr = remaining - 1;
		} else {
			next = (i + 1) % numvertices;
			curr = i;
		}

		if (cd0002b128((struct coord *)((u32)arg2 + curr * sizeof(struct coord)),
					(struct coord *)((u32)arg2 + next * sizeof(struct coord)),
					(struct coord *)((u32)arg3 + curr * sizeof(struct coord)),
					tile, &sp6c, &sp84, &sp78, 0, 0.0f, 0.0f)) {
			cd000251ac(&sp84, &sp78, &sp6c, prop);
			cd00025314((struct coord *)((u32)arg2 + curr * sizeof(struct coord)), (struct coord *)((u32)arg2 + next * sizeof(struct coord)));
			cd000253c4(block);
			result = true;
			break;
		}
	}

	return result;
}

bool cd0002e9d8(struct geoblock *thisblock, s32 numvertices, struct coord *arg2, struct coord *arg3, struct prop *prop, struct geoblock *block)
{
	bool result = false;
	s32 i;
	s32 next;
	s32 curr;
	struct coord sp84;
	struct coord sp78;
	struct coord sp6c;

	for (i = 0; i < numvertices; i++) {
		if (var8005f030) {
			s32 remaining = numvertices - i;
			next = (remaining + numvertices - 2) % numvertices;
			curr = remaining - 1;
		} else {
			next = (i + 1) % numvertices;
			curr = i;
		}

		if (cd0002b560((struct coord *)((u32)arg2 + curr * sizeof(struct coord)),
					(struct coord *)((u32)arg2 + next * sizeof(struct coord)),
					(struct coord *)((u32)arg3 + curr * sizeof(struct coord)),
					thisblock, &sp6c, &sp84, &sp78, 0, 0.0f, 0.0f)) {
			cd000251ac(&sp84, &sp78, &sp6c, prop);
			cd00025314((struct coord *)((u32)arg2 + curr * sizeof(struct coord)), (struct coord *)((u32)arg2 + next * sizeof(struct coord)));
			cd000253c4(block);
			result = true;
			break;
		}
	}

	return result;
}

bool cd0002eb84(struct geocyl *cyl, s32 numvertices, struct coord *arg2, struct coord *arg3, struct prop *prop, struct geoblock *block)
{
	bool result = false;
	s32 i;
	s32 next;
	s32 curr;
	struct coord sp84;
	struct coord sp78;
	struct coord sp6c;

	for (i = 0; i < numvertices; i++) {
		if (var8005f030) {
			s32 remaining = numvertices - i;
			next = (remaining + numvertices - 2) % numvertices;
			curr = remaining - 1;
		} else {
			next = (i + 1) % numvertices;
			curr = i;
		}

		if (cd0002b954((struct coord *)((u32)arg2 + curr * sizeof(struct coord)),
					(struct coord *)((u32)arg2 + next * sizeof(struct coord)),
					(struct coord *)((u32)arg3 + curr * sizeof(struct coord)),
					cyl, &sp6c, &sp84, &sp78, 0, 0.0f, 0.0f)) {
			cd000251ac(&sp84, &sp78, &sp6c, prop);
			cd00025314((struct coord *)((u32)arg2 + curr * sizeof(struct coord)), (struct coord *)((u32)arg2 + next * sizeof(struct coord)));
			cd000253c4(block);
			result = true;
			break;
		}
	}

	return result;
}

bool cd0002ed30(u8 *start, u8 *end, struct geoblock *block, s32 numvertices, struct coord *arg4, struct coord *arg5, u16 flags, struct prop *prop)
{
	struct geo *geo = (struct geo *) start;

	while (geo < (struct geo *) end) {
		if (1);

		if (geo->type == GEOTYPE_TILE_I) {
			struct geotilei *tile = (struct geotilei *)geo;

			if ((flags & geo->flags)
					&& *(s16 *)(tile->ymax + (u32)tile) >= block->ymin
					&& *(s16 *)(tile->ymin + (u32)tile) <= block->ymax
					&& cd0002e680(tile, numvertices, arg4, arg5, prop, block)) {
				return false;
			}

			geo = (struct geo *)((u32)geo + tile->header.numvertices * 6 + 0xe);
		} else if (geo->type == GEOTYPE_TILE_F) {
			struct geotilef *tile = (struct geotilef *)geo;

			if ((flags & geo->flags)
					&& tile->vertices[tile->ymax].y >= block->ymin
					&& tile->vertices[tile->ymin].y <= block->ymax
					&& cd0002e82c(tile, numvertices, arg4, arg5, prop, block)) {
				return false;
			}

			geo = (struct geo *)((u32)geo + (u32)(tile->header.numvertices - 0x40) * 0xc + 0x310);
		} else if (geo->type == GEOTYPE_BLOCK) {
			struct geoblock *block2 = (struct geoblock *)geo;

			if ((flags & (GEOFLAG_COLLISIONS | GEOFLAG_0008 | GEOFLAG_OPAQUE))
					&& block2->ymax >= block->ymin
					&& block2->ymin <= block->ymax
					&& cd0002e9d8(block2, numvertices, arg4, arg5, prop, block)) {
				return false;
			}

			geo = (struct geo *)((u32)geo + sizeof(struct geoblock));
		} else if (geo->type == GEOTYPE_CYL) {
			struct geocyl *cyl = (struct geocyl *)geo;

			if ((flags & geo->flags)
					&& cyl->ymax >= block->ymin
					&& cyl->ymin <= block->ymax
					&& cd0002eb84(cyl, numvertices, arg4, arg5, prop, block)) {
				return false;
			}

			geo = (struct geo *)((u32)geo + sizeof(struct geocyl));
		}
	}

	return true;
}

bool cd0002f02c(struct geoblock *block, s16 *rooms, s32 types)
{
	s32 numvertices = block->header.numvertices;
	s32 i;
	u8 *start;
	u8 *end;
	s32 next;
	s16 propnums[256];
	s16 *propnumptr;
	bool result = true;
	struct coord spcc[8];
	struct coord sp6c[8];

	for (i = 0; i < numvertices; i++) {
		spcc[i].x = block->vertices[i][0];
		spcc[i].y = block->ymax;
		spcc[i].z = block->vertices[i][1];
	}

	for (i = 0; i < numvertices; i++) {
		next = (i + 1) % numvertices;

		sp6c[i].x = spcc[next].x - spcc[i].x;
		sp6c[i].y = spcc[next].y - spcc[i].y;
		sp6c[i].z = spcc[next].z - spcc[i].z;
	}

	if (types & CDTYPE_BG) {
		s16 *roomsptr = rooms;
		s32 roomnum = *roomsptr;

		while (roomnum != -1) {
			if (roomnum < g_TileNumRooms) {
				start = g_TileFileData.u8 + g_TileRooms[roomnum];
				end = g_TileFileData.u8 + g_TileRooms[roomnum + 1];

				result = cd0002ed30(start, end, block, numvertices, spcc, sp6c, 4, NULL);

				if (!result) {
					break;
				}
			}

			roomsptr++;
			roomnum = *roomsptr;
		}
	}

	if (result) {
		roomGetProps(rooms, propnums, 256);

		propnumptr = propnums;

		while (*propnumptr >= 0) {
			struct prop *prop = &g_Vars.props[*propnumptr];

			if (propIsOfCdType(prop, types)) {
				if (propUpdateGeometry(prop, &start, &end)) {
					result = cd0002ed30(start, end, block, numvertices, spcc, sp6c, 4, prop);

					if (!result) {
						break;
					}
				}
			}

			propnumptr++;
		}
	}

	return result;
}

#if VERSION < VERSION_NTSC_1_0
struct debugtri *cdReadIntTileVertices(struct debugtri *ptr, s32 *remaining, struct geotilei *tile)
{
	if (tile->header.flags & (GEOFLAG_0001 | GEOFLAG_COLLISIONS)) {
		s32 i;
		s32 numvertices = tile->header.numvertices;
		s16 vertices[16][3];

		if (numvertices > 16) {
			numvertices = 16;
		}

		for (i = 0; i < numvertices; i++) {
			vertices[i][0] = tile->vertices[i][0] - g_Vars.currentplayer->globaldrawworldoffset.x;
			vertices[i][1] = tile->vertices[i][1] - g_Vars.currentplayer->globaldrawworldoffset.y;
			vertices[i][2] = tile->vertices[i][2] - g_Vars.currentplayer->globaldrawworldoffset.z;
		}

		for (i = 2; i < numvertices; i++) {
			if (*remaining > 0) {
				ptr->vertices[0][0] = vertices[0][0];
				ptr->vertices[0][1] = vertices[0][1];
				ptr->vertices[0][2] = vertices[0][2];
				ptr->vertices[1][0] = vertices[i - 1][0];
				ptr->vertices[1][1] = vertices[i - 1][1];
				ptr->vertices[1][2] = vertices[i - 1][2];
				ptr->vertices[2][0] = vertices[i][0];
				ptr->vertices[2][1] = vertices[i][1];
				ptr->vertices[2][2] = vertices[i][2];
				ptr->unk12 = 0;

				ptr++;
			}

			*remaining -= 1;
		}
	}

	return ptr;
}
#endif

#if VERSION < VERSION_NTSC_1_0
struct debugtri *cdReadFltTileVertices(struct debugtri *ptr, s32 *remaining, struct geotilef *tile)
{
	if (tile->header.flags & (GEOFLAG_0001 | GEOFLAG_COLLISIONS)) {
		s32 i;
		s32 numvertices = tile->header.numvertices;
		s16 vertices[16][3];

		if (numvertices > 16) {
			numvertices = 16;
		}

		for (i = 0; i < numvertices; i++) {
			vertices[i][0] = tile->vertices[i].x - g_Vars.currentplayer->globaldrawworldoffset.x;
			vertices[i][1] = tile->vertices[i].y - g_Vars.currentplayer->globaldrawworldoffset.y;
			vertices[i][2] = tile->vertices[i].z - g_Vars.currentplayer->globaldrawworldoffset.z;
		}

		for (i = 2; i < numvertices; i++) {
			if (*remaining > 0) {
				ptr->vertices[0][0] = vertices[0][0];
				ptr->vertices[0][1] = vertices[0][1];
				ptr->vertices[0][2] = vertices[0][2];
				ptr->vertices[1][0] = vertices[i - 1][0];
				ptr->vertices[1][1] = vertices[i - 1][1];
				ptr->vertices[1][2] = vertices[i - 1][2];
				ptr->vertices[2][0] = vertices[i][0];
				ptr->vertices[2][1] = vertices[i][1];
				ptr->vertices[2][2] = vertices[i][2];
				ptr->unk12 = 0;

				ptr++;
			}

			*remaining -= 1;
		}
	}

	return ptr;
}
#endif

Gfx *cd0002f2e8(Gfx *gdl, u32 arg1, u32 arg2, u32 arg3)
{
	return gdl;
}

void func0002f2fc(u32 arg0, u32 arg1)
{
	// empty
}

bool cd0002f308(struct coord *viewpos, s16 *rooms, struct coord *targetpos, f32 distance, s32 arg4, u16 arg5)
{
	struct coord diff;
	f32 x;
	f32 z;
	struct coord vector;

	if (cd0002db98(viewpos, rooms, targetpos, arg4, arg5)) {
		return true;
	}

	vector.x = targetpos->x - viewpos->x;
	vector.y = 0;
	vector.z = targetpos->z - viewpos->z;

	guNormalize(&vector.x, &vector.y, &vector.z);

	x = vector.f[0] * distance;
	z = vector.f[2] * distance;

	diff.x = targetpos->x - z;
	diff.y = targetpos->y;
	diff.z = targetpos->z + x;

	if (cd0002db98(viewpos, rooms, &diff, arg4, arg5)) {
		return true;
	}

	diff.x = targetpos->x + z;
	diff.y = targetpos->y;
	diff.z = targetpos->z - x;

	if (cd0002db98(viewpos, rooms, &diff, arg4, arg5)) {
		return true;
	}

	return false;
}

bool cd0002f450(struct coord *viewpos, s16 *rooms, struct coord *targetpos, f32 distance, s32 arg4)
{
	return cd0002f308(viewpos, rooms, targetpos, distance, arg4, 8);
}
