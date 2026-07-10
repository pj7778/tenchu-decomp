#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/FileRead", FileRead);

// triage: EASY — 57 insns, 7 callees, ~0.08 to LoadTIMAndFree
// likely-relevant cookbook sections:
//   - Dispatch: if/switch ladder — reload vs CSE, signed vs unsigned
//   - gp vs absolute globals: gp-relative smalls — tools/gpsyms.py

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// ulong * FileRead(char *path)
//
// {
//   uint uVar1;
//   code *f;
//   ulong *puVar2;
//
//   CdaStop();
//   if (AccessPower < 0) {
//     f = (f *)0x0;
//   }
//   else {
//     AccessPower = 0;
//     f = cbAccess;
//   }
//   VSyncCallback(f);
//   if (ReadMode == -1) {
//     TotalIO = 0;
//     ReadMode = 0;
//     PCinit();
//   }
//   uVar1 = ReadMode & 3;
//   if (uVar1 == 1) {
//     puVar2 = LoadFromMEMORY((uchar *)path);
//   }
//   else if (uVar1 < 2) {
//     if (uVar1 == 0) {
//       puVar2 = LoadFromDEVPC((uchar *)path);
//     }
//     else {
//       puVar2 = (ulong *)0x0;
//     }
//   }
//   else if (uVar1 == 2) {
//     puVar2 = LoadFromCDROM((uchar *)path);
//   }
//   else {
//     puVar2 = (ulong *)0x0;
//   }
//   FUN_80018f00();
//   return puVar2;
// }
