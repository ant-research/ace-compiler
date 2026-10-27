//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_BASE_TIMING_UTIL_H
#define AIR_BASE_TIMING_UTIL_H

#include "air/base/container.h"
#include "air/base/rt_timing.h"
#include "air/base/st.h"

namespace air {

namespace base {

//! @brief Utility to generate STMTs for TIMING
class TIMING_GEN {
public:
  TIMING_GEN(CONTAINER* cntr, uint32_t event, uint32_t id, SPOS spos)
      : _cntr(cntr), _event(event), _id(id), _spos(spos) {
    AIR_ASSERT(event != RTM_NONE);
  }

  STMT_PTR New_tm_start() {
    STMT_PTR stmt = _cntr->New_intrn_call(TM_START, PREG_PTR(), 2, _spos);
    _cntr->New_arg(stmt, 0,
                   _cntr->New_intconst(PRIMITIVE_TYPE::INT_S32, _event, _spos));
    _cntr->New_arg(stmt, 1,
                   _cntr->New_intconst(PRIMITIVE_TYPE::INT_S32, _id, _spos));
    return stmt;
  }

  STMT_PTR New_tm_end() {
    STMT_PTR stmt = _cntr->New_intrn_call(TM_END, PREG_PTR(), 2, _spos);
    _cntr->New_arg(stmt, 0,
                   _cntr->New_intconst(PRIMITIVE_TYPE::INT_S32, _event, _spos));
    _cntr->New_arg(stmt, 1,
                   _cntr->New_intconst(PRIMITIVE_TYPE::INT_S32, _id, _spos));
    return stmt;
  }

  bool Enable() const { return _event != RTM_NONE; }

private:
  CONTAINER* _cntr;
  uint32_t   _event;
  uint32_t   _id;
  SPOS       _spos;

  static constexpr const char* TM_START = "CMPLR_TM_START";
  static constexpr const char* TM_END   = "CMPLR_TM_END";
};

//! @brief Utility to insert timing STMTs
template <typename CONTEXT>
class TIMING_UTIL {
public:
  TIMING_UTIL(CONTEXT& ctx, uint32_t event, uint32_t id, const SPOS& spos,
              bool prepend)
      : _ctx(ctx), _gen(ctx.Container(), event, id, spos), _prepend(prepend) {
    if (_ctx.Rt_timing()) {
      // Prepend TM_START before node
      _ctx.Prepend(_gen.New_tm_start());
    }
  }

  ~TIMING_UTIL() {
    if (_ctx.Rt_timing()) {
      // Append TM_TAKEN after node
      STMT_PTR tm_end = _gen.New_tm_end();
      if (_prepend) {
        _ctx.Prepend(tm_end);
      } else {
        _ctx.Append(tm_end);
      }
    }
  }

private:
  CONTEXT&   _ctx;
  TIMING_GEN _gen;
  bool       _prepend;
};

}  // namespace base

}  // namespace air

#endif  // AIR_BASE_TIMING_UTIL_H
