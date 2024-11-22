//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_BASE_ST_ITER_H
#define AIR_BASE_ST_ITER_H

#include "air/base/st_decl.h"
#include "air/base/st_sym.h"
#include "air/base/st_trait.h"

namespace air {
namespace base {

template <typename V, typename B>
class TAB_ITER_BASE {
public:
  typedef TAB_ITER_BASE<V, B> SELF_TYPE;
  typedef B                   BASE_ID_TYPE;
  typedef typename V::ID_TYPE ID_TYPE;

  TAB_ITER_BASE() : _scope(0), _cur() {}

  SELF_TYPE& operator++() {
    AIR_ASSERT(_scope);
    ++_cur;
    return *this;
  }

  bool operator==(const SELF_TYPE& iter) const { return _cur == iter._cur; }
  bool operator!=(const SELF_TYPE& iter) const { return _cur != iter._cur; }
  operator bool() const { return !_cur.Is_null(); }
  bool Is_null() const { return _cur.Is_null(); }

protected:
  TAB_ITER_BASE(const GLOB_SCOPE& glob)
      : _scope(const_cast<GLOB_SCOPE*>(&glob)),
        _cur(glob.Begin(BASE_ID_TYPE())) {}
  TAB_ITER_BASE(const FUNC_SCOPE& func)
      : _scope(const_cast<FUNC_SCOPE*>(&func)),
        _cur(func.Begin(BASE_ID_TYPE())) {}

  V operator*() {
    AIR_ASSERT(_scope);
    typedef typename V::BASE_TYPE BASE_VAL_TYPE;
    BASE_VAL_TYPE                 val;
    uint32_t                      level = _scope->Scope_level();

    uint32_t prim_id = *_cur;
    AIR_ASSERT(prim_id != Null_st_id);
    if (_scope->Is_glob()) {
      val = BASE_VAL_TYPE(static_cast<GLOB_SCOPE&>(*_scope),
                          ID_TYPE(prim_id, level));
    } else {
      val = BASE_VAL_TYPE(static_cast<FUNC_SCOPE&>(*_scope),
                          ID_TYPE(prim_id, level));
    }
    return static_cast<const V&>(val);
  }

  typedef PRIM_ID_ITER ITER_TYPE;

  ITER_TYPE   _cur;
  SCOPE_BASE* _scope;
};

template <class V, class B, class S>
class TAB_ITER : public TAB_ITER_BASE<V, B> {
public:
  typedef TAB_ITER_BASE<V, B> BASE_TYPE;
  typedef TAB_ITER<V, B, S>   SELF_TYPE;
  typedef B                   BASE_ID_TYPE;
  typedef typename V::ID_TYPE ID_TYPE;
  typedef PTR<V>              POINTER_TYPE;

  TAB_ITER() : BASE_TYPE() {}
  TAB_ITER(const GLOB_SCOPE& glob, const S& sel) : BASE_TYPE(glob), _sel(sel) {
    this->Adjust();
    AIR_ASSERT(BASE_TYPE::Is_null() || _sel(BASE_TYPE::operator*()));
  }
  TAB_ITER(const FUNC_SCOPE& func, const S& sel) : BASE_TYPE(func), _sel(sel) {
    this->Adjust();
    AIR_ASSERT(BASE_TYPE::Is_null() || _sel(BASE_TYPE::operator*()));
  }

  SELF_TYPE& operator++() {
    BASE_TYPE::operator++();
    this->Adjust();
    AIR_ASSERT(BASE_TYPE::Is_null() || _sel(BASE_TYPE::operator*()));
    return *this;
  }

  POINTER_TYPE operator*() { return POINTER_TYPE(BASE_TYPE::operator*()); }

private:
  void Adjust() {
    SELF_TYPE& self = *this;
    while (!self.Is_null() && !_sel(BASE_TYPE::operator*())) {
      BASE_TYPE::operator++();
    }
  }
  S _sel;
};

template <SYMBOL_CLASS T>
class SYM_SEL {
public:
  typedef typename SYM_TYPE_TRAITS<T>::SYM_TYPE SYM_TYPE;
  bool operator()(const SYM_TYPE& sym) const { return (T == sym.Kind()); }
};

class ADDR_DATUM_SEL {
public:
  typedef ADDR_DATUM SYM_TYPE;
  bool operator()(const SYM_TYPE& sym) const { return (sym.Is_addr_datum()); }
};

class FILE_ITER {
  friend class GLOB_SCOPE;

public:
  FILE_ITER() : _scope(0), _end(true) {}

