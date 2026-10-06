"""F4.2 POC -- fhert OpenFHE 1.4.2 binding bootstrap gap-filler.

Gap-filler: this POC never existed in git. It proves the load-bearing
assumption of the rt_fhert port (lands in F4.4+): the fhert OpenFHE 1.4.2
pybind11 binding imports natively under CPython 3.12 (the cp312 rebuild of
``fhert/_C/_lib`` + ``libfhert``), and a minimal ``EvalBootstrap`` on a toy
CKKS ring recovers a real-valued message within ``max_abs < 1e-4`` HOST-ONLY
(no CModel device, no network). This is the design-first POC gate before
any contract or impl.

Config mirrors the rt_fhert provider's REAL ``_build_bootstrap_context``
(rtlib-py/rtlib_py/rt_fhert/__init__.py, feat 2859787f): ring 16384, CKKS,
``SecretKeyDist.SPARSE_TERNARY`` (delta #3), ``KeySwitchTechnique.HYBRID``,
``level_budget=[4,4]`` (``_BTS_LEVEL_BUDGET``), ``correctionFactor=13``
(env ``FHE_BTS_CORRECTION`` default), full packing ``num_slots=ring/2``, and
``EvalBootstrapSetup(level_budget, slots=, correctionFactor=)`` in the
fhert-1.4.2 kwarg form (rt_openfhe 1.5.1 is positional-only).

Scaling (F4.1 delta #2): the fhert binding re-exports the bare enum VALUES,
NOT the enum classes, so ``FIXEDMANUAL`` / ``FLEXIBLEAUTO`` /
``FLEXIBLEAUTOEXT`` / ``HEStd_NotSet`` are all bare ``_of.<VALUE>`` calls,
never ``_of.ScalingTechnique.<VALUE>`` or ``_of.SecurityLevel.<VALUE>``.
This POC uses the provider DEFAULT ``FLEXIBLEAUTOEXT`` (``_BTS_SCALING``
default, rt_fhert/__init__.py:169 / :1099 "default EXT ... baseline"): a
single FLEX bootstrap re-anchors the scale in ``AdjustCiphertext`` and
clears the 1e-4 gate (~5e-5 on this toy ring). The non-FLEX ``FIXEDMANUAL``
baseline is also reachable as a bare ``_of.FIXEDMANUAL`` call, but its
scale-drift floor at ring 16384 (~4e-4, probed) exceeds 1e-4 (FIXEDMANUAL
blindly assumes the canonical Delta; FLEX reads the real scale and
compensates), so FLEXEXT is the provider's validated default.

Run:
  FHERT_ROOT=<fhert-root> python3.12 poc_fhert_binding.py
exits 0 and prints ``POC OK max_abs=<v>`` (v < 1e-4) on success.
"""

import os
import sys


def _ensure_of():
    """Resolve the fhert OpenFHE 1.4.2 binding the way rt_fhert._ensure_of
    does, but minimal: the cp312 native bindings ship under ``FHERT_ROOT``;
    add it to ``sys.path`` and import ``fhert.openfhe`` directly. HOST-ONLY:
    no CModel device is touched anywhere in this POC.
    """
    fheroot = os.environ.get("FHERT_ROOT")
    if not fheroot:
        raise SystemExit(
            "set FHERT_ROOT to the fhert source root (host-only cp312 binding)")
    if os.path.isdir(fheroot) and fheroot not in sys.path:
        sys.path.insert(0, fheroot)
    import fhert.openfhe as of  # native cp312 import
    return of


