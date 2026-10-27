//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_IR_GEN_H
#define FHE_IR_GEN_H

#include <sstream>

#include "air/base/container.h"
#include "air/base/st.h"
#include "air/util/messg.h"
#include "fhe/core/lower_ctx.h"

namespace fhe {
namespace poly {

class VAR;

using CONST_VAR = const VAR;
using NODE_PAIR = std::pair<air::base::NODE_PTR, air::base::NODE_PTR>;
using STMT_PAIR = std::pair<air::base::STMT_PTR, air::base::STMT_PTR>;
using NODE_TRIPLE =
    std::tuple<air::base::NODE_PTR, air::base::NODE_PTR, air::base::NODE_PTR>;
using STMT_TRIPLE =
    std::tuple<air::base::STMT_PTR, air::base::STMT_PTR, air::base::STMT_PTR>;
using POLY_MEM_POOL  = air::util::STACKED_MEM_POOL<4096>;
using POLY_ALLOCATOR = air::util::MEM_ALLOCATOR<POLY_MEM_POOL>;
using VAR_ARR        = std::vector<VAR>;

//! @brief Type kind used in Polynomial IR
enum VAR_TYPE_KIND : uint8_t {
  INDEX,
  POLY,
  PLAIN,
  CIPH,
  CIPH3,
  CIPH_PTR,
  POLY_COEFFS,
  MODULUS_PTR,
  SWK_POLYS,
  SWK_PTR,
  PUBKEY_PTR,
  UINT32,
  INT32,
  UINT64,
  INT64,
  INT64_PTR,
  UINT64_PTR,
  POLY_PTR,
  POLY_PTR_PTR,
  LAST_TYPE
};

//! @brief Local variables been created in polynomial IR
enum POLY_PREDEF_VAR : uint8_t {
  VAR_NUM_Q,
  VAR_NUM_P,
  VAR_P_OFST,
  VAR_P_IDX,
  VAR_KEY_P_OFST,
  VAR_KEY_P_IDX,
  VAR_RNS_IDX,
  VAR_PART_IDX,
  VAR_BCONV_IDX,
  VAR_MODULUS,
  VAR_SWK,
  VAR_SWK_C0,
  VAR_SWK_C1,
  VAR_DECOMP,
  VAR_EXT,
  VAR_PUB_KEY0,
  VAR_PUB_KEY1,
  VAR_MOD_DOWN_C0,
  VAR_MOD_DOWN_C1,
  VAR_AUTO_ORDER,
  VAR_TMP_POLY,
  VAR_TMP_COEFFS,
  VAR_MUL_0_POLY,
  VAR_MUL_1_POLY,
  VAR_MUL_2_POLY,
  VAR_ROT_RES,
  VAR_RELIN_RES,
  VAR_PRECOMP,
  LAST_VAR
};

#define MAX_PARAM_NUM 7

//! @brief Poly builtin function prototype info data structure
typedef struct {
  core::FHE_FUNC _func_id;                     // FHE func id
  uint32_t       _num_params;                  // number of parameters
  VAR_TYPE_KIND  _retv_type;                   // return value type kind
  VAR_TYPE_KIND  _param_types[MAX_PARAM_NUM];  // parameter type kind
  const char*    _param_names[MAX_PARAM_NUM];  // parameter names
  const char*    _fname;                       // function name
} POLY_FUNC_INFO;

//! @breif An encapsulation for air base ADDR_DATUM and PREG
class VAR {
public:
  static constexpr uint32_t NO_INDEX = UINT32_MAX;
  VAR() : _fscope(nullptr), _is_preg(false), _fld_id(NO_INDEX) {
    _var_id = air::base::Null_st_id;
  }

  template <typename VAR_TYPE>
  VAR(air::base::FUNC_SCOPE* fscope, VAR_TYPE var,
      air::base::FIELD_PTR fld = air::base::Null_ptr) {
    Set_var(fscope, var);
    _fld_id = fld->Id().Value();
  }

  VAR(CONST_VAR& var) {
    _fscope  = var._fscope;
    _is_preg = var._is_preg;
    _var_id  = var._var_id;
    _fld_id  = var._fld_id;
  }

  VAR& operator=(CONST_VAR& var) {
    _fscope  = var._fscope;
    _is_preg = var._is_preg;
    _var_id  = var._var_id;
    _fld_id  = var._fld_id;
    return *this;
  }

