#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/FUN_8004a6bc", FUN_8004a6bc);

// triage: EASY — 53 insns, 1 loop, 2 callees, ~0.18 to DisposeWeapon
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void FUN_8004a6bc(void)
//
// {
//   GsIMAGE *pGVar1;
//   undefined4 uVar2;
//   undefined4 *puVar3;
//   int iVar4;
//   int iVar5;
//
//   iVar5 = 0;
//   puVar3 = &DAT_8008e4b4;
//   iVar4 = 0;
//   do {
//     iVar5 = iVar5 + 1;
//     pGVar1 = GetImage((uint)*(byte *)(puVar3 + 1));
//     InitSprite(pGVar1,(GsSPRITE *)((int)&DAT_8008e41c + iVar4));
//     *(undefined2 *)((int)&DAT_8008e434 + iVar4) = 0;
//     *(undefined2 *)((int)&DAT_8008e436 + iVar4) = 0;
//     uVar2 = *puVar3;
//     ((GsSPRITE *)((int)&DAT_8008e41c + iVar4))->attribute = 0x40000000;
//     *(undefined4 *)((int)&DAT_8008e43c + iVar4) = uVar2;
//     pGVar1 = GetImage((uint)*(byte *)((int)puVar3 + 5));
//     InitSprite(pGVar1,(GsSPRITE *)((int)&DAT_8008e440 + iVar4));
//     *(undefined2 *)((int)&DAT_8008e458 + iVar4) = 0;
//     *(undefined2 *)((int)&DAT_8008e45a + iVar4) = 0;
//     uVar2 = *puVar3;
//     puVar3 = puVar3 + 2;
//     ((GsSPRITE *)((int)&DAT_8008e440 + iVar4))->attribute = 0x50000000;
//     *(undefined4 *)((int)&DAT_8008e460 + iVar4) = uVar2;
//     iVar4 = iVar4 + 0x50;
//   } while (iVar5 < 2);
//   return;
// }