  FILE_ITER& operator++();
  FILE_PTR   operator*() const;
  bool       operator==(const FILE_ITER&) const;
  bool       operator!=(FILE_ITER& iter) const { return !operator==(iter); }

protected:
  FILE_ITER(const GLOB_SCOPE& glob);

  typedef PRIM_ID_ITER ITER_TYPE;

  const GLOB_SCOPE* _scope;
  ITER_TYPE         _cur;
  bool              _end;
};

class AUX_DATA_ITER {
  friend class SYM;
  friend class ENTRY;
  friend class AUX_TAB;

public:
  AUX_DATA_ITER() : _aux() {}

protected:
  AUX_DATA_PTR Aux_data_ptr() const { return _aux; }

  AUX_DATA_ITER& operator++();
  AUX_DATA_PTR&  operator*() { return _aux; }
  bool           operator==(const AUX_DATA_ITER& o) const {
    return _aux == o.Aux_data_ptr();
  }
  bool operator!=(const AUX_DATA_ITER& o) const { return !operator==(o); }

  template <typename H>
  AUX_DATA_ITER(const H& head);

  AUX_DATA_ITER(AUX_DATA_PTR aux, AUX_TAB* tab) : _tab(tab), _aux(aux) {
    if (!aux) _tab = 0;
  }

  AUX_DATA_PTR _aux;
  AUX_TAB*     _tab;
};

class PARAM_ITER {
  friend class SIGNATURE_TYPE;

public:
  PARAM_ITER() : _glob(0), _cur(Null_st_id) {}

  PARAM_ITER& operator++();
  PARAM_PTR   operator*();
  bool operator==(const PARAM_ITER& iter) const { return (_cur == iter._cur); }
  bool operator!=(const PARAM_ITER& iter) const { return !operator==(iter); }

  PARAM_ID Cur_param_id() const { return _cur; }

protected:
  PARAM_ITER(const SIGNATURE_TYPE& sig);

  bool Is_end() const { return _cur.Is_null(); }

  PARAM_ID    _cur;
  GLOB_SCOPE* _glob;
};

class STR_ITER {
public:
  STR_ITER() : _scope(0), _end(true) {}
  STR_ITER(const GLOB_SCOPE& glob);

  STR_ITER& operator++();
  STR_PTR   operator*() const;
  bool      operator==(const STR_ITER&) const;
  bool      operator!=(const STR_ITER& iter) const { return !operator==(iter); }

protected:
  typedef PRIM_ID_ITER ITER_TYPE;

  const GLOB_SCOPE* _scope;
  ITER_TYPE         _cur;
  bool              _end;
};

class TYPE_ITER {
  friend class GLOB_SCOPE;

public:
  typedef TYPE_PTR VAL_TYPE;

  TYPE_ITER() : _scope(0), _end(true) {}

  TYPE_ITER& operator++();
  VAL_TYPE   operator*() const;
  bool       operator==(const TYPE_ITER& o) const;
  bool operator!=(const TYPE_ITER& iter) const { return !operator==(iter); }

protected:
  TYPE_ITER(const GLOB_SCOPE& glob);

  typedef PRIM_ID_ITER ITER_TYPE;

  const GLOB_SCOPE* _scope;
  ITER_TYPE         _cur;
  bool              _end;
};

class PREG_ITER {
  friend class FUNC_SCOPE;

public:
  typedef PREG_PTR VAL_TYPE;

  PREG_ITER() : _func(0), _end(true) {}

  PREG_ITER& operator++();
  VAL_TYPE   operator*() const;
  bool       operator==(const PREG_ITER& o) const;
  bool       operator!=(const PREG_ITER& o) const { return !operator==(o); }

protected:
  PREG_ITER(const FUNC_SCOPE& func);

