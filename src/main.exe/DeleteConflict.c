#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/DeleteConflict", DeleteConflict);

// triage: EASY — 67 insns, 2 loop, 0 callees, ~0.08 to PlayerOption
// likely-relevant cookbook sections:
//   - Loops: 2 back-edge(s) — for/while/do vs goto shape
//   - gp vs absolute globals: gp-relative smalls — tools/gpsyms.py

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void DeleteConflict(ModelType *model)
//
// {
//   short sVar1;
//   int iVar2;
//   ConflictObjectType *pCVar3;
//   ConflictObjectType *pCVar4;
//   short sVar5;
//   ConflictObjectType *pCVar6;
//   ConflictObjectType *pCVar7;
//   ushort uVar8;
//   long lVar9;
//   long lVar10;
//   long lVar11;
//
//   if (model->id != -1) {
//     uVar8 = 0;
//     if (0 < ConflictObjects) {
//       iVar2 = 0;
//       do {
//         sVar5 = ConflictObjects + -1;
//         if (ConflictObject[iVar2 >> 0x10].model == model) {
//           pCVar4 = ConflictObject + sVar5;
//           pCVar7 = ConflictObject + (iVar2 >> 0x10);
//           ConflictObjects = sVar5;
//           do {
//             pCVar6 = pCVar7;
//             pCVar3 = pCVar4;
//             lVar9 = (pCVar3->position).vx;
//             lVar10 = (pCVar3->position).vy;
//             lVar11 = (pCVar3->position).vz;
//             pCVar6->model = pCVar3->model;
//             (pCVar6->position).vx = lVar9;
//             (pCVar6->position).vy = lVar10;
//             (pCVar6->position).vz = lVar11;
//             pCVar4 = (ConflictObjectType *)&(pCVar3->position).pad;
//             pCVar7 = (ConflictObjectType *)&(pCVar6->position).pad;
//           } while (pCVar4 != (ConflictObjectType *)&ConflictObject[sVar5].field14_0x70);
//           sVar5 = (pCVar3->offset).vx;
//           sVar1 = (pCVar3->offset).vy;
//           *(long *)pCVar7 = *(long *)pCVar4;
//           (pCVar6->offset).vx = sVar5;
//           (pCVar6->offset).vy = sVar1;
//           (ConflictObject[(short)uVar8].model)->id = uVar8;
//         }
//         else {
//           uVar8 = uVar8 + 1;
//         }
//         iVar2 = (uint)uVar8 << 0x10;
//       } while ((short)uVar8 < ConflictObjects);
//     }
//     model->id = -1;
//     model->attribute = model->attribute & 0x3fff;
//   }
//   return;
// }
