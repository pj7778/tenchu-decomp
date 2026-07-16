#include "common.h"
#include "main.exe.h"

/*
 * dmyGsPrstTF4LFG (0x800678cc) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_80015370[]; /* "PrstTF4LFG\n" */
extern s32 D_8008f6dc;

void *dmyGsPrstTF4LFG(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f6dc == 0)
    {
        printf(D_80015370);
        D_8008f6dc = 1;
    }
    return arg3;
}
