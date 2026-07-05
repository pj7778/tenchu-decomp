#include "common.h"
#include "main.exe.h"

/*
 * AfsFileSize (0x8005f008) — TAFS file-size accessor: `handle` (TAFS *) is
 * taken but never used ($a0 dead in the asm — confirmed by m2c); the real
 * work is on the second parameter, a `TAFSFileHandle *` in $a1 (see
 * TAFS/TAFSFileHandle in AfsInit.c for the proven layout), returning
 * fh->info->size — the directory-entry size cached behind the handle's
 * TAFSElement. Ghidra mistypes the second parameter `TAFSElement *element`
 * and renders the double-dereference as raw arithmetic
 * (`*(undefined4 *)(element->size + 8)`); m2c's raw `arg1->unk8->unk8`
 * disambiguates it as two pointer hops, matching TAFSFileHandle.info (0x8)
 * then TAFSElement.size (0x8) — same struct-name-distrust rule as AfsClose.
 * A NULL handle reports via AdtMessageBox() and returns 0 (not -1, unlike
 * the other four CD/AFS wrappers in this batch).
 *
 * Guard-clause polarity: written first in source per the cd_close/AfsClose
 * precedent (both if-arms return, so the literal if-body lands at the
 * branch target next to the epilogue, not the fallthrough).
 */

typedef struct TAFS TAFS;
typedef struct TAFSElement TAFSElement;
typedef struct TAFSFileHandle TAFSFileHandle;

struct TAFSElement
{
    u16 flag;    /* 0x0 */
    u32 pos;     /* 0x4 */
    u32 size;    /* 0x8 */
    u32 psize;   /* 0xC */
    u8 name[20]; /* 0x10 */
};

struct TAFSFileHandle
{
    s32 flagUse;       /* 0x0 */
    u32 pos;           /* 0x4 */
    TAFSElement *info; /* 0x8 */
};

extern void AdtMessageBox(char *fmt, ...);
extern char D_800149B4[]; /* "AfsFileSize: invalid handle" — lives in this
                            * TU's unsplit data blob (splat auto-symbol),
                            * same pattern as AfsInit's D_80014944. */

int AfsFileSize(TAFS *handle, TAFSFileHandle *fh)
{
    if (fh == 0) {
        AdtMessageBox(D_800149B4);
        return 0;
    }
    return fh->info->size;
}
