# rt_fhert handoff and open issues (F4.12)

This document is the F4.12 handoff note for the `rt_fhert` rtlib-py provider.
It records the provider methodology, the F4 reconstruction shape, the
host-gate status, the device status (honestly framed), and the open-issues
catalog at the close of the F4 stage of the PR676 refactor. It supersedes the
prior feat-state methodology doc `doc/fhert-integration-handoff.md` (which
lived at a different path); that older doc is OUT per the refactor section 5,
and its durable methodology survives here in a trimmed form. Per the
refactor section 5 triage, this trimmed form drops per-run numbers, per-ROUND
labels, ISO dates, absolute paths, and "current status" lines.

Audience: a `fhert` backend engineer or a refactor reviewer who needs to
re-derive the `rt_fhert` provider state without re-reading the F4 stage
trail. The full provider design is in `doc/rtlib-py-doc/fhert-backend-design.md`
(the F4.1 design note). The device issue catalog (device HBM accumulation,
the device FIXEDMANUAL harden methodology, the device-kernel VRF/HBM gaps the
fhert team owns) is in `doc/fhert-device-fix/OPEN-ISSUES.md` (the F4.10
catalog). This handoff references both and does not reproduce them.

## 1. The rt_fhert provider

`rt_fhert` is the third rtlib-py provider (after the plaintext reference
`rt_ref` and the OpenFHE 1.5.1 `rt_openfhe`). It executes `-CKKS2PY`-emitted
modules against real CKKS ciphertexts under OpenFHE 1.4.2 via the fhert
pybind11 binding `fhert.openfhe` (a binding fhert ships, not one this
provider writes). The compiler option `-CKKS2PY:rt=rt_fhert` makes the
emitter write `from rtlib_py.rt_fhert import *` and the driver fetch this
provider at run time.

The provider is a near-verbatim port of `rt_openfhe/__init__.py`'s host
machinery onto the fhert 1.4.2 binding: the lazy level bridge, the
model-driven `EvalBootstrap` context, the `nsd`-reset, the
`Compress`-with-explicit-`nsd` droplevel, the `_align` auto-level-adjust, and
the zero-operand short-circuit. fhert's CKKS surface is a faithful
openfhe-python replica, so host-mode arithmetic is observationally the same
over a 1.4.2 build. The binding interface differs in exactly three places;
the F4.1 design note enumerates them. They are summarized below.

### 1.1 cp312 stub-bypass (`_ensure_of`)

fhert is a PATH-based bazel tree, not pip-installed. The upstream
`import fhert.openfhe` runs `fhert/__init__.py`, which loads `fhert._C.libfhert`
(the MLIR/IR core). A prebuilt `libfhert.so` built for CPython 3.13 eagerly
calls the 3.13-only symbol `PyThreadState_GetUnchecked`; CPython 3.12's
libpython does not export that symbol, so a naive `import fhert.openfhe`
raised `ImportError` on a 3.12 host.

The host OpenFHE 1.4.2 binding itself (the pybind submodule
`fhert._C._lib.openfhe`) loads cleanly on 3.12. `_ensure_of`, called inside
`_seed_ctx` (never at module import-time), therefore originally preferred the
native `import fhert.openfhe` and fell back to loading
`fhert._C._lib.openfhe` directly with stub `fhert` and `fhert._C` packages
that skip the `libfhert` import chain.

After a cp312 rebuild of `_lib` and `libfhert`, the native
`import fhert.openfhe` (and `from fhert.backends import CModelDevice`)
succeeds; the stub-bypass path is now redundant. The lazy-import structure
stays: importing this provider never crashes without the binding. The
`FHERT_ROOT` env var points at the bazel fhert tree root and is added to
`sys.path` before the binding import.

### 1.2 bare-enum adaptation

