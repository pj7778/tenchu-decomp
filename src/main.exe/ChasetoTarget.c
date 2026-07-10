#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/ChasetoTarget", ChasetoTarget);

// triage: MEDIUM — 82 insns, mul/div, 4 callees, ~0.07 to SetupMotionRegist
// likely-relevant cookbook sections:
//   - Dispatch: if/switch ladder — reload vs CSE, signed vs unsigned
//   - Expressions: mult/div — magic-multiply constants, fold
//   - gp vs absolute globals: gp-relative smalls — tools/gpsyms.py

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// short ChasetoTarget(long length)
//
// {
//   Humanoid *pHVar1;
//   short sVar2;
//   int iVar3;
//   int iVar4;
//   ModelType *pMVar5;
//   int iVar6;
//   int iVar7;
//
//   pHVar1 = Me_THINK_C;
//   pMVar5 = Me_THINK_C->target;
//   sVar2 = 0;
//   if (pMVar5 != (ModelType *)0x0) {
//     iVar7 = ((pMVar5->locate).coord.t[0] + Me_THINK_C->chase[0]) - Me_THINK_C->locate->vx;
//     iVar6 = ((pMVar5->locate).coord.t[2] + Me_THINK_C->chase[1]) - Me_THINK_C->locate->vz;
//     iVar3 = iVar7;
//     if (iVar7 < 0) {
//       iVar3 = -iVar7;
//     }
//     if (iVar3 < 500) {
//       iVar3 = iVar6;
//       if (iVar6 < 0) {
//         iVar3 = -iVar6;
//       }
//       if (iVar3 < 500) {
//         return 0;
//       }
//     }
//     sVar2 = 0;
//     if (((Attrib & 0x400U) == 0) && (sVar2 = 0, 999 < Distance)) {
//       if (((Attrib & 0xc000U) != 0) || (Me_THINK_C->chase[0] == 0 && Me_THINK_C->chase[1] == 0)) {
//         iVar3 = rand();
//         iVar4 = rcos((int)(short)iVar3);
//         pHVar1->chase[0] = iVar4 * length >> 0xc;
//         iVar3 = rsin((int)(short)iVar3);
//         pHVar1->chase[1] = iVar3 * length >> 0xc;
//       }
//       sVar2 = FUN_8002b990(iVar7,iVar6);
//     }
//   }
//   return sVar2;
// }
