#include "common.h"
#include "main.exe.h"

/*
 * ProcItemKusuri (0x80040500) — the kusuri (healing potion) item processor.
 * mode 0: freeze the drinker (dispose weapon, drink animation 0xF01, status 0xF),
 * attach the item model to the character's hand and nudge it into place; mode 1:
 * while the drink animation plays, track the hand (copy the item coordinate into
 * the sprite and draw it); at animation frame 0x37 switch to mode 2; if the
 * animation was interrupted, drop the item (ReqItemDrop with a random toss) and
 * dispose; mode 2: heal to full, spray 20 random SetBleed particles, play the
 * gulp sound, and dispose of the item.
 *
 * Matching notes (each verified against the original bytes; see also
 * ProcItemManebue.c for the item-TU conventions):
 *  - `ff` holds 0xFF in a callee-saved reg ($s4) across calls: used by the
 *    entry test and the drop path's `item->mode = ff`; mode 2's dispose writes
 *    a literal 0xff instead ($s4 is &buf by then).
 *  - The dispatch is a real `switch`: it reloads item->mode (fresh index load)
 *    and compares it SIGNED (slti) — an if-ladder CSEs the load and compares
 *    unsigned. Case bodies sit in source order (0, 1, 2).
 *  - `i = 0` is case 2's first statement (reorg hoists it into the case-2
 *    branch delay slot). The bleed loop is `while (1) { if (!(i < 0x14))
 *    break; ...; i++; }` — a for/while-with-condition gets its exit test
 *    duplicated at the entry (jump.c duplicate_loop_exit_test) and then
 *    constant-folded away; the while(1)+break form keeps the original's
 *    top-test + unconditional back-jump while still letting loop.c hoist the
 *    invariants (&buf, &buf[0x10], the two magic divisors).
 *  - mode 2's jitter is written `t[n] + (rand() % 1000 - 500)`: fold's
 *    associate step canonicalizes it to the original's (t[n]-500) + rem shape,
 *    whereas writing `t[n] - 500 + rand() % 1000` gets reassociated the wrong
 *    way (constant pulled onto the remainder).
 *  - The position stores go through the same base as the VECTOR copy
 *    (((s32 *)(buf + 0x10))[n]) so the scheduler sees the store/copy
 *    dependence; a separate `buf + 0x18` base lets it interleave the copy
 *    into the divide latency.
 *  - The dispose tail is written out twice (drop path + mode 2); GCC's
 *    cross-jump merges the common suffix from the jalr on. The null check
 *    reads `ppu = item->proc` but the call is `item->proc(item)` (cse reuses
 *    the load) — checking and calling through `ppu` allocates $v1 instead of
 *    the original's $v0.
 */
typedef struct tag_TItem tag_TItem;

typedef struct ModelType ModelType;
struct ModelType
{
    GsCOORDINATE2 locate;        /* 0x00 */
    SVECTOR rotate;              /* 0x50 */
    s16 id;                      /* 0x58 */
    s16 attribute;               /* 0x5A */
    SVECTOR clip;                /* 0x5C */
};

typedef struct
{
    GsCOORDINATE2 locate;        /* 0x00 */
    SVECTOR rotate;              /* 0x50 */
    s16 id;                      /* 0x58 */
    s16 attribute;               /* 0x5A */
    SVECTOR clip;                /* 0x5C */
    s16 n;                       /* 0x64 */
    ModelType **object;          /* 0x68 */
} ModelArchiveType;

typedef struct
{
    GsCOORDINATE2 locate;        /* 0x00 */
    SVECTOR rotate;              /* 0x50 */
    s16 id;                      /* 0x58 */
    s16 attribute;               /* 0x5A */
    SVECTOR clip;                /* 0x5C */
    s32 scale;                   /* 0x64 */
} Sprite3D;

typedef struct
{
    u8 n;                        /* 0x0 */
    u8 sweep;                    /* 0x1 */
    u8 orderspd;                 /* 0x2 */
    u8 sidespd;                  /* 0x3 */
} MotionDataType;

