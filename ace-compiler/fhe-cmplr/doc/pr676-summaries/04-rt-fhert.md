# PR #676 - Part 4: rt_fhert host+device provider

This is the fourth of five functional parts that compose PR #676
(`feat/openfhe-c-bootstrap`), landed as reviewable commits on the
`feat/openfhe-c-bootstrap-v2` branch (off `master`, base commit `30f387ff`).
Because the review system accepts only one overall PR, each functional part ends
with a marker commit that carries a summary like this one - the per-part
description that would otherwise live in a separate PR message.

## Scope

- **F4** - the `rt_fhert` host+device provider: the third `rtlib-py` provider
  (alongside the plaintext `rt_ref` from Part 1 and the upstream `rt_openfhe`
  from Part 2), a near-verbatim port of `rt_openfhe` onto fhert's own OpenFHE
  1.4.2 pybind11 binding `fhert.openfhe`. Host mode runs real CKKS on the host
  via a lazy binding import; device mode is opt-in via `CModelDevice` (fhert's
  device simulator). Selected by the single compiler field `-CKKS2PY:rt=rt_fhert`.

No other provider ships here. `rt_ref` (Part 1), `rt_openfhe` (Part 2), and the
OpenFHE C-side bootstrap (Part 3) already landed; the ckks2py flag-policy
end-state and methodology docs are Part 5.

## What it adds

**Design and binding POC**

- **F4.1 - port design** (`doc/rtlib-py-doc/fhert-backend-design.md`, net-new)
  the design-approval gate. Enumerates the three deliberate deltas the port
  carries against `rt_openfhe`: (1) a cp312 stub-bypass binding import (fhert is
  a PATH-based bazel tree, not pip-installed; `_ensure_of` prefers the full
  `fhert.openfhe` and falls back to loading `fhert._C._lib.openfhe` directly
  with stub packages that skip the libfhert import chain); (2) bare-enum
  adaptation (fhert re-exports the enum values `FIXEDMANUAL` / `HEStd_NotSet`
  but not the `ScalingTechnique` / `SecurityLevel` classes, so call-sites use
  the bare `_of.FIXEDMANUAL` form); (3) `SPARSE_TERNARY` + an explicit
  `correctionFactor` (fhert 1.4.2 rejects the default `UNIFORM_TERNARY`
  bootstrap on the EvalMod Chebyshev degree). Lays out the host-vs-device
  execution plan (host bootstrap on ring 16384, model-driven context, lazy
  level bridge; device opt-in via `CModelDevice` with inline round-trip seams,
  HBM budget env, half-packing opt-in, per-bootstrap reclaim).
- **F4.2 - binding POC** (`rtlib-py/rtlib_py/poc_fhert_binding.py`, net-new) a
  gap-filler proving the load-bearing assumption of the port: the fhert
  binding imports natively on CPython 3.12 host-only and a minimal
  `EvalBootstrap` on ring 16384 recovers a real message within the tolerance.
  No CModel / device dependency.

**Contract and monolithic reconstruction** (`rtlib-py/rtlib_py/rt_fhert/__init__.py`)

- **F4.3 - contract stub** the 39 `CONTRACT_NAMES` (36 functional calls +
  `Cipher` / `Plain` / `FakeRtError`) with the exact emitter signatures, every
  body raising `NotImplementedError`, and `__all__` parity with `rt_ref` (41
  names incl. the `CMPLR_TM_*` timing symbols).
- **F4.4a - skeleton prefix** the first verbatim block of feat `2859787f`
  (setup + globals + driver glue + constraint helpers), replacing the F4.3
  stub, with the section-5 surgical trim of the module docstring (the
  cp3.12 / cp3.13 / `PyThreadState_GetUnchecked` machine-build-state narrative
  excised). The FLEX parser (`_parse_flex_scaling`) and the inert FLEX module
  globals (`_FLEX_MODE` = `_FLEX_EXT` = `False`, `_FLEX_SCALE` = 2) are
  pre-baked at import.
- **F4.4b - skeleton completion** the value classes (`Cipher.level` on-demand
  `@property`, `Plain`), the openfhe-primitive helpers (`_of_level`,
  `_drop_to_of_level` with its inline FLEX-EXT Compress gate + inline
  device-Compress fallback, `_align_ct_for_binop`, `_encrypt_values`,
  `_decrypt_slots`, `_plain_*`), the lazy binding seam (`_ensure_of`), and the
  FULL dispatcher (`_build_context` with the bootstrap / device branches as
  forward-refs resolved via late binding).
