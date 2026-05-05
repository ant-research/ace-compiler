//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_CG_TARG_INFO_H
#define AIR_CG_TARG_INFO_H

#include <ostream>
#include <string>
#include <vector>

#include "air/cg/cgir_enum.h"
#include "air/util/debug.h"

namespace air {

namespace cg {

//! @brief REG_META
//!  define register in a register class
struct REG_META {
  const char* _name;       //!< register name
  uint16_t    _code;       //!< register encoding (reserved for assembler)
  uint16_t    _size_mask;  //!< allowed data width in this register
  uint32_t    _flags;      //!< register flags

  void        Print(std::ostream& os, uint32_t indent) const;
  void        Print() const;
  std::string To_str() const;
};

//! @brief REG_INFO_META
//!  define register class
struct REG_INFO_META {
  const char*            _name;       //!< register class name
  uint8_t                _reg_cls;    //!< register class id
  uint8_t                _reg_num;    //!< number of registers in this class
  uint16_t               _size_mask;  //!< allowed data width in this class
  uint32_t               _flags;      //!< register class flags
  const struct REG_META* _reg_meta;   //!< register meta info in this class

  //! @brief get REG_CLASS name
  const char* Reg_class_name() const { return _name; }

  //! @brief get REG name
  const char* Reg_name(uint8_t reg_num) const {
    AIR_ASSERT(reg_num < _reg_num);
    return _reg_meta[reg_num]._name;
  }
  //! @brief return REG_META of register reg_num
  const struct REG_META& Reg_meta(uint8_t reg_num) const {
    AIR_ASSERT(reg_num < _reg_num);
    return _reg_meta[reg_num];
  }
  //! @brief return number of registers in current class
  uint8_t Reg_num(void) const { return _reg_num; }
  //! @brief return true if the register is allocatable;
  //! otherwise, return false.
  bool Is_allocatable(uint32_t reg_num) const {
    return Reg_meta(reg_num)._flags & REG_ALLOCATABLE;
  }
  //! @brief return true if the register is parameter;
  //! otherwise, return false.
  bool Is_param(uint32_t reg_num) const {
    return Reg_meta(reg_num)._flags & REG_PARAMETER;
  }
  //! @brief return true if the register is caller saved;
  //! otherwise, return false.
  bool Is_caller_save(uint32_t reg_num) const {
    return Reg_meta(reg_num)._flags & REG_SCRATCH;
  }
  //! @brief return true if the register is callee saved;
  //! otherwise, return false.
  bool Is_callee_save(uint32_t reg_num) const {
    return Reg_meta(reg_num)._flags & REG_PRESERVED;
  }
  //! @brief return true if the register is read only;
  //! otherwise, return false.
  bool Is_readonly(uint32_t reg_num) const {
    return Reg_meta(reg_num)._flags & REG_READONLY;
  }
  //! @brief return true if the register is zero;
  //! otherwise, return false.
  bool Is_zero(uint32_t reg_num) const {
    return Reg_meta(reg_num)._flags & REG_ZERO;
  }

  uint32_t    Id() const { return _reg_cls; }
  void        Print(std::ostream& os, uint32_t indent) const;
  void        Print() const;
  std::string To_str() const;
};

//! @brief OPND_META
//!  define operand used by an instruction
struct OPND_META {
  OPND_KIND _opnd_kind : 8;  //!< operand kind
  uint8_t   _reg_cls;        //!< register class for register operand
  uint8_t   _reg_num;        //!< register number for register operand
  uint8_t   _size_mask;      //!< allowed data width of the operand

  void        Print(std::ostream& os, uint32_t indent) const;
  void        Print() const;
  std::string To_str() const;
};

//! @brief OPND_FLAG_META
//! define operand flag
struct OPND_FLAG_META {
  uint16_t    _id;    //!< opnd flag id
  const char* _desc;  //!< opnd flag description

  void        Print(std::ostream& os, uint32_t indent) const;
  void        Print() const;
  std::string To_str() const;
};

//! @brief INST_META
//!  define instruction in the ISA
struct INST_META {
  const char* _name;      //!< instruction name
  uint16_t    _code;      //!< instruction encoding (reserved for assembler)
  uint8_t     _res_num;   //!< number of result
  uint8_t     _opnd_num;  //!< number of operand
  uint32_t    _flag;      //!< flags identifying instruction class
  const struct OPND_META _res_opnd[];  //!< OPND_META for each res/opnd

