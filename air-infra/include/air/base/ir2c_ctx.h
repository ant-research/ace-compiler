//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_BASE_IR2C_CTX_H
#define AIR_BASE_IR2C_CTX_H

#include "air/base/analyze_ctx.h"
#include "air/base/ir2c_util.h"
#include "air/base/meta_info.h"

namespace air {
namespace base {

//! @brief Return value for IR2C to describe how kid is handled
class IR2C_RETV {
public:
  //! @brief Code to describe how kid is handled
  enum IR2C_RETC {
    NORMAL,    //!< kid is handled and return expression with value
    VAR_USED,  //!< kid is handled and value stored in var from parent
    TMP_USED,  //!< kid is handled and value stored in temp
  };

  //! @brief Construct a NORMAL IR2C_RETV object
  IR2C_RETV() : _retc(NORMAL), _var(Null_st_id) {}

  //! @brief Contruct a VAR/TMP IR2C_RETV object with ADDR_DATUM_ID
  IR2C_RETV(IR2C_RETC retc, ADDR_DATUM_ID var) : _retc(retc), _var(var) {
    AIR_ASSERT(var != Null_ptr);
  }

  //! @brief Check if retv uses variable
  bool Has_var() const { return _retc != IR2C_RETC::NORMAL; }

  //! @brief Get variable id if VAR_USED or TMP_USED
  ADDR_DATUM_ID Var_id() const {
    AIR_ASSERT(_retc == IR2C_RETC::VAR_USED || _retc == IR2C_RETC::TMP_USED);
    return _var;
  }

private:
  IR2C_RETC     _retc;
  ADDR_DATUM_ID _var;
};  // IR2C_RETV

//! @brief Context for IR2C
class IR2C_CTX : public ANALYZE_CTX {
public:
  //! @brief Construct a new ir2c ctx object
  //! @param os output stream
  explicit IR2C_CTX(std::ostream& os) : _ir2c_util(os) {}

  ~IR2C_CTX()                          = default;
  IR2C_CTX(void)                       = delete;
  IR2C_CTX(const IR2C_CTX&)            = delete;
  IR2C_CTX& operator=(const IR2C_CTX&) = delete;

  //! @brief Special handling for BLOCK for IR2C
  template <typename RETV, typename VISITOR>
  RETV Handle_block(VISITOR* visitor, NODE_PTR node) {
    AIR_ASSERT(node->Is_block());
    Begin_block(node);
    for (STMT_PTR stmt = node->Begin_stmt(); stmt != node->End_stmt();
         stmt          = stmt->Next()) {
      NODE_PTR snode = stmt->Node();
      Begin_stmt(snode);
      visitor->template Visit<RETV>(snode);
      End_stmt(snode);
    }
    End_block(node);
    return RETV();
  }

  //! @brief Level of current node in nested block structures
  //! @return int Current Level
  int Level() const { return _ir2c_util.Level(); }

  //! @brief Level of current node in nested block structures
  //! @return int Current Level
  int& Level() { return _ir2c_util.Level(); }

  //! @brief Emit initialization data for constant array
  //! @tparam ElemT Type of array element
  template <typename ElemT>
  void Emit_array_init(CONSTANT_PTR cst) {
    _ir2c_util.Emit_array_init<ElemT>(cst);
  }

  //! @brief Emit a scalar init value
  void Emit_scalar_init(CONSTANT_PTR cst) { _ir2c_util.Emit_scalar_init(cst); }

  //! @brief Emit a constant string
  void Emit_constant_str_init(CONSTANT_PTR cst) {
    _ir2c_util.Emit_constant_str_init(cst);
  }

  //! @brief Emit a constant id as the name of the constant
  void Emit_constant_id(CONSTANT_ID cst) { _ir2c_util.Emit_constant_id(cst); }

  //! @brief Emit a constant and mark constant used
  void Emit_constant_name(CONSTANT_ID cst) {
    _ir2c_util.Emit_constant_name(cst);
  }

  //! @brief Emit a constant array
  void Emit_constant_array(CONSTANT_PTR cst, bool decl_only) {
    _ir2c_util.Emit_constant_array(cst, decl_only);
  }

  //! @brief Emit a constant scalar
  void Emit_constant_scalar(CONSTANT_PTR cst) {
    _ir2c_util.Emit_constant_scalar(cst);
  }

  //! @brief Emit all global constants
  void Emit_global_constants(GLOB_SCOPE* glob, bool decl_only) {
    _ir2c_util.Emit_global_constants(glob, decl_only);
  }

  //! @brief Emit all global type definitions
  void Emit_global_type(GLOB_SCOPE* glob) { _ir2c_util.Emit_global_type(glob); }

  //! @brief Emit a type definition. If name_only is true, only emit the type
  //! name
  //! @param name_only Only emit type name or full type definition
  void Emit_type(TYPE_PTR type, bool name_only) {
    _ir2c_util.Emit_type(type, name_only);
  }

  //! @brief Emit dimensions for array type
  //! @param flatten Treat multi-dimention array as 1-D array or not
  uint64_t Emit_arb(ARRAY_TYPE_PTR type, bool flatten) {
    return _ir2c_util.Emit_arb(type, flatten);
  }

