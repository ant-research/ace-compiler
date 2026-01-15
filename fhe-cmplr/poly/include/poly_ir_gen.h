//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_POLY_POLY_IR_GEN_H
#define FHE_POLY_POLY_IR_GEN_H

#include "fhe/poly/ir_gen.h"
#include "fhe/poly/opcode.h"
namespace fhe {

namespace poly {

class POLY_IR_GEN : public IR_GEN {
public:
  POLY_IR_GEN(air::base::CONTAINER* cont, fhe::core::LOWER_CTX* ctx,
              POLY_MEM_POOL* pool)
      : IR_GEN(cont, ctx, pool) {}

  //! @brief Create a new polynomial op
  //! @param opcode Polynomial opcode
  //! @param spos Source position
  //! @return air::base::NODE_PTR
  air::base::NODE_PTR New_poly_node(OPCODE                    opcode,
                                    air::base::CONST_TYPE_PTR rtype,
                                    const air::base::SPOS&    spos);

  //! @brief Create a new polynomial statement
  //! @param opcode Polynomial opcde
  //! @param spos Source position
  //! @return air::base::STMT_PTR
  air::base::STMT_PTR New_poly_stmt(OPCODE opcode, const air::base::SPOS& spos);

  //! @brief Create fhe::ckks::CKKS_OPERATOR::encode
  air::base::NODE_PTR New_encode(air::base::NODE_PTR    n_data,
                                 air::base::NODE_PTR    n_len,
                                 air::base::NODE_PTR    n_scale,
                                 air::base::NODE_PTR    n_level,
                                 const air::base::SPOS& spos);

  //! @brief Create a new polynomial binary arithmetic node
  air::base::NODE_PTR New_bin_arith(OPCODE opc, air::base::NODE_PTR opnd0,
                                    air::base::NODE_PTR    opnd1,
                                    const air::base::SPOS& spos);

  //! @brief Create a new polynomial add node
  air::base::NODE_PTR New_poly_add(air::base::NODE_PTR    opnd0,
                                   air::base::NODE_PTR    opnd1,
                                   const air::base::SPOS& spos) {
    return New_bin_arith(ADD, opnd0, opnd1, spos);
  }

  //! @brief Create a new polynomial add_ext node
  air::base::NODE_PTR New_poly_add_ext(air::base::NODE_PTR    opnd0,
                                       air::base::NODE_PTR    opnd1,
                                       const air::base::SPOS& spos) {
    return New_bin_arith(ADD_EXT, opnd0, opnd1, spos);
  }

  //! @brief Create a new HPOLY sub node
  air::base::NODE_PTR New_poly_sub(air::base::NODE_PTR    opnd0,
                                   air::base::NODE_PTR    opnd1,
                                   const air::base::SPOS& spos) {
    return New_bin_arith(SUB, opnd0, opnd1, spos);
  }

  //! @brief Create a new HPOLY sub_ext node
  air::base::NODE_PTR New_poly_sub_ext(air::base::NODE_PTR    opnd0,
                                       air::base::NODE_PTR    opnd1,
                                       const air::base::SPOS& spos) {
    return New_bin_arith(SUB_EXT, opnd0, opnd1, spos);
  }

  //! @brief Create a new HPOLY mul node
  air::base::NODE_PTR New_poly_mul(air::base::NODE_PTR    opnd0,
                                   air::base::NODE_PTR    opnd1,
                                   const air::base::SPOS& spos) {
    return New_bin_arith(MUL, opnd0, opnd1, spos);
  }

  //! @brief Create a new HPOLY mul_ext node
  air::base::NODE_PTR New_poly_mul_ext(air::base::NODE_PTR    opnd0,
                                       air::base::NODE_PTR    opnd1,
                                       const air::base::SPOS& spos) {
    return New_bin_arith(MUL_EXT, opnd0, opnd1, spos);
  }

  //! @brief Create a new HPOLY mac node
  air::base::NODE_PTR New_poly_mac(air::base::NODE_PTR opnd0,
                                   air::base::NODE_PTR opnd1,
                                   air::base::NODE_PTR opnd2, bool is_ext,
                                   const air::base::SPOS& spos);

