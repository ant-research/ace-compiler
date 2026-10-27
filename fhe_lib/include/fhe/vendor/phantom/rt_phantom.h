#ifndef RTLIB_RT_PHANTOM_RT_PHANTOM_H
#define RTLIB_RT_PHANTOM_RT_PHANTOM_H

//! @brief rt_phantom.h
//! Compiler dispatch header for the Phantom GPU backend.
//!
//! Compiler-generated .c files include this header and call functions like
//! Add_ciph(), Encode_float(), etc. Each inline function here dispatches
//! directly to FheContext<VendorTraits>::Current() which reads a
//! thread-local pointer — this replaces the old singleton ::Context().
//!
//! Before: .c → Add_ciph → Phantom_add_ciph → PHANTOM_CONTEXT::Add  (3 hops)
//! After:  .c → Add_ciph → FheContext::Current().Add()              (2 hops)

#include "common/io_api.h"
#include "common/pt_mgr.h"
#include "common/rt_api.h"
#include "fhe/context.h"
#include "fhe/vendor/phantom/vendor_traits.h"

// SCALE_T / LEVEL_T — same as in old phantom_api.h
typedef double SCALE_T;
typedef size_t LEVEL_T;

// Shortcut: the FheContext singleton for Phantom
using FHertPhantomContext = fhert::FheContext<phantom::VendorTraits>;

//! @brief Get polynomial degree
inline uint32_t Degree()
{
    return Get_context_params()->_poly_degree;
}

//! @brief Get input cipher by name and index
inline CIPHERTEXT Get_input_data(const char* name, size_t idx)
{
    auto result = FHertPhantomContext::Current().Get_input_data(name, idx);
    return result;
}

//! @brief Set output cipher by name and index
inline void Set_output_data(const char* name, size_t idx, CIPHER data)
{
    // Delegate to Io API — kept for backward compat
    Io_set_output(name, idx, new PhantomCiphertext(std::move(*data)));
}

//! @brief Encode float array into plaintext.
//! The trailing `is_ext` (extended-basis flag, emitted by the mdown-sinking
//! pass) is accepted for ABI parity with the compiler emit (ir2c_ctx.h
//! ENCODE_CHILD::EXT) but not forwarded — the phantom backend's
//! CKKSEncoder::encode chain does not take an extended-basis selection, and
//! phantom runs the ckks codegen path that the mdown-sinking pass does not
//! promote on. Mirrors rtlib ant Encode_float_mask handling.
inline void Encode_float(PLAIN plain, float* input, size_t len, SCALE_T sc_degree, LEVEL_T level, bool is_ext = false)
{
    (void)is_ext;
    FHertPhantomContext::Current().Encode_float(*plain, input, len, sc_degree, level);
}

inline void Encode_float_cst_lvl(PLAIN plain, float* input, size_t len, SCALE_T sc_degree, int level,
                                 bool is_ext = false)
{
    (void)is_ext;
    FHertPhantomContext::Current().Encode_float_cst_lvl(*plain, input, len, sc_degree, level);
}

inline void Encode_float_mask(PLAIN plain, float input, size_t len, SCALE_T sc_degree, LEVEL_T level,
                              bool is_ext = false)
{
    (void)is_ext;
    FHertPhantomContext::Current().Encode_float_mask(*plain, input, len, sc_degree, level);
}

inline void Encode_float_mask_cst_lvl(PLAIN plain, float input, size_t len, SCALE_T sc_degree, int level,
                                      bool is_ext = false)
{
    (void)is_ext;
    FHertPhantomContext::Current().Encode_float_mask_cst_lvl(*plain, input, len, sc_degree, level);
}

// HE Operations — dispatch directly to FheContext methods

inline CIPHER Add_scalar(CIPHER res, CIPHER op, double op2)
{
    FHertPhantomContext::Current().Add_const(*res, *op, op2);
    return res;
}

inline CIPHER Add_ciph(CIPHER res, CIPHER op1, CIPHER op2)
{
    FHertPhantomContext::Current().Add(*res, *op1, *op2);
    return res;
}

inline CIPHER Add_plain(CIPHER res, CIPHER op1, PLAIN op2)
{
    FHertPhantomContext::Current().Add(*res, *op1, *op2);
    return res;
}

inline CIPHER Mul_ciph(CIPHER res, CIPHER op1, CIPHER op2)
{
    FHertPhantomContext::Current().Mul(*res, *op1, *op2);
    return res;
}

inline CIPHER Mul_scalar(CIPHER res, CIPHER op1, double op2)
{
    FHertPhantomContext::Current().Mul_const(*res, *op1, op2);
    return res;
}

inline CIPHER Mul_plain(CIPHER res, CIPHER op1, PLAIN op2)
{
    FHertPhantomContext::Current().Mul(*res, *op1, *op2);
    return res;
}

