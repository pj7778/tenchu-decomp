#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 *
 * Globals it touches, as the original declared them:
 *     extern long GameClock;
 * END PSX.SYM */

/*
 * STATUS: NON_MATCHING — 29 of 412 bytes differ; RIGHT LENGTH (103
 * instructions both sides, confirmed by asmdiff: no inserts/deletes, only
 * register-name/operand-order replacements in 3 small clusters). Both
 * clusters are the same permuter-immune family this cookbook already
 * documents: which of two independent, data-flow-equivalent loads the
 * scheduler issues first when nothing else is ready to fill the delay
 * slot (verified: swapping declaration order of the two operands in
 * source has NO effect on the compiled order — tried both ways).
 *  - Reset body: the `tmp.max` load (`this+0x20`) and its store schedule
 *    with an unfilled `nop`, while `GameClock`'s load/store interleave
 *    tightly with `tmp.sndIdx`'s — a scheduler tie over which independent
 *    load fills which delay slot, not a source-order question (confirmed:
 *    the field-assignment order in the C already matches the target's
 *    dataflow; only the *scheduled* instruction order differs).
 *  - Reschedule guard: `sched->max`/`sched->min` load in the opposite
 *    order from the target's `max`-then-`min` in BOTH the initial compare
 *    and the post-`rand()` reload, regardless of which local (`hi`/`lo`)
 *    is declared/assigned first.
 *  - Final store: the shared-tail `sched->next = GameClock + lo;` (written
 *    once per if/else arm so cc1 keeps two separate `GameClock` reloads,
 *    matching the target) addresses through `this+0x18` (`$s0`) instead of
 *    through the already-live `sched` pointer (`$s1`) — a coloring/
 *    rematerialization choice, not a spelling issue (`sched->next = ...`
 *    is exactly how it's written).
 * `tools/permute.py` ran one bounded ~280s run (`--stop-on-zero -j4`): best
 * scores found were 375/380/440 (its own weighted metric, not raw bytes),
 * never 0, and never monotonically improving — consistent with the
 * cookbook's "same-length residual a bounded run doesn't close is a PARK
 * signal". Parked per the sub-C-level early-stop.
 *
 * FUN_8004c59c (0x8004c59c, 0x19C bytes) — periodic-sound-emitter think
 * function (message-style: called with `(this, msg)`, no direct `jal`
 * callers found — reached through a function-pointer table like
 * ProcXXX/ReqXXX). Shares its object layout with the neighbouring
 * FUN_8004c350 (same TU: x/y/z position at 0x4/0x8/0xC, a "played" byte flag
 * at 0x15, GameClock-compared reschedule word at 0x18) — msg==0 resets,
 * msg<4 is otherwise ignored, msg>=4 fires once `played==0` and
 * `GameClock>=sched.next`, then reschedules.
 *
 * `sched` (`this+0x18`) is a 3-word (0xC byte) sub-struct: `next` (s32,
 * GameClock deadline), `min`/`max` (s16, the delay range for the next
 * reschedule) and `sndIdx` (u8, added to 0x44 as the SoundEx sound id).
 *
 * Matching notes:
 *  - The msg==0 reset is NOT a simple field clear — it's a whole-struct
 *    assignment `*sched = tmp;` through a freshly-built local `Schedule
 *    tmp`, which is why cc1 emits 3 WORD lw/sw pairs through the stack
 *    (emit_block_move on a 3-word-aligned struct) instead of narrow
 *    per-field stores (cookbook: "the cast type's alignment drives copy
 *    code"). `tmp`'s own fields are read from DIFFERENT offsets than
 *    where they end up: `tmp.max` is read from `this+0x20` (sched's own
 *    `sndIdx` position, reinterpreted as u16 — reading 1 real byte plus 1
 *    pad byte) and `tmp.sndIdx` is read from `this+0x18` (sched's own
 *    `next`, truncated to its low byte) — the reset intentionally recycles
 *    the old `next`/`sndIdx` bytes into the new `sndIdx`/`max` slots.
 *    `tmp.min` is a plain same-value copy-through (`this+0x1C`), and
 *    `tmp.next` is fresh `GameClock`. The two copy-through reads
 *    (`tmp.min`/`tmp.max`) load `lhu` regardless of `min`'s true `s16`-ness
 *    (cookbook: "a pure narrowing struct-field copy uses lhu/lbu even for
 *    signed fields") — only the explicit `u16 *` cast reproduces that.
 *  - The three early-return guards (msg<4, played!=0, GameClock<sched.next)
 *    are flat guard clauses (IsVisible's shape), not one nested `&&`-chain
 *    — each is its own conditional `return`.
 *  - The position is copied through TWO separate VECTOR locals (a memset
 *    scratch, then a whole-struct-copied second local whose address is
 *    what's actually passed to SoundEx) — same "two same-type address-
 *    taken stack objects, by REFERENCE order" shape as leLayoutEnemy.
 *  - `sched->min`/`sched->max` in the reschedule arithmetic are read via
 *    the persistent `Schedule *sched` pointer (computed once, before the
 *    reset-vs-else branch, and used by the msg>=4 body) — the reset branch
 *    instead addresses `this` directly with raw offset casts, since it
 *    never uses `sched` as a variable.
 *  - Needs maspsx's --expand-div (rand() % (max-min) divides by a runtime
 *    value); no explicit trap() calls belong in the C.
 *
 * Session addendum (re-verified the "permuter-immune" verdict against the
 * cookbook's named classes first, then escalated to `tools/rtldump.py
 * --draft --pass all` for the two clusters with the most bytes, per the
 * escalation protocol, before re-confirming this park):
 *  - Reschedule guard order (cluster 2, `sched->max`/`sched->min`):
 *    RE-TESTED the declared-order swap (`hi = sched->max; lo = sched->min;`
 *    instead of `lo = ...; hi = ...;`) in isolation — byte-for-byte
 *    IDENTICAL output, confirming the header's claim precisely. Also
 *    noticed the header's "both the initial compare and the post-rand()
 *    reload" is now stale: only the FIRST occurrence (0x8004c6a4) still
 *    differs from target; the second (0x8004c6c4, inside `rand() %
 *    (sched->max - sched->min)`) already matches (max read before min,
 *    matching the subexpression's own textual order) — a prior session's
 *    partial fix that the header text wasn't updated for.
 *  - Final store address (cluster 3, `sched->next = GameClock + lo;`,
 *    0x8004c720): DUMP-CONFIRMED root cause, not just "coloring". `.greg`'s
 *    register-disposition list shows pseudo 82 (`sched`) correctly
 *    allocated to `$s1` (`82 in 17`) — allocation is NOT the problem. The
 *    RAW `.rtl` dump (pre-optimization) shows both `sched->next` stores as
 *    `(set (mem (reg 82)) ...)` — a bare zero-offset dereference, exactly
 *    as written. By the `.cse`/`.cse2`/`.loop`/`.lreg` dumps (all AFTER
 *    cse1), both stores have ALREADY been rewritten to `(set (mem (plus
 *    (reg 80) 24)) ...)` — `reg 80` is `this` (`arg0`), and `sched`'s own
 *    defining insn is literally `reg82 = reg80 + 24` (`sched = (Schedule
 *    *)(arg0 + 0x18)`). cse1 canonicalizes the OFFSET-ZERO dereference
 *    `MEM(reg82)` back to its defining expression `MEM(reg80+24)` — but
 *    does NOT do this for the NONZERO-offset dereferences of the same
 *    pointer (`sched->min`/`max`/`sndIdx` at +4/+6/+8 all correctly keep
 *    `reg82` unmodified, confirmed in the same dumps). This is the
 *    "offset-0 alias vs enclosing-struct-member" cookbook lever's sibling
 *    mechanism for a LOCAL pointer instead of a global `%hi` symbol: a
 *    bare-register MEM address (offset 0) is available for cse's
 *    value-substitution in a way a `(plus reg N)` MEM address is not — the
 *    substitution costs the SAME instruction count either way (a MIPS `sw`
 *    embeds a 16-bit offset for free), so nothing downstream can tell cse
 *    to prefer the register. Tried and REJECTED: `sched = sched;`
 *    (self-assignment; cc1 elides it as dead, no RTL effect, confirmed via
 *    rebuild). NOT yet tried: restructuring `sched`'s own definition so its
 *    defining insn is NOT a simple `reg+const` cse can fold back to (e.g. a
 *    genuine function PARAMETER `Schedule *sched` instead of a locally
 *    derived pointer) — would require re-deriving the ORIGINAL function's
 *    true signature/call sites first (no direct `jal` callers found per
 *    the header), so not attempted this session.
 *  - Reset body (cluster 1, the bulk of the residual): `.sched`'s "basic
 *    block number 3" dump for the `emit_block_move`-generated struct copy
 *    shows an 11-insn dependency DAG (insns 33-61) with TWO explicit
 *    scheduler STALLS reported (`launching 48 before 50 with 1 stalls`,
 *    `launching 33 before 35 with 1 stalls`) — genuine list-scheduling
 *    complexity, not a simple 2-candidate tie. Consistent with the header's
 *    existing verdict; not pursued further (out of proportion to the ~20
 *    of 29 residual bytes it accounts for, given the confirmed-unfixable
 *    precedent from cluster 2's identical-order-swap-no-effect test).
 *  - `tools/autorules.py` re-run: no improving edit found (5 candidates,
 *    all worse or exactly tied) — confirms no false wins remain.
 * Left at the documented 29-byte baseline; no source changes this session.
 */

