//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_UTIL_OPTION_H
#define AIR_UTIL_OPTION_H

#include <functional>
#include <string>
#include <vector>

#include "air/util/debug.h"

namespace air {

namespace util {

#define EQUAL_SIGN   '='
#define DASH_SIGN    '-'
#define COMMA_SIGN   ','
#define COLON_SIGN   ':'
#define BFILE_SUFFIX ".B"
#define CFILE_SUFFIX ".c"
#define TFILE_SUFFIX ".t"
#define PFILE_SUFFIX ".json"

uint32_t constexpr INDENT_SPACE = 2;

//! @brief Define the option value kind
typedef enum {
  K_INVALID = 0,
  K_NONE    = 1,  // does not take a value
  K_BOOL    = 2,
  K_INT64   = 3,
  K_UINT64  = 4,
  K_DOUBLE  = 5,
  K_STR     = 6,
  K_LIST    = 7,  // list of option name/value pairs
  K_LAST    = 63
} KIND;

typedef enum {
  V_NONE       = 0,  // not exist
  V_SPACE      = 1,  // space
  V_EQUAL      = 2,
  V_NONE_SPACE = 3  // such as '-I/xx/xx' '-I /xxx/xxx'
} VALUE_MAKER;

//! @brief Define the option descriptor, to be initialized statically
typedef struct {
  const char* Name(void) const { return _name; }
  const char* Abbrev_name(void) const { return _abbrev_name; }
  const char* Description(void) const { return _description; }
  void*       Option_var(void) const { return _option_var; }
  char        Kind(void) const { return _kind; }
  char        Val_maker(void) const { return _val_maker; }
  void        Val_maker(VALUE_MAKER val) { _val_maker = val; }
  char        Pragma_changable(void) const {
    return _pragma_changable;
  }  // used for debug

  const char* _name;
  const char* _abbrev_name;
  const char* _description;
  void*       _option_var;
  KIND        _kind;
  bool        _pragma_changable;
  VALUE_MAKER _val_maker;
} OPTION_DESC;  // end of OPTION_DESC

//! @brief class wraps option descriptor table and its element count
typedef struct {
  uint32_t     Size() const { return _elem_cnt; }
  OPTION_DESC* Option_table() const { return _desc_tab; }
  OPTION_DESC* Option(uint32_t i) const {
    AIR_ASSERT(i < _elem_cnt);
    return _desc_tab + i;
  }

  uint32_t     _elem_cnt;
  OPTION_DESC* _desc_tab;
} OPTION_DESC_HANDLE;

//! @brief Define the option group, to be initialized statically
typedef struct {
  const char*         Name(void) const { return _name; }
  const char*         Description(void) const { return _description; }
  char                Separator(void) const { return _separator; }
  char                Val_maker(void) const { return _val_maker; }
  OPTION_DESC_HANDLE* Options(void) const { return _options; }

  const char* _name;
  const char* _description;
  // the seperator should be restricted in several kinds, now only colon : is
  // supported
  char                _separator;
  VALUE_MAKER         _val_maker;
  OPTION_DESC_HANDLE* _options;
} OPTION_GRP;  // end of OPTION_GRP

//! @brief Define management class for option process
//!        It provides methods to register option descriptor handle,
//!        both the top level and in option groups.
//!        It also provides methods to parse the command line.
class OPTION_MGR {
public:
  OPTION_MGR()
      : _ifile(nullptr),
        _ofile(""),
        _tfile(""),
        _pfile(""),
        _next_option(""),
        _exe_name(nullptr),
        _option_increment(1){};

  ~OPTION_MGR(){};

  //! @brief show the available command line options
  void Print(std::ostream& os, uint32_t indent = 0);

  //! @brief show the available command line options
  void Print();

  //! @brief register option description hanle for top level options
  //! @param desc_handle the pointer of the option descriptor handle to be
  //!  registered
  void Register_top_level_option(OPTION_DESC_HANDLE* desc_handle) {
    _top_level.push_back(desc_handle);
    Top_level_option_rule_checker();
  }

  //! @brief register option descriptions by option group
  //! @param group the pointer of the option group to be registered
  void Register_option_group(OPTION_GRP* group) {
    Group_option_preprocess(group);
    _groups.push_back(group);
    Group_option_rule_checker();
  }

