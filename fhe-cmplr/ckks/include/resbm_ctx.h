//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CKKS_RESBM_CTX_H
#define FHE_CKKS_RESBM_CTX_H

#include <cfloat>

#include "air/base/analyze_ctx.h"
#include "air/base/container.h"
#include "air/base/st_const.h"
#include "air/driver/driver_ctx.h"
#include "air/opt/dfg_builder.h"
#include "air/opt/scc_container.h"
#include "air/opt/ssa_build.h"
#include "air/opt/ssa_container.h"
#include "air/util/debug.h"
#include "air/util/messg.h"
#include "ckks_cost_model.h"
#include "dfg_region.h"
#include "dfg_region_container.h"
#include "fhe/ckks/ckks_opcode.h"
#include "fhe/ckks/config.h"
#include "fhe/core/lower_ctx.h"
#include "min_cut_region.h"

namespace fhe {
namespace ckks {

#define INVALID_SCALE   UINT_MAX
#define INVALID_LEVEL   UINT_MAX
#define INVALID_LATENCY DBL_MAX
#define MAX_LATENCY     FLT_MAX

//! @brief ELEM_BTS_INFO contains bootstrap information of a region element,
//! provides a comparison operator for SET containers, and includes access
//! and print APIs for the region element and bootstrap result level.
//! _bts_lev: resulting level of bootstrap.
class ELEM_BTS_INFO {
public:
  using NODE_ID    = air::base::NODE_ID;
  using SSA_VER_ID = air::opt::SSA_VER_ID;
  using SET        = std::set<ELEM_BTS_INFO>;
  ELEM_BTS_INFO(REGION_ELEM_PTR elem, uint32_t lev)
      : _elem(elem), _bts_lev(lev) {}
  ELEM_BTS_INFO(const ELEM_BTS_INFO& o) : ELEM_BTS_INFO(o._elem, o._bts_lev) {}
  ELEM_BTS_INFO(void) : _elem(), _bts_lev(0) {}
  ~ELEM_BTS_INFO() {}

  bool Is_invalid(void) const {
    return _elem == REGION_ELEM_PTR() || _bts_lev == 0;
  }
  bool       Is_expr(void) const { return _elem->Dfg_node()->Is_node(); }
  bool       Is_ssa_ver(void) const { return _elem->Dfg_node()->Is_ssa_ver(); }
  NODE_ID    Expr(void) const { return _elem->Dfg_node()->Node_id(); }
  SSA_VER_ID Ssa_ver(void) const { return _elem->Dfg_node()->Ssa_ver_id(); }
  uint32_t   Bts_lev(void) const { return _bts_lev; }
  REGION_ELEM_PTR Region_elem(void) const { return _elem; }

  bool operator<(const ELEM_BTS_INFO& o) const {
    if (_elem->Dfg_node_id() < o._elem->Dfg_node_id()) return true;
    if (o._elem->Dfg_node_id() < _elem->Dfg_node_id()) return false;
    return _bts_lev < o._bts_lev;
  }
  bool operator==(const ELEM_BTS_INFO& o) const {
    return _elem->Dfg_node_id() == o._elem->Dfg_node_id() &&
           _bts_lev == o._bts_lev;
  }

  std::string To_str(void) const {
    if (Is_invalid()) return "Invalid BTS_INFO";
    return _elem->To_str() + ", bts_lev= " + std::to_string(_bts_lev);
  }

  void Print(std::ostream& os) const { os << To_str() << std::endl; }
  void Print(void) const { Print(std::cout); }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  ELEM_BTS_INFO& operator=(const ELEM_BTS_INFO&);

  REGION_ELEM_PTR _elem;
  uint32_t        _bts_lev;
};

//! @brief SCALE_LEVEL contains scale and level of a region element,
//! and provides access and print APIs for them.
class SCALE_LEVEL {
public:
  SCALE_LEVEL(uint32_t scale, uint32_t level) : _scale(scale), _level(level) {}
  SCALE_LEVEL(void) : SCALE_LEVEL(INVALID_SCALE, INVALID_LEVEL) {}
  SCALE_LEVEL(const SCALE_LEVEL& o) : SCALE_LEVEL(o.Scale(), o.Level()) {}
  SCALE_LEVEL& operator=(const SCALE_LEVEL& other) {
    _scale = other._scale;
    _level = other._level;
    return *this;
  }
  ~SCALE_LEVEL() {}

