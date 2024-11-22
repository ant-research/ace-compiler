//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_BASE_NODE_DATA_H
#define AIR_BASE_NODE_DATA_H

#include "air/base/container_decl.h"
#include "air/base/opcode.h"
#include "air/base/spos.h"
#include "air/base/st_decl.h"

namespace air {
namespace base {

class NODE_DATA {
public:
  friend class NODE;
  friend class STMT;
  friend class STMT_DATA;
  friend class CONTAINER;

private:
  NODE_DATA();
  NODE_DATA(OPCODE opc, const SPOS& spos, TYPE_ID tid);
  NODE_DATA(OPCODE opc, const SPOS& spos, TYPE_ID tid, NODE_ID nid);
  NODE_DATA(OPCODE opc, const SPOS& spos, TYPE_ID tid, NODE_ID nid1,
            NODE_ID nid2);
  NODE_DATA(OPCODE opc, const SPOS& spos, TYPE_ID tid, NODE_ID nid1,
            NODE_ID nid2, NODE_ID nid3);
  NODE_DATA(OPCODE opc, const SPOS& spos, TYPE_ID tid, NODE_ID nid1,
            NODE_ID nid2, NODE_ID nid3, NODE_ID nid4);
  NODE_DATA(OPCODE opc, const SPOS& spos, TYPE_ID tid, NODE_ID nid1,
            NODE_ID nid2, NODE_ID nid3, NODE_ID nid4, NODE_ID nid5);
  NODE_DATA(OPCODE opc, const SPOS& spos, TYPE_ID tid, NODE_ID nid1,
            NODE_ID nid2, NODE_ID nid3, NODE_ID nid4, NODE_ID nid5,
            NODE_ID nid6);

  NODE_DATA(const NODE_DATA& node);
  NODE_DATA& operator=(const NODE_DATA&);  // REQUIRED UNDEFINED UNWANTED method

  static size_t Size(OPCODE opc, uint32_t num);

  struct COMM {
    struct {
      uint32_t _opcode : 16;
      uint32_t _num_added_chld : 16;  //!< extra number of child needed
    } _core;

    union {
      uint32_t _rtype;        //!< result type of an express node
      uint32_t _parent_node;  //!< parent block of an statement node
      uint32_t _parent_stmt;  //!< parent statement of a block node
    };

    SPOS    _spos;
    ATTR_ID _attr;
  } _comm;

  union {
    uint32_t _fields[1];

    union {
      uint32_t _const;
      uint32_t _str;
      uint32_t _addr_datum;
      uint32_t _entry;
      uint32_t _label;
      uint32_t _subtype;
      uint32_t _blk;
      uint32_t _preg;
      uint32_t _iv;
    };

    union {
      struct {
        uint32_t _begin_stmt;
        uint32_t _end_stmt;
      } _block;
      struct {
        uint32_t _val;
        uint32_t _dummy_padding;
      } _int_const;
      struct {
        uint32_t _dummy_datum;
        uint32_t _atype;
      } _mem_acc;
    } _u2;

    union {
      struct {
        uint32_t _dummy_ent;
        uint32_t _retv;
        uint32_t _flag;
      } _call;
      struct {
        uint32_t _dummy_datum;
        uint32_t _dummy_atype;
        uint32_t _fld;
      } _fld_acc;
    } _u3;

    union {
      struct {
        uint32_t _dummy_datum;
        uint32_t _dummy_atype;
        uint32_t _ofst;
        uint32_t _dummy_padding;
      } _ofst_acc;
    } _u4;
  } _uu;
};

class STMT_DATA {
public:
  friend class NODE;
  friend class STMT;
  friend class CONTAINER;

private:
  STMT_DATA() : _prev(Null_id), _next(Null_id), _data() {}
  STMT_DATA(OPCODE opc, const SPOS& spos)
      : _prev(Null_id), _next(Null_id), _data(opc, spos, TYPE_ID()) {}
  STMT_DATA(OPCODE opc, const SPOS& spos, NODE_ID nid)
      : _prev(Null_id), _next(Null_id), _data(opc, spos, TYPE_ID(), nid) {}
  STMT_DATA(OPCODE opc, const SPOS& spos, NODE_ID nid1, NODE_ID nid2)
      : _prev(Null_id),
        _next(Null_id),
        _data(opc, spos, TYPE_ID(), nid1, nid2) {}
  STMT_DATA(OPCODE opc, const SPOS& spos, NODE_ID nid1, NODE_ID nid2,
            NODE_ID nid3)
      : _prev(Null_id),
        _next(Null_id),
        _data(opc, spos, TYPE_ID(), nid1, nid2, nid3) {}
  STMT_DATA(OPCODE opc, const SPOS& spos, NODE_ID nid1, NODE_ID nid2,
            NODE_ID nid3, NODE_ID nid4)
      : _prev(Null_id),
        _next(Null_id),
        _data(opc, spos, TYPE_ID(), nid1, nid2, nid3, nid4) {}
  STMT_DATA(OPCODE opc, const SPOS& spos, NODE_ID nid1, NODE_ID nid2,
            NODE_ID nid3, NODE_ID nid4, NODE_ID nid5)
      : _prev(Null_id),
        _next(Null_id),
        _data(opc, spos, TYPE_ID(), nid1, nid2, nid3, nid4, nid5) {}
  STMT_DATA(OPCODE opc, const SPOS& spos, NODE_ID nid1, NODE_ID nid2,
            NODE_ID nid3, NODE_ID nid4, NODE_ID nid5, NODE_ID nid6)
      : _prev(Null_id),
        _next(Null_id),
        _data(opc, spos, TYPE_ID(), nid1, nid2, nid3, nid4, nid5, nid6) {}

  STMT_DATA(const STMT_DATA& node)
      : _prev(node._prev), _next(node._next), _data(node._data) {}
  STMT_DATA& operator=(const STMT_DATA&);  // REQUIRED UNDEFINED UNWANTED method

  STMT_ID   _prev;
  STMT_ID   _next;
  NODE_DATA _data;
};

}  // namespace base
}  // namespace air

#endif
