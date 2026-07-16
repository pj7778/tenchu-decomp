#include "common.h"
#include "main.exe.h"

/*
 * dmyGsPrstTG3LFG (0x8006720c) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_80015240[]; /* "PrstTG3LFG\n" */
extern s32 D_8008f67c;

void *dmyGsPrstTG3LFG(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f67c == 0)
    {
        printf(D_80015240);
        D_8008f67c = 1;
    }
    return arg3;
}
