#include "common.h"
#include "main.exe.h"
#include "misc.h"

/*
 * Post-mission score/high-score screen (0x80054B48, 0x121C bytes).
 *
 * STATUS: NON_MATCHING -- 169 of 4636 bytes differ (round-17: permuter wins
 * verified + transcribed onto the round-16 human-structure base; the
 * byte-chased checkpoint before round 16 was 145, commit b9be819 -- that
 * draft and the full round 1-15 archaeology live in git history).
 *
 * ROUND-16 (Fable) -- THE HUMAN-STRUCTURE EXPERIMENT.  This file was
 * rewritten from scratch to the source shape the author demonstrably used
 * (matched sibling StageEndScreen.c is the template), then refined only
 * with human-plausible edits.  From-scratch human draft measured 696;
 * the refinements below took it to 187.  What the experiment established:
 *
 * HUMAN STRUCTURE THAT REPRODUCES TARGET BYTES (the payoff):
 * - DRAW_SCORE_NUMBER(sprite_, value_, type_, jump_, label_, x_, y_) is
 *   StageEndScreen's macro with a sprite parameter, transcribed.  Its
 *   mission incarnation is statement-wrapped: an outer do{}while(0)
 *   (carrier assign) + an inner do{}while(0) (head + label + digit loop),
 *   sign arm OUTSIDE both fences, all inside a plain brace declaring a
 *   per-site `negative`.  autorules measures unwrapping these macro fences
 *   at 220 -> 1004: they are the author's macro hygiene, not scaffolding.
 *   (StageEnd's own copy is brace-style; two files, two macro revisions.)
 * - The two-layer fence depth is what makes the /10 magic outrank
 *   numberSprite for $s5 (decimal-loop mults at depth 5 vs ns's colon refs).
 * - topY = -0x47 is an ORDINARY pre-loop variable, the analogue of
 *   StageEnd's top_y.  It loses allocation (every callee-saved reg is
 *   claimed in the render loop), keeps a REG_EQUIV constant, and reload
 *   rematerialises it per use: that IS the target's li t0/t2 constant
 *   shape at the y=-0x47 sites.  (resultX was believed to be its x-side
 *   twin through round 16; round 17 refuted the variable form -- see
 *   below -- and the x=0x66 sites are literals now.)
 * - ten = 10 lives per-site in the sign arms; numberSprite = &number is a
 *   block-local of the colon macro (+ fenced seed in draw-1, plain seed in
 *   draw-4): loop.c's movable chain then emits the target preheader
 *   magic/ten/ns exactly.  A FUNCTION-SCOPE single-set pointer is NEVER
 *   moved (measured twice; its set after the sign-arm join label is not
 *   even scanned as a movable) -- the block-local seeds are load-bearing.
 * - oldPad/pad/background are a small stack struct (memory-resident by
 *   construction, slots 0x180/0x184); insertedRank is a PLAIN s32 local
 *   that must SPILL to 0x188 (t1/t2/t3 reload fingerprint).  Reading it
 *   directly everywhere gives it enough refs to steal $s7, so the
 *   found/insertedAt copy-guards around the insert block are load-bearing.
 *   (A struct-member insertedRank gives v0-class reloads: refuted.)
 * - stageItem is s16 -- the natural type of an lh from D_8008ED50 -- and
 *   that alone reproduces the stock-tail accumulator registers the old
 *   draft bought with a brightness-alias scaffold (round-15's open
 *   micro-question answered).
 * - Draw-1 (enemies - bosses) and draw-4 are hand-expanded instances (the
 *   author treated the two ratio displays specially): draw-1 is
 *   signedValue-first with the round-12 `bosses` operand-birth seed, and
 *   both draw through the colon's numberSprite (move s2,s6).
 * - Sites 0/1/4 carry `GsOT *ot; ot = OTablePt;` before the u-store in
 *   digit loop and sign arm (the target loads OTablePt above the QImode
 *   u-store there -- FACT 3's pin means that order can only come from the
 *   source); sites 2,3,5-8, the row and the colons are plain calls (their
 *   target loads OTablePt at the call, below the store).
 * - The init loops and the row loop are GOTO loops (both re-refuted this
 *   round: do{}while init loops let loop.c hoist the rankSprites base into
 *   a register and delete the pivot recompute; a do{}while row loop deepens
 *   insertedRank/score-lui refs and scrambles s6-s8), and the rank/char
 *   pivot resets must be re-spelled through the bank, not initSprite.
 * - Number-init brightness stays the s16 single-set local (FACT 1+2).
 *   pulse is ONE shared s32 for medal+row (StageEnd's style); the medal
 *   block is plain StageEnd shape with the medalDraw copy straight-line
 *   after rcos and no else-arm.
 *
 * REMAINING SCAFFOLDING (honest; the only non-human constructs left):
 * - ONE do{}while(0) on `rankSpriteBase = rankSprites;` -- lifts its ref
 *   weight (regalloc.py: 4/148->540 vs ten 21/1418->592) so the pair
 *   allocates s7/s8 in target order; the banked draft used 4 nested
 *   fences for the same flip.  A combine-folded second use was tried and
 *   dies in cse before flow counts it.  The original's spelling: unknown.
 * - The empty do{}while(0) between the row char-sprite store and its
 *   GsSortSprite (pins the a0 arg copy below the r/g/b stores; without it
 *   the copy hoists and the stores re-canon onto a0).
 * - Draw-1's fenced numberSprite seed (round-10: unfenced, cse re-canons
 *   the carrier's loop uses onto ns and deletes the copy).
 *
 * ROUND-17 (Fable) -- what moved 187 -> 169, and what it refuted:
 * - The permuter (post -fno-builtin fix; run with --force-early past the
 *   37-block preflight, verified by transcribing the SEMANTIC delta only)
 *   found both wins; every other delta in its candidates was dead noise
 *   (dummy labels, brightness+=0, 11*(2*i)) -- bisect before believing.
 * - x = 0x66 is a LITERAL at all four result-column sites (2/5/7/8); the
 *   resultX variable is DELETED (-12B).  Round-16's "resultX loses
 *   allocation and remats as li t1" was HALF right: the variable does give
 *   the target's li t1,102, but its x-store then reads a remat scratch,
 *   is the only reorg-movable insn before the bgez, and steals the delay
 *   slot that must hold `move s3,zero` (sites 5/7/8).  The literal
 *   serializes x through v0 (li v0,102/sh/li v0,-53/sh), which pins the
 *   store via the v0 anti-dep exactly like the matching literal sites
 *   (draw-3/6), and the slot comes out right.  Cost: li reg field v0-vs-t1
 *   + li/sh interleave, 9B x3 + 4B at draw-2 (was 11B x3 + 10B).  Both
 *   basins measured on top of round-17 state: variable 181, literal 169.
 *   topY STAYS a variable (sites 0/2; unchanged, still load-bearing).
 * - Draw-0's hand expansion drops the INNER macro do{}while(0) (plain
 *   brace instead, -6B: the head li/lbu order cluster).  The other sites
 *   keep the two-layer fence; draw-0 is a per-site macro revision like
 *   draw-1/draw-4 (autorules' fence-unwrap L494 called it "invalid"
 *   because of the label inside; the permuter proved it unwrappable).
 *
 * RESIDUAL AT 169:
 * - entry pair b84/b98 (8B): li s3,128-vs-move a0,s0 -- target lands the
 *   leaf li in calculate_score's jal slot, ours the arg copy.  Same
 *   sched2-group/reorg-steal family as everything below.
 * - bank main + pivot pairs (65B, clusters 2-8): CLOSED categorically at
 *   the C level -- round-17 re-derived round 9's verdict with the full
 *   mechanism: constant-last (addu sp,s1; addiu 96) into a pointer needs
 *   an EXPAND_SUM birth (INDIRECT_REF deref only -- expr.c:6214 sends all
 *   non-EXPAND_SUM pointer PLUS to binop = base-first; c-typeck.c
 *   3038/3047/3113 folds &*p, &a[i], AND &p->field0 back to the plain sum,
 *   so no C pointer value can be born constant-last), while the InitSprite
 *   ARG must read a pre-existing register (any arg expression births its
 *   own base-first pair; M-form measured +24B, plain-&arr[i] form loses
 *   the pivot recompute to cse, -16B).  Target needs main=constant-last
 *   AND arg-from-that-value: unreachable.  Do not respell.
 * - three ot sites (36B, lbu 14(s2) below the OTablePt lui/lw, ours above)
 *   + draw-1 head rotation (21B) + colon-2 tail move/sb (8B) + x-li sites
 *   (27B) + draw-2 li regs (4B): one systematic sched2 signature (ours
 *   sinks the free store/leaf, target keeps C order; reorg then steals a
 *   different insn).  sched-deps/schedtrace show our side is deterministic
 *   (equal-pri group + potential-hazard pick, sched.c:2687); the upstream
 *   input-order lever was not found by hand or by four bounded permuter
 *   runs (187->175->169->plateau->plateau).
 *
 * ROUND-17 NEGATIVES (measured; do not repeat):
 * - ot-site statement swap (`ot = OTablePt;` above `baseU = sprite->u;`
 *   at the three digit loops): nullcheck NO-OP -- that load order is
 *   scheduler-owned, source order is invisible.  Same for x/y store
 *   statement order at sites 5/7/8 (nullcheck NO-OP).
 * - entry pair by statement position: rankColour=128 after i=0 -> 171;
 *   before init_score_stats -> 208.  Current position is the best basin.
 * - draw-1 without the `bosses` temp (direct enemies-bosses subtract,
 *   chasing the target's lbu65-before-lbu64) -> 184; round-12's seed
 *   stands even though the target loads 65 first.
 * - resultX restored as a variable on the round-17 base -> 181 (see the
 *   li-t1-vs-slot tradeoff above; the two goals need one more upstream
 *   lever nobody has found).
 * - autorules 169: no win among 177; --guided: none among 157 (deadline
 *   cut, all tried were worse or neutral).  Byte-NEUTRAL fence-unwraps
 *   exist at some macro sites (L553/L557/L627/L724-class, and L721 at
 *   169) -- neutral, not adopted.
 */

