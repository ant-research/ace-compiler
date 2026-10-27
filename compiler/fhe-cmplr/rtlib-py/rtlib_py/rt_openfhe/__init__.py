"""`rt_openfhe` -- real-HE rtlib-py provider via upstream openfhe-python.

This is Phase 3 of the ckks IR -> Python backend: a SECOND rtlib-py provider
(alongside the plaintext reference `rt_ref`) that executes emitted `-CKKS2PY`
modules against **real CKKS ciphertexts** under OpenFHE 1.5.1 (via its
pybind11 binding `openfhe-python`, a ready upstream library -- NOT one we
write). It is selected by the single compiler field `POLY2PY_CONFIG::
_py_rt_module`: `-CKKS2PY:rt=rt_openfhe` makes the emitter write
``from rtlib_py.rt_openfhe import *`` and the driver fetch this provider.

Fidelity decisions (locked with the user, plan
`doc/rtlib-py-integrate/rt_openfhe_python_plan.md`, decisions #1/#2):

  * **FIXEDMANUAL leveld-SHE mirror.** IR operators map one-to-one to OpenFHE
    primitives and KEEP rt_ref's `[strict]` level/scale invariants copied
    verbatim, so a real-HE run still catches compiler level/scale bugs:
      ``add_ciph``=EvalAdd, ``sub_*``=EvalSub, ``mul_ciph``=EvalMultNoRelin
      (-> cipher3, nsd+=), ``mul_plain``/``mul_mono_ciph``=EvalMult(ct, pt),
      ``mul_scalar``=EvalMult(ct, float) (nsd+1), ``neg_ciph``=EvalNegate,
      ``relin``=Relinearize (no-op if already 2-comp),
      ``rescale_ciph``=Rescale + (if cipher3) immediate Relinearize back to 2
      components.
  * **Level mapping (LAZY bridge).** openfhe ``GetLevel()`` starts at 0 at the
    chain top and increments by 1 per Rescale (CONSUMED depth); an IR REMAINING
    level counts DOWN from the chain top. The IR-mapping capacity is
    ``_ctx_ir_capacity()`` (= ``GetMultiplicativeDepth()+1`` for 3a; for 3b the
    minimal chain ``max(input_level, max_target + bts_depth + 1)`` -- the TRUE
    chain ``_ctx_num_primes()`` adds the R down-headroom reserve below IR 0),
    so ``ir_level = _ctx_ir_capacity() - ct.GetLevel()`` and ``scale_deg ==
    ct.GetNoiseScaleDeg()`` exactly. A fresh input cipher is encoded at openfhe
    level 0 (3a) / the app top (3b), nsd 1 -> ``ir_level = ir_capacity``,
    ``scale_deg = 1``.

    THIS PROVIDER STORES NO TRACKED ``ir_level``: ``Cipher.level`` is an on-demand
    ``@property`` (``_ctx_ir_capacity() - ct.GetLevel()``), and every operator
    builds its result via ``Cipher._wrap(ct, scale_deg, is_cipher3, n_logical)``
    with NO level argument -- the result level is always recomputed from the
    produced ciphertext. This is the structural elimination of the
    droplevel-upward drift the earlier EAGER bridge suffered: it stored a tracked
    level and updated it per op, so a post-bootstrap track-only droplevel (target
    above the post-bootstrap position) pinned ``level=lv`` while the real cipher
    sat deeper, and a later ``_align`` then computed an unreachable upward drop
    (``droplevel target out of reachable`` [hard] on resnet20). With nothing
    stored, nothing drifts; ``level(a)`` is always the cipher's actual remaining
    headroom, and ``num_primes`` (not the smaller emitted ``input_level``) is the
    conversion basis, so it is ALWAYS ``>= 0`` (a cipher cannot be deeper than the
    chain). The two REMAINING<->CONSUMED IR-boundaries are just ``level()`` (on
    demand) and the bootstrap/raise_mod droplevel target (``_of_level(lv)``);
    internal ops (``_align``, ``_drop_to_of_level``) stay in OpenFHE's own
    GetLevel units. (rt_ref keeps a STORED level -- it is plaintext with no
    ``GetLevel`` to query, and has no drift source; the two providers' ``Cipher``
    are different classes, as before -- the frozen contract covers function
    signatures, not ``Cipher`` internals.)

Real-HE precision (3a green-gate workloads) converges to the plaintext result
within ~1e-10; `rt_ref` is the golden reference and `rt_openfhe` agrees within
the verify epsilon. Real ``EvalBootstrap`` (3b, ring 2^12) recovers a small
message within ~2e-4 -- the Phase-3 swap behind the single Phase-2.4 name
``bootstrap_ciph(a, level, slot)``.

Lazy import: `openfhe` is imported INSIDE ``_build_context`` (via ``_seed_ctx``), so merely importing
this module (or listing it in the provider registry) never crashes on a host
without the binding. Real-HE tests skip (ctest SKIP_RETURN_CODE 77) when
`import openfhe` fails; the `rt_ref`/`phase1_verify` path is untouched.

Modulus-level drops (``mod_switch``; downward ``raise_mod``): the upstream
`openfhe-python` binding exposes NO ``LevelReduce`` (only ``Rescale``/
``ModReduce``, both decrement nsd) -- BUT a scale-preserving droplevel IS
reachable via ``cc.Compress(ct, towersLeft, noiseScaleDeg=ct.GetNoiseScaleDeg())``:
with the CURRENT nsd passed explicitly the ModReduce loop is skipped and only
``LevelReduceInternalInPlace`` runs (a pure tower drop, rns-leveledshe.cpp:358-381).
The binding DEFAULT ``noiseScaleDeg=1`` is a footgun (it ModReduces to nsd=1
first, changing the scale -> a rescale, NOT a mod_switch), so the helper always
passes the explicit nsd. ``Compress`` is documented as a serialization-compress
primitive; using it as a compute-stage droplevel is an empirically-verified
unconventional usage (EvalAdd/EvalMult continue correctly post-drop). See
``_drop_to_of_level`` and each operator's docstring.

``upscale_ciph`` is still track-only (plan-sanctioned) unless a workload needs a
real scale raise. Upward ``raise_mod`` (modraise) and ``bootstrap_ciph``'s
target level need a real ``EvalBootstrap`` (refresh up to near-top) followed by
a droplevel to the target, since bootstrap has NO per-call target level (F4);
both raise `[hard]` without a bootstrap context (``ctx['bootstrap']: True``).
"""

# ---------------------------------------------------------------------------
# configuration + stores (mirror rt_ref's per-run state names so the driver,
# rtlib_py.common.driver, reaches them as plain module attributes)
# ---------------------------------------------------------------------------

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

# ReSBM bottom-bootstrap down-headroom (FHE_BTS_RESERVE). The .py's ReSBM schedule
# drives the critical-path cipher to IR 1 (its bookkeeping bottom) before each
# bootstrap. openfhe EvalBootstrap (the binding's default non-StCFirst path --
# BTSlotsEncoding=false, so ckksrns-fhe.cpp:434 is not taken) ModRaises the input
# to GL 0 before consuming, so its only hard floor is the cipher needing >=2 towers
# (a 1-tower cipher hits a dcrtpoly error); it does NOT need the lvlb_dec+2 towers
# below the input that the StCFirst path's L0-(lvlb+2) check would demand. _build_bootstrap_context
# therefore reserves R = max(FHE_BTS_RESERVE, 2) towers below IR 0: IR 1 then has
# R+1 >= 3 towers (1 margin over the 2-tower floor; verified at N=65536). The
# IR<->openfhe mapping stays at ir_capacity (the chain WITHOUT R) via
# _ctx_ir_capacity(), so the .py's deepest point IR 1 maps to GL (ir_capacity-1)
# and R towers remain below it as bootstrap headroom; _ctx_num_primes() (TRUE chain
# = ir_capacity + R) is still used for Compress/droplevel tower math. 3a (no
# bootstrap) ignores R, so ir_capacity == num_primes and green-gate is unchanged.
import os as _env_os
_BTS_RESERVE = int(_env_os.environ.get("FHE_BTS_RESERVE", "0"))
_BTS_LEVEL_BUDGET = [int(x) for x in _env_os.environ.get(
    "FHE_BTS_LEVEL_BUDGET", "4,4").split(",")][:2]  # s2c/c2s level budget
