#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/FUN_80039ddc", FUN_80039ddc);

// triage: MEDIUM — 117 insns, mul/div, 2 callees, ~0.10 to bow_shoot_logic
// likely-relevant cookbook sections:
//   - Dispatch: if/switch ladder — reload vs CSE, signed vs unsigned
//   - Expressions: mult/div — magic-multiply constants, fold

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// /* WARNING: Removing unreachable block (ram,0x80039e94) */
//
// int FUN_80039ddc(int *param_1,int *param_2,int *param_3)
//
// {
//   long lVar1;
//   int iVar2;
//   long lVar3;
//   int iVar4;
//   int iVar5;
//   int iVar6;
//   int iVar7;
//   int iVar8;
//   int iVar9;
//   int iVar10;
//   AreaNodeType *local_40;
//   int local_3c;
//   int local_38;
//   int local_34;
//   int local_30;
//   int local_2c;
//
//   local_3c = *param_1;
//   local_34 = *param_2 - local_3c;
//   local_38 = param_1[1];
//   local_30 = param_2[1] - local_38;
//   iVar8 = param_1[2];
//   local_2c = param_2[2] - iVar8;
//   lVar1 = GetVectorLength(local_34,local_30,local_2c);
//   if (lVar1 == 0) {
//     trap(0x1c00);
//   }
//   local_40 = (AreaNodeType *)0x0;
//   iVar7 = iVar8;
//   iVar9 = local_38;
//   iVar10 = local_3c;
//   for (iVar6 = 0x1f4000 / lVar1; iVar6 < 0x1000; iVar6 = iVar6 + 0x1f4000 / lVar1) {
//     iVar2 = local_34 * iVar6;
//     if (iVar2 < 0) {
//       iVar2 = iVar2 + 0xfff;
//     }
//     iVar4 = local_30 * iVar6;
//     iVar2 = local_3c + (iVar2 >> 0xc);
//     if (iVar4 < 0) {
//       iVar4 = iVar4 + 0xfff;
//     }
//     iVar5 = local_2c * iVar6;
//     iVar4 = local_38 + (iVar4 >> 0xc);
//     if (iVar5 < 0) {
//       iVar5 = iVar5 + 0xfff;
//     }
//     iVar5 = iVar8 + (iVar5 >> 0xc);
//     lVar3 = CGetLevel(&local_40,iVar2,iVar4,iVar5);
//     if (lVar3 < iVar4) break;
//     iVar7 = iVar5;
//     iVar9 = iVar4;
//     iVar10 = iVar2;
//   }
//   if (param_3 != (int *)0x0) {
//     *param_3 = iVar10;
//     param_3[1] = iVar9;
//     param_3[2] = iVar7;
//   }
//   return iVar6;
// }
