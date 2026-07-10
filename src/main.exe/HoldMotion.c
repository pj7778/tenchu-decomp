#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * short HoldMotion(struct MotionManager *mmp);
 *     ACTION.C:242, 26 src lines, frame 32 bytes, saved-reg mask 0x80070000
 *
 * Original parameters and locals (the demo build's register allocation may
 * differ from retail, but the COUNT and TYPES drive cc1's codegen and carry
 * over). A repeated name is a nested-block scope, not a duplicate:
 *     param $s1       struct MotionManager * mmp
 *     reg   $s2       struct MotionDataType * mot
 *     reg   $a1       struct ModelType * object
 *     reg   $s0       short i
 * END PSX.SYM */

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
