"""`rt_fhert` -- real-HE rtlib-py provider via fhert's OpenFHE 1.4.2 binding.

A THIRD rtlib-py provider (alongside the plaintext reference `rt_ref` and the
upstream `rt_openfhe`) that executes emitted `-CKKS2PY` modules against **real CKKS
ciphertexts** under OpenFHE **1.4.2** via fhert's own pybind11 binding
`fhert.openfhe` (a ready binding fhert ships -- NOT one we write). It is selected
by the single compiler field `POLY2PY_CONFIG::_py_rt_module`: `-CKKS2PY:rt=rt_fhert`
makes the emitter write ``from rtlib_py.rt_fhert import *`` and the driver fetch
this provider.

This provider is a **port of `rt_openfhe/__init__.py`**'s host machinery -- the
lazy level bridge, the model-driven EvalBootstrap context, the nsd-reset, the
``Compress``-with-explicit-nsd droplevel, the ``_align`` auto-level-adjust, and
the zero-operand short-circuit -- because fhert's CKKS surface is a faithful
openfhe-python replica and host-mode ckks arithmetic is observationally the same
over a 1.4.2 build. The deliberate deltas vs ``rt_openfhe`` (see `_ensure_of` +
`_build_bootstrap_context`):

  1. **Host-only binding import via stub bypass.** fhert is a PATH-based bazel
     tree (not pip-installed); the upstream ``import fhert.openfhe`` runs
     ``fhert/__init__.py`` -> ``language``/``profiler`` -> ``fhert._C.libfhert``
     (the MLIR/IR core). When this chain is unavailable on the host (a libfhert
     build mismatch), the host OpenFHE 1.4.2 binding itself -- the pybind
     ``_lib.so`` submodule ``fhert._C._lib.openfhe`` -- still loads. ``_ensure_of``
     therefore prefers the full ``fhert.openfhe`` (kept for hosts with a loadable
     libfhert) and falls back to loading ``fhert._C._lib.openfhe`` directly with
     stub ``fhert``/``fhert._C`` packages that skip the ``libfhert`` import chain.
     The fallback exposes the NATIVE OpenFHE classes
     (Eval*/Rescale/Compress/EvalBootstrap/...) with the same bare-enum symbols;
     the device-patch layer (``_ciphertext``/...) is skipped because it pulls
     in ``fhert.language``/``fhert.backends`` (-> libfhert) and only adds
     device dispatch, which host mode never uses.
     CONSEQUENCE: device (CModelDevice) mode is UNavailable on such a host
     (device dispatch needs the patched classes + ``fhert.backends``); it is
     a documented backend limitation, not an integration defect. See
     `_ensure_of` and "Remaining backend limitations" in the delivery report.
  2. **Bare-enum adaptation (the ONE binding incompatibility).** fhert's
     ``fhert.openfhe`` (and ``fhert._C._lib.openfhe``) re-exports the bare enum
     values ``FIXEDMANUAL`` and ``HEStd_NotSet`` but NOT the ``ScalingTechnique``
     / ``SecurityLevel`` enum classes, so the call-sites that wrote
     ``_of.ScalingTechnique.FIXEDMANUAL`` / ``_of.SecurityLevel.HEStd_NotSet``
     in `rt_openfhe` here use the bare ``_of.FIXEDMANUAL`` / ``_of.HEStd_NotSet``.
     ``KeySwitchTechnique.HYBRID`` and ``SecretKeyDist.*`` ARE class-exported and
     stay verbatim.
  3. **Bootstrap uses SPARSE_TERNARY + an explicit correction factor.** fhert
     bundles OpenFHE 1.4.2; under 64-bit native BOTH FIXEDMANUAL and
     FLEXIBLEAUTOEXT bootstrap are supported (the 1.4.2 ``FLEXIBLE*`` throw is
     ``#if NATIVEINT == 128`` only). However fhert's CKKS bootstrap under the
     default ``UNIFORM_TERNARY`` (rt_openfhe's 1.5.1 choice) is unusable here:
     the dense key raises ``Degree [...] must be <= correction factor [9]``
     (EvalMod Chebyshev degree exceeds the auto correction factor) and, where it
     does not crash, gives poor refresh precision at packing slots >= 1024
     (fhert's own ``examples/profile_bootstrap.py`` documents this and uses
     ``SPARSE_TERNARY``).
     This provider therefore bootstraps with ``SPARSE_TERNARY`` + an explicit
     ``correctionFactor`` (env ``FHE_BTS_CORRECTION``, default 13 -- the clamp
     max, also fhert's test value), passing it through ``EvalBootstrapSetup``
     (the fhert binding accepts ``slots``/``correctionFactor`` kwargs; rt_openfhe
     used the 1.5.1 positional-only call). The scaling technique is FLEXIBLEAUTOEXT by
     default (env ``FHE_BTS_SCALING``; ``fixedmanual`` and FLEXIBLEAUTO no-EXT
     (``flexibleauto``/``flex``/``noext``) are selectable): on fhert's 64-bit native
     all three are supported (fhert bit-exact-validates no-EXT on the device bootstrap
     path, tests/ckks/test_bootstrap.py #99). FLEXIBLEAUTOEXT avoids the FIXEDMANUAL
     scale-drift that blows up resnet20's degree-20 Chebyshev relu; FLEXIBLEAUTO no-EXT
     is the architecture team's resnet20 choice (drops the EXT tower: qnum -1, one more
     usable post-ModRaise limb, still auto-adjusted -- no drift). The ``_FLEX_MODE``
     shim (constant tracked nsd = 2) applies to BOTH FLEX variants (the binding treats
     every non-FIXEDMANUAL technique identically in ModReduce / nsd-bounding); only the
     +1 Compress compensation is EXT-specific, gated on ``_FLEX_EXT`` (FLEX Compress
     drops the EXT auxiliary tower). The host non-bootstrap path stays FIXEDMANUAL, and
     the device path defaults to FLEX (EXT, byte-identical baseline) with FIXEDMANUAL
     and FLEX-no-EXT selectable via ``FHE_DEV_SCALING``. Everything else about the bootstrap context --
     level_budget [4,4], the minimal ``ir_capacity`` chain sizing, the ReSBM ``R``
     down-headroom reserve, ``FHE_BTS_SF``, and the model-driven ring/moduli/depth
     -- is verbatim from rt_openfhe, so the number of usable levels after
     bootstrap is IDENTICAL to the openfhe-python integration.

Fidelity decisions IDENTICAL to `rt_openfhe`:

  * **FIXEDMANUAL leveld-SHE mirror.** IR operators map one-to-one to OpenFHE
    primitives and KEEP rt_ref's ``[strict]`` level/scale invariants: ``add_ciph``
    =EvalAdd, ``sub_*``=EvalSub, ``mul_ciph``=EvalMultNoRelin (-> cipher3, nsd+=),
    ``mul_plain``/``mul_mono_ciph``=EvalMult(ct, pt), ``mul_scalar``=EvalMult(ct,
    float) (nsd+1), ``neg_ciph``=EvalNegate, ``relin``=Relinearize,
    ``rescale_ciph``=Rescale + (if cipher3) immediate Relinearize.
  * **Lazy level bridge.** ``Cipher.level`` is an on-demand ``@property``
    (``_ctx_ir_capacity() - ct.GetLevel()``); no tracked level is stored, so a
    post-bootstrap droplevel-upward drift cannot occur. EvalBootstrap + nsd-reset
    + droplevel-to-target mirrors rt_openfhe exactly (ant-faithful
    ``while(nsd>1) Rescale`` reset, real-nsd tracking).

Lazy import: the native binding is imported INSIDE ``_ensure_of`` (called from
``_seed_ctx``), so merely importing this module never crashes without it.

Modulus-level drops (``mod_switch``; downward ``raise_mod``): the SAME
scale-preserving droplevel as rt_openfhe --
``cc.Compress(ct, towersLeft, noiseScaleDeg=ct.GetNoiseScaleDeg())`` -- bound in
fhert's 1.4.2 build with the identical skip-ModReduce-when-nsd-passed semantics
(probed: nsd preserved, towers dropped). The binding default ``noiseScaleDeg=1``
is the same footgun, so the helper always passes the explicit nsd.
"""

# ---------------------------------------------------------------------------
# configuration + stores (mirror rt_ref's per-run state names so the driver,
# rtlib_py.common.driver, reaches them as plain module attributes)
# ---------------------------------------------------------------------------

import os as _env_os
import struct as _struct  # C binary side-data-file parsing (load_data_entry,
                          # copied verbatim from rt_ref -- provider-agnostic)

# rtpy :rtt timing (mirror rtlib-c rtlib_timing.h) -- emitted by -POLY:rtt/:rtt.
from rtlib_py.common.timing import CMPLR_TM_START, CMPLR_TM_END

_STRICT = True            # default STRICT: [strict] invariant violations raise
_VIOLATIONS = []          # collected in lenient mode
_CTX = {}                 # seeded by the driver from get_context_params()
_INPUTS = {}              # (name, idx) -> Cipher/Plain
_OUTPUTS = {}             # (name, idx) -> Cipher/Plain (set_output_data)
_DEFAULT_SLOTS = 2        # fallback slot count when _CTX has no poly_degree

# Offline data-file descriptor (Phase 2.3, `-CKKS2PY:df=`). Seeded by the driver
# from the emitted module's get_rt_data_info(); None when no data file.
# load_data_entry reads the C binary side data file (`fhe/core/rt_data_def.h`
# format) it names -- copied verbatim from rt_ref (provider-agnostic parser).
_DATA_INFO = None
_DATA_FILE_CACHE = None   # raw bytes, read once per data file

# The real OpenFHE context + keys, built lazily in _seed_ctx. Kept module-level
# (NOT cleared by _reset) so repeated identical contexts reuse them; _seed_ctx
# rebuilds only when the params change.
_CC = None                # openfhe CryptoContext
_PK = None                # public key
_SK = None                # secret key (lazy-decrypt needs it)
_N_SLOTS = 0              # ring_dim // 2 (full packing slot count)
_CTX_PARAMS_KEY = None    # cache key of the currently-built context
_BOOTSTRAP_READY = False  # set True by _seed_ctx when a bootstrap context built
_ROT_KEYS = frozenset()   # rotation indices with generated keys (rotate guard)
_NATIVE = 0               # get_native_int(): 64 or 128
_OF = None                # cached native openfhe binding (from _ensure_of)
_BTS_CALL_N = 0           # bootstrap_ciph call counter (FHE_BTS_PROGRESS monitor)

# ReSBM bottom-bootstrap down-headroom (FHE_BTS_RESERVE) + level budget + scaling
# factor override (FHE_BTS_SF) + correction factor (FHE_BTS_CORRECTION). Verbatim
# from rt_openfhe (env-gated, default = green-gate parity / fhert-safe defaults)
# EXCEPT the correction-factor default is 13 (fhert 1.4.2 SPARSE_TERNARY needs
# an explicit correction factor >= the EvalMod Chebyshev degree; the auto value
# 9 under FIXEDMANUAL is too small and raises "Degree must be <= correction
# factor"). See _build_bootstrap_context.
_BTS_RESERVE = int(_env_os.environ.get("FHE_BTS_RESERVE", "0"))
_BTS_LEVEL_BUDGET = [int(x) for x in _env_os.environ.get(
    "FHE_BTS_LEVEL_BUDGET", "4,4").split(",")][:2]  # s2c/c2s level budget
_BTS_CORRECTION = int(_env_os.environ.get("FHE_BTS_CORRECTION", "13"))
# fhert bootstrap scaling technique. fhert 1.4.2 + 64-bit native supports both
# FIXEDMANUAL and FLEXIBLEAUTOEXT bootstrap (the 1.4.2 FLEXIBLE* throw is
# ``#if NATIVEINT==128`` only). FLEXIBLEAUTOEXT is the DEFAULT (and fhert's own
# canonical bootstrap choice): it eliminates the FIXEDMANUAL scale-drift that
# blows up resnet20's degree-20 Chebyshev relu (a single FLEX bootstrap is
# ~500x more precise than FIXEDMANUAL at the same sf -- probed 5e-6 vs 3e-3,
# because FLEX's AdjustCiphertext reads the real scale and compensates, while
# FIXEDMANUAL blindly assumes the canonical Delta). FLEX keeps the SAME chain
# sizing (technique-independent ``ir_capacity`` formula) so the post-bootstrap
# usable level count is IDENTICAL to rt_openfhe. Override with
# ``FHE_BTS_SCALING=fixedmanual`` to compare. 3a (no bootstrap) is ALWAYS
# FIXEDMANUAL (rt_openfhe green-gate parity); this knob affects only the
# bootstrap context.
_BTS_SCALING = _env_os.environ.get("FHE_BTS_SCALING", "flexibleautoext").lower()
# _FLEX_MODE is True under ANY non-FIXEDMANUAL technique (FLEXIBLEAUTO or
# FLEXIBLEAUTOEXT): the constant-nsd=2 shim (Cipher clamp, ModReduce no-op, bounded
# nsd-reset) applies to both -- the fhert binding treats every non-FIXEDMANUAL
# technique identically there (_ckks_context.py:990-993 ModReduce no-op; nsd bounded
# {1,2}). _FLEX_EXT is True ONLY under FLEXIBLEAUTOEXT: the +1 Compress compensation
# (_drop_to_of_level passes towers_left+1, because FLEX Compress drops the EXT
# auxiliary tower) is EXT-specific; FLEX-no-EXT / FIXEDMANUAL Compress are exact (no
# compensation). Compress gates on _FLEX_EXT, not _FLEX_MODE.
_FLEX_EXT = False
# _parse_flex_scaling: "fixed*" -> FIXEDMANUAL; "noext"/"flexnoext"/"flex"/
# "flexibleauto" -> FLEXIBLEAUTO no-EXT (architecture team's resnet20 choice);
# otherwise (incl. unset default "flexibleautoext") -> FLEXIBLEAUTOEXT. Default is
# EXT on both host (FHE_BTS_SCALING) and device (FHE_DEV_SCALING) so the validated
# baseline stays byte-identical; no-EXT is an explicit opt-in ("flexibleauto" is the
# literal no-EXT OpenFHE name and no longer the prior silent EXT alias).


def _parse_flex_scaling(env_val, default):
    """Map a scaling env value to a tag in {"flexext","flexnoext","fixed"}."""
    v = (env_val if env_val is not None else (default or "")).lower()
    if v.startswith("fixed"):
        return "fixed"
    if v in ("noext", "flexnoext", "flex", "flexibleauto"):
        return "flexnoext"
    return "flexext"
_FLEX_MODE = False          # True under ANY non-FIXEDMANUAL technique, set by
                            # _build_bootstrap_context (host 3b) / _build_device_context
                            # (device). The constant-nsd=2 shim applies to both FLEX
                            # variants (binding: every non-FIXEDMANUAL technique is
                            # ModReduce-no-op, nsd bounded {1,2}); the +1 Compress
                            # compensation is EXT-only (_FLEX_EXT, see below).
_FLEX_SCALE = 2             # under FLEX the noiseScaleDeg is a constant 2 (fresh=1, but
                            # tracked as 2 for all non-zero ciphers -- FLEX auto-manages
                            # the real scale; the level, not nsd, is the tracked quantity)
_CTX_IR_CAPACITY = None   # the IR-mapping basis (chain without the R reserve)

# --- Device (CModelDevice) state -- opt-in via _CTX['device'] or env FHE_DEVICE.
# The CModelDevice is MODULE-LEVEL and LONG-LIVED (built once per device ring_dim,
# reused across _reset / _seed_ctx; a cc rebuild re-runs SetParamsTo +
# SetEvalKeysTo + SetBootstrapPrecomTo on the SAME _DEV so its uploaded CRT/keys
# match the new cc). Host mode (_device_mode() falsy) never touches _DEV.
_DEV = None               # CModelDevice (None in host mode or before first build)
_DEV_RING = 0             # ring_dim the current _DEV was built for (cache check)
# Reclaim diagnostic counter: bumped each _reclaim_bts_device_scratch call; the
# one-shot _run_reclaim_diag fires exactly when this == 2 (after bootstrap #2's
# reclaim) so the leak wrappers from bootstrap #1 are already accumulated and
# the long gc.get_objects() sweep is paid only once. Reset to 0 disables it.
_RECLAIM_DIAG_N = 0
# The validated smallest device config (M0 probe + device-bootstrap gate): the
# device ring_dim is fixed by the CModelDevice grid (num_col*num_row); the
# default MPUH1Config(64,64,1,1) -> ring 4096. Override via ctx['device_ring']
# (+ matching ctx['device_hw'] = (num_col,num_row,*,num_peg)) for a larger ring.
_DEFAULT_DEV_RING = 4096
_DEFAULT_DEV_HW = (64, 64, 1, 1)
# Standard cmodel grids fhert validates (col,row in {64,128,256}). The cmodel's
# bconv/NTT modulus indexing only aligns on these grids: a NON-standard grid
# (e.g. 32x64 for N=2048) mis-indexes -> cmodel "ntt, input a < mod" (an
# unreduced coefficient), and forcing ALL models to N=65536 changes the emitted
# rotation fractions for large-ms (~N/2) models -> the stride-2 MISMATCH. So:
# map each model's emitted poly_degree to the SMALLEST standard grid whose ring
# (col*row) >= N, at peg=8 (the HYBRID-keyswitch VRF-bank capacity fhert's
# test_rotate/test_bootstrap use -- peg=1 has VRF-bank capacity 1 at 256x256,
# too small for the keyswitch's >=2-poly digit slice). Using the model's own
# (or rounded-up for sub-4096 toy N) ring keeps its rotations at their intended
# fractions AND lands on a grid the cmodel validates.
_STD_DEV_GRIDS = (
    (4096,  (64, 64, 1, 8)),
    (8192,  (128, 64, 1, 8)),
    (16384, (128, 128, 1, 8)),
    (32768, (256, 128, 1, 8)),
    (65536, (256, 256, 1, 8)),
)


