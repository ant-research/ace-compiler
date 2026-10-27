# PR #676 Refactor Plan

This document proposes how to restructure PR #676 (commits `e62a4f57^..HEAD` on
`feat/openfhe-c-bootstrap`) to satisfy the reviewer's design-first process and
the per-commit code-volume requirement.

This is a one-time refactor work-plan. Per the reviewer principle ("commit what
you will maintain; never commit what is true for one machine at one point in
time") the durable architecture and stage content IS committed to the permanent
tree at `doc/pr676-refactor/REFACTOR-PLAN.md` per the reviewer request. The
per-PR SEED drafts under `doc/pr676-refactor/pr2-...`/ and `pr3-...`/ stay
local/untracked alongside it (their promoted, committed copies live at
`doc/rtlib-py-doc/` and `doc/rtlib-c-bootstrap/`).

The plan is a single coherent statement. Earlier draft layers (an adversarial
"override" appendix and a "calibration" appendix) are folded into the body here;
there is no section that rewrites another section. Per-PR corrections sourced
from the prfix cleanup worktree (branch `fix/rt-openfhe-level-reduce`, HEAD
`8e1011cc`) and from an adversarial pass are baked into the stages they affect.

## 1. What the reviewer objected to, and the real numbers

The reviewer's stated number (1700 LOC) understates the problem. The actual PR
is ~19 commits over 108 files, and the opening commit alone dwarfs the figure
the reviewer was reacting to:

- **e62a4f57** "feat:rtlib-python-openfhe, e2e resnet20 tested" -- ~12394
  insertions across 57 files in a single checkin, ~7x the number the reviewer
  named. Carries design docs + architecture headers + .cxx impl + three
  independent providers + a test harness + refactor of existing test drivers +
  .gitignore hygiene + result-dump scripts in one commit.
- **aba1d435** "add EvalBootstrap to the C-side OpenFHE backend" -- ~4257 LOC /
  21 files, bundling a design plan, the stage-N journals (build / compiler-hook
  / impl / bootstrap-test / harness / regress58 / fixspec / resnet20),
  architecture headers, a .cxx impl, incidental backend bug fixes, a gtest, a
  smoke example, plain .inc selectors, and a result dump.
- **24cb1cf7** "integrate fhert OpenFHE-1.4.2 backend" -- ~3143 LOC / 22 files
  mixing provider impl + tests + a design-handoff doc + result CSV/JSON dumps
  + a refactor migration in one checkin.
- **cba6b674** "add local_test corpus results" -- ~967 LOC / 10 files mixing
  durable integration methodology, BUILD/README fixes, per-phase PASS/NOISY
  snapshots, and a result-matrix README.

Layer-mixing inside single checkins is pervasive. No design-first gate exists:
design/architecture docs and the frozen runtime contract are committed in the
SAME commit as their implementation (`e62a4f57`, `aba1d435`), not proposed and
approved first. No proof-of-concept stage precedes any impl -- the path jumps
from nothing to a 12k-LOC implementation. Three independent provider layers ship
together in `e62a4f57` (rt_ref ~987, rt_openfhe ~1595, rt_fhert ~1488) instead of
staged with approval between, even though rt_fhert is explicitly a near-verbatim
port of rt_openfhe.

Machine-point-in-time data entered git in at least eight commits: result CSVs
and JSON with dated run labels (round/phase tags with ISO dates, e.g.
`r3/phase1/r3postfix`), per-run seconds, `max_abs` numbers, machine-local
absolute paths (build-host workspace-rooted and tmp-rooted paths), and captured
stdout; plus one-off repro/adjudication scripts committed as if maintained.
This violates "never commit what is true for one machine at one point in time"
(section 5 is the governing triage).

## 2. The process this rebuild follows

For this PR the expected sequence is a set of approval gates, each producing a
reviewable commit (or small commit series) before the next layer lands. The PR
is split **by function, not by phase**: each functional PR (F1-F6) is a vertical
slice that carries its own design doc -> POC -> contract/headers -> impl ->
tests -> summary -> follow-up internally. F0 is the one deliberately horizontal
PR and it is kept slim: it holds only the genuinely shared interfaces (the
frozen runtime contract and the emitter headers) and the single shared design
that governs them. No PR bundles multiple heavy backends.

### 2.1 Approval gates (vertical, inside each functional PR)

1. **Design proposal** -- a standalone doc OR a recorded meeting outcome,
   approved after discussion. No code. Within each functional PR the design is
   reviewed and approved before POC / headers / impl are pushed; it may be a
   separate pre-impl PR when a hard merge gate is wanted, otherwise the first
   commits of the PR, with the PR not advancing to impl until design is approved.
2. **Proof of concept** -- a tiny hand-built example proving the load-bearing
   assumption before headers land.
3. **Headers / contract** -- the interface contracts on both sides of every
   boundary, approvable as a translation unit before any body.
4. **Implementation, per backend** -- the simplest reference first, then the
   dependents; the C-side bootstrap and the SIHE scalar/mask ops land as
   separate impl commits.
5. **Tests** -- focused unit tests, then the executor/workloads harness, then the
   e2e onnx driver, then the bootstrap smoke + onnx sweep; CMake wiring last.
6. **Summary / follow-up** -- (a) the in-tree PR-message marker at
   `doc/pr676-summaries/NN-*.md` (the per-PR description that would otherwise
   live in a separate PR message, since the review system allows only one total
   PR; it carries scope, stages, gate results, the section-2.6 backup-diff
   status, and an honest follow-up, with no machine-point-in-time data);
   (b) an open-issues catalog naming known limitations and deferred work
   honestly (see 2.3); (c) a per-PR backup (a v2 worktree snapshot excluding
   build, a full-history git bundle with no prerequisites, plus a LATEST pointer
   file).

### 2.2 Incremental construction of monolithic files

Several provider backends live in one large `__init__.py` (rt_openfhe core +
bootstrap + df + shim; rt_fhert skeleton + SHE + host bootstrap + device seams
+ FLEX). These are built **incrementally**: each stage leaves the file in a
complete, importable state for that stage, and the diff from stage N to N+1 is
**additive-only** (new methods appended; existing function bodies not rewritten).
The reviewer therefore sees a sequence of small, reviewable additions -- never a
final-version dump, never an opaque blob.

The intermediate layer-states (skeleton-only, SHE-only, host-bootstrap-only,
device-seams-only) never existed in git. They are hand-re-authored as real,
coherent, importable states -- do NOT attempt `git checkout <hash> -- <file>` +
`git add -p` for these, which can only separate hunks within one blob and
cannot manufacture a state that never existed (and cannot cleanly separate
interleaved core/bootstrap/df/device methods). `git checkout <hash> -- <paths>`
is valid for physically separate files (headers, .cxx bodies, tests, docs).
The gate for every impl stage on a monolithic file is: **the file imports at
this stage; the new stage's ops work; the diff vs the previous stage is
additive-only.**

For the rt_openfhe/rt_fhert monolithic files the FLEX and device features are
threaded as localized branches INSIDE methods whose bodies land in earlier
stages. To keep diffs additive-only: (i) the skeleton stage (F2.3a / F4.4)
lands the FLEX parser (`_parse_flex_scaling`, `_scaling_technique` where it
exists, and the `_FLEX_MODE`/`_FLEX_EXT`/`_FLEX_SCALE` module globals -- these
are pre-existing in the providers, not net-new from 2859787f) and ships the
FULL dispatcher with the bootstrap/device branches as forward references
(Python late binding makes these import-safe; the 3a gate never takes those
branches); (ii) every method body that later gains a FLEX/device branch
pre-bakes the no-op branch (`if _FLEX_MODE:` / `if _device_mode():` guarded,
defaulting to the fixed/host path) at the stage it first lands; (iii) the
later FLEX/device stages (F2.5 / F4.7 / F4.8) change only the helper functions
and activation assignments -- NOT the bodies of methods added earlier. This is
the only way the sec. 2.2 additive-only diff holds against the real interleaving
(verified: ~50 FLEX refs interleave rt_openfhe; ~159 FLEX/device refs
interleave rt_fhert).

### 2.3 Honest limitations vs. unfinished content

A feature may ship with a documented known limitation. It may not ship
unfinished work silently framed as done. Concretely:

- **fhert device mode** is a working feature and is included in F4: lenet_x2
  and the operator-level regress58 device suite pass. `resnet20` device is a
  known not-yet-passing case (device-HBM per-cycle accumulation; the principled
  fix B1 -- a per-cycle `dev.reset_scratch` on the device allocator -- is future
  work and is NOT implemented this round). The resnet20 device case is gated out
  of the test gate and recorded in the open-issues catalog, never presented as
  passing.
- **openfhe C-side FIXEDMANUAL drift** is NOT claimed solved. The C-side
  bootstrap (F3) ships per the C-bootstrap docs (encode/seam fix; the docs
  report resnet20 acc=1.0/pred=3). The scale-drift investigation
  (`doc/ant-vs-openfhe-ckks-scale-drift/`, untracked) disagrees and was never
  reconciled in code -- it is recorded in the open-issues catalog as needing a
  re-run of `rt_openfhe-c` to reconcile. The Python FLEX re-anchor shim (F2.5)
  is opt-in, host-only, off-by-default; it is labeled as such and does NOT
  imply the default path or the C-side path is fixed.

### 2.4 Result-data rule (the triage in section 5 governs)

Methodology, runners, catalogs, and architecture descriptions stay in git.
Result dumps, seconds/GB numbers, absolute paths, dated run labels, csv/json
run outputs, and one-off repro scripts leave git (OUT to a gitignored local dir,
or TRIM in place). Section 5 is the single authoritative OUT/TRIM/KEEP table.

### 2.5 LOC granularity

A single commit should be at most ~400-600 LOC, with headers, impl, and tests
in separable commits. Architecture headers may exceed this only when the file
is a cohesive 1:1 mirror of an existing contract; in that case the commit is
split into sub-commits by opcode group, not the file split into pieces. Anything
over ~600 LOC is flagged "OVER" and is sub-split or modularized before merge.
Result dumps and one-off repro scripts never enter git in any form.

### 2.6 Diff-against-backup verification (every commit)

The backup tree -- the pre-refactor backup snapshot of the feat tree at HEAD
`2859787f` (a LATEST pointer file records its path) -- is the pre-refactor
reference: the full `feat/openfhe-c-bootstrap` working-tree state at `2859787f`
(including unstaged/untracked). Every commit on the v2 rebuild worktree branch
is checked against it, in addition to its own functional gate.

Universal rule -- every commit's verification confirms:
1. the functionality that commit is responsible for is fully present in this commit; AND
2. in the scope this commit implements, the committed code matches the backup byte-for-byte, allowing ONLY the execution-time edits explicitly named for that stage (`3fabb255` style reflow, `+ckks_opcode.h` self-containment include, OUT-result trims, machine-path to env path-parameterization, CJK to English). A commit may implement a SUBSET of the backup (features arrive in later commits) -- the parts present must match the backup; the parts absent arrive in later commits.

Concrete checks:
- **Separate-file commit** (header/.cxx/test/doc): `diff <(git -C <v2-worktree> show HEAD:<path>) <backup>/<path>` shows ONLY the sanctioned deltas (or is empty) -- everything else byte-identical to the backup.
- **Monolithic `__init__.py` hand-re-author commit** (rt_ref F1.4a-c, rt_openfhe F2.3/2.4/2.5, rt_fhert F4.4-F4.8): the methods ADDED in this commit match the backup's version of those methods byte-for-byte (no-op/pre-baked branches present as no-ops at early stages). After the file's FINAL reconstitution stage (1.4c for rt_ref, F2.5 for rt_openfhe, F4.8 for rt_fhert), `git -C <v2-worktree> diff 2859787f -- <file>` is EMPTY -- the staged union reproduces the backup's logic exactly.

FLEX/device pre-bake callout (sec. 2.2(iii)) -- the safeguard the backup exists for: for rt_openfhe/rt_fhert, open a verification subagent BOTH after the early (fixed/host-only, pre-baked no-op) commit AND after the flex/device-fill commit. Early-check: the fixed/host branches match the backup's fixed/host branches and the no-op branches are inert by default. Fill-check: `git -C <v2-worktree> diff 2859787f -- <file>` is empty -- the early + fill union reproduces the backup's full FLEX/device logic, with the pre-baked no-op branches now activated. A mismatch blocks the commit.

This rule is implicit in every stage Gate (not repeated per cell); a gate fails if its backup-diff does not satisfy the two checks above.

## 3. PR breakdown (by function)

F0 is the slim shared foundation (contract + emitter headers + their shared
design). F1-F6 are vertical functional PRs, each carrying its own design doc
and POC. Dependencies: F0 -> F1 -> F2; F3 stands alone (C-side bootstrap touches
the C-path handler in master, not the python emitter headers); F2 -> F4 (rt_fhert
ports rt_openfhe); F1.2 -> F5; F0 -> F6 (doc-only PR; its methodology is
runtime-exercised against F1/F2/F4). No PR bundles multiple heavy backends.

The 7 functional units (F0 the horizontal foundation; F1-F6 the vertical
provider/methodology PRs) landed as 6 review-PRs, each closed with a marker
commit under `doc/pr676-summaries/` plus a per-PR backup: Part 1 = F0+F1 (merged
into one review-PR); Part 2 = F2; Part 3 = F3; Part 4 = F4; Part 5 = F5;
Part 6 = F6. (F0 and F1 stage tables stay separate below; only the review-PR
grouping merges F0+F1 into Part 1.)

LOC figures flagged "OVER" exceed the guideline and are sub-split or
modularized before merge. "DROP" / "OUT" entries are result-data removals
detailed in section 5.

### F0 -- Foundation: shared contract, emitter headers, shared emission design

| Stage | Title | Layer | From | Deps | LOC | Gate |
|---|---|---|---|---|---|---|
| 0.1 | rtlib-py emission + contract architecture (the trimmed shared design; landed distributed, not as a single standalone committed design doc) | design-doc | e3f6d7af, e62a4f57 | -- | ~250 (distributed) | the committed rtlib-py architecture doc (path per the foundation PR -- the shared emission design is the headers themselves in F0.2 + the foundation summary; the contract architecture is the `rtlib_py.common` layer in F0.3) names the load-bearing concepts -- `pass`/`contract`/`provider`/`df`/`lazy-bridge` each present (>= 1 hit each in the foundation PR's architecture material); no dated residue (no round/phase ISO-date labels, no per-run `max_abs`, no machine-local absolute paths) |
| 0.2 | ckks2py emitter headers at final `include/fhe/ckks/` (ctx + core + handler; sub-split `ir2py_handler.h` by opcode group: arithmetic/IO/query vs encode/bootstrap) | header | e62a4f57, 5d3f0896 | 0.1 | 2090 OVER -> 5 sub-commits (see sec. 3 OVER sub-splits) | `g++ -std=c++17 -fsyntax-only -I include -I "$NN_ADDON_DIR/include" -I "$AIR_INFRA_DIR/include" -I "$BUILD_DIR/include" include/fhe/ckks/ckks2py_driver.h` exits 0 (the aggregator transitively includes `ir2py_ctx.h`/`ir2py_core.h`/`ir2py_handler.h`); `grep -E 'CKKS2PY' include/fhe/ckks/ckks2py_pass.h` shows the `-CKKS2PY` group registered default-inactive; `diff <(grep -oE 'Handle_[a-z_]+' include/fhe/ckks/ir2c_handler.h | sort -u) <(grep -oE 'Handle_[a-z_]+' include/fhe/ckks/ir2py_handler.h | sort -u)` shows the 1:1 opcode mirror modulo the intentional `Handle_mod_switch`->`Handle_modswitch` collapse; `grep -c Handle_bootstrap include/fhe/ckks/ir2py_handler.h` >= 1 |
| 0.3 | rtlib_py common contract layer (frozen 36-name) + RuntimeProvider Protocol | header | e62a4f57 | 0.1 | 410 | `PYTHONPATH=rtlib-py python3 -c "import rtlib_py.common; from rtlib_py.common.contract import CONTRACT_NAMES, check_provider; from rtlib_py.common import register, ALL_PROVIDERS; import types; stub=types.ModuleType('stub'); [setattr(stub,n,lambda *a,**k:None) for n in CONTRACT_NAMES]; check_provider(stub); print('contract OK', len(CONTRACT_NAMES))"` exits 0 and reports 39 names (36 functional + `Cipher`/`Plain`/`FakeRtError`); `python3 -c "import rtlib_py.common"` exits 0 |

F0 holds ONLY the shared interfaces + their shared design. Per-backend design
docs and POCs (rt_openfhe, C-bootstrap, fhert) live inside F2, F3, F4.

### F1 -- rtlib-py framework + rt_ref

| Stage | Title | Layer | From | Deps | LOC | Gate |
|---|---|---|---|---|---|---|
| 1.1 | rtlib-py POC (gap-filler; never existed) | poc | e62a4f57 | 0.3 | 150 | `PYTHONPATH=rtlib-py python3 -c "from rtlib_py.rt_ref import Cipher, add_ciph, FakeRtError; a=Cipher([1.0,2.0],0,1); b=Cipher([3.0,4.0],0,1); r=add_ciph(a,b); assert list(r.slots)==[4.0,6.0] and r.scale_deg==1 and r.level==0; print('rt_ref add/ciph OK')"` exits 0; the POC `.py` (hand-built) imports the contract stub and runs one `add_ciph` end-to-end |
| 1.2 | ckks2py pass impl (.cxx bodies) incl. `-CKKS2PY:rt=` activation at final state | impl | e62a4f57, d1848ec3 | 0.2, 0.3 | 450 | `fhe_cmplr -CKKS2PY:rt=openfhe` builds and emits .py + .conf, no .c leak; default C path byte-identical |
| 1.3 | rtlib_py common driver impl | impl | e62a4f57 | 0.3 | 275 | `grep -E '^def run\(py_file, spec, provider' rtlib-py/rtlib_py/common/driver.py` exits 0 (driver `run()` entrypoint landed); `PYTHONPATH=rtlib-py:test/rtlib_py/py_verify python3 test/rtlib_py/py_verify/run_py_exec.py <a hand-built add/mul toy module> --provider rt_ref` exits 0 with `max_abs` within epsilon via the driver path |
| 1.4 | rt_ref plaintext provider (sub-split: core ops / encode-IO / query) | impl | e62a4f57 | 1.3 | 987 OVER -> 3 sub-commits | `PYTHONPATH=rtlib-py python3 -c "from rtlib_py.common.contract import CONTRACT_NAMES; import rtlib_py.rt_ref as r; assert [n for n in CONTRACT_NAMES if not hasattr(r,n)]==[]; print('rt_ref surface complete')"` exits 0 (36-name contract, no missing entries); a toy `add_ciph`/`mul_ciph`/`rescale_ciph`/`rotate_ciph` runs construction-exact (max_abs=0 by plaintext construction, no FHE noise); STRICT-mode `[strict]` level/scale invariants raise via `_check_level_match`/`_check_scale_match` (a pytest of a level-mismatch `add_ciph` raises `FakeRtError`) |
| 1.5 | poly2py allops C++ emit+exec test | test | e62a4f57 | 1.2, 1.4 | 392 | `ctest -R test_ckks2py_allops` green (emits .py + execs via run_py_exec against rt_ref) |
| 1.6 | py-verify executor + workloads + runners (incl. `gen_workload_onnx.py`) | test | e62a4f57 | 1.4, 1.5 | 875 OVER -> 3 sub-commits | `PYTHONPATH=rtlib-py:test/rtlib_py/py_verify python3 -c "import run_py_verify, workloads, gen_workload_onnx; print('harness OK')"` exits 0 (`workloads` imports `gen_workload_onnx` so the 1.6c-before-1.6b order matters); a one-workload rt_ref smoke (`python3 test/rtlib_py/py_verify/run_py_verify.py <one minimal workload> --provider rt_ref`) within epsilon |
| 1.7 | py-verify e2e onnx driver | test | e62a4f57 | 1.5, 1.6 | 433 | `ctest test_fhe_cmplr_py_verify_onnx` green (rt_ref over regress-58) |
| 1.8 | wire rtlib-py tests into CMake | test | e62a4f57 | 1.5, 1.7 | 48 | `grep -E 'SKIP_RETURN_CODE|TIMEOUT|LABELS' test/CMakeLists.txt` shows the rtlib-py ctest entries with `SKIP_RETURN_CODE 77` + `TIMEOUT` (e.g., `test_fhe_cmplr_py_verify_onnx` LABELS `phase1_onnx_models` TIMEOUT 1800); `ctest -N` lists `test_fhe_cmplr_py_verify_onnx`, `test_openfhe_bootstrap`, `test_ckks2py_allops` (at minimum); a SKIP exit returns 0 in an install-only container (`test_fhe_cmplr_py_verify_onnx.sh` exits 77 cleanly) |
| 1.9 | refactor existing test drivers for routing | refactor | e62a4f57 | 1.2 | 150 | `ctest -R 'test_fhe_cmplr|test_bts_rescale|test_ckks2py_allops'` runs the existing C-path tests green; the default (no `-CKKS2PY`) `.c` emitted by `fhe_cmplr` for a single onnx is byte-identical to the master C output via `diff` (`-CKKS2PY` mapping is additive/optional; default C path unchanged) |
| 1.10 | gitignore + psutil hygiene | fix | e62a4f57 | -- | 140 | `git status` clean after a fhe_cmplr run; psutil shim importable |
| 1.11 | rtlib-py open-issues catalog (generalized limitations; reuses the open-issues material) | doc | e62a4f57 | 1.8 | 102 | `grep -E 'r3|r2\b|phase[0-9]|20[0-9]{2}-[01][0-9]|max_abs' doc/rtlib-py-doc/40-open-issues.md` returns 0 and the doc has no machine-local absolute paths (generalized, no point-in-time run numbers); `wc -l doc/rtlib-py-doc/40-open-issues.md` <= ~102 |

rt= activation lands in 1.2 so its gate is passable without depending on a later
PR (this resolves the dependency ordering the draft got wrong).

### F2 -- rt_openfhe (python)

| Stage | Title | Layer | From | Deps | LOC | Gate |
|---|---|---|---|---|---|---|
| 2.1 | rt_openfhe provider design (lazy level bridge, bootstrap, df) | design-doc | e62a4f57 | 0.1, 1.4 | -- | the F2.1-laid design doc (`doc/rtlib-py-doc/rt-openfhe-design.md`) names the load-bearing elements -- `grep -E 'lazy.{0,3}bridge|_ctx_num_primes|ir_capacity|df|FLEX' doc/rtlib-py-doc/rt-openfhe-design.md` >= 1 each (lazy bridge / `_ctx_num_primes` / `ir_capacity` / `df` / `FLEX`); the doc has no dated residue (no round/phase ISO-date labels, no per-run `max_abs`) and no machine-local absolute paths |
| 2.2 | OpenFHE EvalBootstrap toy POC (gap-filler; mirrors the F4.2 fhert binding POC) | poc | aba1d435 | 2.1 | 80 | poc layer is python: `rtlib-py/rtlib_py/poc_openfhe_bootstrap.py` exercises `EvalBootstrap` + `Rescale` + the nsd-reset (`LevelReduceInPlace` path) against the openfhe-python 1.5.1 binding via `$OPENFHE_PY_BUILD_DIR`; default ring 2^13 (env `POC_RING_DIM`), default scaling FIXEDMANUAL / UNIFORM_TERNARY (env `POC_SCALING`); gate: `OPENFHE_PY_BUILD_DIR=$OPENFHE_PY_BUILD_DIR PYTHONPATH=rtlib-py python3 rtlib-py/rtlib_py/poc_openfhe_bootstrap.py` exits 0, asserts recovered-vs-expected max error < 1e-3, and prints `POC OK max_abs=<v>` |
| 2.3 | rt_openfhe provider core (context, lazy level bridge, arithmetic/IO/encode/query) -- hand-re-authored | impl | e62a4f57 | 1.4 | 850 OVER -> 2 sub-commits | `PYTHONPATH=rtlib-py OPENFHE_PY_BUILD_DIR=/nonexistent python3 -c "import rtlib_py.rt_openfhe; print('import OK no binding')"` exits 0 (lazy `_ensure_of` does not bind at import; 3a context never builds without a real binding either); a toy module driven by `fhe_cmplr -CKKS2PY:rt=openfhe` reproduces `rt_ref` on `add_ciph`/`mul_ciph`/`rescale_ciph`/`rotate_ciph` within 1e-5; the source file has no machine-local absolute paths (no build-host workspace-rooted or tmp-rooted absolute paths; anything machine-local is env-overridable via `OPENFHE_PY_BUILD_DIR` / `FHE_BTS_*` defaults) |
| 2.4 | `_build_bootstrap_context` (incl. the inline `ir_capacity=max(input_level,max_target+bts_depth+1)`/`R=max(RESERVE,2)` tuning from 992cae93) + `bootstrap_ciph` -- hand-re-authored | impl | e62a4f57, 992cae93 | 2.3 | 357 | `OPENFHE_RUN_BOOTSTRAP=1 python3 -m pytest test/rtlib_py/py_verify/test_openfhe_bootstrap.py -q` (<=1e-2 with `scale_deg=1`); `fhe_cmplr -CKKS2PY:df=<csvfile>`-emitted `.py` references `load_data_entry`; `grep -E '_CTX_IR_CAPACITY = max\(.+,\s*.+\+1\)' rtlib-py/rtlib_py/rt_openfhe/__init__.py` shows the inline `ir_capacity=max(input_level,max_target+bts_depth+1)` and `R = max(_BTS_RESERVE, 2)` tuning; `PYTHONPATH=rtlib-py python3 -c "import rtlib_py.rt_openfhe"` exits 0; `git diff --stat 2.3..2.4` shows insertions only |
| 2.5 | rt_openfhe host-only FLEX-EXT Compress-compensation patch (late; opt-in, labeled) -- the `compress_towers = towers_left+1 if _FLEX_EXT else towers_left` branch inside `_drop_to_of_level` + `Cipher.__init__`'s FLEX `scale_deg` clamp (a localized late patch per sec. 2.2(iii)) | impl | 2859787f | 2.4 | 90 | gated by `FHE_BTS_SCALING`; default fixedmanual byte-identical; FLEX engages only under `FHE_BTS_SCALING=flex*`; opt-in / host-only / off-by-default; does NOT claim FIXEDMANUAL drift solved (introduces no `_scaling_technique` -- that landed in 2.3a with the parser) |
| 2.6 | rt_openfhe bootstrap + onnx tests | test | e62a4f57 | 2.4, 1.7 | 340 | `ctest -R test_openfhe_bootstrap` exits 0 (test_openfhe_bootstrap: `OPENFHE_RUN_BOOTSTRAP=1 python3 -m pytest test_openfhe_bootstrap.py -q`); onnx sweep `ctest -R 'test_fhe_cmplr_py_verify_onnx_openfhe'` exits 0 (SKIP (exit 77) where onnx binding absent); no onnx sweep regressions vs master |

### F3 -- OpenFHE C-side bootstrap

Independent of F0 and F2. Splits the 4257-LOC `aba1d435` into its real layers and
isolates the `c851516f` sharding fix from its doc mass.

| Stage | Title | Layer | From | Deps | LOC | Gate |
|---|---|---|---|---|---|---|
| 3.1 | OpenFHE C-bootstrap integration plan (reuses `doc/rtlib-c-bootstrap/openfhe-bootstrap-integration-plan.md`) | design-doc | aba1d435 | -- | 529 | `grep -cE 'level_budget|FIXEDMANUAL|UNIFORM_TERNARY|HYBRID|ir_capacity|R-tower' doc/rtlib-c-bootstrap/openfhe-bootstrap-integration-plan.md` >= 5 (bootstrap context layout, ir_capacity/R-tower, level_budget [4,4], FIXEDMANUAL+UNIFORM_TERNARY+HYBRID all named); `grep -E 'r3|r2\b|phase[0-9]|20[0-9]{2}-[01][0-9]|max_abs' doc/rtlib-c-bootstrap/openfhe-bootstrap-integration-plan.md` returns 0 (no per-run numbers, no dated residue) and the doc has no machine-local absolute paths |
| 3.1b | OpenFHE C-side EvalBootstrap toy POC (gap-filler) -- standalone C++ TU: OpenFHE EvalBootstrap + Rescale + LevelReduceInPlace recovers the message on a toy ring (N=4096) before any header/impl | poc | aba1d435 | 3.1 | 80 | `g++ -std=c++17 <poc.cxx> -I"$OPENFHE_DIR/include" -L"$OPENFHE_DIR/lib" -lOPENFHEpke -lOPENFHEcore -o poc && ./poc` exits 0 and asserts `max_abs < 1e-3` on a toy ring N=4096 (C-side bootstrap mechanism works before headers land); OpenFHE 1.5.1 is installed under `$OPENFHE_DIR` (defaults to `/usr/local` in this environment; headers under `$OPENFHE_DIR/include/openfhe/`, libs under `$OPENFHE_DIR/lib`); cmake find via `OpenFHE_DIR=$OPENFHE_DIR/lib/cmake/OpenFHE` also resolves |
| 3.2 | OpenFHE C bootstrap API header + compiler hook (`rt_openfhe.h` Bootstrap + Get_input_data decls + `Emit_need_bts`) | header | aba1d435 | 3.1, 3.1b | 59 | `grep -E 'Bootstrap\b|Get_input_data' rtlib/include/rt_openfhe/rt_openfhe.h` shows decls parallel to the other backends (`rtlib/include/rt_ace/rt_ace.h`, `rtlib/include/rt_phantom/rt_phantom.h`, `rtlib/include/rt_seal/rt_seal.h`) -- diff is provider-namespace cosmetics only; `grep -E 'Emit_need_bts.*OPENFHE|Emit_need_bts.*openfhe|Emit_bootstrap.*openfhe' include/fhe/ckks/ir2py_handler.h rtlib/include/rt_openfhe/rt_openfhe.h` exits 0 (Emit_need_bts for `PROVIDER::OPENFHE`); `g++ -std=c++17 -fsyntax-only -I include -I "$NN_ADDON_DIR/include" -I "$AIR_INFRA_DIR/include" -I "$BUILD_DIR/include" rtlib/include/rt_openfhe/rt_openfhe.h` exits 0 |
| 3.3 | impl bootstrap context + method | impl | aba1d435 | 3.2 | 260 | `grep -E 'Need_bts\(\)|EvalBootstrap|LevelReduceInPlace' rtlib/openfhe/src/openfhe_lib.cxx` >= 1 each (the impl has the `Need_bts()`-gated OPENFHE_CONTEXT branch; `Bootstrap() = EvalBootstrap -> Rescale-while-nsd>1 -> LevelReduceInPlace`; `Prepare_input` encrypts at `of_level=_bts_ir_capacity-input_level`); AND `ctest -R ut_openfhe_bootstrap` exits 0 (forward-ref to the F3.7 gtest -- its passage is the only observable of this fix) |
| 3.4 | fix backend bugs from wiring (by-value CIPHERTEXT overloads; 4-arg Bootstrap; Openphe_copy deep Clone; conv2d_keep_shape) | fix | aba1d435 | 3.3, 3.2 | 55 | `ctest -R ut_openfhe_bootstrap` exits 0 (the backend-wiring fixes -- by-value CIPHERTEXT overloads; 4-arg Bootstrap; Openphe_copy deep Clone; conv2d_keep_shape -- are observable only through the test binary; no silent pass) |
| 3.5 | impl SIHE scalar + mask ops | impl | aba1d435 | 3.2 | 45 | `grep -E 'Mul_scalar\b|Add_scalar\b|Encode_float_mask\b' rtlib/include/rt_openfhe/rt_openfhe.h` shows the SIHE scalar/mask decls and `diff` against `rtlib/include/rt_seal/rt_seal.h rtlib/include/rt_ace/rt_ace.h rtlib/include/rt_phantom/rt_phantom.h` shows matching signatures (modulo provider-namespace typedefs; e.g. `Mul_scalar(PLAINTEXT/CIPHER res, CIPHER op1, double op2)`, `Encode_float_mask(PLAIN plain, float input, size_t len, ...)`); OR a registered `ctest -R ut_openfhe_sihe_scalar_mask` exits 0 (the SIHE scalar-mask gtest added in F3.5) |
| 3.6 | fix Prepare_input sharding (via Get_encode_scheme) | fix | c851516f | 3.3, 3.4 | 94 | regress18-conv2d PASS via cti local_test `-VEC:ms=4096:sharding`; single-shard cases byte-identical; the `--lib openfhe` local_test harness integration was DEFERRED (`run_test.sh` supports ant|ace only -- harness gap), so the sharding code fix landed without a `--lib openfhe` cti run; the section-2.6 byte-match against the feat baseline (incl resnet18-conv2d sharding) is the standing guard |
| 3.7 | test bootstrap unittest | test | aba1d435 | 3.3, 3.4 | 169 | `ctest -R ut_openfhe_bootstrap` green |
| 3.8 | test bootstrap smoke + plain .inc | test | aba1d435 | 3.3, 3.4 | 195 | `rtlib/openfhe/example/eg_rtopenfhe_boot.cxx` + `eg_rtopenfhe_boot.inc` build and run SUCCESS; a ctest entry that wraps the example `ctest -R openfhe_boot` exits 0 (if registered); the plain `.inc` examples (`eg_rtopenfhe_add.inc`, `eg_rtopenfhe_add_const.inc`, `eg_rtopenfhe_mult_const.inc`, `eg_rtopenfhe_conv2d_keep_shape.inc`) succeed under the `Need_bts()==false` bypass; `grep -E 'Need_bts\(\) \{ return false|Need_bts\(\) == false' rtlib/openfhe/example/eg_rtopenfhe_add.inc` exits 0 (the false-bypass is present) |
| 3.9 | doc bootstrap architecture notes (methodology only) | doc | aba1d435 | 3.3 | 300 | the F3.9 architecture note lands at `doc/rtlib-c-bootstrap/bootstrap-architecture.md` (committed, promoted from the local seed); `grep -E 'r3|r2\b|phase[0-9]|20[0-9]{2}-[01][0-9]|max_abs|seconds|PASS-count' doc/rtlib-c-bootstrap/bootstrap-architecture.md` returns 0 (no dated numbers/seconds/max_abs/paths/pass-counts); `grep -cE '1\.5\.1 API sig|ir_capacity|R-tower|level_budget|input-seam' doc/rtlib-c-bootstrap/bootstrap-architecture.md` >= 5 (1.5.1 API sigs, ir_capacity/R-tower, level_budget, input-seam contract, fix designs all named). NOTE: F3.9 actually landed as an on-top backfill after Part 6 (commit `949ee834`), not in its in-F3-sequence position; see sec. 6 "Actual landing order" |
| 3.10 | doc sharding gap analysis (methodology only; trim below ~600, split `python-sharding-vs-c.md` out if needed) | doc | c851516f | 3.6 | 805 OVER -> 2 sub-commits (trim) | `grep -E '20[0-9]{2}-[01][0-9]|PASS-count|max_abs' doc/rtlib-c-bootstrap/regress58-sharding-gap-analysis.md doc/rtlib-c-bootstrap/python-sharding-vs-c.md` returns 0 (stripped dated run output, pass-counts, max_abs bands); the docs have no tmp-rooted r58 paths and no openfhe-prefixed local roots; `wc -l doc/rtlib-c-bootstrap/regress58-sharding-gap-analysis.md doc/rtlib-c-bootstrap/python-sharding-vs-c.md` each <= ~600 |

Open-issues note (folded into the catalog): the FIXEDMANUAL-scale-drift vs
encode-seam-fix conflict between `doc/ant-vs-openfhe-ckks-scale-drift/`
(untracked) and the C-bootstrap docs is recorded as needing a re-run of
`rt_openfhe-c` to reconcile. F3 ships the encode-seam fix and does NOT claim the
C-side drift is solved.

### F4 -- rt_fhert (host + device)

Depends on F2.4 (rt_openfhe lazy-bridge design, which the port mirrors). Device
mode is included as a feature; resnet20 device is a known not-yet-passing case.

| Stage | Title | Layer | From | Deps | LOC | Gate |
|---|---|---|---|---|---|---|
| 4.1 | rt_fhert port design (3 deltas: cp312 stub-bypass, bare-enum adaptation, SPARSE_TERNARY+cf=13; host-vs-device plan) | design-doc | 24cb1cf7, 41af5282 | 2.1, 3.1 | 180 | `test -s doc/fhert-device-fix/STATUS.md` (the design was written against the committed/local STATUS.md snap); the F4.1-laid design doc `doc/rtlib-py-doc/fhert-backend-design.md` names the deltas -- `grep -cE 'cp312|bare-enum|SPARSE_TERNARY|cf=13' doc/rtlib-py-doc/fhert-backend-design.md` >= 3 (cp312 stub-bypass, bare-enum adaptation, `SPARSE_TERNARY`+cf=13); `grep -E 'r3|r2\b|phase[0-9]|20[0-9]{2}-[01][0-9]|max_abs' doc/rtlib-py-doc/fhert-backend-design.md` returns 0 (no dated residue) |
| 4.2 | fhert binding POC (gap-filler) | poc | 24cb1cf7 | 4.1 | 150 | with `FHERT_ROOT` pointing at the bazel fhert tree root: `python3.12 -c "import fhert.openfhe; print('fhert 1.4.2 binding import OK on cp312')"` exits 0; the POC runs a minimal `EvalBootstrap` and asserts `max_abs < 1e-4` on a toy ring (docs on a stub that does not require a CModel device) |
| 4.3 | rt_fhert contract stub (sigs + docstrings, bodies raise NotImplementedError) | header | 24cb1cf7 | 0.3, 4.1 | 130 | `PYTHONPATH=rtlib-py python3 -c "from rtlib_py.common.contract import CONTRACT_NAMES; import rtlib_py.rt_fhert as r; import rtlib_py.rt_ref as ref; assert [n for n in CONTRACT_NAMES if not hasattr(r,n)]==[]; print('rt_fhert stub matches CONTRACT_NAMES')"` exits 0 (the stub matches rt_ref's 36-name surface with no missing entries); `diff <(python3 -c 'import rtlib_py.rt_ref as r; print(sorted(n for n in r.__all__))') <(python3 -c 'import rtlib_py.rt_fhert as r; print(sorted(n for n in r.__all__))')` shows no missing entries |
| 4.4 | impl rt_fhert skeleton + lazy bridge (hand-re-authored from e62a4f57 base; NOT `git checkout 24cb1cf7`) -- includes the FLEX parser (`_parse_flex_scaling` + `_FLEX_MODE`/`_FLEX_EXT`/`_FLEX_SCALE`, pre-existing; `rt_fhert/__init__.py:1099` inside `_build_bootstrap_context` and `:1348` inside `_build_device_context` activate them) and ships the FULL dispatcher with the device/bootstrap branches as forward-refs per sec. 2.2(i) | impl | e62a4f57, 24cb1cf7 | 4.3, 4.2, 2.3 | 500 | module imports without binding (`OPENFHE_PY_BUILD_DIR=/nonexistent python -c "import rtlib_py.rt_fhert"` exits 0); 3a leveld-SHE FIXEDMANUAL context builds from emitted ctx params; `Cipher.level` is on-demand `@property`; `_parse_flex_scaling` + `_FLEX_MODE`/`_FLEX_EXT`/`_FLEX_SCALE` present and module-level `False` at import (no binding); full dispatcher (device/bootstrap branches as forward-refs) importable; file imports at this stage |
| 4.5 | impl rt_fhert SHE ops + encode/IO | impl | 24cb1cf7 | 4.4 | 450 | `python3 doc/regress58-params/run_cti_params_sweep.py --providers rt_fhert --df-states off,on --csv doc/regress58-params/regress58_catalog.csv` records 0 hard fails (no MISMATCH/NAME_ERROR; the rt_openfhe NOISY subset is the matching rt_fhert NOISY subset, df off and on); `PYTHONPATH=rtlib-py python3 -c "import rtlib_py.rt_fhert"` exits 0 (file imports at this stage); `git diff --stat 4.4..4.5` shows insertions only |
| 4.6 | impl rt_fhert host bootstrap | impl | 24cb1cf7 | 4.5 | 350 | `FHE_RUN_BOOTSTRAP=1 python3 -m pytest test/rtlib_py/py_verify/test_fhert_bootstrap_host.py -q` exits 0 (host bootstrap smoke on ring 16384, `max_abs < 1e-2` by default or as set via `FHE_BTS_TOL`); with `FHERT_ROOT` pointing at the bazel fhert tree root, `python3 test/rtlib_py/py_verify/run_resnet20_vs_onnx.py --backend rt_fhert` exits 0 with `pred == golden_label` (host resnet20 harness green); `PYTHONPATH=rtlib-py python3 -c "import rtlib_py.rt_fhert"` exits 0; `git diff --stat 4.5..4.6` shows insertions only |
| 4.7a | impl rt_fhert device context build + per-bootstrap reclaim core -- `_build_device_context` + `_reclaim_bts_device_scratch` + `_drop_bts_plain_diags`/`_device_bconv_cram_total` + device constants (~290) | impl | 41af5282 | 4.6 | 290 | device context + reclaim core present and importable; A7 (plain-diags drop + its env) and A8 (reclaim diagnostic) deliberately NOT committed -- `git ls-files 'doc/fhert-device-fix/*'` shows no A7/A8/reclaim_diag; diff additive-only vs 4.6 |
| 4.7b | impl rt_fhert inline device seams -- `_drop_to_of_level`/`_encrypt_values`/`_decrypt_slots`/`Cipher.clone`/`add_ciph`/`sub_ciph` (device round-trip `.to(_DEV)`/`.to(None)`) + host-EVK-clear block + device-HBM budget env (`FHE_DEVICE_HBM_GB`, default 16, 64 guard) + half-packing env (opt-in) + input-seam fix (~360) | impl | 41af5282 | 4.7a | 360 | inline device round-trip seams + HBM/half-packing envs + input-seam present; default host path byte-identical to 4.6 baseline; diff additive-only vs 4.7a |
| 4.8 | impl rt_fhert FLEX-EXT Compress-compensation gate in `_drop_to_of_level` (`compress_towers = towers_left+1 if _FLEX_EXT else towers_left`) + the `_parse_flex_scaling` activation under `FHE_BTS_SCALING`/`FHE_DEV_SCALING` (pre-existing in rt_fhert in da3f9b69; rides along; `_scaling_technique` is rt_openfhe-only -- rt_fhert drives the OpenFHE scaling technique by branching on the parsed tag) | impl | da3f9b69 | 4.7b | 190 | `_parse_flex_scaling` present; `FHE_BTS_SCALING=fixedmanual` output byte-identical to the 4.7b baseline via diff; `flexnoext` engages the constant-nsd=2 path; diff insertions only vs 4.7b |
| 4.9 | test host bootstrap + resnet20 harness + device tests | test | 24cb1cf7, 41af5282 | 4.6, 4.7a, 4.7b, 4.8 | 300 | host bootstrap smoke green (or SKIP where binding absent); host resnet20-vs-onnxruntime e2e pred matches label; **device: bootstrap implemented + device smoke PASS + device lenet_x2 e2e PASS + device operator-level regress58 (the rt_openfhe NOISY subset, 0 hard fail); resnet20 device is NOT run this refactor (~10 h, unacceptable) -- recorded as a follow-up in the F4.12 open-issues catalog; a `test_fhert_device_resnet20` ctest is registered with `SKIP_RETURN_CODE 77` gated on `FHE_RUN_RESNET20_DEVICE=1` (default off -> never runs unless manually opted in; not an XFAIL, not a silent exclusion)** |
| 4.10 | document device FIXEDMANUAL harden (conditional _FLEX_MODE gate + Cipher.clone short-circuit + CRAM fail-fast guard; committed in 98fd6e92) | fix/doc | 98fd6e92 | 4.7b, 4.9 | 290 | `git diff 4.7b..4.10 -- rtlib-py/rtlib_py/rt_fhert/__init__.py` shows the `_FLEX_MODE`-conditioned `Cipher.clone` short-circuit line + CRAM fail-fast guard (`violation("hard", "CRAM", ...)`-style assertion present in 98fd6e92); the F4.10 methodology docs -- `grep -E 'r3|r2\b|phase[0-9]|20[0-9]{2}-[01][0-9]|max_abs|seconds' doc/fhert-device-fix/OPEN-ISSUES.md doc/rtlib-py-doc/40-open-issues.md` returns 0 (per-run PASS/NOISY/MISMATCH counts DROP -- methodology only) |
| 4.11 | regress58 sweep runner + catalogs (parameterized to env vars) | test | 24cb1cf7 | 4.9 | 420 | `python3 doc/regress58-params/run_cti_params_sweep.py --csv doc/regress58-params/regress58_catalog.csv --providers rt_fhert --df-states off --filter <one row>` exits 0 (1-row smoke green); the runner and catalogs have no machine-local absolute paths (no build-host workspace-rooted or tmp-rooted absolute paths; anything machine-local is env-overridable via `--onnx-root` / `OPENFHE_PY_BUILD_DIR` / `FHERT_ROOT`, not hardcoded) |
| 4.12 | fhert handoff backend methodology + open-issues (resnet20 device HBM accumulation known limitation; fix B1 future) | doc | 24cb1cf7, 98fd6e92, da3f9b69, 41af5282 | 4.9, 4.10, 4.11 | 450 | the F4.12-laid handoff doc (e.g. `doc/rtlib-py-doc/fhert-handoff.md`) has `grep -E 'r3|r2\b|phase[0-9]|20[0-9]{2}-[01][0-9]|seconds|max_abs|current.status' doc/rtlib-py-doc/fhert-handoff.md` returning 0 (no dated run labels, per-run seconds/max_abs, "current status" lines); `grep -cE 'resnet20 device|HBM|fix B1|per-cycle dev.reset_scratch|A9' doc/rtlib-py-doc/fhert-handoff.md` >= 1 (the methodology names the known limitation + the fix-B1 future + the A9 follow-up); resnet20 device NOT tested this refactor (~10 h, unacceptable; per-cycle HBM accumulation; fix B1 = per-cycle dev.reset_scratch is future work, NOT implemented; A9) -- recorded as follow-up; lenet_x2 + operator level pass |

Uncatalogued-LOC note: `rt_fhert/__init__.py` is ~2573 LOC monolithic; the original F4 stage table attributed only ~250 LOC (F4.7) to the device seams and left ~700 LOC of device-context/build/reclaim/inline-seam machinery uncatalogued. The F4.7a+F4.7b split (290+360 = ~650) attributes this bulk into the additive build sequence across 4.4-4.7b. The guard for that bulk is the per-stage additive diff + the import gate, not a single LOC count.

### F5 -- ckks2py flag-policy end-state + cleanup

Decision: keep the `-CKKS2PY` end state natively. There is NO rename step and NO
rename narrative -- the refactored PR uses `-CKKS2PY` / `fhe::ckks` from the first
commit, as if it were always named that. The introduce-lib / remove-lib /
clean-dead-code chain collapses into one impl reaching the final state. F5 has
no POC stage: flag-policy cleanup introduces no new mechanism, so the design
note (5.1) IS the proposal.

| Stage | Title | Layer | From | Deps | LOC | Gate |
|---|---|---|---|---|---|---|
| 5.1 | ckks2py flag-policy end-state design note (rt= sole activator; lib= removed; fp gates mfree; provider tag from rt; poly->ckks location rationale) -- reuses the flag-policy design material WITHOUT the rename narrative | design-doc | 5d3f0896, d1848ec3, fddd8e7b, 1271443d | 0.2 | 90 | `grep -rnE -- '-P2P' . --include='*.md' --include='*.h' --include='*.cxx' --include='*.cc'` returns 0 (no `-P2P` strings in the kept tree and no rename narrative); the F5.1 design note `doc/ckks2py-flag-policy.md` -- `grep -E 'rt=.*sole activator|lib=.*removed|fp.*mfree|poly.*->.*ckks|poly->ckks' doc/ckks2py-flag-policy.md` >= 1 each (rt= sole activator, lib= removed, fp gates mfree, poly->ckks location rationale); END state approved up front |
| 5.2 | implement flag-policy end-state (the G4 chain d1848ec3 + fddd8e7b + 1271443d collapsed to one impl; rt= activation already in F1.2; here collapses lib= removal + fp-gates-mfree + provider tag; dead `_prov_str` plumbing never lands) | impl | d1848ec3, fddd8e7b, 1271443d | 5.1, 1.2 | 80 | `-CKKS2PY:rt=` alone activates; `-CKKS2PY:lib=` rejected; `-CKKS2PY:fp` inserts free_ciph, without it does not; default byte-identical to C path |
| 5.3 | fix run_py_exec SKIP gate at final ckks/ paths (the ir2py_ctx.h style reflow is folded into F0.2 via 3fabb255) | fix | 3fabb255, a1f2746e | 0.2 | 30 | CI `check_coding_style` green on `ir2py_ctx.h` (style stays correct from F0.2); ctest unit gate SKIP returns 0 in install-only container (`SKIP_RETURN_CODE 77`) |
| 5.4 | purge result dumps + CJK/style/English on kept methodology (one pass; also cleans the r2 runner schema + run_cti_params_sweep) | fix | 163a1c50, 90b8c90e, 6e5cae6c | -- | 70 | `git ls-files` shows no result CSVs/JSON in kept dirs; `grep -r 'runner-schema' doc/local-test-rtpy/00-methodology.md doc/rtlib-py-doc/30-test-status.md` returns 0 (the F5.4 pass rewrites those kept-methodology dangling refs to the OUT'd r2 runner-schema doc or inlines the r2 vocab); `grep -rlP '[\x{4e00}-\x{9fff}]' doc/ test/ include/ rtlib*/` (CJK chars) on kept files returns 0; CI `check_coding_style` green; `py_compile` on kept `.py` files OK |
| 5.5 | sync rtlib-py-doc + handoff docs to flag-policy end-state (no machine-point-in-time data) | doc | b452d0a9, d1848ec3, fddd8e7b, 5d3f0896 | 5.2, 5.4 | 120 | per kept doc `doc/rtlib-py-doc/{00-README,10,20,40}.md` -- `grep -E 'rt=.*sole activator|rt=.*activator' doc/rtlib-py-doc/{00-README,10,20,40}.md` returns >= 1 hit per file (prose references rt= sole activator); `grep -E 'lib=.*removed' doc/rtlib-py-doc/{00-README,10,20,40}.md` returns >= 1 per file (lib= removed); `grep -E 'seconds|PASS-counts|max_abs=' doc/rtlib-py-doc/{00-README,10,20,40}.md` returns 0 (no machine-point-in-time data -- no seconds, PASS-counts, or max_abs bands) |

### F6 -- Methodology docs

| Stage | Title | Layer | From | Deps | LOC | Gate |
|---|---|---|---|---|---|---|
| 6.1 | local_test integration methodology + BUILD fix (trimmed) | doc | cba6b674 | 0.1 | 300 | `cmake -B build` configure + `detect_env --check` pass on a fresh checkout; `grep -E 'PASS|seconds|20[12][0-9]-[01][0-9]-[0-3][0-9]' doc/local_test_integration.md doc/BUILD.md doc/README.md` returns no dated markers; `doc/BUILD.md` mandated to: Release + code-check-OFF / `-j$(nproc)`; `grep 'runner-schema' doc/local_test_integration.md doc/BUILD.md doc/README.md` returns 0 (the F6.1-landed docs inline any r2 vocab they reference, no dangling ref to the OUT'd r2 runner-schema doc) |
| 6.2 | local-test-rtpy Python-provider methodology (trimmed) | doc | cba6b674 | 6.1 | 95 | the F6.2-landed methodology (`doc/local-test-rtpy/00-methodology.md`) strips machine-local overrides (a cache-home tmp-rooted path, the host python interpreter path, cp312 onnxsim); `grep -E 'XDG_CACHE_HOME|onnxsim' doc/local-test-rtpy/00-methodology.md` returns 0 (the stripped tokens are gone); `grep 'runner-schema' doc/local-test-rtpy/00-methodology.md` returns 0 (the r2 runner-schema vocab is inlined, no dangling ref to the OUT'd doc); `grep -E 'MaxPool.*AvgPool|CKKS-golden|df-state' doc/local-test-rtpy/00-methodology.md` >= 1 (semantics cross-checked); no dated residue |
| OUT | point-in-time corpus result snapshots | result-data | cba6b674 | -- | 540 | NOT committed; `git rm --cached` + .gitignore a local results dir; see section 5 |

### OVER stages -- concrete sub-splits

Every stage flagged OVER in the tables above reduces to sub-commits of at most ~600 LOC; none must stay OVER. Sizes are approximate (consistent with each stage's plan LOC, verified against the committed files). For the monolithic `__init__.py` stages the sub-commits are additive in build order (sec. 2.2); for separate-file stages (F0.2, F1.6, F3.10) `git checkout <hash> -- <paths>` applies per file.

| OVER stage | plan LOC | sub-commits (each <= ~600) | note |
|---|---|---|---|
| F0.2 | 2090 | **0.2a** scaffolding `ckks2py_pass.h`+`ckks2py_config.h`+`ckks2py_driver.h` (282); **0.2b** `ir2py_ctx.h` (550); **0.2c** `ir2py_core.h` (576); **0.2d1** `ir2py_handler.h` -- class skeleton + ALL arithmetic/IO/query dispatch: `Handle_add`/`Handle_sub`/`Handle_neg`/`Handle_rotate`/`Handle_relin`/`Handle_rescale`/`Handle_modswitch`/`Handle_upscale`/`Handle_raise_mod`/`Handle_conjugate`/`Handle_mul_mono`/`Handle_free`/`Handle_level`/`Handle_scale` + `Emit_una_op`+`Emit_bin_op` (~340); **0.2d2** `ir2py_handler.h` -- `Handle_encode` + `Handle_bootstrap` (+bootstrap sub-handles) + `Emit_bootstrap_op` + the `Emit_encode` offline-encode logic (~349) | `ir2py_ctx.h`/`ir2py_core.h` are under 600 (cohesive 1:1 mirrors of IR2C_*; one commit each); `ir2py_handler.h` (689) splits cleanly by opcode group into 0.2d1/0.2d2 (both <=600); no OVER exception needed |
| F1.4 | 987 | **1.4a** context + `Cipher`/`Plain` + arithmetic ops (add/sub/mul/mono/neg/rotate/conjugate/relin/rescale/mod_switch/upscale/raise_mod/bootstrap_ciph/copy/zero) (~543); **1.4b** encode + data-file + IO + free (get/set input/output, free_*, flatten/encode/_read_data_file/load_data_entry) (~228); **1.4c** query + validate + timing/dump (level, sc_degree, _slots_of, validate, tm_*, dump_cipher_msg) (~106) | additive to `rt_ref/__init__.py`; all under 600 (the re-authored rt_ref moves `get_input_data`/`set_output_data` to the post-arithmetic region for additivity -- mirrors the rt_openfhe/rt_fhert layout) |
| F1.6 | 875 | **1.6a** `run_py_verify.py` (431, the executor/driver); **1.6c** `gen_workload_onnx.py` (124, ONNX workload generator) then **1.6b** `workloads.py` (385, workload definitions) -- landing 1.6c before 1.6b avoids a ModuleNotFoundError (`workloads` imports `gen_workload_onnx`) | split by file (`git checkout` valid); each under 600 -- the 875 "OVER" was only the bundle |
| F2.3 | 850 | **2.3a** context + lazy level bridge + `Cipher`/`Plain` + shared helpers + FLEX parser (`_parse_flex_scaling`/`_scaling_technique`/`_FLEX_MODE`/`_FLEX_EXT`/`_FLEX_SCALE`, pre-existing) (~430); **2.3b** arithmetic ops (incl. `raise_mod`) + encode + IO + df offline-encode (`_seed_data_info`/`_read_data_file`/`load_data_entry`) + query/validate/timing (~420) | additive to `rt_openfhe/__init__.py` (hand-re-authored) |
| F3.10 | 805 | **3.10a** `regress58-sharding-gap-analysis.md` trimmed of tmp-rooted r58 paths, openfhe-prefixed local roots + captured run logs (~280); **3.10b** `python-sharding-vs-c.md` trimmed (~280) | the 805 "OVER" was the two-doc bundle + path-laden bulk; split by doc + trim -> each under 600 |

F2.4 collapsed to a single ~357-LOC stage (bootstrap context + `bootstrap_ciph`; `raise_mod` moved to 2.3b) -- no longer OVER.

No OVER stage remains unsplittable. The structurally largest single subsystem is `ir2py_handler.h` (689); sec. 2.5 prescribes the opcode-group sub-split for cohesive 1:1 mirrors, and the handler's per-opcode `Handle_*` methods admit incremental addition, so it splits cleanly.

## 4. Original-commit -> new-PR mapping

| Hash | Original LOC | Decomposes into |
|---|---|---|
| e62a4f57 | 12394 | F0.1 (shared design), F0.2 (emitter headers), F0.3 (contract), F1.1 (POC), F1.2 (pass impl), F1.3 (driver), F1.4 (rt_ref), F1.5-1.8 (tests + CMake), F1.9 (test-driver refactor), F1.10 (gitignore/psutil), F1.11 (open-issues doc), F2.1 (openfhe design), F2.3 (rt_openfhe core), F2.4 (rt_openfhe bts+df; absorbs 992cae93), F2.6 (openfhe tests), F4.4 (rt_fhert skeleton, hand-re-authored from this base), DROP (result tallies + repro scripts) |
| aba1d435 | 4257 | F3.1 (integration plan), F3.1b (C-side POC, gap-filler), F2.2 (python POC, gap-filler), F3.2 (API header), F3.3 (bootstrap impl), F3.4 (backend bug fixes), F3.5 (SIHE scalar/mask), F3.7 (gtest), F3.8 (smoke example), F3.9 (architecture notes), DROP (RESULT.md + the stage-N journals) |
| 992cae93 | 295 | absorbed into F2.4 (python level-reduction lives in the rt_openfhe bootstrap+df stage) + DROP (its RESULT.md with per-run seconds, the 2^18->2^17 ring, the D 61->41 chain tightening) |
| c851516f | 936 | F3.6 (Prepare_input sharding fix), F3.10 (sharding gap-analysis docs), DROP (RESULT.md amendment + a pass-count amendment) |
| 24cb1cf7 | 3143 | F4.1 (fhert port design), F4.2 (binding POC, gap-filler), F4.3 (contract stub, gap-filler), F4.5 (SHE ops+encode/IO), F4.6 (host bootstrap), F4.9 (host tests+harness), F4.11 (runner+catalogs), F4.12 (handoff methodology), DROP (e2e-py-results.r3.*.csv, fhe-cgo-py-accuracy-results.*.json, r3-results.md) |
| 98fd6e92 | 895 | F4.10 (device FIXEDMANUAL harden, 290 LOC), DROP (RESULTS.md/OPEN-ISSUES.md timings + max_abs + dated refinement). The 895-LOC commit is mostly result-data churn around a ~290-LOC real fix |
| da3f9b69 | 329 | F4.8 (FLEXIBLEAUTO no-EXT scaling, 190 LOC), DROP (RESULTS.md sec. 9 timings + OPEN-ISSUES dated refinement) |
| 5d3f0896 | 185 | F5.1 (relocation rationale folded into the flag-policy design note), F0.2 (headers land at ckks/ directly -- the mechanical move is a non-event in a clean rewrite). The 185-LOC relocation mostly evaporates; what survives is the rationale |
| d1848ec3 | 122 | F1.2 (rt= activation) + F5.2 (folded into flag-policy end-state impl; the "lib= optional/legacy" intermediate never lands) |
| 2375e8d1 | 198 | no rename step in the refactor (the refactored PR is `-CKKS2PY` natively from the first commit). Only the 9 doc-file touch-ups split out of the original 38-file flag rename survive, folded into F5.5 doc-sync reflecting the end state |
| fddd8e7b | 79 | F5.2 (folded into flag-policy end-state; "remove lib=" is the final state, not a separate step) |
| 1271443d | 4 | never lands (dead `_prov_str` plumbing; the end-state impl in F5.2 never introduces it) |
| b452d0a9 | 9 | F5.5 (folded into the consolidated doc-sync commit; exists only because fddd8e7b left stale lib= references) |
| 3fabb255 | 11 | style reflow FOLDED into F0.2 (`ir2py_ctx.h` lands with the prfix style already correct); F5.3 then handles only the test-side `run_py_exec` SKIP gate |
| a1f2746e | 19 | F5.3 (`run_py_exec` SKIP gate applied at the final `ckks/test/test_ckks2py_allops.cxx` path) |
| 6e5cae6c | 1 | F5.4 (combined with 90b8c90e + 163a1c50 into one CJK/style/English fix on kept methodology; result dumps removed) |
| 90b8c90e | 9 | F5.4 (combined as above) |
| 163a1c50 | 165 | F5.4 (CJK->English applied to kept methodology files; the result-dump CSVs the CJK sat inside are OUT) |
| e3f6d7af | 526 | F0.1 (trimmed architecture/code-review map, ~250 LOC, landed distributed), OUT (~290 LOC dated phase chronology + ~72-row source-coverage archive inventory + `doc/rtlib-py-doc/60-development-history-and-review-guide.md`, design narrative) |
| cba6b674 | 967 | F6.1 (local_test integration methodology + BUILD fix, 300 LOC trimmed), F6.2 (local-test-rtpy methodology, 95 LOC trimmed), OUT (per-phase PASS/NOISY snapshots + result-matrix README + result CSVs) |
| 41af5282 | +7687/-16 | F4.7 (curated rt_fhert device seams A1-A6, committed; WIP snapshot) + a `doc/fhert-device-fix/` snapshot OUT at refactor time (stays local). A7-A8 dropped from the curated sub-patch; A9 future work, see F4.12 |
| 2859787f | +249/-15 | F2.5 (rt_openfhe host-only FLEX re-anchor shim, committed; opt-in, labeled) |

Note: the 21 hashes above cover the commit range `e62a4f57^..HEAD`; the two later
rows (`41af5282`, `2859787f`) are post-PR commits that landed during refactor
execution and refine F4 / F2 per this plan.

## 5. Result-data triage (OUT / TRIM / KEEP -- the governing standard)

This is the single authoritative triage; per-PR doc stages apply it inline. It
adopts the prfix cleanup standard directly (the prfix cleanup worktree, branch
`fix/rt-openfhe-level-reduce`, HEAD `8e1011cc`, net -3592 LOC over 27 files),
which is more dev-facing than a plain remove: OUT moves a file to a gitignored
local `doc/_archive/<topic>/` + `local-runs/` (reference kept locally, not in
git); TRIM keeps the file but cuts the point-in-time data; KEEP leaves it. Each
kept doc is landed clean ONCE (no "introduce then CJK-fix" two-pass).

### OUT (move to gitignored local `doc/_archive/` + `local-runs/`; `git rm --cached` + .gitignore)

CSV/JSON run dumps and result READMEs:
- `doc/regress58-params/e2e-py-results.r3.phase1.csv`,
  `e2e-py-results.r3.phase2.csv`, `e2e-py-results.r3postfix.phase2.csv`,
  `e2e-py-results.r3postfix.regress.csv`,
  `fhe-cgo-py-accuracy-results.rt_openfhe.json`,
  `fhe-cgo-py-accuracy-results.rt_ref.json`, `r3-results.md`
- `doc/rtlib-c-bootstrap/openfhe-bootstrap-integration-RESULT.md` (acc=1.0/pred=3, max_abs 0.064, machine-local paths, pass-counts)
- the stage-N journals under `doc/rtlib-c-bootstrap/` (build, compiler-hook, impl, bootstrap-test, harness, regress58, fixspec, resnet20; process journals + fixspec; `openfhe-bootstrap-integration-plan.md` stays KEEP)
- `doc/rt_openfhe-bootstrap-level-reduction/RESULT.md`: see TRIM (prfix trims the point-in-time tables to ~31 LOC dev-facing; else OUT)
- `doc/fhert-integration-handoff.md` (superseded; methodology survives in trimmed F4.12 form)
- `doc/fhert-device-fix/{RESULTS.md, STATUS.md, work-summary.md, device-dispatch-deadlock-and-hbm-fix.md, fhert-reply-followup.md, resnet20-device-hbm-wall.md, dev-hbm-diag.py, repro-bootstrap-correctness.py, repro-launch-kernel-stall.py, repro-vrf-alloc.py, rt-ref-cutoff-compare.py, run-lenet-x2-device.py, run-lenet-x2-host.py, run-resnet20-relu-device.py, run-resnet20-relu-host.py, .gitignore}` (process journals + one-off repro scripts + device-state snapshot; stay local; only `OPEN-ISSUES.md` stays trimmed -- see TRIM)
- `doc/local-test-rtpy/README.md` (per-phase PASS/NOISY/MISMATCH result matrix, per-cell numbers), and per-phase snapshots `10-phase1-rtref.md`, `20-phase2-realckks-operators.md`, `30-phase3-realckks-models.md`, `40-phase4a-device-operators.md`, `50-phase4b-device-models.md` (`00-methodology.md` stays, see TRIM)
- `doc/rtlib-py-doc/30-test-status.md` and `50-resnet-benchmark-case-study.md`: see TRIM (prfix trims to ~106 / ~59 LOC; else OUT)
- the r2 runner-schema doc under `doc/regress58-params/` (the round-2 runner self-doc; the runner self-documents)
- `doc/rtlib-py-doc/60-development-history-and-review-guide.md` (design narrative; prfix OUT'd it)

One-off repro/adjudication scripts (OUT, not maintained runners):
- `test/rtlib_py/py_verify/adjudicate_openfhe.py`, `attrib_C.py`, `reverdict_D.py`, `gen_map_report.py`, `sweep_onnx_models.py`, and the stale `test/rtlib_py/py_verify/run_cti_params_sweep.py` (all one-off adjudication/sweep scripts tied to past archived result sets; distinct from `doc/regress58-params/run_cti_params_sweep.py` which is KEEP -- parameterized)

### TRIM (keep the file; strip point-in-time data in place)

- `doc/rtlib-py-doc/30-test-status.md` -> keep ~106 LOC (generalized status, no per-run) -- if it cannot stay general, OUT instead
- `doc/rtlib-py-doc/50-resnet-benchmark-case-study.md` -> keep ~59 LOC (methodology only, no one-run numbers) -- else OUT
- `doc/rt_openfhe-bootstrap-level-reduction/RESULT.md` -> if kept at all, ~31 LOC dev-facing only -- else OUT
- `doc/local_test_integration.md` -- strip "Status verified on <date> against the cmake build dir/driver ...", the results.tsv timestamp row, the tmp-rooted provenance sequence
- `doc/local-test-rtpy/00-methodology.md` -- strip the cache-home tmp-rooted override, the host python interpreter path, cp312 onnxsim notes
- `doc/fhert-device-fix/OPEN-ISSUES.md` -- strip dated refinement + per-run seconds/max_abs (qualitative "trim" risks residue; strip literally)
- (see OUT) `test/rtlib_py/py_verify/sweep_onnx_models.py` -- one-off sweep, OUT
- `test/fhe-cgo-py-accuracy.py` / `run-py-accuracy.py` -- TRIM (machine-local paths -> env-overridable) if kept as runners; else OUT
- `rtlib-py/rtlib_py/rt_fhert/__init__.py` docstring -- TRIM (drop cp313/3.12 narrative; keep 16384/cram/precision)
- `include/fhe/ckks/ir2py_ctx.h` / `ir2py_handler.h` figure comments -- TRIM

### KEEP (methodology / runners / catalogs / architecture; trimmed)

- `doc/regress58-params/run_cti_params_sweep.py` (parameterized to env vars), `regress58_catalog.csv`, `phase2_catalog.csv`
- `doc/rtlib-py-doc/{00-README,10,20,40}.md` (trimmed)
- `doc/rtlib-c-bootstrap/openfhe-bootstrap-integration-plan.md` (F3.1 design, kept), `regress58-sharding-gap-analysis.md` + `python-sharding-vs-c.md` (F3.10, trim below ~600), `doc/rtlib-c-bootstrap/bootstrap-architecture.md` (F3.9, committed as an on-top backfill after Part 6, commit `949ee834`, promoted from the local seed)
- `doc/fhert-device-fix/OPEN-ISSUES.md` (issue catalog, trim dated; `STATUS.md` is OUT -- stays local)
- `doc/local_test_integration.md`, `doc/BUILD.md`, `doc/README.md` (F6.1, trimmed), `doc/local-test-rtpy/00-methodology.md` (F6.2, trimmed)
- new design notes: `doc/ckks2py-flag-policy.md` (F5.1 -- WITHOUT rename narrative), `doc/rtlib-py-doc/fhert-backend-design.md` (F4.1), `doc/rtlib-py-doc/rt-openfhe-design.md` (F2.1)

`.gitignore` gains `doc/_archive/` and `local-runs/` (OUT products move there,
local reference kept, not plain rm).

## 6. Git mechanics to land it

Decisions locked (this supersedes the earlier Path A/B hedging): the rebuild is
**by function, now**. Device mode is included as a known-limited feature, so the
rebuild does not wait for the fhert device path to fully settle; the two
open-issue areas (openfhe C-side drift, fhert resnet20 device) are recorded as
follow-up rather than blocking.

Recommendation: **start clean on a new branch off master**
(`refactor/python-rtlib`). Do not interactive-rebase the existing branch:
layers are interleaved within single commit hunks (a single hunk in `e62a4f57`
mixes design-doc + header + impl + test + result-dump), so `git rebase -i`
cannot separate what was never separated. A clean rebuild via surgical
`git checkout <hash> -- <paths>` plus hand-re-authoring the monolithic blobs is
faster and produces the reviewable history the reviewer asks for.

Concrete sequence:

1. `git checkout master && git checkout -b refactor/python-rtlib`.
2. Before the first impl PR: dispose of uncommitted/untracked head state per
   section 5 -- OUT result dumps to gitignored `doc/_archive/`; delete one-off
   build artifacts (`a.out`, `*.onnx.c`) and add them to `.gitignore`; the
   `doc/fhert-device-fix/` snapshot stays LOCAL; `doc/ant-vs-openfhe-ckks-scale-drift/`
   stays LOCAL (machine-point-in-time; its conclusions surface via the
   open-issues catalogs); `doc/relu7-feasibility/`, `doc/test-tidyup/`,
   `doc/local-test-rtpy/60-harness-integration.md` defer to a later independent
   PR. The refactor work-plan itself IS committed per the reviewer request at
   `doc/pr676-refactor/REFACTOR-PLAN.md`; the per-PR SEED drafts under
   `doc/pr676-refactor/pr2-rtlib-py/` and `doc/pr676-refactor/pr3-openfhe-c-bootstrap/`
   stay LOCAL/untracked (their promoted, committed copies live at
   `doc/rtlib-py-doc/` and `doc/rtlib-c-bootstrap/`).
3. For each stage, materialize file content from the source commit, apply
   trimming/CJK/style/path-parameterization, and commit with a focused message:
   - **Separate files** (headers, .cxx bodies, tests, docs): `git checkout
     <source-hash> -- <path1> <path2> ...` at the FINAL path (write directly to
     `include/fhe/ckks/`, never to `include/fhe/poly/`); edit; `git add` the
     cohesive layer; commit.
   - **Monolithic `__init__.py`** (rt_openfhe, rt_fhert): hand-re-author each
     stage as a complete, importable file; stage-by-stage additive-only diffs
     per section 2.2. Do NOT `git checkout + add -p` these.
4. Squash decisions:
   - The G4 flag-policy chain `d1848ec3 + fddd8e7b + 1271443d` collapses into one
     F5.2 impl at the final state; the dead `_prov_str` plumbing never lands.
   - The G4 doc drift `d1848ec3 + b452d0a9 + 2375e8d1` doc touches collapse into
     one F5.5 doc-sync reflecting the end state.
   - The G4 style/CJK trio `6e5cae6c + 90b8c90e + 163a1c50` collapses into one
     F5.4 fix at final paths, with result dumps OUT rather than polished.
   - Treat the prfix cleanup `8e1011cc` as the REFERENCE (its 14 file-deletes
     conflict against a tidy branch that never added those OUT files, so do
     NOT `git cherry-pick` it); apply its specific body tweaks (ir2py_ctx/
     handler comment reflow, `sweep_onnx_models` comment, `fhe_cgo` path
     constants -> env) at F5.4 alongside the sec. 5 TRIMs.
5. Conflict hotspots (all are sequential additive edits to the same file --
   apply in order, not true conflicts):
   - `rtlib-py/rtlib_py/rt_fhert/__init__.py` -- build up 4.4 skeleton (with
     FLEX parser + full dispatcher) -> 4.5 SHE -> 4.6 host bootstrap -> 4.7a
     device context/reclaim -> 4.7b inline device seams -> 4.8 FLEX-EXT
     Compress patch, each additive. ~700 LOC of rt_fhert (~2573 total)
     uncatalogued by the original stage table are now attributed across
     4.4-4.7b.
   - `rtlib-py/rtlib_py/rt_openfhe/__init__.py` -- build 2.3 core -> 2.4
     bootstrap+df (absorbing 992cae93) -> 2.5 flexext shim, each additive.
   - `include/fhe/ckks/ir2py_ctx.h` -- land ONCE at the final ckks/ path in F0.2
     with style correct; F5.3 then only handles the test-side SKIP gate.
   - `doc/rtlib-py-doc/{10,20,40}.md` -- land ONCE in F5.5 reflecting the end
     state.
6. Per-PR verification gates (run before advancing):
   - F0: headers compile as TU; `python -c 'import rtlib_py.common'` passes;
     stub provider validates.
   - F1: `ctest -R test_ckks2py_allops` green; `ctest -R test_fhe_cmplr_py_verify_onnx`
     green (rt_ref); default (no `-CKKS2PY`) C output byte-identical to master.
   - F2: `ctest -R test_openfhe_bootstrap` green; FLEX shim default byte-identical
     to fixedmanual baseline; shim labeled opt-in.
   - F3: `ctest -R ut_openfhe_bootstrap` green; eg_rtopenfhe_boot SUCCESS; the
     plain .inc examples SUCCESS; regress18-conv2d sharding PASS (the `--lib openfhe`
     cti harness integration deferred -- harness gap; the sec. 2.6 byte-match is
     the guard); no claim that C-side drift is solved.
   - F4: host bootstrap smoke green; host resnet20 pred matches label; device bootstrap implemented + device smoke + device lenet_x2 e2e PASS; device operator-level regress58: the rt_openfhe NOISY subset, 0 hard fail; resnet20 device NOT run this refactor (~10 h; recorded as follow-up; the `test_fhert_device_resnet20` ctest defaults to SKIP via `SKIP_RETURN_CODE 77` / `FHE_RUN_RESNET20_DEVICE=1` opt-in); no-EXT host resnet20 PASS.
   - F5: `-CKKS2PY:rt=` activates; `-CKKS2PY:lib=` rejected; `-P2P` strings
     absent; CI `check_coding_style` green; no CJK in kept files.
   - F6: `detect_env --check` passes on a fresh checkout; no dated
     PASS/seconds markers in committed docs.
7. Backup-diff verification: after each separate-file commit and after each monolithic-file stage series + FLEX/device fill (per sec. 2.6), a verification subagent runs the diff-against-backup checks against the pre-refactor backup snapshot (the feat tree at HEAD `2859787f`). Any mismatch beyond the sanctioned deltas blocks the commit / the PR.
8. Result-data removal confirmation: after each PR, the kept tree has zero
   references to the result-dump families -- `grep -rE
   'e2e-py-results|fhe-cgo-py-accuracy|r3-results|the cmake build dir|the local_test temp dir|stage-N|work-summary|repro-bootstrap|dev-hbm-diag|the r2 runner schema'
   doc/ test/` returns zero hits (these descriptive forms stand for the
   forbidden result-dump token strings; the kept tree references neither form);
   OUT dirs are gitignored. (the r2 runner schema is OUT -- any dangling refs in
   kept methodology docs like `doc/local-test-rtpy/00-methodology.md` and the
   F6.2 doc must be rewritten to inline the r2 vocab, else the grep catches them.)
9. Per-PR marker + backup (the close of every functional PR): author and commit
   the in-tree PR-message marker `doc/pr676-summaries/NN-<pr>.md` (scope,
   stages, gate results, the section-2.6 backup-diff status, honest follow-up;
   no machine-point-in-time data), then take the per-PR backup: a v2 worktree
   snapshot (excluding build) via `git -C <v2-worktree> archive --format=tar HEAD`,
   a full-history git bundle via `git -C <v2-worktree> bundle create <prNN bundle
   path> refs/heads/<branch>` (no prerequisites; records the tip), and update the
   LATEST pointer file. The six markers land at `doc/pr676-summaries/01-*.md`
   through `06-*.md`.

### Actual landing order (what happened)

The forward-build mechanics above (clean branch off master, surgical checkout
+ hand re-authoring, additive monolithic stages) were the intended methodology.
The actual landing was three phases:

(a) Forward build to completion. The branch was `refactor/python-rtlib` (off
master `30f387ff`). The forward build landed 73 commits carrying 6 Part markers
(Part 1 = F0+F1, then Parts 2-6). F2 skipped its design-first gates: it jumped
from the Part 1 marker straight to F2.3a-1 (no F2.1 design, no F2.2 POC), then
continued through F2.3-F2.6, F3, F4, F5, F6 to the Part 6 marker. At this point
the tip was the original Part 6 marker `7b3a5a9a` (73 commits off master).

(b) F2 design-first backfill via a content-preserving rebase. To insert the
F2.1 (design) and F2.2 (POC) gates that F2 had skipped, the branch was reset to
the Part 1 marker: `git reset --hard a6f1613d`. F2.1 (design) then F2.2 (POC)
were authored and committed. The 51 downstream commits were then replayed on
top: `git cherry-pick a6f1613d..7b3a5a9a` (where `7b3a5a9a` was the original
Part 6 marker tip). The rebase re-parented every Part marker commit (tree
content preserved; F2.1/F2.2 are the only net-new commits): Part 2 `cafff57a`
-> `777d3e43`; Part 3 `221673de` -> `2935c307`; Part 4 `39355efb` -> `3b335008`;
Part 5 `49312ec8` -> `6d5acc97`; Part 6 `7b3a5a9a` -> `ea437fd6`; Part 1
(`a6f1613d`) unchanged. After this rebase the tip was `ea437fd6` (75 commits
off master: 22 for Part 1 through the Part 1 marker, plus F2.1 + F2.2, plus the
51 cherry-picked downstream commits).

(c) F3.9 on-top backfill. The F3.9 architecture note was committed ON TOP of the
Part 6 marker (commit `949ee834`), as an on-top backfill, rather than in its
in-F3-sequence position (the F3 stage table implies F3.9 sits inside Part 3).
PR-F (`ea437fd6`) is now second-to-last; F3.9 (`949ee834`) is the tip. Final
state before this plan commit: 76 commits on `refactor/python-rtlib` at
`949ee834`.

This plan commit (the doc you are reading) then lands on top of `949ee834` as
the new tip.

## 7. How this plan was produced

Drafted from the actual diffs of the commit range, the prfix cleanup worktree
(branch `fix/rt-openfhe-level-reduce`, HEAD `8e1011cc`), and the committed state
of `41af5282` / `2859787f`; then adversarially re-checked for file coverage,
dependency closure, LOC granularity, rename-narrative residue,
machine-point-in-time residue, device/C-drift honesty, and
incremental-construction gates, with fixes applied inline (no corrections
appendix). The actual-landing-order subsection (section 6) records the
three-phase execution (forward build, F2 design-first rebase backfill, F3.9
on-top backfill) against which the plan was reconciled.
