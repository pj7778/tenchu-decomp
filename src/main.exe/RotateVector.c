#include "common.h"
#include "main.exe.h"

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