- **F4.6 - host bootstrap context** (`_build_bootstrap_context`) sets
  `SPARSE_TERNARY` and the `_BTS_CORRECTION` = 13 correction-factor gate, and
  activates FLEX-EXT via `_parse_flex_scaling(_BTS_SCALING, "flexibleautoext")`
  with the `_FLEX_EXT` guard. Resolves the dispatcher bootstrap forward-ref.
- **F4.7a - device context + reclaim core** (`_build_device_context`) the HBM
  budget env (`FHE_DEVICE_HBM_GB`), half-packing opt-in
  (`FHE_DEV_BTS_HALF`), the device FLEX selector (`FHE_DEV_SCALING`), the
  plain-diags drop (`FHE_DEV_BTS_NO_PLAINDIAGS`), and the CRAM accounting.
  Curation A1-A6 in, A7-A8 OUT, A9 future. Resolves the dispatcher device
  forward-ref.
- **F4.5a - SHE ops + IO** `add`/`sub`/`mul`/`unary`/`relin`/`rescale`/
  `mod_switch`/`upscale` plus the IO helpers, with the inline
  `if _device_mode():` round-trip seams and `if _FLEX_EXT:` guards pre-baked
  verbatim from feat (inert at import, late-bound at call).
- **F4.7b - device reclaim helpers + input-seam**
  `_reclaim_bts_device_scratch` (deterministic device-HBM-scratch reclaim
  after `EvalBootstrap`) and `_run_reclaim_diag`, the late-in-feat device
  block (the inline device seams and HBM / half-packing env already landed
  verbatim at F4.4b / F4.7a / F4.5a).
- **F4.5b - raise_mod + bootstrap_ciph runtime op** the downward raise via
  `_drop_to_of_level`, the FLEX nsd-reset loop (the FLEX-shim runtime site,
  gated on `_FLEX_MODE`), and `bootstrap_ciph` (the real-`EvalBootstrap`
  runtime op the emitted .py calls; invokes `_reclaim_bts_device_scratch`
  after a device bootstrap). Resolves the dispatcher bootstrap branch.
- **F4.5c - lifecycle + encode + data-file readers** `copy_ciph` /
  `zero_ciph` / `free_*` lifecycle, the encode trio (`encode_float` /
  `encode_double` / `encode_dcmplx`), and the offline data-file readers
  (`_read_data_file`, `load_data_entry`, verbatim from `rt_ref`).
- **F4.8 - queries + observability + export tail** the `level` / `sc_degree`
  queries, the `validate` / timing / dump observability, and the `__all__`
  export tail (the full 41-name export list). This completes the module.

**Monolithic reconstruction note (section 2.6).** F4.4a establishes the first
verbatim block (replacing the F4.3 stub); eight additive verbatim-prefix
sub-commits then append the remaining blocks:
`F4.4b`, `F4.6`, `F4.7a`, `F4.5a`, `F4.7b`, `F4.5b`, `F4.5c`, `F4.8`. The
sequence reproduces feat `2859787f`'s `rt_fhert/__init__.py` byte-for-byte
except the one sanctioned module-docstring section-5 trim. The FLEX-EXT
Compress gate (`towers_left + 1 if _FLEX_EXT else towers_left` inside
`_drop_to_of_level`) and the `_FLEX_EXT` activation landed byte-verbatim
across F4.4b / F4.6 / F4.7a, so F4.8 adds no new FLEX code - it lands only the
tail and `__all__`. `git diff 2859787f HEAD --
rtlib-py/rtlib_py/rt_fhert/__init__.py` shows only the docstring trim.

**Tests, runner, and docs**

- **F4.9 - tests** (`test/rtlib_py/test_fhert_bootstrap_host.py` + the `.sh`
  wrapper, `test/rtlib_py/py_verify/run_resnet20_vs_onnx.py`,
  `test/rtlib_py/test_fhert_device_resnet20.sh`, `test/CMakeLists.txt`) the
  host bootstrap smoke, the host resnet20-vs-onnxruntime harness, and the
  ctest entries. Test-harness deps `test/ace_compile.py` and
  `test/run_py_accuracy.py` are brought verbatim from feat so the host
  resnet20 gate can run. `test_fhert_bootstrap_host.py`,
  `test/run_py_accuracy.py`, and `test/ace_compile.py` are byte-identical to
  feat; `run_resnet20_vs_onnx.py` carries only the sanctioned section-5
  path-parameterization (the default ONNX / CIFAR paths become env-overridable
  via `FHE_ONNX` / `FHE_CIFAR`; the assertion logic unchanged).
