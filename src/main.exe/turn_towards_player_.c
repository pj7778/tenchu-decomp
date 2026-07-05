#include "common.h"
#include <psxsdk/libgs.h>
#include "game_types.h"

/*
 * STATUS: NON_MATCHING — 76 of 480 bytes differ (whole function, right
 * length: the residual doesn't shift anything downstream).
 *
 * turn_towards_player_ (0x8002b990, 0x1E0 bytes) — the shared "which way to
 * turn, and is it safe to step" helper called by nearly every AI think
 * handler in this TU (Think1sleep/Think2confirm/think_setting_go_towards_-
 * player and many unmatched Think* siblings). Result is a bitmask of
 * button-like turn/walk bits (0x1000 "close enough to stop turning",
 * 0x2000/0x8000 "turn right/left") truncated to s16 on return (same
 * shared-tail sll/sra-once shape as GetDirection).
 *
 * Deliberately does NOT include main.exe.h: this file needs `Attrib` and
 * `Degree` read as u16 (`lhu`), conflicting with main.exe.h's `s16 Attrib`
 * (used by Think1sleep.c/Think2confirm.c) — same per-file respelling as
 * think_setting_go_towards_player.c.
 *
 * Structure (verified against the raw .s, not just Ghidra/m2c's flattening) —
 * everything below is BYTE-PROVEN except the final residual:
 *  - `dir` is u16 (matches m2c): raw GetDirection()/Degree bits, given an
 *    explicit `(s16)` cast at each of its 3 separate comparison sites
 *    (turn-vs-dir, dir-vs-turn, abs-for-500) rather than a cached temp — the
 *    asm shows ONE shared truncation reused for the first two comparisons
 *    (they're in the same extended basic block) but a FRESH one for the
 *    third (past the if/else-if join's CODE_LABEL, outside cse's window).
 *  - The turn/dir clamp is GetDirection's own default-then-override shape:
 *    default (0) was set at function entry, not immediately before the if.
 *  - `cached` (D_80097F10) is read once into a callee-saved local — it
 *    survives both GetAreaMapLevel calls and is re-tested afterward.
 *  - The two GetAreaMapLevel calls each re-read
 *    `Me_THINK_C->some_kind_of_current_position` fresh (no cached pointer
 *    var) — proven by the asm reloading it independently both times.
 *  - The final flag computation is a goto-shared store: read literally off
 *    m2c's structure (a comma-expression side effect inside the second `&&`
 *    is exactly what the asm's unconditional delay-slot `li` proves).
 *
 * RESIDUAL (the whole 76-byte gap): the `flag`/`(result & 0x8000)` test
 * register assignment in the final flag-computation block. The target keeps
 * `flag` in $v1 in BOTH branches (a direct `lui $v1,...` in the first, an
 * explicit `move $v1,$v0` in the second, after $v0 was needed transiently
 * for the `d2==0x80000000` compare) and reserves $v0 for the `result&0x8000`
 * test value across the intervening `goto` — this draft's cc1 instead puts
 * `flag` in $v0 for the first branch and the test in $v1, an internally
 * self-consistent but registers-swapped allocation (confirmed by
 * `tools/regalloc.py`: a `v0 -> v1` copy-chain at the `flag` merge point that
 * the target doesn't have). Tried and rejected (all produced IDENTICAL
 * asm to the comma-expression baseline, or worse):
 *   - hoisting `result & 0x8000` into an explicit `test8000` local computed
 *     right before the first `if` (no change — cc1 still picks the same
 *     registers) or before the two GetAreaMapLevel calls (WORSE: extends
 *     test8000's live range across both calls, forcing an extra
 *     callee-saved register and growing the frame by 8 bytes);
 *   - replacing the second condition's comma-expression with an equivalent
 *     nested-if + goto (produced byte-IDENTICAL output to the
 *     comma-expression form — cc1 desugars both the same way, so this
 *     wasn't the lever);
 *   - swapping the `s32 d1, d2, flag;` / `test8000` declaration order
 *     (no effect).
 *   - `tools/permute.py turn_towards_player_ -- --stop-on-zero -j4`, a
 *     bounded ~7 minute run: best score 400 (worse than this draft),
 *     never found 0 — consistent with the cookbook's sub-C-level
 *     early-stop guidance (permuter-immune within budget).
 * decomp.me (psyq4.3 preset) would be the next arbiter if this is revisited.
 */
#ifndef NON_MATCHING
INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/turn_towards_player_", turn_towards_player_);
#else
extern character_state *Me_THINK_C;
extern u16 Attrib;
extern u16 Degree;
extern s32 D_80097F10;
extern void *GlobalAreaMap;

extern s16 GetDirection(s32 dx, s32 dz, s32 roty);
extern void GetMoveSpeed(SVECTOR *out, s32 roty, s32 b, s32 width);
extern s32 GetAreaMapLevel(void *map, s32 x, s32 y, s32 z, u16 flag);

s16 turn_towards_player_(s32 x_diff, s32 z_diff)
{
    u16 dir;
    s32 turn;
    s32 result;
    s32 adir;

    result = 0;
    if (x_diff != 0 || z_diff != 0)
    {
        dir = GetDirection(x_diff, z_diff,
                            Me_THINK_C->something_about_player_rotation_perhaps->character_rotation);
    }
    else
    {
        dir = Degree;
    }
    turn = Me_THINK_C->character_rotation_speed;
    if (turn < (s16)dir)
    {
        result = 0x2000;
    }
    else if ((s16)dir < -turn)
    {
        result = -0x8000;
    }
    adir = (s16)dir;
    if (adir < 0)
    {
        adir = -adir;
    }
    if (adir < 500)
    {
        result |= 0x1000;
    }
    if (!(Attrib & 3))
    {
        if (Attrib & 0x400)
        {
            s32 cached;

            cached = D_80097F10;
            if (cached == 0x80000000)
            {
                if (Me_THINK_C->field76_0xb0 == 0)
                {
                    SVECTOR local;
                    s32 d1, d2, flag;

                    GetMoveSpeed(&local,
                                 Me_THINK_C->something_about_player_rotation_perhaps->character_rotation,
                                 0, Me_THINK_C->field6_0xc);
                    d1 = GetAreaMapLevel(GlobalAreaMap,
                                         Me_THINK_C->some_kind_of_current_position->vx + local.vx,
                                         Me_THINK_C->some_kind_of_current_position->vy - 500,
                                         Me_THINK_C->some_kind_of_current_position->vz + local.vz,
                                         0x1a);
                    d2 = GetAreaMapLevel(GlobalAreaMap,
                                         Me_THINK_C->some_kind_of_current_position->vx - local.vx,
                                         Me_THINK_C->some_kind_of_current_position->vy - 500,
                                         Me_THINK_C->some_kind_of_current_position->vz - local.vz,
                                         0x1a);
                    if ((result & 0x2000) && (d1 != cached))
                    {
                        flag = 0x20000000;
                        goto apply;
                    }
                    if ((result & 0x8000) && (flag = 0x80000000, d2 != 0x80000000))
                    {
                    apply:
                        Me_THINK_C->field76_0xb0 = flag | 0x1e;
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }
    return result;
}
#endif
