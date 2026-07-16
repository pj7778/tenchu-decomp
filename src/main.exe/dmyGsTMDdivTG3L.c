#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDdivTG3L (0x80067374) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_80015284[]; /* "TMDdivTG3L\n" */
extern s32 D_8008f690;

void *dmyGsTMDdivTG3L(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f690 == 0)
    {
        printf(D_80015284);
        D_8008f690 = 1;
    }
    return arg3;
}
