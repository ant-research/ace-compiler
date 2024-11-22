//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_BASE_OPCODE_GEN_H
#define AIR_BASE_OPCODE_GEN_H

//! @brief OPCODE_ENUM_GEN generates ENUM constant for each OPCODE
#define OPCODE_ENUM_GEN(NAME, name, cat, kid_num, fld_num, prop) NAME,

//! @brief OPCODE_META_GEN generates META info for each OPCODE
#define OPCODE_META_GEN(NAME, name, cat, kid_num, fld_num, prop) \
  {#name, cat, kid_num, fld_num, prop},

//! @brief OPCODE_HANDLER_GEN generates case statement in Handler for each
//! OPCODE
#define OPCODE_HANDLER_GEN(NAME, name, cat, kid_num, fld_num, prop) \
  case OPCODE::NAME:                                                \
    return Handle_##name<RETV, VISITOR>(visitor, node);

//! @brief OPCODE_IMPL_GEN generates dispatch function to IMPL for each OPCODE
#define OPCODE_IMPL_GEN(NAME, name, cat, kid_num, fld_num, prop)       \
  template <typename RETV, typename VISITOR>                           \
  RETV Handle_##name(VISITOR* visitor, air::base::NODE_PTR node) {     \
    return _impl.template Handle_##name<RETV, VISITOR>(visitor, node); \
  }

//! @brief OPCODE_NULL_HANDLER_GEN generates null handler for each OPCODE
#define OPCODE_NULL_HANDLER_GEN(NAME, name, cat, kid_num, fld_num, prop) \
  template <typename RETV, typename VISITOR>                             \
  RETV Handle_##name(VISITOR* visitor, air::base::NODE_PTR node) {       \
    return RETV();                                                       \
  }

//! @brief OPCODE_DEFAULT_HANDLER_GEN generates default handler for each OPCODE
#define OPCODE_DEFAULT_HANDLER_GEN(NAME, name, cat, kid_num, fld_num, prop) \
  template <typename RETV, typename VISITOR>                                \
  RETV Handle_##name(VISITOR* visitor, air::base::NODE_PTR node) {          \
    return visitor->Context().template Handle_node<RETV, VISITOR>(visitor,  \
                                                                  node);    \
  }

//! @brief OPCODE_INVALID_HANDLER_GEN generates invalid handler for each OPCODE
#define OPCODE_INVALID_HANDLER_GEN(NAME, name, cat, kid_num, fld_num, prop) \
  template <typename RETV, typename VISITOR>                                \
  RETV Handle_##name(VISITOR* visitor, air::base::NODE_PTR node) {          \
    CMPLR_ASSERT(false, "Unexpected " #NAME);                               \
    return RETV();                                                          \
  }

//! @brief OPCODE_CLONE_HANDLER_GEN generates clone handler for each OPCODE
#define OPCODE_CLONE_HANDLER_GEN(NAME, name, cat, kid_num, fld_num, prop) \
  template <typename RETV, typename VISITOR>                              \
  RETV Handle_##name(VISITOR* visitor, air::base::NODE_PTR node) {        \
    return visitor->template Handle_node<RETV>(node);                     \
  }

#endif  // AIR_BASE_OPCODE_GEN_H
