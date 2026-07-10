#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * short FormatCard(void);
 *     MEMCARD.C:108, 8 src lines, frame 32 bytes, saved-reg mask 0x80000000
 *
 * Original parameters and locals (the demo build's register allocation may
 * differ from retail, but the COUNT and TYPES drive cc1's codegen and carry
 * over). A repeated name is a nested-block scope, not a duplicate:
 *     stack sp+16     long cmd
 *     stack sp+20     long result
 * END PSX.SYM */

/*
 * FormatCard — MemCardFormat on slot 0, then block on MemCardSync and return
 * its result truncated to a short. Structurally identical to ChkCard.c (see
 * that file for the shared-stack-slot note); only the kick-off call differs.
 */

extern s32 MemCardFormat(s32 chan);
extern s32 MemCardSync(s32 mode, s32 *cmd, s32 *result);

s16 FormatCard(void)
{
    s32 cmd;
    s32 result;

    result = MemCardFormat(0);
    MemCardSync(0, &cmd, &result);
    return result;
}
