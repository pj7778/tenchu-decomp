#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * long GetTargetDistance(struct Humanoid *human, short *deg);
 *     HUMAN.C:394, 10 src lines, frame 40 bytes, saved-reg mask 0x800f0000
 *
 * Original parameters and locals (the demo build's register allocation may
 * differ from retail, but the COUNT and TYPES drive cc1's codegen and carry
 * over). A repeated name is a nested-block scope, not a duplicate:
 *     param $a0       struct Humanoid * human
 *     param $a1       short * deg
 * END PSX.SYM */

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/GetTargetDistance", GetTargetDistance);

// triage: EASY — 52 insns, mul/div, 2 callees, ~0.21 to GetVectorRotation
// likely-relevant cookbook sections:
//   - Dispatch: if/switch ladder — reload vs CSE, signed vs unsigned
//   - Expressions: mult/div — magic-multiply constants, fold

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// long GetTargetDistance(Humanoid *human,short *deg)
//
// {
//   ushort uVar1;
//   long lVar2;
//   int iVar3;
//   short sVar4;
//   int iVar5;
//   int iVar6;
//
//   iVar6 = (human->target->locate).coord.t[0] - human->locate->vx;
//   iVar5 = (human->target->locate).coord.t[2] - human->locate->vz;
//   uVar1 = human->rotate->vy;
//   lVar2 = ratan2(-iVar6,-iVar5);
//   iVar3 = lVar2 - (uint)uVar1;
//   sVar4 = (short)iVar3;
//   iVar3 = iVar3 * 0x10000 >> 0x10;
//   if (iVar3 < 0x801) {
//     if (-0x800 < iVar3) goto LAB_80029828;
//   }
//   else {
//     sVar4 = -sVar4;
//   }
//   sVar4 = sVar4 + 0x1000;
// LAB_80029828:
//   *deg = sVar4;
//   lVar2 = SquareRoot0(iVar6 * iVar6 + iVar5 * iVar5);
//   return lVar2;
// }
