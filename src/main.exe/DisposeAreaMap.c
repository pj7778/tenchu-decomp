#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * void DisposeAreaMap(unsigned long *area);
 *     CONFLICT.C:74, 4 src lines, frame 24 bytes, saved-reg mask 0x80000000
 *
 * Original parameters and locals (the demo build's register allocation may
 * differ from retail, but the COUNT and TYPES drive cc1's codegen and carry
 * over). A repeated name is a nested-block scope, not a duplicate:
 *     param $a0       unsigned long * area
 *
 * Globals it touches, as the original declared them:
 *     extern unsigned long *GlobalAreaMap;
 * END PSX.SYM */

/*
 * DisposeAreaMap (0x8001ab28) — free an area-map object; if none was given,
 * fall back to the cached GlobalAreaMap (clearing the cache) instead. Unlike
 * Ghidra's flattened rendering (which loads GlobalAreaMap unconditionally at
 * entry), the asm only loads it inside the `area == 0` path — a nested if,
 * not a hoisted read (trust the asm's branch shape over Ghidra's SSA-style
 * early temp).
 */
extern void vfree(void *p);
extern void *GlobalAreaMap;

void DisposeAreaMap(void *area)
{
    if (area == 0) {
        void *tmp = GlobalAreaMap;
        if (tmp != 0) {
            area = tmp;
            GlobalAreaMap = 0;
        }
    }
    vfree(area);
}
