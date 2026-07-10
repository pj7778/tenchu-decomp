#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/AntiWall", AntiWall);

// triage: MEDIUM — 173 insns, 6 callees, ~0.06 to AdtFntOpen

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void AntiWall(GsRVIEW2 *vinfo,GsRVIEW2 *target)
//
// {
//   long lVar1;
//   long lVar2;
//   char *pcVar3;
//   char *pcVar4;
//   int iVar5;
//   byte bVar6;
//   int iVar7;
//   int iVar8;
//   int iVar9;
//   int iVar10;
//   int iVar11;
//   int iVar12;
//   VECTOR local_58;
//   VECTOR local_48;
//   VECTOR local_38;
//   byte local_28 [4];
//   byte local_24 [4];
//
//   GetVectorRotation((VECTOR *)target,(VECTOR *)&target->vrx,(int *)local_28,(int *)local_24);
//   Scratchpad[4] = 0;
//   Scratchpad[5] = 0;
//   Scratchpad[0] = local_28[0];
//   Scratchpad[1] = local_28[1];
//   Scratchpad[2] = local_24[0];
//   Scratchpad[3] = local_24[1];
//   RotMatrixYXZ((SVECTOR *)Scratchpad,(MATRIX *)(Scratchpad + 0x40));
//   SetRotMatrix((MATRIX *)(Scratchpad + 0x40));
//   ApplyRotMatrix((SVECTOR *)&DAT_800979f4,&local_58);
//   ApplyRotMatrix((SVECTOR *)&DAT_800979fc,&local_48);
//   lVar1 = GetAreaMapLevel(GlobalAreaMap,target->vpx + local_48.vx,target->vpy + local_48.vy);
//   pcVar3 = (char *)(target->vpx + local_58.vx);
//   pcVar4 = (char *)(target->vpy + local_58.vy);
//   iVar5 = target->vpz + local_58.vz;
//   lVar2 = GetAreaMapLevel(GlobalAreaMap,(long)pcVar3,(long)pcVar4);
//   bVar6 = lVar2 <= target->vpy;
//   if ((bool)bVar6) {
//     FntPrint(&DAT_80097a04,pcVar3,pcVar4,iVar5);
//   }
//   if (lVar1 <= target->vpy) {
//     bVar6 = bVar6 | 2;
//     FntPrint(&DAT_80097a08,pcVar3,pcVar4,iVar5);
//   }
//   if (bVar6 != 0) {
//     local_38.vx = 0;
//     local_38.vy = 0;
//     local_38.vz = 0;
//     if (bVar6 == 1) {
//       Scratchpad[0] = 0xe8;
//       Scratchpad[1] = 3;
//       Scratchpad[2] = 0;
//       Scratchpad[3] = 0;
//       Scratchpad[4] = 0;
//       Scratchpad[5] = 0;
//       ApplyRotMatrix((SVECTOR *)Scratchpad,&local_38);
//     }
//     if (bVar6 == 2) {
//       Scratchpad[0] = 0x18;
//       Scratchpad[1] = 0xfc;
//       Scratchpad[2] = 0;
//       Scratchpad[3] = 0;
//       Scratchpad[4] = 0;
//       Scratchpad[5] = 0;
//       ApplyRotMatrix((SVECTOR *)Scratchpad,&local_38);
//     }
//     iVar12 = local_38.vx - (local_38.vx >> 0x1f);
//     iVar11 = local_38.vx / 2;
//     pcVar3 = (char *)(target->vpx + local_38.vx + iVar11);
//     iVar10 = local_38.vy - (local_38.vy >> 0x1f);
//     iVar9 = local_38.vy / 2;
//     pcVar4 = (char *)(target->vpy + local_38.vy + iVar9);
//     iVar8 = local_38.vz - (local_38.vz >> 0x1f);
//     iVar7 = local_38.vz / 2;
//     iVar5 = target->vpz + local_38.vz + iVar7;
//     lVar1 = GetAreaMapLevel(GlobalAreaMap,(long)pcVar3,(long)pcVar4);
//     if (lVar1 <= target->vpy) {
//       local_38.vx = iVar11 - (iVar12 >> 0x1f) >> 1;
//       local_38.vy = iVar9 - (iVar10 >> 0x1f) >> 1;
//       local_38.vz = iVar7 - (iVar8 >> 0x1f) >> 1;
//       FntPrint(&DAT_80097a0c,pcVar3,pcVar4,iVar5);
//     }
//     target->vpx = target->vpx + local_38.vx;
//     target->vpy = target->vpy + local_38.vy;
//     target->vpz = target->vpz + local_38.vz;
//   }
//   return;
// }
