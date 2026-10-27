# OpenFHE C-side bootstrap integration plan

## 1. Goal and scope

Add a working bootstrap to the C-side `rt_openfhe` backend so that
`rtlib/openfhe` can run bootstrap-requiring models (deep CNNs such as
`resnet20_cifar10`) end to end, mirroring the bootstrap that already exists in
`rtlib-py/rtlib_py/rt_openfhe`.

Today the C-side OpenFHE backend implements only levelled-SHE primitives
(add, mul, rotate, rescale, mod-switch, relin). It exposes no Bootstrap at all:
`include/rt_openfhe/openfhe_api.h` has no `Openfhe_bootstrap` / `EvalBootstrap`
wiring, and `openfhe/src/openfhe_lib.cxx` builds a plain `CryptoContext` with
only `SetMultiplicativeDepth(_mul_depth)` and `Enable(PKE | KEYSWITCH |
LEVELEDSHE)`. A generated graph that needs bootstrap calls `Bootstrap(res, op,
level)` unconditionally (for example `resnet20_cifar10_pre.c:1835`), so on the
current backend a bootstrap-requiring model either fails to link or runs out of
modulus budget and produces garbage.

This is a single, non-gated work item. It has a natural implementation order
(enable the build, then verify the plain path, then add bootstrap, then test),
but the pieces below are not separate approval phases.

Scope covered by this plan:

- enable the build of `libFHErt_openfhe.a`;
- add the bootstrap capability to the runtime and the one compiler-side hook it
  depends on;
- a focused bootstrap-correctness test;
- the `resnet20_cifar10` end-to-end test;
- a full functional and correctness sweep over `regress58` (operator path, no
  bootstrap) plus `resnet20` (model path, with bootstrap).

Out of scope: multi-`num_slots` support (full packing only, as in rtlib-py),
GPU backends, and the SEAL/Phantom/ACE backends.

## 2. Reference: what rtlib-py rt_openfhe already does

The Python provider at `rtlib-py/rtlib_py/rt_openfhe/__init__.py` is the
behavioral reference. Its bootstrap path (`_build_bootstrap_context`,
`bootstrap_ciph`) is what this C-side work reproduces:

- Two context modes, selected by a `bootstrap`/`need_bts` flag:
  - 3a plain levelled-SHE (current C-side behavior).
  - 3b model-driven `EvalBootstrap` context.