  uint32_t Scale(void) const { return _scale; }
  uint32_t Level(void) const { return _level; }
  bool     Is_invalid(void) const {
    return _scale == INVALID_SCALE || _level == INVALID_LEVEL;
  }
  std::string To_str(void) const {
    return "[scale=" + std::to_string(_scale) +
           ",level=" + std::to_string(_level) + "]";
  }
  void Print(std::ostream& os) const { os << To_str() << std::endl; }
  void Print(void) const { Print(std::cout); }

private:
  uint32_t _scale = INVALID_SCALE;
  uint32_t _level = INVALID_LEVEL;
};

//! @brief VAR_SCALE_LEV contains scale/level of a variable.
class VAR_SCALE_LEV {
public:
  using VEC = std::vector<VAR_SCALE_LEV>;
  VAR_SCALE_LEV(air::base::ADDR_DATUM_PTR addr_datum, SCALE_LEVEL scale_lev)
      : _addr_datum(addr_datum), _scale_lev(scale_lev) {}
  VAR_SCALE_LEV(const VAR_SCALE_LEV& o)
      : VAR_SCALE_LEV(o.Var(), o.Scale_lev()) {}
  VAR_SCALE_LEV(void) : _addr_datum(), _scale_lev() {}
  VAR_SCALE_LEV& operator=(const VAR_SCALE_LEV& o) {
    _addr_datum = o.Var();
    _scale_lev  = o.Scale_lev();
    return *this;
  }
  ~VAR_SCALE_LEV() {}

  air::base::ADDR_DATUM_PTR Var(void) const { return _addr_datum; }
  const SCALE_LEVEL&        Scale_lev(void) const { return _scale_lev; }
  void Set_scale_lev(const SCALE_LEVEL& scale_lev) { _scale_lev = scale_lev; }
  void Print(std::ostream& os) const {
    os << _addr_datum->To_str() << ": " << _scale_lev.To_str() << std::endl;
  }
  void Print(void) const { Print(std::cout); }

private:
  air::base::ADDR_DATUM_PTR _addr_datum;
  SCALE_LEVEL               _scale_lev;
};

//! @brief MIN_LATENCY_PLAN contains information of minimal latency plan of
//! regions in [_src, _dst]. _src/_dst: the start region and the end region of
//! current plan; _elem_scale_lev: scale and level of each region element;
//! _formal_scale_lev: scale and level of a formal;
//! _bypass_edge_bts_lev: bootstrap level for bypass edge;
//! _region_laten: latency of each region in [_src, _dst];
//! _region_cut: the cut to insert rescale/bootstrap operations. The index is
//! calculated as
//!              (region_id - _src). The cut at _src is for bootstrap points,
//!              while cuts at other regions are for rescale.
//! _redundant_bts: redundant bootstrap operations resulting from current plan;
//! _used_lev: consumed level by current plan.
class MIN_LATENCY_PLAN {
public:
  using ELEM_SCALE_LEV      = std::map<REGION_ELEM_ID, SCALE_LEVEL>;
  using BYPASS_EDGE_BTS_LEV = std::map<REGION_ELEM_ID, uint32_t>;
  using FORMAL_SCALE_LEV    = std::map<CALLSITE_INFO, std::list<VAR_SCALE_LEV>>;

  MIN_LATENCY_PLAN(const REGION_CONTAINER* cntr, REGION_ID src, REGION_ID dst)
      : _region_cntr(cntr), _src(src), _dst(dst) {}
  MIN_LATENCY_PLAN(const REGION_CONTAINER* cntr, REGION_ID dst)
      : MIN_LATENCY_PLAN(cntr, REGION_ID(), dst) {}
  ~MIN_LATENCY_PLAN() {}

