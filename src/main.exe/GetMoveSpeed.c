#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/GetMoveSpeed", GetMoveSpeed);

// triage: EASY — 51 insns, mul/div, 2 callees, ~0.22 to MoveHumanoid
// likely-relevant cookbook sections:
//   - Expressions: mult/div — magic-multiply constants, fold

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void GetMoveSpeed(SVECTOR *vect,short ry,short ordr,short side)
//
// {
//   int iVar1;
//   int iVar2;
//
//   iVar1 = rsin((int)ry);
//   iVar2 = rcos((int)ry);
//   vect->vy = 0;
//   vect->vx = (short)((int)(short)-(short)iVar1 * (int)ordr - (int)(short)-(short)iVar2 * (int)side
//                     >> 0xc);
//   vect->vz = (short)((int)(short)-(short)iVar2 * (int)ordr + (int)(short)-(short)iVar1 * (int)side
//                     >> 0xc);
//   return;
// }
