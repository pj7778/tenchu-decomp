#include "common.h"
#include "main.exe.h"

/*
 * dmyGsPrstF3GNL (0x80068844) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_8001568c[]; /* "PrstF3GNL\n" */
extern s32 D_8008f7b8;

void *dmyGsPrstF3GNL(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f7b8 == 0)
    {
        printf(D_8001568c);
        D_8008f7b8 = 1;
    }
    return arg3;
}
