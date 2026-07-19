# Real-edit regression fixture

The committed form of the 2026-07-20 grown-function proof: `PadProc.c` is the
matched translation unit plus a call into `mod_probe.c`, a brand-new TU with a
zero-initialized `.sbss` counter (`ModTickCount`) and an initialized `.sdata`
magic (`ModMagic = 0x600DF00D`).

`./Build check-relink-realedit` compiles both with the pinned pipeline,
replays them through the normal-relink composition in an isolated lane
(`.shake/relink-realedit/`) using the same override machinery as
`src/mod-relink/`, and verifies with `tools/relink_realedit.py`: the grown
function, the relocated call, the loaded magic word, a zero-finding input
audit, LMA/VMA congruence, and (unless `TENCHU_REALEDIT_NO_SMOKE=1`) a
PCSX-Redux boot in which the counter demonstrably increments.

If `src/main.exe/PadProc.c` changes, regenerate `PadProc.c` here by copying
it and re-adding the two `ModProbeTick` lines, then rerun the gate.
