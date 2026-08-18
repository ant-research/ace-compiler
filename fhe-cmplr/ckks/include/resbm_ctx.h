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
#include <climits>
#include <fstream>
#include <ios>
#include <memory>
#include <ostream>
#include <random>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "../opt/include/scc_builder.h"
#include "air/base/analyze_ctx.h"
#include "air/base/container.h"
#include "air/base/container_decl.h"
#include "air/base/id_wrapper.h"
#include "air/base/st.h"
#include "air/base/st_decl.h"
#include "air/base/transform_ctx.h"
#include "air/driver/common_config.h"
#include "air/driver/driver_ctx.h"
#include "air/opt/dfg_builder.h"
#include "air/opt/scc_container.h"
#include "air/opt/ssa_build.h"
#include "air/opt/ssa_container.h"
#include "air/util/debug.h"
#include "air/util/error.h"
#include "ckks_cost_model.h"
#include "dfg_region.h"
#include "dfg_region_builder.h"
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

class MIN_LATENCY_PLAN {
public:
  class SCALE_INFO {
  public:
    SCALE_INFO(uint32_t scale, uint32_t level) : _scale(scale), _level(level) {}
    SCALE_INFO(void) : _scale(INVALID_SCALE), _level(INVALID_LEVEL) {}
    SCALE_INFO(const SCALE_INFO& other)
        : SCALE_INFO(other.Scale(), other.Level()) {}
    SCALE_INFO& operator=(const SCALE_INFO& other) {
      _scale = other._scale;
      _level = other._level;
      return *this;
    }
    ~SCALE_INFO() {}

    uint32_t    Scale(void) const { return _scale; }
    uint32_t    Level(void) const { return _level; }
    std::string To_str(void) const {
      return "[scale=" + std::to_string(_scale) +
             ",level=" + std::to_string(_level) + "]";
    }

  private:
    uint32_t _scale;
    uint32_t _level;
  };

  //! @brief scale info of variable.
  class VAR_SCALE_INFO {
  public:
    VAR_SCALE_INFO(air::base::ADDR_DATUM_PTR addr_datum, SCALE_INFO scale_info)
        : _addr_datum(addr_datum), _scale_info(scale_info) {}
    VAR_SCALE_INFO(const VAR_SCALE_INFO& o)
        : VAR_SCALE_INFO(o.Var(), o.Scale_info()) {}
    VAR_SCALE_INFO& operator=(const VAR_SCALE_INFO& o) {
      _addr_datum = o.Var();
      _scale_info = o.Scale_info();
      return *this;
    }
    ~VAR_SCALE_INFO() {}

    air::base::ADDR_DATUM_PTR Var(void) const { return _addr_datum; }
    const SCALE_INFO&         Scale_info(void) const { return _scale_info; }
    void                      Set_scale_info(const SCALE_INFO& scale_info) {
      _scale_info = scale_info;
    }
    void Print(std::ostream& os, uint32_t indent) const {
      os << std::string(indent, ' ') << _addr_datum->To_str() << ": "
         << _scale_info.To_str() << std::endl;
    }
    void Print() const { Print(std::cout, 0); }

  private:
    // REQUIRED UNDEFINED UNWANTED methods
    VAR_SCALE_INFO(void);

    air::base::ADDR_DATUM_PTR _addr_datum;
    SCALE_INFO                _scale_info;
  };

  using ELEM_SCALE_INFO     = std::map<REGION_ELEM_ID, SCALE_INFO>;
  using BYPASS_EDGE_BTS_LEV = std::map<REGION_ELEM_ID, uint32_t>;
  using FORMAL_SCALE_INFO = std::map<CALLSITE_INFO, std::list<VAR_SCALE_INFO>>;

  MIN_LATENCY_PLAN(const REGION_CONTAINER* cntr, REGION_ID start, REGION_ID end)
      : _region_cntr(cntr), _start_region(start), _end_region(end) {}
  MIN_LATENCY_PLAN(const REGION_CONTAINER* cntr, REGION_ID end)
      : _region_cntr(cntr),
        _start_region(air::base::Null_id),
        _end_region(end) {}

  ~MIN_LATENCY_PLAN() {}

  REGION_ID Start_region(void) const { return _start_region; }
  REGION_ID End_region(void) const { return _end_region; }

