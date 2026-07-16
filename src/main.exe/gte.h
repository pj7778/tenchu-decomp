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

#endif /* GTE_H */
