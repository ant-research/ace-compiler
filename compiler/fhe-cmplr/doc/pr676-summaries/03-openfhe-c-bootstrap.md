# PR #676 - Part 3: OpenFHE C-side bootstrap

This is the third of five functional parts that compose PR #676
(`feat/openfhe-c-bootstrap`), landed as reviewable commits on the
`feat/openfhe-c-bootstrap-v2` branch (off `master`, base commit `30f387ff`).
Because the review system accepts only one overall PR, each functional part ends
with a marker commit that carries a summary like this one - the per-part
description that would otherwise live in a separate PR message.

## Scope

- **F3** - the OpenFHE **C-side** bootstrap: the `EvalBootstrap` runtime + the
  one compiler-side hook it depends on, for the `rtlib/openfhe` backend (the
  C-path that `fhe_cmplr -P2C:lib=openfhe` generates against). Independent of the
  Python emitter headers (F0/F2); it touches the C-path handler
  (`include/fhe/poly/ir2c_ctx.h`), not the python emitter.

No other backend ships here. `rt_ref` (Part 1) and `rt_openfhe` (Part 2) already
landed; `rt_fhert` (host+device) is Part 4; the ckks2py flag-policy end-state and
methodology docs are Part 5.

## What it adds

- **F3.1 - integration plan** (`doc/rtlib-c-bootstrap/openfhe-bootstrap-integration-plan.md`)
  brought from the feat backup with the section-5 trim applied: absolute
  build/source/cti paths parameterized, the per-run recovery-error vocabulary
  generalized, dated run labels removed. The per-stage process journals and the
  RESULT dump stay OUT (section 5).
- **F3.1b - EvalBootstrap POC** (`rtlib/openfhe/poc/eval_bootstrap_poc.cxx`, a
  gap-filler that never existed in git) - a standalone C++ TU against OpenFHE
  1.5.1 proving the bootstrap mechanism before any header/impl: a toy CKKS
  context (ring N=4096, FIXEDMANUAL, UNIFORM_TERNARY secret, HYBRID keyswitch,
  level_budget [4,4]) encodes a message, runs `EvalBootstrap` + Rescale-while-
  nsd>1 + `LevelReduceInPlace`, and recovers within max_abs < 1e-3.
- **F3.2 - API header + compiler hook** - `rt_openfhe.h` (the `Bootstrap`
  wrapper, the by-value `CIPHERTEXT` `Add_ciph`/`Rotate_ciph`/`Rescale_ciph`
  overloads, the `Mul_scalar`/`Add_scalar`/`Encode_float_mask` inline wrappers),
  `openfhe_api.h` (the matching `Openfhe_*` declarations + `Need_bts()`), and
  `include/fhe/poly/ir2c_ctx.h` `Emit_need_bts()` (now emits `bool Need_bts()`
  for `PROVIDER::OPENFHE`, so fhe-cmplr generates the symbol for
  `-P2C:lib=openfhe` targets). Byte-identical to the backup.
- **F3.3-F3.6 - `openfhe_lib.cxx`** rebuilt as four layered commits that follow
  the feat's own evolution (`aba1d435` -> `c851516f` -> `2859787f`):
  - **F3.3** the bootstrap core: the `Need_bts()`-gated `OPENFHE_CONTEXT`
    constructor branch (`EvalBootstrapSetup`/`EvalBootstrapKeyGen`, FIXEDMANUAL /
    UNIFORM_TERNARY / HYBRID / level_budget [4,4], env-overridable
    `FHE_BTS_SF`/`FHE_BTS_RESERVE`, the `ir_capacity` chain sizing with the
    R-tower reserve below IR 0); the `Bootstrap()` method (EvalBootstrap ->
    Rescale-while-nsd>1 -> LevelReduceInPlace to the ir_capacity target); the
    `_bts_ir_capacity`/`_bts_R` members; the `Openfhe_bootstrap` C wrapper; and
    the `Prepare_input` input-seam (encrypt at
    `of_level = _bts_ir_capacity - input_level`). Hand-re-authored as the
    `aba1d435` .cxx minus the SIHE scalar/mask ops and the deep Clone (those
    land in F3.4/F3.5); the bootstrap-core methods are byte-identical to the
    backup.
  - **F3.4** the `Openfhe_copy` deep `Clone()` - the shallow `*res = *op`
    aliased dst with src and the in-place `EvalAddInPlace` corrupted src during
    the op/gemm BSGS reduce.
  - **F3.5** the SIHE scalar/mask ops (`Encode_float_mask`, the scalar
    `Mul`/`Add` overloads, the `Openfhe_mul_scalar`/`Openfhe_add_scalar`/
    `Openfhe_encode_float_mask` wrappers), restored by bringing `aba1d435`'s
    .cxx wholesale over F3.4.
  - **F3.6** the `Prepare_input` sharding rewrite (`Get_encode_scheme`): one
    ciphertext per scheme shard per its `MAP_DESC` at the `of_level` seam, so
    every shard stays within the slot count and the resnet18-conv2d `:sharding`
    crash is fixed. Brings `2859787f`'s .cxx wholesale over F3.5; at this point
    `git diff 2859787f -- rtlib/openfhe/src/openfhe_lib.cxx` is empty, and the
    C-side bootstrap .cxx reconstruction is byte-identical to the feat backup.
