#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/MakeCameraPosition", MakeCameraPosition);

// triage: MEDIUM — 165 insns, mul/div, 8 callees, ~0.08 to AdtFntOpen
// likely-relevant cookbook sections:
//   - Expressions: mult/div — magic-multiply constants, fold

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void MakeCameraPosition(VECTOR *orgpos,SVECTOR *orgrot,SVECTOR *campos,SVECTOR *ref)
//
// {
//   short sVar1;
//   int iVar2;
//   int iVar3;
//   int iVar4;
//   undefined1 local_88 [16];
//   int local_78;
//   int local_74;
//   VECTOR local_68;
//   VECTOR local_58;
//   VECTOR VStack_48;
//   VECTOR VStack_38;
//   long alStack_28 [2];
//
//   sVar1 = FUN_8002fd9c(CamState.Owner);
//   Scratchpad._64_2_ = orgrot->vx + sVar1;
//   Scratchpad._66_2_ = orgrot->vy;
//   Scratchpad._68_2_ = orgrot->vz;
//   RotMatrixYXZ((SVECTOR *)(Scratchpad + 0x40),(MATRIX *)(Scratchpad + 0x80));
//   Scratchpad._148_4_ = orgpos->vx;
//   Scratchpad._152_4_ = orgpos->vy;
//   Scratchpad._156_4_ = orgpos->vz;
//   SetRotMatrix((MATRIX *)(Scratchpad + 0x80));
//   SetTransMatrix((MATRIX *)(Scratchpad + 0x80));
//   RotTrans(campos,&local_68,alStack_28);
//   RotTrans(campos + 1,&local_58,alStack_28);
//   RotTrans(campos + 2,&VStack_48,alStack_28);
//   RotTrans(campos + 3,&VStack_38,alStack_28);
//   iVar2 = FUN_80039ddc(&VStack_48,&VStack_38,local_88,0);
//   iVar3 = (local_58.vx - local_68.vx) * iVar2;
//   if (iVar3 < 0) {
//     iVar3 = iVar3 + 0xfff;
//   }
//   iVar4 = (local_58.vy - local_68.vy) * iVar2;
//   local_88._12_4_ = (iVar3 >> 0xc) + local_68.vx;
//   if (iVar4 < 0) {
//     iVar4 = iVar4 + 0xfff;
//   }
//   iVar2 = (local_58.vz - local_68.vz) * iVar2;
//   local_78 = (iVar4 >> 0xc) + local_68.vy;
//   if (iVar2 < 0) {
//     iVar2 = iVar2 + 0xfff;
//   }
//   local_74 = (iVar2 >> 0xc) + local_68.vz;
//   AntiWall(&ViewInfo,(GsRVIEW2 *)local_88);
//   if (CamState.OldMode._1_1_ == 1) {
//     *(long *)ref = local_88._0_4_ - ViewInfo.vpx;
//     *(long *)&ref->vz = local_88._4_4_ - ViewInfo.vpy;
//     *(long *)(ref + 1) = local_88._8_4_ - ViewInfo.vpz;
//     *(long *)&ref[1].vz = local_88._12_4_ - ViewInfo.vrx;
//     *(int *)(ref + 2) = local_78 - ViewInfo.vry;
//     *(int *)&ref[2].vz = local_74 - ViewInfo.vrz;
//     CamState.OldMode._1_1_ = CMODE_NORMAL >> 8;
//   }
//   else {
//     MakeDifSub((VECTOR *)&ViewInfo.vrx,(VECTOR *)(local_88 + 0xc),(VECTOR *)&ref[1].vz,
//                (TMakeDifInfo *)&CamState.Valiation);
//     MakeDifSub((VECTOR *)&ViewInfo,(VECTOR *)local_88,(VECTOR *)ref,&pnt);
//   }
//   return;
// }
