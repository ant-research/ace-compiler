//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_CG_CGIR_NODE_H
#define AIR_CG_CGIR_NODE_H

#include "air/base/bb_enum.h"
#include "air/base/container.h"
#include "air/base/st_decl.h"
#include "air/cg/cgir_data.h"
#include "air/cg/cgir_decl.h"
#include "air/cg/cgir_enum.h"
#include "air/cg/targ_info.h"
#include "air/util/cfg_base.h"
#include "air/util/debug.h"

namespace air {

namespace cg {

class CGIR_CONTAINER;
class ISA_INFO_META;

//! @brief OPND
//!  Operand of the instruction
class OPND {
  friend class CGIR_CONTAINER;
  friend class INST;
  friend class air::base::PTR_TO_CONST<OPND>;
  friend class air::base::PTR<OPND>;
  friend class SSA_BUILD_CTX;

public:
  OPND_ID   Id() const { return _data.Id(); }
  OPND_KIND Kind() const { return (OPND_KIND)_data->_kind; }
  bool      Is_register() const { return Kind() == OPND_KIND::REGISTER; }
  bool Is_preg() const { return Is_register() && Reg_num() == REG_UNKNOWN; }
  bool Is_immediate() const { return Kind() == OPND_KIND::IMMEDIATE; }
  bool Is_constant() const { return Kind() == OPND_KIND::CONSTANT; }
  bool Is_symbol() const { return Kind() == OPND_KIND::SYMBOL; }
  bool Is_bb() const { return Kind() == OPND_KIND::BB; }

  //! @brief Get register class for register operand
  uint8_t Reg_class() const {
    AIR_ASSERT(Is_register());
    return _data->_u._reg_info._reg_pair[0]._reg_cls;
  }
  //! @brief Get register number for register operand
  uint8_t Reg_num() const {
    AIR_ASSERT(Is_register());
    return _data->_u._reg_info._reg_pair[0]._reg_num;
  }
  void Set_reg_num(uint8_t reg_num) {
    _data->_u._reg_info._reg_pair[0]._reg_num = reg_num;
  }
  //! @brief Get version for register opnd in SSA form
  uint32_t Version() const {
    AIR_ASSERT(Is_register());
    return _data->_rel_ver;
  }
  //! @brief Get def_inst for register opnd in SSA form
  INST_ID Def_inst_id() const {
    AIR_ASSERT(Is_register());
    return INST_ID(_data->_u._reg_info._def_inst);
  }
  //! @brief Get def_inst for register opnd in SSA form
  INST_PTR Def_inst() const;

  //! @brief Get operand flag
  uint16_t Opnd_flag(void) const { return _data->_flag; }

  //! @brief Get immediate value for immediate operand
  int64_t Immediate() const {
    AIR_ASSERT(Is_immediate());
    return _data->_u._val;
  }

  //! @brief Get offset for symbol or constant operand
  int64_t Offset() const {
    AIR_ASSERT(Is_constant() || Is_symbol());
    return _data->_u._val;
  }
  //! @brief Get constant id for constant operand
  air::base::CONSTANT_ID Constant() const {
    AIR_ASSERT(Is_constant());
    return air::base::CONSTANT_ID(_data->_id);
  }
  //! @brief Get symbol id for symbol operand
  air::base::SYM_ID Symbol() const {
    AIR_ASSERT(Is_symbol());
    return air::base::SYM_ID(_data->_id);
  }
  //! @brief Get preg id for preg operand
  air::base::PREG_ID Preg() const {
    AIR_ASSERT(Is_register() && Reg_num() == REG_UNKNOWN);
    return base::PREG_ID(_data->_id);
  }
  //! @brief Set preg for preg operand
  void Set_preg(base::PREG_PTR preg) {
    AIR_ASSERT(Is_register() && Reg_num() == REG_UNKNOWN);
    _data->_id = preg->Id().Value();
  }

  //! @brief Get bb id for label operand
  BB_ID Bb() const {
    AIR_ASSERT(Is_bb());
    return BB_ID(_data->_id);
  }

public:
  void        Print(std::ostream& os, uint32_t indent = 0,
                    const ISA_INFO_META* ti = nullptr) const;
  void        Print() const;
  std::string To_str() const;

private:
  OPND() : _cont(nullptr), _data() {}
  OPND(const CGIR_CONTAINER* cont, OPND_DATA_PTR data)
      : _cont(const_cast<CGIR_CONTAINER*>(cont)), _data(data) {}
  OPND_DATA_PTR Data() const { return _data; }
  bool          Is_null() const { return _data.Is_null(); }