- Bootstrap context build (`_build_bootstrap_context`, lines 781-889):
  - `SetSecretKeyDist(UNIFORM_TERNARY)`;
  - `SetScalingTechnique(FIXEDMANUAL)` (decision 2), `SetKeySwitchTechnique(HYBRID)`;
  - ring, first-modulus, scaling-modulus come from the emitted
    `get_context_params`; scaling-modulus is overridable via `FHE_BTS_SF`
    (defaults to the emitted `sf`; `resnet20` sets `FHE_BTS_SF=59` to lift
    bootstrap refresh precision past the degree-20 Chebyshev ReLU blowup
    threshold);
  - `level_budget = [4,4]` (overridable via `FHE_BTS_LEVEL_BUDGET`);
  - `bts_depth = FHECKKSRNS.GetBootstrapDepth(level_budget, secretKeyDist)`;
  - chain depth is a minimal sizing, not `levels_after = mul_depth`:
    - `c_bts_d = 15` if hamming weight is in `(0, 192]` else `19`;
    - `max_target = max(0, (mul_depth + 1) - c_bts_d)` (deepest bootstrap
      target in IR levels);
    - `ir_capacity = max(input_level, max_target + bts_depth + 1)`;
    - `levels_after = ir_capacity - bts_depth - 1`;
    - `R = max(FHE_BTS_RESERVE, 2)` (down-headroom reserve, 2-tower floor + 1
      margin; the OpenFHE non-StCFirst path ModRaises before consuming so it
      does not need `lvlb_dec` towers below the input);
    - `depth = levels_after + bts_depth + R`; `SetMultiplicativeDepth(depth)`;
  - `Enable(PKE | KEYSWITCH | LEVELEDSHE | ADVANCEDSHE | FHE)`;
  - `cc.EvalBootstrapSetup(level_budget)`, `KeyGen`, `EvalMultKeyGen`,
    `EvalBootstrapKeyGen(secretKey, num_slots)`, and the model rotation keys via
    `EvalRotateKeyGen(secretKey, rot_idxs)` (bootstrap keygen does not cover
    the model's arbitrary rotations).
- `bootstrap_ciph(a, level, slot)` (lines 1251-1327):
  - `ct = cc.EvalBootstrap(a._ct)` (refresh near top; OpenFHE has no per-call
    target level, and the post-bootstrap `noiseScaleDeg` is usually 2);
  - ant-faithful nsd reset: `while GetNoiseScaleDeg() > 1: ct = Rescale(ct)`
    (without this the real nsd drifts up across the emitted
    rescale-after-every-mul schedule and OpenFHE `SetLevel` eventually throws
    "multiplicative depth insufficient", the historical `resnet20` crash);
  - droplevel to the IR target `level`: `target_gl = num_primes - level`
    (`_of_level`), drop `(target_gl - post_gl)` towers when the target is at or
    below the post-reset position (scale-preserving);
  - result: `scale_deg = 1`, the cipher is not in 3-element form, level computed on demand.
- Underlying library: OpenFHE 1.5.1, already installed at `/usr/local`
  (`OpenFHEConfig.cmake`, `libOPENFHEpke.so` etc.).

## 3. Current C-side state and gaps

- `rtlib/CMakeLists.txt` enables the OpenFHE subdirectory only when
  `RTLIB_ENABLE_OPENFHE` is on and `RTLIB_OPENFHE_PATH` (or the OpenMP path) is
  set. The existing build directory was configured without that env var, so
  no `openfhe` dir was built. `FHE_ENABLE_OPENFHE` is already `ON` in
  `CMakeCache.txt`.
- `rtlib/openfhe/CMakeLists.txt` already does `find_package(OpenFHE QUIET)` and
  builds `FHErt_openfhe` (static), plus `ut_fhert_openfhe` and example apps.
- `include/rt_openfhe/openfhe_api.h` declares only levelled-SHE entries; no
  `Need_bts`, no `Openfhe_bootstrap`.
- `include/rt_openfhe/rt_openfhe.h` has inline `Rescale_ciph` but no
  `Bootstrap`.
- `openfhe/src/openfhe_lib.cxx` `OPENFHE_CONTEXT` builds one plain context; no
  bootstrap keygen, no `EvalBootstrap`.
- Compiler side: `include/fhe/poly/ir2c_ctx.h:Emit_need_bts()` (lines 119-132)
  emits `bool Need_bts()` only for `SEAL`, `PHANTOM`, `ACE`. It does not emit
  it for `OPENFHE`, so the C-side OpenFHE backend has no generated
  `Need_bts()` to branch on.
- Generated graphs call the 3-argument `Bootstrap(CIPHER res, CIPHER op, int
  level)` uniformly (`resnet20`, `resnet110`, `relu1` all emit this form); the
  emitted `level` is the IR target the bootstrapped cipher must land at.

## 4. Objects to modify

| File | Object | Change |
|---|---|---|
| `rtlib/include/rt_openfhe/openfhe_api.h` | C API declarations | Add `bool Need_bts();` and `void Openfhe_bootstrap(CIPHER res, CIPHER op, int level);` inside the `extern "C"` block, beside the other evaluation entries. |
| `rtlib/include/rt_openfhe/rt_openfhe.h` | inline header API | Add `inline CIPHER Bootstrap(CIPHER res, CIPHER op, int level)` that calls `Openfhe_bootstrap(res, op, level)` and returns `res`, matching the `Bootstrap` shape in `rt_ant/rt_ant.h` and `rt_seal/rt_seal.h`. Add a 4-arg `Bootstrap(CIPHER res, CIPHER op, int level, int slots)` overload that forwards to the 3-arg form (the `slots` argument is informational: num_slots is fixed at setup) only if a later emitted graph uses it. |
| `rtlib/openfhe/src/openfhe_lib.cxx` | `OPENFHE_CONTEXT` | 1) Store a bootstrap-ready flag and the bootstrap sizing constants. 2) Split the constructor into a `Need_bts() == false` branch (current plain context, unchanged) and a `Need_bts() == true` branch that builds the bootstrap context from Section 5. 3) Add a `Bootstrap(const Ciphertext* op, Ciphertext* res, int level)` method (Section 5). 4) Add the `Openfhe_bootstrap` C entry. |
| `include/fhe/poly/ir2c_ctx.h` | `Emit_need_bts()` | Add `core::PROVIDER::OPENFHE` to the provider condition so `bool Need_bts() { return <true|false>; }` is emitted for OpenFHE targets. |
| `rtlib/openfhe/unittest/ut_openfhe_bootstrap.cxx` (new) | gtest | Bootstrap correctness test (Section 6). |
| `rtlib/openfhe/example/eg_rtopenfhe_boot.cxx` + `eg_rtopenfhe_boot.inc` (new) | example app | Standalone "bootstrap runs and refreshes levels" smoke, modeled on `rtlib/phantom/example/eg_rtphantom_boot.inc` (which defines `bool Need_bts() { return true; }`). |
| `cti/local_test/tool/detect_env.sh`, `run_test.sh`, `fhe_build_openfhe.sh` (new) | harness | Add `--lib openfhe` support so `regress58` and `resnet20` run against the C OpenFHE backend (Section 8). These live in the read-only `cti` mount, so apply them in the writable copy described in Section 8. |

