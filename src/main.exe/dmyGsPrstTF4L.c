#include "common.h"
#include "main.exe.h"

/*
 * dmyGsPrstTF4L (0x80067914) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_8001537c[]; /* "PrstTF4L\n" */
extern s32 D_8008f6e0;

void *dmyGsPrstTF4L(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f6e0 == 0)
    {
        printf(D_8001537c);
        D_8008f6e0 = 1;
    }
    return arg3;
}
