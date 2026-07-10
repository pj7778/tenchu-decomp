#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/LoadExecEx", LoadExecEx);

// triage: EASY — 43 insns, 12 callees, ~0.19 to DoBriefingAndInventorySelection

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void LoadExecEx(uchar *file,ulong stack,ulong size)
//
// {
//   FUN_8001b2b8();
//   CdaStop();
//   SsEnd();
//   FUN_8006ebe4();
//   ResetGraph(3);
//   PadStopCom();
//   MemCardStop();
//   MemCardEnd();
//   StopCallback();
//   FUN_8005e8f0(file,stack,size);
//   CdInit();
//   FUN_8005e834("\\TENCHU\\RUN.EXE;1",&DAT_801ffff0,0);
//   return;
// }
