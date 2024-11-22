//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_POLY_POLY_IR_GEN_H
#define FHE_POLY_POLY_IR_GEN_H

#include <map>
#include <tuple>

#include "air/base/container.h"
#include "air/base/node.h"
#include "air/base/ptr_wrapper.h"
#include "air/base/st.h"
#include "air/util/mem_allocator.h"
#include "air/util/mem_pool.h"
#include "fhe/core/lower_ctx.h"
#include "fhe/poly/opcode.h"

namespace fhe {

namespace poly {

class VAR;
using GLOB_SCOPE      = air::base::GLOB_SCOPE;
using FUNC_SCOPE      = air::base::FUNC_SCOPE;
using VAR_ID          = air::base::ADDR_DATUM_ID;
using TYPE_ID         = air::base::TYPE_ID;
using TYPE_PTR        = air::base::TYPE_PTR;
using FIELD_PTR       = air::base::FIELD_PTR;
using SPOS            = air::base::SPOS;
using NODE_ID         = air::base::NODE_ID;
using STMT_PTR        = air::base::STMT_PTR;
using NODE_PTR        = air::base::NODE_PTR;
using STMT_LIST       = air::base::STMT_LIST;
using FUNC_SCOPE_ITER = air::base::GLOB_SCOPE::FUNC_SCOPE_ITER;
using CONTAINER       = air::base::CONTAINER;
using CONSTANT_PTR    = air::base::CONSTANT_PTR;
using PREG_PTR        = air::base::PREG_PTR;
using POLY_MEM_POOL   = air::util::STACKED_MEM_POOL<4096>;
using POLY_ALLOCATOR  = air::util::MEM_ALLOCATOR<POLY_MEM_POOL>;
using STR             = const char*;
using NODE_PAIR       = std::pair<NODE_PTR, NODE_PTR>;
using NODE_TRIPLE     = std::tuple<NODE_PTR, NODE_PTR, NODE_PTR>;
using STMT_PAIR       = std::pair<STMT_PTR, STMT_PTR>;
using STMT_TRIPLE     = std::tuple<STMT_PTR, STMT_PTR, STMT_PTR>;
using CONST_VAR       = const VAR;
#define UNK_LEVEL            -1
#define POLY_TMP_NAME_PREFIX "_poly"

/**
 * @brief Type kind used in Polynomial IR
 *
 */
enum VAR_TYPE_KIND : uint8_t {
  INDEX,
  POLY,
  PLAIN,
  CIPH,
  CIPH3,
  CIPH_PTR,
  POLY_COEFFS,
  MODULUS_PTR,
  SWK_PTR,
  PUBKEY_PTR,
  INT_PTR,
  LAST_TYPE
};

/**
 * @brief Local variables been created in polynomial IR
 *
 */
enum POLY_PREDEF_VAR : uint8_t {
  VAR_NUM_Q,
  VAR_NUM_P,
  VAR_P_OFST,
  VAR_P_IDX,
  VAR_KEY_P_OFST,
  VAR_KEY_P_IDX,
  VAR_RNS_IDX,
  VAR_PART_IDX,
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
  LAST_VAR,
};

//! An encapsulation for air base ADDR_DATUM and PREG
class VAR {
public:
  VAR() : _fscope(nullptr), _is_preg(false) { _var_id = air::base::Null_st_id; }

  template <typename VAR_TYPE>
  VAR(FUNC_SCOPE* fscope, VAR_TYPE var) {
    Set_var(fscope, var);
  }

  VAR(CONST_VAR& var) {
    _fscope  = var._fscope;
    _is_preg = var._is_preg;
    _var_id  = var._var_id;
  }

  VAR& operator=(CONST_VAR& var) {
    _fscope  = var._fscope;
    _is_preg = var._is_preg;
    _var_id  = var._var_id;
    return *this;
  }

  void Set_var(FUNC_SCOPE* fscope, air::base::ADDR_DATUM_PTR var) {
    _is_preg = false;
    _var_id  = var->Id().Value();
    _fscope  = fscope;
  }

  void Set_var(FUNC_SCOPE* fscope, air::base::PREG_PTR preg) {
    _is_preg = true;
    _var_id  = preg->Id().Value();
    _fscope  = fscope;
  }

  air::base::FUNC_SCOPE* Func_scope(void) const { return _fscope; }

