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
 * END PSX.SYM */

/*
 * STATUS: NON_MATCHING — 16 of 248 linked and whole-image bytes differ. The
 * candidate has the target's exact 62 instructions and optimized CFG: 4
 * conditional branches, 1 unconditional jump, no calls, and 1 return.
 *
 * FUN_8003944c (0x8003944c, 0xf8 bytes) — EFFECT.C effect-pool allocator:
 * same EffectSlot[200] round-robin search as SetSplash/SetFrame/SetBleed/
 * FUN_80038fdc (see SetSplash.c for the full writeup of the shared idioms).
 * Unlike FUN_80038fdc, this one fills EVERY BloodType field straight from
 * its own 10 parameters (a raw "spawn a blood-family effect exactly as
 * told" setter, not a computed one like SetBlood.c) and hands the slot to
 * DrawImpact (calls RotTransPers/GsSortSprite/GetScreenPosition — the same
 * projection/draw family as DrawBlood.c). Called by DamageControl,
 * ProcItemGosin and
 * ProcItemShinsoku (all still asm) — likely something like SetBloodDirect/
 * AddBloodEffect; no candidate in reference/psxsym-candidates.tsv to
 * corroborate.
 *
 * param_9/param_10 are stack args read with `lhu` (16-bit) even though
 * Ghidra types them `undefined1`/`uchar` and only their low byte is ever
 * stored (`sb`) — trusting the raw access width over Ghidra's guess
 * (cookbook: "Ghidra can mistype a stack-passed parameter's width").
 * effect.h's BloodType gained a real `unk22` field here: offset 0x22 was
 * previously unnamed alignment padding, but this function's `sb` store
 * there is a genuine write, not an artifact.
 *
 * A weight-free empty one-shot loop immediately after the scale assignment
 * fixes the old scheduler island: cc1 now loads the stack argument, preserves
 * the target load-delay `nop`, stores scale, and restores the exact extent.
 * The remaining three aligned lines are only the rotate producer: retail
 * loads it immediately after scale and keeps it in $v1 until the return delay
 * slot; this draft sinks that same load to just before the final stores.
 * Empty and weighted producer boundaries were flat. An erased identical-arm
 * use and a redundant early rotate store loaded it at function entry and
 * disturbed the whole allocation, so they were rejected. Keep the guarded
 * 16-byte checkpoint; revisit the rotate lifetime only after the requested
 * untouched/zero-percent target pool.
 */
extern void DrawImpact(TEffectSlot *ef);

#ifndef NON_MATCHING
INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/FUN_8003944c", FUN_8003944c);
#else
void FUN_8003944c(long *param_1, long param_2, short param_3, short param_4,
                   long param_5, long param_6, u16 param_7, u16 param_8,
                   u16 param_9, u16 param_10)
{
    int idx;
    TEffectSlot *base;
    TEffectSlot *slot;
    int count;
    TEffectSlot *ef;
    BloodType *pp;
    long py;
    long rot;

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
    ef->proc = (void (*)())DrawImpact;
    ef->param.blood.hint = (struct AreaNodeType *)param_1[0];
    pp = &ef->param.blood;
    pp->px = param_1[1];
    py = param_1[2];
    pp->pz = param_2;
    pp->vy = param_7;
    pp->vz = param_8;
    pp->scale = param_5;
    do {
    } while (0);
    rot = param_6;
    pp->vx = param_4;
    pp->time = param_3;
    pp->unk22 = (u8)param_9;
    pp->bright = 0;
    pp->mode = (u8)param_10;
    pp->py = py;
    pp->rotate = rot;
}
#endif
