#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/FUN_80058a54", FUN_80058a54);

// triage: MEDIUM — 135 insns, 1 loop, 4 callees, ~0.13 to AdtFntOpen
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void FUN_80058a54(uint *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)
//
// {
//   int iVar1;
//   byte bVar2;
//   undefined4 *puVar3;
//   ushort *puVar4;
//   int iVar5;
//   undefined4 uVar6;
//
//   puVar3 = (undefined4 *)param_1[2];
//   DAT_800c6568 = *param_1 >> 3 & 3;
//   puVar4 = (ushort *)puVar3[4];
//   iVar5 = puVar3[5];
//   DAT_800c656c = *param_1 >> 5 & 1;
//   uVar6 = *puVar3;
//   DAT_800c6564 = *param_1 >> 6 & 1;
//   DAT_800c6588 = *param_1 >> 9 & 7;
//   DAT_800c6584 = *param_1 >> 0x1e & 1;
//   do {
//     if (iVar5 == 0) {
//       return;
//     }
//     bVar2 = *(byte *)((int)puVar4 + 3) & 0xfd;
//     if (bVar2 == 0x35) {
//       GsOUT_PACKET_P = GsTMDfastTNG3(puVar4,uVar6,GsOUT_PACKET_P,*puVar4,param_3,param_2,param_4);
//       iVar5 = iVar5 - (uint)*puVar4;
//       iVar1 = (uint)*puVar4 * 9;
// LAB_80058c40:
//       iVar1 = iVar1 << 2;
//     }
//     else {
//       if (0x35 < bVar2) {
//         if (bVar2 != 0x3d) {
//           return;
//         }
//         GsOUT_PACKET_P = FUN_80058c70(puVar4,uVar6,GsOUT_PACKET_P,*puVar4,param_3,param_2,param_4);
//         iVar5 = iVar5 - (uint)*puVar4;
//         iVar1 = (uint)*puVar4 * 0xb;
//         goto LAB_80058c40;
//       }
//       if (bVar2 == 0x25) {
//         GsOUT_PACKET_P = GsTMDfastTNF3(puVar4,uVar6,GsOUT_PACKET_P,*puVar4,param_3,param_2,param_4);
//         iVar5 = iVar5 - (uint)*puVar4;
//         iVar1 = (uint)*puVar4 * 7;
//         goto LAB_80058c40;
//       }
//       if (bVar2 != 0x2d) {
//         return;
//       }
//       GsOUT_PACKET_P = FUN_80059008(puVar4,uVar6,GsOUT_PACKET_P,*puVar4,param_3,param_2,param_4);
//       iVar5 = iVar5 - (uint)*puVar4;
//       iVar1 = (uint)*puVar4 << 5;
//     }
//     puVar4 = (ushort *)((int)puVar4 + iVar1);
//   } while( true );
// }