# Bootstrap correction factor (rt_fhert delta #3; mirrored only under FLEX).
# FIXEDMANUAL uses the 1.5.1 auto correction factor (via the signature-only
# EvalBootstrapSetup call). Under FLEX the auto factor can leave the EvalMod
# Chebyshev degree too small for the deep resnet20 chain (rt_fhert proves
# correctionFactor=13 -- the fhert clamp max -- is required), so this default
# is applied to the FLEX bootstrap context. Env-overridable.
_BTS_CORRECTION = int(_env_os.environ.get("FHE_BTS_CORRECTION", "13"))
# Bootstrap scaling technique (env ``FHE_BTS_SCALING``, default ``fixedmanual``
# -- preserves the prior baseline byte-for-byte; mirror rt_fhert:169, whose
# default ``flexibleautoext`` is the fhert canonical choice). FIXEDMANUAL is the
# green-gate default and the resnet20 rescale-drift baseline. FLEXIBLEAUTO
# (``flex``/``flexibleauto``/``noext``, no EXT tower) and FLEXIBLEAUTOEXT
# (``flexibleautoext``/``fext``/``ext``) re-anchor the scale at every
# AdjustCiphertext, eliminating the FIXEDMANUAL scale-drift that accumulates
# across resnet20's ~17 bootstraps on the seed-1 input -- the ant-vs-openfhe
# CKKS scale-drift RESULT thread-2 clean fix (a single FLEX bootstrap is ~500x
# more precise than FIXEDMANUAL at the same sf; ~2-3x slower but drift-free).
# Default ``fixedmanual`` keeps existing runs unchanged: ``FHE_BTS_SCALING``
# unset -> FIXEDMANUAL at BOTH SetScalingTechnique call-sites (the 3a leveld-SHE
# context and the 3b EvalBootstrap context). The FLEX re-anchor shim (ported
# from rt_fhert:169-204) engages ONLY under FLEX so FIXEDMANUAL (default) stays
# byte-identical: the constant-nsd=2 tracking (_FLEX_MODE), the +1 Compress
# compensation (_FLEX_EXT), and the bounded (post_nsd-1) nsd-reset in
# bootstrap_ciph/raise_mod bridge the OpenFHE 1.5.1 Rescale/Compress
# accounting (Rescale does NOT decrement noiseScaleDeg under FLEX; FLEX-EXT
# Compress drops an extra tower); without them the ``while nsd>1`` no-op spins
# and the bootstrap target becomes unreachable (resnet20 level-underflow
# crash). Like rt_fhert, FLEX's drift-free AdjustCiphertext re-anchor is the
# anti-drift lever; the shim only routes the accounting so the chain
# progresses (rt_fhert FLEX-EXT seed-1 proof: max_abs=5.9e-4 argmax=5=golden).
_BTS_SCALING = _env_os.environ.get("FHE_BTS_SCALING", "fixedmanual").lower()
# FLEX re-anchor shim (ported from rt_fhert:169-204, threads the drift fix):
#   * ``_FLEX_MODE``: True under ANY non-FIXEDMANUAL technique (FLEXIBLEAUTO or
#     FLEXIBLEAUTOEXT). The constant-nsd=2 tracking (Cipher clamp) + the
#     bounded (post_nsd-1) nsd-reset in bootstrap_ciph/raise_mod apply to both
#     -- under FLEX Rescale does NOT decrement noiseScaleDeg (it stays at 2;
#     FLEX auto-manages the real scale), so the while-loop form spins; the
#     bounded form does EXACTLY (post_nsd - 1) Rescales (the SAME tower count
#     as FIXEDMANUAL's reset), keeping FLEX land at the SAME GetLevel.
#   * ``_FLEX_EXT``: True ONLY under FLEXIBLEAUTOEXT. The +1 Compress
#     compensation applies here -- FLEX-EXT Compress(ct, towersLeft=X) drops
#     ONE EXTRA tower (reserves the EXT auxiliary modulus for the flexible
#     first-level scale), so to land at INTENDED towers=X pass towers_left+1.
#     The no-EXT / FIXEDMANUAL Compress is EXACT (no compensation).
# Both are reset only by _build_bootstrap_context (3b, sets them from the
# tag) / _build_context (3a, resets to False); the default ``fixedmanual``
# keeps them False -> the FLEX shim lines never engage (byte-identical
# baseline preserved).
_FLEX_MODE = False          # set True by _build_bootstrap_context under any FLEX
_FLEX_EXT = False           # set True by _build_bootstrap_context under FLEX-EXT only
_FLEX_SCALE = 2             # under FLEX the noiseScaleDeg is a constant 2
_CTX_IR_CAPACITY = None   # the IR-mapping basis (chain without the R reserve)


def _parse_flex_scaling(env_val, default):
    """Map a scaling env value to a tag in {``flexext``, ``flexnoext``,
    ``fixed``}. FLEX re-anchor shim (ported from rt_fhert:169-204). The tag
    drives ``_FLEX_MODE`` (any FLEX) and ``_FLEX_EXT`` (EXT only) and MUST
    agree with :func:`_scaling_technique` (the SetScalingTechnique enum
    selector) on every input so the FLEX shim gates match the built
    technique. Spelling variants:

      * ``fixed*`` / ``manual`` / ``""`` -> ``fixed`` (FIXEDMANUAL baseline);
      * ``flex`` / ``flexibleauto`` / ``noext`` / ``flexnoext`` ->
        ``flexnoext`` (FLEXIBLEAUTO, no EXT tower);
      * ``flexibleautoext`` / ``fext`` / ``flexext`` / ``ext`` -> ``flexext``
        (FLEXIBLEAUTOEXT, EXT tower);
      * unknown -> ``fixed`` (preserves baseline; stderr notice).
    """
    v = (env_val if env_val is not None else (default or "")).lower()
    if v.startswith("fixed") or v in ("", "manual"):
        return "fixed"
    if v in ("flex", "flexibleauto", "noext", "flexnoext"):
        return "flexnoext"
    if v in ("flexibleautoext", "fext", "flexext", "ext"):
        return "flexext"
    import sys as _sys
    _sys.stderr.write(
        "rt_openfhe: unknown FHE_BTS_SCALING=%r, falling back to fixed\n"
        % (env_val,))
    return "fixed"


def _scaling_technique(of, name):
    """Map an FHE_BTS_SCALING value to an openfhe ScalingTechnique enum
    (``of`` = the openfhe binding -- rt_openfhe imports it lazily as ``_of``
    inside _build_context and receives it as the ``of`` arg of
    _build_bootstrap_context). Spelling variants: ``fixed*``/``manual``/``""``
    -> FIXEDMANUAL (baseline); ``flex``/``flexibleauto``/``noext``/``flexnoext``
    -> FLEXIBLEAUTO (no EXT tower); ``flexibleautoext``/``fext``/``flexext``/
    ``ext`` -> FLEXIBLEAUTOEXT (EXT tower). Unknown -> FIXEDMANUAL with a stderr
    notice (baseline preserved)."""
    v = (name or "").lower()
    if v.startswith("fixed") or v in ("", "manual"):
        return of.ScalingTechnique.FIXEDMANUAL
    if v in ("flex", "flexibleauto", "noext", "flexnoext"):
        return of.ScalingTechnique.FLEXIBLEAUTO
    if v in ("flexibleautoext", "fext", "flexext", "ext"):
        return of.ScalingTechnique.FLEXIBLEAUTOEXT
    import sys as _sys
    _sys.stderr.write(
        "rt_openfhe: unknown FHE_BTS_SCALING=%r, falling back to FIXEDMANUAL\n"
        % (name,))
    return of.ScalingTechnique.FIXEDMANUAL


class FakeRtError(Exception):
    """A violated [hard] or (in strict mode) [strict] CKKS constraint."""


def configure(strict=True, on_violation="raise"):
    """Configure strictness -- byte-identical to rt_ref: strict=False records
    [strict] violations to stderr and continues; [hard] errors always raise."""
    global _STRICT
    _STRICT = bool(strict)


def _reset():
    """Clear per-run state (mirror rt_ref). Keeps the built OpenFHE context
    (_CC/_PK/_SK/_N_SLOTS/_CTX_PARAMS_KEY/_BOOTSTRAP_READY) so repeated
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
    get_context_params() dict. Imports `openfhe` lazily HERE so module import
    never crashes without the binding. Reuses the cached context when the
    params are unchanged. ctx_params keys (emitted by poly2py_driver.cxx):
    provider, poly_degree, security_level, mul_depth, input_level,
    first_prime_bit_num, scaling_factor_bit_num, q_part_num, hamming_weight,
    rotate_keys; a 3b test may add ``'bootstrap': True`` to build a toy
    EvalBootstrap context instead."""
    global _CTX, _CTX_PARAMS_KEY
    _CTX = dict(ctx_params)
    key = _ctx_key(ctx_params)
    if key != _CTX_PARAMS_KEY:
        _build_context(ctx_params)
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
    """The lowered ``input_level`` field from the emitted get_context_params, as
    a fallback / app-level notion (mul_depth+1). NOTE: under the lazy bridge the
    authoritative chain capacity is :func:`_ctx_num_primes` (the REAL built
    chain, ``GetMultiplicativeDepth()+1``); this helper is kept only as the
    fallback app-top for the input-encryption seam and is otherwise unused.
    Historical note: the field is occasionally filled WRONG (E models
    input_level=3 while mul_depth=26), so ``mul_depth+1`` -- not the raw field
    -- is the safe app-top."""
    return _ctx_mul_depth() + 1


