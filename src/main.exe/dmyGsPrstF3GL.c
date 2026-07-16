#include "common.h"
#include "main.exe.h"

/*
 * dmyGsPrstF3GL (0x800687b4) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_80015674[]; /* "PrstF3GL\n" */
extern s32 D_8008f7b0;

void *dmyGsPrstF3GL(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f7b0 == 0)
    {
        printf(D_80015674);
        D_8008f7b0 = 1;
    }
    return arg3;
}
