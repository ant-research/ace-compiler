//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/opt/ssa_node_list.h"

#include <sstream>

namespace air {

namespace opt {

template <>
void SSA_NODE_LIST<MU_NODE_ID, MU_NODE_PTR>::Print(std::ostream& os,
                                                   uint32_t      indent) const {
  auto print = [](MU_NODE_PTR ptr, std::ostream& os, uint32_t indent) {
    os << std::string(indent * air::base::INDENT_SPACE, ' ');
    ptr->Print(os);
  };
  For_each(print, os, indent);
}

template <>
void SSA_NODE_LIST<CHI_NODE_ID, CHI_NODE_PTR>::Print(std::ostream& os,
                                                     uint32_t indent) const {
  auto print = [](CHI_NODE_PTR ptr, std::ostream& os, uint32_t indent) {
    os << std::string(indent * air::base::INDENT_SPACE, ' ');
    ptr->Print(os);
  };
  For_each(print, os, indent);
}

template <>
void SSA_NODE_LIST<PHI_NODE_ID, PHI_NODE_PTR>::Print(std::ostream& os,
                                                     uint32_t indent) const {
  auto print = [](PHI_NODE_PTR ptr, std::ostream& os, uint32_t indent) {
    ptr->Print(os, indent);
    os << std::endl;
  };
  For_each(print, os, indent);
}

template <typename ID_TYPE, typename PTR_TYPE>
void SSA_NODE_LIST<ID_TYPE, PTR_TYPE>::Print() const {
  Print(std::cout, 0);
  std::cout << std::endl;
}

template <typename ID_TYPE, typename PTR_TYPE>
std::string SSA_NODE_LIST<ID_TYPE, PTR_TYPE>::To_str() const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os, 0);
  return buf.str();
}

}  // namespace opt

}  // namespace air
