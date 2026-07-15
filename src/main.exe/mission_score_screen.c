#include "common.h"
#include "main.exe.h"
#include "misc.h"

/*
 * Post-mission score/high-score screen (0x80054B48, 0x121C bytes).
 *
 * STATUS: NON_MATCHING -- 2654 of 4636 bytes differ.  The guarded draft has
 * the exact target length (1159 instructions) and frame size (0x1B8), with
 * 53/53 conditional branches, 4/8 unconditional jumps, 60/60 calls, and one
 * return.  No retail-name record was found in PSX.SYM or the demo symbol
 * export.
 *
 * The local aggregates reproduce the original stack layout.  In particular,
 * the packed ScoreResult copy starts at sp+50, the two sprite banks start at
 * sp+60/sp+118, and the one GsIMAGE scratch is reused at sp+160 by all seven
 * sprite initialisations.
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
    s16 score;
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
    s32 insertedRank;
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

extern u8 D_8001044C[5];
extern u8 D_80010451[5];
extern s32 D_80010458[5];
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

/* The retail code recomputes each bank address for the final pivot writes. */
#define SCORE_SPRITE_AT(bank_, index_)                                       \
    ((GsSPRITE *)((u8 *)(bank_) + (index_) * sizeof(GsSPRITE)))

/* This is deliberately a macro.  Retail repeats the decimal loop at every
 * call site, with fresh arithmetic identities but shared sign/base-U state. */
#define DRAW_SCORE_NUMBER(value_, type_, jump_, label_, sprite_, x_, y_)     \
    do                                                                       \
    {                                                                        \
        s32 dividend;                                                        \
        s32 remainder;                                                       \
        s32 quotient;                                                        \
        u32 value;                                                           \
        s16 signedValue;                                                     \
        GsSPRITE *drawnSprite;                                               \
                                                                             \
        drawnSprite = (sprite_);                                             \
        value = (value_);                                                    \
        signedValue = (type_)value;                                          \
        drawnSprite->x = (x_);                                               \
        drawnSprite->y = (y_);                                               \
        if (jump_)                                                           \
        {                                                                    \
            if (signedValue < 0)                                             \
            {                                                                \
                value = -signedValue;                                        \
                negative = 1;                                                \
                goto label_;                                                 \
            }                                                                \
            else                                                             \
            {                                                                \
                negative = 0;                                                \
            }                                                                \
        }                                                                    \
        else                                                                 \
        {                                                                    \
            negative = 0;                                                    \
            if (signedValue >= 0)                                            \
                goto label_;                                                 \
            value = -signedValue;                                            \
            negative = 1;                                                    \
        }                                                                    \
label_:                                                                      \
        do                                                                   \
        {                                                                    \
            dividend = (s16)value;                                           \
            quotient = dividend / 10;                                        \
            remainder = dividend % 10;                                       \
            baseU = drawnSprite->u;                                          \
            drawnSprite->u = baseU + (s16)remainder * drawnSprite->w;        \
            GsSortSprite(drawnSprite, OTablePt, 0);                           \
            drawnSprite->x -= 12;                                            \
            value = quotient;                                                \
            quotient <<= 16;                                                 \
            drawnSprite->u = baseU;                                          \
        } while (quotient != 0);                                             \
        if (negative != 0)                                                   \
        {                                                                    \
            u32 signBaseU;                                                   \
                                                                             \
            signBaseU = drawnSprite->u;                                      \
            drawnSprite->u = signBaseU + drawnSprite->w * ten;               \
            GsSortSprite(drawnSprite, OTablePt, 0);                           \
            drawnSprite->u = signBaseU;                                      \
        }                                                                    \
    } while (0)

