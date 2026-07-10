# PSX.SYM — the original source's names and types

The Japanese demo disc (`Rittai Ninja Katsugeki Tenchu (Japan) (Demo)`, PAPX-90029,
1997-10-26) shipped `PSX.SYM` inside its AFS archive, alongside leftover build-pipeline
junk (`VOLMAKE.BAT`, `AFSMAKE.EXE`). It is the Psy-Q linker's debug output — the
authors' own source file names, function prototypes with parameter names, struct /
union / enum layouts with field names, line numbers, and the `static`-vs-`extern`
storage class of every symbol.

boricj found it and [wrote it up](https://boricj.net/tenchu1/2024/04/01/part-8.html);
our Ghidra symbols descend from his version-tracking of it. But that propagation was
lossy, and it only ever carried *names*. The types, prototypes and translation-unit
structure never reached us. They have now.

## Getting it

```console
$ tools/extract-demo.py ~/Downloads/PAPX90029.BIN -o disks/demo
```

Walks the MODE2/2352 ISO9660 tree, pulls `TENCHU.VOL`, walks its AFS directory
(36-byte big-endian records, `IX` signature) and carves `PSX.SYM` plus the demo
executables. `disks/` is gitignored; the extractor asserts
`sha1(PSX.SYM) = ffc15ac96e595a2c073529b368be8bcc53a52bcd`.

## Reading it

`tools/psxsym.py` parses the `MND` format: an 8-byte header then a stream of
`u32 offset ; u8 tag ; payload`. The tag table is in the module docstring. It parses
the whole 1.48 MB to EOF with **zero unknown tags** and reproduces the 1097 functions
Ghidra shows in `PSX.SYM.elf` — that agreement is the correctness check.

Contents: **1696 plain symbols**, **442 functions with full debug info** across
**31 source files**, **60894 type definitions**.

```console
$ tools/symdump.py            # regenerates everything under reference/
```

| file | what |
|---|---|
| `reference/psxsym-types.h` | 415 aggregates + 289 typedefs, original field names |
| `reference/psxsym-protos.h` | 442 prototypes, 308 with the authors' parameter names |
| `reference/psxsym-tu-map.tsv` | function → source file, line, frame size, saved-reg mask |
| `reference/psxsym-data.tsv` | 567 data symbols with their original types |
| `reference/psxsym-statics.tsv` | 304 `static` declarations (73 functions, 231 objects) |

`tools/matcher-prompt.py` injects the prototype, the TU and the storage class into
every matcher-agent launch prompt automatically.

**The addresses in PSX.SYM are useless.** It describes an *earlier, lost build* —
its `.text` starts at `0x8001387c`, the demo's own `PSX.EXE` at `0x8001389c`, and a
constant `+0x20` shift only reconciles 85% of the functions. Names and types transfer;
addresses do not.

## What survives the build gap

Two structural facts do:

* the linker emits each translation unit **contiguously**, and
* within a TU the compiler emits functions in **source order**.

Retail `main.exe` reproduces the demo's 31 TUs as 36 contiguous blocks. That is what
makes name recovery possible at all.

## The three matchers

Each is independent, each measures its own precision against a control set of
functions already named on both sides. Never trust one alone — see the worked failure
below.

| tool | signal | control precision | good for |
|---|---|---|---|
| `tools/symmatch.py` | TU + source order, Needleman-Wunsch, scored on frame size / saved-reg mask / code size | frame+mask agree on only **63%** of anchors | game code inside the 31 debug TUs |
| `tools/xbuildnames.py` | normalized instruction identity vs demo `PSX.EXE` | **96.5%** (341 control pairs) | library / support code, unedited bodies |
| `tools/callmatch.py` | multiset of named `jal` targets | **97.9%** (94 control pairs) | anything whose body was rewritten |

`xbuildnames` masks link-time immediates (`%hi`/`%lo`, gp and stack offsets, `j`/`jal`
targets) and keeps PC-relative branch offsets, which survive relinking. Its known
failure mode is functions differing *only* in a masked immediate: it happily equates
`SpuRead` and `SpuWrite`. Uniqueness on both sides plus the PSX.SYM provenance filter
keeps that out of the proposals, but do not lower those guards.

### Verify before you adopt

```console
$ tools/callmatch.py --verify reference/psxsym-applied.tsv
```

For each proposed name it checks that the demo function's named callees are
**contained in** the retail function's (containment, not equality — retail functions
grow). This is not theoretical: `symmatch` proposed `SetPadState` for `0x80032610`
because the frame size *and* the saved-register mask both matched. The callees said
otherwise —

