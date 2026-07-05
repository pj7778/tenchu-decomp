#include "common.h"
#include "main.exe.h"

/*
 * DemoPatchInit (0x8004c044) — snapshot a small VRAM rect (x=0x3f0,y=0x1ff,
 * w=0x10,h=1 — a single 16-pixel row, i.e. a 16-colour CLUT) into
 * DemoBackupArea via the PSYQ libgpu StoreImage2, then DrawSync(0) waits for
 * the transfer to finish. Same per-TU RECT convention as FUN_80038ce0.c (no
 * libgpu.h vendored in this repo).
 */
typedef struct
{
    s16 x;                        /* 0x0 */
    s16 y;                        /* 0x2 */
    s16 w;                        /* 0x4 */
    s16 h;                        /* 0x6 */
} RECT;                           /* 0x8 (PSYQ libgpu.h) */

extern int StoreImage2(RECT *rect, u_long *p);
extern int DrawSync(int mode);
extern u_long DemoBackupArea[];

void DemoPatchInit(void)
{
    RECT r;

    r.x = 0x3f0;
    r.y = 0x1ff;
    r.w = 0x10;
    r.h = 1;
    StoreImage2(&r, DemoBackupArea);
    DrawSync(0);
}
