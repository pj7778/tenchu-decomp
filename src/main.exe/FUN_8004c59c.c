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

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/FUN_8004c59c", FUN_8004c59c);

// triage: MEDIUM — 103 insns, mul/div, 3 callees, ~0.08 to bow_shoot_logic
// likely-relevant cookbook sections:
//   - Dispatch: if/switch ladder — reload vs CSE, signed vs unsigned
//   - Expressions: mult/div — magic-multiply constants, fold

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void FUN_8004c59c(int param_1,uint param_2)
//
// {
//   int iVar1;
//   int iVar2;
//   undefined4 local_3c;
//   undefined4 local_38;
//   VECTOR local_30;
//   long local_20;
//   long local_1c;
//   long local_18;
//   long local_14;
//
//   if (param_2 == 0) {
//     local_3c = CONCAT22(*(undefined2 *)(param_1 + 0x20),*(undefined2 *)(param_1 + 0x1c));
//     local_38 = CONCAT31(local_38._1_3_,*(undefined1 *)(param_1 + 0x18));
//     *(long *)(param_1 + 0x18) = GameClock;
//     *(undefined4 *)(param_1 + 0x1c) = local_3c;
//     *(undefined4 *)(param_1 + 0x20) = local_38;
//     *(undefined1 *)(param_1 + 0x15) = 0;
//   }
//   else if (((3 < param_2) && (*(char *)(param_1 + 0x15) == '\0')) &&
//           (*(int *)(param_1 + 0x18) <= GameClock)) {
//     memset((uchar *)&local_20,'\0',0x10);
//     local_30.vx = *(long *)(param_1 + 4);
//     local_30.vy = *(long *)(param_1 + 8);
//     local_30.vz = *(long *)(param_1 + 0xc);
//     local_30.pad = local_14;
//     local_20 = local_30.vx;
//     local_1c = local_30.vy;
//     local_18 = local_30.vz;
//     SoundEx(&local_30,*(byte *)(param_1 + 0x20) + 0x44);
//     iVar1 = (int)*(short *)(param_1 + 0x1c);
//     if (0 < *(short *)(param_1 + 0x1e) - iVar1) {
//       iVar1 = rand();
//       iVar2 = (int)*(short *)(param_1 + 0x1e) - (int)*(short *)(param_1 + 0x1c);
//       if (iVar2 == 0) {
//         trap(0x1c00);
//       }
//       if ((iVar2 == -1) && (iVar1 == -0x80000000)) {
//         trap(0x1800);
//       }
//       iVar1 = iVar1 % iVar2 + (int)*(short *)(param_1 + 0x1c);
//     }
//     *(long *)(param_1 + 0x18) = GameClock + iVar1;
//   }
//   return;
// }