Files not changed: the rtlib-py Python provider (reference, not the target),
the other backends, and the compiler bootstrap inserter
(`ckks/include/smo_bootstrap_inserter.h`) which already emits `Bootstrap` calls
for the OpenFHE/lib path.

## 5. Build enabling and new-feature flow

### 5.1 Enable the OpenFHE backend build

The build only needs the OpenFHE prefix so the rtlib gate resolves and
`find_package(OpenFHE)` succeeds:

```bash
RTLIB_OPENFHE_PATH=/usr/local \
cmake -S <fhe-cmplr source tree> -B <build dir> \
  -DFHE_WITH_SRC="air-infra;nn-addon" \
  -DCMAKE_BUILD_TYPE=Release -DFHE_CODE_CHECK=OFF \
  -DFHE_ENABLE_OPENFHE=ON
cmake --build <build dir> --target FHErt_openfhe -- -j$(nproc)
```

Expected artifact: `<build dir>/rtlib/build/openfhe/libFHErt_openfhe.a`
(built by `rtlib/openfhe/CMakeLists.txt:28-35`). Incremental reconfigure
re-uses the already-fetched `uthash`/`blake2` (FetchContent disconnected), per
the existing build setup.

Do this first, then confirm the plain-levelled-SHE path still builds and the
existing examples (`eg_rtopenfhe_add`, `eg_rtopenfhe_mult_const`,
`eg_rtopenfhe_conv2d_keep_shape`) run, before adding bootstrap. This isolates
bootstrap changes from build-enabling changes.

### 5.2 Context constructor flow

Pseudocode for the constructor branches (mirrors
`_build_bootstrap_context`):

