#include "common.h"
#include "main.exe.h"
#include "item.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * void UpdateCoordinate2(struct ModelType *dim);
 *     3DCTRL.C:212, 4 src lines, frame 24 bytes, saved-reg mask 0x80010000
 *
 * Original parameters and locals (the demo build's register allocation may
 * differ from retail, but the COUNT and TYPES drive cc1's codegen and carry
 * over). A repeated name is a nested-block scope, not a duplicate:
 *     param $a0       struct ModelType * dim
 * END PSX.SYM */

extern MATRIX *RotMatrix(SVECTOR *r, MATRIX *m);

void UpdateCoordinate2(ModelType *dim)
{
    RotMatrix(&dim->rotate, &dim->locate.coord);
    dim->locate.flg = 0;
}