  bool Is_null(void) const { return _var_id == air::base::Null_st_id; }

  bool Is_preg(void) const { return _is_preg; }

  bool Is_sym(void) const { return !_is_preg; }

  TYPE_PTR Type(void) const { return _fscope->Glob_scope().Type(Type_id()); }

  TYPE_ID Type_id(void) const {
    if (Is_preg()) {
      return Preg_var()->Type_id();
    } else {
      return Addr_var()->Type_id();
    }
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
    return (_var_id == other._var_id && _fscope == other._fscope);
  }

  bool operator!=(CONST_VAR& other) const { return !(*this == other); }

private:
  air::base::FUNC_SCOPE* _fscope;
  uint32_t               _var_id;
  bool                   _is_preg;
};

/**
 * @brief polynomial IR generation for symbol/type/node/stmt
 *
 */
class POLY_IR_GEN {
public:
  /**
   * @brief Construct a new poly ir gen object
   *
   * @param pool Memory pool for IR GEN
   */
  POLY_IR_GEN(CONTAINER* cont, fhe::core::LOWER_CTX* ctx, POLY_MEM_POOL* pool)
      : _glob_scope(cont ? cont->Glob_scope() : NULL),
        _func_scope(cont ? cont->Parent_func_scope() : NULL),
        _container(cont),
        _ctx(ctx),
        _pool(pool),
        _rotate_func_entry(air::base::Null_ptr),
        _predef_var(),
        _tmp_var(),
        _ty_table(VAR_TYPE_KIND::LAST_TYPE),
        _node2var_map() {}

  CONTAINER* Container() { return _container; }

  fhe::core::LOWER_CTX* Lower_ctx() { return _ctx; }

  GLOB_SCOPE* Glob_scope() { return _glob_scope; }

  FUNC_SCOPE* Func_scope() { return _func_scope; }
  /**
   * @brief Enter a function scope
   *
   * @param fscope Function Scope
   */
  void Enter_func(FUNC_SCOPE* fscope);

  /**
   * @brief Return or create a new polynomial type
   *
   * @return TYPE_PTR polynomial type
   */
  TYPE_PTR Poly_type(void);

  /**
   * @brief Returns cached pre-defined variables
   *
   * @return std::map<uint64_t, VAR>&
   */
  std::map<uint64_t, VAR>& Predef_var() { return _predef_var; }

  /**
   * @brief Return created tempory local Plain/Ciph/Poly variable
   *
   * @return std::vector<VAR>&
   */
  std::vector<VAR>& Tmp_var() { return _tmp_var; }

  /**
   * @brief Return cached types
   *
   * @return std::vector<TYPE_ID>&
   */
  std::vector<TYPE_ID>& Types() { return _ty_table; }

  /**
   * @brief Find type with given kind
   *
   * @param kind Type Kind
   * @return TYPE_PTR
   */
  TYPE_PTR Get_type(VAR_TYPE_KIND kind, const SPOS& spos);

  /**
   * @brief Find or create local variable with given id
   *
   * @param id Local variable id
   * @param spos source position
   * @return VAR Local Symbol
   */
  CONST_VAR& Get_var(POLY_PREDEF_VAR id, const SPOS& spos);

  /**
   * @brief Generate a uniq temporary name
   *
   * @return NAME_PTR
   */
  air::base::STR_PTR Gen_tmp_name();

  /**
   * @brief Create a new plaintext variable
   *
   * @param spos
   * @return air::base::ADDR_DATUM_PTR
   */
  air::base::ADDR_DATUM_PTR New_plain_var(const SPOS& spos);

  /**
   * @brief Create a new ciphertext variable
   *
   * @param spos
   * @return air::base::ADDR_DATUM_PTR
   */
  air::base::ADDR_DATUM_PTR New_ciph_var(const SPOS& spos);

  //! @brief Create a new CIPHER3 variable
  air::base::ADDR_DATUM_PTR New_ciph3_var(const SPOS& spos);

  /**
   * @brief Create a new polynomial variable
   *
   * @param spos
   * @return air::base::ADDR_DATUM_PTR
   */
  air::base::ADDR_DATUM_PTR New_poly_var(const SPOS& spos);

