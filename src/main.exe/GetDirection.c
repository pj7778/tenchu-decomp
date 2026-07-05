#include "common.h"
#include "main.exe.h"

/*
 * GetDirection (0x8002972c, 0x68 bytes) — angle-difference-to-target,
 * wrapped into [-0x800, 0x7FF] (a 0x1000/4096 = one full turn representation,
 * same units as SVECTOR rotation components). Same "Humanoid control" TU as
 * is_character_state_present_on_stage_.c; called by turn_towards_player_.
 *
 * `diff` (the raw ratan2()-roty subtraction) stays WIDE (s32) and feeds the
 * two wrap corrections (0x1000-diff / diff+0x1000) directly; a SEPARATE
 * s16 truncated copy (`sdiff`) is compared against the range constants. The
 * asm proves the split: the comparisons read a sign-extended $a0 (one
 * sll+sra, CSE'd across both slti's), while the arithmetic reads the
 * original untruncated $v1 — if `diff` itself were s16, the arithmetic
 * would have to reuse the SAME truncated register instead of a second one.
 * `result` defaults to `diff` (the mid-range case) as the FIRST statement,
 * unconditionally, before either test — this default-then-override shape
 * (not if/else-if/else) is what lets reorg hoist the default assignment
 * into the first branch's delay slot, harmless on the "too high" path
 * since it's overwritten before use there. The final (short)-return
 * truncation then applies to whichever value `result` holds: reorg
 * speculatively duplicates it into the second test's delay slot (valid on
 * the "mid-range" not-taken path, where `result` is already the default)
 * as well as the shared join reached by the "too high"/"too low" paths.
 */
extern s32 ratan2(s32 a, s32 b);

s16 GetDirection(s32 dx, s32 dz, s32 roty)
{
    s32 diff;
    s16 sdiff;
    s16 result;

    diff = ratan2(-dx, -dz) - roty;
    sdiff = diff;
    result = diff;
    if (sdiff >= 0x801)
    {
        result = 0x1000 - diff;
    }
    else if (sdiff < -0x7FF)
    {
        result = diff + 0x1000;
    }
    return result;
}