  REGION_ID  Src_id(void) const { return _src; }
  REGION_PTR Src(void) const { return _region_cntr->Region(_src); }
  REGION_ID  Dst_id(void) const { return _dst; }
  REGION_PTR Dst(void) const { return _region_cntr->Region(_dst); }
  void       Set_src_region(REGION_ID region) { _src = region; }
  void       Set_dst_region(REGION_ID region) { _dst = region; }

  void Set_scale_lev(REGION_ELEM_ID elem, const SCALE_LEVEL& scale_lev) {
    _elem_scale_lev[elem] = scale_lev;
  }
  const ELEM_SCALE_LEV& Scale_lev(void) const { return _elem_scale_lev; }
  SCALE_LEVEL           Scale_lev(REGION_ELEM_ID elem) {
    ELEM_SCALE_LEV::const_iterator iter = _elem_scale_lev.find(elem);
    if (iter == _elem_scale_lev.end()) return SCALE_LEVEL();
    return iter->second;
  }

  //! @brief Update scale and level of formals at a call_site.
  void Set_scale_lev(CALLSITE_INFO call_site, VAR_SCALE_LEV scale_lev) {
    // Predicator to find the formal of scale_lev.
    auto pred = [&](const VAR_SCALE_LEV& var_scale) -> bool {
      return var_scale.Var() == scale_lev.Var();
    };
    std::list<VAR_SCALE_LEV>& formal_scale_lev = _formal_scale_lev[call_site];
    std::list<VAR_SCALE_LEV>::iterator iter =
        std::find_if(formal_scale_lev.begin(), formal_scale_lev.end(), pred);
    if (iter != formal_scale_lev.end()) {
      iter->Set_scale_lev(scale_lev.Scale_lev());
    } else {
      formal_scale_lev.push_back(scale_lev);
    }
  }
  FORMAL_SCALE_LEV&       Formal_scale_lev(void) { return _formal_scale_lev; }
  const FORMAL_SCALE_LEV& Formal_scale_lev(void) const {
    return _formal_scale_lev;
  }

  uint32_t Used_level(void) const { return _used_lev; }
  void     Set_used_level(uint32_t val) { _used_lev = val; }

  //! @brief Set bootstrap level of the src element of a bypass edge.
  void Set_bts_lev(REGION_ELEM_ID elem, uint32_t lev) {
    std::pair<BYPASS_EDGE_BTS_LEV::iterator, bool> res =
        _bypass_edge_bts_lev.emplace(elem, lev);
    if (res.second) return;
    if (res.first->second >= lev) return;
    res.first->second = lev;
  }
  //! @brief Return bootstrap level of bypass edges.
  const BYPASS_EDGE_BTS_LEV& Bypass_edge_bts_info(void) const {
    return _bypass_edge_bts_lev;
  }

  //! @brief Set cut of region at which insert rescale/bootstrap.
  void Set_cut(REGION_ID region, const CUT_TYPE& cut) {
    AIR_ASSERT(region.Value() >= _src.Value());
    AIR_ASSERT(region.Value() <= _dst.Value());
    AIR_ASSERT(!cut.Empty());

    uint32_t id = region.Value() - _src.Value();
    if (_region_cut.size() <= id) {
      _region_cut.resize(id + 1, CUT_TYPE(Region_cntr()));
    }
    _region_cut[id] = cut;
  }
  //! @brief Return the cut to insert bootstrap.
  const CUT_TYPE& Bootstrap_point(void) const {
    AIR_ASSERT(!_region_cut.empty());
    return _region_cut[0];
  }
  CUT_TYPE& Bootstrap_point(void) {
    AIR_ASSERT(!_region_cut.empty());
    return _region_cut[0];
  }
  //! @brief Return the cut at region.
  const CUT_TYPE& Min_cut(REGION_ID region) const {
    AIR_ASSERT_MSG(region.Value() >= _src.Value(), "region out of range");
    AIR_ASSERT_MSG(region.Value() <= _dst.Value(), "region out of range");
    uint32_t id = region.Value() - _src.Value();
    AIR_ASSERT_MSG(id < _region_cut.size(), "min cut not set");
    return _region_cut[id];
  }

