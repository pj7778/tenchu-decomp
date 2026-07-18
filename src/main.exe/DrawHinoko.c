#include "common.h"
#include "main.exe.h"
#include "effect.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * static void DrawHinoko(struct tag_EffectSlot *ef);
 *     EFFECT.C:1258, 61 src lines, frame 32 bytes, saved-reg mask 0x80010000 (DEMO build -- see below)
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
 *     param $a1       struct tag_EffectSlot * ef
 *     reg   $a2       struct ExplosionType * param
 *     reg   $s0       struct Sprite3D * spr
 *     reg   $a3       unsigned char alfa
 *
 * Globals it touches, as the original declared them:
 *     extern struct Sprite3D *sprBomb[3];
 * END PSX.SYM */

/*
 * STATUS: NON_MATCHING — 18 of 308 bytes differ. Length is EXACT (77 of 77
 * instructions) — the previous 1-instruction-short bug (76/77, LENGTH
 * MISMATCH) is FIXED; see "Length fix" below. The residual is a pure
 * register tie confined to one basic block; see "Residual" below.
 *
 * ROUND 2026-07-18: fixed-permuter re-screened (timeout 300, -j4). Authoritative
 * best is again output-25-1 at 4 — the SAME `new_var` second-pointer-identity
 * scaffold already vetoed below (target's join block has NO third pointer
 * register; only a2/s0). scaffold-rejected; floor at 18 stands.
 *
 * DrawHinoko (0x800368b0, 0x134 bytes) — bomb/explosion sprite proc:
 * bumps the explosion state machine (mode 0 -> 1 with a 30-tick timer;
 * mode 1 fades an alpha value from the countdown and self-disposes at 0),
 * then integrates the SVECTOR velocity into the VECTOR position every
 * frame, applies it to sprBomb[2], and draws it.
 *
 * Matching notes:
 *  - `(ef->param)` here is `struct ExplosionType`, NOT `BloodType` — a
 *    DIFFERENT field layout occupying the same union bytes (effect.h's new
 *    `explosion` member): `vec`(SVECTOR)@0x0, `pos`(VECTOR)@0x8,
 *    `rotate`@0x18, `scale`@0x1c, `time`@0x20, `mode`@0x21. Ghidra's names
 *    ("blood.py"/"blood.pz"/"blood.scale"/"smoke.*") are its own union
 *    disambiguation guesses and are WRONG about which member is live here;
 *    only the numeric offsets (confirmed against the raw `.s`) are trusted.
 *  - The `alfa` (0x80 default -> countdown-derived fade) and the
 *    `ef->proc = 0` self-dispose are NOT a single `&&`-chained condition
 *    despite Ghidra's rendering (`(mode==1) && (t=..., alfa=f(t), t==0)`):
 *    the real asm computes `alfa` UNCONDITIONALLY once inside the
 *    `mode == 1` arm (its `srl` sits in a branch delay slot that always
 *    executes) and ONLY THEN separately tests `t == 0` for the dispose —
 *    two statements, not a comma-chained guard. `t` IS read only ONCE in
 *    the target (one `lbu`, reused for both the multiply and the zero
 *    test) — unlike DrawExplosion's case 2, nothing intervenes to evict it
 *    from its register here, so caching it in `t` (PSX.SYM has no local for
 *    it, but the demo list is not exhaustive — see DrawSmoke/DrawExplosion,
 *    which each need several untracked locals too) is correct, not a
 *    byte-chase: removing the cache would UNDER-read relative to target.
 *  - `param->vec.vy` is read TWICE by the target with different widths for
 *    different purposes, un-CSE'd: the narrowing self-update
 *    (`param->vec.vy = param->vec.vy + 5;`, written as a direct compound
 *    statement with NO named temp — PSX.SYM has no `vy` local either) loads
 *    `lhu` (result truncates back to 16 bits so signedness doesn't matter),
 *    the later full-width add into `pos.vy` loads `lh` (needs the sign) —
 *    matching the cookbook's "pure narrowing field-to-field copy is
 *    lhu/lbu regardless of signedness" and "N loads adjacent... are source
 *    temps" rules. A prior draft cached the first read in a named `s16 vy`
 *    local; removing it (direct `param->vec.vy = param->vec.vy + 5;`) is
 *    BYTE-IDENTICAL (confirmed) but matches PSX.SYM's locals list more
 *    closely and is the more human-plausible spelling.
 *  - The final sprite-field stores (`spr->locate.coord.t[0..2]`,
 *    `spr->scale`) re-read `param->pos.*`/`param->scale` FRESH from memory
 *    (matching Ghidra's literal rendering) instead of reusing the values
 *    just computed above — write them as direct field reads, not cached
 *    locals.
 *
 * Length fix (the 1-missing-instruction bug, NOW FIXED): `rotate` needs its
 * own named local (`long rotate;`), read right after `spr->scale =
 * param->scale;` (BEFORE the r/g/b stores) but STORED only at the end
 * (`spr->sprite.rotate = rotate;`, AFTER r/g/b) — this is EXACTLY
 * DrawSmoke.c's proven, matched idiom for the identical Sprite3D tail
 * (its header: "without it the r/g/b stores drift one instruction early,
 * filling a load-delay slot the target leaves as a bare `nop`"), and
 * DrawExplosion.c has the identical `long rotate;` local for the same
 * reason. The previous draft wrote `spr->sprite.rotate = param->rotate;`
 * as ONE direct statement (read and store together, at the end) — that
 * spelling reads `rotate` LATE, so cc1's scheduler filled the earlier
 * load-delay slot (after `spr->scale`'s reload) with the r/g/b stores
 * instead of leaving it as the target's bare `nop` — exactly 1 instruction
 * short. Caching the read early (matching the sibling) reproduces the
 * target's nop and fixes the length to the correct 308 bytes / 77
 * instructions. (NEW RULE for the cookbook: this is the SAME mechanism as
 * DrawSmoke's own rotate idiom — general form: "if a target's tail writes
 * several unrelated byte fields (like r/g/b here) between a field's READ
 * and its STORE, and your draft reads-and-stores that field as ONE direct
 * statement positioned at the store's location, you will emit those
 * unrelated stores ONE INSTRUCTION TOO EARLY (into a load-delay slot the
 * target leaves as `nop`) and come up exactly 1 instruction short overall
 * — split the field into its own named local, read early / store late,
 * matching where the target's actual load lands.")
 *
 * RESIDUAL (18 bytes, all in ONE basic block — the shared "join" tail):
 * a pure register-allocation tie among 4 short-lived, mutually-conflicting
 * temps that all read/write different offsets off `param` (`a2`): `scale`,
 * `vec.vx`, `vec.vz`, `vec.vy` (1st, unsigned read). Target puts
 * scale->v1, vx->a0, vz->a1, vy->v0; this draft puts scale->v0, vx->a1,
 * vz->a0, vy->v1 (a clean swap in each pair) — every later instruction in
 * the block (pos.vz/vx/vy stores, the sprite writes, rotate, both calls,
 * epilogue) is BYTE-IDENTICAL, so this is confined entirely to these 4
 * quantities' initial hard-register colouring.
 *
 * Evidence this is NOT reachable by respelling (do not re-attempt these):
 *  - Statement reorder (`scale`'s statement moved before/after `vy`'s
 *    read), local DECLARATION reorder (`vx, vz` vs `vz, vx`), and removing
 *    the `vy` temp for a direct compound self-update: all THREE produced
 *    BYTE-IDENTICAL compiled output (confirmed via rebuild + matchdiff each
 *    time, not assumed). Root cause (toplev.c:3422 vs :3447, read directly
 *    from the pinned gcc-2.8.1 source via `tools/ccsrc.py`): **sched1 runs
 *    BEFORE local-alloc**, so by the time local_alloc computes each
 *    pseudo's birth/death/QTY_CMP_PRI, the block has ALREADY been
 *    rescheduled from its dependency DAG — source statement order among
 *    mutually-independent loads/stores in one straight-line block does not
 *    survive to influence local-alloc at all. (NEW COOKBOOK FACT: add to
 *    compiler-facts.md's pass-order notes — this generalizes the existing
 *    "sched1 runs before reload" fact.)
 *  - `tools/autorules.py DrawHinoko`: swept 19 candidates (type-width on
 *    every local, all 4 `binop-operand-seed`/compound-assignment forms),
 *    no improving edit — confirms the residual is not one of the
 *    mechanised rules.
 *  - `tools/regalloc.py DrawHinoko --local`: reports an explicit
 *    **SELF-VALIDATION FAILURE** on this exact block (its simulated
 *    block_alloc walk predicts p104/p107/p108 should land v1/v0/v1, cc1
 *    actually assigns v0/v1/v0) — the tool's own message: "the
 *    suggestion/coalescing model may be incomplete for this block." This
 *    is a genuine gap in a tool that self-validates with 0 divergences
 *    across ~150 other functions; reported per the tool's own request.
 *    Read `local-alloc.c`'s `qty_compare`/`QTY_CMP_PRI` (shorter-lived,
 *    more-referenced quantities win the numerically-first free register)
 *    directly via `tools/ccsrc.py` — confirms the PRIORITY mechanism, but
 *    the tool's quantity-COALESCING model (which raw pseudos merge into
 *    one quantity before that sort runs) diverges from cc1's real graph
 *    here; the cause of the divergence itself is still open. The
 *    `next_qty<=3` incomplete compare-exchange (local-alloc.c:1638) does
 *    NOT apply — this block's quantity count is much larger than 3, so
 *    the correct `qsort` path runs.
 *  - One bounded permuter run (`timeout 240 tools/permute.py DrawHinoko --
 *    --stop-on-zero -j4`) found a best candidate at 4 whole-image bytes
 *    (`output-25-1`, full-link rescored) — REJECTED on a semantic veto,
 *    not adopted: it introduces a second pointer local (`new_var`) that is
 *    genuinely unreachable dead code (assigned between an unconditional
 *    `goto join;` and a label, never executed) purely to perturb global
 *    allocation, then reads `pos.vz` through it instead of through
 *    `param`. Checked against the target's OWN evidence: every single one
 *    of the ~14 loads in the target's real 45-instruction join block goes
 *    through EITHER `a2` (param) or `s0` (spr) — there is no third pointer
 *    register anywhere in the target's actual code, so a second pointer
 *    identity is categorically not the real structure, just a coincidental
 *    global-alloc side effect (confirmed by testing 3 different positions
 *    for the dead statement and a recomputed-address variant: all four
 *    landed on the identical wrong register, `$t0`, stolen from mode1's
 *    own `mfhi` scratch need — never reproducing the target's plain `a2`
 *    read). The second-best candidate (6 bytes, `output-40-1`) uses a bare
 *    `(0, vx)` comma-expression trick — also rejected, both worse and
 *    equally synthetic. Per the owner's steer this round: avoid weird
 *    byte-chasing constructs; a 4-byte score is not worth adopting
 *    dead-code allocation-pressure hacks that contradict the target's own
 *    register evidence.
 *
 * This is evidence-complete as a sub-C register-allocation tie: right
 * length, right instruction SET, 4 short-lived same-class temps correctly
 * identified as mutually conflicting, wrong specific v0/v1/a0/a1 colouring
 * that no tested source respelling reaches (sched1 normalizes ahead of
 * local-alloc) and that the specialised tool cannot currently model
 * (self-validation failure, reported above).
 */