```
retail 0x80032610 calls: GsGetWorkBase, GsSetWorkBase, SetDrawMove, AddPrim
demo   UpdateTexScroll : GsGetWorkBase, GsSetWorkBase, SetDrawMove, AddPrim
demo   SetPadState     : GsSortSprite x5
```

— and the name was wrong. Frame/mask agreement is weak evidence on its own.
Call signature outranks it.

## What we adopted

**34 names**, all rebuilt byte-identical, listed in `reference/psxsym-applied.tsv`.
Six of them replaced *guessed* names with the originals, and the guesses corroborate
the recovery:

| our guess | original |
|---|---|
| `handle_char_state_using_item_` | `ActITEM` |
| `handle_char_state_posing_` | `ActACTION` |
| `handle_char_state_attacking_SEVEN_` | `AttackFire` |
| `think_setting_weapon_logic1_` | `Think3area` |
| `load_sprite_info_perhaps` | `SetupImageToPolyFT4` |
| `change_clear_pad_init_card_` | `InitCARD` |

## What we did NOT get, and where it went

Three lists are regenerated by `tools/symmatch.py --candidates --unplaced --left`:

* **`reference/psxsym-candidates.tsv`** — 17 names the TU alignment can suggest.
  The `LOW` rows are *not* applied: positional evidence only. Five were then actively
  **rejected** by `callmatch --verify` (`ViewAdjustBG`, `SetGore`, `GetFreeItemSlot`,
  `RequestItem`, and the `SetPadState` above). One, `AttackFire`, was rescued from
  `LOW` to adopted when its callees came back 100% contained. The rest await better
  evidence — the placeholder stays, the candidate is recorded.

* **`reference/psxsym-unplaced.tsv`** — 61 functions PSX.SYM knows that we could not
  locate. Ten each in `OPENING.C` and `OPMOVIE.C` and six in `MOJI.C` are almost
  certainly *not in `main.exe` at all* (the demo was one monolithic `PSX.EXE`; retail
  split menu/opening/ending into `MENU.EXE` and `ENDING.EXE`). The `strInit`/`strNext`/
  `open_stream` cluster is Psy-Q `libpress` streaming. The rest — `ActDEAD`, `DrawImpact`,
  `DrawTarget`, `GetScreenPosition`, `PutMoji*`, `QuakeCamera` — are real game code and
  should be findable; they are the best remaining targets.

* **`reference/psxsym-unnamed.tsv`** — 136 retail placeholders with no candidate at
  all. Roughly 60 are in the `0x8005xxxx` library region (no debug info in the SYM),
  the rest are game code added after the demo.

## Still on the table

* **Data symbols.** 567 named globals with original types (`BattleDB` is
  `struct BattleType[78]`, `ActionFunc` is `void (*[18])()`, `AccessPower` is a `static int`).
  Nothing has been matched against our `D_800xxxxx` placeholders yet — the data segment
  moved more than `.text` did, so it needs its own anchored alignment.
* **Statics.** 73 functions and 231 objects the original build declared `static`.
  A `static` never gets a `%gp` extern and changes how the symbol is emitted; this list
  should feed `tools/gpsyms.py`.
* **Line numbers.** Every function records a start line and an end line, and the SLD
  stream carries per-instruction line deltas we currently discard. Function *length in
  source lines* is a usable prior on how much C a body should be.
* **`MENU.EXE` / `ENDING.EXE`.** The unplaced `OPENING.C`/`OPMOVIE.C`/`MOJI.C` functions
  are presumably there. Same three matchers apply.
* **`VOLMAKE.BAT`** (embedded in `TENCHU.VOL`) names the stage and enemy assets:
  `ma_akin`→`akindo`, `ma_cat`→`castle`, `ma_jyou1`→`jouka`, `ma_tan`→`tanren`,
  `ma_tem`→`temple`, `ma_yami`→`yamijou`, and enemies `rounin`, `rouban`, `echigoya`,
  `hanbe`, `jochu`, `rat`, `cat`, `dog`. Useful for naming stage/enemy tables.
* The original tree lived at `J:\DEVELOP\PS\tenchu\tomo\NINJA_OP\`.
