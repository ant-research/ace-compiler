//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_RT_ACE_RT_ACE_H
#define RTLIB_RT_ACE_RT_ACE_H

//! @brief rt_ace.h
//! ACE-specific runtime wrapper.  Both libFHErt_ace.so and the
//! generated kernel .cu are compiled against the real SDK headers, so
//! acelib::DeviceCipher / DevicePlain are the real types everywhere.
//!
//! After the common-layer refactoring, all HE dispatch goes directly to
//! FheContext<VendorTraits> methods — there are NO AceLibrary_* free
//! function intermediaries.

#include "common/io_api.h"
#include "common/pt_mgr.h"
#include "common/rt_api.h"
#include "fhe/context.h"
#include "fhe/need_bts.h"
#include "fhe/vendor/ace/vendor_traits.h"

// SCALE_T / LEVEL_T — defined here in vendor_traits.h (folded from rt_def.h)

// Shortcut: the FheContext singleton for ACE
using FHertAceContext = fhert::FheContext<ace::VendorTraits>;

//! @brief Get polynomial degree
inline uint32_t Degree()
{
    return Get_context_params()->_poly_degree;
}

//! @brief Get input cipher by name and index (returns by value, like phantom)
inline CIPHERTEXT Get_input_data(const char* name, size_t idx)
{
    return FHertAceContext::Current().Get_input_data(name, idx);
}

//! @brief Set output cipher by name and index
inline void Set_output_data(const char* name, size_t idx, CIPHER data)
{
    Io_set_output(name, idx, new acelib::DeviceCipher(std::move(*data)));
}

//! @brief Encode float array into plaintext.
//! The trailing `is_ext` (extended-basis flag, emitted by the mdown-sinking
//! pass) is accepted for ABI parity with the compiler emit (ir2c_ctx.h
//! ENCODE_CHILD::EXT) but not forwarded — the ace backend's encode chain does
//! not take an extended-basis selection, and ace runs the ckks codegen path
//! that the mdown-sinking pass does not promote on. Mirrors rtlib ant
//! Encode_float_mask handling.
inline void Encode_float(PLAIN plain, float* input, size_t len, SCALE_T sc_degree, LEVEL_T level, bool is_ext = false)
{
    (void)is_ext;
    FHertAceContext::Current().Encode_float(*plain, input, len, sc_degree, level);
}

inline void Encode_float_cst_lvl(PLAIN plain, float* input, size_t len, SCALE_T sc_degree, int level,
                                 bool is_ext = false)
{
    (void)is_ext;
    FHertAceContext::Current().Encode_float_cst_lvl(*plain, input, len, sc_degree, level);
}

inline void Encode_float_gpu(PLAIN plain, float* input, size_t len, SCALE_T sc_degree, int level, bool is_ext = false)
{
    (void)is_ext;
    // GPU encode path — same as cst_lvl for ACE (encoder is always GPU)
    FHertAceContext::Current().Encode_float_cst_lvl(*plain, input, len, sc_degree, level);
}

inline void Encode_double(PLAIN plain, double* input, size_t len, SCALE_T sc_degree, LEVEL_T level, bool is_ext = false)
{
    (void)is_ext;
    // ACE uses same encoding path for float/double (double is cast internally)
    std::vector<float> fvec(input, input + len);
    FHertAceContext::Current().Encode_float(*plain, fvec.data(), len, sc_degree, level);
}

inline void Encode_double_cst_lvl(PLAIN plain, double* input, size_t len, SCALE_T sc_degree, int level,
                                  bool is_ext = false)
{
    (void)is_ext;
    std::vector<float> fvec(input, input + len);
    FHertAceContext::Current().Encode_float_cst_lvl(*plain, fvec.data(), len, sc_degree, level);
}

inline void Encode_float_mask(PLAIN plain, float input, size_t len, SCALE_T sc_degree, LEVEL_T level,
                              bool is_ext = false)
{
    (void)is_ext;
    FHertAceContext::Current().Encode_float_mask(*plain, input, len, sc_degree, level);
}