typedef struct
{
    u8 stageBosses;
    u8 stageEnemies;
    u8 findEnemies;
    u8 murders;
    u8 criticals;
    u8 friendHits;
    u8 pad[2];
    s32 clock;
} ScoreStats;

typedef struct
{
    u16 field0;
    u16 field2;
    u16 field4;
    u16 field6;
    u16 score;
    s16 grade;
} ScoreResult;

typedef struct
{
    u8 pad_000[0x44C];
    u8 characters[5];
    u8 grades[5];
    u8 pad_456[2];
    s32 scores[5];
} MissionScorePersistent;

typedef struct
{
    u16 oldPad;
    u16 pad;
    void *background;
} MissionScoreTail;

extern u8 CHOSEN_CHARACTER;
extern u8 CHOSEN_STAGE;
extern u8 CHOSEN_LANGUAGE;
extern u8 STAGE_LAYOUT_NUMBER;

extern char NUMBER_TIM_PATH[];
extern char *RANKS_ARCHIVE_PTRS[];
extern char *TRN_SPRITE_PTRS[];
extern char D_80013AA8[];
extern char D_80013AC0[];
extern u8 D_8001005C;

extern Sprite3D *ItemImage[];
extern s16 D_8008ED50[];

extern s32 GameClock;
extern s16 SkipFrame;
extern GsOT *OTablePt;