  void        Print(std::ostream& os, uint32_t indent) const;
  void        Print() const;
  std::string To_str() const;
};

//! @brief ISA_INFO_META
//!  define the instruction set architecture info
struct ISA_INFO_META {
  const char*                    _name;            //!< target name
  uint16_t                       _isa_id;          //!< target ISA id
  uint16_t                       _inst_num;        //!< number of instructions
  uint16_t                       _opnd_flag_num;   //!< number of opnd flags
  const struct INST_META* const* _inst_meta;       //!< instruction meta info
  const struct OPND_FLAG_META*   _opnd_flag_meta;  //!< opnd flag meta info

  //! @brief get INST op name
  const char* Op_name(uint32_t oper) const {
    AIR_ASSERT(oper < _inst_num);
    return _inst_meta[oper]->_name;
  }

  //! @brief get INST result number
  uint32_t Res_num(uint32_t oper) const {
    AIR_ASSERT(oper < _inst_num);
    return _inst_meta[oper]->_res_num;
  }

  //! @brief get INST operand number
  uint32_t Opnd_num(uint32_t oper) const {
    AIR_ASSERT(oper < _inst_num);
    return _inst_meta[oper]->_opnd_num;
  }

  //! @brief get INST result and operand number
  uint32_t Res_opnd_num(uint32_t oper) const {
    AIR_ASSERT(oper < _inst_num);
    return _inst_meta[oper]->_res_num + _inst_meta[oper]->_opnd_num;
  }
  //! @brief return true if operation is call; otherwise, return false.
  bool Is_call(uint32_t oper) const {
    AIR_ASSERT(oper < _inst_num);
    return _inst_meta[oper]->_flag & INST_CALL;
  }
  //! @brief return true if operation src opnds are commutable;
  //! otherwise, return false.
  bool Is_commutable(uint32_t oper) const {
    AIR_ASSERT(oper < _inst_num);
    return _inst_meta[oper]->_flag & INST_COMMUTABLE;
  }
  //! @brief return true if operation is return; otherwise, return false.
  bool Is_return(uint32_t oper) const {
    AIR_ASSERT(oper < _inst_num);
    return _inst_meta[oper]->_flag & INST_RETURN;
  }
  //! @brief return true if operation is return; otherwise, return false.
  bool Is_terminator(uint32_t oper) const {
    AIR_ASSERT(oper < _inst_num);
    return _inst_meta[oper]->_flag & INST_TERMINATOR;
  }
  //! @brief return true if operation is branch; otherwise, return false.
  bool Is_branch(uint32_t oper) const {
    AIR_ASSERT(oper < _inst_num);
    return _inst_meta[oper]->_flag & INST_BRANCH;
  }
  //! @brief return true if operation is comparison; otherwise, return false.
  bool Is_cmp(uint32_t oper) const {
    AIR_ASSERT(oper < _inst_num);
    return _inst_meta[oper]->_flag & INST_CMP;
  }

  //! @brief get INST operand flag desc
  const char* Opnd_flag(uint32_t flag) const {
    for (uint32_t i = 0; i < _opnd_flag_num; ++i) {
      if (_opnd_flag_meta[i]._id == flag) {
        return _opnd_flag_meta[i]._desc;
      }
    }
    AIR_ASSERT_MSG(false, "Failed finding target operand flag.");
    return nullptr;
  }

  uint32_t    Id() const { return _isa_id; }
  void        Print(std::ostream& os, uint32_t indent) const;
  void        Print() const;
  std::string To_str() const;
};

//! @brief REG_CONV_META
//!  define register convention for parameter passing and return value
struct REG_CONV_META {
  uint8_t _reg_cls;            //!< register class
  uint8_t _reg_zero;           //!< constant ZERO register in this class
  uint8_t _num_param;          //!< number of register for parameter passing
  uint8_t _num_retv;           //!< number of register for return value
  uint8_t _param_retv_regs[];  //!< register number for param and return value

