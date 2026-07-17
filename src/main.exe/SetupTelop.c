#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * void SetupTelop(unsigned char *telop);
 *     CHRANIM.C:372, 44 src lines, frame 560 bytes, saved-reg mask 0x801f0000 (DEMO build -- see below)
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
 *     param $s4       unsigned char * telop
 *     reg   $a2       short * font
 *     stack sp+16     short [16][16] bitmap
 *     reg   $s0       short n
 *     reg   $t0       short u
 *     reg   $t1       short v
 *     stack sp+528    struct RECT rect
 *
 * Globals it touches, as the original declared them:
 *     extern struct POLY_FT4 TelopP;
 *     extern struct tag_TItem items[30];
 * END PSX.SYM */

/* STATUS: NON_MATCHING — 11 of 1076 bytes differ (10 insns, all in loop 1).
 *
 * Exact instruction sequence; the ONLY residual is a 3-cycle register
 * rotation in the bitmap-fill loop:
 *     col (inner var)  ours $a1  <->  target $t0
 *     bits             ours $t1  <->  target $a1
 *     fill_white       ours $t0  <->  target $t1
 *
 * DIAGNOSIS (global.c allocno_compare, verified against all 25 allocnos of
 * this function via tools/regalloc.py):
 *     priority = floor_log2(n_refs) * n_refs / live_length * 10000
 * where n_refs is LOOP-DEPTH-WEIGHTED (each RTL mention counts its depth).
 * Measured here: col 16/30 -> 21333, font 19/43 -> 17674, v 35/111 -> 15765,
 * fill_white 19/80 -> 9500, bits 7/19 -> 7368. Allocation walks that order and
 * takes the lowest free reg (MIPS REG_ALLOC_ORDER v0,v1,a0,a1,a2,a3,t0,t1...),
 * which reproduces our a1,a2,a3,t0,t1 exactly.
 *
 * The target's registers require the order  bits > font > v > col > fill_white.
 * So bits must outrank font: >17674 needs n_refs >= 12 at live_length 19.
 * bits has exactly TWO RTL mentions -- def at loop depth 3 (+3) and one use at
 * depth 4 (+4) = 7, and that is forced by the (already byte-identical) insn
 * sequence. Every extra mention costs an instruction. The only other way to
 * claim $a1 from last place is a hard-reg preference (regs_someone_prefers),
 * and gcc-2.8.1 records those ONLY from a pseudo<->hard-reg copy, i.e. a call;
 * loop 1 contains none. Both levers are therefore out of reach from C.
 *
 * MEASURED, so the next lane need not re-derive (all rebuilt + matchdiff'd):
 *   - col == u merged (PSX.SYM lists one `u`):  11 -> 38. Refuted. Target's
 *     loop-1 inner var and loop-2 `u` share $t0 but are NOT one variable --
 *     merging raises n_refs, so the pseudo allocates EARLIER and takes a LOWER
 *     reg ($a2), displacing font.
 *   - `*pixel = 0x7fff` literal instead of fill_white: LENGTH 1084 != 1076.
 *     Refuted -- fill_white must be a real local despite PSX.SYM's 7-local list.
 *   - dropping `final_u = fill_white`: 11 -> 33, but it DOES move fill_white
 *     t0->t1 (correct!) by removing one depth-4 ref. This is the one verified
 *     lever on this residual; it regresses the tail because final_u is reused
 *     at the GetTPage block.
 *   - byte-swap inlined into the `if` (as Ghidra renders it): 11 -> 13. loop.c
 *     still hoists it to 0x80057384, and the rotation is UNCHANGED -- an
 *     explicit variable and a loop-hoisted temp score identically. It also
 *     shows the target's srl-before-sll needs the two-statement form
 *     (`bits = raw_bits >> 8; bits |= raw_bits << 8;`); the fused expression
 *     flips the operand order.
 *   - tools/autorules.py: no improving edit among 73 candidates.
 *   - tools/permute.py, bounded 300s -j4 --stop-on-zero: no improvement.
 *
 * ROUND 2 (base re-verified: 11/1076, matches this file's STATUS exactly).
 * Re-ran the ladder end to end (reghist, autorules, ONE bounded permuter,
 * cc1says, regalloc --order/--compare, rtlguide, autorules --guided) and
 * closed the "premise is wrong" gap the previous park flagged, without
 * finding a lever:
 *   - reghist: delta sum 0, only $a1/$t0 counts differ (+4/-4) -- confirms
 *     pure-rotation, not a mega-pseudo; consistent with the diagnosis above.
 *   - tools/autorules.py: re-run fresh, 0 of 63 (ruleset has not grown a
 *     lever for this shape since the last park).
 *   - tools/autorules.py --guided (rtlguide's mechanical search, which adds
 *     regalloc-specific rules absent from the plain sweep -- loop-fence,
 *     nested-loop-fence, paired-loop-fence, disjoint-local-alias,
 *     allocation-donor-fence, deferred-global-capture, shared-result-return):
 *     0 of 160 compiled candidates.
 *   - tools/permute.py, bounded 240s -j4 --stop-on-zero: 17831 iterations,
 *     authoritative rescore still base.c at 11/11/1076. Two independent
 *     bounded runs (this one and the prior 300s run) now agree: not
 *     permuter-reachable, not a crashed/short search either.
 *   - THE CONFLICT GRAPH (dump_conflicts, global.c:1666-1676, printed BEFORE
 *     coloring -- the STATIC interference graph, not a priority number):
 *     col/font/bits/v mutually conflict (a full 4-clique), but fill_white
 *     conflicts with v ONLY -- it does NOT conflict with col, font, or bits.
 *     This means fill_white's own register is decided entirely by ITS OTHER
 *     roommates-of-roommates (p197/p196/p193 block a0-a3 for it indirectly;
 *     p98 or a LOCAL pseudo blocks $t0), never by a direct fight with
 *     col/font/bits. It also means the required order is a strict
 *     tournament over the 4-clique -- bits, font, v, col MUST be colored in
 *     that exact relative order to land (a1,a2,a3,t0), full stop; there is
 *     no partial-order slack to exploit.
 *   - regalloc.py --compare/--between (independent confirmation of the
 *     priority-table arithmetic above, via the tool rather than by hand):
 *     `p179 > p87` needs +5 weighted refs (matches "n_refs >= 12" exactly).
 *     `p85 > p86` (i.e. col dropping below v) needs col to shed only 1
 *     weighted ref (16->15 crosses the floor_log2 4->3 cliff, 21333->15000).
 *     But shedding col ALONE is insufficient and moves the WRONG variable
 *     into $a1: with col demoted, font becomes first-processed in the
 *     4-clique and takes $a1 itself (verified by hand-simulating the
 *     4-clique in priority order) -- bits, font and col do not have
 *     independent levers; only the bits-vs-font gap actually gates the
 *     rotation, and font can shed at most ~2 refs (19->17, priority 15813)
 *     before it would drop below v and swap ITS OWN register with v's --
 *     so the effective combined budget is still ~+5 weighted refs, however
 *     split. Given loop 1's instructions are already exact, every such
 *     mention is forced by a real instruction (flow.c:1969/2404: REG_N_REFS
 *     += loop_depth per actual SET/USE rtx in the final RTL) -- there is no
 *     "free" mention CSE won't fold away before reg-scan runs.
 *   - find_reg's preference mechanism (global.c:900-1037, read via
 *     tools/ccsrc.py rather than recalled): `hard_reg_copy_preferences` /
 *     `hard_reg_preferences` can only PROMOTE a pseudo to a DIFFERENT
 *     register that is already outside `used` (i.e. still legitimately
 *     available to it) -- it can never override a real conflict-based
 *     exclusion. bits has zero recorded preferences (no "179 preferences:"
 *     line in the raw .greg dump), consistent with crossing 0 calls in loop
 *     1. This closes off preference-based promotion as a second lever, not
 *     just the priority-race one the previous park already ruled out.
 *   - Retested the ONE verified lever (dropping `final_u = fill_white`) with
 *     a CLEAN substitution (`*pixel = fill_white;` directly, not leaving
 *     final_u read uninitialized) to make sure the prior 11->33 wasn't an
 *     artifact of a botched edit: reproduces 11->33 exactly. Reading the
 *     allocation after this edit shows the mechanism is NOT what the prior
 *     note guessed -- fill_white does not cleanly move to $t1; instead bits
 *     and fill_white COLLAPSE onto the SAME register ($t0, legally, since
 *     they don't conflict), col is still wrong in $a1, and a new 12-insn/
 *     16-byte cluster opens at the GetTPage tail (0x800575e8-0x80057628)
 *     where final_u's other, unrelated use lives. Confirms this edit is a
 *     genuine dead end, not a near-miss.
 *   - tools/rtlguide.py surfaces one loose thread for the next lane: its
 *     register-goal summary for $t0<->$t1 lists THREE priority figures
 *     (9500, 6750, 6666), not the two accounted for above (fill_white=9500,
 *     the unrelated p98=6750) -- there is a third contender at priority
 *     ~6666 this park has not identified (likely a LOCAL allocno under
 *     local-alloc's different quantity-ordering formula, e.g. a roommate of
 *     p356/t0, since global-alloc's 25-allocno table has no entry at 6666).
 *     Not chased further this round; may or may not matter to the rotation.
 *
 * Net: two independently-run lanes, via different methods (hand priority
 * arithmetic vs tool-driven conflict-graph + preference-mechanism reading),
 * now agree the bits-vs-font ordering is the single gating constraint, that
 * closing it costs ~5 weighted refs with no free source of them, and that
 * neither the plain nor guided mechanical sweep nor two bounded permuter
 * runs (300s, 240s) find a way around it. Still park-worthy, not proven
 * mathematically impossible (a restructuring outside this park's 5-variable
 * framing could exist), but the priority-race framing itself is no longer
 * an open question -- only a from-scratch alternate decomposition remains
 * unexplored, and it risks the already-exact instruction sequence.
 */
