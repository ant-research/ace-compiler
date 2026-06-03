//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_CG_CGIR_UTIL_H
#define AIR_CG_CGIR_UTIL_H

#include "air/cg/cgir_container.h"
#include "air/util/debug.h"

namespace air {

namespace cg {
//! @brief LAYOUT_ITER
//  ITERATOR to traverse all BB in layout order
class LAYOUT_ITER {
public:
  enum class ITER_KIND {
    FWD,  // forward iterator
    REV,  // reverse iterator
  };
  //! @brief internal ITERATOR
  template <ITER_KIND kind = ITER_KIND::FWD>
  class ITER_IMPL {
    friend class LAYOUT_ITER;

  public:
    BB_PTR operator*() const { return _cur_ptr; }
    BB_PTR operator->() const { return _cur_ptr; }

    ITER_IMPL<kind>& operator++() {
      if (kind == ITER_KIND::FWD) {
        _cur_ptr = _cur_ptr->Next();
      } else {
        AIR_ASSERT(kind == ITER_KIND::REV);
        _cur_ptr = _cur_ptr->Prev();
      }
      return *this;
    }
    ITER_IMPL<kind>& operator--() {
      if (kind == ITER_KIND::FWD) {
        _cur_ptr = _cur_ptr->Prev();
      } else {
        AIR_ASSERT(kind == ITER_KIND::REV);
        _cur_ptr = _cur_ptr->Next();
      }
      return *this;
    }

    bool operator==(const ITER_IMPL<kind>& rhs) {
      return _cur_ptr == rhs._cur_ptr;
    }
    bool operator!=(const ITER_IMPL<kind>& rhs) {
      return _cur_ptr != rhs._cur_ptr;
    }

  private:
    ITER_IMPL(BB_PTR bb) : _cur_ptr(bb) {}
    BB_PTR _cur_ptr;
  };
  using ITERATOR         = ITER_IMPL<ITER_KIND::FWD>;
  using REVERSE_ITERATOR = ITER_IMPL<ITER_KIND::REV>;

public:
  LAYOUT_ITER(const CGIR_CONTAINER* cntr) : _cntr(cntr) {}

  // NOLINTBEGIN (readability-identifier-naming)
  ITERATOR begin() const { return ITERATOR(_cntr->Entry()); }
  ITERATOR end() const { return ITERATOR(BB_PTR()); }

  REVERSE_ITERATOR rbegin() const { return REVERSE_ITERATOR(_cntr->Exit()); }
  REVERSE_ITERATOR rend() const { return REVERSE_ITERATOR(BB_PTR()); }
  // NOLINTEND (readability-identifier-naming)

private:
  const CGIR_CONTAINER* _cntr;
};

//! @brief PRED_ITER
//  ITERATOR tp traverse all predecessors of a BB
class PRED_ITER {
public:
  class ITERATOR : public CGIR_CONTAINER::BB_PRED_ITER {
  public:
    ITERATOR(BB_PTR bb) : CGIR_CONTAINER::BB_PRED_ITER(bb) {}
    BB_PTR operator*() const { return Cntr()->Bb(Node_id()); }
    BB_PTR operator->() const { return Cntr()->Bb(Node_id()); }
  };

public:
  PRED_ITER(BB_PTR bb) : _bb(bb) {}

  // NOLINTBEGIN (readability-identifier-naming)
  ITERATOR begin() const { return ITERATOR(_bb); }
  ITERATOR end() const { return ITERATOR(air::base::Null_ptr); }
  // NOLINTEND (readability-identifier-naming)

private:
  BB_PTR _bb;
};

//! @brief SUCC_ITER
//  ITERATOR to traverse all successors of a BB
class SUCC_ITER {
public:
  class ITERATOR : public CGIR_CONTAINER::BB_SUCC_ITER {
  public:
    ITERATOR(BB_PTR bb) : CGIR_CONTAINER::BB_SUCC_ITER(bb) {}
    BB_PTR operator*() const { return Cntr()->Bb(Node_id()); }
    BB_PTR operator->() const { return Cntr()->Bb(Node_id()); }
  };

public:
  SUCC_ITER(BB_PTR bb) : _bb(bb) {}

  // NOLINTBEGIN (readability-identifier-naming)
  ITERATOR begin() const { return ITERATOR(_bb); }
  ITERATOR end() const { return ITERATOR(air::base::Null_ptr); }
  // NOLINTEND (readability-identifier-naming)

private:
  BB_PTR _bb;
};

//! @brief INST_ITER
//  ITERATOR to traverse all INST inside a BB
class INST_ITER {
public:
  enum class ITER_KIND {
    FWD,  // forward iterator
    REV,  // reverse iterator
  };
  //! @brief internal ITERATOR
  template <ITER_KIND kind = ITER_KIND::FWD>
  class ITER_IMPL {
    friend class INST_ITER;

  public:
    INST_PTR operator*() const { return _cur_ptr; }
    INST_PTR operator->() const { return _cur_ptr; }

    ITER_IMPL<kind>& operator++() {
      if (kind == ITER_KIND::FWD) {
        _cur_ptr = _cur_ptr->Next();
      } else {
        AIR_ASSERT(kind == ITER_KIND::REV);
        _cur_ptr = _cur_ptr->Prev();
      }
      return *this;
    }
    ITER_IMPL<kind>& operator--() {
      if (kind == ITER_KIND::FWD) {
        _cur_ptr = _cur_ptr->Prev();
      } else {
        AIR_ASSERT(kind == ITER_KIND::REV);
        _cur_ptr = _cur_ptr->Next();
      }
      return *this;
    }

    bool operator==(const ITER_IMPL<kind>& rhs) {
      return _cur_ptr == rhs._cur_ptr;
    }
    bool operator!=(const ITER_IMPL<kind>& rhs) {
      return _cur_ptr != rhs._cur_ptr;
    }

  private:
    ITER_IMPL(INST_PTR inst) : _cur_ptr(inst) {}
    INST_PTR _cur_ptr;
  };
  using ITERATOR         = ITER_IMPL<ITER_KIND::FWD>;
  using REVERSE_ITERATOR = ITER_IMPL<ITER_KIND::REV>;

public:
  INST_ITER(BB_PTR bb) : _bb(bb) {}

  // NOLINTBEGIN (readability-identifier-naming)
  ITERATOR         begin() const { return ITERATOR(_bb->First()); }
  ITERATOR         end() const { return ITERATOR(INST_PTR()); }
  REVERSE_ITERATOR rbegin() const { return REVERSE_ITERATOR(_bb->Last()); }
  REVERSE_ITERATOR rend() const { return REVERSE_ITERATOR(INST_PTR()); }
  // NOLINTEND (readability-identifier-naming)

private:
  BB_PTR _bb;
};

}  // namespace cg

}  // namespace air

#endif  // AIR_CG_CGIR_UTIL_H