extern void init_score_stats(ScoreStats *stats);
extern ScoreResult *calculate_score(ScoreStats *stats, s32 stage);
extern u_long *FileRead(char *path);
extern void vfree(void *ptr);
extern u_long *get_tim_from_archive(u_long *archive, s32 index);
extern void InitSprite(GsIMAGE *image, GsSPRITE *sprite);
extern void LoadTIM(u_long *tim);
extern void _PlayMusic(s32 music, s32 mode);
extern u32 GetRealPad(s32 port);
extern void StartDrawing(void);
extern void DrawBG(void *background);
extern void EndDrawing(s32 arg);
extern void DisposeBG(void *background);
extern void GsSortSprite(GsSPRITE *sprite, GsOT *ot, s32 priority);
extern void FUN_800515b0(GsSPRITE *number, s32 value, s16 x, s32 y,
                         s32 mode);
extern s32 rcos(s32 angle);
extern s32 rsin(s32 angle);
extern void FadeOutDirect(s16 time, s16 attribute, u8 r, u8 g, u8 b);
extern void FUN_80038ce0(void);
extern void FUN_800514d8(void);
extern void FUN_8004f6c0(s32 screen);

static inline void InitScoreSprite(u_long *tim, GsIMAGE *image,
                                   GsSPRITE *sprite)
{
    GetTIMInfo(tim, image);
    InitSprite(image, sprite);
}

