#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/ResetInfoview", ResetInfoview);

// triage: EASY — 32 insns, 1 loop, 4 callees, ~0.17 to FUN_8004f598
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void ResetInfoview(int stage)
//
// {
//   int iVar1;
//   ulong *adr;
//   int iVar2;
//   GsIMAGE GStack_28;
//
//   iVar2 = 4;
//   iVar1 = -0x7ff3f6e8;
//   do {
//     *(undefined4 *)(iVar1 + 0xc) = 0;
//     iVar2 = iVar2 + -1;
//     iVar1 = iVar1 + -0x14;
//   } while (-1 < iVar2);
//   if (-1 < stage) {
//     adr = PathFileRead(ImagePath,(uchar *)"chizu.tim");
//     GetTIMInfo(adr,&GStack_28);
//     LoadTIMAndFree(adr);
//     InitSprite(&GStack_28,&MapImage);
//   }
//   return;
// }