fhert's `fhert.openfhe` (and `fhert._C._lib.openfhe`) re-exports the bare
enum VALUES `FIXEDMANUAL` and `HEStd_NotSet` but NOT the enum CLASSES
`ScalingTechnique` and `SecurityLevel`. `rt_openfhe` (against OpenFHE 1.5.1)
writes `_of.ScalingTechnique.FIXEDMANUAL` and
`_of.SecurityLevel.HEStd_NotSet`; `rt_fhert` writes the bare `_of.FIXEDMANUAL`
and `_of.HEStd_NotSet`.

This is the one binding incompatibility between fhert's 1.4.2 binding and
upstream openfhe-python. `KeySwitchTechnique.HYBRID` and `SecretKeyDist.*`
ARE class-exported in fhert's binding and stay verbatim. Consequence for the
FLEX plumbing: the parsed FLEX tag drives the bare `_of.FIXEDMANUAL` /
`_of.FLEXIBLEAUTO` / `_of.FLEXIBLEAUTOEXT` calls directly. `rt_openfhe`
carries an additional `_scaling_technique(of, name)` helper (mapped to the
`ScalingTechnique` class); `rt_fhert` does NOT carry that helper - it
branches on the parsed tag instead, so `rt_fhert` never references a
`ScalingTechnique` enum class.

### 1.3 SPARSE_TERNARY + cf=13 (bootstrap secret-key distribution)

fhert bundles OpenFHE 1.4.2. Under 64-bit native both FIXEDMANUAL and
FLEXIBLEAUTOEXT bootstrap are supported (the 1.4.2 `FLEXIBLE*` throw is
`#if NATIVEINT == 128` only). But fhert's CKKS bootstrap under the default
`UNIFORM_TERNARY` (the `rt_openfhe` 1.5.1 choice) is unusable here:

  - the dense key raises `Degree [...] must be <= correction factor [9]`
    (the EvalMod Chebyshev degree exceeds the auto correction factor);
  - where it does not crash, the dense key gives poor refresh precision at
    packing slots >= 1024 (fhert's own `examples/profile_bootstrap.py` uses
    half packing + `level_budget=(3,3)` for this reason).

`rt_fhert` therefore fixes `secret_key_dist = SPARSE_TERNARY` in the
bootstrap context and passes `correctionFactor = 13` explicitly to
`EvalBootstrapSetup` (env `FHE_BTS_CORRECTION`, default 13). The auto value
(9 under FIXEDMANUAL) is below the EvalMod Chebyshev degree; the clamp max
13 is the value the deep chain needs.

## 2. FLEX scaling path: FLEX-EXT / no-EXT / FIXEDMANUAL

`rt_fhert` drives the OpenFHE scaling technique by branching on the parsed
FLEX tag inside `_build_bootstrap_context` (host) and `_build_device_context`
(device). The module globals `_FLEX_MODE` (any non-FIXEDMANUAL), `_FLEX_EXT`
(FLEX-EXT only), and `_FLEX_SCALE` (the constant 2 FLEX `noiseScaleDeg`)
carry the FLEX compensations:

  - constant-nsd=2 tracking (FLEX `Rescale`/`ModReduce` do not decrement
    `NoiseScaleDeg`, so `scale_deg` is tracked as the constant 2);
  - bounded `nsd`-reset (`for _ in range(post_nsd - 1)` rather than
    `while nsd > 1`);
  - the EXT-only Compress `+1` tower compensation
    (`compress_towers = towers_left + 1 if _FLEX_EXT else towers_left`)
    inside `_drop_to_of_level`, so under FLEX-EXT every droplevel (including
    the post-bootstrap droplevel) lands at the intended level rather than
    one level too deep.

The default `FHE_BTS_SCALING` (host) / `FHE_DEV_SCALING` (device) value is
`flexibleautoext` (FLEX-EXT, the byte-identical baseline). The alternatives:

  - `fixed*` selects FIXEDMANUAL (honest real-nsd tracking, host-3a parity);
    the no-bootstrap 3a path stays FIXEDMANUAL unconditionally.
  - `flexnoext` / `flexibleauto` / `flex` / `noext` selects FLEX no-EXT (the
    architecture team's resnet20 recipe: `qnum - 1`, one more usable
    post-ModRaise limb, still auto-adjusted).

