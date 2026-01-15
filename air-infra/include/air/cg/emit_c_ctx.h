//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================
#ifndef AIR_CG_EMIT_C_CTX_H
#define AIR_CG_EMIT_C_CTX_H

#include <fstream>
#include <functional>
#include <ostream>
#include <set>
#include <utility>
#include <vector>

#include "air/base/ir2c_util.h"
#include "air/base/st.h"
#include "air/cg/analyze_ctx.h"
#include "air/cg/cgir_container.h"
#include "air/cg/cgir_decl.h"
#include "air/cg/cgir_util.h"
#include "air/cg/config.h"
#include "air/cg/isa_core.h"
#include "air/cg/targ_info.h"
#include "air/driver/driver_ctx.h"
#include "air/util/error.h"
namespace air {
namespace cg {

class EMIT_C_CTX;

// accept uint32_t and return string
using HANDLER = std::function<std::string(EMIT_C_CTX* self, uint32_t)>;

struct SYMBOL_HANDLER {
  std::string _prefix;
  HANDLER     _handler;
};

// later should extract the hpu related to hpu-cg repo, only keep the common
// here.
//! @brief Context of the C emitter providing trace APIs and recording
//! constants used in the current GLOB_SCOPE.
class EMIT_C_CTX : public ANALYZE_CTX {
  using GLOB_SCOPE  = base::GLOB_SCOPE;
  using DATA_FORMAT = base::DATA_FORMAT;

public:
  EMIT_C_CTX(const driver::DRIVER_CTX* driver_ctx, const CODE_GEN_CONFIG* cfg,
             GLOB_SCOPE* glob, std::ostream& ofile)
      : _driver_ctx(driver_ctx), _cfg(cfg), _glob(glob), _ir2c_util(ofile) {}

  //! @brief Emit an operand
  R_CODE Handle_opnd(OPND_PTR opnd, bool hex_imm = false,
                     bool process_array_opnd = false);

  //! @brief Emit an operation result variable
  R_CODE Emit_res_var(OPND_PTR res, const char* type,
                      const std::string& assign_prefix, bool hex_imm = false);

  //! @brief Handle instructions to output C code
  void Handle_inst(const char* op_name, INST_PTR inst,
                   const char* res_decl_type, bool hex_imm = false,
                   bool pure_operand = false);

  //! @brief Handle array related instructions to output C code
  void Handle_process_array_inst(const char* macro_name, INST_PTR inst,
                                 const char* res_decl_type,
                                 bool        hex_imm = false);

  //! @brief Default instruction handler
  template <typename VISITOR>
  void Handle_inst(VISITOR* visitor, INST_PTR inst);

  //! @brief Emit global include header files
  void Emit_global_include();
  //! @brief Emit the function header
  void Emit_func_header(CGIR_CONTAINER* cont);
  //! @brief Emit the function end
  void Emit_func_end(CGIR_CONTAINER* cont);
  //! @brief Emit the function
  template <typename VISITOR>
  void Handle_func(VISITOR* visitor, CGIR_CONTAINER* cont);

  //! @brief Emit a name from STR_PTR
  void Emit_name(base::STR_PTR name) { Emit_identifier(name->Char_str()); }

  //! @brief Emit a identifier from a C-string
  void Emit_identifier(const char* str) { _ir2c_util.Emit_identifier(str); }

  //! @brief Emit a constant id as the name of the constant
  void Emit_constant_id(base::CONSTANT_ID cst) {
    _ir2c_util.Emit_constant_id(cst);
  }

  //! @brief Emit dimensions for array type
  //! @param flatten Treat multi-dimention array as 1-D array or not
  uint64_t Emit_arb(base::ARRAY_TYPE_PTR type, bool flatten) {
    return _ir2c_util.Emit_arb(type, flatten);
  }

  //! @brief Emit initialization data for constant array
  //! @tparam ElemT Type of array element
  template <typename ElemT>
  void Emit_array_init(base::CONSTANT_PTR cst,
                       DATA_FORMAT        format = DATA_FORMAT::DEC) {
    _ir2c_util.Emit_array_init<ElemT>(cst, format);
  }

