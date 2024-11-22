//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/base/node_data.h"

#include "air/base/meta_info.h"

namespace air {
namespace base {

//=============================================================================
// class NODE_DATA member functions
//=============================================================================

NODE_DATA::NODE_DATA() {
  _comm._core._opcode         = static_cast<uint32_t>(OPCODE::INVALID);
  _comm._core._num_added_chld = 0;
  _comm._spos                 = SPOS();
  _comm._parent_node          = Null_prim_id;
  _comm._attr                 = Null_id;
}

NODE_DATA::NODE_DATA(OPCODE op, const SPOS& spos, TYPE_ID type) {
  AIR_ASSERT(META_INFO::Op_num_child(op) == 0);
  _comm._core._opcode         = static_cast<uint32_t>(op);
  _comm._core._num_added_chld = 0;
  _comm._spos                 = spos;
  _comm._rtype                = type.Value();
  _comm._attr                 = Null_id;
}

NODE_DATA::NODE_DATA(OPCODE op, const SPOS& spos, TYPE_ID type, NODE_ID nid) {
  AIR_ASSERT(META_INFO::Op_num_child(op) == 1);
  _comm._core._opcode                      = static_cast<uint32_t>(op);
  _comm._core._num_added_chld              = 0;
  _comm._spos                              = spos;
  _comm._rtype                             = type.Value();
  _comm._attr                              = Null_id;
  _uu._fields[META_INFO::Op_num_field(op)] = nid.Value();
}

NODE_DATA::NODE_DATA(OPCODE op, const SPOS& spos, TYPE_ID type, NODE_ID nid1,
                     NODE_ID nid2) {
  AIR_ASSERT(META_INFO::Op_num_child(op) == 2);
  _comm._core._opcode                          = static_cast<uint32_t>(op);
  _comm._core._num_added_chld                  = 0;
  _comm._spos                                  = spos;
  _comm._rtype                                 = type.Value();
  _comm._attr                                  = Null_id;
  _uu._fields[META_INFO::Op_num_field(op)]     = nid1.Value();
  _uu._fields[META_INFO::Op_num_field(op) + 1] = nid2.Value();
}

NODE_DATA::NODE_DATA(OPCODE op, const SPOS& spos, TYPE_ID type, NODE_ID nid1,
                     NODE_ID nid2, NODE_ID nid3) {
  AIR_ASSERT(META_INFO::Op_num_child(op) == 3);
  _comm._core._opcode                          = static_cast<uint32_t>(op);
  _comm._core._num_added_chld                  = 0;
  _comm._spos                                  = spos;
  _comm._rtype                                 = type.Value();
  _comm._attr                                  = Null_id;
  _uu._fields[META_INFO::Op_num_field(op)]     = nid1.Value();
  _uu._fields[META_INFO::Op_num_field(op) + 1] = nid2.Value();
  _uu._fields[META_INFO::Op_num_field(op) + 2] = nid3.Value();
}

NODE_DATA::NODE_DATA(OPCODE op, const SPOS& spos, TYPE_ID type, NODE_ID nid1,
                     NODE_ID nid2, NODE_ID nid3, NODE_ID nid4) {
  AIR_ASSERT(META_INFO::Op_num_child(op) == 4);
  _comm._core._opcode                          = static_cast<uint32_t>(op);
  _comm._core._num_added_chld                  = 0;
  _comm._spos                                  = spos;
  _comm._rtype                                 = type.Value();
  _comm._attr                                  = Null_id;
  _uu._fields[META_INFO::Op_num_field(op)]     = nid1.Value();
  _uu._fields[META_INFO::Op_num_field(op) + 1] = nid2.Value();
  _uu._fields[META_INFO::Op_num_field(op) + 2] = nid3.Value();
  _uu._fields[META_INFO::Op_num_field(op) + 3] = nid4.Value();
}

NODE_DATA::NODE_DATA(OPCODE op, const SPOS& spos, TYPE_ID type, NODE_ID nid1,
                     NODE_ID nid2, NODE_ID nid3, NODE_ID nid4, NODE_ID nid5) {
  AIR_ASSERT(META_INFO::Op_num_child(op) == 5);
  _comm._core._opcode                          = static_cast<uint32_t>(op);
  _comm._core._num_added_chld                  = 0;
  _comm._spos                                  = spos;
  _comm._rtype                                 = type.Value();
  _comm._attr                                  = Null_id;
  _uu._fields[META_INFO::Op_num_field(op)]     = nid1.Value();
  _uu._fields[META_INFO::Op_num_field(op) + 1] = nid2.Value();
  _uu._fields[META_INFO::Op_num_field(op) + 2] = nid3.Value();
  _uu._fields[META_INFO::Op_num_field(op) + 3] = nid4.Value();
  _uu._fields[META_INFO::Op_num_field(op) + 4] = nid5.Value();
}

NODE_DATA::NODE_DATA(OPCODE op, const SPOS& spos, TYPE_ID type, NODE_ID nid1,
                     NODE_ID nid2, NODE_ID nid3, NODE_ID nid4, NODE_ID nid5,
                     NODE_ID nid6) {
  AIR_ASSERT(META_INFO::Op_num_child(op) == 6);
  _comm._core._opcode                          = static_cast<uint32_t>(op);
  _comm._core._num_added_chld                  = 0;
  _comm._spos                                  = spos;
  _comm._rtype                                 = type.Value();
  _comm._attr                                  = Null_id;
  _uu._fields[META_INFO::Op_num_field(op)]     = nid1.Value();
  _uu._fields[META_INFO::Op_num_field(op) + 1] = nid2.Value();
  _uu._fields[META_INFO::Op_num_field(op) + 2] = nid3.Value();
  _uu._fields[META_INFO::Op_num_field(op) + 3] = nid4.Value();
  _uu._fields[META_INFO::Op_num_field(op) + 4] = nid5.Value();
  _uu._fields[META_INFO::Op_num_field(op) + 5] = nid6.Value();
}

NODE_DATA::NODE_DATA(const NODE_DATA& node) {
  OPCODE op = OPCODE(node._comm._core._opcode);
  memcpy(this, &node, Size(op, node._comm._core._num_added_chld));
}

size_t NODE_DATA::Size(OPCODE op, uint32_t num) {
  size_t sz = sizeof(COMM) + sizeof(uint32_t) * META_INFO::Op_num_field(op) +
              sizeof(NODE_ID) * META_INFO::Op_num_child(op);
  if (META_INFO::Has_prop<OPR_PROP::EX_CHILD>(op)) {
    return (sz + sizeof(NODE_ID) * num);
  } else {
    AIR_ASSERT(num == 0);
    return sz;
  }
}

}  // namespace base
}  // namespace air
