//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_CG_CGIR_NODE_H
#define AIR_CG_CGIR_NODE_H

#include "air/base/container.h"
#include "air/cg/cgir_data.h"
#include "air/util/cfg_base.h"

namespace air {

namespace cg {

class CGIR_CONTAINER;
class TARG_INFO;

//! @brief OPND
//!  Operand of the instruction
class OPND {
  friend class CGIR_CONTAINER;
  friend class INST;
  friend class air::base::PTR_TO_CONST<OPND>;
  friend class air::base::PTR<OPND>;

public:
  OPND_ID   Id() const { return _data.Id(); }
  OPND_KIND Kind() const { return (OPND_KIND)_data->_kind; }
  bool      Is_register() const { return Kind() == OPND_KIND::REGISTER; }
  bool      Is_immediate() const { return Kind() == OPND_KIND::IMMEDIATE; }
  bool      Is_constant() const { return Kind() == OPND_KIND::CONSTANT; }
  bool      Is_symbol() const { return Kind() == OPND_KIND::SYMBOL; }
  bool      Is_label() const { return Kind() == OPND_KIND::LABEL; }

  //! @brief Get register class for register operand
  REG_CLASS Reg_class() const {
    AIR_ASSERT(Is_register());
    return (REG_CLASS)_data->_u._reg_info._reg_pair[0]._reg_cls;
  }
  //! @brief Get register number for register operand
  uint8_t Reg_num() const {
    AIR_ASSERT(Is_register());
    return _data->_u._reg_info._reg_pair[0]._reg_num;
  }

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

  //! @brief Get label id for label operand
  air::base::LABEL_ID Label() const {
    AIR_ASSERT(Is_label());
    return air::base::LABEL_ID(_data->_id);
  }

public:
  void        Print(std::ostream& os, uint32_t indent = 0,
                    const TARG_INFO* ti = nullptr) const;
  void        Print() const;
  std::string To_str() const;

private:
  OPND() : _cont(nullptr), _data() {}
  OPND(const CGIR_CONTAINER* cont, OPND_DATA_PTR data)
      : _cont(const_cast<CGIR_CONTAINER*>(cont)), _data(data) {}
  bool Is_null() const { return _data.Is_null(); }

  CGIR_CONTAINER* _cont;
  OPND_DATA_PTR   _data;

};  // OPND

//! @brief INST
//!  Instruction
class INST {
  friend class NODE;
  friend class CGIR_CONTAINER;
  friend class air::base::PTR_TO_CONST<INST>;
  friend class air::base::PTR<INST>;

public:
  INST_ID         Id() const { return _data.Id(); }
  air::base::SPOS Spos() const { return _data->_spos; }
  uint32_t        Isa() const { return _data->_isa; }
  uint32_t        Opcode() const { return _data->_opcode; }
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

public:
  void        Print(std::ostream& os, uint32_t indent = 0) const;
  void        Print() const;
  std::string To_str() const;

private:
  INST() : _cont(nullptr), _data() {}
  INST(const CGIR_CONTAINER* cont, INST_DATA_PTR data)
      : _cont(const_cast<CGIR_CONTAINER*>(cont)), _data(data) {}
  bool Is_null() const { return _data.Is_null(); }
  void Set_prev(INST_ID id) { _data->_prev_inst = id; }
  void Set_next(INST_ID id) { _data->_next_inst = id; }
  void Set_res_opnd(uint32_t idx, OPND_ID opnd) {
    _data->_res_opnd[idx] = opnd;
  }

  CGIR_CONTAINER* _cont;
  INST_DATA_PTR   _data;

};  // INST

//! @brief NODE
//!  CFG NODE (Basic Block)
class NODE : public air::util::CFG<NODE_DATA, EDGE_DATA, CGIR_CONTAINER>::NODE {
  friend class CGIR_CONTAINER;
  friend class air::base::PTR_TO_CONST<NODE>;
  friend class air::base::PTR<NODE>;

  using PARENT = air::util::CFG<NODE_DATA, EDGE_DATA, CGIR_CONTAINER>::NODE;
  using NODE_DATA_PTR =
      air::util::CFG<NODE_DATA, EDGE_DATA, CGIR_CONTAINER>::NODE_DATA_PTR;

public:
  INST_ID  First_id() const { return _data->_first_inst; }
  INST_ID  Last_id() const { return _data->_last_inst; }
  INST_PTR First() const;
  INST_PTR Last() const;

  //! @brief prepend inst to node as new first instruction
  void Prepend(INST_PTR inst);

  //! @brief prepend inst to node before pos
  void Prepend(INST_PTR pos, INST_PTR inst);

  //! @brief append inst to node as new last instruction
  void Append(INST_PTR inst);

  //! @brief append inst to node after pos
  void Append(INST_PTR pos, INST_PTR inst);

public:
  void        Print(std::ostream& os, uint32_t indent = 0) const;
  void        Print() const;
  std::string To_str() const;

private:
  NODE() : PARENT() {}
  NODE(const CGIR_CONTAINER* cont, NODE_DATA_PTR data) : PARENT(cont, data) {}

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

};  // NODE

//! @brief EDGE
//!  CFG EDGE (Edge between Basic Blocks)
class EDGE : public air::util::CFG<NODE_DATA, EDGE_DATA, CGIR_CONTAINER>::EDGE {
  friend class CGIR_CONTAINER;
  friend class air::base::PTR_TO_CONST<EDGE>;
  friend class air::base::PTR<EDGE>;

  using PARENT = air::util::CFG<NODE_DATA, EDGE_DATA, CGIR_CONTAINER>::EDGE;
  using EDGE_DATA_PTR =
      air::util::CFG<NODE_DATA, EDGE_DATA, CGIR_CONTAINER>::EDGE_DATA_PTR;

private:
  EDGE() : PARENT() {}
  EDGE(const CGIR_CONTAINER* cont, EDGE_DATA_PTR data) : PARENT(cont, data) {}

};  // EDGE

}  // namespace cg

}  // namespace air

#endif  // AIR_CG_CGIR_NODE_H
