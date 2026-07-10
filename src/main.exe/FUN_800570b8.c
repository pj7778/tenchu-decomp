#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/FUN_800570b8", FUN_800570b8);

// triage: EASY — 88 insns, 1 loop, 2 callees, ~0.10 to FUN_800576e8
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void FUN_800570b8(undefined4 param_1,int param_2,int param_3,byte *param_4)
//
// {
//   bool bVar1;
//   uint uVar2;
//   int iVar3;
//
//   if (*param_4 != 0) {
//     if ((TelopP.u0 == '\0') && (iVar3 = param_2, TelopP.u1 == '\0')) {
//       do {
//         if (*param_4 == 10) {
//           param_3 = param_3 + 0x10;
//           iVar3 = param_2;
//         }
//         else {
//           FUN_8005778c(param_1,iVar3,param_3,*param_4);
//           uVar2 = (uint)*param_4;
//           if (uVar2 == 0x92) {
//             uVar2 = 0x27;
//           }
//           bVar1 = 0xbf < uVar2;
//           if (0x1f < uVar2) {
//             uVar2 = uVar2 - 0x20;
//           }
//           if (bVar1) {
//             uVar2 = uVar2 - 0x40;
//           }
//           iVar3 = iVar3 + (uint)*(byte *)(uVar2 + 0x8008ef98);
//         }
//         param_4 = param_4 + 1;
//       } while (*param_4 != 0);
//     }
//     else {
//       TelopP.y0 = (short)param_3;
//       TelopP.y2 = TelopP.y0 + 0xf;
//       TelopP.x0 = (short)param_2;
//       TelopP.x1 = TelopP.x0 + ((ushort)TelopP.u1 - (ushort)TelopP.u0);
//       TelopP.y1 = TelopP.y0;
//       TelopP.x2 = TelopP.x0;
//       TelopP.x3 = TelopP.x1;
//       TelopP.y3 = TelopP.y2;
//       GsSortPoly(&TelopP,OTablePt,0);
//     }
//   }
//   return;
// }