typedef struct
{
    s16 mid;                     /* 0x0 */
    s16 count;                   /* 0x2 */
    s16 loop;                    /* 0x4 */
    s16 n;                       /* 0x6 */
    s16 mask;                    /* 0x8 */
    s16 mode;                    /* 0xA */
    void *model;                 /* 0xC */
    MotionDataType *motion;      /* 0x10 */
} MotionManager;

typedef struct
{
    s16 type;                    /* 0x00 */
    s16 status;                  /* 0x02 */
    s16 attribute;               /* 0x04 */
    s16 turn;                    /* 0x06 */
    s16 life;                    /* 0x08 */
    u16 lifemax;                 /* 0x0A */
    u8 pad0[0x2C];               /* 0x0C */
    VECTOR *locate;              /* 0x38 */
    u8 pad1[0x1C];               /* 0x3C */
    ModelArchiveType *model;     /* 0x58 */
    MotionManager *motion;       /* 0x5C */
} Humanoid;

typedef struct
{
    s32 type;                    /* 0x00 */
    Humanoid *user;              /* 0x04 */
    VECTOR start;                /* 0x08 */
    VECTOR end;                  /* 0x18 */
} PARAM_ITEM_USE;                /* 0x28 */

struct tag_TItem
{
    Humanoid *owner;             /* 0x00 */
    Sprite3D *model;             /* 0x04 */
    s32 type;                    /* 0x08 */
    void (*proc)(tag_TItem *);   /* 0x0C */
    ModelType *locate;           /* 0x10 */
    s32 coll_mode;               /* 0x14 */
    s32 coll_pause;              /* 0x18 */
    s16 coll_size;               /* 0x1C */
    s16 coll_ofsY;               /* 0x1E */
    u8 param[0x34];              /* 0x20 */
    u8 mode;                     /* 0x54 */
};

extern void dispose_weapon_data_of_char_(Humanoid *h, int a);
extern void UpdateMotion(MotionManager *m, int id);
extern void MoveHumanoid(Humanoid *h, int a, int b);
extern void UpdateCoordinate(ModelType *m);
extern void DrawSprite(Sprite3D *s);
extern VECTOR *GetAbsolutePosition(ModelType *m, int x, int y, int z);
extern void ReqItemDrop(PARAM_ITEM_USE *p);
extern void SetBleed(VECTOR *pos, SVECTOR *vel, int a, int col);
extern void SoundEx(VECTOR *loc, int id);
extern void DeleteConflict(ModelType *m);
extern void AdtMessageBox(char *fmt, ...);
extern int rand(void);
extern void *memset(void *s, int c, u32 n);
/* Absolute in this TU (the item TU doesn't define it; think's TU does) — this
 * file is deliberately NOT in Build.hs's maspsxGpExterns list. */
extern s16 ActionHalt;
extern char D_800121CC[];

