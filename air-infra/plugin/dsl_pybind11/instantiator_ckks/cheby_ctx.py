#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

import math
from enum import Enum
from typing import List, Tuple

# Sparse polynomial coefficients
G_COEFFICIENTS_SPARSE = [
    -0.18646470117093214, 0.036680543700430925, -0.20323558926782626,
    0.029327390306199311, -0.24346234149506416, 0.011710240188138248,
    -0.27023281815251715, -0.017621188001030602, -0.21383614034992021,
    -0.048567932060728937, -0.013982336571484519, -0.051097367628344978,
    0.24300487324019346, 0.0016547743046161035, 0.23316923792642233,
    0.060707936480887646, -0.18317928363421143, 0.0076878773048247966,
    -0.24293447776635235, -0.071417413140564698, 0.37747441314067182,
    0.065154496937795681, -0.24810721693607704, -0.033588418808958603,
    0.10510660697380972, 0.012045222815124426, -0.032574751830745423,
    -0.0032761730196023873, 0.0078689491066424744, 0.00070965574480802061,
    -0.0015405394287521192, -0.00012640521062948649, 0.00025108496615830787,
    0.000018944629154033562, -0.000034753284216308228, -2.4309868106111825e-6,
    4.1486274737866247e-6, 2.7079833113674568e-7, -4.3245388569898879e-7,
    -2.6482744214856919e-8, 3.9770028771436554e-8, 2.2951153557906580e-9,
    -3.2556026220554990e-9, -1.7691071323926939e-10, 2.5459052150406730e-10
]

# Uniform polynomial coefficients (truncated, full List in actual implementation)
G_COEFFICIENTS_UNIFORM_HW_192 = [
    # deg= 54, K= 32, R= 3
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
    -7.81869621069283006e-14
]

G_EVEN_COEFFICIENTS_UNIFORM_HW_192 = [
    # deg= 54, K= 32, R= 3
    1.77971635352991098e-01, 0., 1.91996814052353887e-01, 0.,
    2.26777345979688877e-01, 0., 2.56023212794501354e-01, 0.,
    2.28873417064574675e-01, 0., 8.96127719947621554e-02, 0.,
    -1.43017428970428406e-01, 0., -2.76659752059622344e-01, 0.,
    -6.29490797706480504e-02, 0., 2.87683628440008832e-01, 0.,
    5.65192156341060015e-02, 0., -3.49301381204462469e-01, 0.,
    3.25488720813115806e-01, 0., -1.77752619056938649e-01, 0.,
    6.89401240320876713e-02, 0., -2.05871659483433701e-02, 0.,
    4.95511697341025851e-03, 0., -9.90559795000754261e-04, 0.,
    1.68043275555716744e-04, 0., -2.45908673799202791e-05, 0.,
    3.14454083284708929e-06, 0., -3.55103745652075532e-07, 0.,
    3.57269609461536428e-08, 0., -3.22653396792257366e-09, 0.,
    2.63261700897943539e-10, 0., -1.95167968786489595e-11, 0.,
    1.32120352716508172e-12, 0., -8.22849593050680629e-14,
]