  //! @brief Get poly field from var at given field id
  //! @param var CIPHER/CIPHER3/PLAIN variable
  //! @param fld_id field id
  //! @return polynomial field ptr
  FIELD_PTR Get_poly_fld(CONST_VAR var, uint32_t fld_id);

  /**
   * @brief Add <node, var> to map
   *
   * @param node
   * @param var
   */
  CONST_VAR& Add_node_var(NODE_PTR node, air::base::ADDR_DATUM_PTR sym) {
    VAR var(Func_scope(), sym);
    CMPLR_ASSERT((_node2var_map.find(node->Id()) == _node2var_map.end() ||
                  _node2var_map[node->Id()] == var),
                 "node variable already exists");
    _node2var_map[node->Id()] = var;
    return _node2var_map[node->Id()];
  }

  CONST_VAR& Add_node_var(NODE_PTR node, air::base::PREG_PTR preg) {
    VAR var(Func_scope(), preg);
    AIR_ASSERT_MSG((_node2var_map.find(node->Id()) == _node2var_map.end() ||
                    _node2var_map[node->Id()] == var),
                   "node variable already exists");
    _node2var_map[node->Id()] = var;
    return _node2var_map[node->Id()];
  }

  bool Has_node_var(NODE_PTR node) {
    std::map<NODE_ID, VAR>::iterator iter = _node2var_map.find(node->Id());
    if (iter != _node2var_map.end()) {
      return true;
    } else {
      return false;
    }
  }

  //! @brief Find or create node's result symbol
  CONST_VAR& Node_var(NODE_PTR node) {
    std::map<NODE_ID, VAR>::iterator iter = _node2var_map.find(node->Id());
    if (iter != _node2var_map.end()) {
      return iter->second;
    } else if (node->Is_ld() && node->Has_sym()) {
      return Add_node_var(node, node->Addr_datum());
    } else if (node->Is_ld() && node->Has_preg()) {
      return Add_node_var(node, node->Preg());
    } else if (Lower_ctx()->Is_cipher_type(node->Rtype_id())) {
      return Add_node_var(node, New_ciph_var(node->Spos()));
    } else if (Lower_ctx()->Is_cipher3_type(node->Rtype_id())) {
      return Add_node_var(node, New_ciph3_var(node->Spos()));
    } else if (Lower_ctx()->Is_plain_type(node->Rtype_id())) {
      return Add_node_var(node, New_plain_var(node->Spos()));
    } else {
      CMPLR_ASSERT(false, "Node_var: unsupported node type");
    }
    return Add_node_var(air::base::Null_ptr,
                        (air::base::ADDR_DATUM_PTR)air::base::Null_ptr);
  }

  //! @brief Get cached rotate function entry
  air::base::ENTRY_PTR Rotate_entry() { return _rotate_func_entry; }

  //! @brief Set rotate function entry
  void Set_rotate_entry(air::base::ENTRY_PTR entry) {
    CMPLR_ASSERT(_rotate_func_entry == air::base::Null_ptr,
                 "rotate function already generated");
    _rotate_func_entry = entry;
  }

  //! @brief Get cached relinearize function entry
  air::base::ENTRY_PTR Relin_entry() { return _relin_func_entry; }

  //! @brief Set relinearize function entry
  void Set_relin_entry(air::base::ENTRY_PTR entry) {
    CMPLR_ASSERT(_relin_func_entry == air::base::Null_ptr,
                 "relinearize function already generated");
    _relin_func_entry = entry;
  }

  /**
   * @brief Create Modulus ptr type
   *
   * @param spos Variable declare spos
   * @return TYPE_PTR
   */
  TYPE_PTR New_modulus_ptr_type(const SPOS& spos);

  /**
   * @brief Create Switch Key ptr type
   *
   * @param spos Variable declare spos
   * @return TYPE_PTR
   */
  TYPE_PTR New_swk_ptr_type(const SPOS& spos);

  /**
   * @brief Create Public Key ptr type
   *
   * @param spos Variable declare spos
   * @return TYPE_PTR
   */
  TYPE_PTR New_pubkey_ptr_type(const SPOS& spos);

