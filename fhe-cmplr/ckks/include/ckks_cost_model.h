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
#include "air/util/debug.h"
#include "dfg_region.h"
#include "fhe/ckks/ckks_opcode.h"

namespace fhe {
namespace ckks {
//! @brief CKKS_OP_COST contains the cost of a CKKS operation at each level.
//! The costs were tested with an Intel Xeon Platinum 8369B CPU @2.70GHz and 512
//! GB of memory.
class CKKS_OP_COST {
public:
  CKKS_OP_COST(air::base::OPCODE opc, std::vector<double> cost)
      : _opc(opc), _cost(cost) {}
  // constructor for operations with the same cost at each level in [0, max_lev]
  CKKS_OP_COST(air::base::OPCODE opc, double cost, uint32_t max_lev)
      : _opc(opc), _cost(cost, max_lev + 1) {}
  ~CKKS_OP_COST() {}

  air::base::OPCODE Opcode(void) const { return _opc; }
  double            Cost(uint32_t level) const {
    AIR_ASSERT_MSG(level < _cost.size(), "cipher level= %u out of range",
                              level);
    return _cost[level];
  }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  CKKS_OP_COST(void);
  CKKS_OP_COST(const CKKS_OP_COST&);
  CKKS_OP_COST operator=(const CKKS_OP_COST&);

  air::base::OPCODE   _opc;   // CKKS domain opcode
  std::vector<double> _cost;  // index is level of ciphertext
};

//! @brief Return the cost of operation opc at level= lev and N= poly_deg.
//! The current cost model provides the costs for N= 2^16 and N= 2^17. If
//! poly_deg <= 2^16, the cost for N= 2^16 is returned, otherwise the cost for
//! N= 2^17 is returned.
double Operation_cost(air::base::OPCODE opc, uint32_t lev, uint32_t poly_deg);

//! @brief Return the cost of a rescaling operation at level= lev and N=
//! poly_deg. The poly_num must be at least 2, with the default cost
//! corresponding to poly_num = 2. If poly_num is greater than 2, the cost is
//! adjusted by a factor of poly_num/2.
double Rescale_cost(uint32_t lev, uint32_t poly_num, uint32_t poly_deg);

}  // namespace ckks
}  // namespace fhe

#endif  // FHE_CKKS_CKKS_COST_MODEL_H