def _ctx_num_primes():
    """The REAL prime-chain capacity = ``_CC.GetMultiplicativeDepth() + 1`` (the
    total tower count of the BUILT OpenFHE context). This is the lazy bridge's
    single conversion basis for the two IR-boundaries (``level(a)`` and the
    bootstrap/raise_mod droplevel target) and for the encode/raise_mod ceiling
    ``cap``. It is ALWAYS ``>= ct.GetLevel()`` (a cipher cannot sit deeper than
    the chain), so ``_ctx_num_primes() - ct.GetLevel() >= 0`` ALWAYS -- the
    structural guarantee that eliminates the negative-tracked-level drift the
    eager bridge produced (it used ``input_level = mul_depth+1``, which for a
    bootstrap context is SMALLER than the real chain ``GetMultiplicativeDepth()+1``
    -- a cipher refreshed by EvalBootstrap lands deeper than ``mul_depth``,
    giving ``input_level - GetLevel < 0``).

    For a 3a leveld-SHE context ``GetMultiplicativeDepth() == mul_depth``, so
    this equals the old ``_ctx_input_level()`` (a no-op for the green-gate /
    E-model corpus). For a 3b bootstrap context
    ``GetMultiplicativeDepth() == mul_depth + GetBootstrapDepth(...)`` (the real
    chain is augmented with the bootstrap reserve), so this is the only basis
    that keeps ``level(a)`` non-negative post-bootstrap."""
    return int(_CC.GetMultiplicativeDepth()) + 1 if _CC is not None else _ctx_mul_depth() + 1


def _ctx_ir_capacity():
    """The IR<->openfhe mapping basis = the prime-chain capacity WITHOUT the
    ReSBM bottom-bootstrap reserve R. _build_bootstrap_context sets this to
    ``max(input_level, max_target + bts_depth + 1)`` -- the minimal chain that
    fits the input (IR input_level) and lets every ReSBM bootstrap target be
    reached from EvalBootstrap's post-output landing by a downward droplevel; R
    towers below IR 0 are then added as bootstrap down-headroom (see _BTS_RESERVE).
    ``level(a)``, ``_of_level``, and the encode/raise_mod caps use THIS (not the
    true chain) so the .py's emitted IR levels (1..input_level) map to the SAME
    openfhe GetLevel region as green-gate, while the R reserved towers sit BELOW
    IR 0 (GL ir_capacity .. true_primes-1). This is the DECOUPLED mapping:
    deepening the chain no longer shifts the IR region (the failure mode of a
    naive FHE_BTS_DEPTH_MARGIN experiment, which coupled the mapping to the new
    num_primes and still hit the bottom).

    For 3a (no bootstrap), _CTX_IR_CAPACITY is set to ``num_primes`` directly in
    _build_context, so this is a no-op and green-gate is byte-identical. For 3b,
    R = max(_BTS_RESERVE, 2) >= 2 always, so ir_capacity is TRUE chain minus R
    (>= 2). Always >= the GetLevel of any cipher the .py can observe
    (the .py bottoms out at IR 1 = GL ir_capacity-1; only EvalBootstrap's
    INTERNAL depletion touches the reserved region, never exposed to level())."""
    if _CTX_IR_CAPACITY is not None:
        return _CTX_IR_CAPACITY
    return _ctx_num_primes()  # pre-build fallback (3a or before _seed_ctx)


def _ctx_q_part_num():
    return int(_CTX.get("q_part_num", 0))


def _default_slot_count():
    # Mirrors rt_ref's shape; rt_openfhe actually uses _N_SLOTS (the real ring
    # slot count) for zero_ciph. Provided for driver-glue parity.
    if _N_SLOTS:
        return _N_SLOTS
    pd = int(_CTX.get("poly_degree", 0))
    return pd // 2 if pd > 0 else _DEFAULT_SLOTS


# ---------------------------------------------------------------------------
# constraint helpers -- copied VERBATIM from rt_ref so [hard]/[strict] semantics
# (and the level/scale [strict] invariants that catch compiler bugs) are
# byte-identical. Real HE itself stays silent on misuse; these checks surface
# it. (FakeRtError is this module's own class, but the message text is identical
# so the ported unit tests match on the same substrings.)
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
    # via Adjust_level (ciphertext.h:203-238). rt_openfhe mirrors this with a
    # REAL scale-preserving droplevel of the higher-ir-level operand before the
    # Eval* call (see _align_ct_for_binop); the tracked result level is min.
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
    CKKS state ``(level, scale_deg, is_cipher3)`` and the number of *logical*
    slots (the meaningful message length -- a real cipher always holds the full
    ``_N_SLOTS`` ring slots; we pad inputs with 0 and truncate the decrypted
    vector back to the logical length so the verify gate compares like rt_ref).

    ``_slots`` is three-state: ``None`` = not yet decrypted (lazy-decrypt on
    first ``.slots`` access via the secret key), ``list[float]`` = decoded
    values, ``list[Cipher]`` = an array container (array-typed input store/load
    via ``__getitem__``/``__setitem__`` -- then ``_ct`` is ``None``).

    The constructor is the **encryption seam**: the driver builds input ciphers
    as ``Cipher([float values], input_level, 1, False)``, so a list of floats
    triggers ``MakeCKKSPackedPlaintext`` + ``Encrypt`` at the openfhe level
    ``_of_level(level) = _ctx_num_primes() - level`` (openfhe level 0 = the chain
    top for a fresh 3a input; the app-top for a 3b input), nsd 1. ``level`` is
    consumed here ONLY to pick the encryption level and is then DISCARDED -- the
    IR REMAINING level is never stored (see ``level`` property below). Arithmetic
    ops build Ciphers via ``_wrap`` (no re-encryption, no level arg)."""

    __slots__ = ("_ct", "_slots", "scale_deg", "is_cipher3", "_n_logical")
    # NOTE: no ``level`` slot -- under the lazy bridge the IR REMAINING level is
    # NOT stored; it is computed on demand by the ``level`` property below
    # (``_ctx_num_primes() - ct.GetLevel()``). Storing a tracked level (the old
    # eager bridge) is what let it drift from the real cipher level
    # (post-bootstrap droplevel-upward); on-demand has nothing to drift.

    def __init__(self, slots, level, scale_deg, is_cipher3=False):
        # ``level`` is the IR REMAINING level of the ENCRYPTION SEAM only (the
        # driver builds inputs as ``Cipher(values, input_level, 1, False)``; a
        # few tests pass ``Cipher(values, m.level, ...)``). It is consumed HERE
        # to pick the openfhe encryption level (``_of_level(level)``) and then
        # DISCARDED -- it is never stored; ``self.level`` later recomputes it
        # on demand from the wrapped ciphertext. So a stored tracked level can
        # never go stale (no drift source).
        self.scale_deg = int(scale_deg)
        # FLEX re-anchor (per scale-drift RESULT thread-2; shim ported from
        # rt_fhert:536-604): under FLEXIBLEAUTOEXT/FLEXIBLEAUTO the
        # noiseScaleDeg is a CONSTANT (_FLEX_SCALE=2) -- FLEX auto-manages the
        # real scale; the level (lazy bridge) is the tracked quantity. Clamp
        # the tracked scale_deg to the constant for all non-zero ciphers
        # (keep 0 for the zero-operand sentinel). FIXEDMANUAL (default,
        # _FLEX_MODE False) tracks the real nsd (sd_a+sd_b / -1) -- the clamp
        # is a no-op so the baseline is byte-identical.
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
        deeper than the IR region -- the structural elimination of the
        negative-tracked-level drift the eager bridge produced. An array
        container (``_ct is None``) has no single ciphertext; its elements are
        fresh inputs, so it reports the full chain top (the only query path is
        informational -- arithmetic indexes the elements first)."""
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
        # FLEX re-anchor (per scale-drift RESULT thread-2; shim ported from
        # rt_fhert:536-604): produce-side mirror of Cipher.__init__'s clamp --
        # track the constant nsd(=2) under FLEX (any non-FIXEDMANUAL). FLEX
        # auto-manages the real scale, so the level (via the lazy bridge)
        # is the tracked quantity. FIXEDMANUAL (default, _FLEX_MODE False)
        # tracks the real nsd -- the clamp is a no-op so the baseline is
        # byte-identical.
        if _FLEX_MODE and c.scale_deg != 0:
            c.scale_deg = _FLEX_SCALE
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
            c = Cipher._wrap(self._ct.Clone(), self.scale_deg,
                             self.is_cipher3, self._n_logical)
            if isinstance(self._slots, list):
                c._slots = list(self._slots)  # copy any cached decoded values
            return c
        # Array container: deep-clone each element. ``self.level`` (on-demand
        # property; num_primes for a fresh array) is accepted by the ctor's
        # array path but unused there -- kept for call-shape parity.
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

    Single lazy-bridge conversion: this is the SAME basis used by ``level(a)``
    (on-demand REMAINING) and by the bootstrap/raise_mod droplevel targets, so
    the two IR-boundaries and the input-encryption seam all agree. For a 3a
    context ``_ctx_ir_capacity() == _ctx_num_primes() == mul_depth+1 ==
    _ctx_input_level()``, so this is byte-identical to the old ``input_level -
    ir_level`` for the green-gate corpus; only bootstrap contexts (where
    ``_ctx_ir_capacity()`` is the minimal chain ``max(input_level, max_target +
    bts_depth + 1)`` and the R reserve sits below IR 0) see a change,
    and there the change is the structural drift fix."""
    return _ctx_ir_capacity() - int(ir_level)


def _drop_to_of_level(ct, target_gl, nsd, op):
    """Scale-preserving droplevel (a.k.a. mod_switch): drop the openfhe
    ``Ciphertext`` ``ct`` DOWN to openfhe GetLevel ``target_gl`` (CONSUMED
    space: raising GetLevel by dropping towers) while keeping
    ``noiseScaleDeg = nsd`` unchanged -- the real-CKKS analog of a modulus-level
    drop that preserves the scale (rt_ref ``mod_switch`` / downward
    ``raise_mod``: SET the level, KEEP the scale degree).

    Pure CONSUMED (GetLevel) space: ``target_gl`` is the destination GetLevel,
    NOT an IR REMAINING level. The lazy bridge keeps this internal droplevel
    primitive in OpenFHE's own units -- the REMAINING<->CONSUMED conversion
    lives ONLY at the two IR-boundaries (``level()`` and the bootstrap/raise_mod
    droplevel target), both via ``_of_level``; everything inside is GetLevel.

    Implementation: ``cc.Compress(ct, towersLeft, noiseScaleDeg=nsd)`` with the
    CURRENT nsd passed EXPLICITLY. The binding default ``noiseScaleDeg=1`` is a
    footgun: it first ModReduces the cipher to nsd=1 (changing the scale) before
    LevelReduce -- that would turn this into a rescale, NOT a mod_switch
    (verified: a cipher3 at nsd=2 + default Compress -> nsd 1). With the nsd
    passed explicitly the ``while (nsd > noiseScaleDeg) ModReduce`` loop is
    skipped (rns-leveledshe.cpp:358-381) and only ``LevelReduceInternalInPlace``
    runs (a pure tower drop, scale unchanged). C++ ``LevelReduce`` exists but is
    NOT pybind-bound, so ``Compress``-with-explicit-nsd is the ONLY bound
    scale-preserving droplevel.

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
    # towers). Convert that openfhe RuntimeError into a clean [hard] with a clear
    # message: a nsd-2 cipher (e.g. a cipher3) needs >= 3 towers to drop one, so
    # mod_switch/raise_mod on such a cipher in a too-shallow context (mul_depth
    # giving fewer towers) is physically impossible under real HE -- use a deeper
    # context. This is the cipher3-boundary the plan's V5 anticipated.
    if towers_left < int(nsd):
        _violation("hard", op,
                   f"towers_left {towers_left} < noiseScaleDeg {int(nsd)}: a "
                   f"{int(nsd)}-scale cipher needs at least {int(nsd) + 1} "
                   f"towers to drop one level (Compress requires "
                   f"towersLeft>=nsd); this context (total {total} towers, "
                   f"target GetLevel {target_gl}) is too shallow. "
                   f"mod_switch/raise_mod on a high-nsd (e.g. cipher3) cipher "
                   f"needs a deeper multiplicative depth.")
    # FLEX re-anchor (per scale-drift RESULT thread-2; shim ported from
    # rt_fhert:755-769): FLEXIBLEAUTOEXT Compress(ct, towersLeft=X) drops ONE
    # EXTRA tower (reserves the EXT auxiliary modulus for the flexible
    # first-level scale), yielding X-1 towers -- the cipher lands 1 level
    # DEEPER than requested. To land at the INTENDED target_gl (towers =
    # towers_left), pass towers_left+1 so Compress yields towers_left towers.
    # Without this, every droplevel (mod_switch / downward raise_mod / binop
    # _align / the post-bootstrap droplevel) lands 1 level too deep under
    # FLEX-EXT, and the per-bootstrap 1-level deficit accumulates to level
    # exhaustion (resnet20). EXT-specific: FLEXIBLEAUTO (no-EXT) and
    # FIXEDMANUAL Compress are EXACT (no extra tower), gated on _FLEX_EXT
    # (NOT _FLEX_MODE). The guards above use the INTENDED towers_left (the
    # real tower count the cipher will hold), so they stay correct. The
    # default fixedmanual keeps _FLEX_EXT False -> compress_towers ==
    # towers_left (byte-identical baseline).
    compress_towers = towers_left + 1 if _FLEX_EXT else towers_left
    return _CC.Compress(ct, compress_towers, noiseScaleDeg=int(nsd))


