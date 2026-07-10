#include "common.h"
#include "main.exe.h"

INCLUDE_ASM(".shake/gen/main.exe/asm/nonmatchings/LoadSI", LoadSI);
INCLUDE_ASM(".shake/gen/main.exe/asm/nonmatchings/LoadSI", load_stage_resource___override__prt_8005c2f4_aee7b64a);
INCLUDE_ASM(".shake/gen/main.exe/asm/nonmatchings/LoadSI", load_stage_resource___override__prt_8005c36c_6152584a);

// triage: MEDIUM — 80 insns, frame 0x20f8, 9 callees, ~0.11 to SetupSE
// likely-relevant cookbook sections:
//   - gp vs absolute globals: gp-relative smalls — tools/gpsyms.py
//   - Stack objects: 0x20f8 frame — buffer casts / spills

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// undefined * LoadSI(int target,uchar *name)
//
// {
//   ulong *puVar1;
//   uchar *pt;
//   char *fmt;
//   char acStack_20e0 [200];
//   ulong auStack_2018 [128];
//   uchar auStack_1e18 [7680];
//   long lStack_18;
//   int local_14;
//
//   if (target == 0) {
//     sprintf(acStack_20e0,&DAT_80097d90,ImagePath,name);
//     puVar1 = FileRead(acStack_20e0);
//     return (undefined *)puVar1;
//   }
//   fmt = (char *)0x0;
//   pt = valloc(0x2000);
//   MemCardAccept(0);
//   MemCardSync(0,&lStack_18,&local_14);
//   if ((local_14 == 0) || (local_14 == 3)) {
//     sprintf(acStack_20e0,s__s_d__s_80097d98,PTR_s_BISLPS_00000_80097d8c,StageID,name);
//     MemCardReadFile(0,acStack_20e0,auStack_2018,0,0x2000);
//     MemCardSync(0,&lStack_18,&local_14);
//     if (local_14 == 0) {
//       memcpy(pt,auStack_1e18,0x1e00);
//       goto LAB_8005c3d4;
//     }
//     fmt = "file read error";
//   }
//   else {
//     fmt = "card error %d";
//   }
//   vfree(pt);
//   pt = (uchar *)0x0;
// LAB_8005c3d4:
//   if (fmt != (char *)0x0) {
//     AdtMessageBox(fmt,local_14);
//   }
//   return pt;
// }
