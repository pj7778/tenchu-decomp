#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/GetCenterAndSize", GetCenterAndSize);

// triage: EASY — 105 insns, 1 loop, 0 callees, ~0.03 to DeleteConflict
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void GetCenterAndSize(ulong *tmd,SVECTOR *center,int *size)
//
// {
//   ushort *puVar1;
//   int iVar2;
//   uint uVar3;
//   uint uVar4;
//   uint uVar5;
//   uint uVar6;
//   uint uVar7;
//   uint uVar8;
//   uint uVar9;
//   uint uVar10;
//
//   uVar8 = 0;
//   uVar10 = 0;
//   uVar5 = 0;
//   uVar6 = 0;
//   uVar7 = 0;
//   uVar3 = 0;
//   puVar1 = (ushort *)*tmd;
//   iVar2 = 0;
//   uVar4 = uVar3;
//   if (0 < (int)tmd[1]) {
//     do {
//       uVar3 = (uint)*puVar1;
//       uVar9 = uVar8;
//       if (((short)uVar6 < (short)*puVar1) ||
//          (uVar3 = uVar6, uVar9 = (uint)*puVar1, (short)*puVar1 < (short)uVar8)) {
//         uVar6 = uVar3;
//         uVar8 = uVar9;
//       }
//       uVar3 = (uint)puVar1[1];
//       uVar9 = uVar10;
//       if (((short)uVar7 < (short)puVar1[1]) ||
//          (uVar3 = uVar7, uVar9 = (uint)puVar1[1], (short)puVar1[1] < (short)uVar10)) {
//         uVar7 = uVar3;
//         uVar10 = uVar9;
//       }
//       uVar3 = (uint)puVar1[2];
//       if (((short)puVar1[2] <= (short)uVar4) && (uVar3 = uVar4, (short)puVar1[2] < (short)uVar5)) {
//         uVar5 = (uint)puVar1[2];
//       }
//       iVar2 = iVar2 + 1;
//       puVar1 = puVar1 + 4;
//       uVar4 = uVar3;
//     } while (iVar2 < (int)tmd[1]);
//   }
//   center->vx = (short)(((int)(short)uVar6 + (int)(short)uVar8) / 2);
//   center->vy = (short)(((int)(short)uVar7 + (int)(short)uVar10) / 2);
//   center->vz = (short)(((int)(short)uVar3 + (int)(short)uVar5) / 2);
//   iVar2 = uVar3 - uVar5;
//   if ((int)((uVar3 - uVar5) * 0x10000) < (int)((uVar7 - uVar10) * 0x10000)) {
//     iVar2 = uVar7 - uVar10;
//   }
//   if (iVar2 << 0x10 < (int)((uVar6 - uVar8) * 0x10000)) {
//     iVar2 = uVar6 - uVar8;
//   }
//   *size = ((iVar2 << 0x10) >> 0x10) - ((iVar2 << 0x10) >> 0x1f) >> 1;
//   return;
// }
