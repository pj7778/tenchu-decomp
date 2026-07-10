#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/InitPadControl", InitPadControl);

// triage: EASY — 40 insns, 1 loop, 7 callees, ~0.08 to DoBriefingAndInventorySelection
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// /* WARNING: Unknown calling convention -- yet parameter storage is locked */
//
// void InitPadControl(void)
//
// {
//   int iVar1;
//
//   MemCardInit(0);
//   MemCardStart();
//   memset(ComBuf[0],'\0',0x44);
//   memset(&PadPort,'\0',0x70);
//   PadInitDirect(ComBuf[0],ComBuf[0x11]);
//   PadStartCom();
//   iVar1 = 0xf;
//   if ((PersistentState._71_1_ & 1) != 0) {
//     do {
//       VSync(0);
//       iVar1 = iVar1 + -1;
//     } while (0 < iVar1);
//     PadSetMainMode(0,1,0);
//   }
//   return;
// }
