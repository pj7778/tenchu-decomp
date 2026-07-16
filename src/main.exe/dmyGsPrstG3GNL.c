#include "common.h"
#include "main.exe.h"

/*
 * dmyGsPrstG3GNL (0x8006891c) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_800156b0[]; /* "PrstG3GNL\n" */
extern s32 D_8008f7c4;

void *dmyGsPrstG3GNL(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f7c4 == 0)
    {
        printf(D_800156b0);
        D_8008f7c4 = 1;
    }
    return arg3;
}
