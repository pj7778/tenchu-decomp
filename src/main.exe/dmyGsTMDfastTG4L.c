#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDfastTG4L (0x80068574) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_800155f4[]; /* "TMDfastTG4L\n" */
extern s32 D_8008f790;

void *dmyGsTMDfastTG4L(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f790 == 0)
    {
        printf(D_800155f4);
        D_8008f790 = 1;
    }
    return arg3;
}
