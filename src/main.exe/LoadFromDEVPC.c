#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * unsigned long * LoadFromDEVPC(unsigned char *filename);
 *     FILEIO.C:214, 29 src lines, frame 32 bytes, saved-reg mask 0x80070000
 *
 * Original parameters and locals (the demo build's register allocation may
 * differ from retail, but the COUNT and TYPES drive cc1's codegen and carry
 * over). A repeated name is a nested-block scope, not a duplicate:
 *     param $a0       unsigned char * filename
 *
 * Globals it touches, as the original declared them:
 *     extern int TotalIO;
 *     extern int ReadMode;
 *     extern unsigned long *MemoryLoadAddress;
 * END PSX.SYM */

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/LoadFromDEVPC", LoadFromDEVPC);

// triage: EASY — 64 insns, 6 callees, ~0.20 to FileWrite
// likely-relevant cookbook sections:
//   - gp vs absolute globals: gp-relative smalls — tools/gpsyms.py

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// ulong * LoadFromDEVPC(uchar *filename)
//
// {
//   int fd;
//   ulong size;
//   ulong *buff;
//
//   TotalIO = TotalIO + 1;
//   fd = PCopen((char *)filename,0,0);
//   if ((fd == -1) || (size = PClseek(fd,0,2), (int)size < 1)) {
//     AdtMessageBox("LOAD(PC) ERROR\n%d[%s]",TotalIO,filename);
//     buff = (ulong *)0x0;
//   }
//   else {
//     if ((ReadMode & 4U) != 0) {
//       AdtMessageBox("%$LOAD(PC)\n%d[%s]",TotalIO,filename);
//     }
//     PClseek(fd,0,0);
//     buff = MemoryLoadAddress;
//     if (MemoryLoadAddress == (ulong *)0x0) {
//       buff = (ulong *)valloc(size);
//     }
//     else {
//       MemoryLoadAddress = (ulong *)0x0;
//     }
//     PCread(fd,(char *)buff,size);
//     PCclose(fd);
//   }
//   return buff;
// }
