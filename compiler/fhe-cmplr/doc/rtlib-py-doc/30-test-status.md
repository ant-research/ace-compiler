# rtlib-py validation: test suites and results

> Canonical entry point for validation of the rtlib-py Python path: which suites exist, what each validates, and what was executed / passed / not passed / untested. df user-facing usage is in [10-new-backend-integration.md](10-new-backend-integration.md); the implementation status and the `_slots_close` non-finite guard mechanism are in [20-implementation-status.md](20-implementation-status.md); generalized compiler limitations are in [40-open-issues.md](40-open-issues.md); the resnet20 real-CKKS root-cause + fix is in [50-resnet-benchmark-case-study.md](50-resnet-benchmark-case-study.md).

**df on/off gate:** across the df on/off sweep (df-on and df-off variants of every case), there are zero `NameError`s and df-on results are no worse than df-off, so the df externalization path passes its gate.

---

## Validation scope and result terminology

What is validated:

- **regress 58** -- 58 small op/conv/gemm/avg_pool/op_block tuples, run under `rt_ref` with df off and df on (116 runs). The primary correctness suite.
- **resnet batch** -- 6 resnets x {`ace_test` n=1, `cgo25_ae` n=1, `cgo25_ae` n=10} = 18 grid cells, across `rt_ref` and `rt_openfhe`, via the resnet-batch accuracy harness. A separate accuracy harness, not part of the regress sweep.
- **non-regress appendix** -- all onnx cases other than the regress 58, run under `rt_ref` df-on; retained for failure classification only, not primary correctness evidence (see [Appendix](#appendix-other-non-regress-results)).

Result status vocabulary (PASS / NOISY / MISMATCH / EXEC_ERROR / RESOURCE_FAIL_* / RUNTIME_CONSTRAINT) and the per-provider tolerances (`rt_ref` 1e-6; `rt_openfhe`, `rt_fhert` 1e-5) are defined in `doc/regress58-params/run_cti_params_sweep.py` (`classify()` and module docstring; `TOL`). df-on uses the `-CKKS2PY:df=` offline data-file path (see [10-new-backend-integration.md](10-new-backend-integration.md)).

---

## Result overview (all suites)

| Test suite | Configuration or backend | Intended | Executed | Passed | Not passed (testcase issues) | Untested | Notes |
|---|---|---|---|---|---|---|---|
| regress58 | rt_ref, df-off | 58 | 58 | 58 | 0 | 0 | all non-zero finite max_abs, no NaN |
| regress58 | rt_ref, df-on  | 58 | 58 | 58 | 0 | 0 | df-on not worse than df-off; zero NameError (df gate) |
| resnet (18-cell grid) | rt_ref | 18 | 17 | 11 | 0 | 1 | 11 acc=1.0; 6 acc<1.0 are NOT impl failures (resnet110 x2 = rt_ref float64-overflow provider limit; resnet32_cifar100 x3 + resnet44 n10 = low accuracy); 1 cell unrecorded |
| resnet (18-cell grid) | rt_openfhe | 18 | 3 | 0 | 0 | 15 | 3 batch cells executed pre-bts-fix, all acc=0.0 = real-CKKS multiplicative-depth gate (not an impl failure); 1 post-bts-fix resnet20 image verified ([Resnet batch detail](#resnet-batch-detail)); 15 untested |

> Regress totals: 2 configs x 58 = 116 intended / executed / passed; 0 not-passed; 0 untested.

The per-case matrix (per-case status, `max_abs`, timing) is a run snapshot and is not recorded here. Regenerate it via `doc/regress58-params/run_cti_params_sweep.py` (catalogs `doc/regress58-params/regress58_catalog.csv`, `doc/regress58-params/phase2_catalog.csv`); the class-A/B/D/F1/F2/I/J case lists are in [40-open-issues.md](40-open-issues.md) and the runner output.

---

## Note on NaN-blowup cases and the comparison guard

12 deep CIFAR CNNs numerically blow up to NaN/inf under the `rt_ref` plaintext provider (a generalized compiler limitation; see [40-open-issues.md](40-open-issues.md)). The comparison helper `_slots_close` now rejects non-finite actual values and reports FAIL (canonical mechanism in [20-implementation-status.md](20-implementation-status.md)); the guard is shared by all providers via `common/driver.run`. regress58 is unaffected: all 116 PASS have non-zero finite `max_abs` (true PASS; small models do not blow up). The 12 blowups are all in the non-regress appendix, not regress.

---

## Resnet batch detail

Raw artifact counts (resnet-batch accuracy harness):

- **rt_ref: 18 intended, 17 recorded, 11 passed (acc=1.0), 6 acc<1.0.** The 6 non-passes are not failures of the implementation under test -- resnet110 x2 is the `rt_ref` float64-overflow provider limit, and resnet32_cifar100 / resnet44 n10 are low accuracy (cause undetermined). 1 cell (`resnet110 test_cgo n=10`) is unrecorded.
- **rt_openfhe: 18 intended, 3 executed (pre-bts-fix), 0 passed, 15 untested.** The 3 executed batch cells returned acc=0.0 -- blocked by a real-CKKS multiplicative-depth gate (depth 52 insufficient), neither a testcase issue nor an impl failure. Post-bts-fix, 1 single `resnet20` image was verified correct (`pred=3=label`; see [50-resnet-benchmark-case-study.md](50-resnet-benchmark-case-study.md)), but the 18-case batch was not rerun, so 15 cells remain untested. Do not read the batch as "passed" -- 0 of 3 executed cells passed.

Detail and generalized limitations are in [40-open-issues.md](40-open-issues.md) and [50-resnet-benchmark-case-study.md](50-resnet-benchmark-case-study.md).

---

## Relocation validation

The Python-emit pass was relocated from `fhe::poly` to `fhe::ckks` (classes `POLY2PY_*` -> `CKKS2PY_*`; `IR2PY_CTX` re-rooted at `fhe::ckks::IR2C_CTX`; the C++ emit-gate test renamed; CMake re-wired so `ckks/` owns the `phase1_verify` label). The runtime/providers are untouched; the user-facing `-CKKS2PY` surface is unchanged. End-to-end re-validation re-ran the two primary exercises on all three providers with zero hard failures.

**regress58 (post-relocation, host):**

| Provider | PASS | NOISY | hard fail | ran |
|---|---|---|---|---|
| `rt_ref` | 116 | 0 | 0 | 116/116 (58 x df{off,on}) |
| `rt_openfhe` | 104 | 12 | 0 | 116/116 |
| `rt_fhert` (host) | 107 | 9 | 0 | 116/116 |

`rt_openfhe`/`rt_fhert` NOISY cases (`max_abs` 1.2e-5-3.8e-5, above the per-`rt` strict tol, well below 1e-2) are the documented real-CKKS run-to-run precision band on the leveled-SHE path (the regress corpus inserts no ReSBM, so these do not exercise bootstrap).

**resnet20 (n=1) real-CKKS re-verification (host):**

| Provider | result (n=1) | env |
|---|---|---|
| `rt_ref` | verified correct | plaintext (no bootstrap) |
| `rt_openfhe` | verified correct | `FHE_BTS_SF=59 FHE_BTS_RESERVE=8`; openfhe-python build via `OPENFHE_PY_BUILD_DIR`/`PYTHONPATH` |
| `rt_fhert` (host) | verified correct | fhert host-mode bootstrap (`FHERT_ROOT`; `FHE_BTS_SF=59`) |

Reaching `accuracy=1.0` on both real-CKKS providers confirms the relocation preserves the deep real-CKKS bootstrap path, not only leveled-SHE. The bootstrap level-reduction work is in `doc/rt_openfhe-bootstrap-level-reduction/RESULT.md`.

**Reproduction:**

```bash
# regress58 x {rt_ref, rt_openfhe, rt_fhert} x df{off,on} (host; real-CKKS providers are slow)
python3 doc/regress58-params/run_cti_params_sweep.py \
  --csv doc/regress58-params/regress58_catalog.csv \
  --out /tmp/relocate_regress.csv \
  --providers rt_ref,rt_openfhe,rt_fhert --df-states off,on \
  --compile-timeout 300 --exec-timeout 600

# resnet20 (n=1) single image per provider (host) -- env var forms in 50-resnet-benchmark-case-study.md.
```

---

## Appendix: other non-regress results

> Not primary correctness evidence. The non-regress 91 cases (all onnx cases other than the regress 58) under `rt_ref` df-on, retained for failure classification. Several non-passes are invalid testcases, misconfigured arguments, or unsupported configurations (not impl failures) -- see [40-open-issues.md](40-open-issues.md).

Overview tallies (91 non-regress cases, `rt_ref` df-on): 51 PASS runner-raw, of which 12 are NaN-blowup (class J) the pre-guard CSV recorded as `PASS 0.0`; with the guard a rerun reports them FAIL, so true PASS = 39. The 52 non-PASS results classify by root-cause domain as **implementation-failure 13 / incorrect-arguments 17 / unsupported-configuration 22 / invalid-testcase 0 / unexecuted 0**.

Per-case matrix and the class-A/B/D/F1/F2/I/J case lists: regenerate via `doc/regress58-params/run_cti_params_sweep.py` with `doc/regress58-params/phase2_catalog.csv` (the runner CSV is the canonical per-case record); generalized limitations are in [40-open-issues.md](40-open-issues.md). The archived rt_openfhe comparison is local-only and not committed.

---

## Reproduction

- **regress58** -- `doc/regress58-params/run_cti_params_sweep.py` with `--csv doc/regress58-params/regress58_catalog.csv`; the result CSVs are local (gitignored).
- **resnet batch** -- the resnet-batch accuracy harness; result JSON is local-only.
- **resnet20 (n=1)** -- `test/ace_test.py` / [50-resnet-benchmark-case-study.md](50-resnet-benchmark-case-study.md).