  //! @brief Emit a pointer type. If name_only is true, only emit the type name
  //! @param name_only Only emit type name and star
  void Emit_pointer_type(base::POINTER_TYPE_PTR type, bool name_only) {
    _ir2c_util.Emit_pointer_type(type, name_only);
  }

  //! @brief Emit a record type. If name_only is true, only emit the type name
  //! @param name_only Only emit type keyword and type name
  void Emit_record_type(base::RECORD_TYPE_PTR type, bool name_only) {
    _ir2c_util.Emit_record_type(type, name_only);
  }

  //! @brief Emit a type definition. If name_only is true, only emit the type
  //! name
  //! @param name_only Only emit type name or full type definition
  void Emit_type(base::TYPE_PTR type, bool name_only) {
    _ir2c_util.Emit_type(type, name_only);
  }

  //! @brief Generate a unified register name string based on the operand
  inline std::string Make_reg_name(const OPND_PTR opnd) {
    // get base register name
    const char* base =
        TARG_INFO_MGR::Reg_name(opnd->Reg_class(), opnd->Reg_num());
    AIR_ASSERT(base);

    // format output reg name
    std::ostringstream oss;
    oss << base << opnd->Id().Value();
    return oss.str();
  }

  //! @brief Emit a constant array
  void Emit_constant_array(base::CONSTANT_PTR cst, bool decl_only,
                           air::base::DATA_FORMAT format = DATA_FORMAT::DEC);

  //! @brief Get constant symbol name
  std::string Cst_sym_name(base::CONSTANT_PTR cst) const {
    return "_cst_" + std::to_string(cst->Id().Value());
  }

  //! @brief Emit global constants.
  void Emit_cst(bool decl_only);

  const ISA_INFO_META* Get_isa_info_meta(INST_PTR inst) {
    uint8_t              isa      = inst->Isa();
    const ISA_INFO_META* isa_info = TARG_INFO_MGR::Isa_info(isa);
    return isa_info;
  }

  bool Is_isa_core(INST_PTR inst) {
    uint8_t isa = inst->Isa();
    if (isa == ISA_CORE) {
      AIR_ASSERT(inst->Opcode() == CORE_CHI);
      return true;
    }
    return false;
  }

  bool First_occur(uint32_t opnd_id) {
    if (std::find(_opnd_vec.begin(), _opnd_vec.end(), opnd_id) !=
        _opnd_vec.end()) {
      return false;
    }

    _opnd_vec.push_back(opnd_id);
    return true;
  }

  //! @brief Get or assign index to a symbol id
  //! @tparam ElemT Type of map
  template <typename MapT>
  uint32_t Get_or_assign_index(MapT& map, uint32_t sym_id) {
    auto it = map.find(sym_id);
    if (it != map.end()) {
      return it->second;
    }
    uint32_t index = static_cast<uint32_t>(map.size());
    map.emplace(sym_id, index);
    return index;
  }

  //! @brief Get the index corresponding to the input sym id
  uint32_t Input_index(uint32_t sym_id) {
    return Get_or_assign_index(_input_sym_idx_map, sym_id);
  }

  //! @brief Get the index corresponding to the weight sym id
  uint32_t Weight_index(uint32_t sym_id) {
    return Get_or_assign_index(_weight_sym_idx_map, sym_id);
  }

  //! @brief Get the index corresponding to the relinearization key sym id
  uint32_t Relin_key_index(uint32_t sym_id) {
    return Get_or_assign_index(_relin_key_sym_idx_map, sym_id);
  }

  //! @brief Get the index corresponding to the output sym id
  uint32_t Output_index(uint32_t sym_id) {
    return Get_or_assign_index(_output_sym_idx_map, sym_id);
  }

  bool Starts_with(const std::string& s, const std::string& prefix) {
    return s.size() >= prefix.size() &&
           s.compare(0, prefix.size(), prefix) == 0;
  }

  //! @brief Expand the input string s if it begins with fhe.setmod
  std::string Expand_setmod(std::string s) {
    const std::string target = "fhe.setmod";
    if (!Starts_with(s, target)) {
      return s;  // return if not start with fhe.setmod
    }

    size_t prefix_len = target.size();
    // over or start with '.'
    if (prefix_len == s.size() || s[prefix_len] == '.') {
      std::string result = "fhe.setmodulus";
      if (prefix_len < s.size())
        result.append(s.begin() + prefix_len, s.end());  // copy suffix
      return result;
    }
    return s;  // return original string is not the desired pattern
  }

