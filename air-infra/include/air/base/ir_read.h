//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_BASE_IR_READ_H
#define AIR_BASE_IR_READ_H

#include <iostream>
#include <vector>

#include "air/base/meta_info.h"
#include "air/base/node.h"
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
    Recovery(glob->Lit_table(), air::util::SHDR::LIT_TAB);
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
  }

  //! @brief Get phase name from ELF header
  const char* Get_phase() const { return _elf.Get_phase(); }

  //! @brief Get metadata version from ELF header
  uint32_t Get_metadata_version() const { return _elf.Get_metadata_version(); }

  void Handler_func_scope(GLOB_SCOPE* glob) {
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

    BYTE_PTR pos       = t.Recovery(offset);
    uint32_t actual_sz = pos - offset;
    AIR_ASSERT(sz == actual_sz);
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

    // Read num
    uint32_t num = *reinterpret_cast<uint32_t*>(pos);
    pos += sizeof(uint32_t);

    // Read is_root flags
    uint32_t flags_sz = *reinterpret_cast<uint32_t*>(pos);
    pos += sizeof(uint32_t);
    std::vector<uint8_t> is_root_flags(pos, pos + flags_sz);
    pos += flags_sz;

    // Read offsets
    std::vector<uint32_t> offsets(num);
    for (uint32_t i = 0; i < num; i++) {
      offsets[i] = *reinterpret_cast<uint32_t*>(pos);
      pos += sizeof(uint32_t);
    }

    // Read total size
    uint32_t total_sz = *reinterpret_cast<uint32_t*>(pos);
    pos += sizeof(uint32_t);

    // For root nodes (STMT), we need to account for STMT_DATA header
    // STMT_DATA has _prev (4 bytes) + _next (4 bytes) + _data (NODE_DATA)
    const size_t stmt_hdr_sz =
        sizeof(uint32_t) + sizeof(uint32_t);  // _prev + _next

    // Allocate memory for all nodes
    BYTE_PTR data = (BYTE_PTR)code->Malloc(total_sz).Addr();

    // IMPORTANT: Malloc adds one entry to _id_array, but we need num entries.
    // Resize the arrays to num entries before calling Set_item.
    code->Resize(num);

    // Copy node data first
    memcpy(data, pos, total_sz);

    // Set up _id_array and _sz_array
    for (uint32_t i = 0; i < num; i++) {
      BYTE_PTR node_ptr = data + offsets[i];
      uint32_t node_sz  = (i + 1 < num) ? (offsets[i + 1] - offsets[i])
                                        : (total_sz - offsets[i]);

      bool is_root = (is_root_flags[i / 8] & (1 << (i % 8))) != 0;

      if (is_root) {
        // For root nodes, _id_array should point to NODE_DATA (after STMT_DATA
        // header)
        code->Set_item(i, node_ptr + stmt_hdr_sz, node_sz);
      } else {
        code->Set_item(i, node_ptr, node_sz);
      }
    }

    return pos + total_sz;
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
