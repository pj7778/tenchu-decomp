#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/ProcSightShot", ProcSightShot);

// triage: MEDIUM — 264 insns, mul/div, indirect-call, 11 callees, ~0.20 to ProcItemKusuri
// likely-relevant cookbook sections:
//   - Expressions: mult/div — magic-multiply constants, fold
//   - Register allocation steering: indirect call — null-check-var/call-field

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void ProcSightShot(tag_TItem *item)
//
// {
//   uchar uVar1;
//   VECTOR *pVVar2;
//   int iVar3;
//   Humanoid *pHVar4;
//   TItemType TVar5;
//   PARAM_ITEM_USE local_50;
//   SVECTOR local_28;
//   short local_20 [2];
//   short local_1c [2];
//
//   if (item->mode == 0xff) {
//     item->owner->field_0xcd = 0;
//     item->mode = '\0';
//   }
//   else {
//     pHVar4 = item->owner;
//     if (pHVar4->field_0xcd == '\0') {
//       if (pHVar4->item[item->type] != 0xff) {
//         pHVar4->item[item->type] = pHVar4->item[item->type] + '\x01';
//       }
//       SetCameraMode(CMODE_DIRECTION);
//     }
//     else {
//       if (pHVar4->motion->mid == 0xe00) {
//         uVar1 = (item->param).launch.count;
//         if (uVar1 != '\0') {
//           (item->param).launch.count = uVar1 + 0xff;
//         }
//         if (((item->owner->pad).data & 0x10) != 0) {
//           if ((item->param).launch.count != '\0') {
//             return;
//           }
//           SetCameraMode(CMODE_SIGHT);
//           GsSortSprite(TargetSprite,OTablePt,0);
//           return;
//         }
//         if ((item->param).launch.count == '\0') {
//           local_50.type = item->type;
//           local_50.user = item->owner;
//           local_50.start.vx = (item->locate->locate).coord.t[0];
//           local_50.start.vy = (item->locate->locate).coord.t[1];
//           local_50.start.vz = (item->locate->locate).coord.t[2];
//           GetVectorRotation((VECTOR *)&ViewInfo,(VECTOR *)&ViewInfo.vrx,(int *)local_20,
//                             (int *)local_1c);
//           local_28.vz = 0;
//           local_28.vx = local_20[0];
//           local_28.vy = local_1c[0];
//           SearchItemTarget2(local_50.user,&local_28,(VECTOR *)&ViewInfo,&local_50.end);
//           if (item->proc != (undefined **)0x0) {
//             item->mode = 0xff;
//             (*(code *)item->proc)(item);
//             DeleteConflict(item->locate);
//             if (item->mode != 0) {
//               AdtMessageBox("item dispose fail   id %d  mode %d",item->type,(uint)item->mode);
//             }
//             item->owner = (Humanoid *)0x0;
//             item->proc = (undefined **)0x0;
//           }
//           SetCameraMode(CMODE_LOCK);
//         }
//         else {
//           local_50.type = item->type;
//           local_50.user = item->owner;
//           local_50.start.vx = (item->locate->locate).coord.t[0];
//           local_50.start.vy = (item->locate->locate).coord.t[1];
//           local_50.start.vz = (item->locate->locate).coord.t[2];
//           SearchItemTarget2(local_50.user,&item->owner->model->rotate,&local_50.start,&local_50.end)
//           ;
//           if (item->proc != (undefined **)0x0) {
//             item->mode = 0xff;
//             (*(code *)item->proc)(item);
//             DeleteConflict(item->locate);
//             if (item->mode != 0) {
//               AdtMessageBox("item dispose fail   id %d  mode %d",item->type,(uint)item->mode);
//             }
//             item->owner = (Humanoid *)0x0;
//             item->proc = (undefined **)0x0;
//           }
//         }
//         ReqItemLaunch(&local_50);
//         return;
//       }
//       pVVar2 = GetAbsolutePosition(item->locate,0,0,0);
//       pHVar4 = item->owner;
//       TVar5 = item->type;
//       memset((uchar *)&local_50,'\0',0x28);
//       local_50.start.vx = pVVar2->vx;
//       local_50.start.vy = pVVar2->vy;
//       local_50.start.vz = pVVar2->vz;
//       local_50.type = TVar5;
//       local_50.user = pHVar4;
//       iVar3 = rand();
//       local_50.end.vx = iVar3 % 200 + -100;
//       iVar3 = rand();
//       local_50.end.vy = iVar3 % 100 + -200;
//       iVar3 = rand();
//       local_50.end.vz = iVar3 % 200 + -100;
//       ReqItemDrop(&local_50);
//     }
//     if (item->proc != (undefined **)0x0) {
//       item->mode = 0xff;
//       (*(code *)item->proc)(item);
//       DeleteConflict(item->locate);
//       if (item->mode != 0) {
//         AdtMessageBox("item dispose fail   id %d  mode %d",item->type,(uint)item->mode);
//       }
//       item->owner = (Humanoid *)0x0;
//       item->proc = (undefined **)0x0;
//     }
//   }
//   return;
// }
