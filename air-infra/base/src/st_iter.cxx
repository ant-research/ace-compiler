//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/base/st.h"

namespace air {
namespace base {

//=============================================================================
// class AUX_DATA_ITER member functions
//=============================================================================

AUX_DATA_ITER& AUX_DATA_ITER::operator++() {
  AIR_ASSERT(!_aux.Is_null());
  AIR_ASSERT(_tab);
  _aux = _tab->Aux_entry(_aux->Next());
  return *this;
}

template <typename H>
AUX_DATA_ITER::AUX_DATA_ITER(const H& head) : _tab(&head.Aux_table()) {
  AIR_ASSERT(_tab);
  if (!head.First_aux_entry_id().Is_null()) {
    _aux = _tab->Aux_entry(head.First_aux_entry_id());
  } else {
    _tab = 0;
  }
}

//=============================================================================
// class FIELD_ITER member functions
//=============================================================================

FIELD_ITER::FIELD_ITER(const RECORD_TYPE& r) : _cur() {
  if (!r.First_fld_id().Is_null()) {
    _cur = r.Glob_scope().Field(r.First_fld_id());
  }
}

FIELD_ITER& FIELD_ITER::operator++() {
  AIR_ASSERT(!Is_end());
  _cur = _cur->Next().Is_null() ? FIELD_PTR()
                                : _cur->Glob_scope().Field(_cur->Next());
  return *this;
}

FIELD_PTR
FIELD_ITER::operator*() {
  AIR_ASSERT(!Is_end());
  return _cur;
}

//=============================================================================
// class ATTR_ITER member functions
//=============================================================================
ATTR_ITER& ATTR_ITER::operator++() {
  AIR_ASSERT(!Is_end());
  _cur = (_cur->Next() == Null_id) ? ATTR_PTR()
                                   : _cur->Scope()->Attr(_cur->Next());
  return *this;
}

ATTR_PTR ATTR_ITER::operator*() {
  AIR_ASSERT(!Is_end());
  return _cur;
}

//=============================================================================
// class PARAM_ITER member functions
//=============================================================================

PARAM_ITER::PARAM_ITER(const SIGNATURE_TYPE& sig)
    : _glob(const_cast<GLOB_SCOPE*>(&sig.Glob_scope())),
      _cur(sig.First_param_id()) {}

PARAM_ITER& PARAM_ITER::operator++() {
  AIR_ASSERT(!Is_end())
  AIR_ASSERT(_glob);
  PARAM_PTR param = _glob->Param(Cur_param_id());
  _cur            = param->Next();
  if (Is_end()) _glob = 0;
  return *this;
}

PARAM_PTR
PARAM_ITER::operator*() {
  AIR_ASSERT(!Is_end());
  return _glob->Param(_cur);
}

//=============================================================================
// class STR_ITER member functions
//=============================================================================

STR_ITER::STR_ITER(const GLOB_SCOPE& glob)
    : _scope(&glob), _cur(glob.Str_table().Begin()), _end(false) {
  if (_cur == glob.Str_table().End()) _end = true;
}

STR_ITER& STR_ITER::operator++() {
  if (_end) return *this;

  AIR_ASSERT(_scope);
  if (++_cur == _scope->Str_table().End()) _end = true;
  return *this;
}

STR_PTR
STR_ITER::operator*() const {
  AIR_ASSERT(!_end);
  return _scope->String(STR_ID(*_cur));
}

bool STR_ITER::operator==(const STR_ITER& o) const {
  if (_end && o._end) return true;
  if ((_end && !o._end) || (!_end && o._end)) return false;
  AIR_ASSERT(_scope);
  AIR_ASSERT(_scope == o._scope);
  return (_cur == o._cur);
}

//=============================================================================
// class TYPE_ITER member functions
//=============================================================================

TYPE_ITER::TYPE_ITER(const GLOB_SCOPE& glob)
    : _scope(&glob), _cur(glob.Type_table().Begin()), _end(false) {
  if (_cur == glob.Type_table().End()) _end = true;
}

TYPE_ITER& TYPE_ITER::operator++() {
  if (_end) return *this;
  AIR_ASSERT(_scope);
  ++_cur;
  if (_cur == _scope->Type_table().End()) _end = true;
  return *this;
}

TYPE_PTR
TYPE_ITER::operator*() const {
  if (_end)
    return TYPE_PTR();
  else
    return _scope->Type(TYPE_ID(*_cur));
}

bool TYPE_ITER::operator==(const TYPE_ITER& o) const {
  if (_end && o._end) return true;
  if ((_end && !o._end) || (!_end && o._end)) return false;
  AIR_ASSERT(_scope);
  AIR_ASSERT(_scope == o._scope);
  return (_cur == o._cur);
}

//=============================================================================
// class PREG_ITER member functions
//=============================================================================

PREG_ITER::PREG_ITER(const FUNC_SCOPE& func)
    : _func(&func), _cur(func.Preg_table().Begin()), _end(false) {
  if (_cur == func.Preg_table().End()) _end = true;
}

PREG_ITER& PREG_ITER::operator++() {
  if (_end) return *this;
  AIR_ASSERT(_func);
  ++_cur;
  if (_cur == _func->Preg_table().End()) _end = true;
  return *this;
}

PREG_PTR
PREG_ITER::operator*() const {
  if (_end)
    return PREG_PTR();
  else
    return _func->Preg(PREG_ID(*_cur));
}

bool PREG_ITER::operator==(const PREG_ITER& o) const {
  if (_end && o._end) return true;
  if ((_end && !o._end) || (!_end && o._end)) return false;
  AIR_ASSERT(_func);
  AIR_ASSERT(_func == o._func);
  return (_cur == o._cur);
}

//=============================================================================
// class ARB_ITER member functions
//=============================================================================

ARB_ITER::ARB_ITER(const GLOB_SCOPE& glob)
    : _scope(&glob), _cur(glob.Arb_table().Begin()), _end(false) {
  if (_cur == glob.Arb_table().End()) _end = true;
}

ARB_ITER& ARB_ITER::operator++() {
  if (_end) return *this;
  AIR_ASSERT(_scope);
  ++_cur;
  if (_cur == _scope->Arb_table().End()) _end = true;
  return *this;
}

ARB_PTR ARB_ITER::operator*() const {
  AIR_ASSERT(!_end);
  return _scope->Arb(ARB_ID(*_cur));
}

bool ARB_ITER::operator==(const ARB_ITER& o) const {
  if (_end && o._end) return true;
  if ((_end && !o._end) || (!_end && o._end)) return false;
  AIR_ASSERT(_scope);
  AIR_ASSERT(_scope == o._scope);
  return (_cur == o._cur);
}

//=============================================================================
// class DIM_ITER member functions
//=============================================================================

DIM_ITER::DIM_ITER(const ARRAY_TYPE& arr) : _cur() {
  if (arr.First_dim_id() != Null_id) {
    _cur = arr.Glob_scope().Arb(arr.First_dim_id());
  }
}

DIM_ITER& DIM_ITER::operator++() {
  AIR_ASSERT(!Is_end());
  _cur = (_cur->Next() == Null_id) ? ARB_PTR()
                                   : _cur->Glob_scope().Arb(_cur->Next());
  return *this;
}

ARB_PTR DIM_ITER::operator*() {
  AIR_ASSERT(!Is_end());
  return _cur;
}

//=============================================================================
// class CONSTANT_ITER member functions
//=============================================================================

CONSTANT_ITER::CONSTANT_ITER(const GLOB_SCOPE& glob)
    : _scope(&glob), _cur(glob.Const_table().Begin()), _end(false) {
  while (_cur != glob.Const_table().End()) {
    CONST_CONSTANT_PTR cst  = glob.Constant(CONSTANT_ID(*_cur));
    CONSTANT_KIND      kind = cst->Kind();
    if ((kind != CONSTANT_KIND::STRUCT_FIELD) &&
        (kind != CONSTANT_KIND::ARRAY_ELEM))
      break;
    ++_cur;
  }
  if (_cur == glob.Const_table().End()) _end = true;
}

CONSTANT_ITER& CONSTANT_ITER::operator++() {
  if (_end) return *this;
  AIR_ASSERT(_scope);
  ++_cur;
  while (_cur != _scope->Const_table().End()) {
    CONST_CONSTANT_PTR cst  = _scope->Constant(CONSTANT_ID(*_cur));
    CONSTANT_KIND      kind = cst->Kind();
    if ((kind != CONSTANT_KIND::STRUCT_FIELD) &&
        (kind != CONSTANT_KIND::ARRAY_ELEM))
      break;
    ++_cur;
  }
  if (_cur == _scope->Const_table().End()) _end = true;
  return *this;
}

CONSTANT_PTR
CONSTANT_ITER::operator*() const {
  AIR_ASSERT(!_end);
  return _scope->Constant(CONSTANT_ID(*_cur));
}

bool CONSTANT_ITER::operator==(const CONSTANT_ITER& o) const {
  if (_end && o._end) return true;
  if ((_end && !o._end) || (!_end && o._end)) return false;
  AIR_ASSERT(_scope);
  AIR_ASSERT(_scope == o._scope);
  return (_cur == o._cur);
}

//=============================================================================
// class GLOB_SCOPE::FUNC_SCOPE_ITER member functions
//=============================================================================

GLOB_SCOPE::FUNC_SCOPE_ITER::~FUNC_SCOPE_ITER() {
  if (Is_end()) return;
  if (_prev) {
    // TODO
  }
}

GLOB_SCOPE::FUNC_SCOPE_ITER::FUNC_SCOPE_ITER(const GLOB_SCOPE& glob)
    : _scope(const_cast<GLOB_SCOPE*>(&glob)),
      _prev(0),
      _cur(glob.Begin_func()) {
  AIR_ASSERT(glob.Is_opened());
  Adjust();
}

GLOB_SCOPE::FUNC_SCOPE_ITER::FUNC_SCOPE_ITER(const FUNC_SCOPE_ITER& iter)
    : _scope(iter._scope), _prev(iter._prev), _cur(iter._cur) {
  if (_prev) {
    AIR_ASSERT(_scope);
    // TODO
  }
}

GLOB_SCOPE::FUNC_SCOPE_ITER& GLOB_SCOPE::FUNC_SCOPE_ITER::operator=(
    const FUNC_SCOPE_ITER& o) {
  if (this != &o) {
    if (_prev) {
      // TODO
    }
    _scope = o._scope;
    _prev  = o._prev;
    _cur   = o._cur;
    if (_prev) {
      AIR_ASSERT(_scope);
      // TODO
    }
  }
  return *this;
}

void GLOB_SCOPE::FUNC_SCOPE_ITER::Adjust() {
  AIR_ASSERT(_scope);
  FUNC_ITER func_end = _scope->End_func();
  while (_cur != func_end && !(*_cur)->Is_defined()) ++_cur;

  if (_cur == func_end) {
    if (_prev) {
      // TODO
    }
    _scope = 0;
  }
}

GLOB_SCOPE::FUNC_SCOPE_ITER& GLOB_SCOPE::FUNC_SCOPE_ITER::operator++() {
  if (Is_end()) return *this;
  ++_cur;
  Adjust();
  return *this;
}

FUNC_SCOPE& GLOB_SCOPE::FUNC_SCOPE_ITER::operator*() {
  AIR_ASSERT(_scope);
  if (_prev && _prev->Owning_func_id() == (*_cur)->Id()) return *_prev;
  if (_prev) {
    // TODO
  }

  _prev = &_scope->Open_func_scope((*_cur)->Id());
  AIR_ASSERT(_prev);
  return *_prev;
}

bool GLOB_SCOPE::FUNC_SCOPE_ITER::operator==(const FUNC_SCOPE_ITER& o) const {
  if (Is_end() && o.Is_end()) return true;
  if ((!Is_end() && o.Is_end()) || (Is_end() && !o.Is_end())) return false;
  AIR_ASSERT(_scope);
  AIR_ASSERT(_scope == o._scope);
  return (_cur == o._cur);
}

}  // namespace base
}  // namespace air
