//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <vector>

#include "air/util/cfg_base.h"
#include "air/util/cfg_data.h"
#include "air/util/dom_builder.h"
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
  using NODE          = CFG::NODE;
  using NODE_PTR      = air::base::PTR<NODE>;

  UT_CONTAINER() : _cfg(this) {
    air::util::CXX_MEM_ALLOCATOR<NODE_TAB, MEM_POOL> node_a(&_mpool);
    _node_tab = node_a.Allocate(&_mpool, 1, "node_tab", true);
    air::util::CXX_MEM_ALLOCATOR<EDGE_TAB, MEM_POOL> edge_a(&_mpool);
    _edge_tab = edge_a.Allocate(&_mpool, 1, "edge_tab", true);
    _start    = _cfg.New_node()->Id();
    _exit     = _cfg.New_node()->Id();
  }

  MEM_POOL& Mem_pool() { return _mpool; }
  CFG&      Cfg() { return _cfg; }
  uint32_t  Node_count() const { return _node_tab->Size(); }
  uint32_t  Edge_count() const { return _edge_tab->Size(); }
  NODE_PTR  Node(NODE_ID id) const { return NODE_PTR(NODE(this, Find(id))); }
  const NODE_TAB* Node_tab() const { return _node_tab; }
  const EDGE_TAB* Edge_tab() const { return _edge_tab; }

  NODE_DATA_PTR New_node_data() {
    return _node_tab->template Allocate<NODE_DATA>();
  }
  EDGE_DATA_PTR New_edge_data() {
    return _edge_tab->template Allocate<EDGE_DATA>();
  }

  NODE_DATA_PTR Find(NODE_ID id) const {
    return id != air::base::Null_id ? _node_tab->Find(id) : NODE_DATA_PTR();
  }
  EDGE_DATA_PTR Find(EDGE_ID id) const {
    return id != air::base::Null_id ? _edge_tab->Find(id) : EDGE_DATA_PTR();
  }

  NODE_PTR Entry() const { return Node(_start); }
  NODE_PTR Exit() const { return Node(_exit); }

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

