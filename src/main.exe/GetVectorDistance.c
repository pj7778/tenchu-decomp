#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * int GetVectorDistance(struct VECTOR *v1, struct VECTOR *v2);
 *     EFFECT.C:509, 19 src lines, frame 24 bytes, saved-reg mask 0x80000000
 *
 * Original parameters and locals (the demo build's register allocation may
 * differ from retail, but the COUNT and TYPES drive cc1's codegen and carry
 * over). A repeated name is a nested-block scope, not a duplicate:
 *     param $a0       struct VECTOR * v1
 *     param $a1       struct VECTOR * v2
 * END PSX.SYM */

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/GetVectorDistance", GetVectorDistance);

// triage: EASY — 79 insns, mul/div, 2 callees, ~0.16 to GetVectorRotation
// likely-relevant cookbook sections:
//   - Dispatch: if/switch ladder — reload vs CSE, signed vs unsigned
//   - Expressions: mult/div — magic-multiply constants, fold

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// int GetVectorDistance(VECTOR *v1,VECTOR *v2)
//
// {
//   bool bVar1;
//   int iVar2;
//   long lVar3;
//   int n;
//   int n_00;
//   int n_01;
//
//   bVar1 = false;
//   n_01 = v1->vx - v2->vx;
//   n = v1->vy - v2->vy;
//   n_00 = v1->vz - v2->vz;
//   iVar2 = abs(n_01);
//   if (((0x1000 < iVar2) || (iVar2 = abs(n), 0x1000 < iVar2)) || (iVar2 = abs(n_00), 0x1000 < iVar2))
//   {
//     bVar1 = true;
//   }
//   if (bVar1) {
//     if (n_01 < 0) {
//       n_01 = n_01 + 0xff;
//     }
//     if (n < 0) {
//       n = n + 0xff;
//     }
//     if (n_00 < 0) {
//       n_00 = n_00 + 0xff;
//     }
//     lVar3 = SquareRoot0((n_01 >> 8) * (n_01 >> 8) + (n >> 8) * (n >> 8) + (n_00 >> 8) * (n_00 >> 8))
//     ;
//     iVar2 = lVar3 << 8;
//   }
//   else {
//     iVar2 = SquareRoot0(n_01 * n_01 + n * n + n_00 * n_00);
//   }
//   return iVar2;
// }