  //! @brief Record a redundant bootstrap resulting from select current plan.
  void Add_redundant_bootstrap(const ELEM_BTS_INFO& bts_info) {
    _redundant_bts.insert(bts_info);
  }
  //! @brief Return redundant bootstrap operations resulting from current plan.
  ELEM_BTS_INFO::SET&       Redundant_bootstrap(void) { return _redundant_bts; }
  const ELEM_BTS_INFO::SET& Redundant_bootstrap(void) const {
    return _redundant_bts;
  }

  //! @brief Return latency of region.
  double Latency(REGION_ID region) const {
    AIR_ASSERT_MSG(region.Value() >= _src.Value(), "region out of range");
    AIR_ASSERT_MSG(region.Value() <= _dst.Value(), "region out of range");

    uint32_t id = region.Value() - _src.Value();
    if (id >= _region_laten.size()) return INVALID_LATENCY;
    return _region_laten[id];
  }
  // @brief Set latency of region as val.
  void Set_latency(REGION_ID region, double val) {
    AIR_ASSERT_MSG(region.Value() >= _src.Value(), "region out of range");
    AIR_ASSERT_MSG(region.Value() <= _dst.Value(), "region out of range");

    uint32_t id = region.Value() - _src.Value();
    if (_region_laten.size() <= id) {
      _region_laten.resize(id + 1, INVALID_LATENCY);
    }
    _region_laten[id] = val;
  }
  //! @brief Return latency operations in regions [_src, _dst].
  double Latency(void) const {
    double laten = 0.;
    for (double val : _region_laten) laten += val;
    for (const std::pair<REGION_ELEM_ID, uint32_t>& bypass_edge_info :
         _bypass_edge_bts_lev) {
      uint32_t poly_deg =
          Region_cntr()->Lower_ctx()->Get_ctx_param().Get_poly_degree();
      laten += Operation_cost(OPC_BOOTSTRAP, bypass_edge_info.second, poly_deg);
    }
    return laten;
  }

  const REGION_CONTAINER* Region_cntr(void) const { return _region_cntr; }

  void Print(std::ostream& os, uint32_t indent) const;
  void Print(void) const { Print(std::cout, 0); }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  MIN_LATENCY_PLAN(void);
  MIN_LATENCY_PLAN(const MIN_LATENCY_PLAN&);
  MIN_LATENCY_PLAN& operator=(const MIN_LATENCY_PLAN&);

  const REGION_CONTAINER* _region_cntr = nullptr;
  ELEM_SCALE_LEV          _elem_scale_lev;
  FORMAL_SCALE_LEV        _formal_scale_lev;
  BYPASS_EDGE_BTS_LEV     _bypass_edge_bts_lev;
  std::vector<double>     _region_laten;  // latency of each region
  std::vector<CUT_TYPE>   _region_cut;    // cut to insert rescale/bootstrap
  ELEM_BTS_INFO::SET      _redundant_bts;
  uint32_t                _used_lev;  // consumed level by current plan
  REGION_ID               _src;
  REGION_ID               _dst;
};

//! @brief RESBM Context includes minimal latency plans and provides APIs to
//! print, access, create, and update these plans.
class RESBM_CTX : public air::base::ANALYZE_CTX {
public:
  using DRIVER_CTX = air::driver::DRIVER_CTX;
  using LEVEL2CUT  = std::vector<CUT_TYPE>;   // index is input level
  using REGION_CUT = std::vector<LEVEL2CUT>;  // index is region ID

  RESBM_CTX(const DRIVER_CTX* driver_ctx, const CKKS_CONFIG* config,
            const REGION_CONTAINER* cntr)
      : _driver_ctx(driver_ctx), _config(config), _region_cntr(cntr) {
    AIR_ASSERT_MSG(config != nullptr, "nullptr check");
    AIR_ASSERT_MSG(cntr != nullptr, "nullptr check");
  }
  ~RESBM_CTX() {
    // release minimal latency plans
    for (MIN_LATENCY_PLAN* plan : _plan) {
      if (plan == nullptr) continue;
      plan->~MIN_LATENCY_PLAN();
    }
  }

