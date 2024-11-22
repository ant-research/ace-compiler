//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/opt/ssa_container.h"

#include "air/opt/ssa_node_list.h"

namespace air {

namespace opt {

SSA_CONTAINER::SSA_CONTAINER(air::base::CONTAINER* cont) : _cont(cont) {
  air::util::CXX_MEM_ALLOCATOR<SSA_SYM_TAB, MEM_POOL> stab_a(&_mpool);
  _sym_tab = stab_a.Allocate(&_mpool, SYM_TAB_KIND, "ssa_sym_tab", true);
  air::util::CXX_MEM_ALLOCATOR<SSA_VER_TAB, MEM_POOL> vtab_a(&_mpool);
  _ver_tab = vtab_a.Allocate(&_mpool, VER_TAB_KIND, "ssa_ver_tab", true);
  air::util::CXX_MEM_ALLOCATOR<SSA_MU_TAB, MEM_POOL> mtab_a(&_mpool);
  _mu_tab = mtab_a.Allocate(&_mpool, MU_TAB_KIND, "ssa_mu_tab", true);
  air::util::CXX_MEM_ALLOCATOR<SSA_CHI_TAB, MEM_POOL> ctab_a(&_mpool);
  _chi_tab = ctab_a.Allocate(&_mpool, CHI_TAB_KIND, "ssa_chi_tab", true);
  air::util::CXX_MEM_ALLOCATOR<SSA_PHI_TAB, MEM_POOL> ptab_a(&_mpool);
  _phi_tab = ptab_a.Allocate(&_mpool, PHI_TAB_KIND, "ssa_phi_tab", true);
  air::util::CXX_MEM_ALLOCATOR<U32_U32_MAP, MEM_POOL> u32m_a(&_mpool);
  _sym_map =
      u32m_a.Allocate(13, std::hash<uint32_t>(), std::equal_to<uint32_t>(),
                      U32_U32_ALLOCATOR(&_mpool));
  _ver_map =
      u32m_a.Allocate(13, std::hash<uint32_t>(), std::equal_to<uint32_t>(),
                      U32_U32_ALLOCATOR(&_mpool));
  _mu_map =
      u32m_a.Allocate(13, std::hash<uint32_t>(), std::equal_to<uint32_t>(),
                      U32_U32_ALLOCATOR(&_mpool));
  _chi_map =
      u32m_a.Allocate(13, std::hash<uint32_t>(), std::equal_to<uint32_t>(),
                      U32_U32_ALLOCATOR(&_mpool));
  _phi_map =
      u32m_a.Allocate(13, std::hash<uint32_t>(), std::equal_to<uint32_t>(),
                      U32_U32_ALLOCATOR(&_mpool));
  Set_state(NO_SSA);
}

SSA_CONTAINER::~SSA_CONTAINER() {
  // call destructor to avoid memory leak
  _sym_tab->~SSA_SYM_TAB();
  _ver_tab->~SSA_VER_TAB();
  _mu_tab->~SSA_MU_TAB();
  _chi_tab->~SSA_CHI_TAB();
  _phi_tab->~SSA_PHI_TAB();
}

SSA_SYM_PTR SSA_CONTAINER::New_sym(SSA_SYM_KIND kind, uint32_t var,
                                   uint32_t id) {
  SSA_SYM_DATA_PTR sym_data = _sym_tab->Allocate<SSA_SYM_DATA>();
  sym_data->_attr._kind     = (uint32_t)kind;
  sym_data->_type           = air::base::TYPE_ID();
  sym_data->_parent         = air::opt::SSA_SYM_ID();
  sym_data->_sibling        = air::opt::SSA_SYM_ID();
  sym_data->_child          = air::opt::SSA_SYM_ID();
  sym_data->_var_id         = var;
  sym_data->_sub_idx        = id;
  sym_data->_zero_ver = New_ver(VER_DEF_KIND::UNKNOWN, sym_data.Id())->Id();
  return SSA_SYM_PTR(SSA_SYM(this, sym_data));
}

SSA_VER_PTR SSA_CONTAINER::New_ver(VER_DEF_KIND kind, SSA_SYM_ID sym) {
  SSA_VER_DATA_PTR ver_data = _ver_tab->Allocate<SSA_VER_DATA>();
  ver_data->_attr._def_kind = (uint32_t)kind;
  ver_data->_sym            = sym;
  ver_data->_ver            = SSA_VER::NO_VER;
  ver_data->_def_stmt       = air::base::STMT_ID();
  return SSA_VER_PTR(SSA_VER(this, ver_data));
}

MU_NODE_PTR SSA_CONTAINER::New_mu(SSA_SYM_ID sym) {
  MU_NODE_DATA_PTR mu_data = _mu_tab->Allocate<MU_NODE_DATA>();
  mu_data->_attr._dead     = false;
  mu_data->_next           = air::opt::MU_NODE_ID();
  mu_data->_sym            = sym;
  mu_data->_opnd           = air::opt::SSA_VER_ID();
  return MU_NODE_PTR(MU_NODE(this, mu_data));
}

CHI_NODE_PTR SSA_CONTAINER::New_chi(SSA_SYM_ID sym) {
  CHI_NODE_DATA_PTR chi_data = _chi_tab->Allocate<CHI_NODE_DATA>();
  chi_data->_attr._dead      = false;
  chi_data->_next            = air::opt::CHI_NODE_ID();
  chi_data->_sym             = sym;
  chi_data->_res             = air::opt::SSA_VER_ID();
  chi_data->_opnd            = air::opt::SSA_VER_ID();
  return CHI_NODE_PTR(CHI_NODE(this, chi_data));
}

PHI_NODE_PTR SSA_CONTAINER::New_phi(SSA_SYM_ID sym, uint32_t size) {
  uint32_t mem_size = sizeof(PHI_NODE_DATA) / _phi_tab->Unit_size() + size;
  PHI_NODE_DATA_PTR phi_data =
      air::base::Static_cast<PHI_NODE_DATA_PTR>(_phi_tab->Malloc(mem_size));
  phi_data->_attr._dead = false;
  phi_data->_next       = air::opt::PHI_NODE_ID();
  phi_data->_size       = size;
  phi_data->_sym        = sym;
  phi_data->_res        = air::opt::SSA_VER_ID();
  for (uint32_t i = 0; i < size; ++i) {
    phi_data->_opnd[i] = air::opt::SSA_VER_ID();
  }
  return PHI_NODE_PTR(PHI_NODE(this, phi_data));
}

void SSA_CONTAINER::Print(SSA_SYM_ID sym, std::ostream& os,
                          uint32_t indent) const {
  Sym(sym)->Print(os, indent);
}

void SSA_CONTAINER::Print(SSA_VER_ID ver, std::ostream& os,
                          uint32_t indent) const {
  Ver(ver)->Print(os, indent);
}

void SSA_CONTAINER::Print_ver(SSA_VER_ID ver, std::ostream& os,
                              uint32_t indent) const {
  if (ver != air::base::Null_id) {
    Print(ver, os, indent);
  } else {
    os << "-nil-";
  }
}

void SSA_CONTAINER::Print(SSA_SYM_ID sym) const { Sym(sym)->Print(); }

void SSA_CONTAINER::Print(SSA_VER_ID ver) const { Ver(ver)->Print(); }

void SSA_CONTAINER::Print(MU_NODE_ID mu, std::ostream& os,
                          uint32_t indent) const {
  Mu_node(mu)->Print(os, indent);
}

void SSA_CONTAINER::Print(CHI_NODE_ID chi, std::ostream& os,
                          uint32_t indent) const {
  Chi_node(chi)->Print(os, indent);
}

void SSA_CONTAINER::Print(PHI_NODE_ID phi, std::ostream& os,
                          uint32_t indent) const {
  Phi_node(phi)->Print(os, indent);
}

void SSA_CONTAINER::Print(MU_NODE_ID mu) const { Mu_node(mu)->Print(); }

void SSA_CONTAINER::Print(CHI_NODE_ID chi) const { Chi_node(chi)->Print(); }

void SSA_CONTAINER::Print(PHI_NODE_ID phi) const { Phi_node(phi)->Print(); }

void SSA_CONTAINER::Print(air::base::NODE_ID node, std::ostream& os,
                          uint32_t indent) const {
  air::base::NODE_PTR ptr = Container()->Node(node);
  if (Has_mu(ptr)) {
    MU_LIST mu(this, Node_mu(node));
    mu.Print(os, indent);
  }
  ptr->Print(os, indent);
  U32_U32_MAP::const_iterator it = _ver_map->find(node.Value());
  if (_state == SSA && it != _ver_map->end()) {
    SSA_VER_PTR ver = Ver(SSA_VER_ID(it->second));
    const char* msg = (ver->Kind() == VER_DEF_KIND::STMT &&
                       ver->Def_stmt_id().Value() == node.Value())
                          ? " def("
                          : " use(";
    os << msg;
    Print(SSA_VER_ID(it->second), os);
    os << ")";
  } else {
    it = _sym_map->find(node.Value());
    if (it != _sym_map->end()) {
      os << " sym(";
      Print(SSA_SYM_ID(it->second), os);
      os << ")";
    }
  }
  os << std::endl;
  if (Has_chi(ptr)) {
    CHI_LIST chi(this, Node_chi(ptr->Id()));
    chi.Print(os, indent);
  } else if (Has_phi(ptr)) {
    PHI_LIST phi(this, Node_phi(ptr->Id()));
    phi.Print(os, indent);
  }
}

void SSA_CONTAINER::Print(air::base::NODE_ID node) const {
  Print(node, std::cout, 0);
  std::cout << std::endl;
}

void SSA_CONTAINER::Print_tree(air::base::NODE_ID node, std::ostream& os,
                               uint32_t indent) const {
  os << std::hex << std::showbase;
  air::base::NODE_PTR ptr = Container()->Node(node);
  // print self
  Print(node, os, indent);
  // print child
  if (ptr->Is_block()) {
    for (air::base::CONST_STMT_PTR stmt = ptr->Begin_stmt();
         stmt != ptr->End_stmt(); stmt  = stmt->Next()) {
      air::base::NODE_PTR ptr = stmt->Node();
      Print_tree(ptr->Id(), os, indent + 1);
    }
  } else {
    for (uint32_t i = 0; i < ptr->Num_child(); i++) {
      Print_tree(ptr->Child_id(i), os, indent + 1);
    }
  }
}

void SSA_CONTAINER::Print_tree(air::base::NODE_ID node) const {
  Print_tree(node, std::cout, 0);
  std::cout << std::endl;
}

}  // namespace opt

}  // namespace air
