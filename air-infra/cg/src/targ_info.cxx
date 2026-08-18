//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/cg/targ_info.h"

#include <iomanip>
#include <sstream>

#include "air/util/debug.h"

namespace air {

namespace cg {

template <typename T>
class BIT_MASK {
public:
  void Print(std::ostream& os) const {
    T        mask  = _val;
    uint32_t i     = 1;
    bool     space = false;
    while (mask) {
      if ((mask & 1) == 1) {
        if (space) {
          os << ' ';
        } else {
          space = true;
        }
        os << i;
      }
      mask >>= 1;
      i <<= 1;
    }
  }

  BIT_MASK(T val) : _val(val) {}

private:
  T _val;
};

template <typename T>
static std::ostream& operator<<(std::ostream& os, const BIT_MASK<T>& mask) {
  mask.Print(os);
  return os;
}

class REG_DESC {
public:
  void Print(std::ostream& os) const {
    if (_ti != nullptr) {
      os << _ti->Reg_name(_rc, _reg_num);
    } else {
      os << 'c' << (uint32_t)_rc << 'n' << (uint32_t)_reg_num;
    }
  }

  constexpr REG_DESC(const TARG_INFO_META* ti, REG_CLASS rc, uint8_t reg_num)
      : _ti(static_cast<const TARG_INFO*>(ti)), _rc(rc), _reg_num(reg_num) {}

private:
  const TARG_INFO* _ti;
  REG_CLASS        _rc;
  uint8_t          _reg_num;
};

static std::ostream& operator<<(std::ostream& os, const REG_DESC& reg) {
  reg.Print(os);
  return os;
}

void REG_CLASS_META::Print(std::ostream& os, uint32_t indent) const {
  os << std::string(indent * 2, ' ');
  os << _name << " cls=" << Reg_class_desc(_reg_cls) << " count=" << _count;
  os << " width=[" << BIT_MASK(_size_mask) << "]" << std::endl;
}

void REG_CLASS_META::Print() const { Print(std::cout, 0); }

std::string REG_CLASS_META::To_str() const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os, 0);
  return buf.str();
}

void REG_INFO_META::Print(std::ostream& os, uint32_t indent) const {
  os << std::string(indent * 2, ' ');
  os << _name << " code=" << _code;
  os << " width=[" << BIT_MASK(_size_mask) << "]";
  os << " flags=" << Reg_flag_desc(_flags) << std::endl;
}

void REG_INFO_META::Print() const { Print(std::cout, 0); }

std::string REG_INFO_META::To_str() const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os, 0);
  return buf.str();
}

void REG_CONVENTION::Print(std::ostream& os, uint32_t indent,
                           const TARG_INFO* ti) const {
  os << std::string(indent * 2, ' ');
  os << "fp=" << REG_DESC(ti, REG_CLASS::GPR, _fp);
  os << " sp=" << REG_DESC(ti, REG_CLASS::GPR, _sp);
  os << " gp=" << REG_DESC(ti, REG_CLASS::GPR, _gp);
  os << " tp=" << REG_DESC(ti, REG_CLASS::GPR, _tp);
  os << " ra=" << REG_DESC(ti, REG_CLASS::GPR, _ra);
  os << " int_zero=" << REG_DESC(ti, REG_CLASS::GPR, _int_zero) << std::endl;
  if (_num_int_parm > 0) {
    os << std::string(indent * 2, ' ') << "int parm=[";
    for (uint32_t i = 0; i < _num_int_parm; ++i) {
      if (i > 0) {
        os << " ";
      }
      os << REG_DESC(ti, REG_CLASS::GPR, _param_retv_regs[i]);
    }
    os << "]" << std::endl;
  }
  if (_num_int_retv > 0) {
    os << std::string(indent * 2, ' ') << "int retv=[";
    for (uint32_t i = 0; i < _num_int_retv; ++i) {
      if (i > 0) {
        os << " ";
      }
      os << REG_DESC(ti, REG_CLASS::GPR, _param_retv_regs[_num_int_parm + i]);
    }
    os << "]" << std::endl;
  }
  if (_num_fp_parm > 0) {
    os << std::string(indent * 2, ' ') << "fp parm=[";
    for (uint32_t i = 0; i < _num_fp_parm; ++i) {
      if (i > 0) {
        os << " ";
      }
      os << REG_DESC(ti, REG_CLASS::FPR,
                     _param_retv_regs[_num_int_parm + _num_int_retv + i]);
    }
    os << "]" << std::endl;
  }
  if (_num_fp_retv > 0) {
    os << std::string(indent * 2, ' ') << "fp retv=[";
    for (uint32_t i = 0; i < _num_fp_retv; ++i) {
      if (i > 0) {
        os << " ";
      }
      os << REG_DESC(
          ti, REG_CLASS::FPR,
          _param_retv_regs[_num_int_parm + _num_int_retv + _num_fp_parm + i]);
    }
    os << "]" << std::endl;
  }
}

void REG_CONVENTION::Print() const { Print(std::cout, 0, nullptr); }

std::string REG_CONVENTION::To_str(const TARG_INFO* ti) const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os, 0, ti);
  return buf.str();
}

