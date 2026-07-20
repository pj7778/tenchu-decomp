# Debugging Tenchu from VSCode

PCSX-Redux ships a GDB server; a MIPS-aware gdb attaches to it, and VSCode
drives that gdb. This gives a real IDE debug session — named call stacks,
breakpoints by function, registers, memory, watch expressions, disassembly,
and instruction stepping — instead of PCSX-Redux's built-in debugger UI.

## One-time setup

- **VSCode extension:** install Microsoft C/C++ (`ms-vscode.cpptools`); the
  committed `.vscode/launch.json` uses its `cppdbg` adapter.
- **gdb:** none needed on PATH. `tools/mips-gdb` materialises nixpkgs gdb
  (17.1, `mips:3000` target) behind a gcroot on first use and execs it;
  `launch.json` points `miDebuggerPath` at it. Override the nixpkgs checkout
  with `TENCHU_NIXPKGS` if yours is not at `~/programming/nixpkgs`.

## Attaching

1. Launch the game with the GDB server enabled (port 3333):

   ```console
   $ TENCHU_GDB=1 ./Build run-relink      # or run, run-mod, run-iso*, …
   ```

   `TENCHU_GDB=<port>` picks another port. The server coexists with the
   `-dofile` symbol loader and the deferred-launch wrapper, so the build lock
   is released while you debug.

2. In VSCode, pick the matching **Attach: PCSX-Redux (…)** configuration
   (exact / relink / mod — they differ only in which ELF supplies symbols) and
   start debugging. It connects to `localhost:3333`, stops on connect, and
   you can set breakpoints (`break PadProc`, or click the gutter in the
   disassembly view), inspect `$pc`/registers, read memory, and step.

The ELF's symbol table names everything, so call stacks read as
`PadProc → …` rather than raw addresses — the same names the in-emulator
disassembly gets from `<artifact>.symbols.lua`.

## The one limitation: no C source lines (yet)

Breakpoints, stepping, and stacks work at the **instruction/symbol** level,
not the C source level, because this project builds with the original 1997
`gcc 2.8.1`. That compiler emits only STABS or DWARF 1 debug info, and modern
gdb (17.1) has abandoned both: its STABS reader hard-crashes on gcc 2.8.1's
own type idioms (self-referential integer ranges, `size;0` floats), and
DWARF 1 is unsupported. Adding `-gstabs` to the build was verified to keep
`.text` **byte-identical** (so a parallel debug ELF's addresses match the
running image), but gdb cannot consume the result.

The viable path to source-level stepping is to **synthesise modern DWARF
line tables** from the byte-identical `-gstabs` build: parse the assembled
`.stab` section's `N_SLINE` (line→address) and `N_FUN`/`N_SO` records — whose
addresses are exact because the text matches — and emit a `.debug_line`
(plus a minimal `.debug_info`/`.debug_abbrev` compilation unit) into a debug
ELF, sidestepping gdb's dead STABS reader. gdb reads DWARF line tables
reliably. This is scoped but unbuilt; the inputs are proven. Until then, keep
the C file open beside the disassembly and set breakpoints by function name.

## Notes

- Use one attach at a time; the server serves a single client.
- The disc chain (`run-iso*`) works the same — attach once MAIN.EXE is
  running. On the disc path SLPS/MENU occupy the same addresses first, so set
  function breakpoints (which are re-resolved) rather than raw-address ones.
- `set architecture mips:3000` is applied automatically by the config; if you
  drive gdb by hand, set it before `target remote`.
