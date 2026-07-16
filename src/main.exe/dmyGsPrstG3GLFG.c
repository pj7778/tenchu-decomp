#include "common.h"
#include "main.exe.h"

/*
 * dmyGsPrstG3GLFG (0x800688d4) — LIBGS "dummy" primitive-sort placeholder; warn-once
 * clone of dmyGsPrstF3NL (see that file for the family's shape).
 */
extern int printf(const char *fmt, ...);
extern char D_800156a4[]; /* "PrstG3GLFG\n" */
extern s32 D_8008f7c0;

void *dmyGsPrstG3GLFG(void *arg0, void *arg1, void *arg2, void *arg3)
{
    if (D_8008f7c0 == 0)
    {
        printf(D_800156a4);
        D_8008f7c0 = 1;
    }
    return arg3;
}
