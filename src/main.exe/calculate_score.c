#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/calculate_score", calculate_score);

// triage: MEDIUM — 88 insns, mul/div, 1 callees, ~0.04 to SetupAfterimage
// likely-relevant cookbook sections:
//   - Dispatch: if/switch ladder — reload vs CSE, signed vs unsigned
//   - Expressions: mult/div — magic-multiply constants, fold

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// undefined2 * FUN_8004e794(byte *param_1,short param_2)
//
// {
//   byte bVar1;
//   short sVar2;
//   int iVar3;
//   short sVar4;
//
//   DAT_800c2578 = (ushort)param_1[4] * 0x14;
//   DAT_800c257a = (ushort)param_1[3] * 5;
//   DAT_800c257c = (ushort)param_1[5] * -0x1e;
//   bVar1 = param_1[2];
//   sVar4 = 400;
//   if (bVar1 != 0) {
//     sVar4 = 300;
//   }
//   if (param_2 == 8) {
//     sVar2 = (ushort)bVar1 * 0x28;
//   }
//   else {
//     sVar2 = (ushort)bVar1 * 0x14;
//   }
//   DAT_800c257e = sVar4 - sVar2;
//   if (DAT_800c257e < 0) {
//     DAT_800c257e = 0;
//   }
//   DAT_800c2580 = DAT_800c257e + DAT_800c257c + DAT_800c2578 + DAT_800c257a;
//   iVar3 = (int)DAT_800c2580;
//   if (iVar3 < 0) {
//     iVar3 = 0;
//   }
//   DAT_800c2582 = (undefined2)(iVar3 / 100);
//   if (4 < (iVar3 / 100) * 0x10000 >> 0x10) {
//     DAT_800c2582 = 4;
//   }
//   if ((uint)*param_1 + (uint)param_1[1] == 0) {
//     memset((uchar *)&DAT_800c2578,'\0',0xc);
//   }
//   return &DAT_800c2578;
// }
