//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/opt/node_list.h"

#include "air/opt/hssa_container.h"
#include "air/opt/hssa_stmt.h"

namespace air {

namespace opt {
#if 0
template <typename ID_TYPE, typename PTR_TYPE, typename CONT_TYPE>
void NODE_LIST<ID_TYPE, PTR_TYPE, CONT_TYPE>::Print(std::ostream& os,
                                                     uint32_t indent) const {
  auto print = [](PTR_TYPE ptr, std::ostream& os, uint32_t indent) {
    ptr->Print(os, indent);
    os << std::endl;
  };
  For_each(print, os, indent);
}
#endif
#if 0
template <>
void NODE_LIST<HSTMT_ID, HSTMT_PTR, HSSA_CONTAINER>::Print(std::ostream& os,
                                                     uint32_t indent) const {
  auto print = [](HSTMT_PTR ptr, std::ostream& os, uint32_t indent) {
    ptr->Print(os, indent);
    os << std::endl;
  };
  For_each(print, os, indent);
}
#endif
}  // namespace opt
}  // namespace air