  void Set_var(air::base::FUNC_SCOPE* fscope, air::base::ADDR_DATUM_PTR var,
               air::base::FIELD_PTR fld = air::base::Null_ptr) {
    _is_preg = false;
    _var_id  = var->Id().Value();
    _fscope  = fscope;
    _fld_id  = fld->Id().Value();
  }

  void Set_var(air::base::FUNC_SCOPE* fscope, air::base::PREG_PTR preg,
               air::base::FIELD_PTR fld = air::base::Null_ptr) {
    _is_preg = true;
    _var_id  = preg->Id().Value();
    _fld_id  = fld->Id().Value();
    _fscope  = fscope;
  }

  air::base::FUNC_SCOPE* Func_scope(void) const { return _fscope; }

  bool Is_null(void) const { return _var_id == air::base::Null_st_id; }

  bool Has_fld() const { return _fld_id != NO_INDEX; }

  air::base::FIELD_PTR Fld() const {
    if (!Has_fld()) return air::base::Null_ptr;
    return Func_scope()->Glob_scope().Field(air::base::FIELD_ID(_fld_id));
  }

  bool Is_preg(void) const { return _is_preg; }

  bool Is_sym(void) const { return !_is_preg; }

  air::base::TYPE_ID Type_id(void) const {
    if (Has_fld()) {
      return Fld()->Type_id();
    }
    if (Is_preg()) {
      return Preg_var()->Type_id();
    } else {
      return Addr_var()->Type_id();
    }
  }

  air::base::TYPE_PTR Type(void) const {
    return Func_scope()->Glob_scope().Type(Type_id());
  }

  air::base::ADDR_DATUM_PTR Addr_var(void) const {
    AIR_ASSERT_MSG(!Is_preg(), "not Addr datum");
    air::base::ADDR_DATUM_ID  id(_var_id);
    air::base::ADDR_DATUM_PTR addr_datum = _fscope->Addr_datum(id);
    return addr_datum;
  }

  air::base::PREG_PTR Preg_var(void) const {
    AIR_ASSERT_MSG(Is_preg(), "not Preg");
    air::base::PREG_ID  id(_var_id);
    air::base::PREG_PTR preg = _fscope->Preg(id);
    return preg;
  }

  bool operator==(CONST_VAR& other) const {
    if (_is_preg != other._is_preg) return false;
    return (_var_id == other._var_id && _fscope == other._fscope &&
            _fld_id == other._fld_id);
  }

  bool operator!=(CONST_VAR& other) const { return !(*this == other); }

  bool operator<(CONST_VAR& other) const {
    AIR_ASSERT(_fscope == other._fscope);

    if (_is_preg != other._is_preg) {
      // PREG < ADDR_DATUM
      return _is_preg;
    }

    // same kind, compare order _var_id . _fld_id
    if (_var_id < other._var_id) {
      return true;
    } else if (_var_id > other._var_id) {
      return false;
    } else {
      return _fld_id < other._fld_id;
    }
  }

  void        Print(std::ostream& os, uint32_t indent = 0) const;
  void        Print() const;
  std::string To_str() const;

private:
  air::base::FUNC_SCOPE* _fscope;
  uint32_t               _var_id;
  uint32_t               _fld_id;
  bool                   _is_preg;
};

//! @brief A Basic class for POLY IR generation for symbol/type/node/stmt
class IR_GEN {
public:
  //! @brief Construct a new poly ir gen object
  //! @param pool Memory pool for IR GEN
  IR_GEN(air::base::CONTAINER* cont, fhe::core::LOWER_CTX* ctx,
         POLY_MEM_POOL* pool)
      : _glob_scope(cont ? cont->Glob_scope() : NULL),
        _func_scope(cont ? cont->Parent_func_scope() : NULL),
        _container(cont),
        _ctx(ctx),
        _pool(pool),
        _predef_var(),
        _tmp_var(),
        _ty_table(VAR_TYPE_KIND::LAST_TYPE),
        _node2var_map(),
        _var_map() {}

  air::base::CONTAINER* Container() { return _container; }

  fhe::core::LOWER_CTX* Lower_ctx() { return _ctx; }

  air::base::GLOB_SCOPE* Glob_scope() { return _glob_scope; }

  air::base::FUNC_SCOPE* Func_scope() { return _func_scope; }

  //! @brief Enter a function scope
  //! @param fscope Function Scope
  void Enter_func(air::base::FUNC_SCOPE* fscope);

