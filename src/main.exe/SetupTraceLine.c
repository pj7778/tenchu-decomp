#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/SetupTraceLine", SetupTraceLine);

// triage: MEDIUM — 53 insns, mul/div, 1 loop, 2 callees, ~0.13 to AfsRead
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape
//   - Expressions: mult/div — magic-multiply constants, fold

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// TraceLine * SetupTraceLine(Humanoid *human,TracePoint *point)
//
// {
//   short sVar1;
//   TraceLine *pTVar2;
//
//   if (point == (TracePoint *)0x0) {
//                     /* WARNING: Subroutine does not return */
//     SystemOut((uchar *)"NO TRACE POINT");
//   }
//   pTVar2 = (TraceLine *)valloc(8);
//   pTVar2->count = 0;
//   pTVar2->index = 0;
//   pTVar2->point = point;
//   sVar1 = point->pad;
//   while (sVar1 != -1) {
//     sVar1 = point[1].pad;
//     point = point + 1;
//   }
//   point->x = human->locate->vx;
//   point->z = human->locate->vz;
//   point->range = (short)(human->locate->vy / 100);
//   human->trace = pTVar2;
//   return pTVar2;
// }
