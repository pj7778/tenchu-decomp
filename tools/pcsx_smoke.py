#!/usr/bin/env python3
"""Run a bounded, non-interactive PCSX-Redux smoke test of ``main_relink.exe``.

By default the emulator loads the original disc for game assets but injects
the supplied PS-X EXE at the BIOS shell, matching the fast ``./Build run``
path.  ``--repack`` instead builds a temporary disc and exercises the full
``SLPS -> MENU -> MAIN`` handoff.  A Lua probe derives all code addresses from
the current executable/ELF pair and requires the executable entry, ``main``,
repeated ``PadProc`` calls from the main loop, post-loop VSyncs, and no
first-chance CPU exception.

This is deliberately a runtime sanity gate, not a byte-match check or proof of
the complete launcher/movie/audio path.  It never patches the executable.
"""

from __future__ import annotations

import argparse
from dataclasses import dataclass
import os
from pathlib import Path
import shutil
import signal
import struct
import subprocess
import sys
import tempfile
from typing import Mapping, Sequence

try:
    from tools import psxexe
except ImportError:  # Direct execution (``tools/pcsx_smoke.py``).
    import psxexe  # type: ignore[no-redef]


ROOT = Path(__file__).resolve().parent.parent
DEFAULT_EXE = ROOT / ".shake" / "build" / "tenchu" / "main_relink.exe"
DEFAULT_ELF = ROOT / ".shake" / "build" / "tenchu" / "main_relink.exe.elf"
LUA_PROBE = Path(__file__).with_name("pcsx-smoke.lua")
PSX_EXE_MAGIC = b"PS-X EXE"
PSX_EXE_ENTRY_OFFSET = 0x10


class SmokeError(RuntimeError):
    """An invalid input or failed host-side smoke-test invariant."""


@dataclass(frozen=True)
class ProbeAddresses:
    entry: int
    main: int
    loop: int
    watch_counter: int | None = None
    watch_equals: int | None = None
    watch_equals_value: int | None = None


@dataclass(frozen=True)
class ProcessResult:
    returncode: int
    output: str
    timed_out: bool = False


def _existing_file(path: str | Path, description: str) -> Path:
    candidate = Path(path).expanduser().resolve()
    if not candidate.is_file():
        raise SmokeError(f"{description} not found: {candidate}")
    return candidate


def _existing_executable(path: str | Path, description: str) -> Path:
    candidate = _existing_file(path, description)
    if not os.access(candidate, os.X_OK):
        raise SmokeError(f"{description} is not executable: {candidate}")
    return candidate


def discover_pcsx(
    explicit: str | Path | None,
    *,
    environ: Mapping[str, str] = os.environ,
    home: Path | None = None,
) -> Path:
    """Use the same explicit/env/PATH/checkout preference as ``./Build run``."""
    if explicit is not None:
        return _existing_executable(explicit, "PCSX-Redux executable")
    configured = environ.get("PCSX_REDUX")
    if configured:
        return _existing_executable(configured, "$PCSX_REDUX")
    on_path = shutil.which("pcsx-redux")
    if on_path:
        return _existing_executable(on_path, "pcsx-redux on PATH")
    fallback = (home or Path.home()) / "programming" / "pcsx-redux" / "pcsx-redux"
    if fallback.is_file() and os.access(fallback, os.X_OK):
        return fallback.resolve()
    raise SmokeError(
        "pcsx-redux not found; pass --pcsx, set PCSX_REDUX, put it on PATH, "
        "or build it under ~/programming/pcsx-redux"
    )


def discover_cue(
    explicit: str | Path | None,
    *,
    environ: Mapping[str, str] = os.environ,
    root: Path = ROOT,
    home: Path | None = None,
) -> Path:
    """Find the user-supplied original disc using the documented search order."""
    if explicit is not None:
        return _existing_file(explicit, "original disc cue")
    configured = environ.get("TENCHU_CUE")
    if configured:
        return _existing_file(configured, "$TENCHU_CUE")
    for directory in (root / "disks", (home or Path.home()) / "tenchu-iso"):
        if not directory.is_dir():
            continue
        cues = sorted(
            path for path in directory.iterdir()
            if path.is_file() and path.suffix.lower() == ".cue"
        )
        if cues:
            return cues[0].resolve()
    raise SmokeError(
        "original disc .cue not found; pass --cue, set TENCHU_CUE, or place "
        "it under disks/ or ~/tenchu-iso/"
    )


