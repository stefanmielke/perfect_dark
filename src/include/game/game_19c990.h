#ifndef IN_GAME_GAME_19C990_H
#define IN_GAME_GAME_19C990_H
#include <ultra64.h>
#include "types.h"

extern u8 var80088804;
extern s32 g_FrWeaponNum;
extern u8 var800888a0;

bool ciIsTourDone(void);
u8 ciGetFiringRangeScore(s32 weapon_id);
u32 func0f19c9e4(void);
u32 frIsWeaponFound(s32 weapon);
void frSetWeaponFound(s32 weaponnum);
s32 stageIsComplete(s32 stageindex);
bool func0f19cbcc(s32 weapon);
bool frWeaponIsAvailable(s32 weapon);
u32 func0f19ccc0(u32 weaponnum);
u32 func0f19cdf0(u32 weaponnum);
s32 frIsClassicWeaponUnlocked(u32 weapon);
s32 frGetSlot(void);
void frSetSlot(s32 slot);
u32 frGetWeaponBySlot(s32 slot);
u32 func0f19d2e0(void);
u32 func0f19d338(void);
u32 func0f19d414(void);
void func0f19d4ec(void);
void *func0f19d560(u32 len);
void frSetDifficulty(s32 difficulty);
u32 frGetDifficulty(void);
void func0f19d5f4(void);
struct frdata *getFiringRangeData(void);
u32 func0f19d7d8(void);
u32 func0f19d8a4(void);
u32 func0f19d90c(void);
void func0f19de24(void);
u32 func0f19def4(void);
u32 func0f19df18(void);
u32 func0f19df58(void);
u32 func0f19e090(void);
u32 func0f19e3e0(void);
void func0f19e44c(void);
void frCloseAndLockDoor(void);
void frUnlockDoor(void);
u32 func0f19e7a8(void);
u32 func0f19e900(s32 weapon);
void func0f19e9c0(s32 weapon);
char *frGetWeaponDescription(void);
void func0f19ecdc(s32 arg0);
bool frWasTooInaccurate(void);
void frSetFailReason(s32 failreason);
u32 func0f19f18c(void);
u32 func0f19f220(void);
u32 func0f19f294(void);
u32 func0f19f2ec(void);
u32 func0f19f39c(void);
u32 func0f19f524(void);
void frTick(void);
void func0f1a0924(struct prop *prop);
u32 func0f1a0a70(void);
s32 frIsInTraining(void);
u32 func0f1a0cc0(void);
void func0f1a0fc8(void);
bool ciIsChrBioUnlocked(u32 bodynum);
struct chrbio *ciGetChrBioByBodynum(u32 bodynum);
char *ciGetChrBioDescription(void);
u32 func0f1a11b8(void);
u32 func0f1a1210(u32 arg0);
struct miscbio *ciGetMiscBio(s32 index);
bool ciIsMiscBioUnlocked(s32 index);
s32 ciGetNumUnlockedMiscBios(void);
s32 ciGetMiscBioIndexBySlot(s32 slot);
char *ciGetMiscBioDescription(void);
bool ciIsLocationBioAVehicle(s32 index);
struct locationbio *ciGetLocationBio(s32 index);
bool ciIsLocationBioUnlocked(u32 bioindex);
u32 func0f1a16a4(void);
u32 func0f1a1714(void);
u32 func0f1a176c(void);
u32 func0f1a17e4(void);
struct trainingdata *getDeviceTrainingData(void);
void dtRestorePlayer(void);
void dtPushEndscreen(void);
void dtTick(void);
void func0f1a1ac0(void);
void dtBegin(void);
void dtEnd(void);
bool dtIsAvailable(s32 deviceindex);
s32 dtGetNumAvailable(void);
s32 func0f1a1d68(s32 wantindex);
u32 dtGetWeaponByDeviceIndex(s32 deviceindex);
u32 ciGetStageFlagByDeviceIndex(u32 deviceindex);
char *dtGetDescription(void);
char *dtGetTip1(void);
char *dtGetTip2(void);
struct trainingdata *getHoloTrainingData(void);
void htPushEndscreen(void);
void htTick(void);
void func0f1a2198(void);
void htBegin(void);
void htEnd(void);
bool func0f1a2450(u32 value);
u32 func0f1a2484(void);
u32 func0f1a24dc(u32 arg0);
char *htGetName(s32 index);
u32 func0f1a25c0(s32 index);
char *htGetDescription(void);
char *htGetTip1(void);
char *htGetTip2(void);
void frGetGoalTargetsText(char *buffer);
void frGetTargetsDestroyedValue(char *buffer);
void frGetScoreValue(char *buffer);
void frGetGoalScoreText(char *buffer);
f32 frGetAccuracy(char *buffer);
bool frGetMinAccuracy(char *buffer, f32 accuracy);
u32 func0f1a29b8(void);
bool frGetHudMiddleSubtext(char *buffer);
bool frGetFeedback(char *score, char *zone);
u32 func0f1a2d88(void);
u32 func0f1a2f60(void);

#endif