/* Sprite3D isn't in effect.h; item.h has one but truncated at `scale`
 * (safe for its own TU, which never touches the trailing GsSPRITE sub-
 * struct DrawHinoko needs) — declared locally, full 140-byte layout, per
 * DrawEffect.c's precedent of TU-local type decls even where a same-named,
 * differently-scoped type exists elsewhere. */
typedef struct
{
    GsCOORDINATE2 locate; /* +0x00 */
    SVECTOR rotate;       /* +0x50 */
    s16 id;               /* +0x58 */
    s16 attribute;        /* +0x5a */
    SVECTOR clip;         /* +0x5c */
    long scale;           /* +0x64 */
    GsSPRITE sprite;      /* +0x68 */
} Sprite3D;

extern Sprite3D *sprBomb[3];
extern void UpdateCoordinate(Sprite3D *m);
extern void DrawSprite(Sprite3D *s);

#ifndef NON_MATCHING
INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/DrawHinoko", DrawHinoko);
#else /* NON_MATCHING */

static void DrawHinoko(TEffectSlot *ef)
{
    ExplosionType *param;
    Sprite3D *spr;
    u8 alfa;
    u8 t;
    long vz, vx;
    long vy2;
    long pz;
    long rotate;

    param = &ef->param.explosion;
    spr = sprBomb[2];
    alfa = 0x80;
    if (param->mode == 0) goto mode0;
    if (param->mode == 1) goto mode1;
    goto join;
mode0:
    if (param->time == 0)
    {
        param->mode = 1;
        param->time = 0x1e;
    }
    goto join;
mode1:
    t = param->time;
    alfa = (u8)((t * 0x80) / 30);
    if (t == 0)
    {
        ef->proc = 0;
    }
join:

    vx = param->vec.vx;
    vz = param->vec.vz;
    param->time = param->time - 1;
    param->scale = param->scale + 0xc00;
    pz = param->pos.vz;
    param->vec.vy = param->vec.vy + 5;
    param->pos.vz = pz + vz;
    vy2 = param->vec.vy;
    param->pos.vx = param->pos.vx + vx;
    param->pos.vy = param->pos.vy + vy2;
    spr->locate.coord.t[0] = param->pos.vx;
    spr->locate.coord.t[1] = param->pos.vy;
    spr->locate.coord.t[2] = param->pos.vz;
    spr->scale = param->scale;
    rotate = param->rotate;
    spr->sprite.r = alfa;
    spr->sprite.g = alfa;
    spr->sprite.b = alfa;
    spr->sprite.rotate = rotate;
    UpdateCoordinate(spr);
    DrawSprite(spr);
}

#endif /* NON_MATCHING */
