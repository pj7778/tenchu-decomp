#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDdivTG4L (0x80067c74) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_8001541c[]; /* "TMDdivTG4L\n" */
extern s32 D_8008f710;

void *dmyGsTMDdivTG4L(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f710 == 0)
    {
        printf(D_8001541c);
        D_8008f710 = 1;
    }
    return arg3;
}