Why FLEX-EXT and not FIXEDMANUAL for bootstrap: fhert 1.4.2 FIXEDMANUAL
bootstrap accumulates scale drift (the rescale divisor is the actual prime,
not the nominal scaling factor; `AdjustCiphertext` does not re-anchor in
1.4.2), so on a deep chain with a degree-20 Chebyshev relu the drift blows
the final logits past the decryption bound. A single FLEX bootstrap is far
more precise than FIXEDMANUAL at the same ring/scaling-factor (both probed
under 64-bit native; both supported). The C-side OpenFHE backend (the F3
stage, `rtlib/openfhe`) still uses FIXEDMANUAL; closing that drift is F3's
open issue, recorded in the F3 summary, NOT F4's (section 7.3).

### 2.1 Bootstrap level consumption and the R reserve

The bootstrap context is sized model-driven from the emitted
`get_context_params()` (`poly_degree`, `mul_depth`, `hamming_weight`,
the rotation keys, the per-`relu` `relu_vr` map). The chain sizing formulas
are the same as the `openfhe-python` integration:

  - `level_budget` via env `FHE_BTS_LEVEL_BUDGET`, default `[4,4]`
    (s2c/c2s). The number of levels bootstrap consumes is
    `GetBootstrapDepth(level_budget, secret_key_dist)` (18 for `[4,4]` +
    `SPARSE_TERNARY`). Larger budgets deepen the chain (more towers) without
    changing the post-bootstrap app levels; `[4,4]` is kept (fastest,
    mirrors ant).
  - `ir_capacity = max(input_level, max_target + bts_depth + 1)`;
    `levels_after = ir_capacity - bts_depth - 1`;
    `depth = levels_after + bts_depth + R`.
    `max_target = mul_depth + 1 - Mul_depth_of_bootstrap()` (15 if
    `0 < hamming_weight <= 192`, else 19), overridable by an emitted
    `ctx['max_bts_target']`.
  - `R = max(FHE_BTS_RESERVE, FHE_BTS_RESERVE_FLOOR or 2)`, the
    ReSBM bottom-bootstrap down-headroom. The default 2 is the
    non-`StCFirst` 2-tower floor (matched to the openfhe-python value after
    the bootstrap level-reduction). `R` is purely down-headroom below IR 0;
    it does not affect bootstrap precision (governed by the scaling-factor
    override) and only barely affects the execution time. A larger
    `FHE_BTS_RESERVE` works but is slower with identical logits.
  - `scaling_factor_bit_num` (sf): env `FHE_BTS_SF` overrides it on the
    bootstrap path; the emitted default 56 gives only a few-bit refresh, so
    a deep chain raises it (ResNet20 needs 59). `first_prime_bit_num = 60`
    (default).

Requirement: the number of usable levels after bootstrap is kept identical
to the openfhe-python integration - the same `ir_capacity` formula (the
SPARSE `bts_depth` only deepens the chain; the compiler-fixed app levels
`max_target` are unchanged). The R reserve is below IR 0 and does not change
it.

## 3. Host vs device

  - Host mode: `rt_fhert` uses the OpenFHE-native C++ CKKS path through
    `fhert.openfhe`. The host path is the correctness baseline.
  - Device mode: opt-in via `FHE_DEVICE=1` (or `ctx['device']`), with
    `FHE_DEVICE_RING` / `FHE_DEVICE_HW` to override the device ring/grid and
    `FHE_DEV_BTS_HALF` for half-packing. In device mode
    `_build_device_context` builds a FLEX bootstrap context (default FLEX-EXT)
    and uploads to the `CModelDevice` via `SetParamsTo` / `SetEvalKeysTo` /
    `SetBootstrapPrecomTo`; ciphers are moved to the device via `.to(_DEV)`
    and back via `.to(None)`. The cmodel device memory is `/dev/shm`-backed.

