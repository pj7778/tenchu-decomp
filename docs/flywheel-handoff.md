# Decompilation flywheel handoff — 2026-07-13

This is a dated shutdown snapshot, not a second match-status database.  The
current source, `tools/progress.py`, `tools/triage.py`, and each guarded
function's `STATUS` comment are authoritative.  Re-measure before dispatching
work; do not carry the percentages or target list below forward by assumption.

The flywheel was intentionally wound down at the user's request.  There are no
matcher agents still running, all harvested work is on `master`, and the root
worktree was clean when this note was written.  Resume only when the user asks
to restart matching work.

## Clean anchor

- `master`: `37d661a` (`Recognize branch phi register ties`)
- preceding function checkpoint: `f161de0` (`Document small-rotation Think
  allocator tie`)
- game code: 402/555 functions (72.43%), 191332/302824 bytes (63.18%)
- all six executables passed `./Build check-all` byte-identically
- all 285 matching-tool tests passed
- guarded-draft flags, fuzzy fingerprints, symbol notes, symbol uniqueness,
  and `D_<address>` symbol names passed their global checks

The progress numbers above are merely useful confirmation that the right
anchor was checked out.  Get the current numbers with:

```console
nix develop -c python tools/progress.py
```

## Resume preflight

Run these before selecting targets:

```console
git status --short
git log -5 --oneline
nix develop -c python tools/progress.py
nix develop -c python tools/triage.py
nix develop -c python tools/triage.py --parked
nix develop -c python tools/fuzz-score.py --check
nix develop -c python tools/maspsxflags.py --check
nix develop -c ./Build check-all
```

If the anchor or metadata has moved, that is normal: use the newly measured
state.  Before launching a particular target, run `tools/triage.py <Name>`,
read the top comment in `src/main.exe/<Name>.c`, and confirm that the current
source still contains an active `INCLUDE_ASM`.  This prevents duplicate work
on functions already promoted to exact C.

Run the slower repository-wide hygiene gates before harvesting the first batch
and again at shutdown:

```console
nix develop -c python tools/symnote.py --all --check
nix develop -c python tools/dedupe-symbols.py --check
nix develop -c python tools/symcheck.py
nix develop -c python -m unittest tools.tests.test_matching_tools
```

## Target selection

The user's standing preference is **Think\***, **Proc\***, and **Attack\***.
Honor that preference among *live* targets.  At this shutdown, most of the
remaining named functions in those families were already exact or explicitly
parked after extensive RTL/permuter work, so do not revive one solely because
its name has priority.

On the 2026-07-13 snapshot, a good fresh hard-gameplay slate was:

| Candidate | Size | Why it is useful |
|---|---:|---|
| `AntiWall` | 692 B | substantial gameplay routine, six callees, no known GTE blocker |
| `death_camera_something_` | 520 B | camera/gameplay logic with eight callees and multiply/divide |
| `create_ninken_character_` | 576 B | AI/character setup with two loops; near the preferred Think/Proc domain |
| `FUN_8004c350` | 588 B | misc-handler neighborhood, five callees, some similarity to `ProcMiscSnowfall` |
| `TurnAroundAllItems` | 408 B | item control, four callees, leverage 3, some similarity to `ProcItemGosin` |

Revalidate all five with current triage before using them.  Reasonable next
substitutes were `AfsGetEntry` (564 B), `FUN_8003d768` (672 B), and
`SelectStage` (420 B with a large 0x528-byte frame).  `calculate_score` is
smaller but has call-graph leverage 4.

Do not spend a slot on these merely because they appear difficult:

- GTE/COP2 routines and `DrawTMD` currently lack an acceptable pure-C path.
- `cd_open` and other unmatched jump-table stubs remain fiddly to harvest.
- Anything hidden by default `tools/triage.py` is parked; read its reason
  before considering it.

## Parked priority-family near-matches

These are the most tempting repeats.  Their source comments contain the full
evidence and rejected experiments.