```c++
// OPENFHE_CONTEXT::OPENFHE_CONTEXT()
CKKS_PARAMS* p = Get_context_params();
bool need_bts = Need_bts();   // emitted by fhe-cmplr after Section 4 change

CCParams<CryptoContextCKKSRNS> parms;
parms.SetRingDim(p->_poly_degree);
parms.SetFirstModSize(p->_first_mod_size);
parms.SetBatchSize(p->_poly_degree / 2);   // full packing, num_slots = N/2
// security: mirror the existing branch (HEStd_128_classic at N=65536 is fine;
//           the toy bootstrap unit test may pass HEStd_NotSet to avoid an
//           auto ring raise; see Section 8 risks)

if (!need_bts) {
  // 3a plain levelled-SHE (current behavior, unchanged).
  parms.SetScalingModSize(p->_scaling_mod_size);
  parms.SetMultiplicativeDepth(p->_mul_depth);
  _ctx = GenCryptoContext(parms);
  _ctx->Enable({PKE, KEYSWITCH, LEVELEDSHE});
  _kp = _ctx->KeyGen();
  _ctx->EvalMultKeyGen(_kp.secretKey);
  _ctx->EvalRotateKeyGen(_kp.secretKey, {p->_rot_idxs, p->_rot_idxs + p->_num_rot_idx});
  _bootstrap_ready = false;
  return;
}

// 3b bootstrap context (port of _build_bootstrap_context).
parms.SetSecretKeyDist(UNIFORM_TERNARY);
parms.SetScalingTechnique(FIXEDMANUAL);
parms.SetKeySwitchTechnique(HYBRID);
size_t sf = env_or("FHE_BTS_SF", p->_scaling_mod_size);   // resnet20: set FHE_BTS_SF=59
parms.SetScalingModSize(sf);

auto level_budget = parse_level_budget(env_or("FHE_BTS_LEVEL_BUDGET", "4,4")); // s2c,c2s
uint32_t bts_depth = FHECKKSRNS::GetBootstrapDepth(level_budget, UNIFORM_TERNARY);

uint32_t c_bts_d   = (p->_hamming_weight > 0 && p->_hamming_weight <= 192) ? 15 : 19;
uint32_t max_target = std::max<uint32_t>(0, uint32_t(p->_mul_depth) + 1 - c_bts_d);
uint32_t in_lev     = uint32_t(p->_input_level);
uint32_t ir_capacity = std::max(in_lev, max_target + bts_depth + 1);
uint32_t levels_after = ir_capacity - bts_depth - 1;
uint32_t R = std::max(env_or_uint("FHE_BTS_RESERVE", 2), 2u);
uint32_t depth = levels_after + bts_depth + R;
parms.SetMultiplicativeDepth(depth);

_ctx = GenCryptoContext(parms);
_ctx->Enable({PKE, KEYSWITCH, LEVELEDSHE, ADVANCEDSHE, FHE});
size_t num_slots = _ctx->GetRingDimension() / 2;
_ctx->EvalBootstrapSetup(level_budget);
_kp = _ctx->KeyGen();
_ctx->EvalMultKeyGen(_kp.secretKey);
_ctx->EvalBootstrapKeyGen(_kp.secretKey, num_slots);
_ctx->EvalRotateKeyGen(_kp.secretKey, {p->_rot_idxs, p->_rot_idxs + p->_num_rot_idx});
_bootstrap_ready = true;
_num_primes = _ctx->GetMultiplicativeDepth() + 1;   // for target-level math
```

Rationale carried over from rtlib-py: the minimal `ir_capacity` sizing (not
`levels_after = mul_depth`) keeps the modulus chain from doubling `Q`, and the
`R = 2` reserve gives the deepest IR-1 cipher three towers (one above the
non-StCFirst 2-tower floor). `HEStd_NotSet` is acceptable for the toy unit test
and matches rtlib-py; the real `resnet20` uses `N = 65536` which satisfies
128-bit security, so the existing `SetSecurityLevel` switch can stay.

### 5.3 Bootstrap operation flow

`Bootstrap(const Ciphertext* op, Ciphertext* res, int level)` ports
`bootstrap_ciph`:

```c++
void OPENFHE_CONTEXT::Bootstrap(const Ciphertext* op, Ciphertext* res, int level) {
  IS_TRUE(_bootstrap_ready, "Openfhe_bootstrap: no bootstrap context");

  Ciphertext out = _ctx->EvalBootstrap(*op);

  // ant-faithful nsd reset: EvalBootstrap leaves real noiseScaleDeg == 2; force 1
  // so the emitted rescale-after-every-mul schedule cannot drift up.
  while (out->GetNoiseScaleDeg() > 1) out = _ctx->Rescale(out);

  // droplevel to the IR target (scale-preserving tower drop).
  size_t post_gl   = out->GetLevel();
  size_t target_gl = _num_primes - (size_t)level;   // _of_level(level)
  if (target_gl >= post_gl) {
    size_t drop = target_gl - post_gl;
    // OpenFHE 1.5.1 LevelReduce drops `drop` towers, scale-preserving.
    // For drop == 1 this matches the existing Mod_switch (LevelReduceInPlace(res, nullptr)).
    _ctx->LevelReduceInPlace(out, nullptr, drop);
  }
  *res = out;
}
```

