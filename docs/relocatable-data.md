# Relocatable loaded data

`tools/reloc_audit.py` found a conservative set of 185 aligned literal words
in the current loaded data that point back into the movable MAIN.EXE image.
The reviewed manifest now covers all 185. Use the live audit rather than
treating that count as an invariant. This was a much cleaner problem than the
raw-code candidates:

- every initial and current candidate source word is in trailing owner
  `72CD0.data.s`;
- every target is in the leading-data range;
- the targets inspected so far are exact string or string-pool entry starts;
  and
- the generated owner around a target is often broader than the object being
  addressed, so rewriting a pointer as `D_800xxxxx + addend` would invent an
  ownership claim.

The initial owner distribution is useful for batching the work:

| Generated source owner | Pointers |
|---|---:|
| `D_8008EA90` | 76 |
| `CD_comstr` | 32 |
| `ThinkDB` | 18 |
| `CD_intstr` | 8 |
| Nine coherent four-entry tables | 36 |
| `WeaponModel` | 3 |
| Twelve singletons | 12 |

This inventory is conservative. It says that the word is aligned and falls in
the movable range; table structure, type, and symbol naming still require
review.

## Representation

`tools/reloc_data.py` applies `config/reloc-data.main.exe.json` to copies of
Splat's generated assembly. Each reviewed manifest entry records:

- the pointer's generated file, exact address, and enclosing `dlabel` owner;
- the target's generated file, exact address, and enclosing owner; and
- a human-readable symbol for the exact target.

The transformer inserts a section-relative object label directly at the
target and changes only the pointer operand from a literal to that symbol. For
example, the representation is conceptually:

```asm
/* exact interior target inside the larger generated string-pool owner */
.globl stage_sound_prefix_default
.type stage_sound_prefix_default, @object
stage_sound_prefix_default:
    .asciz "K:\\WORK\\CDIMAGE\\SOUND\\"

STAGE_SOUND_PREFICES:
    .word stage_sound_prefix_english
    .word stage_sound_prefix_french
    .word stage_sound_prefix_italian
    .word stage_sound_prefix_default
```

The enclosing generated owner is only an evidence guard. It is deliberately
not used as the relocation base. This prevents a label at `0x80013524` from
silently becoming `D_80013500 + 0x24`, a top-level relationship not supported
by the data itself.

The tool refuses stale or ambiguous input: wrong owners, missing addresses,
changed pointer literals, duplicate sources, symbol/target conflicts, and
attempts to overwrite the generated input directory all fail before any
output is written.

## Reviewed slices

The committed manifest covers eight clear, game-owned language tables plus a
typed demo-screen asset matrix. The first slice was:

- `STAGE_SOUND_PREFICES` at `0x8008EA58..0x8008EA64`; and
- `STAGE_ANIMATION_PREFICES` at `0x8008EA68..0x8008EA74`.

The second slice adds six adjacent, named four-entry tables:

- `ITEM_SEL_SPRITE_PTRS`;
- `RS_ARCHIVE_PTRS`;
- `RANK_ARCHIVE_PTRS` and `RANKS_ARCHIVE_PTRS`, which deliberately share the
  same four targets;
- `TRN_SPRITE_PTRS`; and
- `GOV_ARCHIVE_PTRS`.

Their strings spell the English, French, Italian, and Japanese filenames in
the same order as the tables.

The next slice covers all 76 remaining literal pointers in `D_8008EA90`.
`FUN_800519bc.c` declares this as `DemoScreenAssets D_8008EA90[][11]`, where
each record contains `background`, `foreground`, and `music` fields. The data
is four language blocks of eleven stage records, and each reviewed target is
the exact start of the corresponding `stNNN.tim` or `mojoN.tim` string. Stage
9 has null asset fields. The four `mojo11.tim` fields were already symbolic
references to the exact enclosing `D_800136B0` label, so the manifest leaves
them alone rather than manufacturing redundant aliases.

The final slice covers the remaining 77 words:

- four `HumanData[].name` fields and three `WeaponModel[].name` fields, whose
  0x18-byte and 0x0c-byte record types are exercised directly by the matching
  C sources;
- all 18 non-null literal `ThinkDB[].name` fields, paired with their adjacent
  values and used as strings by `AddEnemy`;
