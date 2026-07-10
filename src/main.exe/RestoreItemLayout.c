#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/RestoreItemLayout", RestoreItemLayout);

// triage: MEDIUM — 156 insns, indirect-call, 6 callees, ~0.14 to ClearItemLayout
// likely-relevant cookbook sections:
//   - Dispatch: if/switch ladder — reload vs CSE, signed vs unsigned
//   - Register allocation steering: indirect call — null-check-var/call-field

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void RestoreItemLayout(undefined *buf)
//
// {
//   TItemType TVar1;
//   int iVar2;
//   tag_TItem *ptVar3;
//   int iVar4;
//   TItemType TVar5;
//   TItemType x;
//   short *psVar6;
//   int iVar7;
//   PARAM_ITEM_STAY local_58;
//   TItemType local_40;
//   TItemType local_3c;
//   TItemType local_38;
//   TItemType local_34;
//   TItemType local_30;
//
//   ptVar3 = items;
//   for (iVar4 = 0; iVar7 = 0, iVar4 < 0x1e; iVar4 = iVar4 + 1) {
//     if (ptVar3->proc != (undefined **)0x0) {
//       ptVar3->mode = 0xff;
//       (*(code *)ptVar3->proc)(ptVar3);
//       DeleteConflict(ptVar3->locate);
//       if (ptVar3->mode != 0) {
//         AdtMessageBox("item dispose fail   id %d  mode %d",ptVar3->type,(uint)ptVar3->mode);
//       }
//       ptVar3->owner = (Humanoid *)0x0;
//       ptVar3->proc = (undefined **)0x0;
//     }
//     ptVar3 = ptVar3 + 1;
//   }
//   do {
//     if (0x1d < iVar7) {
//       return;
//     }
//     if (*(TItemType *)buf != ~ITEM_KAGINAWA) {
//       memset((uchar *)&local_40,'\0',0x14);
//       local_58.type = *(TItemType *)buf;
//       local_58.locate.vx = *(TItemType *)((int)buf + 4);
//       local_58.locate.vy = *(TItemType *)((int)buf + 8);
//       local_58.locate.vz = *(TItemType *)((int)buf + 0xc);
//       local_58.locate.pad = *(TItemType *)((int)buf + 0x10);
//       local_40 = local_58.type;
//       local_3c = local_58.locate.vx;
//       local_38 = local_58.locate.vy;
//       local_34 = local_58.locate.vz;
//       local_30 = local_58.locate.pad;
//       TVar1 = GetAreaMapLevel(GlobalAreaMap,local_58.locate.vx,local_58.locate.vy);
//       if ((TVar1 == 0x80000000) || (iVar4 = abs(TVar1 - local_58.locate.vy), 999 < iVar4)) {
//         psVar6 = &DAT_8008e404;
//         for (iVar4 = 0; x = local_58.locate.vx, TVar5 = local_58.locate.vz, iVar4 < 4;
//             iVar4 = iVar4 + 1) {
//           x = local_58.locate.vx + *psVar6 * 1000;
//           TVar5 = local_58.locate.vz + psVar6[1] * 1000;
//           TVar1 = GetAreaMapLevel(GlobalAreaMap,x,local_58.locate.vy);
//           if ((TVar1 != 0x80000000) && (iVar2 = abs(TVar1 - local_58.locate.vy), iVar2 < 1000))
//           break;
//           psVar6 = psVar6 + 2;
//         }
//         local_58.locate.vz = TVar5;
//         local_58.locate.vx = x;
//         if (iVar4 == 4) goto LAB_8003d39c;
//       }
//       local_58.locate.vy = TVar1;
//       ReqItemStay(&local_58);
//     }
// LAB_8003d39c:
//     buf = (undefined *)((int)buf + 0x14);
//     iVar7 = iVar7 + 1;
//   } while( true );
// }
