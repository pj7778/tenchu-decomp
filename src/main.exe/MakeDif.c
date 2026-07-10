#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * void MakeDif(struct GsRVIEW2 *vinfo, struct GsRVIEW2 *target, struct GsRVIEW2 *vdif);
 *     CAMERA.C:418, 8 src lines, frame 32 bytes, saved-reg mask 0x80070000
 *
 * Original parameters and locals (the demo build's register allocation may
 * differ from retail, but the COUNT and TYPES drive cc1's codegen and carry
 * over). A repeated name is a nested-block scope, not a duplicate:
 *     param $a0       struct GsRVIEW2 * vinfo
 *     param $a1       struct GsRVIEW2 * target
 *     param $a2       struct GsRVIEW2 * vdif
 *
 * Globals it touches, as the original declared them:
 *     extern struct TCameraStatus CamState;
 * END PSX.SYM */

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
