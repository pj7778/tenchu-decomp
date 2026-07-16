#include "common.h"
#include "main.exe.h"
#include "gte.h"

/*
 * drawF3 (0x8005d0d4, 0x128 bytes) — the flat-triangle (POLY_F3) primitive
 * renderer of the DrawTMD handler family, and the anchor for the restricted
 * gte.h inline-asm policy (docs/gte-policy.md).  The carved .s marks it a
 * "Handwritten function": it was authored in assembly, not compiled from C.
 *
 * STATUS: NON_MATCHING — 8 of 296 bytes differ (72 of 74 instructions exact).
 * The two residuals are cc1-2.8.1 codegen INVARIANTS that the hand-written
 * original does not follow; no C construct, register pinning, statement order,
 * or compiler flag reproduces them (verified against cc1-281 directly):
 *
 *   1. 0x8005d0d8  `beq $v1,$v0` (const first) vs cc1's `beq $v0,$v1`.
 *      cc1 canonicalises a two-register equality to put the lower-regno
 *      operand ($v0 = $2, the incoming count) first.  The count is pinned to
 *      $2 by the non-ABI entry, and 0x304 cannot occupy a register below $2,
 *      so cc1 can only emit $v0 first.  Tried: `!=` / `==` both orders, a
 *      ternary, and a named-sentinel compare — all identical.
 *   2. 0x8005d14c/0x8005d150  the `code = 0` reset.  The hand author wrote
 *      `addiu $s0,$zero,0` (li) placed AFTER `cfc2`; cc1 emits `addu $s0,$0,$0`
 *      (the `move reg,$0` form its movsi ALWAYS uses for an integer set-to-0 —
 *      confirmed for pseudos AND register variables; `= 0x20` correctly gives
 *      `li`).  It must also sit BEFORE `cfc2` so the volatile barrier stops
 *      reorg from sinking it into the flag-test branch delay slot (which the
 *      target leaves a `nop`); placing it after cfc2 loses that nop and the
 *      length.  So both the opcode (addu vs addiu) and the cfc2/reset order are
 *      forced by cc1.
 *
 * Everything else is exact, including all COP2 data moves, the three GTE
 * commands, the deferred ordering-table insertion, and every guard delay slot.
 * The default build keeps the byte-identical INCLUDE_ASM stub; the #else draft
 * is the reference reconstruction (build with `NON_MATCHING=drawF3 ./Build`).
 *
 * Non-ABI entry — the DrawTMD dispatcher hands the handler its working state in
 * live registers (no stack frame, no callee-save):
 *   $v0 ($2)  batch primitive count (0x304 sentinel => 1)
 *   $t0 ($8)  deferred ordering-table slot from the previous primitive
 *   $t2 ($10) ordering-table base
 *   $t3 ($11) output packet cursor (POLY_F3, 0x14 bytes each)
 *   $t4 ($12) transformed-vertex (SVECTOR) base
 *   $t5 ($13) input primitive cursor (0x10 bytes each)
 *   $t6 ($14) running item budget, decremented by this batch's count
 *   $t9 ($25) ordering-table length (depth clamp)
 *   $s0 ($16) pending-primitive code byte carried across iterations (0 = none)
 */

#ifndef NON_MATCHING
INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/drawF3", drawF3);
#else

typedef struct
{
    u8     mode[4]; /* +0x0 */
    u_long rgb;     /* +0x4 (r,g,b,code) */
    u16    v0;      /* +0x8 vertex index 0 */
    u16    v1;      /* +0xA vertex index 1 */
    u16    v2;      /* +0xC vertex index 2 */
    u16    norm;    /* +0xE */
} F3Prim;

/* $s0 is the only callee-saved register the handler clobbers.  A file-scope
 * global register variable reserves it WITHOUT a prologue save/restore — the
 * DrawTMD dispatcher owns the save, exactly the non-ABI handler contract.
 * $t6 (budget) and $t3 (packet) are handed back to the dispatcher updated, so
 * their final writes must not be dead-eliminated / sunk into the return delay
 * slot: global register variables keep the stores in place. */
register int code   __asm__("$16");
register int budget __asm__("$14");
register u_long *packet __asm__("$11");

void drawF3(void)
{
    register int     r_v0    __asm__("$2");
    register u_long *ot_in   __asm__("$8");
    register int     ot_base __asm__("$10");
    register int     vbase   __asm__("$12");
    register F3Prim *prim    __asm__("$13");
    register int     ot_max  __asm__("$25");

    register SVECTOR *r0 __asm__("$4");
    register SVECTOR *r1 __asm__("$5");
    register SVECTOR *r2 __asm__("$6");
    register int      n    __asm__("$15");
    register int      half __asm__("$9");
    register int      mask __asm__("$3");
    u_long *ot_slot;
    int flag;
    int mac0;

    ot_slot = ot_in;
    n = 1;
    if (r_v0 != 0x304)
    {
        n = r_v0;
    }
    budget = budget - n;
loop:
    {
        r0 = (SVECTOR *)(prim->v0 * 8);
        r1 = (SVECTOR *)(prim->v1 * 8);
        r2 = (SVECTOR *)(prim->v2 * 8);
        r0 = (SVECTOR *)((int)r0 + vbase);
        r1 = (SVECTOR *)((int)r1 + vbase);
        r2 = (SVECTOR *)((int)r2 + vbase);
        gte_ldv3(r0, r1, r2);
        mask = 0x1C66000;
        gte_rtpt();
        if (code != 0)
        {
            *ot_slot = (u_long)packet;
            *((u8 *)ot_slot + 3) = 0;
            gte_strgb(packet[1]);
            *((u8 *)packet + 7) = (u8)code;
            packet += 5;
        }
        code = 0;
        gte_stflg(flag);
        if ((flag & mask) == 0)
        {
            gte_nclip();
            gte_stsz3(r0, r1, r2);
            ot_slot = (u_long *)((int)r0 + (int)r1);
            ot_slot = (u_long *)((int)ot_slot + (int)r2);
            half = (u_long)ot_slot >> 2;
            ot_slot = (u_long *)((u_long)ot_slot >> 4);
            gte_stmac0(mac0);
            ot_slot = (u_long *)((u_long)ot_slot + half);
            ot_slot = (u_long *)((u_long)ot_slot >> 4);
            if (mac0 > 0)
            {
                half = (int)ot_slot - ot_max;
                if ((int)ot_slot > 0)
                {
                    ot_slot = (u_long *)((int)ot_slot * 4);
                    if (half < 0)
                    {
                        gte_ldrgb(prim->rgb);
                        ot_slot = (u_long *)((int)ot_slot + ot_base);
                        mac0 = *ot_slot;
                        gte_dpcs();
                        *packet = mac0;
                        mask = 4;
                        *((u8 *)packet + 3) = mask;
                        gte_stsxy3_f3(packet);
                        code = 0x20;
                    }
                }
            }
        }
        n = n - 1;
        prim += 1;
    }
    if (n > 0)
    {
        goto loop;
    }
    if (code != 0)
    {
        *ot_slot = (u_long)packet;
        *((u8 *)ot_slot + 3) = 0;
        gte_strgb(packet[1]);
        *((u8 *)packet + 7) = (u8)code;
        packet += 5;
    }
}

#endif