def _align_ct_for_binop(a, b, op):
    """Mirror ant ``Adjust_level`` (ciphertext.h:203-238) for a binary cipher op
    so both ciphertexts sit at the SAME openfhe GetLevel for EvalAdd/EvalSub/
    EvalMultNoRelin: drop the FRESHER operand (LOWER GetLevel == MORE headroom)
    DOWN to the more-depleted operand's GetLevel. In CONSUMED space the target
    is simply ``max(a.GetLevel(), b.GetLevel())`` -- no IR-REMAINING conversion
    here: the lazy bridge keeps internal ops in OpenFHE's own GetLevel units, so
    only the ``level()`` query and the bootstrap/raise_mod target cross the
    REMAINING<->CONSUMED boundary (via ``_of_level``). The equal-level case is a
    no-op pass-through (green-gate add/mul unchanged). Returns ``(a_ct, b_ct)``
    -- no result level: under the lazy bridge the result level is on demand from
    the produced ciphertext (``Cipher.level`` property), so the caller does not
    need (and cannot usefully be handed) a tracked level.

    A zero operand (``scale_deg == 0``) is a pass-through waiver: the real op is
    short-circuited by add/sub_ciph before reaching here, and mul_ciph on a zero
    simply broadcasts through unchanged (no drop needed -- the zero's real
    ciphertext sits at the top and a mismatched-level EvalMultNoRelin is the
    zero-broadcast, mirroring the pre-lazy behavior)."""
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
    """MakeCKKSPackedPlaintext(vals, nsd, of_level, slots=N) + Encrypt(PK)."""
    pt = _CC.MakeCKKSPackedPlaintext(vals, noiseScaleDeg=int(nsd),
                                    level=int(of_level), slots=_N_SLOTS)
    return _CC.Encrypt(_PK, pt)


def _decrypt_slots(ct):
    """Decrypt to a float list of the full ring slot count (lazy-decrypt)."""
    pt = _CC.Decrypt(_SK, ct)
    pt.SetLength(_N_SLOTS)
    return [float(x) for x in pt.GetRealPackedValue()]


def _plain_packed_for(cipher, plain, op):
    """Build the N-length value list a plain contributes to `cipher`'s slots,
    broadcasting a 1-slot plain across all logical slots or doing an elementwise
    op when the plain packs the same count (rt_ref `_broadcast_plain` parity),
    then padding to the full ring. Raises [hard] on an unbroadcastable count.
    A zero_ciph (wildcard `_n_logical==0`) adopts the plain's shape (rt_ref
    empty-cipher broadcast parity)."""
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
        # (evaluator.c:192-216: no slot-count check; full-ring Mul_poly with the
        # plain zero-padded on the right). Accepts plain LONGER or SHORTER than
        # the cipher (conv 6144 > 3072; gap 4 < 256), matching the meaningful
        # product on the leading slots.
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


# ---------------------------------------------------------------------------
# context build (lazy `import openfhe`; cached on identical params)
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
# their sizes -- fully intact. first_prime_bit_num is NOT raised (each RNS
# prime is hard-capped at 60 bits by the binding). Documented honestly in the
# module docstring + _build_context.
_SCALE_BIT_CAP = 40


def _ctx_key(ctx):
    """Cache key for the openfhe context build (only the params that change the
    context; the bootstrap flag forces a rebuild)."""
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
        int(_env_os.environ.get("FHE_BTS_SF", "0")),
        tuple(ctx.get("rotate_keys", []) or []),
    )


