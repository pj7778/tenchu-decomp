#!/usr/bin/env python3
"""Verify the committed real-edit regression lane (`check-relink-realedit`).

The lane replays the grown-PadProc fixture (tools/fixtures/relink-realedit/)
through the same override machinery as src/mod-relink/, composed into an
isolated normal relink under .shake/relink-realedit/.  This driver verifies
the modding contract end to end:

- the overridden function grew by whole instructions;
- its text contains a relocated JAL into the brand-new translation unit;
- the new initialized word is present in the finalized PS-X EXE payload;
- the strict input-object relocation audit reports zero findings; and
- unless TENCHU_REALEDIT_NO_SMOKE=1 (or --no-smoke), a bounded PCSX-Redux
  boot observes the new counter incrementing and the magic word loaded.

The base for the growth comparison is the retail-shaped BSS oracle ELF, so
user mods present in src/mod-relink/ never influence this gate.
"""

from __future__ import annotations

import argparse
import os
from pathlib import Path
import struct
import subprocess
import sys
from typing import NamedTuple

try:
    from tools import pcsx_smoke
except ImportError:  # Direct execution (``tools/relink_realedit.py``).
    import pcsx_smoke  # type: ignore[no-redef]


ROOT = Path(__file__).resolve().parent.parent
REALEDIT_DIR = ROOT / ".shake" / "relink-realedit"
REALEDIT_ELF = REALEDIT_DIR / "main_realedit.exe.elf"
REALEDIT_MAP = REALEDIT_DIR / "main_realedit.exe.map"
REALEDIT_EXE = REALEDIT_DIR / "main_realedit.exe"
REALEDIT_LINKER = REALEDIT_DIR / "layout" / "main.exe.ld"
BASE_ELF = ROOT / ".shake" / "build" / "tenchu" / "main_reloc_bss.exe.elf"

OVERRIDDEN_FUNCTION = "PadProc"
NEW_FUNCTION = "ModProbeTick"
NEW_COUNTER = "ModTickCount"
NEW_MAGIC = "ModMagic"
NEW_MAGIC_VALUE = 0x600DF00D

SHN_UNDEF = 0
SHN_ABS = 0xFFF1


class RealeditError(RuntimeError):
    """A verified modding-contract property does not hold."""


class SymbolFacts(NamedTuple):
    value: int
    size: int
    section_index: int


def read_symbol_facts(path: Path) -> dict[str, SymbolFacts]:
    """Read name -> (value, size, shndx) from a little-endian ELF32 file."""
    data = path.read_bytes()
    if len(data) < 52 or data[:4] != b"\x7fELF" or data[4] != 1 or data[5] != 1:
        raise RealeditError(f"{path}: not a little-endian ELF32 file")
    _, _, _, _, _, _, shoff, _, _, _, _, shentsize, shnum, _ = struct.unpack_from(
        "<16sHHIIIIIHHHHHH", data, 0
    )
    if shentsize != 40:
        raise RealeditError(f"{path}: unexpected section-header size {shentsize}")
    sections = [
        struct.unpack_from("<IIIIIIIIII", data, shoff + index * 40)
        for index in range(shnum)
    ]
    facts: dict[str, SymbolFacts] = {}
    duplicates: set[str] = set()
    for section in sections:
        sh_type, sh_offset, sh_size, sh_link, sh_entsize = (
            section[1], section[4], section[5], section[6], section[9],
        )
        if sh_type != 2:  # SHT_SYMTAB
            continue
        if sh_entsize != 16 or sh_link >= len(sections):
            raise RealeditError(f"{path}: malformed symbol table")
        strtab = sections[sh_link]
        strings = data[strtab[4] : strtab[4] + strtab[5]]
        for offset in range(0, sh_size, 16):
            name_offset, value, size, _info, _other, shndx = struct.unpack_from(
                "<IIIBBH", data, sh_offset + offset
            )
            if not name_offset:
                continue
            end = strings.find(b"\0", name_offset)
            name = strings[name_offset:end].decode("utf-8", errors="replace")
            if not name or shndx == SHN_UNDEF:
                continue
            if name in facts and facts[name] != SymbolFacts(value, size, shndx):
                duplicates.add(name)
                continue
            facts[name] = SymbolFacts(value, size, shndx)
    for name in duplicates:
        facts.pop(name, None)
    return facts


def require_symbol(facts: dict[str, SymbolFacts], name: str, path: Path) -> SymbolFacts:
    try:
        found = facts[name]
    except KeyError as error:
        raise RealeditError(f"{path}: no unique defined symbol {name!r}") from error
    if found.section_index in (SHN_UNDEF, SHN_ABS):
        raise RealeditError(f"{path}: {name} is not section-owned")
    return found


def jal_word(target: int) -> int:
    return 0x0C000000 | ((target & 0x0FFFFFFF) >> 2)


