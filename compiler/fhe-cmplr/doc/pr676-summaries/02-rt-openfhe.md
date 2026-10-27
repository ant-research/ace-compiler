# PR #676 -- Part 2: rt_openfhe (python) provider

This is the second of five functional parts that compose PR #676
(`feat/openfhe-c-bootstrap`), landed as reviewable commits on the
`feat/openfhe-c-bootstrap-v2` branch (off `master`, base commit `30f387ff`).
Because the review system accepts only one overall PR, each functional part ends
with a marker commit that carries a summary like this one -- the per-part
description that would otherwise live in a separate PR message.

## Scope

- **F2** -- the `rt_openfhe` real-CKKS rtlib-py provider: the lazy level bridge,
  the model-driven `EvalBootstrap` context + op, the FLEX re-anchor shim
  (opt-in, host-only, off-by-default), the data-file readers, and the rt_openfhe
  test gate (a real-bootstrap smoke + the onnx-`rt_openfhe` corpus sweep).

No other backend ships here. `rt_ref` landed in Part 1; the OpenFHE C-side
bootstrap lands in Part 3; `rt_fhert` in Part 4; the ckks2py flag-policy
end-state and methodology docs in Part 5.

## What it adds

- **rt_openfhe/__init__.py** -- rebuilt as eight verbatim-prefix sub-commits that
  follow the backup's exact line order. The backup interleaves the core
  (arithmetic / IO / encode / query) and the bootstrap blocks, and the section 2.6
  byte-identity rule forbids reordering, so the commit sequence interleaves the
  F2.3-core and F2.4-bootstrap stages (each commit message labels its block).
  Each sub-commit is a verbatim prefix of the backup, truncated at a clean
  `def`/`class` boundary: importable without the OpenFHE binding (openfhe is
  imported lazily inside `_build_context`), additive-only versus the prior
  commit, and the final reproduces the backup exactly
  (`git diff 2859787f -- rt_openfhe/__init__.py` is empty). It carries the FLEX
  scaling parser and the `_FLEX_MODE`/`_FLEX_EXT`/`_FLEX_SCALE` module globals
  (default `fixedmanual`, inert); the on-demand level bridge (`Cipher.level` as
  a `@property`); the OpenFHE helpers (`_of_level`, `_drop_to_of_level` with the
  FLEX-EXT `compress_towers` pre-bake, `_align_ct_for_binop`, encrypt/decrypt,
  the plain helpers); the 3a leveld-SHE context builder and the model-driven
  `EvalBootstrap` context builder (the `ir_capacity = max(input_level,
  max_target + bts_depth + 1)` and `R = max(FHE_BTS_RESERVE, 2)` tuning); the
  arithmetic / IO / encode / query / validate / timing ops; the real
  `EvalBootstrap` op `bootstrap_ciph` (the ant-style nsd reset plus the bounded
  FLEX form); the provider-agnostic data-file readers; and the frozen `__all__`
  contract surface.
- **F2.5 -- the FLEX re-anchor shim** -- the
  `compress_towers = towers_left + 1 if _FLEX_EXT else towers_left` branch in
  `_drop_to_of_level` and the `scale_deg` clamp in `Cipher.__init__`/`_wrap` are
  present in the backup. The default `fixedmanual` keeps `_FLEX_MODE`/
  `_FLEX_EXT` `False`, so the fixed path is byte-identical to the baseline; the
  shim engages only under `FHE_BTS_SCALING=flexibleautoext` (opt-in). It is
  host-only and off-by-default and does NOT claim the C-side `FIXEDMANUAL` scale
  drift is solved (that is a Part 3 open issue).
- **F2.6 -- tests** -- `test_openfhe_bootstrap` (a real `EvalBootstrap` smoke: a
  toy Set-A context encodes a small message near the chain top and asserts
  `bootstrap_ciph` recovers it within tolerance with a fresh `scale_deg=1`,
  relined, on-demand level non-negative; SKIP-77 when the OpenFHE-python binding
  or pytest is absent) and `test_fhe_cmplr_py_verify_onnx_openfhe` (the
  real-CKKS onnx corpus sweep over `rt_openfhe`, the analog of the rt_ref onnx
  gate; SKIP-77 when the binding / onnx / pytest is absent), plus their CTest
  registrations. Brought verbatim from the backup (byte-identical); the one-off
  adjudication/sweep scripts that lived alongside them in the original PR stay
  out per the result-data triage.

