#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * void PutNumber(int x, int y, int cols, int n);
 *     INFOVIEW.C:197, 17 src lines, frame 32 bytes, saved-reg mask 0x80070000
 *
 * Original parameters and locals (the demo build's register allocation may
 * differ from retail, but the COUNT and TYPES drive cc1's codegen and carry
 * over). A repeated name is a nested-block scope, not a duplicate:
 *     param $a0       int x
 *     param $a1       int y
 *     param $a2       int cols
 *     param $a3       int n
 *
 * Globals it touches, as the original declared them:
 *     extern struct GsSPRITE NumberImage;
 *     extern struct GsOT *OTablePt;
 * END PSX.SYM */

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/PutNumber", PutNumber);

// triage: EASY — 44 insns, mul/div, 1 loop, 1 callees, ~0.09 to SetupMotionRegist
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape
//   - Expressions: mult/div — magic-multiply constants, fold

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void PutNumber(int x,int y,int cols,int n)
//
// {
//   int iVar1;
//   uchar uVar2;
//
//   uVar2 = NumberImage.u;
//   NumberImage.w = 4;
//   NumberImage.x = (short)x;
//   NumberImage.y = (short)y;
//   do {
//     iVar1 = cols / 10;
//     NumberImage.u = uVar2 + ((char)cols + (char)iVar1 * -10) * '\x04';
//     GsSortSprite(&NumberImage,OTablePt,0);
//     NumberImage.x = NumberImage.x + -6;
//     cols = iVar1;
//   } while (iVar1 != 0);
//   NumberImage.u = uVar2;
//   return;
// }