  /**
   * @brief Create a new binary arithmetic op
   *
   * @param domain Domain ID of the opcode
   * @param opcode Opcode Value
   * @param lhs Binary op left hand side node
   * @param rhs Binary op right hand side node
   * @param spos Source position
   * @return NODE_PTR
   */
  NODE_PTR New_bin_arith(uint32_t domain, uint32_t opcode, NODE_PTR lhs,
                         NODE_PTR rhs, const SPOS& spos);
  /**
   * @brief Create a new polynomial op
   *
   * @param opcode Polynomial opcode
   * @param spos Source position
   * @return NODE_PTR
   */
  NODE_PTR New_poly_node(OPCODE opcode, air::base::CONST_TYPE_PTR rtype,
                         const SPOS& spos);

  /**
   * @brief Create a new polynomial statement
   *
   * @param opcode Polynomial opcde
   * @param spos Source position
   * @return STMT_PTR
   */
  STMT_PTR New_poly_stmt(OPCODE opcode, const SPOS& spos);

  //! @brief Create rotate funtion
  //! @return New function scope
  FUNC_SCOPE* New_rotate_func();

  //! @brief Create relinearize function
  FUNC_SCOPE* New_relin_func();

  //! @brief Create fhe::ckks::CKKS_OPERATOR::encode
  NODE_PTR New_encode(NODE_PTR n_data, NODE_PTR n_len, NODE_PTR n_scale,
                      NODE_PTR n_level, const SPOS& spos);
  /**
   * @brief Create a new polynomial add node
   *
   * @param opnd1 Add operand 1
   * @param opnd2 Add operand 2
   * @param spos Source position
   * @return NODE_PTR
   */
  NODE_PTR New_poly_add(NODE_PTR opnd1, NODE_PTR opnd2, const SPOS& spos);

  //! @brief Create load node for ADDR_DATUM/PREG
  NODE_PTR New_var_load(CONST_VAR var, const SPOS& spos);

  //! @brief Create store stmt for ADDR_DATUM/PREG
  STMT_PTR New_var_store(NODE_PTR val, CONST_VAR var, const SPOS& spos);

  //! @brief Create load ciph's c0 and c1 poly node pair
  //! @param is_rns if true, load the poly at rns index
  //! @return NODE_PAIR two poly load node
  NODE_PAIR New_ciph_poly_load(CONST_VAR v_ciph, bool is_rns, const SPOS& spos);

  //! @brief Create load plain's poly node , if is_rns is true,
  //! @param is_rns if true, load the poly at rns index
  NODE_PTR New_plain_poly_load(CONST_VAR v_plain, bool is_rns,
                               const SPOS& spos);

  NODE_TRIPLE New_ciph3_poly_load(CONST_VAR v_ciph3, bool is_rns,
                                  const SPOS& spos);

  //! @brief Create two store to ciph's c0 and c1
  //! @param is_rns if true, store the poly at rns index
  //! @return STMT_PAIR two store statement
  STMT_PAIR New_ciph_poly_store(CONST_VAR v_ciph, NODE_PTR n_c0, NODE_PTR n_c1,
                                bool is_rns, const SPOS& spos);

  STMT_TRIPLE New_ciph3_poly_store(CONST_VAR v_ciph3, NODE_PTR n_c0,
                                   NODE_PTR n_c1, NODE_PTR n_c2, bool is_rns,
                                   const SPOS& spos);

  /**
   * @brief Create node to load a polynomial from ciphertext at given index
   *
   * @param var CIPHER/CIPHER3/PLAIN symbol ptr
   * @param load_idx Load index
   * @param spos Source position
   * @return NODE_PTR
   */
  NODE_PTR New_poly_load(CONST_VAR var, uint32_t load_idx, const SPOS& spos);

  /**
   * @brief Create node to load coeffcients from polynomial at given level
   *
   * @param n_poly Polynomial node
   * @param v_level Level symbol ptr
   * @return NODE_PTR
   */
  NODE_PTR New_poly_load_at_level(NODE_PTR n_poly, CONST_VAR v_level);

  /**
   * @brief Create node to load coeffcients from polynomial at given level
   *
   * @param n_poly Polynomial node
   * @param n_level Level node
   * @return NODE_PTR
   */
  NODE_PTR New_poly_load_at_level(NODE_PTR n_poly, NODE_PTR n_level);

