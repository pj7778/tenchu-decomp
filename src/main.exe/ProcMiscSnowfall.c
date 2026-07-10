#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/ProcMiscSnowfall", ProcMiscSnowfall);

// triage: MEDIUM — 155 insns, mul/div, 3 callees, ~0.14 to ProcItemKusuri
// likely-relevant cookbook sections:
//   - Dispatch: if/switch ladder — reload vs CSE, signed vs unsigned
//   - Expressions: mult/div — magic-multiply constants, fold

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void ProcMiscSnowfall(tag_TMisc *m,TMiscMessage msg)
//
// {
//   int iVar1;
//   undefined4 local_40;
//   undefined4 local_3c;
//   undefined4 local_38;
//   undefined4 local_34;
//   int local_30;
//   int local_2c;
//   int local_28;
//   undefined4 local_24;
//   int local_20;
//   int local_1c;
//   int local_18;
//   undefined4 local_14;
//
//   if (msg == MM_CREATE) {
//     iVar1 = (m->param).init.b;
//     m->mode = '\0';
//     (m->param).init.b = iVar1;
//   }
//   else if ((MM_RESUME < msg) && ((GameClock & 3U) == 0)) {
//     memset((uchar *)&local_38,'\0',8);
//     iVar1 = rand();
//     local_38 = CONCAT22(local_38._2_2_,(short)iVar1 + (short)(iVar1 / 0x14) * -0x14 + -10);
//     iVar1 = rand();
//     local_38 = CONCAT22((short)iVar1 + (short)(iVar1 / 0x32) * -0x32 + 0x32,(undefined2)local_38);
//     iVar1 = rand();
//     local_34 = CONCAT22(local_34._2_2_,(short)iVar1 + (short)(iVar1 / 0x14) * -0x14 + -10);
//     local_40 = local_38;
//     local_3c = local_34;
//     memset((uchar *)&local_20,'\0',0x10);
//     iVar1 = rand();
//     local_20 = ViewInfo.vrx + -3000 + iVar1 % 6000;
//     iVar1 = rand();
//     local_1c = ViewInfo.vry + -6000 + iVar1 % 3000;
//     iVar1 = rand();
//     local_28 = ViewInfo.vrz + -3000 + iVar1 % 6000;
//     local_30 = local_20;
//     local_2c = local_1c;
//     local_24 = local_14;
//     local_18 = local_28;
//     FUN_80039160(&local_30,&local_40,0x1000,0);
//   }
//   return;
// }
