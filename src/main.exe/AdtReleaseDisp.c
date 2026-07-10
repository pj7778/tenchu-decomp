#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/AdtReleaseDisp", AdtReleaseDisp);

// triage: EASY — 36 insns, 6 callees, ~0.15 to initialise_font

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void AdtReleaseDisp(DRAWENV *param_1)
//
// {
//   FntLoad(AdtFnt_8008f1b8.field6_0x18,AdtFnt_8008f1b8.field7_0x1c);
//   FntOpen(AdtFnt_8008f1b8.field0_0x0,AdtFnt_8008f1b8.field1_0x4,AdtFnt_8008f1b8.field2_0x8,
//           AdtFnt_8008f1b8.field3_0xc,AdtFnt_8008f1b8.field4_0x10,AdtFnt_8008f1b8.field5_0x14);
//   LoadImage((RECT *)&param_1[1].tpage,&param_1[1].dr_env.tag);
//   DrawSync(0);
//   PutDrawEnv(param_1);
//   PutDispEnv((DISPENV *)(param_1 + 1));
//   return;
// }
