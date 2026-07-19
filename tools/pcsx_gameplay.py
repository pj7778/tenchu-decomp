#!/usr/bin/env python3
"""Run the deep-runtime PCSX-Redux gameplay gate on a relinked executable.

Where ``pcsx_smoke.py`` proves boot and the main loop, this wrapper drives
the full disc chain unattended into a real mission and requires:

- ``CreateStage`` — a stage was constructed;
- ``ActivateHumans`` running once per frame, sustained — the character
  simulation is live (``ActSTATE``/``ActNORMAL`` also count when they
  dispatch); and
- sustained ``cbCheckCD`` callbacks — CD/XA streaming continued throughout.

The measured steady state on the ``+0x10004`` grown image is one
``ActivateHumans`` and thirty ``cbCheckCD`` per second of emulated 30 fps
gameplay, so the default thresholds represent about twenty seconds of
uninterrupted simulation.

All probe addresses come from the selected ELF and every breakpoint is
image-word-verified against the selected EXE, so launcher/menu code
executing the same RAM never counts.  ``--repack`` packages the EXE onto a
temporary disc first (including mkiso's auto-LBA layout for grown images);
``--disc`` boots an already-packaged cue directly.
"""

from __future__ import annotations

import argparse
import os
from pathlib import Path
import sys
import tempfile
from typing import Sequence

try:
    from tools import pcsx_smoke
except ImportError:  # Direct execution (``tools/pcsx_gameplay.py``).
    import pcsx_smoke  # type: ignore[no-redef]


ROOT = Path(__file__).resolve().parent.parent
DEFAULT_EXE = ROOT / ".shake" / "build" / "reloc-growth-probe" / "main_growth.exe"
DEFAULT_ELF = ROOT / ".shake" / "build" / "reloc-growth-probe" / "main_growth.exe.elf"
LUA_PROBE = Path(__file__).with_name("pcsx-gameplay.lua")

PROBE_SYMBOLS = {
    "MAIN": "main",
    "LOOP": "PadProc",
    "CREATESTAGE": "CreateStage",
    "ACTIVATE": "ActivateHumans",
    "ACTSTATE": "ActSTATE",
    "ACTNORMAL": "ActNORMAL",
    "CBCHECK": "cbCheckCD",
}


def parse_args(argv: Sequence[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--exe", type=Path, default=DEFAULT_EXE)
    parser.add_argument("--elf", type=Path, default=DEFAULT_ELF)
    parser.add_argument("--cue", type=Path, help="original disc .cue")
    parser.add_argument(
        "--disc", type=Path,
        help="boot this already-packaged .cue instead of repacking",
    )
    parser.add_argument("--pcsx", type=Path, help="PCSX-Redux executable")
    parser.add_argument("--bios", type=Path, help="optional real PS1 BIOS")
    parser.add_argument(
        "--repack", action="store_true",
        help="package the selected EXE onto a temporary disc first",
    )
    parser.add_argument("--repack-timeout", type=float, default=300.0)
    parser.add_argument(
        "--timeout", type=float, default=1200.0,
        help="emulator wall-clock budget (movie, menus, and load are slow)",
    )
    parser.add_argument("--act-min", type=int, default=600,
                        help="required character-simulation dispatches")
    parser.add_argument("--cb-min", type=int, default=300,
                        help="required CD/XA streaming callbacks")
    parser.add_argument("--max-vsyncs", type=int, default=60000,
                        help="emulated frame budget before the probe fails")
    args = parser.parse_args(argv)
    if args.repack and args.disc:
        parser.error("--repack and --disc are mutually exclusive")
    if args.timeout <= 0 or args.repack_timeout <= 0:
        parser.error("timeouts must be positive")
    if args.act_min <= 0 or args.cb_min <= 0 or args.max_vsyncs <= 0:
        parser.error("thresholds must be positive")
    return args


def run(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv)
    try:
        exe = pcsx_smoke._existing_file(args.exe, "relinked PS-X EXE")
        elf = pcsx_smoke._existing_file(args.elf, "relinked ELF")
        pcsx = pcsx_smoke.discover_pcsx(args.pcsx)
        cue = (
            pcsx_smoke._existing_file(args.disc, "packaged disc cue")
            if args.disc
            else pcsx_smoke.discover_cue(args.cue)
        )
        bios = pcsx_smoke._existing_file(args.bios, "BIOS") if args.bios else None
        pcsx_smoke._existing_file(LUA_PROBE, "PCSX gameplay Lua probe")
        metadata = pcsx_smoke.psxexe.read_elf_metadata(elf)
        environment = os.environ.copy()
        for env_suffix, symbol in PROBE_SYMBOLS.items():
            address = metadata.symbol(symbol)
            environment[f"TENCHU_GAMEPLAY_{env_suffix}"] = str(address)
            environment[f"TENCHU_GAMEPLAY_{env_suffix}_W"] = str(
                pcsx_smoke.read_exe_word(exe, address)
            )
    except (pcsx_smoke.SmokeError, pcsx_smoke.psxexe.PsxExeError) as error:
        print(f"pcsx-gameplay: {error}", file=sys.stderr)
        return 2

    environment["TENCHU_GAMEPLAY_ACT_MIN"] = str(args.act_min)
    environment["TENCHU_GAMEPLAY_CB_MIN"] = str(args.cb_min)
    environment["TENCHU_GAMEPLAY_MAX_VSYNCS"] = str(args.max_vsyncs)

    with tempfile.TemporaryDirectory(prefix="tenchu-pcsx-gameplay-") as temporary:
        temp = Path(temporary)
        disc = cue
        if args.repack:
            try:
                disc, package_result = pcsx_smoke.package_disc(
                    exe, cue, temp, environ=environment,
                    timeout=args.repack_timeout,
                )
            except pcsx_smoke.SmokeError as error:
                print(f"pcsx-gameplay: {error}", file=sys.stderr)
                return 2
            if package_result.output:
                print(
                    package_result.output,
                    end="" if package_result.output.endswith("\n") else "\n",
                )
            if package_result.timed_out or package_result.returncode != 0:
                print("pcsx-gameplay: disc repack failed", file=sys.stderr)
                return 1

        command = pcsx_smoke.build_command(
            pcsx, disc, exe, temp, bios=bios, load_exe=False,
            lua_probe=LUA_PROBE,
        )
        try:
            result = pcsx_smoke.run_process(
                command, cwd=temp, environ=environment, timeout=args.timeout
            )
        except pcsx_smoke.SmokeError as error:
            print(f"pcsx-gameplay: {error}", file=sys.stderr)
            return 2

    for line in result.output.splitlines():
        if "TENCHU_GAMEPLAY" in line or "execute " in line:
            print(line)
    if result.timed_out:
        print(
            f"pcsx-gameplay: timed out after {args.timeout:g} seconds",
            file=sys.stderr,
        )
        return 124
    if result.returncode != 0 or "TENCHU_GAMEPLAY FAIL " in result.output:
        print("pcsx-gameplay: probe reported failure", file=sys.stderr)
        return 1
    if "TENCHU_GAMEPLAY PASS " not in result.output:
        print("pcsx-gameplay: emulator exited without the PASS marker", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(run())