def _build_context(ctx):
    """Build (or rebuild) the OpenFHE CryptoContext + keys from ctx params.
    Imports openfhe lazily. A ``ctx['bootstrap']`` truthy flag builds the toy
    Set-A EvalBootstrap context (3b) instead of the plain leveld-SHE one (3a)."""
    global _CC, _PK, _SK, _N_SLOTS, _BOOTSTRAP_READY, _ROT_KEYS, _NATIVE, _CTX_IR_CAPACITY
    global _FLEX_MODE, _FLEX_EXT
    import os as _os
    import sys as _sys
    # Self-locate the upstream openfhe-python binding so the provider works when
    # driven directly (the harness/tests ALSO inject this via run_py_exec; this
    # is the robust path). OPENFHE_PY_BUILD_DIR overrides; default the build dir.
    _bd = _os.environ.get("OPENFHE_PY_BUILD_DIR") or "/workspace/openfhe-python/build"
    if _os.path.isdir(_bd) and _bd not in _sys.path:
        _sys.path.insert(0, _bd)
    import openfhe as _of  # lazy: module import stays crash-free without it

    _NATIVE = int(_of.get_native_int())
    if ctx.get("bootstrap") or ctx.get("need_bts"):
        _build_bootstrap_context(_of, ctx)
        return

    # 3a: plain leveld-SHE FIXEDMANUAL context, faithful to the emitted params
    # modulo the toy-safety scaling cap.
    sc = min(int(ctx.get("scaling_factor_bit_num", 50)), _SCALE_BIT_CAP)
    params = _of.CCParamsCKKSRNS()
    params.SetScalingTechnique(_scaling_technique(_of, _BTS_SCALING))  # FHE_BTS_SCALING (default fixedmanual = decision #2 baseline; rt_fhert:169 + ant-vs-openfhe scale-drift FLEX re-anchor kills resnet20 drift; ~2-3x slower but drift-free)
    params.SetKeySwitchTechnique(_of.KeySwitchTechnique.HYBRID)   # bootstrap-ready
    poly_deg = int(ctx.get("poly_degree", 1 << 4))
    params.SetMultiplicativeDepth(int(ctx.get("mul_depth", 0)))
    params.SetFirstModSize(int(ctx.get("first_prime_bit_num", 60)))
    params.SetScalingModSize(sc)
    params.SetRingDim(poly_deg)
    params.SetSecurityLevel(_of.SecurityLevel.HEStd_NotSet)  # toy poly_degree
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
    # 3a is always FIXEDMANUAL shim-wise (the bootstrap-only FLEX shim is NOT
    # engaged; the byte-identical default ``fixedmanual`` keeps the enum aligned
    # too). The explicit reset handles a later 3b-build -> 3a-rebuild transition
    # where the previous builder set _FLEX_MODE/_FLEX_EXT True (rt_fhert:1025).
    _FLEX_MODE = False                   # 3a: no FLEX shim (FIXEDMANUAL tracking)
    _FLEX_EXT = False                    # 3a: no EXT Compress compensation
    # 3a: no bootstrap reserve applies; IR capacity == TRUE chain.
    _CTX_IR_CAPACITY = int(_CC.GetMultiplicativeDepth()) + 1


