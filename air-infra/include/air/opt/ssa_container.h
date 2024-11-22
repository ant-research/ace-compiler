//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OPT_SSA_CONTAINER_H
#define AIR_OPT_SSA_CONTAINER_H

#include <unordered_map>

#include "air/base/container.h"
#include "air/base/meta_info.h"
#include "air/core/opcode.h"
#include "air/opt/ssa_node.h"
#include "air/opt/ssa_st.h"

namespace air {

namespace opt {

enum LOOP_PHI_OPND_ID : uint8_t {
  PREHEADER_PHI_OPND_ID = 0,
  BACK_EDGE_PHI_OPND_ID = 1,
};

//! @brief SSA CONTAINER
//! Contains all SSA related tables, include:
//!   - SSA Symbol Table
//!   - SSA Version Table
//!   - SSA PHI_NODE Table
//! and maps from NODE_ID to SSA object ID:
//!   - SYM/VER Map
//!   - PHI Map

//! @brief SSA_CONTAINER
class SSA_CONTAINER {
public:
  SSA_CONTAINER(air::base::CONTAINER* cont);
  ~SSA_CONTAINER();

  air::base::CONTAINER*  Container() const { return _cont; }
  air::base::FUNC_SCOPE* Func_scope() const {
    return _cont->Parent_func_scope();
  }
  air::base::GLOB_SCOPE* Glob_scope() const { return _cont->Glob_scope(); }

  SSA_SYM_PTR New_sym(SSA_SYM_KIND kind, uint32_t var, uint32_t id);
  SSA_VER_PTR New_ver(VER_DEF_KIND kind, SSA_SYM_ID sym);

  MU_NODE_PTR  New_mu(SSA_SYM_ID sym);
  CHI_NODE_PTR New_chi(SSA_SYM_ID sym);
  PHI_NODE_PTR New_phi(SSA_SYM_ID sym, uint32_t size);

  SSA_SYM_PTR Sym(SSA_SYM_ID sym) const {
    return SSA_SYM_PTR(SSA_SYM(this, _sym_tab->Find(sym)));
  }
  SSA_VER_PTR Ver(SSA_VER_ID ver) const {
    return SSA_VER_PTR(SSA_VER(this, _ver_tab->Find(ver)));
  }
  SSA_SYM_PTR Ver_sym(SSA_VER_ID ver) const {
    SSA_VER_PTR ver_ptr = Ver(ver);
    return Sym(ver_ptr->Sym_id());
  }

  MU_NODE_PTR Mu_node(MU_NODE_ID mu) const {
    return MU_NODE_PTR(MU_NODE(this, _mu_tab->Find(mu)));
  }
  CHI_NODE_PTR Chi_node(CHI_NODE_ID chi) const {
    return CHI_NODE_PTR(CHI_NODE(this, _chi_tab->Find(chi)));
  }
  PHI_NODE_PTR Phi_node(PHI_NODE_ID phi) const {
    return PHI_NODE_PTR(PHI_NODE(this, _phi_tab->Find(phi)));
  }

  MU_NODE_PTR  Node(MU_NODE_ID mu) const { return Mu_node(mu); }
  CHI_NODE_PTR Node(CHI_NODE_ID mu) const { return Chi_node(mu); }
  PHI_NODE_PTR Node(PHI_NODE_ID mu) const { return Phi_node(mu); }

  uint32_t Num_sym() const { return _sym_tab->Size(); }
  uint32_t Num_ver() const { return _ver_tab->Size(); }
  uint32_t Num_mu() const { return _mu_tab->Size(); }
  uint32_t Num_chi() const { return _chi_tab->Size(); }
  uint32_t Num_phi() const { return _phi_tab->Size(); }

  SSA_SYM_ID Node_sym_id(air::base::NODE_ID node) const {
    U32_U32_MAP::const_iterator it = _sym_map->find(node.Value());
    return (it != _sym_map->end()) ? SSA_SYM_ID(it->second) : SSA_SYM_ID();
  }
  SSA_VER_ID Node_ver_id(air::base::NODE_ID node) const {
    U32_U32_MAP::const_iterator it = _ver_map->find(node.Value());
    return (it != _ver_map->end()) ? SSA_VER_ID(it->second) : SSA_VER_ID();
  }
  SSA_SYM_PTR Node_sym(air::base::NODE_ID node) const {
    return Sym(Node_sym_id(node));
  }
  SSA_VER_PTR Node_ver(air::base::NODE_ID node) const {
    return Ver(Node_ver_id(node));
  }