  void Set_def(INST_ID def, uint32_t ver) {
    AIR_ASSERT(Is_register());
    _data->_rel_ver               = ver;
    _data->_u._reg_info._def_inst = def.Value();
  }

  CGIR_CONTAINER* _cont;
  OPND_DATA_PTR   _data;

};  // OPND

//! @brief INST
//!  Instruction
class INST {
  friend class BB;
  friend class CGIR_CONTAINER;
  friend class air::base::PTR_TO_CONST<INST>;
  friend class air::base::PTR<INST>;

public:
  INST_ID         Id() const { return _data.Id(); }
  INST_DATA_PTR   Data() const { return _data; }
  air::base::SPOS Spos() const { return _data->_spos; }
  uint32_t        Isa() const { return _data->_isa; }
  uint32_t        Operator() const { return _data->_operator; }
  uint32_t        Opcode() const { return (Isa() << 16) + Operator(); }
  uint32_t        Res_count() const { return _data->_res_cnt; }
  uint32_t        Opnd_count() const { return _data->_opnd_cnt; }
  INST_PTR        Prev() const;
  INST_PTR        Next() const;
  INST_ID         Prev_id() const { return _data->_prev_inst; }
  INST_ID         Next_id() const { return _data->_next_inst; }
  OPND_PTR        Opnd(uint32_t idx) const;
  OPND_PTR        Res(uint32_t idx) const;

  OPND_ID Opnd_id(uint32_t idx) const {
    AIR_ASSERT(idx < Opnd_count());
    return _data->_res_opnd[Res_count() + idx];
  }
  OPND_ID Res_id(uint32_t idx) const {
    AIR_ASSERT(idx < Res_count());
    return _data->_res_opnd[idx];
  }

  void Set_opnd(uint32_t idx, OPND_ID opnd);
  void Set_res(uint32_t idx, OPND_ID res);
  void Set_opnd(uint32_t idx, OPND_PTR opnd) { Set_opnd(idx, opnd->Id()); }
  void Set_res(uint32_t idx, OPND_PTR res) { Set_res(idx, res->Id()); }

  //! @brief return true if operation is call; otherwise, return false.
  bool Is_call(void) const {
    const ISA_INFO_META* isa_meta = TARG_INFO_MGR::Isa_info(Isa());
    return isa_meta->Is_call(Operator());
  }
  //! @brief return true if operation is return; otherwise, return false.
  bool Is_return(void) const {
    const ISA_INFO_META* isa_meta = TARG_INFO_MGR::Isa_info(Isa());
    return isa_meta->Is_return(Operator());
  }
  //! @brief return true if operation is terminator; otherwise, return false.
  bool Is_terminator(void) const {
    const ISA_INFO_META* isa_meta = TARG_INFO_MGR::Isa_info(Isa());
    return isa_meta->Is_terminator(Operator());
  }
  //! @brief return true if operation is branch; otherwise, return false.
  bool Is_branch(void) const {
    const ISA_INFO_META* isa_meta = TARG_INFO_MGR::Isa_info(Isa());
    return isa_meta->Is_branch(Operator());
  }
  //! @brief return true if operation is comparison; otherwise, return false.
  bool Is_cmp(void) const {
    const ISA_INFO_META* isa_meta = TARG_INFO_MGR::Isa_info(Isa());
    return isa_meta->Is_cmp(Operator());
  }
  //! @brief return true if src opnds of operation is commutable;
  //! otherwise, return false.
  bool Is_commutable(void) const {
    const ISA_INFO_META* isa_meta = TARG_INFO_MGR::Isa_info(Isa());
    return isa_meta->Is_commutable(Operator());
  }

public:
  void        Print(std::ostream& os, uint32_t indent = 0) const;
  void        Print() const;
  std::string To_str() const;

private:
  INST() : _cont(nullptr), _data() {}
  INST(const CGIR_CONTAINER* cont, INST_DATA_PTR data)
      : _cont(const_cast<CGIR_CONTAINER*>(cont)), _data(data) {}
  bool     Is_null() const { return _data.Is_null(); }
  void     Set_prev(INST_ID id) { _data->_prev_inst = id; }
  void     Set_next(INST_ID id) { _data->_next_inst = id; }
  OPND_PTR Res_opnd(uint32_t idx) const;
  void     Set_res_opnd(uint32_t idx, OPND_ID opnd) {
    _data->_res_opnd[idx] = opnd;
  }

