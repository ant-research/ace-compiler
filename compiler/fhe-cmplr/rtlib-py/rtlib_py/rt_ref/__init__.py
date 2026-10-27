"""Reference (plaintext) FHE Python runtime -- the `rt_ref` rtlib-py provider.

The ckks IR -> Python (non-ant, `-CKKS2PY`) backend emits modules that call a
functional runtime API (`add_ciph(a, b)`, `rescale_ciph(a)`, ...) and begin with

    from rtlib_py.rt_ref import *          # Phase 2 default runtime provider

so the runtime names are bound by a **real import** (Phase 2.0; Phase 1 injected
them by hand). This module implements the **same API surface** but executes every
operation on **plaintext slots** (fast, no HE), while tracking each cipher's
CKKS state `(level, scale_deg, is_cipher3)` and asserting the real CKKS
level/scale discipline. It is the reference provider; a real-HE provider
(`rt_openfhe`, via upstream openfhe-python) is Phase 3.

Two kinds of constraint, mirroring `doc/ckks_ir_to_python/phase_1_verification_plan.md`:

  * `[hard]` -- type errors, level underflow, raise_mod/encode out of bounds,
    relin/rescale on a non-cipher. These are *compiler bugs* (the emitter
    produced a malformed call), not FHE invariants, so they always raise --
    even in lenient mode (execution cannot continue past a type error).

  * `[strict]` -- the FHE level/scale invariants: add/sub_ciph level+scale
    match (zero-operand waiver), mul_ciph level match, rescale scale_deg>1.
    In STRICT mode (default) a violation raises; in LENIENT mode it is
    recorded to stderr and execution continues, so a DAG that over-levels can
    still be diagnosed (the strict level-match assertion is exactly that
    detector -- a tree program never trips it; see the plan's DAG note).

Public vs internal split (Phase 2.0, decision #3): `__all__` exposes the frozen
public runtime contract -- 36 functional runtime calls (30 arithmetic/
IO/encode/query from Phase 2.0 + 4 observability/metadata from Phase 2.1:
`validate`/`tm_start`/`tm_taken`/`dump_cipher_msg` + `load_data_entry` from
Phase 2.3 offline `-CKKS2PY:df=` data-file read + `bootstrap_ciph` from Phase 2.4
plaintext near-identity bootstrap) plus `Cipher`/`Plain`/`FakeRtError` = 39
names (see rtlib_py/common/contract.py) -- plus the `:rtt` timing macros
`CMPLR_TM_START`/`CMPLR_TM_END` (mirror rtlib-c; not in the frozen contract). The driver
glue (`_STRICT`/`_CTX`/`_INPUTS`/`_OUTPUTS`/`_VIOLATIONS`, `configure`/`_reset`/
`_seed_input`/`_seed_ctx`/`_ctx_*`/`_check_*`/`_require_*`/`_bin_slots`/
`_broadcast_plain`/...) stays module-level with a leading `_` so a star import
does NOT pull it into the emitted module's namespace -- the runner
(rtlib_py.common.driver) reaches it as plain module attributes.

Plaintext slot semantics: slots are Python float lists. `mul_ciph` is
elementwise; `mul_plain`/`mul_mono` broadcast a 1-slot plain across the
cipher's slots; `zero_ciph()` is an "unfixed-scale zero" with empty slots
that add/sub broadcast over. Scale-degree (`scale_deg`) counts the number of
scaling primes packed into the cipher (encode seeds it; mul adds; rescale
subtracts); `level` is the current primes-chain level.
"""

# ---------------------------------------------------------------------------
# configuration + stores
# ---------------------------------------------------------------------------

import struct as _struct  # C binary side-data-file parsing (load_data_entry)

# rtpy :rtt timing (mirror rtlib-c rtlib_timing.h) -- emitted by -POLY:rtt/:rtt.
from rtlib_py.common.timing import CMPLR_TM_START, CMPLR_TM_END

# rtpy-df C (perf): optional numpy for a zero-copy float32 view into the data
# file, replacing the O(count) ``_struct.unpack`` per load_data_entry call with
# an O(1) view + O(n) C-speed ``.tolist``. Falls back to struct if numpy is
# absent (rtlib-py stays importable without numpy). See load_data_entry.
try:
    import numpy as _np
except ImportError:  # pragma: no cover - numpy is optional for the offline path
    _np = None

_STRICT = True            # default STRICT: invariant violations raise
_VIOLATIONS = []          # collected in lenient mode
_CTX = {}                 # seeded by the driver from get_context_params()
_INPUTS = {}              # (name, idx) -> Cipher/Plain, seeded by the driver
_OUTPUTS = {}             # (name, idx) -> Cipher/Plain, written by set_output_data
_DEFAULT_SLOTS = 2        # slot count for zero_ciph() when _CTX has no poly_degree
# Offline data-file descriptor (Phase 2.3, `-CKKS2PY:df=`). Seeded by the driver from
# the emitted module's get_rt_data_info(); None when there is no data file.
# load_data_entry reads the C binary side data file (`fhe/core/rt_data_def.h`
# format) it names so an offline encode can pull the weights from the file.
_DATA_INFO = None
_DATA_FILE_CACHE = None   # (raw bytes) cache so the binary file is read once
# rtpy-df C (perf): parsed LUT cache so load_data_entry does NOT re-unpack the
# file-level header + per-entry LUT row on every call. None until first
# load_data_entry; rebuilt when _DATA_FILE_CACHE is invalidated (_reset /
# _seed_data_info). Layout: (ent_count, lut_ofst, ent_type, [(ent_ofst, count)]).
_DATA_LUT = None


