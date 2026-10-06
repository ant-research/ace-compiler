# rtlib-py limitations and open issues

> Single source of truth for **generalized compiler limitations** of the rtlib-py Python path. Validation evidence (which cases passed / failed / were untested, and the per-case matrix) is in [`30-test-status.md`](30-test-status.md); the implementation status and the `_slots_close` non-finite guard mechanism are in [`20-implementation-status.md`](20-implementation-status.md); the resnet real-CKKS benchmark case study is in [`50-resnet-benchmark-case-study.md`](50-resnet-benchmark-case-study.md).
> Testcase defects, incorrect test arguments, and unexecuted tests are validation facts recorded in [`30-test-status.md`](30-test-status.md) section appendix-other-non-regress-results -- they are **not** compiler limitations and are not duplicated here. Only an item that exposes an independently verified compiler limitation appears below.
>
> **Flag-policy end state:** `-CKKS2PY:rt=<module>` is the sole activator of the Python emit pass (selects the Python provider module and the derived `_provider` tag); the `-CKKS2PY:lib=` sub-flag is removed -- rejected at parse time, not a soft-ignored option -- so activation, the Python module selection, and the derived `_provider` tag flow strictly through `rt=`. The `-CKKS2PY:fp` bare flag inserts `free_ciph` calls (result-neutral on every provider). The end-state design note is [`doc/ckks2py-flag-policy.md`](../ckks2py-flag-policy.md).

## rtlib-py limitations and open issues

Classification legend:

- **Unsupported or unimplemented** -- a feature the path does not provide.
- **Known compiler defects** -- the implementation produces wrong behavior (crash / wrong output) under a valid configuration; root cause triaged or suspected.
- **Unsupported configuration (upstream, read-only)** -- a configuration the rtlib-py layer cannot support because the front-end lowering (air-infra / nn-addon) is read-only, or a provider-level limit not fixable in rtlib-py.
- **df-related** -- limits of the `-CKKS2PY:df=` static-data externalization.
- **Performance and scalability** -- throughput / volume the path is not designed to meet.
- **Driver and tooling coverage** -- gaps in the validation harness's observability.

Each item states the current limitation, affected scope, user-visible impact, workaround if any, and a link to supporting evidence in [`30-test-status.md`](30-test-status.md) or [`50-resnet-benchmark-case-study.md`](50-resnet-benchmark-case-study.md).

## Unsupported or unimplemented

**Multi-cipher output decode is not implemented (rtlib-py driver).** The rtlib-py driver output check reads only a single cipher at index 0 -- `prov._OUTPUTS.get((name, 0))` in `common/driver.py` -- and never invokes the emitted `get_decode_scheme` to stitch a multi-cipher output array. Additionally, the emitted `set_output_data("<var>", 0, ...)` keys the output under the returned accumulator variable's name (e.g. `VS_out_conv1`), not the onnx output name (`output`), so the driver's lookup for `('output', 0)` misses. The C-side `rtlib/ant/ckks/src/rtlib.c:Post_decode_scheme`/`Handle_output` implement `NORMAL`-kind chunk stitching and loop the scheme cipher count, and the emitter always emits `kind=NORMAL`, so the C path is a viable mirror for this rtlib-py gap.
- Scope: the 3 large-spatial sharded convs (`cnn/conv_64x64x56x3`, `cnn/conv_128x128x28x3`, `cnn/conv_256x256x14x3`) (class I).
- Impact: these cases fail at run time with `expected output 'output' not captured; got keys=...`.
- Workaround: none.
- Evidence: [`30-test-status.md`](30-test-status.md) section appendix-other-non-regress-results (class I); `rtlib_py/common/driver.py`; `rtlib/ant/ckks/src/rtlib.c` (`Post_decode_scheme`, `Handle_output`).

