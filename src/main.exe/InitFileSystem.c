#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/InitFileSystem", InitFileSystem);

// triage: EASY — 87 insns, 7 callees, ~0.10 to initialise_default_player_cameras_
// likely-relevant cookbook sections:
//   - Dispatch: if/switch ladder — reload vs CSE, signed vs unsigned
//   - gp vs absolute globals: gp-relative smalls — tools/gpsyms.py

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// /* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
//
// void InitFileSystem(int mode)
//
// {
//   undefined4 uVar1;
//   int iVar2;
//   uint uVar3;
//
//   uVar3 = mode & 3;
//   TotalIO = 0;
//   ReadMode = mode;
//   if (uVar3 == 1) {
//     PCinit();
//     iVar2 = strncmp(&DAT_807f0000,"ACQUREMEMORYDISK",0x10);
//     if (iVar2 != 0) {
//       vinit((undefined *)0x0,0);
//       DAT_807f0000 = s_ACQUREMEMORYDISK_800110f0[0];
//       UNK_807f0001 = s_ACQUREMEMORYDISK_800110f0[1];
//       UNK_807f0002 = s_ACQUREMEMORYDISK_800110f0[2];
//       UNK_807f0003 = s_ACQUREMEMORYDISK_800110f0[3];
//       DAT_807f0004 = s_ACQUREMEMORYDISK_800110f0[4];
//       UNK_807f0005 = s_ACQUREMEMORYDISK_800110f0[5];
//       UNK_807f0006 = s_ACQUREMEMORYDISK_800110f0[6];
//       UNK_807f0007 = s_ACQUREMEMORYDISK_800110f0[7];
//       DAT_807f0008 = s_ACQUREMEMORYDISK_800110f0[8];
//       UNK_807f0009 = s_ACQUREMEMORYDISK_800110f0[9];
//       UNK_807f000a = s_ACQUREMEMORYDISK_800110f0[10];
//       UNK_807f000b = s_ACQUREMEMORYDISK_800110f0[0xb];
//       DAT_807f000c = s_ACQUREMEMORYDISK_800110f0[0xc];
//       UNK_807f000d = s_ACQUREMEMORYDISK_800110f0[0xd];
//       UNK_807f000e = s_ACQUREMEMORYDISK_800110f0[0xe];
//       UNK_807f000f = s_ACQUREMEMORYDISK_800110f0[0xf];
//       ReadMode = ReadMode | 9;
//     }
//     uVar1 = virtual_memory_pool;
//     if ((ReadMode & 9U) != 0) {
//       vinit(&UNK_80200000,0x5f0000);
//       vcalloc(0x8000,'\0');
//     }
//     DAT_80097eb8 = &UNK_80200008;
//     virtual_memory_pool = uVar1;
//   }
//   else if (uVar3 < 2) {
//     if (uVar3 == 0) {
//       PCinit();
//     }
//   }
//   else if (uVar3 == 2) {
//     CdInit();
//     cd_init();
//     AfsOpenVolume((TAFS *)&systemAFS,"TENCHU\\DATA");
//   }
//   return;
// }
