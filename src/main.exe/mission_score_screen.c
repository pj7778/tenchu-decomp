#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/mission_score_screen", mission_score_screen);

// triage: VERY-HARD — 1159 insns, mul/div, 16 loop, 25 callees, ~0.11 to BriefingAndInventorySelectionScreen
// likely-relevant cookbook sections:
//   - Loops: 16 back-edge(s) — for/while/do vs goto shape
//   - Expressions: mult/div — magic-multiply constants, fold

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// /* WARNING: Removing unreachable block (ram,0x80055338) */
// /* WARNING: Removing unreachable block (ram,0x800553c4) */
// /* WARNING: Removing unreachable block (ram,0x80055454) */
// /* WARNING: Removing unreachable block (ram,0x800554e0) */
// /* WARNING: Removing unreachable block (ram,0x80055618) */
// /* WARNING: Removing unreachable block (ram,0x800556a4) */
//
// void FUN_80054b48(void)
//
// {
//   byte bVar1;
//   bool bVar2;
//   char cVar3;
//   uchar uVar4;
//   ushort uVar5;
//   short sVar6;
//   short sVar7;
//   uint *puVar8;
//   ulong *puVar9;
//   ulong *puVar10;
//   ushort uVar11;
//   uint uVar12;
//   int iVar13;
//   undefined4 uVar14;
//   int iVar15;
//   GsSPRITE *sprite;
//   int iVar16;
//   GsSPRITE local_1a0;
//   byte local_178;
//   byte local_177;
//   byte local_176;
//   byte local_175;
//   byte local_174;
//   int local_170;
//   undefined4 local_168;
//   undefined4 local_164;
//   undefined4 local_160;
//   GsSPRITE local_158 [5];
//   GsSPRITE local_a0 [2];
//   GsIMAGE GStack_58;
//   ushort local_38;
//   BackGround *local_34;
//   int local_30;
//
//   local_38 = 0;
//   FUN_8004e964(&local_178);
//   iVar16 = 0;
//   puVar8 = (uint *)FUN_8004e794(&local_178,PersistentState._5_1_);
//   local_168 = *puVar8;
//   local_164 = puVar8[1];
//   local_160 = puVar8[2];
//   puVar9 = FileRead("K:\\WORK\\CDIMAGE\\DEMO\\number.tim");
//   GetTIMInfo(puVar9,&GStack_58);
//   InitSprite(&GStack_58,&local_1a0);
//   local_1a0.attribute = local_1a0.attribute | 0x50000000;
//   local_1a0.x = -0x8c;
//   local_1a0.y = -0x28;
//   local_1a0.r = 0x80;
//   local_1a0.g = 0x80;
//   local_1a0.b = 0x80;
//   local_1a0.mx = 0;
//   local_1a0.my = 0;
//   LoadTIMAndFree(puVar9);
//   local_1a0.w = 0xc;
//   puVar9 = FileRead((&PTR_s_K__WORK_CDIMAGE_DEMO_Ranks_e_Arc_8008ef58)[(byte)PersistentState._94_1_]
//                    );
//   do {
//     iVar15 = (int)(short)iVar16;
//     puVar10 = (ulong *)FUN_8004f1d8(puVar9,iVar15);
//     sprite = local_158 + iVar15;
//     GetTIMInfo(puVar10,&GStack_58);
//     InitSprite(&GStack_58,sprite);
//     uVar12 = sprite->attribute;
//     local_158[iVar15].x = -0xa0;
//     local_158[iVar15].y = -0x78;
//     uVar5 = local_158[iVar15].w;
//     local_158[iVar15].r = 0x80;
//     local_158[iVar15].g = 0x80;
//     local_158[iVar15].b = 0x80;
//     local_158[iVar15].mx = uVar5 >> 1;
//     sprite->attribute = uVar12 | 0x50000000;
//     local_158[iVar15].my = local_158[iVar15].h >> 1;
//     local_158[iVar15].mx = 0;
//     local_158[iVar15].my = 0;
//     LoadTIM(puVar10);
//     iVar16 = iVar16 + 1;
//   } while (iVar16 * 0x10000 >> 0x10 < 5);
//   iVar16 = 0;
//   do {
//     iVar15 = (int)(short)iVar16;
//     puVar10 = (ulong *)FUN_8004f1d8(puVar9,iVar15 + 5);
//     GetTIMInfo(puVar10,&GStack_58);
//     InitSprite(&GStack_58,local_a0 + iVar15);
//     local_a0[iVar15].x = -0xa0;
//     local_a0[iVar15].y = -0x78;
//     uVar5 = local_a0[iVar15].w;
//     uVar11 = local_a0[iVar15].h;
//     local_a0[iVar15].r = 0x80;
//     local_a0[iVar15].g = 0x80;
//     local_a0[iVar15].b = 0x80;
//     local_a0[iVar15].mx = uVar5 >> 1;
//     local_a0[iVar15].my = uVar11 >> 1;
//     local_a0[iVar15].mx = 0;
//     local_a0[iVar15].my = 0;
//     LoadTIM(puVar10);
//     iVar16 = iVar16 + 1;
//   } while (iVar16 * 0x10000 >> 0x10 < 2);
//   vfree((undefined *)puVar9);
//   iVar15 = 0;
//   puVar9 = FileRead((&PTR_s_K__WORK_CDIMAGE_DEMO_trn_eng_tim_8008ef68)[(byte)PersistentState._94_1_]
//                    );
//   local_34 = (BackGround *)FUN_8004f4f8(puVar9);
//   vfree((undefined *)puVar9);
//   iVar16 = 0;
//   do {
//     iVar16 = iVar16 >> 0x10;
//     if (*(int *)(&PersistentState.field_0x458 + iVar16 * 4) == 0) {
//       *(undefined4 *)(&PersistentState.field_0x458 + iVar16 * 4) = 0x1a5c2;
//       (&PersistentState.field_0x44c)[iVar16] = 0;
//       (&PersistentState.field_0x451)[iVar16] = 0;
//     }
//     iVar15 = iVar15 + 1;
//     iVar16 = iVar15 * 0x10000;
//   } while (iVar15 * 0x10000 >> 0x10 < 5);
//   iVar15 = 0;
//   local_30 = -1;
//   iVar16 = 0;
//   do {
//     iVar13 = iVar16 >> 0x10;
//     if (((short)(ushort)(byte)(&PersistentState.field_0x451)[iVar13] < (short)local_160._2_2_) ||
//        ((local_160._2_2_ == (byte)(&PersistentState.field_0x451)[iVar13] &&
//         (local_170 < *(int *)(&PersistentState.field_0x458 + iVar13 * 4))))) break;
//     iVar15 = iVar15 + 1;
//     iVar16 = iVar15 * 0x10000;
//     iVar13 = local_30;
//   } while (iVar15 * 0x10000 >> 0x10 < 5);
//   local_30 = iVar13;
//   if (-1 < local_30) {
//     iVar16 = 4;
//     if (local_30 < 4) {
//       iVar15 = 0x40000;
//       do {
//         iVar15 = iVar15 >> 0x10;
//         *(undefined4 *)(&PersistentState.field_0x458 + iVar15 * 4) =
//              *(undefined4 *)(&PersistentState.field_0x454 + iVar15 * 4);
//         (&PersistentState.field_0x44c)[iVar15] = (&PersistentState.field_0x44b)[iVar15];
//         iVar16 = iVar16 + -1;
//         (&PersistentState.field_0x451)[iVar15] = (&PersistentState.field_0x450)[iVar15];
//         iVar15 = iVar16 * 0x10000;
//       } while (local_30 < iVar16 * 0x10000 >> 0x10);
//     }
//     *(int *)(&PersistentState.field_0x458 + local_30 * 4) = local_170;
//     (&PersistentState.field_0x44c)[local_30] = PersistentState._4_1_;
//     (&PersistentState.field_0x451)[local_30] = local_160._2_1_;
//   }
//   _PlayMusic(0xc,1);
//   while( true ) {
//     uVar5 = GetRealPad(0);
//     uVar11 = uVar5 & (uVar5 ^ local_38);
//     local_38 = uVar5;
//     if ((uVar11 & 0x20) != 0) break;
//     bVar2 = true;
//     if ((uVar11 & 0x800) != 0) goto LAB_80055c1c;
//     StartDrawing();
//     DrawBG(local_34);
//     FUN_800515b0(&local_1a0,local_170,0x46,0xffffff9f,1);
//     uVar4 = local_1a0.u;
//     uVar12 = (uint)local_174;
//     local_1a0.x = 0x16;
//     local_1a0.y = -0x47;
//     if ((int)uVar12 < 0) {
//       uVar12 = -uVar12;
//       bVar2 = true;
//     }
//     else {
//       bVar2 = false;
//     }
//     do {
//       sVar6 = (short)uVar12;
//       uVar12 = (int)sVar6 / 10;
//       local_1a0.u = uVar4 + (char)((uint)(((int)sVar6 % 10) * 0x10000) >> 0x10) * (char)local_1a0.w;
//       GsSortSprite(&local_1a0,OTablePt,0);
//       local_1a0.x = local_1a0.x + -0xc;
//     } while ((uVar12 & 0xffff) != 0);
//     if (bVar2) {
//       local_1a0.u = uVar4 + (char)local_1a0.w * '\n';
//       GsSortSprite(&local_1a0,OTablePt,0);
//     }
//     local_1a0.x = 0x1f;
//     local_1a0.u = uVar4 + (char)local_1a0.w * '\f';
//     GsSortSprite(&local_1a0,OTablePt,0);
//     local_1a0.x = 0x2f;
//     local_1a0.y = -0x47;
//     uVar12 = (uint)local_177 - (uint)local_178;
//     if ((int)uVar12 < 0) {
//       uVar12 = -uVar12;
//       bVar2 = true;
//     }
//     else {
//       bVar2 = false;
//     }
//     do {
//       sVar6 = (short)uVar12;
//       uVar12 = (int)sVar6 / 10;
//       local_1a0.u = uVar4 + (char)((uint)(((int)sVar6 % 10) * 0x10000) >> 0x10) * (char)local_1a0.w;
//       GsSortSprite(&local_1a0,OTablePt,0);
//       local_1a0.x = local_1a0.x + -0xc;
//     } while ((uVar12 & 0xffff) != 0);
//     if (bVar2) {
//       local_1a0.u = uVar4 + (char)local_1a0.w * '\n';
//       GsSortSprite(&local_1a0,OTablePt,0);
//     }
//     uVar12 = local_168 & 0xffff;
//     local_1a0.x = 0x66;
//     local_1a0.y = -0x47;
//     if ((short)local_168 < 0) {
//       uVar12 = -(int)(short)local_168;
//       bVar2 = true;
//     }
//     else {
//       bVar2 = false;
//     }
//     do {
//       sVar6 = (short)uVar12;
//       uVar12 = (int)sVar6 / 10;
//       local_1a0.u = uVar4 + (char)((uint)(((int)sVar6 % 10) * 0x10000) >> 0x10) * (char)local_1a0.w;
//       GsSortSprite(&local_1a0,OTablePt,0);
//       local_1a0.x = local_1a0.x + -0xc;
//     } while ((uVar12 & 0xffff) != 0);
//     if (bVar2) {
//       local_1a0.u = uVar4 + (char)local_1a0.w * '\n';
//       GsSortSprite(&local_1a0,OTablePt,0);
//     }
//     uVar12 = (uint)local_175;
//     local_1a0.x = 0x16;
//     local_1a0.y = -0x35;
//     do {
//       sVar6 = (short)uVar12;
//       uVar12 = (int)sVar6 / 10;
//       local_1a0.u = uVar4 + (char)((uint)(((int)sVar6 % 10) * 0x10000) >> 0x10) * (char)local_1a0.w;
//       GsSortSprite(&local_1a0,OTablePt,0);
//       local_1a0.x = local_1a0.x + -0xc;
//     } while ((uVar12 & 0xffff) != 0);
//     local_1a0.x = 0x1f;
//     local_1a0.u = uVar4 + (char)local_1a0.w * '\f';
//     GsSortSprite(&local_1a0,OTablePt,0);
//     uVar12 = (uint)local_177;
//     local_1a0.x = 0x2f;
//     local_1a0.y = -0x35;
//     do {
//       sVar6 = (short)uVar12;
//       uVar12 = (int)sVar6 / 10;
//       local_1a0.u = uVar4 + (char)((uint)(((int)sVar6 % 10) * 0x10000) >> 0x10) * (char)local_1a0.w;
//       GsSortSprite(&local_1a0,OTablePt,0);
//       local_1a0.x = local_1a0.x + -0xc;
//     } while ((uVar12 & 0xffff) != 0);
//     uVar12 = (uint)local_168._2_2_;
//     local_1a0.x = 0x66;
//     local_1a0.y = -0x35;
//     bVar2 = false;
//     if ((short)local_168._2_2_ < 0) {
//       uVar12 = -(int)(short)local_168._2_2_;
//       bVar2 = true;
//     }
//     do {
//       sVar6 = (short)uVar12;
//       uVar12 = (int)sVar6 / 10;
//       local_1a0.u = uVar4 + (char)((uint)(((int)sVar6 % 10) * 0x10000) >> 0x10) * (char)local_1a0.w;
//       GsSortSprite(&local_1a0,OTablePt,0);
//       local_1a0.x = local_1a0.x + -0xc;
//     } while ((uVar12 & 0xffff) != 0);
//     if (bVar2) {
//       local_1a0.u = uVar4 + (char)local_1a0.w * '\n';
//       GsSortSprite(&local_1a0,OTablePt,0);
//     }
//     uVar12 = (uint)local_176;
//     local_1a0.x = 0x23;
//     local_1a0.y = -0x24;
//     do {
//       sVar6 = (short)uVar12;
//       uVar12 = (int)sVar6 / 10;
//       local_1a0.u = uVar4 + (char)((uint)(((int)sVar6 % 10) * 0x10000) >> 0x10) * (char)local_1a0.w;
//       GsSortSprite(&local_1a0,OTablePt,0);
//       local_1a0.x = local_1a0.x + -0xc;
//     } while ((uVar12 & 0xffff) != 0);
//     uVar12 = (uint)local_164._2_2_;
//     local_1a0.x = 0x66;
//     local_1a0.y = -0x24;
//     bVar2 = false;
//     if ((short)local_164._2_2_ < 0) {
//       uVar12 = -(int)(short)local_164._2_2_;
//       bVar2 = true;
//     }
//     do {
//       sVar6 = (short)uVar12;
//       uVar12 = (int)sVar6 / 10;
//       local_1a0.u = uVar4 + (char)((uint)(((int)sVar6 % 10) * 0x10000) >> 0x10) * (char)local_1a0.w;
//       GsSortSprite(&local_1a0,OTablePt,0);
//       local_1a0.x = local_1a0.x + -0xc;
//     } while ((uVar12 & 0xffff) != 0);
//     if (bVar2) {
//       local_1a0.u = uVar4 + (char)local_1a0.w * '\n';
//       GsSortSprite(&local_1a0,OTablePt,0);
//     }
//     uVar12 = local_160 & 0xffff;
//     local_1a0.x = 0x66;
//     local_1a0.y = -0x12;
//     bVar2 = false;
//     if ((short)local_160 < 0) {
//       uVar12 = -(int)(short)local_160;
//       bVar2 = true;
//     }
//     do {
//       sVar6 = (short)uVar12;
//       uVar12 = (int)sVar6 / 10;
//       local_1a0.u = uVar4 + (char)((uint)(((int)sVar6 % 10) * 0x10000) >> 0x10) * (char)local_1a0.w;
//       GsSortSprite(&local_1a0,OTablePt,0);
//       local_1a0.x = local_1a0.x + -0xc;
//     } while ((uVar12 & 0xffff) != 0);
//     if (bVar2) {
//       local_1a0.u = uVar4 + (char)local_1a0.w * '\n';
//       GsSortSprite(&local_1a0,OTablePt,0);
//     }
//     local_1a0.u = uVar4;
//     if (local_160._2_2_ == 4) {
//       iVar15 = GameClock * 0x1000;
//       iVar16 = (&DAT_8008e5bc)[*(short *)(&DAT_8008ed50 + (uint)(byte)PersistentState._5_1_ * 2)];
//       *(undefined2 *)(iVar16 + 0x6c) = 0x8a;
//       *(undefined2 *)(iVar16 + 0x6e) = 0xfff2;
//       *(undefined2 *)(iVar16 + 0x84) = 0x1000;
//       *(undefined2 *)(iVar16 + 0x86) = 0x1000;
//       iVar15 = rcos(iVar15 / 0x5a);
//       iVar15 = iVar15 * 0x50;
//       if (iVar15 < 0) {
//         iVar15 = iVar15 + 0xfff;
//       }
//       cVar3 = (char)(iVar15 >> 0xc) + '\x7f';
//       *(char *)(iVar16 + 0x7e) = cVar3;
//       *(char *)(iVar16 + 0x7d) = cVar3;
//       *(char *)(iVar16 + 0x7c) = cVar3;
//       GsSortSprite((GsSPRITE *)(iVar16 + 0x68),OTablePt,1);
//     }
//     uVar12 = 1;
//     iVar16 = 0;
//     do {
//       uVar4 = local_1a0.u;
//       sVar6 = (short)iVar16;
//       local_1a0.x = -0x8f;
//       local_1a0.y = sVar6 * 0x16 + 0x18;
//       if ((short)uVar12 < 0) {
//         uVar12 = -(int)(short)uVar12;
//         bVar2 = true;
//       }
//       else {
//         bVar2 = false;
//       }
//       do {
//         sVar7 = (short)uVar12;
//         uVar12 = (int)sVar7 / 10;
//         local_1a0.u = uVar4 + (char)((uint)(((int)sVar7 % 10) * 0x10000) >> 0x10) *
//                               (char)local_1a0.w;
//         GsSortSprite(&local_1a0,OTablePt,0);
//         local_1a0.x = local_1a0.x + -0xc;
//       } while ((uVar12 & 0xffff) != 0);
//       if (bVar2) {
//         local_1a0.u = uVar4 + (char)local_1a0.w * '\n';
//         GsSortSprite(&local_1a0,OTablePt,0);
//       }
//       iVar15 = (int)sVar6;
//       local_1a0.u = uVar4;
//       FUN_800515b0(&local_1a0,*(undefined4 *)(&PersistentState.field_0x458 + iVar15 * 4),0x79,
//                    iVar15 * 0x16 + 0x18,1);
//       bVar1 = (&PersistentState.field_0x44c)[iVar15];
//       local_a0[bVar1].x = -0x79;
//       local_a0[bVar1].y = (short)(iVar15 * 0x16) + 0x16;
//       uVar4 = 0x80;
//       if (iVar15 == local_30) {
//         iVar15 = rsin((GameClock * 0x1000) / 0x5a);
//         iVar15 = iVar15 * 0x3c;
//         if (iVar15 < 0) {
//           iVar15 = iVar15 + 0xfff;
//         }
//         uVar4 = (char)(iVar15 >> 0xc) + 'd';
//       }
//       local_a0[bVar1].b = uVar4;
//       local_a0[bVar1].g = uVar4;
//       local_a0[bVar1].r = uVar4;
//       GsSortSprite(local_a0 + bVar1,OTablePt,1);
//       bVar1 = (&PersistentState.field_0x451)[sVar6];
//       local_158[bVar1].b = '\x7f';
//       local_158[bVar1].g = '\x7f';
//                     /* Possible PsyQ macro: setSprt16() + setSemiTrans(sprt16, 1) +
//                        setShadeTex(sprt16, 1) */
//       local_158[bVar1].r = '\x7f';
//       local_158[bVar1].scaley = 0xb33;
//       local_158[bVar1].scalex = 0xb33;
//       local_158[bVar1].x = -0x2f;
//       local_158[bVar1].y = sVar6 * 0x16 + 0x16;
//       GsSortSprite(local_158 + bVar1,OTablePt,1);
//       iVar15 = iVar16 + 1;
//       uVar12 = iVar16 + 2;
//       iVar16 = iVar15;
//     } while (iVar15 * 0x10000 >> 0x10 < 3);
//     SkipFrame = 2;
//     EndDrawing(0);
//   }
//   bVar2 = false;
// LAB_80055c1c:
//   if (local_160._2_2_ == 4) {
//     iVar16 = (int)*(short *)(&DAT_8008ed50 + (uint)(byte)PersistentState._5_1_ * 2) +
//              (uint)(byte)PersistentState._4_1_ * 0x20;
//     if ((&PersistentState.field_0x40c)[iVar16] == -2) {
//       (&PersistentState.field_0x40c)[iVar16] = 1;
//     }
//     if ((&PersistentState.field_0x27)
//         [*(short *)(&DAT_8008ed50 + (uint)(byte)PersistentState._5_1_ * 2)] == -2) {
//       (&PersistentState.field_0x27)
//       [*(short *)(&DAT_8008ed50 + (uint)(byte)PersistentState._5_1_ * 2)] = 1;
//     }
//   }
//   FadeOutDirect(0x20,2,'\b','\b');
//   FUN_80038ce0();
//   if (PersistentState._92_1_ != '\0') {
//     puVar9 = PathFileRead((uchar *)"K:\\WORK\\CDIMAGE\\IMAGE\\",(uchar *)"font.tim");
//     LoadTIMAndFree(puVar9);
//     FUN_800514d8();
//   }
//   DisposeBG(local_34);
//   if (bVar2) {
//     uVar14 = 0x11;
//   }
//   else {
//     PersistentState._6_1_ = 0xff;
//     uVar14 = 0x10;
//   }
//   FUN_8004f6c0(uVar14);
//   return;
// }
