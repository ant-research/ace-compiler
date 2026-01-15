//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OPT_SSA_SIMPLE_BUILDER_H
#define AIR_OPT_SSA_SIMPLE_BUILDER_H

#include <unordered_map>
#include <unordered_set>

#include "air/base/analyze_ctx.h"
#include "air/core/default_handler.h"
#include "air/opt/ssa_container.h"

namespace air {

namespace opt {

//! @brief Context for SIMPLE SSA BUILDER
//! No alias analysis, no irregular control flow.
class SIMPLE_BUILDER_CTX : public air::base::ANALYZE_CTX {
private:
  class SCF_SYM_INFO;

  typedef air::util::MEM_POOL<512>                             MEM_POOL;
  typedef air::util::CXX_MEM_ALLOCATOR<uint32_t, MEM_POOL>     U32_ALLOCATOR;
  typedef air::util::CXX_MEM_ALLOCATOR<SCF_SYM_INFO, MEM_POOL> SCF_ALLOCATOR;

  typedef std::unordered_set<uint32_t, std::hash<uint32_t>,
                             std::equal_to<uint32_t>, U32_ALLOCATOR>
      U32_SET;

  typedef std::pair<uint64_t, SSA_SYM_ID> U64_SYM_PAIR;
  typedef air::util::CXX_MEM_ALLOCATOR<U64_SYM_PAIR, MEM_POOL>
      U64_SYM_ALLOCATOR;
  typedef std::unordered_map<uint64_t, SSA_SYM_ID, std::hash<uint64_t>,
                             std::equal_to<uint64_t>, U64_SYM_ALLOCATOR>
      U64_SYM_MAP;

  typedef std::pair<uint32_t, SCF_SYM_INFO*> U32_SCF_PAIR;
  typedef air::util::CXX_MEM_ALLOCATOR<U32_SCF_PAIR, MEM_POOL>
      U32_SCF_ALLOCATOR;
  typedef std::unordered_map<uint32_t, SCF_SYM_INFO*, std::hash<uint32_t>,
                             std::equal_to<uint32_t>, U32_SCF_ALLOCATOR>
      U32_SCF_MAP;

  // Keep an record for all SSA symbols modified inside the SCF.
  // If an SSA symbol is modifed inside the SCF, a PHI_NODE for this symbol
  // is needed.
  class SCF_SYM_INFO {
  public:
    U32_SET _sym;

    SCF_SYM_INFO(MEM_POOL* mp)
        : _sym(13, std::hash<uint32_t>(), std::equal_to<uint32_t>(),
               U32_ALLOCATOR(mp)) {}

    void Add_ssa_sym(SSA_SYM_ID sym) { _sym.insert(sym.Value()); }

    typedef U32_SET::const_iterator ITERATOR;
    ITERATOR                        Begin() const { return _sym.begin(); }
    ITERATOR                        End() const { return _sym.end(); }
  };

  // find SSA symbol corresponding to given var and index in the map
  SSA_SYM_PTR Ssa_sym(const U64_SYM_MAP& map, uint32_t var,
                      uint32_t index) const {
    uint64_t                    key = ((uint64_t)var << 32) | index;
    U64_SYM_MAP::const_iterator it  = map.find(key);
    AIR_ASSERT(it != map.end());
    AIR_ASSERT(it->second != air::base::Null_id);
    return _ssa_cont->Sym(it->second);
  }