  MU_NODE_ID Node_mu(air::base::NODE_ID node) const {
    AIR_ASSERT(Has_mu(_cont->Node(node)));
    U32_U32_MAP::const_iterator it = _mu_map->find(node.Value());
    return (it != _mu_map->end()) ? MU_NODE_ID(it->second) : MU_NODE_ID();
  }
  CHI_NODE_ID Node_chi(air::base::NODE_ID node) const {
    AIR_ASSERT(Has_chi(_cont->Node(node)));
    U32_U32_MAP::const_iterator it = _chi_map->find(node.Value());
    return (it != _chi_map->end()) ? CHI_NODE_ID(it->second) : CHI_NODE_ID();
  }
  PHI_NODE_ID Node_phi(air::base::NODE_ID node) const {
    AIR_ASSERT(Has_phi(_cont->Node(node)));
    U32_U32_MAP::const_iterator it = _phi_map->find(node.Value());
    return (it != _phi_map->end()) ? PHI_NODE_ID(it->second) : PHI_NODE_ID();
  }

  void Set_node_sym(air::base::NODE_ID node, SSA_SYM_ID sym) {
    AIR_ASSERT(Has_sym(_cont->Node(node)) || Sym(sym)->Is_iv());
    AIR_ASSERT(sym.Value() < _sym_tab->Size());
    (*_sym_map)[node.Value()] = sym.Value();
  }

  void Set_node_ver(air::base::NODE_ID node, SSA_VER_ID ver) {
    AIR_ASSERT(Has_ver(_cont->Node(node)) || Ver_sym(ver)->Is_iv());
    AIR_ASSERT(ver.Value() < _ver_tab->Size());
    (*_ver_map)[node.Value()] = ver.Value();
  }

  void Set_node_mu(air::base::NODE_ID node, MU_NODE_ID mu) {
    AIR_ASSERT(Has_mu(_cont->Node(node)));
    AIR_ASSERT(mu.Value() < _mu_tab->Size());
    (*_mu_map)[node.Value()] = mu.Value();
  }
  void Set_node_chi(air::base::NODE_ID node, CHI_NODE_ID chi) {
    AIR_ASSERT(Has_chi(_cont->Node(node)));
    AIR_ASSERT(chi.Value() < _chi_tab->Size());
    (*_chi_map)[node.Value()] = chi.Value();
  }
  void Set_node_phi(air::base::NODE_ID node, PHI_NODE_ID phi) {
    AIR_ASSERT(Has_phi(_cont->Node(node)));
    AIR_ASSERT(phi.Value() < _phi_tab->Size());
    (*_phi_map)[node.Value()] = phi.Value();
  }

public:
  //! @brief SSA container state
  enum STATE {
    NO_SSA,       //!< No SSA. Before SSA Build
    SYM_CREATE,   //!< During SSA Symbol Creation Phase
    PHI_INSERT,   //!< During SSA PHI Insertion Phase
    RENAME,       //!< During SSA Renaming Phase
    SSA,          //!< Valid SSA. After SSA Build
    INVALID_SSA,  //!< Invalid SSA. IR was updated.
  };

  STATE State() const { return _state; }
  void  Set_state(STATE state) { _state = state; }

public:
  //! @brief Get size of phi operands for given NODE
  static uint32_t Phi_size(air::base::NODE_PTR node) {
    if (node->Opcode() == air::core::OPC_DO_LOOP ||
        node->Opcode() == air::core::OPC_IF) {
      return 2;
    }
    AIR_ASSERT(false);
    return 0;
  }

  //! @brief Check if node can have SSA Symbol
  static bool Has_sym(air::base::NODE_PTR node) {
    return air::base::META_INFO::Op_category(node->Opcode()) ==
               air::base::OPR_CAT::LDST ||
           node->Is_do_loop() || node->Is_call() ||
           node->Opcode() == core::OPC_IDNAME;
  }

  //! @brief Check if node can have SSA Version
  static bool Has_ver(air::base::NODE_PTR node) { return Has_sym(node); }

