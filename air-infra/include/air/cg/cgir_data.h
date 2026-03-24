//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_CG_CGIR_DATA_H
#define AIR_CG_CGIR_DATA_H

#include "air/base/st.h"
#include "air/base/st_decl.h"
#include "air/cg/cgir_decl.h"
#include "air/cg/cgir_enum.h"
#include "air/util/cfg_data.h"

namespace air {

namespace cg {

// Operand data
class OPND_DATA {
  friend class OPND;
  friend class INST;
  friend class CGIR_CONTAINER;

private:
  // disable copy constructor and assignment operator
  OPND_DATA(const OPND_DATA& rhs)            = delete;
  OPND_DATA& operator=(const OPND_DATA& rhs) = delete;

  // construct a register operand
  OPND_DATA(uint8_t reg_cls, uint8_t reg_num = REG_UNKNOWN, uint32_t ver = 0)
      : _kind(OPND_KIND::REGISTER), _rel_ver(ver), _flag(0), _id(0) {
    _u._val                            = 0;
    _u._reg_info._reg_pair[0]._reg_cls = reg_cls;
    _u._reg_info._reg_pair[0]._reg_num = reg_num;
  }

  // construct a immediate operand
  OPND_DATA(int64_t val)
      : _kind(OPND_KIND::IMMEDIATE), _rel_ver(0), _flag(0), _id(0) {
    _u._val = val;
  }

  // construct a symbol operand
  OPND_DATA(air::base::SYM_ID sym, int64_t ofst = 0, uint16_t flag = 0)
      : _kind(OPND_KIND::SYMBOL), _rel_ver(0), _flag(flag), _id(sym.Value()) {
    _u._val = ofst;
  }

  // construct a constant operand
  OPND_DATA(air::base::CONSTANT_ID cst, int64_t ofst = 0, uint16_t flag = 0)
      : _kind(OPND_KIND::CONSTANT), _rel_ver(0), _flag(flag), _id(cst.Value()) {
    _u._val = ofst;
  }

  // construct a bb operand
  OPND_DATA(BB_ID id)
      : _kind(OPND_KIND::BB), _rel_ver(0), _flag(0), _id(id.Value()) {
    _u._val = 0;
  }

  // for Clone()
  OPND_DATA(const OPND_DATA* data) {
    _kind    = data->_kind;
    _rel_ver = data->_rel_ver;
    _flag    = data->_flag;
    _id      = data->_id;
    _u._val  = data->_u._val;
  }

private:
  OPND_KIND _kind : 4;  // operand kind
  uint16_t
      _rel_ver : 12;  // relocation type for constant/symbol or version for reg
  uint16_t _flag;     // operand flag
  uint32_t _id;       // constant/symbol/label/preg id

  union {
    struct {
      uint32_t _def_inst;  // instruction id which defines the register
      struct {
        uint8_t _reg_cls;  // register class: gpr/fpr/etc
        uint8_t _reg_num;  // physical register number
      } _reg_pair[2];
    } _reg_info;
    int64_t _val;  // immediate value, constant/symbol offset
  } _u;

};  // OPND_DATA

// Instruction data
class INST_DATA {
  friend class INST;
  friend class BB;
  friend class CGIR_CONTAINER;

private:
  // disable copy constructor and assignment operator
  INST_DATA(const INST_DATA& rhs)            = delete;
  INST_DATA& operator=(const INST_DATA& rhs) = delete;

  INST_DATA(air::base::SPOS spos, uint32_t isa, uint32_t op, uint32_t res_cnt,
            uint32_t opnd_cnt)
      : _spos(spos),
        _prev_inst(),
        _next_inst(),
        _isa(isa),
        _operator(op),
        _res_cnt(res_cnt),
        _opnd_cnt(opnd_cnt) {}

  // for Clone()
  INST_DATA(const INST_DATA* data) {
    _spos     = data->_spos;
    _isa      = data->_isa;
    _operator = data->_operator;
    _res_cnt  = data->_res_cnt;
    _opnd_cnt = data->_opnd_cnt;
    _flag     = data->_flag;
  }

  uint32_t Res_opnd_count() const { return _res_cnt + _opnd_cnt; }
  OPND_ID  Res_opnd(uint32_t idx) const { return _res_opnd[idx]; }
  void     Set_res_opnd(uint32_t idx, OPND_ID opnd) { _res_opnd[idx] = opnd; }

private:
  air::base::SPOS _spos;           // source position
  INST_ID         _prev_inst;      // previous instruction
  INST_ID         _next_inst;      // next instruction
  uint32_t        _isa : 8;        // instruction set architecture
  uint32_t        _operator : 16;  // instruction operator
  uint32_t        _res_cnt : 4;    // result count
  uint32_t        _opnd_cnt : 4;   // operand count
  uint32_t        _flag;           // instruction flags
  OPND_ID         _res_opnd[0];    // results and operands

};  // INST_DATA

// Control flow graph node (Basic Block) info
class BB_DATA {
  friend class BB;
  friend class CGIR_CONTAINER;
  friend air::util::CFG<BB_DATA, EDGE_DATA, CGIR_CONTAINER>;

private:
  // disable assignment operator
  BB_DATA& operator=(const BB_DATA& rhs) = delete;

  // for Clone()
  BB_DATA(const BB_DATA* data) {
    _cfg_node_info = data->_cfg_node_info;
    _layour_info   = data->_layour_info;
    _attr          = data->_attr;
  }

public:
  BB_DATA() : _first_inst(), _last_inst(), _attr(0) {}

private:
  ENABLE_CFG_DATA_INFO()  // pred & succ on CFG
  ENABLE_LAYOUT_INFO()    // layout
  INST_ID  _first_inst;   // first instruction in the basic block
  INST_ID  _last_inst;    // last instruction in the basic block
  uint32_t _attr = 0;     // attribute of the basic block
};                        // BB_DATA

// Control flow graph edge info
class EDGE_DATA {
  friend class EDGE;
  friend class CGIR_CONTAINER;
  friend air::util::CFG<BB_DATA, EDGE_DATA, CGIR_CONTAINER>;

private:
  // disable assignment operator
  EDGE_DATA& operator=(const EDGE_DATA& rhs) = delete;

public:
  EDGE_DATA() {}

private:
  ENABLE_CFG_EDGE_INFO()

};  // EDGE_DATA

}  // namespace cg

}  // namespace air

#endif  // AIR_CG_CGIR_DATA_H