Device-specific seams (all in the reconstructed provider, byte-verbatim from
feat):

  - Inline round-trip seams `_encrypt_values` / `_decrypt_slots` /
    `Cipher.clone` (host round-trip for `.to(_DEV)` / `.to(None)`): a device
    ciphertext is moved host for ops the cmodel does not expose, then moved
    back. A bare OpenFHE `Clone()` strips the `_device` field and yields a
    stale-host ciphertext; `Cipher.clone()` round-trips through host instead.
  - host-EVK-clear block: `ClearEvalAutomorphismKeys` (plus the native
    `ClearEvalMultKeys`) releases the OpenFHE host C++ eval-key keystore after
    the device EVK upload; gated by `FHE_DEVICE_HOST_FALLBACK` (default off,
    the clearing path). The device reads the `/dev/shm`-uploaded EVK, not the
    host keys, so clearing them is safe.
  - HBM budget env `FHE_DEVICE_HBM_GB` (default per domain, guarded at 64)
    plus a CRAM fail-fast on the `dsa_bconv_table[16384]` CRT-qnum ceiling
    before the long keygen step (avoids a native abort inside `SetParamsTo`).
  - Half-packing opt-in `FHE_DEV_BTS_HALF` (bootstrap at
    `num_slots = ring_dim/4` rather than full `ring_dim/2`; the architecture
    resnet20 reference does this). `SetBatchSize` (message packing) stays
    `ring_dim/2` in both cases.
  - Per-bootstrap reclaim `_reclaim_bts_device_scratch(snap, survivor_ct)`:
    mirrors the hand-written cmodel's per-cycle bump-pointer rewind - free
    every address allocated DURING the just-finished bootstrap except a
    keep-set (the surviving result ciphertext + the `_rns_factor_cache` and
    `_monomial_cache` device caches). The host path is a no-op
    (`snap is None`).
  - Input-seam: `_encrypt_values` encrypts at
    `of_level = _bts_ir_capacity - input_level` so every shard stays within
    the slot count (the same seam as the F3.6 C-path fix, applied at the
    rtlib-py provider rather than the C-path handler).

These seams are exercised by the device bootstrap smoke (at a
device-compatible ring) and the device lenet_x2 end-to-end run on a
full-cmodel host - see section 6 for the honest status of the device suite
in THIS refactor's env.

## 4. F4 reconstruction shape

The F4 stage reconstructed `rtlib-py/rtlib_py/rt_fhert/__init__.py` as a
verbatim-prefix union of additive sub-commits, each layering one concern:

  - F4.4a `7db33163` - skeleton + lazy bridge + FLEX parser + FULL
    dispatcher
  - F4.4b `34e42903` - host bootstrap context
  - F4.6 `d8ea731c` - host `EvalBootstrap` body
  - F4.5a `e5503201` - SHE / encode / IO surface (the encode/decode + IO
    shard seam)
  - F4.7a `a94eaa5c` - device context + reclaim core
  - F4.5b `58670ca8` - SHE tail
  - F4.7b `88605ba9` - inline device seams + HBM/half-packing/input-seam
  - F4.5c `2b2d2d7b` - remainder parity (the 41-name contract surface)
  - F4.8 `eee497bc` - FLEX-EXT Compress-compensation gate + queries +
    observability + `__all__` export tail (FINAL reconstruct; the
    EXT-only `+1` Compress towers compensation and the
    `_parse_flex_scaling` activation).

The FINAL reconstructed provider is byte-identical to feat `2859787f` except
a single module-docstring section-5 trim (the cp.12/3.13 host-build-mismatch
narrative excised; everything else byte-verbatim). The diff vs `2859787f`
is only the docstring trim. The 41-name contract surface is restored; the
`__all__` list parity with `rt_ref` holds.

