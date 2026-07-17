#include "common.h"
#include "main.exe.h"
#include "gte.h"

/*
 * FUN_80059ff4 (0x80059ff4, 0x3d8 bytes) — DecodeTMD-family primitive
 * renderer, TMD primitive code 0x25 in FUN_800593a0's switch (paired there
 * with FUN_8005961c/FUN_80059b08/FUN_8005a3cc — all four called with the
 * identical (u_short *, u_long, u_long *, u_short, u_long *) signature).
 * Builds one POLY_GT3 (Gouraud-shaded, textured triangle, GPU code 0x34 —
 * reference/psxsym-types.h's recorded 40-byte layout matches every offset
 * this function touches exactly) output packet per input record: transforms
 * the record's 3 vertex indices through the GTE (RTPT), discards
 * back-facing/degenerate triangles (NCLIP MAC0 <= 0), discards triangles
 * whose screen-space bbox misses the caller's clip rectangle, computes an
 * OTZ bucket index from the rounded max depth, applies depth-cueing (DPCS)
 * to each vertex's pre-baked color when within the far-fog range, then
 * copies the finished packet into the caller's output list and re-links the
 * caller's OT bucket to point at it.
 *
 * param_5 ("work") is the shared per-call rendering context also used by the
 * sibling pair FUN_80058c70/FUN_80059008 (offsets 0x94/0x98/0x9c/0xa0 =
 * screen clip box match FUN_800593a0's own setup of that same struct at
 * entry) — this function additionally uses +0x34 (POLY_GT3 staging), +0x38
 * (a SEPARATE cached pointer 4 bytes into the same staging area — see
 * below), +0x5c/+0x60/+0x64 (per-vertex SZ), +0x74 (GTE FLAG), +0x78 (OTZ
 * index), +0x80 (NCLIP/MAC0 winding), +0x84 (near-Z reject threshold), +0x88
 * (OT bucket shift), +0x8c (far-fog Z), +0x90 (indirect OT table pointer).
 *
 * Matching notes:
 *  - Compiled-style GTE function under the restricted gte.h policy
 *    (docs/gte-policy.md, config/gte-allowlist.txt) — the COP2 moves and
 *    commands are the original INLINE_N.H mechanism. Two macros were
 *    missing and are added in gte.h, both verbatim from the real PsyQ SDK's
 *    INLINE_C.H (the INLINE_N.H equivalent in the extracted psyq4.5 tree):
 *    `gte_lddp` (mtc2 to $8/IR0, feeds DPCS's depth-cue factor) and
 *    `gte_stsxy3_gt3` (the GT3-strided SXY store, offsets 8/0x14/0x20 —
 *    Ghidra had already guessed this exact name from the offset pattern;
 *    the real header confirms it byte for byte, no nops on either macro).
 *  - GOTO LOOP, not do-while. The sibling pair FUN_80058c70/FUN_80059008
 *    (same DrawTMD render family, same "walking record pointer read at
 *    small fixed offsets" shape) independently measured that a genuine
 *    do-while triggers loop.c strength reduction into a second parallel
 *    induction register; only a hand `goto` loop (no NOTE_INSN_LOOP, no
 *    hoisting) reproduced the target. Applied here on the same evidence: all
 *    six values live across every iteration (prim, codeAddr, codeVal,
 *    rec, and the two pointers folded into gte_stsz3's call) are plain
 *    locals assigned once before `loop_top:`, reproducing the target's six
 *    simultaneously-live preheader registers (t3/t8/t9/t7/t6/t5) with no
 *    loop optimizer involved.
 *  - `int param_4`, not u_short, mirrors the caller's unmasked
 *    `addiu a3,a3,-1; bnez a3` — same lesson as FUN_80058c70 (the caller's
 *    extern says u_short count; the callee is ABI-compatible as int, and
 *    the unmasked branch proves it is one).
 *  - The whole guard chain (FLAG<0, winding<=0, X/Y bbox miss, near-Z miss)
 *    is a flat `if (reject) goto next;` ladder, not nested ifs — all seven
 *    reject branches in the target address the exact same label (the
 *    decrement-and-continue tail), the multi-guard-clause shape (cookbook
 *    3.2).
 *  - Each min/max-of-3 ladder (screen X, screen Y, then the three raw SZs)
 *    is `lo=a; hi=b; if (b<a) {lo=b;hi=a;} if (c<lo) lo=c; else if (hi<c)
 *    hi=c;` — traced instruction-by-instruction off the raw asm (the
 *    interesting bit is that the compare-then-delay-slot pairing makes `a0`
 *    end up holding the true min and `t0` the true max on EVERY exit path,
 *    so no third "which-one-changed" temp is needed, unlike Ghidra's
 *    comma-expression rendering of the same bytes).
 *  - The OTZ index is a plain signed `/4`: cc1's magic sequence for signed
 *    division by a power of two (`(hi<0 ? hi+3 : hi) >> 2`) reproduces the
 *    target's `bgez/addiu 3/sra 2` exactly; do not hand-spell the +3.
 *  - `work->farZ` (+0x8c) is captured in a local ONLY for the outer guard +
 *    the first DPCS block (matching the target reusing one register there);
 *    the second and third DPCS blocks re-read `*(s32 *)(...+0x8c)` fresh —
 *    the target reloads it every time even though nothing clobbers the
 *    register in between, so this is source structure, not allocator
 *    behaviour ("target's redundant reload = a fresh field dereference in
 *    source", cookbook 3.4).
 *  - The color word at `rec+0` is read three separate times (once before
 *    NCLIP for vertex0, twice more after NCLIP for vertex1/vertex2 — all
 *    three vertices share ONE baked color): three literal dereferences, not
 *    a cached temp, matching the target's three un-CSE'd `lw`s.
 *  - The final packet copy (40 bytes: two 16-byte loop iterations then an
 *    8-byte remainder) and the "mask low 24 bits into the OT slot" tail are
 *    IDENTICAL in the sibling FUN_8005a3cc's own disassembly (confirmed
 *    with `tools/tdis.py` on both) — the two functions are
 *    instruction-for-instruction identical, differing only in embedded
 *    constants (record stride/offsets, packet field layout). Whatever
 *    structure closes this function should transcribe directly.
 */

