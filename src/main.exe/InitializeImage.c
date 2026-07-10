#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * void InitializeImage(void);
 *     IMAGES.C:32, 16 src lines, frame 40 bytes, saved-reg mask 0x800f0000
 *
 * Globals it touches, as the original declared them:
 *     extern struct GsIMAGE Images[52];
 * END PSX.SYM */

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/InitializeImage", InitializeImage);

// triage: EASY — 43 insns, 1 loop, 6 callees, ~0.12 to FileWrite
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void InitializeImage(void)
//
// {
//   ulong *pt;
//   ulong *adr;
//   int iVar1;
//   GsIMAGE *image;
//
//   pt = FileRead("K:\\WORK\\CDIMAGE\\IMAGE\\images.arc");
//   if ((short)*pt < 0x3e) {
//     AdtMessageBox("bad image file");
//   }
//   iVar1 = 0;
//   image = Images;
//   do {
//     adr = (ulong *)FUN_8004f1d8(pt,iVar1);
//     GetTIMInfo(adr,image);
//     LoadTIM(adr);
//     iVar1 = iVar1 + 1;
//     image = image + 1;
//   } while (iVar1 < 0x3e);
//   vfree((undefined *)pt);
//   return;
// }
