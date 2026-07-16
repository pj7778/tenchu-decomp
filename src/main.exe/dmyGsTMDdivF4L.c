#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDdivF4L (0x800675b4) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_800152e8[]; /* "TMDdivF4L\n" */
extern s32 D_8008f6b0;

void *dmyGsTMDdivF4L(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f6b0 == 0)
    {
        printf(D_800152e8);
        D_8008f6b0 = 1;
    }
    return arg3;
}
