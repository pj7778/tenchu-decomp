#include "common.h"
#include "main.exe.h"

/*
 * dmyGsPrstG3NL (0x80066d44) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_80015168[]; /* "PrstG3NL\n" */
extern s32 D_8008f638;

void *dmyGsPrstG3NL(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f638 == 0)
    {
        printf(D_80015168);
        D_8008f638 = 1;
    }
    return arg3;
}