  //! @brief Create a new HPOLY rotate node
  air::base::NODE_PTR New_poly_rotate(air::base::NODE_PTR    opnd0,
                                      air::base::NODE_PTR    opnd1,
                                      const air::base::SPOS& spos);

  //! @brief Create load ciph's c0 and c1 poly node pair
  //! @param is_rns if true, load the poly at rns index
  //! @return NODE_PAIR two poly load node
  NODE_PAIR New_ciph_poly_load(CONST_VAR v_ciph, bool is_rns,
                               const air::base::SPOS& spos);

  //! @brief Create load plain's poly node , if is_rns is true,
  //! @param is_rns if true, load the poly at rns index
  air::base::NODE_PTR New_plain_poly_load(CONST_VAR v_plain, bool is_rns,
                                          const air::base::SPOS& spos);

  NODE_TRIPLE New_ciph3_poly_load(CONST_VAR v_ciph3, bool is_rns,
                                  const air::base::SPOS& spos);

  air::base::STMT_PTR New_plain_poly_store(CONST_VAR              v_plain,
                                           air::base::NODE_PTR    node,
                                           bool                   is_rns,
                                           const air::base::SPOS& spos);

  //! @brief Create two store to ciph's c0 and c1
  //! @param is_rns if true, store the poly at rns index
  //! @return STMT_PAIR two store statement
  STMT_PAIR New_ciph_poly_store(CONST_VAR v_ciph, air::base::NODE_PTR n_c0,
                                air::base::NODE_PTR n_c1, bool is_rns,
                                const air::base::SPOS& spos);

  STMT_TRIPLE New_ciph3_poly_store(CONST_VAR v_ciph3, air::base::NODE_PTR n_c0,
                                   air::base::NODE_PTR n_c1,
                                   air::base::NODE_PTR n_c2, bool is_rns,
                                   const air::base::SPOS& spos);

  //! @brief Create node to load a polynomial from ciphertext at given index
  //! @param var CIPHER/CIPHER3/PLAIN symbol ptr
  //! @param load_idx Load index
  //! @param spos Source position
  //! @return air::base::NODE_PTR
  air::base::NODE_PTR New_poly_load(CONST_VAR var, uint32_t load_idx,
                                    const air::base::SPOS& spos);

  //! @brief Create node to load coeffcients from polynomial at given level
  //! @param n_poly Polynomial node
  //! @param v_level Level symbol ptr
  //! @return air::base::NODE_PTR
  air::base::NODE_PTR New_poly_load_at_level(air::base::NODE_PTR n_poly,
                                             CONST_VAR           v_level);

  //! @brief Create node to load coeffcients from polynomial at given level
  //! @param n_poly Polynomial node
  //! @param n_level Level node
  //! @return air::base::NODE_PTR
  air::base::NODE_PTR New_poly_load_at_level(air::base::NODE_PTR n_poly,
                                             air::base::NODE_PTR n_level);

  //! @brief Create node to store a polynomial to ciphertext at given index
  //! @param store_val Store rhs node
  //! @param var Store lhs symbol ptr CIPHER/CIPHER3/PLAIN
  //! @param store_idx index to store
  //! @param spos Source position
  //! @return air::base::STMT_PTR
  air::base::STMT_PTR New_poly_store(air::base::NODE_PTR store_val,
                                     CONST_VAR var, uint32_t store_idx,
                                     const air::base::SPOS& spos);

  //! @brief Create stmt to store coeffcients to polynomial at given level
  //! @param n_lhs Store lhs node (polynomial)
  //! @param n_rhs Store rhs node (coeffcients)
  //! @param v_level Polynomial level index to store
  //! @return air::base::STMT_PTR
  air::base::STMT_PTR New_poly_store_at_level(air::base::NODE_PTR n_lhs,
                                              air::base::NODE_PTR n_rhs,
                                              CONST_VAR           v_level);

  //! @brief Create stmt to store coeffcients to polynomial at given level
  //! @param n_lhs Store lhs node (polynomial)
  //! @param n_rhs Store rhs node (coeffcients)
  //! @param n_level Polynomial level index node
  //! @return air::base::STMT_PTR
  air::base::STMT_PTR New_poly_store_at_level(air::base::NODE_PTR n_lhs,
                                              air::base::NODE_PTR n_rhs,
                                              air::base::NODE_PTR n_level);

