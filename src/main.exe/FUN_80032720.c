#include "common.h"
#include "main.exe.h"
#include "effect.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 *
 * Globals it touches, as the original declared them:
 *     extern struct tag_EffectSlot EffectSlot[200];
 *
 * PSX.SYM suggests this may be `SetPadState` (LOW confidence, EFFECT.C) — NOT
 * adopted. Corroborate with `tools/callmatch.py --verify` before renaming.
 * END PSX.SYM */

/*
 * STATUS: NON_MATCHING — exact target extent (560 bytes / 140 instructions),
 * 25 differing bytes, fuzzy 90.00. A volatile shifted copy now reproduces
 * the target's otherwise-unreachable `scrollY` stack round-trip: `sll` +
 * `sw`, then one `lw` + `sra` per outer-loop iteration. Removing Ghidra's
 * redundant `& 0x1f` from the shift count also removes the draft's extra
 * `andi`; j/i only produce counts 0..3, so this is behaviorally identical.
 *
 * The remaining residual is one stack-slot ordering and its small scheduler
 * cascade. The target first homes parameters y/z at sp+0x10/sp+0x12, then
 * the reload spill allocator puts shifted scrollY at sp+0x14. An explicit C
 * volatile is allocated first instead (sp+0x10), moving y/z to sp+0x14/
 * sp+0x16. The inner loop consequently uses different temporary registers
 * for its signed i, shift constant, and second multiply result. Explicit
 * aggregate or separate volatile homes can force the target offsets, but
 * perturb the pool-search and tail schedules and regress to 107+ differing
 * bytes. Keep this exact-length checkpoint until a natural allocator-spill
 * source shape is found.
 *
 * FUN_80032720 (0x80032720, 0x230 bytes) — spawns an EffectSlot pool entry
 * that draws a small animated 2x2-cell water/warp tile grid from a texture
 * page (AddMisc.c: `extern void FUN_80032720(GsIMAGE *im, short y, short
 * z);`, called with the just-uploaded TIM's own GsIMAGE). Same round-robin
 * EffectSlot[200] pool search as SetSplash/SetFrame/SetBleed/SetSmoke (see
 * SetSplash.c for the shared idiom writeup — goto loop instead of
 * while(1)+break so loop.c doesn't hoist `&dmy`'s address, idx-computed-
 * before-slot, cursor-update store living inside `if (slot->proc==0)
 * {...break;}` for the right branch polarity).
 *
 * The found slot's `param` union is written through FIVE different
 * overlapping views (proven by `tools/access.py --order`, not Ghidra's own
 * struct-name guesses):
 *  - smoke.vec.vy/vx (offsets 2/0) zeroed;
 *  - splash.sx/sy (offsets 0xc/0xe) get the pool's own scroll cursor
 *    (`D_80097F30`/`D_80097F32`, a shared animated texture-scroll offset
 *    also advanced at the very end);
 *  - offsets 8/0xa get `im->px`/`im->py` (the GsIMAGE's own texture-page
 *    position) through a RAW OFFSET CAST — this does NOT line up with any
 *    proven named field at that width (BleedType.pos.vz is a 4-byte field
 *    there, not two shorts): a genuinely divergent union member, per the
 *    cookbook's "reach it via an explicit offset cast off the SAME proven
 *    pointer, don't invent a new named struct" rule;
 *  - bleed.vec.vx/vy/vz/pad (offsets 0x10/0x12/0x14/0x16) get the SAME
 *    im->px/im->py plus im->pw/im->ph (the tile's pixel size) — this is
 *    the actual source RECT the 2x2 grid's MoveImage calls read from;
 *  - smoke.vec.vz/pad (offsets 4/6) get this call's own `y`/`z` params,
 *    written only at the very end (right before `ef->proc` is set),
 *    matching the target's own late spill-reload from their stack homes.
 *
 * Matching notes:
 *  - The 2x2 grid loop uses `short` counters (`j`,`i`), not `int` — a
 *    `short` loop counter suppresses loop.c's strength reduction and keeps
 *    the target's own `(x<<0x10)>>0x10`-style recompute-from-base shape
 *    (cookbook: "a short loop counter suppresses strength reduction").
 *  - The bit test `(0xF >> (j*2+i)) & 1` is always true for j,i in {0,1}
 *    (indices 0-3, all set in 0xF), but the target still computes it. Ghidra
 *    renders an extra `& 0x1F` because MIPS variable shifts mask their count
 *    in hardware; retaining that decompiler artifact emits a real `andi`
 *    which is absent from the target.
 *  - `D_80097F30`/`D_80097F32` are read ONCE into named locals right
 *    after the slot is found (not hoisted to the top the way Ghidra's own
 *    SSA rendering shows `sVar2 = DAT_80097f32; sVar3 = DAT_80097f30;` as
 *    the first two statements) — the raw .s doesn't read them until deep
 *    into the found-body, immediately before the splash.sx/sy stores
 *    (cookbook: "trust the assembly over Ghidra's statement order").
 */

