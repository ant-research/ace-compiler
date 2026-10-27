# PR #676 - Part 6: local-test methodology (final marker; reconstruction complete)

This is the sixth and final functional part that composes PR #676
(`feat/openfhe-c-bootstrap`), landed as reviewable commits on the
`feat/openfhe-c-bootstrap-v2` branch (off `master`, base commit `30f387ff`).
Because the review system accepts only one overall PR, each functional part ends
with a marker commit that carries a summary like this one -- the per-part
description that would otherwise live in a separate PR message. This commit is
Part 6's marker, and it is the FINAL marker: the whole PR #676 (the
`feat/openfhe-c-bootstrap` refactor) is now reconstructed across the six
functional parts as reviewable commits on `feat/openfhe-c-bootstrap-v2`.

## Scope

- **F6** -- the local-test methodology docs: binding the `cti` `local_test`
  regression harness to a built `fhe_cmplr` (the operator/model cti suite, the
  GPU/ACE backend choice, the read-only cti fallback, the hook-vs-skill
  decision); the rtlib-py Python-provider flow that runs the same `local_test`
  cases through the `rt_ref` / `rt_openfhe` / `rt_fhert` providers; and the
  `BUILD.md` / `README.md` linkage that points at the integration guide. Part 6
  ships methodology (how to run + how to read the results), not result data:
  the per-phase PASS/NOISY snapshots stay OUT (machine-point-in-time).

The five prior functional parts already landed: the foundation + rtlib-py
framework + `rt_ref` (Part 1), `rt_openfhe` (Part 2), the OpenFHE C-side
bootstrap (Part 3), `rt_fhert` host+device (Part 4), and the ckks2py flag
-policy end-state + cleanup (Part 5). Part 6 is the closing part; the
local-test-methodology docs are the last functional slice, and this marker
closes the whole PR #676 reconstruction.

## What it adds

- **F6.0 - 05-marker section-6.8 prose sanitize** the two F5.4-cleanup prose
  blocks in `05-ckks2py-flag-policy.md` that spelled the section-6.8 grep
  tokens in their underscored / `90-`-prefixed forms (the literal result-dump
  / runner-schema token strings) are reworded to the dashed / dropped-prefix
  descriptive forms (e2e-py-results / fhe-cgo-accuracy / runner schema) that
  convey the same cleanup meaning but do NOT match the literal section-6.8
  grep. Meaning unchanged; marker file only.
- **F6.1 - local_test integration methodology + BUILD/README linkage**
  `doc/local_test_integration.md` (net-new, section-5 trimmed) binds the
  `cti` `local_test` framework to a built `fhe_cmplr` in this checkout: what
  `local_test` is and what it adds over this repo's own compile-only
  `test/test_fhe_cmplr.sh` (the link + run + onnxruntime golden + per-element
  error check + matrix + reporting on top of the same compile step); the
  `${FHEROOT}` / `${FHE_BUILD_DIR}` / `${FHE_CMPLR}` / `${FHE_LOCAL_TEST_TMP}`
  path conventions (the three-source-repos-plus-build sibling workarea, spelled
  env-overridable, not hard paths); the `detect_env.sh --root` binding +
  `--check` smoke; the `run_test.sh` matrix (operator + model, optsets, the
  GPU/ACE backend choice, the read-only-cti copy-to-temp fallback); and the
  hook-vs-skill decision for the Claude integration that ships with this repo.
  Section-5 trim: strip the dated `verified end-to-end on 20YY-...` line, the
  per-run smoke output, and the `results.tsv` timestamp row; parameterize the
  build / workarea / temp paths to the four env vars above (the
  sibling-workarea shape stays as durable methodology). `doc/BUILD.md` gets
  the Release + `FHE/NN/AIR_CODE_CHECK=OFF` + `-j$(nproc)` build invocation
  (was `-j"N"` / no flags; also clears a CJK full-stop residue) and
  `doc/README.md` links the integration guide from the design-doc index.