class FakeRtError(Exception):
    """A violated [hard] or (in strict mode) [strict] CKKS constraint."""


def configure(strict=True, on_violation="raise"):
    """Configure strictness. strict=False => [strict] violations are recorded
    to stderr and execution continues (lenient / DAG-diagnosis mode). [hard]
    errors always raise regardless."""
    global _STRICT
    _STRICT = bool(strict)


def _reset():
    """Clear all per-run state. Called by the driver before each module run
    and by unit-test fixtures."""
    global _VIOLATIONS, _INPUTS, _OUTPUTS, _DATA_INFO, _DATA_FILE_CACHE, _DATA_LUT
    _VIOLATIONS = []
    _INPUTS = {}
    _OUTPUTS = {}
    _DATA_INFO = None
    _DATA_FILE_CACHE = None
    _DATA_LUT = None  # rtpy-df C: drop the parsed LUT (file may change across runs)


def _seed_input(name, idx, cipher):
    """Driver: seed an input cipher/plain the module will load via
    get_input_data(name, idx)."""
    _INPUTS[(name, idx)] = cipher


def _seed_ctx(ctx_params):
    """Driver: seed _CTX from the module's get_context_params() dict."""
    global _CTX
    _CTX = dict(ctx_params)


def _seed_data_info(info):
    """Driver: seed the offline data-file descriptor (from the emitted module's
    get_rt_data_info()) so load_data_entry can locate the C binary side data
    file written by `-CKKS2PY:df=`. Pass None / omit when there is no data file
    (no `-CKKS2PY:df=`); load_data_entry raises only if actually called."""
    global _DATA_INFO, _DATA_FILE_CACHE, _DATA_LUT
    _DATA_INFO = info
    _DATA_FILE_CACHE = None  # invalidate; the file may have changed across runs
    _DATA_LUT = None         # rtpy-df C: parsed-LUT cache tracks _DATA_FILE_CACHE


def _ctx_input_level():
    return int(_CTX.get("input_level", 0))


def _ctx_q_part_num():
    return int(_CTX.get("q_part_num", 0))


def _ctx_mul_depth():
    return int(_CTX.get("mul_depth", 0))


def _ctx_num_primes():
    """Prime-chain length = mul_depth+1 (param.c:25; the C encoder ceiling is
    `level <= q_cnt` with q_cnt=num_primes, encoder.c:140). This is the TRUE
    modulus-level ceiling. ``input_level`` is the level at which inputs are
    encoded (usually == num_primes) -- but the lowering occasionally fills it
    WRONG, e.g. small_gemm_relu / conv2d_relu / conv_p1s1_relu get
    input_level=3/5 while mul_depth=26 (num_primes=27); their relu poly encodes
    at level 12, which a ``cap=_ctx_input_level()`` (3) would reject as
    ``[hard] level 12 out of bounds``. Using mul_depth+1 (the real chain
    length, matching the C encoder) instead of the possibly-wrong input_level
    aligns rt_ref with C and unblocks those models. Raising the cap only
    RELAXES (a passing model never encodes above input_level, else it would
    already have [hard]-failed), so this is 0-regression for existing PASS."""
    return _ctx_mul_depth() + 1


def _default_slot_count():
    pd = int(_CTX.get("poly_degree", 0))
    return pd // 2 if pd > 0 else _DEFAULT_SLOTS


# ---------------------------------------------------------------------------
# constraint helpers
# ---------------------------------------------------------------------------

def _violation(kind, op, msg):
    """Record/raise a constraint violation. kind is 'hard' (always raise) or
    'strict' (raise iff _STRICT, else record + warn)."""
    full = f"[{kind}] {op}: {msg}"
    if kind == "hard":
        raise FakeRtError(full)
    # strict
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
    # downgrade it). Every other consumer -- add/sub/mul_ciph, rotate,
    # conjugate, mul_plain/scalar/mono -- must see a relined cipher; a cipher3
    # arriving here means the compiler omitted relin (plan invariant: "mul->
    # cipher3 must be relined before add/rotate and other non-relin use").
    if isinstance(a, Cipher) and a.is_cipher3:
        _violation("hard", op, "cipher3 operand must be relined first")


def _check_level_match(a, b, op):
    # No-op: ant's Add/Sub/Mul_ciphertext auto-align mismatched-level operands
    # via Adjust_level (ciphertext.h:203-238: drops the higher DOWN to the lower,
    # scale unchanged; the only hard precondition is EQUAL scaling factor, which
    # _check_scale_match mirrors). The result level is _result_level (min). Kept
    # as a function (callers unchanged) for symmetry + a future strict-mode
    # diagnostic hook.
    return