  /**
   * @brief Create node to store a polynomial to ciphertext at given index
   *
   * @param store_val Store rhs node
   * @param var Store lhs symbol ptr CIPHER/CIPHER3/PLAIN
   * @param store_idx index to store
   * @param spos Source position
   * @return STMT_PTR
   */
  STMT_PTR New_poly_store(NODE_PTR store_val, CONST_VAR var, uint32_t store_idx,
                          const SPOS& spos);

  /**
   * @brief Create stmt to store coeffcients to polynomial at given level
   *
   * @param n_lhs Store lhs node (polynomial)
   * @param n_rhs Store rhs node (coeffcients)
   * @param v_level Polynomial level index to store
   * @return STMT_PTR
   */
  STMT_PTR New_poly_store_at_level(NODE_PTR n_lhs, NODE_PTR n_rhs,
                                   CONST_VAR v_level);

  /**
   * @brief Create stmt to store coeffcients to polynomial at given level
   *
   * @param n_lhs Store lhs node (polynomial)
   * @param n_rhs Store rhs node (coeffcients)
   * @param n_level Polynomial level index node
   * @return STMT_PTR
   */
  STMT_PTR New_poly_store_at_level(NODE_PTR n_lhs, NODE_PTR n_rhs,
                                   NODE_PTR n_level);

  /**
   * @brief Create stmt to init ciphertext, set level to min(level_rhs0,
   * level_rhs1) and set scaling factor to opnd scaling factor
   *
   * @param lhs Init Ciphertxt result
   * @param rhs_0 Init opnd 0, type is Ciphertext
   * @param rhs_1 Init opnd 1, rhs_1 type can be Ciphertext or Plaintext
   * @param spos Source position
   * @return STMT_PTR
   */
  STMT_PTR New_init_ciph_same_scale(NODE_PTR lhs, NODE_PTR rhs_0,
                                    NODE_PTR rhs_1, const SPOS& spos);

  /**
   * @brief Create stmt to init ciphertext, set level to min(level_rhs0,
   * level_rhs1) and set scaling factor to sc_rhs0 * sc_rhs1
   *
   * @param lhs Init Ciphertxt result
   * @param rhs_0 Init opnd 0, type is Ciphertext
   * @param rhs_1 Init opnd 1, rhs_1 type can be Ciphertext or Plaintext
   * @param spos Source position
   * @return STMT_PTR
   */
  STMT_PTR New_init_ciph_up_scale(NODE_PTR lhs, NODE_PTR rhs_0, NODE_PTR rhs_1,
                                  const SPOS& spos);

  /**
   * @brief Create stmt to init ciphertext, set scaling factor to sc_rhs - 1
   *
   * @param lhs Init Ciphertxt result
   * @param rhs Init opnd 0, type is Ciphertext
   * @param spos Source position
   */
  STMT_PTR New_init_ciph_down_scale(NODE_PTR lhs, NODE_PTR rhs,
                                    const SPOS& spos);

  //! Create init ciph statment
  STMT_PTR New_init_ciph(CONST_VAR v_parent, NODE_PTR node);

  /**
   * @brief Create node to get ring degree
   *
   * @param spos source position
   * @return NODE_PTR
   */
  NODE_PTR New_degree(const SPOS& spos);

  /**
   * @brief Create node to allocate memory for polynomial
   *
   * @param poly_node Polynomial node
   * @param is_ext Allocated with extended size(P primes included)
   * @param spos source position
   * @return NODE_PTR
   */
  NODE_PTR New_alloc_poly(NODE_PTR poly_node, bool is_ext, const SPOS& spos);

  //! @brief Create node to allocate memory for polynomial
  //! @param prime_cnt Number of primes in the polynomial
  NODE_PTR New_alloc_poly(uint32_t prime_cnt, const SPOS& spos);

  /**
   * @brief Create node to allocate memory for polynomial
   *
   * @param n_degree Polynomial degree node
   * @param n_num_q Polynomial number of q primes node
   * @param n_is_ext Is allocat a extended polynomial
   * @param spos Source position
   * @return NODE_PTR
   */
  NODE_PTR New_alloc_poly(NODE_PTR n_degree, NODE_PTR n_num_q,
                          NODE_PTR n_is_ext, const SPOS& spos);

  /**
   * @brief Create node to free polynomial memory
   *
   * @param n_poly Polynomial variable
   * @param spos Source position
   * @return STMT_PTR
   */
  STMT_PTR New_free_poly(CONST_VAR v_poly, const SPOS& spos);

