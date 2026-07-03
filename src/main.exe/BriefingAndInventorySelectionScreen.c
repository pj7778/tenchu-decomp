#include "common.h"
#include "main.exe.h"

/*
 * Pre-mission briefing / item selection screen (0x80052084, 0xE24 bytes).
 *
 * WIP draft — see the INCLUDE_ASM stub at the bottom which is still the
 * linked implementation while this is iterated with matchdiff.
 */

/* The persistent state is accessed three ways in the original, on purpose:
 *  - through short-lived pointer locals (q/ps/r below) -> reg+disp addressing;
 *  - through PSTATE casts in the two entry loops -> one hoisted 0x80010000;
 *  - through plain extern globals -> assembler one-line macro (lui+op pairs).
 */
#define PSTATE ((PersistentState *)0x80010000)

typedef struct
{
    s16 x;        /* 0x0 grid position */
    s16 y;        /* 0x2 */
    s32 itemIndex; /* 0x4 */
    u8 maxStock;  /* 0x8 */
    u8 pad[3];    /* 0x9 */
} ShopItemDefault;

typedef struct
{
    u8 uid;       /* 0x0 */
    u8 pad[0x1B]; /* 0x1 */
} StageConfigEntry;

typedef struct { char *p[4]; } HelpPathBlock;  /* 0x10 */

extern u8 CHOSEN_CHARACTER;
extern u8 STAGE_LAYOUT_NUMBER;
extern u8 D_80010019;
extern u8 D_8001001A;
extern u8 D_80010048;
extern u8 SHOP_STOCK_STATE_BY_CHAR[];
extern ShopItemDefault SHOP_ITEM_DEFAULTS[];
extern StageConfigEntry StageConfig[];
extern char *ITEM_SEL_SPRITE_PTRS[];
extern char NUMBER_TIM_PATH[];
extern char *ITEM_HELP_TIM_PATHS[];
extern s16 CARRY_30_ITEMS_CHEAT_APPLIED; /* gp-relative (TU-local .sdata) */
extern u16 SkipFrame;
extern GsOT *OTablePt;

extern int rand(void);
extern u_long *FileRead(char *path);
extern void vfree(void *p);
extern void *FUN_8004f4f8(u_long *tim);
extern void InitSprite(GsIMAGE *im, GsSPRITE *sp);
extern void LoadTIM(u_long *tim);
extern void FadeOutDirect(s16 time, s16 attrib, u8 r, u8 g, u8 b);
extern void FUN_80038ce0(void);
extern void FUN_8004f6c0(int arg);
extern void SoundEx(VECTOR *loc, int id);
extern void StartDrawing(void);
extern void DrawBG(void *bg);
extern void EndDrawing(int arg);
extern void PutNumber(int x, int y, int n);
extern void PutItemIcon(int id, int x, int y, int scale);
extern void PutItemCursor(int x, int y, int size, int rotdif);
extern void DisposeBG(void *bg);
extern int check_for_known_button_combination(s16 pad, s16 newpress);
extern u_long *get_tim_from_archive(u_long *archive, int idx);
extern void GsSortSprite(GsSPRITE *sp, GsOT *ot, int pri);
extern buttons_held GetRealPad(s32 which);
extern void FUN_800519bc(void);

/*
 * The two TIM-sprite setup blocks are inlined static helpers (same mechanism
 * as DoInfoViewProc's menus): the GsIMAGE scratch is the helper's own local,
 * so its address expands from the inlined frame base (bare register — every
 * call gets a direct addiu into the arg register instead of a CSE'd
 * callee-saved pseudo), and the two inline expansions reuse one freed temp
 * slot after the caller's locals (spr @ +0, hspr @ +0x28, tim @ +0x50).
 */
static inline u_long *LoadHelpArchive(PersistentState *q)
{
    char *paths[4];

    *(HelpPathBlock *)paths = *(HelpPathBlock *)ITEM_HELP_TIM_PATHS;
    return FileRead(paths[q->language]);
}

static inline void TimToSprite(u_long *buf, GsSPRITE *sp)
{
    GsIMAGE tim;

    GetTIMInfo(buf, &tim);
    InitSprite(&tim, sp);
}

INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/BriefingAndInventorySelectionScreen", BriefingAndInventorySelectionScreen);

