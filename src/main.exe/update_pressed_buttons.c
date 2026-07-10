#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/update_pressed_buttons", update_pressed_buttons);

// triage: EASY — 47 insns, 1 loop, 0 callees, ~0.02 to FUN_800565f0
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// int FUN_8001b1a4(ushort *param_1,ushort param_2)
//
// {
//   ushort uVar1;
//   short sVar2;
//   int iVar3;
//
//   uVar1 = param_1[3];
//   if (((short)param_1[3] < 6) &&
//      (param_1[3] = uVar1 + 1, (int)((uint)(ushort)(uVar1 + 1) << 0x10) < 0)) {
//     param_2 = 0;
//   }
//   uVar1 = *param_1;
//   *param_1 = param_2;
//   param_1[1] = uVar1;
//   param_1[2] = param_2 & ~uVar1;
//   if ((param_1[1] != param_2) && (param_2 != 0)) {
//     iVar3 = 3;
//     if (5 < (short)param_1[3]) {
//       param_1[4] = 0;
//     }
//     do {
//       sVar2 = (short)iVar3;
//       iVar3 = iVar3 + -1;
//       param_1[sVar2 + 4] = param_1[sVar2 + 3];
//     } while (0 < iVar3 * 0x10000);
//     param_1[3] = 0;
//     param_1[4] = *param_1;
//   }
//   return (int)(short)param_2;
// }
