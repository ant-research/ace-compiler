"""The frozen public runtime contract for rtlib-py providers.

This mirrors the C rtlib weak-symbol contract (`rtlib/include/common/rt_api.h`)
as a Python surface: the names an emitted `-CKKS2PY` module may call after
`from rtlib_py.<provider> import *`. The contract is FROZEN (Phase 2.0) -- a
provider must export every name in `CONTRACT_NAMES` and honor these signatures.
Adding a runtime capability means extending BOTH the emitter (an IR2PY_HANDLER
call) AND this contract; Phase 2.1-2.4 extend it (validate/tm_*/dump_*,
bootstrap_ciph, ...).

Python has no weak symbols, so the contract is a `typing.Protocol` (documentary
+ runtime-checkable) plus the `CONTRACT_NAMES` tuple the registry validates a
provider against at registration. The emitted module defines its OWN
`Main_graph` / `get_context_params` / `get_input_count` / `get_output_count`
(mirroring the C `__attribute__((weak))` generated functions), so those are
NOT part of the provider contract -- they live in the emitted module.

NOTE (gotcha d, decision #2): the C-side `.conf` JSON keys
(`_provider`(int)/`_num_q_parts`/`_first_mod_size`/...; `lower_conf.h`) are
DIFFERENT from the `get_context_params()` dict keys
(`provider`(str)/`q_part_num`/`first_prime_bit_num`/...; `poly2py_driver.cxx`).
`rt_ref` reads the dict; the `.conf` is parity-only (see common/conf.py).
"""

from typing import Any, Protocol, runtime_checkable

# The functional runtime calls the emitter may emit. Phase 2.0 froze the 30
# arithmetic/IO/encode/query calls; Phase 2.1 adds the 4 observability/metadata
# calls (validate/tm_start/tm_taken/dump_cipher_msg) = 34; Phase 2.3 adds
# `load_data_entry` (offline `-CKKS2PY:df=` data-file read) = 35; Phase 2.4 adds
# `bootstrap_ciph` (plaintext near-identity bootstrap, mirror C non-ANT collapse)
# = 36. Total = 36.
PUBLIC_API = (
    # IO
    "get_input_data", "set_output_data",
    # arithmetic: add / sub  (ciph / plain / scalar)
    "add_ciph", "add_plain", "add_scalar",
    "sub_ciph", "sub_plain", "sub_scalar",
    # arithmetic: mul  (ciph / plain / scalar / mono)
    "mul_ciph", "mul_plain", "mul_scalar", "mul_mono_ciph",
    # unary / shape
    "neg_ciph", "rotate_ciph", "conjugate_ciph",
    # modulus management
    "relin", "rescale_ciph", "mod_switch", "upscale_ciph", "raise_mod",
    "bootstrap_ciph",  # bootstrap_ciph(a, level, slot)  [Phase 2.4 near-identity]
    # lifecycle
    "copy_ciph", "zero_ciph",
    "free_ciph", "free_plain", "free_ciph_poly",
    # encode / queries
    "encode_float", "encode_double", "encode_dcmplx",
    "load_data_entry",  # load_data_entry(idx, n)  [Phase 2.3 offline -CKKS2PY:df=]
    "level", "sc_degree",
    # observability / metadata (Phase 2.1)
    "validate",         # validate(ciph, msg, len, eps)  [mirror C Validate]
    "tm_start",         # tm_start(label)               [no-op; C Tm_start]
    "tm_taken",         # tm_taken(label)               [no-op; C Tm_taken]
    "dump_cipher_msg",  # dump_cipher_msg(name, ciph, len) [C Dump_cipher_msg]
)

# Value types + the provider's error class.
PUBLIC_TYPES = ("Cipher", "Plain", "FakeRtError")

# The complete frozen contract: a provider exporting `*` must bind all of these.
CONTRACT_NAMES = PUBLIC_API + PUBLIC_TYPES


@runtime_checkable
class RuntimeProvider(Protocol):
    """Documentary Protocol for an rtlib-py provider module.

    Phase 2.0 = 30 functional calls + `Cipher`/`Plain`/`FakeRtError`. Phase 2.1
    adds the 4 observability/metadata calls (`validate`/`tm_start`/`tm_taken`/
    `dump_cipher_msg`); 2.3 adds `load_data_entry` (offline `-CKKS2PY:df=`); 2.4
    adds `bootstrap_ciph`. Signatures follow the C rtlib semantics (return-value
    style; C `&dst` out-params become return values). Only the names are enforced
    (via CONTRACT_NAMES); the Protocol records the intent.
    """

    # value types
    Cipher: Any
    Plain: Any
    FakeRtError: Any
    # IO
    def get_input_data(self, name: str, idx: int) -> Any: ...
    def set_output_data(self, name: str, idx: int, val: Any) -> None: ...
    # arithmetic (representative; see PUBLIC_API for the full set)
    def add_ciph(self, a: Any, b: Any) -> Any: ...
    def mul_ciph(self, a: Any, b: Any) -> Any: ...
    def rescale_ciph(self, a: Any) -> Any: ...
    def relin(self, a: Any) -> Any: ...
    def bootstrap_ciph(self, a: Any, level: int, slot: int) -> Any: ...  # 2.4
    # encode / queries
    def encode_float(self, buf: Any, n: int, scale: Any, level: int) -> Any: ...
    def load_data_entry(self, idx: int, n: int) -> Any: ...  # Phase 2.3 offline
    def level(self, a: Any) -> int: ...
    def sc_degree(self, a: Any) -> int: ...
    # observability / metadata (Phase 2.1)
    def validate(self, ciph: Any, msg: Any, length: int, epsilon: int) -> None: ...
    def tm_start(self, label: str) -> None: ...
    def tm_taken(self, label: str) -> None: ...
    def dump_cipher_msg(self, name: str, ciph: Any, length: int) -> None: ...


def check_provider(mod) -> None:
    """Assert `mod` exports every frozen contract name. Called by the registry
    on (lazy) registration so a missing capability fails loudly at the first
    use rather than as a mid-execution NameError in the emitted module."""
    missing = [n for n in CONTRACT_NAMES if not hasattr(mod, n)]
    if missing:
        raise AttributeError(
            f"{mod.__name__} is not a conforming rtlib-py provider: missing "
            f"{missing} (frozen contract: rtlib_py.common.contract)")


__all__ = ["PUBLIC_API", "PUBLIC_TYPES", "CONTRACT_NAMES",
           "RuntimeProvider", "check_provider"]