#include "common.h"
#include "main.exe.h"

/*
 * dmyGsPrstNF3 (0x80066bdc) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_80015128[]; /* "PrstNF3\n" */
extern s32 D_8008f624;

void *dmyGsPrstNF3(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f624 == 0)
    {
        printf(D_80015128);
        D_8008f624 = 1;
    }
    return arg3;
}
