# PR #676 -- Part 1: Foundation + rtlib-py framework + rt_ref

This is the first of five functional parts that compose PR #676
(`feat/openfhe-c-bootstrap`), landed as a single reviewable PR on the
`feat/openfhe-c-bootstrap-v2` branch (off `master`, base commit `30f387ff`).
Because the review system only accepts one overall PR, each functional part
ends with a marker commit that carries a summary like this one -- serving as
the per-part description that would otherwise live in a separate PR message.

## Scope

- **F0** -- the slim shared foundation: the frozen runtime contract, the
  ckks2py emitter headers, and the single shared design that governs them.
- **F1** -- the rtlib-py framework (common driver + contract layer, the
  run_py_verify executor and workloads harness) and the `rt_ref` plaintext
  reference provider; CMake wiring; existing C-path test-driver routing;
  result-data and psutil gitignore hygiene; the rtlib-py open-issues catalog.

No backend beyond `rt_ref` ships here. `rt_openfhe` (python) lands in Part 2;
the OpenFHE C-side bootstrap in Part 3; `rt_fhert` in Part 4; the ckks2py
flag-policy end-state and methodology docs in Part 5.

## What it adds

- F0.2 -- the ckks2py emitter headers at `include/fhe/ckks/`
  (`ckks2py_pass.h`, `ckks2py_config.h`, `ckks2py_driver.h`, `ir2py_ctx.h`,
  `ir2py_core.h`, `ir2py_handler.h`), split by opcode group into reviewable
  sub-commits. The `-CKKS2PY` group is registered default-inactive.
- F0.3 -- the `rtlib_py.common` contract layer: the 39 frozen names
  (36 functional + `Cipher` / `Plain` / `FakeRtError`), the `RuntimeProvider`
  Protocol, `check_provider`, and the `register` / `ALL_PROVIDERS` machinery.
- F1.1 -- a hand-built contract round-trip POC (gap-filler; never existed before).
- F1.2 -- the `ckks2py` pass `.cxx` impl + the `-CKKS2PY:rt=` activation +
  CMake wiring. The F1.2b build gateway is verified: a fresh build, python
  emission, and default-C byte-identity to the master baseline.
- F1.3 -- the `run_py_exec` executor.
- F1.4 -- the `rt_ref` plaintext provider (`rt_ref/__init__.py`), hand-re-authored
  as three additive sub-commits (context + arithmetic; encode + IO + free;
  query + validate + timing). Each sub-commit leaves the file importable and
  the new ops working; the diff vs the previous sub-commit is additive-only.
- F1.5 -- the poly2py allops C++ emit+exec ctest (`test_ckks2py_allops`).
- F1.6 -- the py-verify harness (`run_py_verify.py`, `gen_workload_onnx.py`,
  `workloads.py`), landed in the import-correct order (generator, then
  workloads, then the executor; the executor imports `workloads` module-scope).
- F1.7 -- the py-verify e2e onnx driver (`test_fhe_cmplr_py_verify_onnx`),
  exercising `rt_ref` over the onnx corpus (green single-ops + peripheral +
  heavy compile-only full-nets; zero FAIL).
- F1.10 -- gitignore + psutil hygiene: the `.gitignore` converged to the feat
  end-state so `git status` stays clean after a fhe_cmplr run (`.conf` / `.t`
  byproducts ignored), and the psutil shims import so the test harness runs
  without the `psutil` pip package.
- F1.11 -- the rtlib-py open-issues catalog (generalized, no point-in-time
  run data).

## Verification (gate results)

- A fresh `fhe_cmplr` builds from v2 source (Release, code-check off, offline
  hermetic deps reused).
- `fhe_cmplr -CKKS2PY:rt=rt_ref` (and `rt=openfhe`) emits a `.py` + a `.conf`
  and suppresses the `.c` -- the python emitter activates, the C emitter does
  not leak.
- The default (no `-CKKS2PY`) `.c` for a single onnx is byte-identical to a
  clean `master` (`30f387ff`) build.
- `ctest -R test_ckks2py_allops` green; `ctest -R test_fhe_cmplr_py_verify_onnx`
  green (`rt_ref` over the onnx corpus: green sweep + informational probes;
  heavy full-nets compile-only; zero FAIL).
- `ctest -R 'test_fhe_cmplr|test_bts_rescale|test_ckks2py_allops'` green --
  the existing C-path tests route through the refactor unchanged.
- The `-CKKS2PY` mapping is additive/optional; the default C path is
  byte-identical to master.
- `git status` is clean after a fhe_cmplr run; the psutil shim is importable.

## section 2.6 backup-diff

Every committed file matches the feat `2859787f` backup byte-for-byte, allowing
only the explicitly sanctioned deltas:
- one added `#include "fhe/ckks/ckks_opcode.h"` in `ir2py_ctx.h`
  (self-containment);
- the F1.11 open-issues catalog residue trim (point-in-time run numbers
  generalized per the result-data triage);
- the `.gitignore` converged to the feat end-state.
- After `rt_ref`'s final reconstitution (F1.4c),
  `git diff 2859787f -- rtlib-py/rtlib_py/rt_ref/__init__.py` is empty -- the
  staged union reproduces the backup's logic exactly.

## Known follow-ups (honest limitations, not blocking)

- Three section 6.8 residue hits remain in the kept tree at this part's end: a
  hardcoded default build path in `run_py_verify.py` and a reference to an
  OUT result script in the open-issues catalog. These are the
  path-constants-to-env + result-reference cleanup slated for the cleanup
  part (Part 5).
- `fhe_cmplr -CKKS2PY:rt=openfhe` emits `from rtlib_py.openfhe import *`; the
  provider module is `rtlib_py.rt_openfhe`. The `openfhe` alias resolution is
  verified in Part 2 (`rt_openfhe`).

## Next

Part 2: the `rt_openfhe` (python) provider -- lazy level bridge, bootstrap,
data-file, FLEX scaling.