TEST(cfg, DOM_INFO) {
  using CSET = air::util::DOM_INFO::ID_CSET;
  using CVEC = air::util::DOM_INFO::ID_VEC;
  // initialize CFG
  //     n0 (common entry)
  //  e0 |
  //     n2 -----+
  //  e1 |       |
  //     n3      |
  // e2 /  \ e3  |
  //  n4    n5   | e7
  // e4 \  / e5  |
  //     n6      |
  //  e6 |       |
  //     n7 <----+
  //  e8 |
  //     n1 (common exit)
  UT_CONTAINER cont;
  CFG&         cfg = cont.Cfg();
  EXPECT_EQ(cfg.Node_count(), 2);  // fake entry & exit
  air::util::DOM_INFO                  dom_info;
  air::util::DOM_BUILDER<UT_CONTAINER> dom_builder(&cont, &dom_info);

  CFG::NODE_PTR n0 = cont.Entry();
  n0->Data()->Set_info(0, 0);
  CFG::NODE_PTR n2 = cfg.New_node();
  n2->Data()->Set_info(2, 2);
  CFG::NODE_PTR n3 = cfg.New_node();
  n3->Data()->Set_info(3, 3);
  CFG::NODE_PTR n4 = cfg.New_node();
  n4->Data()->Set_info(4, 4);
  CFG::NODE_PTR n5 = cfg.New_node();
  n5->Data()->Set_info(5, 5);
  CFG::NODE_PTR n6 = cfg.New_node();
  n6->Data()->Set_info(6, 6);
  CFG::NODE_PTR n7 = cfg.New_node();
  n7->Data()->Set_info(7, 7);
  CFG::NODE_PTR n1 = cont.Exit();
  n1->Data()->Set_info(1, 1);

  CFG::EDGE_PTR e0 = cfg.Connect(n0, n2);
  e0->Data()->Set_info(1, 0);
  CFG::EDGE_PTR e1 = cfg.Connect(n2, n3);
  e1->Data()->Set_info(1, 1);
  CFG::EDGE_PTR e2 = cfg.Connect(n3, n4);
  e2->Data()->Set_info(1, 2);
  CFG::EDGE_PTR e3 = cfg.Connect(n3, n5);
  e3->Data()->Set_info(1, 3);
  CFG::EDGE_PTR e4 = cfg.Connect(n4, n6);
  e4->Data()->Set_info(1, 4);
  CFG::EDGE_PTR e5 = cfg.Connect(n5, n6);
  e5->Data()->Set_info(1, 5);
  CFG::EDGE_PTR e6 = cfg.Connect(n6, n7);
  e6->Data()->Set_info(1, 6);
  CFG::EDGE_PTR e7 = cfg.Connect(n2, n7);
  e7->Data()->Set_info(1, 7);
  CFG::EDGE_PTR e8 = cfg.Connect(n7, n1);
  e8->Data()->Set_info(1, 8);

  // Build dom info
  dom_builder.Run();

  // post order
  EXPECT_EQ(dom_info.Get_post_order(n0->Id().Value()), 7);
  EXPECT_EQ(dom_info.Get_post_order(n1->Id().Value()), 0);
  EXPECT_EQ(dom_info.Get_post_order(n2->Id().Value()), 6);
  EXPECT_EQ(dom_info.Get_post_order(n3->Id().Value()), 5);
  EXPECT_EQ(dom_info.Get_post_order(n4->Id().Value()), 4);
  EXPECT_EQ(dom_info.Get_post_order(n5->Id().Value()), 3);
  EXPECT_EQ(dom_info.Get_post_order(n6->Id().Value()), 2);
  EXPECT_EQ(dom_info.Get_post_order(n7->Id().Value()), 1);

  // rev post order
  EXPECT_EQ(dom_info.Get_rev_post_order(0), n0->Id().Value());
  EXPECT_EQ(dom_info.Get_rev_post_order(1), n2->Id().Value());
  EXPECT_EQ(dom_info.Get_rev_post_order(2), n3->Id().Value());
  EXPECT_EQ(dom_info.Get_rev_post_order(3), n4->Id().Value());
  EXPECT_EQ(dom_info.Get_rev_post_order(4), n5->Id().Value());
  EXPECT_EQ(dom_info.Get_rev_post_order(5), n6->Id().Value());
  EXPECT_EQ(dom_info.Get_rev_post_order(6), n7->Id().Value());
  EXPECT_EQ(dom_info.Get_rev_post_order(7), n1->Id().Value());

  // immediate dominator
  EXPECT_EQ(dom_info.Get_imm_dom(n0), n0->Id().Value());
  EXPECT_EQ(dom_info.Get_imm_dom(n1), n7->Id().Value());
  EXPECT_EQ(dom_info.Get_imm_dom(n2), n0->Id().Value());
  EXPECT_EQ(dom_info.Get_imm_dom(n3), n2->Id().Value());
  EXPECT_EQ(dom_info.Get_imm_dom(n4), n3->Id().Value());
  EXPECT_EQ(dom_info.Get_imm_dom(n5), n3->Id().Value());
  EXPECT_EQ(dom_info.Get_imm_dom(n6), n3->Id().Value());
  EXPECT_EQ(dom_info.Get_imm_dom(n7), n2->Id().Value());

  // dominate frontiers
  air::util::MEM_POOL<4096> pool1;
  const CSET& n0_df = dom_info.Get_dom_frontiers(n0->Id().Value());
  const CSET& n1_df = dom_info.Get_dom_frontiers(n1->Id().Value());
  const CSET& n2_df = dom_info.Get_dom_frontiers(n2->Id().Value());
  const CSET& n3_df = dom_info.Get_dom_frontiers(n3->Id().Value());
  const CSET& n4_df = dom_info.Get_dom_frontiers(n4->Id().Value());
  const CSET& n5_df = dom_info.Get_dom_frontiers(n5->Id().Value());
  const CSET& n6_df = dom_info.Get_dom_frontiers(n6->Id().Value());
  const CSET& n7_df = dom_info.Get_dom_frontiers(n7->Id().Value());
  CSET        empty_set(&pool1);
  CSET        exp_n3_df({7}, &pool1);
  CSET        exp_n4_df({6}, &pool1);
  CSET        exp_n5_df({6}, &pool1);
  CSET        exp_n6_df({7}, &pool1);
  EXPECT_EQ(n0_df, empty_set);
  EXPECT_EQ(n1_df, empty_set);
  EXPECT_EQ(n2_df, empty_set);
  EXPECT_EQ(n3_df, exp_n3_df);
  EXPECT_EQ(n4_df, exp_n4_df);
  EXPECT_EQ(n5_df, exp_n5_df);
  EXPECT_EQ(n6_df, exp_n6_df);
  EXPECT_EQ(n7_df, empty_set);

  // iterative dominate frontiers
  const CSET& n0_iter_df = dom_info.Get_iter_dom_frontiers(n0->Id().Value());
  const CSET& n1_iter_df = dom_info.Get_iter_dom_frontiers(n1->Id().Value());
  const CSET& n2_iter_df = dom_info.Get_iter_dom_frontiers(n2->Id().Value());
  const CSET& n3_iter_df = dom_info.Get_iter_dom_frontiers(n3->Id().Value());
  const CSET& n4_iter_df = dom_info.Get_iter_dom_frontiers(n4->Id().Value());
  const CSET& n5_iter_df = dom_info.Get_iter_dom_frontiers(n5->Id().Value());
  const CSET& n6_iter_df = dom_info.Get_iter_dom_frontiers(n6->Id().Value());
  const CSET& n7_iter_df = dom_info.Get_iter_dom_frontiers(n7->Id().Value());
  CSET        exp_iter_n3({7}, &pool1);
  CSET        exp_iter_n4({6, 7}, &pool1);
  CSET        exp_iter_n5({6, 7}, &pool1);
  CSET        exp_iter_n6({7}, &pool1);
  EXPECT_EQ(n0_iter_df, empty_set);
  EXPECT_EQ(n1_iter_df, empty_set);
  EXPECT_EQ(n2_iter_df, empty_set);
  EXPECT_EQ(n3_iter_df, exp_iter_n3);
  EXPECT_EQ(n4_iter_df, exp_iter_n4);
  EXPECT_EQ(n5_iter_df, exp_iter_n5);
  EXPECT_EQ(n6_iter_df, exp_iter_n6);
  EXPECT_EQ(n7_iter_df, empty_set);

  // dom tree
  //       n0
  //       |
  //       n2
  //     /    \
  //   n3      n7
  //  / | \    |
  // n4 n5 n6  n1
  air::util::MEM_POOL<4096> pool2;
  const CVEC&               n0_dt = dom_info.Get_dom_children(n0->Id().Value());
  const CVEC&               n1_dt = dom_info.Get_dom_children(n1->Id().Value());
  const CVEC&               n2_dt = dom_info.Get_dom_children(n2->Id().Value());
  const CVEC&               n3_dt = dom_info.Get_dom_children(n3->Id().Value());
  const CVEC&               n4_dt = dom_info.Get_dom_children(n4->Id().Value());
  const CVEC&               n5_dt = dom_info.Get_dom_children(n5->Id().Value());
  const CVEC&               n6_dt = dom_info.Get_dom_children(n6->Id().Value());
  const CVEC&               n7_dt = dom_info.Get_dom_children(n7->Id().Value());
  CVEC                      empty_vec(&pool2);
  CVEC                      exp_dt_n0({2}, &pool2);
  CVEC                      exp_dt_n2({3, 7}, &pool2);
  CVEC                      exp_dt_n3({4, 5, 6}, &pool2);
  CVEC                      exp_dt_n7({1}, &pool2);
  EXPECT_EQ(n0_dt, exp_dt_n0);
  EXPECT_EQ(n1_dt, empty_vec);
  EXPECT_EQ(n2_dt, exp_dt_n2);
  EXPECT_EQ(n3_dt, exp_dt_n3);
  EXPECT_EQ(n4_dt, empty_vec);
  EXPECT_EQ(n5_dt, empty_vec);
  EXPECT_EQ(n6_dt, empty_vec);
  EXPECT_EQ(n7_dt, exp_dt_n7);

  // dom tree pre order
  // n0->n2->n3->n4->n5->n6->n7->n1
  const CVEC& dt_pre_order = dom_info.Dom_tree_pre_order();
  CVEC        exp_pre_order({0, 2, 3, 4, 5, 6, 7, 1}, &pool2);
  EXPECT_EQ(dt_pre_order, exp_pre_order);

  // dom tree pre order index
  EXPECT_EQ(dom_info.Get_dom_tree_pre_idx(n0), 0);
  EXPECT_EQ(dom_info.Get_dom_tree_pre_idx(n1), 7);
  EXPECT_EQ(dom_info.Get_dom_tree_pre_idx(n2), 1);
  EXPECT_EQ(dom_info.Get_dom_tree_pre_idx(n3), 2);
  EXPECT_EQ(dom_info.Get_dom_tree_pre_idx(n4), 3);
  EXPECT_EQ(dom_info.Get_dom_tree_pre_idx(n5), 4);
  EXPECT_EQ(dom_info.Get_dom_tree_pre_idx(n6), 5);
  EXPECT_EQ(dom_info.Get_dom_tree_pre_idx(n7), 6);
}

}  // namespace
