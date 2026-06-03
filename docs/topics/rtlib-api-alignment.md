# RTLib API Alignment: Unifying Ciphertext Memory Management Across Providers

## Background

The FHE compiler (fhe-cmplr) generates C code via the IR-to-C (ir2c) pass. This generated code is linked against a provider-specific runtime library (rtlib) — either **antlib** (CPU) or **phantom** (GPU). Both providers implement the same FHE operations (add, multiply, rotate, etc.) but historically used different naming conventions and different ownership semantics for ciphertext memory management.

This mismatch forced the compiler to emit **provider-conditional code**, violating the principle that generated code should be provider-agnostic.

## Problems Before Alignment

### 1. Inconsistent Function Names

| Operation | antlib | phantom (before) |
|-----------|--------|------------------|
| Free ciphertext | `Free_cipher` | `Free_ciph` |
| Free plaintext | `Free_plain` (via `Free_plaintext`) | `Free_plain` |
| Free cipher array | `Free_ciph_poly` | `Free_ciph_array` |

The compiler's `Handle_free` had to emit different function names depending on the target provider, which required `if (ctx.Provider() == core::PROVIDER::ANT)` guards in the ir2c code.

### 2. Different Ownership Models in Set_output_data

**antlib** (`rtlib/ant/ckks/src/rtlib.c`):
```c
void Set_output_data(const char *name, size_t idx, CIPHER data) {
  CIPHER output = Alloc_ciphertext();
  Copy_ciph(output, data);
  Free_ciph_poly(data, 1);  // consumes input's poly data
  Io_set_output(name, idx, output);
}
```
`Set_output_data` copies the ciphertext and frees the input's poly data. No explicit `Free_cipher` call is needed after `Set_output_data`.

**phantom** (before, `rtlib/phantom/src/phantom_lib.cu`):
```cpp
void Set_output_data(const char *name, size_t idx, Ciphertext *ct) {
  Io_set_output(name, idx, new Ciphertext(*ct));  // deep copy only
}
```
`Set_output_data` deep-copied the ciphertext but did NOT free the input. This required the compiler to emit an explicit `Free_ciph(&var)` after `Set_output_data`, but only for phantom — another provider guard.

### 3. Deep Copies in Phantom Operations

Phantom's internal `Add`/`Mul` methods deep-copied operands before level alignment:
```cpp
void Add(Ciphertext *op1, Ciphertext *op2, Ciphertext *res) {
  Ciphertext final_op1 = *op1;  // deep copy
  Ciphertext final_op2 = *op2;  // deep copy
  Equal_level(final_op1, final_op2);
  _evaluator->evaluator.add(final_op1, final_op2, *res);
}
```
These copies were unnecessary overhead — the level-aligned copies are still needed (since `Equal_level` modifies operands in-place), but the parameter passing style (raw pointers with implicit copy semantics) was not idiomatic C++.

### 4. Memory Leaks

- `Get_input_data`: deep-copied the ciphertext, then called `Free_ciph` + `delete` — the `Free_ciph` was redundant since `delete` would trigger the destructor anyway, but the deep copy + free pattern was wasteful.
- `Handle_output`: called `Free_ciph(data)` but never `delete data` — the `Ciphertext` object allocated by `new` in `Set_output_data` was leaked.

## Changes

### Phase 1: Phantom Internal API — Pointer to Reference

Changed `PHANTOM_CONTEXT` internal methods from pointer parameters to reference parameters:

```cpp
// Before
void Add(Ciphertext *op1, Ciphertext *op2, Ciphertext *res);

// After
void Add(Ciphertext &res, const Ciphertext &op1, const Ciphertext &op2);
```

This applies to all evaluation methods: `Add`, `Mul`, `Rotate`, `Rescale`, `Mod_switch`, `Relin`, `Bootstrap`, `Free_cipher`, `Free_plain`, `Encode_float`, `Decrypt`, `Decode`, `Scale`, `Level`, etc.

The outer `Phantom_*` C-compatible functions keep their pointer interface and dereference to call the internal reference-based methods:
```cpp
void Phantom_add_ciph(CIPHER res, CIPHER op1, CIPHER op2) {
  PHANTOM_CONTEXT::Context()->Add(*res, *op1, *op2);
}
```

### Phase 2: Move Semantics for Ownership Transfer

Replaced deep copies with `std::move` where ownership transfer is intended:

**Set_output_data** — consumes input via move:
```cpp
void Set_output_data(const char *name, size_t idx, Ciphertext &ct) {
  Io_set_output(name, idx, new Ciphertext(std::move(ct)));
}
```
After this call, `ct` is in a moved-from state (size == 0). The caller should not use `ct` anymore, and no explicit `Free_cipher` is needed after `Set_output_data`.

