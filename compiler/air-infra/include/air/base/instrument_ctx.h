//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_BASE_INSTRUMENT_CTX_H
#define AIR_BASE_INSTRUMENT_CTX_H

#include <vector>

#include "air/base/analyze_ctx.h"

namespace air {
namespace base {

//! @brief context for in-place instrumentation used by VISITOR
//! This context insert new expr/stmt into original IR without clone local or
//! global scope. To instrument into new IR, use TRANSFORM_CTX instead.
class INSTRUMENT_CTX : public ANALYZE_CTX {
public:
  //! @brief Construct a new in-place instrument context
  INSTRUMENT_CTX() {}

  //! @brief Prepend new stmt before current stmt
  void Prepend(STMT_PTR stmt) { _pre_stmt.push_back(stmt); }

  //! @brief Append new stmt after current stmt
  void Append(STMT_PTR stmt) { _post_stmt.push_back(stmt); }

  //! @brief Default BLOCK handler to traverse stmt list and
  //! prepend/append new stmt into block
  template <typename RETV, typename VISITOR>
  RETV Handle_block(VISITOR* visitor, air::base::NODE_PTR node) {
    AIR_ASSERT(node->Is_block());
    STMT_LIST list(node);
    STMT_PTR  stmt = node->Begin_stmt();
    STMT_PTR  end  = node->End_stmt();
    while (stmt != end) {
      visitor->template Visit<RETV>(stmt->Node());
      if (!_pre_stmt.empty()) {
        for (auto&& it : _pre_stmt) {
          list.Prepend(stmt, it);
        }
        _pre_stmt.clear();
      }
      stmt = stmt->Next();
      if (!_post_stmt.empty()) {
        for (auto&& it : _post_stmt) {
          list.Prepend(stmt, it);
        }
        _post_stmt.clear();
      }
    }
    return RETV();
  }

protected:
  std::vector<STMT_PTR> _pre_stmt;
  std::vector<STMT_PTR> _post_stmt;

};  // INSTRUMENT_CTX

}  // namespace base
}  // namespace air

#endif  // AIR_BASE_INSTRUMENT_CTX_H
