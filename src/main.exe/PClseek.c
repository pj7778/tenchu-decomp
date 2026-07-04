#include "common.h"
#include "main.exe.h"

/*
 * PClseek (0x80060224) — PC-file lseek stub (part of the PCopen/PCclose/
 * PClseek/PCcreat/PCinit/PCread/PCwrite host-file-I/O family; "Possible
 * LSEEK.OBJ/PClseek" per Ghidra, i.e. a PsyQ CRT library object). It shifts
 * its 3 args up one register slot (fd:$a0->$a1, offset:$a1->$a2, mode:$a2->
 * $a3) and executes a raw `break 0x0,0x107` (MIPS trap/syscall exception) —
 * the PS1 BIOS's host-link mechanism for a real lseek() when running under a
 * dev kit. Ghidra models the two BIOS outputs as bare incoming registers
 * `in_v0`/`in_v1` (error code / result): `if (in_v0 != 0) in_v1 = -1; return
 * in_v1;`.
 *
 * STATUS: NON_MATCHING — 7 of 36 bytes differ (whole-image). Two compounding
 * problems, not just one:
 *
 * 1. `break 0x0,0x107` has NO C representation at all — the only way to
 *    emit it is inline asm with register-pinned variables (confirmed
 *    working below: `register int x __asm__("$5") = fd;` etc., feeding a
 *    `break` instruction via extended asm). This project explicitly treats
 *    inline asm as unresolved/sensitive (see GetPad.c's header: "a genuine
 *    optimizer barrier... is NOVEL to this project... parked pending a
 *    project decision") — introducing it here is the same open question,
 *    not a call an individual matcher pass should make unilaterally.
 * 2. Tool finding (reusable): maspsx's `break` handling
 *    (`maspsx/__init__.py`, `elif op == "break"`) expects the SINGLE-value
 *    form (`break 0x107`) and itself splits it into the two 10-bit fields —
 *    NOT the two-operand disassembly form (`break 0, 263`) Ghidra/objdump
 *    print; the latter makes maspsx raise `invalid literal for int() with
 *    base 0: '0,'`.
 * 3. Even past both of those, the draft below has a stubborn 7-byte
 *    register-materialization-timing residual: the target computes the
 *    result into $v0 EARLY (a `move v0,v1` in the `beqz`'s delay slot, so
 *    both the success path (v0=v1) and the error path (v0=-1) leave the
 *    answer in $v0 well before the epilogue, which is then a bare `nop`),
 *    whereas cc1 here keeps the value in the trap's own $v1 output
 *    throughout (a zero-cost coalesce — "result = r_v1" is free once r_v1
 *    is pinned to $3) and only inserts the ABI-mandated $v1->$v0 move at the
 *    true `return` (the LAST possible point, not the first). Tried: a
 *    shared `result` local (same outcome — coalesces with r_v1 regardless),
 *    two early returns (`if(err) return -1; return r_v1;` — fixes the
 *    v0/v1 choice but cc1 no longer cross-jumps the two returns into one
 *    epilogue, costing a whole extra `j ra` = wrong length), reusing r_v0
 *    itself as the accumulator (same extra-epilogue problem), and a ternary
 *    return (different wrong shape: routes through $a0 as scratch instead).
 *    This looks like a genuine sub-C-level regalloc tie on top of the
 *    inline-asm question, not a source-shaping problem — a candidate for
 *    the permuter IF inline asm is ever accepted here, but not chased
 *    further while that policy question is open.
 *
 * The #else draft is the CLOSEST variant found (single shared return,
 * 7/36 bytes off); default build keeps the byte-identical two-piece
 * INCLUDE_ASM stub (LSEEK_OBJ_1C is this function's own tail: the `jr ra`
 * epilogue at 0x80060240, reached both by fallthrough and by the branch —
 * not a separate function; splat auto-carved it as its own piece only
 * because it already had a symbols.main.exe.txt name).
 */
#ifndef NON_MATCHING
INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/PClseek", PClseek);
INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/PClseek", LSEEK_OBJ_1C);
#else
int PClseek(int fd, int offset, int mode)
{
    register int r_a3 __asm__("$7") = mode;
    register int r_a2 __asm__("$6") = offset;
    register int r_a1 __asm__("$5") = fd;
    register int r_v0 __asm__("$2");
    register int r_v1 __asm__("$3");

    __asm__ volatile("break 0x107"
                      : "=r"(r_v0), "=r"(r_v1)
                      : "r"(r_a1), "r"(r_a2), "r"(r_a3));

    if (r_v0 != 0)
        r_v1 = -1;
    return r_v1;
}
#endif
