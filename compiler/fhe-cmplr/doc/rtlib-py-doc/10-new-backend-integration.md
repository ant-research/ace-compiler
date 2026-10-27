# rtlib-py user guide: compiling, running, and adding providers

> How to compile an ONNX model to the rtlib-py Python path with `fhe_cmplr -CKKS2PY`, run it on a runtime provider, externalize static data with `-CKKS2PY:df`, choose a provider, and -- for backend integrators -- add a new runtime provider.
>
> Part I is the user guide; Part II (Adding a new runtime provider) is for integrators onboarding a new backend. Implementation internals are in [`20-implementation-status.md`](20-implementation-status.md); validation in [`30-test-status.md`](30-test-status.md); limitations in [`40-open-issues.md`](40-open-issues.md).

## What rtlib-py provides

rtlib-py is a CKKS IR -> Python emission path. `fhe_cmplr -CKKS2PY` lowers an ONNX model through air-infra/nn-addon and an `ir2py` emit pass that produces a `.py` file. The emitted module does `from rtlib_py.<provider> import *` and runs on a pure-Python runtime whose providers all share a frozen 39-name contract.

The problem it solves: swappable Python execution for CKKS programs. A plaintext provider (`rt_ref`) gives a fast reference for validation, and real-CKKS providers (`rt_openfhe`, `rt_fhert`) execute actual FHE ciphertexts without touching the C runtime. The frozen contract means the emitted code is provider-neutral -- switching providers only changes the emitted first-line `import`.

## Where it fits in the pipeline

```
ONNX -> fhe_cmplr (air-infra/nn-addon lowering)
        -> ir2py emission pass  (-CKKS2PY:rt= activates it and sets the provider)
        -> out.py  (first line: `from rtlib_py.<provider> import *`)
        -> rtlib_py.<provider>  (run via common/driver.run())
```