  // create SSA symbol corresponding to given var and index in the map
  void Enter_var(U64_SYM_MAP& map, SSA_SYM_KIND kind, uint32_t var,
                 uint32_t index, base::TYPE_ID type_id, bool real_occ) {
    uint64_t              key = ((uint64_t)var << 32) | index;
    U64_SYM_MAP::iterator it  = map.find(key);
    if (it != map.end()) {
      SSA_SYM_PTR sym = _ssa_cont->Sym(it->second);
      if (real_occ && !sym->Real_occ()) {
        sym->Set_real_occ();
      }
    } else {
      SSA_SYM_PTR sym = _ssa_cont->New_sym(kind, var, index, type_id);
      if (real_occ) {
        sym->Set_real_occ();
      }
      map.insert(it, std::make_pair(key, sym->Id()));
      if (index != SSA_SYM::NO_INDEX) {
        // add to group if sym is a field or element
        // TODO: find parent field_id instead of NO_INDEX for nested struct?
        SSA_SYM_PTR p_sym = Ssa_sym(map, var, SSA_SYM::NO_INDEX);
        AIR_ASSERT(p_sym != air::base::Null_ptr);
        sym->Set_parent(p_sym->Id());
        sym->Set_sibling(p_sym->Child_id());
        p_sym->Set_child(sym->Id());
      }
    }
  }

  // find or create SCF_SYM_INFO corresponding to given SCF stmt id
  SCF_SYM_INFO* Get_scf_info(uint32_t scf) {
    U32_SCF_MAP::iterator it = _scf_map->find(scf);
    if (it != _scf_map->end()) {
      return it->second;
    }
    SCF_ALLOCATOR allocator(&_mpool);
    SCF_SYM_INFO* info = allocator.Allocate(&_mpool);
    _scf_map->insert(it, std::make_pair(scf, info));
    return info;
  }

  void Append_phi_def(air::base::STMT_PTR stmt, SSA_SYM_ID sym) {
    while (stmt != air::base::Null_ptr) {
      if (SSA_CONTAINER::Has_phi(stmt->Node())) {
        SCF_SYM_INFO* info = Get_scf_info(stmt->Id().Value());
        info->Add_ssa_sym(sym);
      }
      stmt = stmt->Enclosing_stmt();
    }
  }

  CHI_NODE_ID Handle_child_def(SSA_SYM_PTR sym, air::base::NODE_PTR node,
                               CHI_NODE_ID pre_chi_id) {
    // handle child
    SSA_SYM_ID child = sym->Child_id();
    if (child != air::base::Null_id) {
      pre_chi_id = Handle_child_def(_ssa_cont->Sym(child), node, pre_chi_id);
      AIR_ASSERT(pre_chi_id != air::base::Null_id);
    }
    // handle sibling
    SSA_SYM_ID sibling = sym->Sibling_id();
    if (sibling != air::base::Null_id) {
      pre_chi_id = Handle_child_def(_ssa_cont->Sym(sibling), node, pre_chi_id);
      AIR_ASSERT(pre_chi_id != air::base::Null_id);
    }
    if (sym->Real_occ()) {
      // set sym updated by node for phi insertion
      Append_phi_def(node->Stmt(), sym->Id());
      // create chi for sym
      CHI_NODE_PTR chi = _ssa_cont->New_chi(sym->Id());
      chi->Set_next(pre_chi_id);
      pre_chi_id = chi->Id();
    }
    return pre_chi_id;
  }

  CHI_NODE_ID Handle_alias_def(SSA_SYM_PTR sym, air::base::NODE_PTR node,
                               CHI_NODE_ID pre_chi_id) {
    // handle child
    SSA_SYM_ID child = sym->Child_id();
    if (child != air::base::Null_id) {
      pre_chi_id = Handle_child_def(_ssa_cont->Sym(child), node, pre_chi_id);
      AIR_ASSERT(pre_chi_id != air::base::Null_id);
    }
    // handle parents
    SSA_SYM_ID parent = sym->Parent_id();
    while (parent != air::base::Null_id) {
      SSA_SYM_PTR par_ptr = _ssa_cont->Sym(parent);
      if (par_ptr->Real_occ()) {
        Append_phi_def(node->Stmt(), parent);
        CHI_NODE_PTR chi = _ssa_cont->New_chi(parent);
        chi->Set_next(pre_chi_id);
        pre_chi_id = chi->Id();
      }
      parent = par_ptr->Parent_id();
    }
    return pre_chi_id;
  }