  //! @brief Format the output string
  std::string Output_format(std::string in) {
    std::string        expand_str = Expand_setmod(in);
    std::ostringstream out;
    bool               need_sep = false;

    for (char ch : expand_str) {
      if (ch == '.') {
        need_sep = true;  // add underline before next word
        continue;
      }
      if (need_sep) {
        out << '_';
        need_sep = false;
      }
      out << static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
    }
    return out.str();
  }

  //! @brief Return a default indent for instructions.
  std::string Default_indent(void) const {
    return std::string(_cfg->Inst_indent(), ' ');
  }

  void Set_container(CGIR_CONTAINER* cont) { _cont = cont; }

  template <typename T>
  EMIT_C_CTX& operator<<(const T& str) {
    _ir2c_util << str;
    return *this;
  }

  GLOB_SCOPE*     Glob(void) const { return _glob; }
  CGIR_CONTAINER* Container(void) const { return _cont; }

  DECLARE_TRACE_DETAIL_API((*_cfg), _driver_ctx)
private:
  const driver::DRIVER_CTX*              _driver_ctx = nullptr;
  const CODE_GEN_CONFIG*                 _cfg        = nullptr;
  GLOB_SCOPE*                            _glob       = nullptr;
  CGIR_CONTAINER*                        _cont       = nullptr;
  air::base::IR2C_UTIL                   _ir2c_util;
  std::vector<uint32_t>                  _opnd_vec;
  std::unordered_map<uint32_t, uint32_t> _input_sym_idx_map;
  std::unordered_map<uint32_t, uint32_t> _weight_sym_idx_map;
  std::unordered_map<uint32_t, uint32_t> _relin_key_sym_idx_map;
  std::unordered_map<uint32_t, uint32_t> _output_sym_idx_map;
};

//! @brief Default instruction handler.
template <typename VISITOR>
void EMIT_C_CTX::Handle_inst(VISITOR* visitor, INST_PTR inst) {
  uint8_t              isa      = inst->Isa();
  const ISA_INFO_META* isa_info = TARG_INFO_MGR::Isa_info(isa);
  // skip CORE ISA
  if (isa == ISA_CORE) {
    AIR_ASSERT(inst->Opcode() == CORE_CHI);
    return;
  }

  std::string op_name = Output_format(isa_info->Op_name(inst->Operator()));
  _ir2c_util << Default_indent() << op_name << "(";

  uint32_t res_num  = isa_info->Res_num(inst->Operator());
  uint32_t opnd_num = isa_info->Opnd_num(inst->Operator());
  for (uint32_t idx = 0; idx < res_num; ++idx) {
    OPND_PTR res = inst->Res(idx);
    R_CODE   r   = Handle_opnd(res);
    if (r != R_CODE::NORMAL) return;
  }

  for (uint32_t idx = 0; idx < opnd_num; ++idx) {
    OPND_PTR opnd = inst->Opnd(idx);
    R_CODE   r    = Handle_opnd(opnd);
    if (r != R_CODE::NORMAL) return;

    bool is_last_opnd = ((idx + 1) == opnd_num);
    // print comma to sperate operands
    if (!is_last_opnd) _ir2c_util << ", ";
  }
  _ir2c_util << ");";
  _ir2c_util << std::endl << std::endl;
}

template <typename VISITOR>
void EMIT_C_CTX::Handle_func(VISITOR* visitor, CGIR_CONTAINER* cont) {
  Set_container(cont);

  // emit include header file
  Emit_global_include();

  // emit global constants
  Emit_cst(false);

  Emit_func_header(cont);

  // emit IR according LAYOUT
  visitor->template Visit<air::cg::LAYOUT_ITER, air::cg::INST_ITER>(
      air::cg::LAYOUT_ITER(cont));

  Emit_func_end(cont);
}

}  // namespace cg
}  // namespace air

#endif  // AIR_CG_EMIT_C_CTX_H
