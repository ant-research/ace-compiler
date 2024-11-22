//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_VECTOR_CTX_H
#define NN_VECTOR_CTX_H

namespace nn {
namespace vector {

//! @brief Context for passes in VECTOR phase
class VECTOR_CTX {
public:
  VECTOR_CTX() : _num_vloop(0), _num_op_ca_th(0), _num_op_ca_t2vsh(0) {}

  void     Incr_num_op_ca_th() { _num_op_ca_th++; }
  uint32_t Get_num_op_ca_th() const { return _num_op_ca_th; }
  void     Incr_num_op_ca_t2vsh() { _num_op_ca_t2vsh++; }
  uint32_t Get_num_op_ca_t2vsh() const { return _num_op_ca_t2vsh; }

  void     Incr_num_vloop() { _num_vloop++; }
  uint32_t Get_num_vloop() const { return _num_vloop; }

private:
  VECTOR_CTX(const VECTOR_CTX&)            = delete;
  VECTOR_CTX& operator=(const VECTOR_CTX&) = delete;

  uint32_t _num_vloop    = 0;
  uint32_t _num_op_ca_th = 0;  // number of t2vslice_handler conv, avg op
  uint32_t _num_op_ca_t2vsh =
      0;  // number of tensor2vector_handler conv, avg op
};

//! @brief Macro to define API to access context
#define DECLARE_VECTOR_CTX_ACCESS_API(cfg)                             \
  void     Incr_num_vloop() { cfg.Incr_num_vloop(); }                  \
  uint32_t Get_num_vloop() const { return cfg.Get_num_vloop(); }       \
  void     Incr_num_op_ca_th() { cfg.Incr_num_op_ca_th(); }            \
  uint32_t Get_num_op_ca_th() const { return cfg.Get_num_op_ca_th(); } \
  void     Incr_num_op_ca_t2vsh() { cfg.Incr_num_op_ca_t2vsh(); }      \
  uint32_t Get_num_op_ca_t2vsh() const { return cfg.Get_num_op_ca_t2vsh(); }

}  // namespace vector
}  // namespace nn

#endif  // NN_VECTOR_CTX_H
