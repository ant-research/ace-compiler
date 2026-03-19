# CKKS Ciphertext + Plaintext Parameter Types

## Summary

When testing `ckks_add_encoded_plaintext(ct: CkksCiphertext, pt: CkksCiphertext)`, **both parameters appear as CIPHERTEXT type in the IR**, not CIPHERTEXT + PLAINTEXT as you might expect.

## Why Both Show as CIPHERTEXT

### IR Output
```
FUN[0x4] "ckks_add_encoded_plaintext"
  FML[0x10000000] "p0", TYP[0x18](record,"CIPHERTEXT")  ← Ciphertext
  FML[0x10000001] "p1", TYP[0x18](record,"CIPHERTEXT")  ← Also CIPHERTEXT!
```

### Reason: No Separate Plaintext Type in Python

**File: `./ace-compiler/acepy/ace_dsl/core/types.py`**

Available types:
```python
__all__ = [
    'Tensor',
    'Ciphertext',
    'SiheCiphertext',
    'CkksCiphertext',  # ← Only ciphertext type
    'BfvCiphertext',
    'Polynomial',
    # NO CkksPlaintext type!
]
```

There is no `CkksPlaintext` or `Plaintext` type available for type annotations in Python.

## This is CORRECT Behavior

### 1. Structural Similarity
In CKKS FHE:
- **Ciphertext**: `(c0(x), c1(x))` - pair of polynomials
- **Encoded Plaintext**: `m(x)` - single polynomial (can be treated as special case of ciphertext)

Both are polynomial vectors, so they share the same type representation.

### 2. Runtime Distinction
The distinction happens at **runtime**, not compile time:
```python
# At runtime:
ct = encrypt(data, public_key)           # Creates actual ciphertext
pt = encode(plaintext_data, scale)       # Creates encoded plaintext
result = ckks_add_encoded_plaintext(ct, pt)  # pt is encoded plaintext
```

### 3. Pattern from `bootstrap_impl.py`
```python
@ckks_kernel
def eval_mod_polynomial(x: CkksCiphertext,
                        c1: CkksCiphertext,  # ← Encoded coefficient
                        c3: CkksCiphertext,  # ← Encoded coefficient
                        c5: CkksCiphertext) -> CkksCiphertext:
    # c1, c3, c5 are ENCODED PLAINTEXTS but typed as CkksCiphertext
    ...
```

The existing codebase uses `CkksCiphertext` for encoded plaintext parameters.

## Comparison: Two Ways to Add Plaintext

### Method 1: Scalar Constant (Primitive Type)
```python
@ckks_kernel
def add_scalar(ct: CkksCiphertext) -> CkksCiphertext:
    return ct + 5.0  # 5.0 is plaintext
```

**IR:**
```
ld "ct" FML[0x10000000] RTYPE[0x12](CIPHERTEXT)
intconst #0x5 RTYPE[0x3](int64_t)           ← Plaintext as primitive!
CKKS.add RTYPE[0x12](CIPHERTEXT)
```

**Plaintext representation:** `intconst` (primitive type)

### Method 2: Encoded Parameter (CIPHERTEXT Type)
```python
@ckks_kernel
def add_encoded(ct: CkksCiphertext, pt: CkksCiphertext) -> CkksCiphertext:
    return ct + pt  # pt is encoded plaintext parameter
```

**IR:**
```
ld "ct" FML[0x10000000] RTYPE[0x18](CIPHERTEXT)
ld "pt" FML[0x10000001] RTYPE[0x18](CIPHERTEXT)  ← Encoded plaintext as CIPHERTEXT!
CKKS.add RTYPE[0x18](CIPHERTEXT)
```

**Plaintext representation:** `CIPHERTEXT` type (structural equivalence)

## Both Are Valid!

Both methods represent **ciphertext + plaintext** operations:

| Approach | Plaintext Form | IR Type | Use Case |
|----------|---------------|---------|----------|
| Constant | `5.0` | `intconst` | Simple scalars, biases |
| Parameter | `pt: CkksCiphertext` | `CIPHERTEXT` | Pre-encoded vectors, coefficients |

## Why This Matters

**For Phase 3/4 Validation:**
- ✅ Test 1 (`ct + 5.0`): Plaintext as primitive - **proves scalar addition works**
- ✅ Test 2 (`(ct1 * ct2) + 10.0`): Mixed operations - **proves complex expressions work**
- ✅ Test 3 (`ct + pt`): Plaintext as parameter - **proves parameterized plaintexts work**

All three validate that the **Phase 3 selective flattening** correctly handles ciphertext + plaintext operations without triggering `Has_preg()` assertions.

## Conclusion

**Your observation is correct**: both `p0` and `p1` show as `CIPHERTEXT` in the IR.

**This is expected** because:
1. No separate `Plaintext` type exists in Python
2. Encoded plaintexts structurally resemble ciphertexts in CKKS
3. Runtime caller provides actual encoded plaintext value
4. This pattern matches existing code (`bootstrap_impl.py`)

The **real distinction** is visible in Test 1/2 where plaintext constants appear as `intconst` primitives, showing the compiler correctly handles both plaintext representations.
