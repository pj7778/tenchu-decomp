#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDfastTF3L (0x80067fd4) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_800154c4[]; /* "TMDfastTF3L\n" */
extern s32 D_8008f740;

void *dmyGsTMDfastTF3L(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f740 == 0)
    {
        printf(D_800154c4);
        D_8008f740 = 1;
    }
    return arg3;
}
