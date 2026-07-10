#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/SetupSpline", SetupSpline);

// triage: EASY — 60 insns, 1 loop, 1 callees, ~0.11 to update_something_for_each_visible_enemy_
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void SetupSpline(MotionManager *mmp)
//
// {
//   short sVar1;
//   int iVar2;
//   MotionElementType *pMVar3;
//   SplineControlType *pSVar4;
//   int iVar5;
//
//   pSVar4 = mmp->control;
//   sVar1 = mmp->motion->time;
//   pMVar3 = mmp->motion->locate;
//   pSVar4->key0 = pMVar3;
//   (pSVar4->dd0).pad = sVar1;
//   if (sVar1 != 0) {
//     pSVar4->key1 = pMVar3 + 1;
//     UpdateSplineControl(pSVar4);
//   }
//   iVar5 = 0;
//   if (0 < mmp->n) {
//     iVar2 = 0;
//     do {
//       pMVar3 = mmp->motion->rotate[iVar2 >> 0x10];
//       pSVar4 = mmp->control + (iVar2 >> 0x10) + 1;
//       (pSVar4->dd0).pad = sVar1;
//       pSVar4->key0 = pMVar3;
//       if (sVar1 != 0) {
//         pSVar4->key1 = pMVar3 + 1;
//         UpdateSplineControl(pSVar4);
//       }
//       iVar5 = iVar5 + 1;
//       iVar2 = iVar5 * 0x10000;
//     } while (iVar5 * 0x10000 >> 0x10 < (int)mmp->n);
//   }
//   return;
// }