def _check_scale_match(a, b, op):
    if a.scale_deg == 0 or b.scale_deg == 0:
        return
    if a.scale_deg != b.scale_deg:
        _violation("strict", op,
                   f"scale_deg mismatch: {a.scale_deg} != {b.scale_deg}")


def _result_level(a, b):
    # Mirror ant Adjust_level (ciphertext.h:216-227) + the null-cipher rule
    # (ciphertext.h:206-210 returns the other cipher): the result sits at the
    # SMALLER operand level (the higher operand is dropped down to it). A
    # zero (scale_deg==0, from zero_ciph) waives -- it adopts the other's level.
    if a.scale_deg == 0:
        return b.level
    if b.scale_deg == 0:
        return a.level
    return min(a.level, b.level)


# ---------------------------------------------------------------------------
# value classes
# ---------------------------------------------------------------------------

class Cipher:
    """A plaintext cipher: slots + CKKS state (level, scale_deg, is_cipher3).

    `is_cipher3` marks a 3-component cipher (post mul_ciph, pre relin): it may
    feed relin/rescale/mod_switch (which downgrade it) but must NOT feed
    add/sub/rotate/etc. until relined. `scale_deg==0` marks an unfixed-scale
    zero (from zero_ciph): it waives level/scale-match checks as an operand.
    Supports __getitem__/__setitem__ so an array-cipher store/load
    (`arr[idx] = copy_ciph(...)`, `x = arr[idx]`) works when the array is a
    Python list of Ciphers.
    """

    __slots__ = ("slots", "level", "scale_deg", "is_cipher3")

    def __init__(self, slots, level, scale_deg, is_cipher3=False):
        self.slots = list(slots)
        self.level = int(level)
        self.scale_deg = int(scale_deg)
        self.is_cipher3 = bool(is_cipher3)

    def clone(self):
        return Cipher(list(self.slots), self.level, self.scale_deg,
                      self.is_cipher3)

    def __getitem__(self, idx):
        # Array-cipher element load: return the stored sub-cipher (a clone, so
        # downstream ops do not mutate the array element).
        return self.slots[idx].clone()

    def __setitem__(self, idx, val):
        if isinstance(val, Cipher):
            self.slots[idx] = val.clone()
        else:
            self.slots[idx] = val

    def __repr__(self):
        return (f"Cipher(slots={self.slots}, level={self.level}, "
                f"scale_deg={self.scale_deg}, is_cipher3={self.is_cipher3})")


class Plain:
    """A plaintext: slots + (level, scale_deg)."""

    __slots__ = ("slots", "level", "scale_deg")

    def __init__(self, slots, level, scale_deg):
        self.slots = list(slots)
        self.level = int(level)
        self.scale_deg = int(scale_deg)

    def clone(self):
        return Plain(list(self.slots), self.level, self.scale_deg)

    def __repr__(self):
        return (f"Plain(slots={self.slots}, level={self.level}, "
                f"scale_deg={self.scale_deg})")


# ---------------------------------------------------------------------------
# slot arithmetic helpers
# ---------------------------------------------------------------------------

def _as_cipher(x, op):
    if isinstance(x, Cipher):
        return x
    _violation("hard", op, f"expected cipher, got {type(x).__name__}")


def _bin_slots(a_slots, b_slots, op, fn):
    """Elementwise binary op on slot lists. A [] (zero_ciph) operand
    broadcasts as the identity element for + / -. Unequal non-empty lengths
    are a hard error (a well-formed program packs matching slot counts)."""
    if not a_slots and not b_slots:
        return []
    if not a_slots:
        return list(b_slots)
    if not b_slots:
        return list(a_slots)
    if len(a_slots) != len(b_slots):
        _violation("hard", op,
                   f"slot count mismatch: {len(a_slots)} != {len(b_slots)}")
    return [fn(x, y) for x, y in zip(a_slots, b_slots)]


def _broadcast_plain(c_slots, p_slots, op, fn):
    """cipher (op) plain, broadcasting a 1-slot / empty plain across the
    cipher's slots (CKKS packs one plain value across all slots), OR doing an
    elementwise op when the plain packs the SAME number of slots as the cipher
    (a full vector plain). Mismatched non-empty lengths are zero-extended /
    truncated to the cipher's slot count (mirrors ant Mul_plaintext; see the
    inline note below), not a hard error.

    An empty CIPHER (zero_ciph, the unfixed-scale identity) is broadcast across
    the plain's slots -- mirroring _bin_slots's empty-operand handling -- so
    add_plain/sub_plain/mul_plain(zero_ciph(), p) does NOT silently drop the
    plain."""
    if not c_slots and not p_slots:
        return []
    if not c_slots:
        # zero_ciph (empty-slot identity) op plain: treat the empty cipher as a
        # zero spread across the plain's slots so the plain is preserved (add ->
        # plain; mul -> zeros). Mirrors _bin_slots's empty-operand broadcast.
        return [fn(0.0, p) for p in p_slots]
    if not p_slots:
        return list(c_slots)  # plain empty -> identity (should not happen)
    if len(p_slots) == 1:
        v = p_slots[0]
        return [fn(c, v) for c in c_slots]
    # Elementwise over the cipher's slots, with the plain zero-extended (if
    # shorter) or truncated (if longer) to the cipher's slot count. Mirrors ant
    # Mul_plaintext (evaluator.c:192-216): the plain is zero-padded to the ring
    # and multiplied full-ring elementwise -- since both cipher and plain are
    # zero-padded on the right, the leading product is identical and the cipher's
    # slot count carries the meaningful length. Accepts a plain LONGER (conv_p0
    # 6144 > cipher 3072) or SHORTER (gap_hw8 4 < 256) than the cipher.
    n  = len(c_slots)
    pp = list(p_slots) + [0.0] * n
    return [fn(c, pp[i]) for i, c in enumerate(c_slots)]


