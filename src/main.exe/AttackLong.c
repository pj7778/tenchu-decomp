#include "common.h"
#include <psxsdk/libgs.h>
#include "game_types.h"
#include "item.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * static short AttackLong(void);
 *     THINK_3.C:450, 71 src lines, frame 24 bytes, saved-reg mask 0x80010000 (DEMO build -- see below)
 *
 * Original parameters and locals (the demo build's register allocation may
 * differ from retail, but the COUNT and TYPES drive cc1's codegen and carry
 * over). A repeated name is a nested-block scope, not a duplicate.
 * A ZERO-locals record is unverified, not a claim that the function has none:
 * vfree lists zero locals yet its byte-matched source needs seven.
 * The frame size and saved-reg mask above are the DEMO's: retail often needs
 * FEWER callee-saved registers (measured: Think1random exact; Think1chase's
 * 0x800f0000 = s0-s3+ra vs retail's s0,s1,ra). Treat them as an upper bound
 * and a hint at how many values stay live, never as a spec. The asm wins.
 * Locals:
 *     reg   $a0       short ad
 *     reg   $s0       short pad
 *     reg   $s0       short pad
 *
 * Globals it touches, as the original declared them:
 *     extern struct BattleType BattleDB[78];
 *     extern long Distance;
 *     extern short Degree;
 *     extern short EngageLevel;
 *     extern short Attrib;
 *     extern long GameClock;
 *     extern long AttackActionCount;
 * END PSX.SYM */

/*
 * STATUS: NON_MATCHING -- 3 of 1248 bytes differ, with the exact target
 * length (312 instructions).  The residual is one commutative addition's
 * destination register in the AttackActionCount update:
 *
 *     target: addu a0,a0,v1; sw a0,AttackActionCount
 *     draft:  addu v1,v1,a0; sw v1,AttackActionCount
 *
 * Here a0 is GameClock and v1 is EngageLevel*10.  `rtldump.py` confirms the
 * difference is fixed at combine/local allocation after the expression has
 * become a plain commutative PLUS.  Both operand spellings, explicit temps,
 * compound-assignment shapes, and a bounded 2,500-candidate permuter run
 * were checked; all either preserve these three bytes or regress scheduling
 * and register allocation.  Keep the semantically correct draft rather than
 * introducing register-forcing assembly.
 *
 * Two reusable structural facts closed the rest of the function:
 *  - `(raw >= 0) ? raw : -raw` reaches GCC's abssi2 expansion and emits the
 *    target's copy-then-self-negu form.  The LT ternary does not.
 *  - Spelling the two random choices as a nested positive arm makes ItemUse
 *    the cold final arm and lets jump2 merge the four SetCommand calls into
 *    the target's single call tail.
 */

extern Humanoid *Me_THINK_C;
extern BattleType BattleDB[78];
extern s32 Distance;
extern s16 Degree;
extern s16 EngageLevel;
extern u16 Attrib;
extern s32 GameClock[];
extern s32 AttackActionCount;

extern s16 ChasetoTarget(s32 distance);
extern s16 SetCommand(PADtype *pad, s16 command);
extern s16 ItemUse(void);

#ifndef NON_MATCHING
INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/AttackLong", AttackLong);
#else /* NON_MATCHING */

short AttackLong(void)
{
    s16 ad;
    s16 pad;
    s16 status7_result;
    s32 degree;

    pad = 0;
    if (Me_THINK_C->status == 7)
    {
        s32 status_degree;

        do
        {
            if (Me_THINK_C->motion->count !=
                BattleDB[Me_THINK_C->warid].contfrm)
            {
                status7_result = 0;
                goto status7_return;
            }
            if (Distance < 3000)
            {
                status_degree = Degree;
                if (status_degree < 0)
                {
                    status_degree = -status_degree;
                }
                if (status_degree < 1500)
                {
                    goto choose_status7;
                }
            }
            if (rand() % (EngageLevel + 1) != 0)
            {
                status7_result = pad;
                goto status7_return;
            }
        } while (0);

choose_status7:
        if (Degree >= 301)
        {
            pad = 0x2000;
        }
        else
        {
            pad |= 0x80;
            if (Degree < -300)
            {
                pad = -0x8000;
            }
            else
            {
                goto status7_value;
            }
        }
        pad |= 0x80;

status7_value:
        status7_result = pad;
status7_return:
        return status7_result;
    }

    if (Me_THINK_C->status == 9)
    {
        return 0;
    }

    if (Me_THINK_C->motion->mid == 0x500)
    {
        return (u16)(rand() % (EngageLevel + 1) != 0) << 14;
    }

    if (Me_THINK_C->actmode == 0)
    {
        pad = ChasetoTarget(3000);
        if (pad == 0 || (Attrib & 0x4000) != 0)
        {
            Me_THINK_C->actmode = 1;
        }
        if (Me_THINK_C->motion->count != 0)
        {
            goto return_pad;
        }
        if (rand() % 5 != 0)
        {
            goto return_pad;
        }
        pad = 0x1040;
        goto return_pad;
    }

    if ((Me_THINK_C->motion->count & 0xf) != 0)
    {
        s32 motion_degree;

        pad = Me_THINK_C->pad.data;
        if (Distance >= 3000)
        {
            goto return_pad;
        }
        ad = Degree;
        motion_degree = (ad >= 0) ? ad : -ad;
        if (motion_degree < 500)
        {
            pad = 0x4000;
            goto return_pad;
        }
        if (motion_degree < 1501)
        {
            goto return_pad;
        }
        pad = 0x1000;
        goto return_pad;
    }

    if (Distance >= 5001)
    {
        Humanoid *me;

        Me_THINK_C->actmode = 0;
        me = Me_THINK_C;
        Me_THINK_C->chase[1] = 0;
        me->chase[0] = 0;
        ItemUse();
        return 0;
    }

    if ((Attrib & 0x400) != 0)
    {
        Me_THINK_C->actmode = 0;
    }

    if (Degree >= 301)
    {
        pad = 0x2000;
    }
    else if (Degree < -300)
    {
        pad = -0x8000;
    }

    if ((u32)(Distance - 3001) < 999)
    {
        if (rand() % (EngageLevel + 1) == 0)
        {
            AttackActionCount = EngageLevel * 10 + GameClock[0];
            return pad | 0x80;
        }
    }

    ad = Degree;
    degree = (ad >= 0) ? ad : -ad;

    if (degree > 1000 || Distance < 3000)
    {
        if (Distance < 1000)
        {
            pad = 0xa0;
            if ((rand() & 1) != 0)
            {
                pad = 0x4040;
            }
        }
        else
        {
            pad = 0x2080;
            if ((rand() & 1) != 0)
            {
                pad = -0x7f80;
            }
        }
        goto return_pad;
    }

    if (Distance < 4001)
    {
        goto return_pad;
    }

    if (degree < 50)
    {
        pad = SetCommand(&Me_THINK_C->pad, 0x21);
        goto return_pad;
    }
    else
    {
        if (Me_THINK_C->motion->count != 0)
        {
            pad |= 0x1000;
            goto return_pad;
        }
        if (ad > 50)
        {
            pad = SetCommand(&Me_THINK_C->pad, 4);
            goto return_pad;
        }
        if (ad < -50)
        {
            pad = SetCommand(&Me_THINK_C->pad, 3);
            goto return_pad;
        }
        if ((rand() & 1) != 0)
        {
            if ((rand() & 1) != 0)
            {
                pad = SetCommand(&Me_THINK_C->pad, 1);
                goto return_pad;
            }
            pad = 0xc0;
        }
        else
        {
            ItemUse();
        }
        goto return_pad;
    }

return_pad:
    return pad;
}

#endif /* NON_MATCHING */
