"""Phase 3b: a REAL OpenFHE CKKS ``EvalBootstrap`` smoke test for
`rtlib_py.rt_openfhe`.

Builds the toy Set-A bootstrap context (ring 2^12, level_budget [4,4],
UNIFORM_TERNARY, FIXEDMANUAL per decision #2; mirrors
examples/pke/simple-ckks-bootstrapping.py), encodes a small message near the
top of the level chain, calls ``bootstrap_ciph(a, level, slot)`` -- the single
Phase-2.4 name behind which Phase 3 swaps a real bootstrap -- and asserts the
message is recovered within 1e-2 (bootstrap is lossy) with a fresh
``scale_deg=1`` and ``is_cipher3=False``.

Slow (~6s on a 64-bit native). The whole module skips when `import openfhe`
fails (pytest.importorskip; ctest SKIP_RETURN_CODE 77). Set
``OPENFHE_RUN_BOOTSTRAP=0`` to opt out (a CI-flakiness escape hatch; the test
runs by default so the 3b gate is a real gate). Run:
``OPENFHE_RUN_BOOTSTRAP=1 python3 -m pytest test_openfhe_bootstrap.py -q``.
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

_OPENFHE_BUILD_DIR = os.environ.get("OPENFHE_PY_BUILD_DIR") or \
    "/workspace/openfhe-python/build"
if os.path.isdir(_OPENFHE_BUILD_DIR) and _OPENFHE_BUILD_DIR not in sys.path:
    sys.path.insert(0, _OPENFHE_BUILD_DIR)

# Skip the whole module on a missing binding (skip-77 philosophy).
pytest.importorskip("openfhe")

# Opt-out escape hatch for CI flakiness (the test runs by default).
if os.environ.get("OPENFHE_RUN_BOOTSTRAP", "1") == "0":
    pytest.skip("OPENFHE_RUN_BOOTSTRAP=0 -> skipping the real-bootstrap smoke "
                "test", allow_module_level=True)

import rtlib_py.rt_openfhe as F  # noqa: E402


def test_real_evalbootstrap_recovers_message():
    # Seed the toy Set-A bootstrap context. _build_context sees 'bootstrap':
    # True and calls _build_bootstrap_context (ring 2^12, FIXEDMANUAL, level
    # budget [4,4]); the green-gate toy params are ignored (too small to
    # bootstrap).
    F._reset()
    F._seed_ctx({"provider": "LIB_OPENFHE", "poly_degree": 4096,
                 "security_level": 0, "mul_depth": 10, "input_level": 2,
                 "first_prime_bit_num": 60, "scaling_factor_bit_num": 59,
                 "q_part_num": 2, "hamming_weight": 192, "rotate_keys": [],
                 "bootstrap": True})
    assert F._BOOTSTRAP_READY is True
    cc = F._CC
    depth = cc.GetMultiplicativeDepth()
    num_slots = cc.GetRingDimension() // 2

    # Encode a small message near the top of the level chain (depth-1), mirror
    # the upstream example, and encrypt it directly via the openfhe context.
    x = [0.25, 0.5, 0.75, 1.0, 2.0, 3.0, 4.0, 5.0]
    pt = cc.MakeCKKSPackedPlaintext(x, 1, depth - 1)
    pt.SetLength(len(x))
    ct = cc.Encrypt(F._PK, pt)
    # Wrap it as an rt_openfhe Cipher (bootstrap_ciph does not read a.level;
    # n_logical is preserved onto the result). tracked level/scale reflect a
    # near-top input.
    a = F.Cipher._wrap(ct, scale_deg=1, is_cipher3=False, n_logical=len(x))

    # The Phase-3 swap: a REAL EvalBootstrap behind the Phase-2.4 name.
    r = F.bootstrap_ciph(a, level=depth, slot=num_slots)

    # Fresh post-bootstrap scale + relined. Under the lazy bridge ``r.level`` is
    # on demand (``_ctx_ir_capacity() - ct.GetLevel()``) and ALWAYS >= 0 -- the
    # structural drift fix (the eager bridge produced ``r.level == -10`` here,
    # pinning the IR target while the cipher sat deeper). The IR target
    # ``level=depth`` (near the top) is ABOVE the post-bootstrap landing, so the
    # droplevel is skipped; ``r.level`` reflects the ACTUAL remaining headroom
    # rather than the unreachable target. (The basis is ``ir_capacity`` -- the
    # IR-mapping capacity, which is the TRUE chain minus the R down-headroom
    # reserve -- not ``_ctx_num_primes``; the two differ when R>0.)
    assert r.is_cipher3 is False
    assert r.scale_deg == 1
    assert r.level == F._ctx_ir_capacity() - int(r._ct.GetLevel())
    assert r.level >= 0
    # The message is recovered within the bootstrap tolerance (lossy refresh).
    recovered = list(r.slots)
    assert len(recovered) == len(x)
    for i, (got, exp) in enumerate(zip(recovered, x)):
        assert abs(got - exp) < 1e-2, \
            f"slot {i}: {got} not within 1e-2 of {exp}"

    # Sanity: a cipher that did NOT bootstrap would sit at a much lower level;
    # the bootstrapped result has many levels remaining (the refresh worked).
    levels_remaining = depth - r._ct.GetLevel() - (r._ct.GetNoiseScaleDeg() - 1)
    assert levels_remaining > 1
    F._reset()