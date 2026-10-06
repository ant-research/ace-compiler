//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/cg/live_interval.h"

#include <string>

#include "air/base/bb_enum.h"
#include "air/base/id_wrapper.h"
#include "air/base/st_decl.h"
#include "air/cg/cgir_decl.h"
#include "air/cg/cgir_enum.h"
#include "air/cg/cgir_util.h"
#include "air/cg/isa_core.h"
#include "air/cg/lsra.h"
#include "air/cg/targ_info.h"
#include "air/util/debug.h"

namespace air {
namespace cg {

void LIVE_RANGE::Print(std::ostream& os, uint32_t indent) const {
  os << std::string(indent, ' ') << "[" << Start().Idx() << ", " << End().Idx()
     << ")";
}

void DREG_INFO::Print(std::ostream& os, uint32_t indent) const {
  os << std::string(indent, ' ');
  if (Kind() == LOC_KIND::REGISTER) {
    if (Reg_num() == REG_UNKNOWN) {
      os << TARG_INFO_MGR::Reg_class_name(Reg_cls()) << "*";
    } else {
      os << TARG_INFO_MGR::Reg_info(Reg_cls())->Reg_name(Reg_num());
    }
  } else if (Kind() == LOC_KIND::STACK) {
    os << "stack_" << Stack_id();
  } else {
    os << " not allocated.";
  }
}

void PREG_INFO::Print(std::ostream& os, uint32_t indent) const {
  std::string indent0(indent, ' ');
  os << indent0 << "PREG(%" << Preg().Index() << ") defined in INST-"
     << Def_inst().Value() << " allocated in ";
  Reg_info().Print(os, 0);
}

INST_PTR LIVE_INTERVAL::Def_inst(void) const {
  return Container()->Inst(Def_inst_id());
}

uint32_t LIVE_INTERVAL::Intersect_pos(const LIVE_INTERVAL* li) const {
  if (li == nullptr || li->Empty() || Empty()) return SLOT::INVALID_MAX_ID;

  for (const LIVE_RANGE& lr : li->_range) {
    const LIVE_RANGE* cand_intersect_lr = Advance_to(lr.Start());
    if (cand_intersect_lr == nullptr) return SLOT::INVALID_MAX_ID;
    // candidate intersecting live range starts later than lr, try another
    // live range in li.
    if (cand_intersect_lr->Start().Idx() >= lr.End().Idx()) continue;
    // find the intersecting live range, return the min start position.
    return std::max(cand_intersect_lr->Start().Idx(), lr.Start().Idx());
  }
  return SLOT::INVALID_MAX_ID;
}

void LIVE_INTERVAL::Update_start_slot(SLOT slot) {
  LIVE_RANGE* lr = Advance_to(slot);
  if (lr == nullptr) {
    AIR_ASSERT(_range.empty());
    AIR_ASSERT(slot.Kind() == REGISTER_SLOT);
    slot.Set_kind(DEAD_SLOT);
    Range().push_back(LIVE_RANGE(slot, slot.Next(), {}));
    return;
  }
  lr->Set_start(slot);
}

bool LIVE_INTERVAL::Merge_succ(LR_LIST::iterator lr_iter) {
  bool changed = false;
  if (lr_iter == Range().end()) return changed;

  LIVE_RANGE& cur = *lr_iter;
  ++lr_iter;
  while (lr_iter != Range().end() &&
         (cur.Intersect(*(lr_iter)) || cur.End() == lr_iter->Start())) {
    cur.Merge(*lr_iter);
    lr_iter = Range().erase(lr_iter);
    changed = true;
  }
  return changed;
}

void LIVE_INTERVAL::Add_range(const LIVE_RANGE& lr) {
  if (Range().empty()) {
    Range().push_back(lr);
    return;
  }
  bool              updated = false;
  LR_LIST::iterator iter    = _range.begin();
  for (; iter != _range.end(); ++iter) {
    // merge live range intersecting with lr
    if (iter->Intersect(lr) || iter->Start() == lr.End()) {
      iter->Merge(lr);
      Merge_succ(iter);
      return;
    } else if (iter->Start() > lr.End()) {
      Range().insert(iter, lr);
      return;
    }
  }
  AIR_ASSERT(iter != _range.end());
  _range.push_back(lr);
}

bool LIVE_INTERVAL::Verify(void) const {
  for (LR_CONST_ITER iter = _range.begin(); iter != _range.end(); ++iter) {
    LR_CONST_ITER next_iter = iter;
    ++next_iter;
    if (next_iter == _range.end()) return true;

    if (iter->Start().Idx() >= next_iter->Start().Idx()) {
      AIR_ASSERT_MSG(false, "Order of live range is broken.");
      return false;
    }
    if (iter->End() == next_iter->Start()) {
      AIR_ASSERT_MSG(false, "Existing fusible successive live ranges.")
      return false;
    }
    if (iter->Intersect(*next_iter)) {
      AIR_ASSERT_MSG(false, "Encounter overlap live range.");
      return false;
    }
  }
  return true;
}

void LIVE_INTERVAL::Print(std::ostream& os, uint32_t indent) const {
  std::string indent0(indent, ' ');
  std::string indent1(indent + 4, ' ');
  Preg_info().Print(os, indent);
  os << " contains follow live ranges: {" << std::endl;
  uint32_t lr_cnt = 0;
  for (const LIVE_RANGE& lr : _range) {
    os << indent1 << "live range " << ++lr_cnt << ": ";
    lr.Print(os, 0);
    os << std::endl;
  }
  os << indent0 << "}" << std::endl;
}

//! @brief Update _li to include all the live intervals contained in _value_li
void LIVE_INTERVAL_INFO::Update(void) {
  LI_LIST& li = Live_interval();
  li.clear();
  for (VALUE2LI::iterator iter = _value_li.begin(); iter != _value_li.end();
       ++iter) {
    li.push_back(&(iter->second));
  }
  li.sort([](const LIVE_INTERVAL* li0, const LIVE_INTERVAL* li1) {
    bool res = (*li0 < *li1);
    return (res);
  });
  AIR_ASSERT(Verify());
}

//! @brief Verify that live intervals in _li are ordered by their start
//! positions.
bool LIVE_INTERVAL_INFO::Verify(void) const {
  LI_LIST::const_iterator iter = Begin_li();
  AIR_ASSERT((*iter)->Verify());
  for (; iter != End_li(); ++iter) {
    LI_LIST::const_iterator next = iter;
    ++next;
    if (next == End_li()) break;
    AIR_ASSERT((*next)->Verify());
    if ((*iter)->Start() > (*next)->Start()) {
      (*iter)->Print();
      (*next)->Print();
      AIR_ASSERT_MSG(false, "Existing unordered live interval");
    }
  }
  return true;
}

void LIVE_INTERVAL_INFO::Print(std::ostream& os, uint32_t indent) const {
  std::string indent0(indent, ' ');
  std::string indent1(indent + 4, ' ');
  os << indent0 << "LIVE_INTERVAL_INFO contains " << _value_li.size()
     << " LIVE_INTERVAL {" << std::endl;
  for (const std::pair<const PREG_INFO, LIVE_INTERVAL>& val2li : _value_li) {
    val2li.second.Print(os, indent + 4);
  }
  os << indent0 << "}" << std::endl;
}

}  // namespace cg
}  // namespace air