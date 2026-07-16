#include "common.h"
#include "main.exe.h"

/*
 * dmyGsPrstF4L (0x80067494) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_800152b4[]; /* "PrstF4L\n" */
extern s32 D_8008f6a0;

void *dmyGsPrstF4L(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f6a0 == 0)
    {
        printf(D_800152b4);
        D_8008f6a0 = 1;
    }
    return arg3;
}