The shared CKKS IR lowering is the same one the C path (`-P2C`) uses; the two paths have independent runtimes. Implementation internals and the emitter integration points are in [`20-implementation-status.md`](20-implementation-status.md#architecture-and-pipeline).

## Prerequisites

- **`fhe_cmplr` binary** -- the compiler binary; set its path with the `FHE_CMPLR` env var (the runner also accepts a `--bin` flag). No host-fixed default is recorded here. Used to compile ONNX to a `.py`.
- **rtlib-py package** -- pure Python, no build step. The test harness bootstraps `rtlib-py/` onto `sys.path` automatically; set `RTLIB_PY_DIR` only to point elsewhere.
- **A native binding** -- only required for real-CKKS providers. `rt_openfhe` needs `OPENFHE_PY_BUILD_DIR` (default `/workspace/openfhe-python/build`, so `import openfhe` resolves). `rt_fhert` needs `FHERT_ROOT` (default `/workspace/fhert`). The plaintext `rt_ref` needs no binding.
- **onnx-models** -- the `onnx-models/` test corpus (an OSS pull, byte-identical to the upstream snapshot).

Build notes and the `PYTHON_EXECUTABLE` divergence footgun are in [`20-implementation-status.md`](20-implementation-status.md#build-and-run-prerequisites).

## Compiling and running a model

Compile with `-o` naming the emitted Python file and a `-CKKS2PY:` group selecting the backend; then run the emitted module.

```bash
fhe_cmplr onnx-models/op/add.onnx -o out.py \
    -CKKS2PY:rt=rt_openfhe                # -CKKS2PY:rt= alone activates the pass; lib is removed at parse time
python3 test/rtlib_py/py_verify/run_py_exec.py out.py @add1
```

Important `-CKKS2PY` group members:

| Argument | Meaning |
| --- | --- |
| `-CKKS2PY:rt=<name>` | The Python provider module baked into the emitted `from rtlib_py.<name> import *` (`rt_ref`, `rt_openfhe`, or `rt_fhert`). Activates the pass; the emitted `'provider'` tag is derived from it (`rt_openfhe`/`rt_fhert` -> `LIB_OPENFHE`, `rt_ref` -> `rt_ref`). |
| `-CKKS2PY:df=<path>` | Externalize static data into a binary data file (see [Externalizing static data with the `df` argument](#externalizing-static-data-with-the-df-argument)). |
| `-POLY:rtt` | Emit timing calls into the `.py`. |
| `-o <out>.py` | Emitted Python module path. |

The compile-time `-CKKS2PY:rt=` value and the execution-time provider must agree. The execution-time provider is the `provider=` argument to `rtlib_py.common.driver.run(...)` (or the `--rt` / inferred-first-line value used by the harness). A mismatch makes `Main_graph` bind one provider's names while the driver seeds another's state, producing incoherent behavior.

Expected artifacts (one compile run):

- the Python module `<out>.py`;
- a `.conf` JSON next to it (pointer-only on the Python path; the runtime reads the emitted `get_context_params()`/`get_rt_data_info()` dicts, not the `.conf`);
- when `-CKKS2PY:df=` is set, the binary data file at the given path (see below);
- `.t` scratch files land in the current working directory, not next to `-o`. For ad-hoc invocations use the `test/run_fhe_cmplr_tmp.sh` wrapper (which `cd`s into a tempdir); the formal harness manages this itself.

## Externalizing static data with the `df` argument

Static data is the model's plaintext float32 weight constants (ONNX initializers) that, by default, the compiler embeds directly in the emitted `.py` as Python list literals (`_cst_N`). A large model produces a large `.py` -- historically multi-gigabyte for some convnets, in cases exceeding CPython's source-line parser limit. The `df` argument externalizes this static data into a separate binary data file and the emitted program reads it back by index.

| Fact | Detail |
| --- | --- |
| **CLI syntax** | `-CKKS2PY:df=<data_file>` -- a `key=value` member of the `-CKKS2PY` group. |
| **Argument form** | `<data_file>` is a path-valued string -- the literal filesystem path of the binary data file. There is no flag or boolean form; it must be an `=`-value. |
| **Default when omitted** | Static data is embedded inline as `_cst_N` lists in the `.py`, and the emitted `get_rt_data_info()` returns `None`. **Note:** `-CKKS2PY:df=` alone has no effect because the pass stays inactive until `-CKKS2PY:rt=` is given. |
| **Data-file location and naming** | The compiler writes the file to the exact path given (no extension, prefix, or auto-derivation); the test-harness convention is `weights.dat` placed beside the emitted `.py`. |
| **One file** | All externalized constants are appended into the one named file as a single lookup table (LUT); the generated code references each by integer index, so there is one `.dat` per emitted module, not one per constant. |
| **How generated code uses it** | The emitted module defines `get_rt_data_info()` returning `{'file': <data_file>, 'uuid': <placeholder>, 'entry_type': 'DE_MSG_F32'}` (or `None` without `df`). The runtime driver seeds this into the provider, and each externalized encode becomes `encode_float(load_data_entry(idx, n), n, sc, lv)` -- `load_data_entry(idx, n)` reads entry `idx` and returns its first `n` float32 message slots, which `encode_float` then encodes. |
| **Runtime requirements** | The `.dat` must be present at the `file` path at run time (resolved relative to the runner's current working directory -- moving the file or running from another directory makes `load_data_entry` raise). The format is the compiler's binary `rt_data_def.h` format (`!ANTFHE` magic, header, float32 entries, LUT), shared with the C path. The Python offline reader is `DE_MSG_F32` / float32-only. |
| **Expected artifacts** | With `df`: a compressed `.py` (data referenced by index) plus the binary `.dat`. Without `df`: just the `.py` with embedded `_cst_N` lists. The `.conf` also records the data-file path, but that field is parity-only -- the runtime reads `get_rt_data_info()`, not the `.conf`. |

Minimal example (the form used by the test harness):

```bash
fhe_cmplr onnx-models/op/mult_const.onnx -o out.py \
    -CKKS2PY:rt=rt_openfhe -CKKS2PY:df=weights.dat
```

This produces `out.py` (which calls `load_data_entry`) and `weights.dat` (the externalized weights), both beside each other so the relative path resolves at run time.

User-visible limitations:

- Only **FLOAT_32 array constants** reached by a whole-array load or a per-slot slice are externalized. **Mask, complex (`dcmplx`), non-float32, and other encode forms stay embedded** as `_cst_N` lists, so `df` does not shrink the `.py` for those.
- `df` only takes effect on the `-CKKS2PY` (Python) backend **and only when the pass is active** (`-CKKS2PY:rt=` given).
- The recorded `uuid` is a fixed placeholder, not a genuine model identifier -- do not rely on it for model-file matching.
- The `.dat` must be shipped alongside the `.py`.

For the externalization mechanism in the pipeline see [`20-implementation-status.md`](20-implementation-status.md#how-df-externalizes-static-data-implementation); for validation of the `df` on/off gate see [`30-test-status.md`](30-test-status.md#validation-scope-and-result-terminology); for generalized `df` limitations see [`40-open-issues.md`](40-open-issues.md#df-related-limitations).

## Choosing a runtime provider

| provider | CLI | Device | Backend | Bootstrap |
| --- | --- | --- | --- | --- |
| **rt_ref** | `rt_ref` | CPU | plaintext (pure Python float) | plaintext near-identity |
| **rt_openfhe** | `rt_openfhe` | CPU | real CKKS, openfhe-python 1.5.1 | real `EvalBootstrap`, isolated use only (see below) |
| **rt_fhert** | `rt_fhert` | CPU + device (CModelDevice) | real CKKS, fhert OpenFHE 1.4.2 | real `EvalBootstrap` on host; device bootstrap deferred |

Use `rt_ref` for fast plaintext validation. Use `rt_openfhe` or `rt_fhert` to execute real CKKS ciphertexts; both require their native binding on `sys.path`.

The compiler-side `-CKKS2PY:` option selects the rtlib-py runtime; there is no separate backend axis now:
- `-CKKS2PY:rt=<name>` is the sole activator of the Python emit pass and selects the **Python provider module** (the emitted `from rtlib_py.<name> import *` and the execution-time provider); `rt=` alone is enough to emit Python. The emitted `get_context_params()['provider']` tag is derived from `rt` (read at runtime only as a context cache-key salt, not a Python module).
- The `-CKKS2PY:lib=` sub-flag is removed -- it is rejected at parse time, not a soft-ignored option -- so activation flows strictly through `rt=`. The end-state design note is [`doc/ckks2py-flag-policy.md`](../ckks2py-flag-policy.md).

The provider design -- including the lazy level bridge that makes `rt_openfhe` structurally immune to post-bootstrap level drift -- is in [`20-implementation-status.md`](20-implementation-status.md#provider-rt_openfhe-real-ckks-openfhe-python-151). A newly onboarded real-CKKS provider should prefer rt_openfhe's lazy bridge over rt_fhert's eager tracked-level model.

## Limitations to be aware of

Pointers only; the catalog is [`40-open-issues.md`](40-open-issues.md):
- Multi-cipher output decode is not implemented -- sharded conv outputs are not stitched back ([40](40-open-issues.md#unsupported-or-unimplemented)).
- Deep CIFAR CNNs can produce NaN/inf under `rt_ref` ([40](40-open-issues.md#known-compiler-defects)).
- Real `EvalBootstrap` is not exercised in the green-gate/regress corpus (only an isolated smoke gate plus the resnet20 verification; see [50](50-resnet-benchmark-case-study.md#findings-and-analysis)).
- `df` externalizes float32 array constants only; mask/dcmplx/other encodes stay embedded ([40](40-open-issues.md#df-related-limitations)).
- `resnet110` overflows float64 under `rt_ref` (degree-20 relu intermediate); rt_ref is not changed ([40](40-open-issues.md#unsupported-configuration-upstream-read-only)).

---

## Adding a new runtime provider (onboarding guide)

Part II. Adding a provider to rtlib-py means adding **one Python subpackage `rtlib_py/<name>/__init__.py`** that satisfies the frozen 39-name contract, plus **one line** added to the `ALL_PROVIDERS` registry in `common/provider.py`. The emitter already emits only neutral contract calls, so you usually do not touch the emitter, CMake, or the C-layer rtlib. This is a different layer from C-layer rtlib, which covers onboarding a vendor library to the C-layer rtlib via `-P2C:lib=`. See [Choosing a runtime provider](#choosing-a-runtime-provider).

## Provider module skeleton and the frozen 39-name contract

Create `rtlib-py/rtlib_py/rt_<name>/__init__.py`. Minimal skeleton:

```python
"""`rt_<name>` -- <one-line description of backend + deltas vs rt_ref/rt_openfhe>.

Document any native-binding deltas here.
"""
import struct as _struct                # C-binary side-data parsing (load_data_entry)
from rtlib_py.common.timing import CMPLR_TM_START, CMPLR_TM_END

_STRICT = True
_VIOLATIONS = []; _CTX = {}; _INPUTS = {}; _OUTPUTS = {}
_DEFAULT_SLOTS = 2; _DATA_INFO = None; _DATA_FILE_CACHE = None

class FakeRtError(Exception):
    """A violated [hard] or (in strict mode) [strict] CKKS constraint."""

class Cipher: ...
class Plain: ...

# ... 36 contract calls + driver-glue hooks ...

__all__ = [ ... 39 contract names + CMPLR_TM_START + CMPLR_TM_END ... ]
```

> **Naming constraint:** the provider submodule name, the CLI `-CKKS2PY:rt=` value, and the `ALL_PROVIDERS` entry must all agree (`provider.py` uses `importlib.import_module(f"rtlib_py.{name}")`, and the emitted `from rtlib_py.{name} import *` both depend on it).

The contract is **36 functional calls** + **3 types** = **39 names**, plus two re-exported timing names. At registration `check_provider` asserts all 39 are present. Copy `rt_ref`'s `__all__` verbatim -- all providers' `__all__` are byte-identical.

| Category | Names | C counterpart |
| --- | --- | --- |
| Types | `Cipher`, `Plain`, `FakeRtError` | CIPHER/PLAIN + error class |
| IO | `get_input_data`, `set_output_data` | Get_input_data / Set_output_data |
| Arithmetic add/sub | `add_ciph`, `add_plain`, `add_scalar`, `sub_ciph`, `sub_plain`, `sub_scalar` | Add_ciph/Add_plain/... |
| Arithmetic mul | `mul_ciph`, `mul_plain`, `mul_scalar`, `mul_mono_ciph` | Mul_ciph/Mul_plain/... |
| unary/shape | `neg_ciph`, `rotate_ciph`, `conjugate_ciph` | Neg/Rotate/Conjugate |
| modulus mgmt | `relin`, `rescale_ciph`, `mod_switch`, `upscale_ciph`, `raise_mod`, `bootstrap_ciph` | Relin/Rescale/Mod_switch/Raise_mod/Bootstrap |
| lifecycle | `copy_ciph`, `zero_ciph`, `free_ciph`, `free_plain`, `free_ciph_poly` | Copy/Zero/Free |
| encode/query | `encode_float`, `encode_double`, `encode_dcmplx`, `load_data_entry`, `level`, `sc_degree` | Encode_float/.../Pt_from_msg/Level/Sc_degree |
| observability | `validate`, `tm_start`, `tm_taken`, `dump_cipher_msg` | Validate/Tm_*/Dump_cipher_msg |
| timing (shared `common/timing.py`) | `CMPLR_TM_START`, `CMPLR_TM_END` | rtlib-c CMPLR_TM_* macros |

Python has no weak symbols or C `&dst` out-params, so C's `Add_ciph(CIPHER res, CIPHER a, CIPHER b)` (writes `res`) becomes `add_ciph(a, b) -> Cipher` (returns a new cipher). The emitter already emits in this return-value style; implement accordingly. The `RuntimeProvider` Protocol records intent only -- **enforced are the names, not the signatures.**

> **Frozen-contract extension rule:** adding a capability requires simultaneously extending the emitter (one `IR2PY_HANDLER` call) and `PUBLIC_API`/`CONTRACT_NAMES`, and adding the name to every provider's `__all__`. If any provider lacks it, `check_provider` raises `AttributeError` on first fetch.

## Driver-glue hooks and lazy imports

The driver accesses these leading-underscore hooks as **plain module attributes** on the provider -- they are deliberately not in `__all__` (so the emitted namespace stays clean) but must be defined at module top level. Copy them from `rt_ref`/`rt_openfhe`:

| Hook | Purpose |
| --- | --- |
| `configure(strict=True, on_violation="raise")` | set strictness (`_STRICT`) |
| `_reset()` | clear per-run state (`_VIOLATIONS`/`_INPUTS`/`_OUTPUTS`/`_DATA_INFO`/`_DATA_FILE_CACHE`) |
| `_seed_input(name, idx, cipher)` | driver feeds an input cipher into `_INPUTS[(name,idx)]` |
| `_seed_ctx(ctx_params)` | feed `_CTX` from `get_context_params()` (real HE lazily builds context here, see below) |
| `_seed_data_info(info)` | feed the `df` data-file descriptor from `get_rt_data_info()` |
| `_INPUTS` / `_OUTPUTS` / `_VIOLATIONS` / `_CTX` | per-run stores (dict / list) |
| `_ctx_input_level()` / `_ctx_num_primes()` / `_ctx_q_part_num()` / `_ctx_mul_depth()` / `_default_slot_count()` | accessors reading `_CTX` parameters |

The driver also calls two private functions, `_seed_inputs(prov, spec, schemes)` and `_slots_close(actual, expected, epsilon)`, implemented in `common/driver.py` -- the provider does not need to write them; they invoke the hooks above via the `prov` instance.

**Lazy-import pattern for a heavyweight native binding.** A real-CKKS provider must never import its binding at module top level -- otherwise any `import rtlib_py.<name>` (or listing it in `ALL_PROVIDERS`) would crash on a host lacking the binding. Import it inside `_build_context(ctx)` (called by `_seed_ctx` when params change) and locate the binding on `sys.path` via an env override (rt_openfhe reads `OPENFHE_PY_BUILD_DIR`; rt_fhert reads `FHERT_ROOT`; your provider should read a corresponding `NEWCKKS_BUILD_DIR`/`NEWCKKS_ROOT`). This way a missing binding makes tests skip (ctest `SKIP_RETURN_CODE 77`) rather than fail. `_seed_ctx` should also cache context: building a real-HE context is expensive, so only rebuild when params change, and `_reset()` must not clear `_CC/_PK/_SK` (reused across runs).

## Registration and validation

Add the provider name to `ALL_PROVIDERS`:

```python
ALL_PROVIDERS = ("rt_ref", "rt_openfhe", "rt_fhert", "rt_<name>")  # <- add this entry
```

`get_provider("rt_<name>")` then does `importlib.import_module("rtlib_py.rt_<name>")` + first-time `check_provider`.

**No compiler-side allowlist is needed** by default. The emitter writes `-CKKS2PY:rt=<anything>` verbatim into the emitted `from rtlib_py.<anything> import *`, which does no allowlist check at runtime -- only that `rtlib_py.<anything>` is importable. An out-of-tree provider can be registered explicitly with `provider.register(name, module)`.

Validate a new provider in four steps:
1. **Contract self-check** -- `from rtlib_py.common.provider import get_provider; get_provider("rt_<name>")` (auto-runs `check_provider`; a missing name raises `AttributeError`).
2. **driver-glue self-check** -- run a known small model (e.g. `op/add`) through `rtlib_py.common.driver.run`; it exercises `_reset`/`_seed_ctx`/`_seed_inputs`/`Main_graph`/`_OUTPUTS`.
3. **pytest** -- pin each op's plaintext semantics + level/scale state machine + strictness against `rt_ref`; run with `test/rtlib_py/py_verify/run_pytest.sh`.
4. **e2e sweep** -- run the runner (`doc/regress58-params/run_cti_params_sweep.py`) over the regress 58 catalog with `--providers rt_<name> --df-states off,on` for an onnxruntime-golden comparison.

## Full provider template and known pitfalls

For a minimal plaintext backend, copy `rt_ref/__init__.py` and change three places: the module docstring; delete the native-binding lazy import (a plaintext reference needs none); and any backend-specific op semantics. For a real-CKKS backend, copy `rt_openfhe/__init__.py` (or `rt_fhert`) and adapt the binding path, enum handling, and bootstrap implementation -- `rt_fhert` is a useful example of "near-verbatim port of rt_openfhe + minimal delta".

Load-bearing pitfalls:
- **`__all__` must contain all 39 names verbatim plus the two timing names.** All providers' `__all__` are byte-identical -- copy `rt_ref`'s.
- **Driver-glue names are not renamable.** `driver.py` accesses `prov._reset`/`prov._seed_ctx`/`prov._INPUTS`/... as attributes; renaming one raises `AttributeError`. They are deliberately not in `__all__`.
- **`-CKKS2PY:rt=` must match the execution-time provider.** A mismatch makes the emitted module bind one provider's names while the driver seeds another's state.
- **Native bindings must be lazy-imported** (only inside `_build_context`/`_seed_ctx`), so a missing binding skips rather than crashes.
- **Unify the level map on one base.** `level(a)`, the encode/`raise_mod` upper bound, and the droplevel target must all use the same base. rt_openfhe uses `_ctx_num_primes() = GetMultiplicativeDepth()+1` (the real built chain, not the emitted `input_level`, which the lazy bridge relies on to keep `level >= 0`). Follow the lazy bridge unless there is a strong reason otherwise (see [Choosing a runtime provider](#choosing-a-runtime-provider)).
- **Real-HE binary ops need level alignment.** Real CKKS cannot `EvalAdd`/`EvalSub`/`EvalMultNoRelin` across OpenFHE levels; rt_openfhe mirrors ant's `Adjust_level` by dropping the higher-level operand before each binop. A plaintext provider has no such constraint.
- **`load_data_entry` is provider-agnostic.** Copy rt_ref's implementation verbatim (it parses the C-binary side-data file, float32-only). Do not rewrite it.
- **fhert bare-enum trap.** `fhert.openfhe` re-exports bare enum values (`FIXEDMANUAL`/`HEStd_NotSet`) but not the enum classes -- use `_of.FIXEDMANUAL`, not `_of.ScalingTechnique.FIXEDMANUAL`. Other enums (`KeySwitchTechnique.HYBRID`) are class exports. Probe a new binding first.

## Paths and background

- rtlib-py is the `rtlib-py/` directory at the repo root -- a pure-Python package with no build step. The harness bootstraps it onto `sys.path` directly.
- rtlib-py and the C-layer `rtlib/` are two independent runtimes: the `.py` from `ir2py` goes through rtlib-py; the `.c` from `-P2C` goes through C rtlib. They share one CKKS IR lowering (air-infra/nn-addon + the 1:1 mirror of ir2py/ir2c handlers), but each runtime implements itself.
- Emitter integration points and the ir2py mechanism are listed in [`20-implementation-status.md`](20-implementation-status.md#emitter-integration-compiler-side). Onboarding a new rtlib-py provider usually does not touch them -- they already emit only the neutral 39-name calls.