def _build_bootstrap_context(of, ctx):
    """Build the model-driven EvalBootstrap context (Phase 3 wiring). Ring
    dimension, first/moduli bit sizes, and multiplicative depth are derived
    from the emitted get_context_params (ctx: poly_degree /
    first_prime_bit_num / scaling_factor_bit_num / mul_depth) -- NOT the toy
    Set-A constants. Uses the scaling technique selected by FHE_BTS_SCALING
    (default ``fixedmanual`` = decision #2 baseline; ``flexibleauto``/
    ``flexibleautoext`` enable the FLEX re-anchor that eliminates the
    FIXEDMANUAL scale-drift on resnet20's ~17 bootstraps, ~2-3x slower but
    drift-free -- see _scaling_technique); level_budget [4,4] +
    UNIFORM_TERNARY remain fixed-scheme policy (mirror ant/seal/phantom +
    sibling grounding). Mirrors examples/pke/simple-ckks-bootstrapping.py
    setup order."""
    global _CC, _PK, _SK, _N_SLOTS, _BOOTSTRAP_READY, _ROT_KEYS, _CTX_IR_CAPACITY
    global _FLEX_MODE, _FLEX_EXT
    # FLEX re-anchor scaling tag (computed once here, consumed for
    # secret-key dist + correction factor + _FLEX_MODE/_FLEX_EXT below).
    _bts_tag = _parse_flex_scaling(_BTS_SCALING, "fixedmanual")
    params = of.CCParamsCKKSRNS()
    # FLEX re-anchor (shim ported from rt_fhert delta #3): under FLEX the secret
    # key distribution switches to SPARSE_TERNARY + correctionFactor=13. rt_fhert
    # uses SPARSE_TERNARY because UNIFORM_TERNARY raises "Degree must be <=
    # correction factor" under 1.4.2 FLEX (dense key's EvalMod Chebyshev degree
    # exceeds the auto correction factor), and -- empirically -- even on 1.5.1
    # (where the throw is absent and a single FLEX-EXT UNIFORM bootstrap yields
    # excellent refresh precision of ~1e-12), the resnet20 deep chain
    # (17 bootstraps + degree-20 Chebyshev Relu interleavings, ring 65536) under
    # UNIFORM_TERNARY produces GARBAGE (max_abs=91, seed-1, probed 2026-09-15)
    # while rt_fhert's SPARSE_TERNARY + cf=13 on 1.4.2 gives the class-correct
    # max_abs=5.9e-4 argmax=5=golden. SPARSE_TERNARY (small Hamming weight)
    # keeps the EvalMod Chebyshev degree low and the sine approximation stable
    # across the resnet20 increasing-magnitude relu chain. Gated on the FLEX
    # tag so FIXEDMANUAL (default) keeps UNIFORM_TERNARY (byte-identical
    # baseline preserved); _bts_correction is 0 (auto) for FIXEDMANUAL and
    # _BTS_CORRECTION (default 13) for FLEX.
    if _bts_tag == "fixed":
        secret_key_dist = of.SecretKeyDist.UNIFORM_TERNARY  # baseline (sibling + ant/seal/phantom grounding)
        _bts_correction = 0  # auto via signature-only EvalBootstrapSetup
    else:
        secret_key_dist = of.SecretKeyDist.SPARSE_TERNARY  # FLEX: rt_fhert delta #3
        _bts_correction = _BTS_CORRECTION  # default 13 (env-overridable)
    params.SetSecretKeyDist(secret_key_dist)
    # Model-driven bootstrap context (Phase 3 wiring): ring/moduli/depth come
    # from the emitted get_context_params (ctx), NOT the toy Set-A constants.
    # level_budget [4,4] + the OpenFHE-native depth formula are policy (mirror
    # ant/seal/phantom fixed-scheme + sibling grounding; IMPLEMENTATION-PLAN section 4/
    # section 5.1 Step2). HEStd_NotSet kept (HEStd_128_classic would auto-raise the ring
    # -- deferred, plan section 6.4). num_slots stays full packing (plan section 6.8, no
    # multi-num_slots this phase).
    params.SetSecurityLevel(of.SecurityLevel.HEStd_NotSet)
    params.SetRingDim(int(ctx.get("poly_degree", 1 << 12)))
    # native-int-aware moduli default, overridable by the model's prime/scaling
    # bit nums (64-bit: first 60 / dcrt 59; 128-bit: 89 / 78).
    _native128 = (of.get_native_int() == 128)
    first_mod = int(ctx.get("first_prime_bit_num", 89 if _native128 else 60))
    dcrt_bits = int(ctx.get("scaling_factor_bit_num", 78 if _native128 else 59))
    # FHE_BTS_SF overrides the scaling-modulus bit size on the bootstrap path.
    # The .py emits the compiler's simulation-era sf=56; FIXEDMANUAL at a deep
    # bootstrap chain suffers scaling-factor drift (q_i vs Delta), and sf=56
    # gives only ~6-bit refresh precision at the resnet20 bootstrap levels
    # (inGL 38/52, D=60). openfhe's own 64-bit bootstrap recommendation (see
    # doc/.../openfhe-bts-scaling-mode.md section 17) is FIXEDMANUAL + sf=59, which
    # empirically gives ~9-10 bit refresh (vs 6 at sf=56) -- below the degree-20
    # Chebyshev relu blowup threshold. sf is a per-prime SIZE, not a count, so
    # it does NOT change the post-bootstrap available level count (stays = the
    # .py's IR target = ant's). Default = the .py's emitted value (56) for
    # green-gate parity; set FHE_BTS_SF=59 for resnet20.
    import os as _os_sf
    dcrt_bits = int(_os_sf.environ.get("FHE_BTS_SF", str(dcrt_bits)))
    params.SetScalingModSize(dcrt_bits)
    # FHE_BTS_SCALING (default fixedmanual = decision #2 baseline; FLEXIBLEAUTO
    # / FLEXIBLEAUTOEXT re-anchor the scale at every AdjustCiphertext, killing
    # the FIXEDMANUAL drift that accumulates across resnet20's ~17 bootstraps on
    # the seed-1 input -- the ant-vs-openfhe CKKS scale-drift thread-2 fix.
    params.SetScalingTechnique(_scaling_technique(of, _BTS_SCALING))
    params.SetFirstModSize(first_mod)
    level_budget = list(_BTS_LEVEL_BUDGET)  # FHE_BTS_LEVEL_BUDGET (default [4,4]); see _BTS_LEVEL_BUDGET
    _bts_depth = of.FHECKKSRNS.GetBootstrapDepth(level_budget, secret_key_dist)
    # ---- Minimal modulus-chain sizing (replaces ``levels_after = mul_depth``, which
    # over-allocated the C-bootstrap depth + ~25 towers of unused top slack, roughly
    # doubling Q and forcing HEStd_NotSet). The OpenFHE chain only needs to fit two
    # IR-REMAINING quantities:
    #   (1) the input, emitted at IR ``input_level``; and
    #   (2) the deepest ReSBM bootstrap *target* reachable from EvalBootstrap's
    #       natural post-output landing by a downward droplevel.
    # EvalBootstrap (the non-StCFirst path -- the openfhe-python binding defaults
    # EvalBootstrapSetup BTSlotsEncoding=false, so ckksrns-fhe.cpp:434 is not taken)
    # ModRaises the input to GL 0 and runs EvalMod+CoeffsToSlots top-down, landing at
    # openfhe GetLevel ``bts_depth-1`` (verified: postGL=bts_depth-1 across budgets
    # [4,4]/[3,3]/[2,2] at N=65536); the ant-style nsd-reset then rescales once
    # (nsd 2->1, GetLevel+1), so the post-bootstrap landing the droplevel must reach
    # is ``bts_depth``. The deepest ReSBM target is the C model's app portion
    # ``mul_level - Mul_depth_of_bootstrap()`` (== the emitted ``bootstrap_ciph``
    # level args -- resnet20 emits 14/17, and 32-15=17; Mul_depth_of_bootstrap is 15
    # for hamming_weight<=192, else 19; core/src/scheme_info.cxx). The minimal IR
    # capacity is therefore ``max(input_level, max_target + bts_depth)`` (+1 tower
    # safety margin so the max-target droplevel is not at the exact boundary). The
    # ``levels_after`` bookkeeping and the chain depth follow from ir_capacity. A
    # future emitter may pass ``max_bts_target`` explicitly; until then the formula
    # is used.
    # Below IR 0, R towers reserve EvalBootstrap down-headroom at the .py's deepest
    # point (IR 1). The non-StCFirst path's only hard floor is the cipher needing
    # >=2 towers (it ModRaises before consuming, so it does NOT need lvlb_dec towers
    # below the input -- unlike the StCFirst path's L0-(lvlb+2) check, which this
    # binding does not take); R=2 gives IR 1 three towers (1 margin over the 2-tower
    # floor; verified to bootstrap without "Not enough levels"/dcrtpoly error at
    # N=65536). R is tiny relative to the old lvlb_dec+2 guess because the binding
    # is on the ModRaise-first path. FHE_BTS_RESERVE overrides (backward compat).
    mul_depth   = int(ctx.get("mul_depth", 10))
    hw          = int(ctx.get("hamming_weight", 0))
    _c_bts_d    = 15 if 0 < hw <= 192 else 19   # CTX_PARAM::Mul_depth_of_bootstrap
    _max_target = int(ctx.get("max_bts_target",
                             max(0, (mul_depth + 1) - _c_bts_d)))  # deepest bootstrap target (IR)
    _in_lev     = int(ctx.get("input_level", mul_depth + 1))
    _CTX_IR_CAPACITY = max(_in_lev, _max_target + _bts_depth + 1)  # +1 safety margin
    levels_after = _CTX_IR_CAPACITY - _bts_depth - 1
    R = max(_BTS_RESERVE, 2)                     # 2-tower floor + 1 margin; env override
    depth = levels_after + _bts_depth + R
    params.SetMultiplicativeDepth(depth)
    cc = of.GenCryptoContext(params)
    for f in (of.PKE, of.KEYSWITCH, of.LEVELEDSHE, of.ADVANCEDSHE, of.FHE):
        cc.Enable(f)
    num_slots = cc.GetRingDimension() // 2
    # The mandatory bootstrap setup order (from the upstream example). Under
    # FLEX the SPARSE_TERNARY + correctionFactor=13 (delta #3) requires the
    # explicit correctionFactor kwarg (md rt_fhert:1144); the 1.5.1 binding
    # accepts it. FIXEDMANUAL (default fixedmanual) keeps the signature-only
    # call (auto correction factor; byte-identical baseline preserved).
    if _bts_correction > 0:
        cc.EvalBootstrapSetup(level_budget, slots=num_slots,
                              correctionFactor=_bts_correction)
    else:
        cc.EvalBootstrapSetup(level_budget)
    keys = cc.KeyGen()
    cc.EvalMultKeyGen(keys.secretKey)
    cc.EvalBootstrapKeyGen(keys.secretKey, num_slots)
    # Model rotation keys -- mirror 3a _build_context (:613-615). The bootstrap
    # branch must ALSO generate galois keys for the emitted rotate_keys, else the
    # model's rotate_ciph(idx) hits [hard] no rotation key (EvalBootstrapKeyGen
    # only generates the bootstrap's own CoeffsToSlots/SlotsToCoeffs keys, not
    # the model's arbitrary negative/positive rotations like -4096).
    rot = list(ctx.get("rotate_keys", []) or [])
    if rot:
        cc.EvalRotateKeyGen(keys.secretKey, rot)
    _CC, _PK, _SK = cc, keys.publicKey, keys.secretKey
    _N_SLOTS = num_slots
    _BOOTSTRAP_READY = True
    # FLEX re-anchor (per scale-drift RESULT thread-2; shim ported from
    # rt_fhert:1158-1159): drive the constant-nsd=2 shim (_FLEX_MODE) and the
    # +1 Compress compensation (_FLEX_EXT) from the scaling tag computed at
    # the top of this function (it also set the secret-key dist + correction
    # factor for SPARSE_TERNARY under FLEX).
    #   * _FLEX_MODE = any FLEX (FLEXIBLEAUTO or FLEXIBLEAUTOEXT) -- the
    #     constant-nsd tracking and the bounded (post_nsd-1) nsd-reset apply
    #     to both; the binding treats every non-FIXEDMANUAL technique
    #     identically in Rescale (nsd constant) / ModReduce.
    #   * _FLEX_EXT = EXT only -- the +1 Compress compensation is EXT-specific
    #     (no-EXT/FIXEDMANUAL Compress is exact).
    # FIXED (default ``fixedmanual``) -> both False -> the shim lines never
    # engage (byte-identical baseline preserved).
    _FLEX_MODE = _bts_tag in ("flexext", "flexnoext")
    _FLEX_EXT = (_bts_tag == "flexext")
    _ROT_KEYS = frozenset(rot)


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
    parity); return it. The real op operates on all _N_SLOTS; this is the
    logical count for output truncation. A zero_ciph (wildcard `_n_logical==0`)
    adopts the other operand's shape -- it is the + / - identity (rt_ref's
    empty-slot broadcast), so add/sub_ciph(zero, x) returns x's count rather
    than a [hard] mismatch."""
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
    # Skips the real EvalAdd so the scale_deg=0 zero is never added to a
    # higher-scale term -- real CKKS forbids adding mismatched-scale ciphers.
    # Cloned so the caller owns the handle (mirrors rt_ref's list(slots) copy).
    if a.scale_deg == 0:
        return Cipher._wrap(b._ct.Clone(), b.scale_deg, False, b._n_logical)
    if b.scale_deg == 0:
        return Cipher._wrap(a._ct.Clone(), a.scale_deg, False, a._n_logical)
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
    pt = _plain_pt(a, p, a.scale_deg, op)
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
    # zero - X == -X. Skips the real EvalSub on a mismatched-scale zero.
    if b.scale_deg == 0:
        return Cipher._wrap(a._ct.Clone(), a.scale_deg, False, a._n_logical)
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
    pt = _plain_pt(a, p, a.scale_deg, op)
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
    pt = _plain_pt(a, p, p.scale_deg, op)
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
    pt = _plain_pt(a, p, p.scale_deg, op)
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
    # for +k does NOT enable rotation by -k (confirmed: EvalRotate(ct,-1) with
    # only key +1 raises). Without the key the binding raises RuntimeError, so
    # surface a clean [hard] here naming the missing index instead.
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
    # parity). A native complex-packing conjugate (automorphism 2N-1) is not
    # implemented; no e2e dcmplx workload exists yet (plan open question).
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
    # update; noiseScaleDeg drops by 1.
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
    the helper passes ``ct.GetNoiseScaleDeg()`` so the scale is preserved.

    Tracking mirrors rt_ref exactly (``level-1``, ``scale_deg``/``is_cipher3``
    unchanged). NOT emitted by the 3a green-gate corpus, but the emitted
    ``test_allops`` module emits ``mod_switch(e)`` as a dead-end statement, so
    this now executes cleanly under rt_openfhe (it [hard]-raised pre-patch).
    A cipher3 operand stays cipher3 (Compress keeps all 3 components; verified)
    -- rt_ref instead clears ``is_cipher3`` here, a plaintext simplification the
    real-HE path does NOT mirror (documented divergence; no emitted corpus puts
    a cipher3 through mod_switch)."""
    op = "mod_switch"
    _require_cipher_or_cipher3(a, op)
    if a.level - 1 < 0:
        _violation("hard", op, f"level underflow: {a.level}-1 < 0")
    nsd = int(a._ct.GetNoiseScaleDeg())   # preserve scale: pass current nsd (F1/F3)
    ct = _drop_to_of_level(a._ct, a._ct.GetLevel() + 1, nsd, op)  # drop 1 tower
    return Cipher._wrap(ct, a.scale_deg, a.is_cipher3, a._n_logical)


