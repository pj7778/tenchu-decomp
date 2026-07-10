#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 *
 * Globals it touches, as the original declared them:
 *     extern struct TCameraStatus CamState;
 *     extern struct tag_TItem items[30];
 *
 * PSX.SYM suggests this may be `RequestItem` (LOW confidence, ITEM.C) — NOT
 * adopted. Corroborate with `tools/callmatch.py --verify` before renaming.
 * END PSX.SYM */

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/FUN_8003d768", FUN_8003d768);

// triage: MEDIUM — 168 insns, mul/div, 3 callees, ~0.09 to GetVectorRotation
// likely-relevant cookbook sections:
//   - Dispatch: if/switch ladder — reload vs CSE, signed vs unsigned
//   - Expressions: mult/div — magic-multiply constants, fold

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void FUN_8003d768(int param_1,int param_2,int *param_3)
//
// {
//   int iVar1;
//   int iVar2;
//   int iVar3;
//   int iVar4;
//   tag_TItem *ptVar5;
//   int iVar6;
//   int iVar7;
//   int iVar8;
//
//   iVar6 = *param_3;
//   if (iVar6 < 1) {
//     iVar6 = 1;
//   }
//   iVar1 = rsin(param_3[1]);
//   iVar2 = rcos(param_3[1]);
//   if (iVar6 == 0) {
//     trap(0x1c00);
//   }
//   if ((iVar6 == -1) && (param_1 == -0x80000000)) {
//     trap(0x1800);
//   }
//   if (iVar6 == 0) {
//     trap(0x1c00);
//   }
//   if ((iVar6 == -1) && (param_2 == -0x80000000)) {
//     trap(0x1800);
//   }
//   iVar3 = (param_1 / iVar6) * iVar2 + (param_2 / iVar6) * iVar1;
//   if (iVar3 < 0) {
//     iVar3 = iVar3 + 0xfff;
//   }
//   iVar4 = (param_1 / iVar6) * iVar1 - (param_2 / iVar6) * iVar2;
//   if (iVar4 < 0) {
//     iVar4 = iVar4 + 0xfff;
//   }
//   DrawTargetS((iVar3 >> 0xc) + param_3[2],(iVar4 >> 0xc) + param_3[3],0,0xc81414);
//   ptVar5 = items;
//   for (iVar3 = 0; iVar3 < 0x1e; iVar3 = iVar3 + 1) {
//     if (((ptVar5->proc != (undefined **)0x0) && (ptVar5->type == ITEM_GOSHIKIMAI)) &&
//        (ptVar5->owner == CamState.Owner)) {
//       iVar4 = (ptVar5->locate->locate).coord.t[0];
//       iVar7 = iVar4 / iVar6;
//       if (iVar6 == 0) {
//         trap(0x1c00);
//       }
//       if ((iVar6 == -1) && (iVar4 == -0x80000000)) {
//         trap(0x1800);
//       }
//       iVar4 = (ptVar5->locate->locate).coord.t[2];
//       iVar8 = iVar4 / iVar6;
//       if (iVar6 == 0) {
//         trap(0x1c00);
//       }
//       if ((iVar6 == -1) && (iVar4 == -0x80000000)) {
//         trap(0x1800);
//       }
//       iVar4 = iVar7 * iVar2 + iVar8 * iVar1;
//       if (iVar4 < 0) {
//         iVar4 = iVar4 + 0xfff;
//       }
//       iVar7 = iVar7 * iVar1 - iVar8 * iVar2;
//       if (iVar7 < 0) {
//         iVar7 = iVar7 + 0xfff;
//       }
//       DrawTargetS((iVar4 >> 0xc) + param_3[2],(iVar7 >> 0xc) + param_3[3],0,0x1414c8);
//     }
//     ptVar5 = ptVar5 + 1;
//   }
//   return;
// }
