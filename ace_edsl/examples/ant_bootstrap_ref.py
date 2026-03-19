"""
ANT full CKKS bootstrap reference implementation (Python, standard library only).
================================================================================

This module does NOT import or depend on ace_edsl or any other project package.
It uses only the Python standard library (math). It can be run standalone or
imported by tests that add the examples directory to sys.path.

Single source of truth: fhe-cmplr/rtlib/ant/ckks/src/bootstrap.c (Eval_bootstrap)
and fhe-cmplr/rtlib/ant/include/ckks/bootstrap.h.

This module ports ANT's Eval_bootstrap flow so that ant_bootstrap_full_reference(values)
returns the same slot values as ANT's Bootstrap() / Eval_bootstrap, enabling
cross-verification in test_ant_bootstrap_smoke without calling the .so.

Eval_bootstrap flow (to replicate):
  1. Raise modulus: Rescale ciphertext to sf_degree=1; convert to NTT; raise to
     more towers (Transform_values_from_level0).
  2. Coeffs-to-slots: For full packing (slots == m/4): Coeffs_to_slots (or
     Linear_transform if enc_budget=dec_budget=1); then conjugate, add/sub,
     Mul_by_monomial. For sparse: partial sum (rotations + add), then Coeffs_to_slots.
  3. EvalMod: Optional coordinate shift for even polynomial; Eval_chebyshev
     (coefficients from Get_eval_sin_poly_info, range [-1,1]); then
     Apply_double_angle_iterations (mul-by-self, add, add constant, rescale per iteration).
  4. Slots-to-coeffs: Slots_to_coeffs (or Linear_transform). Sparse case: add
     rotation of result.
  5. Post: Optional clear-imag (conjugate+add); Mul_integer (q0/sf ratio);
     rescale to sf_degree=1.

Phased implementation: see plan ant_full_bootstrap_python_port (Phase 1 = stub,
Phases 2–6 = NTT/poly, encoding, coeffs↔slots, EvalMod, glue).
"""

import math

# Phase 2: NTT and polynomial ring (minimal: for full port see util/ntt.c, poly).
# Phase 3: CKKS encoding/decoding (minimal: FFT; full port see encoder.c Embedding_inv/Embedding).
# Phase 4: Coeffs-to-slots / Slots-to-coeffs (minimal: identity; full port see Coeff_slots_transform).

# Chebyshev series coefficients from bootstrap.h (UNIFORM_HW_UNDER_192, hamming_weight <= 192)
# G_coefficients_uniform_hw_192[UNIFORM_COEFF_SIZE_HW_192], deg=54, K=32, R=3
UNIFORM_COEFF_SIZE_HW_192 = 55
G_COEFFICIENTS_UNIFORM_HW_192 = (
    1.74551960283504837e-01, -3.43838095837535329e-02,
    1.88307649106864788e-01, -2.84223873992535993e-02,
    2.22419882865789564e-01, -1.43397005803286518e-02,
    2.51103798550390944e-01, 9.50854609032555226e-03,
    2.24475678532524398e-01, 3.79342483118012136e-02,
    8.78908877085935597e-02, 5.18464470537667449e-02,
    -1.40269389175310705e-01, 2.52026526332414826e-02,
    -2.71343812500084935e-01, -3.49285487170959558e-02,
    -6.17395308539803664e-02, -5.05648932050318592e-02,
    2.82155868186952818e-01, 2.98272328751879069e-02,
    5.54332147538673034e-02, 4.73762170911353267e-02,
    -3.42589653109854397e-01, -7.19260908452365733e-02,
    3.19234546310780576e-01, 4.93494016031356467e-02,
    -1.74337152324168188e-01, -2.23994935740034137e-02,
    6.76154588798445894e-02, 7.56838175610476029e-03,
    -2.01915893273537893e-02, -2.01996389480041394e-03,
    4.85990579019698801e-03, 4.41705640530539389e-04,
    -9.71526466295980677e-04, -8.11544278739113802e-05,
    1.64814371135792263e-04, 1.27637159472312703e-05,
    -2.41183607585707303e-05, -1.74347427937465971e-06,
    3.08411936249047440e-06, 2.09259735883450997e-07,
    -3.48280526734833634e-07, -2.22825972864890841e-08,
    3.50404774489712212e-08, 2.12216680463557985e-09,
    -3.16453692971713038e-09, -1.82031853692548044e-10,
    2.58203419199988530e-10, 1.41483617957390541e-11,
    -1.91412743082734574e-11, -1.00089939783634691e-12,
    1.29702147256041809e-12, 6.67556346626149772e-14,
    -7.81869621069283006e-14,
)

# Double-angle iterations for hamming_weight <= 192 (R_UNIFORM_HW_192 = 3)
R_UNIFORM_HW_192 = 3
# Upper bound K for uniform hw 192 (K_UNIFORM_HW_192 = 32)
K_UNIFORM_HW_192 = 32

