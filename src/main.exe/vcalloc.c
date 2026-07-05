#include "common.h"
#include "main.exe.h"

/*
 * vcalloc (0x80016d2c) — calloc-shaped wrapper over the virtual allocator
 * (same TU as vinit.c: virtual_memory_pool/valloc/vfree/vgetmaxsize/
 * vgetfreesize/vcalloc all cluster together in this address range):
 * allocate `size` bytes from the pool, then fill them with byte `c`.
 */
extern void *valloc(u32 size);
extern void *memset(void *s, int c, u32 n);

void *vcalloc(u32 size, u8 c)
{
    void *p;

    p = valloc(size);
    memset(p, c, size);
    return p;
}
