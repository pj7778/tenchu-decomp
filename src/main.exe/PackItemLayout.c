#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * void PackItemLayout(void *buf, long size);
 *     ITEM.C:473, 30 src lines, frame 224 bytes, saved-reg mask 0x80000000
 *
 * Original parameters and locals (the demo build's register allocation may
 * differ from retail, but the COUNT and TYPES drive cc1's codegen and carry
 * over). A repeated name is a nested-block scope, not a duplicate:
 *     param $a0       void * buf
 *     param $a1       long size
 *     stack sp+16     unsigned char [200] fn
 *     reg   $a2       int i
 *
 * Globals it touches, as the original declared them:
 *     extern struct tag_TItem items[30];
 * END PSX.SYM */

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/PackItemLayout", PackItemLayout);

// triage: EASY — 46 insns, 1 loop, 1 callees, ~0.20 to lePackEnemyLayout
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void PackItemLayout(undefined *buf,long size)
//
// {
//   tag_TItem *ptVar1;
//   int iVar2;
//
//   if ((uint)size < 600) {
//     AdtMessageBox("item storing size too small %d/%d",size,600);
//   }
//   else {
//     iVar2 = 0;
//     ptVar1 = items;
//     do {
//       if (ptVar1->proc == (undefined **)0x0) {
//         *(TItemType *)buf = ~ITEM_KAGINAWA;
//       }
//       else {
//         *(TItemType *)buf = ptVar1->type;
//         *(long *)((int)buf + 4) = (ptVar1->locate->locate).coord.t[0];
//         *(long *)((int)buf + 8) = (ptVar1->locate->locate).coord.t[1];
//         *(long *)((int)buf + 0xc) = (ptVar1->locate->locate).coord.t[2];
//       }
//       buf = (undefined *)((int)buf + 0x14);
//       iVar2 = iVar2 + 1;
//       ptVar1 = ptVar1 + 1;
//     } while (iVar2 < 0x1e);
//   }
//   return;
// }
