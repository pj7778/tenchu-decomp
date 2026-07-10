#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * void RotateVectorS(struct SVECTOR *vec, int rx, int ry, int rz);
 *     EFFECT.C:480, 9 src lines, frame 88 bytes, saved-reg mask 0x801f0000
 *
 * Original parameters and locals (the demo build's register allocation may
 * differ from retail, but the COUNT and TYPES drive cc1's codegen and carry
 * over). A repeated name is a nested-block scope, not a duplicate:
 *     param $a0       struct SVECTOR * vec
 *     param $a1       int rx
 *     param $a2       int ry
 *     param $a3       int rz
 *     stack sp+16     struct MATRIX SMAT
 *     stack sp+48     struct SVECTOR rot
 *     stack sp+56     struct SVECTOR vo
 * END PSX.SYM */

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/RotateVectorS", RotateVectorS);

// triage: TRIVIAL — 43 insns, 3 callees, ~0.14 to vcalloc

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void RotateVectorS(SVECTOR *vec,int rx,int ry,int rz)
//
// {
//   MATRIX MStack_48;
//   SVECTOR local_28;
//   SVECTOR local_20;
//
//   memset((uchar *)&local_28,'\0',8);
//   local_28.vx = (short)rx;
//   local_28.vy = (short)ry;
//   local_28.vz = (short)rz;
//   RotMatrixYXZ(&local_28,&MStack_48);
//   ApplyMatrixSV(&MStack_48,vec,&local_20);
//   vec->vx = local_20.vx;
//   vec->vy = local_20.vy;
//   vec->vz = local_20.vz;
//   return;
// }