def _build_bootstrap_context(of):
    """Build the toy CKKS bootstrap context + keys, mirroring rt_fhert's
    ``_build_bootstrap_context`` params on the fhert 1.4.2 binding. Returns
    ``(cc, keys, num_slots)`` with EvalBootstrap ready to call.
    """
    params = of.CCParamsCKKSRNS()
    # Delta #3: SPARSE_TERNARY keeps the EvalMod Chebyshev degree low and the
    # sine accurate, and is REQUIRED for the explicit correctionFactor below.
    params.SetSecretKeyDist(of.SecretKeyDist.SPARSE_TERNARY)
    # Delta #2: bare enum VALUE re-exports, not the enum classes.
    params.SetSecurityLevel(of.HEStd_NotSet)
    params.SetScalingTechnique(of.FLEXIBLEAUTOEXT)   # provider default; re-anchors scale
    params.SetKeySwitchTechnique(of.KeySwitchTechnique.HYBRID)
    params.SetRingDim(1 << 14)             # 16384 (rt_fhert smoke ring)
    params.SetFirstModSize(60)             # 64-bit native default
    params.SetScalingModSize(59)           # 64-bit native default (sf=59)
    level_budget = [4, 4]                  # _BTS_LEVEL_BUDGET default
    bts_depth = of.FHECKKSRNS.GetBootstrapDepth(
        level_budget, of.SecretKeyDist.SPARSE_TERNARY)
    # Toy chain sizing. The real formula at mul_depth=0 collapses to
    # depth = levels_after(0) + bts_depth + R(2); R=2 is the 2-tower floor so
    # EvalBootstrap CoeffsToSlots keeps >=2 towers on the depleted branch.
    depth = bts_depth + 2
    params.SetMultiplicativeDepth(depth)
    cc = of.GenCryptoContext(params)
    for feature in (of.PKE, of.KEYSWITCH, of.LEVELEDSHE, of.ADVANCEDSHE, of.FHE):
        cc.Enable(feature)
    num_slots = cc.GetRingDimension() // 2  # full packing (FHE_DEV_BTS_HALF off)
    # fhert 1.4.2 EvalBootstrapSetup takes slots= and correctionFactor= kwargs;
    # cf=13 (env FHE_BTS_CORRECTION default) is REQUIRED under SPARSE_TERNARY
    # (the auto factor 9 is below the EvalMod Chebyshev degree).
    cc.EvalBootstrapSetup(level_budget, slots=num_slots, correctionFactor=13)
    keys = cc.KeyGen()
    cc.EvalMultKeyGen(keys.secretKey)
    cc.EvalBootstrapKeyGen(keys.secretKey, num_slots)
    return cc, keys, num_slots


def main():
    of = _ensure_of()
    cc, keys, _num_slots = _build_bootstrap_context(of)
    num_slots = _num_slots
    # Small real-valued message in the lower slot range; the bootstrap
    # refreshes it from a depleted (near-bottom) level back near the top.
    message = [0.25, 0.5, -0.375, 0.875, 0.0625, -0.5, 1.0, 0.125]
    depth = cc.GetMultiplicativeDepth()
    # Encrypt at level depth-1: the depleted input EvalBootstrap refreshes
    # (mirrors fhert tests/ckks/test_bootstrap_internals.py).
    plaintext = cc.MakeCKKSPackedPlaintext(
        message, noiseScaleDeg=1, level=depth - 1, slots=num_slots)
    ciphertext = cc.Encrypt(keys.publicKey, plaintext)
    refreshed = cc.EvalBootstrap(ciphertext)
    # rt_fhert ant-faithful nsd-reset (rt_fhert/__init__.py:2147-2149): under
    # FLEX, EvalBootstrap leaves the real noiseScaleDeg at 2; do (post_nsd-1)
    # bounded Rescales so downstream arithmetic re-enters at a clean nsd=1.
    post_nsd = int(refreshed.GetNoiseScaleDeg())
    for _ in range(max(0, post_nsd - 1)):
        refreshed = cc.Rescale(refreshed)
    decoded = cc.Decrypt(keys.secretKey, refreshed)
    decoded.SetLength(num_slots)
    recovered = [float(x) for x in decoded.GetRealPackedValue()]
    n = len(message)
    max_abs = max(abs(message[i] - recovered[i]) for i in range(n))
    assert max_abs < 1e-4, (
        f"bootstrap max_abs {max_abs:g} not < 1e-4 "
        f"(ring 16384, lb=[4,4], cf=13, SPARSE_TERNARY, FLEXIBLEAUTOEXT)")
    print(f"POC OK max_abs={max_abs:g}")


if __name__ == "__main__":
    main()
