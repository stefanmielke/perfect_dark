#include <ultra64.h>
#include "constants.h"
#include "game/game_0b3350.h"
#include "game/texdecompress.h"
#include "bss.h"
#include "lib/rzip.h"
#include "lib/dma.h"
#include "lib/main.h"
#include "lib/memp.h"
#include "data.h"
#include "types.h"

struct texture *g_Textures;
u32 var800aabc4;
struct texturething var800aabc8;
struct texcacheitem g_TexCacheItems[150];
s32 g_TexCacheCount;
s32 var800ab53c;
u8 *var800ab540;
u32 var800ab544;
s32 var800ab548;
u32 var800ab54c;
u32 g_TexBase;
u8 *g_TextureConfigSegment;
s32 g_TexNumConfigs;
u32 *g_TexWords;
struct textureconfig *g_TexWallhitConfigs;
Gfx *g_TexGdl1;
Gfx *g_TexGdl2;
Gfx *g_TexGdl3;
struct texturepair *g_ExplosionTexturePairs;
struct textureconfig *g_TexBeamConfigs;
struct textureconfig *g_TexLaserConfigs;
struct textureconfig *g_TexGroup03Configs;
struct textureconfig *g_TexGeCrosshairConfigs;
struct textureconfig *g_TexRedLinesConfigs;
struct textureconfig *g_TexGroup06Configs;
struct textureconfig *g_TexShieldConfigs;
struct textureconfig *g_TexShardConfigs;
struct textureconfig *g_TexScreenConfigs;
struct textureconfig *g_TexWaterConfigs;
struct textureconfig *g_TexSkyConfigs;
struct textureconfig *g_TexLightGlareConfigs;
struct textureconfig *g_TexSparkConfigs;
struct textureconfig *g_TexGeneralConfigs;
struct textureconfig *g_TexRadarConfigs;

u32 var800841b0 = 0x0006ddd0;
u32 var800841b4 = 0x00000000;

// The number of channels, excluding 1-bit alpha channels.
s32 g_TexFormatNumChannels[] = { 4, 3, 3, 3, 2, 2, 1, 1, 1, 1, 1, 1, 1 };

// Whether each format supports a 1-bit alpha channel.
s32 g_TexFormatHas1BitAlpha[] = { 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 };

// For non-paletted images, size in decimal of each colour channel.
// Eg. 32 means each channel can store up to 32 values (5-bits per channel).
// For paletted images, same thing but for the palette indices instead.
s32 g_TexFormatChannelSizes[] = { 256, 32, 256, 32, 256, 16, 8, 256, 16, 256, 16, 256, 16 };

s32 g_TexFormatBitsPerPixel[] = { 32, 16, 24, 15, 16, 8, 4, 8, 4, 16, 16, 16, 16 };

// Mapping to GBI format
s32 g_TexFormatGbiMappings[] = { G_IM_FMT_RGBA, G_IM_FMT_RGBA, G_IM_FMT_RGBA, G_IM_FMT_RGBA, G_IM_FMT_IA, G_IM_FMT_IA, G_IM_FMT_IA, G_IM_FMT_I, G_IM_FMT_I, G_IM_FMT_CI, G_IM_FMT_CI, G_IM_FMT_CI, G_IM_FMT_CI };

s32 var800842bc[] = { 3, 2, 3, 2, 2, 1, 0, 1, 0, 1, 0, 1, 0 };

s32 var800842f0[] = { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x8000, 0x8000, 0xc000, 0xc000 };

void func0f16e810(u32 arg0)
{
	// empty
}

/**
 * Inflate images (levels of detail) from a zlib-compressed texture.
 *
 * Zlib-compressed textures are always paletted and always use 16-bit colours.
 * The texture header contains palette information, then each image follows with
 * its own header and zlib compresed data.
 *
 * The texture header is:
 *
 * ffffffff nnnnnnnn [palette]
 *
 * f = pixel format (see TEXFORMAT constants)
 * n = number of colours in the palette minus 1
 * [palette] = 16 bits * number of colours
 *
 * Each images's header is:
 *
 * wwwwwwww hhhhhhhh [data]
 *
 * w = width in pixels
 * h = height in pixels
 * [data] = zlib compressed list of indices into the palette
 *
 * The zlib data is prefixed with the standard 5-byte rarezip header.
 */
s32 texInflateZlib(u8 *src, u8 *dst, s32 arg2, s32 forcenumimages, struct texturething *arg4, s32 arg5)
{
	s32 i;
	s32 imagebytesout;
	s32 numimages;
	bool writetocache;
	s32 format;
	bool foundthething;
	s32 totalbytesout;
	s32 width;
	s32 height;
	s32 numcolours;
	u8 *end;
	u8 *start;
	s32 j;
	u8 scratch2[0x800];
	u16 palette[256];
	u8 scratch[5120];

	writetocache = false;
	totalbytesout = 0;

	texSetBitstring(src);

	if (arg2 && forcenumimages) {
		numimages = forcenumimages;
	} else {
		numimages = 1;
	}

	arg4->unk0c->unk0b_00 = forcenumimages;
	arg4->unk0c->unk0c_02 = arg2;

	if (arg2) {
		writetocache = true;

		for (i = 0; i < g_TexCacheCount; i++) {
			if (g_TexCacheItems[i].texturenum == arg4->unk0c->texturenum) {
				writetocache = false;
			}
		}
	}

	format = texReadBits(8);
	numcolours = texReadBits(8) + 1;

	for (i = 0; i < numcolours; i++) {
		palette[i] = texReadBits(16);
	}

	foundthething = false;

	for (j = 0; j < numimages; j++) {
		width = texReadBits(8);
		height = texReadBits(8);

		if (j == 0) {
			arg4->unk0c->width = width;
			arg4->unk0c->height = height;
			arg4->unk0c->unk0a = numcolours - 1;
			arg4->unk0c->gbiformat = g_TexFormatGbiMappings[format];
			arg4->unk0c->unk0b_06 = var800842bc[format];
			arg4->unk0c->unk0c_00 = var800842f0[format] >> 14;
		} else if (writetocache) {
			g_TexCacheItems[g_TexCacheCount].widths[j - 1] = width;
			g_TexCacheItems[g_TexCacheCount].heights[j - 1] = height;
		}

		if (rzipInflate(var800ab540, scratch2, scratch) == 0) {
#if VERSION < VERSION_NTSC_1_0
			char message[128];
			sprintf(message, "DMA-Crash %s %d Ram: %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
					"texdecompress.c", 357,
					var800ab540[0], var800ab540[1], var800ab540[2], var800ab540[3],
					var800ab540[4], var800ab540[5], var800ab540[6], var800ab540[7],
					var800ab540[8], var800ab540[9], var800ab540[10], var800ab540[11],
					var800ab540[12], var800ab540[13], var800ab540[14], var800ab540[15]);
			crashSetMessage(message);
			CRASH();
#endif
		}

		imagebytesout = func0f16edac(scratch2, width, height, format, &dst[totalbytesout]);
		texSetBitstring(rzipGetSomething());

		if (arg2 == 1) {
			if (IS4MB() && j == 2 && !foundthething) {
				arg4->unk0c->unk0b_00 = j;
				foundthething = true;
			}

			if (totalbytesout + imagebytesout > 0x800 || foundthething) {
				if (!foundthething) {
					arg4->unk0c->unk0b_00 = j;
					foundthething = true;
				}
			} else {
				func0f1729f8(&dst[totalbytesout], width, height, format);
				totalbytesout += imagebytesout;
			}
		} else {
			totalbytesout += imagebytesout;
		}
	}

	if (writetocache) {
		g_TexCacheItems[g_TexCacheCount].texturenum = arg4->unk0c->texturenum;

		g_TexCacheCount++;

		if (g_TexCacheCount >= ARRAYCOUNT(g_TexCacheItems)) {
			g_TexCacheCount = 0;
		}
	}

	if (!arg2) {
		if (forcenumimages >= 2) {
			s32 tmpwidth = width;
			s32 tmpheight = height;

			start = dst;
			end = &dst[totalbytesout];

			for (j = 1; j < forcenumimages; j++) {
				imagebytesout = func0f16f0f4(start, end, tmpwidth, tmpheight, format, palette, numcolours);

				if (IS4MB() && j == 2) {
					arg4->unk0c->unk0b_00 = j;
					break;
				}

				if (totalbytesout + imagebytesout > 0x800) {
					arg4->unk0c->unk0b_00 = j;
					break;
				}

				func0f1729f8(start, tmpwidth, tmpheight, format);

				totalbytesout += imagebytesout;

				tmpwidth = (tmpwidth + 1) >> 1;
				tmpheight = (tmpheight + 1) >> 1;

				start = end;
				end += imagebytesout;
			}

			func0f1729f8(start, tmpwidth, tmpheight, format);
		} else {
			func0f1729f8(dst, width, height, format);
		}
	}

	for (i = 0; i < numcolours; i++) {
		dst[totalbytesout + 0] = palette[i] >> 8;
		dst[totalbytesout + 1] = palette[i] & 0xff;
		totalbytesout += 2;
	}

	if (numcolours & 1) {
#if VERSION < VERSION_NTSC_1_0
		u32 stack;
#endif

		dst[totalbytesout + 0] = dst[totalbytesout - 2];
		dst[totalbytesout + 1] = dst[totalbytesout - 1];
		totalbytesout += 2;
		arg4->unk0c->unk0a++;
	}

	totalbytesout = (totalbytesout + 7) & ~7;

	return totalbytesout;
}

GLOBAL_ASM(
glabel func0f16edac
/*  f16edac:	27bdffe0 */ 	addiu	$sp,$sp,-32
/*  f16edb0:	afb1000c */ 	sw	$s1,0xc($sp)
/*  f16edb4:	afb00008 */ 	sw	$s0,0x8($sp)
/*  f16edb8:	24010009 */ 	addiu	$at,$zero,0x9
/*  f16edbc:	00a08025 */ 	or	$s0,$a1,$zero
/*  f16edc0:	00808825 */ 	or	$s1,$a0,$zero
/*  f16edc4:	10e10003 */ 	beq	$a3,$at,.L0f16edd4
/*  f16edc8:	8fa30030 */ 	lw	$v1,0x30($sp)
/*  f16edcc:	2401000b */ 	addiu	$at,$zero,0xb
/*  f16edd0:	14e10003 */ 	bne	$a3,$at,.L0f16ede0
.L0f16edd4:
/*  f16edd4:	24020001 */ 	addiu	$v0,$zero,0x1
/*  f16edd8:	10000008 */ 	b	.L0f16edfc
/*  f16eddc:	afa20010 */ 	sw	$v0,0x10($sp)
.L0f16ede0:
/*  f16ede0:	2401000a */ 	addiu	$at,$zero,0xa
/*  f16ede4:	10e10004 */ 	beq	$a3,$at,.L0f16edf8
/*  f16ede8:	24020002 */ 	addiu	$v0,$zero,0x2
/*  f16edec:	2401000c */ 	addiu	$at,$zero,0xc
/*  f16edf0:	54e10003 */ 	bnel	$a3,$at,.L0f16ee00
/*  f16edf4:	8fa20010 */ 	lw	$v0,0x10($sp)
.L0f16edf8:
/*  f16edf8:	afa20010 */ 	sw	$v0,0x10($sp)
.L0f16edfc:
/*  f16edfc:	8fa20010 */ 	lw	$v0,0x10($sp)
.L0f16ee00:
/*  f16ee00:	18c0000f */ 	blez	$a2,.L0f16ee40
/*  f16ee04:	00002025 */ 	or	$a0,$zero,$zero
/*  f16ee08:	2407fff8 */ 	addiu	$a3,$zero,-8
.L0f16ee0c:
/*  f16ee0c:	1a000008 */ 	blez	$s0,.L0f16ee30
/*  f16ee10:	00002825 */ 	or	$a1,$zero,$zero
.L0f16ee14:
/*  f16ee14:	922e0000 */ 	lbu	$t6,0x0($s1)
/*  f16ee18:	00a22821 */ 	addu	$a1,$a1,$v0
/*  f16ee1c:	00b0082a */ 	slt	$at,$a1,$s0
/*  f16ee20:	24630001 */ 	addiu	$v1,$v1,0x1
/*  f16ee24:	26310001 */ 	addiu	$s1,$s1,0x1
/*  f16ee28:	1420fffa */ 	bnez	$at,.L0f16ee14
/*  f16ee2c:	a06effff */ 	sb	$t6,-0x1($v1)
.L0f16ee30:
/*  f16ee30:	24840001 */ 	addiu	$a0,$a0,0x1
/*  f16ee34:	246f0007 */ 	addiu	$t7,$v1,0x7
/*  f16ee38:	1486fff4 */ 	bne	$a0,$a2,.L0f16ee0c
/*  f16ee3c:	01e71824 */ 	and	$v1,$t7,$a3
.L0f16ee40:
/*  f16ee40:	8fb80030 */ 	lw	$t8,0x30($sp)
/*  f16ee44:	8fb00008 */ 	lw	$s0,0x8($sp)
/*  f16ee48:	8fb1000c */ 	lw	$s1,0xc($sp)
/*  f16ee4c:	27bd0020 */ 	addiu	$sp,$sp,0x20
/*  f16ee50:	03e00008 */ 	jr	$ra
/*  f16ee54:	00781023 */ 	subu	$v0,$v1,$t8
);

GLOBAL_ASM(
glabel func0f16ee58
/*  f16ee58:	000462c3 */ 	sra	$t4,$a0,0xb
/*  f16ee5c:	318d001f */ 	andi	$t5,$t4,0x1f
/*  f16ee60:	000d70c0 */ 	sll	$t6,$t5,0x3
/*  f16ee64:	00047b43 */ 	sra	$t7,$a0,0xd
/*  f16ee68:	31f80007 */ 	andi	$t8,$t7,0x7
/*  f16ee6c:	01d8c825 */ 	or	$t9,$t6,$t8
/*  f16ee70:	000562c3 */ 	sra	$t4,$a1,0xb
/*  f16ee74:	318d001f */ 	andi	$t5,$t4,0x1f
/*  f16ee78:	00057343 */ 	sra	$t6,$a1,0xd
/*  f16ee7c:	31d80007 */ 	andi	$t8,$t6,0x7
/*  f16ee80:	000d78c0 */ 	sll	$t7,$t5,0x3
/*  f16ee84:	01f86025 */ 	or	$t4,$t7,$t8
/*  f16ee88:	032c6821 */ 	addu	$t5,$t9,$t4
/*  f16ee8c:	000672c3 */ 	sra	$t6,$a2,0xb
/*  f16ee90:	31cf001f */ 	andi	$t7,$t6,0x1f
/*  f16ee94:	0006cb43 */ 	sra	$t9,$a2,0xd
/*  f16ee98:	332c0007 */ 	andi	$t4,$t9,0x7
/*  f16ee9c:	000fc0c0 */ 	sll	$t8,$t7,0x3
/*  f16eea0:	030c7025 */ 	or	$t6,$t8,$t4
/*  f16eea4:	01ae7821 */ 	addu	$t7,$t5,$t6
/*  f16eea8:	0007cac3 */ 	sra	$t9,$a3,0xb
/*  f16eeac:	3338001f */ 	andi	$t8,$t9,0x1f
/*  f16eeb0:	00076b43 */ 	sra	$t5,$a3,0xd
/*  f16eeb4:	31ae0007 */ 	andi	$t6,$t5,0x7
/*  f16eeb8:	001860c0 */ 	sll	$t4,$t8,0x3
/*  f16eebc:	018ec825 */ 	or	$t9,$t4,$t6
/*  f16eec0:	01f95821 */ 	addu	$t3,$t7,$t9
/*  f16eec4:	256b0002 */ 	addiu	$t3,$t3,0x2
/*  f16eec8:	000b1883 */ 	sra	$v1,$t3,0x2
/*  f16eecc:	afa40000 */ 	sw	$a0,0x0($sp)
/*  f16eed0:	afa50004 */ 	sw	$a1,0x4($sp)
/*  f16eed4:	afa60008 */ 	sw	$a2,0x8($sp)
/*  f16eed8:	04610002 */ 	bgez	$v1,.L0f16eee4
/*  f16eedc:	afa7000c */ 	sw	$a3,0xc($sp)
/*  f16eee0:	00001825 */ 	or	$v1,$zero,$zero
.L0f16eee4:
/*  f16eee4:	28610100 */ 	slti	$at,$v1,0x100
/*  f16eee8:	14200002 */ 	bnez	$at,.L0f16eef4
/*  f16eeec:	00000000 */ 	nop
/*  f16eef0:	240300ff */ 	addiu	$v1,$zero,0xff
.L0f16eef4:
/*  f16eef4:	03e00008 */ 	jr	$ra
/*  f16eef8:	00601025 */ 	or	$v0,$v1,$zero
);

GLOBAL_ASM(
glabel func0f16eefc
/*  f16eefc:	00046183 */ 	sra	$t4,$a0,0x6
/*  f16ef00:	318d001f */ 	andi	$t5,$t4,0x1f
/*  f16ef04:	000d70c0 */ 	sll	$t6,$t5,0x3
/*  f16ef08:	00047a03 */ 	sra	$t7,$a0,0x8
/*  f16ef0c:	31f80007 */ 	andi	$t8,$t7,0x7
/*  f16ef10:	01d8c825 */ 	or	$t9,$t6,$t8
/*  f16ef14:	00056183 */ 	sra	$t4,$a1,0x6
/*  f16ef18:	318d001f */ 	andi	$t5,$t4,0x1f
/*  f16ef1c:	00057203 */ 	sra	$t6,$a1,0x8
/*  f16ef20:	31d80007 */ 	andi	$t8,$t6,0x7
/*  f16ef24:	000d78c0 */ 	sll	$t7,$t5,0x3
/*  f16ef28:	01f86025 */ 	or	$t4,$t7,$t8
/*  f16ef2c:	032c6821 */ 	addu	$t5,$t9,$t4
/*  f16ef30:	00067183 */ 	sra	$t6,$a2,0x6
/*  f16ef34:	31cf001f */ 	andi	$t7,$t6,0x1f
/*  f16ef38:	0006ca03 */ 	sra	$t9,$a2,0x8
/*  f16ef3c:	332c0007 */ 	andi	$t4,$t9,0x7
/*  f16ef40:	000fc0c0 */ 	sll	$t8,$t7,0x3
/*  f16ef44:	030c7025 */ 	or	$t6,$t8,$t4
/*  f16ef48:	01ae7821 */ 	addu	$t7,$t5,$t6
/*  f16ef4c:	0007c983 */ 	sra	$t9,$a3,0x6
/*  f16ef50:	3338001f */ 	andi	$t8,$t9,0x1f
/*  f16ef54:	00076a03 */ 	sra	$t5,$a3,0x8
/*  f16ef58:	31ae0007 */ 	andi	$t6,$t5,0x7
/*  f16ef5c:	001860c0 */ 	sll	$t4,$t8,0x3
/*  f16ef60:	018ec825 */ 	or	$t9,$t4,$t6
/*  f16ef64:	01f95821 */ 	addu	$t3,$t7,$t9
/*  f16ef68:	256b0002 */ 	addiu	$t3,$t3,0x2
/*  f16ef6c:	000b1883 */ 	sra	$v1,$t3,0x2
/*  f16ef70:	afa40000 */ 	sw	$a0,0x0($sp)
/*  f16ef74:	afa50004 */ 	sw	$a1,0x4($sp)
/*  f16ef78:	afa60008 */ 	sw	$a2,0x8($sp)
/*  f16ef7c:	04610002 */ 	bgez	$v1,.L0f16ef88
/*  f16ef80:	afa7000c */ 	sw	$a3,0xc($sp)
/*  f16ef84:	00001825 */ 	or	$v1,$zero,$zero
.L0f16ef88:
/*  f16ef88:	28610100 */ 	slti	$at,$v1,0x100
/*  f16ef8c:	14200002 */ 	bnez	$at,.L0f16ef98
/*  f16ef90:	00000000 */ 	nop
/*  f16ef94:	240300ff */ 	addiu	$v1,$zero,0xff
.L0f16ef98:
/*  f16ef98:	03e00008 */ 	jr	$ra
/*  f16ef9c:	00601025 */ 	or	$v0,$v1,$zero
);

GLOBAL_ASM(
glabel func0f16efa0
/*  f16efa0:	00046043 */ 	sra	$t4,$a0,0x1
/*  f16efa4:	318d001f */ 	andi	$t5,$t4,0x1f
/*  f16efa8:	000d70c0 */ 	sll	$t6,$t5,0x3
/*  f16efac:	000478c3 */ 	sra	$t7,$a0,0x3
/*  f16efb0:	31f80007 */ 	andi	$t8,$t7,0x7
/*  f16efb4:	01d8c825 */ 	or	$t9,$t6,$t8
/*  f16efb8:	00056043 */ 	sra	$t4,$a1,0x1
/*  f16efbc:	318d001f */ 	andi	$t5,$t4,0x1f
/*  f16efc0:	000570c3 */ 	sra	$t6,$a1,0x3
/*  f16efc4:	31d80007 */ 	andi	$t8,$t6,0x7
/*  f16efc8:	000d78c0 */ 	sll	$t7,$t5,0x3
/*  f16efcc:	01f86025 */ 	or	$t4,$t7,$t8
/*  f16efd0:	032c6821 */ 	addu	$t5,$t9,$t4
/*  f16efd4:	00067043 */ 	sra	$t6,$a2,0x1
/*  f16efd8:	31cf001f */ 	andi	$t7,$t6,0x1f
/*  f16efdc:	0006c8c3 */ 	sra	$t9,$a2,0x3
/*  f16efe0:	332c0007 */ 	andi	$t4,$t9,0x7
/*  f16efe4:	000fc0c0 */ 	sll	$t8,$t7,0x3
/*  f16efe8:	030c7025 */ 	or	$t6,$t8,$t4
/*  f16efec:	01ae7821 */ 	addu	$t7,$t5,$t6
/*  f16eff0:	0007c843 */ 	sra	$t9,$a3,0x1
/*  f16eff4:	3338001f */ 	andi	$t8,$t9,0x1f
/*  f16eff8:	000768c3 */ 	sra	$t5,$a3,0x3
/*  f16effc:	31ae0007 */ 	andi	$t6,$t5,0x7
/*  f16f000:	001860c0 */ 	sll	$t4,$t8,0x3
/*  f16f004:	018ec825 */ 	or	$t9,$t4,$t6
/*  f16f008:	01f95821 */ 	addu	$t3,$t7,$t9
/*  f16f00c:	256b0002 */ 	addiu	$t3,$t3,0x2
/*  f16f010:	000b1883 */ 	sra	$v1,$t3,0x2
/*  f16f014:	afa40000 */ 	sw	$a0,0x0($sp)
/*  f16f018:	afa50004 */ 	sw	$a1,0x4($sp)
/*  f16f01c:	afa60008 */ 	sw	$a2,0x8($sp)
/*  f16f020:	04610002 */ 	bgez	$v1,.L0f16f02c
/*  f16f024:	afa7000c */ 	sw	$a3,0xc($sp)
/*  f16f028:	00001825 */ 	or	$v1,$zero,$zero
.L0f16f02c:
/*  f16f02c:	28610100 */ 	slti	$at,$v1,0x100
/*  f16f030:	14200002 */ 	bnez	$at,.L0f16f03c
/*  f16f034:	00000000 */ 	nop
/*  f16f038:	240300ff */ 	addiu	$v1,$zero,0xff
.L0f16f03c:
/*  f16f03c:	03e00008 */ 	jr	$ra
/*  f16f040:	00601025 */ 	or	$v0,$v1,$zero
);

GLOBAL_ASM(
glabel func0f16f044
/*  f16f044:	afa40000 */ 	sw	$a0,0x0($sp)
/*  f16f048:	308effff */ 	andi	$t6,$a0,0xffff
/*  f16f04c:	afa50004 */ 	sw	$a1,0x4($sp)
/*  f16f050:	30afffff */ 	andi	$t7,$a1,0xffff
/*  f16f054:	afa7000c */ 	sw	$a3,0xc($sp)
/*  f16f058:	30f9ffff */ 	andi	$t9,$a3,0xffff
/*  f16f05c:	30c80001 */ 	andi	$t0,$a2,0x1
/*  f16f060:	03203825 */ 	or	$a3,$t9,$zero
/*  f16f064:	01e02825 */ 	or	$a1,$t7,$zero
/*  f16f068:	01c02025 */ 	or	$a0,$t6,$zero
/*  f16f06c:	11000003 */ 	beqz	$t0,.L0f16f07c
/*  f16f070:	afa60008 */ 	sw	$a2,0x8($sp)
/*  f16f074:	10000002 */ 	b	.L0f16f080
/*  f16f078:	240200ff */ 	addiu	$v0,$zero,0xff
.L0f16f07c:
/*  f16f07c:	00001025 */ 	or	$v0,$zero,$zero
.L0f16f080:
/*  f16f080:	30a90001 */ 	andi	$t1,$a1,0x1
/*  f16f084:	11200003 */ 	beqz	$t1,.L0f16f094
/*  f16f088:	308a0001 */ 	andi	$t2,$a0,0x1
/*  f16f08c:	10000002 */ 	b	.L0f16f098
/*  f16f090:	240300ff */ 	addiu	$v1,$zero,0xff
.L0f16f094:
/*  f16f094:	00001825 */ 	or	$v1,$zero,$zero
.L0f16f098:
/*  f16f098:	11400003 */ 	beqz	$t2,.L0f16f0a8
/*  f16f09c:	30eb0001 */ 	andi	$t3,$a3,0x1
/*  f16f0a0:	10000002 */ 	b	.L0f16f0ac
/*  f16f0a4:	240400ff */ 	addiu	$a0,$zero,0xff
.L0f16f0a8:
/*  f16f0a8:	00002025 */ 	or	$a0,$zero,$zero
.L0f16f0ac:
/*  f16f0ac:	11600003 */ 	beqz	$t3,.L0f16f0bc
/*  f16f0b0:	00002825 */ 	or	$a1,$zero,$zero
/*  f16f0b4:	10000001 */ 	b	.L0f16f0bc
/*  f16f0b8:	240500ff */ 	addiu	$a1,$zero,0xff
.L0f16f0bc:
/*  f16f0bc:	00a46021 */ 	addu	$t4,$a1,$a0
/*  f16f0c0:	01836821 */ 	addu	$t5,$t4,$v1
/*  f16f0c4:	01a23821 */ 	addu	$a3,$t5,$v0
/*  f16f0c8:	24e70002 */ 	addiu	$a3,$a3,0x2
/*  f16f0cc:	00073083 */ 	sra	$a2,$a3,0x2
/*  f16f0d0:	04c30003 */ 	bgezl	$a2,.L0f16f0e0
/*  f16f0d4:	28c10100 */ 	slti	$at,$a2,0x100
/*  f16f0d8:	00003025 */ 	or	$a2,$zero,$zero
/*  f16f0dc:	28c10100 */ 	slti	$at,$a2,0x100
.L0f16f0e0:
/*  f16f0e0:	14200002 */ 	bnez	$at,.L0f16f0ec
/*  f16f0e4:	00000000 */ 	nop
/*  f16f0e8:	240600ff */ 	addiu	$a2,$zero,0xff
.L0f16f0ec:
/*  f16f0ec:	03e00008 */ 	jr	$ra
/*  f16f0f0:	00c01025 */ 	or	$v0,$a2,$zero
);

GLOBAL_ASM(
glabel func0f16f0f4
/*  f16f0f4:	27bdfb50 */ 	addiu	$sp,$sp,-1200
/*  f16f0f8:	afb00020 */ 	sw	$s0,0x20($sp)
/*  f16f0fc:	8fb004c0 */ 	lw	$s0,0x4c0($sp)
/*  f16f100:	24ef0001 */ 	addiu	$t7,$a3,0x1
/*  f16f104:	afb20028 */ 	sw	$s2,0x28($sp)
/*  f16f108:	afb10024 */ 	sw	$s1,0x24($sp)
/*  f16f10c:	24080009 */ 	addiu	$t0,$zero,0x9
/*  f16f110:	000fc043 */ 	sra	$t8,$t7,0x1
/*  f16f114:	00a08825 */ 	or	$s1,$a1,$zero
/*  f16f118:	00809025 */ 	or	$s2,$a0,$zero
/*  f16f11c:	afbf0044 */ 	sw	$ra,0x44($sp)
/*  f16f120:	afbe0040 */ 	sw	$s8,0x40($sp)
/*  f16f124:	afb7003c */ 	sw	$s7,0x3c($sp)
/*  f16f128:	afb60038 */ 	sw	$s6,0x38($sp)
/*  f16f12c:	afb50034 */ 	sw	$s5,0x34($sp)
/*  f16f130:	afb40030 */ 	sw	$s4,0x30($sp)
/*  f16f134:	afb3002c */ 	sw	$s3,0x2c($sp)
/*  f16f138:	afa604b8 */ 	sw	$a2,0x4b8($sp)
/*  f16f13c:	afa704bc */ 	sw	$a3,0x4bc($sp)
/*  f16f140:	afb80068 */ 	sw	$t8,0x68($sp)
/*  f16f144:	1208000a */ 	beq	$s0,$t0,.L0f16f170
/*  f16f148:	afb8049c */ 	sw	$t8,0x49c($sp)
/*  f16f14c:	2409000a */ 	addiu	$t1,$zero,0xa
/*  f16f150:	12090012 */ 	beq	$s0,$t1,.L0f16f19c
/*  f16f154:	2401000b */ 	addiu	$at,$zero,0xb
/*  f16f158:	12010006 */ 	beq	$s0,$at,.L0f16f174
/*  f16f15c:	2401000c */ 	addiu	$at,$zero,0xc
/*  f16f160:	5201000f */ 	beql	$s0,$at,.L0f16f1a0
/*  f16f164:	8fab04b8 */ 	lw	$t3,0x4b8($sp)
/*  f16f168:	10000015 */ 	b	.L0f16f1c0
/*  f16f16c:	00000000 */ 	nop
.L0f16f170:
/*  f16f170:	2409000a */ 	addiu	$t1,$zero,0xa
.L0f16f174:
/*  f16f174:	8fb904b8 */ 	lw	$t9,0x4b8($sp)
/*  f16f178:	272b0001 */ 	addiu	$t3,$t9,0x1
/*  f16f17c:	000b6043 */ 	sra	$t4,$t3,0x1
/*  f16f180:	258d0007 */ 	addiu	$t5,$t4,0x7
/*  f16f184:	272f0007 */ 	addiu	$t7,$t9,0x7
/*  f16f188:	31ae0ff8 */ 	andi	$t6,$t5,0xff8
/*  f16f18c:	31f80ff8 */ 	andi	$t8,$t7,0xff8
/*  f16f190:	afae04a0 */ 	sw	$t6,0x4a0($sp)
/*  f16f194:	1000000a */ 	b	.L0f16f1c0
/*  f16f198:	afb804a4 */ 	sw	$t8,0x4a4($sp)
.L0f16f19c:
/*  f16f19c:	8fab04b8 */ 	lw	$t3,0x4b8($sp)
.L0f16f1a0:
/*  f16f1a0:	256c0001 */ 	addiu	$t4,$t3,0x1
/*  f16f1a4:	000c6843 */ 	sra	$t5,$t4,0x1
/*  f16f1a8:	25ae000f */ 	addiu	$t6,$t5,0xf
/*  f16f1ac:	256f000f */ 	addiu	$t7,$t3,0xf
/*  f16f1b0:	31d90ff0 */ 	andi	$t9,$t6,0xff0
/*  f16f1b4:	31f80ff0 */ 	andi	$t8,$t7,0xff0
/*  f16f1b8:	afb904a0 */ 	sw	$t9,0x4a0($sp)
/*  f16f1bc:	afb804a4 */ 	sw	$t8,0x4a4($sp)
.L0f16f1c0:
/*  f16f1c0:	12080003 */ 	beq	$s0,$t0,.L0f16f1d0
/*  f16f1c4:	8fac04c8 */ 	lw	$t4,0x4c8($sp)
/*  f16f1c8:	56090024 */ 	bnel	$s0,$t1,.L0f16f25c
/*  f16f1cc:	afb10474 */ 	sw	$s1,0x474($sp)
.L0f16f1d0:
/*  f16f1d0:	19800021 */ 	blez	$t4,.L0f16f258
/*  f16f1d4:	00003025 */ 	or	$a2,$zero,$zero
/*  f16f1d8:	8fa404c4 */ 	lw	$a0,0x4c4($sp)
/*  f16f1dc:	27a30070 */ 	addiu	$v1,$sp,0x70
/*  f16f1e0:	240700ff */ 	addiu	$a3,$zero,0xff
.L0f16f1e4:
/*  f16f1e4:	94950000 */ 	lhu	$s5,0x0($a0)
/*  f16f1e8:	001572c3 */ 	sra	$t6,$s5,0xb
/*  f16f1ec:	00155b43 */ 	sra	$t3,$s5,0xd
/*  f16f1f0:	316f0007 */ 	andi	$t7,$t3,0x7
/*  f16f1f4:	000ec8c0 */ 	sll	$t9,$t6,0x3
/*  f16f1f8:	032fc025 */ 	or	$t8,$t9,$t7
/*  f16f1fc:	00155a03 */ 	sra	$t3,$s5,0x8
/*  f16f200:	00156983 */ 	sra	$t5,$s5,0x6
/*  f16f204:	000d70c0 */ 	sll	$t6,$t5,0x3
/*  f16f208:	31790007 */ 	andi	$t9,$t3,0x7
/*  f16f20c:	01d97825 */ 	or	$t7,$t6,$t9
/*  f16f210:	001558c3 */ 	sra	$t3,$s5,0x3
/*  f16f214:	00156043 */ 	sra	$t4,$s5,0x1
/*  f16f218:	a06f0001 */ 	sb	$t7,0x1($v1)
/*  f16f21c:	000c68c0 */ 	sll	$t5,$t4,0x3
/*  f16f220:	316e0007 */ 	andi	$t6,$t3,0x7
/*  f16f224:	01aec825 */ 	or	$t9,$t5,$t6
/*  f16f228:	32af0001 */ 	andi	$t7,$s5,0x1
/*  f16f22c:	a0780000 */ 	sb	$t8,0x0($v1)
/*  f16f230:	11e00003 */ 	beqz	$t7,.L0f16f240
/*  f16f234:	a0790002 */ 	sb	$t9,0x2($v1)
/*  f16f238:	10000002 */ 	b	.L0f16f244
/*  f16f23c:	a0670003 */ 	sb	$a3,0x3($v1)
.L0f16f240:
/*  f16f240:	a0600003 */ 	sb	$zero,0x3($v1)
.L0f16f244:
/*  f16f244:	8fb804c8 */ 	lw	$t8,0x4c8($sp)
/*  f16f248:	24c60001 */ 	addiu	$a2,$a2,0x1
/*  f16f24c:	24840002 */ 	addiu	$a0,$a0,0x2
/*  f16f250:	14d8ffe4 */ 	bne	$a2,$t8,.L0f16f1e4
/*  f16f254:	24630004 */ 	addiu	$v1,$v1,0x4
.L0f16f258:
/*  f16f258:	afb10474 */ 	sw	$s1,0x474($sp)
.L0f16f25c:
/*  f16f25c:	12080009 */ 	beq	$s0,$t0,.L0f16f284
/*  f16f260:	afb20470 */ 	sw	$s2,0x470($sp)
/*  f16f264:	120900e8 */ 	beq	$s0,$t1,.L0f16f608
/*  f16f268:	2401000b */ 	addiu	$at,$zero,0xb
/*  f16f26c:	12010077 */ 	beq	$s0,$at,.L0f16f44c
/*  f16f270:	2401000c */ 	addiu	$at,$zero,0xc
/*  f16f274:	120101b6 */ 	beq	$s0,$at,.L0f16f950
/*  f16f278:	00001025 */ 	or	$v0,$zero,$zero
/*  f16f27c:	10000277 */ 	b	.L0f16fc5c
/*  f16f280:	00000000 */ 	nop
.L0f16f284:
/*  f16f284:	8fac04bc */ 	lw	$t4,0x4bc($sp)
/*  f16f288:	8fab04a4 */ 	lw	$t3,0x4a4($sp)
/*  f16f28c:	afa004a8 */ 	sw	$zero,0x4a8($sp)
/*  f16f290:	19800068 */ 	blez	$t4,.L0f16f434
/*  f16f294:	000b6840 */ 	sll	$t5,$t3,0x1
/*  f16f298:	afad0058 */ 	sw	$t5,0x58($sp)
.L0f16f29c:
/*  f16f29c:	8fb904a8 */ 	lw	$t9,0x4a8($sp)
/*  f16f2a0:	8fae04bc */ 	lw	$t6,0x4bc($sp)
/*  f16f2a4:	0000b825 */ 	or	$s7,$zero,$zero
/*  f16f2a8:	272f0001 */ 	addiu	$t7,$t9,0x1
/*  f16f2ac:	01ee082a */ 	slt	$at,$t7,$t6
/*  f16f2b0:	10200003 */ 	beqz	$at,.L0f16f2c0
/*  f16f2b4:	8fb804a4 */ 	lw	$t8,0x4a4($sp)
/*  f16f2b8:	10000002 */ 	b	.L0f16f2c4
/*  f16f2bc:	afb80480 */ 	sw	$t8,0x480($sp)
.L0f16f2c0:
/*  f16f2c0:	afa00480 */ 	sw	$zero,0x480($sp)
.L0f16f2c4:
/*  f16f2c4:	8fac04a4 */ 	lw	$t4,0x4a4($sp)
/*  f16f2c8:	8fbe0470 */ 	lw	$s8,0x470($sp)
/*  f16f2cc:	8fad0480 */ 	lw	$t5,0x480($sp)
/*  f16f2d0:	1980004a */ 	blez	$t4,.L0f16f3fc
/*  f16f2d4:	01bec821 */ 	addu	$t9,$t5,$s8
/*  f16f2d8:	afb9005c */ 	sw	$t9,0x5c($sp)
.L0f16f2dc:
/*  f16f2dc:	8fae04b8 */ 	lw	$t6,0x4b8($sp)
/*  f16f2e0:	26e20001 */ 	addiu	$v0,$s7,0x1
/*  f16f2e4:	02e01825 */ 	or	$v1,$s7,$zero
/*  f16f2e8:	004e082a */ 	slt	$at,$v0,$t6
/*  f16f2ec:	10200003 */ 	beqz	$at,.L0f16f2fc
/*  f16f2f0:	8fab0470 */ 	lw	$t3,0x470($sp)
/*  f16f2f4:	10000001 */ 	b	.L0f16f2fc
/*  f16f2f8:	00401825 */ 	or	$v1,$v0,$zero
.L0f16f2fc:
/*  f16f2fc:	93d80000 */ 	lbu	$t8,0x0($s8)
/*  f16f300:	8faf04c4 */ 	lw	$t7,0x4c4($sp)
/*  f16f304:	0163c821 */ 	addu	$t9,$t3,$v1
/*  f16f308:	00186040 */ 	sll	$t4,$t8,0x1
/*  f16f30c:	932e0000 */ 	lbu	$t6,0x0($t9)
/*  f16f310:	01ec6821 */ 	addu	$t5,$t7,$t4
/*  f16f314:	95b50000 */ 	lhu	$s5,0x0($t5)
/*  f16f318:	8fad005c */ 	lw	$t5,0x5c($sp)
/*  f16f31c:	000ec040 */ 	sll	$t8,$t6,0x1
/*  f16f320:	01f86021 */ 	addu	$t4,$t7,$t8
/*  f16f324:	95910000 */ 	lhu	$s1,0x0($t4)
/*  f16f328:	91b90000 */ 	lbu	$t9,0x0($t5)
/*  f16f32c:	8fac0480 */ 	lw	$t4,0x480($sp)
/*  f16f330:	02a02025 */ 	or	$a0,$s5,$zero
/*  f16f334:	00197040 */ 	sll	$t6,$t9,0x1
/*  f16f338:	01836821 */ 	addu	$t5,$t4,$v1
/*  f16f33c:	01abc821 */ 	addu	$t9,$t5,$t3
/*  f16f340:	01eec021 */ 	addu	$t8,$t7,$t6
/*  f16f344:	932e0000 */ 	lbu	$t6,0x0($t9)
/*  f16f348:	97120000 */ 	lhu	$s2,0x0($t8)
/*  f16f34c:	02202825 */ 	or	$a1,$s1,$zero
/*  f16f350:	000ec040 */ 	sll	$t8,$t6,0x1
/*  f16f354:	01f86021 */ 	addu	$t4,$t7,$t8
/*  f16f358:	95900000 */ 	lhu	$s0,0x0($t4)
/*  f16f35c:	02403025 */ 	or	$a2,$s2,$zero
/*  f16f360:	0fc5bb96 */ 	jal	func0f16ee58
/*  f16f364:	02003825 */ 	or	$a3,$s0,$zero
/*  f16f368:	0040b025 */ 	or	$s6,$v0,$zero
/*  f16f36c:	32a4ffff */ 	andi	$a0,$s5,0xffff
/*  f16f370:	3225ffff */ 	andi	$a1,$s1,0xffff
/*  f16f374:	3246ffff */ 	andi	$a2,$s2,0xffff
/*  f16f378:	0fc5bbbf */ 	jal	func0f16eefc
/*  f16f37c:	3207ffff */ 	andi	$a3,$s0,0xffff
/*  f16f380:	00409825 */ 	or	$s3,$v0,$zero
/*  f16f384:	32a4ffff */ 	andi	$a0,$s5,0xffff
/*  f16f388:	3225ffff */ 	andi	$a1,$s1,0xffff
/*  f16f38c:	3246ffff */ 	andi	$a2,$s2,0xffff
/*  f16f390:	0fc5bbe8 */ 	jal	func0f16efa0
/*  f16f394:	3207ffff */ 	andi	$a3,$s0,0xffff
/*  f16f398:	0040a025 */ 	or	$s4,$v0,$zero
/*  f16f39c:	32a4ffff */ 	andi	$a0,$s5,0xffff
/*  f16f3a0:	3225ffff */ 	andi	$a1,$s1,0xffff
/*  f16f3a4:	3246ffff */ 	andi	$a2,$s2,0xffff
/*  f16f3a8:	0fc5bc11 */ 	jal	func0f16f044
/*  f16f3ac:	3207ffff */ 	andi	$a3,$s0,0xffff
/*  f16f3b0:	27a40070 */ 	addiu	$a0,$sp,0x70
/*  f16f3b4:	8fa504c8 */ 	lw	$a1,0x4c8($sp)
/*  f16f3b8:	02c03025 */ 	or	$a2,$s6,$zero
/*  f16f3bc:	02603825 */ 	or	$a3,$s3,$zero
/*  f16f3c0:	afb40010 */ 	sw	$s4,0x10($sp)
/*  f16f3c4:	0fc5bf23 */ 	jal	func0f16fc8c
/*  f16f3c8:	afa20014 */ 	sw	$v0,0x14($sp)
/*  f16f3cc:	8fad0474 */ 	lw	$t5,0x474($sp)
/*  f16f3d0:	00175843 */ 	sra	$t3,$s7,0x1
/*  f16f3d4:	26f70002 */ 	addiu	$s7,$s7,0x2
/*  f16f3d8:	01abc821 */ 	addu	$t9,$t5,$t3
/*  f16f3dc:	a3220000 */ 	sb	$v0,0x0($t9)
/*  f16f3e0:	8fb804a4 */ 	lw	$t8,0x4a4($sp)
/*  f16f3e4:	8fae005c */ 	lw	$t6,0x5c($sp)
/*  f16f3e8:	27de0002 */ 	addiu	$s8,$s8,0x2
/*  f16f3ec:	02f8082a */ 	slt	$at,$s7,$t8
/*  f16f3f0:	25cf0002 */ 	addiu	$t7,$t6,0x2
/*  f16f3f4:	1420ffb9 */ 	bnez	$at,.L0f16f2dc
/*  f16f3f8:	afaf005c */ 	sw	$t7,0x5c($sp)
.L0f16f3fc:
/*  f16f3fc:	8fac0474 */ 	lw	$t4,0x474($sp)
/*  f16f400:	8fad04a0 */ 	lw	$t5,0x4a0($sp)
/*  f16f404:	8fb804a8 */ 	lw	$t8,0x4a8($sp)
/*  f16f408:	8fb90470 */ 	lw	$t9,0x470($sp)
/*  f16f40c:	018d5821 */ 	addu	$t3,$t4,$t5
/*  f16f410:	8fad04bc */ 	lw	$t5,0x4bc($sp)
/*  f16f414:	8fae0058 */ 	lw	$t6,0x58($sp)
/*  f16f418:	270c0002 */ 	addiu	$t4,$t8,0x2
/*  f16f41c:	018d082a */ 	slt	$at,$t4,$t5
/*  f16f420:	032e7821 */ 	addu	$t7,$t9,$t6
/*  f16f424:	afac04a8 */ 	sw	$t4,0x4a8($sp)
/*  f16f428:	afab0474 */ 	sw	$t3,0x474($sp)
/*  f16f42c:	1420ff9b */ 	bnez	$at,.L0f16f29c
/*  f16f430:	afaf0470 */ 	sw	$t7,0x470($sp)
.L0f16f434:
/*  f16f434:	8fab0068 */ 	lw	$t3,0x68($sp)
/*  f16f438:	8fb904a0 */ 	lw	$t9,0x4a0($sp)
/*  f16f43c:	01790019 */ 	multu	$t3,$t9
/*  f16f440:	00001012 */ 	mflo	$v0
/*  f16f444:	10000206 */ 	b	.L0f16fc60
/*  f16f448:	8fbf0044 */ 	lw	$ra,0x44($sp)
.L0f16f44c:
/*  f16f44c:	8fae04bc */ 	lw	$t6,0x4bc($sp)
/*  f16f450:	8faf04a4 */ 	lw	$t7,0x4a4($sp)
/*  f16f454:	afa004a8 */ 	sw	$zero,0x4a8($sp)
/*  f16f458:	19c00065 */ 	blez	$t6,.L0f16f5f0
/*  f16f45c:	000fc040 */ 	sll	$t8,$t7,0x1
/*  f16f460:	afb80058 */ 	sw	$t8,0x58($sp)
.L0f16f464:
/*  f16f464:	8fad04a8 */ 	lw	$t5,0x4a8($sp)
/*  f16f468:	8fac04bc */ 	lw	$t4,0x4bc($sp)
/*  f16f46c:	0000b825 */ 	or	$s7,$zero,$zero
/*  f16f470:	25ab0001 */ 	addiu	$t3,$t5,0x1
/*  f16f474:	016c082a */ 	slt	$at,$t3,$t4
/*  f16f478:	10200003 */ 	beqz	$at,.L0f16f488
/*  f16f47c:	8fb904a4 */ 	lw	$t9,0x4a4($sp)
/*  f16f480:	10000002 */ 	b	.L0f16f48c
/*  f16f484:	afb90480 */ 	sw	$t9,0x480($sp)
.L0f16f488:
/*  f16f488:	afa00480 */ 	sw	$zero,0x480($sp)
.L0f16f48c:
/*  f16f48c:	8fae04a4 */ 	lw	$t6,0x4a4($sp)
/*  f16f490:	8fbe0470 */ 	lw	$s8,0x470($sp)
/*  f16f494:	8fb80480 */ 	lw	$t8,0x480($sp)
/*  f16f498:	19c00047 */ 	blez	$t6,.L0f16f5b8
/*  f16f49c:	031e6821 */ 	addu	$t5,$t8,$s8
/*  f16f4a0:	afad005c */ 	sw	$t5,0x5c($sp)
.L0f16f4a4:
/*  f16f4a4:	8fac04b8 */ 	lw	$t4,0x4b8($sp)
/*  f16f4a8:	26e20001 */ 	addiu	$v0,$s7,0x1
/*  f16f4ac:	8fab04c4 */ 	lw	$t3,0x4c4($sp)
/*  f16f4b0:	004c082a */ 	slt	$at,$v0,$t4
/*  f16f4b4:	10200003 */ 	beqz	$at,.L0f16f4c4
/*  f16f4b8:	8fa40470 */ 	lw	$a0,0x470($sp)
/*  f16f4bc:	10000002 */ 	b	.L0f16f4c8
/*  f16f4c0:	00401825 */ 	or	$v1,$v0,$zero
.L0f16f4c4:
/*  f16f4c4:	02e01825 */ 	or	$v1,$s7,$zero
.L0f16f4c8:
/*  f16f4c8:	93d90000 */ 	lbu	$t9,0x0($s8)
/*  f16f4cc:	00837821 */ 	addu	$t7,$a0,$v1
/*  f16f4d0:	91ed0000 */ 	lbu	$t5,0x0($t7)
/*  f16f4d4:	00197040 */ 	sll	$t6,$t9,0x1
/*  f16f4d8:	016ec021 */ 	addu	$t8,$t3,$t6
/*  f16f4dc:	8fae005c */ 	lw	$t6,0x5c($sp)
/*  f16f4e0:	000d6040 */ 	sll	$t4,$t5,0x1
/*  f16f4e4:	016cc821 */ 	addu	$t9,$t3,$t4
/*  f16f4e8:	97020000 */ 	lhu	$v0,0x0($t8)
/*  f16f4ec:	8fac0480 */ 	lw	$t4,0x480($sp)
/*  f16f4f0:	91d80000 */ 	lbu	$t8,0x0($t6)
/*  f16f4f4:	97310000 */ 	lhu	$s1,0x0($t9)
/*  f16f4f8:	0183c821 */ 	addu	$t9,$t4,$v1
/*  f16f4fc:	03247021 */ 	addu	$t6,$t9,$a0
/*  f16f500:	00187840 */ 	sll	$t7,$t8,0x1
/*  f16f504:	91d80000 */ 	lbu	$t8,0x0($t6)
/*  f16f508:	016f6821 */ 	addu	$t5,$t3,$t7
/*  f16f50c:	95a80000 */ 	lhu	$t0,0x0($t5)
/*  f16f510:	00187840 */ 	sll	$t7,$t8,0x1
/*  f16f514:	016f6821 */ 	addu	$t5,$t3,$t7
/*  f16f518:	95a90000 */ 	lhu	$t1,0x0($t5)
/*  f16f51c:	00026203 */ 	sra	$t4,$v0,0x8
/*  f16f520:	00117203 */ 	sra	$t6,$s1,0x8
/*  f16f524:	31d800ff */ 	andi	$t8,$t6,0xff
/*  f16f528:	319900ff */ 	andi	$t9,$t4,0xff
/*  f16f52c:	03387821 */ 	addu	$t7,$t9,$t8
/*  f16f530:	01602025 */ 	or	$a0,$t3,$zero
/*  f16f534:	00086a03 */ 	sra	$t5,$t0,0x8
/*  f16f538:	31ab00ff */ 	andi	$t3,$t5,0xff
/*  f16f53c:	00097203 */ 	sra	$t6,$t1,0x8
/*  f16f540:	31d900ff */ 	andi	$t9,$t6,0xff
/*  f16f544:	01eb6021 */ 	addu	$t4,$t7,$t3
/*  f16f548:	01993021 */ 	addu	$a2,$t4,$t9
/*  f16f54c:	0006c083 */ 	sra	$t8,$a2,0x2
/*  f16f550:	304f00ff */ 	andi	$t7,$v0,0xff
/*  f16f554:	322b00ff */ 	andi	$t3,$s1,0xff
/*  f16f558:	01eb7021 */ 	addu	$t6,$t7,$t3
/*  f16f55c:	330600ff */ 	andi	$a2,$t8,0xff
/*  f16f560:	310c00ff */ 	andi	$t4,$t0,0xff
/*  f16f564:	01ccc821 */ 	addu	$t9,$t6,$t4
/*  f16f568:	313800ff */ 	andi	$t8,$t1,0xff
/*  f16f56c:	03383821 */ 	addu	$a3,$t9,$t8
/*  f16f570:	24e70001 */ 	addiu	$a3,$a3,0x1
/*  f16f574:	00076883 */ 	sra	$t5,$a3,0x2
/*  f16f578:	31a700ff */ 	andi	$a3,$t5,0xff
/*  f16f57c:	8fa504c8 */ 	lw	$a1,0x4c8($sp)
/*  f16f580:	0fc5bf54 */ 	jal	func0f16fd50
/*  f16f584:	02201825 */ 	or	$v1,$s1,$zero
/*  f16f588:	8fab0474 */ 	lw	$t3,0x474($sp)
/*  f16f58c:	00177043 */ 	sra	$t6,$s7,0x1
/*  f16f590:	26f70002 */ 	addiu	$s7,$s7,0x2
/*  f16f594:	016e6021 */ 	addu	$t4,$t3,$t6
/*  f16f598:	a1820000 */ 	sb	$v0,0x0($t4)
/*  f16f59c:	8fad04a4 */ 	lw	$t5,0x4a4($sp)
/*  f16f5a0:	8fb9005c */ 	lw	$t9,0x5c($sp)
/*  f16f5a4:	27de0002 */ 	addiu	$s8,$s8,0x2
/*  f16f5a8:	02ed082a */ 	slt	$at,$s7,$t5
/*  f16f5ac:	27380002 */ 	addiu	$t8,$t9,0x2
/*  f16f5b0:	1420ffbc */ 	bnez	$at,.L0f16f4a4
/*  f16f5b4:	afb8005c */ 	sw	$t8,0x5c($sp)
.L0f16f5b8:
/*  f16f5b8:	8faf0474 */ 	lw	$t7,0x474($sp)
/*  f16f5bc:	8fab04a0 */ 	lw	$t3,0x4a0($sp)
/*  f16f5c0:	8fad04a8 */ 	lw	$t5,0x4a8($sp)
/*  f16f5c4:	8fac0470 */ 	lw	$t4,0x470($sp)
/*  f16f5c8:	01eb7021 */ 	addu	$t6,$t7,$t3
/*  f16f5cc:	8fab04bc */ 	lw	$t3,0x4bc($sp)
/*  f16f5d0:	8fb90058 */ 	lw	$t9,0x58($sp)
/*  f16f5d4:	25af0002 */ 	addiu	$t7,$t5,0x2
/*  f16f5d8:	01eb082a */ 	slt	$at,$t7,$t3
/*  f16f5dc:	0199c021 */ 	addu	$t8,$t4,$t9
/*  f16f5e0:	afaf04a8 */ 	sw	$t7,0x4a8($sp)
/*  f16f5e4:	afae0474 */ 	sw	$t6,0x474($sp)
/*  f16f5e8:	1420ff9e */ 	bnez	$at,.L0f16f464
/*  f16f5ec:	afb80470 */ 	sw	$t8,0x470($sp)
.L0f16f5f0:
/*  f16f5f0:	8fae0068 */ 	lw	$t6,0x68($sp)
/*  f16f5f4:	8fac04a0 */ 	lw	$t4,0x4a0($sp)
/*  f16f5f8:	01cc0019 */ 	multu	$t6,$t4
/*  f16f5fc:	00001012 */ 	mflo	$v0
/*  f16f600:	10000197 */ 	b	.L0f16fc60
/*  f16f604:	8fbf0044 */ 	lw	$ra,0x44($sp)
.L0f16f608:
/*  f16f608:	8fb904bc */ 	lw	$t9,0x4bc($sp)
/*  f16f60c:	8fb804a0 */ 	lw	$t8,0x4a0($sp)
/*  f16f610:	afa004a8 */ 	sw	$zero,0x4a8($sp)
/*  f16f614:	1b2000c7 */ 	blez	$t9,.L0f16f934
/*  f16f618:	00186843 */ 	sra	$t5,$t8,0x1
/*  f16f61c:	afad0054 */ 	sw	$t5,0x54($sp)
.L0f16f620:
/*  f16f620:	8fab04a8 */ 	lw	$t3,0x4a8($sp)
/*  f16f624:	8faf04bc */ 	lw	$t7,0x4bc($sp)
/*  f16f628:	0000b825 */ 	or	$s7,$zero,$zero
/*  f16f62c:	256e0001 */ 	addiu	$t6,$t3,0x1
/*  f16f630:	01cf082a */ 	slt	$at,$t6,$t7
/*  f16f634:	10200004 */ 	beqz	$at,.L0f16f648
/*  f16f638:	8fac04a4 */ 	lw	$t4,0x4a4($sp)
/*  f16f63c:	000cc843 */ 	sra	$t9,$t4,0x1
/*  f16f640:	10000002 */ 	b	.L0f16f64c
/*  f16f644:	afb90480 */ 	sw	$t9,0x480($sp)
.L0f16f648:
/*  f16f648:	afa00480 */ 	sw	$zero,0x480($sp)
.L0f16f64c:
/*  f16f64c:	8fb804a4 */ 	lw	$t8,0x4a4($sp)
/*  f16f650:	8fad0470 */ 	lw	$t5,0x470($sp)
/*  f16f654:	8fab0480 */ 	lw	$t3,0x480($sp)
/*  f16f658:	1b0000a8 */ 	blez	$t8,.L0f16f8fc
/*  f16f65c:	01ab7821 */ 	addu	$t7,$t5,$t3
/*  f16f660:	afaf005c */ 	sw	$t7,0x5c($sp)
/*  f16f664:	8fae0470 */ 	lw	$t6,0x470($sp)
.L0f16f668:
/*  f16f668:	00174043 */ 	sra	$t0,$s7,0x1
/*  f16f66c:	8fa504c4 */ 	lw	$a1,0x4c4($sp)
/*  f16f670:	010e6021 */ 	addu	$t4,$t0,$t6
/*  f16f674:	91820000 */ 	lbu	$v0,0x0($t4)
/*  f16f678:	8faf04b8 */ 	lw	$t7,0x4b8($sp)
/*  f16f67c:	26ee0001 */ 	addiu	$t6,$s7,0x1
/*  f16f680:	0002c903 */ 	sra	$t9,$v0,0x4
/*  f16f684:	3338000f */ 	andi	$t8,$t9,0xf
/*  f16f688:	00186840 */ 	sll	$t5,$t8,0x1
/*  f16f68c:	00ad5821 */ 	addu	$t3,$a1,$t5
/*  f16f690:	01cf182a */ 	slt	$v1,$t6,$t7
/*  f16f694:	10600003 */ 	beqz	$v1,.L0f16f6a4
/*  f16f698:	95750000 */ 	lhu	$s5,0x0($t3)
/*  f16f69c:	10000002 */ 	b	.L0f16f6a8
/*  f16f6a0:	00005025 */ 	or	$t2,$zero,$zero
.L0f16f6a4:
/*  f16f6a4:	240a0004 */ 	addiu	$t2,$zero,0x4
.L0f16f6a8:
/*  f16f6a8:	8fab0480 */ 	lw	$t3,0x480($sp)
/*  f16f6ac:	8fae0470 */ 	lw	$t6,0x470($sp)
/*  f16f6b0:	01426007 */ 	srav	$t4,$v0,$t2
/*  f16f6b4:	3199000f */ 	andi	$t9,$t4,0xf
/*  f16f6b8:	8fa904c4 */ 	lw	$t1,0x4c4($sp)
/*  f16f6bc:	010b7821 */ 	addu	$t7,$t0,$t3
/*  f16f6c0:	0019c040 */ 	sll	$t8,$t9,0x1
/*  f16f6c4:	01ee6021 */ 	addu	$t4,$t7,$t6
/*  f16f6c8:	91990000 */ 	lbu	$t9,0x0($t4)
/*  f16f6cc:	01386821 */ 	addu	$t5,$t1,$t8
/*  f16f6d0:	95b10000 */ 	lhu	$s1,0x0($t5)
/*  f16f6d4:	0019c103 */ 	sra	$t8,$t9,0x4
/*  f16f6d8:	330d000f */ 	andi	$t5,$t8,0xf
/*  f16f6dc:	000d5840 */ 	sll	$t3,$t5,0x1
/*  f16f6e0:	012b7821 */ 	addu	$t7,$t1,$t3
/*  f16f6e4:	10600003 */ 	beqz	$v1,.L0f16f6f4
/*  f16f6e8:	95f20000 */ 	lhu	$s2,0x0($t7)
/*  f16f6ec:	10000002 */ 	b	.L0f16f6f8
/*  f16f6f0:	00005025 */ 	or	$t2,$zero,$zero
.L0f16f6f4:
/*  f16f6f4:	240a0004 */ 	addiu	$t2,$zero,0x4
.L0f16f6f8:
/*  f16f6f8:	8fae005c */ 	lw	$t6,0x5c($sp)
/*  f16f6fc:	32a4ffff */ 	andi	$a0,$s5,0xffff
/*  f16f700:	3225ffff */ 	andi	$a1,$s1,0xffff
/*  f16f704:	01c86021 */ 	addu	$t4,$t6,$t0
/*  f16f708:	91990000 */ 	lbu	$t9,0x0($t4)
/*  f16f70c:	3246ffff */ 	andi	$a2,$s2,0xffff
/*  f16f710:	0159c007 */ 	srav	$t8,$t9,$t2
/*  f16f714:	330d000f */ 	andi	$t5,$t8,0xf
/*  f16f718:	000d5840 */ 	sll	$t3,$t5,0x1
/*  f16f71c:	012b7821 */ 	addu	$t7,$t1,$t3
/*  f16f720:	95f00000 */ 	lhu	$s0,0x0($t7)
/*  f16f724:	0fc5bb96 */ 	jal	func0f16ee58
/*  f16f728:	02003825 */ 	or	$a3,$s0,$zero
/*  f16f72c:	0040b025 */ 	or	$s6,$v0,$zero
/*  f16f730:	32a4ffff */ 	andi	$a0,$s5,0xffff
/*  f16f734:	3225ffff */ 	andi	$a1,$s1,0xffff
/*  f16f738:	3246ffff */ 	andi	$a2,$s2,0xffff
/*  f16f73c:	0fc5bbbf */ 	jal	func0f16eefc
/*  f16f740:	3207ffff */ 	andi	$a3,$s0,0xffff
/*  f16f744:	00409825 */ 	or	$s3,$v0,$zero
/*  f16f748:	32a4ffff */ 	andi	$a0,$s5,0xffff
/*  f16f74c:	3225ffff */ 	andi	$a1,$s1,0xffff
/*  f16f750:	3246ffff */ 	andi	$a2,$s2,0xffff
/*  f16f754:	0fc5bbe8 */ 	jal	func0f16efa0
/*  f16f758:	3207ffff */ 	andi	$a3,$s0,0xffff
/*  f16f75c:	0040a025 */ 	or	$s4,$v0,$zero
/*  f16f760:	32a4ffff */ 	andi	$a0,$s5,0xffff
/*  f16f764:	3225ffff */ 	andi	$a1,$s1,0xffff
/*  f16f768:	3246ffff */ 	andi	$a2,$s2,0xffff
/*  f16f76c:	0fc5bc11 */ 	jal	func0f16f044
/*  f16f770:	3207ffff */ 	andi	$a3,$s0,0xffff
/*  f16f774:	27a40070 */ 	addiu	$a0,$sp,0x70
/*  f16f778:	8fa504c8 */ 	lw	$a1,0x4c8($sp)
/*  f16f77c:	02c03025 */ 	or	$a2,$s6,$zero
/*  f16f780:	02603825 */ 	or	$a3,$s3,$zero
/*  f16f784:	afb40010 */ 	sw	$s4,0x10($sp)
/*  f16f788:	0fc5bf23 */ 	jal	func0f16fc8c
/*  f16f78c:	afa20014 */ 	sw	$v0,0x14($sp)
/*  f16f790:	8fae0474 */ 	lw	$t6,0x474($sp)
/*  f16f794:	00176083 */ 	sra	$t4,$s7,0x2
/*  f16f798:	0002c900 */ 	sll	$t9,$v0,0x4
/*  f16f79c:	01ccf021 */ 	addu	$s8,$t6,$t4
/*  f16f7a0:	a3d90000 */ 	sb	$t9,0x0($s8)
/*  f16f7a4:	8fad0470 */ 	lw	$t5,0x470($sp)
/*  f16f7a8:	26e80002 */ 	addiu	$t0,$s7,0x2
/*  f16f7ac:	0008c043 */ 	sra	$t8,$t0,0x1
/*  f16f7b0:	030d5821 */ 	addu	$t3,$t8,$t5
/*  f16f7b4:	91630000 */ 	lbu	$v1,0x0($t3)
/*  f16f7b8:	8faf04c4 */ 	lw	$t7,0x4c4($sp)
/*  f16f7bc:	8fad04b8 */ 	lw	$t5,0x4b8($sp)
/*  f16f7c0:	00037103 */ 	sra	$t6,$v1,0x4
/*  f16f7c4:	31cc000f */ 	andi	$t4,$t6,0xf
/*  f16f7c8:	000cc840 */ 	sll	$t9,$t4,0x1
/*  f16f7cc:	03004025 */ 	or	$t0,$t8,$zero
/*  f16f7d0:	26eb0003 */ 	addiu	$t3,$s7,0x3
/*  f16f7d4:	01f9c021 */ 	addu	$t8,$t7,$t9
/*  f16f7d8:	016d202a */ 	slt	$a0,$t3,$t5
/*  f16f7dc:	10800003 */ 	beqz	$a0,.L0f16f7ec
/*  f16f7e0:	97150000 */ 	lhu	$s5,0x0($t8)
/*  f16f7e4:	10000002 */ 	b	.L0f16f7f0
/*  f16f7e8:	00005025 */ 	or	$t2,$zero,$zero
.L0f16f7ec:
/*  f16f7ec:	240a0004 */ 	addiu	$t2,$zero,0x4
.L0f16f7f0:
/*  f16f7f0:	01436007 */ 	srav	$t4,$v1,$t2
/*  f16f7f4:	8fad0480 */ 	lw	$t5,0x480($sp)
/*  f16f7f8:	318f000f */ 	andi	$t7,$t4,0xf
/*  f16f7fc:	8fae04c4 */ 	lw	$t6,0x4c4($sp)
/*  f16f800:	8fac0470 */ 	lw	$t4,0x470($sp)
/*  f16f804:	000fc840 */ 	sll	$t9,$t7,0x1
/*  f16f808:	010d5821 */ 	addu	$t3,$t0,$t5
/*  f16f80c:	01d9c021 */ 	addu	$t8,$t6,$t9
/*  f16f810:	016c7821 */ 	addu	$t7,$t3,$t4
/*  f16f814:	91f90000 */ 	lbu	$t9,0x0($t7)
/*  f16f818:	97110000 */ 	lhu	$s1,0x0($t8)
/*  f16f81c:	240a0004 */ 	addiu	$t2,$zero,0x4
/*  f16f820:	0019c103 */ 	sra	$t8,$t9,0x4
/*  f16f824:	330d000f */ 	andi	$t5,$t8,0xf
/*  f16f828:	000d5840 */ 	sll	$t3,$t5,0x1
/*  f16f82c:	01cb6021 */ 	addu	$t4,$t6,$t3
/*  f16f830:	95920000 */ 	lhu	$s2,0x0($t4)
/*  f16f834:	10800003 */ 	beqz	$a0,.L0f16f844
/*  f16f838:	8fb9005c */ 	lw	$t9,0x5c($sp)
/*  f16f83c:	10000001 */ 	b	.L0f16f844
/*  f16f840:	00005025 */ 	or	$t2,$zero,$zero
.L0f16f844:
/*  f16f844:	0328c021 */ 	addu	$t8,$t9,$t0
/*  f16f848:	930d0000 */ 	lbu	$t5,0x0($t8)
/*  f16f84c:	8faf04c4 */ 	lw	$t7,0x4c4($sp)
/*  f16f850:	32a4ffff */ 	andi	$a0,$s5,0xffff
/*  f16f854:	014d7007 */ 	srav	$t6,$t5,$t2
/*  f16f858:	31cb000f */ 	andi	$t3,$t6,0xf
/*  f16f85c:	000b6040 */ 	sll	$t4,$t3,0x1
/*  f16f860:	01ecc821 */ 	addu	$t9,$t7,$t4
/*  f16f864:	97300000 */ 	lhu	$s0,0x0($t9)
/*  f16f868:	3225ffff */ 	andi	$a1,$s1,0xffff
/*  f16f86c:	3246ffff */ 	andi	$a2,$s2,0xffff
/*  f16f870:	0fc5bb96 */ 	jal	func0f16ee58
/*  f16f874:	02003825 */ 	or	$a3,$s0,$zero
/*  f16f878:	0040b025 */ 	or	$s6,$v0,$zero
/*  f16f87c:	32a4ffff */ 	andi	$a0,$s5,0xffff
/*  f16f880:	3225ffff */ 	andi	$a1,$s1,0xffff
/*  f16f884:	3246ffff */ 	andi	$a2,$s2,0xffff
/*  f16f888:	0fc5bbbf */ 	jal	func0f16eefc
/*  f16f88c:	3207ffff */ 	andi	$a3,$s0,0xffff
/*  f16f890:	00409825 */ 	or	$s3,$v0,$zero
/*  f16f894:	32a4ffff */ 	andi	$a0,$s5,0xffff
/*  f16f898:	3225ffff */ 	andi	$a1,$s1,0xffff
/*  f16f89c:	3246ffff */ 	andi	$a2,$s2,0xffff
/*  f16f8a0:	0fc5bbe8 */ 	jal	func0f16efa0
/*  f16f8a4:	3207ffff */ 	andi	$a3,$s0,0xffff
/*  f16f8a8:	0040a025 */ 	or	$s4,$v0,$zero
/*  f16f8ac:	32a4ffff */ 	andi	$a0,$s5,0xffff
/*  f16f8b0:	3225ffff */ 	andi	$a1,$s1,0xffff
/*  f16f8b4:	3246ffff */ 	andi	$a2,$s2,0xffff
/*  f16f8b8:	0fc5bc11 */ 	jal	func0f16f044
/*  f16f8bc:	3207ffff */ 	andi	$a3,$s0,0xffff
/*  f16f8c0:	27a40070 */ 	addiu	$a0,$sp,0x70
/*  f16f8c4:	8fa504c8 */ 	lw	$a1,0x4c8($sp)
/*  f16f8c8:	02c03025 */ 	or	$a2,$s6,$zero
/*  f16f8cc:	02603825 */ 	or	$a3,$s3,$zero
/*  f16f8d0:	afb40010 */ 	sw	$s4,0x10($sp)
/*  f16f8d4:	0fc5bf23 */ 	jal	func0f16fc8c
/*  f16f8d8:	afa20014 */ 	sw	$v0,0x14($sp)
/*  f16f8dc:	93d80000 */ 	lbu	$t8,0x0($s8)
/*  f16f8e0:	26f70004 */ 	addiu	$s7,$s7,0x4
/*  f16f8e4:	03027025 */ 	or	$t6,$t8,$v0
/*  f16f8e8:	a3ce0000 */ 	sb	$t6,0x0($s8)
/*  f16f8ec:	8fab04a4 */ 	lw	$t3,0x4a4($sp)
/*  f16f8f0:	02eb082a */ 	slt	$at,$s7,$t3
/*  f16f8f4:	5420ff5c */ 	bnezl	$at,.L0f16f668
/*  f16f8f8:	8fae0470 */ 	lw	$t6,0x470($sp)
.L0f16f8fc:
/*  f16f8fc:	8faf0474 */ 	lw	$t7,0x474($sp)
/*  f16f900:	8fac0054 */ 	lw	$t4,0x54($sp)
/*  f16f904:	8fab04a8 */ 	lw	$t3,0x4a8($sp)
/*  f16f908:	8fb80470 */ 	lw	$t8,0x470($sp)
/*  f16f90c:	01ecc821 */ 	addu	$t9,$t7,$t4
/*  f16f910:	8fac04bc */ 	lw	$t4,0x4bc($sp)
/*  f16f914:	8fad04a4 */ 	lw	$t5,0x4a4($sp)
/*  f16f918:	256f0002 */ 	addiu	$t7,$t3,0x2
/*  f16f91c:	01ec082a */ 	slt	$at,$t7,$t4
/*  f16f920:	030d7021 */ 	addu	$t6,$t8,$t5
/*  f16f924:	afaf04a8 */ 	sw	$t7,0x4a8($sp)
/*  f16f928:	afb90474 */ 	sw	$t9,0x474($sp)
/*  f16f92c:	1420ff3c */ 	bnez	$at,.L0f16f620
/*  f16f930:	afae0470 */ 	sw	$t6,0x470($sp)
.L0f16f934:
/*  f16f934:	8fb904a0 */ 	lw	$t9,0x4a0($sp)
/*  f16f938:	8fad049c */ 	lw	$t5,0x49c($sp)
/*  f16f93c:	0019c043 */ 	sra	$t8,$t9,0x1
/*  f16f940:	030d0019 */ 	multu	$t8,$t5
/*  f16f944:	00001012 */ 	mflo	$v0
/*  f16f948:	100000c5 */ 	b	.L0f16fc60
/*  f16f94c:	8fbf0044 */ 	lw	$ra,0x44($sp)
.L0f16f950:
/*  f16f950:	8fae04bc */ 	lw	$t6,0x4bc($sp)
/*  f16f954:	8fab04a0 */ 	lw	$t3,0x4a0($sp)
/*  f16f958:	afa004a8 */ 	sw	$zero,0x4a8($sp)
/*  f16f95c:	19c000b8 */ 	blez	$t6,.L0f16fc40
/*  f16f960:	000b7843 */ 	sra	$t7,$t3,0x1
/*  f16f964:	afaf0054 */ 	sw	$t7,0x54($sp)
.L0f16f968:
/*  f16f968:	8fb904a8 */ 	lw	$t9,0x4a8($sp)
/*  f16f96c:	8fac04bc */ 	lw	$t4,0x4bc($sp)
/*  f16f970:	0000b825 */ 	or	$s7,$zero,$zero
/*  f16f974:	27380001 */ 	addiu	$t8,$t9,0x1
/*  f16f978:	030c082a */ 	slt	$at,$t8,$t4
/*  f16f97c:	10200004 */ 	beqz	$at,.L0f16f990
/*  f16f980:	8fad04a4 */ 	lw	$t5,0x4a4($sp)
/*  f16f984:	000d7043 */ 	sra	$t6,$t5,0x1
/*  f16f988:	10000002 */ 	b	.L0f16f994
/*  f16f98c:	afae0480 */ 	sw	$t6,0x480($sp)
.L0f16f990:
/*  f16f990:	afa00480 */ 	sw	$zero,0x480($sp)
.L0f16f994:
/*  f16f994:	8fab04a4 */ 	lw	$t3,0x4a4($sp)
/*  f16f998:	5960009c */ 	blezl	$t3,.L0f16fc0c
/*  f16f99c:	8fae0474 */ 	lw	$t6,0x474($sp)
/*  f16f9a0:	8faf0470 */ 	lw	$t7,0x470($sp)
.L0f16f9a4:
/*  f16f9a4:	00174043 */ 	sra	$t0,$s7,0x1
/*  f16f9a8:	8fab04b8 */ 	lw	$t3,0x4b8($sp)
/*  f16f9ac:	010fc821 */ 	addu	$t9,$t0,$t7
/*  f16f9b0:	93220000 */ 	lbu	$v0,0x0($t9)
/*  f16f9b4:	8fa504c4 */ 	lw	$a1,0x4c4($sp)
/*  f16f9b8:	26ef0001 */ 	addiu	$t7,$s7,0x1
/*  f16f9bc:	00026103 */ 	sra	$t4,$v0,0x4
/*  f16f9c0:	3198000f */ 	andi	$t8,$t4,0xf
/*  f16f9c4:	00186840 */ 	sll	$t5,$t8,0x1
/*  f16f9c8:	01eb182a */ 	slt	$v1,$t7,$t3
/*  f16f9cc:	0062c807 */ 	srav	$t9,$v0,$v1
/*  f16f9d0:	00ad7021 */ 	addu	$t6,$a1,$t5
/*  f16f9d4:	13200003 */ 	beqz	$t9,.L0f16f9e4
/*  f16f9d8:	95d50000 */ 	lhu	$s5,0x0($t6)
/*  f16f9dc:	10000002 */ 	b	.L0f16f9e8
/*  f16f9e0:	00005025 */ 	or	$t2,$zero,$zero
.L0f16f9e4:
/*  f16f9e4:	240a0004 */ 	addiu	$t2,$zero,0x4
.L0f16f9e8:
/*  f16f9e8:	8fae0480 */ 	lw	$t6,0x480($sp)
/*  f16f9ec:	8faf0470 */ 	lw	$t7,0x470($sp)
/*  f16f9f0:	8fa404c4 */ 	lw	$a0,0x4c4($sp)
/*  f16f9f4:	010e5821 */ 	addu	$t3,$t0,$t6
/*  f16f9f8:	016fc821 */ 	addu	$t9,$t3,$t7
/*  f16f9fc:	93220000 */ 	lbu	$v0,0x0($t9)
/*  f16fa00:	314c000f */ 	andi	$t4,$t2,0xf
/*  f16fa04:	000cc040 */ 	sll	$t8,$t4,0x1
/*  f16fa08:	00986821 */ 	addu	$t5,$a0,$t8
/*  f16fa0c:	00026103 */ 	sra	$t4,$v0,0x4
/*  f16fa10:	95b10000 */ 	lhu	$s1,0x0($t5)
/*  f16fa14:	3198000f */ 	andi	$t8,$t4,0xf
/*  f16fa18:	00186840 */ 	sll	$t5,$t8,0x1
/*  f16fa1c:	008d7021 */ 	addu	$t6,$a0,$t5
/*  f16fa20:	00625807 */ 	srav	$t3,$v0,$v1
/*  f16fa24:	11600003 */ 	beqz	$t3,.L0f16fa34
/*  f16fa28:	95d20000 */ 	lhu	$s2,0x0($t6)
/*  f16fa2c:	10000002 */ 	b	.L0f16fa38
/*  f16fa30:	00005025 */ 	or	$t2,$zero,$zero
.L0f16fa34:
/*  f16fa34:	240a0004 */ 	addiu	$t2,$zero,0x4
.L0f16fa38:
/*  f16fa38:	314f000f */ 	andi	$t7,$t2,0xf
/*  f16fa3c:	000fc840 */ 	sll	$t9,$t7,0x1
/*  f16fa40:	00996021 */ 	addu	$t4,$a0,$t9
/*  f16fa44:	95890000 */ 	lhu	$t1,0x0($t4)
/*  f16fa48:	0015c203 */ 	sra	$t8,$s5,0x8
/*  f16fa4c:	00117203 */ 	sra	$t6,$s1,0x8
/*  f16fa50:	31cb00ff */ 	andi	$t3,$t6,0xff
/*  f16fa54:	330d00ff */ 	andi	$t5,$t8,0xff
/*  f16fa58:	01ab7821 */ 	addu	$t7,$t5,$t3
/*  f16fa5c:	0012ca03 */ 	sra	$t9,$s2,0x8
/*  f16fa60:	332c00ff */ 	andi	$t4,$t9,0xff
/*  f16fa64:	00097203 */ 	sra	$t6,$t1,0x8
/*  f16fa68:	31cd00ff */ 	andi	$t5,$t6,0xff
/*  f16fa6c:	01ecc021 */ 	addu	$t8,$t7,$t4
/*  f16fa70:	030d3021 */ 	addu	$a2,$t8,$t5
/*  f16fa74:	00065883 */ 	sra	$t3,$a2,0x2
/*  f16fa78:	322c00ff */ 	andi	$t4,$s1,0xff
/*  f16fa7c:	32af00ff */ 	andi	$t7,$s5,0xff
/*  f16fa80:	01ec7021 */ 	addu	$t6,$t7,$t4
/*  f16fa84:	316600ff */ 	andi	$a2,$t3,0xff
/*  f16fa88:	325800ff */ 	andi	$t8,$s2,0xff
/*  f16fa8c:	01d86821 */ 	addu	$t5,$t6,$t8
/*  f16fa90:	312b00ff */ 	andi	$t3,$t1,0xff
/*  f16fa94:	01ab3821 */ 	addu	$a3,$t5,$t3
/*  f16fa98:	24e70001 */ 	addiu	$a3,$a3,0x1
/*  f16fa9c:	0007c883 */ 	sra	$t9,$a3,0x2
/*  f16faa0:	332700ff */ 	andi	$a3,$t9,0xff
/*  f16faa4:	8fa504c8 */ 	lw	$a1,0x4c8($sp)
/*  f16faa8:	02a01025 */ 	or	$v0,$s5,$zero
/*  f16faac:	02201825 */ 	or	$v1,$s1,$zero
/*  f16fab0:	0fc5bf54 */ 	jal	func0f16fd50
/*  f16fab4:	02404025 */ 	or	$t0,$s2,$zero
/*  f16fab8:	8fac0474 */ 	lw	$t4,0x474($sp)
/*  f16fabc:	8fa60470 */ 	lw	$a2,0x470($sp)
/*  f16fac0:	00177083 */ 	sra	$t6,$s7,0x2
/*  f16fac4:	26e80002 */ 	addiu	$t0,$s7,0x2
/*  f16fac8:	8fa504c4 */ 	lw	$a1,0x4c4($sp)
/*  f16facc:	0002c100 */ 	sll	$t8,$v0,0x4
/*  f16fad0:	00086843 */ 	sra	$t5,$t0,0x1
/*  f16fad4:	018ef021 */ 	addu	$s8,$t4,$t6
/*  f16fad8:	a3d80000 */ 	sb	$t8,0x0($s8)
/*  f16fadc:	01a65821 */ 	addu	$t3,$t5,$a2
/*  f16fae0:	91630000 */ 	lbu	$v1,0x0($t3)
/*  f16fae4:	8fb804b8 */ 	lw	$t8,0x4b8($sp)
/*  f16fae8:	01a04025 */ 	or	$t0,$t5,$zero
/*  f16faec:	0003c903 */ 	sra	$t9,$v1,0x4
/*  f16faf0:	332f000f */ 	andi	$t7,$t9,0xf
/*  f16faf4:	26ed0003 */ 	addiu	$t5,$s7,0x3
/*  f16faf8:	000f6040 */ 	sll	$t4,$t7,0x1
/*  f16fafc:	01b8202a */ 	slt	$a0,$t5,$t8
/*  f16fb00:	00ac7021 */ 	addu	$t6,$a1,$t4
/*  f16fb04:	00835807 */ 	srav	$t3,$v1,$a0
/*  f16fb08:	11600003 */ 	beqz	$t3,.L0f16fb18
/*  f16fb0c:	95d50000 */ 	lhu	$s5,0x0($t6)
/*  f16fb10:	10000002 */ 	b	.L0f16fb1c
/*  f16fb14:	00005025 */ 	or	$t2,$zero,$zero
.L0f16fb18:
/*  f16fb18:	240a0004 */ 	addiu	$t2,$zero,0x4
.L0f16fb1c:
/*  f16fb1c:	8fae0480 */ 	lw	$t6,0x480($sp)
/*  f16fb20:	3159000f */ 	andi	$t9,$t2,0xf
/*  f16fb24:	00197840 */ 	sll	$t7,$t9,0x1
/*  f16fb28:	010ec021 */ 	addu	$t8,$t0,$t6
/*  f16fb2c:	03066821 */ 	addu	$t5,$t8,$a2
/*  f16fb30:	91a20000 */ 	lbu	$v0,0x0($t5)
/*  f16fb34:	00af6021 */ 	addu	$t4,$a1,$t7
/*  f16fb38:	95910000 */ 	lhu	$s1,0x0($t4)
/*  f16fb3c:	00025903 */ 	sra	$t3,$v0,0x4
/*  f16fb40:	3179000f */ 	andi	$t9,$t3,0xf
/*  f16fb44:	00197840 */ 	sll	$t7,$t9,0x1
/*  f16fb48:	00af6021 */ 	addu	$t4,$a1,$t7
/*  f16fb4c:	00827007 */ 	srav	$t6,$v0,$a0
/*  f16fb50:	11c00003 */ 	beqz	$t6,.L0f16fb60
/*  f16fb54:	95920000 */ 	lhu	$s2,0x0($t4)
/*  f16fb58:	10000002 */ 	b	.L0f16fb64
/*  f16fb5c:	00005025 */ 	or	$t2,$zero,$zero
.L0f16fb60:
/*  f16fb60:	240a0004 */ 	addiu	$t2,$zero,0x4
.L0f16fb64:
/*  f16fb64:	8fa404c4 */ 	lw	$a0,0x4c4($sp)
/*  f16fb68:	3158000f */ 	andi	$t8,$t2,0xf
/*  f16fb6c:	00186840 */ 	sll	$t5,$t8,0x1
/*  f16fb70:	008d5821 */ 	addu	$t3,$a0,$t5
/*  f16fb74:	95690000 */ 	lhu	$t1,0x0($t3)
/*  f16fb78:	0015ca03 */ 	sra	$t9,$s5,0x8
/*  f16fb7c:	00116203 */ 	sra	$t4,$s1,0x8
/*  f16fb80:	318e00ff */ 	andi	$t6,$t4,0xff
/*  f16fb84:	332f00ff */ 	andi	$t7,$t9,0xff
/*  f16fb88:	01eec021 */ 	addu	$t8,$t7,$t6
/*  f16fb8c:	00126a03 */ 	sra	$t5,$s2,0x8
/*  f16fb90:	31ab00ff */ 	andi	$t3,$t5,0xff
/*  f16fb94:	00096203 */ 	sra	$t4,$t1,0x8
/*  f16fb98:	318f00ff */ 	andi	$t7,$t4,0xff
/*  f16fb9c:	030bc821 */ 	addu	$t9,$t8,$t3
/*  f16fba0:	032f3021 */ 	addu	$a2,$t9,$t7
/*  f16fba4:	00067083 */ 	sra	$t6,$a2,0x2
/*  f16fba8:	322b00ff */ 	andi	$t3,$s1,0xff
/*  f16fbac:	32b800ff */ 	andi	$t8,$s5,0xff
/*  f16fbb0:	030b6021 */ 	addu	$t4,$t8,$t3
/*  f16fbb4:	31c600ff */ 	andi	$a2,$t6,0xff
/*  f16fbb8:	325900ff */ 	andi	$t9,$s2,0xff
/*  f16fbbc:	01997821 */ 	addu	$t7,$t4,$t9
/*  f16fbc0:	312e00ff */ 	andi	$t6,$t1,0xff
/*  f16fbc4:	01ee3821 */ 	addu	$a3,$t7,$t6
/*  f16fbc8:	24e70001 */ 	addiu	$a3,$a3,0x1
/*  f16fbcc:	00076883 */ 	sra	$t5,$a3,0x2
/*  f16fbd0:	31a700ff */ 	andi	$a3,$t5,0xff
/*  f16fbd4:	8fa504c8 */ 	lw	$a1,0x4c8($sp)
/*  f16fbd8:	02a01025 */ 	or	$v0,$s5,$zero
/*  f16fbdc:	02201825 */ 	or	$v1,$s1,$zero
/*  f16fbe0:	0fc5bf54 */ 	jal	func0f16fd50
/*  f16fbe4:	02404025 */ 	or	$t0,$s2,$zero
/*  f16fbe8:	93cb0000 */ 	lbu	$t3,0x0($s8)
/*  f16fbec:	26f70004 */ 	addiu	$s7,$s7,0x4
/*  f16fbf0:	0162c825 */ 	or	$t9,$t3,$v0
/*  f16fbf4:	a3d90000 */ 	sb	$t9,0x0($s8)
/*  f16fbf8:	8faf04a4 */ 	lw	$t7,0x4a4($sp)
/*  f16fbfc:	02ef082a */ 	slt	$at,$s7,$t7
/*  f16fc00:	5420ff68 */ 	bnezl	$at,.L0f16f9a4
/*  f16fc04:	8faf0470 */ 	lw	$t7,0x470($sp)
/*  f16fc08:	8fae0474 */ 	lw	$t6,0x474($sp)
.L0f16fc0c:
/*  f16fc0c:	8fad0054 */ 	lw	$t5,0x54($sp)
/*  f16fc10:	8faf04a8 */ 	lw	$t7,0x4a8($sp)
/*  f16fc14:	8fab0470 */ 	lw	$t3,0x470($sp)
/*  f16fc18:	01cdc021 */ 	addu	$t8,$t6,$t5
/*  f16fc1c:	8fad04bc */ 	lw	$t5,0x4bc($sp)
/*  f16fc20:	8fac04a4 */ 	lw	$t4,0x4a4($sp)
/*  f16fc24:	25ee0002 */ 	addiu	$t6,$t7,0x2
/*  f16fc28:	01cd082a */ 	slt	$at,$t6,$t5
/*  f16fc2c:	016cc821 */ 	addu	$t9,$t3,$t4
/*  f16fc30:	afae04a8 */ 	sw	$t6,0x4a8($sp)
/*  f16fc34:	afb80474 */ 	sw	$t8,0x474($sp)
/*  f16fc38:	1420ff4b */ 	bnez	$at,.L0f16f968
/*  f16fc3c:	afb90470 */ 	sw	$t9,0x470($sp)
.L0f16fc40:
/*  f16fc40:	8fb804a0 */ 	lw	$t8,0x4a0($sp)
/*  f16fc44:	8fac049c */ 	lw	$t4,0x49c($sp)
/*  f16fc48:	00185843 */ 	sra	$t3,$t8,0x1
/*  f16fc4c:	016c0019 */ 	multu	$t3,$t4
/*  f16fc50:	00001012 */ 	mflo	$v0
/*  f16fc54:	10000002 */ 	b	.L0f16fc60
/*  f16fc58:	8fbf0044 */ 	lw	$ra,0x44($sp)
.L0f16fc5c:
/*  f16fc5c:	8fbf0044 */ 	lw	$ra,0x44($sp)
.L0f16fc60:
/*  f16fc60:	8fb00020 */ 	lw	$s0,0x20($sp)
/*  f16fc64:	8fb10024 */ 	lw	$s1,0x24($sp)
/*  f16fc68:	8fb20028 */ 	lw	$s2,0x28($sp)
/*  f16fc6c:	8fb3002c */ 	lw	$s3,0x2c($sp)
/*  f16fc70:	8fb40030 */ 	lw	$s4,0x30($sp)
/*  f16fc74:	8fb50034 */ 	lw	$s5,0x34($sp)
/*  f16fc78:	8fb60038 */ 	lw	$s6,0x38($sp)
/*  f16fc7c:	8fb7003c */ 	lw	$s7,0x3c($sp)
/*  f16fc80:	8fbe0040 */ 	lw	$s8,0x40($sp)
/*  f16fc84:	03e00008 */ 	jr	$ra
/*  f16fc88:	27bd04b0 */ 	addiu	$sp,$sp,0x4b0
);

GLOBAL_ASM(
glabel func0f16fc8c
/*  f16fc8c:	27bdfff0 */ 	addiu	$sp,$sp,-16
/*  f16fc90:	afb1000c */ 	sw	$s1,0xc($sp)
/*  f16fc94:	afb00008 */ 	sw	$s0,0x8($sp)
/*  f16fc98:	3c0205f5 */ 	lui	$v0,0x5f5
/*  f16fc9c:	00c08025 */ 	or	$s0,$a2,$zero
/*  f16fca0:	00e08825 */ 	or	$s1,$a3,$zero
/*  f16fca4:	00001825 */ 	or	$v1,$zero,$zero
/*  f16fca8:	3442e0ff */ 	ori	$v0,$v0,0xe0ff
/*  f16fcac:	18a00023 */ 	blez	$a1,.L0f16fd3c
/*  f16fcb0:	00004025 */ 	or	$t0,$zero,$zero
/*  f16fcb4:	00803025 */ 	or	$a2,$a0,$zero
/*  f16fcb8:	8faa0024 */ 	lw	$t2,0x24($sp)
/*  f16fcbc:	8fa90020 */ 	lw	$t1,0x20($sp)
.L0f16fcc0:
/*  f16fcc0:	90ce0000 */ 	lbu	$t6,0x0($a2)
/*  f16fcc4:	90cf0001 */ 	lbu	$t7,0x1($a2)
/*  f16fcc8:	90d90002 */ 	lbu	$t9,0x2($a2)
/*  f16fccc:	01d02023 */ 	subu	$a0,$t6,$s0
/*  f16fcd0:	00840019 */ 	multu	$a0,$a0
/*  f16fcd4:	01f12023 */ 	subu	$a0,$t7,$s1
/*  f16fcd8:	90cc0003 */ 	lbu	$t4,0x3($a2)
/*  f16fcdc:	00003812 */ 	mflo	$a3
/*  f16fce0:	00000000 */ 	nop
/*  f16fce4:	00000000 */ 	nop
/*  f16fce8:	00840019 */ 	multu	$a0,$a0
/*  f16fcec:	03292023 */ 	subu	$a0,$t9,$t1
/*  f16fcf0:	0000c012 */ 	mflo	$t8
/*  f16fcf4:	00f83821 */ 	addu	$a3,$a3,$t8
/*  f16fcf8:	00000000 */ 	nop
/*  f16fcfc:	00840019 */ 	multu	$a0,$a0
/*  f16fd00:	018a2023 */ 	subu	$a0,$t4,$t2
/*  f16fd04:	00005812 */ 	mflo	$t3
/*  f16fd08:	00eb3821 */ 	addu	$a3,$a3,$t3
/*  f16fd0c:	00000000 */ 	nop
/*  f16fd10:	00840019 */ 	multu	$a0,$a0
/*  f16fd14:	00006812 */ 	mflo	$t5
/*  f16fd18:	00ed3821 */ 	addu	$a3,$a3,$t5
/*  f16fd1c:	00e2082a */ 	slt	$at,$a3,$v0
/*  f16fd20:	50200004 */ 	beqzl	$at,.L0f16fd34
/*  f16fd24:	25080001 */ 	addiu	$t0,$t0,0x1
/*  f16fd28:	01001825 */ 	or	$v1,$t0,$zero
/*  f16fd2c:	00e01025 */ 	or	$v0,$a3,$zero
/*  f16fd30:	25080001 */ 	addiu	$t0,$t0,0x1
.L0f16fd34:
/*  f16fd34:	1505ffe2 */ 	bne	$t0,$a1,.L0f16fcc0
/*  f16fd38:	24c60004 */ 	addiu	$a2,$a2,0x4
.L0f16fd3c:
/*  f16fd3c:	8fb00008 */ 	lw	$s0,0x8($sp)
/*  f16fd40:	8fb1000c */ 	lw	$s1,0xc($sp)
/*  f16fd44:	27bd0010 */ 	addiu	$sp,$sp,0x10
/*  f16fd48:	03e00008 */ 	jr	$ra
/*  f16fd4c:	00601025 */ 	or	$v0,$v1,$zero
);

s32 func0f16fd50(u16 *arg0, s32 arg1, s32 arg2, s32 arg3)
{
	s32 bestindex = 0;
	s32 bestvalue = 99999999;
	s32 i;

	for (i = 0; i < arg1; i++) {
		s32 value = arg0[i];
		s32 a = ((value >> 8) & 0xff) - arg2;
		s32 b = (value & 0xff) - arg3;
		s32 sum = a * a + b * b;

		if (sum < bestvalue) {
			bestindex = i;
			bestvalue = sum;
		}
	}

	return bestindex;
}

/**
 * Inflate images (levels of detail) from a non-zlib texture.
 *
 * Each image can have a different compression method and pixel format,
 * which is described in a three byte header per image:
 *
 * ffffwwww wwwwhhhh hhhhcccc
 *
 * f = pixel format (see TEXFORMAT constants)
 * w = width in pixels
 * h = height in pixels
 * c = compression method (see TEXCOMPMETHOD constants)
 */
s32 texInflateNonZlib(u8 *src, u8 *dst, s32 arg2, s32 forcenumimages, struct texturething *arg4, s32 arg5)
{
	u8 scratch[0x2000];
	u8 lookup[0x1000];
	u32 stack;
	s32 i;
	s32 numimages;
	s32 width;
	s32 height;
	s32 compmethod;
	s32 j;
	s32 totalbytesout = 0;
	s32 imagebytesout;
	s32 format;
	s32 value;
	u8 *start;
	u8 *end;
	bool writetocache = false;

	texSetBitstring(src);

	numimages = arg2 && forcenumimages ? forcenumimages : 1;

	arg4->unk0c->unk0b_00 = forcenumimages;
	arg4->unk0c->unk0c_02 = arg2;

	if (arg2) {
		writetocache = true;

		for (i = 0; i < g_TexCacheCount; i++) {
			if (g_TexCacheItems[i].texturenum == arg4->unk0c->texturenum) {
				writetocache = false;
			}
		}
	}

	for (i = 0; i < numimages; i++) {
		format = texReadBits(4);
		width = texReadBits(8);
		height = texReadBits(8);
		compmethod = texReadBits(4);

		if (i == 0) {
			arg4->unk0c->width = width;
			arg4->unk0c->height = height;
			arg4->unk0c->gbiformat = g_TexFormatGbiMappings[format];
			arg4->unk0c->unk0b_06 = var800842bc[format];
			arg4->unk0c->unk0c_00 = var800842f0[format] >> 14;
		} else if (writetocache) {
			g_TexCacheItems[g_TexCacheCount].widths[i - 1] = width;
			g_TexCacheItems[g_TexCacheCount].heights[i - 1] = height;
		}

		if (width * height > 0x2000) {
			return 0;
		}

		switch (compmethod) {
		case TEXCOMPMETHOD_0:
		case TEXCOMPMETHOD_1:
			imagebytesout = texInflateMethod1(&dst[totalbytesout], width, height, format);
			break;
		case TEXCOMPMETHOD_HUFFMAN2:
			func0f1711b4(scratch, g_TexFormatNumChannels[format] * width * height, g_TexFormatChannelSizes[format]);

			if (g_TexFormatHas1BitAlpha[format]) {
				texReadAlphaBits(&scratch[width * height * 3], width * height);
			}

			imagebytesout = func0f171ba8(scratch, width, height, &dst[totalbytesout], format);
			break;
		case TEXCOMPMETHOD_HUFFMAN3:
			for (j = 0; j < g_TexFormatNumChannels[format]; j++) {
				func0f1711b4(&scratch[width * height * j], width * height, g_TexFormatChannelSizes[format]);
			}

			if (g_TexFormatHas1BitAlpha[format]) {
				texReadAlphaBits(&scratch[width * height * 3], width * height);
			}

			imagebytesout = func0f171ba8(scratch, width, height, &dst[totalbytesout], format);
			break;
		case TEXCOMPMETHOD_RLE:
			texInflateRle(scratch, g_TexFormatNumChannels[format] * width * height);

			if (g_TexFormatHas1BitAlpha[format]) {
				texReadAlphaBits(&scratch[width * height * 3], width * height);
			}

			imagebytesout = func0f171ba8(scratch, width, height, &dst[totalbytesout], format);
			break;
		case TEXCOMPMETHOD_LOOKUP:
			value = texBuildLookup(lookup, g_TexFormatBitsPerPixel[format]);
			imagebytesout = func0f17218c(width, height, &dst[totalbytesout], lookup, value, format);
			break;
		case TEXCOMPMETHOD_HUFFMANLOOKUP:
			value = texBuildLookup(lookup, g_TexFormatBitsPerPixel[format]);
			func0f1711b4(scratch, width * height, value);
			imagebytesout = func0f172554(scratch, width, height, &dst[totalbytesout], lookup, value, format);
			break;
		case TEXCOMPMETHOD_RLELOOKUP:
			value = texBuildLookup(lookup, g_TexFormatBitsPerPixel[format]);
			texInflateRle(scratch, width * height);
			imagebytesout = func0f172554(scratch, width, height, &dst[totalbytesout], lookup, value, format);
			break;
		case TEXCOMPMETHOD_HUFFMANPREPROCESSED:
			value = texReadBits(3);
			func0f1711b4(scratch, g_TexFormatNumChannels[format] * width * height, g_TexFormatChannelSizes[format]);
			func0f172b5c(scratch, width, g_TexFormatNumChannels[format] * height, value, g_TexFormatChannelSizes[format]);

			if (g_TexFormatHas1BitAlpha[format]) {
				texReadAlphaBits(&scratch[width * height * 3], width * height);
			}

			imagebytesout = func0f171ba8(scratch, width, height, &dst[totalbytesout], format);
			break;
		case TEXCOMPMETHOD_9:
			value = texReadBits(3);
			texInflateRle(scratch, g_TexFormatNumChannels[format] * width * height);
			func0f172b5c(scratch, width, g_TexFormatNumChannels[format] * height, value, g_TexFormatChannelSizes[format]);

			if (g_TexFormatHas1BitAlpha[format]) {
				texReadAlphaBits(&scratch[width * height * 3], width * height);
			}

			imagebytesout = func0f171ba8(scratch, width, height, &dst[totalbytesout], format);
			break;
		}

		if (arg2 == 1) {
			func0f1729f8(&dst[totalbytesout], width, height, format);
		}

		imagebytesout = (imagebytesout + 7) & ~7;
		totalbytesout += imagebytesout;

		if (var800ab548 == 0) {
			var800ab540++;
		} else {
			var800ab548 = 0;
		}
	}

	if (writetocache) {
		g_TexCacheItems[g_TexCacheCount].texturenum = arg4->unk0c->texturenum;

		g_TexCacheCount++;

		// Resetting this variable to 0 here suggests that the g_TexCacheItems
		// array is used in a circular manner, and that g_TexCacheCount is just
		// the index of the oldest/next element. But earlier in this function
		// there's a loop that iterates up to g_TexCacheCount, which doesn't
		// make any sense if this value is used as a pointer in a circular list.
		// Could be a @bug, or maybe they intended to reset the cache every time
		// it fills up.
		if (g_TexCacheCount >= ARRAYCOUNT(g_TexCacheItems)) {
			g_TexCacheCount = 0;
		}
	}

	if (!arg2) {
		if (forcenumimages >= 2) {
			s32 tmpwidth = width;
			s32 tmpheight = height;

			start = dst;
			if (1);
			end = &dst[totalbytesout];

			for (i = 1; i < forcenumimages; i++) {
				imagebytesout = func0f1706ec(start, end, tmpwidth, tmpheight, format);
				func0f1729f8(start, tmpwidth, tmpheight, format);
				totalbytesout += imagebytesout;
				tmpwidth = (tmpwidth + 1) >> 1;
				tmpheight = (tmpheight + 1) >> 1;
				start = end;
				end += imagebytesout;
			}

			func0f1729f8(start, tmpwidth, tmpheight, format);
		} else {
			func0f1729f8(dst, width, height, format);
		}
	}

	return totalbytesout;
}

GLOBAL_ASM(
glabel func0f1706ec
.late_rodata
glabel var7f1b7a78
.word func0f1706ec+0x78 # f170764
glabel var7f1b7a7c
.word func0f1706ec+0x9c # f170788
glabel var7f1b7a80
.word func0f1706ec+0x78 # f170764
glabel var7f1b7a84
.word func0f1706ec+0x9c # f170788
glabel var7f1b7a88
.word func0f1706ec+0x9c # f170788
glabel var7f1b7a8c
.word func0f1706ec+0xc0 # f1707ac
glabel var7f1b7a90
.word func0f1706ec+0xe4 # f1707d0
glabel var7f1b7a94
.word func0f1706ec+0xc0 # f1707ac
glabel var7f1b7a98
.word func0f1706ec+0xe4 # f1707d0
glabel var7f1b7a9c
.word func0f1706ec+0x128 # f170814
glabel var7f1b7aa0
.word func0f1706ec+0x2d8 # f1709c4
glabel var7f1b7aa4
.word func0f1706ec+0x128 # f170814
glabel var7f1b7aa8
.word func0f1706ec+0x2d8 # f1709c4
glabel var7f1b7aac
.word func0f1706ec+0x488 # f170b74
glabel var7f1b7ab0
.word func0f1706ec+0x5c8 # f170cb4
glabel var7f1b7ab4
.word func0f1706ec+0x7f8 # f170ee4
glabel var7f1b7ab8
.word func0f1706ec+0x714 # f170e00
glabel var7f1b7abc
.word func0f1706ec+0x988 # f171074
.text
/*  f1706ec:	27bdff58 */ 	addiu	$sp,$sp,-168
/*  f1706f0:	8fa200b8 */ 	lw	$v0,0xb8($sp)
/*  f1706f4:	afb00004 */ 	sw	$s0,0x4($sp)
/*  f1706f8:	24ee0001 */ 	addiu	$t6,$a3,0x1
/*  f1706fc:	afb70020 */ 	sw	$s7,0x20($sp)
/*  f170700:	afb6001c */ 	sw	$s6,0x1c($sp)
/*  f170704:	afb50018 */ 	sw	$s5,0x18($sp)
/*  f170708:	afb2000c */ 	sw	$s2,0xc($sp)
/*  f17070c:	000e8043 */ 	sra	$s0,$t6,0x1
/*  f170710:	2c410009 */ 	sltiu	$at,$v0,0x9
/*  f170714:	00c0b025 */ 	or	$s6,$a2,$zero
/*  f170718:	00e0b825 */ 	or	$s7,$a3,$zero
/*  f17071c:	afbe0024 */ 	sw	$s8,0x24($sp)
/*  f170720:	afb40014 */ 	sw	$s4,0x14($sp)
/*  f170724:	afb30010 */ 	sw	$s3,0x10($sp)
/*  f170728:	afb10008 */ 	sw	$s1,0x8($sp)
/*  f17072c:	00a04025 */ 	or	$t0,$a1,$zero
/*  f170730:	00a06825 */ 	or	$t5,$a1,$zero
/*  f170734:	00a0a825 */ 	or	$s5,$a1,$zero
/*  f170738:	00801825 */ 	or	$v1,$a0,$zero
/*  f17073c:	00806025 */ 	or	$t4,$a0,$zero
/*  f170740:	00809025 */ 	or	$s2,$a0,$zero
/*  f170744:	1020002a */ 	beqz	$at,.L0f1707f0
/*  f170748:	afb0003c */ 	sw	$s0,0x3c($sp)
/*  f17074c:	0002c880 */ 	sll	$t9,$v0,0x2
/*  f170750:	3c017f1b */ 	lui	$at,%hi(var7f1b7a78)
/*  f170754:	00390821 */ 	addu	$at,$at,$t9
/*  f170758:	8c397a78 */ 	lw	$t9,%lo(var7f1b7a78)($at)
/*  f17075c:	03200008 */ 	jr	$t9
/*  f170760:	00000000 */ 	nop
/*  f170764:	26d80001 */ 	addiu	$t8,$s6,0x1
/*  f170768:	00187043 */ 	sra	$t6,$t8,0x1
/*  f17076c:	25cf0003 */ 	addiu	$t7,$t6,0x3
/*  f170770:	26d30003 */ 	addiu	$s3,$s6,0x3
/*  f170774:	31f90ffc */ 	andi	$t9,$t7,0xffc
/*  f170778:	32780ffc */ 	andi	$t8,$s3,0xffc
/*  f17077c:	afb90098 */ 	sw	$t9,0x98($sp)
/*  f170780:	1000001b */ 	b	.L0f1707f0
/*  f170784:	afb8009c */ 	sw	$t8,0x9c($sp)
/*  f170788:	26ce0001 */ 	addiu	$t6,$s6,0x1
/*  f17078c:	000e7843 */ 	sra	$t7,$t6,0x1
/*  f170790:	25f90003 */ 	addiu	$t9,$t7,0x3
/*  f170794:	26d30003 */ 	addiu	$s3,$s6,0x3
/*  f170798:	33380ffc */ 	andi	$t8,$t9,0xffc
/*  f17079c:	326e0ffc */ 	andi	$t6,$s3,0xffc
/*  f1707a0:	afb80098 */ 	sw	$t8,0x98($sp)
/*  f1707a4:	10000012 */ 	b	.L0f1707f0
/*  f1707a8:	afae009c */ 	sw	$t6,0x9c($sp)
/*  f1707ac:	26cf0001 */ 	addiu	$t7,$s6,0x1
/*  f1707b0:	000fc843 */ 	sra	$t9,$t7,0x1
/*  f1707b4:	27380007 */ 	addiu	$t8,$t9,0x7
/*  f1707b8:	26d30007 */ 	addiu	$s3,$s6,0x7
/*  f1707bc:	330e0ff8 */ 	andi	$t6,$t8,0xff8
/*  f1707c0:	326f0ff8 */ 	andi	$t7,$s3,0xff8
/*  f1707c4:	afae0098 */ 	sw	$t6,0x98($sp)
/*  f1707c8:	10000009 */ 	b	.L0f1707f0
/*  f1707cc:	afaf009c */ 	sw	$t7,0x9c($sp)
/*  f1707d0:	26d90001 */ 	addiu	$t9,$s6,0x1
/*  f1707d4:	0019c043 */ 	sra	$t8,$t9,0x1
/*  f1707d8:	270e000f */ 	addiu	$t6,$t8,0xf
/*  f1707dc:	26d3000f */ 	addiu	$s3,$s6,0xf
/*  f1707e0:	31cf0ff0 */ 	andi	$t7,$t6,0xff0
/*  f1707e4:	32790ff0 */ 	andi	$t9,$s3,0xff0
/*  f1707e8:	afaf0098 */ 	sw	$t7,0x98($sp)
/*  f1707ec:	afb9009c */ 	sw	$t9,0x9c($sp)
.L0f1707f0:
/*  f1707f0:	2c410009 */ 	sltiu	$at,$v0,0x9
/*  f1707f4:	10200263 */ 	beqz	$at,.L0f171184
/*  f1707f8:	8fb3009c */ 	lw	$s3,0x9c($sp)
/*  f1707fc:	0002c080 */ 	sll	$t8,$v0,0x2
/*  f170800:	3c017f1b */ 	lui	$at,%hi(var7f1b7a9c)
/*  f170804:	00380821 */ 	addu	$at,$at,$t8
/*  f170808:	8c387a9c */ 	lw	$t8,%lo(var7f1b7a9c)($at)
/*  f17080c:	03000008 */ 	jr	$t8
/*  f170810:	00000000 */ 	nop
/*  f170814:	1ae00064 */ 	blez	$s7,.L0f1709a8
/*  f170818:	0000f025 */ 	or	$s8,$zero,$zero
/*  f17081c:	8fb00098 */ 	lw	$s0,0x98($sp)
/*  f170820:	02608825 */ 	or	$s1,$s3,$zero
/*  f170824:	001178c0 */ 	sll	$t7,$s1,0x3
/*  f170828:	00107080 */ 	sll	$t6,$s0,0x2
/*  f17082c:	01c08025 */ 	or	$s0,$t6,$zero
/*  f170830:	01e08825 */ 	or	$s1,$t7,$zero
.L0f170834:
/*  f170834:	27d90001 */ 	addiu	$t9,$s8,0x1
/*  f170838:	0337082a */ 	slt	$at,$t9,$s7
/*  f17083c:	10200003 */ 	beqz	$at,.L0f17084c
/*  f170840:	00005825 */ 	or	$t3,$zero,$zero
/*  f170844:	10000002 */ 	b	.L0f170850
/*  f170848:	0260a025 */ 	or	$s4,$s3,$zero
.L0f17084c:
/*  f17084c:	0000a025 */ 	or	$s4,$zero,$zero
.L0f170850:
/*  f170850:	1a600051 */ 	blez	$s3,.L0f170998
/*  f170854:	27de0002 */ 	addiu	$s8,$s8,0x2
/*  f170858:	0014c080 */ 	sll	$t8,$s4,0x2
/*  f17085c:	00785021 */ 	addu	$t2,$v1,$t8
/*  f170860:	01406025 */ 	or	$t4,$t2,$zero
/*  f170864:	00602025 */ 	or	$a0,$v1,$zero
.L0f170868:
/*  f170868:	25620001 */ 	addiu	$v0,$t3,0x1
/*  f17086c:	0056082a */ 	slt	$at,$v0,$s6
/*  f170870:	10200003 */ 	beqz	$at,.L0f170880
/*  f170874:	01602825 */ 	or	$a1,$t3,$zero
/*  f170878:	10000001 */ 	b	.L0f170880
/*  f17087c:	00402825 */ 	or	$a1,$v0,$zero
.L0f170880:
/*  f170880:	00053880 */ 	sll	$a3,$a1,0x2
/*  f170884:	00677021 */ 	addu	$t6,$v1,$a3
/*  f170888:	8dc60000 */ 	lw	$a2,0x0($t6)
/*  f17088c:	8c820000 */ 	lw	$v0,0x0($a0)
/*  f170890:	01477821 */ 	addu	$t7,$t2,$a3
/*  f170894:	8d890000 */ 	lw	$t1,0x0($t4)
/*  f170898:	8ded0000 */ 	lw	$t5,0x0($t7)
/*  f17089c:	00067602 */ 	srl	$t6,$a2,0x18
/*  f1708a0:	31cf00ff */ 	andi	$t7,$t6,0xff
/*  f1708a4:	0002ce02 */ 	srl	$t9,$v0,0x18
/*  f1708a8:	333800ff */ 	andi	$t8,$t9,0xff
/*  f1708ac:	030fc821 */ 	addu	$t9,$t8,$t7
/*  f1708b0:	00097602 */ 	srl	$t6,$t1,0x18
/*  f1708b4:	31d800ff */ 	andi	$t8,$t6,0xff
/*  f1708b8:	03387821 */ 	addu	$t7,$t9,$t8
/*  f1708bc:	000d7602 */ 	srl	$t6,$t5,0x18
/*  f1708c0:	31d900ff */ 	andi	$t9,$t6,0xff
/*  f1708c4:	01f9c021 */ 	addu	$t8,$t7,$t9
/*  f1708c8:	00187882 */ 	srl	$t7,$t8,0x2
/*  f1708cc:	000fce00 */ 	sll	$t9,$t7,0x18
/*  f1708d0:	0002c402 */ 	srl	$t8,$v0,0x10
/*  f1708d4:	330e00ff */ 	andi	$t6,$t8,0xff
/*  f1708d8:	00067c02 */ 	srl	$t7,$a2,0x10
/*  f1708dc:	31f800ff */ 	andi	$t8,$t7,0xff
/*  f1708e0:	01d87821 */ 	addu	$t7,$t6,$t8
/*  f1708e4:	00097402 */ 	srl	$t6,$t1,0x10
/*  f1708e8:	31d800ff */ 	andi	$t8,$t6,0xff
/*  f1708ec:	01f87021 */ 	addu	$t6,$t7,$t8
/*  f1708f0:	000d7c02 */ 	srl	$t7,$t5,0x10
/*  f1708f4:	31f800ff */ 	andi	$t8,$t7,0xff
/*  f1708f8:	01d87821 */ 	addu	$t7,$t6,$t8
/*  f1708fc:	000f7082 */ 	srl	$t6,$t7,0x2
/*  f170900:	31d800ff */ 	andi	$t8,$t6,0xff
/*  f170904:	00187c00 */ 	sll	$t7,$t8,0x10
/*  f170908:	032f7025 */ 	or	$t6,$t9,$t7
/*  f17090c:	0002c202 */ 	srl	$t8,$v0,0x8
/*  f170910:	331900ff */ 	andi	$t9,$t8,0xff
/*  f170914:	00067a02 */ 	srl	$t7,$a2,0x8
/*  f170918:	31f800ff */ 	andi	$t8,$t7,0xff
/*  f17091c:	03387821 */ 	addu	$t7,$t9,$t8
/*  f170920:	0009ca02 */ 	srl	$t9,$t1,0x8
/*  f170924:	333800ff */ 	andi	$t8,$t9,0xff
/*  f170928:	01f8c821 */ 	addu	$t9,$t7,$t8
/*  f17092c:	000d7a02 */ 	srl	$t7,$t5,0x8
/*  f170930:	31f800ff */ 	andi	$t8,$t7,0xff
/*  f170934:	03387821 */ 	addu	$t7,$t9,$t8
/*  f170938:	000fc882 */ 	srl	$t9,$t7,0x2
/*  f17093c:	333800ff */ 	andi	$t8,$t9,0xff
/*  f170940:	00187a00 */ 	sll	$t7,$t8,0x8
/*  f170944:	01cfc825 */ 	or	$t9,$t6,$t7
/*  f170948:	30ce00ff */ 	andi	$t6,$a2,0xff
/*  f17094c:	305800ff */ 	andi	$t8,$v0,0xff
/*  f170950:	030e7821 */ 	addu	$t7,$t8,$t6
/*  f170954:	313800ff */ 	andi	$t8,$t1,0xff
/*  f170958:	01f87021 */ 	addu	$t6,$t7,$t8
/*  f17095c:	31af00ff */ 	andi	$t7,$t5,0xff
/*  f170960:	01cfc021 */ 	addu	$t8,$t6,$t7
/*  f170964:	270e0001 */ 	addiu	$t6,$t8,0x1
/*  f170968:	000e7882 */ 	srl	$t7,$t6,0x2
/*  f17096c:	31f800ff */ 	andi	$t8,$t7,0xff
/*  f170970:	000b7843 */ 	sra	$t7,$t3,0x1
/*  f170974:	03387025 */ 	or	$t6,$t9,$t8
/*  f170978:	256b0002 */ 	addiu	$t3,$t3,0x2
/*  f17097c:	000fc880 */ 	sll	$t9,$t7,0x2
/*  f170980:	0173082a */ 	slt	$at,$t3,$s3
/*  f170984:	0119c021 */ 	addu	$t8,$t0,$t9
/*  f170988:	24840008 */ 	addiu	$a0,$a0,0x8
/*  f17098c:	258c0008 */ 	addiu	$t4,$t4,0x8
/*  f170990:	1420ffb5 */ 	bnez	$at,.L0f170868
/*  f170994:	af0e0000 */ 	sw	$t6,0x0($t8)
.L0f170998:
/*  f170998:	03d7082a */ 	slt	$at,$s8,$s7
/*  f17099c:	01104021 */ 	addu	$t0,$t0,$s0
/*  f1709a0:	1420ffa4 */ 	bnez	$at,.L0f170834
/*  f1709a4:	00711821 */ 	addu	$v1,$v1,$s1
.L0f1709a8:
/*  f1709a8:	8faf003c */ 	lw	$t7,0x3c($sp)
/*  f1709ac:	8fb90098 */ 	lw	$t9,0x98($sp)
/*  f1709b0:	01f90019 */ 	multu	$t7,$t9
/*  f1709b4:	00001012 */ 	mflo	$v0
/*  f1709b8:	00027080 */ 	sll	$t6,$v0,0x2
/*  f1709bc:	100001f2 */ 	b	.L0f171188
/*  f1709c0:	01c01025 */ 	or	$v0,$t6,$zero
/*  f1709c4:	1ae00064 */ 	blez	$s7,.L0f170b58
/*  f1709c8:	0000f025 */ 	or	$s8,$zero,$zero
/*  f1709cc:	8fb00098 */ 	lw	$s0,0x98($sp)
/*  f1709d0:	02608825 */ 	or	$s1,$s3,$zero
/*  f1709d4:	00117880 */ 	sll	$t7,$s1,0x2
/*  f1709d8:	0010c040 */ 	sll	$t8,$s0,0x1
/*  f1709dc:	03008025 */ 	or	$s0,$t8,$zero
/*  f1709e0:	01e08825 */ 	or	$s1,$t7,$zero
.L0f1709e4:
/*  f1709e4:	27d90001 */ 	addiu	$t9,$s8,0x1
/*  f1709e8:	0337082a */ 	slt	$at,$t9,$s7
/*  f1709ec:	10200003 */ 	beqz	$at,.L0f1709fc
/*  f1709f0:	00005825 */ 	or	$t3,$zero,$zero
/*  f1709f4:	10000002 */ 	b	.L0f170a00
/*  f1709f8:	0260a025 */ 	or	$s4,$s3,$zero
.L0f1709fc:
/*  f1709fc:	0000a025 */ 	or	$s4,$zero,$zero
.L0f170a00:
/*  f170a00:	1a600051 */ 	blez	$s3,.L0f170b48
/*  f170a04:	27de0002 */ 	addiu	$s8,$s8,0x2
/*  f170a08:	00147040 */ 	sll	$t6,$s4,0x1
/*  f170a0c:	018e5021 */ 	addu	$t2,$t4,$t6
/*  f170a10:	01404825 */ 	or	$t1,$t2,$zero
/*  f170a14:	01804025 */ 	or	$t0,$t4,$zero
.L0f170a18:
/*  f170a18:	25620001 */ 	addiu	$v0,$t3,0x1
/*  f170a1c:	0056082a */ 	slt	$at,$v0,$s6
/*  f170a20:	10200003 */ 	beqz	$at,.L0f170a30
/*  f170a24:	01602825 */ 	or	$a1,$t3,$zero
/*  f170a28:	10000001 */ 	b	.L0f170a30
/*  f170a2c:	00402825 */ 	or	$a1,$v0,$zero
.L0f170a30:
/*  f170a30:	00059040 */ 	sll	$s2,$a1,0x1
/*  f170a34:	0192c021 */ 	addu	$t8,$t4,$s2
/*  f170a38:	97030000 */ 	lhu	$v1,0x0($t8)
/*  f170a3c:	95020000 */ 	lhu	$v0,0x0($t0)
/*  f170a40:	01527821 */ 	addu	$t7,$t2,$s2
/*  f170a44:	95240000 */ 	lhu	$a0,0x0($t1)
/*  f170a48:	95e60000 */ 	lhu	$a2,0x0($t7)
/*  f170a4c:	0003c2c3 */ 	sra	$t8,$v1,0xb
/*  f170a50:	330f001f */ 	andi	$t7,$t8,0x1f
/*  f170a54:	0002cac3 */ 	sra	$t9,$v0,0xb
/*  f170a58:	332e001f */ 	andi	$t6,$t9,0x1f
/*  f170a5c:	01cfc821 */ 	addu	$t9,$t6,$t7
/*  f170a60:	0004c2c3 */ 	sra	$t8,$a0,0xb
/*  f170a64:	330e001f */ 	andi	$t6,$t8,0x1f
/*  f170a68:	032e7821 */ 	addu	$t7,$t9,$t6
/*  f170a6c:	0006c2c3 */ 	sra	$t8,$a2,0xb
/*  f170a70:	3319001f */ 	andi	$t9,$t8,0x1f
/*  f170a74:	01f97021 */ 	addu	$t6,$t7,$t9
/*  f170a78:	000e7883 */ 	sra	$t7,$t6,0x2
/*  f170a7c:	000fcac0 */ 	sll	$t9,$t7,0xb
/*  f170a80:	00027183 */ 	sra	$t6,$v0,0x6
/*  f170a84:	31d8001f */ 	andi	$t8,$t6,0x1f
/*  f170a88:	00037983 */ 	sra	$t7,$v1,0x6
/*  f170a8c:	31ee001f */ 	andi	$t6,$t7,0x1f
/*  f170a90:	030e7821 */ 	addu	$t7,$t8,$t6
/*  f170a94:	0004c183 */ 	sra	$t8,$a0,0x6
/*  f170a98:	330e001f */ 	andi	$t6,$t8,0x1f
/*  f170a9c:	01eec021 */ 	addu	$t8,$t7,$t6
/*  f170aa0:	00067983 */ 	sra	$t7,$a2,0x6
/*  f170aa4:	31ee001f */ 	andi	$t6,$t7,0x1f
/*  f170aa8:	030e7821 */ 	addu	$t7,$t8,$t6
/*  f170aac:	000fc083 */ 	sra	$t8,$t7,0x2
/*  f170ab0:	330e001f */ 	andi	$t6,$t8,0x1f
/*  f170ab4:	000e7980 */ 	sll	$t7,$t6,0x6
/*  f170ab8:	032fc025 */ 	or	$t8,$t9,$t7
/*  f170abc:	00027043 */ 	sra	$t6,$v0,0x1
/*  f170ac0:	31d9001f */ 	andi	$t9,$t6,0x1f
/*  f170ac4:	00037843 */ 	sra	$t7,$v1,0x1
/*  f170ac8:	31ee001f */ 	andi	$t6,$t7,0x1f
/*  f170acc:	032e7821 */ 	addu	$t7,$t9,$t6
/*  f170ad0:	0004c843 */ 	sra	$t9,$a0,0x1
/*  f170ad4:	332e001f */ 	andi	$t6,$t9,0x1f
/*  f170ad8:	01eec821 */ 	addu	$t9,$t7,$t6
/*  f170adc:	00067843 */ 	sra	$t7,$a2,0x1
/*  f170ae0:	31ee001f */ 	andi	$t6,$t7,0x1f
/*  f170ae4:	032e7821 */ 	addu	$t7,$t9,$t6
/*  f170ae8:	000fc883 */ 	sra	$t9,$t7,0x2
/*  f170aec:	332e001f */ 	andi	$t6,$t9,0x1f
/*  f170af0:	000e7840 */ 	sll	$t7,$t6,0x1
/*  f170af4:	030fc825 */ 	or	$t9,$t8,$t7
/*  f170af8:	30780001 */ 	andi	$t8,$v1,0x1
/*  f170afc:	304e0001 */ 	andi	$t6,$v0,0x1
/*  f170b00:	01d87821 */ 	addu	$t7,$t6,$t8
/*  f170b04:	308e0001 */ 	andi	$t6,$a0,0x1
/*  f170b08:	01eec021 */ 	addu	$t8,$t7,$t6
/*  f170b0c:	30cf0001 */ 	andi	$t7,$a2,0x1
/*  f170b10:	030f7021 */ 	addu	$t6,$t8,$t7
/*  f170b14:	25d80002 */ 	addiu	$t8,$t6,0x2
/*  f170b18:	00187883 */ 	sra	$t7,$t8,0x2
/*  f170b1c:	31ee0001 */ 	andi	$t6,$t7,0x1
/*  f170b20:	000b7843 */ 	sra	$t7,$t3,0x1
/*  f170b24:	032ec025 */ 	or	$t8,$t9,$t6
/*  f170b28:	256b0002 */ 	addiu	$t3,$t3,0x2
/*  f170b2c:	000fc840 */ 	sll	$t9,$t7,0x1
/*  f170b30:	0173082a */ 	slt	$at,$t3,$s3
/*  f170b34:	01b97021 */ 	addu	$t6,$t5,$t9
/*  f170b38:	25080004 */ 	addiu	$t0,$t0,0x4
/*  f170b3c:	25290004 */ 	addiu	$t1,$t1,0x4
/*  f170b40:	1420ffb5 */ 	bnez	$at,.L0f170a18
/*  f170b44:	a5d80000 */ 	sh	$t8,0x0($t6)
.L0f170b48:
/*  f170b48:	03d7082a */ 	slt	$at,$s8,$s7
/*  f170b4c:	01b06821 */ 	addu	$t5,$t5,$s0
/*  f170b50:	1420ffa4 */ 	bnez	$at,.L0f1709e4
/*  f170b54:	01916021 */ 	addu	$t4,$t4,$s1
.L0f170b58:
/*  f170b58:	8faf003c */ 	lw	$t7,0x3c($sp)
/*  f170b5c:	8fb90098 */ 	lw	$t9,0x98($sp)
/*  f170b60:	01f90019 */ 	multu	$t7,$t9
/*  f170b64:	00001012 */ 	mflo	$v0
/*  f170b68:	0002c040 */ 	sll	$t8,$v0,0x1
/*  f170b6c:	10000186 */ 	b	.L0f171188
/*  f170b70:	03001025 */ 	or	$v0,$t8,$zero
/*  f170b74:	1ae00048 */ 	blez	$s7,.L0f170c98
/*  f170b78:	0000f025 */ 	or	$s8,$zero,$zero
/*  f170b7c:	8fb00098 */ 	lw	$s0,0x98($sp)
/*  f170b80:	02608825 */ 	or	$s1,$s3,$zero
/*  f170b84:	00117880 */ 	sll	$t7,$s1,0x2
/*  f170b88:	00107040 */ 	sll	$t6,$s0,0x1
/*  f170b8c:	01c08025 */ 	or	$s0,$t6,$zero
/*  f170b90:	01e08825 */ 	or	$s1,$t7,$zero
.L0f170b94:
/*  f170b94:	27d90001 */ 	addiu	$t9,$s8,0x1
/*  f170b98:	0337082a */ 	slt	$at,$t9,$s7
/*  f170b9c:	10200003 */ 	beqz	$at,.L0f170bac
/*  f170ba0:	00005825 */ 	or	$t3,$zero,$zero
/*  f170ba4:	10000002 */ 	b	.L0f170bb0
/*  f170ba8:	0260a025 */ 	or	$s4,$s3,$zero
.L0f170bac:
/*  f170bac:	0000a025 */ 	or	$s4,$zero,$zero
.L0f170bb0:
/*  f170bb0:	1a600035 */ 	blez	$s3,.L0f170c88
/*  f170bb4:	27de0002 */ 	addiu	$s8,$s8,0x2
/*  f170bb8:	0014c040 */ 	sll	$t8,$s4,0x1
/*  f170bbc:	01985021 */ 	addu	$t2,$t4,$t8
/*  f170bc0:	01404825 */ 	or	$t1,$t2,$zero
/*  f170bc4:	01804025 */ 	or	$t0,$t4,$zero
.L0f170bc8:
/*  f170bc8:	25620001 */ 	addiu	$v0,$t3,0x1
/*  f170bcc:	0056082a */ 	slt	$at,$v0,$s6
/*  f170bd0:	10200003 */ 	beqz	$at,.L0f170be0
/*  f170bd4:	01602825 */ 	or	$a1,$t3,$zero
/*  f170bd8:	10000001 */ 	b	.L0f170be0
/*  f170bdc:	00402825 */ 	or	$a1,$v0,$zero
.L0f170be0:
/*  f170be0:	00059040 */ 	sll	$s2,$a1,0x1
/*  f170be4:	01927021 */ 	addu	$t6,$t4,$s2
/*  f170be8:	95c30000 */ 	lhu	$v1,0x0($t6)
/*  f170bec:	95020000 */ 	lhu	$v0,0x0($t0)
/*  f170bf0:	01527821 */ 	addu	$t7,$t2,$s2
/*  f170bf4:	95240000 */ 	lhu	$a0,0x0($t1)
/*  f170bf8:	95e60000 */ 	lhu	$a2,0x0($t7)
/*  f170bfc:	00037203 */ 	sra	$t6,$v1,0x8
/*  f170c00:	31cf00ff */ 	andi	$t7,$t6,0xff
/*  f170c04:	0002ca03 */ 	sra	$t9,$v0,0x8
/*  f170c08:	333800ff */ 	andi	$t8,$t9,0xff
/*  f170c0c:	030fc821 */ 	addu	$t9,$t8,$t7
/*  f170c10:	00047203 */ 	sra	$t6,$a0,0x8
/*  f170c14:	31d800ff */ 	andi	$t8,$t6,0xff
/*  f170c18:	03387821 */ 	addu	$t7,$t9,$t8
/*  f170c1c:	00067203 */ 	sra	$t6,$a2,0x8
/*  f170c20:	31d900ff */ 	andi	$t9,$t6,0xff
/*  f170c24:	01f9a021 */ 	addu	$s4,$t7,$t9
/*  f170c28:	26940002 */ 	addiu	$s4,$s4,0x2
/*  f170c2c:	00143883 */ 	sra	$a3,$s4,0x2
/*  f170c30:	04e10002 */ 	bgez	$a3,.L0f170c3c
/*  f170c34:	000bc843 */ 	sra	$t9,$t3,0x1
/*  f170c38:	00003825 */ 	or	$a3,$zero,$zero
.L0f170c3c:
/*  f170c3c:	28e10100 */ 	slti	$at,$a3,0x100
/*  f170c40:	14200002 */ 	bnez	$at,.L0f170c4c
/*  f170c44:	0019c040 */ 	sll	$t8,$t9,0x1
/*  f170c48:	240700ff */ 	addiu	$a3,$zero,0xff
.L0f170c4c:
/*  f170c4c:	00072883 */ 	sra	$a1,$a3,0x2
/*  f170c50:	04a10002 */ 	bgez	$a1,.L0f170c5c
/*  f170c54:	00077200 */ 	sll	$t6,$a3,0x8
/*  f170c58:	00002825 */ 	or	$a1,$zero,$zero
.L0f170c5c:
/*  f170c5c:	28a10100 */ 	slti	$at,$a1,0x100
/*  f170c60:	14200002 */ 	bnez	$at,.L0f170c6c
/*  f170c64:	256b0002 */ 	addiu	$t3,$t3,0x2
/*  f170c68:	240500ff */ 	addiu	$a1,$zero,0xff
.L0f170c6c:
/*  f170c6c:	01c57825 */ 	or	$t7,$t6,$a1
/*  f170c70:	0173082a */ 	slt	$at,$t3,$s3
/*  f170c74:	01b87021 */ 	addu	$t6,$t5,$t8
/*  f170c78:	25080004 */ 	addiu	$t0,$t0,0x4
/*  f170c7c:	25290004 */ 	addiu	$t1,$t1,0x4
/*  f170c80:	1420ffd1 */ 	bnez	$at,.L0f170bc8
/*  f170c84:	a5cf0000 */ 	sh	$t7,0x0($t6)
.L0f170c88:
/*  f170c88:	03d7082a */ 	slt	$at,$s8,$s7
/*  f170c8c:	01b06821 */ 	addu	$t5,$t5,$s0
/*  f170c90:	1420ffc0 */ 	bnez	$at,.L0f170b94
/*  f170c94:	01916021 */ 	addu	$t4,$t4,$s1
.L0f170c98:
/*  f170c98:	8fb9003c */ 	lw	$t9,0x3c($sp)
/*  f170c9c:	8fb80098 */ 	lw	$t8,0x98($sp)
/*  f170ca0:	03380019 */ 	multu	$t9,$t8
/*  f170ca4:	00001012 */ 	mflo	$v0
/*  f170ca8:	00027840 */ 	sll	$t7,$v0,0x1
/*  f170cac:	10000136 */ 	b	.L0f171188
/*  f170cb0:	01e01025 */ 	or	$v0,$t7,$zero
/*  f170cb4:	1ae0004c */ 	blez	$s7,.L0f170de8
/*  f170cb8:	0000f025 */ 	or	$s8,$zero,$zero
/*  f170cbc:	00137040 */ 	sll	$t6,$s3,0x1
/*  f170cc0:	afae0034 */ 	sw	$t6,0x34($sp)
.L0f170cc4:
/*  f170cc4:	27d90001 */ 	addiu	$t9,$s8,0x1
/*  f170cc8:	0337082a */ 	slt	$at,$t9,$s7
/*  f170ccc:	10200003 */ 	beqz	$at,.L0f170cdc
/*  f170cd0:	00005825 */ 	or	$t3,$zero,$zero
/*  f170cd4:	10000002 */ 	b	.L0f170ce0
/*  f170cd8:	0260a025 */ 	or	$s4,$s3,$zero
.L0f170cdc:
/*  f170cdc:	0000a025 */ 	or	$s4,$zero,$zero
.L0f170ce0:
/*  f170ce0:	1a60003b */ 	blez	$s3,.L0f170dd0
/*  f170ce4:	27de0002 */ 	addiu	$s8,$s8,0x2
/*  f170ce8:	02408025 */ 	or	$s0,$s2,$zero
/*  f170cec:	02928821 */ 	addu	$s1,$s4,$s2
.L0f170cf0:
/*  f170cf0:	25620001 */ 	addiu	$v0,$t3,0x1
/*  f170cf4:	0056082a */ 	slt	$at,$v0,$s6
/*  f170cf8:	10200003 */ 	beqz	$at,.L0f170d08
/*  f170cfc:	01602825 */ 	or	$a1,$t3,$zero
/*  f170d00:	10000001 */ 	b	.L0f170d08
/*  f170d04:	00402825 */ 	or	$a1,$v0,$zero
.L0f170d08:
/*  f170d08:	0245c021 */ 	addu	$t8,$s2,$a1
/*  f170d0c:	93030000 */ 	lbu	$v1,0x0($t8)
/*  f170d10:	02857821 */ 	addu	$t7,$s4,$a1
/*  f170d14:	92020000 */ 	lbu	$v0,0x0($s0)
/*  f170d18:	01f27021 */ 	addu	$t6,$t7,$s2
/*  f170d1c:	92240000 */ 	lbu	$a0,0x0($s1)
/*  f170d20:	91c80000 */ 	lbu	$t0,0x0($t6)
/*  f170d24:	00037903 */ 	sra	$t7,$v1,0x4
/*  f170d28:	31ee000f */ 	andi	$t6,$t7,0xf
/*  f170d2c:	0002c903 */ 	sra	$t9,$v0,0x4
/*  f170d30:	3338000f */ 	andi	$t8,$t9,0xf
/*  f170d34:	030ec821 */ 	addu	$t9,$t8,$t6
/*  f170d38:	00047903 */ 	sra	$t7,$a0,0x4
/*  f170d3c:	31f8000f */ 	andi	$t8,$t7,0xf
/*  f170d40:	03387021 */ 	addu	$t6,$t9,$t8
/*  f170d44:	00087903 */ 	sra	$t7,$t0,0x4
/*  f170d48:	31f9000f */ 	andi	$t9,$t7,0xf
/*  f170d4c:	01d93021 */ 	addu	$a2,$t6,$t9
/*  f170d50:	24c60002 */ 	addiu	$a2,$a2,0x2
/*  f170d54:	00063883 */ 	sra	$a3,$a2,0x2
/*  f170d58:	04e10002 */ 	bgez	$a3,.L0f170d64
/*  f170d5c:	304e000f */ 	andi	$t6,$v0,0xf
/*  f170d60:	00003825 */ 	or	$a3,$zero,$zero
.L0f170d64:
/*  f170d64:	28e10010 */ 	slti	$at,$a3,0x10
/*  f170d68:	14200002 */ 	bnez	$at,.L0f170d74
/*  f170d6c:	3079000f */ 	andi	$t9,$v1,0xf
/*  f170d70:	2407000f */ 	addiu	$a3,$zero,0xf
.L0f170d74:
/*  f170d74:	00077900 */ 	sll	$t7,$a3,0x4
/*  f170d78:	01e03825 */ 	or	$a3,$t7,$zero
/*  f170d7c:	01d9c021 */ 	addu	$t8,$t6,$t9
/*  f170d80:	308f000f */ 	andi	$t7,$a0,0xf
/*  f170d84:	030f7021 */ 	addu	$t6,$t8,$t7
/*  f170d88:	3119000f */ 	andi	$t9,$t0,0xf
/*  f170d8c:	01d93021 */ 	addu	$a2,$t6,$t9
/*  f170d90:	24c60002 */ 	addiu	$a2,$a2,0x2
/*  f170d94:	00062883 */ 	sra	$a1,$a2,0x2
/*  f170d98:	04a10002 */ 	bgez	$a1,.L0f170da4
/*  f170d9c:	000b7043 */ 	sra	$t6,$t3,0x1
/*  f170da0:	00002825 */ 	or	$a1,$zero,$zero
.L0f170da4:
/*  f170da4:	28a10010 */ 	slti	$at,$a1,0x10
/*  f170da8:	14200002 */ 	bnez	$at,.L0f170db4
/*  f170dac:	02aec821 */ 	addu	$t9,$s5,$t6
/*  f170db0:	2405000f */ 	addiu	$a1,$zero,0xf
.L0f170db4:
/*  f170db4:	256b0002 */ 	addiu	$t3,$t3,0x2
/*  f170db8:	0173082a */ 	slt	$at,$t3,$s3
/*  f170dbc:	00e57825 */ 	or	$t7,$a3,$a1
/*  f170dc0:	26100002 */ 	addiu	$s0,$s0,0x2
/*  f170dc4:	26310002 */ 	addiu	$s1,$s1,0x2
/*  f170dc8:	1420ffc9 */ 	bnez	$at,.L0f170cf0
/*  f170dcc:	a32f0000 */ 	sb	$t7,0x0($t9)
.L0f170dd0:
/*  f170dd0:	8fb80098 */ 	lw	$t8,0x98($sp)
/*  f170dd4:	8fae0034 */ 	lw	$t6,0x34($sp)
/*  f170dd8:	03d7082a */ 	slt	$at,$s8,$s7
/*  f170ddc:	02b8a821 */ 	addu	$s5,$s5,$t8
/*  f170de0:	1420ffb8 */ 	bnez	$at,.L0f170cc4
/*  f170de4:	024e9021 */ 	addu	$s2,$s2,$t6
.L0f170de8:
/*  f170de8:	8faf003c */ 	lw	$t7,0x3c($sp)
/*  f170dec:	8fb90098 */ 	lw	$t9,0x98($sp)
/*  f170df0:	01f90019 */ 	multu	$t7,$t9
/*  f170df4:	00001012 */ 	mflo	$v0
/*  f170df8:	100000e4 */ 	b	.L0f17118c
/*  f170dfc:	8fb00004 */ 	lw	$s0,0x4($sp)
/*  f170e00:	1ae00032 */ 	blez	$s7,.L0f170ecc
/*  f170e04:	0000f025 */ 	or	$s8,$zero,$zero
/*  f170e08:	0013c040 */ 	sll	$t8,$s3,0x1
/*  f170e0c:	afb80034 */ 	sw	$t8,0x34($sp)
/*  f170e10:	93ad004c */ 	lbu	$t5,0x4c($sp)
.L0f170e14:
/*  f170e14:	27ce0001 */ 	addiu	$t6,$s8,0x1
/*  f170e18:	01d7082a */ 	slt	$at,$t6,$s7
/*  f170e1c:	10200003 */ 	beqz	$at,.L0f170e2c
/*  f170e20:	00005825 */ 	or	$t3,$zero,$zero
/*  f170e24:	10000002 */ 	b	.L0f170e30
/*  f170e28:	0260a025 */ 	or	$s4,$s3,$zero
.L0f170e2c:
/*  f170e2c:	0000a025 */ 	or	$s4,$zero,$zero
.L0f170e30:
/*  f170e30:	1a600020 */ 	blez	$s3,.L0f170eb4
/*  f170e34:	27de0002 */ 	addiu	$s8,$s8,0x2
/*  f170e38:	02408025 */ 	or	$s0,$s2,$zero
/*  f170e3c:	01a04025 */ 	or	$t0,$t5,$zero
.L0f170e40:
/*  f170e40:	25620001 */ 	addiu	$v0,$t3,0x1
/*  f170e44:	0056082a */ 	slt	$at,$v0,$s6
/*  f170e48:	10200003 */ 	beqz	$at,.L0f170e58
/*  f170e4c:	01602825 */ 	or	$a1,$t3,$zero
/*  f170e50:	10000001 */ 	b	.L0f170e58
/*  f170e54:	00402825 */ 	or	$a1,$v0,$zero
.L0f170e58:
/*  f170e58:	02457821 */ 	addu	$t7,$s2,$a1
/*  f170e5c:	0285c821 */ 	addu	$t9,$s4,$a1
/*  f170e60:	920c0000 */ 	lbu	$t4,0x0($s0)
/*  f170e64:	91e90000 */ 	lbu	$t1,0x0($t7)
/*  f170e68:	0332c021 */ 	addu	$t8,$t9,$s2
/*  f170e6c:	930a0000 */ 	lbu	$t2,0x0($t8)
/*  f170e70:	01897021 */ 	addu	$t6,$t4,$t1
/*  f170e74:	000bc043 */ 	sra	$t8,$t3,0x1
/*  f170e78:	01ca7821 */ 	addu	$t7,$t6,$t2
/*  f170e7c:	01e81021 */ 	addu	$v0,$t7,$t0
/*  f170e80:	24420002 */ 	addiu	$v0,$v0,0x2
/*  f170e84:	00023883 */ 	sra	$a3,$v0,0x2
/*  f170e88:	04e10002 */ 	bgez	$a3,.L0f170e94
/*  f170e8c:	02b87021 */ 	addu	$t6,$s5,$t8
/*  f170e90:	00003825 */ 	or	$a3,$zero,$zero
.L0f170e94:
/*  f170e94:	28e10100 */ 	slti	$at,$a3,0x100
/*  f170e98:	14200002 */ 	bnez	$at,.L0f170ea4
/*  f170e9c:	256b0002 */ 	addiu	$t3,$t3,0x2
/*  f170ea0:	240700ff */ 	addiu	$a3,$zero,0xff
.L0f170ea4:
/*  f170ea4:	0173082a */ 	slt	$at,$t3,$s3
/*  f170ea8:	26100002 */ 	addiu	$s0,$s0,0x2
/*  f170eac:	1420ffe4 */ 	bnez	$at,.L0f170e40
/*  f170eb0:	a1c70000 */ 	sb	$a3,0x0($t6)
.L0f170eb4:
/*  f170eb4:	8faf0098 */ 	lw	$t7,0x98($sp)
/*  f170eb8:	8fb90034 */ 	lw	$t9,0x34($sp)
/*  f170ebc:	03d7082a */ 	slt	$at,$s8,$s7
/*  f170ec0:	02afa821 */ 	addu	$s5,$s5,$t7
/*  f170ec4:	1420ffd3 */ 	bnez	$at,.L0f170e14
/*  f170ec8:	02599021 */ 	addu	$s2,$s2,$t9
.L0f170ecc:
/*  f170ecc:	8fb8003c */ 	lw	$t8,0x3c($sp)
/*  f170ed0:	8fae0098 */ 	lw	$t6,0x98($sp)
/*  f170ed4:	030e0019 */ 	multu	$t8,$t6
/*  f170ed8:	00001012 */ 	mflo	$v0
/*  f170edc:	100000ab */ 	b	.L0f17118c
/*  f170ee0:	8fb00004 */ 	lw	$s0,0x4($sp)
/*  f170ee4:	1ae0005d */ 	blez	$s7,.L0f17105c
/*  f170ee8:	0000f025 */ 	or	$s8,$zero,$zero
/*  f170eec:	8fb10098 */ 	lw	$s1,0x98($sp)
/*  f170ef0:	00117843 */ 	sra	$t7,$s1,0x1
/*  f170ef4:	01e08825 */ 	or	$s1,$t7,$zero
.L0f170ef8:
/*  f170ef8:	1a600053 */ 	blez	$s3,.L0f171048
/*  f170efc:	00005825 */ 	or	$t3,$zero,$zero
/*  f170f00:	27c50001 */ 	addiu	$a1,$s8,0x1
.L0f170f04:
/*  f170f04:	000b3043 */ 	sra	$a2,$t3,0x1
/*  f170f08:	00d21021 */ 	addu	$v0,$a2,$s2
/*  f170f0c:	00b7082a */ 	slt	$at,$a1,$s7
/*  f170f10:	10200003 */ 	beqz	$at,.L0f170f20
/*  f170f14:	904c0000 */ 	lbu	$t4,0x0($v0)
/*  f170f18:	10000002 */ 	b	.L0f170f24
/*  f170f1c:	00133843 */ 	sra	$a3,$s3,0x1
.L0f170f20:
/*  f170f20:	00003825 */ 	or	$a3,$zero,$zero
.L0f170f24:
/*  f170f24:	00e6c821 */ 	addu	$t9,$a3,$a2
/*  f170f28:	0332c021 */ 	addu	$t8,$t9,$s2
/*  f170f2c:	00b7082a */ 	slt	$at,$a1,$s7
/*  f170f30:	93090000 */ 	lbu	$t1,0x0($t8)
/*  f170f34:	10200003 */ 	beqz	$at,.L0f170f44
/*  f170f38:	904a0001 */ 	lbu	$t2,0x1($v0)
/*  f170f3c:	10000002 */ 	b	.L0f170f48
/*  f170f40:	00133843 */ 	sra	$a3,$s3,0x1
.L0f170f44:
/*  f170f44:	00003825 */ 	or	$a3,$zero,$zero
.L0f170f48:
/*  f170f48:	00e67021 */ 	addu	$t6,$a3,$a2
/*  f170f4c:	01d27821 */ 	addu	$t7,$t6,$s2
/*  f170f50:	91ed0001 */ 	lbu	$t5,0x1($t7)
/*  f170f54:	000c7043 */ 	sra	$t6,$t4,0x1
/*  f170f58:	31cf0007 */ 	andi	$t7,$t6,0x7
/*  f170f5c:	000cc943 */ 	sra	$t9,$t4,0x5
/*  f170f60:	33380007 */ 	andi	$t8,$t9,0x7
/*  f170f64:	030fc821 */ 	addu	$t9,$t8,$t7
/*  f170f68:	00097143 */ 	sra	$t6,$t1,0x5
/*  f170f6c:	31d80007 */ 	andi	$t8,$t6,0x7
/*  f170f70:	03387821 */ 	addu	$t7,$t9,$t8
/*  f170f74:	00097043 */ 	sra	$t6,$t1,0x1
/*  f170f78:	31d90007 */ 	andi	$t9,$t6,0x7
/*  f170f7c:	01f9c021 */ 	addu	$t8,$t7,$t9
/*  f170f80:	001870c0 */ 	sll	$t6,$t8,0x3
/*  f170f84:	31cf00e0 */ 	andi	$t7,$t6,0xe0
/*  f170f88:	000ac943 */ 	sra	$t9,$t2,0x5
/*  f170f8c:	33380007 */ 	andi	$t8,$t9,0x7
/*  f170f90:	000a7043 */ 	sra	$t6,$t2,0x1
/*  f170f94:	31d90007 */ 	andi	$t9,$t6,0x7
/*  f170f98:	03197021 */ 	addu	$t6,$t8,$t9
/*  f170f9c:	000dc143 */ 	sra	$t8,$t5,0x5
/*  f170fa0:	33190007 */ 	andi	$t9,$t8,0x7
/*  f170fa4:	01d9c021 */ 	addu	$t8,$t6,$t9
/*  f170fa8:	000d7043 */ 	sra	$t6,$t5,0x1
/*  f170fac:	31d90007 */ 	andi	$t9,$t6,0x7
/*  f170fb0:	03197021 */ 	addu	$t6,$t8,$t9
/*  f170fb4:	000ec043 */ 	sra	$t8,$t6,0x1
/*  f170fb8:	3319000e */ 	andi	$t9,$t8,0xe
/*  f170fbc:	01f97025 */ 	or	$t6,$t7,$t9
/*  f170fc0:	000cc103 */ 	sra	$t8,$t4,0x4
/*  f170fc4:	330f0001 */ 	andi	$t7,$t8,0x1
/*  f170fc8:	31990001 */ 	andi	$t9,$t4,0x1
/*  f170fcc:	01f9c021 */ 	addu	$t8,$t7,$t9
/*  f170fd0:	00097903 */ 	sra	$t7,$t1,0x4
/*  f170fd4:	31f90001 */ 	andi	$t9,$t7,0x1
/*  f170fd8:	03197821 */ 	addu	$t7,$t8,$t9
/*  f170fdc:	31380001 */ 	andi	$t8,$t1,0x1
/*  f170fe0:	01f8c821 */ 	addu	$t9,$t7,$t8
/*  f170fe4:	272f0001 */ 	addiu	$t7,$t9,0x1
/*  f170fe8:	000fc080 */ 	sll	$t8,$t7,0x2
/*  f170fec:	33190010 */ 	andi	$t9,$t8,0x10
/*  f170ff0:	000a7903 */ 	sra	$t7,$t2,0x4
/*  f170ff4:	31f80001 */ 	andi	$t8,$t7,0x1
/*  f170ff8:	314f0001 */ 	andi	$t7,$t2,0x1
/*  f170ffc:	030fc021 */ 	addu	$t8,$t8,$t7
/*  f171000:	afae0028 */ 	sw	$t6,0x28($sp)
/*  f171004:	000d7903 */ 	sra	$t7,$t5,0x4
/*  f171008:	31ee0001 */ 	andi	$t6,$t7,0x1
/*  f17100c:	030e7021 */ 	addu	$t6,$t8,$t6
/*  f171010:	31b80001 */ 	andi	$t8,$t5,0x1
/*  f171014:	01d87821 */ 	addu	$t7,$t6,$t8
/*  f171018:	25ee0001 */ 	addiu	$t6,$t7,0x1
/*  f17101c:	000ec083 */ 	sra	$t8,$t6,0x2
/*  f171020:	330f0001 */ 	andi	$t7,$t8,0x1
/*  f171024:	8fb80028 */ 	lw	$t8,0x28($sp)
/*  f171028:	032f7025 */ 	or	$t6,$t9,$t7
/*  f17102c:	000b7883 */ 	sra	$t7,$t3,0x2
/*  f171030:	256b0004 */ 	addiu	$t3,$t3,0x4
/*  f171034:	030ec825 */ 	or	$t9,$t8,$t6
/*  f171038:	0173082a */ 	slt	$at,$t3,$s3
/*  f17103c:	02afc021 */ 	addu	$t8,$s5,$t7
/*  f171040:	1420ffb0 */ 	bnez	$at,.L0f170f04
/*  f171044:	a3190000 */ 	sb	$t9,0x0($t8)
.L0f171048:
/*  f171048:	27de0002 */ 	addiu	$s8,$s8,0x2
/*  f17104c:	03d7082a */ 	slt	$at,$s8,$s7
/*  f171050:	02b1a821 */ 	addu	$s5,$s5,$s1
/*  f171054:	1420ffa8 */ 	bnez	$at,.L0f170ef8
/*  f171058:	02539021 */ 	addu	$s2,$s2,$s3
.L0f17105c:
/*  f17105c:	8fae0098 */ 	lw	$t6,0x98($sp)
/*  f171060:	000e7843 */ 	sra	$t7,$t6,0x1
/*  f171064:	01f00019 */ 	multu	$t7,$s0
/*  f171068:	00001012 */ 	mflo	$v0
/*  f17106c:	10000047 */ 	b	.L0f17118c
/*  f171070:	8fb00004 */ 	lw	$s0,0x4($sp)
/*  f171074:	1ae0003d */ 	blez	$s7,.L0f17116c
/*  f171078:	0000f025 */ 	or	$s8,$zero,$zero
/*  f17107c:	8fb10098 */ 	lw	$s1,0x98($sp)
/*  f171080:	0011c843 */ 	sra	$t9,$s1,0x1
/*  f171084:	03208825 */ 	or	$s1,$t9,$zero
.L0f171088:
/*  f171088:	1a600033 */ 	blez	$s3,.L0f171158
/*  f17108c:	00005825 */ 	or	$t3,$zero,$zero
/*  f171090:	27c50001 */ 	addiu	$a1,$s8,0x1
.L0f171094:
/*  f171094:	000b3043 */ 	sra	$a2,$t3,0x1
/*  f171098:	00d21021 */ 	addu	$v0,$a2,$s2
/*  f17109c:	00b7082a */ 	slt	$at,$a1,$s7
/*  f1710a0:	10200003 */ 	beqz	$at,.L0f1710b0
/*  f1710a4:	904c0000 */ 	lbu	$t4,0x0($v0)
/*  f1710a8:	10000002 */ 	b	.L0f1710b4
/*  f1710ac:	00133843 */ 	sra	$a3,$s3,0x1
.L0f1710b0:
/*  f1710b0:	00003825 */ 	or	$a3,$zero,$zero
.L0f1710b4:
/*  f1710b4:	00e6c021 */ 	addu	$t8,$a3,$a2
/*  f1710b8:	03127021 */ 	addu	$t6,$t8,$s2
/*  f1710bc:	00b7082a */ 	slt	$at,$a1,$s7
/*  f1710c0:	91c90000 */ 	lbu	$t1,0x0($t6)
/*  f1710c4:	10200003 */ 	beqz	$at,.L0f1710d4
/*  f1710c8:	904a0001 */ 	lbu	$t2,0x1($v0)
/*  f1710cc:	10000002 */ 	b	.L0f1710d8
/*  f1710d0:	00133843 */ 	sra	$a3,$s3,0x1
.L0f1710d4:
/*  f1710d4:	00003825 */ 	or	$a3,$zero,$zero
.L0f1710d8:
/*  f1710d8:	00e67821 */ 	addu	$t7,$a3,$a2
/*  f1710dc:	01f2c821 */ 	addu	$t9,$t7,$s2
/*  f1710e0:	000cc103 */ 	sra	$t8,$t4,0x4
/*  f1710e4:	330e000f */ 	andi	$t6,$t8,0xf
/*  f1710e8:	932d0001 */ 	lbu	$t5,0x1($t9)
/*  f1710ec:	318f000f */ 	andi	$t7,$t4,0xf
/*  f1710f0:	01cfc821 */ 	addu	$t9,$t6,$t7
/*  f1710f4:	0009c103 */ 	sra	$t8,$t1,0x4
/*  f1710f8:	330e000f */ 	andi	$t6,$t8,0xf
/*  f1710fc:	032e7821 */ 	addu	$t7,$t9,$t6
/*  f171100:	3138000f */ 	andi	$t8,$t1,0xf
/*  f171104:	01f8c821 */ 	addu	$t9,$t7,$t8
/*  f171108:	00197080 */ 	sll	$t6,$t9,0x2
/*  f17110c:	31cf00f0 */ 	andi	$t7,$t6,0xf0
/*  f171110:	000ac103 */ 	sra	$t8,$t2,0x4
/*  f171114:	3319000f */ 	andi	$t9,$t8,0xf
/*  f171118:	314e000f */ 	andi	$t6,$t2,0xf
/*  f17111c:	032ec021 */ 	addu	$t8,$t9,$t6
/*  f171120:	000dc903 */ 	sra	$t9,$t5,0x4
/*  f171124:	332e000f */ 	andi	$t6,$t9,0xf
/*  f171128:	030ec821 */ 	addu	$t9,$t8,$t6
/*  f17112c:	31b8000f */ 	andi	$t8,$t5,0xf
/*  f171130:	03387021 */ 	addu	$t6,$t9,$t8
/*  f171134:	000ec883 */ 	sra	$t9,$t6,0x2
/*  f171138:	3338000f */ 	andi	$t8,$t9,0xf
/*  f17113c:	000bc883 */ 	sra	$t9,$t3,0x2
/*  f171140:	256b0004 */ 	addiu	$t3,$t3,0x4
/*  f171144:	01f87025 */ 	or	$t6,$t7,$t8
/*  f171148:	0173082a */ 	slt	$at,$t3,$s3
/*  f17114c:	02b97821 */ 	addu	$t7,$s5,$t9
/*  f171150:	1420ffd0 */ 	bnez	$at,.L0f171094
/*  f171154:	a1ee0000 */ 	sb	$t6,0x0($t7)
.L0f171158:
/*  f171158:	27de0002 */ 	addiu	$s8,$s8,0x2
/*  f17115c:	03d7082a */ 	slt	$at,$s8,$s7
/*  f171160:	02b1a821 */ 	addu	$s5,$s5,$s1
/*  f171164:	1420ffc8 */ 	bnez	$at,.L0f171088
/*  f171168:	02539021 */ 	addu	$s2,$s2,$s3
.L0f17116c:
/*  f17116c:	8fb80098 */ 	lw	$t8,0x98($sp)
/*  f171170:	0018c843 */ 	sra	$t9,$t8,0x1
/*  f171174:	03300019 */ 	multu	$t9,$s0
/*  f171178:	00001012 */ 	mflo	$v0
/*  f17117c:	10000003 */ 	b	.L0f17118c
/*  f171180:	8fb00004 */ 	lw	$s0,0x4($sp)
.L0f171184:
/*  f171184:	00001025 */ 	or	$v0,$zero,$zero
.L0f171188:
/*  f171188:	8fb00004 */ 	lw	$s0,0x4($sp)
.L0f17118c:
/*  f17118c:	8fb10008 */ 	lw	$s1,0x8($sp)
/*  f171190:	8fb2000c */ 	lw	$s2,0xc($sp)
/*  f171194:	8fb30010 */ 	lw	$s3,0x10($sp)
/*  f171198:	8fb40014 */ 	lw	$s4,0x14($sp)
/*  f17119c:	8fb50018 */ 	lw	$s5,0x18($sp)
/*  f1711a0:	8fb6001c */ 	lw	$s6,0x1c($sp)
/*  f1711a4:	8fb70020 */ 	lw	$s7,0x20($sp)
/*  f1711a8:	8fbe0024 */ 	lw	$s8,0x24($sp)
/*  f1711ac:	03e00008 */ 	jr	$ra
/*  f1711b0:	27bd00a8 */ 	addiu	$sp,$sp,0xa8
);

GLOBAL_ASM(
glabel func0f1711b4
/*  f1711b4:	27bdcf90 */ 	addiu	$sp,$sp,-12400
/*  f1711b8:	afbf002c */ 	sw	$ra,0x2c($sp)
/*  f1711bc:	afb50028 */ 	sw	$s5,0x28($sp)
/*  f1711c0:	afb40024 */ 	sw	$s4,0x24($sp)
/*  f1711c4:	afb10018 */ 	sw	$s1,0x18($sp)
/*  f1711c8:	00c0a025 */ 	or	$s4,$a2,$zero
/*  f1711cc:	0080a825 */ 	or	$s5,$a0,$zero
/*  f1711d0:	afb30020 */ 	sw	$s3,0x20($sp)
/*  f1711d4:	afb2001c */ 	sw	$s2,0x1c($sp)
/*  f1711d8:	afb00014 */ 	sw	$s0,0x14($sp)
/*  f1711dc:	afa53074 */ 	sw	$a1,0x3074($sp)
/*  f1711e0:	0000f825 */ 	or	$ra,$zero,$zero
/*  f1711e4:	18c0000a */ 	blez	$a2,.L0f171210
/*  f1711e8:	00008825 */ 	or	$s1,$zero,$zero
/*  f1711ec:	27b02070 */ 	addiu	$s0,$sp,0x2070
.L0f1711f0:
/*  f1711f0:	afbf0054 */ 	sw	$ra,0x54($sp)
/*  f1711f4:	0fc5cd4f */ 	jal	texReadBits
/*  f1711f8:	24040008 */ 	addiu	$a0,$zero,0x8
/*  f1711fc:	26310001 */ 	addiu	$s1,$s1,0x1
/*  f171200:	8fbf0054 */ 	lw	$ra,0x54($sp)
/*  f171204:	26100002 */ 	addiu	$s0,$s0,0x2
/*  f171208:	1634fff9 */ 	bne	$s1,$s4,.L0f1711f0
/*  f17120c:	a602fffe */ 	sh	$v0,-0x2($s0)
.L0f171210:
/*  f171210:	27a20070 */ 	addiu	$v0,$sp,0x70
/*  f171214:	27a42070 */ 	addiu	$a0,$sp,0x2070
/*  f171218:	2403ffff */ 	addiu	$v1,$zero,-1
.L0f17121c:
/*  f17121c:	24420004 */ 	addiu	$v0,$v0,0x4
/*  f171220:	0044082b */ 	sltu	$at,$v0,$a0
/*  f171224:	a443fffc */ 	sh	$v1,-0x4($v0)
/*  f171228:	1420fffc */ 	bnez	$at,.L0f17121c
/*  f17122c:	a443fffe */ 	sh	$v1,-0x2($v0)
/*  f171230:	2408270f */ 	addiu	$t0,$zero,0x270f
/*  f171234:	2406270f */ 	addiu	$a2,$zero,0x270f
/*  f171238:	1a80001e */ 	blez	$s4,.L0f1712b4
/*  f17123c:	00008825 */ 	or	$s1,$zero,$zero
/*  f171240:	27b02070 */ 	addiu	$s0,$sp,0x2070
/*  f171244:	8fa9005c */ 	lw	$t1,0x5c($sp)
/*  f171248:	8fa70058 */ 	lw	$a3,0x58($sp)
.L0f17124c:
/*  f17124c:	96040000 */ 	lhu	$a0,0x0($s0)
/*  f171250:	01001825 */ 	or	$v1,$t0,$zero
/*  f171254:	00c02825 */ 	or	$a1,$a2,$zero
/*  f171258:	0088082a */ 	slt	$at,$a0,$t0
/*  f17125c:	1020000a */ 	beqz	$at,.L0f171288
/*  f171260:	00801025 */ 	or	$v0,$a0,$zero
/*  f171264:	00a3082a */ 	slt	$at,$a1,$v1
/*  f171268:	50200005 */ 	beqzl	$at,.L0f171280
/*  f17126c:	3086ffff */ 	andi	$a2,$a0,0xffff
/*  f171270:	3088ffff */ 	andi	$t0,$a0,0xffff
/*  f171274:	10000009 */ 	b	.L0f17129c
/*  f171278:	02204825 */ 	or	$t1,$s1,$zero
/*  f17127c:	3086ffff */ 	andi	$a2,$a0,0xffff
.L0f171280:
/*  f171280:	10000006 */ 	b	.L0f17129c
/*  f171284:	02203825 */ 	or	$a3,$s1,$zero
.L0f171288:
/*  f171288:	0045082a */ 	slt	$at,$v0,$a1
/*  f17128c:	50200004 */ 	beqzl	$at,.L0f1712a0
/*  f171290:	26310001 */ 	addiu	$s1,$s1,0x1
/*  f171294:	3086ffff */ 	andi	$a2,$a0,0xffff
/*  f171298:	02203825 */ 	or	$a3,$s1,$zero
.L0f17129c:
/*  f17129c:	26310001 */ 	addiu	$s1,$s1,0x1
.L0f1712a0:
/*  f1712a0:	1634ffea */ 	bne	$s1,$s4,.L0f17124c
/*  f1712a4:	26100002 */ 	addiu	$s0,$s0,0x2
/*  f1712a8:	00008825 */ 	or	$s1,$zero,$zero
/*  f1712ac:	afa70058 */ 	sw	$a3,0x58($sp)
/*  f1712b0:	afa9005c */ 	sw	$t1,0x5c($sp)
.L0f1712b4:
/*  f1712b4:	8fa70058 */ 	lw	$a3,0x58($sp)
/*  f1712b8:	8fa9005c */ 	lw	$t1,0x5c($sp)
/*  f1712bc:	27b30070 */ 	addiu	$s3,$sp,0x70
/*  f1712c0:	240d270f */ 	addiu	$t5,$zero,0x270f
/*  f1712c4:	240c270f */ 	addiu	$t4,$zero,0x270f
/*  f1712c8:	27ab2070 */ 	addiu	$t3,$sp,0x2070
/*  f1712cc:	00077040 */ 	sll	$t6,$a3,0x1
.L0f1712d0:
/*  f1712d0:	00097840 */ 	sll	$t7,$t1,0x1
/*  f1712d4:	016f1021 */ 	addu	$v0,$t3,$t7
/*  f1712d8:	016e2821 */ 	addu	$a1,$t3,$t6
/*  f1712dc:	94b80000 */ 	lhu	$t8,0x0($a1)
/*  f1712e0:	94590000 */ 	lhu	$t9,0x0($v0)
/*  f1712e4:	00097080 */ 	sll	$t6,$t1,0x2
/*  f1712e8:	00077880 */ 	sll	$t7,$a3,0x2
/*  f1712ec:	03195021 */ 	addu	$t2,$t8,$t9
/*  f1712f0:	026f2021 */ 	addu	$a0,$s3,$t7
/*  f1712f4:	026e1821 */ 	addu	$v1,$s3,$t6
/*  f1712f8:	2408270f */ 	addiu	$t0,$zero,0x270f
/*  f1712fc:	15400002 */ 	bnez	$t2,.L0f171308
/*  f171300:	2406270f */ 	addiu	$a2,$zero,0x270f
/*  f171304:	240a0001 */ 	addiu	$t2,$zero,0x1
.L0f171308:
/*  f171308:	84780000 */ 	lh	$t8,0x0($v1)
/*  f17130c:	a44c0000 */ 	sh	$t4,0x0($v0)
/*  f171310:	a4ac0000 */ 	sh	$t4,0x0($a1)
/*  f171314:	07010013 */ 	bgez	$t8,.L0f171364
/*  f171318:	27b02070 */ 	addiu	$s0,$sp,0x2070
/*  f17131c:	84790002 */ 	lh	$t9,0x2($v1)
/*  f171320:	252e2710 */ 	addiu	$t6,$t1,0x2710
/*  f171324:	07230010 */ 	bgezl	$t9,.L0f171368
/*  f171328:	848e0000 */ 	lh	$t6,0x0($a0)
/*  f17132c:	a46e0000 */ 	sh	$t6,0x0($v1)
/*  f171330:	848f0000 */ 	lh	$t7,0x0($a0)
/*  f171334:	01209025 */ 	or	$s2,$t1,$zero
/*  f171338:	a44a0000 */ 	sh	$t2,0x0($v0)
/*  f17133c:	05e10007 */ 	bgez	$t7,.L0f17135c
/*  f171340:	00000000 */ 	nop
/*  f171344:	84980002 */ 	lh	$t8,0x2($a0)
/*  f171348:	24f92710 */ 	addiu	$t9,$a3,0x2710
/*  f17134c:	07010003 */ 	bgez	$t8,.L0f17135c
/*  f171350:	00000000 */ 	nop
/*  f171354:	10000037 */ 	b	.L0f171434
/*  f171358:	a4790002 */ 	sh	$t9,0x2($v1)
.L0f17135c:
/*  f17135c:	10000035 */ 	b	.L0f171434
/*  f171360:	a4670002 */ 	sh	$a3,0x2($v1)
.L0f171364:
/*  f171364:	848e0000 */ 	lh	$t6,0x0($a0)
.L0f171368:
/*  f171368:	87b80070 */ 	lh	$t8,0x70($sp)
/*  f17136c:	05c10013 */ 	bgez	$t6,.L0f1713bc
/*  f171370:	00000000 */ 	nop
/*  f171374:	848f0002 */ 	lh	$t7,0x2($a0)
/*  f171378:	05e10010 */ 	bgez	$t7,.L0f1713bc
/*  f17137c:	00000000 */ 	nop
/*  f171380:	24f82710 */ 	addiu	$t8,$a3,0x2710
/*  f171384:	a4980000 */ 	sh	$t8,0x0($a0)
/*  f171388:	84790000 */ 	lh	$t9,0x0($v1)
/*  f17138c:	00e09025 */ 	or	$s2,$a3,$zero
/*  f171390:	a4aa0000 */ 	sh	$t2,0x0($a1)
/*  f171394:	07210007 */ 	bgez	$t9,.L0f1713b4
/*  f171398:	00000000 */ 	nop
/*  f17139c:	846e0002 */ 	lh	$t6,0x2($v1)
/*  f1713a0:	252f2710 */ 	addiu	$t7,$t1,0x2710
/*  f1713a4:	05c10003 */ 	bgez	$t6,.L0f1713b4
/*  f1713a8:	00000000 */ 	nop
/*  f1713ac:	10000021 */ 	b	.L0f171434
/*  f1713b0:	a48f0002 */ 	sh	$t7,0x2($a0)
.L0f1713b4:
/*  f1713b4:	1000001f */ 	b	.L0f171434
/*  f1713b8:	a4890002 */ 	sh	$t1,0x2($a0)
.L0f1713bc:
/*  f1713bc:	07010009 */ 	bgez	$t8,.L0f1713e4
/*  f1713c0:	00009025 */ 	or	$s2,$zero,$zero
/*  f1713c4:	0000c880 */ 	sll	$t9,$zero,0x2
/*  f1713c8:	02791021 */ 	addu	$v0,$s3,$t9
/*  f1713cc:	844e0002 */ 	lh	$t6,0x2($v0)
/*  f1713d0:	97af2070 */ 	lhu	$t7,0x2070($sp)
/*  f1713d4:	05c10003 */ 	bgez	$t6,.L0f1713e4
/*  f1713d8:	29e1270f */ 	slti	$at,$t7,0x270f
/*  f1713dc:	50200011 */ 	beqzl	$at,.L0f171424
/*  f1713e0:	00127040 */ 	sll	$t6,$s2,0x1
.L0f1713e4:
/*  f1713e4:	26520001 */ 	addiu	$s2,$s2,0x1
.L0f1713e8:
/*  f1713e8:	0012c080 */ 	sll	$t8,$s2,0x2
/*  f1713ec:	02781021 */ 	addu	$v0,$s3,$t8
/*  f1713f0:	84590000 */ 	lh	$t9,0x0($v0)
/*  f1713f4:	0723fffc */ 	bgezl	$t9,.L0f1713e8
/*  f1713f8:	26520001 */ 	addiu	$s2,$s2,0x1
/*  f1713fc:	844e0002 */ 	lh	$t6,0x2($v0)
/*  f171400:	05c3fff9 */ 	bgezl	$t6,.L0f1713e8
/*  f171404:	26520001 */ 	addiu	$s2,$s2,0x1
/*  f171408:	00127840 */ 	sll	$t7,$s2,0x1
/*  f17140c:	016fc021 */ 	addu	$t8,$t3,$t7
/*  f171410:	97190000 */ 	lhu	$t9,0x0($t8)
/*  f171414:	2b21270f */ 	slti	$at,$t9,0x270f
/*  f171418:	5420fff3 */ 	bnezl	$at,.L0f1713e8
/*  f17141c:	26520001 */ 	addiu	$s2,$s2,0x1
/*  f171420:	00127040 */ 	sll	$t6,$s2,0x1
.L0f171424:
/*  f171424:	016e7821 */ 	addu	$t7,$t3,$t6
/*  f171428:	a5ea0000 */ 	sh	$t2,0x0($t7)
/*  f17142c:	a4490000 */ 	sh	$t1,0x0($v0)
/*  f171430:	a4470002 */ 	sh	$a3,0x2($v0)
.L0f171434:
/*  f171434:	1a800019 */ 	blez	$s4,.L0f17149c
/*  f171438:	00000000 */ 	nop
.L0f17143c:
/*  f17143c:	96040000 */ 	lhu	$a0,0x0($s0)
/*  f171440:	01001825 */ 	or	$v1,$t0,$zero
/*  f171444:	00c02825 */ 	or	$a1,$a2,$zero
/*  f171448:	0088082a */ 	slt	$at,$a0,$t0
/*  f17144c:	1020000a */ 	beqz	$at,.L0f171478
/*  f171450:	00801025 */ 	or	$v0,$a0,$zero
/*  f171454:	00a3082a */ 	slt	$at,$a1,$v1
/*  f171458:	50200005 */ 	beqzl	$at,.L0f171470
/*  f17145c:	3086ffff */ 	andi	$a2,$a0,0xffff
/*  f171460:	3088ffff */ 	andi	$t0,$a0,0xffff
/*  f171464:	10000009 */ 	b	.L0f17148c
/*  f171468:	02204825 */ 	or	$t1,$s1,$zero
/*  f17146c:	3086ffff */ 	andi	$a2,$a0,0xffff
.L0f171470:
/*  f171470:	10000006 */ 	b	.L0f17148c
/*  f171474:	02203825 */ 	or	$a3,$s1,$zero
.L0f171478:
/*  f171478:	0045082a */ 	slt	$at,$v0,$a1
/*  f17147c:	50200004 */ 	beqzl	$at,.L0f171490
/*  f171480:	26310001 */ 	addiu	$s1,$s1,0x1
/*  f171484:	3086ffff */ 	andi	$a2,$a0,0xffff
/*  f171488:	02203825 */ 	or	$a3,$s1,$zero
.L0f17148c:
/*  f17148c:	26310001 */ 	addiu	$s1,$s1,0x1
.L0f171490:
/*  f171490:	1634ffea */ 	bne	$s1,$s4,.L0f17143c
/*  f171494:	26100002 */ 	addiu	$s0,$s0,0x2
/*  f171498:	00008825 */ 	or	$s1,$zero,$zero
.L0f17149c:
/*  f17149c:	51a80004 */ 	beql	$t5,$t0,.L0f1714b0
/*  f1714a0:	241f0001 */ 	addiu	$ra,$zero,0x1
/*  f1714a4:	15a60002 */ 	bne	$t5,$a2,.L0f1714b0
/*  f1714a8:	00000000 */ 	nop
/*  f1714ac:	241f0001 */ 	addiu	$ra,$zero,0x1
.L0f1714b0:
/*  f1714b0:	53e0ff87 */ 	beqzl	$ra,.L0f1712d0
/*  f1714b4:	00077040 */ 	sll	$t6,$a3,0x1
/*  f1714b8:	8fb83074 */ 	lw	$t8,0x3074($sp)
/*  f1714bc:	afb20068 */ 	sw	$s2,0x68($sp)
/*  f1714c0:	27b30070 */ 	addiu	$s3,$sp,0x70
/*  f1714c4:	1b00001b */ 	blez	$t8,.L0f171534
/*  f1714c8:	2a412710 */ 	slti	$at,$s2,0x2710
.L0f1714cc:
/*  f1714cc:	1020000b */ 	beqz	$at,.L0f1714fc
/*  f1714d0:	02408025 */ 	or	$s0,$s2,$zero
.L0f1714d4:
/*  f1714d4:	0fc5cd4f */ 	jal	texReadBits
/*  f1714d8:	24040001 */ 	addiu	$a0,$zero,0x1
/*  f1714dc:	0010c880 */ 	sll	$t9,$s0,0x2
/*  f1714e0:	02797021 */ 	addu	$t6,$s3,$t9
/*  f1714e4:	00027840 */ 	sll	$t7,$v0,0x1
/*  f1714e8:	01cfc021 */ 	addu	$t8,$t6,$t7
/*  f1714ec:	87100000 */ 	lh	$s0,0x0($t8)
/*  f1714f0:	2a012710 */ 	slti	$at,$s0,0x2710
/*  f1714f4:	1420fff7 */ 	bnez	$at,.L0f1714d4
/*  f1714f8:	00000000 */ 	nop
.L0f1714fc:
/*  f1714fc:	2a810101 */ 	slti	$at,$s4,0x101
/*  f171500:	10200005 */ 	beqz	$at,.L0f171518
/*  f171504:	260fd8f0 */ 	addiu	$t7,$s0,-10000
/*  f171508:	2619d8f0 */ 	addiu	$t9,$s0,-10000
/*  f17150c:	02b17021 */ 	addu	$t6,$s5,$s1
/*  f171510:	10000004 */ 	b	.L0f171524
/*  f171514:	a1d90000 */ 	sb	$t9,0x0($t6)
.L0f171518:
/*  f171518:	0011c040 */ 	sll	$t8,$s1,0x1
/*  f17151c:	02b8c821 */ 	addu	$t9,$s5,$t8
/*  f171520:	a72f0000 */ 	sh	$t7,0x0($t9)
.L0f171524:
/*  f171524:	8fae3074 */ 	lw	$t6,0x3074($sp)
/*  f171528:	26310001 */ 	addiu	$s1,$s1,0x1
/*  f17152c:	562effe7 */ 	bnel	$s1,$t6,.L0f1714cc
/*  f171530:	2a412710 */ 	slti	$at,$s2,0x2710
.L0f171534:
/*  f171534:	8fbf002c */ 	lw	$ra,0x2c($sp)
/*  f171538:	8fb00014 */ 	lw	$s0,0x14($sp)
/*  f17153c:	8fb10018 */ 	lw	$s1,0x18($sp)
/*  f171540:	8fb2001c */ 	lw	$s2,0x1c($sp)
/*  f171544:	8fb30020 */ 	lw	$s3,0x20($sp)
/*  f171548:	8fb40024 */ 	lw	$s4,0x24($sp)
/*  f17154c:	8fb50028 */ 	lw	$s5,0x28($sp)
/*  f171550:	03e00008 */ 	jr	$ra
/*  f171554:	27bd3070 */ 	addiu	$sp,$sp,0x3070
);

GLOBAL_ASM(
glabel texInflateRle
/*  f171558:	27bdffc0 */ 	addiu	$sp,$sp,-64
/*  f17155c:	afb20020 */ 	sw	$s2,0x20($sp)
/*  f171560:	00809025 */ 	or	$s2,$a0,$zero
/*  f171564:	afbf003c */ 	sw	$ra,0x3c($sp)
/*  f171568:	afbe0038 */ 	sw	$s8,0x38($sp)
/*  f17156c:	00a0f025 */ 	or	$s8,$a1,$zero
/*  f171570:	afb70034 */ 	sw	$s7,0x34($sp)
/*  f171574:	afb60030 */ 	sw	$s6,0x30($sp)
/*  f171578:	afb5002c */ 	sw	$s5,0x2c($sp)
/*  f17157c:	afb40028 */ 	sw	$s4,0x28($sp)
/*  f171580:	afb30024 */ 	sw	$s3,0x24($sp)
/*  f171584:	afb1001c */ 	sw	$s1,0x1c($sp)
/*  f171588:	afb00018 */ 	sw	$s0,0x18($sp)
/*  f17158c:	0fc5cd4f */ 	jal	texReadBits
/*  f171590:	24040003 */ 	addiu	$a0,$zero,0x3
/*  f171594:	0040b025 */ 	or	$s6,$v0,$zero
/*  f171598:	0fc5cd4f */ 	jal	texReadBits
/*  f17159c:	24040003 */ 	addiu	$a0,$zero,0x3
/*  f1715a0:	0040b825 */ 	or	$s7,$v0,$zero
/*  f1715a4:	0fc5cd4f */ 	jal	texReadBits
/*  f1715a8:	24040004 */ 	addiu	$a0,$zero,0x4
/*  f1715ac:	02d77021 */ 	addu	$t6,$s6,$s7
/*  f1715b0:	01c22021 */ 	addu	$a0,$t6,$v0
/*  f1715b4:	24830001 */ 	addiu	$v1,$a0,0x1
/*  f1715b8:	0040a025 */ 	or	$s4,$v0,$zero
/*  f1715bc:	18600005 */ 	blez	$v1,.L0f1715d4
/*  f1715c0:	0000a825 */ 	or	$s5,$zero,$zero
.L0f1715c4:
/*  f1715c4:	00621823 */ 	subu	$v1,$v1,$v0
/*  f1715c8:	2463ffff */ 	addiu	$v1,$v1,-1
/*  f1715cc:	1c60fffd */ 	bgtz	$v1,.L0f1715c4
/*  f1715d0:	26b50001 */ 	addiu	$s5,$s5,0x1
.L0f1715d4:
/*  f1715d4:	1bc00047 */ 	blez	$s8,.L0f1716f4
/*  f1715d8:	00008825 */ 	or	$s1,$zero,$zero
.L0f1715dc:
/*  f1715dc:	0fc5cd4f */ 	jal	texReadBits
/*  f1715e0:	24040001 */ 	addiu	$a0,$zero,0x1
/*  f1715e4:	1440000f */ 	bnez	$v0,.L0f171624
/*  f1715e8:	2a810009 */ 	slti	$at,$s4,0x9
/*  f1715ec:	10200007 */ 	beqz	$at,.L0f17160c
/*  f1715f0:	02802025 */ 	or	$a0,$s4,$zero
/*  f1715f4:	02802025 */ 	or	$a0,$s4,$zero
/*  f1715f8:	0fc5cd4f */ 	jal	texReadBits
/*  f1715fc:	02518021 */ 	addu	$s0,$s2,$s1
/*  f171600:	a2020000 */ 	sb	$v0,0x0($s0)
/*  f171604:	10000038 */ 	b	.L0f1716e8
/*  f171608:	26310001 */ 	addiu	$s1,$s1,0x1
.L0f17160c:
/*  f17160c:	00117840 */ 	sll	$t7,$s1,0x1
/*  f171610:	0fc5cd4f */ 	jal	texReadBits
/*  f171614:	024f8021 */ 	addu	$s0,$s2,$t7
/*  f171618:	a6020000 */ 	sh	$v0,0x0($s0)
/*  f17161c:	10000032 */ 	b	.L0f1716e8
/*  f171620:	26310001 */ 	addiu	$s1,$s1,0x1
.L0f171624:
/*  f171624:	0fc5cd4f */ 	jal	texReadBits
/*  f171628:	02c02025 */ 	or	$a0,$s6,$zero
/*  f17162c:	02229823 */ 	subu	$s3,$s1,$v0
/*  f171630:	2673ffff */ 	addiu	$s3,$s3,-1
/*  f171634:	0fc5cd4f */ 	jal	texReadBits
/*  f171638:	02e02025 */ 	or	$a0,$s7,$zero
/*  f17163c:	00551821 */ 	addu	$v1,$v0,$s5
/*  f171640:	2a810009 */ 	slti	$at,$s4,0x9
/*  f171644:	10200014 */ 	beqz	$at,.L0f171698
/*  f171648:	00602825 */ 	or	$a1,$v1,$zero
/*  f17164c:	0263c021 */ 	addu	$t8,$s3,$v1
/*  f171650:	0278082a */ 	slt	$at,$s3,$t8
/*  f171654:	02601025 */ 	or	$v0,$s3,$zero
/*  f171658:	1020000a */ 	beqz	$at,.L0f171684
/*  f17165c:	02518021 */ 	addu	$s0,$s2,$s1
/*  f171660:	02652021 */ 	addu	$a0,$s3,$a1
/*  f171664:	02531821 */ 	addu	$v1,$s2,$s3
.L0f171668:
/*  f171668:	90790000 */ 	lbu	$t9,0x0($v1)
/*  f17166c:	24420001 */ 	addiu	$v0,$v0,0x1
/*  f171670:	24630001 */ 	addiu	$v1,$v1,0x1
/*  f171674:	26310001 */ 	addiu	$s1,$s1,0x1
/*  f171678:	26100001 */ 	addiu	$s0,$s0,0x1
/*  f17167c:	1444fffa */ 	bne	$v0,$a0,.L0f171668
/*  f171680:	a219ffff */ 	sb	$t9,-0x1($s0)
.L0f171684:
/*  f171684:	0fc5cd4f */ 	jal	texReadBits
/*  f171688:	02802025 */ 	or	$a0,$s4,$zero
/*  f17168c:	a2020000 */ 	sb	$v0,0x0($s0)
/*  f171690:	10000015 */ 	b	.L0f1716e8
/*  f171694:	26310001 */ 	addiu	$s1,$s1,0x1
.L0f171698:
/*  f171698:	02634821 */ 	addu	$t1,$s3,$v1
/*  f17169c:	0269082a */ 	slt	$at,$s3,$t1
/*  f1716a0:	00114040 */ 	sll	$t0,$s1,0x1
/*  f1716a4:	02488021 */ 	addu	$s0,$s2,$t0
/*  f1716a8:	1020000b */ 	beqz	$at,.L0f1716d8
/*  f1716ac:	02601025 */ 	or	$v0,$s3,$zero
/*  f1716b0:	00135040 */ 	sll	$t2,$s3,0x1
/*  f1716b4:	024a1821 */ 	addu	$v1,$s2,$t2
/*  f1716b8:	02652021 */ 	addu	$a0,$s3,$a1
.L0f1716bc:
/*  f1716bc:	946b0000 */ 	lhu	$t3,0x0($v1)
/*  f1716c0:	24420001 */ 	addiu	$v0,$v0,0x1
/*  f1716c4:	24630002 */ 	addiu	$v1,$v1,0x2
/*  f1716c8:	26310001 */ 	addiu	$s1,$s1,0x1
/*  f1716cc:	26100002 */ 	addiu	$s0,$s0,0x2
/*  f1716d0:	1444fffa */ 	bne	$v0,$a0,.L0f1716bc
/*  f1716d4:	a60bfffe */ 	sh	$t3,-0x2($s0)
.L0f1716d8:
/*  f1716d8:	0fc5cd4f */ 	jal	texReadBits
/*  f1716dc:	02802025 */ 	or	$a0,$s4,$zero
/*  f1716e0:	a6020000 */ 	sh	$v0,0x0($s0)
/*  f1716e4:	26310001 */ 	addiu	$s1,$s1,0x1
.L0f1716e8:
/*  f1716e8:	023e082a */ 	slt	$at,$s1,$s8
/*  f1716ec:	1420ffbb */ 	bnez	$at,.L0f1715dc
/*  f1716f0:	00000000 */ 	nop
.L0f1716f4:
/*  f1716f4:	8fbf003c */ 	lw	$ra,0x3c($sp)
/*  f1716f8:	8fb00018 */ 	lw	$s0,0x18($sp)
/*  f1716fc:	8fb1001c */ 	lw	$s1,0x1c($sp)
/*  f171700:	8fb20020 */ 	lw	$s2,0x20($sp)
/*  f171704:	8fb30024 */ 	lw	$s3,0x24($sp)
/*  f171708:	8fb40028 */ 	lw	$s4,0x28($sp)
/*  f17170c:	8fb5002c */ 	lw	$s5,0x2c($sp)
/*  f171710:	8fb60030 */ 	lw	$s6,0x30($sp)
/*  f171714:	8fb70034 */ 	lw	$s7,0x34($sp)
/*  f171718:	8fbe0038 */ 	lw	$s8,0x38($sp)
/*  f17171c:	03e00008 */ 	jr	$ra
/*  f171720:	27bd0040 */ 	addiu	$sp,$sp,0x40
);

/**
 * Populate a lookup table by reading it out of the bit string.
 *
 * The first 11 bits denate the number of colours in the lookup table.
 * The data following this is a list of colours, where each colour is sized
 * according to the texture's format.
 *
 * This function does NOT work with pixel formats of 8 bits or less.
 */
s32 texBuildLookup(u8 *lookup, s32 bitsperpixel)
{
	s32 numcolours = texReadBits(11);
	s32 i;

	if (bitsperpixel <= 16) {
		u16 *dst = (u16 *)lookup;

		for (i = 0; i < numcolours; i++) {
			dst[i] = texReadBits(bitsperpixel);
		}
	} else if (bitsperpixel <= 24) {
		u32 *dst = (u32 *)lookup;

		for (i = 0; i < numcolours; i++) {
			dst[i] = texReadBits(bitsperpixel);
		}
	} else {
		u32 *dst = (u32 *)lookup;

		for (i = 0; i < numcolours; i++) {
			dst[i] = texReadBits(24) << 8 | texReadBits(bitsperpixel - 24);
		}
	}

	return numcolours;
}

s32 func0f171828(s32 arg0)
{
	s32 count = 0;

	arg0--;

	while (arg0 > 0) {
		arg0 >>= 1;
		count++;
	}

	return count;
}

void texReadAlphaBits(u8 *dst, s32 count)
{
	s32 i;

	for (i = 0; i < count; i++) {
		dst[i] = texReadBits(1);
	}
}

GLOBAL_ASM(
glabel texInflateMethod1
.late_rodata
glabel var7f1b7ac0
.word texInflateMethod1+0x70 # f171910
glabel var7f1b7ac4
.word texInflateMethod1+0x150 # f1719f0
glabel var7f1b7ac8
.word texInflateMethod1+0xe8 # f171988
glabel var7f1b7acc
.word texInflateMethod1+0x1b0 # f171a50
glabel var7f1b7ad0
.word texInflateMethod1+0x150 # f1719f0
glabel var7f1b7ad4
.word texInflateMethod1+0x218 # f171ab8
glabel var7f1b7ad8
.word texInflateMethod1+0x274 # f171b14
glabel var7f1b7adc
.word texInflateMethod1+0x218 # f171ab8
glabel var7f1b7ae0
.word texInflateMethod1+0x274 # f171b14
.text
/*  f1718a0:	27bdffc8 */ 	addiu	$sp,$sp,-56
/*  f1718a4:	afb50028 */ 	sw	$s5,0x28($sp)
/*  f1718a8:	2495000f */ 	addiu	$s5,$a0,0xf
/*  f1718ac:	2401fff0 */ 	addiu	$at,$zero,-16
/*  f1718b0:	02a17024 */ 	and	$t6,$s5,$at
/*  f1718b4:	afb30020 */ 	sw	$s3,0x20($sp)
/*  f1718b8:	2401fff8 */ 	addiu	$at,$zero,-8
/*  f1718bc:	24820007 */ 	addiu	$v0,$a0,0x7
/*  f1718c0:	00419824 */ 	and	$s3,$v0,$at
/*  f1718c4:	afb70030 */ 	sw	$s7,0x30($sp)
/*  f1718c8:	afb6002c */ 	sw	$s6,0x2c($sp)
/*  f1718cc:	afb2001c */ 	sw	$s2,0x1c($sp)
/*  f1718d0:	2ce10009 */ 	sltiu	$at,$a3,0x9
/*  f1718d4:	00a09025 */ 	or	$s2,$a1,$zero
/*  f1718d8:	00c0b825 */ 	or	$s7,$a2,$zero
/*  f1718dc:	afbf0034 */ 	sw	$ra,0x34($sp)
/*  f1718e0:	afb40024 */ 	sw	$s4,0x24($sp)
/*  f1718e4:	afb10018 */ 	sw	$s1,0x18($sp)
/*  f1718e8:	afb00014 */ 	sw	$s0,0x14($sp)
/*  f1718ec:	01c0a825 */ 	or	$s5,$t6,$zero
/*  f1718f0:	102000a1 */ 	beqz	$at,.L0f171b78
/*  f1718f4:	0260b025 */ 	or	$s6,$s3,$zero
/*  f1718f8:	0007c080 */ 	sll	$t8,$a3,0x2
/*  f1718fc:	3c017f1b */ 	lui	$at,%hi(var7f1b7ac0)
/*  f171900:	00380821 */ 	addu	$at,$at,$t8
/*  f171904:	8c387ac0 */ 	lw	$t8,%lo(var7f1b7ac0)($at)
/*  f171908:	03000008 */ 	jr	$t8
/*  f17190c:	00000000 */ 	nop
/*  f171910:	1ae00016 */ 	blez	$s7,.L0f17196c
/*  f171914:	0000a025 */ 	or	$s4,$zero,$zero
/*  f171918:	26530003 */ 	addiu	$s3,$s2,0x3
/*  f17191c:	32790ffc */ 	andi	$t9,$s3,0xffc
/*  f171920:	00199880 */ 	sll	$s3,$t9,0x2
.L0f171924:
/*  f171924:	1a40000e */ 	blez	$s2,.L0f171960
/*  f171928:	00008025 */ 	or	$s0,$zero,$zero
/*  f17192c:	02a08825 */ 	or	$s1,$s5,$zero
.L0f171930:
/*  f171930:	0fc5cd4f */ 	jal	texReadBits
/*  f171934:	24040010 */ 	addiu	$a0,$zero,0x10
/*  f171938:	00024c00 */ 	sll	$t1,$v0,0x10
/*  f17193c:	ae290000 */ 	sw	$t1,0x0($s1)
/*  f171940:	0fc5cd4f */ 	jal	texReadBits
/*  f171944:	24040010 */ 	addiu	$a0,$zero,0x10
/*  f171948:	8e2a0000 */ 	lw	$t2,0x0($s1)
/*  f17194c:	26100001 */ 	addiu	$s0,$s0,0x1
/*  f171950:	26310004 */ 	addiu	$s1,$s1,0x4
/*  f171954:	01425825 */ 	or	$t3,$t2,$v0
/*  f171958:	1612fff5 */ 	bne	$s0,$s2,.L0f171930
/*  f17195c:	ae2bfffc */ 	sw	$t3,-0x4($s1)
.L0f171960:
/*  f171960:	26940001 */ 	addiu	$s4,$s4,0x1
/*  f171964:	1697ffef */ 	bne	$s4,$s7,.L0f171924
/*  f171968:	02b3a821 */ 	addu	$s5,$s5,$s3
.L0f17196c:
/*  f17196c:	264c0003 */ 	addiu	$t4,$s2,0x3
/*  f171970:	318d0ffc */ 	andi	$t5,$t4,0xffc
/*  f171974:	01b70019 */ 	multu	$t5,$s7
/*  f171978:	00001012 */ 	mflo	$v0
/*  f17197c:	00027080 */ 	sll	$t6,$v0,0x2
/*  f171980:	1000007e */ 	b	.L0f171b7c
/*  f171984:	01c01025 */ 	or	$v0,$t6,$zero
/*  f171988:	1ae00012 */ 	blez	$s7,.L0f1719d4
/*  f17198c:	0000a025 */ 	or	$s4,$zero,$zero
/*  f171990:	26530003 */ 	addiu	$s3,$s2,0x3
/*  f171994:	326f0ffc */ 	andi	$t7,$s3,0xffc
/*  f171998:	000f9880 */ 	sll	$s3,$t7,0x2
.L0f17199c:
/*  f17199c:	1a40000a */ 	blez	$s2,.L0f1719c8
/*  f1719a0:	00008025 */ 	or	$s0,$zero,$zero
/*  f1719a4:	02a08825 */ 	or	$s1,$s5,$zero
.L0f1719a8:
/*  f1719a8:	0fc5cd4f */ 	jal	texReadBits
/*  f1719ac:	24040018 */ 	addiu	$a0,$zero,0x18
/*  f1719b0:	0002ca00 */ 	sll	$t9,$v0,0x8
/*  f1719b4:	26100001 */ 	addiu	$s0,$s0,0x1
/*  f1719b8:	372800ff */ 	ori	$t0,$t9,0xff
/*  f1719bc:	26310004 */ 	addiu	$s1,$s1,0x4
/*  f1719c0:	1612fff9 */ 	bne	$s0,$s2,.L0f1719a8
/*  f1719c4:	ae28fffc */ 	sw	$t0,-0x4($s1)
.L0f1719c8:
/*  f1719c8:	26940001 */ 	addiu	$s4,$s4,0x1
/*  f1719cc:	1697fff3 */ 	bne	$s4,$s7,.L0f17199c
/*  f1719d0:	02b3a821 */ 	addu	$s5,$s5,$s3
.L0f1719d4:
/*  f1719d4:	26490003 */ 	addiu	$t1,$s2,0x3
/*  f1719d8:	312a0ffc */ 	andi	$t2,$t1,0xffc
/*  f1719dc:	01570019 */ 	multu	$t2,$s7
/*  f1719e0:	00001012 */ 	mflo	$v0
/*  f1719e4:	00025880 */ 	sll	$t3,$v0,0x2
/*  f1719e8:	10000064 */ 	b	.L0f171b7c
/*  f1719ec:	01601025 */ 	or	$v0,$t3,$zero
/*  f1719f0:	1ae00010 */ 	blez	$s7,.L0f171a34
/*  f1719f4:	0000a025 */ 	or	$s4,$zero,$zero
/*  f1719f8:	26530003 */ 	addiu	$s3,$s2,0x3
/*  f1719fc:	326c0ffc */ 	andi	$t4,$s3,0xffc
/*  f171a00:	000c9840 */ 	sll	$s3,$t4,0x1
.L0f171a04:
/*  f171a04:	1a400008 */ 	blez	$s2,.L0f171a28
/*  f171a08:	00008025 */ 	or	$s0,$zero,$zero
/*  f171a0c:	02c08825 */ 	or	$s1,$s6,$zero
.L0f171a10:
/*  f171a10:	0fc5cd4f */ 	jal	texReadBits
/*  f171a14:	24040010 */ 	addiu	$a0,$zero,0x10
/*  f171a18:	26100001 */ 	addiu	$s0,$s0,0x1
/*  f171a1c:	26310002 */ 	addiu	$s1,$s1,0x2
/*  f171a20:	1612fffb */ 	bne	$s0,$s2,.L0f171a10
/*  f171a24:	a622fffe */ 	sh	$v0,-0x2($s1)
.L0f171a28:
/*  f171a28:	26940001 */ 	addiu	$s4,$s4,0x1
/*  f171a2c:	1697fff5 */ 	bne	$s4,$s7,.L0f171a04
/*  f171a30:	02d3b021 */ 	addu	$s6,$s6,$s3
.L0f171a34:
/*  f171a34:	264e0003 */ 	addiu	$t6,$s2,0x3
/*  f171a38:	31cf0ffc */ 	andi	$t7,$t6,0xffc
/*  f171a3c:	01f70019 */ 	multu	$t7,$s7
/*  f171a40:	00001012 */ 	mflo	$v0
/*  f171a44:	0002c040 */ 	sll	$t8,$v0,0x1
/*  f171a48:	1000004c */ 	b	.L0f171b7c
/*  f171a4c:	03001025 */ 	or	$v0,$t8,$zero
/*  f171a50:	1ae00012 */ 	blez	$s7,.L0f171a9c
/*  f171a54:	0000a025 */ 	or	$s4,$zero,$zero
/*  f171a58:	26530003 */ 	addiu	$s3,$s2,0x3
/*  f171a5c:	32790ffc */ 	andi	$t9,$s3,0xffc
/*  f171a60:	00199840 */ 	sll	$s3,$t9,0x1
.L0f171a64:
/*  f171a64:	1a40000a */ 	blez	$s2,.L0f171a90
/*  f171a68:	00008025 */ 	or	$s0,$zero,$zero
/*  f171a6c:	02c08825 */ 	or	$s1,$s6,$zero
.L0f171a70:
/*  f171a70:	0fc5cd4f */ 	jal	texReadBits
/*  f171a74:	2404000f */ 	addiu	$a0,$zero,0xf
/*  f171a78:	00024840 */ 	sll	$t1,$v0,0x1
/*  f171a7c:	26100001 */ 	addiu	$s0,$s0,0x1
/*  f171a80:	352a0001 */ 	ori	$t2,$t1,0x1
/*  f171a84:	26310002 */ 	addiu	$s1,$s1,0x2
/*  f171a88:	1612fff9 */ 	bne	$s0,$s2,.L0f171a70
/*  f171a8c:	a62afffe */ 	sh	$t2,-0x2($s1)
.L0f171a90:
/*  f171a90:	26940001 */ 	addiu	$s4,$s4,0x1
/*  f171a94:	1697fff3 */ 	bne	$s4,$s7,.L0f171a64
/*  f171a98:	02d3b021 */ 	addu	$s6,$s6,$s3
.L0f171a9c:
/*  f171a9c:	264b0003 */ 	addiu	$t3,$s2,0x3
/*  f171aa0:	316c0ffc */ 	andi	$t4,$t3,0xffc
/*  f171aa4:	01970019 */ 	multu	$t4,$s7
/*  f171aa8:	00001012 */ 	mflo	$v0
/*  f171aac:	00026840 */ 	sll	$t5,$v0,0x1
/*  f171ab0:	10000032 */ 	b	.L0f171b7c
/*  f171ab4:	01a01025 */ 	or	$v0,$t5,$zero
/*  f171ab8:	1ae00010 */ 	blez	$s7,.L0f171afc
/*  f171abc:	0000a025 */ 	or	$s4,$zero,$zero
/*  f171ac0:	26550007 */ 	addiu	$s5,$s2,0x7
/*  f171ac4:	32ae0ff8 */ 	andi	$t6,$s5,0xff8
/*  f171ac8:	01c0a825 */ 	or	$s5,$t6,$zero
.L0f171acc:
/*  f171acc:	1a400008 */ 	blez	$s2,.L0f171af0
/*  f171ad0:	00008025 */ 	or	$s0,$zero,$zero
/*  f171ad4:	02608825 */ 	or	$s1,$s3,$zero
.L0f171ad8:
/*  f171ad8:	0fc5cd4f */ 	jal	texReadBits
/*  f171adc:	24040008 */ 	addiu	$a0,$zero,0x8
/*  f171ae0:	26100001 */ 	addiu	$s0,$s0,0x1
/*  f171ae4:	26310001 */ 	addiu	$s1,$s1,0x1
/*  f171ae8:	1612fffb */ 	bne	$s0,$s2,.L0f171ad8
/*  f171aec:	a222ffff */ 	sb	$v0,-0x1($s1)
.L0f171af0:
/*  f171af0:	26940001 */ 	addiu	$s4,$s4,0x1
/*  f171af4:	1697fff5 */ 	bne	$s4,$s7,.L0f171acc
/*  f171af8:	02759821 */ 	addu	$s3,$s3,$s5
.L0f171afc:
/*  f171afc:	264f0007 */ 	addiu	$t7,$s2,0x7
/*  f171b00:	31f80ff8 */ 	andi	$t8,$t7,0xff8
/*  f171b04:	03170019 */ 	multu	$t8,$s7
/*  f171b08:	00001012 */ 	mflo	$v0
/*  f171b0c:	1000001c */ 	b	.L0f171b80
/*  f171b10:	8fbf0034 */ 	lw	$ra,0x34($sp)
/*  f171b14:	1ae00011 */ 	blez	$s7,.L0f171b5c
/*  f171b18:	0000a025 */ 	or	$s4,$zero,$zero
/*  f171b1c:	2651000f */ 	addiu	$s1,$s2,0xf
/*  f171b20:	32390ff0 */ 	andi	$t9,$s1,0xff0
/*  f171b24:	00198843 */ 	sra	$s1,$t9,0x1
.L0f171b28:
/*  f171b28:	1a400009 */ 	blez	$s2,.L0f171b50
/*  f171b2c:	00008025 */ 	or	$s0,$zero,$zero
.L0f171b30:
/*  f171b30:	0fc5cd4f */ 	jal	texReadBits
/*  f171b34:	24040008 */ 	addiu	$a0,$zero,0x8
/*  f171b38:	00104843 */ 	sra	$t1,$s0,0x1
/*  f171b3c:	26100002 */ 	addiu	$s0,$s0,0x2
/*  f171b40:	0212082a */ 	slt	$at,$s0,$s2
/*  f171b44:	02695021 */ 	addu	$t2,$s3,$t1
/*  f171b48:	1420fff9 */ 	bnez	$at,.L0f171b30
/*  f171b4c:	a1420000 */ 	sb	$v0,0x0($t2)
.L0f171b50:
/*  f171b50:	26940001 */ 	addiu	$s4,$s4,0x1
/*  f171b54:	1697fff4 */ 	bne	$s4,$s7,.L0f171b28
/*  f171b58:	02719821 */ 	addu	$s3,$s3,$s1
.L0f171b5c:
/*  f171b5c:	264b000f */ 	addiu	$t3,$s2,0xf
/*  f171b60:	316c0ff0 */ 	andi	$t4,$t3,0xff0
/*  f171b64:	000c6843 */ 	sra	$t5,$t4,0x1
/*  f171b68:	01b70019 */ 	multu	$t5,$s7
/*  f171b6c:	00001012 */ 	mflo	$v0
/*  f171b70:	10000003 */ 	b	.L0f171b80
/*  f171b74:	8fbf0034 */ 	lw	$ra,0x34($sp)
.L0f171b78:
/*  f171b78:	00001025 */ 	or	$v0,$zero,$zero
.L0f171b7c:
/*  f171b7c:	8fbf0034 */ 	lw	$ra,0x34($sp)
.L0f171b80:
/*  f171b80:	8fb00014 */ 	lw	$s0,0x14($sp)
/*  f171b84:	8fb10018 */ 	lw	$s1,0x18($sp)
/*  f171b88:	8fb2001c */ 	lw	$s2,0x1c($sp)
/*  f171b8c:	8fb30020 */ 	lw	$s3,0x20($sp)
/*  f171b90:	8fb40024 */ 	lw	$s4,0x24($sp)
/*  f171b94:	8fb50028 */ 	lw	$s5,0x28($sp)
/*  f171b98:	8fb6002c */ 	lw	$s6,0x2c($sp)
/*  f171b9c:	8fb70030 */ 	lw	$s7,0x30($sp)
/*  f171ba0:	03e00008 */ 	jr	$ra
/*  f171ba4:	27bd0038 */ 	addiu	$sp,$sp,0x38
);

GLOBAL_ASM(
glabel func0f171ba8
.late_rodata
glabel var7f1b7ae4
.word func0f171ba8+0x48 # f171bf0
glabel var7f1b7ae8
.word func0f171ba8+0x1b4 # f171d5c
glabel var7f1b7aec
.word func0f171ba8+0x108 # f171cb0
glabel var7f1b7af0
.word func0f171ba8+0x2fc # f171ea4
glabel var7f1b7af4
.word func0f171ba8+0x274 # f171e1c
glabel var7f1b7af8
.word func0f171ba8+0x3a8 # f171f50
glabel var7f1b7afc
.word func0f171ba8+0x490 # f172038
glabel var7f1b7b00
.word func0f171ba8+0x42c # f171fd4
glabel var7f1b7b04
.word func0f171ba8+0x548 # f1720f0
.text
/*  f171ba8:	27bdfff0 */ 	addiu	$sp,$sp,-16
/*  f171bac:	8fae0020 */ 	lw	$t6,0x20($sp)
/*  f171bb0:	afb00004 */ 	sw	$s0,0x4($sp)
/*  f171bb4:	00808025 */ 	or	$s0,$a0,$zero
/*  f171bb8:	2dc10009 */ 	sltiu	$at,$t6,0x9
/*  f171bbc:	afb2000c */ 	sw	$s2,0xc($sp)
/*  f171bc0:	afb10008 */ 	sw	$s1,0x8($sp)
/*  f171bc4:	00e04825 */ 	or	$t1,$a3,$zero
/*  f171bc8:	00e05025 */ 	or	$t2,$a3,$zero
/*  f171bcc:	00e05825 */ 	or	$t3,$a3,$zero
/*  f171bd0:	10200168 */ 	beqz	$at,.L0f172174
/*  f171bd4:	00004025 */ 	or	$t0,$zero,$zero
/*  f171bd8:	000e7080 */ 	sll	$t6,$t6,0x2
/*  f171bdc:	3c017f1b */ 	lui	$at,%hi(var7f1b7ae4)
/*  f171be0:	002e0821 */ 	addu	$at,$at,$t6
/*  f171be4:	8c2e7ae4 */ 	lw	$t6,%lo(var7f1b7ae4)($at)
/*  f171be8:	01c00008 */ 	jr	$t6
/*  f171bec:	00000000 */ 	nop
/*  f171bf0:	18c00028 */ 	blez	$a2,.L0f171c94
/*  f171bf4:	00002025 */ 	or	$a0,$zero,$zero
/*  f171bf8:	24ac0003 */ 	addiu	$t4,$a1,0x3
/*  f171bfc:	318f0ffc */ 	andi	$t7,$t4,0xffc
/*  f171c00:	000f6080 */ 	sll	$t4,$t7,0x2
.L0f171c04:
/*  f171c04:	18a00020 */ 	blez	$a1,.L0f171c88
/*  f171c08:	00001825 */ 	or	$v1,$zero,$zero
/*  f171c0c:	00a60019 */ 	multu	$a1,$a2
/*  f171c10:	02085821 */ 	addu	$t3,$s0,$t0
/*  f171c14:	01205025 */ 	or	$t2,$t1,$zero
/*  f171c18:	02081021 */ 	addu	$v0,$s0,$t0
/*  f171c1c:	00003812 */ 	mflo	$a3
/*  f171c20:	0007c880 */ 	sll	$t9,$a3,0x2
/*  f171c24:	0327c823 */ 	subu	$t9,$t9,$a3
/*  f171c28:	03287021 */ 	addu	$t6,$t9,$t0
/*  f171c2c:	00077840 */ 	sll	$t7,$a3,0x1
/*  f171c30:	016f9021 */ 	addu	$s2,$t3,$t7
/*  f171c34:	01d06821 */ 	addu	$t5,$t6,$s0
/*  f171c38:	01678821 */ 	addu	$s1,$t3,$a3
.L0f171c3c:
/*  f171c3c:	90590000 */ 	lbu	$t9,0x0($v0)
/*  f171c40:	91b80000 */ 	lbu	$t8,0x0($t5)
/*  f171c44:	24630001 */ 	addiu	$v1,$v1,0x1
/*  f171c48:	00197600 */ 	sll	$t6,$t9,0x18
/*  f171c4c:	92390000 */ 	lbu	$t9,0x0($s1)
/*  f171c50:	030e7825 */ 	or	$t7,$t8,$t6
/*  f171c54:	254a0004 */ 	addiu	$t2,$t2,0x4
/*  f171c58:	0019c400 */ 	sll	$t8,$t9,0x10
/*  f171c5c:	92590000 */ 	lbu	$t9,0x0($s2)
/*  f171c60:	01f87025 */ 	or	$t6,$t7,$t8
/*  f171c64:	25080001 */ 	addiu	$t0,$t0,0x1
/*  f171c68:	00197a00 */ 	sll	$t7,$t9,0x8
/*  f171c6c:	01cfc025 */ 	or	$t8,$t6,$t7
/*  f171c70:	ad58fffc */ 	sw	$t8,-0x4($t2)
/*  f171c74:	24420001 */ 	addiu	$v0,$v0,0x1
/*  f171c78:	25ad0001 */ 	addiu	$t5,$t5,0x1
/*  f171c7c:	26310001 */ 	addiu	$s1,$s1,0x1
/*  f171c80:	1465ffee */ 	bne	$v1,$a1,.L0f171c3c
/*  f171c84:	26520001 */ 	addiu	$s2,$s2,0x1
.L0f171c88:
/*  f171c88:	24840001 */ 	addiu	$a0,$a0,0x1
/*  f171c8c:	1486ffdd */ 	bne	$a0,$a2,.L0f171c04
/*  f171c90:	012c4821 */ 	addu	$t1,$t1,$t4
.L0f171c94:
/*  f171c94:	24b90003 */ 	addiu	$t9,$a1,0x3
/*  f171c98:	332e0ffc */ 	andi	$t6,$t9,0xffc
/*  f171c9c:	01c60019 */ 	multu	$t6,$a2
/*  f171ca0:	00001012 */ 	mflo	$v0
/*  f171ca4:	00027880 */ 	sll	$t7,$v0,0x2
/*  f171ca8:	10000133 */ 	b	.L0f172178
/*  f171cac:	01e01025 */ 	or	$v0,$t7,$zero
/*  f171cb0:	18c00023 */ 	blez	$a2,.L0f171d40
/*  f171cb4:	00002025 */ 	or	$a0,$zero,$zero
/*  f171cb8:	24ac0003 */ 	addiu	$t4,$a1,0x3
/*  f171cbc:	31980ffc */ 	andi	$t8,$t4,0xffc
/*  f171cc0:	00186080 */ 	sll	$t4,$t8,0x2
.L0f171cc4:
/*  f171cc4:	18a0001b */ 	blez	$a1,.L0f171d34
/*  f171cc8:	00001825 */ 	or	$v1,$zero,$zero
/*  f171ccc:	00a60019 */ 	multu	$a1,$a2
/*  f171cd0:	02087021 */ 	addu	$t6,$s0,$t0
/*  f171cd4:	01205025 */ 	or	$t2,$t1,$zero
/*  f171cd8:	02081021 */ 	addu	$v0,$s0,$t0
/*  f171cdc:	00003812 */ 	mflo	$a3
/*  f171ce0:	00077840 */ 	sll	$t7,$a3,0x1
/*  f171ce4:	01e8c021 */ 	addu	$t8,$t7,$t0
/*  f171ce8:	03105821 */ 	addu	$t3,$t8,$s0
/*  f171cec:	01c78821 */ 	addu	$s1,$t6,$a3
.L0f171cf0:
/*  f171cf0:	904f0000 */ 	lbu	$t7,0x0($v0)
/*  f171cf4:	91790000 */ 	lbu	$t9,0x0($t3)
/*  f171cf8:	24630001 */ 	addiu	$v1,$v1,0x1
/*  f171cfc:	000fc600 */ 	sll	$t8,$t7,0x18
/*  f171d00:	922f0000 */ 	lbu	$t7,0x0($s1)
/*  f171d04:	00197200 */ 	sll	$t6,$t9,0x8
/*  f171d08:	01d8c825 */ 	or	$t9,$t6,$t8
/*  f171d0c:	000f7400 */ 	sll	$t6,$t7,0x10
/*  f171d10:	032ec025 */ 	or	$t8,$t9,$t6
/*  f171d14:	370f00ff */ 	ori	$t7,$t8,0xff
/*  f171d18:	254a0004 */ 	addiu	$t2,$t2,0x4
/*  f171d1c:	ad4ffffc */ 	sw	$t7,-0x4($t2)
/*  f171d20:	25080001 */ 	addiu	$t0,$t0,0x1
/*  f171d24:	24420001 */ 	addiu	$v0,$v0,0x1
/*  f171d28:	26310001 */ 	addiu	$s1,$s1,0x1
/*  f171d2c:	1465fff0 */ 	bne	$v1,$a1,.L0f171cf0
/*  f171d30:	256b0001 */ 	addiu	$t3,$t3,0x1
.L0f171d34:
/*  f171d34:	24840001 */ 	addiu	$a0,$a0,0x1
/*  f171d38:	1486ffe2 */ 	bne	$a0,$a2,.L0f171cc4
/*  f171d3c:	012c4821 */ 	addu	$t1,$t1,$t4
.L0f171d40:
/*  f171d40:	24b90003 */ 	addiu	$t9,$a1,0x3
/*  f171d44:	332e0ffc */ 	andi	$t6,$t9,0xffc
/*  f171d48:	01c60019 */ 	multu	$t6,$a2
/*  f171d4c:	00001012 */ 	mflo	$v0
/*  f171d50:	0002c080 */ 	sll	$t8,$v0,0x2
/*  f171d54:	10000108 */ 	b	.L0f172178
/*  f171d58:	03001025 */ 	or	$v0,$t8,$zero
/*  f171d5c:	18c00028 */ 	blez	$a2,.L0f171e00
/*  f171d60:	00002025 */ 	or	$a0,$zero,$zero
/*  f171d64:	24ac0003 */ 	addiu	$t4,$a1,0x3
/*  f171d68:	318f0ffc */ 	andi	$t7,$t4,0xffc
/*  f171d6c:	000f6040 */ 	sll	$t4,$t7,0x1
.L0f171d70:
/*  f171d70:	18a00020 */ 	blez	$a1,.L0f171df4
/*  f171d74:	00001825 */ 	or	$v1,$zero,$zero
/*  f171d78:	00a60019 */ 	multu	$a1,$a2
/*  f171d7c:	02085821 */ 	addu	$t3,$s0,$t0
/*  f171d80:	02081021 */ 	addu	$v0,$s0,$t0
/*  f171d84:	01404825 */ 	or	$t1,$t2,$zero
/*  f171d88:	00003812 */ 	mflo	$a3
/*  f171d8c:	00077080 */ 	sll	$t6,$a3,0x2
/*  f171d90:	01c77023 */ 	subu	$t6,$t6,$a3
/*  f171d94:	01c8c021 */ 	addu	$t8,$t6,$t0
/*  f171d98:	00077840 */ 	sll	$t7,$a3,0x1
/*  f171d9c:	016f9021 */ 	addu	$s2,$t3,$t7
/*  f171da0:	03106821 */ 	addu	$t5,$t8,$s0
/*  f171da4:	01678821 */ 	addu	$s1,$t3,$a3
.L0f171da8:
/*  f171da8:	904e0000 */ 	lbu	$t6,0x0($v0)
/*  f171dac:	91b90000 */ 	lbu	$t9,0x0($t5)
/*  f171db0:	24630001 */ 	addiu	$v1,$v1,0x1
/*  f171db4:	000ec2c0 */ 	sll	$t8,$t6,0xb
/*  f171db8:	922e0000 */ 	lbu	$t6,0x0($s1)
/*  f171dbc:	03387825 */ 	or	$t7,$t9,$t8
/*  f171dc0:	25290002 */ 	addiu	$t1,$t1,0x2
/*  f171dc4:	000ec980 */ 	sll	$t9,$t6,0x6
/*  f171dc8:	924e0000 */ 	lbu	$t6,0x0($s2)
/*  f171dcc:	01f9c025 */ 	or	$t8,$t7,$t9
/*  f171dd0:	25080001 */ 	addiu	$t0,$t0,0x1
/*  f171dd4:	000e7840 */ 	sll	$t7,$t6,0x1
/*  f171dd8:	030fc825 */ 	or	$t9,$t8,$t7
/*  f171ddc:	a539fffe */ 	sh	$t9,-0x2($t1)
/*  f171de0:	24420001 */ 	addiu	$v0,$v0,0x1
/*  f171de4:	25ad0001 */ 	addiu	$t5,$t5,0x1
/*  f171de8:	26310001 */ 	addiu	$s1,$s1,0x1
/*  f171dec:	1465ffee */ 	bne	$v1,$a1,.L0f171da8
/*  f171df0:	26520001 */ 	addiu	$s2,$s2,0x1
.L0f171df4:
/*  f171df4:	24840001 */ 	addiu	$a0,$a0,0x1
/*  f171df8:	1486ffdd */ 	bne	$a0,$a2,.L0f171d70
/*  f171dfc:	014c5021 */ 	addu	$t2,$t2,$t4
.L0f171e00:
/*  f171e00:	24ae0003 */ 	addiu	$t6,$a1,0x3
/*  f171e04:	31d80ffc */ 	andi	$t8,$t6,0xffc
/*  f171e08:	03060019 */ 	multu	$t8,$a2
/*  f171e0c:	00001012 */ 	mflo	$v0
/*  f171e10:	00027840 */ 	sll	$t7,$v0,0x1
/*  f171e14:	100000d8 */ 	b	.L0f172178
/*  f171e18:	01e01025 */ 	or	$v0,$t7,$zero
/*  f171e1c:	18c0001a */ 	blez	$a2,.L0f171e88
/*  f171e20:	00002025 */ 	or	$a0,$zero,$zero
/*  f171e24:	24ac0003 */ 	addiu	$t4,$a1,0x3
/*  f171e28:	31990ffc */ 	andi	$t9,$t4,0xffc
/*  f171e2c:	00196040 */ 	sll	$t4,$t9,0x1
.L0f171e30:
/*  f171e30:	18a00012 */ 	blez	$a1,.L0f171e7c
/*  f171e34:	00001825 */ 	or	$v1,$zero,$zero
/*  f171e38:	00a60019 */ 	multu	$a1,$a2
/*  f171e3c:	02081021 */ 	addu	$v0,$s0,$t0
/*  f171e40:	01404825 */ 	or	$t1,$t2,$zero
/*  f171e44:	0000c012 */ 	mflo	$t8
/*  f171e48:	01187821 */ 	addu	$t7,$t0,$t8
/*  f171e4c:	01f06821 */ 	addu	$t5,$t7,$s0
.L0f171e50:
/*  f171e50:	904e0000 */ 	lbu	$t6,0x0($v0)
/*  f171e54:	91b90000 */ 	lbu	$t9,0x0($t5)
/*  f171e58:	24630001 */ 	addiu	$v1,$v1,0x1
/*  f171e5c:	000ec200 */ 	sll	$t8,$t6,0x8
/*  f171e60:	03387825 */ 	or	$t7,$t9,$t8
/*  f171e64:	25290002 */ 	addiu	$t1,$t1,0x2
/*  f171e68:	a52ffffe */ 	sh	$t7,-0x2($t1)
/*  f171e6c:	25080001 */ 	addiu	$t0,$t0,0x1
/*  f171e70:	24420001 */ 	addiu	$v0,$v0,0x1
/*  f171e74:	1465fff6 */ 	bne	$v1,$a1,.L0f171e50
/*  f171e78:	25ad0001 */ 	addiu	$t5,$t5,0x1
.L0f171e7c:
/*  f171e7c:	24840001 */ 	addiu	$a0,$a0,0x1
/*  f171e80:	1486ffeb */ 	bne	$a0,$a2,.L0f171e30
/*  f171e84:	014c5021 */ 	addu	$t2,$t2,$t4
.L0f171e88:
/*  f171e88:	24ae0003 */ 	addiu	$t6,$a1,0x3
/*  f171e8c:	31d90ffc */ 	andi	$t9,$t6,0xffc
/*  f171e90:	03260019 */ 	multu	$t9,$a2
/*  f171e94:	00001012 */ 	mflo	$v0
/*  f171e98:	0002c040 */ 	sll	$t8,$v0,0x1
/*  f171e9c:	100000b6 */ 	b	.L0f172178
/*  f171ea0:	03001025 */ 	or	$v0,$t8,$zero
/*  f171ea4:	18c00023 */ 	blez	$a2,.L0f171f34
/*  f171ea8:	00002025 */ 	or	$a0,$zero,$zero
/*  f171eac:	24ac0003 */ 	addiu	$t4,$a1,0x3
/*  f171eb0:	318f0ffc */ 	andi	$t7,$t4,0xffc
/*  f171eb4:	000f6040 */ 	sll	$t4,$t7,0x1
.L0f171eb8:
/*  f171eb8:	18a0001b */ 	blez	$a1,.L0f171f28
/*  f171ebc:	00001825 */ 	or	$v1,$zero,$zero
/*  f171ec0:	00a60019 */ 	multu	$a1,$a2
/*  f171ec4:	0208c821 */ 	addu	$t9,$s0,$t0
/*  f171ec8:	02081021 */ 	addu	$v0,$s0,$t0
/*  f171ecc:	01404825 */ 	or	$t1,$t2,$zero
/*  f171ed0:	00003812 */ 	mflo	$a3
/*  f171ed4:	0007c040 */ 	sll	$t8,$a3,0x1
/*  f171ed8:	03087821 */ 	addu	$t7,$t8,$t0
/*  f171edc:	01f05821 */ 	addu	$t3,$t7,$s0
/*  f171ee0:	03278821 */ 	addu	$s1,$t9,$a3
.L0f171ee4:
/*  f171ee4:	90580000 */ 	lbu	$t8,0x0($v0)
/*  f171ee8:	916e0000 */ 	lbu	$t6,0x0($t3)
/*  f171eec:	24630001 */ 	addiu	$v1,$v1,0x1
/*  f171ef0:	00187ac0 */ 	sll	$t7,$t8,0xb
/*  f171ef4:	92380000 */ 	lbu	$t8,0x0($s1)
/*  f171ef8:	000ec840 */ 	sll	$t9,$t6,0x1
/*  f171efc:	032f7025 */ 	or	$t6,$t9,$t7
/*  f171f00:	0018c980 */ 	sll	$t9,$t8,0x6
/*  f171f04:	01d97825 */ 	or	$t7,$t6,$t9
/*  f171f08:	35f80001 */ 	ori	$t8,$t7,0x1
/*  f171f0c:	25290002 */ 	addiu	$t1,$t1,0x2
/*  f171f10:	a538fffe */ 	sh	$t8,-0x2($t1)
/*  f171f14:	25080001 */ 	addiu	$t0,$t0,0x1
/*  f171f18:	24420001 */ 	addiu	$v0,$v0,0x1
/*  f171f1c:	26310001 */ 	addiu	$s1,$s1,0x1
/*  f171f20:	1465fff0 */ 	bne	$v1,$a1,.L0f171ee4
/*  f171f24:	256b0001 */ 	addiu	$t3,$t3,0x1
.L0f171f28:
/*  f171f28:	24840001 */ 	addiu	$a0,$a0,0x1
/*  f171f2c:	1486ffe2 */ 	bne	$a0,$a2,.L0f171eb8
/*  f171f30:	014c5021 */ 	addu	$t2,$t2,$t4
.L0f171f34:
/*  f171f34:	24ae0003 */ 	addiu	$t6,$a1,0x3
/*  f171f38:	31d90ffc */ 	andi	$t9,$t6,0xffc
/*  f171f3c:	03260019 */ 	multu	$t9,$a2
/*  f171f40:	00001012 */ 	mflo	$v0
/*  f171f44:	00027840 */ 	sll	$t7,$v0,0x1
/*  f171f48:	1000008b */ 	b	.L0f172178
/*  f171f4c:	01e01025 */ 	or	$v0,$t7,$zero
/*  f171f50:	18c0001a */ 	blez	$a2,.L0f171fbc
/*  f171f54:	00002025 */ 	or	$a0,$zero,$zero
/*  f171f58:	24a70007 */ 	addiu	$a3,$a1,0x7
/*  f171f5c:	30f80ff8 */ 	andi	$t8,$a3,0xff8
/*  f171f60:	03003825 */ 	or	$a3,$t8,$zero
.L0f171f64:
/*  f171f64:	18a00012 */ 	blez	$a1,.L0f171fb0
/*  f171f68:	00001825 */ 	or	$v1,$zero,$zero
/*  f171f6c:	00a60019 */ 	multu	$a1,$a2
/*  f171f70:	02081021 */ 	addu	$v0,$s0,$t0
/*  f171f74:	01604825 */ 	or	$t1,$t3,$zero
/*  f171f78:	00007012 */ 	mflo	$t6
/*  f171f7c:	010ec821 */ 	addu	$t9,$t0,$t6
/*  f171f80:	03306821 */ 	addu	$t5,$t9,$s0
.L0f171f84:
/*  f171f84:	90580000 */ 	lbu	$t8,0x0($v0)
/*  f171f88:	91af0000 */ 	lbu	$t7,0x0($t5)
/*  f171f8c:	24630001 */ 	addiu	$v1,$v1,0x1
/*  f171f90:	00187100 */ 	sll	$t6,$t8,0x4
/*  f171f94:	01eec825 */ 	or	$t9,$t7,$t6
/*  f171f98:	25290001 */ 	addiu	$t1,$t1,0x1
/*  f171f9c:	a139ffff */ 	sb	$t9,-0x1($t1)
/*  f171fa0:	25080001 */ 	addiu	$t0,$t0,0x1
/*  f171fa4:	24420001 */ 	addiu	$v0,$v0,0x1
/*  f171fa8:	1465fff6 */ 	bne	$v1,$a1,.L0f171f84
/*  f171fac:	25ad0001 */ 	addiu	$t5,$t5,0x1
.L0f171fb0:
/*  f171fb0:	24840001 */ 	addiu	$a0,$a0,0x1
/*  f171fb4:	1486ffeb */ 	bne	$a0,$a2,.L0f171f64
/*  f171fb8:	01675821 */ 	addu	$t3,$t3,$a3
.L0f171fbc:
/*  f171fbc:	24b80007 */ 	addiu	$t8,$a1,0x7
/*  f171fc0:	330f0ff8 */ 	andi	$t7,$t8,0xff8
/*  f171fc4:	01e60019 */ 	multu	$t7,$a2
/*  f171fc8:	00001012 */ 	mflo	$v0
/*  f171fcc:	1000006b */ 	b	.L0f17217c
/*  f171fd0:	8fb00004 */ 	lw	$s0,0x4($sp)
/*  f171fd4:	18c00012 */ 	blez	$a2,.L0f172020
/*  f171fd8:	00002025 */ 	or	$a0,$zero,$zero
/*  f171fdc:	24a70007 */ 	addiu	$a3,$a1,0x7
/*  f171fe0:	30ee0ff8 */ 	andi	$t6,$a3,0xff8
/*  f171fe4:	01c03825 */ 	or	$a3,$t6,$zero
.L0f171fe8:
/*  f171fe8:	18a0000a */ 	blez	$a1,.L0f172014
/*  f171fec:	00001825 */ 	or	$v1,$zero,$zero
/*  f171ff0:	02081021 */ 	addu	$v0,$s0,$t0
/*  f171ff4:	01604825 */ 	or	$t1,$t3,$zero
.L0f171ff8:
/*  f171ff8:	90590000 */ 	lbu	$t9,0x0($v0)
/*  f171ffc:	24630001 */ 	addiu	$v1,$v1,0x1
/*  f172000:	25290001 */ 	addiu	$t1,$t1,0x1
/*  f172004:	25080001 */ 	addiu	$t0,$t0,0x1
/*  f172008:	24420001 */ 	addiu	$v0,$v0,0x1
/*  f17200c:	1465fffa */ 	bne	$v1,$a1,.L0f171ff8
/*  f172010:	a139ffff */ 	sb	$t9,-0x1($t1)
.L0f172014:
/*  f172014:	24840001 */ 	addiu	$a0,$a0,0x1
/*  f172018:	1486fff3 */ 	bne	$a0,$a2,.L0f171fe8
/*  f17201c:	01675821 */ 	addu	$t3,$t3,$a3
.L0f172020:
/*  f172020:	24b80007 */ 	addiu	$t8,$a1,0x7
/*  f172024:	330f0ff8 */ 	andi	$t7,$t8,0xff8
/*  f172028:	01e60019 */ 	multu	$t7,$a2
/*  f17202c:	00001012 */ 	mflo	$v0
/*  f172030:	10000052 */ 	b	.L0f17217c
/*  f172034:	8fb00004 */ 	lw	$s0,0x4($sp)
/*  f172038:	18c00026 */ 	blez	$a2,.L0f1720d4
/*  f17203c:	00002025 */ 	or	$a0,$zero,$zero
/*  f172040:	24a7000f */ 	addiu	$a3,$a1,0xf
/*  f172044:	30ee0ff0 */ 	andi	$t6,$a3,0xff0
/*  f172048:	01c03825 */ 	or	$a3,$t6,$zero
/*  f17204c:	30a90001 */ 	andi	$t1,$a1,0x1
.L0f172050:
/*  f172050:	18a0001b */ 	blez	$a1,.L0f1720c0
/*  f172054:	00001825 */ 	or	$v1,$zero,$zero
/*  f172058:	00a60019 */ 	multu	$a1,$a2
/*  f17205c:	02081021 */ 	addu	$v0,$s0,$t0
/*  f172060:	0000c812 */ 	mflo	$t9
/*  f172064:	0019c080 */ 	sll	$t8,$t9,0x2
/*  f172068:	0319c023 */ 	subu	$t8,$t8,$t9
/*  f17206c:	03087821 */ 	addu	$t7,$t8,$t0
/*  f172070:	01f06821 */ 	addu	$t5,$t7,$s0
.L0f172074:
/*  f172074:	90590000 */ 	lbu	$t9,0x0($v0)
/*  f172078:	91ae0001 */ 	lbu	$t6,0x1($t5)
/*  f17207c:	25080002 */ 	addiu	$t0,$t0,0x2
/*  f172080:	0019c140 */ 	sll	$t8,$t9,0x5
/*  f172084:	91b90000 */ 	lbu	$t9,0x0($t5)
/*  f172088:	01d87825 */ 	or	$t7,$t6,$t8
/*  f17208c:	24420002 */ 	addiu	$v0,$v0,0x2
/*  f172090:	00197100 */ 	sll	$t6,$t9,0x4
/*  f172094:	9059ffff */ 	lbu	$t9,-0x1($v0)
/*  f172098:	01eec025 */ 	or	$t8,$t7,$t6
/*  f17209c:	25ad0002 */ 	addiu	$t5,$t5,0x2
/*  f1720a0:	00197840 */ 	sll	$t7,$t9,0x1
/*  f1720a4:	0003c843 */ 	sra	$t9,$v1,0x1
/*  f1720a8:	24630002 */ 	addiu	$v1,$v1,0x2
/*  f1720ac:	030f7025 */ 	or	$t6,$t8,$t7
/*  f1720b0:	0065082a */ 	slt	$at,$v1,$a1
/*  f1720b4:	0179c021 */ 	addu	$t8,$t3,$t9
/*  f1720b8:	1420ffee */ 	bnez	$at,.L0f172074
/*  f1720bc:	a30e0000 */ 	sb	$t6,0x0($t8)
.L0f1720c0:
/*  f1720c0:	11200002 */ 	beqz	$t1,.L0f1720cc
/*  f1720c4:	24840001 */ 	addiu	$a0,$a0,0x1
/*  f1720c8:	2508ffff */ 	addiu	$t0,$t0,-1
.L0f1720cc:
/*  f1720cc:	1486ffe0 */ 	bne	$a0,$a2,.L0f172050
/*  f1720d0:	01675821 */ 	addu	$t3,$t3,$a3
.L0f1720d4:
/*  f1720d4:	24af000f */ 	addiu	$t7,$a1,0xf
/*  f1720d8:	31f90ff0 */ 	andi	$t9,$t7,0xff0
/*  f1720dc:	00197043 */ 	sra	$t6,$t9,0x1
/*  f1720e0:	01c60019 */ 	multu	$t6,$a2
/*  f1720e4:	00001012 */ 	mflo	$v0
/*  f1720e8:	10000024 */ 	b	.L0f17217c
/*  f1720ec:	8fb00004 */ 	lw	$s0,0x4($sp)
/*  f1720f0:	18c00019 */ 	blez	$a2,.L0f172158
/*  f1720f4:	00002025 */ 	or	$a0,$zero,$zero
/*  f1720f8:	24a7000f */ 	addiu	$a3,$a1,0xf
/*  f1720fc:	30f80ff0 */ 	andi	$t8,$a3,0xff0
/*  f172100:	00183843 */ 	sra	$a3,$t8,0x1
/*  f172104:	30a90001 */ 	andi	$t1,$a1,0x1
.L0f172108:
/*  f172108:	18a0000e */ 	blez	$a1,.L0f172144
/*  f17210c:	00001825 */ 	or	$v1,$zero,$zero
/*  f172110:	02081021 */ 	addu	$v0,$s0,$t0
.L0f172114:
/*  f172114:	904e0000 */ 	lbu	$t6,0x0($v0)
/*  f172118:	90590001 */ 	lbu	$t9,0x1($v0)
/*  f17211c:	25080002 */ 	addiu	$t0,$t0,0x2
/*  f172120:	000ec100 */ 	sll	$t8,$t6,0x4
/*  f172124:	00037043 */ 	sra	$t6,$v1,0x1
/*  f172128:	24630002 */ 	addiu	$v1,$v1,0x2
/*  f17212c:	03387825 */ 	or	$t7,$t9,$t8
/*  f172130:	0065082a */ 	slt	$at,$v1,$a1
/*  f172134:	016ec821 */ 	addu	$t9,$t3,$t6
/*  f172138:	a32f0000 */ 	sb	$t7,0x0($t9)
/*  f17213c:	1420fff5 */ 	bnez	$at,.L0f172114
/*  f172140:	24420002 */ 	addiu	$v0,$v0,0x2
.L0f172144:
/*  f172144:	11200002 */ 	beqz	$t1,.L0f172150
/*  f172148:	24840001 */ 	addiu	$a0,$a0,0x1
/*  f17214c:	2508ffff */ 	addiu	$t0,$t0,-1
.L0f172150:
/*  f172150:	1486ffed */ 	bne	$a0,$a2,.L0f172108
/*  f172154:	01675821 */ 	addu	$t3,$t3,$a3
.L0f172158:
/*  f172158:	24b8000f */ 	addiu	$t8,$a1,0xf
/*  f17215c:	330e0ff0 */ 	andi	$t6,$t8,0xff0
/*  f172160:	000e7843 */ 	sra	$t7,$t6,0x1
/*  f172164:	01e60019 */ 	multu	$t7,$a2
/*  f172168:	00001012 */ 	mflo	$v0
/*  f17216c:	10000003 */ 	b	.L0f17217c
/*  f172170:	8fb00004 */ 	lw	$s0,0x4($sp)
.L0f172174:
/*  f172174:	00001025 */ 	or	$v0,$zero,$zero
.L0f172178:
/*  f172178:	8fb00004 */ 	lw	$s0,0x4($sp)
.L0f17217c:
/*  f17217c:	8fb10008 */ 	lw	$s1,0x8($sp)
/*  f172180:	8fb2000c */ 	lw	$s2,0xc($sp)
/*  f172184:	03e00008 */ 	jr	$ra
/*  f172188:	27bd0010 */ 	addiu	$sp,$sp,0x10
);

GLOBAL_ASM(
glabel func0f17218c
.late_rodata
glabel var7f1b7b08
.word func0f17218c+0x78 # f172204
glabel var7f1b7b0c
.word func0f17218c+0x16c # f1722f8
glabel var7f1b7b10
.word func0f17218c+0xec # f172278
glabel var7f1b7b14
.word func0f17218c+0x1e8 # f172374
glabel var7f1b7b18
.word func0f17218c+0x16c # f1722f8
glabel var7f1b7b1c
.word func0f17218c+0x26c # f1723f8
glabel var7f1b7b20
.word func0f17218c+0x2e4 # f172470
glabel var7f1b7b24
.word func0f17218c+0x26c # f1723f8
glabel var7f1b7b28
.word func0f17218c+0x2e4 # f172470
.text
/*  f17218c:	27bdffc0 */ 	addiu	$sp,$sp,-64
/*  f172190:	afb40028 */ 	sw	$s4,0x28($sp)
/*  f172194:	0080a025 */ 	or	$s4,$a0,$zero
/*  f172198:	afbf003c */ 	sw	$ra,0x3c($sp)
/*  f17219c:	afbe0038 */ 	sw	$s8,0x38($sp)
/*  f1721a0:	afb70034 */ 	sw	$s7,0x34($sp)
/*  f1721a4:	afb5002c */ 	sw	$s5,0x2c($sp)
/*  f1721a8:	afb30024 */ 	sw	$s3,0x24($sp)
/*  f1721ac:	00e09825 */ 	or	$s3,$a3,$zero
/*  f1721b0:	afb60030 */ 	sw	$s6,0x30($sp)
/*  f1721b4:	afb20020 */ 	sw	$s2,0x20($sp)
/*  f1721b8:	afb1001c */ 	sw	$s1,0x1c($sp)
/*  f1721bc:	afb00018 */ 	sw	$s0,0x18($sp)
/*  f1721c0:	afa50044 */ 	sw	$a1,0x44($sp)
/*  f1721c4:	00c0b825 */ 	or	$s7,$a2,$zero
/*  f1721c8:	00c0f025 */ 	or	$s8,$a2,$zero
/*  f1721cc:	00c0a825 */ 	or	$s5,$a2,$zero
/*  f1721d0:	0fc5c60a */ 	jal	func0f171828
/*  f1721d4:	8fa40050 */ 	lw	$a0,0x50($sp)
/*  f1721d8:	8fae0054 */ 	lw	$t6,0x54($sp)
/*  f1721dc:	00409025 */ 	or	$s2,$v0,$zero
/*  f1721e0:	00001025 */ 	or	$v0,$zero,$zero
/*  f1721e4:	2dc10009 */ 	sltiu	$at,$t6,0x9
/*  f1721e8:	102000ce */ 	beqz	$at,.L0f172524
/*  f1721ec:	000e7080 */ 	sll	$t6,$t6,0x2
/*  f1721f0:	3c017f1b */ 	lui	$at,%hi(var7f1b7b08)
/*  f1721f4:	002e0821 */ 	addu	$at,$at,$t6
/*  f1721f8:	8c2e7b08 */ 	lw	$t6,%lo(var7f1b7b08)($at)
/*  f1721fc:	01c00008 */ 	jr	$t6
/*  f172200:	00000000 */ 	nop
/*  f172204:	8fa30044 */ 	lw	$v1,0x44($sp)
/*  f172208:	26950003 */ 	addiu	$s5,$s4,0x3
/*  f17220c:	32af0ffc */ 	andi	$t7,$s5,0xffc
/*  f172210:	18600012 */ 	blez	$v1,.L0f17225c
/*  f172214:	0000b025 */ 	or	$s6,$zero,$zero
/*  f172218:	000fa880 */ 	sll	$s5,$t7,0x2
.L0f17221c:
/*  f17221c:	1a80000c */ 	blez	$s4,.L0f172250
/*  f172220:	00008025 */ 	or	$s0,$zero,$zero
/*  f172224:	02e08825 */ 	or	$s1,$s7,$zero
.L0f172228:
/*  f172228:	0fc5cd4f */ 	jal	texReadBits
/*  f17222c:	02402025 */ 	or	$a0,$s2,$zero
/*  f172230:	0002c880 */ 	sll	$t9,$v0,0x2
/*  f172234:	02794021 */ 	addu	$t0,$s3,$t9
/*  f172238:	8d090000 */ 	lw	$t1,0x0($t0)
/*  f17223c:	26100001 */ 	addiu	$s0,$s0,0x1
/*  f172240:	26310004 */ 	addiu	$s1,$s1,0x4
/*  f172244:	1614fff8 */ 	bne	$s0,$s4,.L0f172228
/*  f172248:	ae29fffc */ 	sw	$t1,-0x4($s1)
/*  f17224c:	8fa30044 */ 	lw	$v1,0x44($sp)
.L0f172250:
/*  f172250:	26d60001 */ 	addiu	$s6,$s6,0x1
/*  f172254:	16c3fff1 */ 	bne	$s6,$v1,.L0f17221c
/*  f172258:	02f5b821 */ 	addu	$s7,$s7,$s5
.L0f17225c:
/*  f17225c:	268a0003 */ 	addiu	$t2,$s4,0x3
/*  f172260:	314b0ffc */ 	andi	$t3,$t2,0xffc
/*  f172264:	01630019 */ 	multu	$t3,$v1
/*  f172268:	00001012 */ 	mflo	$v0
/*  f17226c:	00026080 */ 	sll	$t4,$v0,0x2
/*  f172270:	100000ac */ 	b	.L0f172524
/*  f172274:	01801025 */ 	or	$v0,$t4,$zero
/*  f172278:	8fad0044 */ 	lw	$t5,0x44($sp)
/*  f17227c:	26950003 */ 	addiu	$s5,$s4,0x3
/*  f172280:	32ae0ffc */ 	andi	$t6,$s5,0xffc
/*  f172284:	19a00014 */ 	blez	$t5,.L0f1722d8
/*  f172288:	0000b025 */ 	or	$s6,$zero,$zero
/*  f17228c:	000ea880 */ 	sll	$s5,$t6,0x2
.L0f172290:
/*  f172290:	1a80000c */ 	blez	$s4,.L0f1722c4
/*  f172294:	00008025 */ 	or	$s0,$zero,$zero
/*  f172298:	02e08825 */ 	or	$s1,$s7,$zero
.L0f17229c:
/*  f17229c:	0fc5cd4f */ 	jal	texReadBits
/*  f1722a0:	02402025 */ 	or	$a0,$s2,$zero
/*  f1722a4:	0002c080 */ 	sll	$t8,$v0,0x2
/*  f1722a8:	0278c821 */ 	addu	$t9,$s3,$t8
/*  f1722ac:	8f280000 */ 	lw	$t0,0x0($t9)
/*  f1722b0:	26100001 */ 	addiu	$s0,$s0,0x1
/*  f1722b4:	26310004 */ 	addiu	$s1,$s1,0x4
/*  f1722b8:	00084a00 */ 	sll	$t1,$t0,0x8
/*  f1722bc:	1614fff7 */ 	bne	$s0,$s4,.L0f17229c
/*  f1722c0:	ae29fffc */ 	sw	$t1,-0x4($s1)
.L0f1722c4:
/*  f1722c4:	8faa0044 */ 	lw	$t2,0x44($sp)
/*  f1722c8:	26d60001 */ 	addiu	$s6,$s6,0x1
/*  f1722cc:	02f5b821 */ 	addu	$s7,$s7,$s5
/*  f1722d0:	16caffef */ 	bne	$s6,$t2,.L0f172290
/*  f1722d4:	00000000 */ 	nop
.L0f1722d8:
/*  f1722d8:	8fad0044 */ 	lw	$t5,0x44($sp)
/*  f1722dc:	268b0003 */ 	addiu	$t3,$s4,0x3
/*  f1722e0:	316c0ffc */ 	andi	$t4,$t3,0xffc
/*  f1722e4:	018d0019 */ 	multu	$t4,$t5
/*  f1722e8:	00001012 */ 	mflo	$v0
/*  f1722ec:	00027080 */ 	sll	$t6,$v0,0x2
/*  f1722f0:	1000008c */ 	b	.L0f172524
/*  f1722f4:	01c01025 */ 	or	$v0,$t6,$zero
/*  f1722f8:	8faf0044 */ 	lw	$t7,0x44($sp)
/*  f1722fc:	26950003 */ 	addiu	$s5,$s4,0x3
/*  f172300:	32b80ffc */ 	andi	$t8,$s5,0xffc
/*  f172304:	19e00013 */ 	blez	$t7,.L0f172354
/*  f172308:	0000b025 */ 	or	$s6,$zero,$zero
/*  f17230c:	0018a840 */ 	sll	$s5,$t8,0x1
.L0f172310:
/*  f172310:	1a80000b */ 	blez	$s4,.L0f172340
/*  f172314:	00008025 */ 	or	$s0,$zero,$zero
/*  f172318:	03c08825 */ 	or	$s1,$s8,$zero
.L0f17231c:
/*  f17231c:	0fc5cd4f */ 	jal	texReadBits
/*  f172320:	02402025 */ 	or	$a0,$s2,$zero
/*  f172324:	00024040 */ 	sll	$t0,$v0,0x1
/*  f172328:	02684821 */ 	addu	$t1,$s3,$t0
/*  f17232c:	952a0000 */ 	lhu	$t2,0x0($t1)
/*  f172330:	26100001 */ 	addiu	$s0,$s0,0x1
/*  f172334:	26310002 */ 	addiu	$s1,$s1,0x2
/*  f172338:	1614fff8 */ 	bne	$s0,$s4,.L0f17231c
/*  f17233c:	a62afffe */ 	sh	$t2,-0x2($s1)
.L0f172340:
/*  f172340:	8fab0044 */ 	lw	$t3,0x44($sp)
/*  f172344:	26d60001 */ 	addiu	$s6,$s6,0x1
/*  f172348:	03d5f021 */ 	addu	$s8,$s8,$s5
/*  f17234c:	16cbfff0 */ 	bne	$s6,$t3,.L0f172310
/*  f172350:	00000000 */ 	nop
.L0f172354:
/*  f172354:	8fae0044 */ 	lw	$t6,0x44($sp)
/*  f172358:	268c0003 */ 	addiu	$t4,$s4,0x3
/*  f17235c:	318d0ffc */ 	andi	$t5,$t4,0xffc
/*  f172360:	01ae0019 */ 	multu	$t5,$t6
/*  f172364:	00001012 */ 	mflo	$v0
/*  f172368:	00027840 */ 	sll	$t7,$v0,0x1
/*  f17236c:	1000006d */ 	b	.L0f172524
/*  f172370:	01e01025 */ 	or	$v0,$t7,$zero
/*  f172374:	8fb80044 */ 	lw	$t8,0x44($sp)
/*  f172378:	26950003 */ 	addiu	$s5,$s4,0x3
/*  f17237c:	32b90ffc */ 	andi	$t9,$s5,0xffc
/*  f172380:	1b000015 */ 	blez	$t8,.L0f1723d8
/*  f172384:	0000b025 */ 	or	$s6,$zero,$zero
/*  f172388:	0019a840 */ 	sll	$s5,$t9,0x1
.L0f17238c:
/*  f17238c:	1a80000d */ 	blez	$s4,.L0f1723c4
/*  f172390:	00008025 */ 	or	$s0,$zero,$zero
/*  f172394:	03c08825 */ 	or	$s1,$s8,$zero
.L0f172398:
/*  f172398:	0fc5cd4f */ 	jal	texReadBits
/*  f17239c:	02402025 */ 	or	$a0,$s2,$zero
/*  f1723a0:	00024840 */ 	sll	$t1,$v0,0x1
/*  f1723a4:	02695021 */ 	addu	$t2,$s3,$t1
/*  f1723a8:	954b0000 */ 	lhu	$t3,0x0($t2)
/*  f1723ac:	26100001 */ 	addiu	$s0,$s0,0x1
/*  f1723b0:	26310002 */ 	addiu	$s1,$s1,0x2
/*  f1723b4:	000b6040 */ 	sll	$t4,$t3,0x1
/*  f1723b8:	358d0001 */ 	ori	$t5,$t4,0x1
/*  f1723bc:	1614fff6 */ 	bne	$s0,$s4,.L0f172398
/*  f1723c0:	a62dfffe */ 	sh	$t5,-0x2($s1)
.L0f1723c4:
/*  f1723c4:	8fae0044 */ 	lw	$t6,0x44($sp)
/*  f1723c8:	26d60001 */ 	addiu	$s6,$s6,0x1
/*  f1723cc:	03d5f021 */ 	addu	$s8,$s8,$s5
/*  f1723d0:	16ceffee */ 	bne	$s6,$t6,.L0f17238c
/*  f1723d4:	00000000 */ 	nop
.L0f1723d8:
/*  f1723d8:	8fb90044 */ 	lw	$t9,0x44($sp)
/*  f1723dc:	268f0003 */ 	addiu	$t7,$s4,0x3
/*  f1723e0:	31f80ffc */ 	andi	$t8,$t7,0xffc
/*  f1723e4:	03190019 */ 	multu	$t8,$t9
/*  f1723e8:	00001012 */ 	mflo	$v0
/*  f1723ec:	00024040 */ 	sll	$t0,$v0,0x1
/*  f1723f0:	1000004c */ 	b	.L0f172524
/*  f1723f4:	01001025 */ 	or	$v0,$t0,$zero
/*  f1723f8:	8fa90044 */ 	lw	$t1,0x44($sp)
/*  f1723fc:	26970007 */ 	addiu	$s7,$s4,0x7
/*  f172400:	32ea0ff8 */ 	andi	$t2,$s7,0xff8
/*  f172404:	19200013 */ 	blez	$t1,.L0f172454
/*  f172408:	0000b025 */ 	or	$s6,$zero,$zero
/*  f17240c:	0140b825 */ 	or	$s7,$t2,$zero
.L0f172410:
/*  f172410:	1a80000b */ 	blez	$s4,.L0f172440
/*  f172414:	00008025 */ 	or	$s0,$zero,$zero
/*  f172418:	02a08825 */ 	or	$s1,$s5,$zero
.L0f17241c:
/*  f17241c:	0fc5cd4f */ 	jal	texReadBits
/*  f172420:	02402025 */ 	or	$a0,$s2,$zero
/*  f172424:	00025840 */ 	sll	$t3,$v0,0x1
/*  f172428:	026b6021 */ 	addu	$t4,$s3,$t3
/*  f17242c:	958d0000 */ 	lhu	$t5,0x0($t4)
/*  f172430:	26100001 */ 	addiu	$s0,$s0,0x1
/*  f172434:	26310001 */ 	addiu	$s1,$s1,0x1
/*  f172438:	1614fff8 */ 	bne	$s0,$s4,.L0f17241c
/*  f17243c:	a22dffff */ 	sb	$t5,-0x1($s1)
.L0f172440:
/*  f172440:	8fae0044 */ 	lw	$t6,0x44($sp)
/*  f172444:	26d60001 */ 	addiu	$s6,$s6,0x1
/*  f172448:	02b7a821 */ 	addu	$s5,$s5,$s7
/*  f17244c:	16cefff0 */ 	bne	$s6,$t6,.L0f172410
/*  f172450:	00000000 */ 	nop
.L0f172454:
/*  f172454:	8fb90044 */ 	lw	$t9,0x44($sp)
/*  f172458:	268f0007 */ 	addiu	$t7,$s4,0x7
/*  f17245c:	31f80ff8 */ 	andi	$t8,$t7,0xff8
/*  f172460:	03190019 */ 	multu	$t8,$t9
/*  f172464:	00001012 */ 	mflo	$v0
/*  f172468:	1000002f */ 	b	.L0f172528
/*  f17246c:	8fbf003c */ 	lw	$ra,0x3c($sp)
/*  f172470:	8fa80044 */ 	lw	$t0,0x44($sp)
/*  f172474:	2697000f */ 	addiu	$s7,$s4,0xf
/*  f172478:	32e90ff0 */ 	andi	$t1,$s7,0xff0
/*  f17247c:	19000021 */ 	blez	$t0,.L0f172504
/*  f172480:	0000b025 */ 	or	$s6,$zero,$zero
/*  f172484:	0009b843 */ 	sra	$s7,$t1,0x1
.L0f172488:
/*  f172488:	1a800019 */ 	blez	$s4,.L0f1724f0
/*  f17248c:	00008025 */ 	or	$s0,$zero,$zero
.L0f172490:
/*  f172490:	0fc5cd4f */ 	jal	texReadBits
/*  f172494:	02402025 */ 	or	$a0,$s2,$zero
/*  f172498:	00026040 */ 	sll	$t4,$v0,0x1
/*  f17249c:	026c6821 */ 	addu	$t5,$s3,$t4
/*  f1724a0:	95ae0000 */ 	lhu	$t6,0x0($t5)
/*  f1724a4:	00105843 */ 	sra	$t3,$s0,0x1
/*  f1724a8:	26180001 */ 	addiu	$t8,$s0,0x1
/*  f1724ac:	02ab8821 */ 	addu	$s1,$s5,$t3
/*  f1724b0:	0314082a */ 	slt	$at,$t8,$s4
/*  f1724b4:	000e7900 */ 	sll	$t7,$t6,0x4
/*  f1724b8:	10200009 */ 	beqz	$at,.L0f1724e0
/*  f1724bc:	a22f0000 */ 	sb	$t7,0x0($s1)
/*  f1724c0:	0fc5cd4f */ 	jal	texReadBits
/*  f1724c4:	02402025 */ 	or	$a0,$s2,$zero
/*  f1724c8:	00024040 */ 	sll	$t0,$v0,0x1
/*  f1724cc:	02684821 */ 	addu	$t1,$s3,$t0
/*  f1724d0:	912a0001 */ 	lbu	$t2,0x1($t1)
/*  f1724d4:	92390000 */ 	lbu	$t9,0x0($s1)
/*  f1724d8:	032a5825 */ 	or	$t3,$t9,$t2
/*  f1724dc:	a22b0000 */ 	sb	$t3,0x0($s1)
.L0f1724e0:
/*  f1724e0:	26100002 */ 	addiu	$s0,$s0,0x2
/*  f1724e4:	0214082a */ 	slt	$at,$s0,$s4
/*  f1724e8:	1420ffe9 */ 	bnez	$at,.L0f172490
/*  f1724ec:	00000000 */ 	nop
.L0f1724f0:
/*  f1724f0:	8fac0044 */ 	lw	$t4,0x44($sp)
/*  f1724f4:	26d60001 */ 	addiu	$s6,$s6,0x1
/*  f1724f8:	02b7a821 */ 	addu	$s5,$s5,$s7
/*  f1724fc:	16ccffe2 */ 	bne	$s6,$t4,.L0f172488
/*  f172500:	00000000 */ 	nop
.L0f172504:
/*  f172504:	8fb80044 */ 	lw	$t8,0x44($sp)
/*  f172508:	268d000f */ 	addiu	$t5,$s4,0xf
/*  f17250c:	31ae0ff0 */ 	andi	$t6,$t5,0xff0
/*  f172510:	000e7843 */ 	sra	$t7,$t6,0x1
/*  f172514:	01f80019 */ 	multu	$t7,$t8
/*  f172518:	00001012 */ 	mflo	$v0
/*  f17251c:	10000002 */ 	b	.L0f172528
/*  f172520:	8fbf003c */ 	lw	$ra,0x3c($sp)
.L0f172524:
/*  f172524:	8fbf003c */ 	lw	$ra,0x3c($sp)
.L0f172528:
/*  f172528:	8fb00018 */ 	lw	$s0,0x18($sp)
/*  f17252c:	8fb1001c */ 	lw	$s1,0x1c($sp)
/*  f172530:	8fb20020 */ 	lw	$s2,0x20($sp)
/*  f172534:	8fb30024 */ 	lw	$s3,0x24($sp)
/*  f172538:	8fb40028 */ 	lw	$s4,0x28($sp)
/*  f17253c:	8fb5002c */ 	lw	$s5,0x2c($sp)
/*  f172540:	8fb60030 */ 	lw	$s6,0x30($sp)
/*  f172544:	8fb70034 */ 	lw	$s7,0x34($sp)
/*  f172548:	8fbe0038 */ 	lw	$s8,0x38($sp)
/*  f17254c:	03e00008 */ 	jr	$ra
/*  f172550:	27bd0040 */ 	addiu	$sp,$sp,0x40
);

GLOBAL_ASM(
glabel func0f172554
.late_rodata
glabel var7f1b7b2c
.word func0f172554+0x58 # f1725ac
glabel var7f1b7b30
.word func0f172554+0x1b0 # f172704
glabel var7f1b7b34
.word func0f172554+0xfc # f172650
glabel var7f1b7b38
.word func0f172554+0x258 # f1727ac
glabel var7f1b7b3c
.word func0f172554+0x1b0 # f172704
glabel var7f1b7b40
.word func0f172554+0x310 # f172864
glabel var7f1b7b44
.word func0f172554+0x3b0 # f172904
glabel var7f1b7b48
.word func0f172554+0x310 # f172864
glabel var7f1b7b4c
.word func0f172554+0x3b0 # f172904
.text
/*  f172554:	27bdffe0 */ 	addiu	$sp,$sp,-32
/*  f172558:	afb00004 */ 	sw	$s0,0x4($sp)
/*  f17255c:	8fb00034 */ 	lw	$s0,0x34($sp)
/*  f172560:	00e04025 */ 	or	$t0,$a3,$zero
/*  f172564:	00e04825 */ 	or	$t1,$a3,$zero
/*  f172568:	2a010101 */ 	slti	$at,$s0,0x101
/*  f17256c:	10200004 */ 	beqz	$at,.L0f172580
/*  f172570:	00e05025 */ 	or	$t2,$a3,$zero
/*  f172574:	00801025 */ 	or	$v0,$a0,$zero
/*  f172578:	10000003 */ 	b	.L0f172588
/*  f17257c:	8fa30008 */ 	lw	$v1,0x8($sp)
.L0f172580:
/*  f172580:	00801825 */ 	or	$v1,$a0,$zero
/*  f172584:	8fa2000c */ 	lw	$v0,0xc($sp)
.L0f172588:
/*  f172588:	8fae0038 */ 	lw	$t6,0x38($sp)
/*  f17258c:	2dc10009 */ 	sltiu	$at,$t6,0x9
/*  f172590:	10200115 */ 	beqz	$at,.L0f1729e8
/*  f172594:	000e7080 */ 	sll	$t6,$t6,0x2
/*  f172598:	3c017f1b */ 	lui	$at,%hi(var7f1b7b2c)
/*  f17259c:	002e0821 */ 	addu	$at,$at,$t6
/*  f1725a0:	8c2e7b2c */ 	lw	$t6,%lo(var7f1b7b2c)($at)
/*  f1725a4:	01c00008 */ 	jr	$t6
/*  f1725a8:	00000000 */ 	nop
/*  f1725ac:	18c00021 */ 	blez	$a2,.L0f172634
/*  f1725b0:	00003825 */ 	or	$a3,$zero,$zero
/*  f1725b4:	24aa0003 */ 	addiu	$t2,$a1,0x3
/*  f1725b8:	314f0ffc */ 	andi	$t7,$t2,0xffc
/*  f1725bc:	000f5080 */ 	sll	$t2,$t7,0x2
/*  f1725c0:	00056040 */ 	sll	$t4,$a1,0x1
/*  f1725c4:	8fa40030 */ 	lw	$a0,0x30($sp)
.L0f1725c8:
/*  f1725c8:	18a00015 */ 	blez	$a1,.L0f172620
/*  f1725cc:	00005825 */ 	or	$t3,$zero,$zero
/*  f1725d0:	01004825 */ 	or	$t1,$t0,$zero
.L0f1725d4:
/*  f1725d4:	2a010101 */ 	slti	$at,$s0,0x101
/*  f1725d8:	10200008 */ 	beqz	$at,.L0f1725fc
/*  f1725dc:	000b7040 */ 	sll	$t6,$t3,0x1
/*  f1725e0:	004bc821 */ 	addu	$t9,$v0,$t3
/*  f1725e4:	932e0000 */ 	lbu	$t6,0x0($t9)
/*  f1725e8:	000e7880 */ 	sll	$t7,$t6,0x2
/*  f1725ec:	008fc021 */ 	addu	$t8,$a0,$t7
/*  f1725f0:	8f190000 */ 	lw	$t9,0x0($t8)
/*  f1725f4:	10000007 */ 	b	.L0f172614
/*  f1725f8:	ad390000 */ 	sw	$t9,0x0($t1)
.L0f1725fc:
/*  f1725fc:	006e7821 */ 	addu	$t7,$v1,$t6
/*  f172600:	95f80000 */ 	lhu	$t8,0x0($t7)
/*  f172604:	0018c880 */ 	sll	$t9,$t8,0x2
/*  f172608:	00997021 */ 	addu	$t6,$a0,$t9
/*  f17260c:	8dcf0000 */ 	lw	$t7,0x0($t6)
/*  f172610:	ad2f0000 */ 	sw	$t7,0x0($t1)
.L0f172614:
/*  f172614:	256b0001 */ 	addiu	$t3,$t3,0x1
/*  f172618:	1565ffee */ 	bne	$t3,$a1,.L0f1725d4
/*  f17261c:	25290004 */ 	addiu	$t1,$t1,0x4
.L0f172620:
/*  f172620:	24e70001 */ 	addiu	$a3,$a3,0x1
/*  f172624:	010a4021 */ 	addu	$t0,$t0,$t2
/*  f172628:	00451021 */ 	addu	$v0,$v0,$a1
/*  f17262c:	14e6ffe6 */ 	bne	$a3,$a2,.L0f1725c8
/*  f172630:	006c1821 */ 	addu	$v1,$v1,$t4
.L0f172634:
/*  f172634:	24b80003 */ 	addiu	$t8,$a1,0x3
/*  f172638:	33190ffc */ 	andi	$t9,$t8,0xffc
/*  f17263c:	03260019 */ 	multu	$t9,$a2
/*  f172640:	00001012 */ 	mflo	$v0
/*  f172644:	00027080 */ 	sll	$t6,$v0,0x2
/*  f172648:	100000e8 */ 	b	.L0f1729ec
/*  f17264c:	01c01025 */ 	or	$v0,$t6,$zero
/*  f172650:	18c00025 */ 	blez	$a2,.L0f1726e8
/*  f172654:	00003825 */ 	or	$a3,$zero,$zero
/*  f172658:	24aa0003 */ 	addiu	$t2,$a1,0x3
/*  f17265c:	314f0ffc */ 	andi	$t7,$t2,0xffc
/*  f172660:	000f5080 */ 	sll	$t2,$t7,0x2
/*  f172664:	00056040 */ 	sll	$t4,$a1,0x1
/*  f172668:	8fa40030 */ 	lw	$a0,0x30($sp)
.L0f17266c:
/*  f17266c:	18a00019 */ 	blez	$a1,.L0f1726d4
/*  f172670:	00005825 */ 	or	$t3,$zero,$zero
/*  f172674:	01004825 */ 	or	$t1,$t0,$zero
.L0f172678:
/*  f172678:	2a010101 */ 	slti	$at,$s0,0x101
/*  f17267c:	1020000a */ 	beqz	$at,.L0f1726a8
/*  f172680:	000bc040 */ 	sll	$t8,$t3,0x1
/*  f172684:	004bc821 */ 	addu	$t9,$v0,$t3
/*  f172688:	932e0000 */ 	lbu	$t6,0x0($t9)
/*  f17268c:	000e7880 */ 	sll	$t7,$t6,0x2
/*  f172690:	008fc021 */ 	addu	$t8,$a0,$t7
/*  f172694:	8f190000 */ 	lw	$t9,0x0($t8)
/*  f172698:	00197200 */ 	sll	$t6,$t9,0x8
/*  f17269c:	35cf00ff */ 	ori	$t7,$t6,0xff
/*  f1726a0:	10000009 */ 	b	.L0f1726c8
/*  f1726a4:	ad2f0000 */ 	sw	$t7,0x0($t1)
.L0f1726a8:
/*  f1726a8:	0078c821 */ 	addu	$t9,$v1,$t8
/*  f1726ac:	972e0000 */ 	lhu	$t6,0x0($t9)
/*  f1726b0:	000e7880 */ 	sll	$t7,$t6,0x2
/*  f1726b4:	008fc021 */ 	addu	$t8,$a0,$t7
/*  f1726b8:	8f190000 */ 	lw	$t9,0x0($t8)
/*  f1726bc:	00197200 */ 	sll	$t6,$t9,0x8
/*  f1726c0:	35cf00ff */ 	ori	$t7,$t6,0xff
/*  f1726c4:	ad2f0000 */ 	sw	$t7,0x0($t1)
.L0f1726c8:
/*  f1726c8:	256b0001 */ 	addiu	$t3,$t3,0x1
/*  f1726cc:	1565ffea */ 	bne	$t3,$a1,.L0f172678
/*  f1726d0:	25290004 */ 	addiu	$t1,$t1,0x4
.L0f1726d4:
/*  f1726d4:	24e70001 */ 	addiu	$a3,$a3,0x1
/*  f1726d8:	010a4021 */ 	addu	$t0,$t0,$t2
/*  f1726dc:	00451021 */ 	addu	$v0,$v0,$a1
/*  f1726e0:	14e6ffe2 */ 	bne	$a3,$a2,.L0f17266c
/*  f1726e4:	006c1821 */ 	addu	$v1,$v1,$t4
.L0f1726e8:
/*  f1726e8:	24b80003 */ 	addiu	$t8,$a1,0x3
/*  f1726ec:	33190ffc */ 	andi	$t9,$t8,0xffc
/*  f1726f0:	03260019 */ 	multu	$t9,$a2
/*  f1726f4:	00001012 */ 	mflo	$v0
/*  f1726f8:	00027080 */ 	sll	$t6,$v0,0x2
/*  f1726fc:	100000bb */ 	b	.L0f1729ec
/*  f172700:	01c01025 */ 	or	$v0,$t6,$zero
/*  f172704:	18c00022 */ 	blez	$a2,.L0f172790
/*  f172708:	00003825 */ 	or	$a3,$zero,$zero
/*  f17270c:	24ad0003 */ 	addiu	$t5,$a1,0x3
/*  f172710:	31af0ffc */ 	andi	$t7,$t5,0xffc
/*  f172714:	000f6840 */ 	sll	$t5,$t7,0x1
/*  f172718:	00056040 */ 	sll	$t4,$a1,0x1
/*  f17271c:	8fa40030 */ 	lw	$a0,0x30($sp)
.L0f172720:
/*  f172720:	18a00016 */ 	blez	$a1,.L0f17277c
/*  f172724:	00005825 */ 	or	$t3,$zero,$zero
/*  f172728:	00004025 */ 	or	$t0,$zero,$zero
/*  f17272c:	01205025 */ 	or	$t2,$t1,$zero
.L0f172730:
/*  f172730:	2a010101 */ 	slti	$at,$s0,0x101
/*  f172734:	10200008 */ 	beqz	$at,.L0f172758
/*  f172738:	00687021 */ 	addu	$t6,$v1,$t0
/*  f17273c:	004bc821 */ 	addu	$t9,$v0,$t3
/*  f172740:	932e0000 */ 	lbu	$t6,0x0($t9)
/*  f172744:	000e7840 */ 	sll	$t7,$t6,0x1
/*  f172748:	008fc021 */ 	addu	$t8,$a0,$t7
/*  f17274c:	97190000 */ 	lhu	$t9,0x0($t8)
/*  f172750:	10000006 */ 	b	.L0f17276c
/*  f172754:	a5590000 */ 	sh	$t9,0x0($t2)
.L0f172758:
/*  f172758:	95cf0000 */ 	lhu	$t7,0x0($t6)
/*  f17275c:	000fc040 */ 	sll	$t8,$t7,0x1
/*  f172760:	0098c821 */ 	addu	$t9,$a0,$t8
/*  f172764:	972e0000 */ 	lhu	$t6,0x0($t9)
/*  f172768:	a54e0000 */ 	sh	$t6,0x0($t2)
.L0f17276c:
/*  f17276c:	256b0001 */ 	addiu	$t3,$t3,0x1
/*  f172770:	25080002 */ 	addiu	$t0,$t0,0x2
/*  f172774:	1565ffee */ 	bne	$t3,$a1,.L0f172730
/*  f172778:	254a0002 */ 	addiu	$t2,$t2,0x2
.L0f17277c:
/*  f17277c:	24e70001 */ 	addiu	$a3,$a3,0x1
/*  f172780:	012d4821 */ 	addu	$t1,$t1,$t5
/*  f172784:	00451021 */ 	addu	$v0,$v0,$a1
/*  f172788:	14e6ffe5 */ 	bne	$a3,$a2,.L0f172720
/*  f17278c:	006c1821 */ 	addu	$v1,$v1,$t4
.L0f172790:
/*  f172790:	24af0003 */ 	addiu	$t7,$a1,0x3
/*  f172794:	31f80ffc */ 	andi	$t8,$t7,0xffc
/*  f172798:	03060019 */ 	multu	$t8,$a2
/*  f17279c:	00001012 */ 	mflo	$v0
/*  f1727a0:	0002c840 */ 	sll	$t9,$v0,0x1
/*  f1727a4:	10000091 */ 	b	.L0f1729ec
/*  f1727a8:	03201025 */ 	or	$v0,$t9,$zero
/*  f1727ac:	18c00026 */ 	blez	$a2,.L0f172848
/*  f1727b0:	00003825 */ 	or	$a3,$zero,$zero
/*  f1727b4:	24ad0003 */ 	addiu	$t5,$a1,0x3
/*  f1727b8:	31ae0ffc */ 	andi	$t6,$t5,0xffc
/*  f1727bc:	000e6840 */ 	sll	$t5,$t6,0x1
/*  f1727c0:	00056040 */ 	sll	$t4,$a1,0x1
/*  f1727c4:	8fa40030 */ 	lw	$a0,0x30($sp)
.L0f1727c8:
/*  f1727c8:	18a0001a */ 	blez	$a1,.L0f172834
/*  f1727cc:	00005825 */ 	or	$t3,$zero,$zero
/*  f1727d0:	00004025 */ 	or	$t0,$zero,$zero
/*  f1727d4:	01205025 */ 	or	$t2,$t1,$zero
.L0f1727d8:
/*  f1727d8:	2a010101 */ 	slti	$at,$s0,0x101
/*  f1727dc:	1020000a */ 	beqz	$at,.L0f172808
/*  f1727e0:	00687821 */ 	addu	$t7,$v1,$t0
/*  f1727e4:	004bc021 */ 	addu	$t8,$v0,$t3
/*  f1727e8:	93190000 */ 	lbu	$t9,0x0($t8)
/*  f1727ec:	00197040 */ 	sll	$t6,$t9,0x1
/*  f1727f0:	008e7821 */ 	addu	$t7,$a0,$t6
/*  f1727f4:	95f80000 */ 	lhu	$t8,0x0($t7)
/*  f1727f8:	0018c840 */ 	sll	$t9,$t8,0x1
/*  f1727fc:	372e0001 */ 	ori	$t6,$t9,0x1
/*  f172800:	10000008 */ 	b	.L0f172824
/*  f172804:	a54e0000 */ 	sh	$t6,0x0($t2)
.L0f172808:
/*  f172808:	95f80000 */ 	lhu	$t8,0x0($t7)
/*  f17280c:	0018c840 */ 	sll	$t9,$t8,0x1
/*  f172810:	00997021 */ 	addu	$t6,$a0,$t9
/*  f172814:	95cf0000 */ 	lhu	$t7,0x0($t6)
/*  f172818:	000fc040 */ 	sll	$t8,$t7,0x1
/*  f17281c:	37190001 */ 	ori	$t9,$t8,0x1
/*  f172820:	a5590000 */ 	sh	$t9,0x0($t2)
.L0f172824:
/*  f172824:	256b0001 */ 	addiu	$t3,$t3,0x1
/*  f172828:	25080002 */ 	addiu	$t0,$t0,0x2
/*  f17282c:	1565ffea */ 	bne	$t3,$a1,.L0f1727d8
/*  f172830:	254a0002 */ 	addiu	$t2,$t2,0x2
.L0f172834:
/*  f172834:	24e70001 */ 	addiu	$a3,$a3,0x1
/*  f172838:	012d4821 */ 	addu	$t1,$t1,$t5
/*  f17283c:	00451021 */ 	addu	$v0,$v0,$a1
/*  f172840:	14e6ffe1 */ 	bne	$a3,$a2,.L0f1727c8
/*  f172844:	006c1821 */ 	addu	$v1,$v1,$t4
.L0f172848:
/*  f172848:	24ae0003 */ 	addiu	$t6,$a1,0x3
/*  f17284c:	31cf0ffc */ 	andi	$t7,$t6,0xffc
/*  f172850:	01e60019 */ 	multu	$t7,$a2
/*  f172854:	00001012 */ 	mflo	$v0
/*  f172858:	0002c040 */ 	sll	$t8,$v0,0x1
/*  f17285c:	10000063 */ 	b	.L0f1729ec
/*  f172860:	03001025 */ 	or	$v0,$t8,$zero
/*  f172864:	18c00021 */ 	blez	$a2,.L0f1728ec
/*  f172868:	00003825 */ 	or	$a3,$zero,$zero
/*  f17286c:	24a80007 */ 	addiu	$t0,$a1,0x7
/*  f172870:	31190ff8 */ 	andi	$t9,$t0,0xff8
/*  f172874:	03204025 */ 	or	$t0,$t9,$zero
/*  f172878:	00056040 */ 	sll	$t4,$a1,0x1
/*  f17287c:	8fa40030 */ 	lw	$a0,0x30($sp)
.L0f172880:
/*  f172880:	18a00015 */ 	blez	$a1,.L0f1728d8
/*  f172884:	00005825 */ 	or	$t3,$zero,$zero
/*  f172888:	01404825 */ 	or	$t1,$t2,$zero
.L0f17288c:
/*  f17288c:	2a010101 */ 	slti	$at,$s0,0x101
/*  f172890:	10200008 */ 	beqz	$at,.L0f1728b4
/*  f172894:	000b7840 */ 	sll	$t7,$t3,0x1
/*  f172898:	004b7021 */ 	addu	$t6,$v0,$t3
/*  f17289c:	91cf0000 */ 	lbu	$t7,0x0($t6)
/*  f1728a0:	000fc040 */ 	sll	$t8,$t7,0x1
/*  f1728a4:	0098c821 */ 	addu	$t9,$a0,$t8
/*  f1728a8:	972e0000 */ 	lhu	$t6,0x0($t9)
/*  f1728ac:	10000007 */ 	b	.L0f1728cc
/*  f1728b0:	a12e0000 */ 	sb	$t6,0x0($t1)
.L0f1728b4:
/*  f1728b4:	006fc021 */ 	addu	$t8,$v1,$t7
/*  f1728b8:	97190000 */ 	lhu	$t9,0x0($t8)
/*  f1728bc:	00197040 */ 	sll	$t6,$t9,0x1
/*  f1728c0:	008e7821 */ 	addu	$t7,$a0,$t6
/*  f1728c4:	95f80000 */ 	lhu	$t8,0x0($t7)
/*  f1728c8:	a1380000 */ 	sb	$t8,0x0($t1)
.L0f1728cc:
/*  f1728cc:	256b0001 */ 	addiu	$t3,$t3,0x1
/*  f1728d0:	1565ffee */ 	bne	$t3,$a1,.L0f17288c
/*  f1728d4:	25290001 */ 	addiu	$t1,$t1,0x1
.L0f1728d8:
/*  f1728d8:	24e70001 */ 	addiu	$a3,$a3,0x1
/*  f1728dc:	01485021 */ 	addu	$t2,$t2,$t0
/*  f1728e0:	00451021 */ 	addu	$v0,$v0,$a1
/*  f1728e4:	14e6ffe6 */ 	bne	$a3,$a2,.L0f172880
/*  f1728e8:	006c1821 */ 	addu	$v1,$v1,$t4
.L0f1728ec:
/*  f1728ec:	24b90007 */ 	addiu	$t9,$a1,0x7
/*  f1728f0:	332e0ff8 */ 	andi	$t6,$t9,0xff8
/*  f1728f4:	01c60019 */ 	multu	$t6,$a2
/*  f1728f8:	00001012 */ 	mflo	$v0
/*  f1728fc:	1000003c */ 	b	.L0f1729f0
/*  f172900:	8fb00004 */ 	lw	$s0,0x4($sp)
/*  f172904:	18c00031 */ 	blez	$a2,.L0f1729cc
/*  f172908:	00003825 */ 	or	$a3,$zero,$zero
/*  f17290c:	24a9000f */ 	addiu	$t1,$a1,0xf
/*  f172910:	312f0ff0 */ 	andi	$t7,$t1,0xff0
/*  f172914:	000f4843 */ 	sra	$t1,$t7,0x1
/*  f172918:	00056040 */ 	sll	$t4,$a1,0x1
/*  f17291c:	8fa40030 */ 	lw	$a0,0x30($sp)
.L0f172920:
/*  f172920:	18a00025 */ 	blez	$a1,.L0f1729b8
/*  f172924:	00005825 */ 	or	$t3,$zero,$zero
/*  f172928:	2a010101 */ 	slti	$at,$s0,0x101
.L0f17292c:
/*  f17292c:	10200010 */ 	beqz	$at,.L0f172970
/*  f172930:	000b7040 */ 	sll	$t6,$t3,0x1
/*  f172934:	01624021 */ 	addu	$t0,$t3,$v0
/*  f172938:	91190001 */ 	lbu	$t9,0x1($t0)
/*  f17293c:	00197040 */ 	sll	$t6,$t9,0x1
/*  f172940:	91190000 */ 	lbu	$t9,0x0($t0)
/*  f172944:	008e7821 */ 	addu	$t7,$a0,$t6
/*  f172948:	95f80000 */ 	lhu	$t8,0x0($t7)
/*  f17294c:	00197040 */ 	sll	$t6,$t9,0x1
/*  f172950:	008e7821 */ 	addu	$t7,$a0,$t6
/*  f172954:	95f90000 */ 	lhu	$t9,0x0($t7)
/*  f172958:	00197100 */ 	sll	$t6,$t9,0x4
/*  f17295c:	000bc843 */ 	sra	$t9,$t3,0x1
/*  f172960:	030e7825 */ 	or	$t7,$t8,$t6
/*  f172964:	0159c021 */ 	addu	$t8,$t2,$t9
/*  f172968:	1000000f */ 	b	.L0f1729a8
/*  f17296c:	a30f0000 */ 	sb	$t7,0x0($t8)
.L0f172970:
/*  f172970:	006e4021 */ 	addu	$t0,$v1,$t6
/*  f172974:	95190002 */ 	lhu	$t9,0x2($t0)
/*  f172978:	00197840 */ 	sll	$t7,$t9,0x1
/*  f17297c:	95190000 */ 	lhu	$t9,0x0($t0)
/*  f172980:	008fc021 */ 	addu	$t8,$a0,$t7
/*  f172984:	970e0000 */ 	lhu	$t6,0x0($t8)
/*  f172988:	00197840 */ 	sll	$t7,$t9,0x1
/*  f17298c:	008fc021 */ 	addu	$t8,$a0,$t7
/*  f172990:	97190000 */ 	lhu	$t9,0x0($t8)
/*  f172994:	00197900 */ 	sll	$t7,$t9,0x4
/*  f172998:	000bc843 */ 	sra	$t9,$t3,0x1
/*  f17299c:	01cfc025 */ 	or	$t8,$t6,$t7
/*  f1729a0:	01597021 */ 	addu	$t6,$t2,$t9
/*  f1729a4:	a1d80000 */ 	sb	$t8,0x0($t6)
.L0f1729a8:
/*  f1729a8:	256b0002 */ 	addiu	$t3,$t3,0x2
/*  f1729ac:	0165082a */ 	slt	$at,$t3,$a1
/*  f1729b0:	5420ffde */ 	bnezl	$at,.L0f17292c
/*  f1729b4:	2a010101 */ 	slti	$at,$s0,0x101
.L0f1729b8:
/*  f1729b8:	24e70001 */ 	addiu	$a3,$a3,0x1
/*  f1729bc:	01495021 */ 	addu	$t2,$t2,$t1
/*  f1729c0:	00451021 */ 	addu	$v0,$v0,$a1
/*  f1729c4:	14e6ffd6 */ 	bne	$a3,$a2,.L0f172920
/*  f1729c8:	006c1821 */ 	addu	$v1,$v1,$t4
.L0f1729cc:
/*  f1729cc:	24af000f */ 	addiu	$t7,$a1,0xf
/*  f1729d0:	31f90ff0 */ 	andi	$t9,$t7,0xff0
/*  f1729d4:	0019c043 */ 	sra	$t8,$t9,0x1
/*  f1729d8:	03060019 */ 	multu	$t8,$a2
/*  f1729dc:	00001012 */ 	mflo	$v0
/*  f1729e0:	10000003 */ 	b	.L0f1729f0
/*  f1729e4:	8fb00004 */ 	lw	$s0,0x4($sp)
.L0f1729e8:
/*  f1729e8:	00001025 */ 	or	$v0,$zero,$zero
.L0f1729ec:
/*  f1729ec:	8fb00004 */ 	lw	$s0,0x4($sp)
.L0f1729f0:
/*  f1729f0:	03e00008 */ 	jr	$ra
/*  f1729f4:	27bd0020 */ 	addiu	$sp,$sp,0x20
);

GLOBAL_ASM(
glabel func0f1729f8
.late_rodata
glabel var7f1b7b50
.word func0f1729f8+0x2c # f172a24
glabel var7f1b7b54
.word func0f1729f8+0x40 # f172a38
glabel var7f1b7b58
.word func0f1729f8+0x2c # f172a24
glabel var7f1b7b5c
.word func0f1729f8+0x40 # f172a38
glabel var7f1b7b60
.word func0f1729f8+0x40 # f172a38
glabel var7f1b7b64
.word func0f1729f8+0x58 # f172a50
glabel var7f1b7b68
.word func0f1729f8+0x70 # f172a68
glabel var7f1b7b6c
.word func0f1729f8+0x58 # f172a50
glabel var7f1b7b70
.word func0f1729f8+0x70 # f172a68
glabel var7f1b7b74
.word func0f1729f8+0x58 # f172a50
glabel var7f1b7b78
.word func0f1729f8+0x70 # f172a68
glabel var7f1b7b7c
.word func0f1729f8+0x58 # f172a50
glabel var7f1b7b80
.word func0f1729f8+0x70 # f172a68
.text
/*  f1729f8:	27bdfff0 */ 	addiu	$sp,$sp,-16
/*  f1729fc:	2ce1000d */ 	sltiu	$at,$a3,0xd
/*  f172a00:	afa50014 */ 	sw	$a1,0x14($sp)
/*  f172a04:	1020001d */ 	beqz	$at,.L0f172a7c
/*  f172a08:	00e01825 */ 	or	$v1,$a3,$zero
/*  f172a0c:	00077080 */ 	sll	$t6,$a3,0x2
/*  f172a10:	3c017f1b */ 	lui	$at,%hi(var7f1b7b50)
/*  f172a14:	002e0821 */ 	addu	$at,$at,$t6
/*  f172a18:	8c2e7b50 */ 	lw	$t6,%lo(var7f1b7b50)($at)
/*  f172a1c:	01c00008 */ 	jr	$t6
/*  f172a20:	00000000 */ 	nop
/*  f172a24:	8fa50014 */ 	lw	$a1,0x14($sp)
/*  f172a28:	24a50003 */ 	addiu	$a1,$a1,0x3
/*  f172a2c:	30af0ffc */ 	andi	$t7,$a1,0xffc
/*  f172a30:	10000012 */ 	b	.L0f172a7c
/*  f172a34:	afaf0004 */ 	sw	$t7,0x4($sp)
/*  f172a38:	8fa50014 */ 	lw	$a1,0x14($sp)
/*  f172a3c:	24a50003 */ 	addiu	$a1,$a1,0x3
/*  f172a40:	30b80ffc */ 	andi	$t8,$a1,0xffc
/*  f172a44:	0018c843 */ 	sra	$t9,$t8,0x1
/*  f172a48:	1000000c */ 	b	.L0f172a7c
/*  f172a4c:	afb90004 */ 	sw	$t9,0x4($sp)
/*  f172a50:	8fa50014 */ 	lw	$a1,0x14($sp)
/*  f172a54:	24a50007 */ 	addiu	$a1,$a1,0x7
/*  f172a58:	30aa0ff8 */ 	andi	$t2,$a1,0xff8
/*  f172a5c:	000a5883 */ 	sra	$t3,$t2,0x2
/*  f172a60:	10000006 */ 	b	.L0f172a7c
/*  f172a64:	afab0004 */ 	sw	$t3,0x4($sp)
/*  f172a68:	8fa50014 */ 	lw	$a1,0x14($sp)
/*  f172a6c:	24a5000f */ 	addiu	$a1,$a1,0xf
/*  f172a70:	30ac0ff0 */ 	andi	$t4,$a1,0xff0
/*  f172a74:	000c68c3 */ 	sra	$t5,$t4,0x3
/*  f172a78:	afad0004 */ 	sw	$t5,0x4($sp)
.L0f172a7c:
/*  f172a7c:	8fa50004 */ 	lw	$a1,0x4($sp)
/*  f172a80:	24010002 */ 	addiu	$at,$zero,0x2
/*  f172a84:	00057080 */ 	sll	$t6,$a1,0x2
/*  f172a88:	10600002 */ 	beqz	$v1,.L0f172a94
/*  f172a8c:	008e1021 */ 	addu	$v0,$a0,$t6
/*  f172a90:	1461001b */ 	bne	$v1,$at,.L0f172b00
.L0f172a94:
/*  f172a94:	28c10002 */ 	slti	$at,$a2,0x2
/*  f172a98:	1420002e */ 	bnez	$at,.L0f172b54
/*  f172a9c:	24030001 */ 	addiu	$v1,$zero,0x1
/*  f172aa0:	00a04825 */ 	or	$t1,$a1,$zero
/*  f172aa4:	000978c0 */ 	sll	$t7,$t1,0x3
/*  f172aa8:	01e04825 */ 	or	$t1,$t7,$zero
.L0f172aac:
/*  f172aac:	18a0000e */ 	blez	$a1,.L0f172ae8
/*  f172ab0:	00002025 */ 	or	$a0,$zero,$zero
/*  f172ab4:	00404025 */ 	or	$t0,$v0,$zero
.L0f172ab8:
/*  f172ab8:	8d070000 */ 	lw	$a3,0x0($t0)
/*  f172abc:	8d180008 */ 	lw	$t8,0x8($t0)
/*  f172ac0:	8d19000c */ 	lw	$t9,0xc($t0)
/*  f172ac4:	ad070008 */ 	sw	$a3,0x8($t0)
/*  f172ac8:	8d070004 */ 	lw	$a3,0x4($t0)
/*  f172acc:	24840004 */ 	addiu	$a0,$a0,0x4
/*  f172ad0:	0085082a */ 	slt	$at,$a0,$a1
/*  f172ad4:	25080010 */ 	addiu	$t0,$t0,0x10
/*  f172ad8:	ad18fff0 */ 	sw	$t8,-0x10($t0)
/*  f172adc:	ad19fff4 */ 	sw	$t9,-0xc($t0)
/*  f172ae0:	1420fff5 */ 	bnez	$at,.L0f172ab8
/*  f172ae4:	ad07fffc */ 	sw	$a3,-0x4($t0)
.L0f172ae8:
/*  f172ae8:	24630002 */ 	addiu	$v1,$v1,0x2
/*  f172aec:	0066082a */ 	slt	$at,$v1,$a2
/*  f172af0:	1420ffee */ 	bnez	$at,.L0f172aac
/*  f172af4:	00491021 */ 	addu	$v0,$v0,$t1
/*  f172af8:	10000016 */ 	b	.L0f172b54
/*  f172afc:	00000000 */ 	nop
.L0f172b00:
/*  f172b00:	28c10002 */ 	slti	$at,$a2,0x2
/*  f172b04:	14200013 */ 	bnez	$at,.L0f172b54
/*  f172b08:	24030001 */ 	addiu	$v1,$zero,0x1
/*  f172b0c:	00a04825 */ 	or	$t1,$a1,$zero
/*  f172b10:	000950c0 */ 	sll	$t2,$t1,0x3
/*  f172b14:	01404825 */ 	or	$t1,$t2,$zero
.L0f172b18:
/*  f172b18:	18a0000a */ 	blez	$a1,.L0f172b44
/*  f172b1c:	00002025 */ 	or	$a0,$zero,$zero
/*  f172b20:	00404025 */ 	or	$t0,$v0,$zero
.L0f172b24:
/*  f172b24:	8d070000 */ 	lw	$a3,0x0($t0)
/*  f172b28:	8d0b0004 */ 	lw	$t3,0x4($t0)
/*  f172b2c:	24840002 */ 	addiu	$a0,$a0,0x2
/*  f172b30:	0085082a */ 	slt	$at,$a0,$a1
/*  f172b34:	25080008 */ 	addiu	$t0,$t0,0x8
/*  f172b38:	ad07fffc */ 	sw	$a3,-0x4($t0)
/*  f172b3c:	1420fff9 */ 	bnez	$at,.L0f172b24
/*  f172b40:	ad0bfff8 */ 	sw	$t3,-0x8($t0)
.L0f172b44:
/*  f172b44:	24630002 */ 	addiu	$v1,$v1,0x2
/*  f172b48:	0066082a */ 	slt	$at,$v1,$a2
/*  f172b4c:	1420fff2 */ 	bnez	$at,.L0f172b18
/*  f172b50:	00491021 */ 	addu	$v0,$v0,$t1
.L0f172b54:
/*  f172b54:	03e00008 */ 	jr	$ra
/*  f172b58:	27bd0010 */ 	addiu	$sp,$sp,0x10
);

GLOBAL_ASM(
glabel func0f172b5c
.late_rodata
glabel var7f1b7b84
.word func0f172b5c+0xc8 # f172c24
glabel var7f1b7b88
.word func0f172b5c+0x10c # f172c68
glabel var7f1b7b8c
.word func0f172b5c+0x150 # f172cac
glabel var7f1b7b90
.word func0f172b5c+0x194 # f172cf0
glabel var7f1b7b94
.word func0f172b5c+0x1e0 # f172d3c
glabel var7f1b7b98
.word func0f172b5c+0x23c # f172d98
glabel var7f1b7b9c
.word func0f172b5c+0x298 # f172df4
.text
/*  f172b5c:	27bdfff0 */ 	addiu	$sp,$sp,-16
/*  f172b60:	afb10008 */ 	sw	$s1,0x8($sp)
/*  f172b64:	afb00004 */ 	sw	$s0,0x4($sp)
/*  f172b68:	00808025 */ 	or	$s0,$a0,$zero
/*  f172b6c:	00e08825 */ 	or	$s1,$a3,$zero
/*  f172b70:	afb2000c */ 	sw	$s2,0xc($sp)
/*  f172b74:	18c000b9 */ 	blez	$a2,.L0f172e5c
/*  f172b78:	00001025 */ 	or	$v0,$zero,$zero
/*  f172b7c:	8fa90020 */ 	lw	$t1,0x20($sp)
.L0f172b80:
/*  f172b80:	18a000b3 */ 	blez	$a1,.L0f172e50
/*  f172b84:	00001825 */ 	or	$v1,$zero,$zero
/*  f172b88:	00450019 */ 	multu	$v0,$a1
/*  f172b8c:	00095040 */ 	sll	$t2,$t1,0x1
/*  f172b90:	02209025 */ 	or	$s2,$s1,$zero
/*  f172b94:	00003812 */ 	mflo	$a3
/*  f172b98:	00f04021 */ 	addu	$t0,$a3,$s0
/*  f172b9c:	00000000 */ 	nop
.L0f172ba0:
/*  f172ba0:	910e0000 */ 	lbu	$t6,0x0($t0)
/*  f172ba4:	2e410007 */ 	sltiu	$at,$s2,0x7
/*  f172ba8:	18600003 */ 	blez	$v1,.L0f172bb8
/*  f172bac:	01ca2021 */ 	addu	$a0,$t6,$t2
/*  f172bb0:	10000002 */ 	b	.L0f172bbc
/*  f172bb4:	910bffff */ 	lbu	$t3,-0x1($t0)
.L0f172bb8:
/*  f172bb8:	00005825 */ 	or	$t3,$zero,$zero
.L0f172bbc:
/*  f172bbc:	18400008 */ 	blez	$v0,.L0f172be0
/*  f172bc0:	00006025 */ 	or	$t4,$zero,$zero
/*  f172bc4:	244fffff */ 	addiu	$t7,$v0,-1
/*  f172bc8:	01e50019 */ 	multu	$t7,$a1
/*  f172bcc:	0000c012 */ 	mflo	$t8
/*  f172bd0:	0303c821 */ 	addu	$t9,$t8,$v1
/*  f172bd4:	03307021 */ 	addu	$t6,$t9,$s0
/*  f172bd8:	10000001 */ 	b	.L0f172be0
/*  f172bdc:	91cc0000 */ 	lbu	$t4,0x0($t6)
.L0f172be0:
/*  f172be0:	18600009 */ 	blez	$v1,.L0f172c08
/*  f172be4:	00006825 */ 	or	$t5,$zero,$zero
/*  f172be8:	18400007 */ 	blez	$v0,.L0f172c08
/*  f172bec:	244fffff */ 	addiu	$t7,$v0,-1
/*  f172bf0:	01e50019 */ 	multu	$t7,$a1
/*  f172bf4:	0000c012 */ 	mflo	$t8
/*  f172bf8:	0303c821 */ 	addu	$t9,$t8,$v1
/*  f172bfc:	03307021 */ 	addu	$t6,$t9,$s0
/*  f172c00:	10000001 */ 	b	.L0f172c08
/*  f172c04:	91cdffff */ 	lbu	$t5,-0x1($t6)
.L0f172c08:
/*  f172c08:	1020008e */ 	beqz	$at,.L0f172e44
/*  f172c0c:	00127880 */ 	sll	$t7,$s2,0x2
/*  f172c10:	3c017f1b */ 	lui	$at,%hi(var7f1b7b84)
/*  f172c14:	002f0821 */ 	addu	$at,$at,$t7
/*  f172c18:	8c2f7b84 */ 	lw	$t7,%lo(var7f1b7b84)($at)
/*  f172c1c:	01e00008 */ 	jr	$t7
/*  f172c20:	00000000 */ 	nop
/*  f172c24:	008bc021 */ 	addu	$t8,$a0,$t3
/*  f172c28:	0309001a */ 	div	$zero,$t8,$t1
/*  f172c2c:	02077021 */ 	addu	$t6,$s0,$a3
/*  f172c30:	01c37821 */ 	addu	$t7,$t6,$v1
/*  f172c34:	0000c810 */ 	mfhi	$t9
/*  f172c38:	a1f90000 */ 	sb	$t9,0x0($t7)
/*  f172c3c:	15200002 */ 	bnez	$t1,.L0f172c48
/*  f172c40:	00000000 */ 	nop
/*  f172c44:	0007000d */ 	break	0x7
.L0f172c48:
/*  f172c48:	2401ffff */ 	addiu	$at,$zero,-1
/*  f172c4c:	15210004 */ 	bne	$t1,$at,.L0f172c60
/*  f172c50:	3c018000 */ 	lui	$at,0x8000
/*  f172c54:	17010002 */ 	bne	$t8,$at,.L0f172c60
/*  f172c58:	00000000 */ 	nop
/*  f172c5c:	0006000d */ 	break	0x6
.L0f172c60:
/*  f172c60:	10000079 */ 	b	.L0f172e48
/*  f172c64:	24630001 */ 	addiu	$v1,$v1,0x1
/*  f172c68:	008cc021 */ 	addu	$t8,$a0,$t4
/*  f172c6c:	0309001a */ 	div	$zero,$t8,$t1
/*  f172c70:	0207c821 */ 	addu	$t9,$s0,$a3
/*  f172c74:	03237821 */ 	addu	$t7,$t9,$v1
/*  f172c78:	00007010 */ 	mfhi	$t6
/*  f172c7c:	a1ee0000 */ 	sb	$t6,0x0($t7)
/*  f172c80:	15200002 */ 	bnez	$t1,.L0f172c8c
/*  f172c84:	00000000 */ 	nop
/*  f172c88:	0007000d */ 	break	0x7
.L0f172c8c:
/*  f172c8c:	2401ffff */ 	addiu	$at,$zero,-1
/*  f172c90:	15210004 */ 	bne	$t1,$at,.L0f172ca4
/*  f172c94:	3c018000 */ 	lui	$at,0x8000
/*  f172c98:	17010002 */ 	bne	$t8,$at,.L0f172ca4
/*  f172c9c:	00000000 */ 	nop
/*  f172ca0:	0006000d */ 	break	0x6
.L0f172ca4:
/*  f172ca4:	10000068 */ 	b	.L0f172e48
/*  f172ca8:	24630001 */ 	addiu	$v1,$v1,0x1
/*  f172cac:	008dc021 */ 	addu	$t8,$a0,$t5
/*  f172cb0:	0309001a */ 	div	$zero,$t8,$t1
/*  f172cb4:	02077021 */ 	addu	$t6,$s0,$a3
/*  f172cb8:	01c37821 */ 	addu	$t7,$t6,$v1
/*  f172cbc:	0000c810 */ 	mfhi	$t9
/*  f172cc0:	a1f90000 */ 	sb	$t9,0x0($t7)
/*  f172cc4:	15200002 */ 	bnez	$t1,.L0f172cd0
/*  f172cc8:	00000000 */ 	nop
/*  f172ccc:	0007000d */ 	break	0x7
.L0f172cd0:
/*  f172cd0:	2401ffff */ 	addiu	$at,$zero,-1
/*  f172cd4:	15210004 */ 	bne	$t1,$at,.L0f172ce8
/*  f172cd8:	3c018000 */ 	lui	$at,0x8000
/*  f172cdc:	17010002 */ 	bne	$t8,$at,.L0f172ce8
/*  f172ce0:	00000000 */ 	nop
/*  f172ce4:	0006000d */ 	break	0x6
.L0f172ce8:
/*  f172ce8:	10000057 */ 	b	.L0f172e48
/*  f172cec:	24630001 */ 	addiu	$v1,$v1,0x1
/*  f172cf0:	016cc021 */ 	addu	$t8,$t3,$t4
/*  f172cf4:	030d7023 */ 	subu	$t6,$t8,$t5
/*  f172cf8:	01c4c821 */ 	addu	$t9,$t6,$a0
/*  f172cfc:	0329001a */ 	div	$zero,$t9,$t1
/*  f172d00:	0207c021 */ 	addu	$t8,$s0,$a3
/*  f172d04:	03037021 */ 	addu	$t6,$t8,$v1
/*  f172d08:	00007810 */ 	mfhi	$t7
/*  f172d0c:	a1cf0000 */ 	sb	$t7,0x0($t6)
/*  f172d10:	15200002 */ 	bnez	$t1,.L0f172d1c
/*  f172d14:	00000000 */ 	nop
/*  f172d18:	0007000d */ 	break	0x7
.L0f172d1c:
/*  f172d1c:	2401ffff */ 	addiu	$at,$zero,-1
/*  f172d20:	15210004 */ 	bne	$t1,$at,.L0f172d34
/*  f172d24:	3c018000 */ 	lui	$at,0x8000
/*  f172d28:	17210002 */ 	bne	$t9,$at,.L0f172d34
/*  f172d2c:	00000000 */ 	nop
/*  f172d30:	0006000d */ 	break	0x6
.L0f172d34:
/*  f172d34:	10000044 */ 	b	.L0f172e48
/*  f172d38:	24630001 */ 	addiu	$v1,$v1,0x1
/*  f172d3c:	018dc823 */ 	subu	$t9,$t4,$t5
/*  f172d40:	07210003 */ 	bgez	$t9,.L0f172d50
/*  f172d44:	0019c043 */ 	sra	$t8,$t9,0x1
/*  f172d48:	27210001 */ 	addiu	$at,$t9,0x1
/*  f172d4c:	0001c043 */ 	sra	$t8,$at,0x1
.L0f172d50:
/*  f172d50:	030b7821 */ 	addu	$t7,$t8,$t3
/*  f172d54:	01e47021 */ 	addu	$t6,$t7,$a0
/*  f172d58:	01c9001a */ 	div	$zero,$t6,$t1
/*  f172d5c:	0207c021 */ 	addu	$t8,$s0,$a3
/*  f172d60:	03037821 */ 	addu	$t7,$t8,$v1
/*  f172d64:	0000c810 */ 	mfhi	$t9
/*  f172d68:	a1f90000 */ 	sb	$t9,0x0($t7)
/*  f172d6c:	15200002 */ 	bnez	$t1,.L0f172d78
/*  f172d70:	00000000 */ 	nop
/*  f172d74:	0007000d */ 	break	0x7
.L0f172d78:
/*  f172d78:	2401ffff */ 	addiu	$at,$zero,-1
/*  f172d7c:	15210004 */ 	bne	$t1,$at,.L0f172d90
/*  f172d80:	3c018000 */ 	lui	$at,0x8000
/*  f172d84:	15c10002 */ 	bne	$t6,$at,.L0f172d90
/*  f172d88:	00000000 */ 	nop
/*  f172d8c:	0006000d */ 	break	0x6
.L0f172d90:
/*  f172d90:	1000002d */ 	b	.L0f172e48
/*  f172d94:	24630001 */ 	addiu	$v1,$v1,0x1
/*  f172d98:	016d7023 */ 	subu	$t6,$t3,$t5
/*  f172d9c:	05c10003 */ 	bgez	$t6,.L0f172dac
/*  f172da0:	000ec043 */ 	sra	$t8,$t6,0x1
/*  f172da4:	25c10001 */ 	addiu	$at,$t6,0x1
/*  f172da8:	0001c043 */ 	sra	$t8,$at,0x1
.L0f172dac:
/*  f172dac:	030cc821 */ 	addu	$t9,$t8,$t4
/*  f172db0:	03247821 */ 	addu	$t7,$t9,$a0
/*  f172db4:	01e9001a */ 	div	$zero,$t7,$t1
/*  f172db8:	0207c021 */ 	addu	$t8,$s0,$a3
/*  f172dbc:	0303c821 */ 	addu	$t9,$t8,$v1
/*  f172dc0:	00007010 */ 	mfhi	$t6
/*  f172dc4:	a32e0000 */ 	sb	$t6,0x0($t9)
/*  f172dc8:	15200002 */ 	bnez	$t1,.L0f172dd4
/*  f172dcc:	00000000 */ 	nop
/*  f172dd0:	0007000d */ 	break	0x7
.L0f172dd4:
/*  f172dd4:	2401ffff */ 	addiu	$at,$zero,-1
/*  f172dd8:	15210004 */ 	bne	$t1,$at,.L0f172dec
/*  f172ddc:	3c018000 */ 	lui	$at,0x8000
/*  f172de0:	15e10002 */ 	bne	$t7,$at,.L0f172dec
/*  f172de4:	00000000 */ 	nop
/*  f172de8:	0006000d */ 	break	0x6
.L0f172dec:
/*  f172dec:	10000016 */ 	b	.L0f172e48
/*  f172df0:	24630001 */ 	addiu	$v1,$v1,0x1
/*  f172df4:	016c7821 */ 	addu	$t7,$t3,$t4
/*  f172df8:	05e10003 */ 	bgez	$t7,.L0f172e08
/*  f172dfc:	000fc043 */ 	sra	$t8,$t7,0x1
/*  f172e00:	25e10001 */ 	addiu	$at,$t7,0x1
/*  f172e04:	0001c043 */ 	sra	$t8,$at,0x1
.L0f172e08:
/*  f172e08:	03047021 */ 	addu	$t6,$t8,$a0
/*  f172e0c:	01c9001a */ 	div	$zero,$t6,$t1
/*  f172e10:	0000c810 */ 	mfhi	$t9
/*  f172e14:	02077821 */ 	addu	$t7,$s0,$a3
/*  f172e18:	01e3c021 */ 	addu	$t8,$t7,$v1
/*  f172e1c:	a3190000 */ 	sb	$t9,0x0($t8)
/*  f172e20:	15200002 */ 	bnez	$t1,.L0f172e2c
/*  f172e24:	00000000 */ 	nop
/*  f172e28:	0007000d */ 	break	0x7
.L0f172e2c:
/*  f172e2c:	2401ffff */ 	addiu	$at,$zero,-1
/*  f172e30:	15210004 */ 	bne	$t1,$at,.L0f172e44
/*  f172e34:	3c018000 */ 	lui	$at,0x8000
/*  f172e38:	15c10002 */ 	bne	$t6,$at,.L0f172e44
/*  f172e3c:	00000000 */ 	nop
/*  f172e40:	0006000d */ 	break	0x6
.L0f172e44:
/*  f172e44:	24630001 */ 	addiu	$v1,$v1,0x1
.L0f172e48:
/*  f172e48:	1465ff55 */ 	bne	$v1,$a1,.L0f172ba0
/*  f172e4c:	25080001 */ 	addiu	$t0,$t0,0x1
.L0f172e50:
/*  f172e50:	24420001 */ 	addiu	$v0,$v0,0x1
/*  f172e54:	1446ff4a */ 	bne	$v0,$a2,.L0f172b80
/*  f172e58:	00000000 */ 	nop
.L0f172e5c:
/*  f172e5c:	8fb00004 */ 	lw	$s0,0x4($sp)
/*  f172e60:	8fb10008 */ 	lw	$s1,0x8($sp)
/*  f172e64:	8fb2000c */ 	lw	$s2,0xc($sp)
/*  f172e68:	03e00008 */ 	jr	$ra
/*  f172e6c:	27bd0010 */ 	addiu	$sp,$sp,0x10
);

GLOBAL_ASM(
glabel func0f172e70
/*  f172e70:	00a67021 */ 	addu	$t6,$a1,$a2
/*  f172e74:	00a67821 */ 	addu	$t7,$a1,$a2
/*  f172e78:	ac850000 */ 	sw	$a1,0x0($a0)
/*  f172e7c:	ac8e0004 */ 	sw	$t6,0x4($a0)
/*  f172e80:	ac850008 */ 	sw	$a1,0x8($a0)
/*  f172e84:	03e00008 */ 	jr	$ra
/*  f172e88:	ac8f000c */ 	sw	$t7,0xc($a0)
);

GLOBAL_ASM(
glabel func0f172e8c
/*  f172e8c:	14a00003 */ 	bnez	$a1,.L0f172e9c
/*  f172e90:	3c0e800b */ 	lui	$t6,%hi(var800aabc8)
/*  f172e94:	3c05800b */ 	lui	$a1,%hi(var800aabc8)
/*  f172e98:	24a5abc8 */ 	addiu	$a1,$a1,%lo(var800aabc8)
.L0f172e9c:
/*  f172e9c:	25ceabc8 */ 	addiu	$t6,$t6,%lo(var800aabc8)
/*  f172ea0:	54ae0017 */ 	bnel	$a1,$t6,.L0f172f00
/*  f172ea4:	8ca20004 */ 	lw	$v0,0x4($a1)
/*  f172ea8:	8ca30004 */ 	lw	$v1,0x4($a1)
/*  f172eac:	3c058000 */ 	lui	$a1,0x8000
/*  f172eb0:	10600010 */ 	beqz	$v1,.L0f172ef4
/*  f172eb4:	00000000 */ 	nop
/*  f172eb8:	8c6f0000 */ 	lw	$t7,0x0($v1)
.L0f172ebc:
/*  f172ebc:	000fc502 */ 	srl	$t8,$t7,0x14
/*  f172ec0:	54980004 */ 	bnel	$a0,$t8,.L0f172ed4
/*  f172ec4:	8c62000c */ 	lw	$v0,0xc($v1)
/*  f172ec8:	03e00008 */ 	jr	$ra
/*  f172ecc:	00601025 */ 	or	$v0,$v1,$zero
/*  f172ed0:	8c62000c */ 	lw	$v0,0xc($v1)
.L0f172ed4:
/*  f172ed4:	0002c900 */ 	sll	$t9,$v0,0x4
/*  f172ed8:	00191202 */ 	srl	$v0,$t9,0x8
/*  f172edc:	14400003 */ 	bnez	$v0,.L0f172eec
/*  f172ee0:	00451825 */ 	or	$v1,$v0,$a1
/*  f172ee4:	03e00008 */ 	jr	$ra
/*  f172ee8:	00001025 */ 	or	$v0,$zero,$zero
.L0f172eec:
/*  f172eec:	5460fff3 */ 	bnezl	$v1,.L0f172ebc
/*  f172ef0:	8c6f0000 */ 	lw	$t7,0x0($v1)
.L0f172ef4:
/*  f172ef4:	03e00008 */ 	jr	$ra
/*  f172ef8:	00001025 */ 	or	$v0,$zero,$zero
/*  f172efc:	8ca20004 */ 	lw	$v0,0x4($a1)
.L0f172f00:
/*  f172f00:	8ca3000c */ 	lw	$v1,0xc($a1)
/*  f172f04:	0062082b */ 	sltu	$at,$v1,$v0
/*  f172f08:	5020000c */ 	beqzl	$at,.L0f172f3c
/*  f172f0c:	00001025 */ 	or	$v0,$zero,$zero
/*  f172f10:	8c690000 */ 	lw	$t1,0x0($v1)
.L0f172f14:
/*  f172f14:	00095502 */ 	srl	$t2,$t1,0x14
/*  f172f18:	548a0004 */ 	bnel	$a0,$t2,.L0f172f2c
/*  f172f1c:	24630010 */ 	addiu	$v1,$v1,0x10
/*  f172f20:	03e00008 */ 	jr	$ra
/*  f172f24:	00601025 */ 	or	$v0,$v1,$zero
/*  f172f28:	24630010 */ 	addiu	$v1,$v1,0x10
.L0f172f2c:
/*  f172f2c:	0062082b */ 	sltu	$at,$v1,$v0
/*  f172f30:	5420fff8 */ 	bnezl	$at,.L0f172f14
/*  f172f34:	8c690000 */ 	lw	$t1,0x0($v1)
/*  f172f38:	00001025 */ 	or	$v0,$zero,$zero
.L0f172f3c:
/*  f172f3c:	03e00008 */ 	jr	$ra
/*  f172f40:	00000000 */ 	nop
);

s32 func0f172f44(struct texturething *arg0)
{
	return (u32)arg0->unk0c - (u32)arg0->unk08;
}

s32 func0f172f54(s32 *arg0)
{
	return arg0[2];
}

void texLoadFromDisplayList(Gfx *gdl, struct texturething *arg1, s32 arg2)
{
	u8 *bytes = (u8 *)gdl;

	while (bytes[0] != (u8)G_ENDDL) {
		// Look for GBI sequence: fd...... abcd....
		if (bytes[0] == G_SETTIMG && bytes[4] == 0xab && bytes[5] == 0xcd) {
			texLoad((u32 *)((s32)bytes + 4), arg1, arg2);
		}

		bytes += 8;
	}
}

GLOBAL_ASM(
glabel texLoad
/*  f173010:	27bddb10 */ 	addiu	$sp,$sp,-9456
/*  f173014:	afb00020 */ 	sw	$s0,0x20($sp)
/*  f173018:	00a08025 */ 	or	$s0,$a1,$zero
/*  f17301c:	afbf0024 */ 	sw	$ra,0x24($sp)
/*  f173020:	afa424f0 */ 	sw	$a0,0x24f0($sp)
/*  f173024:	afa624f8 */ 	sw	$a2,0x24f8($sp)
/*  f173028:	14a00003 */ 	bnez	$a1,.L0f173038
/*  f17302c:	00003825 */ 	or	$a3,$zero,$zero
/*  f173030:	3c10800b */ 	lui	$s0,%hi(var800aabc8)
/*  f173034:	2610abc8 */ 	addiu	$s0,$s0,%lo(var800aabc8)
.L0f173038:
/*  f173038:	3c0e800b */ 	lui	$t6,%hi(var800aabc8)
/*  f17303c:	25ceabc8 */ 	addiu	$t6,$t6,%lo(var800aabc8)
/*  f173040:	160e0002 */ 	bne	$s0,$t6,.L0f17304c
/*  f173044:	8faf24f0 */ 	lw	$t7,0x24f0($sp)
/*  f173048:	24070001 */ 	addiu	$a3,$zero,0x1
.L0f17304c:
/*  f17304c:	8de20000 */ 	lw	$v0,0x0($t7)
/*  f173050:	3c01ffff */ 	lui	$at,0xffff
/*  f173054:	02002825 */ 	or	$a1,$s0,$zero
/*  f173058:	00411824 */ 	and	$v1,$v0,$at
/*  f17305c:	10600002 */ 	beqz	$v1,.L0f173068
/*  f173060:	3c01abcd */ 	lui	$at,0xabcd
/*  f173064:	146100ee */ 	bne	$v1,$at,.L0f173420
.L0f173068:
/*  f173068:	3c03800b */ 	lui	$v1,%hi(var800ab53c)
/*  f17306c:	2463b53c */ 	addiu	$v1,$v1,%lo(var800ab53c)
/*  f173070:	3044ffff */ 	andi	$a0,$v0,0xffff
/*  f173074:	ac640000 */ 	sw	$a0,0x0($v1)
/*  f173078:	0fc5cba3 */ 	jal	func0f172e8c
/*  f17307c:	a3a7148b */ 	sb	$a3,0x148b($sp)
/*  f173080:	144000e2 */ 	bnez	$v0,.L0f17340c
/*  f173084:	afa2149c */ 	sw	$v0,0x149c($sp)
/*  f173088:	3c19800b */ 	lui	$t9,%hi(var800ab53c)
/*  f17308c:	8f39b53c */ 	lw	$t9,%lo(var800ab53c)($t9)
/*  f173090:	27a314bf */ 	addiu	$v1,$sp,0x14bf
/*  f173094:	00035102 */ 	srl	$t2,$v1,0x4
/*  f173098:	2b210daf */ 	slti	$at,$t9,0xdaf
/*  f17309c:	102000e0 */ 	beqz	$at,.L0f173420
/*  f1730a0:	000a5900 */ 	sll	$t3,$t2,0x4
/*  f1730a4:	0c012048 */ 	jal	osWritebackDCacheAll
/*  f1730a8:	afab002c */ 	sw	$t3,0x2c($sp)
/*  f1730ac:	8fa4002c */ 	lw	$a0,0x2c($sp)
/*  f1730b0:	0c013920 */ 	jal	osInvalDCache
/*  f1730b4:	24052000 */ 	addiu	$a1,$zero,0x2000
/*  f1730b8:	3c0d800b */ 	lui	$t5,%hi(var800ab53c)
/*  f1730bc:	8dadb53c */ 	lw	$t5,%lo(var800ab53c)($t5)
/*  f1730c0:	3c0c800b */ 	lui	$t4,%hi(g_Textures)
/*  f1730c4:	8d8cabc0 */ 	lw	$t4,%lo(g_Textures)($t4)
/*  f1730c8:	000d70c0 */ 	sll	$t6,$t5,0x3
/*  f1730cc:	3c0100ff */ 	lui	$at,0xff
/*  f1730d0:	018e1021 */ 	addu	$v0,$t4,$t6
/*  f1730d4:	8c430000 */ 	lw	$v1,0x0($v0)
/*  f1730d8:	8c470008 */ 	lw	$a3,0x8($v0)
/*  f1730dc:	3421ffff */ 	ori	$at,$at,0xffff
/*  f1730e0:	00617824 */ 	and	$t7,$v1,$at
/*  f1730e4:	00e1c024 */ 	and	$t8,$a3,$at
/*  f1730e8:	11f800cd */ 	beq	$t7,$t8,.L0f173420
/*  f1730ec:	8fa4002c */ 	lw	$a0,0x2c($sp)
/*  f1730f0:	030f3023 */ 	subu	$a2,$t8,$t7
/*  f1730f4:	24c6001f */ 	addiu	$a2,$a2,0x1f
/*  f1730f8:	2401fff8 */ 	addiu	$at,$zero,-8
/*  f1730fc:	3c0a01d6 */ 	lui	$t2,%hi(_texturesdataSegmentRomStart)
/*  f173100:	254a5f40 */ 	addiu	$t2,$t2,%lo(_texturesdataSegmentRomStart)
/*  f173104:	01e1c824 */ 	and	$t9,$t7,$at
/*  f173108:	00065902 */ 	srl	$t3,$a2,0x4
/*  f17310c:	000b3100 */ 	sll	$a2,$t3,0x4
/*  f173110:	032a2821 */ 	addu	$a1,$t9,$t2
/*  f173114:	0c003504 */ 	jal	dmaExec
/*  f173118:	afaf0044 */ 	sw	$t7,0x44($sp)
/*  f17311c:	8fa30044 */ 	lw	$v1,0x44($sp)
/*  f173120:	8fac002c */ 	lw	$t4,0x2c($sp)
/*  f173124:	93aa148b */ 	lbu	$t2,0x148b($sp)
/*  f173128:	306e0007 */ 	andi	$t6,$v1,0x7
/*  f17312c:	018e3021 */ 	addu	$a2,$t4,$t6
/*  f173130:	90c20000 */ 	lbu	$v0,0x0($a2)
/*  f173134:	00002825 */ 	or	$a1,$zero,$zero
/*  f173138:	02002025 */ 	or	$a0,$s0,$zero
/*  f17313c:	304f0080 */ 	andi	$t7,$v0,0x80
/*  f173140:	30490040 */ 	andi	$t1,$v0,0x40
/*  f173144:	3047003f */ 	andi	$a3,$v0,0x3f
/*  f173148:	000fc1c3 */ 	sra	$t8,$t7,0x7
/*  f17314c:	0009c983 */ 	sra	$t9,$t1,0x6
/*  f173150:	28e10006 */ 	slti	$at,$a3,0x6
/*  f173154:	afb814a8 */ 	sw	$t8,0x14a8($sp)
/*  f173158:	14200002 */ 	bnez	$at,.L0f173164
/*  f17315c:	03204825 */ 	or	$t1,$t9,$zero
/*  f173160:	24070005 */ 	addiu	$a3,$zero,0x5
.L0f173164:
/*  f173164:	1140000f */ 	beqz	$t2,.L0f1731a4
/*  f173168:	24c60001 */ 	addiu	$a2,$a2,0x1
/*  f17316c:	24040004 */ 	addiu	$a0,$zero,0x4
/*  f173170:	afa614ac */ 	sw	$a2,0x14ac($sp)
/*  f173174:	afa714a0 */ 	sw	$a3,0x14a0($sp)
/*  f173178:	0c004935 */ 	jal	mempGetPoolFree
/*  f17317c:	afa914a4 */ 	sw	$t1,0x14a4($sp)
/*  f173180:	afa20030 */ 	sw	$v0,0x30($sp)
/*  f173184:	24040004 */ 	addiu	$a0,$zero,0x4
/*  f173188:	0c004935 */ 	jal	mempGetPoolFree
/*  f17318c:	24050001 */ 	addiu	$a1,$zero,0x1
/*  f173190:	8fab0030 */ 	lw	$t3,0x30($sp)
/*  f173194:	8fa714a0 */ 	lw	$a3,0x14a0($sp)
/*  f173198:	8fa914a4 */ 	lw	$t1,0x14a4($sp)
/*  f17319c:	10000008 */ 	b	.L0f1731c0
/*  f1731a0:	004b1821 */ 	addu	$v1,$v0,$t3
.L0f1731a4:
/*  f1731a4:	afa614ac */ 	sw	$a2,0x14ac($sp)
/*  f1731a8:	afa714a0 */ 	sw	$a3,0x14a0($sp)
/*  f1731ac:	0fc5cbd1 */ 	jal	func0f172f44
/*  f1731b0:	afa914a4 */ 	sw	$t1,0x14a4($sp)
/*  f1731b4:	8fa714a0 */ 	lw	$a3,0x14a0($sp)
/*  f1731b8:	8fa914a4 */ 	lw	$t1,0x14a4($sp)
/*  f1731bc:	00401825 */ 	or	$v1,$v0,$zero
.L0f1731c0:
/*  f1731c0:	15200003 */ 	bnez	$t1,.L0f1731d0
/*  f1731c4:	2c6110cc */ 	sltiu	$at,$v1,0x10cc
/*  f1731c8:	14200005 */ 	bnez	$at,.L0f1731e0
/*  f1731cc:	00000000 */ 	nop
.L0f1731d0:
/*  f1731d0:	11200008 */ 	beqz	$t1,.L0f1731f4
/*  f1731d4:	2c610a28 */ 	sltiu	$at,$v1,0xa28
/*  f1731d8:	50200007 */ 	beqzl	$at,.L0f1731f8
/*  f1731dc:	93ac148b */ 	lbu	$t4,0x148b($sp)
.L0f1731e0:
/*  f1731e0:	0c012d20 */ 	jal	osVirtualToPhysical
/*  f1731e4:	8e040000 */ 	lw	$a0,0x0($s0)
/*  f1731e8:	8fad24f0 */ 	lw	$t5,0x24f0($sp)
/*  f1731ec:	1000008c */ 	b	.L0f173420
/*  f1731f0:	ada20000 */ 	sw	$v0,0x0($t5)
.L0f1731f4:
/*  f1731f4:	93ac148b */ 	lbu	$t4,0x148b($sp)
.L0f1731f8:
/*  f1731f8:	27a20057 */ 	addiu	$v0,$sp,0x57
/*  f1731fc:	00027102 */ 	srl	$t6,$v0,0x4
/*  f173200:	11800015 */ 	beqz	$t4,.L0f173258
/*  f173204:	3c0b800b */ 	lui	$t3,%hi(var800ab53c)
/*  f173208:	8e03000c */ 	lw	$v1,0xc($s0)
/*  f17320c:	000e7900 */ 	sll	$t7,$t6,0x4
/*  f173210:	25e20010 */ 	addiu	$v0,$t7,0x10
/*  f173214:	24580010 */ 	addiu	$t8,$v0,0x10
/*  f173218:	ae02000c */ 	sw	$v0,0xc($s0)
/*  f17321c:	ae180008 */ 	sw	$t8,0x8($s0)
/*  f173220:	1060000d */ 	beqz	$v1,.L0f173258
/*  f173224:	afa31490 */ 	sw	$v1,0x1490($sp)
/*  f173228:	3c048000 */ 	lui	$a0,0x8000
/*  f17322c:	8c62000c */ 	lw	$v0,0xc($v1)
.L0f173230:
/*  f173230:	0002c900 */ 	sll	$t9,$v0,0x4
/*  f173234:	00191202 */ 	srl	$v0,$t9,0x8
/*  f173238:	54400004 */ 	bnezl	$v0,.L0f17324c
/*  f17323c:	00441825 */ 	or	$v1,$v0,$a0
/*  f173240:	10000005 */ 	b	.L0f173258
/*  f173244:	afa31490 */ 	sw	$v1,0x1490($sp)
/*  f173248:	00441825 */ 	or	$v1,$v0,$a0
.L0f17324c:
/*  f17324c:	5460fff8 */ 	bnezl	$v1,.L0f173230
/*  f173250:	8c62000c */ 	lw	$v0,0xc($v1)
/*  f173254:	afa31490 */ 	sw	$v1,0x1490($sp)
.L0f173258:
/*  f173258:	8e020008 */ 	lw	$v0,0x8($s0)
/*  f17325c:	8d6bb53c */ 	lw	$t3,%lo(var800ab53c)($t3)
/*  f173260:	8fa31490 */ 	lw	$v1,0x1490($sp)
/*  f173264:	3c18800b */ 	lui	$t8,%hi(var800ab53c)
/*  f173268:	a44b0000 */ 	sh	$t3,0x0($v0)
/*  f17326c:	8e0d0008 */ 	lw	$t5,0x8($s0)
/*  f173270:	8e0e000c */ 	lw	$t6,0xc($s0)
/*  f173274:	25ac0008 */ 	addiu	$t4,$t5,0x8
/*  f173278:	25c8fff0 */ 	addiu	$t0,$t6,-16
/*  f17327c:	ae0c0008 */ 	sw	$t4,0x8($s0)
/*  f173280:	ae08000c */ 	sw	$t0,0xc($s0)
/*  f173284:	950d0000 */ 	lhu	$t5,0x0($t0)
/*  f173288:	8f19b53c */ 	lw	$t9,%lo(var800ab53c)($t8)
/*  f17328c:	9118000c */ 	lbu	$t8,0xc($t0)
/*  f173290:	31ac000f */ 	andi	$t4,$t5,0xf
/*  f173294:	00195900 */ 	sll	$t3,$t9,0x4
/*  f173298:	016c7025 */ 	or	$t6,$t3,$t4
/*  f17329c:	a50e0000 */ 	sh	$t6,0x0($t0)
/*  f1732a0:	8e0f0008 */ 	lw	$t7,0x8($s0)
/*  f1732a4:	3319ffef */ 	andi	$t9,$t8,0xffef
/*  f1732a8:	a119000c */ 	sb	$t9,0xc($t0)
/*  f1732ac:	1120000d */ 	beqz	$t1,.L0f1732e4
/*  f1732b0:	ad0f0004 */ 	sw	$t7,0x4($t0)
/*  f1732b4:	8e050008 */ 	lw	$a1,0x8($s0)
/*  f1732b8:	8faa24f8 */ 	lw	$t2,0x24f8($sp)
/*  f1732bc:	afa8149c */ 	sw	$t0,0x149c($sp)
/*  f1732c0:	afa31490 */ 	sw	$v1,0x1490($sp)
/*  f1732c4:	afb00010 */ 	sw	$s0,0x10($sp)
/*  f1732c8:	8fa414ac */ 	lw	$a0,0x14ac($sp)
/*  f1732cc:	8fa614a8 */ 	lw	$a2,0x14a8($sp)
/*  f1732d0:	0fc5ba06 */ 	jal	texInflateZlib
/*  f1732d4:	afaa0014 */ 	sw	$t2,0x14($sp)
/*  f1732d8:	8fa31490 */ 	lw	$v1,0x1490($sp)
/*  f1732dc:	1000000c */ 	b	.L0f173310
/*  f1732e0:	afa20038 */ 	sw	$v0,0x38($sp)
.L0f1732e4:
/*  f1732e4:	8e050008 */ 	lw	$a1,0x8($s0)
/*  f1732e8:	8fad24f8 */ 	lw	$t5,0x24f8($sp)
/*  f1732ec:	afa8149c */ 	sw	$t0,0x149c($sp)
/*  f1732f0:	afa31490 */ 	sw	$v1,0x1490($sp)
/*  f1732f4:	afb00010 */ 	sw	$s0,0x10($sp)
/*  f1732f8:	8fa414ac */ 	lw	$a0,0x14ac($sp)
/*  f1732fc:	8fa614a8 */ 	lw	$a2,0x14a8($sp)
/*  f173300:	0fc5bf79 */ 	jal	texInflateNonZlib
/*  f173304:	afad0014 */ 	sw	$t5,0x14($sp)
/*  f173308:	8fa31490 */ 	lw	$v1,0x1490($sp)
/*  f17330c:	afa20038 */ 	sw	$v0,0x38($sp)
.L0f173310:
/*  f173310:	93ab148b */ 	lbu	$t3,0x148b($sp)
/*  f173314:	8fa40038 */ 	lw	$a0,0x38($sp)
/*  f173318:	24050004 */ 	addiu	$a1,$zero,0x4
/*  f17331c:	11600032 */ 	beqz	$t3,.L0f1733e8
/*  f173320:	2484002f */ 	addiu	$a0,$a0,0x2f
/*  f173324:	348c000f */ 	ori	$t4,$a0,0xf
/*  f173328:	3984000f */ 	xori	$a0,$t4,0xf
/*  f17332c:	0c004991 */ 	jal	mempAllocFromRight
/*  f173330:	afa31490 */ 	sw	$v1,0x1490($sp)
/*  f173334:	ae02000c */ 	sw	$v0,0xc($s0)
/*  f173338:	afa20034 */ 	sw	$v0,0x34($sp)
/*  f17333c:	8fa4149c */ 	lw	$a0,0x149c($sp)
/*  f173340:	00402825 */ 	or	$a1,$v0,$zero
/*  f173344:	0c012c5c */ 	jal	bcopy
/*  f173348:	24060010 */ 	addiu	$a2,$zero,0x10
/*  f17334c:	8fa70034 */ 	lw	$a3,0x34($sp)
/*  f173350:	8fa60038 */ 	lw	$a2,0x38($sp)
/*  f173354:	afa7149c */ 	sw	$a3,0x149c($sp)
/*  f173358:	8e040008 */ 	lw	$a0,0x8($s0)
/*  f17335c:	24e50010 */ 	addiu	$a1,$a3,0x10
/*  f173360:	afa50034 */ 	sw	$a1,0x34($sp)
/*  f173364:	24c60008 */ 	addiu	$a2,$a2,0x8
/*  f173368:	0c012c5c */ 	jal	bcopy
/*  f17336c:	2484fff8 */ 	addiu	$a0,$a0,-8
/*  f173370:	8fa70034 */ 	lw	$a3,0x34($sp)
/*  f173374:	8e18000c */ 	lw	$t8,0xc($s0)
/*  f173378:	8fa31490 */ 	lw	$v1,0x1490($sp)
/*  f17337c:	24ef0008 */ 	addiu	$t7,$a3,0x8
/*  f173380:	af0f0004 */ 	sw	$t7,0x4($t8)
/*  f173384:	8e02000c */ 	lw	$v0,0xc($s0)
/*  f173388:	3c0100ff */ 	lui	$at,0xff
/*  f17338c:	8c44000c */ 	lw	$a0,0xc($v0)
/*  f173390:	0004c902 */ 	srl	$t9,$a0,0x4
/*  f173394:	00195200 */ 	sll	$t2,$t9,0x8
/*  f173398:	000a6902 */ 	srl	$t5,$t2,0x4
/*  f17339c:	01a45826 */ 	xor	$t3,$t5,$a0
/*  f1733a0:	1060000e */ 	beqz	$v1,.L0f1733dc
/*  f1733a4:	ac4b000c */ 	sw	$t3,0xc($v0)
/*  f1733a8:	8e0c000c */ 	lw	$t4,0xc($s0)
/*  f1733ac:	8c64000c */ 	lw	$a0,0xc($v1)
/*  f1733b0:	3421ffff */ 	ori	$at,$at,0xffff
/*  f1733b4:	01817024 */ 	and	$t6,$t4,$at
/*  f1733b8:	01c17824 */ 	and	$t7,$t6,$at
/*  f1733bc:	0004c102 */ 	srl	$t8,$a0,0x4
/*  f1733c0:	01f8c826 */ 	xor	$t9,$t7,$t8
/*  f1733c4:	00195200 */ 	sll	$t2,$t9,0x8
/*  f1733c8:	000a6902 */ 	srl	$t5,$t2,0x4
/*  f1733cc:	01a45826 */ 	xor	$t3,$t5,$a0
/*  f1733d0:	ac6b000c */ 	sw	$t3,0xc($v1)
/*  f1733d4:	10000003 */ 	b	.L0f1733e4
/*  f1733d8:	8e02000c */ 	lw	$v0,0xc($s0)
.L0f1733dc:
/*  f1733dc:	8e02000c */ 	lw	$v0,0xc($s0)
/*  f1733e0:	ae020004 */ 	sw	$v0,0x4($s0)
.L0f1733e4:
/*  f1733e4:	ae020000 */ 	sw	$v0,0x0($s0)
.L0f1733e8:
/*  f1733e8:	8e0c0008 */ 	lw	$t4,0x8($s0)
/*  f1733ec:	8fae0038 */ 	lw	$t6,0x38($sp)
/*  f1733f0:	018e7821 */ 	addu	$t7,$t4,$t6
/*  f1733f4:	ae0f0008 */ 	sw	$t7,0x8($s0)
/*  f1733f8:	93b8148b */ 	lbu	$t8,0x148b($sp)
/*  f1733fc:	57000004 */ 	bnezl	$t8,.L0f173410
/*  f173400:	8fb9149c */ 	lw	$t9,0x149c($sp)
/*  f173404:	0fc5cbd1 */ 	jal	func0f172f44
/*  f173408:	02002025 */ 	or	$a0,$s0,$zero
.L0f17340c:
/*  f17340c:	8fb9149c */ 	lw	$t9,0x149c($sp)
.L0f173410:
/*  f173410:	0c012d20 */ 	jal	osVirtualToPhysical
/*  f173414:	8f240004 */ 	lw	$a0,0x4($t9)
/*  f173418:	8faa24f0 */ 	lw	$t2,0x24f0($sp)
/*  f17341c:	ad420000 */ 	sw	$v0,0x0($t2)
.L0f173420:
/*  f173420:	8fbf0024 */ 	lw	$ra,0x24($sp)
/*  f173424:	8fb00020 */ 	lw	$s0,0x20($sp)
/*  f173428:	27bd24f0 */ 	addiu	$sp,$sp,0x24f0
/*  f17342c:	03e00008 */ 	jr	$ra
/*  f173430:	00000000 */ 	nop
);

//extern u8 _texturesdataSegmentRomStart;

/**
 * Load and decompress a texture from ROM.
 *
 * The given pointer points to a word which determines what to load.
 * The formats of the word are:
 *
 *     abcdxxxx -> load texture number xxxx
 *     0000xxxx -> load texture number xxxx
 *     (memory address) -> the texture is already loaded, so do nothing
 *
 * After loading and decompressing the texture, the value that's pointed to is
 * changed to be a pointer to... something.
 *
 * There are two types of textures:
 *
 * - Zlib-compressed textures, which are always paletted
 * - Non-zlib textures, which use a variety of (non-zlib) compression methods
 *   and are sometimes paletted
 *
 * Both types have support for multiple levels of detail (ie. multiple images
 * of varying size) within each texture. There are enough bits in the header
 * byte to support 64 levels of detail, but this function caps it to 5. Some
 * textures actually specify up to 7 levels of detail. However testing suggests
 * that the additional levels of detail are not even read.
 *
 * This function reads the above information from the first byte of texture data,
 * then calls the texInflateZlib or texInflateNonZlib to inflate the images.
 *
 * The format of the first byte is:
 * uzllllll
 *
 * u = unknown
 * z = texture is compressed with zlib
 * l = number of levels of detail within the texture
 */
// Mismatch: Reordered instructions and regalloc. Is functionally identical.
//void texLoad(u32 *ptr, struct texturething *arg1, s32 arg2)
//{
//	s32 sp14b0[1040];
//	u8 *sp14ac_ptr;
//	s32 sp14a8;
//	s32 sp14a4_iszlib;
//	s32 sp14a0_lod;
//	struct texloadthing *sp149c;
//	u32 freebytes;
//	u32 stack;
//	struct texloadthing *sp1490;
//	u32 stack2;
//	u8 sp148b_usingsharedstruct;
//	s8 sp48[5187];
//	s32 sp44;
//	s32 sp38;
//	struct texloadthing *sp34;
//	s32 sp30;
//	u8 *sp2c;
//
//	sp148b_usingsharedstruct = 0;
//
//	if (arg1 == NULL) {
//		arg1 = &var800aabc8;
//	}
//
//	if (arg1 == &var800aabc8) {
//		sp148b_usingsharedstruct = 1;
//	}
//
//	if ((*ptr & 0xffff0000) == 0 || (*ptr & 0xffff0000) == 0xabcd0000) {
//		var800ab53c = *ptr & 0xffff;
//
//		sp149c = func0f172e8c(var800ab53c, arg1);
//
//		if (sp149c == NULL && var800ab53c < 0xdaf) {
//			sp2c = (void *)(((u32)sp14b0 + 0xf) >> 4 << 4);
//
//			if (sp2c);
//
//			osWritebackDCacheAll();
//			osInvalDCache(sp2c, DCACHE_SIZE);
//
//			if (g_Textures[var800ab53c].dataoffset == g_Textures[var800ab53c + 1].dataoffset) {
//				return;
//			}
//
//			sp44 = g_Textures[var800ab53c].dataoffset;
//
//			dmaExec(sp2c,
//					(u32)&_texturesdataSegmentRomStart + (sp44 & 0xfffffff8),
//					(((g_Textures[var800ab53c + 1].dataoffset - sp44) + 0x1f) >> 4) * 0x10);
//
//			if (sp148b_usingsharedstruct);
//			sp14ac_ptr = sp2c + (sp44 & 7);
//			sp14a8 = (*sp14ac_ptr & 0x80) >> 7;
//			sp14a4_iszlib = (*sp14ac_ptr & 0x40) >> 6;
//			sp14a0_lod = *sp14ac_ptr & 0x3f;
//
//			if (sp14a0_lod > 5) {
//				sp14a0_lod = 5;
//			}
//
//			sp14ac_ptr++;
//
//			if (sp148b_usingsharedstruct) {
//				freebytes = mempGetPoolFree(MEMPOOL_STAGE, MEMBANK_ONBOARD) + mempGetPoolFree(MEMPOOL_STAGE, MEMBANK_EXPANSION);
//			} else {
//				freebytes = func0f172f44(arg1);
//			}
//
//			if ((sp14a4_iszlib == 0 && freebytes < 4300) || (sp14a4_iszlib != 0 && freebytes < 2600)) {
//				*ptr = osVirtualToPhysical(arg1->unk00);
//				return;
//			}
//
//			if (sp148b_usingsharedstruct) {
//				sp1490 = arg1->unk0c;
//				arg1->unk0c = (struct texloadthing *)((((u32)sp48 + 0xf) >> 4 << 4) + 0x10);
//				arg1->unk08 = arg1->unk0c + 1;
//
//				while (sp1490) {
//					if (sp1490->unk0c_04 == 0) {
//						break;
//					}
//
//					sp1490 = (struct texloadthing *)PHYS_TO_K0(sp1490->unk0c_04);
//				}
//			}
//
//			*(s16 *)(arg1->unk08) = var800ab53c;
//			arg1->unk08 = (void *)((u32)arg1->unk08 + 8);
//			sp149c = (struct texloadthing *)(arg1->unk0c - 1);
//			arg1->unk0c = sp149c;
//			sp149c->texturenum = var800ab53c;
//			sp149c->unk04 = arg1->unk08;
//			sp149c->unk0c_03 = false;
//
//			if (sp14a4_iszlib) {
//				sp38 = texInflateZlib(sp14ac_ptr, (u32 *)arg1->unk08, sp14a8, sp14a0_lod, arg1, arg2);
//			} else {
//				sp38 = texInflateNonZlib(sp14ac_ptr, (u32 *)arg1->unk08, sp14a8, sp14a0_lod, arg1, arg2);
//			}
//
//			if (sp148b_usingsharedstruct) {
//				sp34 = mempAllocFromRight(ALIGN16(sp38 + 0x20), MEMPOOL_STAGE);
//				arg1->unk0c = sp34;
//
//				bcopy(sp149c, sp34, 0x10);
//
//				sp149c = sp34;
//				sp34++;
//
//				bcopy((void *)((u32)arg1->unk08 - 8), sp34, sp38 + 8);
//
//				arg1->unk0c->unk04 = (void *)((u32)sp34 + 8);
//				arg1->unk0c->unk0c_04 = 0;
//
//				if (sp1490 != NULL) {
//					sp1490->unk0c_04 = (u32)arg1->unk0c & 0xffffff;
//				} else {
//					arg1->unk04 = arg1->unk0c;
//				}
//
//				arg1->unk00 = arg1->unk0c;
//			}
//
//			arg1->unk08 = (void *)((u32)arg1->unk08 + sp38);
//
//			if (!sp148b_usingsharedstruct) {
//				func0f172f44(arg1);
//			}
//		}
//	}
//
//	*ptr = osVirtualToPhysical(sp149c->unk04);
//}

void texLoadFromConfigs(struct textureconfig *configs, s32 numconfigs, struct texturething *arg2, s32 arg3)
{
	s32 i;

	for (i = 0; i < numconfigs; i++) {
		if ((s32)configs[i].texturenum < 0xdaf) {
			texLoad(&configs[i].texturenum, arg2, 1);
			configs[i].unk0b = 1;
		} else {
			configs[i].texturenum += arg3;
		}
	}
}

void texLoadFromTextureNum(u32 arg0, struct texturething *arg1)
{
	u32 sp1c = arg0;

	texLoad(&sp1c, arg1, 1);
}

s32 func0f173510(s32 arg0, s32 arg1, s32 arg3)
{
	return arg0;
}
