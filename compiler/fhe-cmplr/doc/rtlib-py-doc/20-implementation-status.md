# rtlib-py implementation: pipeline and provider internals

> Implementation and architecture of the rtlib-py Python compilation path (`fhe_cmplr -> ir2py -> .py -> rtlib_py.<provider>`). Audience: engineers maintaining or extending the compiler or its providers. `file:line` references are against the current code.
>
> Companion docs: user-facing compilation and `df` usage in [`10-new-backend-integration.md`](10-new-backend-integration.md); validation results in [`30-test-status.md`](30-test-status.md); generalized limitations in [`40-open-issues.md`](40-open-issues.md); the resnet real-CKKS case study in [`50-resnet-benchmark-case-study.md`](50-resnet-benchmark-case-study.md).

## One-line status

rtlib-py provides a stable CKKS IR -> Python emission path. Three providers share a frozen 39-name runtime contract, and the emitter (`ir2py`) emits only neutral calls from that contract, so a new provider is executable once it satisfies those 39 names. Generalized limitations are catalogued in [`40-open-issues.md`](40-open-issues.md).

## Architecture and pipeline

```
ONNX model -> fhe_cmplr (air-infra/nn-addon lowering, read-only sibling repos)
              |
              v  CKKS2PY_PASS (Disable POLY+POLY2C; activated by -CKKS2PY:rt=)
          ir2py  -- include/fhe/ckks/ir2py_handler.h   (IR2PY_HANDLER: per-opcode 1:1 mirror of IR2C_HANDLER)
                   |- include/fhe/ckks/ir2py_core.h    (IR2PY_CORE: Handle_st/Handle_stp/Handle_func_entry; array-cipher ZERO store)
                   |- include/fhe/ckks/ir2py_ctx.h     (IR2PY_CTX: Emit_* Python syntax)
                   \_- ckks/src/ckks2py_driver.cxx      (emit get_context_params/get_rt_data_info/get_*_scheme/need_bts)
              |
              v  emit .py:  `from rtlib_py.<provider> import *`   (-CKKS2PY:rt=<provider> baked in)
          rtlib-py -- rtlib-py/rtlib_py/common/driver.py  run()
              |- rt_ref     (plaintext reference: slot semantics + [hard]/[strict] discipline)
              |- rt_openfhe (real CKKS over openfhe-python 1.5.1, lazy level bridge + droplevel)
              \_- rt_fhert   (real CKKS over fhert OpenFHE 1.4.2 + CModelDevice)
              |
              v
          e2e -- test/rtlib_py/py_verify/sweep_onnx_models.py  (catalog dedup + tier)
                \_- doc/regress58-params/run_cti_params_sweep.py  (df dual-state sweep)
                -> onnxruntime golden comparison -> result CSV
```

The four components:
1. **ir2py emission** (compiler pass `CKKS2PY`): when the `-CKKS2PY` pass is active (`-CKKS2PY:rt=` given) it suppresses `POLY/POLY2C`; `ir2py_handler.h` is an opcode-for-opcode 1:1 mirror of `IR2C_HANDLER` -- the foundation that lets the Python runtime mirror the C runtime.
2. **rtlib-py runtime**: pure-Python package with a frozen 39-name contract; the emitted `.py` binds via `import *` and `common/driver.run()` executes it.
3. **Three providers**: `rt_ref` (plaintext) / `rt_openfhe` (real CKKS 1.5.1) / `rt_fhert` (real CKKS 1.4.2 + device), all sharing the same 39-name contract.
4. **onnx e2e testing**: a catalog sweep runs each case with an onnxruntime golden comparison and records PASS/noisy/MISMATCH plus resource and constraint state. Results are in [`30-test-status.md`](30-test-status.md).

## Common layer (contract, driver, registry, timing, conf)

`rtlib-py/rtlib_py/common/` holds the shared runtime layer every provider uses.

