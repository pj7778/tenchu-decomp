#include "common.h"
#include "main.exe.h"

/*
 * dmyGsPrstG3LFG (0x80066d8c) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_80015174[]; /* "PrstG3LFG\n" */
extern s32 D_8008f63c;

void *dmyGsPrstG3LFG(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f63c == 0)
    {
        printf(D_80015174);
        D_8008f63c = 1;
    }
    return arg3;
}