inline void Load_galois_key(int step)
{
    // Explicit per-rotate galois-key prefetch point (stage-2 codegen emits this
    // before Rotate_ciph). Idempotent + no-op unless KeyMode::ON_DEMAND(_FULL).
    // Stage-1 path (Rotate_ciph -> FheContext::Rotate) also ensures; this is a
    // direct call site the codegen can hoist/move for prefetch-timing in stage-2.
    FHertPhantomContext::Current().Ensure_galois_key(step);
}

inline void Free_galois_key(int step)
{
    // Stage-3: codegen emits this at a rotate's LAST use (liveness-driven, the
    // key won't be needed again) so the slot's HBM is reclaimed before the next
    // key loads — resident-set tracks the program's live set, not full-set.
    // No-op unless on-demand. Mirror of Load_galois_key.
    FHertPhantomContext::Current().Evict_galois_key(step);
}

inline CIPHER Rotate_ciph(CIPHER res, CIPHER op, int step)
{
    FHertPhantomContext::Current().Rotate(*res, *op, step);
    return res;
}

inline CIPHER Rescale_ciph(CIPHER res, CIPHER op)
{
    FHertPhantomContext::Current().Rescale(*res, *op);
    return res;
}

inline CIPHER Mod_switch(CIPHER res, CIPHER op)
{
    FHertPhantomContext::Current().Mod_switch(*res, *op);
    return res;
}

inline CIPHER Relin(CIPHER res, CIPHER3 ciph)
{
    FHertPhantomContext::Current().Relin(*res, *ciph);
    return res;
}

inline CIPHER Bootstrap(CIPHER res, CIPHER op, int level, int slot)
{
    FHertPhantomContext::Current().Bootstrap(*res, *op, level, slot);
    return res;
}

inline void Copy_ciph(CIPHER res, CIPHER op)
{
    if (res != op) {
        *res = *op;
    }
}

inline void Zero_ciph(CIPHER res)
{
    res->zero_ciph();
}

inline SCALE_T Sc_degree(CIPHER ct)
{
    auto result = FHertPhantomContext::Current().Scale(*ct);
    return result;
}

inline LEVEL_T Level(CIPHER ct)
{
    auto result = FHertPhantomContext::Current().Level(*ct);
    return result;
}

inline void Free_cipher(CIPHER res)
{
    FHertPhantomContext::Current().Free_cipher(*res);
}

inline void Free_plain(PLAIN res)
{
    FHertPhantomContext::Current().Free_plain(*res);
}

inline void Free_ciph_poly(CIPHER res, size_t size)
{
    for (size_t i = 0; i < size; ++i) {
        res[i].release();
    }
}

// Backward-compatible aliases for legacy generated code
inline void Free_ciph(CIPHER res)
{
    Free_cipher(res);
}
inline void Free_ciph_array(CIPHER res, size_t size)
{
    Free_ciph_poly(res, size);
}

// Dump utilities — kept for backward compatibility
void Dump_ciph(CIPHER ct, size_t start, size_t len);
void Dump_plain(PLAIN pt, size_t start, size_t len);
void Dump_cipher_msg(const char* name, CIPHER ct, uint32_t len);
void Dump_plain_msg(const char* name, PLAIN pt, uint32_t len);
double* Get_msg(CIPHER ct);
double* Get_msg_from_plain(PLAIN pt);

inline uint32_t Get_ciph_slots(CIPHER ct)
{
    return Degree() / 2;
}
inline uint32_t Get_plain_slots(PLAIN pt)
{
    return Degree() / 2;
}

bool Within_value_range(CIPHER ciph, double* msg, uint32_t len);

// KMS key management is NOT dispatched here. Codegen does not emit Save_keys
// / Load_keys; KMS runs through the Kms_* kernel-runner dlsym contract
// (fhe/kms/kms_runtime_api.h) → fhert::SaveKeys<Backend> / LoadKeys<Backend>
// (fhe/kms/kms_api.h). The Save_keys/Load_keys inline wrappers that used to
// live here duplicated that path; removed.

// Per-HE-op profiling is NOT done here. The runtime-wide profiling layer
// (fhe_dsl/csrc/runtime/include/ace/runtime/fhe_profiling.h: FHE_PROFILE_SCOPE)
// is vendor-agnostic and visible in torch.profiler / nsys; it currently covers
// runner phases (fhe::execute, ...) — extending it to per-op slices belongs on
// that layer, not a per-vendor chrono table here. The phantom-specific
// operation_stats / START_TIMER / END_TIMER / Print_performance_stats that
// lived here was dead (no caller, no ENABLE_PERFORMANCE_STATS entry point) and
// has been removed.

// for validate APIs
#include "common/rt_validate.h"

#endif  // RTLIB_RT_PHANTOM_RT_PHANTOM_H