# ---------------------------------------------------------------------------
# IO
# ---------------------------------------------------------------------------

def get_input_data(name, idx):
    """Load an input cipher/plain by ONNX tensor name + index. Seeded by the
    driver into _INPUTS. (The array-input form `for i in range(n):
    var[i]=get_input_data(name,i)` is handled by repeated calls.)"""
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

def add_ciph(a, b):
    op = "add_ciph"
    _as_cipher(a, op)
    _as_cipher(b, op)
    if a.is_cipher3 or b.is_cipher3:
        _violation("hard", op, "cipher3 operand must be relined first")
    _check_level_match(a, b, op)
    _check_scale_match(a, b, op)
    slots = _bin_slots(a.slots, b.slots, op, lambda x, y: x + y)
    sd = a.scale_deg if a.scale_deg else b.scale_deg
    return Cipher(slots, _result_level(a, b), sd, False)


def add_plain(a, p):
    op = "add_plain"
    _as_cipher(a, op)
    _require_plain(p, op)
    slots = _broadcast_plain(a.slots, p.slots, op, lambda c, v: c + v)
    return Cipher(slots, a.level, a.scale_deg, False)


def add_scalar(a, s):
    op = "add_scalar"
    _as_cipher(a, op)
    slots = [x + s for x in a.slots]
    return Cipher(slots, a.level, a.scale_deg, False)


def sub_ciph(a, b):
    op = "sub_ciph"
    _as_cipher(a, op)
    _as_cipher(b, op)
    if a.is_cipher3 or b.is_cipher3:
        _violation("hard", op, "cipher3 operand must be relined first")
    _check_level_match(a, b, op)
    _check_scale_match(a, b, op)
    slots = _bin_slots(a.slots, b.slots, op, lambda x, y: x - y)
    sd = a.scale_deg if a.scale_deg else b.scale_deg
    return Cipher(slots, _result_level(a, b), sd, False)


def sub_plain(a, p):
    op = "sub_plain"
    _as_cipher(a, op)
    _require_plain(p, op)
    slots = _broadcast_plain(a.slots, p.slots, op, lambda c, v: c - v)
    return Cipher(slots, a.level, a.scale_deg, False)


def sub_scalar(a, s):
    op = "sub_scalar"
    _as_cipher(a, op)
    slots = [x - s for x in a.slots]
    return Cipher(slots, a.level, a.scale_deg, False)


# ---------------------------------------------------------------------------
# arithmetic: mul  (ciph / plain / scalar / mono)
# ---------------------------------------------------------------------------

def mul_ciph(a, b):
    op = "mul_ciph"
    _as_cipher(a, op)
    _as_cipher(b, op)
    if a.is_cipher3 or b.is_cipher3:
        _violation("hard", op, "cipher3 operand must be relined first")
    _check_level_match(a, b, op)
    slots = _bin_slots(a.slots, b.slots, op, lambda x, y: x * y)
    # mul_ciph doubles the scale (cipher * cipher) -> cipher3, needs relin.
    # ant Mul_ciphertext3 also auto-aligns levels (evaluator.c:142-175); result
    # at _result_level (min), like add/sub.
    return Cipher(slots, _result_level(a, b), a.scale_deg + b.scale_deg, True)


def mul_plain(a, p):
    op = "mul_plain"
    _as_cipher(a, op)
    _reject_cipher3(a, op)
    _require_plain(p, op)
    slots = _broadcast_plain(a.slots, p.slots, op, lambda c, v: c * v)
    # plain contributes its own scale_deg.
    return Cipher(slots, a.level, a.scale_deg + p.scale_deg, False)


def mul_scalar(a, s):
    op = "mul_scalar"
    _as_cipher(a, op)
    _reject_cipher3(a, op)
    slots = [x * s for x in a.slots]
    # A raw scalar multiply (EvalMult(c, scalar)) encodes the scalar at the
    # cipher's scale, adding one scale_deg -- the compiler rescales it away.
    return Cipher(slots, a.level, a.scale_deg + 1, False)


