#include "common.h"
#include "main.exe.h"

/*
 * dmyGsPrstF4LFG (0x8006744c) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_800152a8[]; /* "PrstF4LFG\n" */
extern s32 D_8008f69c;

void *dmyGsPrstF4LFG(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f69c == 0)
    {
        printf(D_800152a8);
        D_8008f69c = 1;
    }
    return arg3;
}
