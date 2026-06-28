//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_BASE_IR_WRITE_H
#define AIR_BASE_IR_WRITE_H

#include <iostream>
#include <vector>

#include "air/base/node_data.h"
#include "air/util/binary/elf_info.h"
#include "air/util/binary/elf_write.h"

namespace air {
namespace base {

//! @brief Archive AIR to elf section
class IR_WRITE {
  typedef char*                   BYTE_PTR;
  typedef std::array<uint8_t, 16> MEM_ALIGN;

public:
  //! @brief Construct a new ir2b ctx object
  IR_WRITE(const std::string& ofile, std::ostream& os) : _elf(ofile, os) {}

  //! @brief Archive Glob table
  void Write_glob(GLOB_SCOPE* glob) {
    Archive(glob->Str_table(), air::util::SHDR::STR_TAB);
    Archive(glob->Lit_table(), air::util::SHDR::LIT_TAB);
    Archive(glob->Type_table(), air::util::SHDR::TYPE_TAB);
    Archive(glob->Arb_table(), air::util::SHDR::ARB_TAB);
    Archive(glob->Field_table(), air::util::SHDR::FIELD_TAB);
    Archive(glob->Param_table(), air::util::SHDR::PARAM_TAB);
    Archive(glob->Main_table(), air::util::SHDR::MAIN_TAB);
    Archive(glob->Aux_table(), air::util::SHDR::AUX_TAB);
    Archive(glob->Const_table(), air::util::SHDR::CONS_TTAB);
    Archive(glob->Attr_table(), air::util::SHDR::ATTR_TAB);
    Archive(glob->File_table(), air::util::SHDR::FILE_TAB);
    Archive(glob->Func_def_table(), air::util::SHDR::FUNC_DEF_TAB);
    Archive(glob->Blk_table(), air::util::SHDR::BLK_TAB);
  }

  //! @brief Set phase name in ELF header
  void Set_phase(const std::string& phase) { _elf.Set_phase(phase); }

  //! @brief Write .comment section with metadata string
  void Write_comment(const std::string& comment) {
    size_t sz = comment.size() + 1;  // include null terminator
    _elf.Ensure_space(sz);

    BYTE_PTR offset = _elf.Get_pos();
    AIR_ASSERT(offset != nullptr);

    memcpy(offset, comment.c_str(), sz);
    _elf.Set_pos(offset + sz);
    _elf.Update_shdr(air::util::SHDR::COMMENT, offset, sz, 1, 0);
  }

  //! @brief Archive Function data
  template <typename S>
  void Write_func(GLOB_SCOPE* glob, S s) {
    AIR_ASSERT(glob != nullptr);

    size_t   unit_sz = 0;
    size_t   align   = 0;
    uint32_t sz      = 0;

    // First pass: estimate total size needed
    size_t estimated_size = 0;
    for (GLOB_SCOPE::FUNC_SCOPE_ITER it = glob->Begin_func_scope();
         it != glob->End_func_scope(); ++it) {
      FUNC_SCOPE* func = &(*it);
      estimated_size += sizeof(uint32_t) * 2;  // func id and def id
      estimated_size +=
          func->Main_table().Size() * func->Main_table().Unit_sz();
      estimated_size += func->Aux_table().Size() * func->Aux_table().Unit_sz();
      estimated_size +=
          func->Attr_table().Size() * func->Attr_table().Unit_sz();
      estimated_size +=
          func->Preg_table().Size() * func->Preg_table().Unit_sz();
      // Estimate container size
      CODE_ARENA* code = func->Container().Code_arena();
      estimated_size += code->Size() * sizeof(uint32_t) * 3 + code->Mem_size();
    }
    _elf.Ensure_space(estimated_size);

    // Get offset AFTER Ensure_space since map address may change
    BYTE_PTR offset = _elf.Get_pos();
    AIR_ASSERT(offset != nullptr);

    BYTE_PTR pos = offset;

    for (GLOB_SCOPE::FUNC_SCOPE_ITER it = glob->Begin_func_scope();
         it != glob->End_func_scope(); ++it, ++sz) {
      FUNC_SCOPE* func = &(*it);
      align            = func->Main_table().Align();
      unit_sz          = func->Main_table().Unit_sz();

      // hander func id and fun def id
      FUNC_ID id = func->Id();
      memcpy(pos, reinterpret_cast<BYTE_PTR>(&id), sizeof(uint32_t));
      pos += sizeof(uint32_t);
      uint32_t def_id = func->Owning_func()->Func_def_data().Id().Value();
      memcpy(pos, reinterpret_cast<BYTE_PTR>(&def_id), sizeof(uint32_t));
      pos += sizeof(uint32_t);

      // hander func table and data
      pos = func->Main_table().Archive(pos);
      pos = func->Aux_table().Archive(pos);
      pos = func->Attr_table().Archive(pos);
      pos = func->Preg_table().Archive(pos);
      // pos = Code_arena(func, pos);
      pos = Container(func, pos);
    }

    AIR_ASSERT(align != 0);
    AIR_ASSERT(unit_sz != 0);

    _elf.Set_pos(pos);
    _elf.Update_shdr(s, offset, pos - offset, align, sz);
  }

private:
  //! @brief Archive glob table
  template <typename T, typename S>
  void Archive(T& t, S s) {
    size_t unit_sz = t.Unit_sz();
    size_t align   = t.Align();
    AIR_ASSERT(unit_sz != 0);
    AIR_ASSERT(align != 0);

    // Calculate actual size needed: num + size_array + data
    // Use Mem_size() for actual data size (handles variable-sized entries)
    size_t num            = t.Size();
    size_t data_size      = t.Mem_size();
    size_t estimated_size = sizeof(uint32_t) +        // num
                            num * sizeof(uint32_t) +  // size array
                            data_size;                // actual data
    _elf.Ensure_space(estimated_size);

    // Get offset AFTER Ensure_space since map address may change
    BYTE_PTR offset = _elf.Get_pos();
    AIR_ASSERT(offset != nullptr);

    BYTE_PTR pos = t.Archive(offset);
    _elf.Set_pos(pos);
    _elf.Update_shdr(s, offset, pos - offset, align, 0);
  }

