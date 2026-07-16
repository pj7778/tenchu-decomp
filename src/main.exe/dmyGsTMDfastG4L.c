#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDfastG4L (0x80068334) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_8001557c[]; /* "TMDfastG4L\n" */
extern s32 D_8008f770;

void *dmyGsTMDfastG4L(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f770 == 0)
    {
        printf(D_8001557c);
        D_8008f770 = 1;
    }
    return arg3;
}