  void Handle_var_def(U64_SYM_MAP& map, uint32_t var, uint32_t index,
                      air::base::NODE_PTR node, bool alias_def) {
    AIR_ASSERT(node->Is_root());
    SSA_SYM_PTR sym         = Ssa_sym(map, var, index);
    CHI_NODE_ID orig_chi_id = _ssa_cont->Node_chi(node->Id());
    CHI_NODE_ID chi_id      = orig_chi_id;
    if (alias_def) {
      AIR_ASSERT(node->Is_entry() ||
                 (node->Is_call() && node->Ret_preg_id().Value() != var));
      if (sym->Real_occ()) {
        // append to CHI list for alias def
        CHI_NODE_PTR chi = _ssa_cont->New_chi(sym->Id());
        chi->Set_next(chi_id);
        chi_id = chi->Id();
        Append_phi_def(node->Stmt(), sym->Id());
      }
    } else {
      AIR_ASSERT(node->Is_st() ||
                 (node->Is_call() && node->Ret_preg_id().Value() == var));
      // annotate on node for direct def
      _ssa_cont->Set_node_sym(node->Id(), sym->Id());
      Append_phi_def(node->Stmt(), sym->Id());
    }

    // append alias def for all fields/elements
    chi_id = Handle_alias_def(sym, node, chi_id);
    if (chi_id != orig_chi_id) {
      _ssa_cont->Set_node_chi(node->Id(), chi_id);
    }
  }

  MU_NODE_ID Handle_child_use(SSA_SYM_PTR sym, air::base::NODE_PTR node,
                              MU_NODE_ID pre_mu_id) {
    // handle child
    SSA_SYM_ID child = sym->Child_id();
    if (child != air::base::Null_id) {
      pre_mu_id = Handle_child_use(_ssa_cont->Sym(child), node, pre_mu_id);
      AIR_ASSERT(pre_mu_id != air::base::Null_id);
    }
    // handle sibling
    SSA_SYM_ID sibling = sym->Sibling_id();
    if (sibling != air::base::Null_id) {
      pre_mu_id = Handle_child_use(_ssa_cont->Sym(sibling), node, pre_mu_id);
      AIR_ASSERT(pre_mu_id != air::base::Null_id);
    }
    if (sym->Real_occ()) {
      // create mu for sym
      MU_NODE_PTR mu = _ssa_cont->New_mu(sym->Id());
      mu->Set_next(pre_mu_id);
      pre_mu_id = mu->Id();
    }
    return pre_mu_id;
  }

