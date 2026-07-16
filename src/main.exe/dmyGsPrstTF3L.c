#include "common.h"
#include "main.exe.h"

/*
 * dmyGsPrstTF3L (0x80067014) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_800151e4[]; /* "PrstTF3L\n" */
extern s32 D_8008f660;

void *dmyGsPrstTF3L(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f660 == 0)
    {
        printf(D_800151e4);
        D_8008f660 = 1;
    }
    return arg3;
}
