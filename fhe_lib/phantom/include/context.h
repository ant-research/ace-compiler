//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_PHANTOM_CONTEXT_H
#define RTLIB_PHANTOM_CONTEXT_H

// Standard library
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

// External libraries
#include "rt_phantom/phantom_api.h"
#include "rt_phantom/rt_phantom.h"

// Local headers
#include "codec.h"
#include "bootstrap_mgr.h"
#include "key_mgr.h"
#include "pt_mgr.h"

namespace phantom {

class PHANTOM_CONTEXT
{
public:
    // Singleton access
    static PHANTOM_CONTEXT& Context();

    // Component accessors
    KEY_MANAGER& key_mgr() { return _key_mgr; }
    PT_MGR& pt_mgr() { return _pt_mgr; }

    // Low-level context access (for direct GPU kernel calls)
    PhantomContext* phantom_ctx() { return _key_mgr.ctx(); }
    const PhantomContext* phantom_ctx() const { return _key_mgr.ctx(); }

    const PhantomRelinKey& relin_key() const { return _key_mgr.Relin_key(); }
    const PhantomGaloisKey& galois_key() const { return _key_mgr.Rotate_key(); }

    uint64_t slot_count() const { return _slot_count; }
    uint64_t scaling_mod_size() const { return _scaling_mod_size; }
    uint64_t num_prime_parts() const { return _num_prime_parts; }

    // I/O operations
    void Prepare_input(TENSOR *input, const char *name);
    void Set_output_data(const char *name, size_t idx, PhantomCiphertext &ct);
    PhantomCiphertext Get_input_data(const char *name, size_t idx);
    double *Handle_output(const char *name, size_t idx);

    // Encode/Decode
    void Encode_float(PhantomPlaintext &pt, const float *input, size_t len, SCALE_T scale,
                      LEVEL_T level);
    void Encode_float_cst_lvl(PhantomPlaintext &pt, const float *input, size_t len,
                              SCALE_T scale, int level);
    void Encode_float_mask(PhantomPlaintext &pt, float input, size_t len, SCALE_T scale,
                           LEVEL_T level);
    void Encode_float_mask_cst_lvl(PhantomPlaintext &pt, float input, size_t len,
                                   SCALE_T scale, int level);

    void Decrypt(PhantomCiphertext &ct, std::vector<double> &vec);
    void Decode(PhantomPlaintext &pt, std::vector<double> &vec);

    // Arithmetic operations
    void Equal_level(CIPHERTEXT &op1, CIPHERTEXT &op2);
    void Equal_level(CIPHERTEXT &op1, PLAINTEXT &op2);

    void Add(PhantomCiphertext &res, const PhantomCiphertext &op1, const PhantomCiphertext &op2);
    void Add(PhantomCiphertext &res, const PhantomCiphertext &op1, const PhantomPlaintext &op2);
    void Add_const(PhantomCiphertext &res, const PhantomCiphertext &op1, double op2);

    void Mul(PhantomCiphertext &res, const PhantomCiphertext &op1, const PhantomCiphertext &op2);
    void Mul(PhantomCiphertext &res, const PhantomCiphertext &op1, const PhantomPlaintext &op2);
    void Mul_const(PhantomCiphertext &res, const PhantomCiphertext &op1, double op2);

    void Rotate(PhantomCiphertext &res, const PhantomCiphertext &op1, int step);
    void Rescale(PhantomCiphertext &res, const PhantomCiphertext &op1);
    void Mod_switch(PhantomCiphertext &res, const PhantomCiphertext &op1);
    void Relin(PhantomCiphertext &res, const PhantomCiphertext &op1);
    void Bootstrap(PhantomCiphertext &res, PhantomCiphertext &op1, int level, int slot);

    // Memory management
    void Free_cipher(PhantomCiphertext &ct);
    void Free_plain(PhantomPlaintext &pt);
    void Free_ciph_poly(PhantomCiphertext *ct, size_t size);

    // Query
    SCALE_T Scale(const PhantomCiphertext &op);
    LEVEL_T Level(const PhantomCiphertext &op);

private:
    PHANTOM_CONTEXT(const PHANTOM_CONTEXT &) = delete;
    PHANTOM_CONTEXT &operator=(const PHANTOM_CONTEXT &) = delete;

    PHANTOM_CONTEXT();
    ~PHANTOM_CONTEXT();

    static PHANTOM_CONTEXT *Instance;

    // Components
    KEY_MANAGER   _key_mgr;
    CODEC         _codec;
    BOOTSTRAP_MGR _bootstrap;
    PT_MGR        _pt_mgr;

    // State
    uint64_t _scaling_mod_size;
    uint64_t _num_prime_parts;
    uint64_t _slot_count;
};

}  // namespace phantom

#endif  // RTLIB_PHANTOM_CONTEXT_H