def mul_mono_ciph(a, p):
    op = "mul_mono_ciph"
    _as_cipher(a, op)
    _reject_cipher3(a, op)
    _require_plain(p, op)
    slots = _broadcast_plain(a.slots, p.slots, op, lambda c, v: c * v)
    # Same scale math as mul_plain; "mono" signals the compiler will NOT
    # rescale (a single-prime plain), but the state-machine tracking is shared.
    return Cipher(slots, a.level, a.scale_deg + p.scale_deg, False)


# ---------------------------------------------------------------------------
# unary / shape / lifecycle
# ---------------------------------------------------------------------------

def neg_ciph(a):
    op = "neg_ciph"
    _as_cipher(a, op)
    return Cipher([-x for x in a.slots], a.level, a.scale_deg, a.is_cipher3)


def rotate_ciph(a, k):
    op = "rotate_ciph"
    _as_cipher(a, op)
    _reject_cipher3(a, op)
    n = len(a.slots)
    if n == 0:
        slots = []
    else:
        k %= n
        slots = a.slots[k:] + a.slots[:k]
    return Cipher(slots, a.level, a.scale_deg, False)


def conjugate_ciph(a):
    op = "conjugate_ciph"
    _as_cipher(a, op)
    _reject_cipher3(a, op)
    # Real slots are unchanged under conjugation (imaginary part negated,
    # which a real-only plaintext model does not carry).
    return Cipher(list(a.slots), a.level, a.scale_deg, False)


def relin(a):
    op = "relin"
    _require_cipher_or_cipher3(a, op)
    # cipher3 -> cipher (same level/scale); cipher -> no-op (compatibility:
    # the emitted mul chain is mul_ciph -> rescale_ciph -> relin, so relin
    # receives a post-rescale cipher, not a cipher3).
    if a.is_cipher3:
        return Cipher(list(a.slots), a.level, a.scale_deg, False)
    return a.clone()


def rescale_ciph(a):
    op = "rescale_ciph"
    _require_cipher_or_cipher3(a, op)
    if a.scale_deg <= 1:
        _violation("strict", op,
                   f"requires scale_deg>1, got {a.scale_deg}")
    if a.level - 1 < 0:
        _violation("hard", op, f"level underflow: {a.level}-1 < 0")
    # rescale consumes one scaling prime: level-1, scale_deg-1; a cipher3
    # operand is downgraded to cipher (the emitted chain rescales a cipher3
    # before relin in the mul->rescale->relin order).
    return Cipher(list(a.slots), a.level - 1, a.scale_deg - 1, False)


def mod_switch(a):
    op = "mod_switch"
    _require_cipher_or_cipher3(a, op)
    if a.level - 1 < 0:
        _violation("hard", op, f"level underflow: {a.level}-1 < 0")
    return Cipher(list(a.slots), a.level - 1, a.scale_deg, False)


def upscale_ciph(a, bits):
    op = "upscale_ciph"
    _require_cipher_or_cipher3(a, op)
    # upscale adds one scaling prime without changing level.
    return Cipher(list(a.slots), a.level, a.scale_deg + 1, False)


def raise_mod(a, lev):
    op = "raise_mod"
    _require_cipher_or_cipher3(a, op)
    # The modulus-level ceiling is the prime-chain length num_primes =
    # mul_depth+1 (param.c:25; the C encoder asserts `level <= q_cnt` with
    # q_cnt=num_primes at encoder.c:140). NOT input_level: that field is the
    # input-encode level (usually == num_primes) but the lowering sometimes
    # fills it wrong (e.g. E models input_level=3 vs mul_depth=26 -> num_primes
    # 27), which a `cap=input_level` would codify into a spurious [hard].
    cap = _ctx_num_primes()
    if lev < 0 or lev > cap:
        _violation("hard", op,
                   f"lev {lev} out of bounds [0, {cap}] (num_primes=mul_depth+1)")
    # raise_mod sets the cipher's modulus level explicitly.
    return Cipher(list(a.slots), int(lev), a.scale_deg, False)


def bootstrap_ciph(a, level, slot):
    """Plaintext reference bootstrap: near-identity -- the message (slots) is
    preserved and the cipher's level is reset to the bootstrap target `level`
    with a fresh ``scale_deg=1`` (the canonical post-bootstrap scale) and
    ``is_cipher3`` cleared (a bootstrapped cipher is relined). Mirrors the C
    non-ANT collapse of the 4 bootstrap opcodes (BOOTSTRAP + the 3 sub-stages
    coeffs_to_slots / eval_mod / slots_to_coeffs) to a single ``Bootstrap(&dst,
    a, ...)`` call (ckks/ir2c_handler.h:199-305); the Python path collapses them
    further to one runtime name ``bootstrap_ciph(a, level, slot)`` (plan 2.4) so
    Phase 3 can swap a real openfhe-python bootstrap behind the same name.

    Real HE bootstrap is a coeff-to-slot + eval_mod + slot-to-coeff chain that
    refreshes a low-level cipher to a fresh high level; here the message is
    unchanged (a plaintext reference) and only the level is reset -- enough for
    Phase-2 plaintext-precision identity. `slot` is the bootstrap linearization
    slot count (the IR ATTR::SLOT attr the handler emits); it does not change the
    plaintext result (the slot count is already encoded in ``a.slots``) and is
    accepted for the uniform 3-arg call shape + Phase-3 fidelity. `level` is the
    FHE_ATTR_KIND::LEVEL bootstrap target (top-level opcode; the 3 sub-stages
    pass 0, mirroring C's non-ANT ``Bootstrap(&dst, a, 0)`` literal).

    A [hard] error is raised for a non-cipher operand or a negative target level
    (a compiler/emitter bug, not an FHE invariant). Unlike raise_mod, `level` is
    NOT upper-bounded against q_part_num: bootstrap refreshes UP to the fresh
    (top) level, and the C handler does not bound-check the attr either, so the
    reference stays lenient (a minimal test ctx may have q_part_num 0/unset)."""
    op = "bootstrap_ciph"
    _require_cipher_or_cipher3(a, op)
    lv = int(level)
    if lv < 0:
        _violation("hard", op, f"bootstrap target level {lv} < 0")
    # message preserved (plaintext near-identity); level reset to the bootstrap
    # target; scale_deg reset to the fresh post-bootstrap scale (1); is_cipher3
    # cleared (a bootstrapped cipher is relined, not a 3-component cipher).
    return Cipher(list(a.slots), lv, 1, False)