This reconstruction shape is recorded at a methodology altitude - no
per-machine build numbers, no per-run timings, no absolute paths. The
lineage above is the only "when" this doc carries; the commit hashes are
the only identifier that survives the refactor section-5 triage (per-ROUND
labels do not).

## 5. Host gate (F4.9) - qualitative

The F4.9 host gate ran two kinds of verification on the reconstructed
provider in this refactor's environment.

### 5.1 Bootstrap smoke (host)

The ctest `test_fhert_bootstrap_host` seeds a real `EvalBootstrap` context
(a small ring, `mul_depth=10`, FLEX-EXT + SPARSE_TERNARY +
`correctionFactor=13`, level budget `[4,4]`), encrypts a small message near
the bottom of the chain, calls `bootstrap_ciph`, and asserts the message is
recovered within `FHE_BTS_TOL` (default 1e-1) with a fresh post-bootstrap
scale, a valid level, and `levels_remaining > 1` (the three-ciphertext flag
is not set - the bootstrap returns a single refreshed ciphertext).

The host gate is GREEN: the smoke recovers the input message well within
the `FHE_BTS_TOL` tolerance, with a fresh post-bootstrap scale and a valid
post-bootstrap level. The ctest is registered with `SKIP_RETURN_CODE 77`
(a host without the fhert binding SKIPs cleanly rather than failing; the
lazy-import structure makes the import safe).

### 5.2 Host resnet20

The `ace_test.py` harness running `resnet20_cifar10_pre` image 0 through the
reconstructed provider. Under the default FLEX-EXT path (env
`FHE_BTS_SCALING=flexibleautoext`, the fhert bootstrap scaling-factor
override `FHE_BTS_SF`, and `FHE_BTS_CORRECTION=13`), the host resnet20
prediction matches the golden label. Under the no-EXT path
(`FHE_BTS_SCALING=flexnoext`), the host resnet20 prediction ALSO matches
the golden label. Both paths execute the full ReSBM graph (the emitted count
of bootstraps at full packing); the prediction is stable across runs.

Per the refactor section 5, this doc deliberately omits per-run elapsed time
and recovery-error numbers. The qualitative statements above are the
durable record. The host gate is GREEN.

## 6. Device status - honest framing

The device status is the heart of the F4.12 handoff. It must NOT be read as
"the device suite passes in this env." It does not, and this section says so
explicitly.

### 6.1 Device suite proven-by-feat 41af5282 on a full-cmodel host

The device IMPLEMENTATION is present in the reconstructed provider:
`_build_device_context`, `_reclaim_bts_device_scratch`, the inline device
seams, the host-EVK-clear block, the CRAM fail-fast, the half-packing
opt-in. The device suite - the device bootstrap smoke, the device lenet_x2
end-to-end run, and the operator-level regress58 sweep on the device path -
is a working feature per feat commit `41af5282` on a full-cmodel host (one
with a CModel JIT whose kernel headers are in schema sync with the device
runtime Python the cmodel template emits). That is the established, honest
baseline.

### 6.2 NOT re-verified in this refactor's env (CModel JIT PerfSnapshot/lines skew)

The device suite was NOT re-verified in THIS refactor's env. The CModel JIT
kernel compile raises `'struct PerfSnapshot' has no member named 'lines'` -
a kernel-header schema skew between the cmodel source the JIT compiles
against and the device-runtime Python the cmodel template emits. The
latest `fhert/backends/cmodel_device.py` kernel template emits
`perf.lines` / `PerfLineBucket`; the cmodel source must contain
`PerfSnapshot::lines` (the `PerfLineBucket` field in
`intrinsic/include/fhe_perf.h`) for the compiled kernel to typecheck. An
older cmodel lacks that field and every device kernel compilation then
fails with the error above.

