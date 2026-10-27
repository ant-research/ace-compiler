# local-test-rtpy: methodology

Running the `cti` `local_test` operator/model cases through the rtlib-py Python
backends (`rt_ref`, `rt_openfhe`, `rt_fhert`) and recording the results.

This is a separate flow from the C/GPU `local_test` regression (C path via
`run_test.sh`) and from the `regress58` corpus sweep. The cases and compile
parameters come from `local_test`; the execution backend is a rtlib-py Python
provider. The canonical Python-test method is documented in
[`../rtlib-py-doc/`](../rtlib-py-doc/) (`10-new-backend-integration.md`,
`30-test-status.md`, `50-resnet-benchmark-case-study.md`).

## Inputs (all from local_test, not from prior test artifacts)

- Cases: the 17 operator cases and 4 model cases shipped in
  `cti/local_test/{operator,model}/`. The ONNX are (re)generated with
  `tool/gen_onnx_and_main.sh` into a writable copy of local_test (the `cti`
  mount is read-only).
- Compile optsets: `cti/local_test/tool/suite.conf`. Each operator case uses its
  source model's optset (`lenet_n32k` for LeNet-derived, `resnet_n64k` for
  ResNet-20-derived); each model case uses its model profile (`lenet_base`,
  `resnet_base`). The `-P2C:df=` token in `resnet_base` is stripped: on the
  Python path `df` is driven by the sweep `--df-states`, not by `-P2C:df=`.
- Backend selection: `-CKKS2PY:rt=<provider>` activates the Python emit pass
  and selects the provider. Not provided by local_test (which is C-only); the
  python-path convention from `rtlib-py-doc/10` is followed.
- `df`: **on** for this whole run (user directive).
- Tolerance (max_abs vs the onnxruntime golden): `rt_ref` 1e-6,
  `rt_openfhe`/`rt_fhert` 1e-5 (the real-CKKS band). Not a local_test parameter
  (local_test `case_env` REL_ERROR/ABS_ERROR is C-path elementwise error, a
  different semantics); the python-path default from
  `doc/regress58-params/run_cti_params_sweep.py` is used.

## Driver

`doc/regress58-params/run_cti_params_sweep.py` is the runner (validated
end-to-end). It compiles with `-CKKS2PY:rt=<p> [-CKKS2PY:df=]`, executes the
emitted module under `rtlib_py.common.driver.run`, and compares the output to
an onnxruntime golden. A local sweep copy adds one hook (below); the harness
core is unchanged.

## CKKS-semantics golden (MaxPool handling) -- why and how

CKKS has no comparison operator, so `fhe_cmplr` **lowers an ONNX `MaxPool` node
to `AveragePool`** (window mean), measured and documented by local_test itself:
`cti/local_test/tool/onnx2c.py` states "fhe_cmplr computes MaxPool as
AveragePool (output == window mean exactly)", and `--strict-onnx-semantics`
toggles the expected-output semantics (`OFF` = CKKS, the default).

- An **unmodified** sweep computes the golden by running the original ONNX
  through onnxruntime, which executes the TRUE MaxPool (window max). The
  backend computes AvgPool (window mean, the compiler's lowering). Max != Mean
  -> MISMATCH. This is a **golden-semantics mismatch**, not a backend
  defect: the sweep used pure-ONNX semantics where local_test uses CKKS
  semantics.
- local_test's own default (`--strict-onnx-semantics` OFF) computes the golden
  by first rewriting MaxPool->AveragePool in the ONNX, then running onnxruntime
  -> so the golden is AvgPool and matches the compiler-lowered output. The
  local_test C path PASSES the MaxPool operator cases (`i6x28x28_k2p0s2`,
  `i16x10x10_k2p0s2`).

The local sweep copy wraps local_test's **own** function
`onnx2c.apply_ckks_semantics` and rewrites MaxPool->AveragePool **for the
golden only**. The compile still receives the **original** MaxPool ONNX, so
`fhe_cmplr`'s MaxPool->AvgPool lowering is actually exercised and tested (a
case passing at `max_abs` well under tolerance proves the backend correctly
executes the compiler-lowered AvgPool given a MaxPool input). This mirrors
local_test's default verdict exactly; it does NOT skip the lowering and does
NOT mask any backend computation -- it only aligns the golden to local_test's
CKKS semantics.

For full transparency, the pure-ONNX (strict) view is also reported where
relevant: under strict semantics the MaxPool operator cases are MISMATCH, the
real compiler limitation (MaxPool has no polynomial approximation and is
executed as AveragePool) -- the `rtlib-py-doc/30-test-status.md` class F2. This
is not hidden; it is the behavior the CKKS golden accounts for.

## Interpreter environment

A CPython 3.12 environment provides `onnxruntime`, `numpy`, `torch`, `optree`
(the fhert binding dep). Bindings are located via sys.path injection
(`OPENFHE_PY_BUILD_DIR`, `FHERT_ROOT/run_py_exec.py`). onnxsim availability
depends on the interpreter's PyPI mirrors; the two `*_x2` model generators that
call `onnxsim.simplify` require a working onnxsim. The `lenet.py` and
`resnet20_cifar10.py` generators do not use onnxsim and are unaffected.

## Device (cmodel) phase prerequisites

From [`../fhert-device-fix/`](../fhert-device-fix/) (`OPEN-ISSUES.md`):
- Cmodel cache: point `XDG_CACHE_HOME` at a writable dir holding the f817c6f3
  cmodel with `PerfSnapshot::lines` / `PerfLineBucket`; the default `~/.cache`
  is stale and fails every device kernel with `'struct PerfSnapshot' has no
  member named 'lines'`.
- Device env: `FHE_DEV_SCALING=fixed FHE_DEVICE=1 FHERT_ROOT=<the fhert source
  root> PYTHONWARNINGS=ignore`, a CPython 3.12 interpreter.
- Ring N: auto-select per model (do NOT force `FHE_DEVICE_RING`) -- each model
  keeps its emitted N rounded up to a standard cmodel grid (clean; forcing
  N=65536 re-lowers large-ms stride-2 rotations into MISMATCH). The
  `resnet20` full model needs `FHE_DEVICE_RING=65536 FHE_DEVICE_HW=256,256,1,8`
  + `FHE_BTS_SF=59 FHE_BTS_CORRECTION=13` + `sbm:mxbl=8`, and hits a documented
  device VRF/HBM ceiling (OPEN-ISSUES Issue 1) -- an open device issue, not a
  regression.

## Result terminology

`PASS` (max_abs < tol) / `NOISY` (< 1e-2) / `MISMATCH` (>= 1e-2) /
`COMPILE_FAIL` / `EXEC_ERROR` / `RESOURCE_FAIL_{UNROLL,MEM,TIMEOUT}` / `EXC`.
The non-finite guard in `common/driver._slots_close` rejects NaN/inf actuals (a
NaN-blowup is reported `EXEC_ERROR ... blew up to NaN/inf`, not a silent
`PASS 0.0`). The result enum above is the runner result schema.