def read_psx_exe_entry(path: Path) -> int:
    with path.open("rb") as source:
        header = source.read(PSX_EXE_ENTRY_OFFSET + 4)
    if len(header) < PSX_EXE_ENTRY_OFFSET + 4 or header[:8] != PSX_EXE_MAGIC:
        raise SmokeError(f"{path}: not a complete PS-X EXE header")
    entry = struct.unpack_from("<I", header, PSX_EXE_ENTRY_OFFSET)[0]
    if entry == 0:
        raise SmokeError(f"{path}: PS-X EXE entry is zero")
    return entry


def read_exe_word(path: Path, address: int) -> int:
    """Read the u32 the PS-X EXE will place at ``address`` once loaded.

    The probe breakpoints verify these image words at runtime: on the disc
    path SLPS/MENU code occupies the same RAM before MAIN.EXE, so an
    execution of the bare address is not yet evidence that our executable
    is running.
    """
    data = path.read_bytes()
    if len(data) < 0x800 or data[:8] != PSX_EXE_MAGIC:
        raise SmokeError(f"{path}: not a complete PS-X EXE header")
    t_addr, t_size = struct.unpack_from("<II", data, 0x18)
    if not t_addr <= address <= t_addr + t_size - 4:
        raise SmokeError(
            f"{path}: address 0x{address:08x} is outside the loaded image "
            f"0x{t_addr:08x}..0x{t_addr + t_size:08x}"
        )
    offset = 0x800 + (address - t_addr)
    if offset + 4 > len(data):
        raise SmokeError(f"{path}: image truncated at 0x{address:08x}")
    return struct.unpack_from("<I", data, offset)[0]


def parse_watch_equals(spec: str) -> tuple[str, int]:
    """Parse ``SYMBOL=VALUE`` (any int base) for the equality watch."""
    symbol, separator, raw_value = spec.partition("=")
    if not separator or not symbol or not raw_value:
        raise SmokeError(f"--watch-equals expects SYMBOL=VALUE, got: {spec}")
    try:
        value = int(raw_value, 0)
    except ValueError as error:
        raise SmokeError(f"--watch-equals value is not an integer: {spec}") from error
    if not 0 <= value <= 0xFFFFFFFF:
        raise SmokeError(f"--watch-equals value must fit in 32 bits: {spec}")
    return symbol, value


def resolve_probe_addresses(
    exe: Path,
    elf: Path,
    *,
    main_symbol: str = "main",
    loop_symbol: str = "PadProc",
    watch_counter_symbol: str | None = None,
    watch_equals: tuple[str, int] | None = None,
) -> ProbeAddresses:
    entry = read_psx_exe_entry(exe)
    try:
        metadata = psxexe.read_elf_metadata(elf)
        main = metadata.symbol(main_symbol)
        loop = metadata.symbol(loop_symbol)
        watch_counter = (
            metadata.symbol(watch_counter_symbol)
            if watch_counter_symbol is not None
            else None
        )
        watch_equals_address = (
            metadata.symbol(watch_equals[0]) if watch_equals is not None else None
        )
    except (OSError, psxexe.PsxExeError) as error:
        raise SmokeError(str(error)) from error
    return ProbeAddresses(
        entry=entry,
        main=main,
        loop=loop,
        watch_counter=watch_counter,
        watch_equals=watch_equals_address,
        watch_equals_value=watch_equals[1] if watch_equals is not None else None,
    )


def build_command(
    pcsx: Path,
    cue: Path,
    exe: Path,
    temp: Path,
    *,
    bios: Path | None = None,
    load_exe: bool = True,
    lua_probe: Path | None = None,
) -> list[str]:
    command = [
        str(pcsx),
        "-safe",
        "-no-ui",
        "-noupdate",
        "-no-fastboot",
        "-2mb",
        "-run",
        "-iso",
        str(cue),
        # Lua execution breakpoints require the debugger-enabled interpreter.
        # Do not use -testmode: it suppresses first-chance exception pauses,
        # which are one of the failure signals the Lua probe observes.
        "-interpreter",
        "-debugger",
        "-softgpu",
        "-memcard1",
        str(temp / "memcard1.mcd"),
        "-memcard2",
        str(temp / "memcard2.mcd"),
    ]
    if load_exe:
        command.extend(("-loadexe", str(exe)))
    if bios is not None:
        command.extend(("-bios", str(bios)))
    command.extend(("-dofile", str((lua_probe or LUA_PROBE).resolve())))
    return command


