#include "common.h"
#include "main.exe.h"

/*
 * dmyGsPrstTF3LFG (0x80066fcc) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_800151d8[]; /* "PrstTF3LFG\n" */
extern s32 D_8008f65c;

void *dmyGsPrstTF3LFG(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f65c == 0)
    {
        printf(D_800151d8);
        D_8008f65c = 1;
    }
    return arg3;
}