This is a known cmodel-build header-sync issue, NOT a refactor regression:
the `rt_fhert` device code is byte-verbatim from feat `2859787f`, so a
header-schema gap cannot be caused by the F4 reconstruction. Resolution is
a cmodel JIT rebuild against a cmodel source tree that has
`PerfSnapshot::lines` / `PerfLineBucket` (the pinned cmodel and
`cmodel_device.py` must be version-consistent). The follow-up A9 - re-verify
the device suite after that cmodel JIT rebuild - is recorded in section 7.

This doc therefore records the device suite as: working per feat on a
full-cmodel host, NOT re-verified here due to the CModel JIT
PerfSnapshot/lines skew that requires a cmodel rebuild to resolve (a
cmodel-build header-sync gap, not a refactor regression; the rt_fhert device
code is byte-verbatim feat).

### 6.3 resnet20 device NOT run this refactor (fix B1 future, A9)

The resnet20 device case is NOT run in this refactor. It is gated out of
the F4.9 test gate via the ctest `test_fhert_device_resnet20`, which
defaults to SKIP: `SKIP_RETURN_CODE 77` with the opt-in env
`FHE_RUN_RESNET20_DEVICE=1` (default off, so the test exits 77 and ctest
reports SKIP, not FAIL).

The reason is the per-cycle device-HBM accumulation the feat device
investigation identified. The per-bootstrap `_reclaim_bts_device_scratch`
reclaims per-bootstrap scratch, but a holder accumulation ACROSS cycles
persists at the resnet20 scale: the compiled path lacks the hand-written
cmodel's per-cycle `hbm_offset = start_address` bump-reset on
`App_relu_N` return. The principled fix B1 - a per-cycle dev.reset_scratch
call (the keep_set argument preserves the surviving result ciphertext plus
the device caches) mirroring the reference bump-pointer rewind - is future
work and is NOT implemented this round. The resnet20 device case is NEVER
presented as passing.

The follow-up A9 (re-verify the device suite, including the resnet20 device
case, after a cmodel JIT rebuild) is the gate that would re-exercise the
device path end to end. The HBM budget env `FHE_DEVICE_HBM_GB` and the CRAM
fail-fast are the runtime guards already wired in the reconstructed
provider; they make a device-OOM or a CRAM-overflow fail fast rather than
abort natively, but they do NOT resolve the per-cycle accumulation that
fix B1 targets.

## 7. Open-issues catalog

### 7.1 CModel JIT PerfSnapshot/lines skew (blocks device re-verification; follow-up A9)

The CModel JIT kernel compile raises `'struct PerfSnapshot' has no member
named 'lines'` in this refactor's env. The cmodel the JIT compiles against
must contain `PerfSnapshot::lines` (the `PerfLineBucket` field); the latest
`fhert/backends/cmodel_device.py` kernel template emits
`perf.lines` / `PerfLineBucket`. An older cmodel lacks it, and every device
kernel compilation then fails with that error.

This is a cmodel-build header-sync gap, NOT a refactor regression (the
`rt_fhert` device code is byte-verbatim feat). Resolution: rebuild the
cmodel JIT against a cmodel source tree that has `PerfSnapshot::lines` /
`PerfLineBucket`; ensure the pinned cmodel and `cmodel_device.py` are
version-consistent. A runtime-cache redirect (the cmodel device lib is
built into an `XDG_CACHE_HOME`-salt directory; redirect it with
`XDG_CACHE_HOME` pointing at a writable path so the lib builds from the
correct cmodel source) avoids a stale lib loaded from the default cache
location; this is a cache-location note, not the issue itself.

Follow-up A9: re-verify the device suite (the device bootstrap smoke, the
device lenet_x2 end-to-end run, the operator-level regress58 sweep on the
device path) after that cmodel JIT rebuild. Until A9 lands, the device
suite is recorded as working-per-feat-on-a-full-cmodel-host and NOT
re-verified here.

### 7.2 Per-cycle device-HBM accumulation (resnet20 device, fix B1 future)