  CGIR_CONTAINER* _cont;
  INST_DATA_PTR   _data;

};  // INST

//! @brief BB
//!  CFG NODE (Basic Block)
class BB : public air::util::CFG<BB_DATA, EDGE_DATA, CGIR_CONTAINER>::NODE {
  friend class CGIR_CONTAINER;
  friend class air::base::PTR_TO_CONST<BB>;
  friend class air::base::PTR<BB>;

  using PARENT = air::util::CFG<BB_DATA, EDGE_DATA, CGIR_CONTAINER>::NODE;
  using BB_DATA_PTR =
      air::util::CFG<BB_DATA, EDGE_DATA, CGIR_CONTAINER>::NODE_DATA_PTR;

public:
  air::base::SPOS Spos() const { return air::base::SPOS(); }

  INST_ID  First_id() const { return _data->_first_inst; }
  INST_ID  Last_id() const { return _data->_last_inst; }
  INST_PTR First() const;
  INST_PTR Last() const;

  bool Empty() const {
    if (_data->_first_inst == air::base::Null_id) {
      AIR_ASSERT(_data->_last_inst == air::base::Null_id);
      return true;
    } else {
      AIR_ASSERT(_data->_last_inst != air::base::Null_id);
      return false;
    }
  }

  //! @brief prepend inst to node as new first instruction
  void Prepend(INST_PTR inst);

  //! @brief prepend inst to node before pos
  void Prepend(INST_PTR pos, INST_PTR inst);

  //! @brief append inst to node as new last instruction
  void Append(INST_PTR inst);

  //! @brief append inst to node after pos
  void Append(INST_PTR pos, INST_PTR inst);

  //! @brief remove inst for current basic block
  void Remove(INST_PTR inst);

  //! @brief add attribute to current basic block.
  void Set_attr(uint32_t attr) { _data->_attr |= attr; }
  //! @brief return true if current basic block has attribute attr; otherwise
  //! return false.
  bool Has_attr(uint32_t attr) const { return (_data->_attr & attr) == attr; }

public:
  const air::util::LAYOUT_INFO* Layout_info() const {
    return _data->Layout_info();
  }

  BB_ID  Next_id() const { return BB_ID(_data->Layout_info()->Next()); }
  BB_ID  Prev_id() const { return BB_ID(_data->Layout_info()->Prev()); }
  BB_PTR Next() const;
  BB_PTR Prev() const;

public:
  void        Print(std::ostream& os, uint32_t indent = 0) const;
  void        Print() const;
  std::string To_str() const;

private:
  BB() : PARENT() {}
  BB(const CGIR_CONTAINER* cont, BB_DATA_PTR data) : PARENT(cont, data) {}

  void Set_first(INST_ID id) {
    _data->_first_inst = id;
    if (_data->_last_inst == air::base::Null_id) {
      _data->_last_inst = id;
    }
  }

  void Set_last(INST_ID id) {
    _data->_last_inst = id;
    if (_data->_first_inst == air::base::Null_id) {
      _data->_first_inst = id;
    }
  }

  air::util::LAYOUT_INFO* Layout_info() { return _data->Layout_info(); }

};  // BB

//! @brief EDGE
//!  CFG EDGE (Edge between Basic Blocks)
class EDGE : public air::util::CFG<BB_DATA, EDGE_DATA, CGIR_CONTAINER>::EDGE {
  friend class CGIR_CONTAINER;
  friend class air::base::PTR_TO_CONST<EDGE>;
  friend class air::base::PTR<EDGE>;

  using PARENT = air::util::CFG<BB_DATA, EDGE_DATA, CGIR_CONTAINER>::EDGE;
  using EDGE_DATA_PTR =
      air::util::CFG<BB_DATA, EDGE_DATA, CGIR_CONTAINER>::EDGE_DATA_PTR;

private:
  EDGE() : PARENT() {}
  EDGE(const CGIR_CONTAINER* cont, EDGE_DATA_PTR data) : PARENT(cont, data) {}

};  // EDGE

}  // namespace cg

}  // namespace air

#endif  // AIR_CG_CGIR_NODE_H
