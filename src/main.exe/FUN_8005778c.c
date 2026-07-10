#include "common.h"
#include "main.exe.h"

/*
 * STATUS: NON_MATCHING — 149 of 388 bytes differ (length matches: 97
 * instructions both sides).
 *
 * FUN_8005778c (0x8005778c, 0x184 bytes) — draws a bitmap-font glyph: grabs
 * a POLY_GT4 from the work base (and advances it, like FUN_80038c0c's
 * siblings), remaps the raw character code `param_4` to a cell index in the
 * FONT_IMAGE_ sheet (0x92 is special-cased to 0x27, then codes >=0x20/>=0xc0
 * fold down by 0x20/0x40 — a half-width-kana-style remap), copies the whole
 * FONT_IMAGE_ GsIMAGE descriptor onto the stack (one struct assignment —
 * see load_font_image_into_global.c for the identical 7-word unroll), slides
 * its px/py by the cell's (col,row) within the sheet (3 px wide, 0x10 px
 * tall cells), computes an extra x-nudge `sVar3` for a handful of special
 * codes (0xc7/0xe7 get -4/-2/3, everything else in [0xc0,0xdf) or
 * [0xe0,0xff) gets 0), then calls SetupImageToPolyGT4/AddPrim exactly like
 * FUN_80038c0c's neighbours.
 *
 * Matching notes:
 *  - `local_38 = FONT_IMAGE_;` (a plain GsIMAGE struct assignment) is the
 *    proven load_font_image_into_global.c idiom, reused here in the other
 *    direction (global -> stack).
 *  - The `sVar3` nudge is Ghidra's literal `if ((0x1f < (uint)(param_4 -
 *    0xc0)) || (sVar3 = -4, param_4 == 199)) {...}` — an `||` whose SECOND
 *    operand is a comma expression that unconditionally sets `sVar3 = -4`
 *    before testing `== 199`. Writing it as C's own short-circuit `||`
 *    reproduces the control flow directly: transcribe literally, don't
 *    "simplify" the comma away (the -4 must survive to the join point even
 *    on the branch where the equality test fails).
 *  - `SetupImageToPolyGT4`'s `w`/`h` parameters must be declared `short`
 *    (not `s32`, even though other TUs' externs use `s32`) — the RAW
 *    `param_3 + sVar3` sum must be truncated to 16 bits AFTER the addition
 *    (matches the target's `addu` then `sll/sra`), not with param_3
 *    sign-extended to int BEFORE the add (a per-TU prototype-width lever,
 *    same family as GetRealPad's caller-side return-type lever).
 *  - Computing the 4th call argument as its OWN statement
 *    (`short h = param_3 + sVar3; SetupImageToPolyGT4(...,param_2,h);`)
 *    rather than inline in the call fixes a param_2/param_3 <-> s2/s3
 *    register-assignment swap (both parameters are referenced exactly
 *    once, at the very end, with otherwise-identical live ranges — a
 *    classic equal-priority tie où global-alloc's processing order
 *    flips based on which pseudo's "final use" instruction comes first
 *    in the RTL stream).
 *  - Residual (149 bytes, same overall length): the two-stage clamp
 *    (`0x1f<uVar4` then `uVar4<0xc0`, folding down by 0x20/0x40) wants a
 *    literal "eager copy, THEN test the copy" shape — the target computes
 *    `a0 = uVar4;` as a real instruction BEFORE testing `a0<0x20`, and
 *    again produces a fresh `a1 = (adjusted)` copy for the second stage —
 *    but no C spelling tried (`t1 = uVar4; if (0x1f < t1) t1 -= 0x20;`,
 *    self-referencing `t1 -= 0x20`, explicit if/else, chained default-
 *    then-override with per-stage temps) stops cc1 from testing the
 *    ORIGINAL register directly and only materializing the copy in the
 *    branch's delay slot (mathematically identical, wrong instruction
 *    order/count — one word short per stage). This shifts the `local_38.
 *    px`/`py` store scheduling and the s0(param_4)-save prologue position
 *    too (same root cause, different symptom). Tried: ~9 source
 *    reshapings, `tools/autorules.py` (found the `sVar3: short->s32`
 *    win, applied), one bounded `tools/permute.py` run (300s/stop-on-
 *    zero, best score 1000 — its top candidate silently DROPPED the
 *    `uVar2 & 0xf` mask, a wrong-value regression, not a real fix).
 *    Recognize as a sub-C-level scheduling/regalloc tie per the cookbook
 *    and park rather than keep guessing respellings.
 */
#ifndef NON_MATCHING
INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/FUN_8005778c", FUN_8005778c);
#else
typedef struct
{
    u_long tag;
    u_char r0, g0, b0, code;
    short x0, y0;
    u_char u0, v0;
    u_short clut;
    u_char r1, g1, b1, p1;
    short x1, y1;
    u_char u1, v1;
    u_short tpage;
    u_char r2, g2, b2, p2;
    short x2, y2;
    u_char u2, v2;
    u_short pad2;
    u_char r3, g3, b3, p3;
    short x3, y3;
    u_char u3, v3;
    u_short pad3;
} POLY_GT4;

extern GsIMAGE FONT_IMAGE_;
extern void *GsGetWorkBase(void);
extern void GsSetWorkBase(void *workBase);
extern void SetupImageToPolyGT4(GsIMAGE *image, void *quad, short w, short h);
extern void AddPrim(u8 *ot, u8 *prim);

void FUN_8005778c(void *param_1, short param_2, short param_3, u32 param_4)
{
    u16 uVar2;
    POLY_GT4 *ply;
    s32 sVar3;
    s32 uVar4;
    s32 t1;
    s32 t2;
    GsIMAGE local_38;

    ply = (POLY_GT4 *)GsGetWorkBase();
    GsSetWorkBase(ply + 1);
    param_4 = param_4 & 0xff;
    uVar4 = param_4;
    if (param_4 == 0x92)
    {
        uVar4 = 0x27;
    }
    t1 = uVar4;
    if (0x1f < t1)
    {
        t1 = t1 - 0x20;
    }
    if (uVar4 < 0xc0)
    {
        t2 = t1;
    }
    else
    {
        t2 = t1 - 0x40;
    }
    uVar4 = t2;
    uVar2 = (u16)uVar4;
    local_38 = FONT_IMAGE_;
    if ((int)uVar4 < 0)
    {
        uVar4 = uVar4 + 0xf;
    }
    local_38.pw = 3;
    local_38.ph = 0x10;
    local_38.py = local_38.py + (short)((int)uVar4 >> 4) * 0x10;
    local_38.px = local_38.px + (uVar2 & 0xf) * 3;
    if ((0x1f < param_4 - 0xc0) || (sVar3 = -4, param_4 == 199))
    {
        if (param_4 - 0xe0 < 0x20)
        {
            sVar3 = -2;
            if (param_4 == 0xe7)
            {
                sVar3 = 3;
            }
        }
        else
        {
            sVar3 = 0;
        }
    }
    {
        short h = param_3 + sVar3;
        SetupImageToPolyGT4(&local_38, ply, param_2, h);
    }
    AddPrim(param_1, ply);
}
#endif