/* The persistent high-score block, addressed by CONSTANT rather than through a
 * pointer local (the constant form sorts the index term first in the element
 * address, the target's operand order -- round 9). */
#define SCORE_STATE ((MissionScorePersistent *)0x80010000)

#define DRAW_SCORE_NUMBER(sprite_, value_, type_, jump_, label_,           \
        x_, y_)                                                            \
    {                                                                      \
        s32 negative;                                                      \
                                                                           \
    do                                                                     \
    {                                                                      \
        sprite = (sprite_);                                                \
        do                                                                 \
        {                                                                  \
        s32 dividend;                                                      \
        s32 remainder;                                                     \
        s32 quotient;                                                      \
        u32 value;                                                         \
        s16 signedValue;                                                   \
                                                                           \
        value = (value_);                                                  \
        signedValue = (type_)value;                                        \
        sprite->x = (x_);                                                  \
        sprite->y = (y_);                                                  \
        if (jump_)                                                         \
        {                                                                  \
            if (signedValue < 0)                                           \
            {                                                              \
                value = -signedValue;                                      \
                negative = 1;                                              \
                goto label_;                                               \
            }                                                              \
            else                                                           \
            {                                                              \
                negative = 0;                                              \
            }                                                              \
        }                                                                  \
        else                                                               \
        {                                                                  \
            negative = 0;                                                  \
            if (signedValue >= 0)                                          \
                goto label_;                                               \
            value = -signedValue;                                          \
            negative = 1;                                                  \
        }                                                                  \
label_:                                                                    \
        do                                                                 \
        {                                                                  \
            dividend = (s16)value;                                         \
            quotient = dividend / 10;                                      \
            remainder = dividend % 10;                                     \
            baseU = sprite->u;                                             \
            sprite->u = baseU + (s16)remainder * sprite->w;                \
            GsSortSprite(sprite, OTablePt, 0);                             \
            sprite->x -= 12;                                               \
            value = quotient;                                              \
            quotient <<= 16;                                               \
            sprite->u = baseU;                                             \
        } while (quotient != 0);                                           \
        } while (0);                                                       \
    } while (0);                                                           \
    if (negative != 0)                                                     \
    {                                                                      \
        u32 signBaseU;                                                     \
        s32 ten;                                                           \
                                                                           \
        ten = 10;                                                          \
        signBaseU = sprite->u;                                             \
        sprite->u = signBaseU + sprite->w * ten;                           \
        GsSortSprite(sprite, OTablePt, 0);                                 \
        sprite->u = signBaseU;                                             \
    }                                                                      \
    }

#define DRAW_SCORE_COLON(x_)                                               \
    do                                                                     \
    {                                                                      \
        u8 oldU;                                                           \
        s32 colonDigit;                                                    \
        GsSPRITE *numberSprite;                                            \
                                                                           \
        numberSprite = &number;                                            \
        number.x = (x_);                                                   \
        oldU = numberSprite->u;                                            \
        colonDigit = 12;                                                   \
        numberSprite->u = oldU + numberSprite->w * colonDigit;             \
        GsSortSprite(numberSprite, OTablePt, 0);                           \
        numberSprite->u = oldU;                                            \
    } while (0)

