#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/FUN_8004f4f8", FUN_8004f4f8);

// triage: MEDIUM — 40 insns, mul/div, 1 loop, 3 callees, ~0.13 to FUN_80038d10
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape
//   - Expressions: mult/div — magic-multiply constants, fold

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// BackGround * FUN_8004f4f8(ulong *param_1)
//
// {
//   BackGround *pBVar1;
//   int iVar2;
//   GsIMAGE GStack_28;
//
//   GetTIMInfo(param_1,&GStack_28);
//   pBVar1 = SetupBG((BackGround *)&GStack_28,0x140,0xf0);
//   pBVar1->sz = 100;
//   LoadTIM(param_1);
//   iVar2 = 0;
//   if (0 < (int)((uint)(pBVar1->map).ncellw * (uint)(pBVar1->map).ncellh)) {
//     do {
//       pBVar1->index[iVar2] = (ushort)iVar2;
//       iVar2 = iVar2 + 1;
//     } while (iVar2 < (int)((uint)(pBVar1->map).ncellw * (uint)(pBVar1->map).ncellh));
//   }
//   return pBVar1;
// }
