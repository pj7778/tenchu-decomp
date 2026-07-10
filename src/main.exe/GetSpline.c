#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/GetSpline", GetSpline);

// triage: MEDIUM — 84 insns, mul/div, 2 loop, 2 callees, ~0.09 to GetAbsolutePosition
// likely-relevant cookbook sections:
//   - Loops: 2 back-edge(s) — for/while/do vs goto shape
//   - Expressions: mult/div — magic-multiply constants, fold
//   - gp vs absolute globals: gp-relative smalls — tools/gpsyms.py

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void GetSpline(SVECTOR *vect,SplineControlType *spc,short cnt)
//
// {
//   MotionElementType *pMVar1;
//   int iVar2;
//   MotionElementType *pMVar3;
//   int iVar4;
//
//   pMVar1 = spc->key1;
//   if (spc->key1->time < cnt) {
//     do {
//       pMVar3 = pMVar1;
//       spc->key1 = pMVar3 + 1;
//       pMVar1 = pMVar3 + 1;
//     } while (pMVar3[1].time < cnt);
//     spc->key0 = pMVar3;
//   }
//   else {
//     pMVar1 = spc->key0;
//     if (spc->key0->time <= cnt) goto LAB_8001c220;
//     do {
//       pMVar3 = pMVar1;
//       spc->key0 = pMVar3 + -1;
//       pMVar1 = pMVar3 + -1;
//     } while (cnt < pMVar3[-1].time);
//     spc->key1 = pMVar3;
//   }
//   UpdateSplineControl(spc);
// LAB_8001c220:
//   iVar4 = (int)spc->key0->time;
//   iVar2 = (cnt - iVar4) * 0x20;
//   iVar4 = spc->key1->time - iVar4;
//   if (iVar4 == 0) {
//     trap(0x1c00);
//   }
//   if ((iVar4 == -1) && (iVar2 == -0x80000000)) {
//     trap(0x1800);
//   }
//   DAT_80097eec = (short)(iVar2 / iVar4);
//   if ((int)DAT_80097708 != (int)DAT_80097eec) {
//     DAT_80097ee8 = DAT_80097eec * 8 + -0x7ff7975c;
//     DAT_80097708 = DAT_80097eec;
//   }
//   FUN_8001c730(vect,spc,DAT_80097ee8);
//   return;
// }
