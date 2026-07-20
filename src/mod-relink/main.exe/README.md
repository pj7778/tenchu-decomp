# Relink function overrides

Drop `<Name>.c` here to replace the matched translation unit of the same name
in the normal relink lane. Start from a copy of `src/main.exe/<Name>.c` and
edit freely — the function may grow, shrink, gain globals, or call new code.

- `./Build relink` / `iso-relink` / `run-relink` compile this file with the
  identical `cpp | cc1-281 | maspsx | as` pipeline and link its object in
  place of the original, at the original position in the input order. All
  relocation/ownership audits still gate the result.
- `./Build check` stays byte-identical while overrides exist: the exact
  matching lanes never read this directory.
- Brand-new translation units (new helpers, new subsystems) belong in
  `src/main.exe/reloc/` instead; this directory is only for replacing
  functions that already exist.
- The file name must match an existing translation unit; a typo fails the
  link-script generation with "override target … is not a linker input".
- `vinit.c`/`valloc.c` cannot be overridden here: their normal-lane objects
  carry the reviewed pool/capacity relocation transform. Change allocator
  policy in `src/main.exe/ram_layout.h` instead.

Verify a mod boots and behaves with
`python3 tools/pcsx_smoke.py [--repack] [--watch-counter SYM]
[--watch-equals SYM=VALUE]`. See `docs/modding-and-nonmatching.md` and
`docs/relocatable-build.md`.
