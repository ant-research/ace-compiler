//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_CORE_CLONE_HANDLER_RETV_H
#define AIR_CORE_CLONE_HANDLER_RETV_H

#include "air/base/node.h"

namespace air {
namespace core {

/**
 * @brief A default handler return type
 *
 */
class HANDLER_RETV {
public:
  HANDLER_RETV() : _node(air::base::Null_ptr) {}

  HANDLER_RETV(air::base::NODE_PTR node) : _node(node) {}

  air::base::NODE_PTR Node() const { return _node; }

  uint32_t Num_node() const { return 1; }

private:
  air::base::NODE_PTR _node;
};

}  // namespace core
}  // namespace air

#endif  // AIR_CORE_CLONE_HANDLER_RETV_H
