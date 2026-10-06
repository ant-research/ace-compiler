"""F2.2 POC -- openfhe-python EvalBootstrap gap-filler (rt_openfhe, OpenFHE 1.5.1).

Gap-filler: this POC never existed in git. It proves the load-bearing
assumption of the rt_openfhe provider (lands in F2.3a+): the upstream
OpenFHE 1.5.1 ``openfhe-python`` pybind11 binding imports natively under
CPython 3.12, and a minimal ``EvalBootstrap`` on a toy CKKS ring recovers
a real-valued message within ``max_abs < 1e-3`` HOST-ONLY (no device). This
is the design-first POC gate (F2.1 design doc, plan sec.2.1 gate 1) before
any contract or impl.

Config mirrors the rt_openfhe provider's REAL ``_build_bootstrap_context``
(rtlib-py/rtlib_py/rt_openfhe/__init__.py, feat backup 2859787f): CKKS,
``SecretKeyDist.UNIFORM_TERNARY`` (the rt_openfhe 1.5.1 default, NOT fhert's
SPARSE_TERNARY), ``KeySwitchTechnique.HYBRID``,
``SecurityLevel.HEStd_NotSet`` (toy; avoids auto ring raise),
``level_budget=[4,4]`` (env ``FHE_BTS_LEVEL_BUDGET``), full packing
``num_slots=ring/2``, and the 1.5.1 positional ``EvalBootstrapSetup(level_budget)``
form (rt_fhert's 1.4.2 binding takes ``slots`` / ``correctionFactor`` kwargs;
1.5.1 is positional-only).

Scaling (F2.1 design sec. FLEX scaling): OpenFHE 1.5.1 ``openfhe-python``
exports the enum CLASSES (``ScalingTechnique`` / ``SecurityLevel``), NOT the
bare values fhert re-exports, so the POC writes
``of.ScalingTechnique.FIXEDMANUAL`` (class form). The provider DEFAULT is
FIXEDMANUAL (``FHE_BTS_SCALING`` default, ``rt_openfhe/__init__.py`` line
~1000). The locked default below is FIXEDMANUAL at ring 8192 (the toy
refresh floor clears 1e-3 with margin); if the FIXEDMANUAL drift floor at a
given ring exceeds 1e-3 the env ``POC_SCALING=flexibleautoext`` selects the
FLEX-EXT re-anchor the F2.1 design names, and ``POC_RING_DIM`` bumps the
ring (16384 / 32768).

Run:
  OPENFHE_PY_BUILD_DIR=<openfhe-python build dir> python3 poc_openfhe_bootstrap.py
exits 0 and prints ``POC OK max_abs=<v>`` (v < 1e-3) on success.
"""

import os
import sys


def _ensure_of():
    """Resolve the OpenFHE 1.5.1 openfhe-python binding the way rt_openfhe's
    lazy ``import openfhe as _of`` (feat line 884) resolves it, but minimal:
    the native binding ships as ``openfhe.*.so`` under
    ``OPENFHE_PY_BUILD_DIR``; add it to ``sys.path`` and ``import openfhe``.
    HOST-ONLY: no device is touched anywhere in this POC.
    """
    build_dir = os.environ.get("OPENFHE_PY_BUILD_DIR")
    if not build_dir:
        raise SystemExit(
            "set OPENFHE_PY_BUILD_DIR to the openfhe-python build dir "
            "(contains openfhe.*.so)")
    if os.path.isdir(build_dir) and build_dir not in sys.path:
        sys.path.insert(0, build_dir)
    import openfhe as of
    return of


