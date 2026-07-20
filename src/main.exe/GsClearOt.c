#include "common.h"
#include "main.exe.h"

void GsClearOt(unsigned short offset, unsigned short point, GsOT *ot)
{
    ot->offset = offset;
    ot->point = point;
    ot->tag = ot->org + (1 << ot->length) - 1;
    ClearOTagR((u_long *)ot->org, 1 << ot->length);
}