  //! @brief Create stmt to init ciphertext, set level to min(level_rhs0,
  //! level_rhs1) and set scaling factor to opnd scaling factor
  //! @param lhs Init Ciphertxt result
  //! @param rhs_0 Init opnd 0, type is Ciphertext
  //! @param rhs_1 Init opnd 1, rhs_1 type can be Ciphertext or Plaintext
  //! @param spos Source position
  //! @return air::base::STMT_PTR
  air::base::STMT_PTR New_init_ciph_same_scale(air::base::NODE_PTR    lhs,
                                               air::base::NODE_PTR    rhs_0,
                                               air::base::NODE_PTR    rhs_1,
                                               const air::base::SPOS& spos);

  //! @brief Create stmt to init ciphertext, set level to min(level_rhs0,
  //!        level_rhs1) and set scaling factor to sc_rhs0 * sc_rhs1
  //! @param lhs Init Ciphertxt result
  //! @param rhs_0 Init opnd 0, type is Ciphertext
  //! @param rhs_1 Init opnd 1, rhs_1 type can be Ciphertext or Plaintext
  //! @param spos Source position
  //! @return air::base::STMT_PTR
  air::base::STMT_PTR New_init_ciph_up_scale(air::base::NODE_PTR    lhs,
                                             air::base::NODE_PTR    rhs_0,
                                             air::base::NODE_PTR    rhs_1,
                                             const air::base::SPOS& spos);

  //! @brief Create stmt to init ciphertext, set scaling factor to sc_rhs - 1
  //! @param lhs Init Ciphertxt result
  //! @param rhs Init opnd 0, type is Ciphertext
  //! @param spos Source position
  air::base::STMT_PTR New_init_ciph_down_scale(air::base::NODE_PTR    lhs,
                                               air::base::NODE_PTR    rhs,
                                               const air::base::SPOS& spos);

  //! Create init ciph statment
  air::base::STMT_PTR New_init_ciph(CONST_VAR           v_parent,
                                    air::base::NODE_PTR node);

  //! @brief Create stmt to init polynomial by node operands
  //! @param v_res result symbol
  //! @param node operation node(add/mul/rotate...)
  //! @param is_ext allocate result to extended RNS format
  //! @param spos Source position
  //! @return air::base::STMT_PTR
  air::base::STMT_PTR New_init_poly_by_opnd(CONST_VAR              v_res,
                                            air::base::NODE_PTR    node,
                                            bool                   is_ext,
                                            const air::base::SPOS& spos);
  air::base::STMT_PTR New_init_poly_by_opnd(air::base::NODE_PTR    n_res,
                                            air::base::NODE_PTR    n_opnd1,
                                            air::base::NODE_PTR    n_opnd2,
                                            bool                   ext,
                                            const air::base::SPOS& spos);
  air::base::STMT_PTR New_init_poly(CONST_VAR v_res, uint32_t num_q,
                                    bool is_ext, const air::base::SPOS& spos);
  air::base::STMT_PTR New_init_poly(CONST_VAR           v_res,
                                    air::base::NODE_PTR n_num_q, bool is_ext,
                                    const air::base::SPOS& spos);

  air::base::NODE_PTR New_alloc_n(air::base::NODE_PTR    n_cnt,
                                  air::base::NODE_PTR    n_degree,
                                  air::base::NODE_PTR    n_num_q,
                                  air::base::NODE_PTR    n_num_p,
                                  const air::base::SPOS& spos);

  //! @brief Create node to free polynomial memory
  //! @param v_poly RNS_POLY or RNS_POLYs
  //! @param spos Source position
  //! @return air::base::STMT_PTR
  air::base::STMT_PTR New_free_poly(CONST_VAR              v_poly,
                                    const air::base::SPOS& spos);

  //! @brief Create node to get ring degree
  //! @param spos source position
  //! @return air::base::NODE_PTR
  air::base::NODE_PTR New_degree(const air::base::SPOS& spos);

  //! @brief Create a node to get q primes number of a polynomial
  //! @param node Node
  //! @return air::base::NODE_PTR
  air::base::NODE_PTR New_num_q(air::base::NODE_PTR    node,
                                const air::base::SPOS& spos);

