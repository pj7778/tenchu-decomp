#include "common.h"
#include "main.exe.h"

/*
 * dmyGsTMDfastG3GLFG (0x80068724) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_80015654[]; /* "TMDfastG3GLFG\n" */
extern s32 D_8008f7a8;

void *dmyGsTMDfastG3GLFG(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f7a8 == 0)
    {
        printf(D_80015654);
        D_8008f7a8 = 1;
    }
    return arg3;
}
