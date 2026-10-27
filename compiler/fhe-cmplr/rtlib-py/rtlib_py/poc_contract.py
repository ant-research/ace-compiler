"""F1.1 POC -- rtlib-py contract round-trip proof-of-concept.

Gap-filler: this POC never existed in the original PR (the path jumped from
nothing to a 12k-LOC implementation). It proves the frozen common-contract
surface (common/contract.py) round-trips end-to-end against a hand-built stub
provider:

  1. builds a stub provider module implementing all 36 functional
     CONTRACT_NAMES as plaintext no-ops/minimal (add_ciph computes the
     element-wise slot sum);
  2. runs check_provider(stub) to assert the full 39-name contract surface;
  3. runs one add_ciph end-to-end and asserts the result
     (slots [1.0, 2.0] + [3.0, 4.0] -> [4.0, 6.0]);
  4. imports only rtlib_py.common.contract (the F0.3 layer); it does NOT
     import rt_ref / rt_openfhe / rt_fhert -- those providers don't exist
     yet at F1.1, and the POC deliberately depends only on the contract.

Run:
  PYTHONPATH=rtlib-py python3 rtlib-py/rtlib_py/poc_contract.py
exits 0 and prints the round-trip assertion pass on success.
"""

from types import ModuleType

from rtlib_py.common.contract import (
    CONTRACT_NAMES,
    PUBLIC_API,
    PUBLIC_TYPES,
    check_provider,
)


class FakeRtError(Exception):
    """Minimal error class standing in for a provider's FakeRtError type."""


class _SlotCarrier:
    """Minimal plaintext slot carrier shared by Cipher and Plain.

    A real provider's Cipher is a CKKS ciphertext; the POC uses a plain list
    of slot values so add_ciph is a trivial element-wise sum.
    """

    __slots__ = ("slots", "level", "scale_deg")

    def __init__(self, slots, level=0, scale_deg=1):
        self.slots = list(slots)
        self.level = level
        self.scale_deg = scale_deg

    def __repr__(self):
        return (
            f"{type(self).__name__}(slots={self.slots}, level={self.level}, "
            f"scale_deg={self.scale_deg})"
        )


class Cipher(_SlotCarrier):
    pass


class Plain(_SlotCarrier):
    pass


def _add_ciph(a, b):
    """Slot-wise plaintext add: result slots = a.slots + b.slots.

    Mirrors the contract intent (Cipher + Cipher -> Cipher) without any real
    HE noise. Level and scale_deg track the max of the inputs (plaintext add
    does not consume modulus depth).
    """
    if len(a.slots) != len(b.slots):
        raise FakeRtError(
            f"add_ciph slot count mismatch: {len(a.slots)} != {len(b.slots)}")
    out = [a.slots[i] + b.slots[i] for i in range(len(a.slots))]
    return Cipher(out, level=max(a.level, b.level),
                  scale_deg=max(a.scale_deg, b.scale_deg))


def _mul_ciph(a, b):
    """Slot-wise plaintext mul: result slots = a.slots * b.slots."""
    if len(a.slots) != len(b.slots):
        raise FakeRtError(
            f"mul_ciph slot count mismatch: {len(a.slots)} != {len(b.slots)}")
    out = [a.slots[i] * b.slots[i] for i in range(len(a.slots))]
    # plaintext mul consumes one scale degree and one level, like CKKS mul
    return Cipher(out, level=a.level + b.level,
                  scale_deg=a.scale_deg + b.scale_deg)


def _build_stub_provider():
    """Assemble a stub module exporting the full 39-name CONTRACT_NAMES."""
    stub = ModuleType("stub_provider")
    stub.Cipher = Cipher
    stub.Plain = Plain
    stub.FakeRtError = FakeRtError
    stub.add_ciph = _add_ciph
    stub.mul_ciph = _mul_ciph
    # The remaining 34 functional names are no-ops for the POC -- check_provider
    # only asserts that the attribute is present, so a returning-None lambda
    # satisfies the surface. The POC exercises only add_ciph end-to-end.
    for name in PUBLIC_API:
        if name in ("add_ciph", "mul_ciph"):
            continue
        setattr(stub, name, lambda *a, **k: None)
    return stub


def main():
    # 1. Assert the contract surface counts (36 functional + 3 types = 39).
    assert len(PUBLIC_API) == 36, f"PUBLIC_API expected 36, got {len(PUBLIC_API)}"
    assert len(PUBLIC_TYPES) == 3, f"PUBLIC_TYPES expected 3, got {len(PUBLIC_TYPES)}"
    assert len(CONTRACT_NAMES) == 39, (
        f"CONTRACT_NAMES expected 39, got {len(CONTRACT_NAMES)}")

    # 2. Assert the stub provider exports the full frozen contract surface.
    stub = _build_stub_provider()
    check_provider(stub)  # raises AttributeError if any name is missing

    # 3. Run one add_ciph through the stub and assert the result.
    a = Cipher([1.0, 2.0], level=0, scale_deg=1)
    b = Cipher([3.0, 4.0], level=0, scale_deg=1)
    r = stub.add_ciph(a, b)
    assert list(r.slots) == [4.0, 6.0], f"slots mismatch: {r.slots}"
    assert r.scale_deg == 1, f"scale_deg mismatch: {r.scale_deg}"
    assert r.level == 0, f"level mismatch: {r.level}"

    print("POC contract round-trip OK: add_ciph [1,2]+[3,4] -> [4,6]")


if __name__ == "__main__":
    main()
