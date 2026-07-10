#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * struct MotionDataType * SearchMotion(short id);
 *     ACTION.C:100, 22 src lines, frame 24 bytes, saved-reg mask 0x80000000
 *
 * Original parameters and locals (the demo build's register allocation may
 * differ from retail, but the COUNT and TYPES drive cc1's codegen and carry
 * over). A repeated name is a nested-block scope, not a duplicate:
 *     param $a1       short id
 *     reg   $a3       struct MotionPackType * mpd
 *     reg   $a2       short i
 *
 * Globals it touches, as the original declared them:
 *     extern struct MotionPackType *CommonMotion;
 *     extern struct MotionPackType *PlayerMotion;
 *     extern struct MotionPackType *StageMotion;
 * END PSX.SYM */

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/SearchMotion", SearchMotion);

// triage: MEDIUM — 82 insns, 5 loop, 0 callees, ~0.42 to GetHumanoid
// likely-relevant cookbook sections:
//   - Loops: 5 back-edge(s) — for/while/do vs goto shape
//   - gp vs absolute globals: gp-relative smalls — tools/gpsyms.py

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// MotionDataType * SearchMotion(short id)
//
// {
//   int iVar1;
//   MotionDataType *pMVar2;
//   int iVar3;
//
//   if (CommonMotion != (int *)0x0) {
//     iVar3 = 0;
//     if (0 < *CommonMotion) {
//       iVar1 = 0;
//       do {
//         pMVar2 = *(MotionDataType **)((int)CommonMotion + (iVar1 >> 0xe) + 4);
//         iVar3 = iVar3 + 1;
//         if (pMVar2->id == id) {
//           return pMVar2;
//         }
//         iVar1 = iVar3 * 0x10000;
//       } while (iVar3 * 0x10000 >> 0x10 < *CommonMotion);
//     }
//   }
//   if (PlayerMotion != (int *)0x0) {
//     iVar3 = 0;
//     if (0 < *PlayerMotion) {
//       iVar1 = 0;
//       do {
//         pMVar2 = *(MotionDataType **)((int)PlayerMotion + (iVar1 >> 0xe) + 4);
//         iVar3 = iVar3 + 1;
//         if (pMVar2->id == id) {
//           return pMVar2;
//         }
//         iVar1 = iVar3 * 0x10000;
//       } while (iVar3 * 0x10000 >> 0x10 < *PlayerMotion);
//     }
//   }
//   if (StageMotion != (int *)0x0) {
//     iVar3 = 0;
//     if (0 < *StageMotion) {
//       iVar1 = 0;
//       do {
//         pMVar2 = *(MotionDataType **)((int)StageMotion + (iVar1 >> 0xe) + 4);
//         iVar3 = iVar3 + 1;
//         if (pMVar2->id == id) {
//           return pMVar2;
//         }
//         iVar1 = iVar3 * 0x10000;
//       } while (iVar3 * 0x10000 >> 0x10 < *StageMotion);
//     }
//   }
//   return (MotionDataType *)0x0;
// }