def _device_grid_for_ring(n):
    """Return ``(ring, (col,row,num_poly,peg))``: the smallest standard cmodel
    grid whose ring (col*row) >= ``n``. Clamps to the largest grid for n>65536."""
    for _ring, _hw in _STD_DEV_GRIDS:
        if _ring >= n:
            return _ring, _hw
    return _STD_DEV_GRIDS[-1]  # n>65536: clamp to 65536 (regress58 max)
# env FHE_DEVICE opts into device mode without touching the emitted ctx (so the
# SAME emitted .py runs on device by setting the env). Host default.
_DEVICE_ON = _env_os.environ.get("FHE_DEVICE", "0") not in ("0", "", "false", "False")


class FakeRtError(Exception):
    """A violated [hard] or (in strict mode) [strict] CKKS constraint."""


def configure(strict=True, on_violation="raise"):
    """Configure strictness -- byte-identical to rt_ref: strict=False records
    [strict] violations to stderr and continues; [hard] errors always raise."""
    global _STRICT
    _STRICT = bool(strict)


def _reset():
    """Clear per-run state (mirror rt_ref). Keeps the built OpenFHE context
    (_CC/_PK/_SK/_N_SLOTS/_CTX_PARAMS_KEY/_BOOTSTRAP_READY/_OF) so repeated
    identical contexts are reused -- the driver calls _seed_ctx after _reset,
    which rebuilds only on a param change."""
    global _VIOLATIONS, _INPUTS, _OUTPUTS, _DATA_INFO, _DATA_FILE_CACHE
    _VIOLATIONS = []
    _INPUTS = {}
    _OUTPUTS = {}
    _DATA_INFO = None
    _DATA_FILE_CACHE = None


def _seed_input(name, idx, cipher):
    """Driver: seed an input cipher/plain the module loads via get_input_data."""
    _INPUTS[(name, idx)] = cipher


def _seed_ctx(ctx_params):
    """Driver: build the real OpenFHE context from the module's
    get_context_params() dict. Imports the binding lazily HERE so module import
    never crashes without it. Reuses the cached context when the params are
    unchanged. ctx_params keys (emitted by poly2py_driver.cxx): provider,
    poly_degree, security_level, mul_depth, input_level, first_prime_bit_num,
    scaling_factor_bit_num, q_part_num, hamming_weight, rotate_keys; a 3b test
    may add ``'bootstrap': True`` to build a real EvalBootstrap context.
    ``FHE_DEVICE=1`` (env) or ``ctx['device']`` opts into CModelDevice mode: the
    emitted poly_degree is then OVERRIDDEN to the device ring_dim (fixed by the
    CModelDevice grid; a too-small ring cannot hold the model's slots)."""
    global _CTX, _CTX_PARAMS_KEY
    _CTX = dict(ctx_params)
    if _DEVICE_ON:
        _CTX["device"] = True
    if _device_mode():
        # The device ring/grid is chosen per model (NOT a single forced N):
        #   * explicit env FHE_DEVICE_HW (col,row,*,peg) overrides everything
        #     (and FHE_DEVICE_RING if also given);
        #   * explicit env FHE_DEVICE_RING (no hw) -> the standard grid for that ring;
        #   * DEFAULT (no env) -> AUTO-SELECT the smallest standard cmodel grid
        #     whose ring >= the emitted poly_degree (see _device_grid_for_ring),
        #     at peg=8. This keeps the model's rotations at their own (intended)
        #     fractions -- forcing every model to N=65536 mangles large-ms
        #     (~N/2) stride-2 rotations -> MISMATCH -- AND lands on a grid the
        #     cmodel validates (non-standard grids mis-index the bconv/NTT).
        hw_env = _env_os.environ.get("FHE_DEVICE_HW")
        ring_env = _env_os.environ.get("FHE_DEVICE_RING")
        if hw_env:
            _CTX["device_hw"] = tuple(int(x) for x in hw_env.split(","))
            if ring_env:
                _CTX["device_ring"] = int(ring_env)
        elif ring_env:
            _ring, _hw = _device_grid_for_ring(int(ring_env))
            _CTX["device_ring"], _CTX["device_hw"] = _ring, _hw
        else:
            _ring, _hw = _device_grid_for_ring(int(_CTX.get("poly_degree") or _DEFAULT_DEV_RING))
            _CTX["device_ring"], _CTX["device_hw"] = _ring, _hw
        # Env FHE_DEVICE_HBM_GB: HBM size per domain, in GB (default 16; the
        # per-domain local address space caps usable values at 64 -- larger
        # values alias across domains). Passed to MPUH1Config as a KEYWORD
        # (the 5th FHE_DEVICE_HW positional is cram_size, NOT HBM). NB the
        # cmodel backs 4 domain replicas with /dev/shm: allow ~4*N GB +
        # ~260 MiB (VRF + constant RAM) of tmpfs headroom.
        _hbm_env = _env_os.environ.get("FHE_DEVICE_HBM_GB")
        if _hbm_env:
            try:
                _hbm = int(_hbm_env)
            except ValueError:
                _violation("hard", "FHE_DEVICE_HBM_GB",
                           f"expected an integer GB value, got {_hbm_env!r}")
            if not 0 < _hbm <= 64:
                _violation("hard", "FHE_DEVICE_HBM_GB",
                           f"expected an integer GB value in 1..64, got "
                           f"{_hbm} (zero/negative is meaningless; above 64 the "
                           f"per-domain local address space aliases across "
                           f"domains). The cmodel allocates 4 domain replicas "
                           f"in /dev/shm: ensure ~4*N GB + ~260 MiB is free.")
            _CTX["device_hbm_gb"] = _hbm
        # The device ring_dim is fixed by the CModelDevice grid (num_col*num_row);
        # OVERRIDE the emitted poly_degree to the device ring (a toy poly_degree
        # mis-indexes the cmodel grid; a too-small ring can't hold the model's
        # slots). input_level/q_part_num/mul_depth keep their emitted values.
        _CTX["poly_degree"] = int(_CTX.get("device_ring") or _DEFAULT_DEV_RING)
    key = _ctx_key(_CTX)
    if key != _CTX_PARAMS_KEY:
        _build_context(_CTX)
        _CTX_PARAMS_KEY = key


def _seed_data_info(info):
    """Driver: seed the offline data-file descriptor (from the emitted module's
    get_rt_data_info()). Copied verbatim from rt_ref."""
    global _DATA_INFO, _DATA_FILE_CACHE
    _DATA_INFO = info
    _DATA_FILE_CACHE = None


def _ctx_mul_depth():
    return int(_CTX.get("mul_depth", 0))


def _ctx_input_level():
    """The input-encryption level for the driver's input ciphers (the driver
    seeds inputs as ``Cipher(values, input_level, 1, False)``).

    Non-bootstrap (3a): ``mul_depth + 1`` is the safe app-top -- the raw field
    is occasionally filled WRONG (E models ``input_level=3`` while
    ``mul_depth=26``), so ``mul_depth+1`` (NOT the raw field) is used.

    Bootstrap (3b, ``ctx['bootstrap']``/``ctx['need_bts']``): use the EMITTED
    ``input_level`` (the deep IR level the compiler lowered the input to) so
    the seeded input matches the chain sizing ``ir_capacity = max(input_level,
    max_target + bts_depth + 1)`` that already uses this emitted ``input_level``
    (``__init__.py:1078`` / ``:1261``). Returning ``mul_depth+1`` here instead
    encrypted the input near the chain TOP, so the first relu-split bootstrap
    saw an input ABOVE its landing (input qnum=21 vs ``qmax - bts_depth = 13``),
    making ``EvalBootstrap`` a no-op that the device asserts on
    (``_ckks_context.py:3489``; OpenFHE-native on host silently returns the
    input and still finishes). Using the emitted ``input_level`` puts the input
    low enough that the bootstrap refreshes and matches the chain assumption.

    NOTE: under the lazy bridge the authoritative chain capacity is
    :func:`_ctx_num_primes`; this helper is only the input-encryption seam."""
    if _CTX.get("bootstrap") or _CTX.get("need_bts"):
        return int(_CTX.get("input_level", _ctx_mul_depth() + 1))
    return _ctx_mul_depth() + 1


def _ctx_num_primes():
    """The REAL prime-chain capacity = ``_CC.GetMultiplicativeDepth() + 1`` (the
    total tower count of the BUILT OpenFHE context). This is the lazy bridge's
    single conversion basis for the two IR-boundaries (``level(a)`` and the
    bootstrap/raise_mod droplevel target) and for the encode/raise_mod ceiling.
    It is ALWAYS ``>= ct.GetLevel()`` (a cipher cannot sit deeper than the
    chain), so ``level(a) >= 0`` ALWAYS -- the structural guarantee that
    eliminates the negative-tracked-level drift an eager bridge produced."""
    return int(_CC.GetMultiplicativeDepth()) + 1 if _CC is not None else _ctx_mul_depth() + 1


def _ctx_ir_capacity():
    """The IR<->openfhe mapping basis = the prime-chain capacity WITHOUT the
    ReSBM bottom-bootstrap reserve R. _build_bootstrap_context sets this to
    ``max(input_level, max_target + bts_depth + 1)``; _build_context (3a) sets
    it to ``num_primes`` directly. For 3a (no bootstrap) this is a no-op and the
    green-gate corpus is byte-identical to rt_openfhe. For 3b, R = max(_BTS_RESERVE,
    2) >= 2 always, so ir_capacity is TRUE chain minus R. Always >= the GetLevel
    of any cipher the .py can observe."""
    if _CTX_IR_CAPACITY is not None:
        return _CTX_IR_CAPACITY
    return _ctx_num_primes()  # pre-build fallback (3a or before _seed_ctx)


def _ctx_q_part_num():
    return int(_CTX.get("q_part_num", 0))


def _default_slot_count():
    # Mirrors rt_ref's shape; rt_fhert actually uses _N_SLOTS (the real ring
    # slot count) for zero_ciph. Provided for driver-glue parity.
    if _N_SLOTS:
        return _N_SLOTS
    pd = int(_CTX.get("poly_degree", 0))
    return pd // 2 if pd > 0 else _DEFAULT_SLOTS


def _device_mode():
    """True when the seeded _CTX opted into CModelDevice acceleration
    (``_CTX['device']`` truthy, set by an explicit ``ctx['device']`` or by the
    ``FHE_DEVICE`` env). All device seams dispatch on this (NOT on
    ``_DEV is not None``, which persists across a later host-mode _seed_ctx)."""
    return bool(_CTX.get("device"))


def _is_dev_ct(ct):
    """True when ``ct`` is a device-side OpenFHE Ciphertext (``_device`` set).
    Native ``Clone()`` strips this (returns a host ct), so device-aware code
    round-trips through host to clone; see Cipher.clone()."""
    return getattr(ct, "_device", None) is not None


# ---------------------------------------------------------------------------
# constraint helpers -- copied VERBATIM from rt_ref/rt_openfhe so [hard]/[strict]
# semantics (and the level/scale [strict] invariants that catch compiler bugs)
# are byte-identical. Real HE itself stays silent on misuse; these checks
# surface it. (FakeRtError is this module's own class, but the message text is
# identical so the ported unit tests match on the same substrings.)
# ---------------------------------------------------------------------------

def _violation(kind, op, msg):
    full = f"[{kind}] {op}: {msg}"
    if kind == "hard":
        raise FakeRtError(full)
    if _STRICT:
        raise FakeRtError(full)
    _VIOLATIONS.append(full)
    import sys
    print(f"fake_rt WARN: {full}", file=sys.stderr)


def _require_cipher(a, op):
    if not isinstance(a, Cipher):
        _violation("hard", op, f"expected cipher, got {type(a).__name__}")


def _require_cipher_or_cipher3(a, op):
    if not isinstance(a, Cipher):
        _violation("hard", op, f"expected cipher/cipher3, got {type(a).__name__}")


def _require_plain(p, op):
    if not isinstance(p, (Plain,)):
        _violation("hard", op, f"expected plain, got {type(p).__name__}")


def _reject_cipher3(a, op):
    # A cipher3 (post mul_ciph, pre relin) may only feed relin/rescale (which
    # downgrade it). Every other consumer must see a relined cipher.
    if isinstance(a, Cipher) and a.is_cipher3:
        _violation("hard", op, "cipher3 operand must be relined first")


def _check_level_match(a, b, op):
    # No-op: ant's Add/Sub/Mul_ciphertext auto-align mismatched-level operands
    # via Adjust_level. rt_fhert mirrors this with a REAL scale-preserving
    # droplevel of the higher-ir-level operand before the Eval* call (see
    # _align_ct_for_binop); the tracked result level is min.
    return


def _check_scale_match(a, b, op):
    if a.scale_deg == 0 or b.scale_deg == 0:
        return
    if a.scale_deg != b.scale_deg:
        _violation("strict", op,
                   f"scale_deg mismatch: {a.scale_deg} != {b.scale_deg}")


# ---------------------------------------------------------------------------
# value classes
# ---------------------------------------------------------------------------

class Cipher:
    """A real CKKS cipher wrapping an OpenFHE ``Ciphertext`` plus the tracked
    CKKS state ``(scale_deg, is_cipher3)`` and the number of *logical* slots
    (the meaningful message length -- a real cipher always holds the full
    ``_N_SLOTS`` ring slots; we pad inputs with 0 and truncate the decrypted
    vector back to the logical length so the verify gate compares like rt_ref).

    ``_slots`` is three-state: ``None`` = not yet decrypted (lazy-decrypt on
    first ``.slots`` access via the secret key), ``list[float]`` = decoded
    values, ``list[Cipher]`` = an array container (array-typed input store/load
    via ``__getitem__``/``__setitem__`` -- then ``_ct`` is ``None``).

    The constructor is the **encryption seam**: the driver builds input ciphers
    as ``Cipher([float values], input_level, 1, False)``, so a list of floats
    triggers ``MakeCKKSPackedPlaintext`` + ``Encrypt`` at openfhe level
    ``_of_level(level) = ir_capacity - level`` (openfhe level 0 = the chain top
    for a fresh 3a input; the app-top for a 3b input), nsd 1. ``level`` is
    consumed here ONLY to pick the encryption level and is then DISCARDED -- the
    IR REMAINING level is never stored (see ``level`` property below). Arithmetic
    ops build Ciphers via ``_wrap`` (no re-encryption, no level arg)."""

    __slots__ = ("_ct", "_slots", "scale_deg", "is_cipher3", "_n_logical")
    # NOTE: no ``level`` slot -- under the lazy bridge the IR REMAINING level is
    # NOT stored; it is computed on demand by the ``level`` property below
    # (``_ctx_ir_capacity() - ct.GetLevel()``). Storing a tracked level is what
    # let it drift from the real cipher level (post-bootstrap droplevel-upward);
    # on-demand has nothing to drift.

    def __init__(self, slots, level, scale_deg, is_cipher3=False):
        # ``level`` is the IR REMAINING level of the ENCRYPTION SEAM only. It is
        # consumed HERE to pick the openfhe encryption level (``_of_level``) and
        # then DISCARDED -- it is never stored; ``self.level`` later recomputes
        # it on demand. So a stored tracked level can never go stale (no drift).
        self.scale_deg = int(scale_deg)
        # Under FLEXIBLEAUTOEXT the noiseScaleDeg is a constant (2) -- FLEX
        # auto-manages the real scale; the level (lazy bridge) is the tracked
        # quantity. Track the constant for all non-zero ciphers (keep 0 for the
        # zero-operand sentinel). FIXEDMANUAL tracks the real nsd (sd_a+sd_b / -1).
        if _FLEX_MODE and self.scale_deg != 0:
            self.scale_deg = _FLEX_SCALE
        self.is_cipher3 = bool(is_cipher3)
        if isinstance(slots, list) and slots and isinstance(slots[0], Cipher):
            # Array container: a Python list of Ciphers (array-typed input).
            self._ct = None
            self._slots = [c.clone() for c in slots]
            self._n_logical = len(self._slots)
        elif isinstance(slots, list) and not slots:
            # Empty (zero_ciph): encrypt N real zeros at (level, scale_deg).
            # zero_ciph passes scale_deg=0 (the unfixed-scale sentinel, matching
            # rt_ref) so add/sub_ciph short-circuit a zero operand and SKIP the
            # real EvalAdd/EvalSub -- real CKKS cannot add mismatched-scale
            # ciphers, so `add(zero_ciph(), term)` (term at scale_deg 2 from
            # mul) would otherwise trip [strict] scale_deg mismatch.
            # ``_n_logical = 0`` is the WILDCARD/identity sentinel: a zero
            # cipher's meaningful shape is defined by its operand (it adds as
            # the + / - identity, rt_ref `_bin_slots` empty-broadcast parity),
            # so add/sub_ciph(zero, x) returns x's shape rather than a [hard]
            # slot-count mismatch. The real op runs on the full ring slots
            # either way (the zero's real ciphertext holds N encrypted zeros).
            self._ct = _encrypt_values([0.0] * _N_SLOTS, _of_level(level),
                                       int(scale_deg))
            self._slots = None
            self._n_logical = 0
        else:
            # Value list: encrypt (the input-seam path). Pad to the full ring
            # slot count with 0 and remember the logical length for truncation.
            vals = [float(x) for x in slots]
            n = len(vals)
            padded = vals + [0.0] * (_N_SLOTS - n) if _N_SLOTS else vals
            self._ct = _encrypt_values(padded[:max(_N_SLOTS, n)],
                                       _of_level(level),
                                       int(scale_deg))
            self._slots = None
            self._n_logical = n

    @property
    def level(self):
        """The IR REMAINING level, on demand from the REAL ciphertext:
        ``_ctx_ir_capacity() - ct.GetLevel()``. OpenFHE ``GetLevel()`` starts at
        0 at the chain top and grows by 1 per Rescale, so the REMAINING level
        (headroom down to 0) is the IR-mapping capacity minus the consumed depth.
        The basis is ``_ctx_ir_capacity()`` (the chain WITHOUT the R down-headroom
        reserve; == ``_ctx_num_primes()`` only for 3a / R==0), not the emitted
        ``input_level``, which keeps this ``>= 0`` ALWAYS -- a cipher cannot be
        deeper than the IR region. An array container (``_ct is None``) reports
        the full chain top."""
        if self._ct is not None:
            return _ctx_ir_capacity() - int(self._ct.GetLevel())
        return _ctx_ir_capacity()

    @classmethod
    def _wrap(cls, ct, scale_deg, is_cipher3, n_logical):
        """Wrap a PRODUCED openfhe Ciphertext (arithmetic ops use this; no
        re-encryption). No ``ir_level`` argument: under the lazy bridge the
        result level is NEVER stored -- it is recomputed on demand by the
        ``level`` property from ``ct.GetLevel()``; the property reflects the
        ciphertext's actual consumed depth, so it cannot drift."""
        c = cls.__new__(cls)
        c._ct = ct
        c._slots = None
        c.scale_deg = int(scale_deg)
        if _FLEX_MODE and c.scale_deg != 0:
            c.scale_deg = _FLEX_SCALE      # FLEX: constant nsd (see __init__)
        c.is_cipher3 = bool(is_cipher3)
        c._n_logical = int(n_logical)
        return c

    @property
    def slots(self):
        """The decoded message slots. Lazy-decrypts the openfhe ciphertext on
        first access (needs the secret key from _seed_ctx) and caches; truncates
        to the logical length so the verify gate compares the meaningful
        message. Array containers return their list of Ciphers as-is."""
        if self._slots is None:
            if self._ct is not None:
                self._slots = _decrypt_slots(self._ct)  # full _N_SLOTS
            else:
                self._slots = []
        if self._ct is not None and isinstance(self._slots, list) and \
                self._slots and not isinstance(self._slots[0], Cipher):
            return self._slots[:self._n_logical]
        return self._slots

    def clone(self):
        if self._ct is not None:
            # Native Clone() strips device-ness (returns a host ct), so on a
            # device ct round-trip through host: to(None) downloads, Clone()
            # copies the host ct, to(_DEV) re-uploads (bit-exact round-trip).
            if _device_mode() and _is_dev_ct(self._ct):
                ct = self._ct.to(None).Clone().to(_DEV)
            else:
                ct = self._ct.Clone()
            c = Cipher._wrap(ct, self.scale_deg,
                             self.is_cipher3, self._n_logical)
            if isinstance(self._slots, list):
                c._slots = list(self._slots)  # copy any cached decoded values
            return c
        # Array container: deep-clone each element. ``self.level`` (on-demand
        # property) is accepted by the ctor's array path but unused there.
        return Cipher([c.clone() for c in self._slots], self.level,
                      self.scale_deg, self.is_cipher3)

    def __getitem__(self, idx):
        # Array-cipher element load (rt_ref parity): return a clone so
        # downstream ops do not mutate the array element.
        return self._slots[idx].clone()

    def __setitem__(self, idx, val):
        if isinstance(val, Cipher):
            self._slots[idx] = val.clone()
        else:
            self._slots[idx] = val

    def __repr__(self):
        try:
            s = self.slots
        except Exception:
            s = "<undecryptable>"
        return (f"Cipher(slots={s}, level={self.level}, "
                f"scale_deg={self.scale_deg}, is_cipher3={self.is_cipher3})")


