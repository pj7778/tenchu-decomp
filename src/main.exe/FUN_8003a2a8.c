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

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/FUN_8003a2a8", FUN_8003a2a8);

// triage: EASY — 62 insns, mul/div, 2 callees, ~0.07 to bow_shoot_logic
// likely-relevant cookbook sections:
//   - Dispatch: if/switch ladder — reload vs CSE, signed vs unsigned
//   - Expressions: mult/div — magic-multiply constants, fold

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void FUN_8003a2a8(GsSPRITE *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
//                  int param_5)
//
// {
//   short sVar1;
//   int iVar2;
//   int iVar3;
//   short local_18;
//   short local_16;
//   ushort local_14;
//
//   FUN_800396c0(param_2,param_3,param_4,&local_18);
//   iVar2 = (int)(short)local_14;
//   if (0x24 < iVar2) {
//     if (iVar2 == 0) {
//       trap(0x1c00);
//     }
//     if ((iVar2 == -1) && (param_5 * 300 == -0x80000000)) {
//       trap(0x1800);
//     }
//     sVar1 = (short)((param_5 * 300) / iVar2) + 1;
//     param_1->scaley = sVar1;
//     param_1->scalex = sVar1;
//     param_1->x = local_18;
//     param_1->y = local_16;
//     iVar2 = (int)((uint)local_14 << 0x10) >> 0x12;
//     if (iVar2 < 0) {
//       iVar3 = 0;
//     }
//     else {
//       iVar3 = 0x4e1;
//       if (iVar2 < 0x4e2) {
//         iVar3 = iVar2;
//       }
//     }
//     GsSortSprite(param_1,OTablePt,(ushort)iVar3);
//   }
//   return;
// }
