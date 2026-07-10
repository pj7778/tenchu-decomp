#include "common.h"
#include "main.exe.h"

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
