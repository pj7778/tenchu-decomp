#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDfastTF4L (0x80068454) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_800155b4[]; /* "TMDfastTF4L\n" */
extern s32 D_8008f780;

void *dmyGsTMDfastTF4L(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f780 == 0)
    {
        printf(D_800155b4);
        D_8008f780 = 1;
    }
    return arg3;
}