class Plain:
    """A plaintext: logical message values + (level, scale_deg). The openfhe
    ``Plaintext`` is built lazily at the consuming cipher's level/scale (a
    plain must match its operand cipher for EvalAdd/EvalMult)."""

    __slots__ = ("_slots", "_pt", "level", "scale_deg")

    def __init__(self, slots, level, scale_deg):
        self._slots = [float(x) for x in slots]
        self._pt = None
        self.level = int(level)
        self.scale_deg = int(scale_deg)

    @property
    def slots(self):
        return list(self._slots)

    def clone(self):
        return Plain(list(self._slots), self.level, self.scale_deg)

    def __repr__(self):
        return (f"Plain(slots={self._slots}, level={self.level}, "
                f"scale_deg={self.scale_deg})")


# ---------------------------------------------------------------------------
# openfhe-primitive helpers (module-level; use the globals set by _seed_ctx)
# ---------------------------------------------------------------------------

def _of_level(ir_level):
    """Map an IR REMAINING level to the openfhe CONSUMED GetLevel. OpenFHE
    ``GetLevel()`` starts at 0 at the chain top and grows by 1 per Rescale; an
    IR REMAINING level counts DOWN from the chain top. The chain top is
    :func:`_ctx_ir_capacity` (the IR-mapping capacity, i.e. the chain WITHOUT the
    R down-headroom reserve), so ``of_level = _ctx_ir_capacity() - ir_level``.
    Single lazy-bridge conversion: same basis as ``level(a)`` and the
    bootstrap/raise_mod droplevel targets. For a 3a context
    ``_ctx_ir_capacity() == _ctx_num_primes() == mul_depth+1 == _ctx_input_level()``,
    byte-identical to the old ``input_level - ir_level`` for the green-gate
    corpus; only bootstrap contexts see a change (the structural drift fix)."""
    return _ctx_ir_capacity() - int(ir_level)


def _drop_to_of_level(ct, target_gl, nsd, op):
    """Scale-preserving droplevel (a.k.a. mod_switch): drop the openfhe
    ``Ciphertext`` ``ct`` DOWN to openfhe GetLevel ``target_gl`` (CONSUMED
    space: raising GetLevel by dropping towers) while keeping
    ``noiseScaleDeg = nsd`` unchanged -- the real-CKKS analog of a modulus-level
    drop that preserves the scale (rt_ref ``mod_switch`` / downward
    ``raise_mod``: SET the level, KEEP the scale degree).

    Implementation: ``cc.Compress(ct, towersLeft, noiseScaleDeg=nsd)`` with the
    CURRENT nsd passed EXPLICITLY. The binding default ``noiseScaleDeg=1`` is a
    footgun: it first ModReduces the cipher to nsd=1 (changing the scale) before
    LevelReduce -- that would turn this into a rescale, NOT a mod_switch
    (verified: a cipher3 at nsd=2 + default Compress -> nsd 1). With the nsd
    passed explicitly the ``while (nsd > noiseScaleDeg) ModReduce`` loop is
    skipped and only ``LevelReduceInternalInPlace`` runs (a pure tower drop,
    scale unchanged). Probed on fhert 1.4.2: nsd preserved, towers dropped --
    identical to rt_openfhe/upstream.

    ``Compress``'s ``towersLeft`` is the towers REMAINING (not the count to
    drop); the total is ``_ctx_num_primes()`` = ``GetMultiplicativeDepth()+1``
    (the REAL built chain). Droplevel can only drop towers (raise GetLevel), so
    ``target_gl`` must be ``>= ct.GetLevel()`` and ``< total``; a target below
    the current level (an upward raise) is a caller bug -> [hard] (needs a
    bootstrap, not a drop). Works on a cipher3 (3 unrelin components): Compress
    operates per-component and keeps all 3 (verified), so the caller preserves
    ``is_cipher3``."""
    total = _ctx_num_primes()                              # real total tower count
    cur_gl = int(ct.GetLevel())
    if target_gl < cur_gl or target_gl >= total:
        _violation("hard", op,
                   f"droplevel target GetLevel {target_gl} out of reachable "
                   f"[{cur_gl}, {total - 1}] (can only drop towers, not raise; "
                   f"an upward target needs bootstrap)")
    towers_left = total - target_gl
    if towers_left < 1:
        _violation("hard", op, f"towers_left {towers_left} < 1 (underflow)")
    # Compress asserts towersLeft >= noiseScaleDeg (a cipher at nsd `n` needs at
    # least `n` towers to hold its scale chain, so it cannot be dropped below n
    # towers). Convert that openfhe RuntimeError into a clean [hard].
    if towers_left < int(nsd):
        _violation("hard", op,
                   f"towers_left {towers_left} < noiseScaleDeg {int(nsd)}: a "
                   f"{int(nsd)}-scale cipher needs at least {int(nsd) + 1} "
                   f"towers to drop one level (Compress requires "
                   f"towersLeft>=nsd); this context (total {total} towers, "
                   f"target GetLevel {target_gl}) is too shallow. "
                   f"mod_switch/raise_mod on a high-nsd (e.g. cipher3) cipher "
                   f"needs a deeper multiplicative depth.")
    # FLEXIBLEAUTOEXT Compress compensation: fhert 1.4.2 FLEX Compress(ct,
    # towersLeft=X) drops ONE EXTRA tower (yields X-1 towers, leaving the cipher 1
    # level deeper than requested), because FLEXIBLEAUTOEXT reserves the EXT
    # auxiliary modulus for the flexible first-level scale. To land at the INTENDED
    # target_gl (towers = towers_left), pass towers_left+1 so Compress yields
    # towers_left towers. Without this, every droplevel (mod_switch / downward
    # raise_mod / binop _align / the bootstrap droplevel) lands 1 level too deep
    # under FLEX-EXT, and the per-bootstrap 1-level deficit accumulates to level
    # exhaustion (resnet20). This is EXT-specific: FLEXIBLEAUTO (no-EXT) and
    # FIXEDMANUAL Compress are EXACT (no extra tower), so the compensation gates on
    # _FLEX_EXT, not _FLEX_MODE (probed exact under FLEX-EXT vs FIXEDMANUAL;
    # FLEX-no-EXT must behave like FIXEDMANUAL here -- verify on device). The
    # guards above use the INTENDED towers_left (the real tower count the cipher
    # will hold), so they stay correct.
    compress_towers = towers_left + 1 if _FLEX_EXT else towers_left
    # Device path has NO Compress (the cmodel device exposes no Compress): a
    # device ct cannot be Compressed in place. Fall back to the host primitive
    # -- download (to(None)) -> host Compress (scale-preserving droplevel) ->
    # re-upload (to(_DEV)). Host path uses Compress directly.
    if _device_mode() and _is_dev_ct(ct):
        host_ct = ct.to(None)
        if _env_os.environ.get("FHE_DEV_DEBUG"):
            import sys as _sys
            try:
                _hl = int(host_ct.GetLevel())
            except Exception:
                _hl = "?"
            print(f"[FHE_DEV_DEBUG] devCompress: compress_towers={compress_towers} "
                  f"nsd={int(nsd)} flex={_FLEX_MODE} total={total} target_gl={target_gl} "
                  f"cur_gl={cur_gl} towers_left={towers_left} host_ct.GetLevel()={_hl}",
                  file=_sys.stderr, flush=True)
        dropped = _CC.Compress(host_ct, compress_towers, noiseScaleDeg=int(nsd))
        return dropped.to(_DEV)
    if _env_os.environ.get("FHE_DEV_DEBUG"):
        import sys as _sys
        _isd = _is_dev_ct(ct) if _device_mode() else "n/a"
        try:
            _cl = int(ct.GetLevel())
        except Exception:
            _cl = "?"
        print(f"[FHE_DEV_DEBUG] hostCompress: compress_towers={compress_towers} "
              f"nsd={int(nsd)} flex={_FLEX_MODE} total={total} target_gl={target_gl} "
              f"cur_gl={cur_gl} towers_left={towers_left} _is_dev_ct={_isd} "
              f"_device_addr={getattr(ct, '_device_address', 'NOATTR')} "
              f"ct.GetLevel()={_cl}",
              file=_sys.stderr, flush=True)
    return _CC.Compress(ct, compress_towers, noiseScaleDeg=int(nsd))


def _align_ct_for_binop(a, b, op):
    """Mirror ant ``Adjust_level`` (ciphertext.h:203-238) for a binary cipher op
    so both ciphertexts sit at the SAME openfhe GetLevel for EvalAdd/EvalSub/
    EvalMultNoRelin: drop the FRESHER operand (LOWER GetLevel == MORE headroom)
    DOWN to the more-depleted operand's GetLevel. In CONSUMED space the target
    is simply ``max(a.GetLevel(), b.GetLevel())``. The equal-level case is a
    no-op pass-through (green-gate add/mul unchanged). Returns ``(a_ct, b_ct)``.

    A zero operand (``scale_deg == 0``) is a pass-through waiver: the real op is
    short-circuited by add/sub_ciph before reaching here, and mul_ciph on a zero
    broadcasts through unchanged."""
    if a.scale_deg == 0 or b.scale_deg == 0:
        return a._ct, b._ct
    a_gl = int(a._ct.GetLevel())
    b_gl = int(b._ct.GetLevel())
    if a_gl == b_gl:
        return a._ct, b._ct
    target_gl = max(a_gl, b_gl)
    if a_gl < b_gl:
        a_ct = _drop_to_of_level(a._ct, target_gl,
                                 int(a._ct.GetNoiseScaleDeg()), op)
        return a_ct, b._ct
    b_ct = _drop_to_of_level(b._ct, target_gl,
                             int(b._ct.GetNoiseScaleDeg()), op)
    return a._ct, b_ct


def _encrypt_values(vals, of_level, nsd):
    """MakeCKKSPackedPlaintext(vals, nsd, of_level, slots=N) + Encrypt(PK). In
    device mode the input cipher is uploaded to the CModelDevice (host Encrypt
    then ``ct.to(_DEV)``); downstream arithmetic then auto-dispatches to the
    device kernels via the device-patch layer."""
    pt = _CC.MakeCKKSPackedPlaintext(vals, noiseScaleDeg=int(nsd),
                                    level=int(of_level), slots=_N_SLOTS)
    ct = _CC.Encrypt(_PK, pt)
    if _device_mode():
        ct = ct.to(_DEV)
    return ct


def _decrypt_slots(ct):
    """Decrypt to a float list of the full ring slot count (lazy-decrypt). In
    device mode ``ct`` is device-side: ``cc.Decrypt`` is host-only and a device
    ct's ``GetElements`` raises, so download via ``ct.to(None)`` first (bit-exact
    round-trip). The original device ct is unchanged (``to(None)`` returns a NEW
    host ct)."""
    if _device_mode() and _is_dev_ct(ct):
        ct = ct.to(None)
    pt = _CC.Decrypt(_SK, ct)
    pt.SetLength(_N_SLOTS)
    return [float(x) for x in pt.GetRealPackedValue()]


def _plain_packed_for(cipher, plain, op):
    """Build the N-length value list a plain contributes to `cipher`'s slots,
    broadcasting a 1-slot plain across all logical slots or doing an elementwise
    op when the plain packs the same-count/shorter/longer (rt_ref
    `_broadcast_plain` + ant Mul_plaintext parity), then padding to the full
    ring. Raises [hard] only on a 1-but-mismatched unbroadcastable count. A
    zero_ciph (wildcard `_n_logical==0`) adopts the plain's shape."""
    p = [float(x) for x in plain.slots]
    n = cipher._n_logical
    if n == 0:
        # wildcard zero cipher: the plain defines the result shape.
        n = len(p) if p else 1
    if not p:
        meaningful = [0.0] * n
    elif len(p) == 1:
        meaningful = [p[0]] * n
    else:
        # Elementwise over the cipher's logical slots, plain zero-extended (if
        # shorter) or truncated (if longer) to n -- mirrors ant Mul_plaintext
        # (full-ring Mul_poly with the plain zero-padded on the right).
        meaningful = (list(p) + [0.0] * n)[:n]
    return meaningful + [0.0] * (_N_SLOTS - len(meaningful))


def _cipher_plain_result_n(cipher, plain):
    """Logical slot count of a plaintext op's result: the cipher's count, or
    (for a wildcard zero_ciph) the plain's count, which the plain defines."""
    return cipher._n_logical if cipher._n_logical else len(plain.slots)


def _plain_pt(cipher, plain, nsd, op):
    """Re-encode `plain` at `cipher`'s openfhe level and the given nsd, so the
    openfhe EvalAdd/EvalMult level/scale preconditions hold. Returns an openfhe
    Plaintext packed into the full ring slots."""
    vals = _plain_packed_for(cipher, plain, op)
    return _CC.MakeCKKSPackedPlaintext(vals, noiseScaleDeg=int(nsd),
                                      level=_of_level(cipher.level),
                                      slots=_N_SLOTS)


def _plain_for_op(cipher, plain, nsd, op):
    """The operand-plain for an EvalAdd/EvalSub/EvalMult: ``_plain_pt`` re-encoded
    at the cipher's level/nsd. In device mode the plain is UPLOADED to the
    CModelDevice (``pt.to(_DEV)``) so ``cc.EvalAdd(ct, pt)`` auto-dispatches to
    the device ``EvalAddPlain`` (which asserts ``plain._device is dev``). Host
    mode returns the host Plaintext as-is."""
    pt = _plain_pt(cipher, plain, nsd, op)
    if _device_mode():
        pt = pt.to(_DEV)
    return pt


# ---------------------------------------------------------------------------
# binding import (lazy; cached on identical params) -- the fhert path-injection
# seam, NOT rt_openfhe's OPENFHE_PY_BUILD_DIR.
# ---------------------------------------------------------------------------

# Toy-safety cap on the per-prime scaling bit size. The compiler emits
# scaling_factor_bit_num=56 (a simulation-era value chosen without real-HE
# precision in mind). Under real CKKS at the toy ring dims the green-gate uses
# (poly_degree as small as 4), SetMultiplicativeDepth(0) builds a modulus of
# just the first prime (~60 bits), so a 56-bit scaling leaves only ~4 integer
# bits and values >= 16 wrap (garbage, not low precision). Capping the prime
# SIZE (not its count) to 40 restores integer+fractional precision (values up
# to ~2^20 decrypt within ~1e-10) while leaving the count-based level/scale_deg
# bookkeeping and the [strict] invariant checks -- which count primes, not
# their sizes -- fully intact. Verbatim from rt_openfhe (identical corpus).
_SCALE_BIT_CAP = 40