  //! @brief Return or create a new polynomial type
  //! @return air::base::TYPE_PTR polynomial type
  air::base::TYPE_PTR Poly_type(void);

  //! @brief Returns cached pre-defined variables
  //! @return std::map<uint64_t, VAR>&
  std::map<uint64_t, VAR>& Predef_var() { return _predef_var; }

  //! @brief Return created tempory local Plain/Ciph/Poly variable
  //! @return std::vector<VAR>&
  std::vector<VAR>& Tmp_var() { return _tmp_var; }

  //! @brief Return cached types
  //! @return std::vector<air::base::TYPE_ID>&
  std::vector<air::base::TYPE_ID>& Types() { return _ty_table; }

  //! @brief Find type with given kind
  //! @param kind Type Kind
  //! @return air::base::TYPE_PTR
  air::base::TYPE_PTR Get_type(VAR_TYPE_KIND          kind,
                               const air::base::SPOS& spos = air::base::SPOS());

  //! @brief Check if ty matches specified type kind
  bool Is_type_of(air::base::TYPE_ID ty_id, VAR_TYPE_KIND kind);

  //! @brief Check if ty is RNS_POLY array type
  bool Is_poly_array(air::base::TYPE_PTR ty);

  //! @brief Check if ty is integer array
  bool Is_int_array(air::base::TYPE_PTR ty);

  //! @brief Check if ty is FHE related type:
  //! CIPH/CIPH3/PLAIN/RNS_POLY/POLY/RNS_POLY[]
  bool Is_fhe_type(air::base::TYPE_ID ty_id);

  //! @brief Find or create local variable with given id
  //! @param id Local variable id
  //! @param spos source position
  //! @return VAR Local Symbol
  CONST_VAR& Get_var(POLY_PREDEF_VAR id, const air::base::SPOS& spos);

  //! @brief Generate a uniq temporary name
  //! @return NAME_PTR
  air::base::STR_PTR Gen_tmp_name();

  //! @brief Add <node, var> to map
  //! @param node
  //! @param var
  CONST_VAR& Add_node_var(air::base::NODE_PTR       node,
                          air::base::ADDR_DATUM_PTR sym,
                          air::base::FIELD_PTR      fld = air::base::Null_ptr) {
    VAR var(Func_scope(), sym, fld);
    CMPLR_ASSERT((_node2var_map.find(node->Id()) == _node2var_map.end() ||
                  _node2var_map[node->Id()] == var),
                 "node variable already exists");
    _node2var_map[node->Id()] = var;
    return _node2var_map[node->Id()];
  }

  CONST_VAR& Add_node_var(air::base::NODE_PTR node, air::base::PREG_PTR preg,
                          air::base::FIELD_PTR fld = air::base::Null_ptr) {
    VAR var(Func_scope(), preg, fld);
    AIR_ASSERT_MSG((_node2var_map.find(node->Id()) == _node2var_map.end() ||
                    _node2var_map[node->Id()] == var),
                   "node variable already exists");
    _node2var_map[node->Id()] = var;
    return _node2var_map[node->Id()];
  }

  bool Has_node_var(air::base::NODE_PTR node) {
    std::map<air::base::NODE_ID, VAR>::iterator iter =
        _node2var_map.find(node->Id());
    if (iter != _node2var_map.end()) {
      return true;
    } else if ((node->Is_ld() || node->Is_st()) &&
               (node->Has_sym() || node->Has_preg())) {
      return true;
    } else {
      return false;
    }
  }

  //! @brief Find or create node's result symbol
  CONST_VAR& Node_var(air::base::NODE_PTR node) {
    std::map<air::base::NODE_ID, VAR>::iterator iter =
        _node2var_map.find(node->Id());
    if (iter != _node2var_map.end()) {
      return iter->second;
    } else if ((node->Is_ld() || node->Is_st()) && node->Has_sym()) {
      return Add_node_var(
          node, node->Addr_datum(),
          node->Has_fld() ? node->Field() : air::base::Null_ptr);
    } else if ((node->Is_ld() || node->Is_st()) && node->Has_preg()) {
      return Add_node_var(
          node, node->Preg(),
          node->Has_fld() ? node->Field() : air::base::Null_ptr);
    } else if (Lower_ctx()->Is_cipher_type(node->Rtype_id())) {
      return Add_node_var(node, New_ciph_var(node->Spos()));
    } else if (Lower_ctx()->Is_cipher3_type(node->Rtype_id())) {
      return Add_node_var(node, New_ciph3_var(node->Spos()));
    } else if (Lower_ctx()->Is_plain_type(node->Rtype_id())) {
      return Add_node_var(node, New_plain_var(node->Spos()));
    } else if (Lower_ctx()->Is_rns_poly_type(node->Rtype_id())) {
      return Add_node_var(node, New_poly_var(node->Spos()));
    } else {
      AIR_ASSERT_MSG(false, "Node_var: unsupported node type");
    }
    return Add_node_var(air::base::Null_ptr,
                        (air::base::ADDR_DATUM_PTR)air::base::Null_ptr);
  }