- **F4.10 - device FIXEDMANUAL harden docs**
  (`doc/fhert-device-fix/OPEN-ISSUES.md`) the section-5-trimmed open-issues
  catalog (dated refinement headers and per-run figures removed; the
  methodology kept). The device FIXEDMANUAL harden code itself - the
  conditional `_FLEX_MODE` Strategy-D gate (FLEX-default-preserves-baseline),
  the `Cipher.clone` device round-trip short-circuit, and the CRAM fail-fast
  guard on the bconv-table overflow - is present byte-verbatim in the
  reconstructed provider (carried from feat via the F4.4b-F4.7b
  reconstruction). No F4.10 code change.
- **F4.11 - regress58 sweep runner + catalogs**
  (`doc/regress58-params/run_cti_params_sweep.py`,
  `doc/regress58-params/regress58_catalog.csv`, and the expanded second
  catalog) the env-parameterized sweep runner (no hardcoded paths) and the
  two config catalogs, brought from feat with the sanctioned
  path-parameterization edits. The catalogs are pure config descriptors; no
  per-run numbers.
- **F4.11b - runner self-contained** inlines the three small pure-utility
  helpers the runner uses (`_graph_info`, `_fill`, `_gen_inputs`) directly
  into `run_cti_params_sweep.py` and removes the dangling import, so the
  runner works in a clean v2 checkout without pulling an OUT-of-tree sweep
  helper.
- **F4.12 - handoff + open-issues doc** (`doc/rtlib-py-doc/fhert-handoff.md`,
  net-new) the rt_fhert handoff and open-issues catalog (provider
  methodology, FLEX-EXT / no-EXT scaling path, host-vs-device, the F4
  reconstruction lineage, open issues A9 / B1 / F3).

## How it was verified (the CP5 gate)

- **Host bootstrap smoke GREEN** - ring 16384, the recovery error well under
  `FHE_BTS_TOL` (default 1e-1). Door: `test_fhert_bootstrap_host.py` + the
  `.sh` wrapper, ctest entry `test_fhert_bootstrap_host`.
- **Host resnet20-vs-onnxruntime GREEN** - the `rt_fhert` prediction matches
  the onnxruntime golden label under BOTH the default FLEX-EXT path AND the
  no-EXT path (`FHE_BTS_SCALING=flexnoext`). Qualitative per section 5; no
  per-run figures in this marker.
- **Contract surface** - 41-name export surface (39 `CONTRACT_NAMES`: 36
  functional calls + `Cipher` / `Plain` / `FakeRtError`, plus the
  `CMPLR_TM_*` timing symbols); `__all__` parity with `rt_ref`.
- **Monolithic byte-identity** - `git diff 2859787f HEAD --
  rtlib-py/rtlib_py/rt_fhert/__init__.py` shows only the docstring section-5
  trim.
- **Regress58 sweep runner** - the 1-row host smoke (`rt_fhert`, host
  binding, no device) is green and the runner is self-contained in v2 (no
  OUT-of-tree import).
- **resnet20-device ctest** - `test_fhert_device_resnet20` defaults to SKIP
  (`SKIP_RETURN_CODE` 77; opt-in `FHE_RUN_RESNET20_DEVICE=1`); it exits 77
  (ctest SKIP, not FAIL) under the default.

## Honest follow-ups (limitations, not silent "done")

- **resnet20 device NOT run this refactor.** The device end-to-end resnet20
  run is not executed here; the ctest defaults to SKIP (`SKIP_RETURN_CODE`
  77, `FHE_RUN_RESNET20_DEVICE=1` opt-in). The per-cycle device-HBM
  accumulation is a known not-yet-passing limitation; the principled fix B1 -
  a per-cycle `dev.reset_scratch` call mirroring the hand-written cmodel's
  bump-pointer rewind, which bounds that accumulation - is future work and
  is NOT implemented this round. Follow-up A9 re-verifies the device suite
  after the cmodel JIT rebuild (below).
- **Device suite proven-by-feat, NOT re-verified here.** The device suite
  (bootstrap smoke, lenet_x2 end-to-end, operator-level regress58) is a
  working feature per feat `41af5282` on a full-cmodel host, but was NOT
  re-verified in THIS refactor's env because the CModel JIT kernel compile
  raises `struct PerfSnapshot has no member named 'lines'` - a cmodel
  header-schema skew, not a refactor regression (the `rt_fhert` device code
  is byte-verbatim feat). Resolution requires a cmodel JIT rebuild;
  re-verification is recorded as follow-up A9.
- **C-side FIXEDMANUAL drift is an F3 open issue, not F4.** The F4.10 device
  FIXEDMANUAL harden (conditional `_FLEX_MODE` Strategy-D gate, `Cipher.clone`
  device round-trip short-circuit, CRAM fail-fast guard) is byte-verbatim in
  the reconstructed provider; the C-side (`rtlib/openfhe`) scale-drift
  question is tracked under Part 3, not here.