def upscale_ciph(a, bits):
    """Track-only (plan-sanctioned for the 3a corpus): increment the tracked
    scale_deg without a real op. A real scale raise would be EvalMult(ct,
    2**bits) + Rescale -- but that changes the level and is not needed by any
    3a workload. The tracking lets a compiler DAG that probes the state machine
    proceed; NOT emitted by the green-gate corpus."""
    op = "upscale_ciph"
    _require_cipher_or_cipher3(a, op)
    if a._ct is not None:
        return Cipher._wrap(a._ct.Clone(), a.scale_deg + 1, False, a._n_logical)
    # Array container: preserve the element ciphers (track-only bumps the
    # container scale_deg; elements keep their own state, mirroring clone()).
    return Cipher([c.clone() for c in a._slots], a.level, a.scale_deg + 1,
                  a.is_cipher3)


def raise_mod(a, lev):
    """SET the modulus level explicitly to ``lev`` (rt_ref ``raise_mod``: SET
    ``level=lev``, keep ``scale_deg``, ``lev in [0, ir_capacity]`` where
    ir_capacity is the IR-mapping prime-chain ceiling). Real HE
    cannot change a cipher's modulus level without a primitive, so the direction
    decides the mechanism (F5: ``lev`` is an ABSOLUTE level index and the
    emitted corpus's ``raise_mod(e_8, 1)`` is DOWNWARD 2->1, non-top):

      * ``lev == a.level``: no-op (clone) -- rt_ref parity.
      * ``lev <  a.level`` (DOWNWARD, the emitted case): a scale-preserving
        droplevel ``(a.level - lev)`` towers via ``_drop_to_of_level`` -- the
        SAME primitive as ``mod_switch``. Tracking: ``level=lev``, keep
        ``scale_deg``, keep ``is_cipher3`` (a cipher3 stays cipher3 under a pure
        tower drop; rt_ref clears ``is_cipher3`` -- a documented plaintext
        divergence no emitted corpus exercises).
      * ``lev >  a.level`` (UPWARD / modraise): real CKKS cannot raise the
        modulus without bootstrap, so ``EvalBootstrap`` (refresh up to near-top)
        + droplevel to ``lev`` (F4: bootstrap has NO per-call target, so the
        target must be reached by a post-bootstrap drop). ``scale_deg`` resets
        to 1 (post-bootstrap canonical scale). Needs a bootstrap context
        (``ctx['bootstrap']: True``), else [hard].

    Fidelity deviation (documented): rt_ref keeps ``scale_deg`` on an UPWARD
    raise_mod, but a real HE modulus raise requires a bootstrap that resets the
    scale to 1 -- an inherent consequence of real HE (shared with
    ``bootstrap_ciph``), not an rt_openfhe simplification. The DOWNWARD case has
    NO deviation (droplevel preserves the scale, exactly rt_ref)."""
    op = "raise_mod"
    _require_cipher_or_cipher3(a, op)
    # Ceiling = the IR-mapping capacity ``_ctx_ir_capacity()`` (the chain WITHOUT
    # the R down-headroom reserve; == ``GetMultiplicativeDepth()+1`` for 3a). NOT
    # q_part_num (RNS key-switch decomposition count, 1..3) -- the old bound
    # wrongly rejected legitimate top-of-chain levels (relu 12 vs q_part 3) -- and
    # NOT the lowered ``input_level`` field, which for a bootstrap context is
    # SMALLER than the chain and would reject valid mid-chain targets.
    cap = _ctx_ir_capacity()
    if lev < 0 or lev > cap:
        _violation("hard", op,
                   f"lev {lev} out of bounds [0, {cap}] (ir_capacity=IR-mapping chain)")
    lv = int(lev)
    if lv == a.level:
        return a.clone()
    if lv < a.level:
        # downward (the emitted case): scale-preserving droplevel, mirroring
        # rt_ref SET level=lev / keep scale_deg. The new cipher's on-demand
        # ``level`` lands at ``lv`` (ir_capacity - target_gl, target_gl =
        # ir_capacity - lv).
        nsd = int(a._ct.GetNoiseScaleDeg())
        ct = _drop_to_of_level(a._ct, _of_level(lv), nsd, op)
        return Cipher._wrap(ct, a.scale_deg, a.is_cipher3, a._n_logical)
    # upward (modraise): bootstrap to near-top, then droplevel to the target.
    if not _BOOTSTRAP_READY:
        _violation("hard", op,
                   f"raise_mod upward {a.level}->{lv} needs bootstrap; seed _CTX "
                   f"with {{'bootstrap': True}} (and a param-consistent bootstrap "
                   f"context)")
    ct = _CC.EvalBootstrap(a._ct)
    # ant-faithful nsd reset (see bootstrap_ciph): openfhe EvalBootstrap leaves
    # real nsd=2; force it to 1 (ant bootstrap.c:1610/1822) so the tracked nsd
    # reflects reality and downstream mul/rescale guards are not blinded.
    # FLEX re-anchor (per scale-drift RESULT thread-2; shim ported from
    # rt_fhert:1782-1788): FLEXIBLEAUTOEXT/FLEXIBLEAUTO's noiseScaleDeg is a
    # CONSTANT 2 (Rescale does NOT decrement it under FLEX; FLEX auto-manages
    # the real scale), so the ``while nsd>1`` form would NOT terminate -- a
    # no-op spin. Instead do EXACTLY (post_nsd - 1) Rescales (the SAME tower
    # count as FIXEDMANUAL's reset) so FLEX lands at the SAME GetLevel as
    # FIXED (level-budget parity); without it the FLEX post-bootstrap
    # GetLevel sits 1 deeper than FIXED -> the target is unreachable from
    # above -> level exhaustion (resnet20 level-underflow). FIXEDMANUAL
    # (default, _FLEX_MODE False): the real-nsd while loop (byte-identical
    # baseline).
    _post_nsd = int(ct.GetNoiseScaleDeg())
    if _FLEX_MODE:
        for _ in range(_post_nsd - 1):
            ct = _CC.Rescale(ct)
    else:
        while int(ct.GetNoiseScaleDeg()) > 1:
            ct = _CC.Rescale(ct)
    nsd = int(ct.GetNoiseScaleDeg())              # post-reset real nsd (1 FIXED; 2 FLEX); track REAL
    post_gl = int(ct.GetLevel())                  # recompute AFTER the reset
    target_gl = _of_level(lv)
    # droplevel can only drop (raise GetLevel); a target ABOVE the post-reset
    # position is unreachable without another bootstrap => the cipher stays at
    # post_gl (the lazy bridge has no tracked level to "pin", so the result's
    # on-demand ``level`` simply reflects the actual post-gl -- NO drift).
    if target_gl >= post_gl:
        ct = _drop_to_of_level(ct, target_gl, nsd, op)
    return Cipher._wrap(ct, int(ct.GetNoiseScaleDeg()), False,
                        a._n_logical)  # track REAL nsd (1 after reset); level on demand


