//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_BASE_IR_READ_H
#define AIR_BASE_IR_READ_H

#include "air/util/binary/elf_info.h"
#include "air/util/binary/elf_read.h"

namespace air {
namespace base {

//! @brief Recovery elf section to AIR
class IR_READ {
  typedef char* BYTE_PTR;

public:
  //! @brief Construct a new b2ir ctx object
  IR_READ(const std::string& ifile, std::ostream& os)
      : _elf(ifile, os), _os(os) {}

  //! @brief Archive Glob table
  void Read_glob(GLOB_SCOPE* glob) {
    Recovery(glob->Str_table(), air::util::SHDR::STR_TAB);
    Recovery(glob->Type_table(), air::util::SHDR::TYPE_TAB);
    Recovery(glob->Arb_table(), air::util::SHDR::ARB_TAB);
    Recovery(glob->Field_table(), air::util::SHDR::FIELD_TAB);
    Recovery(glob->Param_table(), air::util::SHDR::PARAM_TAB);
    Recovery(glob->Main_table(), air::util::SHDR::MAIN_TAB);
    Recovery(glob->Aux_table(), air::util::SHDR::AUX_TAB);
    Recovery(glob->Const_table(), air::util::SHDR::CONS_TTAB);
    Recovery(glob->Attr_table(), air::util::SHDR::ATTR_TAB);
    Recovery(glob->File_table(), air::util::SHDR::FILE_TAB);
    Recovery(glob->Func_def_table(), air::util::SHDR::FUNC_DEF_TAB);
    Recovery(glob->Blk_table(), air::util::SHDR::BLK_TAB);

    Set_func(glob);
  }

  //! @brief Set function undefined status
  void Set_func(GLOB_SCOPE* glob) {
    // Set all functions of this global scope undefined
    FUNC_ITER iter = glob->Begin_func();
    FUNC_ITER end  = glob->End_func();
    for (; iter != end; ++iter) {
      FUNC_PTR func = *iter;
      if (func->Is_defined()) {
        func->Set_undefined();
      }
    }
  }

  void Handler_func_scope(GLOB_SCOPE* glob) {
    // FUNC_SCOPE* func = &glob->New_func_scope((FUNC_ID)0);
    FUNC_SCOPE* func = &glob->New_func_scope((FUNC_ID)0, (FUNC_DEF_ID)0);

    for (GLOB_SCOPE::FUNC_SCOPE_ITER it = glob->Begin_func_scope();
         it != glob->End_func_scope(); ++it) {
      FUNC_SCOPE* func = &(*it);
      func->Print(_os);
    }
  }

  //! @brief Recovery function data
  template <typename S>
  void Read_func(GLOB_SCOPE* glob, S s) {
    BYTE_PTR offset = _elf.Get_pos(s);
    size_t   align  = _elf.Get_addralign(s);
    uint32_t num    = _elf.Get_entsize(s);
    uint32_t sz     = _elf.Get_size(s);
    AIR_ASSERT(offset != nullptr);

    BYTE_PTR pos = offset;
    for (uint32_t i = 0; i < num; ++i) {
      // Get func id & def id
      uint32_t id = *reinterpret_cast<uint32_t*>(pos);
      pos += sizeof(uint32_t);
      uint32_t def_id = *reinterpret_cast<uint32_t*>(pos);
      pos += sizeof(uint32_t);

      // Recovery func scope & func data
      FUNC_SCOPE* func =
          &glob->New_func_scope((FUNC_ID)id, (FUNC_DEF_ID)def_id);
      pos = func->Main_table().Recovery(pos);
      pos = func->Aux_table().Recovery(pos);
      pos = func->Attr_table().Recovery(pos);
      pos = func->Preg_table().Recovery(pos);
      // pos = Code_arena(func, pos);
      pos = Container(func, pos);

      // func->Print(_os);
      // AIR_ASSERT(sz == (pos - offset));
    }
  }

private:
  //! @brief Recovery glob table
  template <typename T, typename S>
  void Recovery(T& t, S s) {
    BYTE_PTR offset = _elf.Get_pos(s);
    size_t   align  = _elf.Get_addralign(s);
    uint32_t num    = _elf.Get_entsize(s);
    uint32_t sz     = _elf.Get_size(s);
    AIR_ASSERT(offset != nullptr);
    AIR_ASSERT(align == t.Align());

    BYTE_PTR pos = t.Recovery(offset);
    AIR_ASSERT(sz == (pos - offset));
  }

  //! @brief Recovery Code_arena
  BYTE_PTR Code_arena(FUNC_SCOPE* func, BYTE_PTR pos) {
    CODE_ARENA* code = func->Container().Code_arena();

    // clear Default Code_arena() data
    uint32_t clear = 0;
    code->Recovery(reinterpret_cast<BYTE_PTR>(&clear));

    uint32_t sz = *reinterpret_cast<uint32_t*>(pos);
    pos += sizeof(uint32_t);
    memcpy((BYTE_PTR)code, pos, sz);
    pos += sz;

    return pos;
  }

  //! @brief Recovery Container data
  BYTE_PTR Container(FUNC_SCOPE* func, BYTE_PTR pos) {
    CONTAINER&  cntr = func->Container();
    CODE_ARENA* code = func->Container().Code_arena();
    // node-Stmt(), first 8byte fill 0xff
    memset((BYTE_PTR)&cntr + sizeof(CONTAINER), 0xff, 0x8);

    // Calculate node data offset position
    uint32_t num    = *reinterpret_cast<uint32_t*>(pos);
    uint32_t len    = sizeof(uint32_t) + num * sizeof(uint32_t);
    uint32_t sz     = *reinterpret_cast<uint32_t*>(pos + len);
    BYTE_PTR offset = pos + len + sizeof(uint32_t);

    // Recovery node address with map address for leave out memcpy
    pos = code->Recovery_offset(offset, pos);
    // memcpy((BYTE_PTR)&cntr + sizeof(CONTAINER), pos, sz);

    // position to next function
    pos += sz + sizeof(uint32_t);

    return pos;
  }

  template <typename S>
  void Print(std::ostream& os, S s) {
    os << "  offset: " << _elf.Get_offset(s) << " size: " << _elf.Get_size(s)
       << std::endl;
  }

  //! @brief Handler string index and number
  uint32_t Handler_str(std::ostream& os, BYTE_PTR offset, uint32_t sz) {
    uint32_t len = 0;
    uint32_t num = 0;

    os << "IR_READ::Handler_str: " << std::endl;
    BYTE_PTR pos = offset;
    while (sz > 0) {
      uint32_t len = strlen(pos) + 1;
      os << "  idx: " << pos - offset << ", str: " << pos << std::endl;
      pos += len;
      sz -= len;
      num++;
    };

    os << "IR_READ::Handler_str num: " << num << std::endl;
    return num;
  }

  //! @brief Check Str_table()
  void Check_str_table(std::ostream& os, GLOB_SCOPE* glob) {
    uint32_t id = 0;
    for (STR_ITER it = glob->Begin_str(); it != glob->End_str(); ++it, ++id) {
      os << " index: " << id << ", str: " << (*it)->Char_str() << std::endl;
    }
  }

private:
  air::util::ELF_READ _elf;
  std::ostream&       _os;
};  // IR_READ

}  // namespace base
}  // namespace air

#endif  // AIR_BASE_IR_READ_H