  /**
   * @brief Create a node to get q primes number of a polynomial
   *
   * @param node Node
   * @return NODE_PTR
   */
  NODE_PTR New_get_num_q(NODE_PTR node, const SPOS& spos);

  /**
   * @brief Create a node to get the p primes number of a polynomial
   *
   * @param node Node
   * @return NODE_PTR
   */
  NODE_PTR New_num_p(NODE_PTR node, const SPOS& spos);

  /**
   * @brief Create a node to get the allocated primes of a polynomial
   *
   * @param node Node
   * @return NODE_PTR
   */
  NODE_PTR New_num_alloc(NODE_PTR node, const SPOS& spos);

  /**
   * @brief Create hardware modadd node
   *
   * @param opnd0 operand 0
   * @param opnd1 Operand 1
   * @param opnd2 Operand 2
   * @param spos Source position
   * @return NODE_PTR
   */
  NODE_PTR New_hw_modadd(NODE_PTR opnd0, NODE_PTR opnd1, NODE_PTR opnd2,
                         const SPOS& spos);

  /**
   * @brief Create hardware modmul node
   *
   * @param opnd0 operand 0
   * @param opnd1 Operand 1
   * @param opnd2 Operand 2
   * @param spos Source position
   * @return NODE_PTR
   */
  NODE_PTR New_hw_modmul(NODE_PTR opnd0, NODE_PTR opnd1, NODE_PTR opnd2,
                         const SPOS& spos);

  /**
   * @brief Create HW_ROTATE NODE
   *
   * @param opnd0 Rotate source node
   * @param opnd1 automorphism orders node
   * @param opnd2 Modulus node
   * @param spos Source position
   * @return NODE_PTR
   */
  NODE_PTR New_hw_rotate(NODE_PTR opnd0, NODE_PTR opnd1, NODE_PTR opnd2,
                         const SPOS& spos);

  /**
   * @brief Create RESCALE NODE
   *
   * @param opnd Rescale opnd node
   * @param spos Source position
   * @return NODE_PTR
   */
  NODE_PTR New_rescale(NODE_PTR n_opnd, const SPOS& spos);

  /**
   * @brief Create MOD_UP Node
   *
   * @param node Modup Source node
   * @param v_part_idx Part index variable
   * @param spos  Source position
   * @return NODE_PTR
   */
  NODE_PTR New_mod_up(NODE_PTR node, CONST_VAR v_part_idx, const SPOS& spos);

  /**
   * @brief Create MOD_DOWN node
   *
   * @param node Moddown Source node
   * @param spos  Source position
   * @return NODE_PTR
   */
  NODE_PTR New_mod_down(NODE_PTR node, const SPOS& spos);

  //! @brief Create DECOMP_MODUP node
  //! @param node Source node
  //! @param v_part_idx Decompose part index variable
  //! @param spos Source position
  //! @return NODE_PTR
  NODE_PTR New_decomp_modup(NODE_PTR node, CONST_VAR v_part_idx,
                            const SPOS& spos);

  /**
   * @brief Create node to get automorphism order
   *
   * @param node Rotation index node
   * @param spos Source position
   * @return NODE_PTR
   */
  NODE_PTR New_auto_order(NODE_PTR node, const SPOS& spos);

  /**
   * @brief Create node to get switch key from Rotation Key or Relinearize key
   *
   * @param is_rot Is a rotation key or a relin key
   * @param spos Source position
   * @param n_rot_idx Rotation index node
   * @return NODE_PTR
   */
  NODE_PTR New_swk(bool is_rot, const SPOS& spos,
                   NODE_PTR n_rot_idx = air::base::Null_ptr);

  /**
   * @brief Create node to decompose a polynomial
   *
   * @param node Decompose souce polynomial node
   * @param v_part_idx Part index variable
   * @param spos Source position
   * @return NODE_PTR
   */
  NODE_PTR New_decomp(NODE_PTR node, CONST_VAR v_part_idx, const SPOS& spos);

  /**
   * @brief Create node to get public key0 from switch key at given part index
   */
  NODE_PTR New_pk0_at(CONST_VAR v_swk, CONST_VAR v_part_idx, const SPOS& spos);

  /**
   * @brief Create node to get public key1 from switch key at given part index
   */
  NODE_PTR New_pk1_at(CONST_VAR v_swk, CONST_VAR v_part_idx, const SPOS& spos);