  void Handle_var_use(U64_SYM_MAP& map, uint32_t var, uint32_t index,
                      air::base::NODE_PTR node, bool alias_use) {
    SSA_SYM_PTR sym = Ssa_sym(map, var, index);
    if (alias_use) {
      // alias use, append to mu list
      MU_NODE_ID mu =
          Handle_child_use(sym, node, _ssa_cont->Node_mu(node->Id()));
      AIR_ASSERT(mu != air::base::Null_id);
      _ssa_cont->Set_node_mu(node->Id(), mu);
    } else {
      // directly use, annotate to node
      _ssa_cont->Set_node_sym(node->Id(), sym->Id());
    }
  }

public:
  SIMPLE_BUILDER_CTX(SSA_CONTAINER* cont) : _ssa_cont(cont) {
    air::util::CXX_MEM_ALLOCATOR<U64_SYM_MAP, MEM_POOL> u64_map_allocator(
        &_mpool);
    _datum_map = u64_map_allocator.Allocate(13, std::hash<uint64_t>(),
                                            std::equal_to<uint64_t>(),
                                            U64_SYM_ALLOCATOR(&_mpool));
    _preg_map  = u64_map_allocator.Allocate(13, std::hash<uint64_t>(),
                                            std::equal_to<uint64_t>(),
                                            U64_SYM_ALLOCATOR(&_mpool));
    air::util::CXX_MEM_ALLOCATOR<U32_SCF_MAP, MEM_POOL> u32_scf_allocator(
        &_mpool);
    _scf_map = u32_scf_allocator.Allocate(13, std::hash<uint32_t>(),
                                          std::equal_to<uint32_t>(),
                                          U64_SYM_ALLOCATOR(&_mpool));
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_unknown_domain(VISITOR* visitor, air::base::NODE_PTR node) {
    AIR_ASSERT(!node->Is_block());
    return Handle_node<RETV>(visitor, node);
  }

public:
  uint32_t Vsym_index(air::base::NODE_PTR node) {
    return SSA_CONTAINER::Vsym_index(node);
  }

  void Enter_var(air::base::ADDR_DATUM_ID var, uint32_t index,
                 air::base::TYPE_ID type_id, bool real_occ) {
    SSA_SYM_KIND kind = SSA_SYM_KIND::ADDR_DATUM;
    Enter_var(*_datum_map, kind, var.Value(), index, type_id, real_occ);
  }

  void Enter_var(air::base::PREG_ID var, uint32_t index,
                 air::base::TYPE_ID type_id, bool real_occ) {
    SSA_SYM_KIND kind = SSA_SYM_KIND::PREG;
    Enter_var(*_preg_map, kind, var.Value(), index, type_id, real_occ);
  }

  void Handle_var_def(air::base::ADDR_DATUM_ID var, uint32_t index,
                      air::base::NODE_PTR node, bool alias_def = false) {
    Handle_var_def(*_datum_map, var.Value(), index, node, alias_def);
  }

  void Handle_var_def(air::base::PREG_ID var, uint32_t index,
                      air::base::NODE_PTR node, bool alias_def = false) {
    Handle_var_def(*_preg_map, var.Value(), index, node, alias_def);
  }

  void Handle_var_use(air::base::ADDR_DATUM_ID var, uint32_t index,
                      air::base::NODE_PTR node, bool alias_use = false) {
    Handle_var_use(*_datum_map, var.Value(), index, node, alias_use);
  }

  void Handle_var_use(air::base::PREG_ID var, uint32_t index,
                      air::base::NODE_PTR node, bool alias_use = false) {
    Handle_var_use(*_preg_map, var.Value(), index, node, alias_use);
  }

  void Handle_do_loop_iv(air::base::NODE_PTR node) {
    SSA_SYM_PTR sym =
        Ssa_sym(*_datum_map, node->Iv_id().Value(), SSA_SYM::NO_INDEX);
    sym->Set_iv();
    // IV-init
    _ssa_cont->Set_node_sym(node->Child_id(0), sym->Id());
    // IV-update
    _ssa_cont->Set_node_sym(node->Child_id(2), sym->Id());
    // append PHI_NODE
    Append_phi_def(node->Stmt(), sym->Id());
  }

public:
  void Insert_phi() {
    for (auto it = _scf_map->begin(); it != _scf_map->end(); ++it) {
      air::base::NODE_PTR scf =
          _ssa_cont->Container()->Node(air::base::NODE_ID(it->first));
      AIR_ASSERT(SSA_CONTAINER::Has_phi(scf));
      SCF_SYM_INFO* info = it->second;
      AIR_ASSERT(info != nullptr);
      uint32_t    size = SSA_CONTAINER::Phi_size(scf);
      PHI_NODE_ID list;
      for (auto it = info->Begin(); it != info->End(); ++it) {
        PHI_NODE_PTR phi = _ssa_cont->New_phi(SSA_SYM_ID(*it), size);
        phi->Set_next(list);
        list = phi->Id();
      }
      _ssa_cont->Set_node_phi(scf->Id(), list);
    }
  }

private:
  SSA_CONTAINER* _ssa_cont;
  MEM_POOL       _mpool;
  // for ssa symbol creation
  U64_SYM_MAP* _datum_map;
  U64_SYM_MAP* _preg_map;
  // for phi insertion
  U32_SCF_MAP* _scf_map;
};

//! @brief SIMPLE_SYMTAB_HANDLER
//! Handle load/store/call/do_loop to generate SSA_SYM
class SIMPLE_SYMTAB_HANDLER : public air::core::DEFAULT_HANDLER {
public:
  template <typename RETV, typename VISITOR>
  RETV Handle_idname(VISITOR* visitor, air::base::NODE_PTR node) {
    visitor->Context().Enter_var(node->Addr_datum_id(), SSA_SYM::NO_INDEX,
                                 node->Rtype_id(), false);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ld(VISITOR* visitor, air::base::NODE_PTR node) {
    visitor->Context().Enter_var(node->Addr_datum_id(), SSA_SYM::NO_INDEX,
                                 node->Rtype_id(), true);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ild(VISITOR* visitor, air::base::NODE_PTR node) {
    // 1. handle address node
    base::NODE_PTR addr_child = node->Child(0);
    if (addr_child->Opcode() == air::core::OPC_ADD ||
        addr_child->Opcode() == air::core::OPC_SUB) {
      // ist(add(array(lda, idx), ofst), rhs_node)
      addr_child = addr_child->Child(0);
    }
    AIR_ASSERT(addr_child->Opcode() == air::core::OPC_ARRAY);
    air::base::NODE_PTR base = addr_child->Child(0);
    if (base->Opcode() == air::core::OPC_LDCA) {
      return RETV();
    }
    AIR_ASSERT(base->Opcode() == air::core::OPC_LDA);
    visitor->template Visit<RETV>(addr_child);

    // 2. handle may used virtual variable
    base::ADDR_DATUM_PTR array_addr_datum = base->Addr_datum();
    SIMPLE_BUILDER_CTX&  ctx              = visitor->Context();
    uint32_t             index            = ctx.Vsym_index(addr_child);
    if (index != SSA_SYM::NO_INDEX) {
      // create variable (no real-occ) for whole array if only read an element
      ctx.Enter_var(array_addr_datum->Id(), SSA_SYM::NO_INDEX,
                    array_addr_datum->Type_id(), false);
    }
    ctx.Enter_var(array_addr_datum->Id(), index, node->Rtype_id(), true);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ldf(VISITOR* visitor, air::base::NODE_PTR node) {
    SIMPLE_BUILDER_CTX&  ctx        = visitor->Context();
    base::ADDR_DATUM_PTR addr_datum = node->Addr_datum();
    // create variable for whole struct (no real-occ) and field (real-occ)
    ctx.Enter_var(addr_datum->Id(), SSA_SYM::NO_INDEX, addr_datum->Type_id(),
                  false);
    ctx.Enter_var(addr_datum->Id(), node->Field_id().Value(),
                  node->Access_type_id(), true);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ldo(VISITOR* visitor, air::base::NODE_PTR node) {
    AIR_ASSERT(false);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ldp(VISITOR* visitor, air::base::NODE_PTR node) {
    visitor->Context().Enter_var(node->Preg_id(), SSA_SYM::NO_INDEX,
                                 node->Rtype_id(), true);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ldpf(VISITOR* visitor, air::base::NODE_PTR node) {
    SIMPLE_BUILDER_CTX& ctx  = visitor->Context();
    base::PREG_PTR      preg = node->Preg();
    // create preg for whole struct (no real-occ) and field (real-occ)
    ctx.Enter_var(preg->Id(), SSA_SYM::NO_INDEX, preg->Type_id(), false);
    ctx.Enter_var(preg->Id(), node->Field_id().Value(), node->Access_type_id(),
                  true);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_st(VISITOR* visitor, air::base::NODE_PTR node) {
    visitor->template Visit<RETV>(node->Child(0));
    base::ADDR_DATUM_PTR addr_datum = node->Addr_datum();
    visitor->Context().Enter_var(addr_datum->Id(), SSA_SYM::NO_INDEX,
                                 addr_datum->Type_id(), true);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ist(VISITOR* visitor, air::base::NODE_PTR node) {
    // 1. handle rhs node
    base::NODE_PTR rhs_child = node->Child(1);
    visitor->template Visit<RETV>(rhs_child);

    // 2. handle address node
    base::NODE_PTR addr_child = node->Child(0);
    if (addr_child->Opcode() == air::core::OPC_ADD ||
        addr_child->Opcode() == air::core::OPC_SUB) {
      // ist(add(array(lda, idx), ofst), rhs_node)
      addr_child = addr_child->Child(0);
    }
    AIR_ASSERT(addr_child->Opcode() == air::core::OPC_ARRAY);
    air::base::NODE_PTR base = addr_child->Child(0);
    AIR_ASSERT(base->Opcode() == air::core::OPC_LDA);
    visitor->template Visit<RETV>(addr_child);

    // 3. handle may defined virtual variables
    // for ist(array(lda, idx), rhs_node), add chi of addr_datum of array.
    base::ADDR_DATUM_PTR array_addr_datum = base->Addr_datum();
    SIMPLE_BUILDER_CTX&  ctx              = visitor->Context();
    uint32_t             index            = ctx.Vsym_index(addr_child);

    bool invalid_idx = (index == SSA_SYM::NO_INDEX);
    ctx.Enter_var(array_addr_datum->Id(), SSA_SYM::NO_INDEX,
                  array_addr_datum->Type_id(), invalid_idx);

    if (!invalid_idx) {
      // create a real-occ variable for an array element when writing to an
      // element with a constant index.
      ctx.Enter_var(array_addr_datum->Id(), index, node->Access_type_id(),
                    true);
    }
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_stf(VISITOR* visitor, air::base::NODE_PTR node) {
    visitor->template Visit<RETV>(node->Child(0));
    SIMPLE_BUILDER_CTX&  ctx        = visitor->Context();
    base::ADDR_DATUM_PTR addr_datum = node->Addr_datum();
    // create variable for whole struct (no real-occ) and field (real-occ)
    ctx.Enter_var(addr_datum->Id(), SSA_SYM::NO_INDEX, addr_datum->Type_id(),
                  false);
    ctx.Enter_var(addr_datum->Id(), node->Field_id().Index(),
                  node->Access_type_id(), true);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_sto(VISITOR* visitor, air::base::NODE_PTR node) {
    AIR_ASSERT(false);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_stp(VISITOR* visitor, air::base::NODE_PTR node) {
    visitor->template Visit<RETV>(node->Child(0));
    visitor->Context().Enter_var(node->Preg_id(), SSA_SYM::NO_INDEX,
                                 node->Access_type_id(), true);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_stpf(VISITOR* visitor, air::base::NODE_PTR node) {
    visitor->template Visit<RETV>(node->Child(0));
    SIMPLE_BUILDER_CTX& ctx  = visitor->Context();
    base::PREG_PTR      preg = node->Preg();
    // create preg for whole struct (no real-occ) and field (real-occ)
    ctx.Enter_var(preg->Id(), SSA_SYM::NO_INDEX, preg->Type_id(), false);
    ctx.Enter_var(preg->Id(), node->Field_id().Index(), node->Access_type_id(),
                  true);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_call(VISITOR* visitor, air::base::NODE_PTR node) {
    for (uint32_t i = 0; i < node->Num_child(); ++i) {
      visitor->template Visit<RETV>(node->Child(i));
    }
    air::base::PREG_PTR preg = node->Ret_preg();
    if (!air::base::Is_null_id(preg->Id())) {
      visitor->Context().Enter_var(preg->Id(), SSA_SYM::NO_INDEX,
                                   preg->Type_id(), true);
    }
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_do_loop(VISITOR* visitor, air::base::NODE_PTR node) {
    // enter IV variable
    visitor->Context().Enter_var(node->Iv_id(), SSA_SYM::NO_INDEX,
                                 node->Iv()->Type_id(), true);
    // normal handling of do_loop children
    for (uint32_t i = 0; i < node->Num_child(); ++i) {
      visitor->template Visit<RETV>(node->Child(i));
    }
  }
};

//! @brief SIMPLE_MU_CHI_HANDLER
//! Handle load/store/call to create MU and CHI list
class SIMPLE_MU_CHI_HANDLER : public air::core::DEFAULT_HANDLER {
public:
  template <typename RETV, typename VISITOR>
  RETV Handle_func_entry(VISITOR* visitor, air::base::NODE_PTR node) {
    SIMPLE_BUILDER_CTX& ctx = visitor->Context();
    int                 i;
    for (i = 0; i < node->Num_child() - 1; ++i) {
      air::base::NODE_PTR param = node->Child(i);
      AIR_ASSERT(param->Opcode() == air::core::OPC_IDNAME);
      visitor->template Visit<RETV>(param);
      // create chi list for param on func_entry
      ctx.Handle_var_def(param->Addr_datum_id(), SSA_SYM::NO_INDEX, node, true);
    }
    // visit function body
    visitor->template Visit<RETV>(node->Child(i));
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_idname(VISITOR* visitor, air::base::NODE_PTR node) {
    visitor->Context().Handle_var_use(node->Addr_datum_id(), SSA_SYM::NO_INDEX,
                                      node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ld(VISITOR* visitor, air::base::NODE_PTR node) {
    visitor->Context().Handle_var_use(node->Addr_datum_id(), SSA_SYM::NO_INDEX,
                                      node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ild(VISITOR* visitor, air::base::NODE_PTR node) {
    // 1. handle address node
    base::NODE_PTR addr_child = node->Child(0);
    if (addr_child->Opcode() == air::core::OPC_ADD ||
        addr_child->Opcode() == air::core::OPC_SUB) {
      // ist(add(array(lda, idx), ofst), rhs_node)
      addr_child = addr_child->Child(0);
    }
    AIR_ASSERT(addr_child->Opcode() == air::core::OPC_ARRAY);
    air::base::NODE_PTR base = addr_child->Child(0);
    if (base->Opcode() == air::core::OPC_LDCA) {
      return RETV();
    }
    AIR_ASSERT(base->Opcode() == air::core::OPC_LDA);
    visitor->template Visit<RETV>(addr_child);

    // 2. handle direct access virtual variable. ignore aliased use
    base::ADDR_DATUM_PTR array_addr_datum = base->Addr_datum();
    SIMPLE_BUILDER_CTX&  ctx              = visitor->Context();
    uint32_t             index            = ctx.Vsym_index(addr_child);
    ctx.Handle_var_use(array_addr_datum->Id(), index, node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ldf(VISITOR* visitor, air::base::NODE_PTR node) {
    visitor->Context().Handle_var_use(node->Addr_datum_id(),
                                      node->Field_id().Value(), node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ldo(VISITOR* visitor, air::base::NODE_PTR node) {
    AIR_ASSERT(false);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ldp(VISITOR* visitor, air::base::NODE_PTR node) {
    visitor->Context().Handle_var_use(node->Preg_id(), SSA_SYM::NO_INDEX, node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ldpf(VISITOR* visitor, air::base::NODE_PTR node) {
    visitor->Context().Handle_var_use(node->Preg_id(), node->Field_id().Value(),
                                      node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_retv(VISITOR* visitor, air::base::NODE_PTR node) {
    air::base::NODE_PTR retv = node->Child(0);
    visitor->template Visit<RETV>(retv);

    if (retv->Opcode() == air::core::OPC_LD) {
      visitor->Context().Handle_var_use(retv->Addr_datum_id(),
                                        SSA_SYM::NO_INDEX, node, true);
    } else {
      visitor->Context().Handle_var_use(retv->Preg_id(), SSA_SYM::NO_INDEX,
                                        node, true);
    }
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_st(VISITOR* visitor, air::base::NODE_PTR node) {
    visitor->template Visit<RETV>(node->Child(0));
    base::ADDR_DATUM_PTR addr_datum = node->Addr_datum();
    visitor->Context().Handle_var_def(addr_datum->Id(), SSA_SYM::NO_INDEX,
                                      node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ist(VISITOR* visitor, air::base::NODE_PTR node) {
    // 1. handle rhs node
    base::NODE_PTR rhs_child = node->Child(1);
    visitor->template Visit<RETV>(rhs_child);

    // 2. handle address node
    base::NODE_PTR addr_child = node->Child(0);
    if (addr_child->Opcode() == air::core::OPC_ADD ||
        addr_child->Opcode() == air::core::OPC_SUB) {
      // ist(add(array(lda, idx), ofst), rhs_node)
      addr_child = addr_child->Child(0);
    }
    AIR_ASSERT(addr_child->Opcode() == air::core::OPC_ARRAY);
    air::base::NODE_PTR base = addr_child->Child(0);
    AIR_ASSERT(base->Opcode() == air::core::OPC_LDA);
    visitor->template Visit<RETV>(addr_child);

    // 3. handle may defined virtual variables
    // for ist(array(lda, idx), rhs_node), add chi of addr_datum of array.
    base::ADDR_DATUM_PTR array_addr_datum = base->Addr_datum();
    SIMPLE_BUILDER_CTX&  ctx              = visitor->Context();
    uint32_t             index            = ctx.Vsym_index(addr_child);
    ctx.Handle_var_def(array_addr_datum->Id(), index, node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_stf(VISITOR* visitor, air::base::NODE_PTR node) {
    visitor->template Visit<RETV>(node->Child(0));
    base::ADDR_DATUM_PTR addr_datum = node->Addr_datum();
    visitor->Context().Handle_var_def(addr_datum->Id(),
                                      node->Field_id().Index(), node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_sto(VISITOR* visitor, air::base::NODE_PTR node) {
    AIR_ASSERT(false);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_stp(VISITOR* visitor, air::base::NODE_PTR node) {
    visitor->template Visit<RETV>(node->Child(0));
    base::PREG_PTR preg = node->Preg();
    visitor->Context().Handle_var_def(preg->Id(), SSA_SYM::NO_INDEX, node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_stpf(VISITOR* visitor, air::base::NODE_PTR node) {
    visitor->template Visit<RETV>(node->Child(0));
    base::PREG_PTR preg = node->Preg();
    visitor->Context().Handle_var_def(preg->Id(), node->Field_id().Index(),
                                      node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_call(VISITOR* visitor, air::base::NODE_PTR node) {
    for (uint32_t i = 0; i < node->Num_child(); ++i) {
      visitor->template Visit<RETV>(node->Child(i));
    }
    air::base::PREG_PTR preg = node->Ret_preg();
    if (!air::base::Is_null_id(preg->Id())) {
      visitor->Context().Handle_var_def(preg->Id(), SSA_SYM::NO_INDEX, node);
    }
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_do_loop(VISITOR* visitor, air::base::NODE_PTR node) {
    // normal handling of do_loop children
    for (uint32_t i = 0; i < node->Num_child(); ++i) {
      visitor->template Visit<RETV>(node->Child(i));
    }
    // special handling of do_loop IV
    visitor->Context().Handle_do_loop_iv(node);
  }
};

}  // namespace opt

}  // namespace air

#endif  // AIR_OPT_SSA_SIMPLE_BUILDER_H
