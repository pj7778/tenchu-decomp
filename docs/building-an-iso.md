# Building a bootable ISO (`./Build iso`)

To actually run the decomp/mods, rebuild the game's CD image with our `main.exe`
and load it in an emulator (pcsx-redux) or on hardware.

## Prerequisites: the original disc

You must provide the original disc — it's copyrighted, so it isn't in the repo.
A CD dump as a `.cue` + `.bin` (redump/DIC style; a data track `MODE2/2352` plus
a CD-DA audio track) is what's expected. Point the build at it one of two ways:

- set `TENCHU_CUE=/path/to/game.cue`, or
- drop the `.cue` (+ its `.bin`s) under `disks/` or `~/tenchu-iso/` (auto-found).

On first use the disc is dumped once with `dumpsxiso` into `TENCHU_ISO_WORK`
(default `.shake/iso/`, ~700 MB, cached; override the env var to relocate it).

## Commands

```console
$ nix develop            # provides mkpsxiso + dumpsxiso (packaged in nix/mkpsxiso.nix)
$ ./Build iso            # matching main.exe -> .shake/build/tenchu/tenchu.{bin,cue}
$ ./Build iso-mod        # modded main.exe   -> .shake/build/tenchu/tenchu-mod.{bin,cue}
$ ./Build run            # FAST: -loadexe main.exe over the original disc (no repack)
$ ./Build run-iso        # FAITHFUL: repack the disc with main.exe and boot it
$ ./Build run-mod        # fast, -loadexe the modded main.exe
$ ./Build run-iso-mod    # FAITHFUL: repack the disc with the modded main.exe and boot it
```

**Mods are patched in place, so the disc stays faithful.** `./Build mod`
(tools/mkmod.py) compiles each `src/mod/main.exe/<fn>.c`, links it at the function's
*original* address, and overwrites that function's bytes — the function has to fit in
its original slot (up to the next symbol), so `main_mod.exe` is **exactly the same
size** as `main.exe`. That matters because the disc is packed with **no free sectors**:
`MAIN.EXE` is a 271-sector slot at LBA 319, immediately followed by `TRIAL/ENDING.EXE`,
`DATA.VOL`, and the `MOVIE/*.STR` + `XA/*.XA` streams. A *bigger* `MAIN.EXE` would shove
them all to new LBAs, and streamed cutscenes seek by absolute sector, so they'd read the
wrong data and hang (right after the loading screen). Because the modded exe is the same
size, mkiso keeps **forced LBAs** — the mod disc is byte-identical to the original except
`MAIN.EXE`'s sectors, so the full `SLPS→MENU→MAIN` boot and every cutscene work, on any
emulator or real hardware.

If a modded function outgrows its slot, `./Build mod` aborts and tells you by how many
bytes — trim it (drop debug logging, simplify) until it fits. See
[modding-and-nonmatching.md](modding-and-nonmatching.md).

Two ways to launch:

- **`./Build run`** (fast) mounts the original disc and `-loadexe`s our `main.exe`
  over it — no ISO repack, so the edit→test loop is just `./Build run`. Because
  Tenchu boots `SLPS_019.01 → … → TENCHU/MAIN.EXE`, this jumps straight to
  `MAIN.EXE` and skips that launcher; great for iterating on `main.exe`, but if the
  game needs the full boot chain use `run-iso`.
- **`./Build run-iso`** (faithful) boots the repacked disc (our `MAIN.EXE` swapped
  in), running the real `SLPS_019.01 → … → MAIN.EXE`. You can also load `tenchu.cue`
  by hand (drag it into pcsx-redux, or *File → Open Disk Image*).

`tenchu.{bin,cue}` (and `tenchu-mod.{bin,cue}`) are real Shake targets, so `iso` /
`run-iso` **repack only when the exe changed** — repeated `run-iso` launches don't
rebuild the ~750 MB image. The one thing not tracked is a swap of the *original*
disc (mkiso.py discovers it dynamically): `./Build clean` to force a repack then.

`run` finds the emulator via `$PCSX_REDUX`, then `pcsx-redux` on `PATH`, then
`~/programming/pcsx-redux/pcsx-redux`; extra emulator flags go in `$PCSX_REDUX_ARGS`.
Both modes need the original disc (the fast path mounts it for assets; the faithful
path repacks from it), found via `$TENCHU_CUE` or under `disks/` / `~/tenchu-iso/`.

**CPU/BIOS:** no BIOS needed — pcsx-redux falls back to OpenBIOS. But its x64
**dynarec crashes with OpenBIOS** ("Unrecoverable error while running recompiler"
during Tenchu's intro) — a regression of
[grumpycoders/pcsx-redux#695](https://github.com/grumpycoders/pcsx-redux/issues/695)
(that combo was fixed in 2022, then re-broke in the emulator's memory-system
rework). So `run` forces `-interpreter` by default. To use the faster dynarec,
supply a real BIOS and opt in:
`PCSX_REDUX_ARGS='-bios /path/scph.bin -dynarec' ./Build run` (any of `-dynarec`,
`-bios`, `-interpreter` in `$PCSX_REDUX_ARGS` disables the forced default).

## What you get

- **`./Build iso`** points the disc's `TENCHU/MAIN.EXE` at the matching build and
  rebuilds with forced LBAs, so the data track is **byte-identical to the
  original disc except `main.exe`'s sectors** (verified). A same-size mod (edit
  `src/main.exe/…` keeping instruction count) rides the same path — only your
  changed bytes differ, so it boots exactly like the original.

- **`./Build iso-mod`** puts the modded `main_mod.exe` (functions patched in place
  by `./Build mod`, see [modding-and-nonmatching.md](modding-and-nonmatching.md)) on
  the disc. Because it's the **same size** as the original `main.exe`, the build keeps
  forced LBAs — the data track is byte-identical to the original except `main.exe`'s
  sectors, exactly like `./Build iso`. So the full boot chain and all streamed
  movies/XA work; `run-iso-mod` is the faithful way to play a mod.

## How it works

`tools/mkiso.py`: dumps the original once (`dumpsxiso -l`, capturing the exact
layout + PSX license), rewrites the `MAIN.EXE` `source` in the generated XML to
our build, then rebuilds with `mkpsxiso`. mkpsxiso reproduces the disc
byte-for-byte from that XML (validated: full round-trip of the untouched disc is
byte-identical on the data track), so swapping only `main.exe` yields a clean,
bootable image. The output is a single `.bin` with two tracks (data + CD-DA
audio) described by the `.cue`.

Note: the disc build is separate from the byte-match `check` — it produces a
*runnable* image, not a byte-match target (except incidentally, for the matching
build).
