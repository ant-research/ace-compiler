//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OPT_DFG_NODE_H
#define AIR_OPT_DFG_NODE_H

#include <list>

#include "air/base/container_decl.h"
#include "air/base/id_wrapper.h"
#include "air/base/ptr_wrapper.h"
#include "air/base/st_decl.h"
#include "air/opt/ssa_decl.h"
#include "air/util/debug.h"
#include "air/util/graph_base.h"
#include "dfg_data.h"

namespace air {
namespace opt {
class DFG_CONTAINER;
//! @brief DFG_EDGE contains the pointer of DFG_CONTAINER and EDGE_DATA_PTR.
//! Provides APIs to access the dst node and the next edge.
class DFG_EDGE {
public:
  friend class DFG_CONTAINER;
  DFG_EDGE(const DFG_CONTAINER* cntr, DFG_EDGE_DATA_PTR data)
      : _cntr(cntr), _data(data) {}
  DFG_EDGE(void) : _cntr(nullptr), _data() {}
  DFG_EDGE(const DFG_EDGE& other) : DFG_EDGE(other.Cntr(), other.Data()) {}

  ~DFG_EDGE() {}

  DFG_EDGE_ID  Id() const { return _data.Id(); }
  DFG_EDGE_PTR Next() const;
  DFG_EDGE_ID  Next_id() const { return _data->Next(); }
  DFG_NODE_PTR Dst() const;
  DFG_NODE_ID  Dst_id() const { return DFG_NODE_ID(_data->Dst()); }
  void         Set_next(DFG_EDGE_ID next) { _data->Set_next(next); }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  DFG_EDGE operator=(const DFG_EDGE&);

  const DFG_CONTAINER* Cntr() const { return _cntr; }
  DFG_EDGE_DATA_PTR    Data() const { return _data; }

  const DFG_CONTAINER* _cntr;
  DFG_EDGE_DATA_PTR    _data;
};

//! @brief iterator of DFG_EDGE provides deref, incr, and eq/ne operators.
class DFG_EDGE_ITER {
public:
  DFG_EDGE_ITER(const DFG_CONTAINER* cntr, DFG_EDGE_ID edge)
      : _cntr(cntr), _edge(edge) {}
  ~DFG_EDGE_ITER() {}

  DFG_EDGE_PTR  operator*() const;
  DFG_EDGE_ITER operator++();
  DFG_EDGE_ITER operator++(int);
  DFG_EDGE_PTR  operator->() const;
  bool          operator==(const DFG_EDGE_ITER& other) const;
  bool          operator!=(const DFG_EDGE_ITER& other) const;

private:
  // REQUIRED UNDEFINED UNWANTED methods
  DFG_EDGE_ITER(void);
  DFG_EDGE_ITER(const DFG_EDGE_ITER&);
  DFG_EDGE_ITER operator=(const DFG_EDGE_ITER&);

  const DFG_CONTAINER* Cntr() const { return _cntr; }
  DFG_EDGE_ID          Edge() const { return _edge; }

  const DFG_CONTAINER* _cntr;
  DFG_EDGE_ID          _edge;
};

//! @brief DFG_NODE contains the pointer of DFG_CONTAINER and NODE_DATA_PTR.
//! Offers APIs to access and update DFG_NODE_DATA, as well as to traverse
//! operand and successor nodes.
class DFG_NODE {
public:
  DFG_NODE(DFG_CONTAINER* cntr, DFG_NODE_DATA_PTR data)
      : _cntr(cntr), _data(data) {}
  DFG_NODE(void) : _cntr(nullptr), _data() {}
  DFG_NODE(const DFG_NODE& other) : _cntr(other.Cntr()), _data(other.Data()) {}
  DFG_NODE& operator=(const DFG_NODE& other) {
    if (&other != this) {
      _cntr = other.Cntr();
      _data = other.Data();
    }
    return (*this);
  }
  ~DFG_NODE() {}

  DFG_NODE_ID Id() const { return Data().Id(); }
  uint32_t    Opnd_cnt(void) const { return Data()->Opnd_cnt(); }
  void        Set_opnd(uint32_t id, DFG_NODE_PTR opnd);
  void        Set_opnd(uint32_t id, DFG_NODE_ID opnd);

  DFG_NODE_ID  Opnd_id(uint32_t id) const { return _data->Opnd(id); }
  DFG_NODE_PTR Opnd(uint32_t id) const;
  bool         Is_opnd(uint32_t id, air::base::NODE_PTR node) const;
  bool         Is_opnd(uint32_t id, air::opt::SSA_VER_PTR ver) const;
  bool         Is_opnd(uint32_t id, DFG_NODE_ID opnd) const;
  bool         Has_valid_pred(void) const {
    for (uint32_t id = 0; id < Opnd_cnt(); ++id) {
      if (Opnd_id(id) != base::Null_id) return true;
    }
    return false;
  }

  bool                  Is_ssa_ver(void) const { return Data()->Is_ssa_ver(); }
  bool                  Is_node(void) const { return Data()->Is_node(); }
  air::opt::SSA_VER_ID  Ssa_ver_id(void) const { return Data()->Ssa_ver(); }
  air::opt::SSA_VER_PTR Ssa_ver(void) const;
  air::base::NODE_ID    Node_id(void) const { return Data()->Node(); }
  air::base::NODE_PTR   Node(void) const;
  base::TYPE_ID         Type(void) const { return Data()->Type(); }

  DFG_EDGE_ITER Begin_succ(void) const {
    return DFG_EDGE_ITER(Cntr(), Data()->Succ());
  }
  DFG_EDGE_ITER End_succ(void) const {
    return DFG_EDGE_ITER(Cntr(), air::base::Null_id);
  }
  uint32_t     Succ_count(void) const;
  DFG_NODE_ID* Begin_opnd(void) const { return Data()->Begin_opnd(); }
  DFG_NODE_ID* End_opnd(void) const { return Data()->End_opnd(); }
  DFG_EDGE_ID  Succ_id() const { return Data()->Succ(); }
  DFG_EDGE_PTR Succ() const;
  void         Set_succ(DFG_EDGE_ID succ) { Data()->Set_succ(succ); }
  bool         Is_succ(DFG_NODE_ID node) const;

  bool All_opnd_at_state(TRAV_STATE state) const;
  bool Has_opnd_at_state(TRAV_STATE state) const;
  bool All_succ_at_state(TRAV_STATE state) const;
  bool Has_succ_at_state(TRAV_STATE state) const;

  uint32_t Freq(void) const { return Data()->Freq(); }
  void     Set_freq(uint32_t freq) const { Data()->Set_freq(freq); }

  void       Set_trav_state(TRAV_STATE state) { _data->Set_trav_state(state); }
  TRAV_STATE Trav_state(void) const { return _data->Trav_state(); }

  void              Print(std::ostream& os, uint32_t indent) const;
  void              Print() const;
  void              Print_dot(std::ostream& os, uint32_t indent) const;
  std::string       To_str() const;
  DFG_NODE_DATA_PTR Data() const { return _data; }
  DFG_CONTAINER*    Cntr() const { return _cntr; }

private:
  DFG_CONTAINER*    _cntr;
  DFG_NODE_DATA_PTR _data;
};

}  // namespace opt
}  // namespace air
#endif  // AIR_OPT_DFG_NODE_H