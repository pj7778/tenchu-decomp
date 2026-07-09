#include "common.h"
#include "main.exe.h"

/*
 * vsize (0x80016e80, 0xc bytes) — same TU as vinit.c/vgetfreesize.c
 * (virtual_memory_pool/valloc/vfree/vgetmaxsize/vgetfreesize/vcalloc all
 * cluster together): given a pointer returned by valloc/vcalloc, reads the
 * `size` (word count) out of the PoolBlock header that immediately precedes
 * the payload and returns the allocation's size in BYTES.
 */

u32 vsize(void *pt)
{
    return *(s32 *)((u8 *)pt - 8) << 2;
}