def _ensure_of():
    """Locate and import the fhert OpenFHE 1.4.2 binding lazily, caching it in
    the module global ``_OF``. The fhert native bindings (``fhert/_C/_lib.so``
    OpenFHE 1.4.2 + ``fhert/_C/libfhert.so`` IR core) are built for CPython 3.12
    (bazel ``--@rules_python//python/config_settings:python_version=3.12``,
    ``--distdir=/workspace/fhert/bazel-distdir``), so ``import fhert.openfhe``
    loads natively on this interpreter and brings the device-patch layer
    (``_ciphertext``/``_crypto_context``/``_plaintext``) with it -- both host
    mode and (when ``_CTX['device']`` is set) device mode route through the
    native classes. NB: the prebuilt ``libfhert.so`` was originally a cp313
    build whose module-init eagerly called the 3.13-only symbol
    ``PyThreadState_GetUnchecked`` (3.12 libpython does not export it), so the
    full ``import fhert`` failed on 3.12 and a stub-bypass fallback to the
    host-only ``fhert._C._lib.openfhe`` submodule was needed; after the cp312
    rebuild of ``_lib`` + ``libfhert`` the native import works and the fallback
    was removed (the stub-bypass was confirmed redundant: native
    ``import fhert``/``import fhert.openfhe``/``from fhert.backends import
    CModelDevice`` all succeed on the cp312 build). If the binding cannot be
    imported (missing FHERT_ROOT), the ImportError propagates -- rtlib-py tests
    then skip rather than crash."""
    global _OF
    if _OF is not None:
        return _OF
    import sys as _sys
    _fr = _env_os.environ.get("FHERT_ROOT") or "/workspace/fhert"
    if _env_os.path.isdir(_fr) and _fr not in _sys.path:
        _sys.path.insert(0, _fr)
    import fhert.openfhe as _of  # native cp312 import; brings the device-patch layer
    _OF = _of
    return _of


def _ctx_key(ctx):
    """Cache key for the openfhe context build (only the params that change the
    context; the bootstrap flag + knobs force a rebuild)."""
    return (
        ctx.get("provider"),
        int(ctx.get("poly_degree", 0)),
        int(ctx.get("mul_depth", 0)),
        int(ctx.get("input_level", 0)),
        int(ctx.get("hamming_weight", 0)),
        int(ctx.get("first_prime_bit_num", 0)),
        min(int(ctx.get("scaling_factor_bit_num", 0)), _SCALE_BIT_CAP),
        int(ctx.get("q_part_num", 0)),
        bool(ctx.get("bootstrap", False)),
        int(ctx.get("max_bts_target", 0)),
        int(_BTS_RESERVE),
        tuple(_BTS_LEVEL_BUDGET),
        int(_BTS_CORRECTION),
        int(_env_os.environ.get("FHE_BTS_SF", "0")),
        _BTS_SCALING,
        tuple(ctx.get("rotate_keys", []) or []),
        bool(ctx.get("device", False)),                       # device vs host rebuild
        int(ctx.get("device_ring", 0)),
        tuple(ctx.get("device_hw") or ()),
        int(ctx.get("device_hbm_gb", 0)),                     # FHE_DEVICE_HBM_GB
    )


