//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_CG_ANALYZE_CTX_H
#define AIR_CG_ANALYZE_CTX_H

#include <vector>

#include "air/cg/cgir_node.h"

namespace air {

namespace cg {

//! @brief context for cgir analyze pass used by VISITOR
class ANALYZE_CTX {
public:
  //! @brief construct a new analyze context
  ANALYZE_CTX() {}

  //! @brief destruct TRANSFORM object
  ~ANALYZE_CTX() { AIR_ASSERT(_stack.empty()); }

  //! @brief manually push a bb to visiting stack
  //! @param bb BB to be pushed onto stack
  void Push(BB_PTR bb) { _stack.push_back(bb); }

  //! @brief manually pop a bb from visiting stack
  //! @param bb BB to be poped from stack
  void Pop(BB_PTR bb) {
    AIR_ASSERT(!_stack.empty() && _stack.back() == bb);
    _stack.pop_back();
  }

  //! @brief get top BB from visiting stack
  const BB_PTR Top() const {
    AIR_ASSERT(!_stack.empty());
    return _stack.back();
  }

  //! @brief check if bb stack is empty
  bool Empty() const { return _stack.empty(); }

  //! @brief begin analyze a BB
  void Begin_bb(BB_PTR bb) { Push(bb); }

  //! @brief end analyze a BB
  void End_bb(BB_PTR bb) { Pop(bb); }

  //! @brief begin analyze an instruction
  void Begin_inst(INST_PTR inst) {}

  //! @brief end analyze3 an instruction
  void End_inst(INST_PTR inst) {}

  //! @brief default handler for all instructions
  template <typename VISITOR>
  void Handle_inst(VISITOR* visitor, INST_PTR inst) {
    AIR_ASSERT_MSG(false, "Internal error: not implemented");
  }

  //! @brief define type for BB_STACK
  using BB_STACK = std::vector<BB_PTR>;

private:
  ANALYZE_CTX(const ANALYZE_CTX&)            = delete;
  ANALYZE_CTX(const ANALYZE_CTX&&)           = delete;
  ANALYZE_CTX& operator=(const ANALYZE_CTX&) = delete;

protected:
  // Get stack for all parents of current bb
  BB_STACK& Bb_stack() { return _stack; }

  // stack for parent bb in container
  BB_STACK _stack;

};  // ANALYZE_CTX

}  // namespace cg

}  // namespace air

#endif  // AIR_CG_ANALYZE_CTX_H