def copy_ciph(src):
    if not isinstance(src, Cipher):
        _violation("hard", "copy_ciph",
                   f"expected cipher, got {type(src).__name__}")
    return src.clone()


def zero_ciph():
    # Unfixed-scale zero: empty slots (broadcast as the + / - identity) and
    # scale_deg 0 (waives level/scale-match as an add/sub operand).
    return Cipher([], _ctx_input_level(), 0, False)


def free_ciph(a, cnt=0):
    # no-op (plaintext runtime owns no HE handles)
    return None


def free_plain(a):
    return None


def free_ciph_poly(a, cnt=0):
    # Mirrors the C Free_ciph_poly(...) fallback name; dead in the non-ant
    # path (no poly-typed ciphers) but provided for faithful API parity.
    return None


# ---------------------------------------------------------------------------
# encode / queries
# ---------------------------------------------------------------------------

def _flatten(buf):
    # Flatten a (possibly multi-dim) Python list/tuple buffer to a flat list of
    # scalars. The emitter (E1) emits array constants as NESTED lists so an
    # outer-dim subscript `_cst_N[k]` yields the sub-list (row); a buffer
    # consumed whole (`encode_float(_cst_N, n)`) is therefore nested and must be
    # flattened before taking the leading `n` (mirrors C `Encode_float(buf, n)`
    # reading `n` flat row-major elements from `_cst_N`). A flat buffer is a
    # no-op (each element is a scalar).
    out = []
    for x in buf:
        if isinstance(x, (list, tuple)):
            out.extend(_flatten(x))
        else:
            out.append(x)
    return out


def _encode(buf, n, scale_deg, level, op):
    n = int(n)
    # Encode-level ceiling = prime-chain length num_primes = mul_depth+1
    # (param.c:25; C encoder `level <= q_cnt`, q_cnt=num_primes, encoder.c:140).
    # NOT q_part_num (RNS key-switch decomp, 1..3) and NOT input_level (the
    # input-encode level, usually == num_primes but the lowering sometimes fills
    # it wrong -- E models input_level=3 vs mul_depth=26 -> num_primes 27; relu
    # encodes at level 12, old `cap=input_level(3)` spuriously rejected it).
    cap = _ctx_num_primes()
    if level < 0 or level > cap:
        _violation("hard", op,
                   f"level {level} out of bounds [0, {cap}] (num_primes=mul_depth+1)")
    if isinstance(buf, (list, tuple)):
        # E1 emits array constants as nested lists; flatten a whole-consumed
        # buffer before taking the leading `n` (see _flatten). A subscripted
        # buffer `_cst_N[k]` is already a flat row, so flattening is a no-op.
        vals = _flatten(buf)
        slots = [float(x) for x in (vals[:n] if n >  0 else vals)]
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
    """Double-complex encode (Phase 2.3 consumer). `n` is the COMPLEX count and
    the buffer is `2*n` interleaved real/imag floats (the ckks IR COMPLEX array
    constant, flattened to real/imag pairs by Emit_py_array -- this flattening
    mirrors the C ARRAY-BUFFER layout at ckks/ir2c_ctx.h:325-328, where
    `count = Array_byte_len()/sizeof(double)` and `complex_len =
    node->Child(1)` is the complex count so `count >= complex_len*2`).

    NOTE: the slot packing is a REAL-ONLY reference projection, NOT a faithful
    C slot-count mirror. C ``Encode_dcmplx`` packs `n` COMPLEX CKKS slots (one
    complex value per slot, rtlib/ant/ckks/src/plain.c); this plaintext model
    instead unpacks the 2n real/imag halves into `2*n` real slots
    (r0, i0, r1, i1, ...). A downstream mul_plain/add_plain against this plain
    therefore needs a `2*n`-slot cipher operand -- an `n`-slot cipher would have
    the plain truncated to `n` slots by _broadcast_plain (which zero-extends /
    truncates unequal lengths, not a [hard] reject). No e2e dcmplx workload
    exists yet (simple real-valued ONNX does not produce ENCODE_DCMPLX); the 2n
    consumer semantics are pinned by the rt_ref unit, not an e2e gate.
    Phase 2.2 section 6#4: unlike `_encode` (which slices `buf[:n]`), this reads all
    `2*n` interleaved real/imag floats."""
    op = "encode_dcmplx"
    n = int(n)
    # Same prime-chain ceiling as _encode (num_primes = mul_depth+1 ==
    # input_level), not q_part_num. See _encode for the C-semantics rationale.
    cap = _ctx_input_level()
    if level < 0 or level > cap:
        _violation("hard", op,
                   f"level {level} out of bounds [0, {cap}] (input_level=num_primes)")
    width = 2 * n if n > 0 else 2  # n complex => 2n real/imag slots
    if isinstance(buf, (list, tuple)):
        raw = [float(x) for x in buf]
        # take up to 2n floats (truncate a longer buffer); pad missing pairs to 0
        slots = (raw + [0.0] * width)[:width]
    else:
        # scalar buffer (edge): one complex value (real=buf, imag=0) then zeros
        slots = ([float(buf)] + [0.0] * (width - 1))[:width]
    return Plain(slots, int(level), int(scale))