#ifndef NON_MATCHING
INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/SetupTelop", SetupTelop);
#else

typedef struct
{
    s16 x, y, w, h;
} RECT;

typedef struct
{
    u8 pad[3];
    u8 len;
} TelopTag;

typedef struct
{
    TelopTag tag;
    u8 r0, g0, b0, code;
    s16 x0, y0;
    u8 u0, v0;
    u16 clut;
    s16 x1, y1;
    u8 u1, v1;
    u16 tpage;
    s16 x2, y2;
    u8 u2, v2;
    u16 pad1;
    s16 x3, y3;
    u8 u3, v3;
    u16 pad2;
} TelopPoly;

extern TelopPoly TelopP;
extern u16 D_8008F078[];

extern int ClearImage(RECT *rect, u8 r, u8 g, u8 b);
extern int DrawSync(int mode);
extern s16 *Krom2RawAdd(u32 code);
extern int LoadImage(RECT *rect, u_long *pixels);
extern void *memset(void *dst, int value, u32 size);

void SetupTelop(u8 *telop, short line)
{
    s16 bitmap[16][16];
    RECT rect;
    s16 n;
    s16 u;
    s16 v;
    s16 col;
    s16 *font;
    s16 *pixel;
    s32 pixel_offset;
    s16 bits;
    u16 raw_bits;
    s16 north;
    s16 fill_white;
    s16 outline_white;
    s32 scaled_y;
    s32 line_y;
    s32 signed_v;
    s32 final_u;
    s32 final_v;
    s32 final_v2;

    TelopP.u1 = 0;
    TelopP.u0 = 0;
    if ((*telop & 0x80) != 0 && (telop[2] & 0x80) != 0)
    {
        scaled_y = ((s32)line << 0x10) >> 0xc;
        rect.x = 0x300;
        rect.y = 0x1f0 - scaled_y;
        rect.w = 0x100;
        rect.h = 0xf;
        line_y = scaled_y;
        ClearImage(&rect, 0, 0, 0);
        DrawSync(0);
        rect.w = 0x10;
        rect.h = 0xf;

        do
        {
            if (*telop == 0)
            {
                TelopP.u1 = 0;
                TelopP.u0 = 0;
                return;
            }
        } while (0);

        n = 0;
        while (1)
        {
            if (n >= 0x20)
            {
                break;
            }

            do
            {
                do
                {
                    do
                    {
                        do
                        {
                            do
                            {
                                if (telop[n] == 0x81 && telop[n + 1] == 0x99)
                                {
                                    font = D_8008F078;
                                }
                                else
                                {
                                    font = Krom2RawAdd((telop[n] << 8) | telop[n + 1]);
                                }
                            } while (0);
                        } while (0);
                    } while (0);
                } while (0);
            } while (0);

            if (font != (s16 *)-1)
            {
                fill_white = 0x7fff;
                v = 0;
                do
                {
                    raw_bits = font[v];
                    bits = raw_bits >> 8;
                    bits |= raw_bits << 8;
                    col = 0;
                    do
                    {
                        do
                        {
                            pixel_offset = (15 - col) * 2 + v * 0x20;
                        } while (0);
                        final_u = fill_white;
                        pixel = (s16 *)((u8 *)bitmap + pixel_offset);
                        if ((bits >> col) & 1)
                        {
                            *pixel = final_u;
                        }
                        else
                        {
                            *pixel = 0;
                        }
                        col++;
                    } while (col < 16);
                    do
                    {
                        do
                        {
                            v++;
                        } while (0);
                    } while (0);
                } while (v < 15);

                outline_white = 0x7fff;
                u = 1;
                v = 1;
                do
                {
                    if (bitmap[v][u] == 0)
                    {
                        north = bitmap[v - 1][u];
                        if ((north == outline_white && bitmap[v][u - 1] == outline_white) ||
                            (bitmap[v][u - 1] == outline_white && bitmap[v + 1][u] == outline_white) ||
                            (bitmap[v + 1][u] == outline_white && bitmap[v][u + 1] == outline_white) ||
                            (bitmap[v][u + 1] == outline_white && north == outline_white))
                        {
                            bitmap[v][u] = 0x1ce7;
                        }
                    }
                    do
                    {
                        u++;
                    } while (0);
                    signed_v = v;
                    if (u >= 15)
                    {
                        u = 1;
                        v = signed_v + 1;
                    }
                    else
                    {
                        v = signed_v;
                    }
                } while (v < 14);

                LoadImage(&rect, (u_long *)bitmap);
                DrawSync(0);
                rect.x += rect.w;
            }

            n += 2;
            if (telop[n] == 0)
            {
                break;
            }
        }

        memset(&TelopP, 0xff, sizeof(TelopP));
        final_v = 0xf0 - line_y;
        final_v2 = (u8)rect.h + final_v;
        final_u = (u16)rect.x - 0x301;
        TelopP.tag.len = 9;
        TelopP.code = 0x2c;
        TelopP.u2 = 0;
        TelopP.u0 = 0;
        TelopP.v1 = final_v;
        TelopP.v0 = final_v;
        TelopP.u3 = final_u;
        TelopP.u1 = final_u;
        TelopP.v3 = final_v2;
        TelopP.v2 = final_v2;
        TelopP.tpage = GetTPage(2, 0, 0x300,
                               0x1f0 - (s16)line_y);
    }
}