void ProcItemKusuri(tag_TItem *item)
{
    Sprite3D *sprt;
    void (*ppu)(tag_TItem *);
    u8 ff;
    s32 i;
    /* One shared 40-byte scratch buffer (Ghidra's local_40): the drop path views
     * it as a PARAM_ITEM_USE; mode 2 views buf as the SetBleed position VECTOR,
     * buf+0x10 as its build area / the velocity SVECTOR, and buf+0x18 as the
     * velocity build area. The casts (not typed locals) are what reproduce the
     * original's stack layout and copy alignments (VECTOR copies are word moves,
     * SVECTOR copies are lwl/lwr). */
    u8 buf[0x28];

    sprt = item->model;
    ff = 0xff;
    if (item->mode == ff)
    {
        item->mode = 0;
        return;
    }
    switch (item->mode)
    {
    case 0:
    {
        Humanoid *own;

        own = item->owner;
        if (ActionHalt == 0 && 0 < own->life)
        {
            MotionDataType *md;

            dispose_weapon_data_of_char_(own, 3);
            UpdateMotion(own->motion, 0xf01);
            own->status = 0xf;
            md = own->motion->motion;
            MoveHumanoid(own, md->orderspd, md->sidespd);
        }
    }
        {
            ModelArchiveType *arc;

            arc = item->owner->model;
            if (0xe < arc->n)
                item->locate->locate.super = &arc->object[0xe]->locate;
            else
                item->locate->locate.super = &arc->object[2]->locate;
        }
        item->locate->locate.coord.t[0] = 0;
        item->locate->locate.coord.t[1] = 0x32;
        item->locate->locate.coord.t[2] = 0;
        item->mode = item->mode + 1;
        return;

    case 1:
    {
        MotionManager *mot;

        mot = item->owner->motion;
        if (mot->mid != 0xf01)
        {
            /* animation interrupted: toss the item back out */
            VECTOR *pos;
            Humanoid *own;
            s32 ty;

            pos = GetAbsolutePosition(item->locate, 0, 0, 0);
            own = item->owner;
            ty = item->type;
            memset(buf, 0, 0x28);
            ((PARAM_ITEM_USE *)buf)->type = ty;
            ((PARAM_ITEM_USE *)buf)->user = own;
            ((PARAM_ITEM_USE *)buf)->start.vx = pos->vx;
            ((PARAM_ITEM_USE *)buf)->start.vy = pos->vy;
            ((PARAM_ITEM_USE *)buf)->start.vz = pos->vz;
            ((PARAM_ITEM_USE *)buf)->end.vx = rand() % 200 - 100;
            ((PARAM_ITEM_USE *)buf)->end.vy = rand() % 100 - 200;
            ((PARAM_ITEM_USE *)buf)->end.vz = rand() % 200 - 100;
            ReqItemDrop((PARAM_ITEM_USE *)buf);
            ppu = item->proc;
            if (ppu == 0)
                return;
            item->mode = ff;
            item->proc(item);
            DeleteConflict(item->locate);
            if (item->mode != 0)
            {
                AdtMessageBox(D_800121CC, item->type, (u32)item->mode);
            }
            item->owner = 0;
            item->proc = 0;
            return;
        }
        {
            s16 cnt;

            cnt = mot->count;
            if (cnt == 0x37)
            {
                item->mode = 2;
                return;
            }
            if (cnt < 4)
                return;
        }
    }
        UpdateCoordinate(item->locate);
        sprt->locate = item->locate->locate;
        sprt->scale = 0x2000;
        DrawSprite(sprt);
        return;

    case 2:
    {
        i = 0;
        item->owner->life = item->owner->lifemax;
        while (1)
        {
            if (!(i < 0x14))
                break;
            memset(buf + 0x10, 0, 0x10);
            ((s32 *)(buf + 0x10))[0] = item->owner->model->locate.coord.t[0] + (rand() % 1000 - 500);
            ((s32 *)(buf + 0x10))[1] = item->owner->model->locate.coord.t[1] + (rand() % 1000 - 0x4b0);
            ((s32 *)(buf + 0x10))[2] = item->owner->model->locate.coord.t[2] + (rand() % 1000 - 500);
            *(VECTOR *)buf = *(VECTOR *)(buf + 0x10);
            memset(buf + 0x18, 0, 8);
            ((SVECTOR *)(buf + 0x18))->vy = rand() % 10 - 30;
            *(SVECTOR *)(buf + 0x10) = *(SVECTOR *)(buf + 0x18);
            SetBleed((VECTOR *)buf, (SVECTOR *)(buf + 0x10), rand() % 0x10 + 0xf, 0xffff7e);
            i++;
        }
        SoundEx(item->owner->locate, 0x24);
        ppu = item->proc;
        if (ppu == 0)
            return;
        item->mode = 0xff;
        item->proc(item);
        DeleteConflict(item->locate);
        if (item->mode != 0)
        {
            AdtMessageBox(D_800121CC, item->type, (u32)item->mode);
        }
        item->owner = 0;
        item->proc = 0;
    }
    }
}
