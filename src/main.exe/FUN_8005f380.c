#include "common.h"
#include "main.exe.h"

/*
 * STATUS: NON_MATCHING — 23 of 472 bytes differ; RIGHT LENGTH (118
 * instructions both sides, confirmed by asmdiff: no inserts/deletes, only
 * 3 small same-length reorder clusters). All three are the sub-C-level
 * "which independent, harmless instruction fills this delay slot" class
 * this cookbook already names (verified: `tools/autorules.py` found no
 * improving local-type edit; one bounded `tools/permute.py` run,
 * ~260s/`--stop-on-zero -j4`, found only a semantically WRONG candidate —
 * it moved the `CdReady`-failure `goto full_retry;` into the
 * `CdGetSector`-failure branch instead, leaving the real branch empty —
 * rejected per the cookbook's "a permuter winner can carry red herrings"
 * warning; never adopt a control-flow edit that changes which failure
 * retries from which point without re-deriving it from the asm):
 *  - `param[0] = 0xA0;` (the seek-mode scratch byte) schedules into
 *    `CdIntToPos`'s own delay slot in the target (`move a0,s6 / addiu
 *    a1,sp,... / jal CdIntToPos / sb v0,...`); this build emits the store
 *    before the call's own arg setup instead (same 3 instructions, target
 *    order swapped) — tried reordering the two source statements
 *    (`CdIntToPos` first, store second); it moved a DIFFERENT residual
 *    instead of fixing this one (net same size), so reverted.
 *  - The mode-0xE retry loop's own back-edge delay slot: target
 *    rematerialises `li v0,0xA0` there (the same seek-mode constant,
 *    ready for the next retry's store); this build leaves it a bare
 *    `nop`.
 *  - The copy loop's own entry test (`chunk > 0`) and its counter's `i=0`
 *    init are swapped one instruction relative to each other.
 * Parked per the sub-C-level early-stop.
 *
 * FUN_8005f380 (0x8005f380, 0x1D8 bytes) — raw CD sector reader: reads
 * `length` bytes starting at byte `byteOffset` of sector `sector` into
 * `buffer` (see cd_read.c / FUN_8005f7d0.c, both of which already declare
 * `extern void FUN_8005f380(void *buffer, int sector, int byteOffset,
 * int byteLength);`). Below the 0x80060000 PsyQ/CRT boundary, so it's a
 * game-TU function even though every callee it makes is a BIOS libcd
 * primitive (CdControlB/CdReady/CdGetSector/CdPosToInt/CdIntToPos/VSync).
 *
 * Shape: seek to `sector` (CdControlB mode 0xE, retried while busy), then
 * "read" mode (CdControlB mode 6, retried while busy) to prime streaming,
 * then loop: wait for a ready sector (CdReady), pull it whole
 * (CdGetSector, one 0x203-word/2060-byte raw sector: a 12-byte subheader
 * + 2048 data bytes), confirm its position (CdPosToInt) matches the
 * expected running sector — if not, the drive drifted: re-seek (mode 6)
 * to the expected sector and keep waiting. On a match, copy the wanted
 * slice (clamped to one sector, i.e. `min(off+remaining, 0x800) - off`
 * bytes starting `off` bytes into the sector's DATA portion, skipping the
 * 12-byte subheader) into the caller's buffer, advance, and repeat until
 * `remaining` runs out — then a last CdControlB mode-9 "stop" (retried
 * while busy) and return. If `CdReady` itself ever reports "not ready",
 * the WHOLE sequence restarts from the top (re-seek included); if only
 * `CdGetSector` fails, just the mode-6 re-read priming restarts (the
 * original seek position is unchanged).
 *
 * Buffer layout (Ghidra's own locals, all proven by the raw offsets +
 * frame math: 0x810 + 8 + 8 = 0x820 locals, + 0x28 (10 saved regs) + 0x10
 * (own call args) = 0x858, the exact frame):
 *  - Ghidra's `aCStack_848 [3]` (a CdlLOC[3], 12 bytes) immediately
 *    followed by `auStack_83c [2052]` is ONE 0x810-byte buffer (12+2052 =
 *    0x810 exactly) — the raw CdGetSector DMA target (its own 12-byte
 *    subheader, then the 2048-byte payload); Ghidra's split into two
 *    differently-typed locals is just its own per-access-pattern
 *    heuristic, not two real buffers (cookbook: "overlapping stack
 *    locals... the original really used one buf[N] + casts").
 *  - `local_38 [8]` (only byte 0 used, as CdControlB mode 0xE's own
 *    scratch param — a fresh 8-byte local, not read back).
 *  - `aCStack_30 [2]` (only element 0 used; CdIntToPos's output and every
 *    CdControlB mode-6/9 call's own param, always addressed at its OWN
 *    offset 0 — Ghidra's `&aCStack_30[0].minute` is offset 0 of the
 *    struct, i.e. just `(u8 *)loc`, not `.second`).
 *
 * Matching notes:
 *  - `dst`/`curSector`/`remaining`/`off` (Ghidra's puVar8/iVar2/iVar7/
 *    iVar6) are (re)initialised ONCE, right after `CdIntToPos`, BEFORE
 *    either retry loop (mode 0xE then mode 6) — Ghidra's own rendering
 *    nests them inside the mode-6 retry `do {} while`, which is a
 *    decompiler placement artifact (cookbook: "trust the assembly over
 *    Ghidra's statement order"): the raw .s runs that init exactly once
 *    per outer/reseek entry, straight-line, before either loop.
 *  - Two distinct retry destinations, not one: `CdReady` reporting
 *    not-ready restarts from the very top (re-`CdIntToPos`, mode 0xE
 *    seek again); `CdGetSector` failing restarts only from the
 *    reinitialise-cursors + mode-6 re-prime point, skipping the seek.
 *    Written as two labels (`full_retry`/`reseek_retry`) with an
 *    unconditional fallthrough between them, matching the asm's own
 *    `.L8005F3C8`-falls-into-`.L8005F3CC` shape — not two nested loops
 *    (a `break` can't reach two different outer points).
 */

