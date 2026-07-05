#include "common.h"
#include "main.exe.h"

/*
 * UpdateOrnament (0x800187bc) — like UpdateCoordinate, but OrnamentType has no
 * own `rotate` SVECTOR: build one from UnitVector (the identity SVECTOR, see
 * InsertConflict.c) with vy overridden to `ry`, then recompute the local
 * matrix and clear the GsCOORDINATE2 dirty flag.
 *
 * Matching notes (docs/matching-cookbook.md):
 *  - `local = UnitVector;` is the align-2 SVECTOR struct-copy idiom
 *    (lwl/lwr + swl/swr pairs, see InsertConflict's `.offset`/`.size`);
 *    the following `local.vy = ry;` is a plain `sh` overwriting the copied
 *    half, executed AFTER the copy (matches asm order).
 *  - OrnamentType (Ghidra: `{ GsCOORDINATE2 locate; GsDOBJ2 object; }`) isn't
 *    shared via item.h yet; declared locally here with just the accessed
 *    `locate` field to keep this function's edits self-contained.
 */
typedef struct
{
    GsCOORDINATE2 locate;        /* 0x00 */
} OrnamentType;

extern MATRIX *RotMatrixYXZ(SVECTOR *r, MATRIX *m);
extern SVECTOR UnitVector;

void UpdateOrnament(OrnamentType *objp, short ry)
{
    SVECTOR local;

    local = UnitVector;
    local.vy = ry;
    RotMatrixYXZ(&local, &objp->locate.coord);
    objp->locate.flg = 0;
}
