#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/MakeDif", MakeDif);

// triage: EASY — 63 insns, 1 callees, ~0.12 to vcalloc

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void MakeDif(GsRVIEW2 *vinfo,GsRVIEW2 *target,GsRVIEW2 *vdif)
//
// {
//   if (CamState.OldMode._1_1_ == 1) {
//     vdif->vpx = target->vpx - vinfo->vpx;
//     vdif->vpy = target->vpy - vinfo->vpy;
//     vdif->vpz = target->vpz - vinfo->vpz;
//     vdif->vrx = target->vrx - vinfo->vrx;
//     vdif->vry = target->vry - vinfo->vry;
//     vdif->vrz = target->vrz - vinfo->vrz;
//     CamState.OldMode._1_1_ = CMODE_NORMAL >> 8;
//   }
//   else {
//     MakeDifSub((VECTOR *)&vinfo->vrx,(VECTOR *)&target->vrx,(VECTOR *)&vdif->vrx,
//                (TMakeDifInfo *)&CamState.Valiation);
//     MakeDifSub((VECTOR *)vinfo,(VECTOR *)target,(VECTOR *)vdif,&pnt);
//   }
//   return;
// }
