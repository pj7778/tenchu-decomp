#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * int CdaPlayXA(unsigned char *fname, struct CdlLOC *start, struct CdlLOC *end, unsigned char channel, int mode);
 *     OPAUDIO.C:133, 39 src lines, frame 96 bytes, saved-reg mask 0x807f0000
 *
 * Original parameters and locals (the demo build's register allocation may
 * differ from retail, but the COUNT and TYPES drive cc1's codegen and carry
 * over). A repeated name is a nested-block scope, not a duplicate:
 *     param $s3       unsigned char * fname
 *     param $s5       struct CdlLOC * start
 *     param $s4       struct CdlLOC * end
 *     param $s6       unsigned char channel
 *     param stack+16  int mode
 *     stack sp+16     struct CdlFILE cf
 *     stack sp+40     struct CdlFILTER filter
 *     stack sp+48     unsigned char [4] param
 *     stack sp+56     struct CdlLOC loc
 *
 * Globals it touches, as the original declared them:
 *     extern struct TCdaStatus CdaStatus;
 * END PSX.SYM */

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/CdaPlayXA", CdaPlayXA);

// triage: EASY — 89 insns, 6 callees, ~0.11 to AdtFntOpen

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// undefined4 CdaPlayXA(char *param_1,CdlLOC *param_2,CdlLOC *param_3,undefined1 param_4,short param_5)
//
// {
//   CdlFILE *pCVar1;
//   int iVar2;
//   CdlFILE CStack_48;
//   undefined1 local_30;
//   undefined1 local_2f;
//   undefined1 local_28 [8];
//
//   if ((CdaStatus.flag & 1) != 0) {
//     CdaStop();
//     pCVar1 = CdSearchFile(&CStack_48,param_1);
//     if (pCVar1 != (CdlFILE *)0x0) {
//       CdaStatus.mode = param_5;
//       iVar2 = CdPosToInt(&CStack_48.pos);
//       CdaStatus.StartPos = iVar2 + 0x96;
//       if (param_3 == (CdlLOC *)0x0) {
//         CdaStatus.EndPos = CdaStatus.StartPos + (CStack_48.size >> 0xb);
//       }
//       else {
//         iVar2 = CdPosToInt(param_3);
//         CdaStatus.EndPos = CdaStatus.StartPos + iVar2;
//       }
//       if (param_2 != (CdlLOC *)0x0) {
//         iVar2 = CdPosToInt(param_2);
//         CdaStatus.StartPos = CdaStatus.StartPos + iVar2;
//       }
//       local_28[0] = 0xc9;
//       cd_control(0xe,local_28,0);
//       VSync(3);
//       CdaStatus.field9_0x14 = 0x1b;
//       CdaStatus.CheckCount = 0;
//       CdaStatus.status = '\0';
//       local_30 = 1;
//       local_2f = param_4;
//       cd_control(0xd,&local_30,0);
//       VSyncCallback(cbCheckCD);
//       return 1;
//     }
//   }
//   return 0;
// }
