#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/LoadCard", LoadCard);

// triage: MEDIUM — 69 insns, 1 loop, frame 0x2100, 6 callees, ~0.08 to UpdateOrnament
// likely-relevant cookbook sections:
//   - Loops: 1 back-edge(s) — for/while/do vs goto shape
//   - gp vs absolute globals: gp-relative smalls — tools/gpsyms.py
//   - Stack objects: 0x2100 frame — buffer casts / spills

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// short LoadCard(int target,uchar *name)
//
// {
//   undefined *pt;
//   undefined4 *puVar1;
//   PersistentState *pPVar2;
//   undefined4 uVar3;
//   undefined4 uVar4;
//   undefined4 uVar5;
//   char acStack_20e8 [200];
//   ulong auStack_2020 [128];
//   undefined4 local_1e20 [1920];
//   long lStack_20;
//   long local_1c;
//
//   pt = valloc(0x2000);
//   local_1c = MemCardAccept(0);
//   MemCardSync(0,&lStack_20,&local_1c);
//   sprintf(acStack_20e8,&DAT_80097d08,PTR_s_BISLPS_01901_80097d04,name);
//   local_1c = MemCardReadFile(0,acStack_20e8,auStack_2020,0,0x2000);
//   MemCardSync(0,&lStack_20,&local_1c);
//   pPVar2 = &PersistentState;
//   if (local_1c == 0) {
//     puVar1 = local_1e20;
//     do {
//       uVar3 = puVar1[1];
//       uVar4 = puVar1[2];
//       uVar5 = puVar1[3];
//       *(undefined4 *)pPVar2 = *puVar1;
//       *(undefined4 *)&pPVar2->field_0x4 = uVar3;
//       *(undefined4 *)&pPVar2->field_0x8 = uVar4;
//       *(undefined4 *)&pPVar2->field_0xc = uVar5;
//       puVar1 = puVar1 + 4;
//       pPVar2 = (PersistentState *)&pPVar2->field_0x10;
//     } while (puVar1 != local_1e20 + 0x39c);
//   }
//   else {
//     vfree(pt);
//     pt = (undefined *)0x0;
//   }
//   vfree(pt);
//   return (short)local_1c;
// }
