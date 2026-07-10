#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/FUN_800270f8", FUN_800270f8);

// triage: MEDIUM — 70 insns, 2 loop, 0 callees, ~0.07 to SetupThinkFunction
// likely-relevant cookbook sections:
//   - Loops: 2 back-edge(s) — for/while/do vs goto shape

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void FUN_800270f8(int param_1,short param_2)
//
// {
//   int iVar1;
//   short sVar2;
//   int iVar3;
//   int iVar4;
//   int iVar5;
//
//   iVar5 = *(int *)(param_1 + 0x58);
//   sVar2 = *(short *)(iVar5 + 100) + -1;
//   if (0xc < *(short *)(iVar5 + 100)) {
//     sVar2 = 0xc;
//   }
//   iVar3 = (int)sVar2;
//   if (param_2 == 0) {
//     iVar4 = 7;
//     if (6 < iVar3) {
//       do {
//         iVar1 = iVar4 << 0x10;
//         iVar4 = iVar4 + 1;
//         iVar1 = *(int *)((iVar1 >> 0xe) + *(int *)(iVar5 + 0x68));
//         *(ushort *)(iVar1 + 0x5a) = *(ushort *)(iVar1 + 0x5a) & 0xfffe;
//       } while (iVar4 * 0x10000 >> 0x10 <= iVar3);
//     }
//     *(ushort *)(**(int **)(iVar5 + 0x68) + 0x5a) =
//          *(ushort *)(**(int **)(iVar5 + 0x68) + 0x5a) & 0xfffe;
//     return;
//   }
//   iVar4 = 7;
//   if (6 < iVar3) {
//     do {
//       iVar1 = iVar4 << 0x10;
//       iVar4 = iVar4 + 1;
//       iVar1 = *(int *)((iVar1 >> 0xe) + *(int *)(iVar5 + 0x68));
//       *(ushort *)(iVar1 + 0x5a) = *(ushort *)(iVar1 + 0x5a) | 1;
//     } while (iVar4 * 0x10000 >> 0x10 <= iVar3);
//   }
//   *(ushort *)(**(int **)(iVar5 + 0x68) + 0x5a) = *(ushort *)(**(int **)(iVar5 + 0x68) + 0x5a) | 1;
//   return;
// }
