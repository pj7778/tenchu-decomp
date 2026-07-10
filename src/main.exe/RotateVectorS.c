#include "common.h"
#include "main.exe.h"

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
