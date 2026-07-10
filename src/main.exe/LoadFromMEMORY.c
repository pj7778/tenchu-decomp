#include "common.h"
#include "main.exe.h"

/* BEGIN PSX.SYM — the original source's own facts, from the demo disc's
 * debug symbols. Regenerate with `tools/symnote.py --write`; see
 * docs/psx-sym.md. Do not hand-edit.
 *
 * unsigned long * LoadFromMEMORY(unsigned char *filename);
 *     FILEIO.C:247, 45 src lines, frame 64 bytes, saved-reg mask 0x801f0000
 *
 * Original parameters and locals (the demo build's register allocation may
 * differ from retail, but the COUNT and TYPES drive cc1's codegen and carry
 * over). A repeated name is a nested-block scope, not a duplicate:
 *     param $s4       unsigned char * filename
 *     reg   $s1       unsigned long * vmp
 *     reg   $s2       unsigned long * data
 *     stack sp+16     unsigned char [24] name
 *     reg   $s0       short i
 *     reg   $a2       short j
 *     reg   $s4       unsigned char * filename
 *     reg   $s2       int fd
 *     reg   $s3       int size
 *     reg   $s1       unsigned long * data
 * END PSX.SYM */

/*
 * LoadFromMEMORY (0x8001962c) — memory-card/MEMORY loading stub; disabled in
 * the shipped build, so it just reports the "disabled" message and returns
 * null (filename is unused). D_8001113C is the literal string
 * "*** memory load is disabled now ***" living in this TU's rodata blob
 * (unsplit data segment; the address already has an auto symbol, no yaml
 * carve needed — same pattern as D_800121CC/D_80097CB4 elsewhere).
 */
extern void AdtMessageBox(char *fmt, ...);
extern char D_8001113C[];

void *LoadFromMEMORY(u8 *filename)
{
    AdtMessageBox(D_8001113C);
    return 0;
}
