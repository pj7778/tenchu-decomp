#include "common.h"
#include "main.exe.h"

/*
 * AdtSelect (0x8005fecc, 776 bytes) — modal debug-menu selection widget:
 * waits for pad release, saves the display state into a 0x8090-byte frame
 * buffer, then draws the choice list (18 per page) and moves the cursor on
 * edge-detected pad input until confirm (pad & 0x820 -> current entry) or
 * cancel (pad & 0x40 -> last entry); returns the entry's choice_number.
 *
 * STATUS: NON_MATCHING — 9 of 776 bytes differ.  Build the draft with
 * `NON_MATCHING=AdtSelect ./Build` (or tools/matchdiff.py, which sets it
 * automatically); the default build keeps the INCLUDE_ASM stub below.
 *
 * The 9 differing bytes are ONE reload decision, in the entry-count block:
 *
 *   target:  lw a3,0(a3); lw v0,0(a3);  ...  li t0,0x80CC; addu; lw v1,0(t0)
 *   ours:    lw t0,0(a3); lw v0,0(t0);  ...  li a3,0x80CC; addu; lw v1,0(a3)
 *
 * Both are the SAME 4-insn shape (`li`/`addu` materialise sp+0x80CC, load
 * menu, deref).  Only the register holding menu's value differs: the target
 * reuses the address register a3, we take t0.  Site 2's t0<->a3 is NOT an
 * independent difference — see the round-robin note below.
 *
 * ---- WHY (verified line-by-line against the nix-pinned gcc-2.8.1 sources) ----
 *
 * For `name = menu->choice_name` the insn is `(set (reg 93) (mem (reg 81)))`
 * with reg 81 = menu spilled (reg_equiv_address = sp+32972; 32972 > 32767 so
 * the address must be materialised).  Reload does:
 *
 *   reload.c 2552   a MEM operand goes STRAIGHT to find_reloads_address — it
 *                   never reaches find_reloads_toplev.
 *   reload.c 4296   reg_equiv_address branch: recurse on the sp+32972 PLUS
 *                   (RELOAD_FOR_INPADDR_ADDRESS via ADDR_TYPE), THEN
 *                   push_reload menu's value (RELOAD_FOR_INPUT_ADDRESS).
 *   reload.c 3812   *** the step round 1 missed ***  `(mem (reg a3))` matches
 *                   movsi's 'm' constraint, so the OPERAND ITSELF is never
 *                   reloaded (operand_reloadnum[1] < 0).  Both address reloads
 *                   are therefore RETYPED to RELOAD_FOR_OPERAND_ADDRESS
 *                   (reload.c 3854).
 *   reload1.c 4623  RELOAD_FOR_OPERAND_ADDRESS returns 0 for any reg in
 *                   reload_reg_used_in_op_addr — i.e. the second one may not
 *                   reuse the first one's register.  ==> t0.
 *   reload1.c 4345  reload_reg_class_lower's last tiebreak is `r1 - r2`
 *                   (push order), and both reloads share class/nregs, so the
 *                   materialisation is ALWAYS allocated first and always wins
 *                   a3.  The order is not perturbable.
 *
 * This is unconditional for the deref shape, so NO respelling of
 * `menu->choice_name` can self-tie it.
 *
 * The escape hatch in reload.c 3812 is `operand_reloadnum[opnum] >= 0` — the
 * operand must ITSELF be reloaded, which happens only when the operand needs a
 * REGISTER rather than accepting 'm'.  That is the real discriminator, and it
 * explains all four menu sites:
 *   - `if (title == 0)` and the print/tail `menu[i]` / `menu[selection]`:
 *     bare operands of a branch/addu, which require a register.  The operand
 *     is reloaded (RELOAD_FOR_INPUT), so the address reload KEEPS
 *     RELOAD_FOR_INPUT_ADDRESS, and RELOAD_FOR_INPUT (reload1.c 4560) only
 *     scans i > opnum — so it self-ties.  `lw a3,0(a3)`.
 *   - `p = menu`: movsi's 'm' absorbs the MEM, so there is NO value reload at
 *     all — `lw v1,0(t0)` loads straight into p's home.  (This site does NOT
 *     "self-tie"; the old note grouped it with the two above by mistake.)
 *   - `menu->choice_name`: 'm' absorbs it too, but here the MEM is an ADDRESS,
 *     so the retype fires and the second reload is barred.  ==> the residual.
 *
 * ---- WHAT ROUND 2 DISPROVED (do not re-derive) ----
 *
 * Round 1's "open question" — *a copy that is single-use yet outlives combine*
 * — is a DEAD END even if solved.  A surviving copy `X = menu` puts menu's
 * value in X's ALLOCATED register, and reload's spill registers (a3, t0) are
 * excluded from allocation: the .greg dispositions here only ever use
 * $v0/$v1/$a0/$s0-$s7/$fp/HI.  So a copy can only ever emit
 * `lw v0/v1,0(a3); lw v0,0(v0/v1)` — never the target's `lw a3,0(a3)`.
 * Measured directly (tools/rtldump.py --src): the multi-use copy still emits
 * `lw $8,0($7)` at site 1 with p in $3.  Combine also copy-propagates
 * `p = menu` into the deref even when p is multi-use, so the catch-22 the old
 * note described is not even reachable.  Chasing a combine fence here is wasted
 * effort: the copy is the wrong lever, not a blocked one.
 *
 * Also disproved: adding any bare-operand use of `menu` to force the self-tie
 * (e.g. `if (menu == 0) return -1;`) raises menu's ref count enough that it is
 * no longer spilled at all — it lands in $fp and the whole shape changes.
 *
 * ---- WHY IT IS ONE DECISION, NOT TWO ----
 *
 * reload1.c 5082-5091: allocate_reload_reg starts its scan at
 * `last_spill_reg + 1` and wraps `% n_spills`, setting last_spill_reg on each
 * success (5185) — a ROUND ROBIN.  The function's reload regs therefore run
 * a3, t0, a3, t0, ...  Site 1 consuming ONE reload reg (target) instead of TWO
 * (ours) shifts every later reload by one position, which is exactly why site 2
 * flips t0<->a3.  Fixing site 1 would fix site 2 for free; there is no separate
 * site-2 bug.
 *
 * ---- CLOSED ----
 *
 * autorules (23 candidates, no improving edit; both do{}while(0) fences are
 * load-bearing, unwrapping either costs +16); a bounded decomp-permuter run
 * (flat at 9, base best); reghist (194/194 insns, delta sum 0 — no
 * decomposition lever).  The demo build's AdtSelect has a DIFFERENT frame
 * (0x80E0/0x80E4) yet emits the same a3/t0 split, so the shape is a stable
 * source property, not a frame-size artefact.  All other allocation (9
 * callee-saved pseudos + 2 spilled params) matches.
 *
 * WHAT WOULD ACTUALLY CLOSE IT: the target's site-1 operand must be one that
 * REQUIRES A REGISTER (so reload.c 3812's retype does not fire).  Every C
 * spelling of the deref tried so far accepts 'm'.  A round 3 should attack
 * that single question and ignore combine entirely.
 */

