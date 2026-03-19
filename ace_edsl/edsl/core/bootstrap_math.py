"""
Paterson-Stockmeyer algorithm helpers and bootstrap constants.

Pure Python (no EDSL dependency).  Used for compile-time preprocessing
of Chebyshev polynomial division in the PS evaluation scheme.

Source: fhe-cmplr/rtlib/ant/ckks/src/chebyshev_impl.c
"""

import math
from typing import List, Tuple

# ---------------------------------------------------------------------------
# Chebyshev coefficients (UNIFORM_HW_UNDER_192, deg=54, K=32, R=3)
# Source: fhe-cmplr/rtlib/ant/include/ckks/bootstrap.h
# ---------------------------------------------------------------------------
UNIFORM_COEFF_SIZE = 55
CHEBYSHEV_COEFFICIENTS: Tuple[float, ...] = (
    1.74551960283504837e-01, -3.43838095837535329e-02,
    1.88307649106864788e-01, -2.84223873992535993e-02,
    2.22419882865789564e-01, -1.43397005803286518e-02,
    2.51103798550390944e-01,  9.50854609032555226e-03,
    2.24475678532524398e-01,  3.79342483118012136e-02,
    8.78908877085935597e-02,  5.18464470537667449e-02,
   -1.40269389175310705e-01,  2.52026526332414826e-02,
   -2.71343812500084935e-01, -3.49285487170959558e-02,
   -6.17395308539803664e-02, -5.05648932050318592e-02,
    2.82155868186952818e-01,  2.98272328751879069e-02,
    5.54332147538673034e-02,  4.73762170911353267e-02,
   -3.42589653109854397e-01, -7.19260908452365733e-02,
    3.19234546310780576e-01,  4.93494016031356467e-02,
   -1.74337152324168188e-01, -2.23994935740034137e-02,
    6.76154588798445894e-02,  7.56838175610476029e-03,
   -2.01915893273537893e-02, -2.01996389480041394e-03,
    4.85990579019698801e-03,  4.41705640530539389e-04,
   -9.71526466295980677e-04, -8.11544278739113802e-05,
    1.64814371135792263e-04,  1.27637159472312703e-05,
   -2.41183607585707303e-05, -1.74347427937465971e-06,
    3.08411936249047440e-06,  2.09259735883450997e-07,
   -3.48280526734833634e-07, -2.22825972864890841e-08,
    3.50404774489712212e-08,  2.12216680463557985e-09,
   -3.16453692971713038e-09, -1.82031853692548044e-10,
    2.58203419199988530e-10,  1.41483617957390541e-11,
   -1.91412743082734574e-11, -1.00089939783634691e-12,
    1.29702147256041809e-12,  6.67556346626149772e-14,
   -7.81869621069283006e-14,
)

NUM_DOUBLE_ANGLE = 3  # R_UNIFORM_HW_192


def get_double_angle_scalars(r: int = NUM_DOUBLE_ANGLE) -> List[float]:
    """Double-angle scalars: scalar_j = -1/(2*pi)^(2^(j-r)), j=1..r."""
    return [-1.0 / ((2.0 * math.pi) ** (2 ** (j - r))) for j in range(1, r + 1)]


DOUBLE_ANGLE_SCALARS = get_double_angle_scalars()

# Post-scale: 2^deg, deg = round(log2(q0/sf)).  Demo: first_prime=60, sf=56 → deg=4.
BOOTSTRAP_POST_SCALE_DEG = 4
BOOTSTRAP_POST_SCALE = 2 ** BOOTSTRAP_POST_SCALE_DEG

# ---------------------------------------------------------------------------
# Paterson-Stockmeyer parameter table (chebyshev_impl.c)
# ---------------------------------------------------------------------------
UPPER_BOUND_PS = 2204
_PS_RANGES = [2, 11, 13, 17, 55, 59, 76, 239, 247, 284, 991, 1007, 1083, 2015, 2031, UPPER_BOUND_PS]
_PS_VALUES = [1,  2,  3,  2,  3,  4,  3,   4,   5,   4,   5,    6,    5,    6,    7,              6]

_PREC = 9.5367431640625e-07  # 2^-20


def _is_not_equal_one(val: float) -> bool:
    return abs(val - 1.0) > _PREC


def get_degree_from_coeffs(coeffs: List[float]) -> int:
    """Index of last non-zero coefficient (= polynomial degree)."""
    for i in range(len(coeffs) - 1, 0, -1):
        if coeffs[i] != 0.0:
            return i
    return 0


def is_even_poly(coeffs: List[float]) -> bool:
    """True if all odd-degree coefficients are zero."""
    n = get_degree_from_coeffs(coeffs)
    for deg in range(1, n + 1, 2):
        if deg < len(coeffs) and coeffs[deg] != 0.0:
            return False
    return True


def compute_degree_ps(n: int) -> Tuple[int, int]:
    """Compute (k, m) for Paterson-Stockmeyer.

    k: baby-step count, m: doubling steps.
    Guarantees n < k * (2^m - 1).
    """
    assert n > 0
    if n <= UPPER_BOUND_PS:
        idx = n - 1  # 0-based
        m_val = _PS_VALUES[0]
        for i, boundary in enumerate(_PS_RANGES):
            if idx < boundary:
                m_val = _PS_VALUES[i]
                break
        k = int(math.floor(n / ((1 << m_val) - 1))) + 1
        return (k, m_val)
    # heuristic for large degrees
    sqn2 = math.floor(math.log2(math.sqrt(n / 2)))
    res_k, res_m, min_mult = 0, 0, 0xFFFFFFFF
    for k in range(1, n + 1):
        for m in range(1, int(math.ceil(math.log2(n / k) + 1)) + 2):
            if n - k * ((1 << m) - 1) < 0:
                if abs(math.floor(math.log2(k)) - sqn2) <= 1:
                    mul = k + 2 * m + (1 << (m - 1)) - 4
                    if min_mult > mul:
                        min_mult, res_k, res_m = mul, k, m
    return (res_k, res_m)


