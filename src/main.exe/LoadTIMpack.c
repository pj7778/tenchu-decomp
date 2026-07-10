#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * short LoadTIMpack(unsigned long *adr);
 *     3DCTRL.C:759, 39 src lines, frame 80 bytes, saved-reg mask 0x801f0000
 *
 * Original parameters and locals (the demo build's register allocation may
 * differ from retail, but the COUNT and TYPES drive cc1's codegen and carry
 * over). A repeated name is a nested-block scope, not a duplicate:
 *     param $a0       unsigned long * adr
 *     stack sp+16     struct RECT rect
 *     stack sp+24     struct GsIMAGE tim
 * END PSX.SYM */

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/LoadTIMpack", LoadTIMpack);

// triage: EASY — 69 insns, 1 loop, 4 callees, ~0.34 to LoadTIM
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// short LoadTIMpack(ulong *adr)
//
// {
//   ulong uVar1;
//   ulong *puVar2;
//   int iVar3;
//   RECT local_40;
//   GsIMAGE local_38;
//
//   if (adr != (ulong *)0x0) {
//     uVar1 = adr[1];
//     iVar3 = 0;
//     puVar2 = adr + 2;
//     if (0 < (short)uVar1) {
//       do {
//         GsGetTimInfo((ulong *)((int)(adr + 2) + *puVar2 + 4),&local_38);
//         local_40.x = local_38.px;
//         local_40.y = local_38.py;
//         local_40.w = local_38.pw;
//         local_40.h = local_38.ph;
//         LoadImage(&local_40,local_38.pixel);
//         if ((local_38.pmode >> 3 & 1) != 0) {
//           local_40.x = local_38.cx;
//           local_40.y = local_38.cy;
//           local_40.w = local_38.cw;
//           local_40.h = local_38.ch;
//           LoadImage(&local_40,local_38.clut);
//         }
//         iVar3 = iVar3 + 1;
//         puVar2 = puVar2 + 1;
//       } while (iVar3 * 0x10000 >> 0x10 < (int)(short)uVar1);
//     }
//     iVar3 = DrawSync(0);
//     return (short)iVar3;
//   }
//                     /* WARNING: Subroutine does not return */
//   SystemOut((uchar *)"NO IMAGE PACK DATA");
// }
