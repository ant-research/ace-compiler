//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_BASE_META_INFO_H
#define AIR_BASE_META_INFO_H

#include <vector>

#include "air/base/opcode.h"
#include "air/util/debug.h"

namespace air {
namespace base {

/**
 * @brief Operator's category
 * Each operator should only belong to 1 category
 *
 */
enum class OPR_CAT {
  /**! Operator is entry for function or region */
  ENTRY,
  /**! Operator is pragma */
  PRAGMA,
  /**! Operator is control flow */
  CFLOW,
  /**! Operator is load or store */
  LDST,
  /**! Operator is a generic statement */
  STMT,
  /**! Operator is a generic expression */
  EXPR,
  /**! Operator is a call */
  CALL,
  /**! Last category, should never be used */
  LAST_CAT
};  // OPR_CAT

/**
 * @brief Operator's property
 * Each operator has 1 or more properties
 *
 */
enum class OPR_PROP {
  EX_CHILD,   //!< Has extra child
  ENTRY,      //!< Operator has entry
  RET_VAR,    //!< Has return value to PREG
  SCF,        //!< Structured Control Flow
  NON_SCF,    //!< Non-Structured Control Flow
  END_BB,     //!< Operator ends current bb
  LEAF,       //!< Operator is leaf node
  STMT,       //!< Operator is statement
  EXPR,       //!< Operator is expression
  STORE,      //!< Operator is store
  LOAD,       //!< Operator is load
  CALL,       //!< Operator is call
  COMPARE,    //!< Operator is compare
  BOOLEAN,    //!< Operator generates boolean result
  NOT_EXEC,   //!< Operator isn't executable
  SYM,        //!< Operator has symbol
  TYPE,       //!< Operator has type
  LABEL,      //!< Operator has label
  OFFSET,     //!< Operator has offset
  VALUE,      //!< Operator has value
  FLAGS,      //!< Operator has flag
  FIELD_ID,   //!< Operator has field id
  CONST_ID,   //!< Operator has const id
  BARRIER,    //!< Operator is barrier
  PREFETCH,   //!< Operator is prefetch
  ATTR,       //!< Operator has attribute
  ACC_TYPE,   //!< Operator has access type
  PREG,       //!< Operator has preg
  LIB_CALL,   //!< Operator is mapped to library call
  LAST_PROP,  //!< Last operator property, should never be used
};            // OPR_PROP

/**
 * @brief Convert properties enum to integer value
 *
 */
#define PROP_TO_INT(prop) (1 << (int)prop)

/**
 * @brief Predefined integer value for properties
 *
 */
static constexpr uint32_t PROP_EX_CHILD = PROP_TO_INT(OPR_PROP::EX_CHILD);
static constexpr uint32_t PROP_ENTRY    = PROP_TO_INT(OPR_PROP::ENTRY);
static constexpr uint32_t PROP_RET_VAR  = PROP_TO_INT(OPR_PROP::RET_VAR);
static constexpr uint32_t PROP_SCF      = PROP_TO_INT(OPR_PROP::SCF);
static constexpr uint32_t PROP_NON_SCF  = PROP_TO_INT(OPR_PROP::NON_SCF);
static constexpr uint32_t PROP_END_BB   = PROP_TO_INT(OPR_PROP::END_BB);
static constexpr uint32_t PROP_LEAF     = PROP_TO_INT(OPR_PROP::LEAF);
static constexpr uint32_t PROP_STMT     = PROP_TO_INT(OPR_PROP::STMT);
static constexpr uint32_t PROP_EXPR     = PROP_TO_INT(OPR_PROP::EXPR);
static constexpr uint32_t PROP_STORE    = PROP_TO_INT(OPR_PROP::STORE);
static constexpr uint32_t PROP_LOAD     = PROP_TO_INT(OPR_PROP::LOAD);
static constexpr uint32_t PROP_CALL     = PROP_TO_INT(OPR_PROP::CALL);
static constexpr uint32_t PROP_COMPARE  = PROP_TO_INT(OPR_PROP::COMPARE);
static constexpr uint32_t PROP_BOOLEAN  = PROP_TO_INT(OPR_PROP::BOOLEAN);
static constexpr uint32_t PROP_NOT_EXEC = PROP_TO_INT(OPR_PROP::NOT_EXEC);
static constexpr uint32_t PROP_SYM      = PROP_TO_INT(OPR_PROP::SYM);
static constexpr uint32_t PROP_TYPE     = PROP_TO_INT(OPR_PROP::TYPE);
static constexpr uint32_t PROP_LABEL    = PROP_TO_INT(OPR_PROP::LABEL);
static constexpr uint32_t PROP_OFFSET   = PROP_TO_INT(OPR_PROP::OFFSET);
static constexpr uint32_t PROP_VALUE    = PROP_TO_INT(OPR_PROP::VALUE);
static constexpr uint32_t PROP_FLAGS    = PROP_TO_INT(OPR_PROP::FLAGS);
static constexpr uint32_t PROP_FIELD_ID = PROP_TO_INT(OPR_PROP::FIELD_ID);
static constexpr uint32_t PROP_CONST_ID = PROP_TO_INT(OPR_PROP::CONST_ID);
static constexpr uint32_t PROP_BARRIER  = PROP_TO_INT(OPR_PROP::BARRIER);
static constexpr uint32_t PROP_PREFETCH = PROP_TO_INT(OPR_PROP::PREFETCH);
static constexpr uint32_t PROP_ATTR     = PROP_TO_INT(OPR_PROP::ATTR);
static constexpr uint32_t PROP_ACC_TYPE = PROP_TO_INT(OPR_PROP::ACC_TYPE);
static constexpr uint32_t PROP_PREG     = PROP_TO_INT(OPR_PROP::PREG);
static constexpr uint32_t PROP_LIB_CALL = PROP_TO_INT(OPR_PROP::LIB_CALL);

/**
 * @brief Number of operator kids
 *
 */
enum OPR_KIDS : uint16_t { FLEXIBLE = (uint16_t)-1 };

/**
 * @brief Operator information
 * Meta information to describe a operator, include its name, number of
 * kids, category and properties.
 *
 */
struct OPR_INFO {
  /**! Operator name */
  char _name[31];
  /**! Operator category */
  OPR_CAT _cate : 8;
  /**! Number of operator kids. FLEXIBLE(-1) means no fixed kid number */
  uint16_t _nkids;
  /**! Number of operator fields */
  uint16_t _nflds;
  /**! Operator property */
  uint32_t _prop;
};  // OPR_INFO

// uint32_t used for _prop in OPR_INFO, make sure LAST_PROP is less than 32
AIR_STATIC_ASSERT((int)OPR_PROP::LAST_PROP < 32);

/**
 * @brief Domain information
 * Meta information to describe a domain, include its name, id and operator
 * information.
 *
 */
struct DOMAIN_INFO {
  /**! Domain name */
  char _name[23];
  /**! Domain id, must be unique */
  uint8_t _id;
  /**! Number of operators in the domain */
  uint32_t _nopr;
  /**! Pointer to operator info array */
  const OPR_INFO* _opr_info;
};  // DOMAIN_INFO

/**
 * @brief Domain and operator meta info registry
 * Central registry for all available domains and operators.
 *
 */
class META_INFO {
public:
  /**
   * @brief Get operator category name
   *
   * @param cat Operator category
   * @return const char* Name of the category
   */
  static const char* Op_cate_name(OPR_CAT cat);