## Verification (gate results)

- section 2.6: `git diff 2859787f -- rtlib-py/rtlib_py/rt_openfhe/__init__.py` is empty
  -- the staged union of the eight prefix commits reproduces the backup exactly.
  Every F2.6 test file is byte-identical to the backup; the
  `test/CMakeLists.txt` change is additive-only (the two new CTest entries, no
  change to the existing rt_ref entry).
- Import gate: `rt_openfhe` imports without the OpenFHE binding (lazy import
  inside `_build_context`); the FLEX globals default inert.
- Bootstrap smoke (`test_openfhe_bootstrap`): the real `EvalBootstrap` recovers
  the message within tolerance with a fresh `scale_deg=1`, relined, on-demand
  level non-negative -- the Part-2 functional gate.
- FLEX: default `fixedmanual` inert (byte-identical baseline); a FLEX-EXT
  bootstrap context drives `_FLEX_MODE`/`_FLEX_EXT` `True` (engaged).
- Arithmetic: the real-CKKS add/mul/rescale/rotate reproduce the plaintext
  reference within tolerance.
- rt_ref onnx sweep (`test_fhe_cmplr_py_verify_onnx`): unchanged by this part;
  zero FAIL with the onnx-models corpus provisioned (no regression vs master).
- rt_openfhe onnx sweep: the green subset passes except one known pre-existing
  flaky precision-boundary workload (a depthwise grouped conv at `eps=1e-6` that
  straddles the tolerance across runs with byte-identical emission; it passes
  under the plaintext reference). A test-harness disposition open issue, not a
  code defect -- reproduced from the backup byte-for-byte.

## section 2.6 backup-diff

Every committed file matches the feat `2859787f` backup byte-for-byte:

- `rt_openfhe/__init__.py`: empty diff at the final reconstitution (the eight
  prefix commits reproduce the backup exactly); each prefix was verified
  byte-identical to the matching backup slice at its commit.
- the F2.6 test files and `test/CMakeLists.txt`: brought verbatim, no sanctioned
  deltas needed.

No result-data trims were required for Part 2 (the files were already clean --
paths env-overridable via `OPENFHE_PY_BUILD_DIR`, no dated run residue).

## Known follow-ups (honest limitations, not blocking)

- The onnx sweeps (rt_ref and rt_openfhe) require the onnx-models corpus
  provisioned at test time (a transient working-tree `onnx-models` symlink,
  never committed); without the corpus the sweeps cannot run.
- The rt_openfhe onnx green sweep has one known flaky precision-boundary
  workload (depthwise grouped conv, `eps=1e-6`) -- pre-existing (present in the
  backup), passes under plaintext; a test-harness eps/XFAIL disposition, not a
  code defect.
- The heavy compile-only full-nets of the onnx sweeps were not run in this
  part's verification (skip-if-OOM; deferred to a full ctest run in an
  environment with the corpus and binding).
- The OpenFHE C-side `FIXEDMANUAL` scale drift is NOT claimed solved by this
  part; the FLEX shim is opt-in, host-only, off-by-default and does not imply
  the default or the C-side path is fixed (reconciliation is a Part 3 open
  issue).
- The `rt_openfhe/__init__.py` build interleaves the F2.3-core and F2.4-bootstrap
  stages across eight sub-commits, because the backup's line order interleaves
  them and byte-identity forbids reordering; each commit's message labels its
  block's plan-stage.

## Next

Part 3: the OpenFHE C-side bootstrap -- independent of F0/F2; splits the original
`aba1d435` and isolates the `c851516f` sharding fix; `ut_openfhe_bootstrap`
green, `eg_rtopenfhe_boot` smoke, the plain `.inc` examples, the regress18-conv2d
sharding PASS; the C-side `FIXEDMANUAL` drift explicitly NOT claimed solved.