The resnet20 device case is NOT run this refactor (ctest `SKIP_RETURN_CODE
77`; opt-in via `FHE_RUN_RESNET20_DEVICE=1`). The per-bootstrap reclaim
`_reclaim_bts_device_scratch` frees per-bootstrap scratch, but a holder
accumulation across cycles persists at the resnet20 scale. The principled
fix B1 - a per-cycle dev.reset_scratch (keep_set) mirroring the
hand-written cmodel's per-cycle bump-pointer rewind on `App_relu_N` return
- is future work, NOT implemented this round. The HBM budget env
`FHE_DEVICE_HBM_GB` and the CRAM fail-fast are already wired; they make the
failure mode fail fast rather than abort, but they are not the fix.

The full device-kernel VRF/HBM capacity gap (the device keyswitch VRF
allocation vs the bootstrap setup HBM, the 256x256 cmodel grid ceiling, the
`dnum`/`beta` conflict at qnum 30 full packing) is documented in
`doc/fhert-device-fix/OPEN-ISSUES.md` (the F4.10 catalog). That catalog is
the device-cmodel issue list for the fhert team; this handoff references it
and does NOT reproduce it. The fix B1 / A9 pair above is the fhe-cmplr-side
follow-up that this refactor records.

### 7.3 C-side FIXEDMANUAL drift (F3 open issue, not F4)

The C-side OpenFHE backend (the F3 stage, `rtlib/openfhe`) uses FIXEDMANUAL
bootstrap scaling. fhert 1.4.2 FIXEDMANUAL accumulates scale drift on a deep
relu chain (rescale divides by the actual prime, not the nominal scaling
factor, and `AdjustCiphertext` does not re-anchor in 1.4.2); on a deep chain
with a degree-20 Chebyshev relu the drift blows the final logits past the
decryption bound. The rtlib-py `rt_fhert` provider switched its default to
FLEXIBLEAUTOEXT (FLEX-EXT) to side-step the drift.