typedef struct CdlLOC CdlLOC;
struct CdlLOC
{
    u8 minute;
    u8 second;
    u8 sector;
    u8 track;
};

extern int CdControlB(u8 com, u8 *param, u8 *result);
extern int VSync(int mode);
extern int CdReady(int mode, u8 *result);
extern int CdGetSector(void *madr, int size);
extern int CdPosToInt(CdlLOC *pos);
extern void CdIntToPos(s32 n, CdlLOC *pos);

#ifndef NON_MATCHING
INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/FUN_8005f380", FUN_8005f380);
#else

void FUN_8005f380(u8 *buffer, s32 sector, s32 byteOffset, s32 length)
{
    u8 sectorBuf[0x810];
    u8 param[8];
    CdlLOC loc[2];
    u8 *raw;
    u8 *data;
    u8 *dst;
    u8 *src;
    s32 curSector;
    s32 remaining;
    s32 off;
    s32 n;
    s32 vsyncArg;
    s32 chunk;
    s32 i;

    raw = sectorBuf;
    data = sectorBuf + 0xC;

    if (length < 1) return;

full_retry:
    param[0] = 0xA0;
    CdIntToPos(sector, loc);

reseek_retry:
    dst = buffer;
    remaining = length;
    curSector = sector;
    off = byteOffset;

    while (CdControlB(0xE, param, 0) == 0) {
        VSync(0);
    }

    vsyncArg = 3;
    do {
        VSync(vsyncArg);
        n = CdControlB(6, (u8 *)loc, 0);
        vsyncArg = 0;
    } while (n == 0);

    while (remaining >= 1) {
        n = CdReady(0, 0);
        if (n != 1) goto full_retry;
        n = CdGetSector(sectorBuf, 0x203);
        if (n == 0) goto reseek_retry;

        n = CdPosToInt((CdlLOC *)raw);
        if (n != curSector) {
            CdIntToPos(curSector, loc);
            while (CdControlB(6, (u8 *)loc, 0) == 0) {
                VSync(0);
            }
        } else {
            chunk = off + remaining;
            if (chunk > 0x800) chunk = 0x800;
            chunk = chunk - off;
            src = data + off;
            for (i = 0; i < chunk; i++) {
                dst[i] = src[i];
            }
            dst = dst + chunk;
            remaining = remaining - chunk;
            off = 0;
            curSector = curSector + 1;
        }
    }

    while (CdControlB(9, (u8 *)loc, 0) == 0) {
        VSync(0);
    }
}
#endif
