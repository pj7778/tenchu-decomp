# Bridging the Ghidra project into the decomp

You have a separate Ghidra project with symbols/types/decompilation. The bridge is
**bidirectional**:

- **Ghidra → repo** (sections 1–3 below): pull decompiled C, symbol names, and
  types in, one function at a time. The build stays byte-identical at every step.
- **repo → Ghidra** ([sync back](#pushing-the-repo-back-into-ghidra-repo--ghidra)):
  push the knowledge you *proved* here (a type/signature that survives
  `./Build check`) back so Ghidra's decompiler improves for the next function.

The mental model that makes the loop work: **the C bodies never sync** (Ghidra
generates its; the repo has the matching one — different roles). What syncs is the
*knowledge layer* — types, function signatures, global var types, and names — and
the repo is canonical for what the byte-match proves. See PLAN.md / the design
discussion for the full rationale.

## Which program to export from (read this first)

The project `~/programming/ghidra-projects/tenchu-decompile.rep` holds ~60 programs
across every Tenchu release. Our binary is
**`/Rittai Ninja Katsugeki - Tenchu - Shinobi Gaisen (Japan)/TENCHU/MAIN.EXE`**
(1781 functions, 277 `FUN_`). That is what `.shake/ghidra-export` was made from, and
what the repo's names descend from.

**The project also has a program called `/main.exe` in its root. Do not export from
it.** It is a stale, separately-annotated import of the same binary: 302 functions
that `MAIN.EXE` names are still `FUN_` there, and it carries 240 *conflicting guessed*
names (`get_free_memory` where the real name is `vgetfreesize`). `import_symbols.py`
only refuses to overwrite a real name with a `FUN_`-style placeholder, so pointing it
at `/main.exe` would silently regress ~240 names into guesses.

The project also contains `PSX.SYM.elf` — boricj's NOBITS import of the demo's debug
symbols — and the demo binaries. We now read `PSX.SYM` directly instead; see
[psx-sym.md](psx-sym.md).

## 1. Export from Ghidra (once, re-run when it changes)

`tools/ghidra/ExportDecomp.java` decompiles every function and writes, to a chosen
output dir:

- `functions.tsv` — one line per function: `<hexaddr>\t<sizeBytes>\t<name>`
- `c/<hexaddr>.c` — that function's decompiled C

Run it on the program that holds `main.exe` (loaded at `0x80011000`):

- **GUI:** Script Manager → add `tools/ghidra` to script dirs → run `ExportDecomp`
  → pick an output dir; or
- **headless:**
  ```
  analyzeHeadless <proj_dir> <proj_name> -process <program> \
    -scriptPath tools/ghidra -postScript ExportDecomp.java <outDir>
  ```

The function **size** in `functions.tsv` is what makes this reliable — splat needs
the exact extent, and guessing from the next symbol is wrong (there's often data
between functions).

## 2. Reverse a function

```console
$ nix develop
$ tools/reverse.py <name> --ghidra-export <outDir>
```

For that one function it:

1. adds `[offset, c, <name>]` to `config/splat.main.exe.yaml` (splitting the
   surrounding `data` block; leading/trailing data is preserved);
2. re-runs splat, so its ASM appears under `.shake/gen/…/nonmatchings/<name>/`;
3. writes `src/main.exe/<name>.c` = the `INCLUDE_ASM` stub **+ Ghidra's C in `//`
   line comments** (line comments because Ghidra's C often contains `/* */`);
4. rebuilds and asserts **`./Build check` is still byte-identical** — the split
   only relabels bytes as code, so the output must not change. If it fails, the
   function extent was wrong (or the region has rodata splat must be told about);
   revert with `git checkout config/splat.main.exe.yaml && rm src/main.exe/<name>.c`.

Then turn the commented C into real matching C, drop the `INCLUDE_ASM`, and iterate
with the permuter / asm-differ as usual. Same pattern as `get_held_buttons`.

Manual mode (no export): `tools/reverse.py <name> --addr 0x… --size 0x…`.

## Symbol names (done)

`tools/ghidra/ExportSymbolsTypes.java` dumps all global symbols → `symbols.tsv`,
and `tools/import_symbols.py` adopts those names across the whole decomp
(`config/symbols`, the splat yaml, `src/*.c` + filenames, `main.exe.h`). Only
names change (addresses are identical) so `./Build check` stays byte-identical.
Re-run it after you rename things in Ghidra:

```console
$ tools/import_symbols.py --ghidra-export .shake/ghidra-export
```

It renames game-RAM symbols only (leaves PSX hardware/BIOS names), never
downgrades a real name to a Ghidra placeholder (`FUN_…`), and skips collisions.
It also rewrites the quoted symbol form (`"SYM"`) in the gp-extern lists
(`shake/src/Build.hs`, `tools/permute.py`) — a renamed gp-small would otherwise
lose its `--gp-extern` flag and maspsx would emit it absolute (a silent byte
break; `./Build check` catches it, but the rewrite prevents it).

**There is no "more official" name source than Ghidra — `main.exe` is stripped.**
The file is exactly `0x800` (header) + `0x87000` (text) bytes with nothing
appended (no symbol table); the disc carries no `.SYM`; the sibling exes
(trial/menu/ending/run) are stripped too; and the function names are not present
as debug strings in the binary. So **every** name in the repo is
reverse-engineered — there is no developer-symbol table to defer to. The naming
rule is therefore simply: **Ghidra is the source of truth. Where it has a real
name, `import_symbols.py` adopts it; where it still has `FUN_`, the repo keeps
whatever it has** (often a hand-picked placeholder like `item_use_gun`, which is
as authoritative as any CamelCase name). To pull in names you have added in
Ghidra since the last export, re-run `ExportSymbolsTypes.java` first — the
committed `symbols.tsv` can lag the live project (it will show `FUN_` for
addresses the repo has since named by hand).

## Types (reference)

`ExportSymbolsTypes.java` also writes `types.h`; a snapshot is committed at
[`reference/ghidra_types.h`](../reference/ghidra_types.h) (157 structs/enums).
It's **reference only, not built** — types affect codegen, so replacing
`main.exe.h` wholesale risks breaking already-matched functions. Adopt types
**per function** as you reverse (copy the struct you need into `main.exe.h`,
adjust, and let `./Build check` catch layout mistakes).

Example: `Think1sleep` is blocked by a wrong
`character_state` layout. Ghidra's decompilation is `Me_THINK_C->motion->mid` /
`->count` — i.e. the character struct has a `motion` pointer at 0x5C (correct),
and `motion` points to a `{ short mid; short count; … }`. Pull that layout from
`reference/ghidra_types.h` to fix it.

## Pushing the repo back into Ghidra (repo → Ghidra)

`tools/sync_to_ghidra.py` drives `tools/ghidra/ImportToGhidra.java` (headless) to
push the repo's verified knowledge into the Ghidra program:

- **types** — `src/main.exe/game_types.h` (the structs/enums, split out of
  `main.exe.h` as the round-trip unit) is parsed into the program's data type
  manager. Base `sN`/`uN` types Ghidra lacks are supplied; ones it already has
  (`u_long`, `byte`, …) are **not** redefined (that would clobber Ghidra's SDK types).
- **function signatures** — from `main.exe.h`'s `extern` prototypes *and* every
  fully-matched `src/main.exe/*.c` definition. Applied with `NO_CHANGE`, so the
  return/param **types** land but the function is never renamed and its calling
  convention is untouched.
- **global var types** — `extern T x;` globals from `main.exe.h` (`Me_THINK_C` →
  `character_state *`, `SR` → `s16`, …). Arrays are skipped for now.

```console
$ tools/sync_to_ghidra.py            # DRY RUN (read-only) — review the log
$ tools/sync_to_ghidra.py --commit   # actually write to the Ghidra project
```

Everything is resolved **by address** (from `config/symbols`), never by name —
addresses are the invariant; names drift on both sides. Close the Ghidra GUI first
(headless needs the project lock). Dry run changes nothing; it prints exactly what
`--commit` would do. Defaults: project `~/programming/ghidra-projects`, name
`tenchu-decompile`, program `main.exe` (override with `--project/--project-name/--program`).

### Reconciling names (the drift report)

Because it matches by address, the sync ends with a **name-drift report** —
addresses where the repo's name and Ghidra's differ. This is the manual reconcile
step, and it cuts both ways:

```
8001ada4  repo=PadProc        ghidra=FUN_8001ada4              -> push repo name (Ghidra has a placeholder)
800979cc  repo=Me_THINK_C     ghidra=CHARACTER_BEING_UPDATED?  -> pull Ghidra's better name into the repo
8001b260  repo=GetRealPad     ghidra=get_held_buttons          -> decide the canonical name
```

Pull Ghidra's names into the repo with `tools/import_symbols.py`; push the repo's
by renaming in Ghidra (or extend the sync — it deliberately does *not* rename
automatically, to avoid clobbering a name you preferred on either side).

### The loop, end to end

1. Reverse a function (sections 1–2): `reverse.py` + Ghidra's C as reference.
2. Make it match; refine types in `game_types.h`. `./Build check` green **proves**
   the types — the build is a CI test for your Ghidra type model.
3. `tools/sync_to_ghidra.py --commit` pushes the proven types/signatures back;
   Ghidra re-decompiles everything with them → better C for the next function.
4. When both sides moved, `git diff reference/ghidra_types.h` (after a fresh
   `ExportSymbolsTypes` run) shows what Ghidra changed; merge the wanted bits into
   `game_types.h` and let `./Build check` validate. Git is the merge tool; the
   byte-match is the arbiter.

## Notes

- Only the program's data-type manager is exported (PSY-Q + applied game types).
  Game structs in the separate "Shared data types" *archive* that aren't applied
  in `MAIN.EXE` won't appear; export the archive separately if needed.
- rodata: if a split function has associated `.rodata` (jump tables, floats)
  inside its region, the byte-check catches the mismatch — handle case-by-case
  (a splat `rodata` subsegment) for now.