  uint8_t Parm_reg(uint8_t idx) const {
    AIR_ASSERT(idx < _num_param);
    return _param_retv_regs[idx];
  }
  uint8_t Retv_reg(uint8_t idx) const {
    AIR_ASSERT(idx < _num_retv);
    return _param_retv_regs[_num_param + idx];
  }
  void        Print(std::ostream& os, uint32_t indent) const;
  void        Print() const;
  std::string To_str() const;
};

//! @brief ABI_INFO_META
//!  define register convention for ABI
struct ABI_INFO_META {
  const char* _name;    //!< ABI name
  uint8_t     _abi_id;  //!< ABI id
  uint8_t _gpr_cls;  //!< general purpose register class for fp/sp/gp/tp/ra/zero
  uint8_t _fp;       //!< frame pointer. REG_UNKNOWN means no FP
  uint8_t _sp;       //!< stack pointer
  uint8_t _gp;       //!< global pointer
  uint8_t _tp;       //!< thread pointer
  uint8_t _ra;       //!< return addr
  uint8_t _reg_conv_num;  //!< number of REG_CONV_META in this ABI
  const struct REG_CONV_META* const*
      _reg_conv_meta;  //!< REG_CONV_META meta in this ABI
  //! @brief return the idx-th return value register in class rc.
  uint8_t Retv_reg(uint8_t rc, uint8_t idx) const {
    for (uint32_t reg_conv_id = 0; reg_conv_id < _reg_conv_num; ++reg_conv_id) {
      if (_reg_conv_meta[reg_conv_id]->_reg_cls == rc) {
        return (*_reg_conv_meta)[reg_conv_id].Retv_reg(idx);
      }
    }
    AIR_ASSERT_MSG(false, "REG_CONV_META has not been registered");
    return REG_UNKNOWN;
  }
  //! @brief return the idx-th parameter register in class rc.
  uint8_t Parm_reg(uint8_t rc, uint8_t idx) const {
    for (uint32_t reg_conv_id = 0; reg_conv_id < _reg_conv_num; ++reg_conv_id) {
      if (_reg_conv_meta[reg_conv_id]->_reg_cls == rc) {
        return (*_reg_conv_meta)[reg_conv_id].Parm_reg(idx);
      }
    }
    AIR_ASSERT_MSG(false, "REG_CONV_META has not been registered");
    return REG_UNKNOWN;
  }

  uint32_t    Id() const { return _abi_id; }
  void        Print(std::ostream& os, uint32_t indent) const;
  void        Print() const;
  std::string To_str() const;
};

//! @brief TARG_INFO_MGR
//!  manager manages all targets
class TARG_INFO_MGR {
  using REG_INFO_TAB = std::vector<const REG_INFO_META*>;
  using ISA_INFO_TAB = std::vector<const ISA_INFO_META*>;
  using ABI_INFO_TAB = std::vector<const ABI_INFO_META*>;

public:
  //! @brief register REG info
  static bool Register_reg_info(const REG_INFO_META* reg_info);

  //! @brief register ISA info
  static bool Register_isa_info(const ISA_INFO_META* isa_info);

  //! @brief register ABI info
  static bool Register_abi_info(const ABI_INFO_META* abi_info);

  //! @brief get REG_INFO_META by register class
  static const REG_INFO_META* Reg_info(uint8_t cls) {
    return Get_info(Reg_info_tab(), cls);
  }

  //! @brief get ISA_INFO_META by ISA id
  static const ISA_INFO_META* Isa_info(uint8_t isa) {
    return Get_info(Isa_info_tab(), isa);
  }

  //! @brief get ABI_INFO_META by ABI id
  static const ABI_INFO_META* Abi_info(uint8_t abi) {
    return Get_info(Abi_info_tab(), abi);
  }

  //! @brief get register class name
  static const char* Reg_class_name(uint8_t cls) {
    const REG_INFO_TAB& tab = Reg_info_tab();
    if (cls >= tab.size() || tab[cls] == nullptr) {
      return "*cls*";
    } else {
      return tab[cls]->Reg_class_name();
    }
  }

  //! @brief get register name
  static const char* Reg_name(uint8_t cls, uint8_t num) {
    const REG_INFO_TAB& tab = Reg_info_tab();
    if (cls >= tab.size() || tab[cls] == nullptr) {
      return "*reg*";
    } else {
      return tab[cls]->Reg_name(num);
    }
  }

  //! @brief get op name
  static const char* Op_name(uint8_t isa, uint32_t oper) {
    const ISA_INFO_TAB& tab = Isa_info_tab();
    if (isa >= tab.size() || tab[isa] == nullptr) {
      return "*op*";
    } else {
      return tab[isa]->Op_name(oper);
    }
  }

  //! @brief get op result number
  static const uint32_t Op_res_num(uint8_t isa, uint32_t oper) {
    const ISA_INFO_TAB& tab = Isa_info_tab();
    AIR_ASSERT(isa < tab.size() && tab[isa] != nullptr);
    return tab[isa]->Res_num(oper);
  }

  //! @brief get op operand number
  static const uint32_t Op_opnd_num(uint8_t isa, uint32_t oper) {
    const ISA_INFO_TAB& tab = Isa_info_tab();
    AIR_ASSERT(isa < tab.size() && tab[isa] != nullptr);
    return tab[isa]->Res_num(oper);
  }