def _build_context(ctx):
    """Build (or rebuild) the OpenFHE CryptoContext + keys from ctx params.
    Imports the binding lazily via _ensure_of. A ``ctx['bootstrap']`` truthy
    flag (or ``ctx['need_bts']``) builds the real EvalBootstrap context instead
    of the plain leveld-SHE one (3a)."""
    global _CC, _PK, _SK, _N_SLOTS, _BOOTSTRAP_READY, _ROT_KEYS, _NATIVE, _CTX_IR_CAPACITY, _OF, _FLEX_MODE, _FLEX_EXT
    _of = _ensure_of()
    _NATIVE = int(_of.get_native_int())
    if _device_mode():
        _build_device_context(_of, ctx)
        return
    if ctx.get("bootstrap") or ctx.get("need_bts"):
        _build_bootstrap_context(_of, ctx)
        return
    # 3a: plain leveld-SHE FIXEDMANUAL context, faithful to the emitted params
    # modulo the toy-safety scaling cap.
    sc = min(int(ctx.get("scaling_factor_bit_num", 50)), _SCALE_BIT_CAP)
    params = _of.CCParamsCKKSRNS()
    params.SetScalingTechnique(_of.FIXEDMANUAL)                # decision #2 (bare: fhert re-exports FIXEDMANUAL, NOT ScalingTechnique)
    params.SetKeySwitchTechnique(_of.KeySwitchTechnique.HYBRID)   # class IS exported
    poly_deg = int(ctx.get("poly_degree", 1 << 4))
    params.SetMultiplicativeDepth(int(ctx.get("mul_depth", 0)))
    params.SetFirstModSize(int(ctx.get("first_prime_bit_num", 60)))
    params.SetScalingModSize(sc)
    params.SetRingDim(poly_deg)
    params.SetSecurityLevel(_of.HEStd_NotSet)  # toy poly_degree (bare re-export)
    params.SetBatchSize(poly_deg // 2)                        # slot count = N/2
    cc = _of.GenCryptoContext(params)
    for f in (_of.PKE, _of.KEYSWITCH, _of.LEVELEDSHE):
        cc.Enable(f)
    keys = cc.KeyGen()
    cc.EvalMultKeyGen(keys.secretKey)
    rot = list(ctx.get("rotate_keys", []) or [])
    if rot:
        cc.EvalRotateKeyGen(keys.secretKey, rot)
    _CC, _PK, _SK = cc, keys.publicKey, keys.secretKey
    _N_SLOTS = cc.GetRingDimension() // 2
    _ROT_KEYS = frozenset(rot)
    _BOOTSTRAP_READY = False
    _FLEX_MODE = False                            # 3a is always FIXEDMANUAL
    _FLEX_EXT = False                          # 3a -> FIXEDMANUAL (no EXT compensation)
    # 3a: no bootstrap reserve applies; IR capacity == TRUE chain.
    _CTX_IR_CAPACITY = int(_CC.GetMultiplicativeDepth()) + 1


def _build_bootstrap_context(of, ctx):
    """Build the model-driven EvalBootstrap context. Ring dimension, first/moduli
    bit sizes, and multiplicative depth are derived from the emitted
    get_context_params (ctx: poly_degree / first_prime_bit_num /
    scaling_factor_bit_num / mul_depth / hamming_weight) -- NOT toy constants.
    Uses FLEXIBLEAUTOEXT by default (env ``FHE_BTS_SCALING``; ``fixedmanual`` and
    FLEXIBLEAUTO no-EXT (``flexibleauto``/``flex``/``noext``) are selectable): on
    fhert's 64-bit native all three are supported, and any FLEX variant avoids the
    FIXEDMANUAL scale-drift that blows up resnet20's degree-20 Chebyshev relu.
    FLEX-no-EXT is the architecture team's resnet20 recipe (drops the EXT tower:
    qnum -1, one more usable post-ModRaise limb). The ``_FLEX_MODE`` shim (constant
    tracked nsd = 2) applies to both FLEX variants (binding treats every
    non-FIXEDMANUAL technique identically in ModReduce / nsd-bounding); the +1
    Compress compensation is EXT-specific (gated on ``_FLEX_EXT``), so the
    lazy-bridge tracking + nsd-reset stay consistent under any FLEX choice.

    fhert-1.4.2-specific deltas vs rt_openfhe (see module docstring delta #3):
      * secret-key distribution SPARSE_TERNARY (NOT UNIFORM_TERNARY): the dense
        UNIFORM key raises ``Degree must be <= correction factor`` and gives poor
        refresh precision at packing slots; SPARSE_TERNARY (small Hamming weight)
        keeps the EvalMod Chebyshev degree low and the sine accurate.
      * an explicit ``correctionFactor`` (env FHE_BTS_CORRECTION, default 13 --
        the clamp max) is passed to EvalBootstrapSetup: under FIXEDMANUAL the
        auto correction factor is 9, smaller than the EvalMod degree, so the
        explicit value is REQUIRED for the bootstrap to run.

    Everything else is verbatim from rt_openfhe: level_budget [4,4], the minimal
    ``ir_capacity`` chain sizing, the R down-headroom reserve, FHE_BTS_SF, the
    model rotation keys, and the Enable(PKE/KEYSWITCH/LEVELEDSHE/ADVANCEDSHE/FHE)
    order. The post-bootstrap usable-level count is therefore IDENTICAL to
    rt_openfhe (same ir_capacity formula; the larger SPARSE bts_depth only DEEPENS
    the chain, it does not change the compiler-fixed app levels between
    bootstraps). Mirrors examples/pke/simple-ckks-bootstrapping.py setup order."""
    global _CC, _PK, _SK, _N_SLOTS, _BOOTSTRAP_READY, _ROT_KEYS, _CTX_IR_CAPACITY, _FLEX_MODE, _FLEX_EXT
    params = of.CCParamsCKKSRNS()
    secret_key_dist = of.SecretKeyDist.SPARSE_TERNARY       # fhert 1.4.2 (delta #3)
    params.SetSecretKeyDist(secret_key_dist)
    params.SetSecurityLevel(of.HEStd_NotSet)                # bare re-export
    params.SetRingDim(int(ctx.get("poly_degree", 1 << 12)))
    # native-int-aware moduli default, overridable by the model's prime/scaling
    # bit nums (64-bit: first 60 / dcrt 59; 128-bit: 89 / 78).
    _native128 = (of.get_native_int() == 128)
    first_mod = int(ctx.get("first_prime_bit_num", 89 if _native128 else 60))
    dcrt_bits = int(ctx.get("scaling_factor_bit_num", 78 if _native128 else 59))
    # FHE_BTS_SF overrides the scaling-modulus bit size on the bootstrap path.
    # The .py emits the compiler's simulation-era sf=56; FIXEDMANUAL at a deep
    # bootstrap chain suffers scaling-factor drift (q_i vs Delta), and sf=56
    # gives only ~6-bit refresh precision at the resnet20 bootstrap levels.
    # openfhe's own 64-bit bootstrap recommendation is FIXEDMANUAL + sf=59 (see
    # doc/.../openfhe-bts-scaling-mode.md sec.17): ~9-10 bit refresh (vs 6 at sf=56),
    # below the degree-20 Chebyshev relu blowup threshold. sf is a per-prime
    # SIZE, not a count, so it does NOT change the post-bootstrap available
    # level count. Default = the .py's emitted value; set FHE_BTS_SF=59 for
    # resnet20. Verbatim from rt_openfhe.
    dcrt_bits = int(_env_os.environ.get("FHE_BTS_SF", str(dcrt_bits)))
    params.SetScalingModSize(dcrt_bits)
    # Scaling technique: FLEXIBLEAUTOEXT (default, _BTS_SCALING) eliminates the
    # FIXEDMANUAL scale-drift that blows up resnet20's degree-20 relu; both are
    # supported on 64-bit native. FLEX keeps the SAME chain sizing, so the
    # post-bootstrap usable level count is identical to rt_openfhe. Under FLEX
    # the nsd is a constant 2 (FLEX auto-manages scale), so the nsd-reset loop
    # in bootstrap_ciph/raise_mod is skipped and scale_deg is tracked as the
    # constant _FLEX_SCALE (the level, via the lazy bridge, is the real tracker).
    _scaling = _parse_flex_scaling(_BTS_SCALING, "flexibleautoext")  # default EXT (byte-identical baseline)
    if _scaling == "flexnoext":
        params.SetScalingTechnique(of.FLEXIBLEAUTO)        # no-EXT (arch-team resnet20 choice)
    elif _scaling == "flexext":
        params.SetScalingTechnique(of.FLEXIBLEAUTOEXT)     # default; EXT tower (+1 qnum, refresh headroom)
    else:  # "fixed"
        params.SetScalingTechnique(of.FIXEDMANUAL)         # exact real-nsd tracking
    params.SetFirstModSize(first_mod)
    params.SetKeySwitchTechnique(of.KeySwitchTechnique.HYBRID)  # bootstrap-ready
    level_budget = list(_BTS_LEVEL_BUDGET)  # FHE_BTS_LEVEL_BUDGET (default [4,4])
    _bts_depth = of.FHECKKSRNS.GetBootstrapDepth(level_budget, secret_key_dist)
    # ---- Minimal modulus-chain sizing (verbatim from rt_openfhe; see its long
    # comment). ir_capacity = max(input_level, max_target + bts_depth + 1) so the
    # deepest ReSBM bootstrap target is reachable from EvalBootstrap's post-output
    # landing by a downward droplevel. R towers below IR 0 reserve EvalBootstrap
    # down-headroom at the .py's deepest point (IR 1). ----
    mul_depth   = int(ctx.get("mul_depth", 10))
    hw          = int(ctx.get("hamming_weight", 0))
    _c_bts_d    = 15 if 0 < hw <= 192 else 19   # CTX_PARAM::Mul_depth_of_bootstrap
    _max_target = int(ctx.get("max_bts_target",
                             max(0, (mul_depth + 1) - _c_bts_d)))  # deepest target (IR)
    _in_lev     = int(ctx.get("input_level", mul_depth + 1))
    _CTX_IR_CAPACITY = max(_in_lev, _max_target + _bts_depth + 1)  # +1 safety margin
    levels_after = _CTX_IR_CAPACITY - _bts_depth - 1
    # ReSBM down-headroom reserve floor. The structural floor is 2 (a cipher needs
    # >=2 towers so EvalBootstrap CoeffsToSlots does not hit a 1-tower dcrtpoly
    # DropLastElement). FHE_BTS_RESERVE_FLOOR (debug/experiment knob, default 2)
    # lets the buffer sweep go below 2 to find the true minimum; the floor is only
    # ever lowered experimentally and the validated floor remains 2.
    _r_floor = int(_env_os.environ.get("FHE_BTS_RESERVE_FLOOR", "2"))
    R = max(_BTS_RESERVE, _r_floor)              # 2-tower floor + 1 margin; env override
    depth = levels_after + _bts_depth + R
    params.SetMultiplicativeDepth(depth)
    cc = of.GenCryptoContext(params)
    for f in (of.PKE, of.KEYSWITCH, of.LEVELEDSHE, of.ADVANCEDSHE, of.FHE):
        cc.Enable(f)
    # Half-packing / sparse bootstrap: the architecture resnet20 reference runs
    # the bootstrap at num_slots = ring_dim/4 (NOT the full ring/2); this roughly
    # halves the bootstrap-key HBM and the per-bootstrap runtime intermediates
    # (the resnet20 input is small, so its used slots stay in the first half).
    # Opt-in via FHE_DEV_BTS_HALF; default off = full packing (ring/2). SetBatchSize
    # (message packing) stays ring/2 in both cases.
    _bts_half = _env_os.environ.get("FHE_DEV_BTS_HALF", "0") not in ("0", "", "false", "False")
    num_slots = cc.GetRingDimension() // (4 if _bts_half else 2)
    # The mandatory bootstrap setup order (from the upstream example). fhert's
    # EvalBootstrapSetup accepts slots + correctionFactor kwargs; the explicit
    # correctionFactor (FHE_BTS_CORRECTION, default 13) is REQUIRED under
    # FIXEDMANUAL+SPARSE_TERNARY (delta #3): the auto value (9) is below the
    # EvalMod Chebyshev degree and the bootstrap would raise
    # "Degree must be <= correction factor".
    cc.EvalBootstrapSetup(level_budget, slots=num_slots,
                          correctionFactor=int(_BTS_CORRECTION))
    keys = cc.KeyGen()
    cc.EvalMultKeyGen(keys.secretKey)
    cc.EvalBootstrapKeyGen(keys.secretKey, num_slots)
    # Model rotation keys (mirror rt_openfhe): EvalBootstrapKeyGen only generates
    # the bootstrap's own CoeffsToSlots/SlotsToCoeffs keys, not arbitrary model
    # rotations, so generate the emitted rotate_keys too.
    rot = list(ctx.get("rotate_keys", []) or [])
    if rot:
        cc.EvalRotateKeyGen(keys.secretKey, rot)
    _CC, _PK, _SK = cc, keys.publicKey, keys.secretKey
    _N_SLOTS = num_slots
    _BOOTSTRAP_READY = True
    _FLEX_MODE = _scaling in ("flexext", "flexnoext")  # any FLEX -> constant-nsd shim; FIXED -> real-nsd
    _FLEX_EXT  = _scaling == "flexext"                 # EXT-only -> +1 Compress compensation
    _ROT_KEYS = frozenset(rot)


# Device constant-RAM ceiling: the cmodel's ``dsa_bconv_table[PEG_NUM][16384]``
# (fhert/_C/cmodel.cpp ``set_params``) holds the 8 base-conv CRT regions in a
# single fixed 16384-entry-per-PEG array with NO bounds check, and set_params'
# Region-7 (QHatModPi) compact layout makes the high-water offset grow
# ~quadratically in qnum. A CRT basis past the array silently writes out of
# bounds and the 256x256 cmodel core-dumps in ``SetParamsTo`` (exit 139) rather
# than raising. Empirically (ring 65536, 256x256, full packing) the ceiling is
# CRT-qnum 32 (cram total 15505, PASS) -> 34 (total 18422, core dump); the
# resnet20 host config ([4,4] R=2 -> depth 36 -> CRT-qnum 38, FLEX +1) is far
# past it. This helper mirrors set_params' offset math so _build_device_context
# can fail FAST with an actionable message instead of a native core dump.
_BDEV_BCONV_CAP = 16384


def _device_bconv_cram_total(cc):
    """High-water offset ``set_params`` writes into ``dsa_bconv_table[peg]`` for
    the built context (mirrors fhert/_C/cmodel.cpp:82-104). Returns
    ``(total, qnum, pnum, dnum)``; ``total > 16384`` => the 256x256 cmodel
    core-dumps in ``SetParamsTo``."""
    t = cc.get_crt_tables()
    qnum = len(t.moduliQ); pnum = len(t.moduliP); dnum = len(t.QHatInvModQi)
    alpha = (qnum + dnum - 1) // dnum
    bconvPHatInvBase = qnum * (2 + pnum)
    bconvQlInvBase = bconvPHatInvBase + pnum
    bconvQHatInvBase = bconvQlInvBase + (qnum - 1) * (qnum - 1)
    bconvQHatModBase = bconvQHatInvBase + qnum * dnum * alpha
    region7End = bconvQHatModBase
    for ii in range(1, qnum + 1):
        gs = ((ii + alpha - 1) // alpha - 1) * alpha
        region7End += gs * (ii - alpha + pnum) + (ii - gs) * (gs + pnum)
    return region7End + (qnum - 1) * (qnum - 1), qnum, pnum, dnum


def _drop_bts_plain_diags(cc, dev):
    """Env-gated device-HBM reclaim after SetBootstrapPrecomTo
    (FHE_DEV_BTS_NO_PLAINDIAGS; default OFF = byte-identical baseline).

    fhert's ``upload_bootstrap_precomputation``
    (fhert/openfhe/_ckks_context.py:1426-1494) uploads the c2s/s2c FFT-mode
    per-stage diagonals TWICE: once as contiguous ``_StackedDiags`` buffers
    -- the data source the fused ``eval_bsgs_fused`` production kernel reads
    -- and once as per-plaintext ``pt.to(dev)`` copies stored in
    ``precom.encoding_diags_fft`` / ``decoding_diags_fft``. Those per-plaintext
    copies are NOT read on the fused path: ``_eval_coeffs_to_slots_fft`` /
    ``_eval_slots_to_coeffs_fft`` (:2599 / :2670) pass only ``A[s][0]`` per
    stage to ``_eval_linear_transform_stage`` (:2635 / :2702), which forwards
    it as ``seed_pt`` to ``_eval_bsgs_fused`` -- used there ONLY as a metadata
    template (``GetNoiseScaleDeg``/``GetScalingFactor`` at :2358-2359; it is
    not even a kernel argument). The non-fused LT fallback
    (``_eval_linear_transform``, :2832 / :2858) reads the FLATTENED
    ``encoding_diags``/``decoding_diags`` lists, which stay empty unless both
    level budgets are 1 (OpenFHE populates m_U0Pre/m_U0hatTPre only in
    isLTBootstrap mode, ckksrns-fhe.cpp:224-244) -- this hook never touches
    them either way. At the resnet20 device config (ring 65536, [3,3],
    half-packing) the unread copies hold ~1.1 GiB of the 16 GiB simulated
    HBM and tip the first bootstrap's EvalMod over budget.

    The fhert tree is read-only, so this hook runs from the fhe-cmplr side:
    it walks the device CKKS context's ``_bootstrap_precom`` and frees the
    device allocation of every per-stage FFT plaintext EXCEPT ``stage[0]``
    (kept intact, seed template), mirroring ``Plaintext.__del__``'s
    free-and-clear (_plaintext.py:81-86) so the originals never double-free.
    ``_StackedDiags`` and the seed plaintexts stay untouched, so actual
    bootstrap execution is unmodified. Intended for the fused FFT path
    (level_budget > 1); in LT mode ([1,1]) the per-stage lists are empty and
    this is a no-op by construction.
    """
    precom = cc._device_context._bootstrap_precom
    n_freed = 0
    n_dropped = 0
    for attr in ("encoding_diags_fft", "decoding_diags_fft"):
        for stage in getattr(precom, attr, None) or ():
            # Iterate by INDEX so we can also drop the Python-side wrapper
            # reference in the stage list (release); the fused FFT path never
            # reads A_s[1:] after SetBootstrapPrecomTo -- only stage[0]
            # survives (the seed metadata template). Dropping the Python ref
            # also decrements the underlying C++ Plaintext<DCRTPoly> shared_ptr
            # refcount when this wrapper is the LAST reference (the C++
            # CryptoContext's vector<vector<Plaintext>> usually still holds
            # its own shared_ptr, so the C++ data stays alive; this drop is
            # correct but mostly idempotent -- if a future fhert build leaves
            # the C++ precom owned only by the Python list, this is what
            # actually reclaims the host DCRTPoly data the device mirror
            # duplicated).
            for idx in range(1, len(stage)):
                pt = stage[idx]
                if getattr(pt, "_device_address", None) is None:
                    # Already off-device AND already nulled as a Python ref
                    # (idempotent). Keep the None slot (reclaimed already).
                    if pt is None:
                        continue
                    continue  # device addr freed but slot still set; skip
                for addr in pt._device_address:
                    dev.mem_free(addr)
                pt._device_address = None
                pt._device = None
                # Drop the Python wrapper reference so the underlying host C++
                # DCRTPoly (the source the per-plaintext mirror was uploaded
                # from) can be freed as soon as no other Python or C++ shared_ptr
                # holds it. ``stage[idx] = None`` keeps the list shape
                # (so later ``len(stage)`` / ``A[0]`` still work) but releases
                # the element; the seed ``stage[0]`` is left untouched.
                stage[idx] = None
                n_freed += 1
                n_dropped += 1
    if _env_os.environ.get("FHE_DEV_DEBUG"):
        import sys as _sys
        print(f"[FHE_DEV_DEBUG] FHE_DEV_BTS_NO_PLAINDIAGS: freed device "
              f"copies of {n_freed} redundant c2s/s2c per-plaintext diagonals "
              f"(kept _StackedDiags buffers + one seed plaintext per stage); "
              f"dropped {n_dropped} Python wrapper refs to release host "
              f"DCRTPoly data",
              file=_sys.stderr, flush=True)
    return n_freed


def _build_device_context(of, ctx):
    """Device (CModelDevice) context -- the host bootstrap context PLUS the
    device setup (SetParamsTo/SetEvalKeysTo/SetBootstrapPrecomTo). Device mode
    uses FLEXIBLEAUTOEXT by default (byte-identical to the validated baseline);
    ``FHE_DEV_SCALING=fixed*`` selects FIXEDMANUAL and ``FHE_DEV_SCALING=flex``
    (or ``noext``/``flexibleauto``) selects FLEXIBLEAUTO no-EXT (architecture team's
    resnet20 choice), with ``_FLEX_MODE`` set for any FLEX variant and ``_FLEX_EXT``
    only for EXT (FIXEDMANUAL tracks the real nsd instead of the FLEX constant-2
    shim); SPARSE_TERNARY +
    correctionFactor=13 for the device bootstrap (the validated fhert config; the
    device-patch layer then routes every Eval*/EvalBootstrap to the cmodel device
    when a ciphertext carries ``_device``). The host-style seams in this module (_encrypt_values upload,
    _decrypt_slots download, _plain_for_op plain upload, _drop_to_of_level
    host-Compress fallback, Cipher.clone host round-trip) handle the device
    divergences; the arithmetic ops themselves are unchanged (device dispatch is
    transparent).

    ``ctx['need_bts']``/``ctx['bootstrap']`` builds a device BOOTSTRAP context
    (EvalBootstrapSetup(precompute=True) + SetBootstrapPrecomTo +
    EvalBootstrapKeyGen); otherwise a device leveld-SHE context. The device
    ring_dim is the CModelDevice grid (num_col*num_row); ``_seed_ctx`` already
    overrode ``poly_degree`` to ``device_ring`` (default 4096 = MPUH1Config
    64x64). ``ctx['device_hw']`` = (num_col,num_row,*,num_peg) overrides the grid.
    ``ctx['device_hbm_gb']`` (env FHE_DEVICE_HBM_GB) overrides the per-domain
    HBM in GB (default 16, capped at 64 by the per-domain local address space).
    NB: the device bootstrap cold-JIT on the cmodel *simulator* is expensive
    (~150 s/bootstrap at ring 4096; superlinear in slots), so large-ring device
    bootstrap workloads (e.g. resnet20 at 2^16) are impractically slow.

    _DEV is MODULE-LEVEL and LONG-LIVED: built once per ring_dim and reused
    across cc rebuilds (SetParamsTo/SetEvalKeysTo/SetBootstrapPrecomTo below
    refresh its uploaded CRT/keys/precompute for the new cc)."""
    global _CC, _PK, _SK, _N_SLOTS, _BOOTSTRAP_READY, _ROT_KEYS, _CTX_IR_CAPACITY, _FLEX_MODE, _FLEX_EXT
    global _DEV, _DEV_RING
    from fhert.backends import CModelDevice, MPUH1Config
    ring = int(ctx.get("poly_degree") or _DEFAULT_DEV_RING)   # already overridden in _seed_ctx
    hw = tuple(ctx.get("device_hw") or _DEFAULT_DEV_HW)
    # FHE_DEVICE_HBM_GB (ctx['device_hbm_gb']): per-domain HBM in GB, passed as
    # a MPUH1Config KEYWORD only when set (a 5th FHE_DEVICE_HW positional would
    # land in cram_size); unset -> MPUH1Config(*hw), the byte-identical default.
    _hbm = int(ctx.get("device_hbm_gb", 0))
    _hbm_eff = _hbm if _hbm > 0 else 16
    if _DEV is None or _DEV_RING != ring \
            or getattr(_DEV.target, "hbm_size_per_domain", 16) != _hbm_eff:
        _DEV = CModelDevice(MPUH1Config(*hw) if _hbm <= 0
                            else MPUH1Config(*hw, hbm_size_per_domain=_hbm))
        _DEV_RING = ring
    bts = bool(ctx.get("bootstrap") or ctx.get("need_bts"))
    params = of.CCParamsCKKSRNS()
    # Scaling: FLEX (default, EXT) preserves the validated device regress58 baseline;
    # FHE_DEV_SCALING=fixed* selects FIXEDMANUAL (host-3a parity, honest real-nsd
    # tracking); FHE_DEV_SCALING=flex/noext/flexibleauto selects FLEXIBLEAUTO no-EXT
    # (architecture team's resnet20 recipe: qnum -1, +1 usable post-ModRaise limb,
    # still auto-adjusted). The fhert device binding supports all three: it bit-exact
    # validates FLEX-no-EXT on the device bootstrap path (tests/ckks/test_bootstrap.py
    # #99) and fully supports FIXEDMANUAL add/sub/mult plain: _adjust_for_add_or_sub
    # RAISES the plain by SF^diffDepth when plain.nsd<=cipher.nsd and throws ONLY
    # when plain.nsd>cipher.nsd (test_scaling_technique_alignment.py:184-198 PASSES
    # bit-exact). The prior "FIXEDMANUAL throws on device" was rt_fhert's unconditional
    # _FLEX_MODE clamp pinning tracked nsd=2 above the real cipher nsd, NOT a binding
    # limit. _scaling drives _FLEX_MODE (any FLEX) and _FLEX_EXT (EXT only), set near
    # the end of this function: honest real-nsd tracking is active under FIXEDMANUAL
    # (tracked==real at every op -> add_plain/sub_plain diff_depth==0, no throw), and
    # the +1 Compress compensation applies only under EXT.
    _scaling = _parse_flex_scaling(_env_os.environ.get("FHE_DEV_SCALING", ""), "")
    if bts:
        # Bootstrap (3b): SPARSE_TERNARY (avoids UNIFORM's "Degree <= correction
        # factor" at EvalMod); any FLEX avoids FIXEDMANUAL scale drift on the deep
        # chain. sf uncapped (deep bootstrap needs > _SCALE_BIT_CAP below).
        secret_key_dist = of.SecretKeyDist.SPARSE_TERNARY
    else:
        # Non-bootstrap (3a): UNIFORM_TERNARY (OpenFHE default, same as host 3a);
        # sf capped at _SCALE_BIT_CAP below. N auto-selected per model (device).
        secret_key_dist = of.SecretKeyDist.UNIFORM_TERNARY
    params.SetSecretKeyDist(secret_key_dist)
    if _scaling == "flexnoext":
        params.SetScalingTechnique(of.FLEXIBLEAUTO)        # no-EXT (arch-team choice)
    elif _scaling == "flexext":
        params.SetScalingTechnique(of.FLEXIBLEAUTOEXT)     # default; EXT tower (+1 qnum)
    else:  # "fixed"
        params.SetScalingTechnique(of.FIXEDMANUAL)        # host-3a parity
    params.SetSecurityLevel(of.HEStd_NotSet)
    params.SetRingDim(ring)
    params.SetKeySwitchTechnique(of.KeySwitchTechnique.HYBRID)
    # Device VRF-bank fit for the HYBRID keyswitch (rotate/relin): the keyswitch
    # digit slice is `beta = ceil(qnum/dnum)` polys, and a slice wider than the
    # device's per-bank VRF capacity (num_polys_per_bank, ~1 at 256x256 peg=1)
    # aborts MLIR lowering ("result poly size (N) exceeds VRF bank capacity").
    # Raising the HYBRID digit count (dnum = numLargeDigits) shrinks beta toward
    # 1 (one modulus per digit) so the digit slice fits one VRF bank. N is NOT
    # changed; only the keyswitch decomposition. Env-gated; 0 = OpenFHE auto.
    _nld = int(_env_os.environ.get("FHE_DEV_NUM_LARGE_DIGITS", "0"))
    if _nld > 0:
        params.SetNumLargeDigits(_nld)
    _native128 = (of.get_native_int() == 128)
    first_mod = int(ctx.get("first_prime_bit_num", 89 if _native128 else 60))
    dcrt_bits = int(ctx.get("scaling_factor_bit_num", 78 if _native128 else 59))
    if not bts:
        # Non-bootstrap (3a parity): cap sf at _SCALE_BIT_CAP, same as host 3a.
        dcrt_bits = min(dcrt_bits, _SCALE_BIT_CAP)
    # Device modulus bit-size experimentation knobs (the cmodel bconv/NTT
    # reduction path). N is NOT changed; only the moduli bit sizes. Default =
    # the emitted (.py) values. Used only for non-bootstrap device diagnosis.
    first_mod = int(_env_os.environ.get("FHE_DEV_FIRST_MOD", str(first_mod)))
    dcrt_bits = int(_env_os.environ.get("FHE_DEV_DCRT_BITS", str(dcrt_bits)))
    if bts:
        dcrt_bits = int(_env_os.environ.get("FHE_BTS_SF", str(dcrt_bits)))
    params.SetFirstModSize(first_mod)
    params.SetScalingModSize(dcrt_bits)
    params.SetBatchSize(ring // 2)
    level_budget = list(_BTS_LEVEL_BUDGET)
    _bts_depth = of.FHECKKSRNS.GetBootstrapDepth(level_budget, secret_key_dist) if bts else 0
    if bts:
        # minimal chain sizing (same as host _build_bootstrap_context)
        mul_depth   = int(ctx.get("mul_depth", 10))
        hw_ck       = int(ctx.get("hamming_weight", 0))
        _c_bts_d    = 15 if 0 < hw_ck <= 192 else 19
        _max_target = int(ctx.get("max_bts_target", max(0, (mul_depth + 1) - _c_bts_d)))
        _in_lev     = int(ctx.get("input_level", mul_depth + 1))
        _CTX_IR_CAPACITY = max(_in_lev, _max_target + _bts_depth + 1)
        levels_after = _CTX_IR_CAPACITY - _bts_depth - 1
        _r_floor = int(_env_os.environ.get("FHE_BTS_RESERVE_FLOOR", "2"))
        R = max(_BTS_RESERVE, _r_floor)
        depth = levels_after + _bts_depth + R
    else:
        depth = int(ctx.get("mul_depth", 0))
        # Small-N device keyswitch: the fhert kernel emits bconv/NTT modulus
        # indices that misalign for a SHORT RNS chain (small qnum) -> the cmodel
        # "input a < mod" unreduced-coefficient error (see cmodel-source audit).
        # The slip is at SMALL qnum, not small N per se (fhert test_rotate passes
        # at N=65536 qnum=27). Force a deeper chain (larger qnum) at small N to
        # push the keyswitch past the short-chain slip; the model's own ops only
        # consume its emitted depth, the extra levels are headroom (no [hard]
        # violation since towers_left stays large). N is NOT changed.
        _min_depth = int(_env_os.environ.get("FHE_DEV_MIN_DEPTH", "0"))
        if _min_depth > depth:
            depth = _min_depth
        _CTX_IR_CAPACITY = depth + 1
    params.SetMultiplicativeDepth(depth)
    cc = of.GenCryptoContext(params)
    feats = (of.PKE, of.KEYSWITCH, of.LEVELEDSHE, of.ADVANCEDSHE, of.FHE) if bts \
            else (of.PKE, of.KEYSWITCH, of.LEVELEDSHE)
    for f in feats:
        cc.Enable(f)
    # Half-packing / sparse bootstrap (FHE_DEV_BTS_HALF): bootstrap num_slots =
    # ring_dim/4 instead of the full ring/2 (the architecture resnet20 reference
    # does this; halves the bootstrap-key HBM and per-bootstrap runtime buffers;
    # the resnet20 input is small so its used slots stay in the first half).
    # Default off = full packing. SetBatchSize (message) stays ring/2.
    _bts_half = _env_os.environ.get("FHE_DEV_BTS_HALF", "0") not in ("0", "", "false", "False")
    num_slots = cc.GetRingDimension() // (4 if _bts_half else 2)
    # Fail-fast on the device constant-RAM bconv-table ceiling BEFORE the ~8-min
    # keygen: a CRT basis past dsa_bconv_table[16384] makes the 256x256 cmodel
    # core-dump in SetParamsTo (no bounds check in set_params). Surface it as a
    # clear, actionable error instead of a native abort. (CRT tables are fixed at
    # GenCryptoContext, so this is safe before EvalBootstrapSetup/keygen.)
    _cram, _cq, _cp, _cd = _device_bconv_cram_total(cc)
    if _env_os.environ.get("FHE_DEV_DEBUG"):
        _alpha = (_cq + _cd - 1) // _cd  # beta = ceil(qnum/dnum) = digit polys
        _cap = getattr(_DEV.target, "num_polys_per_bank", "?")
        import sys as _sys
        print(f"[FHE_DEV_DEBUG] ring={ring} hw={hw} "
              f"hbm={getattr(_DEV.target, 'hbm_size_per_domain', 16)} "
              f"qnum={_cq} pnum={_cp} "
              f"dnum={_cd} beta(alpha)={_alpha} vrf_bank_cap={_cap} "
              f"cram_total={_cram} (cap {_BDEV_BCONV_CAP})", file=_sys.stderr, flush=True)
    if _cram > _BDEV_BCONV_CAP:
        _r_now = max(_BTS_RESERVE, int(_env_os.environ.get("FHE_BTS_RESERVE_FLOOR", "2")))
        _violation("hard", "SetParamsTo",
                   f"device constant-RAM bconv-table overflow: CRT-qnum {_cq} "
                   f"(pnum {_cp}, dnum {_cd}) -> cram total {_cram} > "
                   f"{_BDEV_BCONV_CAP}; the 256x256 cmodel cannot accept this "
                   f"basis (it would core-dump in SetParamsTo). The device ceiling "
                   f"is CRT-qnum ~32 (cram total <= 16384). Reduce the modulus "
                   f"chain: FHE_BTS_LEVEL_BUDGET={_BTS_LEVEL_BUDGET} "
                   f"reserve R={_r_now}.")
    if bts:
        cc.EvalBootstrapSetup(level_budget, slots=num_slots,
                              correctionFactor=int(_BTS_CORRECTION), precompute=True)
    keys = cc.KeyGen()
    cc.EvalMultKeyGen(keys.secretKey)
    if bts:
        cc.EvalBootstrapKeyGen(keys.secretKey, num_slots)
    rot = list(ctx.get("rotate_keys", []) or [])
    if rot:
        cc.EvalRotateKeyGen(keys.secretKey, rot)
    # device setup: upload CRT table + eval keys (+ rotation keys) [+ bootstrap
    # precompute diagonals]. SetEvalKeysTo MUST pass rotation_indices when there
    # are rotations (probe E6: SetEvalKeysTo(keytag) alone leaves an EvalRotate
    # KeyError -- the rotation key is never uploaded).
    cc.SetParamsTo(_DEV)
    if rot:
        cc.SetEvalKeysTo(keys.secretKey.GetKeyTag(), rotation_indices=rot)
    else:
        cc.SetEvalKeysTo(keys.secretKey.GetKeyTag())
    if bts:
        cc.SetBootstrapPrecomTo(num_slots)
        # Opt-in (FHE_DEV_BTS_NO_PLAINDIAGS; default 0 = current behavior):
        # free the redundant per-plaintext device copies of the c2s/s2c FFT
        # diagonals the fused eval_bsgs_fused path never reads (~1.1 GiB at
        # ring 65536 [3,3] half-packing; see _drop_bts_plain_diags).
        if _env_os.environ.get("FHE_DEV_BTS_NO_PLAINDIAGS", "0") not in (
                "0", "", "false", "False"):
            _drop_bts_plain_diags(cc, _DEV)
    _CC, _PK, _SK = cc, keys.publicKey, keys.secretKey
    _N_SLOTS = num_slots
    _BOOTSTRAP_READY = bts
    _FLEX_MODE = _scaling in ("flexext", "flexnoext")  # any FLEX -> constant-nsd shim; FIXED -> real-nsd
    _FLEX_EXT  = _scaling == "flexext"                 # EXT-only -> +1 Compress compensation
    _ROT_KEYS = frozenset(rot)

    # A-fix: drop the OpenFHE host C++ eval-key keystore (~56 GiB at ring
    # 65536 = 266 automorphism keys + 1 relinearize key), held in the
    # binding CryptoContextImpl<DCRTPoly> global eval-key maps and NOT seen
    # by Python gc/tracemalloc (so a generic gc.collect/null reclaim is
    # inert). The device EvalBootstrap path reads the DEVICE-uploaded EVK
    # (/dev/shm, populated by upload_eval_keys inside SetEvalKeysTo above),
    # not these host keys; the only host fallback (Compress in
    # _drop_to_of_level) is a tower-drop linear op with no key-switches.
    # Gated by FHE_DEVICE_HOST_FALLBACK: ON by default for device (the host
    # C++ keystore is unreachable post-upload); set FHE_DEVICE_HOST_FALLBACK=1
    # to keep the host keys (reserves the ~56 GiB).
    if _env_os.environ.get("FHE_DEVICE_HOST_FALLBACK", "0") in \
            ("0", "", "false", "False"):
        import sys as _sys

        def _vmrss_mb():
            # Inline /proc/self/status reader (one-shot; no psutil dependency)
            # so the keygen-complete VmRSS drop is reported in this log line
            # itself, not just inferred from the 60 s heartbeat.
            try:
                with open("/proc/self/status") as _f:
                    for _ln in _f:
                        if _ln.startswith("VmRSS:"):
                            return int(_ln.split()[1]) / 1024.0
            except Exception:
                return None
            return None
        try:
            # ClearEvalAutomorphismKeys is def_static on the CryptoContext
            # class (callable via the instance); ClearEvalMultKeys is a
            # module-level m.def on the native binding (NOT reexported by
            # fhert.openfhe), so the native submodule import is required.
            _before = _vmrss_mb()
            _CC.ClearEvalAutomorphismKeys()
            import fhert._C._lib.openfhe as _native_of
            _native_of.ClearEvalMultKeys()
            _after = _vmrss_mb()
            if _env_os.environ.get("FHE_DEV_DEBUG"):
                if _before is not None and _after is not None:
                    _drop = _before - _after
                    print(f"[HOST-EVK-CLEAR] released OpenFHE host C++ eval "
                          f"keys VmRSS before={_before:.0f}MB "
                          f"after={_after:.0f}MB dropped={_drop:.0f}MB",
                          file=_sys.stderr, flush=True)
                else:
                    print("[HOST-EVK-CLEAR] released OpenFHE host C++ eval "
                          "keys (VmRSS read failed)",
                          file=_sys.stderr, flush=True)
        except Exception as _e:
            print(f"[HOST-EVK-CLEAR] WARNING skipped: "
                  f"{type(_e).__name__}: {_e}",
                  file=_sys.stderr, flush=True)


# ---------------------------------------------------------------------------
# IO
# ---------------------------------------------------------------------------

def get_input_data(name, idx):
    """Load an input cipher/plain by ONNX tensor name + index (seeded into
    _INPUTS by the driver). The array-input form
    ``for i in range(n): var[i] = get_input_data(name, i)`` is handled by
    repeated calls."""
    key = (name, idx)
    if key not in _INPUTS:
        _violation("hard", "get_input_data",
                   f"no input seeded for ({name!r}, {idx})")
    return _INPUTS[key]


def set_output_data(name, idx, val):
    """Capture a program output by ONNX output name + index."""
    _OUTPUTS[(name, idx)] = val


# ---------------------------------------------------------------------------
# arithmetic: add / sub  (ciph / plain / scalar)
# ---------------------------------------------------------------------------

def _bin_logical_count(a, b, op):
    """Both operands must pack the same logical slot count (rt_ref `_bin_slots`
    parity); return it. A zero_ciph (wildcard `_n_logical==0`) adopts the other
    operand's shape -- it is the + / - identity (rt_ref's empty-slot
    broadcast), so add/sub_ciph(zero, x) returns x's count rather than a [hard]
    mismatch."""
    if a._n_logical == 0:
        return b._n_logical
    if b._n_logical == 0:
        return a._n_logical
    if a._n_logical != b._n_logical:
        _violation("hard", op,
                   f"slot count mismatch: {a._n_logical} != {b._n_logical}")
    return a._n_logical


def add_ciph(a, b):
    op = "add_ciph"
    _require_cipher(a, op)
    _require_cipher(b, op)
    if a.is_cipher3 or b.is_cipher3:
        _violation("hard", op, "cipher3 operand must be relined first")
    # Zero-operand short-circuit (rt_ref scale_deg==0 waiver): zero + X == X.
    # MUST use Cipher.clone(), not a bare ``b._ct.Clone()``: on a device ct a
    # bare OpenFHE Clone() strips ``_device`` and copies the (stale) host
    # buffers, so the returned "host" ct carries device-backed garbage; a
    # later host-side op on it (e.g. mod_switch ``cc.Compress``) then segfaults
    # in ``LeveledSHERNS::Compress``. clone() round-trips to(None)->Clone->to(_DEV).
    if a.scale_deg == 0:
        return b.clone()
    if b.scale_deg == 0:
        return a.clone()
    _check_scale_match(a, b, op)
    n = _bin_logical_count(a, b, op)
    a_ct, b_ct = _align_ct_for_binop(a, b, op)
    ct = _CC.EvalAdd(a_ct, b_ct)
    sd = a.scale_deg if a.scale_deg else b.scale_deg
    return Cipher._wrap(ct, sd, False, n)


def add_plain(a, p):
    op = "add_plain"
    _require_cipher(a, op)
    _reject_cipher3(a, op)
    _require_plain(p, op)
    # Re-encode the plain at the cipher's level + scale (EvalAdd precondition).
    pt = _plain_for_op(a, p, a.scale_deg, op)
    ct = _CC.EvalAdd(a._ct, pt)
    return Cipher._wrap(ct, a.scale_deg, False, _cipher_plain_result_n(a, p))


def add_scalar(a, s):
    op = "add_scalar"
    _require_cipher(a, op)
    ct = _CC.EvalAdd(a._ct, float(s))
    return Cipher._wrap(ct, a.scale_deg, False, a._n_logical)


def sub_ciph(a, b):
    op = "sub_ciph"
    _require_cipher(a, op)
    _require_cipher(b, op)
    if a.is_cipher3 or b.is_cipher3:
        _violation("hard", op, "cipher3 operand must be relined first")
    # Zero-operand short-circuit (rt_ref scale_deg==0 waiver): X - zero == X,
    # zero - X == -X. clone() (not bare _ct.Clone()) round-trips device cts
    # (see add_ciph): a bare Clone of a device ct yields a stale-host ct that
    # segfaults later host-side ops.
    if b.scale_deg == 0:
        return a.clone()
    if a.scale_deg == 0:
        return neg_ciph(b)
    _check_scale_match(a, b, op)
    n = _bin_logical_count(a, b, op)
    a_ct, b_ct = _align_ct_for_binop(a, b, op)
    ct = _CC.EvalSub(a_ct, b_ct)
    sd = a.scale_deg if a.scale_deg else b.scale_deg
    return Cipher._wrap(ct, sd, False, n)


def sub_plain(a, p):
    op = "sub_plain"
    _require_cipher(a, op)
    _reject_cipher3(a, op)
    _require_plain(p, op)
    pt = _plain_for_op(a, p, a.scale_deg, op)
    ct = _CC.EvalSub(a._ct, pt)
    return Cipher._wrap(ct, a.scale_deg, False, _cipher_plain_result_n(a, p))


def sub_scalar(a, s):
    op = "sub_scalar"
    _require_cipher(a, op)
    ct = _CC.EvalSub(a._ct, float(s))
    return Cipher._wrap(ct, a.scale_deg, False, a._n_logical)


# ---------------------------------------------------------------------------
# arithmetic: mul  (ciph / plain / scalar / mono)
# ---------------------------------------------------------------------------

def mul_ciph(a, b):
    op = "mul_ciph"
    _require_cipher(a, op)
    _require_cipher(b, op)
    if a.is_cipher3 or b.is_cipher3:
        _violation("hard", op, "cipher3 operand must be relined first")
    n = _bin_logical_count(a, b, op)
    # ant Mul_ciphertext3 also auto-aligns (evaluator.c:142-175): drop the
    # higher-ir operand to the lower before EvalMultNoRelin, result at min level.
    a_ct, b_ct = _align_ct_for_binop(a, b, op)
    # EvalMultNoRelin -> 3-component cipher (cipher3), nsd += sd_a + sd_b.
    ct = _CC.EvalMultNoRelin(a_ct, b_ct)
    return Cipher._wrap(ct, a.scale_deg + b.scale_deg, True, n)


def mul_plain(a, p):
    op = "mul_plain"
    _require_cipher(a, op)
    _reject_cipher3(a, op)
    _require_plain(p, op)
    # The plain contributes its own scale: encode at its scale_deg.
    pt = _plain_for_op(a, p, p.scale_deg, op)
    ct = _CC.EvalMult(a._ct, pt)
    return Cipher._wrap(ct, a.scale_deg + p.scale_deg, False,
                        _cipher_plain_result_n(a, p))


def mul_scalar(a, s):
    op = "mul_scalar"
    _require_cipher(a, op)
    _reject_cipher3(a, op)
    # EvalMult(c, float) encodes the scalar at the cipher's scale -> nsd+1.
    ct = _CC.EvalMult(a._ct, float(s))
    return Cipher._wrap(ct, a.scale_deg + 1, False, a._n_logical)


def mul_mono_ciph(a, p):
    op = "mul_mono_ciph"
    _require_cipher(a, op)
    _reject_cipher3(a, op)
    _require_plain(p, op)
    pt = _plain_for_op(a, p, p.scale_deg, op)
    ct = _CC.EvalMult(a._ct, pt)
    # Same scale math as mul_plain; "mono" signals no rescale, tracking shared.
    return Cipher._wrap(ct, a.scale_deg + p.scale_deg, False,
                        _cipher_plain_result_n(a, p))


# ---------------------------------------------------------------------------
# unary / shape / lifecycle
# ---------------------------------------------------------------------------

def neg_ciph(a):
    op = "neg_ciph"
    _require_cipher(a, op)
    ct = _CC.EvalNegate(a._ct)
    return Cipher._wrap(ct, a.scale_deg, a.is_cipher3, a._n_logical)


def rotate_ciph(a, k):
    op = "rotate_ciph"
    _require_cipher(a, op)
    _reject_cipher3(a, op)
    ki = int(k)
    # openfhe EvalRotate needs an automorphism key for the EXACT index ki; a key
    # for +k does NOT enable rotation by -k. Without the key the binding raises
    # RuntimeError, so surface a clean [hard] here naming the missing index.
    if ki not in _ROT_KEYS:
        _violation("hard", op,
                   f"no rotation key for index {ki}; generate it via "
                   f"get_context_params()['rotate_keys'] (the exact index, not "
                   f"its negation)")
    ct = _CC.EvalRotate(a._ct, ki)
    return Cipher._wrap(ct, a.scale_deg, False, a._n_logical)


def conjugate_ciph(a):
    op = "conjugate_ciph"
    _require_cipher(a, op)
    _reject_cipher3(a, op)
    # On real CKKS slots a conjugation negates the imaginary part; the
    # green-gate corpus packs real values, so a conjugate is a no-op (rt_ref
    # parity). A native complex-packing conjugate is not implemented.
    return a.clone()


def relin(a):
    op = "relin"
    _require_cipher_or_cipher3(a, op)
    if a.is_cipher3:
        ct = _CC.Relinearize(a._ct)
        return Cipher._wrap(ct, a.scale_deg, False, a._n_logical)
    # cipher -> no-op (compatibility: the emitted mul chain is mul_ciph ->
    # rescale_ciph -> relin, so relin receives a post-rescale cipher).
    return a.clone()


def rescale_ciph(a):
    op = "rescale_ciph"
    _require_cipher_or_cipher3(a, op)
    if a.scale_deg <= 1:
        _violation("strict", op, f"requires scale_deg>1, got {a.scale_deg}")
    if a.level - 1 < 0:
        _violation("hard", op, f"level underflow: {a.level}-1 < 0")
    ct = _CC.Rescale(a._ct)
    # Lazy: ``Rescale`` raises openfhe GetLevel by 1, so the on-demand
    # ``Cipher.level`` (= ir_capacity - GetLevel) drops by 1 with NO stored
    # update; noiseScaleDeg drops by 1 (FIXEDMANUAL -- probed).
    if a.is_cipher3:
        # The emitted chain rescales a cipher3 (EvalMultNoRelin result);
        # Rescale keeps the 3rd component, so relin it back now to a true
        # 2-comp cipher (plan: rescale_ciph = Rescale + eager Relinearize for
        # cipher3).
        ct = _CC.Relinearize(ct)
    return Cipher._wrap(ct, a.scale_deg - 1, False, a._n_logical)


def mod_switch(a):
    """Drop one modulus level while keeping the scale degree (rt_ref
    ``mod_switch``: ``level -= 1``, ``scale_deg`` unchanged, ``is_cipher3``
    preserved). Real-HE implementation: a scale-preserving droplevel via
    ``_drop_to_of_level`` -- ``cc.Compress`` with the CURRENT nsd passed
    explicitly, which drops one tower (GetLevel +1) without ModReduce. The
    binding default ``Compress`` nsd=1 is a footgun (it ModReduces first);
    the helper passes ``ct.GetNoiseScaleDeg()`` so the scale is preserved."""
    op = "mod_switch"
    _require_cipher_or_cipher3(a, op)
    if a.level - 1 < 0:
        _violation("hard", op, f"level underflow: {a.level}-1 < 0")
    nsd = int(a._ct.GetNoiseScaleDeg())   # preserve scale: pass current nsd
    ct = _drop_to_of_level(a._ct, a._ct.GetLevel() + 1, nsd, op)  # drop 1 tower
    return Cipher._wrap(ct, a.scale_deg, a.is_cipher3, a._n_logical)


def upscale_ciph(a, bits):
    """Track-only (plan-sanctioned): increment the tracked scale_deg without a
    real op. A real scale raise would be EvalMult(ct, 2**bits) + Rescale -- but
    that changes the level and is not needed by any green-gate workload. The
    tracking lets a compiler DAG that probes the state machine proceed; NOT
    emitted by the green-gate corpus."""
    op = "upscale_ciph"
    _require_cipher_or_cipher3(a, op)
    if a._ct is not None:
        return Cipher._wrap(a._ct.Clone(), a.scale_deg + 1, False, a._n_logical)
    # Array container: preserve the element ciphers (track-only bumps the
    # container scale_deg; elements keep their own state, mirroring clone()).
    return Cipher([c.clone() for c in a._slots], a.level, a.scale_deg + 1,
                  a.is_cipher3)


def _run_reclaim_diag(freed_ext_addrs, offset, dev):
    """One-shot diagnostic (fires ONLY at the 2nd reclaim). Pin leaked
    EvalBootstrap scratch device-buffer owners (Ciphertext / Plaintext / other
    wrapper with a non-None ``_device_address``): a wrapper is LEAKED if it is
    still ALIVE (in ``gc.get_objects()``) with a non-None ``_device_address``
    while one or more of its device addresses is NO LONGER in
    ``dev._alloc_map`` -- i.e. the deterministic reclaim (or a prior owner
    ``__del__``) already freed the device buffer, but the Python wrapper
    survives -> the host-side state it still pins leaks per bootstrap.

    This ``_alloc_map`` membership check supersedes address-sample matching: it
    catches CROSS-bootstrap retention (e.g. bootstrap #1 wrappers still alive at
    reclaim #2 with device buffers already freed by reclaim #1) as well as
    same-bootstrap retention, since it is not limited to the addrs freed in just
    this reclaim. As a CONTROL it also logs a one-line AGGREGATE
    (total_gc / dev_wrappers / healthy / leaked / freed_this_reclaim /
    alloc_map + by_type / healthy_by_type / leaked_by_type) so a ``leaked=0``
    verdict can be trusted and the holder's Python TYPE is pinned even without
    the referrer walk. For up to 3 leaked wrappers, logs
    ``[reclaim-diag] leaked wrapper id=... type=... owner=... addrs=[...]
    in_map=[...] referrers=[(type#id extra: repr), ...]`` to stderr (the device
    harness dup2's fd2 to the run log). Whole body try/except; never crashes
    the run."""
    import gc as _gc
    import sys as _gsys
    try:
        alloc_keys = set(int(k) for k in dev._alloc_map.keys())
        freed_set = set()
        for _a in freed_ext_addrs:
            try:
                freed_set.add(int(_a))
            except (TypeError, ValueError):
                pass
        _CtCls = None
        _PtCls = None
        try:
            from fhert._C._lib.openfhe import Ciphertext as _CtCls
        except Exception:
            _CtCls = None
        try:
            from fhert._C._lib.openfhe import Plaintext as _PtCls
        except Exception:
            _PtCls = None
        objs = _gc.get_objects()
        # Scan EVERY live object holding a non-None ``_device_address`` (not just
        # Ciphertext) -- the per-bootstrap scratch device buffers can be owned by
        # Plaintext / DeviceArray / stacked-diag wrappers too (the c2s/s2c linear
        # transforms allocate device Plaintext diagonals). A wrapper is LEAKED if
        # its device buffer was already freed by the deterministic reclaim (addr
        # NOT in ``_alloc_map``) but the Python wrapper survives. ``by_type`` /
        # ``leaked_by_type`` break the counts down by Python type so the holder's
        # type is pinned even without the referrer walk.
        wrappers = 0
        healthy = 0
        by_type = {}
        healthy_by_type = {}
        leaked_by_type = {}
        leaked = []
        for o in objs:
            try:
                da = getattr(o, "_device_address", None)
            except Exception:
                continue
            if da is None:
                continue
            addrs = da if isinstance(da, (list, tuple)) else [da]
            try:
                ext_int = [int(_x) for _x in addrs]
                internal = [a - offset for a in ext_int]
            except Exception:
                continue
            if _CtCls is not None and isinstance(o, _CtCls):
                otype = "Ciphertext"
            elif _PtCls is not None and isinstance(o, _PtCls):
                otype = "Plaintext"
            else:
                otype = type(o).__name__
            wrappers += 1
            by_type[otype] = by_type.get(otype, 0) + 1
            in_map = [k in alloc_keys for k in internal]
            if in_map and all(in_map):
                healthy += 1
                healthy_by_type[otype] = healthy_by_type.get(otype, 0) + 1
                continue
            owner = getattr(o, "_device_memory_owner", None)
            if owner is None:
                owner_desc = "None"
            else:
                try:
                    owner_desc = f"{type(owner).__name__}#{id(owner)}"
                except Exception:
                    owner_desc = "<?>"
            leaked_by_type[otype] = leaked_by_type.get(otype, 0) + 1
            if len(leaked) < 8:
                leaked.append((o, otype, ext_int, in_map, owner_desc))
        typestr = ",".join(f"{t}:{c}" for t, c in sorted(by_type.items()))
        htypestr = ",".join(f"{t}:{c}" for t, c in sorted(healthy_by_type.items()))
        ltypestr = ",".join(f"{t}:{c}" for t, c in sorted(leaked_by_type.items()))
        print(f"[reclaim-diag] AGG total_gc={len(objs)} dev_wrappers={wrappers} "
              f"healthy={healthy} leaked={sum(leaked_by_type.values())} "
              f"freed_this_reclaim={len(freed_set)} alloc_map={len(alloc_keys)} "
              f"by_type=[{typestr}] healthy_by_type=[{htypestr}] "
              f"leaked_by_type=[{ltypestr}]",
              file=_gsys.stderr, flush=True)
        if not leaked:
            print("[reclaim-diag] NO leaked wrapper: every live device-buffer "
                  "owner's _device_address addrs are still in _alloc_map (device "
                  "buffers still held) -> no freed-scratch-wrapper leak pinned at "
                  "reclaim #2", file=_gsys.stderr, flush=True)
            return
        for o, otype, ext_int, in_map, owner_desc in leaked[:3]:
            try:
                refs = _gc.get_referrers(o)
            except Exception:
                refs = []
            summ = []
            for r in refs:
                t = type(r).__name__
                try:
                    rep = repr(r)
                except Exception:
                    rep = "<repr-failed>"
                if len(rep) > 220:
                    rep = rep[:220] + "...<trunc>"
                extra = ""
                if isinstance(r, dict):
                    try:
                        ks = list(r.keys())[:8]
                        extra = " keys=" + repr(
                            [f"{type(k).__name__}:{str(k)[:24]}" for k in ks])
                    except Exception:
                        extra = ""
                elif hasattr(r, "f_code") and not isinstance(r, type):
                    try:
                        extra = (f" frame fn={r.f_code.co_name}"
                                 f"@{r.f_code.co_filename}:{r.f_lineno}")
                    except Exception:
                        extra = ""
                elif isinstance(r, (list, tuple)) and not isinstance(r, type):
                    try:
                        extra = (f" len={len(r)} eltypes="
                                 f"{[type(x).__name__ for x in r[:4]]}")
                    except Exception:
                        extra = ""
                summ.append(f"({t}#{id(r)}{extra}: {rep})")
            print(f"[reclaim-diag] leaked wrapper id={id(o)} type={otype} "
                  f"owner={owner_desc} addrs={ext_int[:5]} in_map={in_map[:5]} "
                  f"referrers=[{', '.join(summ)}]",
                  file=_gsys.stderr, flush=True)
    except Exception as _e:
        try:
            print(f"[reclaim-diag] ERROR: {type(_e).__name__}: {_e}",
                  file=_gsys.stderr, flush=True)
        except Exception:
            pass


def _reclaim_bts_device_scratch(snap, survivor_ct):
    """Deterministic device-HBM scratch reclaim after EvalBootstrap, mirroring the
    hand-written CModel-master bump-pointer rewind (``hbm_offset_reset``). fhert
    has no deterministic per-bootstrap scratch reclaim: it relies solely on CPython
    refcount -> Ciphertext.__del__ -> CModelDevice.mem_free, which a live strong
    ref into the EvalMod Chebyshev Paterson-Stockmeyer chain defeats -- so ~2-5 GiB
    of device HBM leaked per bootstrap and OOM'd resnet20 at bootstrap #10 on a
    31 GiB/domain budget.

    This helper frees EVERY device address allocated DURING the just-finished
    EvalBootstrap (i.e. a key of ``dev._alloc_map`` but NOT in ``snap``, the
    pre-bootstrap address set) EXCEPT a keep-set of addresses still alive and
    reused after the call:

      * the surviving result ciphertext ``survivor_ct`` (owns its fresh buffers);
      * the two device caches fhert grows mid-bootstrap and reuses across later
        EvalAdd/EvalMult/Rescale ops -- ``_rns_factor_cache`` (DeviceArray per
        entry, SCALAR ``_device_address``) and ``_monomial_cache`` (device
        Plaintext per entry, LIST ``_device_address``; empty under
        FHE_DEV_BTS_HALF=1, kept defensively).

    This bounds device HBM to persistent + one bootstrap peak.

    Address model (cmodel_device.py:801-836): ``mem_alloc`` stores the INTERNAL
    addr (``aligned_start``) as the ``_alloc_map`` key and returns the EXTERNAL
    addr (``internal + HBM_LOCAL_OFFSET``); ``mem_free`` takes the EXTERNAL addr
    and subtracts the offset. ``snap`` and the keep-set are normalized to INTERNAL
    addrs (external - offset); the free call passes the external addr back.

    The whole body is wrapped in try/except so a wrong attribute name (a future
    binding refactor) NEVER crashes a run -- it logs a warning and proceeds, so
    the worst case is the old gc-only reclaim, not an abort. A per-reclaim log
    line confirms the reclaim is actually reclaiming (a "freed 0" points at a
    stale attribute). Host path: ``snap`` is None -> no-op (host Ciphertext
    ``__del__`` short-circuits on ``_device_address is None``)."""
    if snap is None:
        return
    global _RECLAIM_DIAG_N
    _RECLAIM_DIAG_N += 1
    _diag = (_RECLAIM_DIAG_N == 2)
    try:
        dev = _DEV
        if dev is None:
            return
        offset = int(dev.target.HBM_LOCAL_OFFSET)
        used_before = sum(dev._alloc_map.values())
        keep_internal = set()

        def _add_ext(ext_addrs):
            # ext_addrs: iterable of EXTERNAL device addrs (as stored in
            # _device_address); convert each to its internal _alloc_map key.
            for _a in ext_addrs:
                try:
                    keep_internal.add(int(_a) - offset)
                except (TypeError, ValueError):
                    pass

        # 1. surviving result ciphertext: owns fresh buffers; _device_address is a
        #    LIST of external addrs (one per component). Handle the scalar shape too
        #    (defensive -- Ciphertext is always a list in fhert 1.4.2).
        _surv = getattr(survivor_ct, "_device_address", None)
        if _surv is not None:
            _add_ext(_surv if isinstance(_surv, list) else [_surv])
        # 2. the two device caches grown mid-bootstrap (MUST survive -- freeing
        #    them would corrupt subsequent EvalAdd/EvalMult/Rescale).
        _dctx = getattr(_CC, "_device_context", None)
        if _dctx is not None:
            for _entry in (getattr(_dctx, "_rns_factor_cache", None) or {}).values():
                _a = getattr(_entry, "_device_address", None)
                if _a is not None:
                    _add_ext(_a if isinstance(_a, list) else [_a])
            for _entry in (getattr(_dctx, "_monomial_cache", None) or {}).values():
                _a = getattr(_entry, "_device_address", None)
                if _a is not None:
                    _add_ext(_a if isinstance(_a, list) else [_a])
        # Free every address allocated during the bootstrap (in _alloc_map now but
        # not in the pre-bootstrap snap) that is NOT in the keep-set. mem_free takes
        # the EXTERNAL addr (internal + offset). A RuntimeError from mem_free (a
        # double free / invalid free) is benign here -- mem_free raises BEFORE
        # mutating _free_list so no free-list corruption -- so skip and continue.
        n_freed = 0
        freed_ext = []
        for _internal in list(dev._alloc_map.keys()):
            if _internal in snap or _internal in keep_internal:
                continue
            try:
                _ext = _internal + offset
                dev.mem_free(_ext)
                n_freed += 1
                if _diag:
                    freed_ext.append(_ext)
            except Exception:
                pass
        used_after = sum(dev._alloc_map.values())
        import sys as _rsys
        print(f"[reclaim] bootstrap freed {n_freed} addrs; "
              f"dev_hbm_used_before={used_before / (1024 ** 3):.3f} "
              f"after={used_after / (1024 ** 3):.3f} GiB",
              file=_rsys.stderr, flush=True)
        if _diag:
            _run_reclaim_diag(freed_ext, offset, dev)
    except Exception as _e:
        import sys as _rsys
        print(f"[reclaim] WARNING: device-scratch reclaim skipped: "
              f"{type(_e).__name__}: {_e}", file=_rsys.stderr, flush=True)


def raise_mod(a, lev):
    """SET the modulus level explicitly to ``lev`` (rt_ref ``raise_mod``: SET
    ``level=lev``, keep ``scale_deg``, ``lev in [0, ir_capacity]`` where
    ir_capacity is the IR-mapping prime-chain ceiling). Real HE cannot change a
    cipher's modulus level without a primitive, so the direction decides the
    mechanism (F5: ``lev`` is an ABSOLUTE level index):

      * ``lev == a.level``: no-op (clone) -- rt_ref parity.
      * ``lev <  a.level`` (DOWNWARD): a scale-preserving droplevel via
        ``_drop_to_of_level`` -- the SAME primitive as ``mod_switch``.
      * ``lev >  a.level`` (UPWARD / modraise): real CKKS cannot raise the
        modulus without bootstrap, so ``EvalBootstrap`` (refresh up to near-top)
        + droplevel to ``lev`` (F4: bootstrap has NO per-call target). Needs a
        bootstrap context (``ctx['bootstrap']: True``), else [hard].

    Fidelity deviation (documented): rt_ref keeps ``scale_deg`` on an UPWARD
    raise_mod, but a real HE modulus raise requires a bootstrap that resets the
    scale to 1. The DOWNWARD case has NO deviation (droplevel preserves scale)."""
    op = "raise_mod"
    _require_cipher_or_cipher3(a, op)
    cap = _ctx_ir_capacity()
    if lev < 0 or lev > cap:
        _violation("hard", op,
                   f"lev {lev} out of bounds [0, {cap}] (ir_capacity=IR-mapping chain)")
    lv = int(lev)
    if lv == a.level:
        return a.clone()
    if lv < a.level:
        # downward: scale-preserving droplevel, mirroring rt_ref SET level=lev /
        # keep scale_deg. The new cipher's on-demand ``level`` lands at ``lv``.
        nsd = int(a._ct.GetNoiseScaleDeg())
        ct = _drop_to_of_level(a._ct, _of_level(lv), nsd, op)
        return Cipher._wrap(ct, a.scale_deg, a.is_cipher3, a._n_logical)
    # upward (modraise): bootstrap to near-top, then droplevel to the target.
    if not _BOOTSTRAP_READY:
        _violation("hard", op,
                   f"raise_mod upward {a.level}->{lv} needs bootstrap; seed _CTX "
                   f"with {{'bootstrap': True}} (and a param-consistent bootstrap "
                   f"context)")
    # Snapshot device HBM BEFORE EvalBootstrap so the deterministic reclaim below
    # can free every address allocated DURING it (except the survivor ct + the two
    # reused caches). Host -> None (no-op). See _reclaim_bts_device_scratch.
    _bts_reclaim_snap = (set(_DEV._alloc_map)
                         if (_device_mode() and _DEV is not None) else None)
    ct = _CC.EvalBootstrap(a._ct)
    # Deterministic device-scratch reclaim (mirrors the hand-written CModel-master
    # bump-pointer rewind ``hbm_offset_reset``): free the per-bootstrap temps a live
    # strong ref into the EvalMod Chebyshev Paterson-Stockmeyer chain keeps alive
    # (defeating the CPython refcount -> __del__ -> CModelDevice.mem_free path),
    # which otherwise leaked ~2-5 GiB/bootstrap and OOM'd at bootstrap #10 on a
    # 31 GiB/domain budget. gc.collect() is kept after as a harmless guard; the host
    # path is a no-op (snap None, host Ciphertext __del__ short-circuits anyway).
    _reclaim_bts_device_scratch(_bts_reclaim_snap, ct)
    import gc as _gc
    _gc.collect()
    # ant-faithful nsd reset -- see bootstrap_ciph (FLEX does (post_nsd-1) bounded
    # Rescales for level-alignment parity; FIXED uses the while-loop).
    _post_nsd = int(ct.GetNoiseScaleDeg())
    if _FLEX_MODE:
        for _ in range(_post_nsd - 1):
            ct = _CC.Rescale(ct)
    else:
        while int(ct.GetNoiseScaleDeg()) > 1:
            ct = _CC.Rescale(ct)
    nsd = int(ct.GetNoiseScaleDeg())              # real nsd (1 FIXED; 2 FLEX)
    post_gl = int(ct.GetLevel())                  # recompute AFTER the reset
    target_gl = _of_level(lv)
    if target_gl >= post_gl:
        ct = _drop_to_of_level(ct, target_gl, nsd, op)
    return Cipher._wrap(ct, int(ct.GetNoiseScaleDeg()), False,
                        a._n_logical)  # track REAL nsd; level on demand


def bootstrap_ciph(a, level, slot):
    """Real EvalBootstrap (3b): refresh ``a`` to a fresh high level via the
    coeff-to-slot + eval_mod + slot-to-coeff chain, then droplevel the result
    to the IR target ``level`` when reachable. Requires a bootstrap context
    built in _seed_ctx (ctx['bootstrap']=True -> _BOOTSTRAP_READY). The result
    has ``scale_deg`` = the REAL post-reset noiseScaleDeg (1) and
    ``is_cipher3=False`` (a bootstrapped cipher is relined) -- matching rt_ref's
    contract so a swap swaps cleanly. The result ``level`` is NOT stored/pinned:
    under the lazy bridge it is on demand (``_ctx_ir_capacity() - ct.GetLevel()``),
    reflecting the ACTUAL post-bootstrap (+droplevel) position and cannot drift.

    ant-faithful nsd reset (mirror ant bootstrap.c:1610/1660/1822
    ``while(Get_ciph_sf_degree(raised)>1) Rescale_ciphertext``): after
    EvalBootstrap rescale until nsd == 1. openfhe EvalBootstrap leaves the REAL
    noiseScaleDeg at 2 (FIXEDMANUAL 1.4.2, probed); without the reset the
    post-bootstrap App_relu would be entered at REAL nsd 2 and the emitted
    rescale-after-every-mul schedule would drift up until real nsd exceeds the
    RNS towers (the resnet20 crash mode rt_openfhe fixed). Forcing real nsd back
    to 1 restores the 1->2->1 cap the emitted schedule relies on.

    Target-level fidelity (plan sec.4.4): EvalBootstrap lands the real cipher at
    ``post_gl`` NEAR the top, with NO per-call target level (F4). A non-top IR
    target ``level`` is reached by a post-bootstrap droplevel to
    ``_of_level(level)`` whenever that target is at or below the post-reset
    position. A target ABOVE is left as-is: the lazy bridge has NO stored level
    to "pin", so the result's on-demand ``level`` reflects the actual post_gl --
    the droplevel-upward drift CANNOT occur.

    ``slot`` is the IR ATTR::SLOT linearization slot count; the bootstrap slot
    count is fixed at Setup time (ring_dim//2), so ``slot`` is informational.
    ``level`` is NOT upper-bounded vs q_part_num (rt_ref parity: bootstrap
    refreshes up). A non-cipher operand or a negative target level / no
    bootstrap context is [hard]."""
    op = "bootstrap_ciph"
    _require_cipher_or_cipher3(a, op)
    lv = int(level)
    if lv < 0:
        _violation("hard", op, f"bootstrap target level {lv} < 0")
    if not _BOOTSTRAP_READY:
        _violation("hard", op,
                   "no bootstrap context -- seed _CTX with {'bootstrap': True} "
                   "(3b) to enable real EvalBootstrap; the 3a green-gate "
                   "context is leveld-SHE only")
    global _BTS_CALL_N
    _BTS_CALL_N += 1
    if _env_os.environ.get("FHE_BTS_PROGRESS"):   # env-gated monitor for long runs
        import sys as _sysp
        print(f"[bts #{_BTS_CALL_N}] target={lv} in(gl={int(a._ct.GetLevel())},"
              f"nsd={int(a._ct.GetNoiseScaleDeg())})", file=_sysp.stderr, flush=True)
    # Snapshot device HBM BEFORE EvalBootstrap for the deterministic reclaim below
    # (host -> None, no-op). See _reclaim_bts_device_scratch / the raise_mod site.
    _bts_reclaim_snap = (set(_DEV._alloc_map)
                         if (_device_mode() and _DEV is not None) else None)
    ct = _CC.EvalBootstrap(a._ct)               # near-top (nsd 2; probed)
    # Deterministic device-scratch reclaim (mirrors the hand-written CModel-master
    # bump-pointer rewind ``hbm_offset_reset``): free the per-bootstrap temps a live
    # strong ref into the EvalMod Chebyshev PS chain keeps alive (defeating the
    # CPython refcount -> __del__ -> mem_free path), which otherwise leaked
    # ~2-5 GiB/bootstrap -> OOM at #10 on 31 GiB. gc.collect() stays as a harmless
    # guard; host path is a no-op (snap None).
    _reclaim_bts_device_scratch(_bts_reclaim_snap, ct)
    import gc as _gc
    _gc.collect()
    # ant-faithful nsd reset (mirror ant bootstrap.c ``while(sf_degree>1)
    # Rescale``). openfhe EvalBootstrap leaves the REAL nsd at 2; the reset
    # rescales it down to 1 (FIXEDMANUAL). FLEXIBLEAUTOEXT's nsd is a CONSTANT 2
    # (Rescale does not decrement it; FLEX auto-manages the real scale), so the
    # ``while nsd>1`` form would not terminate -- instead do EXACTLY
    # (post_bootstrap_nsd - 1) Rescales (the SAME tower count as FIXED's reset).
    # This is load-bearing for FLEX level-budget parity: WITHOUT it the FLEX
    # post-bootstrap GetLevel sits 1 deeper than FIXED (the reset's tower drop is
    # skipped), the bootstrap target becomes unreachable from above, and the app
    # exhausts its levels before the next bootstrap (resnet20 ``level underflow``).
    # With it, FLEX lands at the SAME GetLevel as FIXED -> identical level budget.
    _post_nsd = int(ct.GetNoiseScaleDeg())
    if _FLEX_MODE:
        for _ in range(_post_nsd - 1):
            ct = _CC.Rescale(ct)
    else:
        while int(ct.GetNoiseScaleDeg()) > 1:
            ct = _CC.Rescale(ct)
    nsd = int(ct.GetNoiseScaleDeg())            # real nsd (1 FIXED; 2 FLEX) for droplevel
    post_gl = int(ct.GetLevel())                # recompute AFTER the reset
    target_gl = _of_level(lv)
    # droplevel can only drop towers (raise GetLevel); reach the target only when
    # it is at/below the post-reset position. An above-target is left as-is:
    # the lazy bridge has NO stored level to "pin" to lv.
    if target_gl >= post_gl:
        ct = _drop_to_of_level(ct, target_gl, nsd, op)
    return Cipher._wrap(ct, int(ct.GetNoiseScaleDeg()), False,
                        a._n_logical)  # track REAL nsd; level on demand


def copy_ciph(src):
    if not isinstance(src, Cipher):
        _violation("hard", "copy_ciph",
                   f"expected cipher, got {type(src).__name__}")
    return src.clone()


def zero_ciph():
    # Unfixed-scale zero (matches rt_ref): scale_deg 0 so add_ciph/sub_ciph
    # short-circuit a zero operand (zero + X == X, X - zero == X, zero - X ==
    # -X) and SKIP the real EvalAdd/EvalSub -- real CKKS cannot add ciphertexts
    # at mismatched scales. n_logical defaults: an add with a zero operand
    # adopts the other operand's n_logical in the short-circuit.
    return Cipher([], _ctx_input_level(), 0, False)


def free_ciph(a, cnt=0):
    # Release the underlying fhert Ciphertext promptly. The CKKS2PY -fp flag
    # (free_poly) emits this at each cipher's last use; WITHOUT it the live
    # ciphertext set grows ~linearly with the bootstrap count and OOMs the
    # cmodel device HBM (observed ~2-3 GB/bootstrap, OOM at bootstrap #9 on a
    # 30 GiB/domain budget). Nulling _ct drops the fhert Ciphertext ref so its
    # __del__ fires CModelDevice.mem_free (device HBM); nulling _slots drops the
    # cached decoded values and, for an array container, releases the
    # sub-ciphers (which then free their own device buffers). Safe: Cipher.clone()
    # always Clone()s the openfhe Ciphertext -- two Cipher wrappers never share
    # the same _ct -- so releasing one never frees a ciphertext aliased elsewhere;
    # the emitter calls this only at a cipher's last use.
    if a is None:
        return None
    try:
        a._ct = None
        a._slots = None
    except Exception:
        pass
    return None


def free_plain(a):
    # Release the underlying openfhe Plaintext at last use (emitted by
    # -CKKS2PY:fp). Nulling _pt drops the device Plaintext (its __del__ ->
    # CModelDevice.mem_free); nulling _slots drops the cached float list.
    # Plain.clone() builds a fresh Plain (no shared _pt), so this never frees a
    # plaintext aliased elsewhere.
    if a is None:
        return None
    try:
        a._pt = None
        a._slots = None
    except Exception:
        pass
    return None


def free_ciph_poly(a, cnt=0):
    # Same lifetime release as free_ciph for the poly/array fallback name. Handle
    # a Python list/tuple of ciphers (free each) or a single array Cipher.
    if a is None:
        return None
    if isinstance(a, (list, tuple)):
        for _x in a:
            free_ciph(_x)
        return None
    return free_ciph(a, cnt)


# ---------------------------------------------------------------------------
# encode / queries
# ---------------------------------------------------------------------------

def _flatten(buf):
    # Flatten a (possibly multi-dim) Python list/tuple buffer to flat scalars.
    # The emitter (E1) emits array constants as NESTED lists (so a subscript
    # yields the sub-list row); a buffer consumed whole is nested and must be
    # flattened before taking the leading `n`. Parity with rt_ref._flatten.
    out = []
    for x in buf:
        if isinstance(x, (list, tuple)):
            out.extend(_flatten(x))
        else:
            out.append(x)
    return out


def _encode(buf, n, scale_deg, level, op):
    """Build an rt_fhert Plain from a buffer (rt_ref `_encode` parity: bounds
    check the encode level against the IR-mapping ceiling ``ir_capacity``, build
    the logical slot list). The openfhe Plaintext is built lazily at the
    consuming cipher's level when the plain feeds an arithmetic op."""
    n = int(n)
    cap = _ctx_ir_capacity()
    if level < 0 or level > cap:
        _violation("hard", op,
                   f"level {level} out of bounds [0, {cap}] (ir_capacity=IR-mapping chain)")
    if isinstance(buf, (list, tuple)):
        # E1 emits array constants as nested lists; flatten before taking `n`.
        vals = _flatten(buf)
        slots = [float(x) for x in (vals[:n] if n > 0 else vals)]
        if not slots:
            slots = [float(x) for x in vals]
    else:
        # scalar buffer (the emitter visits an ldc literal as a bare number)
        slots = [float(buf)] * max(n, 1)
    return Plain(slots, int(level), int(scale_deg))


def encode_float(buf, n, scale, level):
    return _encode(buf, n, scale, level, "encode_float")


def encode_double(buf, n, scale, level):
    return _encode(buf, n, scale, level, "encode_double")


def encode_dcmplx(buf, n, scale, level):
    """Double-complex encode (Phase 2.3 consumer, provider-agnostic). KEPT as
    rt_ref's REAL-only 2n projection (r0,i0,r1,i1,...) -- NOT native complex
    CKKS slots -- so both providers agree until an e2e dcmplx workload exists.
    The openfhe Plaintext is built lazily at the consuming cipher's level. Body
    copied verbatim from rt_ref."""
    op = "encode_dcmplx"
    n = int(n)
    cap = _ctx_ir_capacity()
    if level < 0 or level > cap:
        _violation("hard", op,
                   f"level {level} out of bounds [0, {cap}] (ir_capacity=IR-mapping chain)")
    width = 2 * n if n > 0 else 2  # n complex => 2n real/imag slots
    if isinstance(buf, (list, tuple)):
        raw = [float(x) for x in buf]
        slots = (raw + [0.0] * width)[:width]
    else:
        # scalar buffer (edge): one complex value (real=buf, imag=0) then zeros
        slots = ([float(buf)] + [0.0] * (width - 1))[:width]
    return Plain(slots, int(level), int(scale))


# ---------------------------------------------------------------------------
# offline data-file encode (Phase 2.3, `-CKKS2PY:df=`) -- copied VERBATIM from
# rt_ref: a provider-agnostic C binary parser (rt_data_def.h format).
# ---------------------------------------------------------------------------

def _read_data_file():
    global _DATA_FILE_CACHE
    if _DATA_FILE_CACHE is not None:
        return _DATA_FILE_CACHE
    if not _DATA_INFO:
        raise FakeRtError("[hard] load_data_entry: no data file "
                          "(get_rt_data_info() not seeded / no -CKKS2PY:df= given)")
    path = _DATA_INFO.get("file") if isinstance(_DATA_INFO, dict) else None
    if not path:
        raise FakeRtError("[hard] load_data_entry: empty data-file path in "
                          "get_rt_data_info() (was -CKKS2PY:df= given?)")
    with open(path, "rb") as f:
        raw = f.read()
    if raw[0:8] != b"!ANTFHE\x00":
        raise FakeRtError(f"[hard] load_data_entry: bad data-file magic "
                          f"(expected !ANTFHE, got {raw[0:8]!r})")
    _DATA_FILE_CACHE = raw
    return raw


def load_data_entry(idx, n):
    """Offline encode support (Phase 2.3, `-CKKS2PY:df=`): read entry `idx` from the
    C binary side data file and return its first `n` message-slot floats. Body
    copied verbatim from rt_ref (provider-agnostic parser)."""
    raw = _read_data_file()
    ent_count = _struct.unpack_from("<Q", raw, 16)[0]
    lut_ofst = _struct.unpack_from("<Q", raw, 24)[0]
    idx = int(idx)
    n = int(n)
    if idx < 0 or idx >= ent_count:
        raise FakeRtError(f"[hard] load_data_entry: idx {idx} out of range "
                          f"[0, {ent_count}) (data-file LUT)")
    ent_type = _struct.unpack_from("<B", raw, 14)[0]   # DATA_FILE_HDR::_ent_type
    if ent_type != 0:  # DE_MSG_F32
        raise FakeRtError(f"[hard] load_data_entry: data-file entry type "
                          f"{ent_type} is not DE_MSG_F32 (Python offline path "
                          f"is float32-only; mirror of ckks/ir2c_ctx.h:101-103)")
    base = lut_ofst + idx * 32
    size = _struct.unpack_from("<I", raw, base + 20)[0]
    bf = _struct.unpack_from("<Q", raw, base + 24)[0]
    ent_ofst = (bf >> 16) & ((1 << 48) - 1)
    count = size // 4   # float32 (DE_MSG_F32)
    if n < 0:
        raise FakeRtError(f"[hard] load_data_entry: negative n={n}")
    if n > count:
        raise FakeRtError(f"[hard] load_data_entry: n={n} exceeds entry slot "
                          f"count {count} (mirror of the C count>=len assert, "
                          f"ir2c_ctx.h:108 / ir2py_handler.h:424) -- the old "
                          f"slots[:n] slice would have silently clamped")
    if not (4096 <= ent_ofst < lut_ofst and ent_ofst + size <= len(raw)):
        raise FakeRtError(f"[hard] load_data_entry: entry offset {ent_ofst} "
                          f"out of range [4096, {lut_ofst}) or past EOF (mirror "
                          f"of C rt_data_file.c:134; a tampered LUT bitfield "
                          f"would otherwise decode header/LUT bytes as floats)")
    slots = list(_struct.unpack_from("<%df" % count, raw, ent_ofst))
    return slots[:n] if n > 0 else []


def level(a):
    """The IR REMAINING level of a cipher/plain, for the emitted `.py` to feed
    ``encode_*`` and branch on. For a Cipher this is the LAZY on-demand value
    (``_ctx_ir_capacity() - ct.GetLevel()`` via the ``Cipher.level`` property) --
    NEVER a stored tracked level, so it cannot drift. For a Plain it is the
    stored REMAINING level (a plain has no ciphertext to query)."""
    if isinstance(a, Cipher):
        return a.level
    if isinstance(a, Plain):
        return a.level
    _violation("hard", "level", f"expected cipher/plain, got {type(a).__name__}")


def sc_degree(a):
    if isinstance(a, Cipher):
        return a.scale_deg
    if isinstance(a, Plain):
        return a.scale_deg
    _violation("hard", "sc_degree",
               f"expected cipher/plain, got {type(a).__name__}")


# ---------------------------------------------------------------------------
# observability / metadata (Phase 2.1) -- copied from rt_ref, adjusted only to
# read rt_fhert's Cipher/Plain `.slots` (which lazily decrypts). validate
# compares the decrypted message to a reference within 10**epsilon (a real-HE
# finding); tm_* are no-ops; dump_cipher_msg is observation-only (never raises).
# ---------------------------------------------------------------------------

def _slots_of(x, op):
    """Coerce a cipher/plain/list/tuple/scalar to a flat float slot list
    (cipher slots are lazy-decrypted via .slots). Copied from rt_ref."""
    if isinstance(x, Cipher):
        return list(x.slots)
    if isinstance(x, Plain):
        return list(x.slots)
    if isinstance(x, (list, tuple)):
        return [float(v) for v in x]
    if isinstance(x, (int, float)):
        return [float(x)]
    _violation("hard", op,
               f"expected cipher/plain/list/scalar, got {type(x).__name__}")


def validate(ciph, msg, length, epsilon):
    """Plaintext-EQUIVALENT validate for real HE: lazy-decrypt `ciph` and
    compare to `msg` within 10**`epsilon` (mirrors C Validate). A mismatch is a
    real-HE/compiler finding, reported as [strict]. Body copied from rt_ref."""
    op = "validate"
    c_slots = _slots_of(ciph, op)
    m_slots = _slots_of(msg, op)
    n = int(length)
    err = 10.0 ** float(epsilon)
    count = min(n, len(c_slots), len(m_slots))
    for i in range(count):
        if abs(float(c_slots[i]) - float(m_slots[i])) > err:
            _violation("strict", op,
                       f"slot {i}: |{c_slots[i]} - {m_slots[i]}| > "
                       f"tol(1e{epsilon})")
            return


def tm_start(label):
    """Timing start (no-op; a real-HE timing layer is out of Phase-3 scope).
    Returns None."""
    return None


def tm_taken(label):
    """Timing taken (no-op). Returns None."""
    return None


def dump_cipher_msg(name, ciph, length):
    """Observability: write a repr of `ciph`'s first `length` message slots
    (lazily decrypted) to stderr. Pure observation -- never raises, never
    touches _OUTPUTS/_VIOLATIONS. Copied from rt_ref."""
    import sys
    n = int(length)
    try:
        slots = _slots_of(ciph, "dump_cipher_msg")
        print(f"[dump] {name}: {list(slots[:n])}", file=sys.stderr)
    except FakeRtError:
        print(f"[dump] {name}: <unprintable {type(ciph).__name__}>",
              file=sys.stderr)
    return None


# ---------------------------------------------------------------------------
# export surface -- the FROZEN PUBLIC runtime contract (identical to rt_ref's
# __all__). A star import binds exactly these into the emitted module's
# namespace; the driver glue (_reset/_seed_*/_CTX/_INPUTS/_OUTPUTS/_VIOLATIONS/
# configure/_ctx_*/...) + the openfhe helpers (_ensure_of/_of_level/
# _encrypt_values/_decrypt_slots/...) stay module-level with a leading `_` and
# are NOT exported.
# ---------------------------------------------------------------------------
__all__ = [
    # value classes
    "Cipher", "Plain", "FakeRtError",
    # IO
    "get_input_data", "set_output_data",
    # arithmetic
    "add_ciph", "add_plain", "add_scalar",
    "sub_ciph", "sub_plain", "sub_scalar",
    "mul_ciph", "mul_plain", "mul_scalar", "mul_mono_ciph",
    # unary / shape
    "neg_ciph", "rotate_ciph", "conjugate_ciph",
    # modulus mgmt
    "relin", "rescale_ciph", "mod_switch", "upscale_ciph", "raise_mod",
    "bootstrap_ciph",  # bootstrap_ciph(a, level, slot) [real EvalBootstrap]
    # lifecycle
    "copy_ciph", "zero_ciph",
    "free_ciph", "free_plain", "free_ciph_poly",
    # encode / queries
    "encode_float", "encode_double", "encode_dcmplx", "load_data_entry",
    "level", "sc_degree",
    # observability / metadata (Phase 2.1)
    "validate", "tm_start", "tm_taken", "dump_cipher_msg",
    # rtpy :rtt timing (mirror rtlib-c; no-op phase 1, full mirror phase 2)
    "CMPLR_TM_START", "CMPLR_TM_END",
]