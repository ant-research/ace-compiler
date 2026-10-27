# rt_fhert backend design (rtlib-py)

`rt_fhert` is the third rtlib-py provider (after the plaintext reference
`rt_ref` and the OpenFHE 1.5.1 `rt_openfhe`). It executes `-CKKS2PY`-emitted
modules against real CKKS ciphertexts under OpenFHE 1.4.2 via the fhert
pybind11 binding `fhert.openfhe` (a binding fhert ships, not one this
provider writes). `-CKKS2PY:rt=rt_fhert` makes the emitter write
`from rtlib_py.rt_fhert import *` and the driver fetch this provider.

The provider is a near-verbatim port of `rt_openfhe/__init__.py`'s host
machinery (the F2.4 lazy-bridge design, mirrored): the lazy level bridge,
the model-driven `EvalBootstrap` context, the nsd-reset, the
`Compress`-with-explicit-nsd droplevel, the `_align` auto-level-adjust,
and the zero-operand short-circuit. The port targets the fhert OpenFHE
1.4.2 binding; fhert's CKKS surface is a faithful openfhe-python replica,
so host-mode arithmetic is observationally the same over a 1.4.2 build.
The binding interface differs in exactly three places; this design
enumerates them and lays out the host-vs-device plan. The implementation
stages (F4.4 through F4.8) and the device inline seams (F4.7b) are
specified elsewhere; this doc is the design-approval gate for F4.

## 1. The three deliberate deltas vs rt_openfhe

### 1.1 cp312 stub-bypass - `_ensure_of` lazy import

fhert is a PATH-based bazel tree, not pip-installed. The upstream
`import fhert.openfhe` runs `fhert/__init__.py`, which loads
`fhert._C.libfhert` (the MLIR/IR core). The prebuilt `libfhert.so` was
originally a CPython 3.13 build whose module-init eagerly called the
3.13-only symbol `PyThreadState_GetUnchecked`; CPython 3.12's libpython
does not export that symbol, so `import fhert.openfhe` raised
`ImportError` on a 3.12 host.

The host OpenFHE 1.4.2 binding itself - the pybind submodule
`fhert._C._lib.openfhe` - loads cleanly on 3.12. `_ensure_of` (defined in
`rtlib-py/rtlib_py/rt_fhert/__init__.py`, near line 932 in the feat backup
`2859787f`) therefore originally preferred `import fhert.openfhe` and
fell back to loading `fhert._C._lib.openfhe` directly with stub `fhert`
and `fhert._C` packages that skip the `libfhert` import chain.

After a cp312 rebuild of `_lib` and `libfhert`, the native
`import fhert.openfhe` (and `from fhert.backends import CModelDevice`)
succeeds; the stub-bypass path is now redundant. The lazy-import
structure (`_ensure_of` called inside `_seed_ctx`, never at module
import-time) stays: importing this provider never crashes without the
binding. The `FHERT_ROOT` env var points at the bazel fhert tree root
(added to `sys.path` before the binding import).

### 1.2 bare-enum adaptation - the ONE binding incompatibility

fhert's `fhert.openfhe` (and `fhert._C._lib.openfhe`) re-exports the bare
enum VALUES `FIXEDMANUAL` and `HEStd_NotSet` but NOT the enum CLASSES
`ScalingTechnique` and `SecurityLevel`. rt_openfhe (against OpenFHE
1.5.1) writes `_of.ScalingTechnique.FIXEDMANUAL` and
`_of.SecurityLevel.HEStd_NotSet`; rt_fhert writes the bare
`_of.FIXEDMANUAL` and `_of.HEStd_NotSet`. See `rt_fhert/__init__.py`
`_build_context` (lines 1009, 1016 in feat) and `_build_bootstrap_context`
(line 1073).

This is the one binding incompatibility between fhert's 1.4.2 binding
and upstream openfhe-python. `KeySwitchTechnique.HYBRID` and
`SecretKeyDist.*` ARE class-exported in fhert's binding and stay
verbatim. Consequence for the FLEX plumbing (delta 3 below): the parsed
FLEX tag drives the bare `_of.FIXEDMANUAL` / `_of.FLEXIBLEAUTO` /
`_of.FLEXIBLEAUTOEXT` calls directly. rt_openfhe carries an additional
`_scaling_technique(of, name)` helper (mapped to the `ScalingTechnique`
class); rt_fhert does NOT carry that helper - it branches on the parsed
tag instead, so rt_fhert never references a `ScalingTechnique` enum class.

### 1.3 SPARSE_TERNARY + cf=13 - bootstrap secret-key distribution