  //! @brief Check if node can have MU_NODE
  static bool Has_mu(air::base::NODE_PTR node) {
    return node->Is_ld() || node->Is_call();
  }
  //! @brief Check if node can have CHI_NODE
  static bool Has_chi(air::base::NODE_PTR node) {
    return node->Is_st() || node->Is_call();
  }
  //! @brief Check if node can have PHI_NODE
  static bool Has_phi(air::base::NODE_PTR node) {
    return node->Is_if() || node->Is_do_loop();
  }

public:
  void Print(SSA_SYM_ID sym, std::ostream& os, uint32_t indent = 0) const;
  void Print(SSA_VER_ID ver, std::ostream& os, uint32_t indent = 0) const;
  void Print_ver(SSA_VER_ID ver, std::ostream& os, uint32_t indent = 0) const;

  void Print(SSA_SYM_ID sym) const;
  void Print(SSA_VER_ID ver) const;

  void Print(MU_NODE_ID mu, std::ostream& os, uint32_t indent = 0) const;
  void Print(CHI_NODE_ID chi, std::ostream& os, uint32_t indent = 0) const;
  void Print(PHI_NODE_ID phi, std::ostream& os, uint32_t indent = 0) const;

  void Print(MU_NODE_ID mu) const;
  void Print(CHI_NODE_ID chi) const;
  void Print(PHI_NODE_ID phi) const;

  void Print(air::base::NODE_ID node, std::ostream& os,
             uint32_t indent = 0) const;
  void Print_tree(air::base::NODE_ID node, std::ostream& os,
                  uint32_t indent = 0) const;

  void Print(air::base::NODE_ID node) const;
  void Print_tree(air::base::NODE_ID node) const;
  void Print(std::ostream& os) const {
    Print_tree(_cont->Entry_node()->Id(), os, 0);
  }

private:
  static constexpr uint32_t SYM_TAB_KIND = 0x10001;
  static constexpr uint32_t VER_TAB_KIND = 0x10002;
  static constexpr uint32_t MU_TAB_KIND  = 0x10003;
  static constexpr uint32_t CHI_TAB_KIND = 0x10004;
  static constexpr uint32_t PHI_TAB_KIND = 0x10005;

  typedef air::util::MEM_POOL<4096> MEM_POOL;

  typedef air::base::ARENA<sizeof(SSA_SYM_DATA), 4, false>  SSA_SYM_TAB;
  typedef air::base::ARENA<sizeof(SSA_VER_DATA), 4, false>  SSA_VER_TAB;
  typedef air::base::ARENA<sizeof(MU_NODE_DATA), 4, false>  SSA_MU_TAB;
  typedef air::base::ARENA<sizeof(CHI_NODE_DATA), 4, false> SSA_CHI_TAB;
  typedef air::base::ARENA<4, 4, false>                     SSA_PHI_TAB;

  typedef std::pair<uint32_t, uint32_t> U32_U32_PAIR;
  typedef air::util::CXX_MEM_ALLOCATOR<U32_U32_PAIR, MEM_POOL>
      U32_U32_ALLOCATOR;
  typedef std::unordered_map<uint32_t, uint32_t, std::hash<uint32_t>,
                             std::equal_to<uint32_t>, U32_U32_ALLOCATOR>
      U32_U32_MAP;

  air::base::CONTAINER* _cont;     //!< IR container
  MEM_POOL              _mpool;    //!< Mempool for all SSA tables
  SSA_SYM_TAB*          _sym_tab;  //!< SSA Symbol Table
  SSA_VER_TAB*          _ver_tab;  //!< SSA Version Table
  SSA_MU_TAB*           _mu_tab;   //!< SSA MU_NODE Table
  SSA_CHI_TAB*          _chi_tab;  //!< SSA CHI_NODE Table
  SSA_PHI_TAB*          _phi_tab;  //!< SSA PHI_NODE Table
  U32_U32_MAP*          _sym_map;  //!< NODE_ID -> SYM_ID MAP
  U32_U32_MAP*          _ver_map;  //!< NODE_ID -> VER_ID MAP
  U32_U32_MAP*          _mu_map;   //!< NODE_ID -> MU_NODE_ID MAP
  U32_U32_MAP*          _chi_map;  //!< NODE_ID -> CHI_NODE_ID MAP
  U32_U32_MAP*          _phi_map;  //!< NODE_ID -> PHI_NODE_ID MAP
  STATE                 _state;    //!< SSA state
};

}  // namespace opt

}  // namespace air

#endif  // AIR_OPT_SSA_CONTAINER_H
