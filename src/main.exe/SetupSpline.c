#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * void SetupSpline(struct MotionManager *mmp);
 *     ACTION.C:344, 17 src lines, frame 40 bytes, saved-reg mask 0x800f0000
 *
 * Original parameters and locals (the demo build's register allocation may
 * differ from retail, but the COUNT and TYPES drive cc1's codegen and carry
 * over). A repeated name is a nested-block scope, not a duplicate:
 *     param $a0       struct MotionManager * mmp
 * END PSX.SYM */

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
