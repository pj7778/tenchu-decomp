#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 *
 * Globals it touches, as the original declared them:
 *     extern struct TCdaStatus CdaStatus;
 * END PSX.SYM */

typedef struct
{
    u8 pad[0x20];
    s32 quiet;
} AdtFnt;

extern AdtFnt D_8008F1B8;

s32 AdtQuiet(s32 quiet)
{
    s32 old = D_8008F1B8.quiet;
    D_8008F1B8.quiet = quiet;
    return old;
}
