#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * static void ProcMiscSprite(struct tag_TMisc *m, enum TMiscMessage msg);
 *     MISC.C:582, 50 src lines, frame 32 bytes, saved-reg mask 0x80030000
 *
 * Original parameters and locals (the demo build's register allocation may
 * differ from retail, but the COUNT and TYPES drive cc1's codegen and carry
 * over). A repeated name is a nested-block scope, not a duplicate:
 *     param $s1       struct tag_TMisc * m
 *     param $a1       enum TMiscMessage msg
 *     reg   $v1       int type
 *     reg   $s0       struct Sprite3D * s
 * END PSX.SYM */

INCLUDE_ASM(".shake/gen/main.exe/asm/nonmatchings/ProcMiscSprite", ProcMiscSprite);
INCLUDE_ASM(".shake/gen/main.exe/asm/nonmatchings/ProcMiscSprite", debug_menu_sprite_spawner__override__prt_8004d8b8_394600bf);

// triage: EASY — 63 insns, mul/div, 4 callees, ~0.10 to DisposeOrnamentArchive
// likely-relevant cookbook sections:
//   - Dispatch: if/switch ladder — reload vs CSE, signed vs unsigned
//   - Expressions: mult/div — magic-multiply constants, fold

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void ProcMiscSprite(int m,TMiscMessage msg)
//
// {
//   uchar uVar1;
//   int iVar2;
//   Sprite3D *sprt;
//
//   if (msg == MM_CREATE) {
//     iVar2 = *(int *)(m + 0x18);
//     if (1 < iVar2) {
//       AdtMessageBox("unknown sprite type");
//       iVar2 = 0;
//     }
//     *(undefined1 *)(m + 0x15) = 0;
//     *(char *)(m + 0x18) = (char)iVar2;
//   }
//   else if (MM_RESUME < msg) {
//     sprt = SpriteData[*(byte *)(m + 0x18)].spr;
//     iVar2 = rand();
//     uVar1 = (char)iVar2 + (char)(iVar2 / 0x3c) * -0x3c + 'b';
//     (sprt->sprite).r = uVar1;
//     (sprt->sprite).g = uVar1;
//     (sprt->sprite).b = uVar1;
//     (sprt->locate).coord.t[0] = *(long *)(m + 4);
//     (sprt->locate).coord.t[1] = *(long *)(m + 8);
//     (sprt->locate).coord.t[2] = *(long *)(m + 0xc);
//     UpdateCoordinate((ModelType *)sprt);
//     DrawSprite(sprt);
//   }
//   return;
// }