  //! @brief Create a node to get the p primes number of a polynomial
  //! @param node Node
  //! @return air::base::NODE_PTR
  air::base::NODE_PTR New_num_p(air::base::NODE_PTR    node,
                                const air::base::SPOS& spos);

  //! @brief Create a node to get the allocated primes of a polynomial
  //! @param node Node
  //! @return air::base::NODE_PTR
  air::base::NODE_PTR New_num_alloc(air::base::NODE_PTR    node,
                                    const air::base::SPOS& spos);

  //! @brief Create a node to get the number of decompose of a polynomial
  //! @param node Node
  //! @return air::base::NODE_PTR
  air::base::NODE_PTR New_num_decomp(air::base::NODE_PTR    node,
                                     const air::base::SPOS& spos);

  //! @brief Create hardware modadd node
  //! @param opnd0 operand 0
  //! @param opnd1 Operand 1
  //! @param opnd2 Operand 2
  //! @param spos Source position
  //! @return air::base::NODE_PTR
  air::base::NODE_PTR New_hw_modadd(air::base::NODE_PTR    opnd0,
                                    air::base::NODE_PTR    opnd1,
                                    air::base::NODE_PTR    opnd2,
                                    const air::base::SPOS& spos);

  //! @brief Create hardware modmul node
  //! @param opnd0 operand 0
  //! @param opnd1 Operand 1
  //! @param opnd2 Operand 2
  //! @param spos Source position
  //! @return air::base::NODE_PTR
  air::base::NODE_PTR New_hw_modmul(air::base::NODE_PTR    opnd0,
                                    air::base::NODE_PTR    opnd1,
                                    air::base::NODE_PTR    opnd2,
                                    const air::base::SPOS& spos);

  //! @brief Create HW_ROTATE NODE
  //! @param opnd0 Rotate source node
  //! @param opnd1 automorphism orders node
  //! @param opnd2 Modulus node
  //! @param spos Source position
  //! @return air::base::NODE_PTR
  air::base::NODE_PTR New_hw_rotate(air::base::NODE_PTR    opnd0,
                                    air::base::NODE_PTR    opnd1,
                                    air::base::NODE_PTR    opnd2,
                                    const air::base::SPOS& spos);

  //! @brief Create RESCALE NODE
  //! @param opnd Rescale opnd node
  //! @param spos Source position
  //! @return air::base::NODE_PTR
  air::base::NODE_PTR New_rescale(air::base::NODE_PTR    n_opnd,
                                  const air::base::SPOS& spos);

  //! @brief Create MODSWITCH NODE
  air::base::NODE_PTR New_modswitch(air::base::NODE_PTR    n_opnd,
                                    const air::base::SPOS& spos);

  //! @brief Create RAISE_MOD NODE
  air::base::NODE_PTR New_raise_mod(air::base::NODE_PTR    n_opnd0,
                                    air::base::NODE_PTR    n_opnd1,
                                    const air::base::SPOS& spos);

  //! @brief Create MOD_UP Node
  //! @param node Modup Source node
  //! @param v_part_idx Part index variable
  //! @param spos  Source position
  //! @return air::base::NODE_PTR
  air::base::NODE_PTR New_mod_up(air::base::NODE_PTR node, CONST_VAR v_part_idx,
                                 const air::base::SPOS& spos);

  //! @brief Create MOD_DOWN node
  //! @param node Moddown Source node
  //! @param spos  Source position
  //! @return air::base::NODE_PTR
  air::base::NODE_PTR New_mod_down(air::base::NODE_PTR    node,
                                   const air::base::SPOS& spos);

  //! @brief Create DECOMP_MODUP node
  //! @param node Source node
  //! @param v_part_idx Decompose part index variable
  //! @param spos Source position
  //! @return air::base::NODE_PTR
  air::base::NODE_PTR New_decomp_modup(air::base::NODE_PTR    node,
                                       CONST_VAR              v_part_idx,
                                       const air::base::SPOS& spos);

  air::base::NODE_PTR New_alloc_for_precomp(air::base::NODE_PTR    node,
                                            const air::base::SPOS& spos);

