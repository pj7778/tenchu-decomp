#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDdivTF4L (0x80067a34) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_800153b4[]; /* "TMDdivTF4L\n" */
extern s32 D_8008f6f0;

void *dmyGsTMDdivTF4L(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f6f0 == 0)
    {
        printf(D_800153b4);
        D_8008f6f0 = 1;
    }
    return arg3;
}
