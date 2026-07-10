#include "common.h"
#include "main.exe.h"

/*
 * STATUS: NON_MATCHING — no C construct can reach the target bytes at all
 * (see below); the draft below is a semantically-faithful reimplementation,
 * not a byte-match attempt. Residual: 220 of 220 bytes (whole function).
 *
 * FUN_8001c730 (0x8001c730, 0xdc bytes) — "rotate a vector by a packed 3x3
 * matrix, then add a weighted offset": loads `v` (the SVECTOR at param_3)
 * as the GTE's V0, loads a 3x3 rotation matrix R from `pp` (columns spread
 * across two pointed-to short arrays plus three direct fields — no proven
 * struct name; see below), runs one GTE `mvmva` (mx=rotation, v=V0, cv=none,
 * sf=1) to get `R * v` in MAC1/2/3, and adds `(v->pad * pp->t[i]) >> 12` to
 * each component before narrowing to the output `short[3]`. `v->pad` reads
 * as a per-vertex blend weight (matches the "GetConflictResult-style pooled
 * struct" callers elsewhere that carry a `pad` blend factor); `pp->t[0..2]`
 * looks like a translation/offset vector scaled by that same weight — this
 * has the shape of a skeletal-skinning "rotate by joint matrix, blend in a
 * weighted offset" helper, though no candidate name exists in
 * reference/psxsym-candidates.tsv and it is not in the demo's PSX.SYM.
 *
 * `pp`'s layout (no proven view; the two pointer fields are genuinely
 * pointers to short[3]s, not adjacent inline data — confirmed by the raw
 * `.s`: `lw $v1,4($a1)` then `lhu $t0,0($v1)`, i.e. a POINTER dereference,
 * not a struct-offset access):
 *     short *m0;   // +0x00: [R11, R21, R31]  (this array's elements 0,1,2)
 *     short *m1;   // +0x04: [R12, R22, R32]
 *     short  m13;  // +0x08: R13
 *     short  m23;  // +0x0A: R23
 *     s32    m33;  // +0x0C: R33 (a full word; the GTE control register only
 *                  //        reads the low 16 bits)
 *     short  t0;   // +0x10
 *     short  t1;   // +0x12
 *     short  t2;   // +0x14
 * Derived by tracing every `ctc2`/`lhu`/`lh` operand's CONCAT halves against
 * the GTE's documented control-register bit layout (RnmRnm packs the LOW
 * register in bits[15:0], the HIGH in bits[31:16]).
 *
 * WHY THIS CANNOT BYTE-MATCH: the function's only floating operation is a
 * single GTE `mvmva` command opcode (confirmed in the raw `.s`:
 * `mvmva 1, 0, 0, 3, 0`). Per docs/matching-cookbook.md's "PS1 GTE command
 * opcodes" section, our pinned cc1 has NO C construct that emits a GTE
 * *command* opcode (only the COP2 *data-move* mnemonics — lwc2/swc2/mfc2/
 * mtc2/ctc2/cfc2 — assemble from plain C via the data-register accesses);
 * reaching `mvmva` itself needs inline asm, which is the same open
 * project-wide policy question blocking GetPad/DrawTMD/ArrangeLocalMatrix.
 * The draft below computes the identical VALUES (a plain 3x3 dot-product
 * matrix multiply plus the weighted-offset term, including the target's
 * `>> 12` on the offset term being a LOGICAL shift — Ghidra's `(uint)(...)
 * >> 0xc` and the raw `.s`'s `srl` both confirm this, vs. the arithmetic
 * `sra` an `s32` shift would otherwise need) but will never assemble to the
 * same bytes as the real `mvmva`.
 */
#ifndef NON_MATCHING
INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/FUN_8001c730", FUN_8001c730);
#else
typedef struct
{
    short *m0; /* +0x00: [R11, R21, R31] */
    short *m1; /* +0x04: [R12, R22, R32] */
    short m13; /* +0x08 */
    short m23; /* +0x0A */
    s32 m33;   /* +0x0C */
    short t0;  /* +0x10 */
    short t1;  /* +0x12 */
    short t2;  /* +0x14 */
} Unk_8001c730_Param;

void FUN_8001c730(short *out, Unk_8001c730_Param *pp, SVECTOR *v)
{
    s32 r11 = pp->m0[0], r12 = pp->m1[0], r13 = pp->m13;
    s32 r21 = pp->m0[1], r22 = pp->m1[1], r23 = pp->m23;
    s32 r31 = pp->m0[2], r32 = pp->m1[2], r33 = pp->m33;
    s32 vx = v->vx, vy = v->vy, vz = v->vz;
    s32 weight = v->pad;
    s32 mac1 = (r11 * vx + r12 * vy + r13 * vz) >> 12;
    s32 mac2 = (r21 * vx + r22 * vy + r23 * vz) >> 12;
    s32 mac3 = (r31 * vx + r32 * vy + r33 * vz) >> 12;

    out[0] = (s16)(mac1 + (s32)((u32)(weight * pp->t0) >> 12));
    out[1] = (s16)(mac2 + (s32)((u32)(weight * pp->t1) >> 12));
    out[2] = (s16)(mac3 + (s32)((u32)(weight * pp->t2) >> 12));
}
#endif