  void Set_start_region(REGION_ID region) { _start_region = region; }
  void Set_end_region(REGION_ID region) { _end_region = region; }
  void Set_elem_scale_info(REGION_ELEM_ID elem, const SCALE_INFO& scale_info) {
    _elem_scale_info[elem] = scale_info;
  }

  const ELEM_SCALE_INFO& Scale_info(void) const { return _elem_scale_info; }
  const SCALE_INFO&      Scale_info(REGION_ELEM_ID elem) {
    ELEM_SCALE_INFO::const_iterator iter = _elem_scale_info.find(elem);
    AIR_ASSERT_MSG(iter != _elem_scale_info.end(),
                        "region element has no scale info");
    return iter->second;
  }
  void Set_formal_scale_info(CALLSITE_INFO  call_site,
                             VAR_SCALE_INFO scale_info) {
    for (VAR_SCALE_INFO& var_scale_info : _formal_scale_info[call_site]) {
      if (var_scale_info.Var() == scale_info.Var()) {
        var_scale_info.Set_scale_info(scale_info.Scale_info());
        return;
      }
    }
    _formal_scale_info[call_site].push_back(scale_info);
  }
  FORMAL_SCALE_INFO& Formal_scale_info(void) { return _formal_scale_info; }
  const FORMAL_SCALE_INFO& Formal_scale_info(void) const {
    return _formal_scale_info;
  }

  uint32_t Consume_level(void) const { return _consume_lev; }
  void     Set_consume_level(uint32_t val) { _consume_lev = val; }

  void Set_bts_lev(REGION_ELEM_ID elem, uint32_t lev) {
    std::pair<BYPASS_EDGE_BTS_LEV::iterator, bool> res =
        _bypass_edge_bts_lev.emplace(elem, lev);
    if (res.second) return;
    if (res.first->second >= lev) return;
    res.first->second = lev;
  }

  const BYPASS_EDGE_BTS_LEV& Bypass_edge_bts_info(void) {
    return _bypass_edge_bts_lev;
  }

  void Set_cut(REGION_ID region, const MIN_CUT::CUT_TYPE& cut) {
    AIR_ASSERT(region.Value() >= _start_region.Value());
    AIR_ASSERT(region.Value() <= _end_region.Value());
    AIR_ASSERT(!cut.empty());

    uint32_t id = region.Value() - _start_region.Value();
    if (_region_cut.size() <= id) {
      _region_cut.resize(id + 1);
    }
    _region_cut[id] = cut;
  }

  const MIN_CUT::CUT_TYPE& Bootstrap_point(void) const {
    AIR_ASSERT(!_region_cut.empty());
    return _region_cut[0];
  }

  const MIN_CUT::CUT_TYPE& Min_cut(REGION_ID region) const {
    AIR_ASSERT_MSG(region.Value() >= _start_region.Value(),
                   "region out of range");
    AIR_ASSERT_MSG(region.Value() <= _end_region.Value(),
                   "region out of range");

    uint32_t id = region.Value() - _start_region.Value();
    AIR_ASSERT_MSG(id < _region_cut.size(), "min cut not set");
    return _region_cut[id];
  }

  double Region_latency(REGION_ID region) const {
    AIR_ASSERT_MSG(region.Value() >= _start_region.Value(),
                   "region out of range");
    AIR_ASSERT_MSG(region.Value() <= _end_region.Value(),
                   "region out of range");

    uint32_t id = region.Value() - _start_region.Value();
    if (id >= _region_laten.size()) return INVALID_LATENCY;
    return _region_laten[id];
  }

  void Set_region_latency(REGION_ID region, double val) {
    AIR_ASSERT_MSG(region.Value() >= _start_region.Value(),
                   "region out of range");
    AIR_ASSERT_MSG(region.Value() <= _end_region.Value(),
                   "region out of range");

    uint32_t id = region.Value() - _start_region.Value();
    if (_region_laten.size() <= id) {
      _region_laten.resize(id + 1, INVALID_LATENCY);
    }
    _region_laten[id] = val;
  }

  double Latency(void) const {
    double laten = 0.;
    for (double val : _region_laten) laten += val;
    for (const std::pair<REGION_ELEM_ID, uint32_t>& bypass_edge_info :
         _bypass_edge_bts_lev) {
      laten += Operation_cost(OPC_BOOTSTRAP, bypass_edge_info.second);
    }
    return laten;
  }

  const REGION_CONTAINER* Region_cntr(void) const { return _region_cntr; }

