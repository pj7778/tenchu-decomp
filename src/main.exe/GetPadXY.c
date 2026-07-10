#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * void GetPadXY(short no, short *x, short *y);
 *     PADCMD.C:285, 6 src lines, frame 8 bytes, saved-reg mask 0x00000000
 *
 * Original parameters and locals (the demo build's register allocation may
 * differ from retail, but the COUNT and TYPES drive cc1's codegen and carry
 * over). A repeated name is a nested-block scope, not a duplicate:
 *     param $a0       short no
 *     param $a1       short * x
 *     param $a2       short * y
 *
 * Globals it touches, as the original declared them:
 *     extern struct TPadPort PadPort[2][4];
 * END PSX.SYM */

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/GetPadXY", GetPadXY);

// triage: HARD — 15 insns, SIGNEXT-SPLIT (GetPad class — likely unmatchable), 0 callees, ~0.22 to FUN_8001b4e0
// likely-relevant cookbook sections:
//   - Toolchain gotchas: sll16/sra-split sign-extension — no natural-C form; see GetPad.c

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void GetPadXY(short no,short *x,short *y)
//
// {
//   *x = *(short *)(&DAT_800be6d2 + no * 0x38);
//   *y = *(short *)(&DAT_800be6d4 + no * 0x38);
//   return;
// }
