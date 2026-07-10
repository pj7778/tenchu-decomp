#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/LoadMotion", LoadMotion);

// triage: EASY — 60 insns, 2 loop, 1 callees, ~0.11 to LoadAreaMap
// likely-relevant cookbook sections:
//   - Loops: 2 back-edge(s) — for/while/do vs goto shape
//   - gp vs absolute globals: gp-relative smalls — tools/gpsyms.py

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// MotionPackType * LoadMotion(ulong *data)
//
// {
//   int iVar1;
//   int iVar2;
//   byte *pbVar3;
//   int iVar4;
//
//   if (data != (ulong *)0x0) {
//     iVar4 = 0;
//     if (0 < (int)*data) {
//       iVar1 = 0;
//       do {
//         pbVar3 = (byte *)(*(int *)((int)data + (iVar1 >> 0xe) + 4) + (int)data);
//         *(byte **)((int)data + (iVar1 >> 0xe) + 4) = pbVar3;
//         if (*pbVar3 != 0) {
//           iVar1 = 0;
//           *(byte **)(pbVar3 + 8) = pbVar3 + *(int *)(pbVar3 + 8);
//           if (*pbVar3 != 0) {
//             iVar2 = 0;
//             do {
//               iVar1 = iVar1 + 1;
//               *(byte **)(pbVar3 + (iVar2 >> 0xe) + 0xc) =
//                    pbVar3 + *(int *)(pbVar3 + (iVar2 >> 0xe) + 0xc);
//               iVar2 = iVar1 * 0x10000;
//             } while (iVar1 * 0x10000 >> 0x10 < (int)(uint)*pbVar3);
//           }
//         }
//         iVar4 = iVar4 + 1;
//         iVar1 = iVar4 * 0x10000;
//       } while (iVar4 * 0x10000 >> 0x10 < (int)*data);
//     }
//     MotionPack = data;
//     return (MotionPackType *)data;
//   }
//                     /* WARNING: Subroutine does not return */
//   SystemOut("NO MOTION DATA");
// }
