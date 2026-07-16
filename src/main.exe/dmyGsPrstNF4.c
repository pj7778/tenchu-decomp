#include "common.h"
#include "main.exe.h"

/*
 * dmyGsPrstNF4 (0x800674dc) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_800152c0[]; /* "PrstNF4\n" */
extern s32 D_8008f6a4;

void *dmyGsPrstNF4(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f6a4 == 0)
    {
        printf(D_800152c0);
        D_8008f6a4 = 1;
    }
    return arg3;
}