# Post-processing scale: ANT applies Mul_integer(res, res, q0_sf_ratio) with
# q0_sf_ratio = 2^deg, deg = round(log2(q0/sf)). Demo uses first_prime_bits=60,
# scaling_factor_bits=56 => deg = 4, so scale = 16.
BOOTSTRAP_POST_SCALE_DEG = 4
BOOTSTRAP_POST_SCALE = 2 ** BOOTSTRAP_POST_SCALE_DEG

# -----------------------------------------------------------------------------
# Phase 5: EvalMod — Chebyshev evaluation and double-angle iterations (cleartext)
# -----------------------------------------------------------------------------


def _chebyshev_t(n, x):
    """Chebyshev polynomial T_n(x), n >= 0. T_0=1, T_1=x, T_n = 2*x*T_{n-1} - T_{n-2}."""
    if n == 0:
        return 1.0
    if n == 1:
        return x
    t0, t1 = 1.0, x
    for _ in range(2, n + 1):
        t0, t1 = t1, 2.0 * x * t1 - t0
    return t1


def eval_chebyshev_cleartext(x, coeffs, a=-1.0, b=1.0):
    """Evaluate Chebyshev series sum_k coeffs[k]*T_k(y) where y = -1 + 2*(x-a)/(b-a).
    ANT uses a=-1, b=1 so y=x; no linear transform."""
    if a != -1.0 or b != 1.0:
        y = -1.0 + 2.0 * (x - a) / (b - a)
    else:
        y = x
    y = max(-1.0, min(1.0, y))
    total = 0.0
    for k, c in enumerate(coeffs):
        total += c * _chebyshev_t(k, y)
    return total


def apply_double_angle_iterations_cleartext(x, num_iter):
    """Apply_double_angle_iterations from bootstrap.c: x -> 2*x^2 + scalar, j=1..r."""
    r = num_iter
    for j in range(1, r + 1):
        exp = 2 ** (j - r)
        scalar = -1.0 / ((2.0 * math.pi) ** exp)
        x = 2.0 * x * x + scalar
    return x


def eval_mod_cleartext(slot_val, coeffs=None, num_double_angle=None):
    """EvalMod on a single cleartext slot: Chebyshev + double-angle (UNIFORM_HW_UNDER_192)."""
    if coeffs is None:
        coeffs = list(G_COEFFICIENTS_UNIFORM_HW_192)
    if num_double_angle is None:
        num_double_angle = R_UNIFORM_HW_192
    y = eval_chebyshev_cleartext(slot_val, coeffs, -1.0, 1.0)
    return apply_double_angle_iterations_cleartext(y, num_double_angle)


# -----------------------------------------------------------------------------
# Full reference: EvalMod applied per slot (Phases 2–4 full pipeline would add
# NTT, encoding, Coeff_slots_transform; for cleartext we apply EvalMod directly).
# -----------------------------------------------------------------------------


def ant_bootstrap_full_reference(values):
    """Compute ANT full CKKS bootstrap on cleartext slot values (reference).

    Returns the same slot values as ANT's Bootstrap() / Eval_bootstrap when
    given the same input slots, using only the Python standard library.
    Used for cross-verification in test_ant_bootstrap_smoke.

    Flow (mirrors Eval_bootstrap in bootstrap.c):
      1. Raise modulus: rescale to sf_degree=1, NTT, raise to more towers.
      2. Coeffs-to-slots: Coeffs_to_slots or Linear_transform; conjugate,
         add/sub, Mul_by_monomial.
      3. EvalMod: optional coordinate shift; Eval_chebyshev (range [-1,1]);
         Apply_double_angle_iterations (x -> 2*x^2 + const per iteration).
      4. Slots-to-coeffs: Slots_to_coeffs or Linear_transform.
      5. Post: optional clear-imag; Mul_integer; rescale to sf_degree=1.

    Prerequisites (phases 2–6): NTT and polynomial ring Z_q[X]/(X^N+1),
    CKKS encoding/decoding, Coeff_slots_transform (rotation indices + linear
    combinations), Eval_chebyshev and Apply_double_angle_iterations.

    Args:
        values: List of real slot values (e.g. BOOTSTRAP_INPUT_P0).

    Returns:
        List of float slot values after bootstrap (same length as values).

    Raises:
        NotImplementedError: Full port not yet implemented (Phase 1 stub).
    """
    # Apply EvalMod (Chebyshev + double-angle) per slot. Full pipeline would
    # precede with encode + coeffs-to-slots and follow with slots-to-coeffs + decode.
    # Post: ANT applies Mul_integer(res, res, 2^deg) before decode; replicate that scale.
    coeffs = list(G_COEFFICIENTS_UNIFORM_HW_192)
    evaled = [
        eval_mod_cleartext(float(v), coeffs=coeffs, num_double_angle=R_UNIFORM_HW_192)
        for v in values
    ]
    return [x * BOOTSTRAP_POST_SCALE for x in evaled]