def bootstrap_ciph(a, level, slot):
    """Real EvalBootstrap (3b): refresh ``a`` to a fresh high level via the
    coeff-to-slot + eval_mod + slot-to-coeff chain, then droplevel the result
    to the IR target ``level`` when reachable. Requires a bootstrap context
    built in _seed_ctx (ctx['bootstrap']=True -> _BOOTSTRAP_READY). The result
    has ``scale_deg=1`` (the canonical post-bootstrap scale) and
    ``is_cipher3=False`` (a bootstrapped cipher is relined) -- matching rt_ref's
    contract so a swap swaps cleanly. The result ``level`` is NOT stored/pinned
    to the IR target: under the lazy bridge it is on demand
    (``_ctx_num_primes() - ct.GetLevel()``), so it reflects the ACTUAL
    post-bootstrap (+droplevel) position and can never drift.

    Target-level fidelity (plan section 4.4): EvalBootstrap lands the real cipher at
    ``post_gl`` NEAR the top, with ``levelsAvailableAfterBootstrap`` headroom
    -- NOT always openfhe level 0, and with NO per-call target level (F4). So a
    non-top IR target ``level`` is reached by a post-bootstrap droplevel to
    ``_of_level(level) = num_primes - level`` (the SAME scale-preserving
    primitive as ``mod_switch``) whenever that target is at or below the
    post-bootstrap position (reachable by dropping towers). A target ABOVE the
    post-bootstrap position (e.g. ``level=depth``, the top) cannot be reached
    by a droplevel (which only drops): the cipher stays at ``post_gl`` and the
    on-demand ``level`` reflects that actual headroom -- there is NO tracked
    level to "pin" to ``lv`` (the eager bridge's pinning was the droplevel-
    upward drift root cause on multi-bootstrap nets; the lazy bridge removes
    it structurally). A true upward raise would need another bootstrap.

    ``slot`` is the IR ATTR::SLOT linearization slot count; the bootstrap slot
    count is fixed at Setup time (ring_dim//2), so ``slot`` is informational.
    ``level`` is NOT upper-bounded vs q_part_num (rt_ref parity: bootstrap
    refreshes up). A non-cipher operand or a negative target level is [hard].
    If no bootstrap context was built, [hard] (3a green-gate workloads do not
    bootstrap)."""
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
    ct = _CC.EvalBootstrap(a._ct)               # near-top (F4: not always level 0, nsd often 2)
    # ant-faithful nsd reset (mirror ant bootstrap.c:1610/1660/1822
    # ``while(Get_ciph_sf_degree(raised)>1) Rescale_ciphertext``): ant rescales
    # the bootstrapped cipher until its scale degree (nsd) == 1, with an explicit
    # IS_TRUE(sf_degree==1) at the callsite. openfhe EvalBootstrap instead leaves
    # the REAL noiseScaleDeg at 2 (F4/F7). Previously this provider READ that real
    # nsd=2 and then DISCARDED it, returning a hardcoded tracked scale_deg=1 --
    # which blinded the rescale_ciph/mul_ciph guards. With real nsd 2 but tracked
    # 1, every post-bootstrap App_relu is entered at REAL nsd 2, so the emitted
    # rescale-after-every-mul sequence (which caps nsd only at nsd-1 ENTRY:
    # mul 1+1=2 -> rescale 2-1=1) instead drifts UP (mul 2+2=4 -> rescale 4-1=3
    # -> ... -> 16): real nsd exceeds the remaining RNS towers and openfhe
    # SetLevel throws ``multiplicative depth [52] insufficient`` (the resnet20
    # crash; trace: bootstrap out=(38,2,1) -> rescale (42,16,2) -> throw, real
    # nsd 16 > limbNum 11). Forcing real nsd back to 1 here -- one openfhe
    # Rescale per excess nsd, each dropping one tower exactly like ant's
    # Rescale_ciphertext (C1: ant Rescale IS a tower drop, not metadata-only) --
    # restores the 1->2->1 cap the emitted schedule relies on. EvalBootstrap
    # refreshes UP to a low GetLevel, so the reset's tower cost still leaves the
    # cipher above the IR target and the droplevel below reaches the target.
    #
    # FLEX re-anchor (per scale-drift RESULT thread-2; shim ported from
    # rt_fhert:1858-1865): FLEXIBLEAUTOEXT/FLEXIBLEAUTO's noiseScaleDeg is a
    # CONSTANT 2 (Rescale does NOT decrement it under FLEX; FLEX auto-manages
    # the real scale), so the ``while nsd>1`` form would NOT terminate -- a
    # no-op spin. Instead do EXACTLY (post_bootstrap_nsd - 1) Rescales (the
    # SAME tower count as FIXEDMANUAL's reset) so FLEX lands at the SAME
    # GetLevel as FIXED (level-budget parity). WITHOUT this bounded form the
    # FLEX post-bootstrap GetLevel sits 1 deeper than FIXED (the reset's
    # tower drop is skipped), the bootstrap target becomes unreachable from
    # above, and the app exhausts its levels before the next bootstrap
    # (resnet20 ``level underflow``); WITH it, FLEX keeps the same level
    # budget. FIXEDMANUAL (default, _FLEX_MODE False): the real-nsd while
    # loop (real nsd decrements to 1) -- byte-identical baseline.
    _post_nsd = int(ct.GetNoiseScaleDeg())
    if _FLEX_MODE:
        for _ in range(_post_nsd - 1):
            ct = _CC.Rescale(ct)
    else:
        while int(ct.GetNoiseScaleDeg()) > 1:
            ct = _CC.Rescale(ct)
    nsd = int(ct.GetNoiseScaleDeg())            # post-reset real nsd (1 FIXED; 2 FLEX); track REAL
    post_gl = int(ct.GetLevel())                # recompute AFTER the reset (it raised GetLevel)
    target_gl = _of_level(lv)
    # droplevel can only drop towers (raise GetLevel); reach the target only when
    # it is at/below the post-reset position. An above-target is left as-is:
    # the lazy bridge has NO stored tracked level to "pin" to lv, so the result's
    # on-demand ``level`` simply reflects the actual post-gl -- the droplevel-
    # upward drift (the eager bridge pinned lv while the cipher sat at post_gl,
    # so a later _align computed an unreachable upward drop) CANNOT occur.
    if target_gl >= post_gl:
        ct = _drop_to_of_level(ct, target_gl, nsd, op)
    return Cipher._wrap(ct, int(ct.GetNoiseScaleDeg()), False,
                        a._n_logical)  # track REAL nsd (1 after reset); level on demand


def copy_ciph(src):
    if not isinstance(src, Cipher):
        _violation("hard", "copy_ciph",
                   f"expected cipher, got {type(src).__name__}")
    return src.clone()


def zero_ciph():
    # Unfixed-scale zero (matches rt_ref): scale_deg 0 so add_ciph/sub_ciph
    # short-circuit a zero operand (zero + X == X, X - zero == X, zero - X ==
    # -X) and SKIP the real EvalAdd/EvalSub -- real CKKS cannot add ciphertexts
    # at mismatched scales, so the accumulator-init pattern `add(zero_ciph(),
    # term)` (term at scale_deg 2 from mul) would otherwise trip [strict]
    # scale_deg mismatch. The zero-operand short-circuit is the real-CKKS analog
    # of rt_ref's scale_deg==0 waiver. n_logical defaults: an add with a zero
    # operand adopts the other operand's n_logical in the short-circuit.
    return Cipher([], _ctx_input_level(), 0, False)


def free_ciph(a, cnt=0):
    # no-op (Python GC owns the openfhe handles)
    return None


def free_plain(a):
    return None


def free_ciph_poly(a, cnt=0):
    # Mirrors the C Free_ciph_poly(...) fallback name; dead in the non-ant path.
    return None


# ---------------------------------------------------------------------------
# encode / queries
# ---------------------------------------------------------------------------

def _flatten(buf):
    # Flatten a (possibly multi-dim) Python list/tuple buffer to flat scalars.
    # The emitter (E1) emits array constants as NESTED lists (so a subscript
    # yields the sub-list row); a buffer consumed whole is nested and must be
    # flattened before taking the leading `n` (mirrors C `Encode_float(buf, n)`
    # reading `n` flat row-major elements). A flat/row buffer is a no-op. Parity
    # with rt_ref._flatten.
    out = []
    for x in buf:
        if isinstance(x, (list, tuple)):
            out.extend(_flatten(x))
        else:
            out.append(x)
    return out


def _encode(buf, n, scale_deg, level, op):
    """Build an rt_openfhe Plain from a buffer (rt_ref `_encode` parity: bounds
    check the encode level against the IR-mapping ceiling ``ir_capacity`` (the
    chain without the R down-headroom reserve), build the logical slot list). The
    openfhe Plaintext is built lazily at the consuming cipher's level when the
    plain feeds an arithmetic op."""
    n = int(n)
    # Ceiling = the IR-mapping capacity ``_ctx_ir_capacity()`` (= the chain
    # WITHOUT the R down-headroom reserve; == ``GetMultiplicativeDepth()+1`` for
    # 3a), NOT q_part_num (key-switch decomp count 1..3) -- the old ``level >
    # q_part_num`` check wrongly rejected high-level encodes (relu 12 vs q_part
    # 3). The lazy bridge's ``level()`` query is ir_capacity-based, so the bound
    # must be too.
    cap = _ctx_ir_capacity()
    if level < 0 or level > cap:
        _violation("hard", op,
                   f"level {level} out of bounds [0, {cap}] (ir_capacity=IR-mapping chain)")
    if isinstance(buf, (list, tuple)):
        # E1 emits array constants as nested lists; flatten a whole-consumed
        # buffer before taking the leading `n` (subscripted row buffers are
        # already flat, so flattening is a no-op). Parity with rt_ref._encode.
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
    CKKS slots -- so both providers agree until an e2e dcmplx workload exists
    (plan open question). The openfhe Plaintext is built lazily at the
    consuming cipher's level. Body copied verbatim from rt_ref."""
    op = "encode_dcmplx"
    n = int(n)
    # Same IR-mapping ceiling as _encode (``_ctx_ir_capacity()``), not
    # q_part_num and not the lowered ``input_level`` field. See _encode.
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
# rt_ref: a provider-agnostic C binary parser (rt_data_def.h format). The
# green-gate `mulc2df` workload reads the weight this way.
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
    (``_ctx_num_primes() - ct.GetLevel()`` via the ``Cipher.level`` property) --
    NEVER a stored tracked level, so it cannot drift from the real cipher. For a
    Plain it is the stored REMAINING level (a plain has no ciphertext to query)."""
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
# read rt_openfhe's Cipher/Plain `.slots` (which lazily decrypts). validate
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
    real-HE/compiler finding, reported as [strict] (raises in STRICT, recorded
    in lenient, first-mismatch return -- C break parity). Body copied from
    rt_ref (operates on .slots, which here decrypts the real cipher)."""
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
# __all__: 36 functional calls + Cipher/Plain/FakeRtError + CMPLR_TM_START/CMPLR_TM_END = 41 names). A star
# import binds exactly these into the emitted module's namespace; the driver
# glue (_reset/_seed_*/_CTX/_INPUTS/_OUTPUTS/_VIOLATIONS/configure/_ctx_*/...)
# + the openfhe helpers (_of_level/_encrypt_values/_decrypt_slots/...) stay
# module-level with a leading `_` and are NOT exported.
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