  //! @brief Parse options in command line, top level and option groups
  //! @param argc the number of commandline arguments
  //! @param options the commandline argument lists passed into main
  //! @return R_CODE during parse process
  R_CODE Parse_options(int argc, char** options);

  //! @brief get top level options
  //! @return vector of top level opion descriptor handler
  std::vector<OPTION_DESC_HANDLE*> Top_level_option() { return _top_level; }

  //! @brief get group options
  //! @return vectors of group opion
  std::vector<OPTION_GRP*> Group_option() { return _groups; }

  //! @brief get input file name
  //! @return input file name
  const char* Ifile() const { return _ifile; }

  //! @brief get ouput file name
  //! @return ouput file name
  const char* Ofile() const { return _ofile.c_str(); }

  //! @brief get trace file name
  //! @return trace file name
  const char* Tfile() const { return _tfile.c_str(); }

  //! @brief get perf file name
  //! @return perf file name
  const char* Pfile() const { return _pfile.c_str(); }

  //! @brief get executable program name
  const char* Exe_name() const { return _exe_name; }

private:
  OPTION_MGR(const OPTION_MGR&);  // REQUIRED UNDEFINED UNWANTED methods
  OPTION_MGR& operator=(
      const OPTION_MGR&);  // REQUIRED UNDEFINED UNWANTED methods

  //! @brief get integer from string
  //! @param str_val the string object
  //! @param convert_func which func will be used to convert the string object
  //! @return pair object. The first element is the target value, the second
  //!  element is bool type to indicate whether this convert is successful. If
  //!  the second element is false, then the first element is undefined.
  template <typename T>
  std::pair<T, bool> String_to_integer(
      const char* str_val,
      std::function<T(const char* __str, char** __endptr, int __base)>
          convert_func);
  //! @brief get command line option increment
  int Get_option_increment() { return _option_increment; }

  //! @brief set command line option increment value
  //! @return the increment to next option
  void Set_option_increment(int value) {
    AIR_ASSERT((value == 1) || (value == 2));
    _option_increment = value;
  }

  //! @brief get next option
  //! @return next option
  const char* Get_next_option() const { return _next_option; }

  //! @brief set the value of next option
  void Set_next_option(const char* op) { _next_option = op; }

  //! @brief get the option value from command line option
  //! @param cml_option the command line option
  //! @param op_desc pointer to option desc type object
  //! @param match_with_name if this command line option matched with option
  //!  name, its value is true
  //! @return the value of the option
  std::string Get_option_value(const std::string& cml_option,
                               OPTION_DESC* op_desc, bool match_with_name);

  //! @brief check whether option group name from command line is exists in
  //!  expected option group name list
  //! @return if matched return option group, otherwise nullptr
  OPTION_GRP* Group_name_match(std::string_view cml_group_name);

  //! @brief check whether option from command line exists in expected option
  //!  list, if find then update its value
  //! @return if matched and update sucess, return true
  bool Option_match_and_update(
      std::string                       cml_option,
      std::vector<OPTION_DESC_HANDLE*>& builtin_options);

  //! @brief check whether option from command line exists in expected option
  //!  list, if find then update its value
  //! @return if matched and update sucess, return true
  bool Option_match_and_update(std::string cml_option, OPTION_GRP* option_grp);

  //! @brief Parse options for an option groups
  int Parse_option_group(char* option_group);

  //! @brief check whether the registered top level options are valid, such as
  //!  duplicate option name
  void Top_level_option_rule_checker();

  //! @brief preprocess group option, such as override the value maker of option
  //!  desc
  void Group_option_preprocess(OPTION_GRP* group);

  //! @brief check whether the registered group options are valid, used after
  //!  register
  void Group_option_rule_checker();

  std::vector<OPTION_GRP*>         _groups;     // all registered option groups
  std::vector<OPTION_DESC_HANDLE*> _top_level;  // top level option handle
  const char*                      _ifile;      // input file
  std::string                      _ofile;      // output file
  std::string                      _tfile;      // trace file
  std::string                      _pfile;      // perf file
  const char*                      _next_option;  // next command line option
  const char*                      _exe_name;     // executable program name
  int _option_increment;  // command line option increment, only support 1 or 2
};                        // OPTION_MGR

}  // namespace util

}  // namespace air

#endif  // AIR_UTIL_OPTION_H