def verify_static(quiet: bool = False) -> None:
    for path, description in (
        (REALEDIT_ELF, "realedit ELF"),
        (REALEDIT_EXE, "realedit PS-X EXE"),
        (REALEDIT_MAP, "realedit map"),
        (REALEDIT_LINKER, "realedit linker script"),
        (BASE_ELF, "retail-shaped base ELF"),
    ):
        if not path.is_file():
            raise RealeditError(
                f"missing {description}: {path}; run `./Build check-relink-realedit`"
            )

    base = read_symbol_facts(BASE_ELF)
    grown = read_symbol_facts(REALEDIT_ELF)

    base_function = require_symbol(base, OVERRIDDEN_FUNCTION, BASE_ELF)
    grown_function = require_symbol(grown, OVERRIDDEN_FUNCTION, REALEDIT_ELF)
    growth = grown_function.size - base_function.size
    if growth <= 0 or growth % 4:
        raise RealeditError(
            f"{OVERRIDDEN_FUNCTION} did not grow by whole instructions: "
            f"{base_function.size} -> {grown_function.size}"
        )
    for name in (NEW_FUNCTION, NEW_COUNTER, NEW_MAGIC):
        if name in base:
            raise RealeditError(f"{BASE_ELF}: fixture symbol {name} already present")
    new_function = require_symbol(grown, NEW_FUNCTION, REALEDIT_ELF)
    require_symbol(grown, NEW_COUNTER, REALEDIT_ELF)
    magic = require_symbol(grown, NEW_MAGIC, REALEDIT_ELF)

    expected_call = jal_word(new_function.value)
    call_sites = [
        offset
        for offset in range(grown_function.value,
                            grown_function.value + grown_function.size, 4)
        if pcsx_smoke.read_exe_word(REALEDIT_EXE, offset) == expected_call
    ]
    if len(call_sites) != 1:
        raise RealeditError(
            f"expected one JAL {NEW_FUNCTION} inside the grown "
            f"{OVERRIDDEN_FUNCTION}, found {len(call_sites)}"
        )

    loaded_magic = pcsx_smoke.read_exe_word(REALEDIT_EXE, magic.value)
    if loaded_magic != NEW_MAGIC_VALUE:
        raise RealeditError(
            f"{NEW_MAGIC} loads as 0x{loaded_magic:08x}, "
            f"expected 0x{NEW_MAGIC_VALUE:08x}"
        )

    audit = subprocess.run(
        [
            sys.executable, str(ROOT / "tools" / "reloc_input_audit.py"),
            "--linker", str(REALEDIT_LINKER),
            "--elf", str(REALEDIT_ELF),
            "--map", str(REALEDIT_MAP),
        ],
        cwd=ROOT,
        text=True,
        capture_output=True,
    )
    if not quiet and audit.stdout:
        print(audit.stdout, end="" if audit.stdout.endswith("\n") else "\n")
    if audit.returncode != 0:
        if audit.stderr:
            print(audit.stderr, file=sys.stderr, end="")
        raise RealeditError("strict input relocation audit failed")

    if not quiet:
        print(
            f"realedit static: {OVERRIDDEN_FUNCTION} {base_function.size}->"
            f"{grown_function.size} (+{growth}), one JAL {NEW_FUNCTION}"
            f"=0x{new_function.value:08x}, {NEW_MAGIC}=0x{loaded_magic:08x}, "
            "input audit clean"
        )


def run_smoke(quiet: bool = False) -> None:
    command = [
        sys.executable, str(ROOT / "tools" / "pcsx_smoke.py"),
        "--exe", str(REALEDIT_EXE),
        "--elf", str(REALEDIT_ELF),
        "--watch-counter", NEW_COUNTER,
        "--watch-equals", f"{NEW_MAGIC}=0x{NEW_MAGIC_VALUE:08X}",
        "--frames", "10",
        "--loop-hits", "3",
        "--timeout", "90",
    ]
    result = subprocess.run(command, cwd=ROOT, text=True, capture_output=True)
    marker_lines = [
        line
        for line in result.stdout.splitlines()
        if "TENCHU_SMOKE" in line
    ]
    if not quiet:
        for line in marker_lines:
            print(line)
    if result.returncode != 0:
        if result.stderr:
            print(result.stderr, file=sys.stderr, end="")
        raise RealeditError(
            "realedit smoke failed; set TENCHU_REALEDIT_NO_SMOKE=1 only for "
            "environments without pcsx-redux or the original disc"
        )


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--no-smoke", action="store_true",
                        help="skip the emulator boot verification")
    parser.add_argument("-q", "--quiet", action="store_true")
    args = parser.parse_args(argv)
    skip_smoke = args.no_smoke or os.environ.get("TENCHU_REALEDIT_NO_SMOKE") == "1"
    try:
        verify_static(quiet=args.quiet)
        if skip_smoke:
            print("realedit: smoke skipped (TENCHU_REALEDIT_NO_SMOKE)")
        else:
            run_smoke(quiet=args.quiet)
    except RealeditError as error:
        print(f"relink-realedit: {error}", file=sys.stderr)
        return 1
    print("relink-realedit: grown-function modding contract holds")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