C entry (declared `extern "C"` in `openfhe_api.h`):

```c++
void Openfhe_bootstrap(CIPHER res, CIPHER op, int level) {
  OPENFHE_CONTEXT::Context()->Bootstrap(op, res, level);
}
```

Two notes on the droplevel:
- OpenFHE 1.5.1 `LevelReduceInPlace(cipher, nullptr, levels)` drops `levels`
  towers. If the installed overload only takes a `psi` to match (no `levels`
  count), fall back to a one-tower loop mirroring the existing `Mod_switch`
  (`LevelReduceInPlace(*res, nullptr)`), `drop` times. Confirm the exact
  overload against the installed `OpenFHEConfig` headers before finalizing.
- The C++ backend, unlike the python binding, exposes `LevelReduce` directly,
  so the scale-preserving droplevel is a real primitive here (rtlib-py had to
  emulate it via `Compress` with an explicit nsd).

### 5.4 Compiler-side hook

In `include/fhe/poly/ir2c_ctx.h`, extend the provider check in `Emit_need_bts`
to include OpenFHE:

```cpp
if (this->Provider() == core::PROVIDER::SEAL ||
    this->Provider() == core::PROVIDER::PHANTOM ||
    this->Provider() == core::PROVIDER::ACE ||
    this->Provider() == core::PROVIDER::OPENFHE) {
  // emit bool Need_bts() { return <true|false>; }
}
```

No other compiler change is needed: the bootstrap inserter already emits
`Bootstrap(res, op, level)` for the OpenFHE/lib path, and the provider
selection (`-P2C:lib=openfhe` -> `LIB_OPENFHE` -> the `rt_openfhe/rt_openfhe.h`
include and `_provider = LIB_OPENFHE`) already exists in
`include/fhe/core/lib_provider.h`.

## 6. Simple bootstrap correctness test

Purpose: prove the bootstrap path is wired and numerically correct on a tiny
case, before the expensive `resnet20` run.

### 6.1 Unit test

Add `rtlib/openfhe/unittest/ut_openfhe_bootstrap.cxx` (gtest, picked up by the
existing `FHERT_OPENFHE_UNITTEST_SRC_FILES` glob in
`rtlib/openfhe/CMakeLists.txt:47`). The test builds a small bootstrap context
directly (it does not depend on a generated graph): ring `N = 4096` (slots
2048) or `N = 8192`, `mul_depth` a few levels, and provides a local
`bool Need_bts() { return true; }` so the constructor takes the 3b branch.

Test flow:

1. Build the context (`OPENFHE_CONTEXT::Init_context()` with a `CKKS_PARAMS`
   whose `_provider == LIB_OPENFHE`, `_hamming_weight == 192`, and a small
   `_poly_degree`). The test sets `FHE_BTS_SF` to the rtlib-py toy default if
   needed.
2. Encode and encrypt a known vector, for example `v[i] = 0.001 * (i - 1024)`
   so values stay well within the bootstrap approximation range.
3. Deepen the cipher: a few `EvalMultNoRelin` + `Rescale` pairs (or
   `LevelReduce`) to consume several modulus levels, recording the pre-bootstrap
   `GetLevel()` / `levelsAvailable`.
4. Call `Openfhe_bootstrap(res, op, target_level)` with a target near the
   input level (the unit test only needs "refresh happened", so any reachable
   non-top target is fine).
