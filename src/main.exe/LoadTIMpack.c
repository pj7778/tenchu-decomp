#include "common.h"
#include "main.exe.h"

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