  typedef PRIM_ID_ITER ITER_TYPE;

  const FUNC_SCOPE* _func;
  ITER_TYPE         _cur;
  bool              _end;
};

class FIELD_ITER {
  friend class RECORD_TYPE;

public:
  FIELD_ITER() : _cur() {}

  FIELD_ITER& operator++();
  FIELD_PTR   operator*();
  bool operator==(const FIELD_ITER& iter) const { return (_cur == iter._cur); }
  bool operator!=(const FIELD_ITER& iter) const { return !operator==(iter); }

private:
  FIELD_ITER(const RECORD_TYPE& r);

  bool Is_end() const { return _cur == Null_ptr; }

  FIELD_PTR _cur;
};

class ATTR_ITER {
  friend class ATTR_LIST;

public:
  ATTR_ITER() : _cur() {}

  ATTR_ITER& operator++();
  ATTR_PTR   operator*();
  bool operator==(const ATTR_ITER& iter) const { return _cur == iter._cur; }
  bool operator!=(const ATTR_ITER& iter) const { return !operator==(iter); }

private:
  ATTR_ITER(ATTR_PTR attr) : _cur(attr) {}

  bool Is_end() const { return _cur == Null_ptr; }

  ATTR_PTR _cur;
};

/**
 * @brief ARB_TAB entry iterator
 *
 */
class ARB_ITER {
public:
  ARB_ITER() : _scope(0), _end(true) {}
  ARB_ITER(const GLOB_SCOPE& glob);

  ARB_ITER& operator++();
  ARB_PTR   operator*() const;
  bool      operator==(const ARB_ITER&) const;
  bool      operator!=(const ARB_ITER& iter) const { return !operator==(iter); }

protected:
  typedef PRIM_ID_ITER ITER_TYPE;

  const GLOB_SCOPE* _scope;
  ITER_TYPE         _cur;
  bool              _end;
};

/**
 * @brief ARRAY_TYPE demension iterator
 *
 */
class DIM_ITER {
  friend class ARRAY_TYPE;

public:
  DIM_ITER() : _cur() {}

  DIM_ITER& operator++();
  ARB_PTR   operator*();
  bool operator==(const DIM_ITER& iter) const { return (_cur == iter._cur); }
  bool operator!=(const DIM_ITER& iter) const { return !operator==(iter); }

private:
  DIM_ITER(const ARRAY_TYPE& arr);

  bool Is_end() const { return (_cur == Null_ptr); }

  ARB_PTR _cur;
};

class CONSTANT_ITER {
public:
  CONSTANT_ITER() : _scope(0), _end(true) {}
  CONSTANT_ITER(const GLOB_SCOPE& glob);

  CONSTANT_ITER& operator++();
  CONSTANT_PTR   operator*() const;
  bool           operator==(const CONSTANT_ITER&) const;
  bool operator!=(const CONSTANT_ITER& iter) const { return !operator==(iter); }

protected:
  typedef PRIM_ID_ITER ITER_TYPE;

  const GLOB_SCOPE* _scope;
  ITER_TYPE         _cur;
  bool              _end;
};

class GLOB_SCOPE::FUNC_SCOPE_ITER {
  friend class GLOB_SCOPE;

public:
  ~FUNC_SCOPE_ITER();

  FUNC_SCOPE_ITER& operator++();
  FUNC_SCOPE&      operator*();
  bool             operator==(const FUNC_SCOPE_ITER&) const;
  bool             operator!=(const FUNC_SCOPE_ITER& iter) const {
    return !operator==(iter);
  }

  FUNC_SCOPE_ITER(const FUNC_SCOPE_ITER&);
  FUNC_SCOPE_ITER& operator=(const FUNC_SCOPE_ITER&);

private:
  FUNC_SCOPE_ITER(const GLOB_SCOPE& glob);
  FUNC_SCOPE_ITER() : _scope(0) {}

  void Adjust();
  bool Is_end() const { return _scope == 0; }

  FUNC_SCOPE* _prev;
  FUNC_ITER   _cur;
  GLOB_SCOPE* _scope;
};

}  // namespace base
}  // namespace air

#endif
