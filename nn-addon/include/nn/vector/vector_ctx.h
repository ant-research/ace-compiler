//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_VECTOR_CTX_H
#define NN_VECTOR_CTX_H

#include "nn/vector/config.h"
#include "nn/vector/vector_utils.h"

namespace nn {
namespace vector {

using NODE_MASK_PAIR = std::pair<NODE_ID, uint32_t>;
using MF_WORKLIST    = std::vector<NODE_MASK_PAIR>;
using SS_WORKLIST    = std::vector<NODE_ID>;

//! @brief Context for passes in VECTOR phase
class VECTOR_CTX {
public:
  VECTOR_CTX() : _num_vloop(0), _slot(MIN_SLOT_ALLOWED) {}

  void     Incr_num_vloop() { _num_vloop++; }
  uint32_t Get_num_vloop() const { return _num_vloop; }

  void Update_slot(int64_t slot) {
    AIR_ASSERT(slot <= MAX_SLOT_ALLOWED);
    if (!Is_power_of_two((int)slot)) {
      slot = Next_power_of_two(slot);
    }
    AIR_ASSERT(slot <= MAX_SLOT_ALLOWED);

    if ((uint32_t)slot > _slot) {
      _slot = slot;
    }
  }

  uint32_t Slot() const { return _slot; }

  void Register_node_mask_len(NODE_ID node, uint32_t mask_len) {
    _mf_wl.push_back(std::make_pair(node, mask_len));
  }

  MF_WORKLIST Get_mf_worklist() { return _mf_wl; }

  void Register_ss_node(NODE_ID node) { _ss_wl.push_back(node); }

  SS_WORKLIST Get_ss_worklist() { return _ss_wl; }

private:
  VECTOR_CTX(const VECTOR_CTX&)            = delete;
  VECTOR_CTX& operator=(const VECTOR_CTX&) = delete;

  uint32_t    _num_vloop;
  uint32_t    _slot;
  MF_WORKLIST _mf_wl;  // used for mask fusion
  SS_WORKLIST _ss_wl;  // used for strided_slice fusion
};

//! @brief Macro to define API to access context
#define DECLARE_VECTOR_CTX_ACCESS_API(cfg)                                   \
  void     Incr_num_vloop() { cfg.Incr_num_vloop(); }                        \
  uint32_t Get_num_vloop() const { return cfg.Get_num_vloop(); }             \
  void     Update_slot(int64_t slot) { cfg.Update_slot(slot); }              \
  uint32_t Slot() const { return cfg.Slot(); }                               \
  void     Register_node_mask_len(NODE_ID node, uint32_t mask_len) {         \
    cfg.Register_node_mask_len(node, mask_len);                          \
  }                                                                          \
  MF_WORKLIST Get_mf_worklist() { return cfg.Get_mf_worklist(); }            \
  void        Register_ss_node(NODE_ID node) { cfg.Register_ss_node(node); } \
  SS_WORKLIST Get_ss_worklist() { return cfg.Get_ss_worklist(); }

}  // namespace vector
}  // namespace nn

#endif  // NN_VECTOR_CTX_H
