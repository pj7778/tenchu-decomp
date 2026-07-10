#include "common.h"
#include "main.exe.h"
#include "item.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * short SoundEx(struct VECTOR *locate, short seid);
 *     SEMNG.C:42, 15 src lines, frame 40 bytes, saved-reg mask 0x800f0000 (DEMO build -- see below)
 *
 * Original parameters and locals (the demo build's register allocation may
 * differ from retail, but the COUNT and TYPES drive cc1's codegen and carry
 * over). A repeated name is a nested-block scope, not a duplicate.
 * The frame size and saved-reg mask above are the DEMO's: retail often needs
 * FEWER callee-saved registers (measured: Think1random exact; Think1chase's
 * 0x800f0000 = s0-s3+ra vs retail's s0,s1,ra). Treat them as an upper bound
 * and a hint at how many values stay live, never as a spec. The asm wins.
 * Locals:
 *     param $a0       struct VECTOR * locate
 *     param $a1       short seid
 *
 * Globals it touches, as the original declared them:
 *     extern struct Humanoid *StagePlayer;
 *     extern struct TCameraStatus CamState;
 * END PSX.SYM */

/*
 * STATUS: NON_MATCHING — 7 of 512 bytes differ (128 of 128 instructions; the
 * function is otherwise byte-identical). The whole residual is one 3-instruction
 * window in the vertical-distance abs block and reduces to a single sub-C
 * register-coloring choice — see RESIDUAL below.
 *
 * SoundEx (0x8004fc70) — play a positional sound effect: if `locate` is null
 * or IS the player's own position, play at a fixed volume (0x3f for seid
 * 0x12, else 0x7f), no direction encoded. Otherwise compute planar+vertical
 * distance from the player; give up (return -1) past 18000/10000 units. If
 * both distances are under 2000, full volume with no direction; otherwise a
 * distance-attenuated volume, ORed with the (clamped, +-0x1000 wrapped)
 * angle from the player to `locate` (relative to the camera's own direction
 * offset in CMODE_DIRECTION) shifted up 8 bits.
 *
 * Matching notes (all verified against the original bytes):
 *  - The two branches of `if (locate == 0 || locate == pp)` each end their
 *    OWN `return PlaySE(...)` — this is NOT a single shared tail after an
 *    if/else. The asm shows the short branch materializing
 *    STAGE_SOUNDS_POINTER/seid and jumping directly to the `jal PlaySE`
 *    instruction, skipping the long branch's own OR-with-angle setup
 *    entirely; jump.c's cross-jumping merged only the byte-identical
 *    `jal PlaySE` + return tail, not the differing argument setup. Ghidra's
 *    `uVar4 = iVar5 << 8 | uVar4;` placed AFTER the if/else is a decompiler
 *    normalization artifact — the short branch never executes that OR.
 *  - **`dist` must carry the final `volume` value** (`dist = 0x7f - ...; dist =
 *    (dist * ...);` then `vol = (angle<<8)|dist`). The SquareRoot0 result and
 *    the volume coalesce into ONE callee-saved register ($s1) in the target,
 *    because `volume` is computed in the ratan2 call's delay slot and survives
 *    the call (forced callee-saved), and `dist` dies exactly where `volume` is
 *    born. Spelling `volume` as its own separate local instead keeps `dist`
 *    caller-saved (a distinct register) and mismatches the entire function via
 *    an extra callee-saved register (frame 0x28 vs the target's 0x30) — 51+
 *    diff lines. This one rewrite collapsed the residual from 61 lines to 5.
 *  - `vol`'s division-by-constant sequences (`mult` + `sra` sign-correction,
 *    not `multu`/`srl`) confirm the intermediate arithmetic is signed —
 *    `dist`/`dy` must stay plain `s32`, not unsigned, for the magic-multiply
 *    constants to match.
 *  - The vertical-distance abs uses a `raw` temp reused for the ratan2 result:
 *    a persistent `raw` keeps the subtraction result caller-saved and separate
 *    from the abs `dy` (the target's `move a0,v0; negu a0,a0` shape). A `raw`
 *    that dies immediately coalesces with `dy` (direct `subu a0` + a speculative
 *    duplicated `slti`, +1 insn); reusing `angle` for it instead colours the
 *    subtraction into $a2 not $v0. This split got it to 7 bytes.
 *  - `CamState` field types/offsets copied verbatim from DoInfoViewProc.c's
 *    proven (fully-matched) local typedef: Mode is a plain `s32` (not the
 *    `enum` type) at 0x14, DirectionRY a signed `s16` at 0x1A.
 *
 * RESIDUAL (7 bytes, same length): the vertical-distance abs's raw subtraction
 * `raw = locate->vy - pp->vy` colours `raw` into $v1 (ours) where the target
 * uses $v0. `locate->vy` loads into $v0 (target) vs $v1 (ours) — two freely
 * interchangeable caller-saved temps; the branch/move/negu that follow all
 * inherit that one choice (`subu v0,v0,v1 / bgez v0 / move a0,v0 / negu a0,a0`
 * vs `subu v1,v1,v0 / bgez v1 / move a0,v1 / negu a0,v1`). No C-level lever
 * reaches it: every abs spelling tried (single-var, two-var `dy=raw`, ternary,
 * dedicated-dying-temp, `angle`-reuse) lands the temp in $v1/$a2 or coalesces
 * it away. tools/regalloc.py shows no copy-chain to break; tools/autorules.py
 * found no width win. Two decomp-permuter runs (25k + 14k iterations, both
 * --stop-on-zero) never reached 0 — best was a garbage `if(dist){A}else{A}`
 * dead-branch at score 35. Same abs-value register-role class that parked the
 * SEMNG/HUMAN sibling GetTargetDistance.c.
 */
extern Humanoid *StagePlayer;

typedef struct
{
    VECTOR TargetVector;         /* 0x00 */
    Humanoid *Owner;              /* 0x10 */
    s32 Mode;                    /* 0x14 */
    s16 DirectionRX;              /* 0x18 */
    s16 DirectionRY;              /* 0x1A */
    s32 OldMode;                  /* 0x1C */
    u8 Valiation;                 /* 0x20 */
} TCameraStatus;

extern TCameraStatus CamState;

enum { CMODE_DIRECTION = 1 };

typedef struct
{
    s16 VABid;
    s16 program;
    void *VABhead;
} SoundEffect;

extern SoundEffect *STAGE_SOUNDS_POINTER;

extern s32 SquareRoot0(s32 x);
extern s32 ratan2(s32 y, s32 x);
extern short PlaySE(SoundEffect *se, short pt, long dv);

#ifndef NON_MATCHING
INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/SoundEx", SoundEx);
#else
short SoundEx(VECTOR *locate, short seid)
{
    VECTOR *pp;
    s32 dist, dx, dz, dy;
    s32 angle;
    s32 vol;
    s32 raw;

    pp = StagePlayer->locate;
    if (locate == 0 || locate == pp) {
        return PlaySE(STAGE_SOUNDS_POINTER, seid, (seid == 0x12) ? 0x3f : 0x7f);
    }

    dx = locate->vx - pp->vx;
    dz = locate->vz - pp->vz;
    dist = SquareRoot0(dx * dx + dz * dz);
    if (17999 < dist) {
        return -1;
    }
    raw = locate->vy - pp->vy;
    dy = raw;
    if (raw < 0) {
        dy = -dy;
    }
    if (9999 < dy) {
        return -1;
    }
    if (dist < 2000 && dy < 2000) {
        angle = 0;
        dist = 0x7f;
    } else {
        dist = 0x7f - (dist << 7) / 18000;
        dist = (dist * (10000 - dy)) / 10000;
        raw = ratan2(-dx, -dz);
        angle = raw - StagePlayer->rotate->vy;
        if (CamState.Mode == CMODE_DIRECTION) {
            angle -= CamState.DirectionRY;
        }
        if (0x800 < angle) {
            angle = 0x1000 - angle;
        } else {
            if (angle < -0x7ff) {
                angle += 0x1000;
            }
        }
    }
    vol = (angle << 8) | dist;
    return PlaySE(STAGE_SOUNDS_POINTER, seid, vol);
}
#endif /* NON_MATCHING */

// triage: MEDIUM — 128 insns, mul/div, 3 callees, ~0.06 to GetVectorRotation
// likely-relevant cookbook sections:
//   - Dispatch: if/switch ladder — reload vs CSE, signed vs unsigned
//   - Expressions: mult/div — magic-multiply constants, fold
//   - gp vs absolute globals: gp-relative smalls — tools/gpsyms.py

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// short SoundEx(VECTOR *locate,short seid)
//
// {
//   short sVar1;
//   long lVar2;
//   int iVar3;
//   uint uVar4;
//   int iVar5;
//   VECTOR *pVVar6;
//   int iVar7;
//   int iVar8;
//
//   pVVar6 = StagePlayer->locate;
//   if ((locate == (VECTOR *)0x0) || (locate == pVVar6)) {
//     uVar4 = 0x7f;
//     if (seid == 0x12) {
//       uVar4 = 0x3f;
//     }
//   }
//   else {
//     iVar8 = locate->vx - pVVar6->vx;
//     iVar7 = locate->vz - pVVar6->vz;
//     lVar2 = SquareRoot0(iVar8 * iVar8 + iVar7 * iVar7);
//     if (17999 < lVar2) {
//       return -1;
//     }
//     iVar3 = locate->vy - pVVar6->vy;
//     if (iVar3 < 0) {
//       iVar3 = -iVar3;
//     }
//     if (9999 < iVar3) {
//       return -1;
//     }
//     if ((lVar2 < 2000) && (iVar5 = 0, iVar3 < 2000)) {
//       uVar4 = 0x7f;
//     }
//     else {
//       uVar4 = ((0x7f - (lVar2 << 7) / 18000) * (10000 - iVar3)) / 10000;
//       lVar2 = ratan2(-iVar8,-iVar7);
//       iVar5 = lVar2 - StagePlayer->rotate->vy;
//       if (CamState.Mode == CMODE_DIRECTION) {
//         iVar5 = iVar5 - CamState.DirectionRY;
//       }
//       if (iVar5 < 0x801) {
//         if (iVar5 < -0x7ff) {
//           iVar5 = iVar5 + 0x1000;
//         }
//       }
//       else {
//         iVar5 = 0x1000 - iVar5;
//       }
//     }
//     uVar4 = iVar5 << 8 | uVar4;
//   }
//   sVar1 = PlaySE(PTR_80097cb0,seid,uVar4);
//   return sVar1;
// }
