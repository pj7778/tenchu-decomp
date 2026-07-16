#include "common.h"
#include "main.exe.h"

/*
 * dmyGsPrstTNF4 (0x8006795c) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_80015388[]; /* "PrstTNF4\n" */
extern s32 D_8008f6e4;

void *dmyGsPrstTNF4(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f6e4 == 0)
    {
        printf(D_80015388);
        D_8008f6e4 = 1;
    }
    return arg3;
}