void OPND_META::Print(std::ostream& os, uint32_t indent) const {
  os << std::string(indent * 2, ' ');
  os << Opnd_kind_desc(_opnd_kind);
  if (_opnd_kind == OPND_KIND::REGISTER) {
    os << " " << Reg_class_desc(_reg_cls);
    if (_reg_num != REG_UNKNOWN) {
      os << _reg_num;
    }
    os << " [" << BIT_MASK(_size_mask) << "]";
  }
}

void OPND_META::Print() const { Print(std::cout, 0); }

std::string OPND_META::To_str() const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os, 0);
  return buf.str();
}

void INST_META::Print(std::ostream& os, uint32_t indent) const {
  os << std::string(indent * 2, ' ');
  os << std::left << std::setw(10) << _name;
  if (_code != 0) {
    os << "(" << _code << ")";
  }
  os << " (";
  for (uint32_t i = 0; i < _res_count; ++i) {
    if (i > 0) {
      os << ", ";
    }
    _res_opnd[i].Print(os, 0);
  }
  os << ") <- (";
  for (uint32_t i = 0; i < _opnd_count; ++i) {
    if (i > 0) {
      os << ", ";
    }
    _res_opnd[_res_count + i].Print(os, 0);
  }
  os << ")" << std::endl;
}

void INST_META::Print() const { Print(std::cout, 0); }

std::string INST_META::To_str() const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os, 0);
  return buf.str();
}

const char* TARG_INFO_META::Reg_name(REG_CLASS rc, uint8_t reg_num) const {
  AIR_ASSERT((uint8_t)rc < _rc_num);
  if (reg_num == REG_UNKNOWN) {
    return "N/A";
  }
  AIR_ASSERT(reg_num < _rc_meta[(uint8_t)rc]._count);
  return _reg_meta[(uint8_t)rc][reg_num]._name;
}

const char* TARG_INFO_META::Op_name(uint32_t opcode) const {
  AIR_ASSERT(opcode < _inst_num);
  return _inst_meta[opcode]->_name;
}

uint32_t TARG_INFO_META::Res_count(uint32_t opcode) const {
  AIR_ASSERT(opcode < _inst_num);
  return _inst_meta[opcode]->_res_count;
}

uint32_t TARG_INFO_META::Opnd_count(uint32_t opcode) const {
  AIR_ASSERT(opcode < _inst_num);
  return _inst_meta[opcode]->_opnd_count;
}

void TARG_INFO_META::Print(std::ostream& os, uint32_t indent) const {
  os << std::string(indent * 2, ' ');
  os << _name << " isa=" << (uint32_t)_isa << std::endl;
  os << std::string(indent * 2, ' ') << " Registers:" << std::endl;
  for (uint32_t i = 0; i < _rc_num; ++i) {
    _rc_meta[i].Print(os, indent + 1);
    for (uint32_t j = 0; j < _rc_meta[i]._count; ++j) {
      _reg_meta[i][j].Print(os, indent + 2);
    }
  }
  os << std::string(indent * 2, ' ') << "Register Convention:" << std::endl;
  _reg_conv->Print(os, indent + 1, (const TARG_INFO*)this);
  os << std::string(indent * 2, ' ') << " Instructions:" << std::endl;
  for (uint32_t i = 0; i < _inst_num; ++i) {
    _inst_meta[i]->Print(os, indent + 1);
  }
}

void TARG_INFO_META::Print() const { Print(std::cout, 0); }

std::string TARG_INFO_META::To_str() const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os, 0);
  return buf.str();
}

TARG_INFO_MGR* TARG_INFO_MGR::Instance = nullptr;

bool TARG_INFO_MGR::Register_targ_info(const TARG_INFO* targ_info) {
  if (Instance == nullptr) {
    Instance = new TARG_INFO_MGR;
  }
  AIR_ASSERT(Instance != nullptr);

  std::vector<const TARG_INFO*>& ti = Instance->_targ_info;
  if (ti.size() <= targ_info->_isa) {
    ti.resize(targ_info->_isa + 2);
  } else if (ti[targ_info->_isa] != nullptr) {
    AIR_ASSERT(false);
    return false;
  }
  ti[targ_info->_isa] = targ_info;
  return true;
}

const TARG_INFO* TARG_INFO_MGR::Targ_info(uint8_t isa) {
  if (Instance == nullptr) {
    return nullptr;
  }
  std::vector<const TARG_INFO*>& ti = Instance->_targ_info;
  AIR_ASSERT(isa < ti.size() && ti[isa] != nullptr);
  return ti[isa];
}

void TARG_INFO_MGR::Print(std::ostream& os, uint32_t indent) {
  AIR_ASSERT(Instance != nullptr);
  std::vector<const TARG_INFO*>& ti = Instance->_targ_info;
  os << std::string(indent * 2, ' ') << "Targets:" << std::endl;
  for (uint32_t i = 0; i < ti.size(); ++i) {
    const TARG_INFO_META* targ = ti[i];
    if (targ != nullptr) {
      targ->Print(os, indent + 1);
    }
  }
}

void TARG_INFO_MGR::Print() { Print(std::cout, 0); }

std::string TARG_INFO_MGR::To_str() {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os, 0);
  return buf.str();
}

}  // namespace cg

}  // namespace air