The C-side path has not switched; closing the C-side drift (either by
porting the FLEX-EXT Compress-compensation gate to the C backend's
`AdjustCiphertext`, or by switching the C backend's scaling technique) is
F3's open issue, recorded in the F3 summary
`doc/pr676-summaries/03-openfhe-c-bootstrap.md`, NOT F4's. This handoff
records it for cross-reference only and does not propose a fix.

### 7.4 Device FIXEDMANUAL harden (F4.10, byte-verbatim in this provider)

The F4.10 catalog `doc/fhert-device-fix/OPEN-ISSUES.md` records the device
FIXEDMANUAL harden methodology: a conditional `_FLEX_MODE` (only set when
non-FIXEDMANUAL, so an emit-side FIXEDMANUAL stays FIXEDMANUAL end-to-end),
a `Cipher.clone` short-circuit that prevents the stale-host-ciphertext
use-after-free, and a CRAM fail-fast guard. The harden code is
byte-verbatim in the reconstructed provider. This handoff cross-references
the F4.10 catalog and does NOT reproduce its methodology.

## 8. Build / environment (parameterized)

The provider has no host-side dependency on absolute paths; it reads
everything it needs from env. There are no workspace-rooted or tmp-rooted
hardcoded paths in the provider; this section names the env knobs and lets
the host wiring inject the actual values.

Host/general env (read by the provider or its harness):
  - `FHERT_ROOT` - the bazel fhert tree root (added to `sys.path` before
    the binding import in `_ensure_of`).
  - `FHE_CMPLR` - the `fhe_cmplr` binary path (harness only).
  - `OPENFHE_PY_BUILD_DIR` - the openfhe-python build dir (test plumbing).
  - `ONNX_ROOT` - the onnx-models tree root (model-level runs).
  - `RTLIB_PY_DIR` - the `rtlib-py` package root (bootstrapped onto
    `sys.path`).

Provider knobs (FLEX/bootstrap): `FHE_BTS_SCALING` (host) /
`FHE_DEV_SCALING` (device) select `flexibleautoext` (default) /
`fixedmanual` / `flexnoext`; `FHE_BTS_SF` (the scaling-factor override;
deep chains raise it above the emitted default 56); `FHE_BTS_CORRECTION`
(the EvalBootstrap correctionFactor, default 13); `FHE_BTS_LEVEL_BUDGET`
(the s2c/c2s level budget, default `4,4`); `FHE_BTS_RESERVE` /
`FHE_BTS_RESERVE_FLOOR` (the R reserve / floor); `FHE_BTS_TOL` (the
bootstrap-smoke tolerance, default 1e-1).

Device knobs (all opt-in): `FHE_DEVICE` / `FHE_DEVICE_RING` /
`FHE_DEVICE_HW` (the device opt-in and the ring/grid override);
`FHE_RUN_RESNET20_DEVICE` (the resnet20 device ctest opt-in; default off,
the ctest SKIPs at 77 otherwise); `FHE_DEVICE_HBM_GB` (the per-domain
device HBM budget, guarded at 64); `FHE_DEV_BTS_HALF` (half-packing
bootstrap opt-in); `FHE_DEVICE_HOST_FALLBACK` (the host-EVK-clear gate,
default off, the clearing path); `XDG_CACHE_HOME` (the cmodel device lib
cache redirect - the lib builds into an `XDG_CACHE_HOME`-salt directory;
redirect to a writable path so the lib builds from the correct cmodel
source).

## 9. Test surface

The F4.9 test surface, all under `test/rtlib_py/py_verify/`:

  - `test_fhert_bootstrap_host.py` - the host bootstrap smoke (ctest target
    `test_fhert_bootstrap_host`, with `SKIP_RETURN_CODE 77`; a host without
    the fhert binding SKIPs rather than failing).
  - `test_fhert_device_resnet20` - the resnet20 device regression (ctest
    target `test_fhert_device_resnet20`, `SKIP_RETURN_CODE 77`, opt-in env
    `FHE_RUN_RESNET20_DEVICE=1`; default off -> exit 77 -> ctest SKIP, not
    FAIL). This is the recorded-as-known-not-yet-passing case that fix B1
    targets.

The operator-level regress58 sweep runs through
`doc/regress58-params/run_cti_params_sweep.py` (the F4.11 runner), which is
self-contained and env-parameterized (no hardcoded paths; it is usable from
a clean v2 checkout). The sweep reads the operator-level catalog
`doc/regress58-params/regress58_catalog.csv` and the model-level catalog in
the same directory; both are structural catalog/scope descriptors, not
per-ROUND run labels. A 1-row HOST smoke (the add operator over `rt_fhert`)
is the F4.11 host gate and is GREEN (the operator recovers well within the
rt_fhert tolerance, which matches the `rt_openfhe` tolerance). The
operator-level sweeps run on the HOST binding, not the device binding -
the device row is NOT the F4.11 gate. The runner accepts
`--providers rt_fhert --df-states off,on` and classifies rows by recovery
error against a per-provider tolerance.

## 10. Cross-references

  - `doc/rtlib-py-doc/fhert-backend-design.md` - the F4.1 design note (full
    provider design; the three deltas; host-vs-device plan; the resnet20
    device known-not-yet-passing note).
  - `doc/fhert-device-fix/OPEN-ISSUES.md` - the F4.10 device issue catalog
    (device-kernel VRF/HBM gaps for the fhert team; the device FIXEDMANUAL
    harden methodology).
  - `doc/regress58-params/run_cti_params_sweep.py` (F4.11) plus the catalogs
    in the same directory (`regress58_catalog.csv` for the operator-level
    sweep; the model-level catalog alongside it).
  - `rtlib-py/rtlib_py/rt_fhert/__init__.py` - the reconstructed provider
    (feat backup `2859787f`, byte-match minus the docstring section-5 trim).
  - `doc/pr676-summaries/03-openfhe-c-bootstrap.md` - the F3 summary (the
    C-side FIXEDMANUAL drift is F3's open issue, not F4's).