**Get_input_data** — moves instead of copy + free:
```cpp
Ciphertext Get_input_data(const char *name, size_t idx) {
  Ciphertext *data = (Ciphertext *)Io_get_input(name, idx);
  Ciphertext ret = std::move(*data);
  delete data;
  return ret;
}
```

**Handle_output** — `delete` replaces `Free_ciph` (the `new Ciphertext` from `Set_output_data` needs `delete`):
```cpp
double *Handle_output(const char *name, size_t idx) {
  Ciphertext *data = (Ciphertext *)Io_get_output(name, idx);
  // ... decrypt and decode ...
  delete data;
  return msg;
}
```

**Phantom_add_ciph accumulation path** — move instead of copy:
```cpp
if (op1->size() == 0) {
  *res = std::move(*op2);
  return;
}
```

### Phase 3: Function Name Alignment

Aligned phantom's free-function naming with antlib:

| phantom (before) | phantom (after) | antlib |
|-------------------|------------------|--------|
| `Free_ciph` | `Free_cipher` | `Free_cipher` |
| `Free_plain` | `Free_plain` | `Free_plain` |
| `Free_ciph_array` | `Free_ciph_poly` | `Free_ciph_poly` |

This eliminates the need for provider-conditional code generation in the compiler.

Backward-compatible aliases are provided in `rt_phantom.h` for legacy generated code that still references the old names:
```cpp
inline void Free_ciph(CIPHER res) { Free_cipher(res); }
inline void Free_ciph_array(CIPHER res, size_t size) { Free_ciph_poly(res, size); }
```

### Phase 4: Remove Provider Guards in ir2c

With both providers now exposing the same function names and `Set_output_data` consuming its input, all provider-specific guards were removed:

- **`include/fhe/ckks/ir2c_handler.h`**: Removed `if (ctx.Provider() == core::PROVIDER::ANT) return;` in `Handle_free`. Changed emitted function names to `Free_cipher`/`Free_plain`/`Free_ciph_poly`.
- **`include/fhe/ckks/ir2c_core.h`**: Removed `Free_ciph` call and provider guard after `Set_output_data` in `Handle_retv`.
- **`include/fhe/poly/ir2c_core.h`**: Removed `Free_ciph` calls and provider guards after `Set_output_data` in `Handle_retv` (both single and batch output paths).

### Idempotent Free_cipher

Made phantom's `Free_cipher` idempotent to handle the case where a ciphertext has already been moved from:
```cpp
void Free_cipher(Ciphertext &ct) {
  if (ct.size() > 0) {
    ct.release();
  }
}
```
This ensures safety if `Free_cipher` is called on a moved-from ciphertext (size == 0 after `Set_output_data` consumed it).

## Why Free_cipher/Free_plain/Free_ciph_poly Are Still Generated

The compiler's **ckks2c_mfree** pass analyzes variable lifetimes and inserts FREE opcodes for intermediate ciphertext variables that are no longer needed. The ir2c `Handle_free` translates these FREE opcodes into C function calls:

- `Free_cipher(&var)` — for cipher/cipher3 type variables
- `Free_plain(&var)` — for plaintext type variables
- `Free_ciph_poly(&arr, n)` — for cipher array type variables

These calls are necessary for **phantom** to release GPU memory held by intermediate variables. For **antlib**, `Free_cipher` calls `Free_ciphertext` which releases CPU memory. Both providers benefit from explicit deallocation of intermediate variables.

The `Set_output_data` path no longer needs explicit free calls because the function now consumes its input via move semantics — the input ciphertext is left in a valid-but-empty state.

## Result

| Aspect | Before | After |
|--------|--------|-------|
| Compiler generates provider-conditional code | Yes (`if (provider != ANT)`) | No |
| Function names consistent across providers | No (`Free_ciph` vs `Free_cipher`) | Yes (`Free_cipher`) |
| Phantom Set_output_data leaks memory | Yes (no free of input) | No (move consumes input) |
| Phantom Handle_output leaks Ciphertext object | Yes (missing `delete`) | No (`delete data`) |
| Phantom Get_input_data unnecessary deep copy | Yes | No (uses `std::move`) |
| Phantom internal API style | C-style pointers, redundant returns | C++ references, void returns |

## Files Changed

| File | Change |
|------|--------|
| `rtlib/phantom/src/phantom_lib.cu` | Reference passing, move semantics, Free_cipher naming |
| `rtlib/include/rt_phantom/rt_phantom.h` | Free_cipher/Free_ciph_poly naming, backward-compatible aliases |
| `rtlib/include/rt_phantom/phantom_api.h` | Phantom_free_cipher/Phantom_free_ciph_poly naming |
| `include/fhe/ckks/ir2c_handler.h` | Remove provider guard, emit Free_cipher/Free_ciph_poly |
| `include/fhe/ckks/ir2c_core.h` | Remove Free_ciph after Set_output_data |
| `include/fhe/poly/ir2c_core.h` | Remove Free_ciph after Set_output_data |