#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/FUN_8002fd9c", FUN_8002fd9c);

// triage: MEDIUM — 198 insns, mul/div, 1 loop, 3 callees, ~0.05 to GetAreaMapLevel
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape
//   - Expressions: mult/div — magic-multiply constants, fold

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// long FUN_8002fd9c(int param_1)
//
// {
//   int iVar1;
//   int iVar2;
//   int iVar3;
//   int iVar4;
//   long lVar5;
//   ushort *puVar6;
//
//   if (*(int *)(param_1 + 0x30) == 0) {
//     return 0;
//   }
//   iVar1 = rsin((int)*(short *)(*(int *)(param_1 + 0x3c) + 2));
//   iVar1 = -iVar1;
//   if (iVar1 < 0) {
//     iVar1 = iVar1 + 0xf;
//   }
//   iVar2 = rcos((int)*(short *)(*(int *)(param_1 + 0x3c) + 2));
//   iVar2 = -iVar2;
//   if (iVar2 < 0) {
//     iVar2 = iVar2 + 0xf;
//   }
//   puVar6 = *(ushort **)(param_1 + 0x30);
//   iVar3 = (uint)puVar6[4] - (uint)puVar6[2];
//   if (((int)(short)puVar6[6] & 0xc000U) == 0x4000) {
//     iVar4 = ((int)((**(int **)(param_1 + 0x38) / 10 - (uint)puVar6[2]) * 0x10000) >> 0x10) *
//             (int)(short)puVar6[1];
// LAB_8002fed4:
//     iVar3 = (iVar3 + 1) * 0x10000 >> 0x10;
//     if (iVar3 == 0) {
//       trap(0x1c00);
//     }
//     if ((iVar3 == -1) && (iVar4 == -0x80000000)) {
//       trap(0x1800);
//     }
//     iVar3 = (int)(((uint)*puVar6 + iVar4 / iVar3) * 0x10000) >> 0x10;
//   }
//   else {
//     if (((int)(short)puVar6[6] & 0xc000U) == 0x8000) {
//       iVar4 = ((int)(((*(int **)(param_1 + 0x38))[2] / 10 - (uint)puVar6[3]) * 0x10000) >> 0x10) *
//               (int)(short)puVar6[1];
//       iVar3 = (uint)puVar6[5] - (uint)puVar6[3];
//       goto LAB_8002fed4;
//     }
//     iVar3 = (int)(short)*puVar6;
//   }
//   if (iVar3 * 10 == -0x80000000) {
//     return 0;
//   }
//   puVar6 = *(ushort **)(param_1 + 0x30);
//   iVar4 = (uint)puVar6[4] - (uint)puVar6[2];
//   if (((int)(short)puVar6[6] & 0xc000U) == 0x4000) {
//     iVar1 = ((int)(((**(int **)(param_1 + 0x38) + (iVar1 >> 4)) / 10 - (uint)puVar6[2]) * 0x10000)
//             >> 0x10) * (int)(short)puVar6[1];
//   }
//   else {
//     if (((int)(short)puVar6[6] & 0xc000U) != 0x8000) {
//       iVar1 = (int)(short)*puVar6;
//       goto LAB_80030048;
//     }
//     iVar1 = ((int)((((*(int **)(param_1 + 0x38))[2] + (iVar2 >> 4)) / 10 - (uint)puVar6[3]) *
//                   0x10000) >> 0x10) * (int)(short)puVar6[1];
//     iVar4 = (uint)puVar6[5] - (uint)puVar6[3];
//   }
//   iVar2 = (iVar4 + 1) * 0x10000 >> 0x10;
//   if (iVar2 == 0) {
//     trap(0x1c00);
//   }
//   if ((iVar2 == -1) && (iVar1 == -0x80000000)) {
//     trap(0x1800);
//   }
//   iVar1 = (int)(((uint)*puVar6 + iVar1 / iVar2) * 0x10000) >> 0x10;
// LAB_80030048:
//   if (iVar1 * 10 == -0x80000000) {
//     return 0;
//   }
//   iVar1 = iVar1 * 10 + iVar3 * -10;
//   if (iVar1 < -699) {
//     return 0;
//   }
//   lVar5 = ratan2(iVar1,0x100);
//   if (0x155 < lVar5) {
//     return 0x155;
//   }
//   if (lVar5 < -0x155) {
//     return -0x155;
//   }
//   return lVar5;
// }