  /**
   * @brief Get operator property name
   *
   * @param prop Operator property
   * @return const char* Name of the property
   */
  static const char* Op_prop_name(OPR_PROP prop);

  /**
   * @brief Get number of domains
   *
   * @return uint32_t number of domains
   */
  static uint32_t Num_domain() { return Domains.size(); }

  /**
   * @brief Get domain name
   *
   * @param id Domain id
   * @return const char* Domain name
   */
  static const char* Domain_name(uint32_t id) {
    AIR_ASSERT(Valid_domain(id));
    return Domains[id]->_name;
  }

  static uint32_t Num_domain_op(uint32_t id) {
    AIR_ASSERT(Valid_domain(id));
    return Domains[id]->_nopr;
  }

  /**
   * @brief Operator name
   *
   * @param id Domain id
   * @param opr Operator id
   * @return const char* Operator name
   */
  static const char* Op_name(uint32_t id, uint32_t opr) {
    AIR_ASSERT(Valid_operator(id, opr));
    return Domains[id]->_opr_info[opr]._name;
  }

  /**
   * @brief Operator name
   *
   * @param opc OPCODE
   * @return const char* Operator name
   */
  static const char* Op_name(OPCODE opc) {
    return Op_name(opc.Domain(), opc.Operator());
  }

  /**
   * @brief Number of operator kids, -1 means no fixed number of kids
   *
   * @param id Domain id
   * @param opr Operator id
   * @return uint32_t Number of kids. -1 means number of kids is flexible
   */
  static uint32_t Op_num_child(uint32_t id, uint32_t opr) {
    AIR_ASSERT(Valid_operator(id, opr));
    return Domains[id]->_opr_info[opr]._nkids;
  }

  /**
   * @brief Number of operator kids, -1 means no fixed number of kids
   *
   * @param opc OPCODE
   * @return uint32_t Number of kids. -1 means number of kids is flexible
   */
  static uint32_t Op_num_child(OPCODE opc) {
    return Op_num_child(opc.Domain(), opc.Operator());
  }

  /**
   * @brief Number of operator fields.
   *
   * @param id Domain id
   * @param opr Operator id
   * @return uint32_t Number of operator fields
   */
  static uint32_t Op_num_field(uint32_t id, uint32_t opr) {
    AIR_ASSERT(Valid_operator(id, opr));
    return Domains[id]->_opr_info[opr]._nflds;
  }

  /**
   * @brief Number of operator fields
   *
   * @param opc OPCODE
   * @return uint32_t Number of operator fields
   */
  static uint32_t Op_num_field(OPCODE opc) {
    return Op_num_field(opc.Domain(), opc.Operator());
  }

