#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * static void SetupFly(struct param_fly *pfly, struct VECTOR *start, struct VECTOR *end, int yw, int yh, int time);
 *     ITEM.C:792, 39 src lines, frame 48 bytes, saved-reg mask 0x807f0000 (DEMO build -- see below)
 *
 * Original parameters and locals (the demo COUNT and TYPES are high-value
 * codegen evidence, not a retail spec: an earlier-build helper/API change
 * can replace either). Retail access widths and callee ABI win. A repeated
 * name is a nested-block scope, not a duplicate.
 * A ZERO-locals record is unverified, not a claim that the function has none:
 * vfree lists zero locals yet its byte-matched source needs seven.
 * The frame size and saved-reg mask above are the DEMO's: retail often needs
 * FEWER callee-saved registers (measured: Think1random exact; Think1chase's
 * 0x800f0000 = s0-s3+ra vs retail's s0,s1,ra). Treat them as an upper bound
 * and a hint at how many values stay live, never as a spec. The asm wins.
 * Locals:
 *     param $a0       struct param_fly * pfly
 *     param $a1       struct VECTOR * start
 *     param $a2       struct VECTOR * end
 *     param $s5       int yw
 *     param stack+16  int yh
 *     param stack+20  int time
 *     reg   $s6       int yh
 *     reg   $s0       int time
 *     reg   $a0       long len
 *     reg   $s1       struct tag_fly * fly
 * END PSX.SYM */

/*
 * STATUS: NON_MATCHING — 668 bytes / 167 instructions versus the 664-byte /
 * 166-instruction target.  The 0x30 frame and s0-s5+ra save set are exact.
 * This guarded checkpoint raises the authoritative fuzzy score from 38.79 to
 * 55.26; asmdiff reports 79 displayed lines in 23 blocks (91 raw-aligned
 * lines in 35 blocks; structural filter: 58 lines in 13 blocks).
 *
 * Two pure-C source identities close the former whole-function coloring
 * cascade.  `InitFly` keeps the initial copies and distance call behind one
 * pointer-formal boundary.  Its nested single-trip `do` blocks optimize away,
 * but give the original compiler enough loop-depth weight on selected `pfly`
 * references to cross the allocator's 31-to-32 reference threshold.  `pfly`,
 * `yw`, `time`, and the now-independent `len` consequently land in the
 * target's s2, s3, s0, and a0.  Mutating the two magnitude parameters after
 * the distance calculation prevents `len` from remaining live through the
 * jitter calls.  `SubFlyJitter` makes the Y result a branch return value, so
 * cc1 retains the target's duplicated subtract tails instead of merging them
 * into one post-branch expression.
 *
 * The remaining one-instruction excess and register residue are coupled.
 * Retail retains start/end in a1/a2 and fills three copy-load delay slots with
 * the late `yw` and call-argument moves.  This candidate coalesces those
 * pointers into a0/a1 in the prologue, leaving an extra load-use nop.  It also
 * colors scaled `yh`/the X midpoint into s1/s5 instead of the target's s5/s1,
 * and cross-jumps the symmetric X/Z jitter tails.  Pointer-formal variants for
 * each axis, address-taken result helpers, and separate product temporaries
 * were all bounded and rejected because they lost saved-register pressure or
 * reduced the byte score.  No asm, register pinning, volatile access, or
 * undefined-value fence is used.
 *
 * Everything structural is reproduced: the 6-arg prototype, mode/speed byte
 * stores, the `dist/time` speed clamp through the byte's own `& 0xff`
 * truncation, the pre-shifted hy and hy/2 asymmetric split, the X/Y/Z jitter
 * polarity, and `--expand-div` for the variable divisions.
 *
 * Matching notes (all verified against the original bytes):
 *  - SetupFly's true arity is 6, not Ghidra's 4: the already-matched callers
 *    (ReqItemArrow, ReqItemHappou, ReqItemLaunch) all declare and call it
 *    `void SetupFly(void *param, VECTOR *start, VECTOR *end, s32 a4, s32 a5,
 *    s32 a6)` — Ghidra drops `yh`/`time`, the two STACK-passed args past the
 *    four register ones (cookbook: Ghidra undercounts stack args). Kept the
 *    first parameter `long *pfly` here (matching Ghidra's own indexing
 *    style) rather than inventing `struct param_fly *` — nothing in this
 *    function needs the union's other members, and the three callers' own
 *    externs already keep it `void *` (respell per-TU; don't fight it).
 *  - `pfly->mode` (byte @0x28) is set to 0 as the FIRST statement, before
 *    start/end are even copied in — matches Ghidra's own rendering exactly.
 *  - The "speed" byte (@0x24, `dist / time`, a genuine variable division —
 *    needs `--expand-div`) is clamped to at least 1 when the division
 *    truncates to zero, using the BYTE value's own truncation as the test
 *    (`if ((speed & 0xff) != 0) goto skip_default;`), not a fresh compare of
 *    the untruncated quotient. It's copied to the adjacent byte (@0x25)
 *    later, interleaved with the X-magnitude scaling (read back through the
 *    pointer both times — no cached local survives that span, matching the
 *    fresh `lbu` reloads in the asm), and decremented once, last, right
 *    before return.
 *  - The X/Y/Z "current point" fields (@0x18/0x1c/0x20) share one shape —
 *    `if (range > 0) mid + (rand() % range OP half) else mid OP half` — but
 *    X and Z use a SYMMETRIC range (`2 * hx`, offset `hx`) while Y instead
 *    splits `half = hy / 2; other = hy - half;` and divides by `other`
 *    (the SAME asymmetric split as SetBlood's time jitter) — verified from
 *    the raw asm, not assumed by symmetry with X/Z. X ADDS its jitter, Y
 *    SUBTRACTS, Z ADDS again.
 *  - X and Z reuse the SAME magnitude `hx` (the yw-derived one, not a
 *    separate z-magnitude) for their own jitter range — confirmed by the
 *    raw asm recomputing `2 * hx` fresh at each site (not CSE'd across the
 *    distance), so both are written as the literal expression `hx * 2`
 *    rather than a shared named temp.
 */
extern int GetVectorDistance(VECTOR *v1, VECTOR *v2);

#ifndef NON_MATCHING
INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/SetupFly", SetupFly);
#else

static inline long InitFly(long *pfly, VECTOR *start, VECTOR *end)
{
    do
    {
        do
        {
            *((u8 *)pfly + 0x28) = 0;
            pfly[0] = start->vx;
        } while (0);
        pfly[1] = start->vy;
        pfly[2] = start->vz;
        pfly[3] = end->vx;
        pfly[4] = end->vy;
        pfly[5] = end->vz;
    } while (0);

    return GetVectorDistance(start, end);
}

static inline long SubFlyJitter(long mid, long half, long range)
{
    if (0 < range)
    {
        return mid - (rand() % range + half);
    }
    return mid - half;
}

void SetupFly(long *pfly, VECTOR *start, VECTOR *end, s32 yw, s32 yh, s32 time)
{
    long len;
    long v3;
    long v8;
    long midx;
    long midy;
    long midz;

    len = InitFly(pfly, start, end);
    if (0 < time)
    {
        *((u8 *)pfly + 0x24) = len / time;
        if ((*((u8 *)pfly + 0x24) & 0xff) != 0)
        {
            goto skip_default;
        }
    }
    *((u8 *)pfly + 0x24) = 1;
skip_default:
    yw = len * (yw / 2);
    *((u8 *)pfly + 0x25) = *((u8 *)pfly + 0x24);
    if (yw < 0)
    {
        yw = yw + 0xfff;
    }
    yh = len * (yh / 2);
    yw = yw >> 12;
    if (yh < 0)
    {
        yh = yh + 0xfff;
    }
    yh = yh >> 12;
    midx = (pfly[0] + pfly[3]) / 2;
    v8 = yw << 1;
    if (v8 < 1)
    {
        v8 = -yw;
    }
    else
    {
        v8 = rand() % v8 - yw;
    }
    midy = (pfly[1] + pfly[4]) / 2;
    v3 = yh / 2;
    yh = yh - v3;
    pfly[6] = midx + v8;
    v3 = SubFlyJitter(midy, v3, yh);
    midz = (pfly[2] + pfly[5]) / 2;
    v8 = yw << 1;
    pfly[7] = v3;
    if (v8 < 1)
    {
        yw = -yw;
    }
    else
    {
        yw = rand() % v8 - yw;
    }
    pfly[8] = midz + yw;
    *((u8 *)pfly + 0x24) = *((u8 *)pfly + 0x24) - 1;
}
#endif
