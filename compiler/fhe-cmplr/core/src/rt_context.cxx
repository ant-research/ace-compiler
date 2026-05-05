//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/core/rt_context.h"

#include "common/cmplr_api.h"

namespace fhe {
namespace rt_context {

void Register_context(const fhe::core::CTX_PARAM& param) {
  Prepare_context_for_cmplr(param.Get_poly_degree(), param.Get_security_level(),
                            param.Get_mul_level() - 1, param.Get_input_level(),
                            param.Get_first_prime_bit_num(),
                            param.Get_scaling_factor_bit_num(),
                            param.Get_q_part_num(), param.Get_hamming_weight());
}

void Release_context() { Finalize_context_for_cmplr(); }

void Fetch_q_primes(std::vector<uint64_t>& q_primes) {
  size_t num_q = ::Get_q_cnt();
  q_primes.resize(num_q);
  // call rtlib Fetch_q_primes
  size_t num_primes = ::Fetch_q_primes(q_primes.data());
  AIR_ASSERT_MSG(num_primes == num_q, "unmatched size");
}

void Fetch_p_primes(std::vector<uint64_t>& p_primes) {
  size_t num_p = ::Get_p_cnt();
  p_primes.resize(num_p);
  // call rtlib Fetch_p_primes
  size_t num_primes = ::Fetch_p_primes(p_primes.data());
  AIR_ASSERT_MSG(num_primes == num_p, "unmatched size");
}

void Get_qlhmodp_at(std::vector<VL>& vals, uint32_t dim1_idx,
                    uint32_t dim2_idx) {
  size_t num_dim3 = ::Get_qlhmodp_dim3_cnt(dim1_idx, dim2_idx);
  vals.resize(num_dim3);
  size_t num_dim4 = 0;
  for (size_t dim3_idx = 0; dim3_idx < num_dim3; dim3_idx++) {
    VL& vl = vals[dim3_idx];
    ::Fetch_qlhmodp_at(vl.Data_ptr(), vl.Len_ptr(), dim1_idx, dim2_idx,
                       dim3_idx);
    num_dim4 = num_dim4 == 0 ? vl.Len() : num_dim4;
    // each dim4 should have same size, otherwise we cannot create 2D array
    // constant
    AIR_ASSERT(num_dim4 == vl.Len());
  }
}

void Get_phmodq(std::vector<VL>& vals) {
  size_t num_dim1 = ::Get_phmodq_dim1_cnt();
  vals.resize(num_dim1);
  for (size_t dim1_idx = 0; dim1_idx < num_dim1; dim1_idx++) {
    VL& vl = vals[dim1_idx];
    ::Fetch_phmodq_at(vl.Data_ptr(), vl.Len_ptr(), dim1_idx);
  }
}

}  // namespace rt_context
}  // namespace fhe