#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/GetAreaMapVector", GetAreaMapVector);

// triage: MEDIUM — 137 insns, mul/div, 1 loop, 1 callees, ~0.08 to AdtFntOpen
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape
//   - Expressions: mult/div — magic-multiply constants, fold
//   - gp vs absolute globals: gp-relative smalls — tools/gpsyms.py

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// long GetAreaMapVector(MapVector *mvp,VECTOR *pos,long wide)
//
// {
//   short sVar1;
//   AreaNodeType *pAVar2;
//   NodeIndexType *pNVar3;
//   long lVar4;
//   int in_a3;
//   byte bVar5;
//   int iVar6;
//   long z;
//   int x;
//   ushort in_stack_00000010;
//
//   x = *(int *)wide;
//   z = *(long *)(wide + 4);
//   lVar4 = GetAreaMapLevel((ulong *)mvp,x,z);
//   pNVar3 = FieldIndex;
//   pAVar2 = FieldArea;
//   sVar1 = FieldAttrib;
//   pos->vx = lVar4;
//   *(short *)&pos->vz = sVar1;
//   pos[1].vx = (long)pAVar2;
//   pos[1].vy = (long)pNVar3;
//   if (lVar4 == -0x80000000) {
//     pos->vy = 0;
//     if ((in_stack_00000010 & 4) == 0) {
//       *(undefined2 *)&pos->vz = 2;
//       *(undefined1 *)((int)&pos->pad + 3) = 0xf;
//       *(undefined1 *)((int)&pos->pad + 2) = 0xf;
//       *(undefined1 *)&pos->pad = 0xf;
//       return -0x80000000;
//     }
//   }
//   else {
//     pos->vy = lVar4 - *(int *)(wide + 4);
//   }
//   iVar6 = 0;
//   bVar5 = 1;
//   *(undefined1 *)((int)&pos->pad + 3) = 0;
//   *(undefined1 *)((int)&pos->pad + 2) = 0;
//   *(undefined1 *)&pos->pad = 0;
//   do {
//     lVar4 = GetAreaMapLevel((ulong *)mvp,
//                             x + *(short *)((int)direction[0] + ((iVar6 << 0x10) >> 0xe)) * in_a3,z);
//     if ((lVar4 == -0x80000000) ||
//        (((lVar4 - z < -500 && ((in_stack_00000010 & 4) == 0)) &&
//         ((((ushort)pos->vz | FieldAttrib) & 0xc000) == 0)))) {
//       *(byte *)&pos->pad = bVar5 | (byte)pos->pad;
//     }
//     else if (pos->vx < lVar4) {
//       *(byte *)((int)&pos->pad + 2) = bVar5 | *(byte *)((int)&pos->pad + 2);
//     }
//     else if (lVar4 < pos->vx) {
//       *(byte *)((int)&pos->pad + 3) = bVar5 | *(byte *)((int)&pos->pad + 3);
//     }
//     iVar6 = iVar6 + 1;
//     bVar5 = bVar5 << 1;
//   } while (iVar6 * 0x10000 >> 0x10 < 4);
//   return pos->vx;
// }