- all 32 `CD_comstr` and eight `CD_intstr` entries, including their shared
  `"?"` string;
- four SDK singleton pointers to the BIOS/intr/GPU revision strings and the
  hexadecimal digit alphabet;
- the six XA filenames consumed by the typed `PlayVoice` paths; and
- the `TENCHU_ID` and `CID` card-id string pointers consumed by the card/save
  C sources.

Together the slices cover 185 pointer words and 112 exact targets across nine
generated files. The names are supported by source table/type/field names,
direct C uses, and literal contents. No wider top-level target ownership is
inferred.

Validate the manifest against a generated tree without writing anything:

```console
$ tools/reloc_data.py \
    --manifest config/reloc-data.main.exe.json \
    --input-dir .shake/gen/main.exe/asm/data \
    --check
reloc-data: validated 185 pointer words and 112 exact targets across 9 files
```

Or write separate assembly inputs directly:

```console
$ tools/reloc_data.py \
    --manifest config/reloc-data.main.exe.json \
    --input-dir .shake/gen/main.exe/asm/data \
    --output-dir .shake/build/reloc-data/asm
```

Only the nine touched files are emitted. The default matching lane still
consumes Splat's files unchanged.

The standalone build gate applies the transform to temporary copies, assembles
the touched objects, and performs the controlled links described below:

```console
$ ./Build check-reloc-data
reloc-data-lane: verified 185 R_MIPS_32 pointer words, 112 exact targets, and 3 link layouts across 9 files
check-reloc-data: reviewed pointer tables are retail-exact and shift-relocatable
```

Composing the whole manifest into `./Build relink` requires replacing all nine
generated objects: `E58`, `1160`, `207C`, `2EB0`, `33C4`, `37A8`, `400C`,
`4900`, and `72CD0`. The BSS transform must remain after the `72CD0` pointer
rewrite. The standalone gate remains the stricter per-record and
controlled-shift oracle.

## Binutils proof

The proof uses GNU MIPS assembler and linker output, not a source-level
assumption:

- in the current generated tree, unmodified `72CD0.data.s.o` has 408
  `R_MIPS_32` records and the rewritten object has 593;
- the 185 additions are individually required to be `R_MIPS_32` at their exact
  reviewed source offsets and against their exact target symbols;
- each target symbol is section-relative at the expected interior offset in
  `207C.data.s.o` or `2EB0.data.s.o`, never `ABS`;
- a retail-address partial link reproduces all 185 shipped pointer words;
- moving all touched loaded-data inputs by `+4` increments every pointer by
  `+4`; and
- moving them by `+0x10004` increments every pointer by `+0x10004`, including
  the high-word carry.

The focused test performs the same assemble/readelf/link checks with hermetic
fixtures when MIPS binutils are available:

```console
$ python3 -m unittest -v \
    tools.tests.test_reloc_data tools.tests.test_reloc_data_lane
```

Applying the manifest to the current generated inputs reduces the live
literal-pointer inventory from 185 to zero. The ordinary generated inputs are
unchanged; only the relocation lane consumes transformed copies.

The default `tools/reloc_audit.py` invocation audits that composed normal
relink, not the untouched matching linker. It resolves ordinary raw objects
through `.shake/gen/main.exe/asm/data` and maps the substituted `207C`, `2EB0`,
and combined `72CD0` objects to their transformed sources. The audit rejects a
linked generated replacement without an exact object-to-source mapping, and it
rejects a mapping whose object is absent from the selected linker. This keeps a
stale retail source from being counted after its object has been replaced. Use
repeatable `--object-source OBJECT=SOURCE` arguments when auditing another
composed layout; the explicit mappings override the applicable defaults.

## Scaling without inventing source

If future generated data introduces new candidates, continue by coherent
owner, not by blindly replacing every aligned word. For every batch:

1. confirm the exact target directive and enclosing owner;
2. name the exact interior object, not an enclosing blob plus a guessed addend;
3. require retail bytes plus `R_MIPS_32` in assembler output;
4. require controlled `+4` and `+0x10004` links; and
5. migrate to typed C tables only when PSX.SYM, headers, or uses establish the
   genuine type.

Symbolic assembly remains an honest, editable relocation-bearing source form
when a stronger type is not yet known. This is the loaded-data counterpart to
the symbolic/original-object policy in [relocatable-build.md](relocatable-build.md).
