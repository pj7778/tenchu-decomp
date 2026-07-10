#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * unsigned long * LoadFromCDROM(unsigned char *filename);
 *     FILEIO.C:296, 31 src lines, frame 40 bytes, saved-reg mask 0x800f0000
 *
 * Original parameters and locals (the demo build's register allocation may
 * differ from retail, but the COUNT and TYPES drive cc1's codegen and carry
 * over). A repeated name is a nested-block scope, not a duplicate:
 *     param $a0       unsigned char * filename
 *
 * Globals it touches, as the original declared them:
 *     extern int TotalIO;
 *     extern struct TAFS systemAFS;
 *     extern int ReadMode;
 *     extern unsigned long *MemoryLoadAddress;
 * END PSX.SYM */

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
