#ifndef GTE_H
#define GTE_H

/*
 * Restricted inline-asm layer for the GTE/COP2 class — the ONLY sanctioned
 * use of __asm__ in matched code, and only in functions on the whitelist in
 * docs/gte-policy.md. Macro names mirror PsyQ's INLINE_N.H, the original
 * SDK's own inline-GTE layer: these operations never had a C spelling in the
 * original source either.
 */

#define gte_ldDQA(x) __asm__ volatile("ctc2\t%0, $27" : : "r"(x))
#define gte_ldDQB(x) __asm__ volatile("ctc2\t%0, $28" : : "r"(x))

/* ---- COP2 data moves (native mnemonics) ---- */

/* Load 3 vertices into VXY0/VZ0, VXY1/VZ1, VXY2/VZ2 from 3 SVECTOR pointers. */
#define gte_ldv3(r0, r1, r2)                                                   \
    __asm__ volatile("lwc2\t$0, 0(%0);lwc2\t$1, 4(%0);"                        \
                     "lwc2\t$2, 0(%1);lwc2\t$3, 4(%1);"                        \
                     "lwc2\t$4, 0(%2);lwc2\t$5, 4(%2)"                         \
                     : : "r"(r0), "r"(r1), "r"(r2))

/* Load the RGB/code word (C2 data reg 6) from memory. */
#define gte_ldrgb(mem) __asm__ volatile("lwc2\t$6, %0" : : "m"(mem))

/* Store RGB2 (C2 data reg 22, the last depth-cued colour) to memory. */
#define gte_strgb(mem) __asm__ volatile("swc2\t$22, %0" : "=m"(mem))

/* Store SXY0/SXY1/SXY2 (C2 data regs 12/13/14) into a POLY_F3 packet's
 * three screen-XY fields (offsets 8/0xC/0x10). */
#define gte_stsxy3_f3(p)                                                       \
    __asm__ volatile("swc2\t$12, 8(%0);swc2\t$13, 0xC(%0);swc2\t$14, 0x10(%0)" \
                     : : "r"(p))

/* Read the FLAG control register (C2 control reg 31) into a CPU register. */
#define gte_stflg(r) __asm__ volatile("cfc2\t%0, $31" : "=r"(r))

/* Read SZ1/SZ2/SZ3 (C2 data regs 17/18/19) into CPU registers. */
#define gte_stsz3(r0, r1, r2)                                                  \
    __asm__ volatile("mfc2\t%0, $17;mfc2\t%1, $18;mfc2\t%2, $19"               \
                     : "=r"(r0), "=r"(r1), "=r"(r2))

/* Read MAC0 (C2 data reg 24) into a CPU register. */
#define gte_stmac0(r) __asm__ volatile("mfc2\t%0, $24" : "=r"(r))

/* ---- GTE command issues (as .word; mnemonic in the comment) ---- */
#define gte_rtpt()  __asm__ volatile(".word\t0x4A280030") /* RTPT  */
#define gte_nclip() __asm__ volatile(".word\t0x4B400006") /* NCLIP */
#define gte_dpcs()  __asm__ volatile(".word\t0x4A780010") /* DPCS  */

#endif /* GTE_H */
