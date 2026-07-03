#ifndef LIBGS_H
#define LIBGS_H

#include <psxsdk/libgte.h>
#include <types.h>

typedef struct GsCOORD2PARAM GsCOORD2PARAM;

struct GsCOORD2PARAM
{
    VECTOR scale;
    SVECTOR rotate;
    VECTOR trans;
};

typedef struct GsCOORDINATE2 GsCOORDINATE2;
struct GsCOORDINATE2
{
    u_long flg;
    MATRIX coord;
    MATRIX workm;

    GsCOORD2PARAM *param;
    GsCOORDINATE2 *super;
    GsCOORDINATE2 *sub;
};

typedef struct GsDOBJ2 GsDOBJ2;

struct GsDOBJ2
{
    u_long attribute;
    GsCOORDINATE2 *coord2;
    u_long *tmd;
    u_long id;
};

/* 0x24 bytes — layout proven by BriefingAndInventorySelectionScreen's stack
 * (two sprites at sp+0x18/0x40, GsIMAGE at 0x68, next local at 0x88). */
typedef struct GsSPRITE GsSPRITE;
struct GsSPRITE
{
    u_long attribute;     /* 0x00 */
    short x, y;           /* 0x04 */
    u_short w, h;         /* 0x08 */
    u_short tpage;        /* 0x0C */
    u_char u, v;          /* 0x0E */
    short cx, cy;         /* 0x10 */
    u_char r, g, b;       /* 0x14 */
    short mx, my;         /* 0x18 */
    short scalex, scaley; /* 0x1C */
    long rotate;          /* 0x20 */
};

/* Ordering table — opaque here (only passed around by pointer). */
typedef struct GsOT GsOT;

typedef struct GsIMAGE GsIMAGE;
struct GsIMAGE
{
    u_long pmode;
    short px;
    short py;
    u_short pw;
    u_short ph;
    u_long *pixel;
    short cx;
    short cy;
    u_short cw;
    u_short ch;
    u_long *clut;
};

#endif