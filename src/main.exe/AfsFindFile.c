#include "common.h"
#include "main.exe.h"

INCLUDE_ASM(".shake/gen/main.exe/asm/nonmatchings/AfsFindFile", AfsFindFile);
INCLUDE_ASM(".shake/gen/main.exe/asm/nonmatchings/AfsFindFile", FUN_8005eb84__override__prt_8005ecbc_8cf8befb);

// triage: MEDIUM — 140 insns, 4 loop, 5 callees, ~0.07 to AfsFilenameFix
// likely-relevant cookbook sections:
//   - Loops: 4 back-edge(s) — for/while/do vs goto shape

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// TAFSElement * AfsFindFile(TAFS *handle,char *path,ushort flags)
//
// {
//   char cVar1;
//   int iVar2;
//   TAFSElement *pTVar3;
//   char *cursorPath;
//   int entryOffset;
//   uint entryIndex;
//   int cursor;
//   char buffer [199];
//   undefined1 local_f1;
//   char component [200];
//
//   strncpy(buffer,path,199);
//   cursorPath = buffer;
//   local_f1 = 0;
//   cVar1 = buffer[0];
//   while (cVar1 != '\0') {
//     cVar1 = toupper(*cursorPath);
//     *cursorPath = cVar1;
//     cursorPath = cursorPath + 1;
//     cVar1 = *cursorPath;
//   }
//   cursor = 0;
//   if (buffer[0] != '\0') {
//     cursorPath = buffer;
//     do {
//       entryIndex = 0;
//       if (*cursorPath == '\\') {
//         *cursorPath = '\0';
//         if (handle->maxElements != 0) {
//           entryOffset = 0;
//           do {
//             iVar2 = strncmp(buffer,(char *)(handle->pElement->name + entryOffset),0x14);
//             if ((iVar2 == 0) &&
//                ((*(ushort *)(handle->pElement->name + entryOffset + -0x10) & 2) != 0))
//             goto LAB_8005ec98;
//             entryIndex = entryIndex + 1;
//             entryOffset = entryOffset + 0x24;
//           } while (entryIndex < handle->maxElements);
//         }
//         entryIndex = 0xffffffff;
// LAB_8005ec98:
//         if ((int)entryIndex < 0) goto LAB_8005ed68;
//         strcpy(component,buffer + cursor + 1);
//         sprintf(buffer,"@%d_%.195s",entryIndex,component);
//         cursor = 0;
//       }
//       else {
//         cursor = cursor + 1;
//       }
//       cursorPath = buffer + cursor;
//     } while (buffer[cursor] != '\0');
//   }
//   entryIndex = 0;
//   if (handle->maxElements != 0) {
//     cursor = 0;
//     do {
//       entryOffset = strncmp(buffer,(char *)(handle->pElement->name + cursor),0x14);
//       if ((entryOffset == 0) &&
//          ((flags & *(ushort *)(handle->pElement->name + cursor + -0x10)) != 0)) goto LAB_8005ed60;
//       entryIndex = entryIndex + 1;
//       cursor = cursor + 0x24;
//     } while (entryIndex < handle->maxElements);
//   }
//   entryIndex = 0xffffffff;
// LAB_8005ed60:
//   if ((int)entryIndex < 0) {
// LAB_8005ed68:
//     pTVar3 = (TAFSElement *)0x0;
//   }
//   else {
//     pTVar3 = handle->pElement + entryIndex;
//   }
//   return pTVar3;
// }