- **F3.7 - bootstrap gtest** (`rtlib/openfhe/unittest/ut_openfhe_bootstrap.cxx`,
  byte-identical to the backup) - globbed into the `ut_fhert_openfhe` gtest
  binary by `rtlib/openfhe/CMakeLists` (no CMake change). The
  `OpenfheBootstrap.Refresh` case builds the bootstrap context, bootstraps a
  known message, and asserts recovery within tolerance with a visible level
  refresh. ctest green (the section-6 CP4 functional gate).
- **F3.8 - bootstrap smoke + plain .inc bypass** - `eg_rtopenfhe_boot.cxx` +
  `.inc` (the smoke example, `Need_bts()` true) and the four plain `.inc`
  (`eg_rtopenfhe_add` / `add_const` / `mult_const` / `conv2d_keep_shape`) gain
  `bool Need_bts() { return false; }` so the plain levelled-SHE examples stay on
  the no-bootstrap path. Byte-identical to the backup.
- **F3.10 - sharding gap-analysis docs** - `regress58-sharding-gap-analysis.md`
  and `python-sharding-vs-c.md` brought from the backup and section-5-trimmed in
  place (absolute /tmp and /workspace paths, dated run labels, per-run
  seconds/max_abs/pass-counts, captured logs removed; the methodology, the
  `Get_encode_scheme`/`MAP_DESC` contract, and the python-vs-C mechanism
  comparison kept).

## How it was verified

- **Byte-identity vs the feat backup `2859787f`** - every C-side F3 file that
  exists in the backup is byte-identical to it: each header / hook / gtest /
  example / .inc separate-file commit verified with an empty diff, and the
  monolithic `openfhe_lib.cxx` verified region-by-region through F3.3-F3.5 and
  then as a whole at F3.6 (the diff vs `2859787f` is empty). The F3.1 and F3.10
  docs diff vs the backup shows only the sanctioned trims (path-parameterization,
  dated/per-run data removed).
- **Functional gates** - F3.1b POC recovers max_abs < 1e-3; the F3.7 gtest
  passes (recovery within 2-4e-4 < 1e-3, level refresh +11); the F3.8 smoke
  example prints SUCCESS with max_abs ~ 3e-4 < 1e-3, level refresh +11, and the
  four plain examples print SUCCESS on the levelled-SHE bypass. All real
  `EvalBootstrap` runs on OpenFHE 1.5.1.
- **No-network build** - the gtest `unittest` ExternalProject clones from the
  internal remote (offline-blocked); the build pre-populates a local gtest
  source so the `unittest` ExternalProject's download step is skipped, and
  uthash/BLAKE2 are FetchContent-pinned to local _deps with the disconnected
  flags. The openfhe sub-build is gated on `RTLIB_OPENFHE_PATH` (set to the
  OpenFHE 1.5.1 install prefix). None of this is committed (build/ is
  gitignored).

## Honest follow-ups (limitations, not silent "done")

- **F3.6 Prepare_input sharding - local e2e not re-run.** The v2 local_test
  harness `run_test.sh` only wires `--lib ant|ace`; the openfhe link/run path
  (C++ link in `fhe_build.sh`, `env.sh`) is a harness gap, not a sharding-code
  gap. The byte-identity to the feat backup (whose `c851516f` passed regress58
  58/58 including the resnet18-conv2d `:sharding` case) is the safeguard; the
  local e2e re-run needs the openfhe harness wired (follow-up).
- **C-side `FIXEDMANUAL` scale drift - NOT claimed solved** (per section 2.3).
  The C-side bootstrap ships the `Prepare_input` encode-seam fix (the docs
  report resnet20 acc=1.0/pred=3). The scale-drift investigation
  (`doc/ant-vs-openfhe-ckks-scale-drift/`, untracked) disagrees and was never
  reconciled in code - recorded as needing a re-run of `rt_openfhe-c` to
  reconcile. The Python FLEX re-anchor shim (F2.5) is opt-in / host-only /
  off-by-default and does NOT imply the C-side path is fixed.
- **resnet20 device - NOT run here** (that is Part 4, `rt_fhert`).
- **Build-infra notes** - the OpenFHE 1.5.1 used for verification is the
  `/usr/local` install; the gtest build needs the local-source pre-population
  above; the openfhe sub-build needs `RTLIB_OPENFHE_PATH=/usr/local` exported.
  These are local build-setup facts, not committed.
- **One stylistic note** - the F3.2 commit message contains a single em-dash
  (standard English punctuation; prescribed by the coordinator). All other F3
  commit messages and all F3 code/docs are ASCII. Flagged for the record.
