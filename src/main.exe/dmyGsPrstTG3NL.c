#include "common.h"
#include "main.exe.h"

/*
 * dmyGsPrstTG3NL (0x800671c4) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_80015234[]; /* "PrstTG3NL\n" */
extern s32 D_8008f678;

void *dmyGsPrstTG3NL(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f678 == 0)
    {
        printf(D_80015234);
        D_8008f678 = 1;
    }
    return arg3;
}