def package_disc(
    exe: Path,
    original_cue: Path,
    temp: Path,
    *,
    environ: Mapping[str, str],
    timeout: float,
) -> tuple[Path, ProcessResult]:
    """Build a temporary disc with mkiso.py, including its grown auto-LBA path."""
    if shutil.which("mkpsxiso") is None:
        raise SmokeError("--repack needs mkpsxiso; run the smoke test inside `nix develop`")
    output_prefix = temp / "tenchu-smoke"
    command = [
        sys.executable,
        str(ROOT / "tools" / "mkiso.py"),
        "--exe",
        str(exe),
        "-o",
        str(output_prefix),
    ]
    package_environment = dict(environ)
    package_environment["TENCHU_CUE"] = str(original_cue)
    result = run_process(
        command, cwd=ROOT, environ=package_environment, timeout=timeout
    )
    return output_prefix.with_suffix(".cue"), result


def _terminate_process_group(process: subprocess.Popen[str]) -> None:
    if process.poll() is not None:
        return
    if os.name == "posix":
        try:
            os.killpg(process.pid, signal.SIGTERM)
        except ProcessLookupError:
            pass
    else:  # pragma: no cover - the project runtime lane is Linux-first.
        process.terminate()


def _kill_process_group(process: subprocess.Popen[str]) -> None:
    if process.poll() is not None:
        return
    if os.name == "posix":
        try:
            os.killpg(process.pid, signal.SIGKILL)
        except ProcessLookupError:
            pass
    else:  # pragma: no cover - the project runtime lane is Linux-first.
        process.kill()


def run_process(
    command: Sequence[str],
    *,
    cwd: Path,
    environ: Mapping[str, str],
    timeout: float,
) -> ProcessResult:
    try:
        process = subprocess.Popen(
            list(command),
            cwd=cwd,
            env=dict(environ),
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            start_new_session=True,
        )
    except OSError as error:
        raise SmokeError(f"could not start {command[0]}: {error}") from error
    try:
        output, _ = process.communicate(timeout=timeout)
        return ProcessResult(process.returncode, output)
    except subprocess.TimeoutExpired:
        _terminate_process_group(process)
        try:
            output, _ = process.communicate(timeout=5)
        except subprocess.TimeoutExpired:
            _kill_process_group(process)
            output, _ = process.communicate()
        return ProcessResult(process.returncode, output, timed_out=True)


