//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CORE_RT_DATA_FILE_H
#define FHE_CORE_RT_DATA_FILE_H

//! @brief rt_data_file.h
//! Common data structure shared by compiler and runtime to read/write
//! data file which contains weight data in either message form or plaintext

//! @brief RT DATA FILE FORMAT
//!   +----------------+
//!   | HEADER         |
//!   |    lut_ofst    +-------\
//!   | padding to 4K  |       |
//!   +----------------+       |
//!   | data entry     |<--\   |
//!   | data entry     |   |   |
//!   +----------------+<--+---/
//!   | LUT entry      |   |
//!   |    ent_ofst    +---/
//!   | LUT entry      |
//!   +----------------+

#include <stdint.h>
#include <string.h>
#include <time.h>

#define DATA_FILE_MAGIC     "!ANTFHE\0"
#define DATA_FILE_PAGE_SIZE 4096

#ifdef __cplusplus
// put into fhe::core namespace for C++ part
namespace fhe {
namespace core {

//! @brief Define all supported external data file entry type. Compiler ONLY
//! DEFINE_DATA_ENTRY_TYPE(enum_id, cmd_line_opts, name_in_C_code)
#define ALL_DATA_ENTRY_TYPES()                            \
  DEFINE_DATA_ENTRY_TYPE(DE_MSG_F32, "f32", "DE_MSG_F32") \
  DEFINE_DATA_ENTRY_TYPE(DE_MSG_F64, "f64", "DE_MSG_F64") \
  DEFINE_DATA_ENTRY_TYPE(DE_PLAINTEXT, "pt", "DE_PLAINTEXT")

//! @brief type for entries in external data file. Compiler ONLY
enum DATA_ENTRY_TYPE : uint32_t {
#define DEFINE_DATA_ENTRY_TYPE(id, opt, name) id,
  ALL_DATA_ENTRY_TYPES()
#undef DEFINE_DATA_ENTRY_TYPE
      DE_ERROR,  //!< Entry type for error detection
};

//! @brief Get enum from command line options. Compiler ONLY
static inline DATA_ENTRY_TYPE Data_entry_type(const char* opt) {
  static const char* names[DE_ERROR] = {
#define DEFINE_DATA_ENTRY_TYPE(id, opt, name) opt,
      ALL_DATA_ENTRY_TYPES()
#undef DEFINE_DATA_ENTRY_TYPE
  };
  if (opt == NULL || opt[0] == '\0') {
    return DE_ERROR;
  }
  for (int i = 0; i < sizeof(names) / sizeof(names[0]); ++i) {
    if (strcmp(opt, names[i]) == 0) {
      return (DATA_ENTRY_TYPE)i;
    }
  }
  return DE_ERROR;
}

//! @brief Get data entry type name to be written to C code. Compiler ONLY
static inline const char* Data_entry_name(DATA_ENTRY_TYPE type) {
  static const char* names[DE_ERROR] = {
#define DEFINE_DATA_ENTRY_TYPE(id, opt, name) name,
      ALL_DATA_ENTRY_TYPES()
#undef DEFINE_DATA_ENTRY_TYPE
  };
  if (type < DE_ERROR) {
    return names[type];
  }
  return NULL;
}
#endif

//! @brief external data file header. For both compiler and rt
struct DATA_FILE_HDR {
  char            _magic[8];   //!< "!ANTFHE\0"
  uint32_t        _rt_ver;     //!< Runtime version: major.minor.patch.build
  uint16_t        _flag;       //!< reserved flags
  uint8_t         _ent_type;   //!< message or plaintext
  uint8_t         _ent_align;  //!< entry alignment, 2^(_entry_align)
  uint64_t        _ent_count;  //!< entry count
  uint64_t        _lut_ofst;   //!< lookup table offset in the file
  struct timespec _ctime;      //!< creation time
  char            _model[48];  //!< uuid to match model and executable file
  char            _uuid[40];   //!< uuid to match model and executable file
};

//! @brief lookup table entry in external data file. For both compiler and rt
struct DATA_LUT_ENTRY {
  char     _name[16];  //!< name of the message/plaintext
  uint32_t _index;     //!< index of the message/plaintext
  uint32_t _size;      //!< size of the data in bytes
  uint64_t _ent_ofst;  //!< offset of the data in file
};

#ifdef __cplusplus
}  // namespace core
}  // namespace fhe
#endif

#endif  // FHE_CORE_RT_DATA_FILE_H
