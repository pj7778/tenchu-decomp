# Non-matching builds: modding & debug

You don't always want a byte-match — modifying behaviour, adding debug output,
cheats, etc. This documents what works today and how to make deviations run.

## Building a non-matching binary

`./Build` already builds regardless of matching — only `./Build check` compares
the sha256. So the mod workflow is just:

```console
$ # edit src/main.exe/<fn>.c however you like
$ ./Build            # produces .shake/build/tenchu/main.exe (no hash gate)
```

`check` is only for verifying a decomp still matches; ignore it while modding.

## What works: same-size edits (verified)

An edit that keeps the function the **same size** (same instruction count)
produces a valid, runnable binary. Verified: changing `arg0 >> 4` to `arg0 >> 5`
in `get_held_buttons` yields a binary that is **byte-identical except the one
changed instruction** — same total size, PS-EXE header untouched. Everything
else stays at its original address, so all the game's pointers (symbolic and raw)
are still correct.

Same-size covers a lot: changing constants/immediates, swapping a branch
condition, changing which register/field is used, reordering independent ops,
tweaking arithmetic. Prefer these when you want a drop-in behaviour change.

## What breaks: size-changing edits (and why)

Adding or removing instructions (e.g. a `printf` debug line, an extra call)
**changes the function's size**. Verified: adding a second call to
`get_held_buttons` grew the binary by 8 bytes and made ~68% of the file differ —
because everything laid out **after** the edited function shifts down by 8.

The build still succeeds (it links), but the result is **not runnable as-is**,
because this is a *partial* decomp with a fixed memory layout:

- The linker script places sections **sequentially** from `0x80011000`, so a
  function that grows pushes every later function and data block down.
- The data is a **mix**: splat has resolved ~1300+ pointers in the first data
  block alone to **symbolic** references (jump tables, function pointers) that
  the linker relocates — but thousands of other words are still **raw** bytes,
  including pointers it hasn't symbolised yet, plus a set of **fixed** addresses
  in `config/symbols.main.exe.txt` / `undefined_symbols_auto`.
- When code shifts, the symbolic references move with it but the raw/fixed ones
  don't. The two disagree → the binary is internally inconsistent → it crashes.
- The PS-EXE header's `.text` size (`0x00087000`, hardcoded in the generated
  `header.s`) also no longer matches the grown image.

In short: a fixed-layout partial decomp can absorb *substitutions* but not
*insertions*.

## Existing convenience lane: `./Build mod` — patch in place

Non-matching behaviour changes (a decompiled function edited however you like) are
handled by **overwriting the function in place**, wired in as the `mod` target:

```console
$ # put your modified function in src/mod/main.exe/<fn>.c (filename == fn)
$ ./Build mod        # -> .shake/build/tenchu/main_mod.exe
mkmod: hooking ProcItemKusuri
  ProcItemKusuri: 1376/1432 bytes -> patched in place @ 0x80040500
mkmod: wrote .shake/build/tenchu/main_mod.exe (555008 bytes, same size as main.exe …)
```

How it works (`tools/mkmod.py`): it starts from the byte-matching `main.exe`, then
for each `src/mod/main.exe/<fn>.c`:

1. compiles it with the normal pipeline (`cpp | cc1 -G8 | maspsx | as`);
2. links it at the function's **original address**, resolving the game's other
   symbols from `main.exe.elf`;
3. overwrites that function's bytes in `main.exe` with the result.

The function must **fit in its original slot** (its address → the next symbol). Since
nothing moves, `main_mod.exe` is byte-identical to `main.exe` except that function —
same size, PS-EXE header untouched — so the disc rebuild keeps forced LBAs and it runs
on any emulator or real hardware. `src/main.exe/` (the matching decomp) is untouched;
mods live only in `src/mod/main.exe/`.

This target is intentionally in-place. It is useful before the normal-link lane
is complete, but it is not the architecture for size-changing edits and it does
not use trampolines or a hidden mod region. A genuine relink can move code,
loaded data, BSS, and linker-derived boundaries within the executable's RAM
budget; the obstacle is making every in-image reference relocation-aware, not
finding a permanent fixed-address cave.

**If it doesn't fit**, `./Build mod` aborts with the overage, e.g.:

```
mkmod: ProcItemKusuri compiled to 1440 bytes but its slot (…) is only 1432 — over by 8.
Trim it (drop debug logging / simplify) so it fits in place …
```

Trim the function — drop debug/log calls, simplify Ghidra-isms — until it fits. (The
worked example `ProcItemKusuri.c` drops the original's `AdtMessageBox("item dispose
fail…")` diagnostic, which is never hit in normal play, to make room.)

## Size-changing goal: a normal relink

`./Build check-reloc-game` is the first exact-at-retail proof for that path. It
removes fixed address assignments for all 555 game inputs and lets GNU `ld` own
their symbols; the resulting executable still matches retail byte-for-byte.
It does **not** yet make a grown image runnable. Remaining work is tracked in
[relocatable-build.md](relocatable-build.md):

- convert raw CRT/SDK instruction words into original relocatable objects or
  canonical symbolic assembly;
- make pointer-bearing loaded data symbolic, including interior pointers;
- give loaded data, BSS, `_gp`, startup clear bounds, and heap boundaries to the
  linker while retaining only reviewed fixed MMIO/cross-executable addresses;
- regenerate the PS-X EXE entry/load-size fields after link; and
- validate the 2 MiB RAM ceiling, complete executable handoff chain, and media
  playback on a repacked disc.

At that point an ordinary function can grow and the linker can choose the new
layout. No call-site trampoline scheme is part of that design.

## Running a modified exe

Both mod launchers use the same-size `main_mod.exe`, so both are byte-faithful except
your function (see [building-an-iso.md](building-an-iso.md)):

- `./Build run-mod` — `-loadexe main_mod.exe` over the original disc. Fast, but boots
  `MAIN.EXE` directly (skips `SLPS→MENU`), so some game state is under-initialised.
- `./Build run-iso-mod` — **faithful**: repacks the disc with `main_mod.exe` (forced
  LBAs, streamed cutscenes intact) and runs the full `SLPS→MENU→MAIN` boot. Use this
  to actually play a mod.
