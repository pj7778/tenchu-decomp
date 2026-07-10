#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/AfsGetEntry", AfsGetEntry);

// triage: MEDIUM — 141 insns, 2 loop, 6 callees, ~0.09 to AfsGetHeader
// likely-relevant cookbook sections:
//   - Loops: 2 back-edge(s) — for/while/do vs goto shape

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// int AfsGetEntry(TAFS *handle)
//
// {
//   int iVar1;
//   TAFSElement *pTVar2;
//   undefined *buffer;
//   ulong uVar3;
//   char *fmt;
//   undefined1 *puVar4;
//   TAFSElement *pTVar5;
//   undefined *puVar6;
//   uint uVar7;
//
//   if (handle->maxElements == 0) {
//     AdtMessageBox("AfsGetEntry: empty index");
//     iVar1 = 0;
//   }
//   else {
//     pTVar2 = (TAFSElement *)valloc(handle->maxElements * 0x24);
//     if (pTVar2 == (TAFSElement *)0x0) {
//       AdtMessageBox("AfsGetEnty: memory not enough!");
//       vfree((undefined *)0x0);
//       iVar1 = 1;
//     }
//     else {
//       buffer = valloc(handle->maxElements * 0x24);
//       if (buffer == (undefined *)0x0) {
//         fmt = "AfsGetEntry: memory not enough!";
// LAB_8005ea08:
//         AdtMessageBox(fmt);
//         iVar1 = 1;
//       }
//       else {
//         uVar7 = 0;
//         cd_seek();
//         cd_read((disc_file_descriptor_t *)handle->fpVol,buffer,handle->maxElements * 0x24);
//         uVar3 = 0;
//         if (handle->maxElements != 0) {
//           puVar4 = buffer + 1;
//           pTVar5 = pTVar2;
//           puVar6 = buffer;
//           do {
//             pTVar5->flag = CONCAT11(puVar4[1],puVar4[2]);
//             pTVar5->pos = (uint)(byte)puVar4[3] << 0x18 | (uint)(byte)puVar4[4] << 0x10 |
//                           (uint)(byte)puVar4[5] << 8 | (uint)(byte)puVar4[6];
//             pTVar5->size = (uint)(byte)puVar4[0xb] << 0x18 | (uint)(byte)puVar4[0xc] << 0x10 |
//                            (uint)(byte)puVar4[0xd] << 8 | (uint)(byte)puVar4[0xe];
//             pTVar5->psize =
//                  (uint)(byte)puVar4[7] << 0x18 | (uint)(byte)puVar4[8] << 0x10 |
//                  (uint)(byte)puVar4[9] << 8 | (uint)(byte)puVar4[10];
//             strncpy((char *)pTVar5->name,puVar6 + 0x10,0x13);
//             pTVar5->name[0x13] = '\0';
//             if (CONCAT11(*puVar6,*puVar4) != 0x4958) {
//               fmt = "Illigal index data\n";
//               goto LAB_8005ea08;
//             }
//             puVar4 = puVar4 + 0x24;
//             puVar6 = puVar6 + 0x24;
//             pTVar5->name[0x13] = '\0';
//             uVar7 = uVar7 + 1;
//             pTVar5 = pTVar5 + 1;
//           } while (uVar7 < handle->maxElements);
//           uVar3 = handle->maxElements;
//         }
//         handle->pElement = pTVar2;
//         handle->maxElementArea = uVar3;
//         vfree(buffer);
//         iVar1 = 0;
//       }
//     }
//   }
//   return iVar1;
// }
