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

class TARG_INFO;

//! @brief REG_CLASS_META
//!  define all register classes available in the target
struct REG_CLASS_META {
  const char* _name;         //!< register class name
  REG_CLASS   _reg_cls : 8;  //!< register class id
  uint16_t    _count : 8;    //!< number of registers in the class
  uint16_t    _size_mask;    //!< allowed data width in the class

  void        Print(std::ostream& os, uint32_t indent) const;
  void        Print() const;
  std::string To_str() const;
};

//! @brief REG_INFO_META
//!  define all registers in a register class
struct REG_INFO_META {
  const char* _name;            //!< register name
  uint32_t    _code : 8;        //!< register encoding
  uint32_t    _size_mask : 24;  //!< allowed data width in the register
  uint32_t    _flags;           //!< register flags

  void        Print(std::ostream& os, uint32_t indent) const;
  void        Print() const;
  std::string To_str() const;
};

//! @brief REG_CONVENTION
//!  define register convention for ABI
struct REG_CONVENTION {
  uint8_t _fp;                 // frame pointer. REG_UNKNOWN means no FP
  uint8_t _sp;                 // stack pointer
  uint8_t _gp;                 // global pointer
  uint8_t _tp;                 // thread pointer
  uint8_t _ra;                 // return addr
  uint8_t _int_zero;           // register contains constant value 0
  uint8_t _num_int_parm;       // number of gpr register for parameter passing
  uint8_t _num_int_retv;       // number of gpr register for return value
  uint8_t _num_fp_parm;        // number of fpr register for parameter passing
  uint8_t _num_fp_retv;        // number of fpr register for parameter passing
  uint8_t _param_retv_regs[];  // register number for param and retval

  void Print(std::ostream& os, uint32_t indent, const TARG_INFO* ti) const;
  void Print() const;
  std::string To_str(const TARG_INFO* ti = nullptr) const;
};

//! @brief OPND_META
//!  define operand used in an instruction
struct OPND_META {
  OPND_KIND _opnd_kind : 8;  //!< operand kind
  REG_CLASS _reg_cls : 8;    //!< register class for register operand
  uint8_t   _reg_num;        //!< register number for register operand
  uint8_t   _size_mask;      //!< allowed data width of the operand

  void        Print(std::ostream& os, uint32_t indent) const;
  void        Print() const;
  std::string To_str() const;
};

//! @brief INST_META
//!  define all instructions available in the target
struct INST_META {
  const char*            _name;        //!< instruction name
  uint32_t               _code;        //!< instruction encoding
  uint8_t                _res_count;   //!< result count
  uint8_t                _opnd_count;  //!< operand count
  const struct OPND_META _res_opnd[];  //!< OPND_META for each res/opnd

  void        Print(std::ostream& os, uint32_t indent) const;
  void        Print() const;
  std::string To_str() const;
};

//! @brief TARG_INFO_META
//!  define the target info
struct TARG_INFO_META {
  const char*                        _name;       //!< target name
  uint8_t                            _isa;        //!< target ISA id
  uint8_t                            _rc_num;     //!< number of register class
  uint16_t                           _inst_num;   //!< number of instructions
  const struct REG_CLASS_META*       _rc_meta;    //!< register class meta info
  const struct REG_INFO_META* const* _reg_meta;   //!< register meta info
  const struct REG_CONVENTION*       _reg_conv;   //!< register convention
  const struct INST_META* const*     _inst_meta;  //!< instruction meta info

  const char* Reg_name(REG_CLASS rc, uint8_t reg_num) const;
  const char* Op_name(uint32_t opcode) const;
  uint32_t    Res_count(uint32_t opcode) const;
  uint32_t    Opnd_count(uint32_t opcode) const;

  uint8_t  Fp() const { return _reg_conv->_fp; }
  uint8_t  Sp() const { return _reg_conv->_sp; }
  uint8_t  Gp() const { return _reg_conv->_gp; }
  uint8_t  Tp() const { return _reg_conv->_tp; }
  uint8_t  Ra() const { return _reg_conv->_ra; }
  uint8_t  Int_zero() const { return _reg_conv->_int_zero; }
  uint32_t Num_int_parm() const { return _reg_conv->_num_int_parm; }
  uint32_t Num_int_retv() const { return _reg_conv->_num_int_retv; }
  uint32_t Num_fp_parm() const { return _reg_conv->_num_fp_parm; }
  uint32_t Num_fp_retv() const { return _reg_conv->_num_fp_retv; }

  uint8_t Int_parm_reg(uint32_t idx) const {
    AIR_ASSERT(idx < Num_int_parm());
    return _reg_conv->_param_retv_regs[idx];
  }

  uint8_t Int_retv_reg(uint32_t idx) const {
    AIR_ASSERT(idx < Num_int_retv());
    return _reg_conv->_param_retv_regs[Num_int_parm() + idx];
  }

  uint8_t Fp_parm_reg(uint32_t idx) const {
    AIR_ASSERT(idx < Num_fp_parm());
    return _reg_conv->_param_retv_regs[Num_int_parm() + Num_int_retv() + idx];
  }

  uint8_t Fp_retv_reg(uint32_t idx) const {
    AIR_ASSERT(idx < Num_fp_retv());
    return _reg_conv->_param_retv_regs[Num_int_parm() + Num_int_retv() +
                                       Num_fp_parm() + idx];
  }

  void        Print(std::ostream& os, uint32_t indent) const;
  void        Print() const;
  std::string To_str() const;
};

//! @brief TARG_INFO
//!  public API to access the target info
class TARG_INFO : public TARG_INFO_META {};

//! @brief TARG_INFO_MGR
//!  manager manages all targets
class TARG_INFO_MGR {
public:
  //! @brief register a target
  static bool Register_targ_info(const TARG_INFO* targ_info);

  //! @brief get the target info by ISA is
  static const TARG_INFO* Targ_info(uint8_t isa);

  static void        Print(std::ostream& os, uint32_t indent);
  static void        Print();
  static std::string To_str();

private:
  // disable copy constructor and assign operator
  TARG_INFO_MGR(const TARG_INFO_MGR& mgr)            = delete;
  TARG_INFO_MGR& operator=(const TARG_INFO_MGR& mgr) = delete;

  TARG_INFO_MGR() {}
  static TARG_INFO_MGR* Instance;  // unique instance

  std::vector<const TARG_INFO*> _targ_info;  // all targets
};

//! @brief TARG_INFO_REGISTER
//!  helper to register target automatically
class TARG_INFO_REGISTER {
public:
  //! @brief register the target in constructor
  TARG_INFO_REGISTER(const TARG_INFO* targ_info) {
    bool ret = TARG_INFO_MGR::Register_targ_info(targ_info);
    AIR_ASSERT(ret == true);
  }
};

}  // namespace cg

}  // namespace air

#endif  // AIR_CG_TARG_INFO_H
