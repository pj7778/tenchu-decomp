#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/GetVectorLength", GetVectorLength);

// triage: EASY — 72 insns, mul/div, 2 callees, ~0.10 to AdtFntLoad
// likely-relevant cookbook sections:
//   - Dispatch: if/switch ladder — reload vs CSE, signed vs unsigned
//   - Expressions: mult/div — magic-multiply constants, fold

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// long GetVectorLength(long dx,long dy,long dz)
//
// {
//   bool bVar1;
//   int iVar2;
//   long lVar3;
//
//   bVar1 = false;
//   iVar2 = abs(dx);
//   if (((0x1000 < iVar2) || (iVar2 = abs(dy), 0x1000 < iVar2)) || (iVar2 = abs(dz), 0x1000 < iVar2))
//   {
//     bVar1 = true;
//   }
//   if (bVar1) {
//     if (dx < 0) {
//       dx = dx + 0xff;
//     }
//     if (dy < 0) {
//       dy = dy + 0xff;
//     }
//     if (dz < 0) {
//       dz = dz + 0xff;
//     }
//     lVar3 = SquareRoot0((dx >> 8) * (dx >> 8) + (dy >> 8) * (dy >> 8) + (dz >> 8) * (dz >> 8));
//     lVar3 = lVar3 << 8;
//   }
//   else {
//     lVar3 = SquareRoot0(dx * dx + dy * dy + dz * dz);
//   }
//   return lVar3;
// }
