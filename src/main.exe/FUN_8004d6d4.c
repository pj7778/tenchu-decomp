#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 *
 * Globals it touches, as the original declared them:
 *     extern long GameClock;
 * END PSX.SYM */

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/FUN_8004d6d4", FUN_8004d6d4);

// triage: MEDIUM — 104 insns, mul/div, 3 callees, ~0.09 to bow_shoot_logic
// likely-relevant cookbook sections:
//   - Dispatch: if/switch ladder — reload vs CSE, signed vs unsigned
//   - Expressions: mult/div — magic-multiply constants, fold

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void FUN_8004d6d4(int param_1,uint param_2)
//
// {
//   int iVar1;
//   int iVar2;
//   int iVar3;
//   VECTOR local_28;
//   SVECTOR local_18;
//
//   if (param_2 == 0) {
//     *(undefined1 *)(param_1 + 0x15) = 0;
//   }
//   else if ((3 < param_2) && (*(char *)(param_1 + 0x15) == '\0')) {
//     iVar3 = *(int *)(param_1 + 4);
//     if (*(int *)(param_1 + 0x1c) << 1 < 1) {
//       local_28.vx = -*(int *)(param_1 + 0x1c);
//     }
//     else {
//       iVar1 = rand();
//       iVar2 = *(int *)(param_1 + 0x1c) << 1;
//       if (iVar2 == 0) {
//         trap(0x1c00);
//       }
//       if ((iVar2 == -1) && (iVar1 == -0x80000000)) {
//         trap(0x1800);
//       }
//       local_28.vx = iVar1 % iVar2 - *(int *)(param_1 + 0x1c);
//     }
//     local_28.vx = iVar3 + local_28.vx;
//     local_28.vy = *(int *)(param_1 + 8);
//     iVar3 = *(int *)(param_1 + 0xc);
//     if (*(int *)(param_1 + 0x20) << 1 < 1) {
//       local_28.vz = -*(int *)(param_1 + 0x20);
//     }
//     else {
//       iVar1 = rand();
//       iVar2 = *(int *)(param_1 + 0x20) << 1;
//       if (iVar2 == 0) {
//         trap(0x1c00);
//       }
//       if ((iVar2 == -1) && (iVar1 == -0x80000000)) {
//         trap(0x1800);
//       }
//       local_28.vz = iVar1 % iVar2 - *(int *)(param_1 + 0x20);
//     }
//     local_28.vz = iVar3 + local_28.vz;
//     if ((GameClock & 1U) == 0) {
//       local_18.vx = 0;
//       local_18.vy = -400;
//       local_18.vz = 0;
//       local_28.vy = local_28.vy + 2000;
//       SetSmoke(&local_28,&local_18,1,1);
//       local_28.vy = local_28.vy + -2000;
//       SetSplash(&local_28,0x4000,0x2000,10);
//     }
//   }
//   return;
// }