/*
 * WIP draft — structure verified against the target (see the header notes),
 * 241 differing instruction-lines remain, almost all register-allocation
 * cascades. Enable by deleting the INCLUDE_ASM above and the #if 0 guard.
 */
#if 0
void BriefingAndInventorySelectionScreen(void)
{
    GsSPRITE spr;
    GsSPRITE hspr;
    s16 bounce;
    u16 pad;
    u16 cap;
    u16 taken;
    void *bg;
    u_long *harc;
    GsSPRITE *p;
    int help;
    PersistentState *q;
    PersistentState *ps;
    PersistentState *r;
    GsSPRITE *dsp;
    u_long *buf;
    int cursor;
    int nsel;
    int scale;
    s16 np;
    int i;
    s16 j;
    s16 gi;
    s16 shown;
    int id;

    cursor = 0;
    pad = -1;
    cap = 0xF;
    taken = 0;
    help = -1;

    for (i = 0; i < 0x14; i++) {
        PSTATE->backup[i] = PSTATE->stock[CHOSEN_CHARACTER * 0x20 + i];
    }
    for (j = 0; j < 0x14; j++) {
        PSTATE->counts[j] = 0;
    }
    q = (PersistentState *)0x80010000;
    if (StageConfig[q->stage].uid == 0) {
        q->counts[0] = 0xFF;
        q->counts[1] = 5;
        return;
    }
    q->counts[0] = 0xFF;
    if ((q->flags48 & 1) == 0) {
        FUN_800519bc();
    }
    bounce = 0;
    scale = 0x1000;
    buf = FileRead(ITEM_SEL_SPRITE_PTRS[q->language]);
    bg = FUN_8004f4f8(buf);
    vfree(buf);
    buf = FileRead(NUMBER_TIM_PATH);
    p = &spr;
    TimToSprite(buf, p);
    spr.attribute |= 0x50000000;
    p->x = -0xA0;
    p->y = -0x78;
    p->r = 0x80;
    p->g = 0x80;
    p->b = 0x80;
    p->mx = p->w >> 1;
    p->my = p->h >> 1;
    spr.mx = 0;
    spr.my = 0;
    LoadTIMAndFree(buf);
    spr.w = 0xC;

    nsel = 1;
    harc = LoadHelpArchive(q);

    for (i = 0; i < 0x13; i++) {
        int n = SHOP_ITEM_DEFAULTS[i].itemIndex + CHOSEN_CHARACTER * 0x20;
        s32 mx = SHOP_ITEM_DEFAULTS[i].maxStock;
        u8 c = PSTATE->stock[n];
        if (c != 0xFE && mx < c) {
            PSTATE->stock[n] = mx;
        }
    }

    ps = (PersistentState *)0x80010000;
    do {
        rand();
        np = pad;
        pad = GetRealPad(0);
        np = pad & (pad ^ np);
        id = check_for_known_button_combination((s16)pad, np);
        switch ((s16)(id - 1)) {
        case 0:
            if (CARRY_30_ITEMS_CHEAT_APPLIED == 0) {
                CARRY_30_ITEMS_CHEAT_APPLIED = 1;
                cap = 0x1E;
            }
            break;
        case 1:
            for (j = 1; j < 9; j++) {
                int n = j + ps->chr * 0x20;
                if (ps->stock[n] == 0xFE) {
                    ps->stock[n] = 1;
                } else {
                    ps->stock[n] = ps->stock[n] + 1;
                }
            }
            for (j = 9; j < 0x14; j++) {
                int n = j + ps->chr * 0x20;
                if (ps->stock[n] != 0xFE) {
                    ps->stock[n] = ps->stock[n] + 1;
                }
            }
            for (i = 0; i < 0x13; i++) {
                int n = SHOP_ITEM_DEFAULTS[i].itemIndex + CHOSEN_CHARACTER * 0x20;
                s32 mx = SHOP_ITEM_DEFAULTS[i].maxStock;
                u8 c = PSTATE->stock[n];
                if (c != 0xFE && mx < c) {
                    PSTATE->stock[n] = mx;
                }
            }
            break;
        case 3:
            for (j = 9; j < 0x14; j++) {
                int n = j + ps->chr * 0x20;
                if (ps->stock[n] == 0xFE) {
                    ps->stock[n] = 1;
                }
            }
            break;
        case 0x1F:
            if (ps->chr != 0) {
                u8 already = ps->counts[0x13];
                if (already != 0 || (&ps->stock[0x13])[ps->chr * 0x20] == 1) {
                    if ((s16)nsel < 6) {
                        if (already == 0) {
                            nsel++;
                            taken++;
                        }
                        ps->counts[0x13] = 0xFF;
                        (&ps->stock[0x13])[ps->chr * 0x20] = 0;
                        SoundEx((VECTOR *)0x0, 8);
                    }
                }
            }
            break;
        case 7:
            for (j = 0; j < 0x14; j++) {
                ps->stock[(int)j + CHOSEN_CHARACTER * 0x20] = ps->backup[j];
            }
            FadeOutDirect(0x20, 2, 8, 8, 8);
            FUN_80038ce0();
            STAGE_LAYOUT_NUMBER = 0xFF;
            D_80010048 = D_80010048 & 0xFE;
            FUN_8004f6c0(0x10);
            break;
        }
        if (np == 0x800) {
            goto quit;
        }
        StartDrawing();
        DrawBG(bg);
        gi = 0;
    grid:
        {
            int n = SHOP_ITEM_DEFAULTS[gi].itemIndex;
            u8 c = ps->stock[n + ps->chr * 0x20];
            if (c != 0xFE) {
                s32 x = SHOP_ITEM_DEFAULTS[gi].x;
                s32 y = SHOP_ITEM_DEFAULTS[gi].y;
                if (c != 0xFF) {
                    PutNumber(x + 0x1A, y + 8, c);
                }
                PutItemIcon(n, x, y, 0x1000);
            }
        }
        gi++;
        if (gi < 0x13)
            goto grid;

        PutItemCursor(SHOP_ITEM_DEFAULTS[cursor].x, SHOP_ITEM_DEFAULTS[cursor].y, 0x1000, -0x3000);
        {
            s16 dy;
            s16 dx;
            int ddy, ddx;
            int k;

            dy = 0x10;
            if ((np & 0x4000) == 0) {
                dy = 0;
                if ((np & 0x1000) != 0) {
                    dy = -0x10;
                }
            }
            dx = 0x10;
            if ((np & 0x2000) == 0) {
                dx = 0;
                if ((np & 0x8000) != 0) {
                    dx = -0x10;
                }
            }
            ddx = dx;
            ddy = dy;
            k = cursor;
            if (ddx != 0 || ddy != 0) {
                int best = 0x7FFFFFFF;
                int bi = cursor;
                int tx = SHOP_ITEM_DEFAULTS[bi].x + ddx;
                int ty = SHOP_ITEM_DEFAULTS[bi].y + ddy;
                for (i = 0; i < 0x13; i++) {
                    int ex = SHOP_ITEM_DEFAULTS[i].x - tx;
                    int ey = SHOP_ITEM_DEFAULTS[i].y - ty;
                    int d = ex * ex + ey * ey;
                    if (d < best && 0 <= ex * ddx && 0 <= ey * ddy && i != k) {
                        best = d;
                        bi = i;
                    }
                }
                cursor = bi;
            }
        }
        if ((np & 0xF000) != 0) {
            SoundEx((VECTOR *)0x0, 0xB);
            help = -1;
        }
        if (np != 0) {
            if ((s16)pad == 0x20) {
                np = 0;
                bounce = 1;
                {
                    s16 idx = SHOP_ITEM_DEFAULTS[cursor].itemIndex;
                    scale = 0x200;
                    if (ps->stock[idx + ps->chr * 0x20] != 0) {
                        if (ps->stock[idx + ps->chr * 0x20] != 0xFE) {
                            if ((s16)taken < cap) {
                                u8 cnt = ps->counts[idx];
                                if (cnt == 0) {
                                    nsel++;
                                }
                                if ((s16)nsel < 6) {
                                    if (idx != 0x13 || D_8001001A == 0) {
                                        ps->counts[idx] = cnt + 1;
                                        taken++;
                                        ps->stock[idx + ps->chr * 0x20]--;
                                    }
                                    SoundEx((VECTOR *)0x0, 0xD);
                                } else {
                                    SoundEx((VECTOR *)0x0, 0xC);
                                    help = 0x14;
                                    nsel--;
                                }
                            } else {
                                SoundEx((VECTOR *)0x0, 0xC);
                                help = 0x13;
                            }
                        }
                    }
                }
            }
            if (np != 0 && (s16)pad == 0x40) {
                s16 idx = SHOP_ITEM_DEFAULTS[cursor].itemIndex;
                bounce = 2;
                {
                    u8 c = ps->counts[idx];
                    scale = 0x1400;
                    if (c != 0) {
                        if (c == 0xFF) {
                            ps->counts[idx] = 0;
                            ps->stock[idx + ps->chr * 0x20] = 1;
                            nsel--;
                        } else {
                            ps->counts[idx] = c - 1;
                            ps->stock[idx + ps->chr * 0x20]++;
                            if (ps->counts[idx] == 0) {
                                nsel--;
                            }
                        }
                        taken--;
                        SoundEx((VECTOR *)0x0, 0x1F);
                    }
                }
                help = -1;
            }
        }
        if ((s16)scale < 0x1000) {
            scale += 0xC0;
        }
        if (help == -1) {
            if (ps->stock[SHOP_ITEM_DEFAULTS[cursor].itemIndex + ps->chr * 0x20] != 0xFE) {
                help = SHOP_ITEM_DEFAULTS[cursor].itemIndex - 1;
            }
        }
        if (help != -1) {
            buf = get_tim_from_archive(harc, help);
            TimToSprite(buf, &hspr);
            hspr.x = -0xA0;
            hspr.y = -0x78;
            hspr.r = 0x80;
            hspr.g = 0x80;
            hspr.b = 0x80;
            hspr.attribute |= 0x50000000;
            hspr.mx = hspr.w >> 1;
            hspr.my = hspr.h >> 1;
            hspr.mx = 0;
            hspr.my = 0;
            LoadTIM(buf);
            hspr.x = -0x92;
            hspr.y = 0x23;
            GsSortSprite(&hspr, OTablePt, 1);
        }
        if (bounce == 1) {
            scale += 0x10;
            if (0x1400 < (s16)scale) {
                bounce ^= 1;
            }
        } else if (bounce == 0) {
            scale -= 0x10;
            if ((s16)scale < 0x1000) {
                bounce ^= 1;
            }
        } else if (bounce == 2) {
            scale -= 0x10;
            if ((s16)scale < 0x1000) {
                bounce = 0;
            }
        }
        shown = 0;
        for (gi = 0; gi < 0x14; gi++) {
            u8 c = ps->counts[gi];
            if (c != 0) {
                if (c != 0xFF) {
                    PutNumber(0xA6 - shown * 0x19, 0x62, c);
                }
                PutItemIcon((int)gi, 0x8C - shown * 0x19, 0x5A, 0x1000);
                shown++;
            }
        }
        {
            int av;
            int neg;
            u8 u0;

            dsp = &spr;
            dsp->x = 0x22;
            dsp->y = -0x32;
            av = cap - taken;
            if ((s16)av < 0) {
                av = -(s16)av;
                neg = 1;
            } else {
                neg = 0;
            }
            do {
                s16 d = (s16)av;
                s32 quo = d / 10;
                u0 = dsp->u;
                dsp->u = u0 + (s16)(d - quo * 10) * dsp->w;
                GsSortSprite(dsp, OTablePt, 0);
                dsp->u = u0;
                dsp->x -= 0xC;
                av = quo;
            } while ((s16)av != 0);
            if (neg) {
                dsp->u = (u8)u0 + dsp->w * 10;
                GsSortSprite(dsp, OTablePt, 0);
                dsp->u = u0;
            }
        }
        SkipFrame = 2;
        EndDrawing(0);
    } while (1);

quit:
    FadeOutDirect(0x20, 2, 8, 8, 8);
    FUN_80038ce0();
    if (PSTATE->counts[0x12] != 0) {
        PSTATE->counts[0x12] = 0xFF;
    }
    r = (PersistentState *)0x80010000;
    for (j = 0; j < 9; j++) {
        int n = r->chr * 0x20 + j;
        if (r->stock[n] == 0) {
            r->stock[n] = 0xFE;
        }
    }
    vfree(harc);
    DisposeBG(bg);
}
#endif
