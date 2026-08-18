//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CKKS_CKKS_COST_MODEL_H
#define FHE_CKKS_CKKS_COST_MODEL_H

#include <vector>

#include "air/base/opcode.h"
#include "fhe/ckks/ckks_opcode.h"

namespace fhe {
namespace ckks {
class CKKS_OP_COST {
public:
  CKKS_OP_COST(air::base::OPCODE opc, std::vector<double> cost)
      : _opc(opc), _cost(cost) {}
  ~CKKS_OP_COST() {}

  air::base::OPCODE Opcode() const { return _opc; }
  double            Cost(uint32_t level) const { return _cost.at(level); }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  CKKS_OP_COST(void);
  CKKS_OP_COST(const CKKS_OP_COST&);
  CKKS_OP_COST operator=(const CKKS_OP_COST&);

  air::base::OPCODE   _opc;
  std::vector<double> _cost;  // index is level of ciphertext
};

double Operation_cost(air::base::OPCODE opc, uint32_t level);
double Rescale_cost(uint32_t level, uint32_t poly_num);

}  // namespace ckks
}  // namespace fhe

#endif  // FHE_CKKS_CKKS_COST_MODEL_H