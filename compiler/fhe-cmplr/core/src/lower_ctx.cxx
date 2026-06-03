//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/core/lower_ctx.h"

#include <sstream>

#include "common/cmplr_api.h"
#include "fhe/core/rt_context.h"

using namespace air::base;
using namespace fhe::rt_context;
namespace fhe {
namespace core {

CONSTANT_PTR CRT_CST::Get_q(GLOB_SCOPE* gs) {
  if (_q != CONSTANT_ID()) {
    return gs->Constant(_q);
  }
  // build q primes constants
  std::vector<uint64_t> q_primes;
  fhe::rt_context::Fetch_q_primes(q_primes);
  CONSTANT_PTR cst =
      Create_array_cst(gs, QPRIMES_NAME, q_primes.data(), q_primes.size());
  Set_q(cst);
  return cst;
}

air::base::CONSTANT_PTR CRT_CST::Get_p(GLOB_SCOPE* gs) {
  if (_p != CONSTANT_ID()) {
    return gs->Constant(_p);
  }
  // build p primes constants
  std::vector<uint64_t> p_primes;
  fhe::rt_context::Fetch_p_primes(p_primes);
  CONSTANT_PTR cst =
      Create_array_cst(gs, PPRIMES_NAME, p_primes.data(), p_primes.size());
  Set_p(cst);
  return cst;
}

uint64_t CRT_CST::Get_prime(GLOB_SCOPE* gs, uint32_t idx) {
  CONSTANT_PTR q     = Get_q(gs);
  size_t       q_len = q->Array_byte_len() / sizeof(uint64_t);
  if (idx < q_len) {
    return q->Array_elem<uint64_t>(idx);
  } else {
    // Get from P primes
    idx            = idx - q_len;
    CONSTANT_PTR p = Get_p(gs);
    return p->Array_elem<uint64_t>(idx);
  }
}

uint64_t CRT_CST::Get_prime(GLOB_SCOPE* gs, uint32_t idx) const {
  CONSTANT_PTR q = Get_q(gs);
  AIR_ASSERT(q != Null_ptr);
  size_t q_len = q->Array_byte_len() / sizeof(uint64_t);
  if (idx < q_len) {
    return q->Array_elem<uint64_t>(idx);
  } else {
    // Get from P primes
    CONSTANT_PTR p = Get_p(gs);
    AIR_ASSERT(p != Null_ptr);
    idx = idx - q_len;
    return p->Array_elem<uint64_t>(idx);
  }
}

CONSTANT_PTR CRT_CST::Get_qlhinvmodq(GLOB_SCOPE* gs, uint32_t part_idx,
                                     uint32_t part_size_idx) {
  uint64_t key = ((uint64_t)part_idx << 32) + (uint64_t)part_size_idx;
  if (_qlhinvmodq_map.find(key) != _qlhinvmodq_map.end()) {
    CONSTANT_ID id = _qlhinvmodq_map[key];
    AIR_ASSERT_MSG(id != air::base::CONSTANT_ID(), "null constant");
    return gs->Constant(id);
  }
  VL vl;
  ::Fetch_qlhinvmodq_at(vl.Data_ptr(), vl.Len_ptr(), part_idx, part_size_idx);
  std::stringstream ss;
  ss << QLHINVMODQ_NAME << "_" << std::to_string(part_idx) << "_"
     << std::to_string(part_size_idx);
  CONSTANT_PTR cst = Create_array_cst(gs, ss.str(), vl.Data(), vl.Len());
  Add_qlhinvmodq(key, cst);
  return cst;
}

CONSTANT_PTR CRT_CST::Get_qlinvmodq(GLOB_SCOPE* gs, uint32_t idx) {
  if (_qlinvmodq_map.find(idx) != _qlinvmodq_map.end()) {
    CONSTANT_ID id = _qlinvmodq_map[idx];
    AIR_ASSERT_MSG(id != air::base::CONSTANT_ID(), "null constant");
    return gs->Constant(id);
  }
  VL vl;
  ::Fetch_qlinvmodq_at(vl.Data_ptr(), vl.Len_ptr(), idx);
  std::stringstream ss;
  ss << QLINVMODQ_NAME << "_" << std::to_string(idx);
  CONSTANT_PTR cst = Create_array_cst(gs, ss.str(), vl.Data(), vl.Len());
  Add_qlinvmodq(idx, cst);
  return cst;
}

CONSTANT_PTR CRT_CST::Get_qlhmodp(air::base::GLOB_SCOPE* gs, uint32_t q_idx,
                                  uint32_t part_idx, uint32_t start_idx,
                                  uint32_t len) {
  // the values are all within 16 bits, compose a 64 bit key
  const uint32_t bit16 = 16;
  const uint32_t bound = 1 << bit16;
  AIR_ASSERT(start_idx < bound && len < bound && q_idx < bound &&
             part_idx < bound);
  uint64_t key = ((uint64_t)start_idx << (bit16 * 3)) +
                 ((uint64_t)len << (bit16 * 2)) + ((uint64_t)q_idx << bit16) +
                 (uint64_t)part_idx;
  if (_qlhmodp_map.find(key) != _qlhmodp_map.end()) {
    CONSTANT_ID id = _qlhmodp_map[key];
    AIR_ASSERT_MSG(id != air::base::CONSTANT_ID(), "null constant");
    return gs->Constant(id);
  }
  std::vector<VL> vals;
  Get_qlhmodp_at(vals, q_idx, part_idx);
  std::vector<VL> sub_vals;
  // extract second dim from [start_idx, start_idx + len]
  for (const auto& row : vals) {
    VL sub_row;
    sub_row.Set_data(row.Data() + start_idx);
    sub_row.Set_len(len);
    sub_vals.push_back(sub_row);
  }

  std::stringstream ss;
  ss << QLHMODP_NAME << "_" << std::to_string(q_idx) << "_"
     << std::to_string(part_idx) << "_" << std::to_string(start_idx);
  // base convert in modup phase needs to transpose qlhmodp data to make sure
  // the same indexing rule with loop induction sequence[i][j]
  CONSTANT_PTR cst = Create_2d_array_cst(gs, ss.str(), sub_vals, true);
  Add_qlhmodp(key, cst);
  return cst;
}

CONSTANT_PTR CRT_CST::Get_phmodq(GLOB_SCOPE* gs) {
  if (_phmodq != CONSTANT_ID()) {
    return gs->Constant(_phmodq);
  }
  std::vector<VL> vals;
  fhe::rt_context::Get_phmodq(vals);
  std::string  name_str = PHMODQ_NAME;
  CONSTANT_PTR cst      = Create_2d_array_cst(gs, name_str, vals);
  Set_phmodq(cst);
  return cst;
}

CONSTANT_PTR CRT_CST::Get_qlhalfmodq(air::base::GLOB_SCOPE* gs, uint32_t idx) {
  if (_qlhalfmodq_map.find(idx) != _qlhalfmodq_map.end()) {
    CONSTANT_ID id = _qlhalfmodq_map[idx];
    AIR_ASSERT_MSG(id != air::base::CONSTANT_ID(), "null constant");
    return gs->Constant(id);
  }
  VL vl;
  ::Fetch_qlhalfmodq_at(vl.Data_ptr(), vl.Len_ptr(), idx);
  std::stringstream ss;
  ss << QLHALFMODQ_NAME << "_" << std::to_string(idx);
  CONSTANT_PTR cst = Create_array_cst(gs, ss.str(), vl.Data(), vl.Len());
  Add_qlhalfmodq_map(idx, cst);
  return cst;
}

CONSTANT_PTR CRT_CST::Get_q0halfmodq(air::base::GLOB_SCOPE* gs) {
  // use the UINT16_MAX as hash idx for q0
  // Since the rtlib does not provide the constant value, (need rtlib
  // enhancement), we calculate them at compile time by iterate the
  // q primes, and use the max value of uint16 as the hash idx, the number
  // of q won't exceed UINT16_MAX in FHE
  uint32_t idx = UINT16_MAX;
  if (_qlhalfmodq_map.find(idx) != _qlhalfmodq_map.end()) {
    CONSTANT_ID id = _qlhalfmodq_map[idx];
    AIR_ASSERT_MSG(id != air::base::CONSTANT_ID(), "null constant");
    return gs->Constant(id);
  }
  CONSTANT_PTR          q_cst = Get_q(gs);
  size_t                q_len = q_cst->Array_byte_len() / sizeof(uint64_t);
  uint64_t              q0    = q_cst->Array_elem<uint64_t>(0);
  std::vector<uint64_t> data;
  for (size_t idx = 0; idx < q_len; idx++) {
    uint64_t val = (q0 / 2) % q_cst->Array_elem<uint64_t>(idx);
    data.push_back(val);
  }
  std::stringstream ss;
  ss << Q0HALFMODQ_NAME << "_" << std::to_string(idx);
  CONSTANT_PTR cst = Create_array_cst(gs, ss.str(), data.data(), data.size());
  Add_qlhalfmodq_map(idx, cst);
  return cst;
}

CONSTANT_PTR CRT_CST::Get_phinvmodp(air::base::GLOB_SCOPE* gs) {
  if (_phinvmodp != CONSTANT_ID()) {
    return gs->Constant(_phinvmodp);
  }
  VL vl;
  ::Fetch_phinvmodp(vl.Data_ptr(), vl.Len_ptr());
  std::string  name_str = PHINVMODP_NAME;
  CONSTANT_PTR cst      = Create_array_cst(gs, name_str, vl.Data(), vl.Len());
  Set_phinvmodp(cst);
  return cst;
}

CONSTANT_PTR CRT_CST::Get_pinvmodq(air::base::GLOB_SCOPE* gs) {
  if (_pinvmodq != CONSTANT_ID()) {
    return gs->Constant(_pinvmodq);
  }
  VL vl;
  ::Fetch_pinvmodq(vl.Data_ptr(), vl.Len_ptr());
  std::string  name_str = PINVMODQ_NAME;
  CONSTANT_PTR cst      = Create_array_cst(gs, name_str, vl.Data(), vl.Len());
  Set_pinvmodq(cst);
  return cst;
}

CONSTANT_PTR CRT_CST::Create_array_cst(GLOB_SCOPE* gs, std::string name_str,
                                       const uint64_t* data, size_t len) {
  AIR_ASSERT(data != NULL && len != 0);
  std::vector<int64_t> dims{(int64_t)len};
  SPOS                 spos      = gs->Unknown_simple_spos();
  TYPE_PTR             u64_type  = gs->Prim_type(PRIMITIVE_TYPE::INT_U64);
  STR_PTR              type_name = gs->New_str(name_str.c_str());
  TYPE_PTR     arr_type = gs->New_arr_type(type_name, u64_type, dims, spos);
  CONSTANT_PTR cst = gs->New_const(CONSTANT_KIND::ARRAY, arr_type, (void*)data,
                                   len * sizeof(uint64_t));
  return cst;
}

CONSTANT_PTR CRT_CST::Create_2d_array_cst(GLOB_SCOPE* gs, std::string name_str,
                                          std::vector<VL>& vec2d,
                                          bool             transpose) {
  std::vector<uint64_t> vec1d;
  size_t                dim1_size = vec2d.size();
  size_t                dim2_size = vec2d[0].Len();
  if (transpose) {
    for (size_t col_idx = 0; col_idx < vec2d[0].Len(); col_idx++) {
      for (size_t row_idx = 0; row_idx < vec2d.size(); row_idx++) {
        VL& row = vec2d[row_idx];
        vec1d.push_back(*(row.Data() + col_idx));
        // each dim should have same size
        AIR_ASSERT(vec2d[0].Len() == row.Len());
      }
    }
    // exchange two dimension size
    dim1_size = vec2d[0].Len();
    dim2_size = vec2d.size();
  } else {
    for (size_t row_idx = 0; row_idx < vec2d.size(); row_idx++) {
      VL& row = vec2d[row_idx];
      for (size_t col_idx = 0; col_idx < vec2d[0].Len(); col_idx++) {
        vec1d.push_back(*(row.Data() + col_idx));
      }
      // each dim2 should have same size
      AIR_ASSERT(vec2d[0].Len() == row.Len());
    }
  }
  std::vector<int64_t> dims{(int64_t)dim1_size, (int64_t)dim2_size};
  SPOS                 spos      = gs->Unknown_simple_spos();
  TYPE_PTR             u64_type  = gs->Prim_type(PRIMITIVE_TYPE::INT_U64);
  STR_PTR              type_name = gs->New_str(name_str.c_str());
  TYPE_PTR     arr_type = gs->New_arr_type(type_name, u64_type, dims, spos);
  CONSTANT_PTR cst =
      gs->New_const(CONSTANT_KIND::ARRAY, arr_type, (void*)vec1d.data(),
                    vec1d.size() * sizeof(uint64_t));
  return cst;
}

void LOWER_CTX::Register_rt_context() const {
  fhe::rt_context::Register_context(Get_ctx_param());
}

void LOWER_CTX::Release_rt_context() const {
  fhe::rt_context::Release_context();
}

}  // namespace core
}  // namespace fhe