#ifndef NON_MATCHING
INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/AdtSelect", AdtSelect);

#else /* NON_MATCHING */

extern s32 (*AdtPadRead)(s32);
extern void AdtGetDisp(u8 *buf);
extern void AdtReleaseDisp(u8 *buf);
extern void DrawPrim(u8 *prim);
extern void FntPrint(char *fmt, ...);
extern s32 FntFlush(s32 id);
extern s32 VSync(s32 mode);

extern char D_80014AFC[]; /* "select item" */
extern char D_80014B08[]; /* " (%d/%d)" */
extern char D_80097E9C[]; /* "%s" */
extern char D_80097EA0[]; /* "\n\n" */
extern char D_80097EA4[]; /* "->" */
extern char D_80097EA8[]; /* "  " */
extern char D_80097EAC[]; /* "\n" */

s32 AdtSelect(char *title, debug_menu_choice *menu, s32 selection)
{
    u8 buf[0x8090];
    s32 last;
    u32 trg;
    debug_menu_choice *p;
    s32 count;
    s32 pages;
    s32 page;
    s32 first;
    u32 pad;
    short i;
    char *fmt;
    char *name;

    do
    {
    } while (AdtPadRead(0) != 0);

    if (title == 0)
        title = D_80014AFC;

    count = 0;
    name = menu->choice_name;
    if (name != 0)
    {
        p = menu;
        do
        {
            p++;
            count++;
        } while (p->choice_name != 0);
    }

    pages = count / 0x12 + 1;
    AdtGetDisp(buf);

    for (;;)
    {
        DrawPrim(buf + 0x8078);
        trg = pad;
        pad = AdtPadRead(0);
        trg = ~trg & pad;
        page = selection / 0x12;
        first = page * 0x12;
        last = first + 0x12;
        if (count < last)
            last = count;
        FntPrint(D_80097E9C, title);
        if (pages > 1)
            FntPrint(D_80014B08, page + 1, pages);
        FntPrint(D_80097EA0);
        i = first;
        do
        {
            /* do{}while(0) wrappers: flow.c loop-depth ref weighting;
               see the matching notes above and docs/matching-cookbook.md */
            for (; i < last; i++)
            {
                if (selection == i)
                    fmt = D_80097EA4;
                else
                    fmt = D_80097EA8;
                FntPrint(fmt);
                FntPrint(menu[i].choice_name);
                FntPrint(D_80097EAC);
            }
        } while (0);
        FntFlush(-1);
        VSync(3);
        if (pad & 0x820)
            break;
        if (pad & 0x40)
        {
            selection = count - 1;
            break;
        }
        i = -1;
        if (!(trg & 0x1000))
        {
            i = 1;
            if (!(trg & 0x4000))
            {
                i = -0x12;
                if (!(trg & 0x8000))
                {
                    i = 0;
                    do
                    {
                        if (trg & 0x2000)
                            i = 0x12;
                    } while (0);
                }
            }
        }
        selection += i;
        if (selection < 0)
            selection = 0;
        else if (count <= selection)
            selection = count - 1;
    }
    AdtReleaseDisp(buf);
    return menu[selection].choice_number;
}

#endif /* NON_MATCHING */
