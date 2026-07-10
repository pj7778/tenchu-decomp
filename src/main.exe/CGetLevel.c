#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/CGetLevel", CGetLevel);

// triage: MEDIUM — 98 insns, mul/div, 2 callees, ~0.05 to GetAreaMapLevel
// likely-relevant cookbook sections:
//   - Expressions: mult/div — magic-multiply constants, fold

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// long CGetLevel(AreaNodeType **hint,long x,long y,long z)
//
// {
//   int x_00;
//   int z_00;
//   long lVar1;
//   AreaNodeType *node;
//   int iVar2;
//
//   x_00 = x / 10;
//   node = *hint;
//   z_00 = z / 10;
//   if (node != (AreaNodeType *)0x0) {
//     iVar2 = (int)node->y;
//     if (((((iVar2 + -200 <= y / 10) && (y / 10 <= iVar2)) && (node->x1 <= x_00)) &&
//         ((node->z1 <= z_00 && (x_00 <= node->x2)))) && (z_00 <= node->z2)) {
//       if ((node->dy != 0) && (iVar2 = ComputeAreaLevel(node,x_00,z_00), iVar2 == -0x80000000)) {
//         return -0x80000000;
//       }
//       return iVar2 * 10;
//     }
//   }
//   lVar1 = GetAreaMapLevel(GlobalAreaMap,x,y + -300);
//   if ((y <= lVar1) && (FieldArea->division == -1)) {
//     *hint = FieldArea;
//   }
//   return lVar1;
// }