5. Decrypt and compare to `v` with the recovery error.
6. Assertions:
   - the post-bootstrap cipher has strictly more modulus levels available than
     the deepened pre-bootstrap cipher (refresh happened);
   - the recovery error is below the bootstrap tolerance, `1e-2` as a conservative gate
     (rtlib-py's toy bootstrap smoke reported `7e-5` at ring `2^12`).

### 6.2 Example smoke

Add `rtlib/openfhe/example/eg_rtopenfhe_boot.cxx` and the shared
`eg_rtopenfhe_boot.inc` (model the `.inc` on
`rtlib/phantom/example/eg_rtphantom_boot.inc`, including the local
`bool Need_bts() { return true; }` declaration). This builds as an example app
via the `FHERT_OPENFHE_EXAMPLE_SRC_FILES` glob. It runs the same encrypt,
deeper, bootstrap, decrypt, and print loop, and exits non-zero on a tolerance
violation. This is the smallest "bootstrap runs at all" check.

### 6.3 Build and run

```bash
cmake --build <build dir> --target ut_fhert_openfhe fhert_openfhe_example -- -j$(nproc)
<build dir>/rtlib/build/openfhe/ut_fhert_openfhe            # gtest
<build dir>/rtlib/build/openfhe/eg_rtopenfhe_boot           # example smoke
```

Pass criterion: both succeed, the unit test's recovery-error assertion holds, and
the example prints a refreshed level count and a sub-tolerance error.

## 7. resnet20 test

Purpose: prove the bootstrap path is correct under a real, many-bootstrap deep
CNN, where drift and depth budget matter.

### 7.1 Model and parameters

Use `resnet20_cifar10` with the established real-CKKS option set from
`test/ace_util.py`:

- base: `-CKKS:hw=192:q0=60:sf=56:sbm`;
- ReLU value-range control: the `CGO25_RESNET20_CIFAR10_RELU_OPT` map
  (`-SIHE:relu_vr_def=3:relu_vr=<per-node map>`); without this `resnet20` is
  NaN/garbage on real CKKS regardless of backend;
- target the OpenFHE C backend: `-P2C:lib=openfhe`;
- env `FHE_BTS_SF=59` (lifts FIXEDMANUAL bootstrap refresh precision, as in
  rtlib-py);
- run a single image (`n=1`); expected result `acc = 1.0`, prediction `3`
  (the cti driver's `Expected_data` label), matching the known rt_ref /
  rtlib-py rt_openfhe references.

The emitted `get_context_params` for this model is:
`_poly_degree = 65536`, `_mul_depth = 31`, `_input_level = 29`,
`_hamming_weight = 192`, `_first_mod_size = 60`, `_provider = LIB_OPENFHE`.
Plugging into Section 5.2: `bts_depth` from `[4,4]` (~15), `c_bts_d = 15`,
`max_target = max(0, 32 - 15) = 17`, `ir_capacity = max(29, 17 + 15 + 1) =
33`, `levels_after = 17`, `R = 2`, `depth = 34`. This matches the rtlib-py
chain sizing that already ran `resnet20` to `acc = 1.0`.

### 7.2 Primary path: local_test

Run via the harness once Section 8 is in place:

```bash
cp -r <cti mount>/local_test <a local temp dir for test byproducts>      # read-only cti fallback
cd <the temp dir> && ./tool/detect_env.sh --root <fhe-cmplr source tree> --force
./tool/run_test.sh m_resnet20 --lib openfhe                 # resnet20 model case
# or the explicit optset if the model case is named differently in the suite.conf
```

### 7.3 Manual fallback path

If the harness extension is not ready, compile and run directly, mirroring the
existing manual-fallback artifacts:

```bash
<build dir>/driver/fhe_cmplr resnet20_cifar10_pre.onnx -o cpath_r20_of.c \
  -P2C:lib=openfhe -CKKS:hw=192:q0=60:sf=56:sbm \
  -SIHE:relu_vr_def=3:relu_vr=<CGO25 map>
g++ -std=c++17 -DRTLIB_SUPPORT_LINUX \
  -I<fhe-cmplr source tree>/rtlib/include -I<fhe-cmplr source tree>/rtlib/ant/include \
  -I<build dir>/rtlib/build/_deps/uthash-src/src \
  -I<air-infra>/include -I<fhe-cmplr source tree>/include -I<nn-addon>/include \
  -I/usr/local/include/openfhe \
  <cti mount>/cti/verify/user_main/cnn/resnet20_cifar10.c cpath_r20_of.c \
  <build dir>/rtlib/build/openfhe/libFHErt_openfhe.a \
  <build dir>/rtlib/build/common/libFHErt_common.a \
  -L/usr/local/lib -lOPENFHEpke -lOPENFHEcore -lOPENFHEbinfhe \
  -lgmp -lpthread -o cpath_r20_of.exe
FHE_BTS_SF=59 ./cpath_r20_of.exe        # input seeded by the driver; print logits + prediction
```

Run in a single copy of the core (the build host is shared; deep-net timings
are unreliable under contention), matching the prior real-CKKS guidance.

### 7.4 Correctness check

Compare the model's logits / argmax against the cti `Expected_data` vector and
against an rt_ref run of the same model. Pass criterion: `acc = 1.0`,
prediction `3`, and per-element logits within the established tolerance of the
rt_ref baseline. An `acc < 1.0` or a NaN means a drift, depth-budget, or
`FHE_BTS_SF` problem in the bootstrap path; trace via
`PRINT_ALL=1 / ABS_ERROR / REL_ERROR` on the run and re-run with
`FHE_BTS_SF` adjusted.

## 8. Full functional and correctness sweep: regress58 plus resnet20

The two suites together cover functional completeness and correctness on the
OpenFHE C backend:

- `regress58` exercises the non-bootstrap levelled-SHE path across all operator
  kinds (add, mul, gemm, conv2d, avg_pool, flatten, ...), each with its own
  small `-VEC:ms` / `-CKKS:N` parameters from
  `doc/regress58-params/regress58_catalog.csv`.
- `resnet20` exercises the bootstrap path end to end (Section 7).

### 8.1 local_test `--lib openfhe` support

The harness currently supports `--lib ant` and `--lib ace` only. To target the
C OpenFHE backend, add (in the writable test temp-dir copy):

- `tool/detect_env.sh`: an OpenFHE probe block mirroring the ant block (around
  lines 152-171) and the ace block (around 261-300). Locate
  `libFHErt_openfhe.a` next to `libFHErt_common.a`, and capture the OpenFHE
  include dir (`/usr/local/include/openfhe`) and libraries
  (`-L/usr/local/lib -lOPENFHEpke -lOPENFHEcore -lOPENFHEbinfhe`). Write these
  into `env.sh` as `RTLIB_OPENFHE_LIBS` and `RTLIB_OPENFHE_INCS`, and add an
  include-compile probe for `rt_openfhe/rt_openfhe.h`.
- `tool/run_test.sh`: add `openfhe) BUILDER="fhe_build_openfhe.sh";;` to the
  `--lib` case (around line 68), and accept `LIB=openfhe`.
- `tool/fhe_build_openfhe.sh`: a new builder modeled on `fhe_build.sh` (ant)
  that appends `-P2C:lib=openfhe` to the option string, compiles the generated
  `.c` and the cti `main.c` (the runtime API is `extern "C"`), and links with
  `g++` against `FHErt_openfhe.a` + `FHErt_common.a` + the OpenFHE shared
  libraries + `libgmp`. The OpenFHE context is C++, so the final link must be
  `g++` even though the generated graph is C.

### 8.2 regress58 run

```bash
cd <the test temp dir>
# regress58 cases come from the catalog; run the operator group on openfhe.
./tool/run_test.sh <regress58-group-or-cases> --lib openfhe
```

Each case compiles with `-P2C:lib=openfhe` plus its catalog `-VEC:ms` /
`-CKKS:N`, links the OpenFHE backend, runs, and verifies against the
onnxruntime golden. Because these are small-`N` operators, none of them triggers
bootstrap; they verify the plain levelled-SHE lowering on the OpenFHE backend.

Pass criterion: every selected `regress58` (case x optset) is `PASS`. Any
`RUN_FAIL` points at an operator-level lowering or scaling mismatch on the
OpenFHE backend (re-run with `ABS_ERROR=0.01 REL_ERROR=0.01 PRINT_ALL=1`); any
`LINK_FAIL` points at the `--lib openfhe` builder wiring.

### 8.3 Combined acceptance

Functional completeness and correctness on the C OpenFHE backend is accepted
when all three hold:

1. Bootstrap self-test (Section 6): gtest and example smoke both pass, with
   the recovery error below tolerance and a visible level refresh.
2. `regress58` on `--lib openfhe`: all operator cases `PASS` against the
   onnxruntime golden.
3. `resnet20` on `--lib openfhe` (or the Section 7.3 manual path): `acc = 1.0`,
   prediction `3`, logits matching the rt_ref baseline within tolerance, with
   `FHE_BTS_SF=59`.

## 9. Risks and open decisions

- `LevelReduceInPlace` overload: confirm whether the installed OpenFHE 1.5.1
  takes a `levels` count or only a `psi` to match. The droplevel falls back to
  a one-tower loop (the existing `Mod_switch` form) if needed.
- Scaling on the plain (3a) branch: leaving it unchanged keeps the existing
  examples working and keeps `regress58` parity simple. rtlib-py uses
  `FIXEDMANUAL` in both branches; the plain branch here can stay on OpenFHE
  defaults unless `regress58` shows a mismatch, in which case set
  `FIXEDMANUAL` on both branches and re-verify `regress58`.
- Security level on the bootstrap branch: `HEStd_NotSet` matches rtlib-py and
  avoids an auto ring raise. `resnet20` at `N = 65536` is 128-bit-safe, so the
  existing `SetSecurityLevel` switch can remain for the model run; the toy unit
  test should use `HEStd_NotSet`.
- `Need_bts` availability: the Section 5.4 compiler change makes fhe-cmplr emit
  `Need_bts()` for the OpenFHE target. If a generated graph is run before that
  change, the link fails on the undefined `Need_bts` symbol; build the compiler
  change first.
- Dataflow (`-P2C:df=`) on the OpenFHE C backend is not exercised here. If
  needed later, verify `Openfhe_encode_float` / the offline encoding path
  separately; the ace backend notably lacks offline encoding, so do not assume
  it for OpenFHE. Run `regress58` and `resnet20` with `df` off (default) in
  this plan.
- Runtime: `resnet20` on real CKKS is minutes-to-tens-of-minutes single-run;
  schedule it uncontended and single-threaded for comparable timings (the
  build host shares cores).

## 10. Implementation order (not gated phases)

1. Enable the OpenFHE backend build (Section 5.1); confirm the plain path builds
   and existing examples run.
2. Add the Section 5.4 compiler hook so `Need_bts()` is emitted for OpenFHE.
3. Add the API/header declarations (Section 4) and the context + Bootstrap
   implementation in `openfhe_lib.cxx` (Section 5.2, 5.3).
4. Add and run the bootstrap correctness test (Section 6).
5. Add the `--lib openfhe` harness support (Section 8.1).
6. Run `regress58` on `--lib openfhe` (Section 8.2).
7. Run `resnet20` on `--lib openfhe` (Section 7); confirm `acc = 1.0`.
8. Confirm the Section 8.3 combined acceptance criteria.

## 11. Acceptance criteria summary

- `libFHErt_openfhe.a` builds from the build directory.
- `bool Need_bts()` is present in the generated OpenFHE target C.
- `Openfhe_bootstrap` / inline `Bootstrap` resolve the emitted `Bootstrap(res,
  op, level)` calls and refresh the modulus budget correctly.
- Bootstrap unit test and example smoke pass at the stated tolerance.
- `regress58` on the OpenFHE C backend: all cases `PASS`.
- `resnet20` on the OpenFHE C backend: `acc = 1.0`, prediction `3`, matching
  the rt_ref baseline.
