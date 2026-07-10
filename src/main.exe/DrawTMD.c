#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/DrawTMD", DrawTMD);

// triage: TRIVIAL — 42 insns, indirect-call, 0 callees, ~0.04 to AdtFntLoad
// likely-relevant cookbook sections:
//   - Register allocation steering: indirect call — null-check-var/call-field

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// /* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
//
// void DrawTMD(int param_1)
//
// {
//   undefined4 uVar1;
//   uint *puVar2;
//   int iVar3;
//
//   puVar2 = *(uint **)(*(int *)(param_1 + 8) + 0x10);
//   iVar3 = *(int *)(*(int *)(param_1 + 8) + 0x14);
//   uVar1 = GsOUT_PACKET_P;
//   while( true ) {
//     if ((iVar3 < 1) || ((*puVar2 >> 0x10 & 1) == 0)) break;
//     (**(code **)((int)PTR_ARRAY_8001480c + (*puVar2 >> 0x18 & 0x1c) + _DrawTMDmode))();
//   }
//   GsOUT_PACKET_P = uVar1;
//   return;
// }