def parse_args(argv: Sequence[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--exe", type=Path, default=DEFAULT_EXE)
    parser.add_argument("--elf", type=Path, default=DEFAULT_ELF)
    parser.add_argument("--cue", type=Path, help="original disc .cue")
    parser.add_argument("--pcsx", type=Path, help="PCSX-Redux executable")
    parser.add_argument("--bios", type=Path, help="optional real PS1 BIOS")
    parser.add_argument(
        "--repack",
        action="store_true",
        help=(
            "temporarily repack the selected EXE and boot the full "
            "SLPS->MENU->MAIN disc path"
        ),
    )
    parser.add_argument(
        "--timeout", type=float,
        help="emulator timeout (default: 60s direct, 180s with --repack)",
    )
    parser.add_argument("--repack-timeout", type=float, default=300.0)
    parser.add_argument("--frames", type=int, default=12,
                        help="VSyncs required after reaching the main loop")
    parser.add_argument("--loop-hits", type=int, default=3,
                        help="required calls to the main-loop PadProc probe")
    parser.add_argument("--main-symbol", default="main")
    parser.add_argument("--loop-symbol", default="PadProc")
    parser.add_argument(
        "--watch-counter", metavar="SYMBOL",
        help=(
            "require the u32 at SYMBOL (resolved from the selected ELF) to be "
            "nonzero and to increase while the main loop runs"
        ),
    )
    parser.add_argument(
        "--watch-equals", metavar="SYMBOL=VALUE",
        help=(
            "require the u32 at SYMBOL (resolved from the selected ELF) to "
            "equal VALUE once the game is running"
        ),
    )
    args = parser.parse_args(argv)
    if args.timeout is not None and args.timeout <= 0:
        parser.error("--timeout must be positive")
    if args.repack_timeout <= 0:
        parser.error("--repack-timeout must be positive")
    if args.frames <= 0:
        parser.error("--frames must be positive")
    if args.loop_hits <= 0:
        parser.error("--loop-hits must be positive")
    return args


def run(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv)
    try:
        exe = _existing_file(args.exe, "relinked PS-X EXE")
        elf = _existing_file(args.elf, "relinked ELF")
        pcsx = discover_pcsx(args.pcsx)
        cue = discover_cue(args.cue)
        bios = _existing_file(args.bios, "BIOS") if args.bios else None
        _existing_file(LUA_PROBE, "PCSX smoke Lua probe")
        watch_equals = (
            parse_watch_equals(args.watch_equals)
            if args.watch_equals is not None
            else None
        )
        addresses = resolve_probe_addresses(
            exe, elf, main_symbol=args.main_symbol, loop_symbol=args.loop_symbol,
            watch_counter_symbol=args.watch_counter, watch_equals=watch_equals,
        )
    except SmokeError as error:
        print(f"pcsx-smoke: {error}", file=sys.stderr)
        return 2

    try:
        entry_word = read_exe_word(exe, addresses.entry)
        main_word = read_exe_word(exe, addresses.main)
        loop_word = read_exe_word(exe, addresses.loop)
    except SmokeError as error:
        print(f"pcsx-smoke: {error}", file=sys.stderr)
        return 2

    environment = os.environ.copy()
    environment.update(
        {
            "TENCHU_SMOKE_ENTRY": str(addresses.entry),
            "TENCHU_SMOKE_MAIN": str(addresses.main),
            "TENCHU_SMOKE_LOOP": str(addresses.loop),
            "TENCHU_SMOKE_ENTRY_WORD": str(entry_word),
            "TENCHU_SMOKE_MAIN_WORD": str(main_word),
            "TENCHU_SMOKE_LOOP_WORD": str(loop_word),
            "TENCHU_SMOKE_FRAMES": str(args.frames),
            "TENCHU_SMOKE_LOOPS": str(args.loop_hits),
            "TENCHU_SMOKE_AUTOPAD": "1" if args.repack else "0",
        }
    )
    if addresses.watch_counter is not None:
        environment["TENCHU_SMOKE_WATCH_COUNTER"] = str(addresses.watch_counter)
    if addresses.watch_equals is not None:
        environment["TENCHU_SMOKE_WATCH_EQUALS_ADDR"] = str(addresses.watch_equals)
        environment["TENCHU_SMOKE_WATCH_EQUALS_VALUE"] = str(
            addresses.watch_equals_value
        )

    emulator_timeout = (
        args.timeout if args.timeout is not None else (180.0 if args.repack else 60.0)
    )
    with tempfile.TemporaryDirectory(prefix="tenchu-pcsx-smoke-") as temporary:
        temp = Path(temporary)
        disc = cue
        if args.repack:
            try:
                disc, package_result = package_disc(
                    exe, cue, temp, environ=environment, timeout=args.repack_timeout
                )
            except SmokeError as error:
                print(f"pcsx-smoke: {error}", file=sys.stderr)
                return 2
            if package_result.output:
                print(
                    package_result.output,
                    end="" if package_result.output.endswith("\n") else "\n",
                )
            if package_result.timed_out:
                print(
                    f"pcsx-smoke: disc repack timed out after {args.repack_timeout:g} seconds",
                    file=sys.stderr,
                )
                return 124
            if package_result.returncode != 0 or not disc.is_file():
                print(
                    f"pcsx-smoke: disc repack failed with status {package_result.returncode}",
                    file=sys.stderr,
                )
                return 1

        command = build_command(
            pcsx, disc, exe, temp, bios=bios, load_exe=not args.repack
        )
        try:
            result = run_process(
                command, cwd=temp, environ=environment, timeout=emulator_timeout
            )
        except SmokeError as error:
            print(f"pcsx-smoke: {error}", file=sys.stderr)
            return 2

    if result.output:
        print(result.output, end="" if result.output.endswith("\n") else "\n")
    if result.timed_out:
        print(
            f"pcsx-smoke: timed out after {emulator_timeout:g} seconds",
            file=sys.stderr,
        )
        return 124
    if result.returncode != 0:
        print(
            f"pcsx-smoke: PCSX-Redux exited with status {result.returncode}",
            file=sys.stderr,
        )
        return 1
    if "TENCHU_SMOKE FAIL " in result.output:
        print("pcsx-smoke: Lua probe reported failure", file=sys.stderr)
        return 1
    if "TENCHU_SMOKE PASS " not in result.output:
        print("pcsx-smoke: PCSX-Redux exited without the Lua PASS marker", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(run())