  //! @brief Emit a pointer type. If name_only is true, only emit the type name
  //! @param name_only Only emit type name and star
  void Emit_pointer_type(POINTER_TYPE_PTR type, bool name_only) {
    _ir2c_util.Emit_pointer_type(type, name_only);
  }

  //! @brief Emit a record type. If name_only is true, only emit the type name
  //! @param name_only Only emit type keyword and type name
  void Emit_record_type(RECORD_TYPE_PTR type, bool name_only) {
    _ir2c_util.Emit_record_type(type, name_only);
  }

  //! @brief Emit function signature for declaration
  void Emit_func_sig(FUNC_PTR func) { _ir2c_util.Emit_func_sig(func); }

  //! @brief Emit function definition
  void Emit_func_def(FUNC_SCOPE* fscope) { _ir2c_util.Emit_func_def(fscope); }

  //! @brief Emit preg id
  void Emit_preg_id(PREG_ID id) { _ir2c_util.Emit_preg_id(id); }

  //! @brief Emit all local variables at beginning of function
  //! body
  void Emit_local_var(FUNC_SCOPE* func) { _ir2c_util.Emit_local_var(func); }

  //! @brief Emit left brace to begin the function
  void Begin_func_body(NODE_PTR node) {
    int level = Level();
    AIR_ASSERT(level == 0);
    _ir2c_util << " {" << std::endl;
  }

  //! @brief Emit right brace to end the function
  void End_func_body(NODE_PTR node) {
    int level = Level();
    AIR_ASSERT(level == 0);
    _ir2c_util << "}" << std::endl;
  }

  //! @brief Emit left brace to begin a block
  void Begin_block(NODE_PTR node) {
    int& level = Level();
    if (level > 0) {
      _ir2c_util << "{" << std::endl;
    }
    level++;
  }

  //! @brief Emit right brace to end a block
  void End_block(NODE_PTR node) {
    int& level = Level();
    level--;
    if (level > 0) {
      _ir2c_util << std::string(level * 2, ' ') << "}" << std::endl;
    }
  }

  //! @brief Begin a new statement
  void Begin_stmt(NODE_PTR node) {
    int level = Level();
    _ir2c_util << std::string(level * 2, ' ');
  }

  //! @brief End current statement
  void End_stmt(NODE_PTR node) {
    if (!META_INFO::Has_prop<OPR_PROP::SCF>(node->Opcode()) &&
        !node->Is_comment() && !node->Is_pragma()) {
      _ir2c_util << ";" << std::endl;
    }
  }

  //! @brief Begin a new expression
  void Begin_expr(NODE_PTR node, NODE_PTR parent) {
    if (!parent->Is_root()) {
      // only add '(' for sub-expression
      _ir2c_util << "(";
    }
  }

  //! @brief End current expression
  void End_expr(NODE_PTR node, NODE_PTR parent) {
    if (!parent->Is_root()) {
      // only add ')' for sub-expression
      _ir2c_util << ")";
    }
  }

  //! @brief Emit a variable name from ADDR_DATUM_PTR
  //!  SYM Index is appended to avoid duplication
  void Emit_var(ADDR_DATUM_PTR var) { _ir2c_util.Emit_var(var); }

  //! @brief Emit a variable name from NODE_PTR
  //! @param node Node with addressable data
  void Emit_var(NODE_PTR node) { _ir2c_util.Emit_var(node->Addr_datum()); }

  //! @brief Emit a field name from NODE_PTR
  //! @param node Node with field info
  void Emit_field(NODE_PTR node) {
    _ir2c_util.Emit_name(node->Field()->Name());
  }

  //! @brief Emit a symbol name from SYM_PTR
  void Emit_sym(SYM_PTR sym) { Emit_name(sym->Name()); }

  //! @brief Emit a name from STR_PTR
  void Emit_name(STR_PTR name) { _ir2c_util.Emit_identifier(name->Char_str()); }

  //! @brief Emit a identifier from a C-string
  void Emit_identifier(const char* str) { _ir2c_util.Emit_identifier(str); }

  //! @brief Emit variable name as string
  //! @param node Node with addressable data
  void Emit_var_str(NODE_PTR node) {
    const char* str = node->Addr_datum()->Base_sym()->Name()->Char_str();
    _ir2c_util << '\"' << str << '\"';
  }

  //! @brief Emit a parameter declaration
  //! @param idx Index of the param. -1 means return value
  void Emit_param(PARAM_PTR param, uint32_t idx) {
    _ir2c_util.Emit_param(param, idx);
  }

  //! @brief Emit a few spaces for indent
  void Emit_indent() { _ir2c_util.Emit_indent(); }

  //! @brief Emit any typed value
  template <typename T>
  IR2C_CTX& operator<<(const T& val) {
    _ir2c_util << val;
    return *this;
  }

  //! @brief Transparent stream manipulators (std::endl, std::hex, etc.)
  IR2C_CTX& operator<<(std::ostream& (*manip)(std::ostream&)) {
    _ir2c_util << manip;
    return *this;
  }

protected:
  IR2C_UTIL _ir2c_util;
};  // IR2C_CTX

}  // namespace base
}  // namespace air

#endif  // AIR_BASE_IR2C_CTX_H