/*
 * STATUS: NON_MATCHING at 619/984 — and that number is the finding, not a failure.
 * Its two family siblings sit at 620/920 (FUN_80058c70) and 620/920
 * (FUN_80059008). Three independent drafts, three ~620-byte residuals, and
 * FUN_80059008's round confirmed the same `.greg` signature driving all of them:
 * one conflict (param_1's pseudo vs the hard register $a0) cascading across the
 * whole function — `matchdiff --clusters` puts 185 of 230 insns in ONE cluster.
 * Landing on the sibling's exact residual from an independent draft is
 * cross-validation that this is a family-wide compiler limitation rather than a
 * drafting error.
 *
 * The permuter cannot search this class at all: gte.h's inline asm defeats its C
 * parser, and permute.py refuses up front and says so (docs/gte-policy.md).
 * autorules found no improving edit.
 *
 * PROVENANCE: salvaged. The lane that drafted this backgrounded a tool, ended its
 * turn waiting for a notification that cannot wake a finished agent, and died
 * without committing — the second lane to do that today. The draft, its identity
 * work (POLY_GT3 / TMD primitive code 0x25, layout cross-checked against
 * reference/psxsym-types.h) and the gte.h `gte_lddp` addition are all sound: the
 * default build is byte-identical, the draft builds, and a full `./Build clean &&
 * ./Build check` proves the shared-header change breaks no other GTE function.
 * The whole family is on config/gte-allowlist.txt, so the macros are legitimate.
 *
 * NEXT: do not re-derive the shape — clone it. FUN_8005a3cc is a 1.00 mnemonic
 * clone of this function (findsimilar now reports clone groups), so whatever
 * moves this one moves that one for a constants edit. The lever, if it exists, is
 * the param_1/$a0 conflict and it is shared with FUN_80058c70/FUN_80059008: fix it
 * once, collect ~3.9k bytes.
 */
