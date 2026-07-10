#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * void RotateVector(struct VECTOR *vec, int rx, int ry, int rz);
 *     EFFECT.C:470, 9 src lines, frame 96 bytes, saved-reg mask 0x801f0000
 *
 * Original parameters and locals (the demo build's register allocation may
 * differ from retail, but the COUNT and TYPES drive cc1's codegen and carry
 * over). A repeated name is a nested-block scope, not a duplicate:
 *     param $a0       struct VECTOR * vec
 *     param $a1       int rx
 *     param $a2       int ry
 *     param $a3       int rz
 *     stack sp+16     struct MATRIX SMAT
 *     stack sp+48     struct SVECTOR rot
 *     stack sp+56     struct VECTOR vo
 * END PSX.SYM */

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/RotateVector", RotateVector);

// triage: TRIVIAL — 43 insns, 3 callees, ~0.15 to AdtFntOpen

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void RotateVector(VECTOR *vec,int rx,int ry,int rz)
//
// {
//   MATRIX MStack_50;
//   SVECTOR local_30;
//   VECTOR local_28;
//
//   memset((uchar *)&local_30,'\0',8);
//   local_30.vx = (short)rx;
//   local_30.vy = (short)ry;
//   local_30.vz = (short)rz;
//   RotMatrixYXZ(&local_30,&MStack_50);
//   ApplyMatrixLV(&MStack_50,vec,&local_28);
//   vec->vx = local_28.vx;
//   vec->vy = local_28.vy;
//   vec->vz = local_28.vz;
//   return;
// }
