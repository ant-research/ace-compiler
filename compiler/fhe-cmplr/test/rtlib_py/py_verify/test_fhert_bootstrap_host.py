"""Phase 3b: a REAL fhert OpenFHE 1.4.2 ``EvalBootstrap`` smoke test for
`rtlib_py.rt_fhert`.

Mirrors ``test_openfhe_bootstrap.py`` but targets the fhert provider. The
fhert bootstrap context is built by ``rt_fhert._build_bootstrap_context``:
FIXEDMANUAL (identical nsd/Rescale semantics to rt_openfhe), SPARSE_TERNARY
secret key + an explicit correction factor (fhert 1.4.2 needs these -- the
default UNIFORM_TERNARY raises ``Degree must be <= correction factor`` and gives
poor refresh precision at packing slots), level_budget [4,4], and the model-driven
ring/moduli. The test encodes a small message near the bottom of the chain,
calls ``bootstrap_ciph(a, level, slot)`` -- the single Phase-2.4 name behind
which a real EvalBootstrap is swapped -- and asserts the message is recovered
within a lossy tolerance with a fresh ``scale_deg=1`` and ``is_cipher3=False``,
and that the cipher was refreshed to a higher level (more headroom).

The whole module skips when the fhert openfhe binding cannot be imported
(pytest.importorskip on a sentinel; ctest SKIP_RETURN_CODE 77). Run:
``FHE_RUN_BOOTSTRAP=1 python3 -m pytest test_fhert_bootstrap_host.py -q``.

NOTE on tolerance: fhert bundles OpenFHE 1.4.2, whose host EvalBootstrap
precision can be looser than the 1.5.1 build rt_openfhe targets. The default
tolerance is 1e-1 (lossy bootstrap refresh); set ``FHE_BTS_TOL`` to tighten it.
The test prints the measured ``max_abs`` so the real precision is reported, not
hidden -- a failure surfaces the actual error magnitude.
"""

import os
import sys

import pytest

_HERE = os.path.dirname(os.path.abspath(__file__))
if _HERE not in sys.path:
    sys.path.insert(0, _HERE)

_RTLIB_PY_DIR = os.environ.get("RTLIB_PY_DIR") or os.path.abspath(
    os.path.join(_HERE, "..", "..", "..", "rtlib-py"))
if os.path.isdir(os.path.join(_RTLIB_PY_DIR, "rtlib_py")) and \
        _RTLIB_PY_DIR not in sys.path:
    sys.path.insert(0, _RTLIB_PY_DIR)

# fhert source root (the provider's _ensure_of also reads FHERT_ROOT; set it
# here so a direct pytest run finds the binding).
_FHERT_ROOT = os.environ.get("FHERT_ROOT", "/workspace/fhert")
if os.path.isdir(_FHERT_ROOT) and _FHERT_ROOT not in sys.path:
    sys.path.insert(0, _FHERT_ROOT)


def _fhert_available():
    """Probe the fhert openfhe binding the way rt_fhert._ensure_of does (native
    import first, stub-bypass fallback second)."""
    try:
        import fhert.openfhe  # noqa: F401
        return True
    except Exception:
        pass
    import types
    import importlib.util
    pfx = os.path.join(_FHERT_ROOT, "fhert")
    lib_path = os.path.join(pfx, "_C", "_lib.so")
    if not os.path.exists(lib_path):
        return False
    for name, path in (("fhert", pfx), ("fhert._C", os.path.join(pfx, "_C"))):
        if name not in sys.modules:
            m = types.ModuleType(name)
            m.__path__ = [path]
            sys.modules[name] = m
    try:
        spec = importlib.util.spec_from_file_location("fhert._C._lib", lib_path)
        lib = importlib.util.module_from_spec(spec)
        sys.modules["fhert._C._lib"] = lib
        spec.loader.exec_module(lib)
        return hasattr(lib, "openfhe")
    except Exception:
        return False


if not _fhert_available():
    pytest.skip("fhert openfhe binding unavailable (FHERT_ROOT=" + _FHERT_ROOT
                + ") -- skipping the fhert real-bootstrap smoke",
                allow_module_level=True)

