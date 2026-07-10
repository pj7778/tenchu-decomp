#include "common.h"
#include "main.exe.h"

/*
 * start_demo_ (0x80055d64) — TODO one-line description.
 *
 * STATUS: NON_MATCHING — split (jump-table) function scaffolded by
 * tools/split-scaffold.py. The #ifndef NON_MATCHING branch is the stub
 * (INCLUDE_ASM pieces + the jump-table pool as one static const array so
 * the .rodata carve has bytes); build the draft with `NON_MATCHING=start_demo_
 * ./Build`. On a full match, delete the guards and the _jtbl array.
 */

#ifndef NON_MATCHING
INCLUDE_ASM(".shake/gen/main.exe/asm/nonmatchings/start_demo_", start_demo_);

INCLUDE_ASM(".shake/gen/main.exe/asm/nonmatchings/start_demo_", start_demo___override__prt_80055ee4_68447b59);

INCLUDE_ASM(".shake/gen/main.exe/asm/nonmatchings/start_demo_", switchD_800561a0__switchD);

INCLUDE_ASM(".shake/gen/main.exe/asm/nonmatchings/start_demo_", switchD_800561a0__caseD_1);

INCLUDE_ASM(".shake/gen/main.exe/asm/nonmatchings/start_demo_", switchD_800561a0__caseD_2);

INCLUDE_ASM(".shake/gen/main.exe/asm/nonmatchings/start_demo_", switchD_800561a0__caseD_3);

INCLUDE_ASM(".shake/gen/main.exe/asm/nonmatchings/start_demo_", switchD_800561a0__caseD_4);

INCLUDE_ASM(".shake/gen/main.exe/asm/nonmatchings/start_demo_", switchD_800561a0__caseD_5);

/* jump-table pool @ 0x80013bb0 (5 words; tables at 0x80013bb0) — stub-only, one array because the object has one .rodata section; the draft's compiled switch emits its own. */
static const u32 start_demo__jtbl[5] = {
    0x800561A8, 0x800561F8, 0x80056438, 0x800564EC,
    0x80056550,
};

#else /* NON_MATCHING */
/* Draft — turn this into matching C, then delete the #ifndef/#else/
   #endif guards and the _jtbl array(s) above.  Reference: */