typedef struct
{
    s32 next;  /* 0x00 (this+0x18) */
    s16 min;   /* 0x04 (this+0x1C) */
    s16 max;   /* 0x06 (this+0x1E) */
    u8 sndIdx; /* 0x08 (this+0x20) */
} Schedule;

extern s32 GameClock;
extern s16 SoundEx(VECTOR *loc, s32 id);
extern s32 rand(void);
extern void *memset(void *dst, s32 c, u32 n);

#ifndef NON_MATCHING
INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/FUN_8004c59c", FUN_8004c59c);
#else

void FUN_8004c59c(u8 *arg0, u32 arg1)
{
    Schedule *sched;
    Schedule tmp;
    VECTOR pos;
    VECTOR snd;
    s16 hi;
    s32 lo;

    sched = (Schedule *)(arg0 + 0x18);

    if (arg1 == 0) goto reset;
    if (3 < arg1) goto normal;
    return;

reset:
    tmp.min = *(u16 *)(arg0 + 0x1C);
    tmp.max = *(u16 *)(arg0 + 0x20);
    tmp.next = GameClock;
    tmp.sndIdx = *(u8 *)(arg0 + 0x18);
    *sched = tmp;
    *(arg0 + 0x15) = 0;
    return;

normal:
    if (*(arg0 + 0x15) != 0) return;
    if (sched->next > GameClock) return;

    memset(&snd, 0, sizeof(snd));
    snd.vx = *(s32 *)(arg0 + 4);
    snd.vy = *(s32 *)(arg0 + 8);
    snd.vz = *(s32 *)(arg0 + 0xC);
    pos = snd;
    SoundEx(&pos, sched->sndIdx + 0x44);

    lo = sched->min;
    hi = sched->max;
    if (hi - lo > 0) {
        lo = rand() % (sched->max - sched->min) + sched->min;
        sched->next = GameClock + lo;
    } else {
        sched->next = GameClock + lo;
    }
}
#endif
