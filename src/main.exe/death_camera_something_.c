#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/death_camera_something_", death_camera_something_);

// triage: MEDIUM — 130 insns, mul/div, 8 callees, ~0.06 to ProcItemGoshikimai
// likely-relevant cookbook sections:
//   - Dispatch: if/switch ladder — reload vs CSE, signed vs unsigned
//   - Expressions: mult/div — magic-multiply constants, fold
//   - gp vs absolute globals: gp-relative smalls — tools/gpsyms.py

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// /* WARNING: Globals starting with '_' overlap smaller symbols at the same address */
//
// void FUN_8003818c(int param_1)
//
// {
//   short sVar1;
//   GsCOORDINATE2 *pGVar2;
//   int iVar3;
//   VECTOR *pVVar4;
//   long lVar5;
//   VECTOR local_48;
//   MATRIX MStack_38;
//   long lStack_18;
//   undefined2 local_14;
//   long lStack_10;
//   long lStack_c;
//
//   if ((*(short *)(*(int *)(param_1 + 0x5c) + 4) < 0) && ((*(ushort *)(param_1 + 0x28) & 0xc) == 0))
//   {
//     if (-1 < *(int *)(param_1 + 0x80)) {
//       iVar3 = *(int *)(param_1 + 0x80) + 0x88;
//       if ((*(ushort *)(param_1 + 0x28) & 0x100) != 0) {
//         if ((GameClock & 0xfU) != 0) {
//           return;
//         }
//         FUN_80037e0c(param_1,0);
//         return;
//       }
//       *(int *)(param_1 + 0x80) = iVar3;
//       if (0x1000 < iVar3) {
//         *(undefined4 *)(param_1 + 0x80) = 0x1000;
//       }
//       pVVar4 = GetAbsolutePosition((ModelType *)**(undefined4 **)(*(int *)(param_1 + 0x58) + 0x68),0
//                                    ,0,0);
//       pGVar2 = DAT_80097f38;
//       sVar1 = *(short *)(*(int *)(param_1 + 0x58) + 0x56);
//       pVVar4->vy = *(long *)(*(int *)(param_1 + 0x58) + 0x1c);
//       (pGVar2->coord).t[0] = pVVar4->vx;
//       (pGVar2->coord).t[1] = pVVar4->vy;
//       (pGVar2->coord).t[2] = pVVar4->vz;
//       iVar3 = *(int *)(param_1 + 0x80) * -(int)sVar1;
//       if (iVar3 < 0) {
//         iVar3 = iVar3 + 0x3ff;
//       }
//       local_48.vx = (iVar3 >> 10) - (*(int *)(param_1 + 0x24) >> 1);
//       if (*(char *)(param_1 + 0x2f) == '\0') {
//         *(undefined2 *)&pGVar2[1].flg = 0;
//         *(undefined2 *)((int)&pGVar2[1].flg + 2) = 0;
//         pGVar2[1].coord.m[0][0] = 0;
//       }
//       else {
//         *(undefined2 *)&pGVar2[1].flg = 0x100;
//         sVar1 = RefrectVector[*(byte *)(param_1 + 0x2f)];
//         pGVar2[1].coord.m[0][0] = 0;
//         *(short *)((int)&pGVar2[1].flg + 2) = sVar1;
//       }
//       local_48.vy = local_48.vx;
//       local_48.vz = local_48.vx;
//       RotMatrixYXZ((SVECTOR *)(DAT_80097f38 + 1),&DAT_80097f38->coord);
//       ScaleMatrix(&DAT_80097f38->coord,&local_48);
//       pGVar2 = DAT_80097f38;
//       DAT_80097f38->flg = 0;
//       GsGetLs(pGVar2,&MStack_38);
//       GsSetLsMatrix(&MStack_38);
//       lVar5 = RotTransPers(&UnitVector,&lStack_18,&lStack_10,&lStack_c);
//       local_14 = (undefined2)lVar5;
//       if (0x4e1 < (lVar5 << 0x10) >> 0x12) {
//         return;
//       }
//       _DrawTMDmode = 0x20;
//       DrawTMD(DAT_80097f38[1].coord.m[2] + 2,OTablePt,0);
//       return;
//     }
//   }
//   *(undefined4 *)(param_1 + 0x80) = 0;
//   return;
// }
