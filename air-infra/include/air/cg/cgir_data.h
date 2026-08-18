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
  OPND_DATA(uint8_t reg_cls, uint8_t reg_num = REG_UNKNOWN)
      : _kind(OPND_KIND::REGISTER), _reloc(0), _flag(0), _id(0) {
    _u._val                            = 0;
    _u._reg_info._reg_pair[0]._reg_cls = reg_cls;
    _u._reg_info._reg_pair[0]._reg_num = reg_num;
  }

  // construct a immediate operand
  OPND_DATA(int64_t val)
      : _kind(OPND_KIND::IMMEDIATE), _reloc(0), _flag(0), _id(0) {
    _u._val = val;
  }

  // construct a symbol operand
  OPND_DATA(air::base::SYM_ID sym, int64_t ofst = 0)
      : _kind(OPND_KIND::SYMBOL), _reloc(0), _flag(0), _id(sym.Value()) {
    _u._val = ofst;
  }

  // construct a constant operand
  OPND_DATA(air::base::CONSTANT_ID cst, int64_t ofst = 0)
      : _kind(OPND_KIND::CONSTANT), _reloc(0), _flag(0), _id(cst.Value()) {
    _u._val = ofst;
  }

  // construct a label operand
  OPND_DATA(air::base::LABEL_ID label)
      : _kind(OPND_KIND::LABEL), _reloc(0), _flag(0), _id(label.Value()) {
    _u._val = 0;
  }

private:
  OPND_KIND _kind : 8;  // operand kind
  uint8_t   _reloc;     // relocation type for constant/symbol
  uint16_t  _flag;      // operand flag
  uint32_t  _id;        // constant/symbol/label id

  union {
    struct {
      uint32_t _spill;  // spill symbol id for the register
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
  friend class NODE;
  friend class CGIR_CONTAINER;

private:
  // disable copy constructor and assignment operator
  INST_DATA(const INST_DATA& rhs)            = delete;
  INST_DATA& operator=(const INST_DATA& rhs) = delete;

  INST_DATA(air::base::SPOS spos, uint32_t isa, uint32_t opcode,
            uint32_t res_cnt, uint32_t opnd_cnt)
      : _spos(spos),
        _prev_inst(),
        _next_inst(),
        _isa(isa),
        _opcode(opcode),
        _res_cnt(res_cnt),
        _opnd_cnt(opnd_cnt) {}

private:
  air::base::SPOS _spos;          // source position
  INST_ID         _prev_inst;     // previous instruction
  INST_ID         _next_inst;     // next instruction
  uint32_t        _isa : 8;       // instruction set architecture
  uint32_t        _opcode : 16;   // instruction opcode
  uint32_t        _res_cnt : 4;   // result count
  uint32_t        _opnd_cnt : 4;  // operand count
  uint32_t        _flag;          // instruction flags
  OPND_ID         _res_opnd[0];   // results and operands

};  // INST_DATA

// Control flow graph node (Basic Block) info
class NODE_DATA {
  friend class NODE;
  friend class CGIR_CONTAINER;
  friend air::util::CFG<NODE_DATA, EDGE_DATA, CGIR_CONTAINER>;

private:
  // disable assignment operator
  NODE_DATA& operator=(const NODE_DATA& rhs) = delete;

public:
  NODE_DATA() : _first_inst(), _last_inst() {}

private:
  ENABLE_CFG_DATA_INFO()
  INST_ID _first_inst;  // first instruction in the basic block
  INST_ID _last_inst;   // last instruction in the basic block

};  // NODE_DATA

// Control flow graph edge info
class EDGE_DATA {
  friend class EDGE;
  friend class CGIR_CONTAINER;
  friend air::util::CFG<NODE_DATA, EDGE_DATA, CGIR_CONTAINER>;

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