  air::base::NODE_PTR New_precomp(air::base::NODE_PTR    node,
                                  const air::base::SPOS& spos);

  air::base::NODE_PTR New_dot_prod(air::base::NODE_PTR    n0,
                                   air::base::NODE_PTR    n1,
                                   air::base::NODE_PTR    n2,
                                   const air::base::SPOS& spos);

  //! @brief Create node to get automorphism order
  //! @param node Rotation index node
  //! @param spos Source position
  //! @return air::base::NODE_PTR
  air::base::NODE_PTR New_auto_order(air::base::NODE_PTR    node,
                                     const air::base::SPOS& spos);

  //! @brief Create node to get switch key from Rotation Key or Relinearize key
  //! @param is_rot Is a rotation key or a relin key
  //! @param spos Source position
  //! @param n_rot_idx Rotation index node
  //! @return air::base::NODE_PTR
  air::base::NODE_PTR New_swk(
      bool is_rot, const air::base::SPOS& spos,
      air::base::NODE_PTR n_rot_idx = air::base::Null_ptr);

  air::base::NODE_PTR New_swk_c0(
      bool is_rot, const air::base::SPOS& spos,
      air::base::NODE_PTR n_rot_idx = air::base::Null_ptr);

  air::base::NODE_PTR New_swk_c1(
      bool is_rot, const air::base::SPOS& spos,
      air::base::NODE_PTR n_rot_idx = air::base::Null_ptr);

  //! @brief Create node to decompose a polynomial
  //! @param node Decompose souce polynomial node
  //! @param v_part_idx Part index variable
  //! @param spos Source position
  //! @return air::base::NODE_PTR
  air::base::NODE_PTR New_decomp(air::base::NODE_PTR node, CONST_VAR v_part_idx,
                                 const air::base::SPOS& spos);

  //! @brief Create node to get public key0 from switch key at given part index
  air::base::NODE_PTR New_pk0_at(CONST_VAR v_swk, CONST_VAR v_part_idx,
                                 const air::base::SPOS& spos);

  //! @brief Create node to get public key1 from switch key at given part index
  air::base::NODE_PTR New_pk1_at(CONST_VAR v_swk, CONST_VAR v_part_idx,
                                 const air::base::SPOS& spos);

  //! @brief Create block of nodes to perform key switch
  //! @param v_swk_c0 KeySwitch's c0 result variable
  //! @param v_swk_c1 KeySwitch's c1 result variable
  //! @param v_c1_ext KeySwitch Source c1_ext variable
  //! @param v_key0 Public key0 variable
  //! @param v_key1 Public key1 variable
  //! @param spos Source position
  //! @param is_ext Is P primes included
  //! @return air::base::NODE_PTR
  air::base::NODE_PTR New_key_switch(CONST_VAR v_swk_c0, CONST_VAR v_swk_c1,
                                     CONST_VAR v_c1_ext, CONST_VAR v_key0,
                                     CONST_VAR              v_key1,
                                     const air::base::SPOS& spos, bool is_ext);

  //! @brief Create block of nodes performs RNS loop operations
  //! @param node Polynomial node which performs RNS computation
  //! @param is_ext Is P primes included
  //! @return air::base::NODE_PTR pair <outer_blk, inner_blk>
  NODE_PAIR New_rns_loop(air::base::NODE_PTR node, bool is_ext);

  //! @brief Create block of nodes perform Decompose loop operations
  //! @param node Node to be decompd
  //! @param spos Source position
  //! @return air::base::STMT_PTR Loop statement
  air::base::STMT_PTR New_decomp_loop(air::base::NODE_PTR    node,
                                      const air::base::SPOS& spos);

  air::base::STMT_PTR New_adjust_p_idx(const air::base::SPOS& spos);

  //! @brief Create node to get next modulus
  //! @param mod_var modulus variable
  //! @param spos Source position
  //! @return air::base::NODE_PTR
  air::base::NODE_PTR New_get_next_modulus(CONST_VAR              mod_var,
                                           const air::base::SPOS& spos);

  //! @brief Create Q_MODULUS node to get q modulus
  //! @param spos Source position
  //! @return air::base::NODE_PTR
  air::base::NODE_PTR New_q_modulus(const air::base::SPOS& spos);

