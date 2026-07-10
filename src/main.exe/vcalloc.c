#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * void * vcalloc(unsigned long size, unsigned char c);
 *     VALLOC.C:203, 9 src lines, frame 32 bytes, saved-reg mask 0x80070000
 *
 * Original parameters and locals (the demo build's register allocation may
 * differ from retail, but the COUNT and TYPES drive cc1's codegen and carry
 * over). A repeated name is a nested-block scope, not a duplicate:
 *     param $a0       unsigned long size
 *     param $a1       unsigned char c
 * END PSX.SYM */

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