#endif

// triage: HARD — 269 insns, 4 loop, frame 0x230, 6 callees, ~0.04 to BriefingAndInventorySelectionScreen
// likely-relevant cookbook sections:
//   - Loops: 4 back-edge(s) — for/while/do vs goto shape
//   - Stack objects: 0x230 frame — buffer casts / spills

// Ghidra decompilation (reference — turn this into matching C,
// then drop the INCLUDE_ASM above):
//
//
// void SetupTelop(uchar *telop)
//
// {
//   short sVar1;
//   short sVar2;
//   undefined2 *puVar3;
//   int iVar4;
//   int iVar5;
//   int iVar6;
//   int in_a1;
//   int iVar7;
//   undefined2 *puVar8;
//   int iVar9;
//   ushort uVar10;
//   int iVar11;
//   int iVar12;
//   u_long auStack_220 [128];
//   RECT local_20;
//
//   TelopP.u1 = '\0';
//   TelopP.u0 = '\0';
//   if (((*telop & 0x80) != 0) && ((telop[2] & 0x80) != 0)) {
//     iVar5 = (in_a1 << 0x10) >> 0xc;
//     local_20.x = 0x300;
//     sVar1 = (short)iVar5;
//     local_20.y = 0x1f0 - sVar1;
//     local_20.w = 0x100;
//     local_20.h = 0xf;
//     ClearImage(&local_20,'\0','\0','\0');
//     DrawSync(0);
//     local_20.w = 0x10;
//     local_20.h = 0xf;
//     if (*telop == '\0') {
//       TelopP.u1 = '\0';
//       TelopP.u0 = '\0';
//     }
//     else {
//       iVar12 = 0;
//       do {
//         sVar2 = (short)iVar12;
//         if (0x1f < sVar2) break;
//         if ((telop[sVar2] == 0x81) && ((telop + sVar2)[1] == 0x99)) {
//           puVar8 = &DAT_8008f078;
//         }
//         else {
//           puVar8 = (undefined2 *)Krom2RawAdd((uint)CONCAT11(telop[sVar2],(telop + sVar2)[1]));
//         }
//         iVar11 = 0;
//         if (puVar8 != (undefined2 *)0xffffffff) {
//           do {
//             iVar4 = 0;
//             uVar10 = puVar8[(short)iVar11];
//             iVar6 = 0;
//             do {
//               puVar3 = (undefined2 *)
//                        ((int)auStack_220 + (0xf - (iVar6 >> 0x10)) * 2 + (short)iVar11 * 0x20);
//               if (((int)(short)(uVar10 >> 8 | uVar10 << 8) >> (iVar6 >> 0x10 & 0x1fU) & 1U) == 0) {
//                 *puVar3 = 0;
//               }
//               else {
//                 *puVar3 = 0x7fff;
//               }
//               iVar4 = iVar4 + 1;
//               iVar6 = iVar4 * 0x10000;
//             } while (iVar4 * 0x10000 >> 0x10 < 0x10);
//             iVar11 = iVar11 + 1;
//           } while (iVar11 * 0x10000 >> 0x10 < 0xf);
//           iVar4 = 1;
//           uVar10 = 1;
//           iVar11 = 0x10000;
//           do {
//             iVar11 = iVar11 >> 0x10;
//             iVar9 = iVar11 * 2;
//             iVar7 = (int)(short)uVar10;
//             iVar6 = iVar7 * 0x20;
//             if (*(short *)((int)auStack_220 + iVar9 + iVar6) == 0) {
//               sVar2 = *(short *)((int)auStack_220 + iVar9 + (iVar7 + -1) * 0x20);
//               if (((((sVar2 == 0x7fff) &&
//                     (*(short *)((int)auStack_220 + (iVar11 + -1) * 2 + iVar6) == 0x7fff)) ||
//                    ((*(short *)((int)auStack_220 + (iVar11 + -1) * 2 + iVar6) == 0x7fff &&
//                     (*(short *)((int)auStack_220 + iVar9 + (iVar7 + 1) * 0x20) == 0x7fff)))) ||
//                   ((*(short *)((int)auStack_220 + iVar9 + (iVar7 + 1) * 0x20) == 0x7fff &&
//                    (*(short *)((int)auStack_220 + (iVar11 + 1) * 2 + iVar6) == 0x7fff)))) ||
//                  ((*(short *)((int)auStack_220 + (iVar11 + 1) * 2 + iVar6) == 0x7fff &&
//                   (sVar2 == 0x7fff)))) {
//                 *(undefined2 *)
//                  ((int)auStack_220 + ((iVar4 << 0x10) >> 0xf) + ((int)((uint)uVar10 << 0x10) >> 0xb)
//                  ) = 0x1ce7;
//               }
//             }
//             iVar4 = iVar4 + 1;
//             if (0xe < iVar4 * 0x10000 >> 0x10) {
//               iVar4 = 1;
//               uVar10 = uVar10 + 1;
//             }
//             iVar11 = iVar4 << 0x10;
//           } while ((short)uVar10 < 0xe);
//           LoadImage(&local_20,auStack_220);
//           DrawSync(0);
//           local_20.x = local_20.x + local_20.w;
//         }
//         iVar12 = iVar12 + 2;
//       } while (telop[iVar12 * 0x10000 >> 0x10] != '\0');
//       memset((uchar *)&TelopP,0xff,0x28);
//       TelopP.tag._3_1_ = 9;
//       TelopP._2 = 0xf0 - (char)iVar5;
//       TelopP.code = ',';
//       TelopP.u2 = '\0';
//       TelopP.u0 = '\0';
//       TelopP.u1 = (char)local_20.x + 0xff;
//       TelopP.v2 = (char)local_20.h + TelopP._2;
//       TelopP._3 = TelopP._2;
//       TelopP.u3 = TelopP.u1;
//       TelopP.v3 = TelopP.v2;
//       TelopP.tpage = GetTPage(2,0,0x300,0x1f0 - sVar1);
//     }
//   }
//   return;
// }