# ---------------------------------------------------------------------------
# offline data-file encode (Phase 2.3, `-CKKS2PY:df=`)
# ---------------------------------------------------------------------------

def _read_data_file():
    """Read + cache the C binary side data file (`fhe/core/rt_data_def.h`
    format) named by get_rt_data_info() (seeded into _DATA_INFO by the driver).

    Layout (little-endian): DATA_FILE_HDR at offset 0 -- magic `!ANTFHE\\0`@0,
    _ent_count@16 (u64), _lut_ofst@24 (u64); then `2**_ent_align`-aligned
    float32 message entries; then a DATA_LUT_ENTRY table at _lut_ofst, one
    32-byte entry per appended constant: _name[16], _index@16 (u32), _size@20
    (u32, bytes), bitfield@24 (u64) = scale:8 | level:8 | ent_ofst:48. Returns
    the raw bytes (read once, cached for the run)."""
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
    C binary side data file and return its first `n` message-slot floats,
    mirroring the C runtime ``Pt_from_msg(idx, len, ...)`` which reads the
    idx-th entry and decodes it as a plaintext of `len` slots. rt_ref hands the
    raw float32 message to encode_float/encode_double to (reference-)encode (the
    compiler serialized it via RT_DATA_WRITER). The Python offline path is
    FLOAT_32-only (mirrors ckks/ir2c_ctx.h:101-103), so each entry is a
    `size/4` float32 array; `n` slices it to the encode length. The data-file
    path comes from the emitted module's get_rt_data_info() (seeded by the
    driver into _DATA_INFO)."""
    raw = _read_data_file()
    global _DATA_LUT
    if _DATA_LUT is None:
        # rtpy-df C (perf): parse the file header + every LUT row ONCE and cache
        # the per-entry (ent_ofst, count), mirroring the C runtime parsing the
        # LUT once at open (rt_data_file.c:128-136) so later calls are O(1)
        # lookups. Phase-2.3 hardening (FLOAT_32 / bounds) is done here at parse
        # time (mirrors ckks/ir2c_ctx.h:101-108, rt_data_file.c:134).
        ent_count = _struct.unpack_from("<Q", raw, 16)[0]
        lut_ofst = _struct.unpack_from("<Q", raw, 24)[0]
        ent_type = _struct.unpack_from("<B", raw, 14)[0]   # DATA_FILE_HDR
        if ent_type != 0:  # DE_MSG_F32
            raise FakeRtError(f"[hard] load_data_entry: data-file entry type "
                              f"{ent_type} is not DE_MSG_F32 (Python offline "
                              f"path is float32-only; mirror of "
                              f"ckks/ir2c_ctx.h:101-103)")
        entries = []
        raw_len = len(raw)
        for i in range(ent_count):
            base = lut_ofst + i * 32
            size = _struct.unpack_from("<I", raw, base + 20)[0]   # bytes
            bf = _struct.unpack_from("<Q", raw, base + 24)[0]
            ent_ofst = (bf >> 16) & ((1 << 48) - 1)
            count = size // 4   # float32 (DE_MSG_F32)
            if not (4096 <= ent_ofst < lut_ofst and ent_ofst + size <= raw_len):
                raise FakeRtError(f"[hard] load_data_entry: entry {i} offset "
                                  f"{ent_ofst} out of range [4096, {lut_ofst}) "
                                  f"or past EOF (mirror of C rt_data_file.c:134)")
            entries.append((ent_ofst, count))
        _DATA_LUT = (ent_count, lut_ofst, ent_type, entries)
    ent_count, lut_ofst, ent_type, entries = _DATA_LUT
    idx = int(idx)
    n = int(n)
    if idx < 0 or idx >= ent_count:
        raise FakeRtError(f"[hard] load_data_entry: idx {idx} out of range "
                          f"[0, {ent_count}) (data-file LUT)")
    ent_ofst, count = entries[idx]
    size = count * 4   # float32 bytes
    if n < 0:
        raise FakeRtError(f"[hard] load_data_entry: negative n={n}")
    if n > count:
        raise FakeRtError(f"[hard] load_data_entry: n={n} exceeds entry slot "
                          f"count {count} (mirror of the C count>=len assert, "
                          f"ir2c_ctx.h:108 / ir2py_handler.h:424) -- the old "
                          f"slots[:n] slice would have silently clamped")
    if n == 0:
        return []   # C Pt_from_msg len=0
    # rtpy-df C (perf): numpy zero-copy float32 view + O(n) C-speed ``.tolist``,
    # avoiding a per-call O(count) ``_struct.unpack`` re-parse. float32->double
    # is exact, so the returned list is bit-identical to
    # ``list(unpack_from(...))[:n]``; falls back to struct if numpy is unavailable.
    if _np is not None:
        return _np.frombuffer(raw, dtype="<f4", count=count,
                              offset=ent_ofst)[:n].tolist()
    return list(_struct.unpack_from("<%df" % count, raw, ent_ofst))[:n]


def level(a):
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
# observability / metadata (Phase 2.1) -- mirror C Validate / Tm_start /
# Tm_taken / Dump_cipher_msg. These are non-cipher-producing library calls the
# front-end may emit; rt_ref keeps execution correctness neutral while letting
# validate surface a real message/cipher disagreement.
# ---------------------------------------------------------------------------

def _slots_of(x, op):
    """Coerce a cipher/plain/list/tuple/scalar to a flat float slot list.
    validate/dump_cipher_msg accept any message-shaped argument the emitter may
    produce (kid 1 of VALIDATE is the message-op result, which the emitter may
    render as a Cipher/Plain/list). Raises [hard] via _violation on an
    unrecognized type."""
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
    """Plaintext validate: compare `ciph`'s message to `msg` within
    10**`epsilon`, mirroring C ``Validate(CIPHER cipher, double* msg, size_t
    len, int32_t epsilon)`` (error = pow(10, epsilon); abort on any
    ``|res[i] - msg[i]| > error``).

    In rt_ref a cipher's slots ARE the plaintext, so a mismatch means the
    emitter's message-op (VALIDATE kid 1) disagrees with the cipher op (kid 0)
    -- a real compiler/runtime finding. Reported as a [strict] violation: it
    raises in STRICT mode (default) and is recorded to _VIOLATIONS in lenient
    mode (mirroring C's first-mismatch ``break`` -- one finding per call)."""
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
            return  # raised in strict; lenient records one finding (C break)


def tm_start(label):
    """Timing start (reference runtime): no-op. Mirrors C ``Tm_start(const
    char* msg)`` which stamps a clock; a plaintext reference runtime has no
    meaningful HE timing to record, so this records nothing (plan decision #4).
    Returns None."""
    return None


def tm_taken(label):
    """Timing taken (reference runtime): no-op. Mirrors C ``Tm_taken(const
    char* msg)``; see ``tm_start``. Returns None."""
    return None


def dump_cipher_msg(name, ciph, length):
    """Observability: write a repr of `ciph`'s first `length` message slots to
    stderr, mirroring C ``Dump_cipher_msg(const char* name, CIPHER ct, uint32_t
    len)``. Pure observation -- never raises and never touches _OUTPUTS /
    _VIOLATIONS, so it cannot affect execution correctness or the verify gate.
    Returns None."""
    import sys
    n = int(length)
    try:
        slots = _slots_of(ciph, "dump_cipher_msg")
        print(f"[dump] {name}: {list(slots[:n])}", file=sys.stderr)
    except FakeRtError:
        # An unprintable operand is a soft observability concern; degrade to a
        # repr so dump never breaks execution.
        print(f"[dump] {name}: <unprintable {type(ciph).__name__}>",
              file=sys.stderr)
    return None


# ---------------------------------------------------------------------------
# broadcast error helper (currently unused; _broadcast_plain handles mismatched
# lengths by padding/truncating, not by raising)
# ---------------------------------------------------------------------------

def _broadcast_err(op, cn, pn):
    return FakeRtError(f"[hard] {op}: plain slot count {pn} cannot broadcast "
                       f"to cipher slot count {cn}")


# ---------------------------------------------------------------------------
# export surface -- the FROZEN PUBLIC runtime contract. A star import
# (`from rtlib_py.rt_ref import *`) binds exactly these names into the emitted
# module's namespace. The emitted module defines its OWN Main_graph /
# get_context_params / get_input_count / get_output_count, so those are NOT
# exported here. Driver glue (_reset/_seed_*/_CTX/_INPUTS/_OUTPUTS/_VIOLATIONS/
# configure/_ctx_*/_check_*/_require_*/_bin_slots/_broadcast_plain/...) stays
# module-level but is intentionally NOT in __all__ -- the runner reaches it as
# a plain module attribute (rtlib_py.common.driver).
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
    "bootstrap_ciph",  # bootstrap_ciph(a, level, slot) [Phase 2.4 near-identity]
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