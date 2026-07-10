#!/usr/bin/env python3
"""Carve PSX.SYM (and the demo executables) out of the Japanese demo disc.

The demo -- ``Rittai Ninja Katsugeki Tenchu (Japan) (Demo)``, PAPX-90029,
1997-10-26 -- shipped the Psy-Q linker's debug output for an earlier build of
the game inside its AFS archive, next to leftover build-pipeline junk
(VOLMAKE.BAT, AFSMAKE.EXE).  ``PSX.SYM`` is the only surviving source of the
game's original source-file names, function prototypes, struct/enum layouts and
static-vs-extern storage classes.  See docs/psx-sym.md.

Usage:  tools/extract-demo.py ~/Downloads/PAPX90029.BIN [-o disks/demo]

The disc is MODE2/2352.  We read the ISO9660 tree, pull TENCHU.VOL, then walk
its AFS directory (36-byte big-endian records, ``IX`` signature) and carve the
entries we care about.
"""
from __future__ import annotations

import argparse, hashlib, os, struct, sys

RAW = 2352
WANT = ("@2_PSX.SYM", "@2_PSX.EXE", "@2_GAME.EXE", "@2_RESTART.EXE", "@2_ENDING.EXE")
# sha1 of the carved artefacts, so a re-extract from a different dump is caught
EXPECT = {"PSX.SYM": "ffc15ac96e595a2c073529b368be8bcc53a52bcd"}


def sector(buf: bytes, lba: int) -> bytes:
    off = lba * RAW
    s = buf[off:off + RAW]
    if len(s) < RAW:
        return b""
    # mode 1: 16-byte header; mode 2 (CD-XA form 1): + 8-byte subheader
    return s[24:24 + 2048] if s[15] == 2 else s[16:16 + 2048]


def extent(buf: bytes, lba: int, length: int) -> bytes:
    n = (length + 2047) // 2048
    return b"".join(sector(buf, lba + i) for i in range(n))[:length]


def find_file(buf: bytes, want: str) -> tuple[int, int]:
    pvd = sector(buf, 16)
    if pvd[1:6] != b"CD001":
        sys.exit("not an ISO9660 image (no CD001 at LBA 16) -- is this MODE2/2352?")
    root = pvd[156:190]
    stack = [(struct.unpack_from("<I", root, 2)[0], struct.unpack_from("<I", root, 10)[0])]
    while stack:
        lba, size = stack.pop()
        data = extent(buf, lba, size)
        i = 0
        while i < len(data):
            rl = data[i]
            if rl == 0:
                i = (i // 2048 + 1) * 2048
                continue
            rec = data[i:i + rl]
            ext = struct.unpack_from("<I", rec, 2)[0]
            sz = struct.unpack_from("<I", rec, 10)[0]
            flags, nlen = rec[25], rec[32]
            name = rec[33:33 + nlen]
            i += rl
            if nlen == 1 and name in (b"\x00", b"\x01"):
                continue
            nm = name.split(b";")[0].decode("ascii", "replace")
            if flags & 2:
                stack.append((ext, sz))
            elif nm.upper() == want:
                return ext, sz
    sys.exit(f"{want} not found on the disc")


def afs_entries(vol: bytes) -> dict[str, tuple[int, int]]:
    if vol[:11] != b"AFS_VOL_200":
        sys.exit(f"not an AFS volume: {vol[:16]!r}")
    out: dict[str, tuple[int, int]] = {}
    i = vol.find(b"IX\x00\x02")          # first directory record
    while i + 36 <= len(vol) and vol[i:i + 2] == b"IX":
        loc, size, _alloc = struct.unpack_from(">III", vol, i + 4)
        nm = vol[i + 16:i + 36].split(b"\x00")[0].decode("ascii", "replace")
        out[nm] = (loc, size)
        i += 36
    return out


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("bin", help="PAPX90029.BIN (MODE2/2352)")
    ap.add_argument("-o", "--out", default="disks/demo")
    args = ap.parse_args()

    buf = open(args.bin, "rb").read()
    print(f"{args.bin}: {len(buf)//RAW} sectors")
    lba, size = find_file(buf, "TENCHU.VOL")
    vol = extent(buf, lba, size)
    print(f"TENCHU.VOL: lba={lba} size={size}")

    ents = afs_entries(vol)
    print(f"AFS: {len(ents)} entries")
    os.makedirs(args.out, exist_ok=True)
    for key in WANT:
        if key not in ents:
            print(f"  MISSING {key}")
            continue
        loc, sz = ents[key]
        blob = vol[loc:loc + sz]
        name = key.split("_", 1)[1]
        path = os.path.join(args.out, name)
        with open(path, "wb") as f:
            f.write(blob)
        h = hashlib.sha1(blob).hexdigest()
        ok = ""
        if name in EXPECT:
            ok = "  OK" if h == EXPECT[name] else f"  !! expected {EXPECT[name]}"
        print(f"  {name:14s} {sz:>9} bytes  sha1={h}{ok}")


if __name__ == "__main__":
    main()
