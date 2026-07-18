#include "common.h"
#include "main.exe.h"
#include "gte.h"

/*
 * FUN_80058c70 (0x80058c70, 0x398 bytes) — a DrawTMD-family fast primitive
 * renderer (TMD primitive code 0x3d), the sibling of the switch arms in
 * FUN_80058a54: it transforms each quad's four vertices through the GTE
 * (RTPT + a backface NCLIP + a per-quad RTPS) and assembles a POLY_GT4
 * (code 0x3c, len 0xc) packet, then hands the working state to FUN_80057b80.
 * Compiled-style GTE function under the restricted gte.h policy
 * (docs/gte-policy.md).
 *
 * STATUS: NON_MATCHING — 26 of 920 byte values differ (correct length,
 * 230/230 instructions). Everything semantic is exact: the whole loop, the
 * store tail, the frame (72), every register role (s0=pkt/param_7, s1=puVar4,
 * s2=param_2, s3=local_38, s4=param_4, s5=iVar3 via `move s5,v0`,
 * s6/s7/s8=r0_00/r2/r1, a3=r0 with caller-save around the jal, t0=puVar5
 * spilled). The residual is ONE 7-insn prologue-leader ROTATION: two ADJACENT
 * groups, SWAPPED — s0-group [sw s0,32; lw s0,96(=pkt); lui/lw HWD0; li v0,4]
 * and s4-group [sw s4,48; move s4,a3(=param_4)]. Target = s0-group first; ours
 * = s4-group first.
 *
 * === THIS ROUND (RTL archaeology; permuter is a NON-LEVER — gte.h) ===========
 * SAME CLASS AS FUN_80057b80 (this function's own CALLEE), which is parked at
 * 8/3796 as a PROVEN-UNREACHABLE sched2 backward-scheduler LUID tie. The tie
 * here is ONE printable sched2 pick — `.i.sched2` ready list at T-34:
 *     ;; ready list at T-34: 25(1) 10(1) 6(1) 12(1) 22(1)   (12,22 block: loads)
 *                             now 25 10 6   -> PICKS 25.  Target needs 10.
 * uids: 25=`li v0,4`(*pkt=4), 10=`move s4,a3`(param_4 copy), 6=`move s2,a1`,
 * 12=`lw s0,96`(pkt), 22=HWD0. All priority 1, class 3 (moves/consts, latency
 * 1), no function unit -> pure INSN_LUID, DESCENDING. sched2 is BACKWARD, so the
 * highest-LUID ready insn is picked and lands LAST; 25 (highest) lands below 10,
 * so the s4-group emits FIRST = our rotation. To flip, move-s4 (uid10) must
 * out-rank 25/22/12, i.e. LUID(move-s4) > LUID(body). But move-s4 is param_4's
 * assign_parms ENTRY copy; the `.greg` chain is 6 -> 10 -> 12 -> ... -> 25, so
 * LUID(10) < LUID(body) is pinned by the ABI: a3 is copied to a callee-saved reg
 * AT ENTRY because the loop's jal to FUN_80057b80 forces param_4 off caller-saved
 * a3. (The 5-param matched siblings FUN_80059ff4/5a3cc/5961c/59b08 have NO jal in
 * their loop, keep the counter in a3/t-regs, never grow this leader, and so their
 * sched2 cannot embody the fix — confirmed by reading their target prologues.)
 *
 * REFUTED this round (each measured on the real build):
 *  - late loop counter `cnt=param_4; if(cnt) ... cnt--` -> cc1 COALESCES cnt into
 *    the entry copy: NO-OP, still 26 (== FUN_80057b80 round 10(C)).
 *  - narrow param_4 -> assign_parms deferral puts a TRUNCATION (sll/sra or andi)
 *    in that slot, not the target's bare `move` (FUN_80057b80 round 8 proof).
 *  - non-volatile param_5/param_6 -> LENGTH MISMATCH (+4): volatile is load-bearing.
 *  - permuter -> REFUSES at permute.py:985 (gte.h inline asm; -fno-builtin fix is
 *    irrelevant to this class). autorules --guided -> no improving path.
 *
 * autorules(default) DOES find `extern int HWD0[]`+`HWD0[0]` -> 22 (the earlier
 * header's "every source-order knob byte-inert" claim was WRONG). REJECTED as a
 * non-human local optimum: HWD0/VWD0 are plain adjacent int globals (0x800c6488/
 * 0x800c648c, symbols.main.exe.txt); the array form breaks the target's fused
 * `lui v1; lw v1,%lo(HWD0)(v1)` into a split `lui v0; ...; lw v1,..(v0)`
 * (structurally FURTHER from target), and VWD0[] scores 1004 — a codegen accident,
 * not the source type (human-source lever, docs owner directive).
 *
 * NOT CLOSED like FUN_80057b80 (whose two competitors were BOTH entry parm-copies,
 * making declaration order rigid). HERE the competitor is a BODY group and the
 * TARGET demonstrably reaches move-s4-after-body, so a C construct lifting the
 * entry-copy's LUID past the body MUST exist in the original; it was not found.
 * That single question — what makes param_4's a3-copy out-rank the *pkt=4 li4 in
 * .greg's chain order — is the whole remaining lever.
 * ============================================================================
 *
 * WHAT UNLOCKED THE 620 PARK (each independently measured; the park's
 * "failed" singles were pair-negatives, cookbook §4):
 *
 * 1. -fno-strength-reduce FOR THIS TU (Build.hs ccExtraFlags + permute.py
 *    CC_EXTRA_FLAGS, LIBMCRD precedent). The provenance is forced two ways:
 *    r0 lives in $a3 with caller-save spill/reload around the jal
 *    (sw a3,24(sp) in the delay slot / lw a3,24(sp) after), and
 *    CALLER_SAVE_PROFITABLE(refs,calls) = 4*calls < refs needs r0's refs
 *    loop-depth-weighted (flow.c REG_N_REFS += loop_depth) => real LOOP
 *    NOTES (do-while, not the park's goto hack); with notes, loop.c would
 *    reduce the 21-giv cursor class (combine_givs sums benefits) into a
 *    second induction register the target provably lacks — no C spelling
 *    denies a 21-member DEST_ADDR class, so SR was off. This one flag +
 *    do-while replaces the park's items 1-2 (the goto loop was compensating
 *    for the wrong root cause and killed the ref weighting).
 * 2. volatile u_long param_5/param_6 (CdaPlayXA parameter-qualifier
 *    precedent), read once each into uVar5/uVar7 at their statement
 *    positions. Plain stack parms' entry copies are pinned at the top and
 *    local-alloc colours them a1/a0 — p85->a0 is what evicted param_1 into
 *    the park's `move t0,a0`. Volatile keeps the reads at their use sites
 *    (lw v0,92/lw v1,88 late), a0 stays free, param_1's pseudo takes its
 *    copy-preference for $a0 and the entry move vanishes.
 * 3. pkt = param_7 local copy carrying ALL body uses. update_equiv_regs
 *    DOUBLES a REG_EQUIV-noted single-set parm's live length
 *    (local-alloc.c:1153), halving param_7's global priority (17880) below
 *    puVar4's (21120) -> wrong s0/s1. The unnoted local scores ~2x and wins
 *    s0 first; the copy coalesces away via the hard-reg copy preference.
 * 4. puVar5/local_38: BOTH defined above the guard (puVar5 = pkt+0x38;
 *    local_38 = puVar5), loop stores via local_38 (the copy -> s3), call arg
 *    = puVar5 (the original -> spilled 16(sp), reload-inherited move s3,t0 +
 *    sw t0,16(sp) in the beqz delay slot, lw a0,16(sp) at the call).
 *    This is the park's two "failed" singles applied JOINTLY.
 * 5. iVar4 = 0x3c named once (li v0,60), stored to +0x53 through it, and
 *    iVar3 = iVar4 inside the guard -> the target's `move s5,v0` (a real
 *    register copy, not a rematerialised li).
 * 6. The gte address-argument scratch reuse: uVar3/uVar8/uVar7 (and their
 *    disjoint second lives) host the stsxy3/stsz4 address temps as OWN
 *    STATEMENTS (in-macro-arg assignments get renamed by combine's
 *    new-pseudo path — measured, +8 bytes): uVar3 = pkt+0x24 (a1, also the
 *    packet-word life), uVar8 = pkt+0x23 / pkt+0x2a (a0 pair), uVar7 =
 *    pkt+0x30 (v1, also the param_5 read). Multi-set kills both
 *    move_movables hoisting (the +144/+168 temps hoisted+spilled otherwise;
 *    loop.c runs before combine so the kill survives the rename) and the
 *    volatile read's birthing bump.
 *
 * Falsify the remaining 26: see the "THIS ROUND" block above — the tie is pure
 * INSN_LUID in sched2 (NOT the potential_hazard/INSN_TICK an earlier header
 * guessed), and the only lever is lifting param_4's a3-entry-copy LUID above the
 * *pkt=4 / HWD0 / pkt-load body leaders in .greg's chain order.
 */