# Opt-out escape hatch for CI flakiness (the test runs by default).
if os.environ.get("FHE_RUN_BOOTSTRAP", "1") == "0":
    pytest.skip("FHE_RUN_BOOTSTRAP=0 -> skipping the fhert real-bootstrap smoke "
                "test", allow_module_level=True)

import rtlib_py.rt_fhert as F  # noqa: E402


def test_real_evalbootstrap_recovers_message():
    # Seed the bootstrap context. _build_context sees 'bootstrap': True and
    # calls _build_bootstrap_context (FIXEDMANUAL, SPARSE_TERNARY, level budget
    # [4,4]); the toy green-gate params are ignored for the scaling technique
    # but poly_degree/sf drive the ring/moduli.
    F._reset()
    F._seed_ctx({"provider": "LIB_OPENFHE", "poly_degree": 1 << 14,
                 "security_level": 0, "mul_depth": 10, "input_level": 2,
                 "first_prime_bit_num": 60, "scaling_factor_bit_num": 59,
                 "q_part_num": 2, "hamming_weight": 192, "rotate_keys": [],
                 "bootstrap": True})
    assert F._BOOTSTRAP_READY is True
    cc = F._CC
    depth = cc.GetMultiplicativeDepth()
    num_slots = cc.GetRingDimension() // 2

    # Encode a small message near the bottom of the level chain (depth-1) and
    # encrypt directly via the fhert context. Wrap as an rt_fhert Cipher.
    x = [0.25, 0.5, 0.75, 1.0, 2.0, 3.0, 4.0, 5.0]
    pt = cc.MakeCKKSPackedPlaintext(x, 1, depth - 1)
    pt.SetLength(len(x))
    ct = cc.Encrypt(F._PK, pt)
    a = F.Cipher._wrap(ct, scale_deg=1, is_cipher3=False, n_logical=len(x))

    # The Phase-3 swap: a REAL EvalBootstrap behind the Phase-2.4 name.
    r = F.bootstrap_ciph(a, level=depth, slot=num_slots)

    # Fresh post-bootstrap scale + relined + non-negative on-demand level. The
    # post-bootstrap noiseScaleDeg is technique-dependent: 1 under FIXEDMANUAL
    # (after the ant-faithful nsd-reset), 2 under FLEXIBLEAUTOEXT (FLEX keeps nsd
    # at a constant 2 -- it auto-manages the real scale; the level, tracked via
    # the lazy bridge, is the meaningful quantity). Both are the canonical
    # post-bootstrap scale for their technique.
    assert r.is_cipher3 is False
    assert r.scale_deg == (2 if F._FLEX_MODE else 1)
    assert r.level == F._ctx_ir_capacity() - int(r._ct.GetLevel())
    assert r.level >= 0

    # The message is recovered within the bootstrap tolerance (lossy refresh).
    tol = float(os.environ.get("FHE_BTS_TOL", "1e-1"))
    recovered = list(r.slots)
    assert len(recovered) == len(x), \
        f"recovered {len(recovered)} slots, expected {len(x)}"
    max_abs = max(abs(got - exp) for got, exp in zip(recovered, x))
    assert max_abs < tol, \
        f"bootstrap recovery max_abs={max_abs:.3e} >= tol {tol:.0e}; " \
        f"recovered={[round(v, 4) for v in recovered]}"

    # Sanity: the bootstrapped result has more levels remaining than the
    # depleted input (the refresh worked -- it did not early-return). Under
    # FIXEDMANUAL a cipher at nsd n occupies n towers for its scale chain, so
    # the available headroom is depth - GetLevel - (nsd-1); under FLEXIBLEAUTOEXT
    # nsd is a constant 2 (FLEX auto-manages scale, no tower-chain for it), so
    # the headroom is just depth - GetLevel.
    if F._FLEX_MODE:
        levels_remaining = depth - r._ct.GetLevel()
    else:
        levels_remaining = depth - r._ct.GetLevel() - (r._ct.GetNoiseScaleDeg() - 1)
    assert levels_remaining > 1, \
        f"levels_remaining={levels_remaining} (bootstrap did not refresh up?)"

    # Report the measured precision (informational; not a gate).
    print(f"fhert bootstrap: ring={cc.GetRingDimension()} slots={num_slots} "
          f"depth={depth} max_abs={max_abs:.2e} levels_remaining="
          f"{levels_remaining}")
    F._reset()