// 
// /* WARNING: Removing unreachable block (ram,0x800565d4) */
// 
// void FUN_80055d64(void)
// 
// {
//   ulong *puVar1;
//   Sprite3D *pSVar2;
//   ulong *puVar3;
//   int iVar4;
//   int iVar5;
//   undefined4 uVar6;
//   GsIMAGE GStack_188;
//   GsSPRITE local_168;
//   GsSPRITE local_140;
//   GsSPRITE local_118;
//   GsSPRITE local_f0;
//   GsSPRITE local_c8;
//   RECT local_a0;
//   char acStack_98 [64];
//   GsIMAGE GStack_58;
//   undefined2 local_38;
//   BackGround *local_34;
//   ulong *local_30;
//   
//   local_38 = 0;
//   SetupAppearance(0,-1);
//   PadShockAR(0,0,0,0);
//   iVar4 = 0;
//   do {
//     iVar5 = iVar4 + 1;
//     (&PersistentState.field_0x27)[iVar4] =
//          (&PersistentState.field_0x40c)[iVar4 + (uint)(byte)PersistentState._4_1_ * 0x20];
//     iVar4 = iVar5;
//   } while (iVar5 < 0x14);
//   FadeOutDirect(0x20,2,'\b','\b');
//   FUN_80038ce0();
//   local_a0.w = 0x400;
//   local_a0.x = 0;
//   local_a0.y = 0;
//   local_a0.h = 0x200;
//   ClearImage(&local_a0,'\0','\0','\0');
//   DrawSync(0);
//   puVar1 = FileRead("K:\\WORK\\CDIMAGE\\DEMO\\start\\fadeio.tim");
//   GetTIMInfo(puVar1,&GStack_188);
//   LoadTIMAndFree(puVar1);
//   pSVar2 = SetupSprite((Sprite3D *)0x0,&GStack_188);
//   uVar6 = 0x72;
//   (pSVar2->sprite).attribute = (pSVar2->sprite).attribute | 0x60000000;
//   if (PersistentState._4_1_ != '\0') {
//     uVar6 = 0x61;
//   }
//   sprintf(acStack_98,"%s%s%c.Arc","K:\\WORK\\CDIMAGE\\DEMO\\",
//           (&PTR_s_Gov_e__8008ef78)[(byte)PersistentState._94_1_],uVar6);
//   puVar1 = FileRead(acStack_98);
//   uVar6 = FUN_8004f1d8(puVar1,0);
//   local_34 = (BackGround *)FUN_8004f4f8(uVar6);
//   local_30 = PathFileRead((uchar *)"K:\\WORK\\CDIMAGE\\DEMO\\",
//                           (&PTR_s_Gov_e_Arc_8008ef88)[(byte)PersistentState._94_1_]);
//   puVar3 = (ulong *)FUN_8004f1d8(local_30,0);
//   GetTIMInfo(puVar3,&GStack_58);
//   InitSprite(&GStack_58,&local_168);
//   local_168.y = -0x28;
//   local_168.x = 0;
//   local_168.r = 0x80;
//   local_168.g = 0x80;
//   local_168.b = 0x80;
//   local_168.attribute = local_168.attribute | 0x50000000;
//   local_168.my = local_168.h >> 1;
//   local_168.mx = local_168.w >> 1;
//   LoadTIM(puVar3);
//   puVar3 = (ulong *)FUN_8004f1d8(local_30,1);
//   GetTIMInfo(puVar3,&GStack_58);
//   InitSprite(&GStack_58,&local_c8);
//   local_c8.y = 0x5f;
//   local_c8.x = 0;
//   local_c8.r = 0x80;
//   local_c8.g = 0x80;
//   local_c8.b = 0x80;
//   local_c8.mx = local_c8.w >> 1;
//   local_c8.my = local_c8.h >> 1;
//   LoadTIM(puVar3);
//   puVar3 = (ulong *)FUN_8004f1d8(puVar1,1);
//   GetTIMInfo(puVar3,&GStack_58);
//   InitSprite(&GStack_58,&local_140);
//   local_140.x = 0;
//   local_140.y = 0;
//   local_140.r = '\0';
//   local_140.g = '\0';
//   local_140.b = '\0';
//   local_140.attribute = local_140.attribute | 0x50000000;
//   local_140.my = local_140.h >> 1;
//   local_140.mx = local_140.w >> 1;
//   LoadTIM(puVar3);
//   puVar3 = (ulong *)FUN_8004f1d8(puVar1,2);
//   GetTIMInfo(puVar3,&GStack_58);
//   InitSprite(&GStack_58,&local_118);
//   local_118.y = 0x14;
//   local_118.x = 0;
//   local_118.r = '\0';
//   local_118.g = '\0';
//   local_118.b = '\0';
//   local_118.attribute = local_118.attribute | 0x50000000;
//   local_118.mx = local_118.w >> 1;
//   local_118.my = local_118.h >> 1;
//   LoadTIM(puVar3);
//   puVar1 = (ulong *)FUN_8004f1d8(puVar1,3);
//   GetTIMInfo(puVar1,&GStack_58);
//   InitSprite(&GStack_58,&local_f0);
//   local_f0.y = 0x28;
//   local_f0.x = 0;
//   local_f0.r = '\0';
//   local_f0.g = '\0';
//   local_f0.b = '\0';
//   local_f0.attribute = local_f0.attribute | 0x50000000;
//   local_f0.mx = local_f0.w >> 1;
//   local_f0.my = local_f0.h >> 1;
//   LoadTIM(puVar1);
//   DrawSync(0);
//   VSync(0);
//   _PlayMusic(0xb,0);
//   StartDrawing();
//   DrawBG(local_34);
//                     /* WARNING: Could not recover jumptable at 0x800561a0. Too many branches */
//                     /* WARNING: Treating indirect jump as call */
//   (*(code *)switchD_800561a0::switchdataD_80013bb0)();
//   return;
// }

#endif /* NON_MATCHING */
