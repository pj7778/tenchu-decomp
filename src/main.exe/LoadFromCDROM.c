#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/LoadFromCDROM", LoadFromCDROM);

// triage: EASY — 70 insns, 7 callees, ~0.13 to DoBriefingAndInventorySelection
// likely-relevant cookbook sections:
//   - gp vs absolute globals: gp-relative smalls — tools/gpsyms.py

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// ulong * LoadFromCDROM(uchar *path)
//
// {
//   undefined4 uVar1;
//   TAFSElement *element;
//   ulong size;
//   ulong *buffer;
//
//   TotalIO = TotalIO + 1;
//   uVar1 = AdtQuiet(0);
//   element = AfsOpen(&systemAFS,(char *)path);
//   if (element == (TAFSElement *)0x0) {
//     AdtQuiet(uVar1);
//     AdtMessageBox("LOAD(CD) ERROR\n%d[%s]",TotalIO,path);
//     buffer = (ulong *)0x0;
//   }
//   else {
//     if ((ReadMode & 4U) != 0) {
//       AdtMessageBox("%$LOAD(CD)\n%d[%s]",TotalIO,path);
//     }
//     size = AfsFileSize(&systemAFS,(TAFSElement *)element);
//     buffer = MemoryLoadAddress;
//     if (MemoryLoadAddress == (ulong *)0x0) {
//       buffer = (ulong *)valloc(size);
//     }
//     else {
//       MemoryLoadAddress = (ulong *)0x0;
//     }
//     AfsRead(&systemAFS,(TAFSElement *)element,(undefined *)buffer,size);
//     AfsClose((TAFSElement *)element);
//     AdtQuiet(uVar1);
//   }
//   return buffer;
// }