#ifndef NON_MATCHING
INCLUDE_ASM("config/../.shake/gen/main.exe/asm/nonmatchings/mission_score_screen", mission_score_screen);
#else
void mission_score_screen(void)
{
    GsSPRITE number;
    ScoreStats stats;
    ScoreResult result;
    GsSPRITE rankSprites[5];
    GsSPRITE characterSprites[2];
    GsIMAGE image;
    MissionScoreTail tail;
    u_long *tim;
    u_long *archive;
    GsSPRITE *initSprite;
    GsSPRITE *sprite;
    GsSPRITE *medal;
    GsSPRITE *medalDraw;
    GsSPRITE *rowSprite;
    PersistentState *statePtr;
    GsSPRITE *rankSpriteBase;
    s16 i;
    s16 newPress;
    s16 brightness;
    s32 pulse;
    s16 stageItem;
    s32 goNext;
    s32 rankColour;
    u32 baseU;
    s32 insertedRank;
    s32 topY;

    tail.oldPad = 0;
    init_score_stats(&stats);
    rankColour = 128;
    i = 0;
    result = *calculate_score(&stats, CHOSEN_STAGE);

    tim = FileRead(NUMBER_TIM_PATH);
    {
        GsSPRITE *initNumber = &number;

        InitScoreSprite(tim, &image, initNumber);
        initNumber->attribute |= 0x50000000;
        initNumber->x = -140;
        initNumber->y = -40;
        brightness = 128;
        initNumber->r = brightness;
        initNumber->g = brightness;
        initNumber->b = brightness;
        initNumber->mx = initNumber->w >> 1;
        initNumber->my = initNumber->h >> 1;
    }
    number.mx = 0;
    number.my = 0;
    LoadTIMAndFree(tim);
    topY = -0x47;
    number.w = 12;

    /* The init loops stay in the goto shape: real do{}while loops here hand
     * the bank addressing to loop.c, which hoists the &rankSprites base into
     * a register (addu s0,s5,v0) where the target recomputes it inline, lets
     * cse delete the pivot recompute, and unifies the two 0x50000000
     * constants -- all measured this round.  The goto shape is the
     * byte-verified conclusion, not scaffolding. */
    {
        u32 attributeMask;

        archive = FileRead(RANKS_ARCHIVE_PTRS[CHOSEN_LANGUAGE]);
        attributeMask = 0x50000000;
score_rank_sprite_init_loop:
        {
            u32 attribute;
            u32 width;

            tim = get_tim_from_archive(archive, i);
            initSprite = ((GsSPRITE *)((u8 *)(rankSprites) + (i) * sizeof(GsSPRITE)));
            InitScoreSprite(tim, &image, initSprite);
            attribute = initSprite->attribute;
            initSprite->x = -160;
            initSprite->y = -120;
            width = initSprite->w;
            initSprite->r = rankColour;
            initSprite->g = rankColour;
            initSprite->b = rankColour;
            initSprite->mx = width >> 1;
            initSprite->attribute = attribute | attributeMask;
            initSprite->my = initSprite->h >> 1;
            ((GsSPRITE *)((u8 *)(rankSprites) + (i) * sizeof(GsSPRITE)))->mx = 0;
            ((GsSPRITE *)((u8 *)(rankSprites) + (i) * sizeof(GsSPRITE)))->my = 0;
            LoadTIM(tim);
        }
        i++;
        if (i < 5)
            goto score_rank_sprite_init_loop;
    }

    {
        s32 characterColour;

        i = 0;
        characterColour = 128;
score_character_sprite_init_loop:
        {
            u32 attribute;
            u32 width;
            u32 height;

            tim = get_tim_from_archive(archive, i + 5);
            initSprite = &characterSprites[i];
            InitScoreSprite(tim, &image, initSprite);
            /* Retail keeps this dead attribute load (value overwritten
             * before any use) -- a leftover of the rank-loop copy. */
            attribute = *(u32 volatile *)&initSprite->attribute;
            width = initSprite->w;
            height = initSprite->h;
            initSprite->x = -160;
            initSprite->y = -120;
            initSprite->g = initSprite->r = characterColour;
            initSprite->b = characterColour;
            initSprite->mx = width >> 1;
            initSprite->my = height >> 1;
            ((GsSPRITE *)((u8 *)(characterSprites) + (i) * sizeof(GsSPRITE)))->mx = 0;
            ((GsSPRITE *)((u8 *)(characterSprites) + (i) * sizeof(GsSPRITE)))->my = 0;
            LoadTIM(tim);
        }
        i++;
        if (i < 2)
            goto score_character_sprite_init_loop;
    }
    vfree(archive);

    tim = FileRead(TRN_SPRITE_PTRS[CHOSEN_LANGUAGE]);
    tail.background = FUN_8004f4f8(tim);
    vfree(tim);

    for (i = 0; i < 5; i++)
    {
        if (SCORE_STATE->scores[i] == 0)
        {
            SCORE_STATE->scores[i] = 0x1A5C2;
            SCORE_STATE->characters[i] = 0;
            SCORE_STATE->grades[i] = 0;
        }
    }

    insertedRank = -1;
    for (i = 0; i < 5; i++)
    {
        if (SCORE_STATE->grades[i] < result.grade)
        {
            insertedRank = i;
            break;
        }
        if (result.grade == SCORE_STATE->grades[i] &&
            stats.clock < SCORE_STATE->scores[i])
        {
            insertedRank = i;
            break;
        }
    }

    {
        s32 found = insertedRank;

        if (found >= 0)
        {
            i = 4;
            if (found < 4)
            {
                do
                {
                    SCORE_STATE->scores[i] = SCORE_STATE->scores[i - 1];
                    SCORE_STATE->characters[i] =
                        SCORE_STATE->characters[i - 1];
                    SCORE_STATE->grades[i] = SCORE_STATE->grades[i - 1];
                    i--;
                } while (insertedRank < (s16)i);
            }

            {
                s32 insertedAt = insertedRank;

                SCORE_STATE->scores[insertedAt] = stats.clock;
                SCORE_STATE->characters[insertedAt] =
                    ((PersistentState *)SCORE_STATE)->chr;
                SCORE_STATE->grades[insertedAt] = result.grade;
            }
        }
    }

    _PlayMusic(12, 1);
    for (;;)
    {
        u16 pad = GetRealPad(0);
        newPress = pad & (pad ^ tail.oldPad);
        tail.oldPad = pad;
        if (newPress & 0x20)
        {
            goNext = 0;
            break;
        }
        if (newPress & 0x800)
        {
            goNext = 1;
            break;
        }

        StartDrawing();
        DrawBG(tail.background);
        FUN_800515b0(&number, stats.clock, 0x46, -0x61, 1);

        {
            s32 negative;

        do
        {
            sprite = &number;
        {
            s32 dividend;
            s32 remainder;
            s32 quotient;
            u32 value;
            s16 signedValue;

            value = (stats.criticals);
            signedValue = (s32)value;
            sprite->x = 0x16;
            sprite->y = topY;
            if (signedValue < 0)
            {
                value = -signedValue;
                negative = 1;
                goto number_0;
            }
            else
            {
                negative = 0;
            }
number_0:
            do
            {
                GsOT *ot;

                dividend = (s16)value;
                quotient = dividend / 10;
                remainder = dividend % 10;
                baseU = sprite->u;
                ot = OTablePt;
                sprite->u = baseU + (s16)remainder * sprite->w;
                GsSortSprite(sprite, ot, 0);
                sprite->x -= 12;
                value = quotient;
                quotient <<= 16;
                sprite->u = baseU;
            } while (quotient != 0);
        }
        } while (0);
        if (negative != 0)
        {
            u32 signBaseU;
            s32 ten;
            GsOT *ot;

            ten = 10;
            signBaseU = sprite->u;
            ot = OTablePt;
            sprite->u = signBaseU + sprite->w * ten;
            GsSortSprite(sprite, ot, 0);
            sprite->u = signBaseU;
        }
        }
        DRAW_SCORE_COLON(0x1F);
        {
            s32 negative;

        do
        {
            GsSPRITE *numberSprite;

            do {
                numberSprite = &number;
            } while (0);
        do
        {
            s32 dividend;
            s32 remainder;
            s32 quotient;
            u16 value;
            s32 signedValue;
            s32 bosses;

            bosses = stats.stageBosses;
            signedValue = (stats.stageEnemies - bosses);
            value = signedValue;
            numberSprite->x = 0x2F;
            numberSprite->y = topY;
            sprite = numberSprite;
            if (signedValue < 0)
            {
                value = -signedValue;
                negative = 1;
                goto number_1;
            }
            else
            {
                negative = 0;
            }
number_1:
            do
            {
                GsOT *ot;

                dividend = (s16)value;
                quotient = dividend / 10;
                remainder = dividend % 10;
                baseU = sprite->u;
                ot = OTablePt;
                sprite->u = baseU + (s16)remainder * sprite->w;
                GsSortSprite(sprite, ot, 0);
                sprite->x -= 12;
                value = quotient;
                quotient <<= 16;
                sprite->u = baseU;
            } while (quotient != 0);
        } while (0);
        } while (0);
        if (negative != 0)
        {
            u32 signBaseU;
            s32 ten;
            GsOT *ot;

            ten = 10;
            signBaseU = sprite->u;
            ot = OTablePt;
            sprite->u = signBaseU + sprite->w * ten;
            GsSortSprite(sprite, ot, 0);
            sprite->u = signBaseU;
        }
        }
        DRAW_SCORE_NUMBER(&number, result.field0, s16, 1, number_2,
            0x66, topY);

        DRAW_SCORE_NUMBER(&number, stats.murders, s32, 0, number_3,
            0x16, -0x35);
        DRAW_SCORE_COLON(0x1F);
        {
            s32 negative;

        do
        {
            GsSPRITE *numberSprite;

            numberSprite = &number;
            sprite = (numberSprite);
        do
        {
            s32 dividend;
            s32 remainder;
            s32 quotient;
            u32 value;
            s16 signedValue;

            value = (stats.stageEnemies);
            signedValue = (s32)value;
            numberSprite->x = 0x2F;
            numberSprite->y = -0x35;
            negative = 0;
            if (signedValue >= 0)
                goto number_4;
            value = -signedValue;
            negative = 1;
number_4:
            do
            {
                GsOT *ot;

                dividend = (s16)value;
                quotient = dividend / 10;
                remainder = dividend % 10;
                baseU = sprite->u;
                ot = OTablePt;
                sprite->u = baseU + (s16)remainder * sprite->w;
                GsSortSprite(sprite, ot, 0);
                sprite->x -= 12;
                value = quotient;
                quotient <<= 16;
                sprite->u = baseU;
            } while (quotient != 0);
        } while (0);
        } while (0);
        if (negative != 0)
        {
            u32 signBaseU;
            s32 ten;
            GsOT *ot;

            ten = 10;
            signBaseU = sprite->u;
            ot = OTablePt;
            sprite->u = signBaseU + sprite->w * ten;
            GsSortSprite(sprite, ot, 0);
            sprite->u = signBaseU;
        }
        }
        DRAW_SCORE_NUMBER(&number, result.field2, s16, 0, number_5,
            0x66, -0x35);

        DRAW_SCORE_NUMBER(&number, stats.findEnemies, s32, 0, number_6,
            0x23, -0x24);
        DRAW_SCORE_NUMBER(&number, result.field6, s16, 0, number_7,
            0x66, -0x24);

        DRAW_SCORE_NUMBER(&number, result.score, s16, 0, number_8,
            0x66, -0x12);

        if (result.grade == RANK_GRAND_MASTER)
        {
            medal = &ItemImage[D_8008ED50[CHOSEN_STAGE]]->sprite;
            medal->x = 0x8A;
            medal->y = -0xE;
            medal->scalex = 0x1000;
            medal->scaley = 0x1000;
            pulse = rcos((GameClock << 12) / 90) * 80;
            medalDraw = medal;
            if (pulse < 0)
            {
                pulse += 0xFFF;
            }
            medalDraw->r = medalDraw->g = medalDraw->b = (pulse >> 12) + 0x7F;
            GsSortSprite(medalDraw, OTablePt, 1);
        }

        i = 0;
        rowSprite = &number;
        /* One loop-note pair on this def: it lifts the ref weight above
         * ten's 592 so the pair allocates s7/s8 in the target's order (the
         * banked draft bought the same flip with four nested fences).  How
         * the original spelled this remains open. */
        do {
            rankSpriteBase = rankSprites;
        } while (0);
score_row_loop:
        {
            s32 negative;

            do
            {
                s32 dividend;
                s32 remainder;
                s32 quotient;
                u16 value;
                s16 signedValue;
                s32 widenedValue;

                signedValue = (i + 1);
                value = signedValue;
                rowSprite->x = -0x8F;
                rowSprite->y = i * 0x16 + 0x18;
                widenedValue = signedValue;
                if (widenedValue < 0)
                {
                    value = -widenedValue;
                    negative = 1;
                    goto number_row;
                }
                else
                {
                    negative = 0;
                }
number_row:
                do
                {
                    dividend = (s16)value;
                    quotient = dividend / 10;
                    remainder = dividend % 10;
                    baseU = rowSprite->u;
                    rowSprite->u = baseU + (s16)remainder * rowSprite->w;
                    GsSortSprite(rowSprite, OTablePt, 0);
                    rowSprite->x -= 12;
                    value = quotient;
                    quotient <<= 16;
                    rowSprite->u = baseU;
                } while (quotient != 0);
            } while (0);
            if (negative != 0)
            {
                u32 signBaseU;
                s32 ten;

                ten = 10;
                signBaseU = rowSprite->u;
                rowSprite->u = signBaseU + rowSprite->w * ten;
                GsSortSprite(rowSprite, OTablePt, 0);
                rowSprite->u = signBaseU;
            }
            FUN_800515b0(&number, SCORE_STATE->scores[i],
                         0x79, i * 0x16 + 0x18, 1);

            sprite = &characterSprites[SCORE_STATE->characters[i]];
            sprite->x = -0x79;
            sprite->y = i * 0x16 + 0x16;
            if (i == insertedRank)
            {
                pulse = rsin((GameClock << 12) / 90) * 60;
                if (pulse < 0)
                {
                    pulse += 0xFFF;
                }
                pulse = (pulse >> 12) + 100;
            }
            else
            {
                pulse = 0x80;
            }
            sprite->r = sprite->g = sprite->b = pulse;
            do {
            } while (0);
            GsSortSprite(sprite, OTablePt, 1);

            {
                GsSPRITE *rankSprite =
                    &rankSpriteBase[SCORE_STATE->grades[i]];

                rankSprite->r = rankSprite->g = rankSprite->b = 0x7F;
                rankSprite->scalex = rankSprite->scaley = 0xB33;
                rankSprite->x = -0x2F;
                rankSprite->y = i * 0x16 + 0x16;
                GsSortSprite(rankSprite, OTablePt, 1);
            }
        }
        i++;
        if (i < 3)
        {
            goto score_row_loop;
        }

        SkipFrame = 2;
        EndDrawing(0);
    }

    if (result.grade == RANK_GRAND_MASTER)
    {
        PersistentState *state = (PersistentState *)0x80010000;

        stageItem = D_8008ED50[state->stage];
        if (state->stock[stageItem + (state->chr << 5)] == 0xFE)
        {
            state->stock[stageItem + (state->chr << 5)] += 3;
        }
        stageItem = D_8008ED50[state->stage];
        if (state->backup[stageItem] == 0xFE)
        {
            state->backup[stageItem] += 3;
        }
    }

    FadeOutDirect(0x20, 2, 8, 8, 8);
    FUN_80038ce0();
    /* Single-use constant pointer: unfenced, cse folds the address into the
     * store (lui at / sb 6(at)) where the target holds the pointer in s0
     * from an early lui (measured this round).  The fence keeps the pointer
     * a real value; how the original spelled this is still unknown. */
    do {
        statePtr = (PersistentState *)0x80010000;
    } while (0);
    if (D_8001005C != 0)
    {
        LoadTIMAndFree(PathFileRead(D_80013AA8, D_80013AC0));
        FUN_800514d8();
    }
    DisposeBG(tail.background);

    if (goNext == 1)
    {
        FUN_8004f6c0(0x11);
    }
    else
    {
        statePtr->layout = 0xFF;
        FUN_8004f6c0(0x10);
    }
}

#undef DRAW_SCORE_NUMBER
#undef DRAW_SCORE_COLON
#undef SCORE_STATE

#endif
