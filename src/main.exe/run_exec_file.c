#include "common.h"
#include "main.exe.h"

INCLUDE_ASM(".shake/gen/main.exe/asm/nonmatchings/run_exec_file", run_exec_file);
INCLUDE_ASM(".shake/gen/main.exe/asm/nonmatchings/run_exec_file", run_exec_file__override__prt_8005e870_13f24ac1);

// triage: MEDIUM — 47 insns, 2 loop, 6 callees, ~0.20 to AddXG4
// likely-relevant cookbook sections:
//   - Loops: 2 back-edge(s) — for/while/do vs goto shape

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void FUN_8005e834(char *param_1,undefined4 param_2,undefined4 param_3)
//
// {
//   EXEC *pEVar1;
//   int iVar2;
//
//   VSyncCallback(FUN_8005e948);
//   do {
//     do {
//       printf("reading exec %s\n",param_1);
//       pEVar1 = CdReadExec(param_1);
//     } while (pEVar1 == (EXEC *)0x0);
//     iVar2 = CdReadSync(0,(u_char *)0x0);
//   } while (iVar2 != 0);
//   VSyncCallback((f *)0x0);
//   printf("exe read ok\n");
//   *(undefined4 *)(pEVar1 + 0x20) = param_2;
//   *(undefined4 *)(pEVar1 + 0x24) = param_3;
//   StopCallback();
//   Exec(pEVar1,0,(char **)0x0);
//   return;
// }
