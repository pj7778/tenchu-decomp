#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * void ActDEAD(void);
 *     MOTION.C:2046, 89 src lines, frame 64 bytes, saved-reg mask 0x800f0000 (DEMO build -- see below)
 *
 * Original parameters and locals (the demo build's register allocation may
 * differ from retail, but the COUNT and TYPES drive cc1's codegen and carry
 * over). A repeated name is a nested-block scope, not a duplicate.
 * A ZERO-locals record is unverified, not a claim that the function has none:
 * vfree lists zero locals yet its byte-matched source needs seven.
 * The frame size and saved-reg mask above are the DEMO's: retail often needs
 * FEWER callee-saved registers (measured: Think1random exact; Think1chase's
 * 0x800f0000 = s0-s3+ra vs retail's s0,s1,ra). Treat them as an upper bound
 * and a hint at how many values stay live, never as a spec. The asm wins.
 * Locals:
 *     reg   $s0       struct VECTOR * pp
 *     stack sp+16     struct VECTOR p
 *     stack sp+32     struct SVECTOR v
 *     reg   $s2       short bldo
 *     reg   $s3       short blds
 *     reg   $s0       short blood
 * END PSX.SYM */

/*
 * ActDEAD (0x800268bc) — TODO one-line description.
 *
 * STATUS: NON_MATCHING — split (jump-table) function scaffolded by
 * tools/split-scaffold.py. The #ifndef NON_MATCHING branch is the stub
 * (INCLUDE_ASM pieces + the jump-table pool as one static const array so
 * the .rodata carve has bytes); build the draft with `NON_MATCHING=ActDEAD
 * ./Build`. On a full match, delete the guards and the _jtbl array.
 */

#ifndef NON_MATCHING
INCLUDE_ASM(".shake/gen/main.exe/asm/nonmatchings/ActDEAD", ActDEAD);

INCLUDE_ASM(".shake/gen/main.exe/asm/nonmatchings/ActDEAD", switchD_80026d70__switchD);

INCLUDE_ASM(".shake/gen/main.exe/asm/nonmatchings/ActDEAD", switchD_80026d70__caseD_0);

INCLUDE_ASM(".shake/gen/main.exe/asm/nonmatchings/ActDEAD", switchD_80026d70__caseD_1);

INCLUDE_ASM(".shake/gen/main.exe/asm/nonmatchings/ActDEAD", switchD_80026d70__caseD_2);

INCLUDE_ASM(".shake/gen/main.exe/asm/nonmatchings/ActDEAD", switchD_80026d70__caseD_3);

INCLUDE_ASM(".shake/gen/main.exe/asm/nonmatchings/ActDEAD", switchD_80026d70__caseD_5);

/* jump-table pool @ 0x80011610 (5 words; tables at 0x80011610) — stub-only, one array because the object has one .rodata section; the draft's compiled switch emits its own. */
static const u32 handle_char_state_dead__jtbl[5] = {
    0x80026D78, 0x80026D90, 0x80026DB4, 0x80026DDC,
    0x80026DDC,
};

#else /* NON_MATCHING */
/* Draft — turn this into matching C, then delete the #ifndef/#else/
   #endif guards and the _jtbl array(s) above.  Reference: */
// 
// void FUN_800268bc(void)
// 
// {
//   ModelArchiveType **ppMVar1;
//   short sVar2;
//   Humanoid *pHVar3;
//   SVECTOR *pSVar4;
//   MotionManager *pMVar5;
//   int iVar6;
//   uint uVar7;
//   uint uVar8;
//   int iVar9;
//   Humanoid *pHVar10;
//   ModelArchiveType *pMVar11;
//   undefined *puVar12;
//   ushort uVar13;
//   ushort unaff_s3;
//   ushort unaff_s4;
//   VECTOR local_40;
//   int local_28;
//   undefined4 local_24;
//   uchar auStack_20 [2];
//   short local_1e;
//   short local_1c;
//   
//   pMVar11 = Me_MOTION_C->model;
//   uVar13 = 0xffff;
//   if (((*pMVar11->object)->id < 0) && (dtM->loop < 0)) {
//     return;
//   }
//   if (dtM->count == 0) {
//     if (dtM->loop != 0) {
//       dtM->loop = -1;
//       goto LAB_80026948;
//     }
//   }
//   else {
// LAB_80026948:
//     pMVar5 = dtM;
//     if ((dtM->loop < 0) && (dtV->vy == 0)) {
//       sVar2 = dtM->motion->time;
//       dtM->loop = 0;
//       pMVar5->count = sVar2;
//       PlayMotion(pMVar5,1);
//       sVar2 = motID;
//       dtM->loop = -1;
//       if (sVar2 == 0x1108) {
//         ppMVar1 = &Me_MOTION_C->model;
//         Me_MOTION_C->attribute = Me_MOTION_C->attribute | 0x10;
//         (*ppMVar1)->attribute = (*ppMVar1)->attribute | 1;
//       }
//       else {
//         Me_MOTION_C->attribute = Me_MOTION_C->attribute & 0xffef;
//       }
//       pSVar4 = dtV;
//       pHVar3 = Me_MOTION_C;
//       pHVar10 = StagePlayer;
//       dtV->vz = 0;
//       pSVar4->vx = 0;
//       if ((pHVar3 != pHVar10) &&
//          (DeleteConflict(*pMVar11->object), (Me_MOTION_C->type & 0xf0U) != 0x80)) {
//         TurnAroundAllItems();
//       }
//       if (dtM->mid < 0x1109) {
//         return;
//       }
//       CamState.OldMode._1_1_ = 1;
//       ActionHalt = 0;
//       return;
//     }
//   }
//   pMVar5 = dtM;
//   if ((0x1108 < dtM->mid) && (dtL->vy != StagePlayer->locate->vy)) {
//     dtL->vy = dtL->vy + -1;
//     motID = 0x1100;
//     ActionHalt = 0;
//     DAT_80097f0e = 1;
//     if (9 < pMVar5->count) {
//       ActionHalt = 0;
//       motID = 0x1100;
//       DAT_80097f0e = 1;
//       return;
//     }
//     PadShockAR(0,0xff,10,10);
//     Sound(Me_MOTION_C,8);
//     Sound(StagePlayer,5);
//     return;
//   }
//   sVar2 = dtM->mid;
//   if (sVar2 == 0x1108) {
//     iVar6 = rand();
//     if (iVar6 == (iVar6 / 0x14) * 0x14) {
//       Sound(Me_MOTION_C,0x16);
//     }
//     local_40.vy = (Me_MOTION_C->map).level;
//     uVar7 = rand();
//     iVar6 = 0;
//     if ((uVar7 & 5) == 0) {
//       do {
//         iVar9 = rand();
//         local_40.vx = (long)Me_MOTION_C->width;
//         if (local_40.vx == 0) {
//           trap(0x1c00);
//         }
//         if ((local_40.vx == -1) && (iVar9 == -0x80000000)) {
//           trap(0x1800);
//         }
//         local_40.vx = (dtL->vx + (iVar9 % local_40.vx) * 2) - local_40.vx;
//         iVar9 = rand();
//         local_40.vz = (long)Me_MOTION_C->width;
//         if (local_40.vz == 0) {
//           trap(0x1c00);
//         }
//         if ((local_40.vz == -1) && (iVar9 == -0x80000000)) {
//           trap(0x1800);
//         }
//         local_40.vz = (dtL->vz + (iVar9 % local_40.vz) * 2) - local_40.vz;
//         uVar7 = rand();
//         uVar8 = rand();
//         SetSplash(&local_40,(short)((uVar7 & 7) << 0xc),(short)((uVar8 & 7) << 0xc),6);
//         iVar6 = iVar6 + 1;
//       } while (iVar6 * 0x10000 >> 0x10 < 5);
//     }
//     goto switchD_80026d70_caseD_5;
//   }
//   if ((sVar2 < 0x1108) || (0x110e < sVar2)) {
//     if ((Me_MOTION_C->type & 0xf0U) != 0xa0) {
//       if ((dtM->count == 5) && (DAT_8009770c == Me_MOTION_C)) {
//         Sound(DAT_8009770c,0x38);
//         DAT_8009770c = (Humanoid *)0x0;
//       }
//       uVar13 = 1;
//       unaff_s4 = 100;
//       unaff_s3 = 0;
//     }
//     goto switchD_80026d70_caseD_5;
//   }
//   puVar12 = (&PTR_DAT_80086b0c)[dtM->mid + -0x1109];
//   iVar6 = 0;
//   if (*(short *)(puVar12 + 2) != 4) {
//     do {
//       iVar9 = iVar6 + 1;
//       if (*(short *)(puVar12 + ((iVar6 << 0x10) >> 0xd)) == dtM->count) break;
//       iVar6 = iVar9;
//     } while (*(short *)(puVar12 + (iVar9 * 0x10000 >> 0xd) + 2) != 4);
//   }
//   if (dtM->count < *(short *)(puVar12 + ((iVar6 << 0x10) >> 0xd))) {
//     return;
//   }
//   switch(*(short *)((int)(puVar12 + ((iVar6 << 0x10) >> 0xd)) + 2)) {
//   case 0:
//     pHVar10 = StagePlayer;
//     goto LAB_80026da0;
//   case 1:
//     pHVar10 = Me_MOTION_C;
// LAB_80026da0:
//     Sound(pHVar10,*(short *)(puVar12 + ((iVar6 << 0x10) >> 0xd) + 4));
//     break;
//   case 2:
//     iVar6 = (iVar6 << 0x10) >> 0xd;
//     PadShockAR(0,0xff,(int)*(short *)(puVar12 + iVar6 + 4),(int)*(short *)(puVar12 + iVar6 + 6));
//     break;
//   case 3:
//   case 4:
//     ReqLifeBar(Me_MOTION_C);
//     iVar6 = (iVar6 << 0x10) >> 0xd;
//     uVar13 = *(ushort *)(puVar12 + iVar6 + 4);
//     unaff_s4 = *(ushort *)(puVar12 + iVar6 + 6) >> 8;
//     unaff_s3 = *(ushort *)(puVar12 + iVar6 + 6) & 0xff;
//   }
// switchD_80026d70_caseD_5:
//   if (((dtM->count & 4U) != 0) && (uVar13 != 0xffff)) {
//     local_28 = DAT_8009771c;
//     local_24 = DAT_80097720;
//     memset(auStack_20,'\0',8);
//     local_1e = -unaff_s3;
//     local_1c = -unaff_s4;
//     if (unaff_s3 == 0) {
//       local_28 = 0xff38;
//       local_24 = CONCAT22(local_24._2_2_,0xff10);
//     }
//     else {
//       local_28 = 0xfe66;
//       local_24 = (uint)local_24._2_2_ << 0x10;
//     }
//     local_28 = local_28 << 0x10;
//     FUN_80035f44(*(undefined4 *)
//                   (((int)((uint)uVar13 << 0x10) >> 0xe) + (int)Me_MOTION_C->model->object),&local_28
//                  ,auStack_20);
//   }
//   return;
// }

#endif /* NON_MATCHING */
