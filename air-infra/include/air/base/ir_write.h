//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_BASE_IR_WRITE_H
#define AIR_BASE_IR_WRITE_H

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

  //! @brief Archive Function data
  template <typename S>
  void Write_func(GLOB_SCOPE* glob, S s) {
    BYTE_PTR offset = _elf.Get_pos();
    AIR_ASSERT(offset != nullptr);
    AIR_ASSERT(glob != nullptr);

    BYTE_PTR pos     = offset;
    size_t   unit_sz = 0;
    size_t   align   = 0;
    uint32_t sz      = 0;

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
    size_t   unit_sz = t.Unit_sz();
    size_t   align   = t.Align();
    BYTE_PTR offset  = _elf.Get_pos();
    AIR_ASSERT(offset != nullptr);
    AIR_ASSERT(unit_sz != 0);
    AIR_ASSERT(align != 0);

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
    uint32_t    sz   = 0;

    // Archive offsest for node and Container size
    pos = code->Archive_offset(pos, &sz);
    sz += 0x40;  // TODO: Length of the last node, not processed for now
    memcpy(pos, reinterpret_cast<BYTE_PTR>(&sz), sizeof(uint32_t));
    pos += sizeof(uint32_t);

    // Archive node data, Entry_node address, drop container first 32byte
    BYTE_PTR data = reinterpret_cast<BYTE_PTR>(&cntr) + sizeof(CONTAINER) + 0x8;

    // memory 16-byte alignment
    _elf.Align_pos(pos, sizeof(MEM_ALIGN));
    memcpy(pos, data, sz);
    pos += sz;

    return pos;
  }

private:
  air::util::ELF_WRITE _elf;
};  // IR_WRITE

}  // namespace base
}  // namespace air

#endif  // AIR_BASE_IR_WRITE_H