G_COEFFICIENTS_UNIFORM = [
    0.15421426400235561, -0.0037671538417132409, 0.16032011744533031,
    -0.0034539657223742453, 0.17711481926851286, -0.0027619720033372291,
    0.19949802549604084, -0.0015928034845171929, 0.21756948616367638,
    0.00010729951647566607, 0.21600427371240055, 0.0022171399198851363,
    0.17647500259573556, 0.0042856217194480991, 0.086174491919472254,
    0.0054640252312780444, -0.046667988130649173, 0.0047346914623733714,
    -0.17712686172280406, 0.0016205080004247200, -0.22703114241338604,
    -0.0028145845916205865, -0.13123089730288540, -0.0056345646688793190,
    0.078818395388692147, -0.0037868875028868542, 0.23226434602675575,
    0.0021116338645426574, 0.13985510526186795, 0.0059365649669377071,
    -0.13918475289368595, 0.0018580676740836374, -0.23254376365752788,
    -0.0054103844866927788, 0.056840618403875359, -0.0035227192748552472,
    0.25667909012207590, 0.0055029673963982112, -0.073334392714092062,
    0.0027810273357488265, -0.24912792167850559, -0.0069524866497120566,
    0.21288810409948347, 0.0017810057298691725, 0.088760951809475269,
    0.0055957188940032095, -0.31937177676259115, -0.0087539416335935556,
    0.34748800245527145, 0.0075378299617709235, -0.25116537379803394,
    -0.0047285674679876204, 0.13970502851683486, 0.0023672533925155220,
    -0.063649401080083698, -0.00098993213448982727, 0.024597838934816905,
    0.00035553235917057483, -0.0082485030307578155, -0.00011176184313622549,
    0.0024390574829093264, 0.000031180384864488629, -0.00064373524734389861,
    -7.8036008952377965e-6, 0.00015310015145922058, 1.7670804180220134e-6,
    -0.000033066844379476900, -3.6460909134279425e-7, 6.5276969021754105e-6,
    6.8957843666189918e-8, -1.1842811187642386e-6, -1.2015133285307312e-8,
    1.9839339947648331e-7, 1.9372045971100854e-9, -3.0815418032523593e-8,
    -2.9013806338735810e-10, 4.4540904298173700e-9, 4.0505136697916078e-11,
    -6.0104912807134771e-10, -5.2873323696828491e-12, 7.5943206779351725e-11,
    6.4679566322060472e-13, -9.0081200925539902e-12, -7.4396949275292252e-14,
    1.0057423059167244e-12, 8.1701187638005194e-15, -1.0611736208855373e-13,
    -8.9597492970451533e-16, 1.1421575296031385e-14]

G_EVEN_COEFFICIENTS_UNIFORM = [
    # deg= 54, K= 512, R= 7
    2.20743281549609704e-01, 0., 2.38139109622304862e-01, 0.,
    2.81278392668466215e-01, 0., 3.17552873147672721e-01, 0.,
    2.83878209255650049e-01, 0., 1.11149270048825827e-01, 0.,
    -1.77388585136637050e-01, 0., -3.43148959783472818e-01, 0.,
    -7.80775341617799268e-02, 0., 3.56822187220964315e-01, 0.,
    7.01023907823122711e-02, 0., -4.33248438628725141e-01, 0.,
    4.03712918618547689e-01, 0., -2.20471629407921144e-01, 0.,
    8.55083967683751417e-02, 0., -2.55348475066272465e-02, 0.,
    6.14597252535931983e-03, 0., -1.22861948920053167e-03, 0.,
    2.08428854491001886e-04, 0., -3.05007522734078458e-05, 0.,
    3.90026343823076681e-06, 0., -4.40445276295426794e-07, 0.,
    4.43131658784083885e-08, 0., -4.00196185587582648e-09, 0.,
    3.26530975833769012e-10, 0., -2.42072383039309157e-11, 0.,
    1.63872631502701690e-12, 0., -1.02060375537654724e-13,
]

HW_192 = 192
PREC = 9.5367431640625e-07  # 2^-20
# Precomputed m values for degrees up to UPPER_BOUND_PS (2204)
# Format: {end_degree: m_value}
PRECOMPUTED_M_RANGES = [
    (2, 1), (11, 2), (13, 3), (17, 2), (55, 3), (59, 4), (76, 3), (239, 4),
    (247, 5), (284, 4), (991, 5), (1007, 6), (1083, 5), (2015, 6),
    (2031, 7), (2204, 6)
]
# Populate the conversion table Degree-to-Multiplicative Depth
LOWER_BOUND_DEGREE = 5
UPPER_BOUND_DEGREE = 2031


