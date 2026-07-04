#include "common.h"
#include "main.exe.h"

/* FUN_8004c024 (0x8004c024) — unconditional thunk for FUN_800566fc(); no
 * direct jal callers (indirect-call only). findsimilar ranks this ~0.57
 * against DisposeModel/DisposeOrnament (same prologue+jal+epilogue shape),
 * but the real asm has NO beqz null-guard here — trust the asm over the
 * similarity score. */

extern void FUN_800566fc(void);

void FUN_8004c024(void)
{
    FUN_800566fc();
}