  void Print(std::ostream& os, uint32_t indent);
  void Print() { Print(std::cout, 0); }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  MIN_LATENCY_PLAN(void);
  MIN_LATENCY_PLAN(const MIN_LATENCY_PLAN&);
  MIN_LATENCY_PLAN& operator=(const MIN_LATENCY_PLAN&);

  const REGION_CONTAINER*        _region_cntr;
  REGION_ID                      _start_region;
  REGION_ID                      _end_region;
  ELEM_SCALE_INFO                _elem_scale_info;
  FORMAL_SCALE_INFO              _formal_scale_info;
  BYPASS_EDGE_BTS_LEV            _bypass_edge_bts_lev;
  std::vector<double>            _region_laten;  // latency of each region
  std::vector<MIN_CUT::CUT_TYPE> _region_cut;
  uint32_t                       _consume_lev;
};
using MIN_LATENCY_PLAN_PTR = std::unique_ptr<MIN_LATENCY_PLAN>;

class RESBM_CTX : public air::base::ANALYZE_CTX {
public:
  using SCALE_INFO = MIN_LATENCY_PLAN::SCALE_INFO;

  RESBM_CTX(const CKKS_CONFIG* config, const REGION_CONTAINER* cntr)
      : _config(config), _region_cntr(cntr) {
    AIR_ASSERT_MSG(config != nullptr, "nullptr check");
    AIR_ASSERT_MSG(cntr != nullptr, "nullptr check");
    _tf = std::ofstream("noise_mng.t", std::ios_base::out);
  }

  ~RESBM_CTX() {}

  MIN_LATENCY_PLAN* Noise_mng_plan(REGION_ID region) const {
    AIR_ASSERT_MSG(region.Value() < _min_laten_plan.size(),
                   "region index out of range");
    return _min_laten_plan[region.Value()].get();
  }

  void Init_noise_mng_plan(uint32_t region_cnt) {
    _min_laten_plan.resize(region_cnt);
  }
  void Update_noise_mng_plan(REGION_ID region, MIN_LATENCY_PLAN* new_plan) {
    _min_laten_plan[region.Value()].reset(new_plan);
  }

  //! @brief Return minimal total latency from main_graph entry to the end of
  //! region.
  double Total_latency(REGION_ID region);

  SCALE_INFO Scale_info(REGION_ID region, REGION_ELEM_PTR elem) const;
  const REGION_CONTAINER* Region_cntr(void) const { return _region_cntr; }

  void Print(std::ostream& os, uint32_t indent, REGION_ID end_region) const {
    os << ">>>>>>>>>>>>Optimal scale mng plan [1," << end_region.Value()
       << "]>>>>>>>>>>>>" << std::endl;
    MIN_LATENCY_PLAN* plan = _min_laten_plan[end_region.Value()].get();
    while (plan != nullptr && plan->Start_region().Value() >= 1) {
      plan->Print(os, 4);
      plan = _min_laten_plan[plan->Start_region().Value()].get();
    }
    os << "<<<<<<<<<<<<END: optimal scale mng plan<<<<<<<<<<<<<<" << std::endl;
  }
  void Print(std::ostream& os, uint32_t indent) const {
    AIR_ASSERT_MSG(_region_cntr->Region_cnt() >= 1, "No valide region");
    Print(os, indent, REGION_ID(_region_cntr->Region_cnt() - 1));
  }
  void Print() const { Print(std::cout, 0); }

  template <typename... ARGS>
  RESBM_CTX* Trace(ARGS&&... args) {
    if (Trace_resbm()) {
      (_tf << ... << args);
      _tf.flush();
    }
    return this;
  }

  template <typename OBJ>
  RESBM_CTX* Trace_obj(const OBJ& obj) {
    if (Trace_resbm()) {
      obj.Print(_tf);
      _tf.flush();
    }
    return this;
  }
  std::ofstream& Trace_file(void) { return _tf; }
  DECLARE_CKKS_CONFIG_ACCESS_API((*_config))

private:
  const CKKS_CONFIG*                _config;
  const REGION_CONTAINER*           _region_cntr;
  std::vector<MIN_LATENCY_PLAN_PTR> _min_laten_plan;  // index is end region ID
  std::ofstream                     _tf;
  uint32_t _l_bts = DEFAULT_MAX_BTS_LEV;  // max resulting level of bootstrap
};
}  // namespace ckks
}  // namespace fhe

#endif  // FHE_CKKS_RESBM_CTX_H