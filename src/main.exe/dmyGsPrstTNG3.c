#include "common.h"
#include "main.exe.h"

/*
 * dmyGsPrstTNG3 (0x8006729c) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_80015258[]; /* "PrstTNG3\n" */
extern s32 D_8008f684;

void *dmyGsPrstTNG3(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f684 == 0)
    {
        printf(D_80015258);
        D_8008f684 = 1;
    }
    return arg3;
}
