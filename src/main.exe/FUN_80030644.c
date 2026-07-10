#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/FUN_80030644", FUN_80030644);

// triage: EASY — 74 insns, 1 callees, ~0.09 to AdtFntOpen

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void FUN_80030644(int *param_1,int param_2)
//
// {
//   short sVar1;
//   uint uVar2;
//   int iVar3;
//   VECTOR local_40;
//   VECTOR VStack_28;
//
//   GetAreaMapVector((MapVector *)GlobalAreaMap,&local_40,(long)param_1);
//   if (local_40.vx == -0x80000000) {
//     return;
//   }
//   if ((byte)local_40.pad == 0) {
//     return;
//   }
//   GetAreaMapVector((MapVector *)GlobalAreaMap,&VStack_28,(long)param_1);
//   uVar2 = (uint)(byte)VStack_28.pad;
//   if (uVar2 == 0) {
//     uVar2 = (uint)(byte)local_40.pad;
//     param_2 = param_2 / 2;
//   }
//   sVar1 = RefrectMove[0][uVar2 * 2 + 1];
//   if (RefrectMove[0][uVar2 * 2] < 1) {
//     if (-1 < RefrectMove[0][uVar2 * 2]) goto LAB_80030724;
//     iVar3 = *param_1 - param_2;
//   }
//   else {
//     iVar3 = *param_1 + param_2;
//   }
//   *param_1 = iVar3;
// LAB_80030724:
//   if (sVar1 < 1) {
//     if (-1 < sVar1) {
//       return;
//     }
//     param_2 = param_1[2] - param_2;
//   }
//   else {
//     param_2 = param_1[2] + param_2;
//   }
//   param_1[2] = param_2;
//   return;
// }
