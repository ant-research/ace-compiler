//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/util/cfg_base.h"
#include "air/util/cfg_data.h"
#include "gtest/gtest.h"

namespace {

struct NODE_DATA {
  ENABLE_CFG_DATA_INFO()
  int32_t _id;
  float   _weight;

  void Set_weight(float w) { _weight = w; }
  void Set_info(int32_t id, float w) {
    _id     = id;
    _weight = w;
  }

  NODE_DATA() : _id(0), _weight(0) {}
};

struct EDGE_DATA {
  ENABLE_CFG_EDGE_INFO()
  int32_t _id;
  float   _weight;

  void Set_weight(float w) { _weight = w; }
  void Set_info(int32_t id, float w) {
    _id     = id;
    _weight = w;
  }

  EDGE_DATA() : _id(0), _weight(0) {}
};

struct UT_CONTAINER {
  using MEM_POOL      = air::util::MEM_POOL<4096>;
  using CFG           = air::util::CFG<NODE_DATA, EDGE_DATA, UT_CONTAINER>;
  using NODE_ID       = CFG::NODE_ID;
  using EDGE_ID       = CFG::EDGE_ID;
  using NODE_DATA_PTR = CFG::NODE_DATA_PTR;
  using EDGE_DATA_PTR = CFG::EDGE_DATA_PTR;
  using NODE_TAB      = air::base::ARENA<sizeof(NODE_DATA), 4, false>;
  using EDGE_TAB      = air::base::ARENA<sizeof(EDGE_DATA), 4, false>;

  UT_CONTAINER() : _cfg(this) {
    air::util::CXX_MEM_ALLOCATOR<NODE_TAB, MEM_POOL> node_a(&_mpool);
    _node_tab = node_a.Allocate(&_mpool, 1, "node_tab", true);
    air::util::CXX_MEM_ALLOCATOR<EDGE_TAB, MEM_POOL> edge_a(&_mpool);
    _edge_tab = edge_a.Allocate(&_mpool, 1, "edge_tab", true);
    _start    = _cfg.New_node()->Id();
    _exit     = _cfg.New_node()->Id();
  }

  MEM_POOL&       Mem_pool() { return _mpool; }
  CFG&            Cfg() { return _cfg; }
  uint32_t        Node_count() const { return _node_tab->Size(); }
  uint32_t        Edge_count() const { return _edge_tab->Size(); }
  const NODE_TAB* Node_tab() const { return _node_tab; }
  const EDGE_TAB* Edge_tab() const { return _edge_tab; }

  NODE_DATA_PTR New_node_data() {
    return _node_tab->template Allocate<NODE_DATA>();
  }
  EDGE_DATA_PTR New_edge_data() {
    return _edge_tab->template Allocate<EDGE_DATA>();
  }

  NODE_DATA_PTR Find(NODE_ID id) {
    return id != air::base::Null_id ? _node_tab->Find(id) : NODE_DATA_PTR();
  }
  EDGE_DATA_PTR Find(EDGE_ID id) {
    return id != air::base::Null_id ? _edge_tab->Find(id) : EDGE_DATA_PTR();
  }