# ---------------------------------------------------------------------------
# Chebyshev polynomial long division  (Long_div_chebyshev in chebyshev_impl.c)
# ---------------------------------------------------------------------------

def long_div_chebyshev(f: List[float], g: List[float]) -> Tuple[List[float], List[float]]:
    """Chebyshev long division f / g → (quotient, remainder).

    Uses the Chebyshev multiplication identity T_m * T_n = (T_{m+n} + T_{|m-n|})/2.
    The zero-th coefficient convention is c0 (not c0/2); the quotient's c0 is doubled.
    """
    n = get_degree_from_coeffs(f)
    k = get_degree_from_coeffs(g)

    r = list(f)

    if n < k:
        return ([0.0], list(r))

    q = [0.0] * (n - k + 1)

    while n > k:
        q_nk = 2.0 * r[n] if n < len(r) else 0.0
        if _is_not_equal_one(g[k]):
            q_nk /= g[k]
        q[n - k] = q_nk

        # build d
        d = [0.0] * (n + 1)
        nmk = n - k
        if k == nmk:
            d[0] = 2.0 * _safe(g, nmk)
            for i in range(1, 2 * k + 1):
                if i < len(d):
                    d[i] = _safe(g, abs(nmk - i))
        elif k > nmk:
            d[0] = 2.0 * _safe(g, nmk)
            for i in range(1, k - nmk + 1):
                if i < len(d):
                    d[i] = _safe(g, abs(nmk - i)) + _safe(g, nmk + i)
            for i in range(k - nmk + 1, n + 1):
                if i < len(d):
                    d[i] = _safe(g, abs(i - n + k))
        else:
            if nmk < len(d):
                d[nmk] = _safe(g, 0)
            for i in range(n - 2 * k, n + 1):
                if 0 <= i < len(d):
                    d[i] = _safe(g, abs(i - n + k))

        r_back = r[n] if n < len(r) else 0.0
        if _is_not_equal_one(r_back):
            d = [di * r_back for di in d]
        g_back = g[k]
        if _is_not_equal_one(g_back):
            d = [di / g_back for di in d]

        for i in range(len(r)):
            if i < len(d):
                r[i] -= d[i]

        if len(r) > 1:
            n = get_degree_from_coeffs(r)
            r = r[: n + 1]

    if n == k:
        r_back = r[n] if n < len(r) else 0.0
        g_back = g[k]
        q0 = r_back
        if _is_not_equal_one(g_back):
            q0 /= g_back
        q[0] = q0

        d = list(g)
        if _is_not_equal_one(r_back):
            d = [di * r_back for di in d]
        if _is_not_equal_one(g_back):
            d = [di / g_back for di in d]
        for i in range(len(r)):
            if i < len(d):
                r[i] -= d[i]
        if len(r) > 1:
            n = get_degree_from_coeffs(r)
            r = r[: n + 1]

    # convention: c0 not c0/2
    q[0] *= 2.0
    return (q, r)


def compute_chebyshev_depths(k: int, is_even: bool) -> List[int]:
    """Compute the multiplicative depth of each baby-step T_1..T_k.

    The depth tells how many levels each T_i consumes during the
    binary-tree construction.  T_k (at index k-1) is the deepest;
    the rtlib (chebyshev_impl.c:611-630) mod-switches shallower T_i
    down to match T_k's level.

    Args:
        k: Baby-step count (number of Chebyshev basis polynomials).
        is_even: True when the polynomial has only even-degree terms.

    Returns:
        List of k integers, where depths[i] is the depth of T_{i+1}.
        ``None``-equivalent entries (odd terms skipped for even poly)
        are set to -1.
    """
    depths = [0] * k
    depths[0] = 0  # T_1 = x, depth 0

    for i in range(2, k + 1):
        j = i - 1  # 0-based index
        if (i & (i - 1)) == 0:
            # power of 2: T_i = 2*T_{i/2}^2 - 1 → depth = depth(T_{i/2}) + 1
            depths[j] = depths[i // 2 - 1] + 1
        elif i % 2 == 1:
            # odd, non-power-of-2
            if is_even:
                depths[j] = -1  # skipped
                continue
            # T_i = 2*T_{floor(i/2)}*T_{ceil(i/2)} - y → max(deps) + 1
            depths[j] = max(depths[i // 2 - 1], depths[i // 2]) + 1
        else:
            # even, non-power-of-2
            ihalf_1 = i // 2
            if is_even and ihalf_1 % 2 == 1:
                ihalf_1 += 1
            ihalf_2 = i - ihalf_1
            depths[j] = max(depths[ihalf_1 - 1], depths[ihalf_2 - 1]) + 1

    return depths


def _safe(lst: List[float], idx: int) -> float:
    """Safe list access, return 0.0 for out-of-bounds."""
    if 0 <= idx < len(lst):
        return lst[idx]
    return 0.0
