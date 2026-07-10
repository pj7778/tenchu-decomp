#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * void DisposeModel(struct ModelType *objp);
 *     3DCTRL.C:312, 2 src lines, frame 24 bytes, saved-reg mask 0x80000000
 *
 * Original parameters and locals (the demo build's register allocation may
 * differ from retail, but the COUNT and TYPES drive cc1's codegen and carry
 * over). A repeated name is a nested-block scope, not a duplicate:
 *     param $a0       struct ModelType * objp
 * END PSX.SYM */

/* DisposeModel (0x800184f8) — free a model object if non-null. Byte-identical
 * to DisposeOrnament (cloned via tools/clonematch.py). */

extern void vfree(void *p);

void DisposeModel(void *objp)
{
    if (objp != 0)
    {
        vfree(objp);
    }
}
