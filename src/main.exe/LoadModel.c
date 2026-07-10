#include "common.h"
#include "main.exe.h"
#include "item.h"

/*
 * LoadModel (0x80018448, 0xb0 bytes) - near-twin of FUN_8001851c.c (both
 * ModelType constructors in this TU): allocate+zero-init a ModelType the
 * same way (self-referencing object.coord2, World-rooted
 * GsInitCoordinate2, zeroed translation/rotate/clip, RotMatrixYXZ fed the
 * object's OWN zeroed `rotate` field), but instead of FUN_8001851c's
 * clone-from-existing-instance tail (`objp->object.tmd`), LoadModel wires
 * the model data in from `adr` (when non-null) via GsMapModelingData/
 * GsLinkObject4 - the same "reassign the pointer parameter in place, then
 * use a smaller residual offset for the second call" idiom as
 * LoadOrnament.c (`adr = adr + 1; GsMapModelingData(adr);
 * GsLinkObject4((u_long)(adr + 2), &base->object, 0);`).
 */
extern void *valloc(u32 size);
extern void GsInitCoordinate2(GsCOORDINATE2 *super, GsCOORDINATE2 *base);
extern MATRIX *RotMatrixYXZ(SVECTOR *r, MATRIX *m);
extern void GsMapModelingData(u_long *adr);
extern void GsLinkObject4(u_long id, GsDOBJ2 *obj, s32 unk);

typedef struct
{
    GsCOORDINATE2 locate; /* 0x00 */
} WorldType;

extern WorldType World;

ModelType *LoadModel(u_long *adr)
{
    ModelType *base;

    base = (ModelType *)valloc(sizeof(ModelType));
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
    base->rotate.vx = 0;
    base->rotate.vy = 0;
    base->rotate.vz = 0;
    base->clip.vx = 0;
    base->clip.vy = 0;
    base->clip.vz = 0;
    RotMatrixYXZ(&base->rotate, &base->locate.coord);
    base->locate.flg = 0;
    base->id = -1;
    base->attribute = 0;
    return base;
}
