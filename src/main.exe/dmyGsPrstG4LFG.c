#include "common.h"
#include "main.exe.h"

/*
 * dmyGsPrstG4LFG (0x8006768c) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_8001530c[]; /* "PrstG4LFG\n" */
extern s32 D_8008f6bc;

void *dmyGsPrstG4LFG(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f6bc == 0)
    {
        printf(D_8001530c);
        D_8008f6bc = 1;
    }
    return arg3;
}