  //! @brief Return min latency plan of region.
  MIN_LATENCY_PLAN* Plan(REGION_ID region) const {
    if (region.Value() >= _plan.size()) return nullptr;
    return _plan[region.Value()];
  }

  //! @brief Set min latency plan of region.
  void Update_plan(REGION_ID region, MIN_LATENCY_PLAN* new_plan) {
    if (_plan.size() <= region.Value()) {
      _plan.resize(region.Value() + 1, nullptr);
    }
    // release prev plan
    if (_plan[region.Value()] != nullptr) {
      uint32_t size = sizeof(MIN_LATENCY_PLAN);
      _mem_pool.Deallocate((char*)_plan[region.Value()], size);
    }
    _plan[region.Value()] = new_plan;
  }

  //! @brief Return new plan for regions in [start, end].
  MIN_LATENCY_PLAN* New_plan(REGION_ID start, REGION_ID end) {
    void* plan = _mem_pool.Allocate(sizeof(MIN_LATENCY_PLAN));
    new (plan) MIN_LATENCY_PLAN(_region_cntr, start, end);
    return (MIN_LATENCY_PLAN*)plan;
  }
  //! @brief Release memory of ptr allocated in _mem_pool
  template <typename T>
  void Release(T* ptr) {
    if (ptr == nullptr) return;
    ptr->~T();
    uint32_t size = sizeof(T);
    _mem_pool.Deallocate((char*)ptr, size);
  }

  //! @brief Return min total latency for regions in [1, region].
  double Total_latency(REGION_ID region);
  //! @brief Return scale and level of region element.
  SCALE_LEVEL Scale_level(REGION_ID region, CONST_REGION_ELEM_PTR elem) const;

  //! @brief Return the min_cut of a region at level = lev and MIN_CUT_KIND =
  //! kind. If the same min_cut has been performed, return the saved result.
  //! Otherwise, perform min_cut for the current region and record the result.
  const CUT_TYPE& Min_cut(REGION_ID region, uint32_t lev, MIN_CUT_KIND kind);

  //! @brief Print context in output stream.
  void Print(std::ostream& os, uint32_t indent, REGION_ID region) const;
  void Print(std::ostream& os, uint32_t indent = 0) const {
    AIR_ASSERT_MSG(_region_cntr->Region_cnt() >= 1, "No valide region");
    Print(os, indent, REGION_ID(_region_cntr->Region_cnt() - 1));
  }
  void Print(void) const { Print(std::cout, 0); }

  const REGION_CONTAINER* Region_cntr(void) const { return _region_cntr; }
  const CKKS_CONFIG*      Config(void) const { return _config; }
  const DRIVER_CTX*       Driver_ctx(void) const { return _driver_ctx; }
  MEMPOOL*                Mem_pool(void) { return &_mem_pool; }
  REGION_CUT&             Rescale_cut(void) { return _rs_cut; }
  REGION_CUT&             Bootstrap_cut(void) { return _bts_cut; }
  uint32_t                Min_bts_lev(void) const {
    return std::min(Config()->Min_bts_lvl(), Max_bts_lvl() / 2);
  }
  DECLARE_CKKS_OPTION_CONFIG_ACCESS_API((*_config))
  DECLARE_TRACE_DETAIL_API((*_config), _driver_ctx)

private:
  MEMPOOL                        _mem_pool;
  const air::driver::DRIVER_CTX* _driver_ctx;
  const CKKS_CONFIG*             _config;
  const REGION_CONTAINER*        _region_cntr;
  std::vector<MIN_LATENCY_PLAN*> _plan;  // index is dst region ID of each plan
  REGION_CUT                     _rs_cut;   // min cut for rescale
  REGION_CUT                     _bts_cut;  // min cut for bootstrap
};
}  // namespace ckks
}  // namespace fhe

#endif  // FHE_CKKS_RESBM_CTX_H