  //! @brief Get TYPE_PTR binded on the node
  air::base::TYPE_PTR Node_ty(air::base::NODE_PTR node) const;

  //! @brief Get TYPE_ID binded on the node
  air::base::TYPE_ID Node_ty_id(air::base::NODE_PTR node) const {
    return Node_ty(node)->Id();
  }

  //! @brief Check if contains mapping vars with key
  bool Has_mapping_vars(CONST_VAR key) const {
    if (_var_map.find(key) != _var_map.end()) return true;
    return false;
  }

  //! @brief Returns the mapping vars of key
  VAR_ARR& Mapping_vars(CONST_VAR key) { return _var_map[key]; }

  //! @brief Create a comment statement with vararg messages
  template <typename... Args>
  air::base::STMT_PTR New_comment(const air::base::SPOS& spos, Args... args) {
    std::stringstream ss;
    Templ_print(ss, args...);
    std::string str = ss.str();
    str.pop_back();  // remove last std::endl from Templ_print
    air::base::STMT_PTR s_msg = Container()->New_comment(str.c_str(), spos);
    return s_msg;
  }

  //! @brief Create a new plaintext variable
  //! @param spos
  //! @return air::base::ADDR_DATUM_PTR
  air::base::ADDR_DATUM_PTR New_plain_var(const air::base::SPOS& spos);

  //! @brief Create a new ciphertext variable
  //! @param spos
  //! @return air::base::ADDR_DATUM_PTR
  air::base::ADDR_DATUM_PTR New_ciph_var(const air::base::SPOS& spos);

  //! @brief Create a new CIPHER3 variable
  air::base::ADDR_DATUM_PTR New_ciph3_var(const air::base::SPOS& spos);

  //! @brief Create a new polynomial symbol
  //! @param spos
  //! @return air::base::ADDR_DATUM_PTR
  air::base::ADDR_DATUM_PTR New_poly_var(const air::base::SPOS& spos);

  //! @brief Create a symbol with RNS_POLY pointer type
  air::base::ADDR_DATUM_PTR New_polys_var(const air::base::SPOS& spos);

  //! @brief Create a symbol for switch key, the type is predefined
  //! SWK_POLYS, an RNS_POLY array with hybrid keyswitch digit number
  air::base::ADDR_DATUM_PTR New_swk_var(const air::base::SPOS& spos);

  //! @brief Create a new polynomial preg
  CONST_VAR New_poly_preg();

  //! @brief Create a new ciphertext preg
  CONST_VAR New_ciph_preg();

  //! @brief Get poly field from var at given field id
  //! @param var CIPHER/CIPHER3/PLAIN variable
  //! @param fld_id field id
  //! @return polynomial field ptr
  air::base::FIELD_PTR Get_poly_fld(CONST_VAR var, uint32_t fld_id);

  //! @brief Create Modulus ptr type
  //! @param spos Variable declare spos
  //! @return air::base::TYPE_PTR
  air::base::TYPE_PTR New_modulus_ptr_type(const air::base::SPOS& spos);

  //! @brief Create Switch Key type - array of RNS_POLY
  //! Array size is set to hybrid keyswitch digit number
  //! @return air::base::TYPE_PTR
  air::base::TYPE_PTR New_swk_type();

  //! @brief Create Switch Key ptr type
  //! @param spos Variable declare spos
  //! @return air::base::TYPE_PTR
  air::base::TYPE_PTR New_swk_ptr_type(const air::base::SPOS& spos);

  //! @brief Create Public Key ptr type
  //! @param spos Variable declare spos
  //! @return air::base::TYPE_PTR
  air::base::TYPE_PTR New_pubkey_ptr_type(const air::base::SPOS& spos);