  /**
   * @brief Create block of nodes to perform key switch
   *
   * @param v_swk_c0 KeySwitch's c0 result variable
   * @param v_swk_c1 KeySwitch's c1 result variable
   * @param v_c1_ext KeySwitch Source c1_ext variable
   * @param v_key0 Public key0 variable
   * @param v_key1 Public key1 variable
   * @param spos Source position
   * @param is_ext Is P primes included
   * @return NODE_PTR
   */
  NODE_PTR New_key_switch(CONST_VAR v_swk_c0, CONST_VAR v_swk_c1,
                          CONST_VAR v_c1_ext, CONST_VAR v_key0,
                          CONST_VAR v_key1, const SPOS& spos, bool is_ext);

  /**
   * @brief Create DO_LOOP to expand polynomial at each level
   *
   * @param induct_var loop induction variable
   * @param upper_bound upper bound node
   * @param start_idx loop start index value
   * @param increment loop increment value
   * @param spos source position
   * @return STMT_PTR
   */
  STMT_PTR New_loop(CONST_VAR induct_var, NODE_PTR upper_bound,
                    uint64_t start_idx, uint64_t increment, const SPOS& spos);

  //! @brief Create block of nodes performs RNS loop operations
  //! @param node Polynomial node which performs RNS computation
  //! @param is_ext Is P primes included
  //! @return NODE_PTR pair <outer_blk, inner_blk>
  NODE_PAIR New_rns_loop(NODE_PTR node, bool is_ext);

  /**
   * @brief Create block of nodes perform Decompose loop operations
   *
   * @param node Node to be decompd
   * @param body_stmts Statements added in loop body
   * @param spos Source position
   * @return NODE_PTR Loop block node
   */
  NODE_PTR New_decomp_loop(NODE_PTR node, std::vector<STMT_PTR>& body_stmts,
                           const SPOS& spos);

  /**
   * @brief Create node to get next modulus
   *
   * @param mod_var modulus variable
   * @param spos Source position
   * @return NODE_PTR
   */
  NODE_PTR New_get_next_modulus(CONST_VAR mod_var, const SPOS& spos);

  /**
   * @brief Create Q_MODULUS node to get q modulus
   *
   * @param spos Source position
   * @return NODE_PTR
   */
  NODE_PTR New_q_modulus(const SPOS& spos);

  /**
   * @brief Create P_MODULUS node to get q modulus
   *
   * @param spos Source position
   * @return NODE_PTR
   */
  NODE_PTR New_p_modulus(const SPOS& spos);

  /**
   * @brief Append stmt to Container stmt list
   *
   * @param stmt STMT_PTR to append
   */
  void Append_stmt(STMT_PTR stmt, NODE_PTR parent_blk);

  //! @brief Append stmt to rns loop block, cannot directly Append to
  //  the End, statment should be inserted before last_statment (modulus++)
  void Append_rns_stmt(STMT_PTR stmt, NODE_PTR parent_blk);

  /**
   * @brief Compile time calculated polynomial's level
   *
   * @param node Polynomial node
   * @return int32_t
   */
  int32_t Get_level(NODE_PTR node);

private:
  POLY_IR_GEN(const POLY_IR_GEN&);  // REQUIRED UNDEFINED UNWANTED methods
  POLY_IR_GEN& operator=(
      const POLY_IR_GEN&);  // REQUIRED UNDEFINED UNWANTED methods

  POLY_MEM_POOL* Mem_pool() { return _pool; }

  GLOB_SCOPE*             _glob_scope;
  FUNC_SCOPE*             _func_scope;
  CONTAINER*              _container;
  fhe::core::LOWER_CTX*   _ctx;
  std::map<uint64_t, VAR> _predef_var;  // < {fs_id, var_id}, VAR >
  std::vector<VAR>        _tmp_var;
  std::vector<TYPE_ID>    _ty_table;
  std::map<NODE_ID, VAR>  _node2var_map;
  air::base::ENTRY_PTR    _rotate_func_entry;
  air::base::ENTRY_PTR    _relin_func_entry;
  POLY_MEM_POOL*          _pool;
};

}  // namespace poly

}  // namespace fhe

#endif  // FHE_POLY_POLY_IR_GEN_H