**`rt_fhert` device-mode `EvalBootstrap` is not implemented; the host-mode real-CKKS bootstrap is verified for resnet20.** `rt_fhert`'s `_build_bootstrap_context` is model-driven (it reads the emitted `get_context_params`, like `rt_openfhe`), and on the **host** path the deep real-CKKS bootstrap is verified end-to-end for `resnet20` (n=1) (`accuracy=1.0`; see [`30-test-status.md`](30-test-status.md) section relocation-validation). On the **device** path `EvalBootstrap` is not implemented: in device mode `bootstrap_ciph` / upward `raise_mod` raise `[hard]` (see [`20-implementation-status.md`](20-implementation-status.md) section provider-rt_fhert-real-ckks-fhert-openfhe-142--device). `rt_openfhe` (the primary real-CKKS path) is model-driven and its real `EvalBootstrap` is verified end-to-end for resnet20 (see [`50-resnet-benchmark-case-study.md`](50-resnet-benchmark-case-study.md)).
- Scope: **device-mode** real-model bootstrapping on `rt_fhert` (host-mode bootstrapping works and is verified for resnet20).
- Impact: deep nets needing bootstrap on the `rt_fhert` **device** path are not supported.
- Workaround: use `rt_fhert` host mode, or use `rt_openfhe` (see [rt_openfhe bootstrap depth/scaling constraints](#known-compiler-defects) for its env-gated requirements, and [`50-resnet-benchmark-case-study.md`](50-resnet-benchmark-case-study.md) for the verified resnet20 run).
- Evidence: `rtlib_py/rt_fhert/__init__.py` (`_build_bootstrap_context`, model-driven); [`30-test-status.md`](30-test-status.md) section relocation-validation (resnet20 host `accuracy=1.0`); [`20-implementation-status.md`](20-implementation-status.md) section provider-rt_fhert-real-ckks-fhert-openfhe-142--device (device-mode bootstrap raises `[hard]`); [`50-resnet-benchmark-case-study.md`](50-resnet-benchmark-case-study.md) (rt_openfhe resnet20). The regress/green-gate corpus inserts no ReSBM, a validation-coverage fact recorded in [`30-test-status.md`](30-test-status.md).

**Multiple `num_slots` is not supported.** `num_slots` (slot count) is fixed at Setup time on all three providers: `rt_openfhe` sets `_N_SLOTS = GetRingDimension() // 2` inside `_seed_ctx`/`_build_bootstrap_context`, `rt_fhert` sets it the same way in `_build_context`, and `rt_ref` derives a single count from `poly_degree` (`_default_slot_count`). Supporting more than one slot count would require a per-slot context plus key generation.
- Scope: any workload needing more than one slot count.
- Impact: such workloads cannot run.
- Workaround: none (per-slot context + keygen would be required).
- Evidence: `rtlib_py/rt_openfhe/__init__.py`, `rtlib_py/rt_fhert/__init__.py`, `rtlib_py/rt_ref/__init__.py`.

## Known compiler defects

**Deep CIFAR CNN numerical blowup under `rt_ref` (root cause unverified).** several deep CIFAR CNNs produce NaN/inf under the `rt_ref` plaintext provider, now reported as FAIL by the shared `_slots_close` non-finite guard (NaN/inf actual -> non-finite (NaN/inf) result). `resnet110` is one of them and its root cause is known separately -- degree-20 relu intermediate overflows float64 (see [`resnet110` `rt_ref` float64-overflow`](#unsupported-configuration-upstream-read-only) under Unsupported configuration). For the remaining cases the root cause is unverified: `rt_ref` is plaintext float64 with no rescaling, so a deep-chain rescale-budget hypothesis does not apply, and this is not confirmed as a compiler lowering defect. The blowup is param-sensitive -- a tuned per-relu degree map avoids it for `resnet20` -- suggesting float64 overflow of relu-polynomial intermediates (a provider-level precision limit) as a plausible cause.
- Scope: several cases (`cnn/resnet20_cifar10`, `ace/resnet{20,32_cifar10,32_cifar100,44,56,110}*`, `alinm/{squeezenet,mobilenet,vgg13,alexnet}`, `resbm/AlexNet`).
- Impact: these cases output NaN/inf (reported FAIL). The regress suite (all non-zero finite, no NaN/inf) is unaffected.
- Workaround: none verified broadly; tuned per-relu degree maps avoid the blowup for `resnet20`.
- Evidence: [`30-test-status.md`](30-test-status.md) section note-on-nan-blowup-cases-and-the-comparison-guard and section appendix-other-non-regress-results (class J); [`20-implementation-status.md`](20-implementation-status.md) section common-layer-contract-driver-registry-timing-conf (the guard mechanism).

**`encode_dcmplx` cap inconsistency in `rt_ref`.** `rt_ref`'s `encode_dcmplx` uses `cap=_ctx_input_level()`, inconsistent with `_encode` and `raise_mod` which use `cap=_ctx_num_primes()`. The inline comment preceding the cap asserts `num_primes == input_level` -- the flawed premise `_encode`/`raise_mod` were fixed to drop -- while the code uses `_ctx_input_level()`; the cap should be `_ctx_num_primes()` to match.
- Scope: `rt_ref` `encode_dcmplx` only.
- Impact: latent -- no e2e double-precision-complex workload exists, and real-valued ONNX does not emit `ENCODE_DCMPLX`, so current impact is zero.
- Workaround: none needed for current workloads.
- Evidence: `rtlib_py/rt_ref/__init__.py` (`encode_dcmplx`, `_encode`, `raise_mod`, `_ctx_num_primes`).

**`rt_openfhe` bootstrap depth/scaling constraints for deep nets.** The nsd-blindness problem (openfhe `EvalBootstrap` leaves `NoiseScaleDeg=2`) is fixed automatically in code: `bootstrap_ciph` rescales until `NoiseScaleDeg=1` after `EvalBootstrap` and tracks the real nsd (always on, not an open defect). The remaining limitation is that deep real-bootstrap nets under `rt_openfhe` require two environment variables because the default behavior leaves no bottom-bootstrap headroom and accumulates FIXEDMANUAL scaling drift, producing crash or garbage. `FHE_BTS_RESERVE` defaults to `0` (no extra R-tower headroom for `EvalBootstrap`'s downward level consumption at the cipher bottom); `FHE_BTS_SF` defaults to the emitted value, which under FIXEDMANUAL accumulates per-rescale drift `prod(delta / q_i)` that FIXEDMANUAL never compensates.
- Scope: deep nets using real `EvalBootstrap` on `rt_openfhe`.
- Impact: with defaults, resnet20 on `rt_openfhe` crashes or outputs garbage.
- Workaround (in-tree, env-gated, defaulting to green-gate parity): `FHE_BTS_RESERVE=8` (extra R-tower for headroom) + `FHE_BTS_SF=59` (openfhe 64-bit FIXEDMANUAL baseline, reducing per-rescale drift). With these, resnet20 image 0 is verified correct (within epsilon, pred matches label); the extended resnet batch was not rerun after the fix (some cells remain untested).
- Evidence: [`50-resnet-benchmark-case-study.md`](50-resnet-benchmark-case-study.md) section findings-and-analysis and section results (the root causes and fixes); [`30-test-status.md`](30-test-status.md) section resnet-batch-detail (15 untested batch cells).

## Unsupported configuration (upstream, read-only)

**air-infra / nn-addon front-end lowering limits.** Certain op/parameter combinations are blocked at compile time by assertions or TODOs in air-infra / nn-addon, both read-only sibling repos. A `k_pad` size-constraint TODO in nn-addon (`vector/src/tensor2vector_util.cxx`) hits 16 cases, and an air-infra rtype assertion (`base/src/node.cxx`, `CMPLR_ASSERT(0, "Do not call this API to change rtype.")`) hits 2.
- Scope: 18 cases block at compile time (16 from the `k_pad` TODO; 2 from the rtype assertion: `cnn/lenet_no_relu`, `cnn/resnet32_cifar10_train`).
- Impact: these cases fail at compile time (`COMPILE_FAIL`).
- Workaround: change parameters or the op subset to bypass the blocked combination (e.g. drop `dmo/cf/sss/mf` markers for the rtype assertion).
- Evidence: [`30-test-status.md`](30-test-status.md) section appendix-other-non-regress-results (class A, full case list).

**`max_pool` is lowered as `average_pool` (no polynomial approximation).** `max_pool` lowers identically to `average_pool` -- both `Handle_max_pool_rtv`/`_ref` and `Handle_average_pool_rtv` call only `Lower_cipher` in the SIHE front-end (`include/fhe/sihe/vector2sihe_impl.h`, part of the fhe-cmplr repo -- a lowering gap upstream of the rtlib-py runtime, not a read-only repo); no max-pool polynomial approximation is emitted.
- Scope: `base/max_pool` and any max-pool workload.
- Impact: max-pool output mismatches the onnxruntime golden (class F2).
- Workaround: none.
- Evidence: [`30-test-status.md`](30-test-status.md) section appendix-other-non-regress-results (class F2, unsupported-configuration); `include/fhe/sihe/vector2sihe_impl.h`.

**`resnet110` `rt_ref` float64-overflow.** Under `rt_ref` (plaintext float64, not changed for this), the resnet110 degree-20 relu intermediate exceeds the float64 representable range and overflows to NaN (acc=0.0). The degree-20 relu intermediate is derived analysis (corroborated in [`50-resnet-benchmark-case-study.md`](50-resnet-benchmark-case-study.md) section evaluation-scope -- the exact relu polynomial degree is not a literal field in the compile flags); the overflow itself is inferred from the acc=0.0 the batch JSON records -- its truncated output tail contains no literal overflow/nan text. This is the same model file (`ace/resnet110_cifar10_train`) that appears in the Deep CIFAR CNN blowup list under [Known compiler defects](#known-compiler-defects), where it is the known-root-cause case.
- Scope: resnet110 under `rt_ref` (2 batch cells).
- Impact: resnet110 produces NaN under `rt_ref`.
- Workaround: none in `rt_ref` (the C path tolerates a larger modulus and partially covers resnet110).
- Evidence: [`30-test-status.md`](30-test-status.md) section resnet-batch-detail; [`50-resnet-benchmark-case-study.md`](50-resnet-benchmark-case-study.md) section evaluation-scope; the resnet110_cifar10_train model under `rt_ref` (observation-only; no in-tree driver covers it in this PR -- the v2 Python accuracy driver is `test/run_py_accuracy.py`, which runs the cifar topo but is not bound to a resnet110 invocation here).

## df-related limitations

df externalizes static data (FLOAT_32 model-weight constants) into a binary `.data` file via `-CKKS2PY:df=<path>`. User-facing usage is in [`10-new-backend-integration.md`](10-new-backend-integration.md) section externalizing-static-data-with-the-df-argument; the externalization mechanism (including the `_used_cst` use-prune that drops dead arrays) is in [`20-implementation-status.md`](20-implementation-status.md) section how-df-externalizes-static-data-implementation.

**df externalization is FLOAT_32-only.** Only FLOAT_32 array constants are externalized -- both the whole-array LDC arm and the per-slot SLICE arm of `Try_emit_offline_encode` require FLOAT_32 element type. Mask encodes, complex (`dcmplx`) encodes, non-float32 element types, non-LDC, and non-INTCONST-span encodes fall through to the runtime branch and remain embedded as Python list constants in the emitted `.py`.
- Scope: non-float32 / mask / dcmplx / non-LDC weight encodes.
- Impact: df does not shrink the `.py` for those. No correctness impact (the embedded constants are still correct, matching the C path's handling of the same cases).
- Evidence: [`30-test-status.md`](30-test-status.md) (df on/off gate: 414 runs, zero NameError, df-on no worse than df-off); `include/fhe/ckks/ir2py_handler.h` (`Try_emit_offline_encode`).

## Performance and scalability

**Single-image real-CKKS-bootstrap inference is throughput-bound.** Real-bootstrap inference on `rt_openfhe` is not designed for throughput-oriented use; a single deep-bootstrap image incurs high per-image latency.
- Scope: real-bootstrap inference on real-CKKS providers.
- Impact: not suitable for throughput-oriented use.
- Workaround: none (throughput is not the current goal).
- Evidence: [`50-resnet-benchmark-case-study.md`](50-resnet-benchmark-case-study.md) section evaluation-scope (the resnet20 single-image measurement and its single-image/uncorroborated-log caveats).

## Driver and tooling coverage

**Runner scripts truncate exception detail (`driver.run` does not).** The runner scripts truncate recorded exception detail -- `run_cti_params_sweep.py` to 200 characters in three places (COMPILE_FAIL, run-detail, CSV-write), and `run_py_verify.py` to 160 characters in one XFAIL branch. `common/driver.py:run` returns the full `traceback.format_exc()` and does not truncate.
- Scope: cases whose recorded result/detail is read from the runner CSV rather than directly from `driver.run`.
- Impact: truncated detail in the runner CSV hinders diagnosis of `Main_graph` tracebacks.
- Workaround: widen the truncation, re-run `driver.run` directly, or capture the full traceback to a file.
- Evidence: `test/rtlib_py/py_verify/run_cti_params_sweep.py`; `test/rtlib_py/py_verify/run_py_verify.py`; `rtlib_py/common/driver.py`.