fhert bundles OpenFHE 1.4.2. Under 64-bit native both FIXEDMANUAL and
FLEXIBLEAUTOEXT bootstrap are supported (the 1.4.2 `FLEXIBLE*` throw is
`#if NATIVEINT == 128` only). But fhert's CKKS bootstrap under the
default `UNIFORM_TERNARY` (rt_openfhe's 1.5.1 choice) is unusable here:

  * the dense key raises `Degree [...] must be <= correction factor [9]`
    (EvalMod Chebyshev degree exceeds the auto correction factor);
  * where it does not crash, dense key gives poor refresh precision at
    packing slots >= 1024 (fhert's own `examples/profile_bootstrap.py`
    documents this and uses SPARSE_TERNARY).

This provider therefore bootstraps with `SPARSE_TERNARY` + an explicit
`correctionFactor` (env `FHE_BTS_CORRECTION`, default 13 - the clamp
max, also fhert's test value), passed through `EvalBootstrapSetup` as
the `slots` and `correctionFactor` kwargs (the fhert 1.4.2 binding
 accepts these; rt_openfhe used the 1.5.1 positional-only call). See
`rt_fhert/__init__.py` `_build_bootstrap_context` (lines 1071,
1144-1150 in feat). The explicit `correctionFactor=13` is REQUIRED
under `SPARSE_TERNARY`: the auto value 9 is below the EvalMod Chebyshev
degree, and without the explicit kwarg the bootstrap would raise
"Degree must be <= correction factor". The non-bootstrap 3a path keeps
`UNIFORM_TERNARY` (OpenFHE default) on both host and device.

## 2. Host vs device execution plan

### 2.1 Host bootstrap path

`_build_bootstrap_context` (feat line 1036) builds the model-driven
`EvalBootstrap` context: the ring dimension, first/moduli bit sizes, and
multiplicative depth are derived from the emitted `get_context_params`
(`poly_degree`, `first_prime_bit_num`, `scaling_factor_bit_num`,
`mul_depth`, `hamming_weight`) - NOT toy constants. The host bootstrap
path mirrors rt_openfhe exactly: `level_budget [4,4]`, the minimal
`ir_capacity` chain sizing (`ir_capacity = max(input_level,
max_target + bts_depth + 1)`), the ReSBM
`R = max(FHE_BTS_RESERVE, FHE_BTS_RESERVE_FLOOR or 2)` down-headroom
reserve, the `FHE_BTS_SF` scaling-modulus override, the model rotation
keys, and the `Enable(PKE, KEYSWITCH, LEVELEDSHE, ADVANCEDSHE, FHE)`
order. The lazy level bridge (`Cipher.level` is an on-demand
`@property`, `_ctx_ir_capacity() - ct.GetLevel()`) prevents
droplevel-upward drift. The bootstrap smoke runs on a small ring (e.g.
16384) and asserts recovery within tolerance.

`bootstrap_ciph` (feat line 2162) is the real `EvalBootstrap` op:
ant-faithful `while(nsd > 1) Rescale` reset then `LevelReduceInPlace` to
the ir_capacity target. Under FLEX the nsd-reset is inert (constant
nsd); the level, via the lazy bridge, is the real tracker.

### 2.2 Device path (CModelDevice, opt-in)

Device mode is opt-in via `FHE_DEVICE` / `FHE_DEVICE_RING` /
`FHE_DEVICE_HW`, or `ctx['device']` set by the harness.
`_build_device_context` (feat line 1284) builds a FLEX bootstrap context
(default FLEX-EXT byte-identical baseline) and uploads to the device via
`SetParamsTo` / `SetEvalKeysTo` / `SetBootstrapPrecomTo`. The cmodel
`CModelDevice` is module-level and long-lived, built once per device
`ring_dim` and reused across `_reset`. The device path adds:

  * inline round-trip seams - `_encrypt_values` / `_decrypt_slots` /
    `Cipher.clone` (host round-trip for `.to(_DEV)` / `.to(None)`) - so
    a device ciphertext is moved host for ops the cmodel does not expose,
    then moved back. A bare OpenFHE `Clone()` strips the `_device` field
    and yields a stale-host ciphertext; `Cipher.clone()` round-trips
    through host instead.
  * host-EVK-clear block - `ClearEvalAutomorphismKeys` (plus the native
    `ClearEvalMultKeys`) releases the OpenFHE host C++ eval-key keystore
    after device EVK upload; gated by `FHE_DEVICE_HOST_FALLBACK` (default
    off, the clearing path). The device reads the `/dev/shm`-uploaded
    EVK, not the host keys, so clearing them is safe.
  * HBM budget env `FHE_DEVICE_HBM_GB` (default 16 GiB per domain,
    guarded at 64); a CRAM fail-fast on the
    `dsa_bconv_table[16384]` CRT-qnum ceiling before the ~8-min keygen
    (avoids a native abort inside `SetParamsTo`).
  * half-packing opt-in `FHE_DEV_BTS_HALF` (bootstrap at
    `num_slots = ring_dim/4` rather than full `ring_dim/2`; the
    architecture resnet20 reference does this);
    `SetBatchSize` (message packing) stays `ring_dim/2` in both cases.
  * per-bootstrap reclaim `_reclaim_bts_device_scratch(snap, survivor_ct)`
    (feat line 1982) mirrors the hand-written cmodel's per-cycle
    bump-pointer rewind: free every address allocated DURING the
    just-finished bootstrap except a keep-set (the surviving result
    ciphertext + the `_rns_factor_cache` and `_monomial_cache` device
    caches). Host path is a no-op (`snap is None`).
  * input-seam: `_encrypt_values` encrypts at
    `of_level = _bts_ir_capacity - input_level` so every shard stays
    within the slot count (the same seam as F3.6, applied at the
    rtlib-py provider rather than the C-path handler).

### 2.3 resnet20 device - known not-yet-passing case

lenet_x2 device passes end-to-end. resnet20 device is a known
not-yet-passing case: per-bootstrap `_reclaim_bts_device_scratch`
reclaims per-bootstrap scratch, but a holder accumulation across
cycles persists - the compiled path lacks the hand-written cmodel's
per-cycle `hbm_offset = start_address` bump-reset on `App_relu_N`
return. The principled fix B1 - a per-cycle `dev.reset_scratch(keep_set)`
mirroring the reference bump-pointer rewind - is future work and is
NOT implemented this round. The resnet20 device case is gated out of the
F4.9 test gate and recorded in the F4.12 open-issues catalog; it is
NEVER presented as passing.

## 3. FLEX scaling note

rt_fhert drives the OpenFHE scaling technique by branching on the
parsed FLEX tag inside `_build_bootstrap_context` (host path) and
`_build_device_context` (device path). The pre-existing module globals
`_FLEX_MODE` (any non-FIXEDMANUAL), `_FLEX_EXT` (FLEX-EXT only), and
`_FLEX_SCALE` (the constant 2 FLEX `noiseScaleDeg`) carry the FLEX
compensations: constant-nsd=2 tracking, bounded nsd-reset, and the
EXT-only Compress `+1` tower compensation
(`compress_towers = towers_left + 1 if _FLEX_EXT else towers_left`,
feat line 774). The default `FHE_BTS_SCALING` / `FHE_DEV_SCALING` value
is `flexibleautoext` (FLEX-EXT, the byte-identical baseline); `fixed*`
selects FIXEDMANUAL (honest real-nsd tracking, host-3a parity) and
`flexnoext` / `flexibleauto` / `flex` / `noext` selects FLEX no-EXT
(the architecture team's resnet20 recipe: `qnum - 1`, one more usable
post-ModRaise limb, still auto-adjusted).

The FLEX-EXT Compress-compensation gate (F4.8, source commit
`da3f9b69`) details the EXT-only `+1` Compress towers compensation in
`_drop_to_of_level` and the `_parse_flex_scaling` activation under
`FHE_BTS_SCALING` (host) / `FHE_DEV_SCALING` (device). This doc
references F4.8 and does NOT reproduce its full specification.

## 4. Out of scope / later stages

  * F4.7b - device inline seams (`_drop_to_of_level` / `_encrypt_values`
    / `_decrypt_slots` / `Cipher.clone` / `add_ciph` / `sub_ciph` device
    round-trip), host-EVK-clear block, device HBM budget env,
    half-packing opt-in, input-seam fix.
  * F4.8 - FLEX-EXT Compress-compensation gate in `_drop_to_of_level`
    and the `_parse_flex_scaling` activation (source commit `da3f9b69`).
  * F4.12 - resnet20 device follow-up: the open-issues catalog names the
    per-cycle HBM accumulation and the fix-B1 future work; resnet20
    device is NOT run this refactor.

## 5. References

  * PR676 `doc/pr676-refactor/REFACTOR-PLAN.md` - section 3 F4 (stage
    table); section 2.2 (incremental construction of the monolithic
    `__init__.py`); section 2.3 (honest limitations); section 5
    (result-data triage; this design is a KEEP new design note under
    `doc/rtlib-py-doc/`).
  * `doc/pr676-summaries/02-rt-openfhe.md` - the F2.4 lazy-bridge
    design this port mirrors.
  * `rtlib-py/rtlib_py/rt_fhert/__init__.py` (feat backup `2859787f`):
    `_ensure_of` (~line 932), `_build_context` 3a (~line 991),
    `_build_bootstrap_context` host 3b (~line 1036),
    `_build_device_context` device (~line 1284),
    `_reclaim_bts_device_scratch` (~line 1982), `bootstrap_ciph`
    (~line 2162). Line numbers refer to the feat backup `2859787f`.
