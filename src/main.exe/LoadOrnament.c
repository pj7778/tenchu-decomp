#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * struct OrnamentType * LoadOrnament(unsigned long *adr);
 *     3DCTRL.C:471, 21 src lines, frame 32 bytes, saved-reg mask 0x80030000
 *
 * Original parameters and locals (the demo build's register allocation may
 * differ from retail, but the COUNT and TYPES drive cc1's codegen and carry
 * over). A repeated name is a nested-block scope, not a duplicate:
 *     param $a0       unsigned long * adr
 *
 * Globals it touches, as the original declared them:
 *     extern struct ModelType World;
 *     extern struct SVECTOR UnitVector;
 * END PSX.SYM */

/*
 * LoadOrnament (0x80018644, 0x90 bytes) - near-twin of
 * CreateCloneOrnament.c (same OrnamentType/WorldType locals, same
 * GsInitCoordinate2/RotMatrixYXZ initialization tail): allocate and
 * initialize an OrnamentType, hook it into World's hierarchy, build an
 * identity matrix at the origin - and, when `adr` is non-null, wire the
 * model data in (GsMapModelingData on the modeling-data block, then
 * GsLinkObject4 to attach the object list) instead of
 * CreateCloneOrnament's plain tmd-pointer copy from an existing clone
 * source.
 */
typedef struct
{
    GsCOORDINATE2 locate; /* 0x00 */
    GsDOBJ2 object;       /* 0x50 */
} OrnamentType;

typedef struct
{
    GsCOORDINATE2 locate; /* 0x00 */
} WorldType;

extern void *valloc(u32 size);
extern void GsInitCoordinate2(GsCOORDINATE2 *super, GsCOORDINATE2 *base);
extern MATRIX *RotMatrixYXZ(SVECTOR *r, MATRIX *m);
extern void GsMapModelingData(u_long *adr);
extern void GsLinkObject4(u_long id, GsDOBJ2 *obj, s32 unk);
extern SVECTOR UnitVector;
extern WorldType World;

OrnamentType *LoadOrnament(u_long *adr)
{
    OrnamentType *base;

    base = (OrnamentType *)valloc(sizeof(OrnamentType));
    if (adr != 0) {
        adr = adr + 1;
        GsMapModelingData(adr);
        GsLinkObject4((u_long)(adr + 2), &base->object, 0);
    }
    base->object.coord2 = (GsCOORDINATE2 *)base;
    base->object.attribute = 0;
    GsInitCoordinate2(&World.locate, (GsCOORDINATE2 *)base);
    base->locate.coord.t[0] = 0;
    base->locate.coord.t[1] = 0;
    base->locate.coord.t[2] = 0;
    RotMatrixYXZ(&UnitVector, &base->locate.coord);
    base->locate.flg = 0;
    return base;
}
