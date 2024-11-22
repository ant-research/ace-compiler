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

  // find or create SSA symbol corresponding to given var and index in the map
  SSA_SYM_PTR Get_ssa_sym(U64_SYM_MAP& map, SSA_SYM_KIND kind, uint32_t var,
                          uint32_t index) {
    uint64_t              key = ((uint64_t)var << 32) | index;
    U64_SYM_MAP::iterator it  = map.find(key);
    if (it != map.end()) {
      SSA_SYM_PTR sym = _ssa_cont->Sym(it->second);
      if (!sym->Real_use()) {
        sym->Set_real_use();
      }
      return sym;
    }
    SSA_SYM_PTR sym = _ssa_cont->New_sym(kind, var, index);
    sym->Set_real_use();
    if (index != SSA_SYM::NO_INDEX) {
      // add to group if sym is a field or element
      // TODO: find parent field_id instead of NO_INDEX for nested struct?
      uint32_t              p_idx = SSA_SYM::NO_INDEX;
      uint64_t              p_key = ((uint64_t)var << 32) | p_idx;
      U64_SYM_MAP::iterator p_it  = map.find(p_key);
      SSA_SYM_PTR           p_sym = (p_it == map.end())
                                        ? _ssa_cont->New_sym(kind, var, p_idx)
                                        : _ssa_cont->Sym(p_it->second);
      sym->Set_parent(p_sym->Id());
      sym->Set_sibling(p_sym->Child_id());
      p_sym->Set_child(sym->Id());
    }
    map.insert(it, std::make_pair(key, sym->Id()));
    return sym;
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

  void Handle_variable(U64_SYM_MAP& map, SSA_SYM_KIND kind, uint32_t var,
                       uint32_t index, air::base::NODE_PTR node, bool def) {
    SSA_SYM_PTR sym = Get_ssa_sym(map, kind, var, index);
    if (def) {
      AIR_ASSERT(node->Is_root());
      Append_phi_def(node->Stmt(), sym->Id());
    }
    _ssa_cont->Set_node_sym(node->Id(), sym->Id());
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
  void Handle_variable(air::base::ADDR_DATUM_ID var, uint32_t index,
                       air::base::NODE_PTR node, bool def) {
    SSA_SYM_KIND kind = SSA_SYM_KIND::ADDR_DATUM;
    Handle_variable(*_datum_map, kind, var.Value(), index, node, def);
  }

  void Handle_variable(air::base::PREG_ID var, uint32_t index,
                       air::base::NODE_PTR node, bool def) {
    SSA_SYM_KIND kind = SSA_SYM_KIND::PREG;
    Handle_variable(*_preg_map, kind, var.Value(), index, node, def);
  }

  void Handle_virtual_variable(base::ADDR_DATUM_ID var, uint32_t index,
                               air::base::NODE_PTR node, bool def) {
    SSA_SYM_KIND kind = SSA_SYM_KIND::ADDR_DATUM;
    SSA_SYM_PTR  sym  = Get_ssa_sym(*_datum_map, kind, var.Value(), index);
    if (def) {
      AIR_ASSERT(node->Is_root());
      Append_phi_def(node->Stmt(), sym->Id());
      CHI_NODE_PTR chi = _ssa_cont->New_chi(sym->Id());
      _ssa_cont->Set_node_chi(node->Id(), chi->Id());
    } else {
      MU_NODE_PTR mu = _ssa_cont->New_mu(sym->Id());
      _ssa_cont->Set_node_mu(node->Id(), mu->Id());
    }
  }

  void Handle_do_loop_iv(air::base::NODE_PTR node) {
    SSA_SYM_PTR sym = Get_ssa_sym(*_datum_map, SSA_SYM_KIND::ADDR_DATUM,
                                  node->Iv_id().Value(), SSA_SYM::NO_INDEX);
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
//! Handle load/store/call/do_loop to generate SSA_SYM and update NODE
class SIMPLE_SYMTAB_HANDLER : public air::core::DEFAULT_HANDLER {
public:
  template <typename RETV, typename VISITOR>
  RETV Handle_idname(VISITOR* visitor, air::base::NODE_PTR node) {
    visitor->Context().Handle_variable(node->Addr_datum_id(), SSA_SYM::NO_INDEX,
                                       node, false);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ld(VISITOR* visitor, air::base::NODE_PTR node) {
    visitor->Context().Handle_variable(node->Addr_datum_id(), SSA_SYM::NO_INDEX,
                                       node, false);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ild(VISITOR* visitor, air::base::NODE_PTR node) {
    // 1. handle address node
    base::NODE_PTR addr_child = node->Child(0);
    AIR_ASSERT(addr_child->Opcode() == air::core::OPC_ARRAY);
    air::base::NODE_PTR base = addr_child->Child(0);
    if (base->Opcode() == air::core::OPC_LDCA) {
      return RETV();
    }
    AIR_ASSERT(base->Opcode() == air::core::OPC_LDA);
    visitor->template Visit<RETV>(addr_child);

    // 2. handle may used virtual variable
    base::ADDR_DATUM_ID array_addr_datum_id = base->Addr_datum_id();
    SIMPLE_BUILDER_CTX& ctx                 = visitor->Context();
    ctx.Handle_virtual_variable(array_addr_datum_id, SSA_SYM::NO_INDEX, node,
                                false);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ldf(VISITOR* visitor, air::base::NODE_PTR node) {
    AIR_ASSERT(false);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ldo(VISITOR* visitor, air::base::NODE_PTR node) {
    AIR_ASSERT(false);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ldp(VISITOR* visitor, air::base::NODE_PTR node) {
    visitor->Context().Handle_variable(node->Preg_id(), SSA_SYM::NO_INDEX, node,
                                       false);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ldpf(VISITOR* visitor, air::base::NODE_PTR node) {
    AIR_ASSERT(false);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_st(VISITOR* visitor, air::base::NODE_PTR node) {
    visitor->template Visit<RETV>(node->Child(0));
    visitor->Context().Handle_variable(node->Addr_datum_id(), SSA_SYM::NO_INDEX,
                                       node, true);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ist(VISITOR* visitor, air::base::NODE_PTR node) {
    // 1. handle rhs node
    base::NODE_PTR rhs_child = node->Child(1);
    visitor->template Visit<RETV>(rhs_child);

    // 2. handle address node
    base::NODE_PTR addr_child = node->Child(0);
    AIR_ASSERT(addr_child->Opcode() == air::core::OPC_ARRAY);
    air::base::NODE_PTR base = addr_child->Child(0);
    AIR_ASSERT(base->Opcode() == air::core::OPC_LDA);
    visitor->template Visit<RETV>(addr_child);

    // 3. handle may defined virtual variables
    // for ist(array(lda, idx), rhs_node), add chi of addr_datum of array.
    base::ADDR_DATUM_ID array_addr_datum_id = base->Addr_datum_id();
    SIMPLE_BUILDER_CTX& ctx                 = visitor->Context();
    ctx.Handle_virtual_variable(array_addr_datum_id, SSA_SYM::NO_INDEX, node,
                                true);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_stf(VISITOR* visitor, air::base::NODE_PTR node) {
    AIR_ASSERT(false);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_sto(VISITOR* visitor, air::base::NODE_PTR node) {
    AIR_ASSERT(false);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_stp(VISITOR* visitor, air::base::NODE_PTR node) {
    visitor->template Visit<RETV>(node->Child(0));
    visitor->Context().Handle_variable(node->Preg_id(), SSA_SYM::NO_INDEX, node,
                                       true);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_stpf(VISITOR* visitor, air::base::NODE_PTR node) {
    AIR_ASSERT(false);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_call(VISITOR* visitor, air::base::NODE_PTR node) {
    for (uint32_t i = 0; i < node->Num_child(); ++i) {
      visitor->template Visit<RETV>(node->Child(i));
    }
    air::base::PREG_ID preg = node->Ret_preg_id();
    if (!air::base::Is_null_id(preg)) {
      visitor->Context().Handle_variable(preg, SSA_SYM::NO_INDEX, node, true);
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
