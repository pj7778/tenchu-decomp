#!/usr/bin/env python3
"""Launch the decomp's build in pcsx-redux. Two modes:

  --loadexe EXE   FAST (default): mount the original disc and -loadexe our PS-EXE
                  over it — no ISO repack, just `./Build` then relaunch. NOTE:
                  Tenchu boots SLPS_019.01 -> ... -> TENCHU/MAIN.EXE, so this jumps
                  straight to MAIN.EXE and skips that launcher. Great for iterating
                  on main.exe; use --iso if the game needs the full boot chain.

  --iso CUE       FAITHFUL: boot the repacked disc (our main.exe swapped in), so the
                  real SLPS_019.01 -> ... -> MAIN.EXE boot runs. Build it first with
                  `./Build run-iso` (which calls tools/mkiso.py).

pcsx-redux falls back to OpenBIOS if no real BIOS is configured. It's found via
$PCSX_REDUX, else `pcsx-redux` on PATH, else ~/programming/pcsx-redux/pcsx-redux.
Extra emulator flags come from $PCSX_REDUX_ARGS and any trailing args.

Usually invoked by `./Build run` (loadexe) / `./Build run-iso`.
"""
import argparse
import glob
import os
import shlex
import shutil
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BUILD = os.path.join(ROOT, ".shake", "build", "tenchu")
DEFAULT_EXE = os.path.join(BUILD, "main.exe")
DEFAULT_CUE = os.path.join(BUILD, "tenchu.cue")
FALLBACK_PCSX = os.path.expanduser("~/programming/pcsx-redux/pcsx-redux")


def find_pcsx():
    env = os.environ.get("PCSX_REDUX")
    if env:
        if os.path.exists(env):
            return env
        sys.exit(f"run: $PCSX_REDUX={env} does not exist.")
    return shutil.which("pcsx-redux") or (
        FALLBACK_PCSX if os.path.exists(FALLBACK_PCSX) else sys.exit(
            "run: pcsx-redux not found. Set PCSX_REDUX=/path/to/pcsx-redux, put it "
            "on PATH, or build it at ~/programming/pcsx-redux."))


def find_disc():
    # The original (copyrighted) disc you provide. Mirrors tools/mkiso.py find_cue().
    env = os.environ.get("TENCHU_CUE")
    if env:
        if os.path.exists(env):
            return env
        sys.exit(f"run: TENCHU_CUE={env} not found")
    for d in (os.path.join(ROOT, "disks"), os.path.expanduser("~/tenchu-iso")):
        cues = sorted(glob.glob(os.path.join(d, "*.cue")))
        if cues:
            return cues[0]
    sys.exit("run: original disc .cue not found. Set TENCHU_CUE=/path/to/game.cue\n"
             "     (the original disc is copyrighted — you provide it).")


def main():
    ap = argparse.ArgumentParser(add_help=True, description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    g = ap.add_mutually_exclusive_group()
    g.add_argument("--loadexe", metavar="EXE", nargs="?", const=DEFAULT_EXE,
                   help="fast: -loadexe this PS-EXE over the original disc")
    g.add_argument("--iso", metavar="CUE", nargs="?", const=DEFAULT_CUE,
                   help="faithful: boot this repacked disc image")
    args, extra = ap.parse_known_args()
    extra = shlex.split(os.environ.get("PCSX_REDUX_ARGS", "")) + extra

    pcsx = find_pcsx()
    if args.iso is not None:
        cue = os.path.abspath(args.iso)
        if not os.path.exists(cue):
            sys.exit(f"run: disc image not found: {cue}\n"
                     f"     Build it with `./Build run-iso`.")
        cmd = [pcsx, "-run", "-iso", cue, *extra]
    else:
        exe = os.path.abspath(args.loadexe or DEFAULT_EXE)
        if not os.path.exists(exe):
            sys.exit(f"run: {exe} missing — run `./Build` (or `./Build mod`) first.")
        cmd = [pcsx, "-run", "-iso", os.path.abspath(find_disc()), "-loadexe", exe, *extra]

    print("run:", " ".join(cmd))
    sys.exit(subprocess.run(cmd).returncode)


if __name__ == "__main__":
    main()
