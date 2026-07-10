#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 *
 * Globals it touches, as the original declared them:
 *     extern struct GsOT *OTablePt;
 * END PSX.SYM */

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/FUN_80039fb0", FUN_80039fb0);

// triage: MEDIUM — 102 insns, mul/div, 2 callees, ~0.06 to AdtFntOpen
// likely-relevant cookbook sections:
//   - Dispatch: if/switch ladder — reload vs CSE, signed vs unsigned
//   - Expressions: mult/div — magic-multiply constants, fold

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void FUN_80039fb0(GsSPRITE *param_1,GsSPRITE *param_2,undefined4 param_3,undefined4 param_4,
//                  undefined4 param_5,int param_6,long param_7,int param_8)
//
// {
//   uchar uVar1;
//   short sVar2;
//   int iVar3;
//   int iVar4;
//   short local_20;
//   short local_1e;
//   ushort local_1c;
//
//   FUN_800396c0(param_3,param_4,param_5,&local_20);
//   iVar3 = (int)(short)local_1c;
//   if (0x24 < iVar3) {
//     if (iVar3 == 0) {
//       trap(0x1c00);
//     }
//     if ((iVar3 == -1) && (param_6 * 300 == -0x80000000)) {
//       trap(0x1800);
//     }
//     sVar2 = (short)((param_6 * 300) / iVar3) + 1;
//     param_2->scaley = sVar2;
//     param_2->scalex = sVar2;
//     param_1->scaley = sVar2;
//     param_1->scalex = sVar2;
//     param_2->rotate = param_7;
//     param_1->rotate = param_7;
//     param_2->x = local_20;
//     param_1->x = local_20;
//     param_2->y = local_1e;
//     param_1->y = local_1e;
//     uVar1 = (uchar)param_8;
//     param_2->r = uVar1;
//     param_2->g = uVar1;
//     param_2->b = uVar1;
//     uVar1 = (uchar)(param_8 / 2);
//     param_1->r = uVar1;
//     param_1->g = uVar1;
//     param_1->b = uVar1;
//     iVar3 = (int)((uint)local_1c << 0x10) >> 0x12;
//     if (iVar3 < 0) {
//       iVar4 = 0;
//     }
//     else {
//       iVar4 = 0x4e1;
//       if (iVar3 < 0x4e2) {
//         iVar4 = iVar3;
//       }
//     }
//     GsSortSprite(param_2,OTablePt,(ushort)iVar4);
//     iVar3 = (int)((uint)local_1c << 0x10) >> 0x12;
//     if (iVar3 < 0) {
//       iVar4 = 0;
//     }
//     else {
//       iVar4 = 0x4e1;
//       if (iVar3 < 0x4e2) {
//         iVar4 = iVar3;
//       }
//     }
//     GsSortSprite(param_1,OTablePt,(ushort)iVar4);
//   }
//   return;
// }
