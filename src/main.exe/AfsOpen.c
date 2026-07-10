#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/AfsOpen", AfsOpen);

// triage: EASY — 39 insns, 2 loop, 2 callees, ~0.20 to vcalloc
// likely-relevant cookbook sections:
//   - Loops: 2 back-edge(s) — for/while/do vs goto shape

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// TAFSElement * AfsOpen(TAFS *handle,char *path)
//
// {
//   afs_entry_t *iVar1;
//   uint uVar1;
//   char *fmt;
//   afs_file_descriptor_t *piVar2;
//
//   iVar1 = (afs_entry_t *)AfsFindFile(handle,path,1);
//   uVar1 = 0;
//   if (iVar1 == (afs_entry_t *)0x0) {
//     fmt = "AfsOpen: %s not found\n";
//   }
//   else {
//     piVar2 = (afs_file_descriptor_t *)handle->pHandle;
//     do {
//       uVar1 = uVar1 + 1;
//       if (piVar2->used == 0) {
//         ((TAFSElement *)piVar2)->size = (ulong)iVar1;
//         ((TAFSElement *)piVar2)->pos = 0;
//         piVar2->used = 1;
//         return (TAFSElement *)piVar2;
//       }
//       piVar2 = (afs_file_descriptor_t *)(((TAFSElement *)piVar2)->name + 8);
//     } while (uVar1 < 5);
//     fmt = "AfsOpen: no more handle\n[%s]";
//   }
//   AdtMessageBox(fmt,path);
//   return (TAFSElement *)0x0;
// }
