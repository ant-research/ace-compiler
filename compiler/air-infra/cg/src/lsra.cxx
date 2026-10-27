//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/cg/lsra.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <list>
#include <vector>

#include "air/base/spos.h"
#include "air/cg/cgir_decl.h"
#include "air/cg/cgir_enum.h"
#include "air/cg/cgir_util.h"
#include "air/cg/isa_core.h"
#include "air/cg/live_interval.h"
#include "air/cg/targ_info.h"
#include "air/util/debug.h"
#include "air/util/error.h"
namespace air {
namespace cg {

bool LSRA::Alloc_blocked_reg() {
  AIR_ASSERT_MSG(false, "Spill not implemented.");
  return false;
}

uint16_t LSRA::Select_reg(const std::vector<uint32_t>& freePos,
                          const LIVE_INTERVAL*         li) {
  return REG_UNKNOWN;
}

bool LSRA::Alloc_free_reg(LIVE_INTERVAL* li) {
  uint8_t              rc       = li->Reg_cls();
  const REG_INFO_META* reg_meta = TARG_INFO_MGR::Reg_info(rc);
  uint32_t             reg_cnt  = reg_meta->Reg_num();
  // 1. calculate free position of each physical register.
  // TODO: add un-usable positions for caller save registers at callsites.
  std::vector<uint32_t> free_until(reg_cnt, UINT32_MAX);
  for (LIVE_INTERVAL* ali : Active()) {
    if (ali->Reg_cls() != rc) continue;
    free_until[ali->Reg_num()] = 0;
  }
  for (LIVE_INTERVAL* ili : Inactive()) {
    if (ili->Reg_cls() != rc) continue;
    uint32_t intersect_pos = ili->Intersect_pos(li);
    uint32_t reg_id        = ili->Reg_num();
    free_until[reg_id]     = std::min(intersect_pos, free_until[reg_id]);
  }

  // 2. calculate the register with max free position.
  uint16_t cand_reg     = REG_UNKNOWN;
  uint32_t max_free_pos = 0;
  for (uint16_t reg_id = 0; reg_id < reg_cnt; ++reg_id) {
    // skip non-allocatable registers
    if (!reg_meta->Is_allocatable(reg_id)) continue;
    // skip not free registers
    if (free_until[reg_id] < li->End().Idx()) continue;
    // select caller_save registers when available.
    if (cand_reg != REG_UNKNOWN && reg_meta->Is_caller_save(reg_id) &&
        !reg_meta->Is_caller_save(cand_reg)) {
      max_free_pos = free_until[reg_id];
      cand_reg     = reg_id;
      continue;
    }
    // select the register with longest free interval.
    if (free_until[reg_id] > max_free_pos) {
      max_free_pos = free_until[reg_id];
      cand_reg     = reg_id;
      continue;
    }
  }

  // 3. allocate register for li.
  if (max_free_pos >= li->End().Idx()) {
    AIR_ASSERT(cand_reg != REG_UNKNOWN);
    // free position is out of range of li.
    Set_dreg_li(rc, cand_reg, li);
  } else {
    AIR_ASSERT(cand_reg == REG_UNKNOWN);
    // free interval is short than li, need spilling.
    Alloc_blocked_reg();
    return false;
  }
  return true;
}

void LSRA::Linear_scan() {
  LIVE_INTERVAL::LIST unhandled = Li_info().Live_interval();

  while (!unhandled.empty()) {
    LIVE_INTERVAL* cur_li = unhandled.front();
    unhandled.pop_front();
    const SLOT& pos = cur_li->Start();

    // 1. update active live intervals.
    for (LIVE_INTERVAL_INFO::ITER iter = Active().begin();
         iter != Active().end();) {
      // 1.1 remove active live intervals expired at pos.
      if ((*iter)->End() <= pos) {
        iter = Active().erase(iter);
        continue;
      }
      // 1.2 update active live interval to inactive if it is not active at pos.
      if (!(*iter)->Contain(pos)) {
        Inactive().push_back(*iter);
        iter = Active().erase(iter);
        continue;
      }
      ++iter;
    }

    // 2. update active live intervals.
    for (LIVE_INTERVAL_INFO::ITER iter = Inactive().begin();
         iter != Inactive().end();) {
      // 2.1 remove inactive live intervals expired at pos.
      if ((*iter)->End() <= pos) {
        iter = Inactive().erase(iter);
        continue;
      }
      // 2.2 change inactive live interval to active if it is active at pos
      if ((*iter)->Contain(pos)) {
        Active().push_back(*iter);
        iter = Inactive().erase(iter);
      }
      ++iter;
    }

    // 3. try allocate free register for current live interval.
    bool res = Alloc_free_reg(cur_li);

    if (!res) {
      res = Alloc_blocked_reg();
    }
    if (res) {
      Active().push_back(cur_li);
    }
  }
  Verify_dreg_li();
}

//! @brief Verify that live ranges in each physical register satisfy the
//! following constraints:
//! 1. Live ranges are ordered by start position;
//! 2. Live ranges do not conflict with each other.
bool LSRA::Verify_dreg_li(void) const {
  for (const DREG2LI& dreg_li : Dreg_li()) {
    if (dreg_li.empty()) continue;
    for (const LIVE_INTERVAL::LIST& li_list : dreg_li) {
      if (li_list.empty()) continue;
      LIVE_INTERVAL::LIST::const_iterator iter = li_list.begin();
      for (; iter != li_list.end(); ++iter) {
        AIR_ASSERT(*iter != nullptr);
        LIVE_INTERVAL::LIST::const_iterator next = iter;
        ++next;
        AIR_ASSERT(*next != nullptr);
        if (next == li_list.end()) break;
        if ((*iter)->Start() > (*next)->Start()) {
          AIR_ASSERT_MSG(false, "Existing un-ordered live interval.");
          return false;
        }
        if ((*iter)->Intersect(*next)) {
          AIR_ASSERT_MSG(false, "Existing intersecting live interval.")
          return false;
        }
      }
    }
  }
  return true;
}

void LSRA::Print(std::ostream& os, uint32_t indent) const {
  std::string indent0(indent, ' ');
  for (uint32_t rc = 0; rc < _dreg_li.size(); ++rc) {
    if (Dreg_li()[rc].empty()) continue;
    const DREG2LI& dreg_li = Dreg_li()[rc];
    for (uint32_t reg_num = 0; reg_num < dreg_li.size(); ++reg_num) {
      const LIVE_INTERVAL::LIST& li_list = dreg_li[reg_num];
      if (li_list.empty()) continue;

      LIVE_INTERVAL::LIST::const_iterator iter = li_list.begin();
      os << indent0 << TARG_INFO_MGR::Reg_info(rc)->Reg_name(reg_num) << " {"
         << std::endl;
      for (; iter != li_list.end(); ++iter) {
        AIR_ASSERT(*iter != nullptr);
        (*iter)->Print(os, indent + 4);
      }
      os << indent0 << "}" << std::endl;
    }
  }
}
}  // namespace cg
}  // namespace air