  //! @brief Archive Code_arena
  BYTE_PTR Code_arena(FUNC_SCOPE* func, BYTE_PTR pos) {
    CODE_ARENA* code = func->Container().Code_arena();
    uint32_t    sz   = func->Container().Code_arena()->Size();

    memcpy(pos, reinterpret_cast<BYTE_PTR>(&sz), sizeof(uint32_t));
    pos += sizeof(uint32_t);
    memcpy(pos, (BYTE_PTR)code, sz);
    pos += sz;

    return pos;
  }

  //! @brief Archive Container data
  BYTE_PTR Container(FUNC_SCOPE* func, BYTE_PTR pos) {
    CONTAINER&  cntr = func->Container();
    CODE_ARENA* code = func->Container().Code_arena();
    uint32_t    num  = code->Size();

    // Write num
    memcpy(pos, reinterpret_cast<BYTE_PTR>(&num), sizeof(uint32_t));
    pos += sizeof(uint32_t);

    // For root nodes (STMT), _id_array points to NODE_DATA but _sz_array
    // includes STMT_DATA header. We need to archive from STMT_DATA start.
    // STMT_DATA has _prev (4 bytes) + _next (4 bytes) + _data (NODE_DATA)
    const size_t stmt_hdr_sz =
        sizeof(uint32_t) + sizeof(uint32_t);  // _prev + _next

    // Write is_root flags as a bitmask (ceil(num/8) bytes)
    std::vector<uint8_t> is_root_flags((num + 7) / 8, 0);
    for (uint32_t i = 0; i < num; i++) {
      BYTE_PTR node_ptr = (BYTE_PTR)code->Find(i);
      NODE_PTR node =
          NODE_PTR(NODE(&cntr, PTR_FROM_DATA<NODE_DATA>(
                                   reinterpret_cast<NODE_DATA*>(node_ptr),
                                   ID<NODE_DATA>(i))));
      if (node->Is_root()) {
        is_root_flags[i / 8] |= (1 << (i % 8));
      }
    }
    uint32_t flags_sz = is_root_flags.size();
    memcpy(pos, reinterpret_cast<BYTE_PTR>(&flags_sz), sizeof(uint32_t));
    pos += sizeof(uint32_t);
    memcpy(pos, is_root_flags.data(), flags_sz);
    pos += flags_sz;

    // Calculate actual sizes and offsets
    std::vector<uint32_t> actual_sizes(num);
    std::vector<BYTE_PTR> archive_ptrs(num);
    uint32_t              offset = 0;
    for (uint32_t i = 0; i < num; i++) {
      memcpy(pos, reinterpret_cast<BYTE_PTR>(&offset), sizeof(uint32_t));
      pos += sizeof(uint32_t);

      BYTE_PTR node_ptr = (BYTE_PTR)code->Find(i);
      uint32_t item_sz  = code->Item_size(i);

      bool is_root = (is_root_flags[i / 8] & (1 << (i % 8))) != 0;

      BYTE_PTR archive_ptr = node_ptr;
      uint32_t archive_sz  = item_sz;

      if (is_root) {
        // For root nodes, archive from STMT_DATA start (before NODE_DATA)
        archive_ptr = node_ptr - stmt_hdr_sz;
      }

      actual_sizes[i] = archive_sz;
      archive_ptrs[i] = archive_ptr;
      offset += archive_sz;
    }
    uint32_t total_sz = offset;

    // Write total size
    memcpy(pos, reinterpret_cast<BYTE_PTR>(&total_sz), sizeof(uint32_t));
    pos += sizeof(uint32_t);

    // Archive each node's data
    for (uint32_t i = 0; i < num; i++) {
      memcpy(pos, archive_ptrs[i], actual_sizes[i]);
      pos += actual_sizes[i];
    }

    return pos;
  }

private:
  air::util::ELF_WRITE _elf;
};  // IR_WRITE

}  // namespace base
}  // namespace air

#endif  // AIR_BASE_IR_WRITE_H