### `contract.py` -- frozen 39-name runtime contract
- **`PUBLIC_API` = 36 functional calls** (`:33-58`): IO(2) + add/sub(6) + mul(4) + unary/shape(3) + modulus management(6, including `bootstrap_ciph`) + lifecycle(5) + encode/query(6, including `load_data_entry`) + observability(4: `validate`/`tm_start`/`tm_taken`/`dump_cipher_msg`).
- **`PUBLIC_TYPES` = 3 types** (`:61`): `Cipher`/`Plain`/`FakeRtError`.
- **`CONTRACT_NAMES = PUBLIC_API + PUBLIC_TYPES`** = 39 names; at registration `check_provider` (`:104-112`) `hasattr`-checks each one.
- `RuntimeProvider` (`:67-101`) is a `typing.Protocol` (`runtime_checkable`) -- a documentary record; what is enforced is the names, not the signatures. `Main_graph`/`get_context_params`/`get_*_count`/`get_*_scheme` are not in the contract (the emitted module defines them, mirroring C weak symbols).
- **Frozen contract rule** (`:1-9`): adding a capability requires simultaneously extending the emitter + `PUBLIC_API` + every provider's `__all__`. If any provider lacks a new name, `check_provider` raises `AttributeError` on its first fetch.

### `provider.py` -- registry + lazy loading
- `ALL_PROVIDERS = ("rt_ref", "rt_openfhe", "rt_fhert")` (`:32`). The key equals the rtlib_py submodule name, `CKKS2PY_CONFIG::_py_rt_module`, and the `-CKKS2PY:rt=` string; all three must agree.
- `get_provider(name)` (`:39-52`): `importlib.import_module(f"rtlib_py.{name}")` + first-time `check_provider` + cache. Heavy native bindings are imported only inside each `_seed_ctx`, so listing or registering a provider never crashes a host lacking the binding.
- `register(name, module)` (`:55-59`): explicit registration for an out-of-tree provider (e.g. injecting a fake in tests).
- **Backend axis** (`:23-27`): `-CKKS2PY:rt=` selects the Python provider and activates the pass; the `get_context_params()['provider']` tag is derived from `rt` (a runtime context cache-key salt). See [`10-new-backend-integration.md`](10-new-backend-integration.md#choosing-a-runtime-provider).

### `driver.py` -- execution entry `run()`
`run(py_file, spec, provider="rt_ref")` (`:158-272`) mirrors C `Run_main_graph`/`Prepare_input`/`Handle_output`:
1. `get_provider(provider)` -> `prov._reset()` + `prov.configure(strict=...)` (`:162-165`);
2. `exec(compile(src, ...))` runs the module top level (`:174-177`) -- the emitted `import *` binds the 39 contract names and the module defines its own `Main_graph`/`get_context_params`;
3. `_seed_ctx` (`:180-187`) + `_seed_data_info` (`:193-199`, seeds the offline `df` data-file descriptor into the provider);
4. read `get_encode_scheme`/`get_input_count` (`:204-214`) for the input packing layout -> `_seed_inputs(prov, spec, schemes)` (`:78-155`) packs per cipher by scheme `desc[k]` (start/count/stride/pad) and zero-pads to `n_slots`; an absent scheme falls back to contiguous slicing;
5. `Main_graph()` (`:222-231`), catching `FakeRtError` (`[hard]` violation) and other exceptions;
6. output comparison (`:233-261`): `_slots_close` (`:29-75`) computes per-slot `max_abs`, truncates to golden length, **rejects non-finite values** (NaN/inf actual -- `max_abs` reported as `inf` -> FAIL), rejects an empty `expected` (vacuous pass, `:239-244`), and fails on non-empty `_VIOLATIONS`.

`_seed_inputs` uses the compiler-emitted `DATA_SCHEME` for multi-cipher input packing (`get_encode_scheme`: per-cipher `desc{start,count,stride,pad}`), mirroring ant's `Prepare_input`/`Pre_encode_scheme` (`rtlib/ant/ckks/src/rtlib.c:67-110,164-192`).

**`_slots_close` non-finite guard (canonical mechanism).** The guard rejects NaN/inf actual values and reports `max_abs` as `inf` rather than a silent PASS. Without it, `abs(nan - expected) = nan` and `nan > eps` is always `False` (NaN comparison semantics), so a non-finite output leaves `max_abs` stuck at `0.0` and the comparison returns PASS. The guard (`math.isfinite`) restores correct non-finite FAIL semantics. It is shared by all providers via `common/driver.run`. The validation evidence and the limitation it surfaces are in [`30-test-status.md`](30-test-status.md#note-on-nan-blowup-cases-and-the-comparison-guard) and [`40-open-issues.md`](40-open-issues.md#known-compiler-defects).

### `timing.py` -- `:rtt` timing-macro mirror
`-POLY:rtt`/`:rtt` makes the emitter emit `CMPLR_TM_START(id,s)`/`CMPLR_TM_END(id,s)`.
- `CMPLR_TM_START/END` (`:23-32`) -> `Mark_rtm_start/end` (`:52-70`): `time.perf_counter_ns` + `Rtm_stack` push/pop.
- `Init_rtlib_timing`/`Append_rtlib_timing`/`Report_rtlib_timing` (`:46-81`) mirror rtlib-c `rtlib_timing.h`.
- All three providers import `CMPLR_TM_START`/`CMPLR_TM_END` from `common/timing` and list them in `__all__`, so a `:rtt`-emitted module runs timing under any provider. A new provider just imports and lists them the same way -- it does not implement timing itself.

### `conf.py` -- `.conf` parity (placeholder)
`parse_conf`/`conf_to_ctx_params` (`:38-65`) parse the C-side `.conf` JSON. rt_ref does not consume `.conf` -- it reads the `get_context_params()` dict. This file exists to record the key-mismatch gotcha (`:10-28`): `.conf` uses `_provider`(int)/`_poly_degree`/...; the dict uses `provider`(str)/`poly_degree`/.... It is not wired in (the `df` offline path reads the file path from `get_rt_data_info().get("file")`, not from `.conf`).

## How `df` externalizes static data (implementation)

`df` is an emit-side mechanism on the `-CKKS2PY` Python path that mirrors the non-ant C `df` path. For user-facing usage see [`10-new-backend-integration.md`](10-new-backend-integration.md#externalizing-static-data-with-the-df-argument).

When `-CKKS2PY:df=<path>` is given, the base `ckks::IR2C_CTX` constructor (`include/fhe/ckks/ir2c_ctx.h:36-43`) opens an `RT_DATA_WRITER` on `<path>` with entry type `DE_MSG_F32` (the `-CKKS2PY` group does not expose `_ct_encode`, so the `DE_PLAINTEXT` variant is never selected here). During body emission, `IR2PY_HANDLER::Handle_encode` first calls `Try_emit_offline_encode` (`ir2py_handler.h:469`), which externalizes **FLOAT_32 array constants** reached by one of two forms:
- a **whole-array LDC encode** (ARRAY-LDC arm): appends the constant's float32 bytes and emits `encode_float(load_data_entry(<idx>, n), n, sc, lv)`;
- a **per-slot OPC_SLICE encode** (SLICE arm, mirroring the live C slice branch `ckks/ir2c_ctx.h:180-248`): a slice with `loop_cnt` iterations becomes `loop_cnt` entries (`cst_<id>_<i>`), emitted as `encode_float(load_data_entry(<start_expr> + base_idx, count), count, sc, lv)`.

Each append writes one indexed LUT entry through `RT_DATA_WRITER::Append` (`include/fhe/core/rt_data_writer.h`); the constant id is recorded via `IR2PY_CTX::Mark_offlined_cst`. `IR2PY_CTX::Emit_global_constants` then skips any array constant in `_offlined_cst` (its bytes now live in the file) or not in `_used_cst` (dead, self-tracked by `IR2PY_CORE::Handle_ldc`/`Handle_ldca` via `Mark_used_cst`, because air-infra's `_cst_used` is private and air-infra is read-only), so offlined arrays are not also emitted as `_cst_N` lists.

The driver (`ckks2py_driver.cxx:122`) emits `get_rt_data_info()` returning `{'file': <data_file>, 'uuid': <placeholder>, 'entry_type': 'DE_MSG_F32'}` (or `None` when `df` is omitted), and writes the same data-file path into the `.conf` for the program entry via `lower_ctx.Emit_conf`. At run time, `rt_ref`/`rt_openfhe`'s `load_data_entry(idx, n)` reads the file at the recorded `file` path, parses the binary `rt_data_def.h` layout once (LUT cached), and returns entry `idx`'s first `n` float32 slots, which `encode_float` then (reference-)encodes. The file format is: magic `!ANTFHE\0`, 4 KiB-padded `DATA_FILE_HDR`, 2^`_ent_align`-aligned float32 entries, trailing 32-byte `DATA_LUT_ENTRY` rows (`include/fhe/core/rt_data_def.h`).

Non-float32, mask, complex (`dcmplx`), non-LDC, and non-`INTCONST`-span encodes fall through `Try_emit_offline_encode` (return false) and stay embedded as `_cst_N` lists using the runtime encode branch -- matching the C path's handling of those cases.

> A stale inline comment near the ARRAY-LDC arm says the Python path "has no slice offline, so it leniently flattens". The live SLICE arm above it contradicts that comment; the lenient flatten applies only to the non-slice ARRAY-LDC case, and behavior is unaffected.

## Provider: `rt_ref` (plaintext reference)

Executes each op with pure-Python float slots, tracks each `Cipher`'s CKKS state `(level, scale_deg, is_cipher3)`, and asserts real-CKKS discipline. It is the golden reference for validating the other providers.

- **State machine** (`rt_ref/__init__.py:246-301`): `Cipher`/`Plain` support `__getitem__`/`__setitem__` (array-cipher store/load works on Python lists of Ciphers); `mul_ciph` doubles scale elementwise producing a cipher3 (needs relin before add/rotate); `zero_ciph()` (`:627-630`) is an unfixed-scale empty-slot zero, and `scale_deg==0` waives level/scale match.
- **`[hard]` vs `[strict]`** (`:15-27, 171-182`): `[hard]` (type error / level underflow / encode OOB / cipher3 misuse) always raises; `[strict]` (add/sub level+scale match, rescale scale_deg>1) raises in STRICT mode and records `_VIOLATIONS` in LENIENT.
- **`_check_level_match` is a no-op** (`:210-219`): ant's `Adjust_level` auto-aligns mismatched-level operands; rt_ref mirrors that, and the result level is the `min` (zero-cipher waiver).
- **level cap = `num_primes = mul_depth+1`** (`_ctx_num_primes` `:146-159`, `_encode` `:669-680`, `raise_mod` `:566-580`): not `q_part_num`, not the emitted `input_level` (which lowering occasionally fills wrong). This cap only loosens, never tightens.
- **`bootstrap_ciph(a, level, slot)`** (`:583-617`): plaintext near-identity -- message preserved, level reset to target, `scale_deg=1`, `is_cipher3` cleared. Mirrors the non-ant C path collapsing four bootstrap opcodes into one runtime name. (Real `EvalBootstrap` behind this name is implemented by `rt_openfhe`, not `rt_ref`; see [Provider: `rt_openfhe`](#provider-rt_openfhe-real-ckks-openfhe-python-151).)
- **`load_data_entry(idx, n)`** (`:774-841`): reads the C binary side-data file (`!ANTFHE` magic, float32-only), mirroring C `Pt_from_msg`; range-checks idx/offset; numpy zero-copy float32 view fast path (`:838-840`) with a `struct` fallback, and `_DATA_LUT` caches parsed LUTs (`:787-815`).
- **`encode_dcmplx` cap inconsistency**: `encode_dcmplx` (`:703-738`) uses the legacy `cap=_ctx_input_level()` instead of `_ctx_num_primes()`. Latent -- no e2e `dcmplx` workload triggers it. See [`40-open-issues.md`](40-open-issues.md#known-compiler-defects).

## Provider: `rt_openfhe` (real CKKS, openfhe-python 1.5.1)

Runs real CKKS ciphertexts on OpenFHE 1.5.1, mapping IR ops 1:1 to OpenFHE primitives while keeping rt_ref's `[strict]` discipline. The module docstring (`rt_openfhe/__init__.py:1-82`) locks the fidelity decisions.

### Lazy level bridge (key design -- structurally eliminates drift)
- **`Cipher.level` is a `@property`** (`:359-374`): `_ctx_num_primes() - ct.GetLevel()`, storing no tracked level. Each op builds its result via `Cipher._wrap(ct, scale_deg, is_cipher3, n_logical)` (`:376-390`), passing no level param.
- An earlier eager bridge stored a tracked level and pinned `level=lv` after bootstrap while the real cipher sat deeper; the subsequent `_align` then computed an unreachable upward drop -> `[hard] droplevel target out of reachable`. On-demand computation has no drift source.
- **`_ctx_num_primes()`** (`:192-211`) = `_CC.GetMultiplicativeDepth()+1` (the real built chain). In a plain leveled-SHE context it equals `mul_depth+1`; in a bootstrap context the real chain is longer, so using `num_primes` (not the emitted `input_level`) guarantees `level >= 0`.
- Both IR boundaries go through `_of_level(lv) = _ctx_num_primes() - lv` (`:470-484`): the `level()` query and the droplevel target for bootstrap/raise_mod. Internal ops (`_align`/`_drop_to_of_level`) stay in OpenFHE's own `GetLevel` units.

### droplevel + binop alignment
- **`_drop_to_of_level`** (`:487-546`): scale-preserving tower drop via `cc.Compress(ct, towersLeft, noiseScaleDeg=nsd)` with `nsd` passed explicitly. The binding default `nsd=1` would `ModReduce` first (changing scale), so passing explicitly skips that loop and runs only `LevelReduceInternalInPlace` (a pure tower drop, scale unchanged). C++ `LevelReduce` is not pybound, so `Compress` with explicit `nsd` is the only bound scale-preserving droplevel.
- **`_align_ct_for_binop`** (`:549-581`): mirrors ant's `Adjust_level`; before a binary cipher op it drops the higher-level (lower `GetLevel`) operand to the lower level so both share an OpenFHE level for `EvalAdd/EvalSub/EvalMultNoRelin`. Equal level is a no-op; a zero operand (`scale_deg==0`) short-circuits (`:834-837`).
- Call sites: `add_ciph:840`, `sub_ciph:878`, `mul_ciph:914` each align before the `Eval*`. `mod_switch` (`:1024-1047`) is a single-tower droplevel; downward `raise_mod` (`:1066-1133`) uses the same primitive; upward `raise_mod`/`bootstrap_ciph` (`:1136-1190`) uses `EvalBootstrap` + droplevel (requires `ctx['bootstrap']:True`).

### Real `EvalBootstrap` (gated by `ctx['bootstrap']:True`)
- `_build_context` (`:675-719`) dispatches: `ctx.get('bootstrap') or ctx.get('need_bts')` truthy -> `_build_bootstrap_context` (`:722-779`) builds a model-driven EvalBootstrap context (ring/moduli/depth read from the emitted `get_context_params`, falling back to toy Set-A values otherwise; FIXEDMANUAL).
- `bootstrap_ciph(a, level, slot)` (`:1136-1190`) requires `_BOOTSTRAP_READY`, runs `_CC.EvalBootstrap` plus a droplevel to `_of_level(level)` when the target is below the post-bootstrap position, and resets `scale_deg=1`, `is_cipher3=False`.
- The green-gate and regress corpus do not trigger real bootstrap: the emitter emits `'need_bts': True` only when it inserts `OPC_BOOTSTRAP` at a ReSBM (`ckks/src/ckks2py_driver.cxx:109-112`), and that corpus has no such deep CNN. With `need_bts=False`, `bootstrap_ciph` raises `[hard] no bootstrap context`. Real `EvalBootstrap` has only run through an isolated smoke gate (`test/rtlib_py/test_openfhe_bootstrap.sh` -> `test/rtlib_py/py_verify/test_openfhe_bootstrap.py`). The full resnet20-on-real-CKKS verification is the case study in [`50-resnet-benchmark-case-study.md`](50-resnet-benchmark-case-study.md#findings-and-analysis), and the generalized bootstrap depth/scaling constraints are in [`40-open-issues.md`](40-open-issues.md#known-compiler-defects).

### Other
- **`_SCALE_BIT_CAP=40`** (`:657`): a toy-safety cap on `scaling_factor_bit_num`. The emitter emits 56 (a simulation-period value); under a toy ring dimension a 56-bit scaling leaves only ~4 integer bits, so values >=16 wrap. Capping to 40 restores the integer+fraction balance. `first_prime` is untouched (each RNS prime is hard-capped at 60 bits).
- **lazy import** (`:675-688`): `openfhe` is imported inside `_build_context`, which reads `OPENFHE_PY_BUILD_DIR` (default `/workspace/openfhe-python/build`) and prepends `sys.path`. A host without the binding makes ctest skip (`SKIP_RETURN_CODE 77`).
- **context caching** (`:151-165, 660-672`): `_ctx_key` computes the key; `_build_context` runs only when params change; `_reset` does not clear `_CC/_PK/_SK`.

## Provider: `rt_fhert` (real CKKS, fhert OpenFHE 1.4.2 + device)

Near-verbatim port of rt_openfhe (the `rt_fhert/__init__.py:1-108` docstring lists the delta); fhert's CKKS surface is a faithful replica of openfhe-python.

### Minimal delta vs rt_openfhe (Phase A)
1. **Path injection** (`:685-694`): fhert is a path-based bazel source tree, not pip, so it injects `FHERT_ROOT` (default `/workspace/fhert`) and `import fhert.openfhe` -- not `openfhe`, not `OPENFHE_PY_BUILD_DIR`, otherwise it would load the wrong module.
2. **Bare-enum-value adaptation** (`:708,715`): fhert.openfhe re-exports bare enum values `FIXEDMANUAL`/`HEStd_NotSet` but not the `ScalingTechnique`/`SecurityLevel` enum classes, so four call-sites use `_of.FIXEDMANUAL`/`_of.HEStd_NotSet` (bare values). `KeySwitchTechnique.HYBRID`/`SecretKeyDist.UNIFORM_TERNARY` are class exports, so `_of.KeySwitchTechnique.HYBRID` is kept.
3. **The rest verbatim**: FIXEDMANUAL leveled-SHE mirror, droplevel with explicit `nsd`, `Cipher`/`Plain`, `[hard]`/`[strict]`, `encode_*`/`load_data_entry`/`validate`/`tm_*`/`dump_cipher_msg`, frozen `__all__`, `_SCALE_BIT_CAP=40`.

### Cipher `level` model (still eager, in-param)
rt_fhert's `Cipher._wrap(cls, ct, ir_level, scale_deg, is_cipher3, n_logical)` (`:385-396`) takes an `ir_level` in-param and `Cipher.level` is a stored field (`:346`); it mirrors an early eager version of rt_openfhe, not the lazy bridge (`_of_level = _ctx_input_level() - ir_level`, `:484-488`). A newly onboarded real-CKKS provider should prefer rt_openfhe's lazy bridge to avoid drift (see [`10-new-backend-integration.md`](10-new-backend-integration.md#choosing-a-runtime-provider)).

### Phase B device acceleration (CModelDevice, opt-in via `_CTX['device']`)
- `_device_mode()` (`:249-253`) dispatches on `_CTX['device']`. The host path (default) does not touch `_DEV`; device patches fall back to native on host ciphers.
- `_build_device_context` (`:776-835`): `CModelDevice(MPUH1Config(*hw))`, ring_dim floored to >= 4096 (a toy `poly_degree=16` would mis-index the cmodel grid), uploading CRT tables and eval keys.
- The device seams: encrypt (`_encrypt_values`, `:556-569`) uploads after host `Encrypt`; decrypt (`_decrypt_slots`, `:572-582`) downloads then `Decrypt`; plain ops (`_plain_for_op`, `:625-636`) upload the plain and let `EvalAddPlain`/`EvalMultPlain` auto-dispatch; droplevel (`_drop_to_of_level`, `:549-553`) falls back to host (device has no `Compress`); `Cipher.clone` (`:414-433`) round-trips via host because native `Clone()` strips `_device`.
- `_DEV` is module-level and long-lived (`:154-162`), built once per ring_dim and reused across runs.
- **Phase C deferred**: device `EvalBootstrap` is not implemented, so in device mode `bootstrap_ciph`/upward `raise_mod` raise `[hard]`.

## Emitter integration (compiler side)

- `ckks/src/ckks2py_pass.cxx` -- `CKKS2PY_PASS` registration; when the pass is active (`-CKKS2PY:rt=` given) it does `Disable POLY+POLY2C`.
- `include/fhe/ckks/ir2py_handler.h` -- `IR2PY_HANDLER`, the per-opcode 1:1 mirror of `IR2C_HANDLER` (the mirroring foundation), including the offline `df` arms (`Try_emit_offline_encode`) described above.
- `include/fhe/ckks/ir2py_core.h` -- `IR2PY_CORE`: `Handle_st`/`Handle_stp`/`Handle_func_entry`. `Handle_st` (`:245,260`) emits a ZERO store `[zero_ciph() for _ in range(N)]` for array-of-cipher.
- `include/fhe/ckks/ir2py_ctx.h` -- `IR2PY_CTX` Python `Emit_*` syntax, `Emit_global_constants` (use-prune over `_offlined_cst`/`_used_cst`), `Mark_offlined_cst`/`Mark_used_cst`.
- `ckks/src/ckks2py_driver.cxx` -- emits `get_context_params`/`get_rt_data_info`/`get_encode_scheme`/`get_decode_scheme`/`get_input_count`/`get_output_count`/`need_bts`; `CKKS2PY_CONFIG::_py_rt_module` <- `-CKKS2PY:rt=`.

The pass lives under `namespace fhe::ckks` (`CKKS2PY_PASS`/`CKKS2PY_DRIVER`/`CKKS2PY_CONFIG`; `IR2PY_CTX : public fhe::ckks::IR2C_CTX`; registered as `PASS_ID::CKKS2PY`, the 5th pass after `POLY2C`). `CKKS2PY_CONFIG` and `ckks2py_driver.cxx` keep two `fhe::poly` references by design -- the config base `fhe::poly::POLY2C_CONFIG` (via `#include "fhe/poly/poly2c_config.h"`) and the Flatten opcode definitions `fhe::poly::POLYNOMIAL_DID`/`fhe::poly::OPC_HW_MODADD`/`OPC_HW_MODSUB`/`OPC_HW_MODMUL`/`OPC_HW_ROTATE` (via `#include "fhe/poly/opcode.h"`) -- so the shared `core::IR2C_CTX` config slot (and thus the C path `POLY2C`) is untouched. Those `fhe::poly` references in the `ckks2py` sources are intentional, not stale. The user-facing `-CKKS2PY` option group (string `"CKKS2PY"`: `rt=`/`df=`/`fp`, where `fp` is a bare flag) and the pass order are unchanged. The activation gate fires on `-CKKS2PY:rt=` only: `-CKKS2PY:rt=<module>` is the sole activator of the Python emit pass (it selects the Python provider module and the derived `_provider` tag), and the `-CKKS2PY:lib=` sub-flag is removed -- it is rejected at parse time, not a soft-ignored option; activation, the Python module selection, and the derived `_provider` tag flow strictly through `rt=`. The disable-`POLY`+`POLY2C` suppression is unchanged. free-cipher insertion (`MFREE_PASS`) is gated on `-CKKS2PY:fp` and is result-neutral on every provider (`free_ciph`/`free_plain` are no-ops; the Python GC owns handles). The end-state design note is [`doc/ckks2py-flag-policy.md`](../ckks2py-flag-policy.md).

Onboarding a new rtlib-py provider usually does not touch these -- they already emit only the neutral 39-name calls.

## Build and run prerequisites

- **Pure-Python package**: rtlib-py has no CMake and no compile step. `pyproject.toml` exists but the tests and harness do not go through pip -- `run_py_exec.py` (`:51-55`) and `run_py_verify.py` (`:62-66`) bootstrap `<repo>/rtlib-py` onto `sys.path` (overridable with `RTLIB_PY_DIR`).
- **`fhe_cmplr` binary**: the path is set with the `FHE_CMPLR` env var or the runner `--bin` flag (no host-fixed default).
- **Native binding** (real-HE providers only):
  - rt_openfhe: set `OPENFHE_PY_BUILD_DIR` (default `/workspace/openfhe-python/build`) so `import openfhe` resolves.
  - rt_fhert: set `FHERT_ROOT` (default `/workspace/fhert`); use fhert's own venv and never load openfhe-python in the same process (`run_py_verify.py:384-394`).
- **`PYTHON_EXECUTABLE` divergence**: the C++ `std::system` python, the shell `$PY`, and the pytest interpreter must be the same binary, or import resolution diverges.
- **`.conf/.t` land in CWD, not next to `-o`**: for ad-hoc invocations use the `test/run_fhe_cmplr_tmp.sh` wrapper (which `cd`s into a tempdir); the formal harness (`run_py_verify._compile_one` / the runner) already manages `tempfile.mkdtemp` + `rmtree`.
- **onnx-models**: the `onnx-models/` directory (158 `.onnx` across 11 folders) is an OSS pull and is byte-identical to the upstream snapshot; treat it as stable dataset provenance.

## Code size

| Component | Files | Lines (approx) |
| --- | --- | --- |
| common (contract/provider/driver/conf/timing/__init__) | 6 | ~610 |
| rt_ref | 1 | ~990 |
| rt_openfhe | 1 | ~1500 |
| rt_fhert | 1 | ~1490 |
| test harness (run_py_verify/run_py_exec/sweep_onnx_models/run_cti_params_sweep + pytest) | ~10 | ~3000 |
| rtlib-py runtime total | 9 | ~4580 |