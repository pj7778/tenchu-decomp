#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDdivTG3NL (0x800672e4) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_80015264[]; /* "TMDdivTG3NL\n" */
extern s32 D_8008f688;

void *dmyGsTMDdivTG3NL(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f688 == 0)
    {
        printf(D_80015264);
        D_8008f688 = 1;
    }
    return arg3;
}