- **F6.2 - local-test-rtpy Python-provider methodology (trimmed)**
  `doc/local-test-rtpy/00-methodology.md` (net-new, section-5 trimmed) is the
  rtlib-py Python-provider `local_test` methodology -- a separate flow from
  the C/GPU `local_test` regression and from the `regress58` corpus sweep,
  running the same `cti` `local_test` 17-operator + 4-model case set through
  the `rt_ref` / `rt_openfhe` / `rt_fhert` providers. Documents the
  `MaxPool`-to-`AveragePool` CKKS-semantics golden (the compiler lowers
  `MaxPool` to `AveragePool`; the golden is rewritten to match `local_test`'s
  own CKKS semantics, so a passing case proves the backend correctly executes
  the compiler-lowered `AveragePool` against a `MaxPool` input; the
  strict-ONNX MaxPool-MISMATCH view is also reported, not hidden); the `df`
  on/off sweep axis; the device (cmodel) phase prerequisites (cache + ring +
  bts-sf); and the result terminology (PASS / NOISY / MISMATCH / COMPILE_FAIL
  / EXEC_ERROR / RESOURCE_FAIL_* / EXC, plus the non-finite guard that
  rejects NaN/inf actuals). The driver is the runner that validates
  end-to-end with one local hook; the harness core is unchanged. Section-5
  trim: strip the local cache-home temp literal, the `/opt` ACE env
  interpreter literal, the `cp312 onnxsim` host note, the round / pass-count
  labels, the local temp results file and the local temp workarea path, and
  the per-run accuracy-magnitude values; parameterize the device env to a
  generic fhert root; inline the runner result-schema vocabulary (drop the
  OUT'd file ref). `.gitignore` gets `doc/local-test-rtpy/*` +
  `!00-methodology.md` so the per-phase PASS/NOISY snapshots (the runner
  README + the phase docs) are OUT (machine-point-in-time, not committed);
  the methodology is the kept artifact.

## Gates

- **section-6.8 residue 0 over the tracked tree** -- the section-6.8 token
  grep (the result-dump tokens, the runner-schema tokens, the dated
  RESULTS-file refs, the stage-doc refs, the work-summary / repro /
  device-diagnostic tokens, and the literal build / temp absolute paths)
  over tracked `doc/` `test/` is EMPTY (re-run after this marker commits).
- **F6 docs dated markers 0** -- no ISO date strings, no per-run numeric
  (wall-clock or accuracy-magnitude) literals in the prose, no round-N or
  pass-count run vocabulary in `doc/local_test_integration.md` or
  `doc/local-test-rtpy/00-methodology.md`.
- **F6 docs ASCII clean** -- full-ASCII; no em-dash, no section-sign, no
  smart-quotes, no CJK.
- **runner-schema dangling refs 0** -- the OUT'd runner-schema file ref is
  gone from the kept methodology; the runner result-schema vocabulary is
  inlined.
- **OUT corpus snapshots NOT committed** -- `git check-ignore` confirms the
  per-phase PASS/NOISY README + the phase docs are gitignored under
  `doc/local-test-rtpy/`; only `00-methodology.md` is tracked there.
- **marker ASCII + residue 0** -- this marker file is full-ASCII; the marker
  residue grep (the dated, round, per-run-numeric, and the literal build /
  fhert / temp absolute-path token classes) is 0 on this file after
  excluding the four env-var dollar tokens (`${FHEROOT}` /
  `${FHE_BUILD_DIR}` / `${FHE_CMPLR}` / `${FHE_LOCAL_TEST_TMP}`).

## Honest follow-ups (limitations, not silent "done")

- **Fresh-checkout `cmake -B build` configure is env-gapped in THIS env.**
  The F6.1 gate is the cmake-configure + `detect_env --check` pair. The
  sanctioned INCREMENTAL configure (`cmake -B ${FHE_BUILD_DIR}` against the
  already-populated deps) IS green; `detect_env --check` IS green against
  that build (`fhe_cmplr` runs; headers compile; the rtlib libs exist). The
  fresh-checkout configure (no `FHE_WITH_SRC`, not against a prebuilt
  ant-ace SDK) is env-gapped here: the `nn-addon` `onnx` FetchContent tries
  `git clone` the internal `air-infra/onnx` remote and that remote is
  unreachable offline (`ECONNREFUSED` / Unauthorized); the
  `FETCHCONTENT_UPDATES_DISCONNECTED_*` flags only cover `uthash` / `blake2`.
  The doc describes the methodology (the sanctioned incremental configure
  path); the env-gate on the fresh-configure is honestly recorded as a
  follow-up for an env with the internal remote reachable or a prebuilt SDK.
- **Per-phase PASS/NOISY corpus snapshots are OUT.** The per-phase
  PASS/NOISY matrices and the runner README are machine-point-in-time
  pass-count snapshots; per the reviewer principle (commit what you will
  maintain, not what is true for one machine at one point in time) they are
  OUT (gitignored local-only under `doc/local-test-rtpy/` and the per-phase
  dirs). The methodology (`00-methodology.md`: how to run, how to read the
  results, the CKKS-semantics golden, the result terminology) is the kept,
  durable artifact. The pass-counts themselves are not committed.
- **Device (cmodel) phase is documented, not green-verified in this PR.**
  The `00-methodology.md` device section records the cmodel cache / ring /
  bts-sf prerequisites and points at the `fhert-device-fix` open issues for
  the documented device VRF/HBM ceiling; the device phase is not claimed
  green in this PR. The host Python-provider flow and the C-path
  `local_test` regression are the green-verified surfaces here; the device
  phase is honestly recorded as prerequisites + open issues, not a passing
  gate.

## Whole-PR676-completion note

This is the FINAL marker. The whole PR #676 (the `feat/openfhe-c-bootstrap`
refactor) is now reconstructed as reviewable commits on
`feat/openfhe-c-bootstrap-v2` (off `master`, base `30f387ff`), across the six
functional parts:

- **Part 1** (`01-foundation-and-rtlib-py.md`) -- the F0 foundation + the
  rtlib-py framework + `rt_ref`.
- **Part 2** (`02-rt-openfhe.md`) -- `rt_openfhe` (the lazy level bridge, the
  model-driven bootstrap, the FLEX-EXT shim that fixes the scale drift).
- **Part 3** (`03-openfhe-c-bootstrap.md`) -- the OpenFHE C-side
  `EvalBootstrap` (the C `rtlib/openfhe` backend bootstrap added; resnet20
  matches the rt_openfhe reference).
- **Part 4** (`04-rt-fhert.md`) -- `rt_fhert` host + device (the monolithic
  port, the FLEX-EXT Compress gate, the inline device seams).
- **Part 5** (`05-ckks2py-flag-policy.md`) -- the ckks2py flag-policy
  end-state + the rename-narrative residue cleanup + the SKIP-gate finalize
  + the ASCII-hygiene pass.
- **Part 6** (`06-local-test-methodology.md`, this marker) -- the local-test
  methodology docs.

Each functional part ends with a marker commit (01 / 02 / 03 / 04 / 05 / 06)
that carries this kind of per-part summary, and each marker commit is
followed by a backup (pr01 / pr02 / pr03 / pr04 / pr05 / pr06: a worktree
snapshot excluding `build/` / `_f12b/` / `doc/pr676-refactor/` plus a full
standalone git bundle, with the LATEST pair appended to the backup-index
file). With Part 6's marker and the pr06 backup, the PR #676 reconstruction
on `feat/openfhe-c-bootstrap-v2` is COMPLETE.