#define DRAW_SCORE_COLON(sprite_, x_)                                        \
    do                                                                       \
    {                                                                        \
        u8 oldU;                                                             \
        s32 colonDigit;                                                      \
                                                                             \
        (sprite_)->x = (x_);                                                 \
        oldU = (sprite_)->u;                                                 \
        colonDigit = 12;                                                     \
        (sprite_)->u = oldU + (sprite_)->w * colonDigit;                     \
        GsSortSprite((sprite_), OTablePt, 0);                                 \
        (sprite_)->u = oldU;                                                 \
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
    register u_long *tim;
    register u_long *archive;
    register GsSPRITE *sprite;
    register GsSPRITE *initSprite;
    register GsSPRITE *numberSprite;
    register GsSPRITE *medal;
    register u8 *unlock;
    register s16 i;
    register u16 rowValue;
    register s16 newPress;
    register s32 brightness;
    register s32 stageItem;
    register s32 goNext;
    s32 ten;
    u32 baseU;
    register s32 negative;

    tail.oldPad = 0;
    init_score_stats(&stats);
    result = *calculate_score(&stats, CHOSEN_STAGE);

    tim = FileRead(NUMBER_TIM_PATH);
    numberSprite = &number;
    InitScoreSprite(tim, &image, numberSprite);
    numberSprite->attribute |= 0x50000000;
    numberSprite->x = -140;
    numberSprite->y = -40;
    numberSprite->r = 128;
    numberSprite->g = 128;
    numberSprite->b = 128;
    numberSprite->mx = numberSprite->w >> 1;
    numberSprite->my = numberSprite->h >> 1;
    number.mx = 0;
    number.my = 0;
    LoadTIMAndFree(tim);
    number.w = 12;

    archive = FileRead(RANKS_ARCHIVE_PTRS[CHOSEN_LANGUAGE]);
    for (i = 0; i < 5; i++)
    {
        u32 attribute;
        u32 width;

        tim = get_tim_from_archive(archive, i);
        initSprite = &rankSprites[i];
        InitScoreSprite(tim, &image, initSprite);
        attribute = initSprite->attribute;
        initSprite->x = -160;
        initSprite->y = -120;
        width = initSprite->w;
        initSprite->r = 128;
        initSprite->g = 128;
        initSprite->b = 128;
        initSprite->mx = width >> 1;
        initSprite->attribute = attribute | 0x50000000;
        initSprite->my = initSprite->h >> 1;
        SCORE_SPRITE_AT(rankSprites, i)->mx = 0;
        SCORE_SPRITE_AT(rankSprites, i)->my = 0;
        LoadTIM(tim);
    }

    {
        register s32 characterColour = 128;

        i = 0;
score_character_sprite_init_loop:
        {
            u32 width;
            u32 height;

            tim = get_tim_from_archive(archive, i + 5);
            initSprite = &characterSprites[i];
            InitScoreSprite(tim, &image, initSprite);
            width = initSprite->w;
            height = initSprite->h;
            initSprite->x = -160;
            initSprite->y = -120;
            initSprite->r = initSprite->g = characterColour;
            initSprite->b = characterColour;
            initSprite->mx = width >> 1;
            SCORE_SPRITE_AT(characterSprites, i)->my = height >> 1;
            SCORE_SPRITE_AT(characterSprites, i)->mx = 0;
            SCORE_SPRITE_AT(characterSprites, i)->my = 0;
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

    {
        register MissionScorePersistent *initState =
            (MissionScorePersistent *)0x80010000;

        for (i = 0; i < 5; i++)
        {
            if (initState->scores[i] == 0)
            {
                initState->scores[i] = 0x1A5C2;
                initState->characters[i] = 0;
                initState->grades[i] = 0;
            }
        }
    }

    tail.insertedRank = -1;
    {
        register MissionScorePersistent *rankState =
            (MissionScorePersistent *)0x80010000;

        for (i = 0; i < 5; i++)
        {
            if (rankState->grades[i] < result.grade ||
                (rankState->grades[i] == result.grade &&
                 stats.clock < rankState->scores[i]))
            {
                tail.insertedRank = i;
                break;
            }
        }
    }

    if (tail.insertedRank >= 0)
    {
        register MissionScorePersistent *scoreState =
            (MissionScorePersistent *)0x80010000;

        i = 4;
        if (tail.insertedRank < 4)
        {
            do
            {
                scoreState->scores[i] = scoreState->scores[i - 1];
                scoreState->characters[i] = scoreState->characters[i - 1];
                i--;
                scoreState->grades[i + 1] = scoreState->grades[i];
            } while (tail.insertedRank < (s16)i);
        }

        scoreState->scores[tail.insertedRank] = stats.clock;
        scoreState->characters[tail.insertedRank] = CHOSEN_CHARACTER;
        scoreState->grades[tail.insertedRank] = result.grade;
    }

    _PlayMusic(12, 1);
    ten = 10;
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

        DRAW_SCORE_NUMBER(stats.criticals, s32, 1, score_number_0,
                          &number, 0x16, -0x47);
        DRAW_SCORE_COLON(numberSprite, 0x1F);
        DRAW_SCORE_NUMBER(stats.stageEnemies - stats.stageBosses,
                          s32, 1, score_number_1, &number, 0x2F, -0x47);
        DRAW_SCORE_NUMBER(result.field0, s16, 1, score_number_2,
                          &number, 0x66, -0x47);

        DRAW_SCORE_NUMBER(stats.murders, s32, 0, score_number_3,
                          &number, 0x16, -0x35);
        DRAW_SCORE_COLON(numberSprite, 0x1F);
        DRAW_SCORE_NUMBER(stats.stageEnemies, s32, 0, score_number_4,
                          &number, 0x2F, -0x35);
        DRAW_SCORE_NUMBER(result.field2, s16, 0, score_number_5,
                          &number, 0x66, -0x35);

        DRAW_SCORE_NUMBER(stats.findEnemies, s32, 0, score_number_6,
                          &number, 0x23, -0x24);
        DRAW_SCORE_NUMBER(result.field6, s16, 0, score_number_7,
                          &number, 0x66, -0x24);
        DRAW_SCORE_NUMBER(result.score, s16, 0, score_number_8,
                          &number, 0x66, -0x12);

        if (result.grade == RANK_GRAND_MASTER)
        {
            medal = &ItemImage[D_8008ED50[CHOSEN_STAGE]]->sprite;
            medal->x = 0x8A;
            medal->y = -0xE;
            medal->scalex = 0x1000;
            medal->scaley = 0x1000;
            brightness = rcos((GameClock << 12) / 90) * 80;
            if (brightness < 0)
            {
                brightness += 0xFFF;
            }
            brightness = (brightness >> 12) + 0x7F;
            medal->r = brightness;
            medal->g = brightness;
            medal->b = brightness;
            GsSortSprite(medal, OTablePt, 1);
        }

        {
            GsSPRITE *rankSpriteBase = rankSprites;

            i = 0;
score_row_loop:
            {
                register MissionScorePersistent *scoreState;
                register MissionScorePersistent *characterState;
                register MissionScorePersistent *rankState;

                rowValue = i + 1;
                DRAW_SCORE_NUMBER(rowValue, s16, 1, score_row_number,
                                  &number, -0x8F, i * 0x16 + 0x18);
                scoreState = (MissionScorePersistent *)0x80010000;
                FUN_800515b0(&number, scoreState->scores[i],
                             0x79, i * 0x16 + 0x18, 1);

                characterState = (MissionScorePersistent *)0x80010000;
                sprite = &characterSprites[characterState->characters[i]];
                sprite->x = -0x79;
                sprite->y = i * 0x16 + 0x16;
                brightness = 0x80;
                if (i == tail.insertedRank)
                {
                    brightness = rsin((GameClock << 12) / 90) * 60;
                    if (brightness < 0)
                    {
                        brightness += 0xFFF;
                    }
                    brightness = (brightness >> 12) + 100;
                }
                sprite->r = brightness;
                sprite->g = brightness;
                do {
                } while (0);
                sprite->b = brightness;
                GsSortSprite(sprite, OTablePt, 1);

                rankState = (MissionScorePersistent *)0x80010000;
                {
                    register GsSPRITE *rankSprite =
                        &rankSpriteBase[rankState->grades[i]];
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
        }

        SkipFrame = 2;
        EndDrawing(0);
    }

    if (result.grade == RANK_GRAND_MASTER)
    {
        register PersistentState *state = (PersistentState *)0x80010000;

        stageItem = D_8008ED50[state->stage];
        unlock = &state->stock[stageItem + (state->chr << 5)];
        if (*unlock == 0xFE)
        {
            *unlock += 3;
        }
        stageItem = D_8008ED50[state->stage];
        if (state->backup[stageItem] == 0xFE)
        {
            state->backup[stageItem] += 3;
        }
    }

    FadeOutDirect(0x20, 2, 8, 8, 8);
    FUN_80038ce0();
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
        STAGE_LAYOUT_NUMBER = 0xFF;
        FUN_8004f6c0(0x10);
    }
}
#endif
