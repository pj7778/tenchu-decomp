#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/AttackCancelControl", AttackCancelControl);

// triage: EASY — 95 insns, 2 callees, ~0.08 to ProcItemGoshikimai
// likely-relevant cookbook sections:
//   - Dispatch: if/switch ladder — reload vs CSE, signed vs unsigned
//   - gp vs absolute globals: gp-relative smalls — tools/gpsyms.py

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void AttackCancelControl(short mode)
//
// {
//   short sVar1;
//   ModelType *model;
//
//   if ((mode & 1U) == 0) goto LAB_8002746c;
//   sVar1 = Me_MOTION_C->wpatk;
//   if (sVar1 == 2) {
//     DeleteConflict(Me_MOTION_C->model->object[8]);
//     model = Me_MOTION_C->model->object[0xb];
//   }
//   else {
//     if (sVar1 < 3) {
//       if (sVar1 == 0) goto LAB_8002746c;
//     }
//     else if (sVar1 == 3) {
//       model = Me_MOTION_C->model->object[2];
//       goto LAB_80027460;
//     }
//     DeleteConflict(Me_MOTION_C->model->object[0xd]);
//     model = Me_MOTION_C->model->object[0xe];
//   }
// LAB_80027460:
//   DeleteConflict(model);
// LAB_8002746c:
//   if ((mode & 2U) != 0) {
//     if ((AfterimageType *)Me_MOTION_C->illusion[0] != (AfterimageType *)0x0) {
//       DisposeAfterimage((AfterimageType *)Me_MOTION_C->illusion[0]);
//       Me_MOTION_C->illusion[0] = (undefined *)0x0;
//     }
//     if ((AfterimageType *)Me_MOTION_C->illusion[1] != (AfterimageType *)0x0) {
//       DisposeAfterimage((AfterimageType *)Me_MOTION_C->illusion[1]);
//       Me_MOTION_C->illusion[1] = (undefined *)0x0;
//     }
//   }
//   dtM->mask = 0x7fff;
//   return;
// }
