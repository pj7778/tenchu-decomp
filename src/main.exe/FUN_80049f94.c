#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/FUN_80049f94", FUN_80049f94);

// triage: MEDIUM — 102 insns, mul/div, 4 callees, ~0.11 to ProcItemKusuri
// likely-relevant cookbook sections:
//   - Dispatch: if/switch ladder — reload vs CSE, signed vs unsigned
//   - Expressions: mult/div — magic-multiply constants, fold

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void FUN_80049f94(Humanoid *param_1)
//
// {
//   VECTOR *pVVar1;
//   int iVar2;
//   int iVar3;
//   TItemType TVar4;
//   PARAM_ITEM_USE local_48;
//
//   for (TVar4 = ITEM_KAGINAWA; iVar3 = 0, (int)TVar4 < 0x19; TVar4 = TVar4 + ITEM_SHURIKEN) {
//     for (; iVar3 < (int)(uint)param_1->item[TVar4]; iVar3 = iVar3 + 1) {
//       pVVar1 = GetAbsolutePosition(*param_1->model->object,0,0,0);
//       memset((uchar *)&local_48,'\0',0x28);
//       local_48.start.vx = pVVar1->vx;
//       local_48.start.vy = pVVar1->vy;
//       local_48.start.vz = pVVar1->vz;
//       local_48.type = TVar4;
//       local_48.user = param_1;
//       iVar2 = rand();
//       local_48.end.vx = iVar2 % 200 + -100;
//       iVar2 = rand();
//       local_48.end.vy = iVar2 % 100 + -200;
//       iVar2 = rand();
//       local_48.end.vz = iVar2 % 200 + -100;
//       ReqItemDrop(&local_48);
//     }
//     param_1->item[TVar4] = '\0';
//   }
//   return;
// }
