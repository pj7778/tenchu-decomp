#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * void LoadExecEx(unsigned char *file, unsigned long stack, unsigned long size);
 *     INFOVIEW.C:1173, 17 src lines, frame 32 bytes, saved-reg mask 0x80070000
 *
 * Original parameters and locals (the demo build's register allocation may
 * differ from retail, but the COUNT and TYPES drive cc1's codegen and carry
 * over). A repeated name is a nested-block scope, not a duplicate:
 *     param $a0       unsigned char * file
 *     param $a1       unsigned long stack
 *     param $a2       unsigned long size
 * END PSX.SYM */

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
