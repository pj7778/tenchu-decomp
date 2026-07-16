#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDfastF4L (0x80068214) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_80015544[]; /* "TMDfastF4L\n" */
extern s32 D_8008f760;

void *dmyGsTMDfastF4L(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f760 == 0)
    {
        printf(D_80015544);
        D_8008f760 = 1;
    }
    return arg3;
}