class EvalSinPolyKind(Enum):
    SPARSE = 0
    UNIFORM_HW_UNDER_192 = 1
    UNIFORM_HW_ABOVE_192 = 2
    UNIFORM_EVEN_HW_UNDER_192 = 3
    UNIFORM_EVEN_HW_ABOVE_192 = 4
    LAST_KIND = 5


class EvalSinPolyInfo:
    def __init__(self, kind: EvalSinPolyKind, upper_bound: int,
                 double_angle: int, coeffs: List[float], d_coeffs=0):
        self._kind = kind
        self._upper_bound = upper_bound
        self._double_angle = double_angle
        self._coeffs = coeffs
        self._d_coeffs = d_coeffs


EVAL_SIN_POLY_INFO: List[EvalSinPolyInfo] = [
    EvalSinPolyInfo(EvalSinPolyKind.SPARSE, 14, 3, G_COEFFICIENTS_SPARSE, 44),
    EvalSinPolyInfo(EvalSinPolyKind.UNIFORM_HW_UNDER_192, 32, 3, G_COEFFICIENTS_UNIFORM_HW_192, 54),
    EvalSinPolyInfo(EvalSinPolyKind.UNIFORM_HW_ABOVE_192, 512, 6, G_COEFFICIENTS_UNIFORM, 88),
    EvalSinPolyInfo(EvalSinPolyKind.UNIFORM_EVEN_HW_UNDER_192, 32,
                    3, G_EVEN_COEFFICIENTS_UNIFORM_HW_192, 54),
    EvalSinPolyInfo(EvalSinPolyKind.UNIFORM_EVEN_HW_ABOVE_192,
                    512, 7, G_EVEN_COEFFICIENTS_UNIFORM, 54)
]


def get_eval_sin_upper_bound_k(hw, is_even):
    poly_kind = EvalSinPolyKind.LAST_KIND
    if hw > 0 and hw <= HW_192:
        poly_kind = EvalSinPolyKind.UNIFORM_EVEN_HW_UNDER_192 if is_even else \
            EvalSinPolyKind.UNIFORM_HW_UNDER_192
    else:
        poly_kind = EvalSinPolyKind.UNIFORM_EVEN_HW_ABOVE_192 if is_even else \
            EvalSinPolyKind.UNIFORM_HW_ABOVE_192
    return EVAL_SIN_POLY_INFO[poly_kind.value]._upper_bound


def gen_depth_by_degree_table():
    """Generate the degree to multiplicative depth lookup table.

    Returns:
        list: A list where index represents degree and value is multiplicative depth.
    """
    # This table would normally be precomputed based on Paterson-Stockmeyer algorithm
    table = [0] * (UPPER_BOUND_DEGREE + 1)

    for degree in range(UPPER_BOUND_DEGREE + 1):
        # degree in [0,4], depth = 3 - the Paterson-Stockmeyer
        # algorithm is not used when degree < 5
        if degree < 5:
            table[degree] = 3
        if degree == 5:
            table[degree] = 4
        elif degree <= 13:
            table[degree] = 5
        elif degree <= 27:
            table[degree] = 6
        elif degree <= 59:
            table[degree] = 7
        elif degree <= 119:
            table[degree] = 8
        elif degree <= 247:
            table[degree] = 9
        elif degree <= 495:
            table[degree] = 10
        elif degree <= 1007:
            table[degree] = 11
        elif degree <= UPPER_BOUND_DEGREE:
            table[degree] = 12
    return table


# Precompute the table once
DEPTH_BY_DEGREE_TABLE = gen_depth_by_degree_table()


