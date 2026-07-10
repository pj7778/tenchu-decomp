#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * void DoItemProc(void);
 *     ITEM.C:3205, 52 src lines, frame 32 bytes, saved-reg mask 0x80030000
 *
 * Globals it touches, as the original declared them:
 *     extern int StageID;
 *     extern long GameClock;
 *     extern struct tag_TItem items[30];
 * END PSX.SYM */

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/DoItemProc", DoItemProc);

// triage: EASY — 43 insns, mul/div, indirect-call, 2 callees, ~0.13 to DrawEffect
// likely-relevant cookbook sections:
//   - Dispatch: if/switch ladder — reload vs CSE, signed vs unsigned
//   - Expressions: mult/div — magic-multiply constants, fold
//   - gp vs absolute globals: gp-relative smalls — tools/gpsyms.py
//   - Register allocation steering: indirect call — null-check-var/call-field

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// /* WARNING: Unknown calling convention -- yet parameter storage is locked */
//
// void DoItemProc(void)
//
// {
//   tag_TItem *ptVar1;
//   int iVar2;
//
//   if (DAT_80097ac8 == '\0') {
//     InitializeItem();
//   }
//   iVar2 = 0;
//   if (GameClock == (GameClock / 10) * 10) {
//     UpdateItemState();
//   }
//   ptVar1 = items;
//   for (; iVar2 < 0x1e; iVar2 = iVar2 + 1) {
//     if (ptVar1->proc != (undefined **)0x0) {
//       (*(code *)ptVar1->proc)(ptVar1);
//     }
//     ptVar1 = ptVar1 + 1;
//   }
//   return;
// }
