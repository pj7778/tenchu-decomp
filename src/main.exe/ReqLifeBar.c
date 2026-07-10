#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * int ReqLifeBar(struct Humanoid *h);
 *     INFOVIEW.C:89, 26 src lines, frame 0 bytes, saved-reg mask 0x00000000
 *
 * Original parameters and locals (the demo build's register allocation may
 * differ from retail, but the COUNT and TYPES drive cc1's codegen and carry
 * over). A repeated name is a nested-block scope, not a duplicate:
 *     param $a0       struct Humanoid * h
 *     reg   $a1       int i
 *     reg   $a2       int g
 *
 * Globals it touches, as the original declared them:
 *     extern struct INFOVIEW__198fake LifeBar[4];
 * END PSX.SYM */

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/ReqLifeBar", ReqLifeBar);

// triage: EASY — 60 insns, 1 loop, 0 callees, ~0.05 to leResetPath
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// int ReqLifeBar(Humanoid *h)
//
// {
//   _198fake_22 *p_Var1;
//   int iVar2;
//   int iVar3;
//   int iVar4;
//
//   iVar3 = -1;
//   iVar2 = 0;
//   p_Var1 = LifeBar;
//   do {
//     if (p_Var1->count < 1) {
//       if (iVar3 == -1) {
//         iVar3 = iVar2;
//       }
//     }
//     else {
//       iVar4 = iVar2;
//       if (p_Var1->target == h) break;
//     }
//     iVar2 = iVar2 + 1;
//     p_Var1 = p_Var1 + 1;
//     iVar4 = iVar3;
//   } while (iVar2 < 5);
//   if (iVar4 == -1) {
//     return 0;
//   }
//   LifeBar[iVar4].target = h;
//   LifeBar[iVar4].style = 1;
//   LifeBar[iVar4].life = (int)h->life;
//   LifeBar[iVar4].max = (int)h->lifemax;
//   iVar3 = 300;
//   if (h->life == 0) {
//     iVar3 = 100;
//   }
//   LifeBar[iVar4].count = iVar3;
//   if (LifeBar[iVar4].max < 1) {
//     LifeBar[iVar4].max = 1;
//   }
//   return 1;
// }
