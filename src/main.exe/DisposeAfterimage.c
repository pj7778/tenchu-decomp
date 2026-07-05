#include "common.h"
#include "main.exe.h"

/*
 * DisposeAfterimage (0x80038e58) — free an afterimage effect's two dynamic
 * buffers (`p1`@0x18, `p2`@0x1C — trail-point arrays, sized by SetupAfterimage
 * per ReqItemHappou/ReqItemLaunch) then the AfterimageType itself. Same
 * null-check-then-free shape as DisposeMotionManager (afi survives all three
 * vfree calls in a callee-saved register). Each TU that touches AfterimageType
 * declares its own local truncated view (ReqItemHappou/ReqItemLaunch only
 * define through vector2 @0xC); this one only needs the p1/p2 tail, so it
 * pads to 0x18 rather than importing the vector fields.
 */
extern void vfree(void *p);

typedef struct
{
    u8 pad0[0x18];
    void *p1;
    void *p2;
} AfterimageType;

void DisposeAfterimage(AfterimageType *afi)
{
    if (afi != 0)
    {
        vfree(afi->p1);
        vfree(afi->p2);
        vfree(afi);
    }
}
