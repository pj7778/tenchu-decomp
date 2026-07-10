#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/HoldMotion", HoldMotion);

// triage: MEDIUM — 84 insns, mul/div, 1 loop, 1 callees, ~0.08 to update_something_for_each_visible_enemy_
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape
//   - Expressions: mult/div — magic-multiply constants, fold

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// short HoldMotion(MotionManager *mmp)
//
// {
//   uint uVar1;
//   ModelType *pMVar2;
//   int iVar3;
//   MotionDataType *pMVar4;
//
//   pMVar4 = mmp->motion;
//   if ((mmp->mask & 1U) != 0) {
//     pMVar2 = *mmp->model->object;
//     (pMVar2->locate).coord.t[0] = (int)pMVar4->locate->x;
//     (pMVar2->locate).coord.t[2] = (int)pMVar4->locate->z;
//     (pMVar2->locate).coord.t[1] = (int)(mmp->model->rotate).pad * (int)pMVar4->locate->y >> 0xc;
//   }
//   iVar3 = 0;
//   if (0 < mmp->n) {
//     do {
//       uVar1 = (uint)(short)iVar3;
//       if (((int)mmp->mask >> (uVar1 & 0x1f) & 1U) != 0) {
//         pMVar2 = mmp->model->object[uVar1];
//         (pMVar2->rotate).vx = pMVar4->rotate[uVar1]->x;
//         (pMVar2->rotate).vy = pMVar4->rotate[uVar1]->y;
//         (pMVar2->rotate).vz = pMVar4->rotate[uVar1]->z;
//         UpdateCoordinate(pMVar2);
//       }
//       iVar3 = iVar3 + 1;
//     } while (iVar3 * 0x10000 >> 0x10 < (int)mmp->n);
//   }
//   mmp->loop = -2;
//   mmp->count = 0;
//   return 0;
// }