  //! @brief get op result and operand number
  static const uint32_t Op_res_opnd_num(uint8_t isa, uint32_t oper) {
    const ISA_INFO_TAB& tab = Isa_info_tab();
    AIR_ASSERT(isa < tab.size() && tab[isa] != nullptr);
    return tab[isa]->Res_opnd_num(oper);
  }

  static void        Print(std::ostream& os, uint32_t indent);
  static void        Print();
  static std::string To_str();

private:
  // disable copy constructor and assign operator
  TARG_INFO_MGR(const TARG_INFO_MGR& mgr)            = delete;
  TARG_INFO_MGR& operator=(const TARG_INFO_MGR& mgr) = delete;

  TARG_INFO_MGR() {}
  static TARG_INFO_MGR* Instance();

  template <typename T>
  static bool Register_info(std::vector<const T*>& vec, const T* info) {
    if (vec.size() <= info->Id()) {
      vec.resize(info->Id() + 2);
    } else if (vec[info->Id()] != nullptr) {
      AIR_ASSERT_MSG(vec[info->Id()] == info, "Info %d already registered",
                     info->Id());
      return false;
    }
    vec[info->Id()] = info;
    return true;
  }

  template <typename T>
  static const T* Get_info(const std::vector<const T*>& vec, uint32_t id) {
    AIR_ASSERT(id < vec.size());
    return vec[id];
  }

  static REG_INFO_TAB& Reg_info_tab() {
    TARG_INFO_MGR* inst = Instance();
    AIR_ASSERT(inst != nullptr);
    return inst->_reg_info;
  }

  static ISA_INFO_TAB& Isa_info_tab() {
    TARG_INFO_MGR* inst = Instance();
    AIR_ASSERT(inst != nullptr);
    return inst->_isa_info;
  }

  static ABI_INFO_TAB& Abi_info_tab() {
    TARG_INFO_MGR* inst = Instance();
    AIR_ASSERT(inst != nullptr);
    return inst->_abi_info;
  }

  REG_INFO_TAB _reg_info;  // all REGs
  ISA_INFO_TAB _isa_info;  // all ISAs
  ABI_INFO_TAB _abi_info;  // all ABIs
};

//! @brief helper class to register REG_INFO_META in constructor
class REG_INFO_REGISTER {
public:
  //! @brief register the reg in constructor
  REG_INFO_REGISTER(const REG_INFO_META* info) {
    bool ret = TARG_INFO_MGR::Register_reg_info(info);
    AIR_ASSERT(ret == true);
  }
};

//! @brief helper class to register ISA_INFO_META in constructor
class ISA_INFO_REGISTER {
public:
  //! @brief register the isa in constructor
  ISA_INFO_REGISTER(const ISA_INFO_META* info) {
    bool ret = TARG_INFO_MGR::Register_isa_info(info);
    AIR_ASSERT(ret == true);
  }
};

//! @brief helper class to register ABI_INFO_META in constructor
class ABI_INFO_REGISTER {
public:
  //! @brief register the abi in constructor
  ABI_INFO_REGISTER(const ABI_INFO_META* info) {
    bool ret = TARG_INFO_MGR::Register_abi_info(info);
    AIR_ASSERT(ret == true);
  }
};

//! @brief helper class to print register
class REG_DESC {
public:
  //! @brief print register info
  void Print(std::ostream& os) const {
    const REG_INFO_META* ri = TARG_INFO_MGR::Reg_info(_reg_cls);
    if (_reg_num != REG_UNKNOWN) {
      if (ri != nullptr) {
        os << ri->Reg_name(_reg_num);
      } else {
        os << 'c' << (uint32_t)_reg_cls << '#' << (uint32_t)_reg_num;
      }
    } else {
      if (ri != nullptr) {
        os << ri->Reg_class_name() << '*';
      } else {
        os << 'c' << (uint32_t)_reg_cls << '*';
      }
    }
  }

  //! @brief construct REG_DESC object
  constexpr REG_DESC(uint8_t reg_cls, uint8_t reg_num)
      : _reg_cls(reg_cls), _reg_num(reg_num) {}

private:
  uint8_t _reg_cls;  // register class
  uint8_t _reg_num;  // register number
};

//! @brief helper function to print register
static inline std::ostream& operator<<(std::ostream& os, const REG_DESC& reg) {
  reg.Print(os);
  return os;
}

}  // namespace cg

}  // namespace air

#endif  // AIR_CG_TARG_INFO_H
