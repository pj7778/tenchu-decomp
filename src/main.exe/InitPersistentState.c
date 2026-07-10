#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/InitPersistentState", InitPersistentState);

// triage: MEDIUM — 92 insns, 1 loop, 4 callees, ~0.08 to load_font_image_into_global
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// undefined4 InitPersistentState(void)
//
// {
//   undefined4 uVar1;
//   int iVar2;
//   int iVar3;
//
//   if (((PersistentState._4_1_ & 0xfe) != 0) || (uVar1 = 1, 10 < (byte)PersistentState._5_1_)) {
//     memset((uchar *)&PersistentState,'\0',0xe70);
//     iVar3 = 0x1f;
//     iVar2 = -0x7ffeffe1;
//     PersistentState._0_4_ = 0x19981110;
//     PersistentState._88_1_ = 0;
//     PersistentState._89_1_ = 1;
//     PersistentState._90_1_ = 0x7f;
//     PersistentState._91_1_ = 0x7f;
//     PersistentState._92_1_ = 0;
//     PersistentState._93_1_ = 1;
//     PersistentState._97_1_ = 1;
//     PersistentState._96_1_ = 1;
//     do {
//       *(undefined1 *)(iVar2 + 0x40c) = 0xfe;
//       iVar3 = iVar3 + -1;
//       iVar2 = iVar2 + -1;
//     } while (-1 < iVar3);
//     PersistentState._1040_2_ = 0x101;
//     PersistentState._1036_4_ = 0x20606ff;
//     PersistentState._1043_1_ = 3;
//     PersistentState._1044_1_ = 5;
//     PersistentState._1068_4_ = 0x20606ff;
//     PersistentState._1072_4_ = PersistentState._1040_4_;
//     PersistentState._1076_4_ = PersistentState._1044_4_;
//     PersistentState._1080_4_ = PersistentState._1048_4_;
//     PersistentState._1084_4_ = PersistentState._1052_4_;
//     PersistentState._1088_4_ = PersistentState._1056_4_;
//     PersistentState._1092_4_ = PersistentState._1060_4_;
//     PersistentState._1096_4_ = PersistentState._1064_4_;
//     PersistentState._8_1_ = 10;
//     PersistentState._7_1_ = 0xff;
//     PersistentState._9_1_ = 5;
//     PersistentState._10_1_ = 2;
//     PersistentState._6_1_ = 0xff;
//     if (PersistentState._89_1_ == '\0') {
//       SsSetMono();
//     }
//     else {
//       SsSetStereo();
//     }
//     FUN_8005c404(&PersistentState);
//     PersistentState._95_1_ = 0;
//     uVar1 = 0;
//   }
//   return uVar1;
// }
