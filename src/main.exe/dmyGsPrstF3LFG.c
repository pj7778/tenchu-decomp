#include "common.h"
#include "main.exe.h"

/*
 * dmyGsPrstF3LFG (0x80066b4c) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_80015110[]; /* "PrstF3LFG\n" */
extern s32 D_8008f61c;

void *dmyGsPrstF3LFG(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f61c == 0)
    {
        printf(D_80015110);
        D_8008f61c = 1;
    }
    return arg3;
}