  //! @brief Create P_MODULUS node to get q modulus
  //! @param spos Source position
  //! @return air::base::NODE_PTR
  air::base::NODE_PTR New_p_modulus(const air::base::SPOS& spos);

  //! @brief Create node to extract RNS_POLY from node n0 with index ranges from
  //! nodes [n1, n2]
  air::base::NODE_PTR New_extract(air::base::NODE_PTR    n0,
                                  air::base::NODE_PTR    n1,
                                  air::base::NODE_PTR    n2,
                                  const air::base::SPOS& spos);

  //! @brief Create node to extract RNS_POLY from node n0 with index ranges from
  //! [s_idx, e_idx]
  air::base::NODE_PTR New_extract(air::base::NODE_PTR n0, uint32_t s_idx,
                                  uint32_t e_idx, const air::base::SPOS& spos);

  //! @brief Create node to concat RNS_POLY n0 and n1
  air::base::NODE_PTR New_concat(air::base::NODE_PTR n0, air::base::NODE_PTR n1,
                                 const air::base::SPOS& spos);

  //! @brief Create node to convert base for n0 to RNS basis with range [s_idx,
  //! e_idx] by convert constant n3
  air::base::NODE_PTR New_bconv(air::base::NODE_PTR n0, air::base::NODE_PTR n1,
                                uint32_t s_idx, uint32_t e_idx,
                                const air::base::SPOS& spos);

  air::base::NODE_PTR New_bswitch(air::base::NODE_PTR n0,
                                  air::base::NODE_PTR n1, uint32_t s_idx,
                                  uint32_t e_idx, const air::base::SPOS& spos);

  //! @brief Create node to perform RNS_POLY NTT convert
  air::base::NODE_PTR New_ntt(air::base::NODE_PTR    n0,
                              const air::base::SPOS& spos);

  //! @brief Create node to perform RNS_POLY INTT convert
  air::base::NODE_PTR New_intt(air::base::NODE_PTR    n0,
                               const air::base::SPOS& spos);

  //! @brief Create node to get the constant of $q_lhat^{-1} % q$ at specified
  //! part index and part size index
  air::base::NODE_PTR New_qlhinvmodq(uint32_t part_idx, uint32_t part_size_idx,
                                     const air::base::SPOS& spos);

  //! @brief Create node to get the constant of $q_l^{-1} % q_i$
  air::base::NODE_PTR New_qlinvmodq(uint32_t idx, const air::base::SPOS& spos);

  //! @brief Create node to get the constant of $q_l/2 % q_i$
  air::base::NODE_PTR New_qlhalfmodq(uint32_t               num_q,
                                     const air::base::SPOS& spos);

  //! @brief Create node to get the constant of $q0/2 % q_i$
  air::base::NODE_PTR New_q0halfmodq(const air::base::SPOS& spos);

  //! @brief Create node to get the constant of $q_lhat % p$ at specified
  //! q index and part index
  air::base::NODE_PTR New_qlhmodp(uint32_t q_idx, uint32_t part_idx,
                                  uint32_t start_idx, uint32_t len,
                                  const air::base::SPOS& spos);

  //! @brief Create node to get the constant of $p_hat^{-1} % p$
  air::base::NODE_PTR New_phinvmodp(const air::base::SPOS& spos);

  //! @brief Create node to get the constant of $p_hat % q$
  air::base::NODE_PTR New_phmodq(const air::base::SPOS& spos);

  //! @brief Create node to get the constant of $P^{-1} % q$
  air::base::NODE_PTR New_pinvmodq(const air::base::SPOS& spos);

  //! @brief Append stmt to rns loop block, cannot directly Append to
  //  the End, statment should be inserted before last_statment (modulus++)
  void Append_rns_stmt(air::base::STMT_PTR stmt,
                       air::base::NODE_PTR parent_blk);

  //! @brief Compile time calculated polynomial's level
  //! @param node Polynomial node
  //! @return int32_t
  int32_t Get_level(air::base::NODE_PTR node);

  void Set_mul_ciph(air::base::NODE_PTR node);
};

}  // namespace poly

}  // namespace fhe

#endif  // FHE_POLY_POLY_IR_GEN_H
