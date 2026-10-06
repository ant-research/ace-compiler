"""`.conf` JSON parser -- parity with the C path (`fhe/core/lower_conf.h`).

PLACEHOLDER (Phase 2.0, decision #2): `rt_ref` does NOT consume the `.conf` --
it reads the emitted module's `get_context_params()` dict directly. This parser
exists only to mirror the C-side `.conf` face and keep the key-mismatch gotcha
documented in one place; it is currently unused (`rt_ref` reads the dict the
emitted module already returns).

GOTCHA (d): the `.conf` JSON keys (underscore-prefixed, provider as INT) and
the `get_context_params()` dict keys (no underscore, provider as STR) DIFFER:

    .conf (lower_conf.h:81-92)        get_context_params() dict (poly2py_driver.cxx)
    -------------------------------   -------------------------------------------
    _provider        (int, 0..4)      provider        (str, 'LIB_OPENFHE' / ...)
    _num_q_parts                      q_part_num
    _input_level                      input_level
    _mul_depth                        mul_depth
    _poly_degree                      poly_degree
    _first_mod_size                   first_prime_bit_num
    _scaling_mod_size                 scaling_factor_bit_num
    _sec_level                        security_level
    _hamming_weight                   hamming_weight
    _num_rot_idx / _rot_idxs          rotate_keys

Provider int<->str (ALL_PROVIDERS order, lib_provider.h): 0=ANT, 1=SEAL,
2=OPENFHE, 3=PHANTOM, 4=ACE  <->  LIB_ANT/LIB_SEAL/LIB_OPENFHE/LIB_PHANTOM/
LIB_ACE. `rt_ref` reads the dict (provider str); the `.conf` is parity-only.
"""

import json

# ALL_PROVIDERS() order (lib_provider.h:18-23) -- int id <-> C name string.
_PROVIDER_INT_TO_NAME = ("LIB_ANT", "LIB_SEAL", "LIB_OPENFHE", "LIB_PHANTOM",
                         "LIB_ACE")


def parse_conf(path):
    """Parse a fhe-cmplr `.conf` JSON file and return the raw dict (the on-disk
    form, underscore-prefixed keys). Caller-normalization is intentionally NOT
    done here -- `rt_ref` does not consume the `.conf` (decision #2)."""
    with open(path, "r") as f:
        return json.load(f)


def conf_to_ctx_params(conf):
    """Translate a parsed `.conf` into the `get_context_params()` dict shape
    (`rt_ref` reads). Documents the key mismatch (gotcha d); currently unused
    -- `rt_ref` reads the dict the emitted module already returns."""
    cp = conf.get("context_params", {})
    prov_int = cp.get("_provider", 0)
    prov_name = _PROVIDER_INT_TO_NAME[prov_int] if 0 <= prov_int < len(
        _PROVIDER_INT_TO_NAME) else "LIB_ANT"
    return {
        "provider": prov_name,
        "poly_degree": cp.get("_poly_degree", 0),
        "security_level": cp.get("_sec_level", 0),
        "mul_depth": cp.get("_mul_depth", 0),
        "input_level": cp.get("_input_level", 0),
        "first_prime_bit_num": cp.get("_first_mod_size", 0),
        "scaling_factor_bit_num": cp.get("_scaling_mod_size", 0),
        "q_part_num": cp.get("_num_q_parts", 0),
        "hamming_weight": cp.get("_hamming_weight", 0),
        "rotate_keys": list(cp.get("_rot_idxs", [])),
    }


__all__ = ["parse_conf", "conf_to_ctx_params"]