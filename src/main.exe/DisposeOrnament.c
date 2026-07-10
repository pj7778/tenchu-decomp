#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * void DisposeOrnament(struct OrnamentType *objp);
 *     3DCTRL.C:510, 3 src lines, frame 24 bytes, saved-reg mask 0x80000000
 *
 * Original parameters and locals (the demo build's register allocation may
 * differ from retail, but the COUNT and TYPES drive cc1's codegen and carry
 * over). A repeated name is a nested-block scope, not a duplicate:
 *     param $a0       struct OrnamentType * objp
 * END PSX.SYM */

/* DisposeOrnament (0x80018718) — byte-identical clone of DisposeModel (tools/clonematch.py). */

extern void vfree(void *p);

void DisposeOrnament(void *objp)
{
    if (objp != 0)
    {
        vfree(objp);
    }
}
