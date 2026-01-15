//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_CG_LSRA_H
#define AIR_CG_LSRA_H

#include <cstddef>
#include <functional>
#include <list>
#include <memory>
#include <set>
#include <unordered_map>
#include <vector>

#include "air/base/st_decl.h"
#include "air/cg/cgir_container.h"
#include "air/cg/cgir_decl.h"
#include "air/cg/config.h"
#include "air/cg/live_interval_builder.h"
#include "air/cg/preg2dreg_rewritter.h"
#include "air/driver/driver_ctx.h"
#include "air/driver/pass.h"
#include "air/util/debug.h"
#include "air/util/error.h"

namespace air {
namespace cg {

//! @brief Context of LSRA.
class LSRA_CTX {
public:
  LSRA_CTX(const driver::DRIVER_CTX* driver_ctx, const CODE_GEN_CONFIG* config)
      : _driver_ctx(driver_ctx), _config(config) {}
  ~LSRA_CTX() {}

  DECLARE_TRACE_DETAIL_API((*_config), _driver_ctx)

  const driver::DRIVER_CTX* Driver_ctx(void) const { return _driver_ctx; }
  const CODE_GEN_CONFIG*    Cfg(void) const { return _config; }
private:
  const driver::DRIVER_CTX* _driver_ctx = nullptr;
  const CODE_GEN_CONFIG*    _config = nullptr;
};

//! @brief Implementation of linear scan register allocation based on SSA.
//! Current implementation is based on article:
//! Wimmer Christian and Michael Franz. Linear scan register allocation on SSA
//! form. IEEE/ACM International Symposium on Code Generation and Optimization
//! (2010).
class LSRA {
public:
  using VALUE2LI =
      std::unordered_map<PREG_INFO, LIVE_INTERVAL, PREG_INFO::HASH>;
  using BB_LIVE_VAR = std::vector<std::set<PREG_INFO>>;
  using DREG2LI     = std::vector<LIVE_INTERVAL::LIST>;  // index is reg_num.
  using RC_DREG2LI  = std::vector<DREG2LI>;  // index is register class.
  LSRA(const driver::DRIVER_CTX* driver_ctx, const CODE_GEN_CONFIG* config,
       CGIR_CONTAINER* cont, uint32_t isa)
      : _ctx(driver_ctx, config),
        _cont(cont),
        _li_info(cont),
        _isa(isa) {}
  ~LSRA() {}

  template <typename IR_GEN>
  R_CODE Run(IR_GEN* ir_gen) {
    // 1. build ssa
    _cont->Build_ssa();

    // 2. create live interval for each pseudo register
    LIVE_INTERVAL_BUILDER li_builder(_ctx.Driver_ctx(), _ctx.Cfg(), _cont, &_li_info);
    R_CODE                ret = li_builder.Run();
    if (ret != R_CODE::NORMAL) return ret;

    // 3. perform linear scan to select physical register for each PREG.
    Linear_scan();
    _ctx.Trace(TRACE_LSRA, "\nLinear scan result:\n");
    _ctx.Trace_obj(TRACE_LSRA, this);

    // 4. rewrite PREG to selected physical register
    PREG2DREG_REWRITTER preg_rewritter(_ctx.Driver_ctx(), _ctx.Cfg(), _cont, &_li_info, &_dreg_li, _isa);
    ret = preg_rewritter.Run(ir_gen);
    if (ret != R_CODE::NORMAL) return ret;

    return R_CODE::NORMAL;
  }

  bool     Alloc_blocked_reg();
  uint16_t Select_reg(const std::vector<uint32_t>& freePos,
                      const LIVE_INTERVAL*         li);
  bool     Alloc_free_reg(LIVE_INTERVAL* li);
  void     Linear_scan();

  void Print(std::ostream& os, uint32_t indent = 0) const;
  void Print(void) const { Print(std::cout, 0); }
private:
  // REQUIRED UNDEFINED UNWANTED methods
  LSRA(void);
  LSRA(const LSRA&);
  LSRA& operator=(const LSRA&);

  const RC_DREG2LI&    Dreg_li(void) const { return _dreg_li; }
  RC_DREG2LI&          Dreg_li(void) { return _dreg_li; }
  LIVE_INTERVAL::LIST& Dreg_li(uint8_t rc, uint16_t reg_num) {
    if (_dreg_li.size() <= rc) {
      _dreg_li.resize(rc + 1);
    }
    if (_dreg_li[rc].size() <= reg_num) {
      _dreg_li[rc].resize(reg_num + 1);
    }
    return _dreg_li[rc][reg_num];
  }
  void Set_dreg_li(uint8_t rc, uint16_t reg_num, LIVE_INTERVAL* li) {
    if (_dreg_li.size() <= rc) {
      _dreg_li.resize(rc + 1);
    }
    if (_dreg_li[rc].size() <= reg_num) {
      _dreg_li[rc].resize(reg_num + 1);
    }
    _dreg_li[rc][reg_num].push_back(li);
    li->Set_reg_num(reg_num);
  }

  //! @brief Verify that live ranges in each physical register satisfy the
  //! following constraints:
  //! 1. Live ranges are ordered by start position;
  //! 2. Live ranges do not conflict with each other.
  bool Verify_dreg_li(void) const;

  LIVE_INTERVAL::LIST& Active(void) { return _active; }
  LIVE_INTERVAL::LIST& Inactive(void) { return _inactive; }
  LIVE_INTERVAL_INFO&  Li_info(void) { return _li_info; }
  CGIR_CONTAINER*      Container(void) { return _cont; }

  LSRA_CTX           _ctx;
  CGIR_CONTAINER*    _cont;
  //< live interval of each preg.
  LIVE_INTERVAL_INFO _li_info;
  //< live interval allocated in each phsical register.
  RC_DREG2LI _dreg_li;

  LIVE_INTERVAL::LIST _active;    //< active live intervals
  LIVE_INTERVAL::LIST _inactive;  //< inactive live intervals
  uint8_t             _isa;       //< target ISA id
};

}  // namespace cg
}  // namespace air
#endif  // AIR_CG_LSRA_H