def _build_bootstrap_context(of):
    """Build the toy CKKS bootstrap context + keys, mirroring rt_openfhe's
    ``_build_bootstrap_context`` params on the OpenFHE 1.5.1 binding. Returns
    ``(cc, keys, num_slots)`` with EvalBootstrap ready to call.
    """
    params = of.CCParamsCKKSRNS()
    params.SetSecretKeyDist(of.SecretKeyDist.UNIFORM_TERNARY)
    # 1.5.1 class enums (NOT the bare values fhert re-exports).
    params.SetSecurityLevel(of.SecurityLevel.HEStd_NotSet)
    scaling = os.environ.get("POC_SCALING", "fixedmanual").lower()
    if scaling in ("flexibleautoext", "ext", "fext", "flexext"):
        params.SetScalingTechnique(of.ScalingTechnique.FLEXIBLEAUTOEXT)
        flex = True
    else:
        params.SetScalingTechnique(of.ScalingTechnique.FIXEDMANUAL)
        flex = False
    params.SetKeySwitchTechnique(of.KeySwitchTechnique.HYBRID)
    ring_dim = int(os.environ.get("POC_RING_DIM", str(1 << 13)))  # 8192 default
    params.SetRingDim(ring_dim)
    params.SetFirstModSize(60)              # 64-bit native default
    params.SetScalingModSize(59)            # sf=59 (F3.1 / provider default)
    lb_env = os.environ.get("FHE_BTS_LEVEL_BUDGET", "4,4")
    level_budget = [int(x) for x in lb_env.split(",")]
    bts_depth = of.FHECKKSRNS.GetBootstrapDepth(
        level_budget, of.SecretKeyDist.UNIFORM_TERNARY)
    # Toy chain sizing: the real formula at mul_depth=0 collapses to
    # depth = bts_depth + R(2); R=2 is the 2-tower floor so EvalBootstrap
    # CoeffsToSlots keeps >=2 towers on the depleted branch.
    depth = bts_depth + 2
    params.SetMultiplicativeDepth(depth)
    cc = of.GenCryptoContext(params)
    for feature in (of.PKE, of.KEYSWITCH, of.LEVELEDSHE, of.ADVANCEDSHE, of.FHE):
        cc.Enable(feature)
    num_slots = cc.GetRingDimension() // 2  # full packing
    # 1.5.1 positional-only EvalBootstrapSetup (no slots/correctionFactor kwargs).
    cc.EvalBootstrapSetup(level_budget)
    keys = cc.KeyGen()
    cc.EvalMultKeyGen(keys.secretKey)
    cc.EvalBootstrapKeyGen(keys.secretKey, num_slots)
    return cc, keys, num_slots, flex


def main():
    of = _ensure_of()
    cc, keys, num_slots, flex = _build_bootstrap_context(of)
    # Small real-valued message in the lower slot range; the bootstrap
    # refreshes it from a depleted (near-bottom) level back near the top.
    message = [0.25, 0.5, -0.375, 0.875, 0.0625, -0.5, 1.0, 0.125]
    depth = cc.GetMultiplicativeDepth()
    # Encrypt at level depth-1: the depleted input EvalBootstrap refreshes.
    plaintext = cc.MakeCKKSPackedPlaintext(
        message, noiseScaleDeg=1, level=depth - 1, slots=num_slots)
    ciphertext = cc.Encrypt(keys.publicKey, plaintext)
    refreshed = cc.EvalBootstrap(ciphertext)
    # ant-faithful nsd-reset (rt_openfhe/__init__.py lines 1542-1548): the
    # bounded (post_nsd - 1) form. Under FIXEDMANUAL Rescale decrements the
    # real nsd, so this lands at nsd == 1; under FLEX Rescale does NOT
    # decrement nsd, so the bounded form is mandatory (the while>1 form would
    # not terminate). Same count of tower drops either way.
    post_nsd = int(refreshed.GetNoiseScaleDeg())
    for _ in range(max(0, post_nsd - 1)):
        refreshed = cc.Rescale(refreshed)
    decoded = cc.Decrypt(keys.secretKey, refreshed)
    n = len(message)
    decoded.SetLength(n)
    recovered = [float(x) for x in decoded.GetRealPackedValue()]
    max_abs = max(abs(message[i] - recovered[i]) for i in range(n))
    assert max_abs < 1e-3, (
        f"bootstrap max_abs {max_abs:g} not < 1e-3 "
        f"(ring {cc.GetRingDimension()}, lb={os.environ.get('FHE_BTS_LEVEL_BUDGET','4,4')}, "
        f"UNIFORM_TERNARY, {'FLEXIBLEAUTOEXT' if flex else 'FIXEDMANUAL'})")
    print(f"POC OK max_abs={max_abs:g}")


if __name__ == "__main__":
    main()
