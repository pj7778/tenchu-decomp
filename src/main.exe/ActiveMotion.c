#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/ActiveMotion", ActiveMotion);

// triage: MEDIUM — 113 insns, mul/div, 1 loop, 3 callees, ~0.12 to update_something_for_each_visible_enemy_
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape
//   - Expressions: mult/div — magic-multiply constants, fold

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// short ActiveMotion(MotionManager *mmp)
//
// {
//   short sVar1;
//   uint uVar2;
//   int iVar3;
//   ModelType *pMVar4;
//   SVECTOR local_20;
//
//   if (mmp->motion->time == 0) {
//     sVar1 = HoldMotion(mmp);
//   }
//   else {
//     sVar1 = mmp->count;
//     mmp->count = sVar1 + 1;
//     if ((mmp->mask & 1U) != 0) {
//       pMVar4 = *mmp->model->object;
//       GetSpline(&local_20,mmp->control,sVar1);
//       (pMVar4->locate).coord.t[0] = (int)local_20.vx;
//       (pMVar4->locate).coord.t[2] = (int)local_20.vz;
//       (pMVar4->locate).coord.t[1] = (int)(mmp->model->rotate).pad * (int)local_20.vy >> 0xc;
//       GetSpline(&pMVar4->rotate,mmp->control + 1,sVar1);
//       UpdateCoordinate(pMVar4);
//     }
//     iVar3 = 1;
//     if (1 < mmp->n) {
//       do {
//         uVar2 = (uint)(short)iVar3;
//         if (((int)mmp->mask >> (uVar2 & 0x1f) & 1U) != 0) {
//           pMVar4 = mmp->model->object[uVar2];
//           GetSpline(&pMVar4->rotate,mmp->control + uVar2 + 1,sVar1);
//           UpdateCoordinate(pMVar4);
//         }
//         iVar3 = iVar3 + 1;
//       } while (iVar3 * 0x10000 >> 0x10 < (int)mmp->n);
//     }
//     sVar1 = mmp->count;
//     if (mmp->motion->time < sVar1) {
//       mmp->count = 0;
//       sVar1 = 0;
//     }
//   }
//   return sVar1;
// }
