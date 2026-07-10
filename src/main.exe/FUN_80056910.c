#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/FUN_80056910", FUN_80056910);

// triage: MEDIUM — 75 insns, 2 loop, 1 callees, ~0.06 to AdtFntOpen
// likely-relevant cookbook sections:
//   - Loops: 2 back-edge(s) — for/while/do vs goto shape

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void FUN_80056910(int param_1,ushort param_2)
//
// {
//   undefined1 uVar1;
//   short sVar2;
//   uint uVar3;
//   int iVar4;
//   uint uVar5;
//
//   uVar5 = *(uint *)(param_1 + 0x68) & 0x8fffffff;
//   *(uint *)(param_1 + 0x68) = uVar5;
//   uVar3 = 0x50000000;
//   if (0 < (int)((uint)param_2 << 0x10)) {
//     uVar3 = 0x60000000;
//   }
//   *(uint *)(param_1 + 0x68) = uVar5 | uVar3;
//   iVar4 = (int)(short)param_2;
//   if (iVar4 < 0) {
//     iVar4 = -iVar4;
//   }
//   uVar1 = (undefined1)iVar4;
//   *(undefined1 *)(param_1 + 0x7e) = uVar1;
//   *(undefined1 *)(param_1 + 0x7d) = uVar1;
//   *(undefined1 *)(param_1 + 0x7c) = uVar1;
//   *(undefined2 *)(param_1 + 0x6e) = 0xff88;
//   if (-0x79 < (int)(*(ushort *)(param_1 + 0x72) + 0x78)) {
//     do {
//       *(undefined2 *)(param_1 + 0x6c) = 0xff60;
//       do {
//         GsSortSprite((GsSPRITE *)(param_1 + 0x68),OTablePt,1);
//         sVar2 = *(short *)(param_1 + 0x6c) + *(short *)(param_1 + 0x70);
//         *(short *)(param_1 + 0x6c) = sVar2;
//       } while (sVar2 < 0xa1);
//       sVar2 = *(short *)(param_1 + 0x6e) + *(ushort *)(param_1 + 0x72);
//       *(short *)(param_1 + 0x6e) = sVar2;
//     } while ((int)sVar2 <= (int)(*(ushort *)(param_1 + 0x72) + 0x78));
//   }
//   return;
// }
