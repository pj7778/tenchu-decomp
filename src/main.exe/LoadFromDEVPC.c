#include "common.h"
#include "main.exe.h"

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
