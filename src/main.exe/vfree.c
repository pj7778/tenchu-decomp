#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * void vfree(void *pt);
 *     VALLOC.C:216, 27 src lines, frame 32 bytes, saved-reg mask 0x80070000
 *
 * Original parameters and locals (the demo build's register allocation may
 * differ from retail, but the COUNT and TYPES drive cc1's codegen and carry
 * over). A repeated name is a nested-block scope, not a duplicate:
 *     param $a0       void * pt
 *
 * Globals it touches, as the original declared them:
 *     extern unsigned long *virtual_memory_pool;
 * END PSX.SYM */

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/vfree", vfree);

// triage: EASY — 65 insns, 1 loop, 1 callees, ~0.09 to DisposeWeapon
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape
//   - gp vs absolute globals: gp-relative smalls — tools/gpsyms.py

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void vfree(undefined *pt)
//
// {
//   uint uVar1;
//   int *piVar2;
//   uint *puVar3;
//   int *piVar4;
//
//   if (pt != (undefined *)0x0) {
//     if (-1 < *(int *)(pt + -8)) {
//                     /* WARNING: Subroutine does not return */
//       SystemOut((uchar *)"DOUBLE MEMORY RELEASE");
//     }
//     uVar1 = *(uint *)(pt + -8);
//     *(uint *)(pt + -8) = uVar1 & 0x7fffffff;
//     puVar3 = *(uint **)(pt + -4);
//     if (puVar3 != (uint *)0x0) {
//       if ((~*puVar3 & 0x80000000) != 0) {
//         *(uint *)(pt + -8) = (uVar1 & 0x7fffffff) + 2 + *puVar3;
//         *(uint *)(pt + -4) = puVar3[1];
//       }
//     }
//     piVar4 = virtual_memory_pool;
//     if (virtual_memory_pool != (int *)0x0) {
//       do {
//         piVar2 = (int *)piVar4[1];
//         if (piVar2 == (int *)(pt + -8)) break;
//         piVar4 = piVar2;
//       } while (piVar2 != (int *)0x0);
//       if ((piVar4 != (int *)0x0) && (-1 < *piVar4)) {
//         *piVar4 = *piVar4 + 2 + *(int *)(pt + -8);
//         piVar4[1] = *(int *)(pt + -4);
//       }
//     }
//   }
//   return;
// }