inline void Encode_float_mask_cst_lvl(PLAIN plain, float input, size_t len, SCALE_T sc_degree, int level,
                                      bool is_ext = false)
{
    (void)is_ext;
    FHertAceContext::Current().Encode_float_mask_cst_lvl(*plain, input, len, sc_degree, level);
}

inline void Encode_double_mask(PLAIN plain, double input, size_t len, SCALE_T sc_degree, LEVEL_T level,
                               bool is_ext = false)
{
    (void)is_ext;
    FHertAceContext::Current().Encode_float_mask(*plain, static_cast<float>(input), len, sc_degree, level);
}

inline void Encode_double_mask_cst_lvl(PLAIN plain, double input, size_t len, SCALE_T sc_degree, int level,
                                       bool is_ext = false)
{
    (void)is_ext;
    FHertAceContext::Current().Encode_float_mask_cst_lvl(*plain, static_cast<float>(input), len, sc_degree, level);
}

// HE Operations — dispatch directly to FheContext methods

inline CIPHER Add_scalar(CIPHER res, CIPHER op, double op2)
{
    FHertAceContext::Current().Add_const(*res, *op, op2);
    return res;
}

inline CIPHER Add_ciph(CIPHER res, CIPHER op1, CIPHER op2)
{
    FHertAceContext::Current().Add(*res, *op1, *op2);
    return res;
}

inline CIPHER Add_plain(CIPHER res, CIPHER op1, PLAIN op2)
{
    FHertAceContext::Current().Add(*res, *op1, *op2);
    return res;
}

inline CIPHER Mul_ciph(CIPHER res, CIPHER op1, CIPHER op2)
{
    FHertAceContext::Current().Mul(*res, *op1, *op2);
    return res;
}

inline CIPHER Mul_scalar(CIPHER res, CIPHER op1, double op2)
{
    FHertAceContext::Current().Mul_const(*res, *op1, op2);
    return res;
}

inline CIPHER Mul_plain(CIPHER res, CIPHER op1, PLAIN op2)
{
    FHertAceContext::Current().Mul(*res, *op1, *op2);
    return res;
}

inline CIPHER Rotate_ciph(CIPHER res, CIPHER op, int step)
{
    FHertAceContext::Current().Rotate(*res, *op, step);
    return res;
}

inline CIPHER Rescale_ciph(CIPHER res, CIPHER op)
{
    FHertAceContext::Current().Rescale(*res, *op);
    return res;
}

inline CIPHER Mod_switch(CIPHER res, CIPHER op)
{
    FHertAceContext::Current().Mod_switch(*res, *op);
    return res;
}

inline CIPHER Relin(CIPHER res, CIPHER3 ciph)
{
    FHertAceContext::Current().Relin(*res, *ciph);
    return res;
}

inline CIPHER Bootstrap(CIPHER res, CIPHER op, int level)
{
    FHertAceContext::Current().Bootstrap(*res, *op, level, 0);
    return res;
}

inline CIPHER Bootstrap(CIPHER res, CIPHER op, int level, int slots)
{
    FHertAceContext::Current().Bootstrap(*res, *op, level, slots);
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
    if (res) {
        *res = CIPHERTEXT();
    }
}

inline void Free_ciph(CIPHER res)
{
    (void)res;
}
inline void Free_plain(PLAIN res)
{
    (void)res;
}

// Scale / Level queries

SCALE_T Sc_degree(CIPHER ct)
{
    return FHertAceContext::Current().Scale(*ct);
}

SCALE_T Sc_factor(CIPHER ct)
{
    return ct->scale();
}

LEVEL_T Level(CIPHER ct)
{
    return FHertAceContext::Current().Level(*ct);
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

// for validate APIs
#include "common/rt_validate.h"

#endif  // RTLIB_RT_ACE_RT_ACE_H
