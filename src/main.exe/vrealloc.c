#include "common.h"
#include "main.exe.h"

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/vrealloc", vrealloc);

// triage: EASY — 129 insns, 4 callees, ~0.08 to AddMisc
// likely-relevant cookbook sections:
//   - Dispatch: if/switch ladder — reload vs CSE, signed vs unsigned

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// undefined * vrealloc(undefined *pt,ulong size)
//
// {
//   uchar *puVar1;
//   int *piVar2;
//   int iVar3;
//   ulong uVar4;
//   uint uVar5;
//   uint uVar6;
//   uint *puVar7;
//   uint uVar8;
//   int local_18;
//   uint local_14;
//
//   if (pt == (undefined *)0x0) {
//     puVar1 = valloc(size);
//   }
//   else {
//     if ((size & 3) != 0) {
//       size = size + 4;
//     }
//     uVar8 = size >> 2;
//     uVar5 = *(uint *)(pt + -8);
//     puVar7 = *(uint **)(pt + -4);
//     puVar1 = pt;
//     if (uVar5 < uVar8) {
//       if (((puVar7 == (uint *)0x0) || ((int)*puVar7 < 0)) ||
//          (uVar5 = uVar5 & 0x7fffffff, uVar5 + *puVar7 + 2 < uVar8)) {
//         puVar1 = valloc(uVar8 << 2);
//         if (pt != (undefined *)0x0) {
//           uVar4 = vsize(pt);
//           if (uVar8 < uVar4) {
//             uVar4 = uVar8;
//           }
//           memcpy(puVar1,pt,uVar4);
//         }
//         vfree(pt);
//       }
//       else {
//         *(uint *)(pt + -8) = uVar5;
//         uVar6 = (uVar5 + *puVar7) - uVar8;
//         if (uVar6 < 0x11) {
//           *(uint *)(pt + -8) = uVar5 + *puVar7 + 2 | 0x80000000;
//           *(uint *)(pt + -4) = puVar7[1];
//         }
//         else {
//           uVar5 = puVar7[1];
//           *(uint *)(pt + -8) = uVar8 | 0x80000000;
//           iVar3 = uVar8 * 4;
//           *(undefined **)(pt + -4) = pt + iVar3;
//           *(uint *)(pt + iVar3) = uVar6;
//           *(uint *)(pt + iVar3 + 4) = uVar5;
//         }
//       }
//     }
//     else {
//       *(uint *)(pt + -8) = uVar5 & 0x7fffffff;
//       uVar5 = (uVar5 & 0x7fffffff) - uVar8;
//       if (0x12 < uVar5) {
//         local_18 = uVar5 - 2;
//         local_14 = *(uint *)(pt + -4);
//         *(uint *)(pt + -8) = uVar8 | 0x80000000;
//         *(undefined **)(pt + -4) = pt + uVar8 * 4;
//         if ((puVar7 != (uint *)0x0) && ((~*puVar7 & 0x80000000) != 0)) {
//           local_18 = uVar5 + *puVar7;
//           local_14 = puVar7[1];
//         }
//         piVar2 = *(int **)(pt + -4);
//         *piVar2 = local_18;
//         piVar2[1] = local_14;
//       }
//     }
//   }
//   return puVar1;
// }