typedef struct RECT RECT;
struct RECT
{
    short x, y, w, h;
};

extern void MoveImage(RECT *rect, int x, int y);
extern void UpdateTexScroll(void);
extern s16 D_80097F30;
extern s16 D_80097F32;

#ifndef NON_MATCHING
INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/FUN_80032720", FUN_80032720);
#else

void FUN_80032720(GsIMAGE *im, short y, short z)
{
    int idx;
    TEffectSlot *base;
    TEffectSlot *slot;
    int count;
    TEffectSlot *ef;
    union EffectParam *pp;
    s16 scrollX;
    short scrollY;
    short px;
    short py;
    short j;
    short i;

    idx = CURRENT_OFFSET_INTO_SOME_SELF_CALL_STRUCT_AREA_;
    count = 0;
    base = EffectSlot;
    slot = base + idx;
loop:
    idx = idx + 1;
    slot = slot + 1;
    if (199 < idx)
    {
        slot = base;
        idx = 0;
    }
    if (slot->proc == 0)
    {
        CURRENT_OFFSET_INTO_SOME_SELF_CALL_STRUCT_AREA_ = idx + 1;
        if (199 < idx + 1)
        {
            CURRENT_OFFSET_INTO_SOME_SELF_CALL_STRUCT_AREA_ = 0;
        }
        ef = slot;
        goto found;
    }
    count = count + 1;
    if (199 < count)
    {
        ef = &dmy;
        goto found;
    }
    goto loop;
found:
{
    volatile u32 scrollYShifted;

    pp = &ef->param;
    pp->smoke.vec.vy = 0;
    ef->param.smoke.vec.vx = 0;

    scrollX = D_80097F30;
    scrollY = D_80097F32;
    pp->splash.sx = scrollX;
    pp->splash.sy = scrollY;

    px = im->px;
    *(short *)((u8 *)pp + 8) = px;
    pp->bleed.vec.vx = px;
    py = im->py;
    *(short *)((u8 *)pp + 0xA) = py;
    pp->bleed.vec.vy = py;
    pp->bleed.vec.vz = im->pw;
    pp->bleed.vec.pad = im->ph;
    scrollYShifted = (u32)(u16)scrollY << 16;

    for (j = 0; j < 2; j++)
    {
        s32 signedScrollY = (s32)scrollYShifted >> 16;

        for (i = 0; i < 2; i++)
        {
            if ((0xF >> (j * 2 + i)) & 1)
            {
                MoveImage((RECT *)&pp->bleed.vec, scrollX + im->pw * i,
                          signedScrollY + im->ph * j);
            }
        }
    }

    D_80097F32 = D_80097F32 + 0x40;
    pp->smoke.vec.vz = y;
    pp->smoke.vec.pad = z;
    ef->proc = (void (*)())UpdateTexScroll;
    if (0x200 < D_80097F32)
    {
        D_80097F32 = 0x100;
        D_80097F30 = D_80097F30 + 0x40;
    }
}
}
#endif
