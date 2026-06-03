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

void REG_META::Print(std::ostream& os, uint32_t indent) const {
  os << std::string(indent * 2, ' ');
  os << _name << " code=" << _code;
  os << " width=[" << BIT_MASK(_size_mask) << "]";
  os << " flags=" << Reg_flag_desc(_flags) << std::endl;
}

void REG_META::Print() const { Print(std::cout, 0); }

std::string REG_META::To_str() const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os, 0);
  return buf.str();
}

void REG_INFO_META::Print(std::ostream& os, uint32_t indent) const {
  os << std::string(indent * 2, ' ');
  os << _name << " cls=" << _reg_cls << " num=" << _reg_num;
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

void OPND_META::Print(std::ostream& os, uint32_t indent) const {
  os << std::string(indent * 2, ' ');
  os << Opnd_kind_desc(_opnd_kind);
  if (_opnd_kind == OPND_KIND::REGISTER) {
    os << " " << REG_DESC(_reg_cls, _reg_num) << " [" << BIT_MASK(_size_mask)
       << "]";
  }
}

void OPND_META::Print() const { Print(std::cout, 0); }

std::string OPND_META::To_str() const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os, 0);
  return buf.str();
}

void OPND_FLAG_META::Print(std::ostream& os, uint32_t indent) const {
  os << std::string(indent * 2, ' ');
  os << _id << " " << _desc;
}

void OPND_FLAG_META::Print() const { Print(std::cout, 0); }

std::string OPND_FLAG_META::To_str() const {
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
  for (uint32_t i = 0; i < _res_num; ++i) {
    if (i > 0) {
      os << ", ";
    }
    _res_opnd[i].Print(os, 0);
  }
  os << ") <- (";
  for (uint32_t i = 0; i < _opnd_num; ++i) {
    if (i > 0) {
      os << ", ";
    }
    _res_opnd[_res_num + i].Print(os, 0);
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

void ISA_INFO_META::Print(std::ostream& os, uint32_t indent) const {
  os << std::string(indent * 2, ' ');
  os << _name << " id=" << _isa_id;
  for (uint32_t i = 0; i < _inst_num; ++i) {
    _inst_meta[i]->Print(os, indent + 1);
  }
  for (uint32_t i = 0; i < _opnd_flag_num; ++i) {
    _opnd_flag_meta[i].Print(os, indent + 1);
  }
}

void ISA_INFO_META::Print() const { Print(std::cout, 0); }

std::string ISA_INFO_META::To_str() const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os, 0);
  return buf.str();
}

void REG_CONV_META::Print(std::ostream& os, uint32_t indent) const {
  os << std::string(indent * 2, ' ');
  if (_reg_zero != REG_UNKNOWN) {
    os << REG_DESC(_reg_cls, _reg_zero);
  }
  for (uint32_t i = 0; i < _num_param; ++i) {
    if (i == 0) {
      os << " param(";
    } else {
      os << ", ";
    }
    os << REG_DESC(_reg_cls, _param_retv_regs[i]);
    if (i == _num_param - 1) {
      os << ")";
    }
  }
  for (uint32_t i = 0; i < _num_retv; ++i) {
    if (i == 0) {
      os << " retv(";
    } else {
      os << ", ";
    }
    os << REG_DESC(_reg_cls, _param_retv_regs[_num_param + i]);
    if (i == _num_param - 1) {
      os << ")";
    }
  }
}

void REG_CONV_META::Print() const { Print(std::cout, 0); }

std::string REG_CONV_META::To_str() const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os, 0);
  return buf.str();
}

void ABI_INFO_META::Print(std::ostream& os, uint32_t indent) const {
  os << std::string(indent * 2, ' ');
  os << _name << " id=" << _abi_id;
  if (_fp != REG_UNKNOWN) {
    os << " fp:" << REG_DESC(_gpr_cls, _fp);
  }
  if (_sp != REG_UNKNOWN) {
    os << " sp=" << REG_DESC(_gpr_cls, _sp);
  }
  if (_gp != REG_UNKNOWN) {
    os << " gp=" << REG_DESC(_gpr_cls, _gp);
  }
  if (_tp != REG_UNKNOWN) {
    os << " tp=" << REG_DESC(_gpr_cls, _tp);
  }
  if (_ra != REG_UNKNOWN) {
    os << " ra=" << REG_DESC(_gpr_cls, _ra);
  }
  for (uint32_t i = 0; i < _reg_conv_num; ++i) {
    _reg_conv_meta[i]->Print(os, indent + 1);
  }
}

void ABI_INFO_META::Print() const { Print(std::cout, 0); }

std::string ABI_INFO_META::To_str() const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os, 0);
  return buf.str();
}

TARG_INFO_MGR* TARG_INFO_MGR::Instance() {
  static TARG_INFO_MGR* instance = nullptr;
  if (instance == nullptr) {
    instance = new TARG_INFO_MGR;
  }
  return instance;
}

bool TARG_INFO_MGR::Register_reg_info(const REG_INFO_META* info) {
  return Register_info(Reg_info_tab(), info);
}

bool TARG_INFO_MGR::Register_isa_info(const ISA_INFO_META* info) {
  return Register_info(Isa_info_tab(), info);
}

bool TARG_INFO_MGR::Register_abi_info(const ABI_INFO_META* info) {
  return Register_info(Abi_info_tab(), info);
}

void TARG_INFO_MGR::Print(std::ostream& os, uint32_t indent) {
  auto print_tab = [](auto&& tab, std::ostream& os, uint32_t indent) {
    for (auto&& item : tab) {
      if (item != nullptr) {
        item->Print(os, indent);
      }
    }
  };

  print_tab(Reg_info_tab(), os, indent + 1);
  print_tab(Isa_info_tab(), os, indent + 1);
  print_tab(Abi_info_tab(), os, indent + 1);
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
