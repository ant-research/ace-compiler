//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_ANT_BENCHMARK_SRC_BM_RTLIB_CKKS_H
#define RTLIB_ANT_BENCHMARK_SRC_BM_RTLIB_CKKS_H

#include "benchmark/benchmark.h"
#include "ckks/bootstrap.h"
#include "ckks/encoder.h"
#include "ckks/encryptor.h"
#include "ckks/evaluator.h"
#include "ckks/key_gen.h"
#include "ckks/param.h"
#include "common/rt_api.h"
#include "context/ckks_context.h"
#include "helper.h"

using namespace benchmark;

class CKKS_BENCH {
public:
  static CKKS_BENCH& Instance() {
    static CKKS_BENCH instance;
    return instance;
  }

  void Setup() {
    size_t degree    = BM_GLOB_ENV::Get_env(TEST_DEGREE);
    size_t num_q     = BM_GLOB_ENV::Get_env(NUM_Q);
    size_t num_parts = BM_GLOB_ENV::Get_env(NUM_Q_PART);
    size_t q0_bits   = BM_GLOB_ENV::Get_env(Q0_BITS);
    size_t sf_bits   = BM_GLOB_ENV::Get_env(SF_BITS);
    size_t hw        = BM_GLOB_ENV::Get_env(HW);
    Set_context_params(degree, num_q, q0_bits, sf_bits, num_parts,
                       0 /*input_level*/, hw);
    Prepare_context();
    // TODO: should be removed after refactor CKKS APIs
    _param     = (CKKS_PARAMETER*)Param();
    _keygen    = (CKKS_KEY_GENERATOR*)Keygen();
    _evaluator = (CKKS_EVALUATOR*)Eval();
    _encoder   = (CKKS_ENCODER*)Encoder();
    _decryptor = (CKKS_DECRYPTOR*)Decryptor();
    _encryptor = (CKKS_ENCRYPTOR*)Encryptor();

    _rnd_ciph1   = Alloc_rnd_cipher();
    _rnd_ciph2   = Alloc_rnd_cipher();
    _rnd_ciph_l1 = Alloc_rnd_cipher(1 /*init_level*/);
  }

  void Teardown() {
    Free_ciphertext(_rnd_ciph1);
    Free_ciphertext(_rnd_ciph2);
    Free_ciphertext(_rnd_ciph_l1);
    Finalize_context();
  }

  size_t              Degree() { return Get_param_degree(_param); }
  CKKS_KEY_GENERATOR* Key_gen() { return _keygen; }
  CKKS_EVALUATOR*     Evaluator() { return _evaluator; }
  CKKS_BTS_CTX*       Bts_ctx() { return Get_bts_ctx(_evaluator); }
  CIPHERTEXT*         Ciph1() { return _rnd_ciph1; }
  CIPHERTEXT*         Ciph2() { return _rnd_ciph1; }
  CIPHERTEXT*         Ciph_l1() { return _rnd_ciph_l1; }
  void                Eval_bts_precom(size_t num_slots);

private:
  CKKS_BENCH()                             = default;
  ~CKKS_BENCH()                            = default;
  CKKS_BENCH(const CKKS_BENCH&)            = delete;
  CKKS_BENCH& operator=(const CKKS_BENCH&) = delete;

  //! @brief Allocate a random ciphertext with input level
  CIPHERTEXT* Alloc_rnd_cipher(size_t level = 0) {
    CIPHERTEXT* ciph   = Alloc_ciphertext();
    PLAINTEXT*  plain  = Alloc_plaintext();
    size_t      degree = Get_param_degree(_param);
    VALUE_LIST* vec    = Alloc_value_list(DCMPLX_TYPE, degree / 2);
    ENCODE_AT_LEVEL(plain, _encoder, vec, level);
    Encrypt_msg(ciph, _encryptor, plain);
    Free_plaintext(plain);
    Free_value_list(vec);
    return ciph;
  }

  CKKS_PARAMETER*     _param;
  CKKS_KEY_GENERATOR* _keygen;
  CKKS_ENCODER*       _encoder;
  CKKS_ENCRYPTOR*     _encryptor;
  CKKS_DECRYPTOR*     _decryptor;
  CKKS_EVALUATOR*     _evaluator;
  CIPHERTEXT*         _rnd_ciph1;
  CIPHERTEXT*         _rnd_ciph2;
  CIPHERTEXT*         _rnd_ciph_l1;
};

//! @brief Eval bootstrap precomp(setup & keygen)
void CKKS_BENCH::Eval_bts_precom(size_t num_slots) {
  CKKS_BTS_CTX*    bts_ctx = Bts_ctx();
  CKKS_BTS_PRECOM* precom  = Get_bts_precom(bts_ctx, num_slots);
  if (!precom) {
    VL_UI32* level_budget          = Alloc_value_list(UI32_TYPE, 2);
    UI32_VALUE_AT(level_budget, 0) = 3;
    UI32_VALUE_AT(level_budget, 1) = 3;
    uint32_t bts_depth =
        Get_bootstrap_depth(level_budget, _param->_hamming_weight);
    FMT_ASSERT(Get_mult_depth(_param) > bts_depth,
               "we cannot obtain more towers from bootstrapping");
    VL_UI32* dim1          = Alloc_value_list(UI32_TYPE, 2);
    UI32_VALUE_AT(dim1, 0) = 0;
    UI32_VALUE_AT(dim1, 1) = 0;
    Bootstrap_setup(bts_ctx, level_budget, dim1, num_slots);
    Free_value_list(dim1);
    Bootstrap_keygen(bts_ctx, num_slots);
    Free_value_list(level_budget);
  }
}

#endif  // RTLIB_ANT_BENCHMARK_SRC_BM_RTLIB_CKKS_H