  /**
   * @brief Operator category
   *
   * @param id Domain id
   * @param opr Operator id
   * @return OPR_CAT Operator category
   */
  static OPR_CAT Op_category(uint32_t id, uint32_t opr) {
    AIR_ASSERT(Valid_operator(id, opr));
    return Domains[id]->_opr_info[opr]._cate;
  }

  /**
   * @brief Operator category
   *
   * @param opc OPCODE
   * @return OPR_CAT Operator category
   */
  static OPR_CAT Op_category(OPCODE opc) {
    return Op_category(opc.Domain(), opc.Operator());
  }

  /**
   * @brief Operator category name
   *
   * @param id Domain id
   * @param opr Operator id
   * @return const char* Operator category name
   */
  static const char* Op_category_name(uint32_t id, uint32_t opr) {
    return Op_cate_name(Op_category(id, opr));
  }

  /**
   * @brief Operator category name
   *
   * @param opc OPCODE
   * @return const char* Operator category name
   */
  static const char* Op_category_name(OPCODE opc) {
    return Op_category_name(opc.Domain(), opc.Operator());
  }

  /**
   * @brief Operator properties
   *
   * @param id Domain id
   * @param opr Operator id
   * @return uint32_t Operator properties
   */
  static uint32_t Op_properties(uint32_t id, uint32_t opr) {
    AIR_ASSERT(Valid_operator(id, opr));
    return Domains[id]->_opr_info[opr]._prop;
  }

  /**
   * @brief Check if operator has given property
   *
   * @param id Domain id
   * @param opr Operator id
   * @param prop Property to check
   * @return true Operator has the property
   * @return false Operator doesn't have the property
   */
  static bool Op_has_prop(uint32_t id, uint32_t opr, OPR_PROP prop) {
    uint32_t op_prop = Op_properties(id, opr);
    return (op_prop & PROP_TO_INT(prop)) != 0;
  }

  /**
   * @brief Check if operator has given property
   *
   * @param opc OPCODE
   * @param prop Property to check
   * @return true Operator has the property
   * @return false Operator doesn't have the property
   */
  static bool Op_has_prop(OPCODE opc, OPR_PROP prop) {
    return Op_has_prop(opc.Domain(), opc.Operator(), prop);
  }

  /**
   * @brief Check if operator has given property
   *
   * @param id Domain id
   * @param opr Operator id
   * @return true Operator has given property
   * @return false Operator doesn't have given property
   */
  template <OPR_PROP _prop>
  static bool Has_prop(uint32_t id, uint32_t opr) {
    uint32_t op_prop = Op_properties(id, opr);
    return (op_prop & PROP_TO_INT(_prop)) != 0;
  }

  /**
   * @brief Check if operator has given property
   *
   * @tparam _prop Property to check
   * @param opc OPCODE
   * @return true Operator has given property
   * @return false Operator doesn't have given property
   */
  template <OPR_PROP _prop>
  static bool Has_prop(OPCODE opc) {
    return Has_prop<_prop>(opc.Domain(), opc.Operator());
  }

  /**
   * @brief Check if a domain id is valid
   *
   * @param id Domain id
   * @return true Domain id is valid
   * @return false Domain id is invalid
   */
  static bool Valid_domain(uint32_t id) {
    return id < Domains.size() && Domains[id] != nullptr &&
           id == Domains[id]->_id;
  }

  /**
   * @brief Check if an opr in domain is valid
   *
   * @param id Domain id
   * @param opr Operator id
   * @return true Operator in the domain is valid
   * @return false Operator in the domain is invalid
   */
  static bool Valid_operator(uint32_t id, uint32_t opr) {
    return Valid_domain(id) && opr < Domains[id]->_nopr;
  }

  /**
   * @brief Check if an OPCODE is valid
   *
   * @param opc OPCODE
   * @return true OPCODE has valid domain and operator
   * @return false OPCODE domain or operator is invalid
   */
  static bool Valid_opcode(OPCODE opc) {
    return Valid_operator(opc.Domain(), opc.Operator());
  }

  /**
   * @brief Register a domain to central registry
   * Register a domain to the registry. Registry only keeps the pointer to
   * the domain info so caller of this function must make sure the pointer
   * is always valid during the execution of the compiler
   *
   * @param info Domain info to be registered
   * @return true Register successfully
   * @return false Failed to register. Usually caused by duplicated domain id
   */
  static bool Register_domain(const DOMAIN_INFO* info);

  /**
   * @brief Remove all registered domain information. For unittest ONLY.
   *
   */
  static void Remove_all() { Domains.clear(); }

  /**
   * @brief Print out opcodes of all registered domains
   *
   * @param os Out stream
   */
  static void Print(std::ostream& os);

  /**
   * @brief Print out OPR_INFO
   *
   * @param opr Operator Info
   */
  static void Print(std::ostream& os, const OPR_INFO* opr);

private:
  // vector to keep all domain info pointers
  static std::vector<const DOMAIN_INFO*> Domains;
};  // META_INFO

}  // namespace base
}  // namespace air

#endif  // AIR_BASE_META_INFO_H