#ifndef NON_MATCHING
INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/FUN_80059ff4", FUN_80059ff4);
#else
u_long *FUN_80059ff4(u_short *param_1, u_long param_2, u_long *param_3, int param_4, u_long *param_5)
{
    u8 *rec;
    u8 *prim;
    u8 *codeAddr;
    s32 codeVal;
    u_long *sz0Ptr;
    u_long *sz1Ptr;
    u32 idx0, idx1, idx2;
    s16 flag;
    s32 a, b, c, lo, hi;
    s32 farZ;
    u_long *otSlot;
    u_long *rgbPtr;
    s32 *flagAddr;
    u_long *src;
    u_long *dst;
    u_long *bound;
    u_long tag;
    u_long w0, w1, w2, w3;
    u_long maskedAddr;

    if (param_4 != 0) {
        prim = (u8 *)param_5 + 0x34;
        codeAddr = (u8 *)param_5 + 0x38;
        codeVal = 0x34;
        sz0Ptr = (u_long *)((u8 *)param_5 + 0x5C);
        sz1Ptr = (u_long *)((u8 *)param_5 + 0x60);
        rec = (u8 *)param_1 + 0x10;

        do {
        idx0 = *(u16 *)(rec + 4);
        idx1 = *(u16 *)(rec + 6);
        idx2 = *(u16 *)(rec + 8);
        gte_ldv3((SVECTOR *)(idx0 * 8 + param_2), (SVECTOR *)(idx1 * 8 + param_2),
                 (SVECTOR *)(idx2 * 8 + param_2));
        gte_rtpt();

        *(s32 *)(prim + 0xC) = *(s32 *)(rec - 12);
        *(s32 *)(prim + 0x18) = *(s32 *)(rec - 8);
        *(s32 *)(prim + 0x24) = *(s32 *)(rec - 4);
        *(s32 *)(prim + 4) = *(s32 *)rec;
        *(codeAddr + 3) = (u8)codeVal;
        flagAddr = (s32 *)((u8 *)param_5 + 0x74);
        gte_stflg(flag);
        *flagAddr = flag;
        if (*(volatile s32 *)((u8 *)param_5 + 0x74) < 0) goto next;

        gte_nclip();
        *(s32 *)(prim + 0x10) = *(s32 *)rec;
        *(s32 *)(prim + 0x1C) = *(s32 *)rec;
        gte_stopz((u_long *)((u8 *)param_5 + 0x80));
        if (*(s32 *)((u8 *)param_5 + 0x80) <= 0) goto next;

        gte_stsxy3_gt3(prim);

        a = *(s16 *)(prim + 8);
        b = *(s16 *)(prim + 0x14);
        if (b < a) {
            lo = b;
            hi = a;
        } else {
            lo = a;
            hi = b;
        }
        c = *(s16 *)(prim + 0x20);
        if (c < lo) {
            lo = c;
        } else if (hi < c) {
            hi = c;
        }
        if (hi < *(s32 *)((u8 *)param_5 + 0x94)) goto next;
        if (*(s32 *)((u8 *)param_5 + 0x98) < lo) goto next;

        a = *(s16 *)(prim + 0xA);
        b = *(s16 *)(prim + 0x16);
        if (b < a) {
            lo = b;
            hi = a;
        } else {
            lo = a;
            hi = b;
        }
        c = *(s16 *)(prim + 0x22);
        if (c < lo) {
            lo = c;
        } else if (hi < c) {
            hi = c;
        }
        if (hi < *(s32 *)((u8 *)param_5 + 0x9C)) goto next;
        if (*(s32 *)((u8 *)param_5 + 0xA0) < lo) goto next;

        gte_stsz3(sz0Ptr, sz1Ptr, (u_long *)((u8 *)param_5 + 0x64));
        a = *(s32 *)((u8 *)param_5 + 0x5C);
        b = *(s32 *)((u8 *)param_5 + 0x60);
        if (b < a) {
            lo = b;
            hi = a;
        } else {
            lo = a;
            hi = b;
        }
        c = *(s32 *)((u8 *)param_5 + 0x64);
        if (c < lo) {
            lo = c;
        } else if (hi < c) {
            hi = c;
        }
        *(s32 *)((u8 *)param_5 + 0x78) = hi / 4;
        if (*(s32 *)((u8 *)param_5 + 0x84) < lo) goto next;

        farZ = *(s32 *)((u8 *)param_5 + 0x8C);
        if (farZ < hi) {
            rgbPtr = (u_long *)(prim + 4);
            gte_ldrgb(*rgbPtr);
            gte_lddp(*(s32 *)((u8 *)param_5 + 0x5C) - farZ);
            gte_dpcs();
            if (farZ < *(s32 *)((u8 *)param_5 + 0x5C)) {
                gte_strgb(*rgbPtr);
            }

            rgbPtr = (u_long *)(prim + 0x10);
            gte_ldrgb(*rgbPtr);
            gte_lddp(*(s32 *)((u8 *)param_5 + 0x60) - *(s32 *)((u8 *)param_5 + 0x8C));
            gte_dpcs();
            if (*(s32 *)((u8 *)param_5 + 0x8C) < *(s32 *)((u8 *)param_5 + 0x60)) {
                gte_strgb(*rgbPtr);
            }

            rgbPtr = (u_long *)(prim + 0x1C);
            gte_ldrgb(*rgbPtr);
            gte_lddp(*(s32 *)((u8 *)param_5 + 0x64) - *(s32 *)((u8 *)param_5 + 0x8C));
            gte_dpcs();
            if (*(s32 *)((u8 *)param_5 + 0x8C) < *(s32 *)((u8 *)param_5 + 0x64)) {
                gte_strgb(*rgbPtr);
            }
        }

        dst = param_3;
        src = (u_long *)prim;
        otSlot = *(u_long **)(*(u_long *)((u8 *)param_5 + 0x90) + 4) +
                 (*(s32 *)((u8 *)param_5 + 0x78) >> *(s32 *)((u8 *)param_5 + 0x88));
        bound = src + 8;
        tag = *otSlot;
        *(u_long *)prim = tag;
        *(prim + 3) = 9;
    copy_top:
        w0 = src[0];
        w1 = src[1];
        w2 = src[2];
        w3 = src[3];
        dst[0] = w0;
        dst[1] = w1;
        dst[2] = w2;
        dst[3] = w3;
        src += 4;
        dst += 4;
        if (src != bound) goto copy_top;
        maskedAddr = (u_long)param_3 & 0xFFFFFF;
        param_3 += 10;
        w1 = src[1];
        dst[0] = src[0];
        dst[1] = w1;
        *otSlot = maskedAddr;

    next:
        param_4--;
        rec += 0x1C;
        } while (param_4 != 0);
    }
    return param_3;
}
#endif