| Function | Checkpoint | Why it stopped |
|---|---|---|
| `AttackShort` | 99.52%; exact 1668 B / 417 instructions / CFG; 8 bytes left | two `move v0,s0` vs `andi v0,s0,0xffff` sites; `narrow-copy-zero-extension`; 22,046 permuter iterations flat |
| `ProcItemNingyo` | 98.40%; exact 2256 B / 564 instructions / CFG; 19 bytes left | launch-position identity, modulus register, and constant scheduling; guided rules and late permuter flat |
| `ProcMiscPitfall` | exact 868 B / 217 instructions; 6 bytes left | global register-colouring tie; 160 guided candidates and 22,097 permuter candidates flat |
| `ProcMiscFire` | exact 356 B; 10 bytes left | confirmed `la` reload tie; RTL excludes the known compiler patch; 27,390 permuter candidates flat |
| `Think1ninja` | exact 356 B; 6 bytes left | fixing local abs shape triggers a worse global-allocation cascade; bounded permuter flat |
| `think_setting_small_rotation_small_steps_` | 98.84%; exact 1392 B / CFG; 5 bytes left | QImode branch-phi register tie; 160 guided candidates and 8,850 permuter candidates flat |
| `AttackBowControl` | 75.00%; exact 272 B; 19 bytes left | first-live-local allocator priority swap; 80k+ permuter candidates flat |
| `StageEndScreen` | 91.26%; exact 6084 B / frame / branch-call inventory; 389 bytes left | complete pure-C reconstruction; broad remaining allocator/scheduling residual |
| `DamageControl` | 88.23%; exact 5812 B / 1453 instructions; 2457 bytes left | complete structural draft, but 140 aligned residual blocks remain |

Only revisit one of these when a *new, specific* compiler/tool insight attacks
the recorded residual class.  Do not rerun the same broad autorules or
permuter search.

## Matching invariants

1. Matching functions stay pure C.  Do not land arbitrary `__asm__` patches.
2. `matchdiff.py`/whole-image bytes and CFG are authoritative.  Fuzzy score is
   presentation metadata, not the acceptance test.
3. After every meaningful guarded-draft edit, immediately run
   `tools/fuzz-score.py --only <Name>`.  After exact promotion, run it again to
   remove the obsolete row.  Never leave changed C with a stale or absent
   fuzzy fingerprint.
4. Use target assembly, demo symbols, siblings, xrefs, `rtlguide.py`, and
   `rtldump.py` before speculative source churn.  `rtlguide.py` now recognizes
   the branch-phi register-tie signature seen in the final Think checkpoint.
5. Run `autorules.py --guided` only on a compiling draft.  Use the permuter
   late: exact or one-instruction-off length, localized allocation/scheduling
   residual, one bounded run.  Never use it as the initial decompiler and do
   not paste a globally nonsensical winning candidate.
6. One worker owns one function or tightly coupled family in one isolated
   worktree.  Do not run matching tools concurrently against the same
   worktree/lock.
7. A worker commits either an exact result or an honestly documented improved
   checkpoint to its branch.  Root reviews and cherry-picks it, resolves only
   additive metadata conflicts, refreshes fuzzy data, and runs the gates.
8. Commit the function/checkpoint first.  Then perform the reflection pass and
   land any cookbook, diagnostic, autorule, test, or naming improvement in a
   separate commit.  Tooling must encode a repeatable decision, not one
   function's accidental spelling.

Recent mechanical lessons are already in
[`matching-cookbook.md`](matching-cookbook.md) and the tools/tests: narrow-copy
zero extension, subtraction-role fusion, branch-phi register ties,
difference-role fusion, shared-result returns, identical-arm fences, terminal
call returns, and default-ladder hoisting.  Search the cookbook and
`rtlguide.py` signatures before inventing a new manual experiment.

## Agent and worktree hygiene

Many historical `/tmp/tenchu-*` worktrees and `codex/*` branches still exist.
They are not evidence of active work.  The final three visible agents
(`activatehumans2`, `damage2`, and `stageend2`) all completed, and their useful
changes were already integrated or recreated on `master`.

Before touching an old worktree, inspect both its dirt and its branch delta:

```console
git worktree list
git -C /path/to/worktree status --short
git log --oneline master..branch-name
```

Do not blindly cherry-pick old worker hashes: integrated commits often have a
different hash after conflict resolution or recreation.  Prefer new uniquely
named worktrees/branches for a restarted batch, and do not delete old user
state as incidental cleanup.

The detailed launch, harvest, conflict, reflection, and shutdown procedures
remain in [`orchestration.md`](orchestration.md).  This file is only the clean
launch pad for the next run.