class ChebyCtx:
    """Stores context for Chebyshev polynomial approximation in CKKS bootstrapping.

    Manages coefficients and parameters for Chebyshev interpolation of the modular
    reduction function during bootstrapping using the Paterson-Stockmeyer algorithm.

    Attributes:
        _coeffs (List[float]): Chebyshev polynomial coefficients for approximation
        _d_coeffs (int): degree of coeffs
        _ub (float): Upper bound of coeffs
        _k (int): Polynomial degree for approximation
        _m (int): Number of approximation intervals
        _is_even_poly (bool): Flag indicating even polynomial symmetry optimization
        _f2 (float): Squared scaling factor for input normalization
        _divq (int): Scaling parameter for coefficient adjustment
        _divqk (int): Scaled polynomial degree (k * divq)
        _divr2_q (int): Residual scaling factor for coefficient division
        _s2 (float): Squared scaling factor for interval normalization
        _s2k (float): Scaled polynomial degree factor (k * s2)
        _dangle_cnt (int): Number of double-angle iterations.
        _dangle_adj(List[float]): Adjust values for double angle
    """
    _pinfo: EvalSinPolyInfo
    _k: int
    _m: int
    _f2: List[float]
    _divq: List[List[float]]
    _divr2_q: List[List[float]]
    _s2: List[List[float]]
    _dangle_adj: List[float]

    def __init__(self, ctx, even_poly):
        if ctx._hw > 0 and ctx._hw <= HW_192:
            pkind = EvalSinPolyKind.UNIFORM_EVEN_HW_UNDER_192 if even_poly else \
                EvalSinPolyKind.UNIFORM_HW_UNDER_192
        else:
            pkind = EvalSinPolyKind.UNIFORM_EVEN_HW_ABOVE_192 if even_poly else \
                EvalSinPolyKind.UNIFORM_HW_ABOVE_192
        self._pinfo = EVAL_SIN_POLY_INFO[pkind.value]
        self._k, self._m = self.compute_degree_ps(self._pinfo._d_coeffs)
        self._f2 = self._pinfo._coeffs[:self._pinfo._d_coeffs + 1]
        k2m2k = self._k * (1 << (self._m - 1)) - self._k
        self._f2 = self.resize(self._f2, 2 * k2m2k + self._k + 1, 0.0)
        self._f2[-1] = 1

        self._dangle_adj = [0.0] * (self._pinfo._double_angle + 1)
        for i in range(1, len(self._dangle_adj)):
            self._dangle_adj[i] = -1.0 / (2.0 * math.pi) ** \
                (2.0 ** (i - self._pinfo._double_angle))

        self._divq = []
        self._divr2_q = []
        self._s2 = []
        self.compute_div_polys(self._k, self._m, self._f2)

    def get_approx_depth(self):
        coeff_size = len(self._pinfo._coeffs)
        dangle_num = self._pinfo._double_angle
        return ChebyCtx.get_mul_depth_by_coeff_vector(coeff_size, True) + dangle_num

    @staticmethod
    def get_depth_by_degree(degree):
        """Get multiplicative depth required for a given polynomial degree.

        Args:
            degree (int): The polynomial degree

        Returns:
            int: The multiplicative depth required
        """
        if degree < LOWER_BOUND_DEGREE or degree > UPPER_BOUND_DEGREE:
            raise ValueError(
                f"Degree must be between {LOWER_BOUND_DEGREE} and {UPPER_BOUND_DEGREE}")
        return DEPTH_BY_DEGREE_TABLE[degree]

    @staticmethod
    def get_mul_depth_by_coeff_vector(vec_len, is_normalized=False):
        """Get the multiplicative depth for a given coefficient vector.

        Args:
            vec_len (int): Length of coefficient vector
            is_normalized (bool): Whether the coefficients are normalized

        Returns:
            int: The multiplicative depth required
        """
        if vec_len == 0:
            raise ValueError("Cannot perform operation on empty vector")

        degree = vec_len - 1
        mult_depth = ChebyCtx.get_depth_by_degree(degree)
        return (mult_depth - 1) if is_normalized else mult_depth

    def is_even_poly(self):
        if self._pinfo._kind == EvalSinPolyKind.UNIFORM_EVEN_HW_UNDER_192 or \
           self._pinfo._kind == EvalSinPolyKind.UNIFORM_EVEN_HW_ABOVE_192:
            return True
        else:
            return False

    def get_degree_from_coeffs(self, coeffs):
        if len(coeffs) == 0:
            return 0
        deg = 1
        for item in coeffs[::-1]:
            if item == 0:
                deg += 1
            else:
                break
        return len(coeffs) - deg

    def compute_degree_ps(self, n: int) -> Tuple[int, int]:
        """
        Compute positive integers k,m such that n < k(2^m-1), k is close to
        sqrt(n/2) and the depth = ceil(log2(k))+m is minimized. Moreover, for that
        depth the number of homomorphic multiplications = k+2m+2^(m-1)-4 is minimized.

        Since finding these parameters involves testing many possible values, we
        hardcode them for commonly used degrees, and provide a heuristic which
        minimizes the number of homomorphic multiplications for the rest of the
        degrees.

        Args:
            n: Polynomial degree to compute parameters for

        Returns:
            Tuple[int, int]: Optimal (k, m) parameters
        """
        if n == 0:
            raise ValueError("degree is zero, no need to evaluate the polynomial")

        # Check precomputed values for degrees up to 2204
        if n <= 2204:
            # Find the appropriate m value from precomputed ranges
            m = 1
            for end_degree, m_val in PRECOMPUTED_M_RANGES:
                if n <= end_degree:
                    m = m_val
                    break

            k = math.floor(n / ((1 << m) - 1)) + 1
            return (k, m)

        # Heuristic for larger degrees
        sqn2 = math.floor(math.log2(math.sqrt(n / 2)))
        min_mult = 0xFFFFFFFF
        res_k, res_m = 0, 0

        # Search space optimization based on original C implementation
        for k in range(1, n + 1):
            max_m = math.ceil(math.log2(n / k) + 1) + 1
            for m in range(1, max_m + 1):
                if n - k * ((1 << m) - 1) < 0:
                    if abs(math.floor(math.log2(k)) - sqn2) <= 1:
                        # Calculate number of multiplications
                        current_mult = k + 2 * m + (1 << (m - 1)) - 4
                        if current_mult <= min_mult:
                            min_mult = current_mult
                            res_k, res_m = k, m

        return (res_k, res_m)

    def resize(self, lst, new_size, fill_value=0):
        """
        Resize the list to a new_size, filling with fill_value if needed.

        :param lst: List to resize.
        :param new_size: Desired size of the list.
        :param fill_value: Value to use for filling if the list is extended.
        :return: Resized list.
        """
        cur_size = len(lst)

        if new_size < cur_size:
            return lst[:new_size]
        elif new_size > cur_size:
            return lst + [fill_value] * (new_size - cur_size)
        else:
            return lst

    def compute_div_polys(self, k, m, coeffs):
        k2m2k = k * (1 << (m - 1)) - k
        tkm_len = k2m2k + k + 1
        tkm = [0.0] * tkm_len
        tkm[-1] = 1

        # Divide coefficients by T^{k*2^{m-1}}
        div_q, div_r = self.long_div_chebyshev(coeffs, tkm)
        self._divq.append(div_q)

        r2 = div_r.copy()

        d_div_r = self.get_degree_from_coeffs(div_r)
        d_r2 = self.get_degree_from_coeffs(r2)

        # Subtract x^{k(2^{m-1} - 1)} from r
        if k2m2k <= d_div_r:
            r2[k2m2k] = r2[k2m2k] - 1
            r2 = r2[:d_r2 + 1]
        else:
            r2 = self.resize(r2, k2m2k + 1, 0.0)
            r2[-1] = -1
        # Divide r2 by div_q
        divr2_q, divr2_r = self.long_div_chebyshev(r2, div_q)
        self._divr2_q.append(divr2_q)

        # Add x^{k(2^{m-1} - 1)} to s2
        s2_len = len(divr2_r)
        if s2_len <= k2m2k + 1:
            s2_len = k2m2k + 1
        s2 = [0.0] * s2_len
        s2[:len(divr2_r)] = divr2_r
        s2[-1] = 1
        self._s2.append(s2)

        d_div_q = self.get_degree_from_coeffs(div_q)
        if d_div_q > k:
            self.compute_div_polys(k, m - 1, div_q)

        d_s2 = self.get_degree_from_coeffs(s2)
        if d_s2 > k:
            self.compute_div_polys(k, m - 1, s2)

    def long_div_chebyshev(
        self,
        f: List[float],
        g: List[float]
    ) -> Tuple[List[float], List[float]]:
        """Chebyshev polynomial long division.

        f and g are vectors of Chebyshev interpolation coefficients of the
        two polynomials. We assume their dominant coefficient is not zero.
        Long_div_chebyshev returns the vector of Chebyshev interpolation
        coefficients for the quotient and remainder of the division f/g.
        We assume that the zero-th coefficient is c0, not c0/2 and returns
        the same format.

        Args:
            f: Dividend coefficients (c0, c1, ..., cn)
            g: Divisor coefficients (c0, c1, ..., ck)

        Returns:
            Tuple: (quotient, remainder) coefficients

        Note:
            - Assumes leading coefficients of f/g are non-zero
            - Maintains original coefficient format (c0 not c0/2)
        """
        assert f and g, "Null input polynomials"

        n = self.get_degree_from_coeffs(f)
        k = self.get_degree_from_coeffs(g)

        assert n == len(f) - 1 and k == len(g) - 1, "The dominant coefficient is zero"
        # breakpoint()
        r = f.copy()
        q = [0.0] * (n - k + 1) if n >= k else [0.0]

        if n >= k:
            while n > k:
                q_idx = n - k
                q_n_k = 2 * r[-1]

                # Handle divisor leading coefficient
                g_back = g[-1]
                if not math.isclose(g[k], 1.0, rel_tol=PREC):
                    q_n_k /= g_back
                q[q_idx] = q_n_k

                # Construct d polynomial
                d = [0.0] * (n + 1)
                if k == n - k:
                    d[0] = 2 * g[n - k]
                    for i in range(1, 2 * k + 1):
                        idx = abs(n - k - i)
                        d[i] = g[idx]
                else:
                    if k > (n - k):
                        d[0] = 2 * g[n - k]
                        for i in range(1, k - (n - k) + 1):
                            d[i] = (g[abs(n - k - i)] +
                                    g[n - k + i])
                        for i in range(k - (n - k) + 1, n + 1):
                            d[i] = g[abs(i - n + k)]
                    else:
                        d[n - k] = g[0]
                        for i in range(n - 2 * k, n + 1):
                            d[i] = g[abs(i - n + k)]

                # Scale d polynomial
                r_back = r[-1]
                if not math.isclose(r_back, 1.0, abs_tol=PREC):
                    d = [x * r_back for x in d]
                if not math.isclose(g_back, 1.0, abs_tol=PREC):
                    d = [x / g_back for x in d]

                # Subtract d from remainder
                r = [r[i] - d[i] for i in range(len(r))]

                # Update degree and truncate remainder
                if len(r) > 1:
                    n = self.get_degree_from_coeffs(r)
                    r = self.resize(r, n + 1, 0)

            if n == k:
                r_back = r[-1]
                g_back = g[-1]
                q[0] = r_back / g_back if not math.isclose(g_back, 1.0, abs_tol=PREC) else r_back

                # Construct d
                d = g.copy()
                if not math.isclose(r_back, 1.0, abs_tol=PREC):
                    d = [x * r_back for x in d]
                if not math.isclose(g_back, 1.0, abs_tol=PREC):
                    d = [x / g_back for x in d]

                # Subtract d from remainder
                r = [r[i] - d[i] for i in range(len(r))]
                n = self.get_degree_from_coeffs(r)
                r = self.resize(r, n + 1, 0)

            # Adjust q[0] to maintain coefficient format
            # We want to have [c0] in the last spot, not [c0/2]
            q[0] *= 2

        return q, r