extern int HWD0;
extern int VWD0;
extern void FUN_80057b80(u_long *outv, u_long *packet, int mode);

#ifndef NON_MATCHING
INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/FUN_80058c70", FUN_80058c70);
#else
u_long *FUN_80058c70(u_short *param_1, u_long param_2, u_long *param_3, int param_4,
                     volatile u_long param_5, volatile u_long param_6, u_long *param_7)
{
    int iVar1;
    int iVar2;
    u_long uVar3;
    int iVar3;
    int iVar4;
    u_long uVar5;
    u_long *pkt;
    u_long uVar7;
    u_long uVar8;
    SVECTOR *r0;
    u_short *puVar4;
    u_long *puVar5;
    u_char uVar6;
    SVECTOR *r0_00;
    SVECTOR *r2;
    SVECTOR *r1;
    u_long *local_38;

    pkt = param_7;
    iVar1 = HWD0;
    *pkt = 4;
    puVar5 = pkt + 0x38;
    iVar2 = VWD0;
    local_38 = puVar5;
    *(short *)(pkt + 0xd) = (short)(iVar1 / 2);
    *(short *)((int)pkt + 0x36) = (short)(iVar2 / 2);
    uVar5 = param_6;
    uVar7 = param_5;
    uVar3 = *(u_long *)(uVar5 + 4);
    pkt[8] = 0x96;
    *(u_char *)((int)pkt + 0x4f) = 0xc;
    iVar4 = 0x3c;
    pkt[3] = uVar7;
    pkt[5] = (u_long)param_3;
    *(u_char *)((int)pkt + 0x53) = iVar4;
    pkt[4] = uVar3;
    if (param_4 != 0) {
        r0 = (SVECTOR *)(pkt + 0x20);
        r1 = (SVECTOR *)(pkt + 0x26);
        r2 = (SVECTOR *)(pkt + 0x2c);
        r0_00 = (SVECTOR *)(pkt + 0x32);
        iVar3 = iVar4;
        puVar4 = param_1 + 5;
        do {
            *(short *)(pkt + 0x20) = *(u_short *)(puVar4[0xd] * 8 + param_2);
            *(short *)((int)pkt + 0x82) = *(u_short *)(puVar4[0xd] * 8 + param_2 + 2);
            *(short *)(pkt + 0x21) = *(u_short *)(puVar4[0xd] * 8 + param_2 + 4);
            *(short *)(pkt + 0x26) = *(u_short *)(puVar4[0xe] * 8 + param_2);
            *(short *)((int)pkt + 0x9a) = *(u_short *)(puVar4[0xe] * 8 + param_2 + 2);
            *(short *)(pkt + 0x27) = *(u_short *)(puVar4[0xe] * 8 + param_2 + 4);
            *(short *)(pkt + 0x2c) = *(u_short *)(puVar4[0xf] * 8 + param_2);
            *(short *)((int)pkt + 0xb2) = *(u_short *)(puVar4[0xf] * 8 + param_2 + 2);
            *(short *)(pkt + 0x2d) = *(u_short *)(puVar4[0xf] * 8 + param_2 + 4);
            *(short *)(pkt + 0x32) = *(u_short *)(puVar4[0x10] * 8 + param_2);
            *(short *)((int)pkt + 0xca) = *(u_short *)(puVar4[0x10] * 8 + param_2 + 2);
            *(short *)(pkt + 0x33) = *(u_short *)(puVar4[0x10] * 8 + param_2 + 4);
            *local_38 = (u_long)r0;
            local_38[1] = (u_long)r1;
            local_38[2] = (u_long)r2;
            local_38[3] = (u_long)r0_00;
            gte_ldv3(r0, r1, r2);
            gte_rtpt();
            *(short *)(pkt + 0x25) = puVar4[-3];
            *(short *)(pkt + 0x2b) = puVar4[-1];
            uVar8 = (u_long)(pkt + 0x23);
            gte_stsxy3((u_long *)uVar8, pkt + 0x29, pkt + 0x2f);
            gte_nclip();
            *(short *)(pkt + 0x31) = puVar4[1];
            *(short *)(pkt + 0x37) = puVar4[3];
            gte_stopz(pkt + 6);
            if (0 < (int)pkt[6]) {
                gte_ldv0(r0_00);
                gte_rtps();
                pkt[0x22] = *(u_long *)(puVar4 + 5);
                uVar6 = (u_char)iVar3;
                *(u_char *)((int)pkt + 0x8b) = uVar6;
                pkt[0x28] = *(u_long *)(puVar4 + 7);
                *(u_char *)((int)pkt + 0xa3) = uVar6;
                pkt[0x2e] = *(u_long *)(puVar4 + 9);
                *(u_char *)((int)pkt + 0xbb) = uVar6;
                pkt[0x34] = *(u_long *)(puVar4 + 0xb);
                *(u_char *)((int)pkt + 0xd3) = uVar6;
                gte_stsxy(pkt + 0x35);
                uVar3 = (u_long)(pkt + 0x24);
                uVar8 = (u_long)(pkt + 0x2a);
                uVar7 = (u_long)(pkt + 0x30);
                gte_stsz4((u_long *)uVar3, (u_long *)uVar8, (u_long *)uVar7, pkt + 0x36);
                *(short *)((int)pkt + 0x5a) = puVar4[-2];
                *(short *)((int)pkt + 0x66) = *puVar4;
                FUN_80057b80(puVar5, pkt, 0);
            }
            param_4 = param_4 + -1;
            puVar4 = puVar4 + 0x16;
        } while (param_4 != 0);
    }
    return (u_long *)pkt[5];
}
#endif
