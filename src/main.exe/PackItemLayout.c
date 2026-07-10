#include "common.h"
#include "main.exe.h"

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