  MEM_POOL  _mpool;
  CFG       _cfg;
  NODE_TAB* _node_tab;
  EDGE_TAB* _edge_tab;
  NODE_ID   _start;
  NODE_ID   _exit;
};

using CFG = UT_CONTAINER::CFG;

TEST(cfg, CFG_BASE) {
  // initialize CFG
  //     n0
  //  e0 |
  //     n1 <----+
  // e1 /  \ e2  |
  //  n2    n3   | e6
  // e3 \  / e4  |
  //     n4 -----+
  //  e5 |
  //     n5
  UT_CONTAINER cont;
  CFG&         cfg = cont.Cfg();
  EXPECT_EQ(cfg.Node_count(), 2);  // fake entry & exit

  CFG::NODE_PTR n0 = cfg.New_node();
  n0->Data()->Set_info(0, 0);
  CFG::NODE_PTR n1 = cfg.New_node();
  n1->Data()->Set_info(1, 1);
  CFG::NODE_PTR n2 = cfg.New_node();
  n2->Data()->Set_info(2, 2);
  CFG::NODE_PTR n3 = cfg.New_node();
  n3->Data()->Set_info(3, 3);
  CFG::NODE_PTR n4 = cfg.New_node();
  n4->Data()->Set_info(4, 4);
  CFG::NODE_PTR n5 = cfg.New_node();
  n5->Data()->Set_info(5, 5);

  CFG::EDGE_PTR e0 = cfg.Connect(n0, n1);
  e0->Data()->Set_info(1, 2);
  CFG::EDGE_PTR e1 = cfg.Connect(n1, n2);
  e1->Data()->Set_info(1, 3);
  CFG::EDGE_PTR e2 = cfg.Connect(n1, n3);
  e2->Data()->Set_info(1, 4);
  CFG::EDGE_PTR e3 = cfg.Connect(n2, n4);
  e3->Data()->Set_info(1, 5);
  CFG::EDGE_PTR e4 = cfg.Connect(n3, n4);
  e4->Data()->Set_info(1, 6);
  CFG::EDGE_PTR e5 = cfg.Connect(n4, n5);
  e5->Data()->Set_info(1, 7);
  CFG::EDGE_PTR e6 = cfg.Connect(n4, n1);
  e6->Data()->Set_info(1, 8);

  // validate nodes
  EXPECT_EQ(cfg.Node_count(), 8);
  EXPECT_EQ(n0->Id().Value(), 2);
  EXPECT_EQ(n0->Data()->_id, 0);
  EXPECT_EQ(n0->Data()->_weight, 0);
  EXPECT_EQ(n1->Id().Value(), 3);
  EXPECT_EQ(n1->Data()->_id, 1);
  EXPECT_EQ(n1->Data()->_weight, 1);
  EXPECT_EQ(n2->Id().Value(), 4);
  EXPECT_EQ(n2->Data()->_id, 2);
  EXPECT_EQ(n2->Data()->_weight, 2);
  EXPECT_EQ(n3->Id().Value(), 5);
  EXPECT_EQ(n3->Data()->_id, 3);
  EXPECT_EQ(n3->Data()->_weight, 3);
  EXPECT_EQ(n4->Id().Value(), 6);
  EXPECT_EQ(n4->Data()->_id, 4);
  EXPECT_EQ(n4->Data()->_weight, 4);
  EXPECT_EQ(n5->Id().Value(), 7);
  EXPECT_EQ(n5->Data()->_id, 5);
  EXPECT_EQ(n5->Data()->_weight, 5);

  // validate edges
  EXPECT_EQ(cfg.Edge_count(), 7);
  EXPECT_EQ(e0->Id().Value(), 0);
  EXPECT_EQ(e0->Data()->_id, 1);
  EXPECT_EQ(e0->Data()->_weight, 2);
  EXPECT_EQ(e1->Id().Value(), 1);
  EXPECT_EQ(e1->Data()->_id, 1);
  EXPECT_EQ(e1->Data()->_weight, 3);
  EXPECT_EQ(e2->Id().Value(), 2);
  EXPECT_EQ(e2->Data()->_id, 1);
  EXPECT_EQ(e2->Data()->_weight, 4);
  EXPECT_EQ(e3->Id().Value(), 3);
  EXPECT_EQ(e3->Data()->_id, 1);
  EXPECT_EQ(e3->Data()->_weight, 5);
  EXPECT_EQ(e4->Id().Value(), 4);
  EXPECT_EQ(e4->Data()->_id, 1);
  EXPECT_EQ(e4->Data()->_weight, 6);
  EXPECT_EQ(e5->Id().Value(), 5);
  EXPECT_EQ(e5->Data()->_id, 1);
  EXPECT_EQ(e5->Data()->_weight, 7);
  EXPECT_EQ(e6->Id().Value(), 6);
  EXPECT_EQ(e6->Data()->_id, 1);
  EXPECT_EQ(e6->Data()->_weight, 8);

  // validate edge pred & dst
  EXPECT_EQ(e0->Pred_id(), n0->Id());
  EXPECT_EQ(e0->Succ_id(), n1->Id());
  EXPECT_EQ(e1->Pred_id(), n1->Id());
  EXPECT_EQ(e1->Succ_id(), n2->Id());
  EXPECT_EQ(e2->Pred_id(), n1->Id());
  EXPECT_EQ(e2->Succ_id(), n3->Id());
  EXPECT_EQ(e3->Pred_id(), n2->Id());
  EXPECT_EQ(e3->Succ_id(), n4->Id());
  EXPECT_EQ(e4->Pred_id(), n3->Id());
  EXPECT_EQ(e4->Succ_id(), n4->Id());
  EXPECT_EQ(e5->Pred_id(), n4->Id());
  EXPECT_EQ(e5->Succ_id(), n5->Id());
  EXPECT_EQ(e6->Pred_id(), n4->Id());
  EXPECT_EQ(e6->Succ_id(), n1->Id());

  // validate find, get and pos
  EXPECT_EQ(n0->Count<CFG::PRED>(), 0);
  EXPECT_EQ(n0->Count<CFG::SUCC>(), 1);
  EXPECT_EQ(n0->Find<CFG::PRED>(n1->Id()), air::base::Null_ptr);
  EXPECT_EQ(n0->Find<CFG::SUCC>(n1->Id()), e0);
  EXPECT_EQ(n0->Pos<CFG::PRED>(n1->Id()), CFG::INVALID_POS);
  EXPECT_EQ(n0->Pos<CFG::SUCC>(n1->Id()), 0);
  EXPECT_EQ(n0->Get<CFG::SUCC>(0), e0);
  EXPECT_EQ(n0->Get<CFG::SUCC>(1), air::base::Null_ptr);

  EXPECT_EQ(n1->Count<CFG::PRED>(), 2);
  EXPECT_EQ(n1->Count<CFG::SUCC>(), 2);
  EXPECT_EQ(n1->Find<CFG::PRED>(n0->Id()), e0);
  EXPECT_EQ(n1->Find<CFG::PRED>(n4->Id()), e6);
  EXPECT_EQ(n1->Pos<CFG::PRED>(n0->Id()), 1);
  EXPECT_EQ(n1->Pos<CFG::PRED>(n4->Id()), 0);
  EXPECT_EQ(n1->Find<CFG::SUCC>(n2->Id()), e1);
  EXPECT_EQ(n1->Find<CFG::SUCC>(n3->Id()), e2);
  EXPECT_EQ(n1->Pos<CFG::SUCC>(n2->Id()), 1);
  EXPECT_EQ(n1->Pos<CFG::SUCC>(n3->Id()), 0);
  EXPECT_EQ(n1->Get<CFG::PRED>(0), e6);
  EXPECT_EQ(n1->Get<CFG::PRED>(1), e0);
  EXPECT_EQ(n1->Get<CFG::SUCC>(0), e2);
  EXPECT_EQ(n1->Get<CFG::SUCC>(1), e1);

  EXPECT_EQ(n2->Count<CFG::PRED>(), 1);
  EXPECT_EQ(n2->Count<CFG::SUCC>(), 1);
  EXPECT_EQ(n2->Find<CFG::PRED>(n1->Id()), e1);
  EXPECT_EQ(n2->Find<CFG::SUCC>(n4->Id()), e3);
  EXPECT_EQ(n2->Get<CFG::PRED>(0), e1);
  EXPECT_EQ(n2->Get<CFG::SUCC>(0), e3);

  EXPECT_EQ(n3->Count<CFG::PRED>(), 1);
  EXPECT_EQ(n3->Count<CFG::SUCC>(), 1);
  EXPECT_EQ(n3->Find<CFG::PRED>(n1->Id()), e2);
  EXPECT_EQ(n3->Find<CFG::SUCC>(n4->Id()), e4);
  EXPECT_EQ(n3->Get<CFG::PRED>(0), e2);
  EXPECT_EQ(n3->Get<CFG::SUCC>(0), e4);

  EXPECT_EQ(n4->Count<CFG::PRED>(), 2);
  EXPECT_EQ(n4->Count<CFG::SUCC>(), 2);
  EXPECT_EQ(n4->Find<CFG::PRED>(n2->Id()), e3);
  EXPECT_EQ(n4->Find<CFG::PRED>(n3->Id()), e4);
  EXPECT_EQ(n4->Find<CFG::SUCC>(n5->Id()), e5);
  EXPECT_EQ(n4->Find<CFG::SUCC>(n1->Id()), e6);
  EXPECT_EQ(n4->Get<CFG::PRED>(0), e4);
  EXPECT_EQ(n4->Get<CFG::PRED>(1), e3);
  EXPECT_EQ(n4->Get<CFG::SUCC>(0), e6);
  EXPECT_EQ(n4->Get<CFG::SUCC>(1), e5);

  EXPECT_EQ(n5->Count<CFG::PRED>(), 1);
  EXPECT_EQ(n5->Count<CFG::SUCC>(), 0);
  EXPECT_EQ(n5->Find<CFG::PRED>(n4->Id()), e5);
  EXPECT_EQ(n5->Find<CFG::SUCC>(n0->Id()), air::base::Null_ptr);
  EXPECT_EQ(n5->Get<CFG::PRED>(0), e5);
  EXPECT_EQ(n5->Get<CFG::SUCC>(0), air::base::Null_ptr);

  // update field
  n0->Data()->Set_weight(2);
  e0->Data()->Set_weight(3);
  EXPECT_EQ(n0->Data()->_weight, 2);
  EXPECT_EQ(e0->Data()->_weight, 3);
  cfg.Print();
}

}  // namespace