  //! @brief Create a new binary arithmetic op
  //! @param domain Domain ID of the opcode
  //! @param opcode Opcode Value
  //! @param lhs Binary op left hand side node
  //! @param rhs Binary op right hand side node
  //! @param spos Source position
  //! @return air::base::NODE_PTR
  air::base::NODE_PTR New_bin_arith(uint32_t domain, uint32_t opcode,
                                    air::base::CONST_TYPE_PTR rtype,
                                    air::base::NODE_PTR       lhs,
                                    air::base::NODE_PTR       rhs,
                                    const air::base::SPOS&    spos);

  //! @brief Create new function with predefined func_id
  //! @return New function scope
  air::base::FUNC_SCOPE* New_func(core::FHE_FUNC         func_id,
                                  const air::base::SPOS& spos);

  //! @brief Create load node for ADDR_DATUM/PREG
  air::base::NODE_PTR New_var_load(CONST_VAR var, const air::base::SPOS& spos);

  //! @brief Create store stmt for ADDR_DATUM/PREG
  air::base::STMT_PTR New_var_store(air::base::NODE_PTR val, CONST_VAR var,
                                    const air::base::SPOS& spos);

  //! @brief Create DO_LOOP to expand polynomial at each level
  //! @param induct_var loop induction variable
  //! @param upper_bound upper bound node
  //! @param start_idx loop start index value
  //! @param increment loop increment value
  //! @param spos source position
  //! @return air::base::STMT_PTR
  air::base::STMT_PTR New_loop(CONST_VAR           induct_var,
                               air::base::NODE_PTR upper_bound,
                               uint64_t start_idx, uint64_t increment,
                               const air::base::SPOS& spos);

  //! @brief Get num_q value from node attribute
  uint32_t Get_num_q(air::base::NODE_PTR node);

  //! @brief Get num_p value from node attribute
  uint32_t Get_num_p(air::base::NODE_PTR node);

  //! @brief Get num_qp value
  uint32_t Get_num_qp(air::base::NODE_PTR node);

  //! @brief Get num_decomp value
  uint32_t Get_num_decomp(air::base::NODE_PTR node);

  //! @brief Get start RNS basis index
  uint32_t Get_sbase(air::base::NODE_PTR node);

  //! @brief Get scaling factor degree
  uint32_t Get_sf_deg(air::base::NODE_PTR node);

  //! @brief Check if the RNS_POLY/POLY with coefficient mode
  //! RNS_POLY/POLY can hold either NTT or COEFF MODE.
  //! The MODE switching happens at at NTT/INTT operations
  bool Is_coeff_mode(air::base::NODE_PTR node);

  //! @brief Set node LEVEL/NUM_Q attribute
  void Set_num_q(air::base::NODE_PTR node, uint32_t num_q);

  //! @brief Set node NUM_P attribute
  void Set_num_p(air::base::NODE_PTR node, uint32_t num_p);

  //! @brief Set node SBASE attribute
  void Set_sbase(air::base::NODE_PTR node, uint32_t idx);

  //! @brief Set node SCALE attribute
  void Set_sf_deg(air::base::NODE_PTR node, uint32_t sf_deg);

  //! @brief Set node COEFF_MODE attribute
  void Set_coeff_mode(air::base::NODE_PTR node, uint32_t value);

  //! @brief Get poly offset
  //!        c0: 0, c1 = num_qp, c2 = 2 * num_qp
  uint32_t Get_poly_ofst(air::base::NODE_PTR node);

private:
  IR_GEN(const IR_GEN&);             // REQUIRED UNDEFINED UNWANTED methods
  IR_GEN& operator=(const IR_GEN&);  // REQUIRED UNDEFINED UNWANTED methods

  POLY_MEM_POOL* Mem_pool() { return _pool; }

  air::base::GLOB_SCOPE*            _glob_scope;
  air::base::FUNC_SCOPE*            _func_scope;
  air::base::CONTAINER*             _container;
  fhe::core::LOWER_CTX*             _ctx;
  std::map<uint64_t, VAR>           _predef_var;  // < {fs_id, var_id}, VAR >
  VAR_ARR                           _tmp_var;
  std::vector<air::base::TYPE_ID>   _ty_table;
  std::map<air::base::NODE_ID, VAR> _node2var_map;
  std::map<VAR, VAR_ARR>            _var_map;
  POLY_MEM_POOL*                    _pool;
};

}  // namespace poly
}  // namespace fhe

#endif