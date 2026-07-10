#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * void GetMoveSpeed(struct SVECTOR *vect, short ry, short ordr, short side);
 *     HUMAN.C:370, 7 src lines, frame 40 bytes, saved-reg mask 0x800f0000
 *
 * Original parameters and locals (the demo build's register allocation may
 * differ from retail, but the COUNT and TYPES drive cc1's codegen and carry
 * over). A repeated name is a nested-block scope, not a duplicate:
 *     param $a0       struct SVECTOR * vect
 *     param $a1       short ry
 *     param $a2       short ordr
 *     param $a3       short side
 * END PSX.SYM */

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
