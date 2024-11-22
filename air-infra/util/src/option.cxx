//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/util/option.h"

#include <filesystem>
#include <iostream>
#include <unordered_set>

using namespace std;

namespace air {
namespace util {

void OPTION_MGR::Print(ostream& os, uint32_t indent) {
  os << "Usage: " << Exe_name() << " [options] file..." << endl;

  vector<OPTION_DESC_HANDLE*> od_handles = this->Top_level_option();
  AIR_ASSERT(od_handles.size() != 0);

  os << endl << "OPTIONS:" << endl;
  string value_prompt = " ";
  string display_info;
  for (auto od_handle : od_handles) {
    for (auto i = 0; i < od_handle->Size(); i++) {
      string name  = od_handle->Option(i)->Name();
      string aname = od_handle->Option(i)->Abbrev_name();
      if (!name.empty()) {
        display_info = DASH_SIGN + string(od_handle->Option(i)->Name());
      }
      if (!aname.empty()) {
        if (!display_info.empty()) {
          display_info += COMMA_SIGN;
        }
        display_info += (DASH_SIGN + aname);
      }

      value_prompt = " ";
      // TODO: need to process other kind option
      switch (od_handle->Option(i)->Kind()) {
        case K_STR:
        case K_INT64:
        case K_UINT64:
        case K_DOUBLE:
          value_prompt = "<value>";
          break;
        default:
          break;
      }

      switch (od_handle->Option(i)->Val_maker()) {
        case V_EQUAL:
          display_info += EQUAL_SIGN;
          display_info += value_prompt;
          break;
        case V_SPACE:
          display_info += ' ';
          display_info += value_prompt;
          break;
        case V_NONE_SPACE: {
          string null_case  = display_info + value_prompt;
          string space_case = display_info + ' ' + value_prompt;
          display_info      = null_case + "(or " + space_case + ")";
          break;
        }
        default:
          break;
      }
      os << string(indent * INDENT_SPACE, ' ') << left << setw(30)
         << display_info << od_handle->Option(i)->Description() << endl;
    }
  }

  value_prompt                    = "<value>";
  vector<OPTION_GRP*> grp_options = this->Group_option();
  for (auto grp_option : grp_options) {
    OPTION_DESC_HANDLE* od_handle = grp_option->Options();

    string group_prefix = "";
    os << endl << grp_option->Name() << " group OPTIONS: " << endl;
    os << "(currently when using mutiple group options, only -"
       << grp_option->Name() << grp_option->Separator() << "option_name1"
       << grp_option->Separator() << "option_name2="
       << "<value> format is supported. Below is the supported option "
          "names:)"
       << endl;

    for (auto i = 0; i < od_handle->Size(); i++) {
      string name  = od_handle->Option(i)->Name();
      string aname = od_handle->Option(i)->Abbrev_name();
      if (!name.empty()) {
        display_info = name;
        if (od_handle->Option(i)->Val_maker() == V_EQUAL) {
          display_info = name + EQUAL_SIGN + value_prompt;
        }
      }
      if (!aname.empty()) {
        string abbrev_case = aname;
        if (od_handle->Option(i)->Val_maker() == V_EQUAL) {
          abbrev_case += EQUAL_SIGN + value_prompt;
        }
        if (!display_info.empty()) {
          display_info += "(" + abbrev_case + ")";
        } else {
          display_info += abbrev_case;
        }
      }
      os << string(indent * INDENT_SPACE, ' ') << left << setw(30)
         << display_info << od_handle->Option(i)->Description() << endl;
    }
  }
  os << endl;
}

void OPTION_MGR::Print() { Print(cout, 1); }

void OPTION_MGR::Top_level_option_rule_checker() {
  vector<OPTION_DESC_HANDLE*> option_descs = Top_level_option();
  unordered_set<string>       name_set;
  for (auto option_desc : option_descs) {
    for (int i = 0; i < option_desc->Size(); i++) {
      string op_name        = option_desc->Option(i)->Name();
      string abbrev_op_name = option_desc->Option(i)->Abbrev_name();
      if (name_set.find(op_name) == name_set.end()) {
        name_set.insert(op_name);
      } else {
        AIR_DEBUG("option name %s is duplicated", op_name.c_str());
        CMPLR_USR_MSG(U_CODE::Incorrect_Option, op_name.c_str());
        exit(1);
      }

      if (!abbrev_op_name.empty()) {
        if (name_set.find(abbrev_op_name) == name_set.end()) {
          name_set.insert(abbrev_op_name);
        } else {
          AIR_DEBUG("abbreviated option name %s is duplicated",
                    abbrev_op_name.c_str());
          CMPLR_USR_MSG(U_CODE::Incorrect_Option, abbrev_op_name.c_str());
          exit(1);
        }
      }
    }
  }
}

void OPTION_MGR::Group_option_preprocess(OPTION_GRP* group) {
  // only support colon : as group option separator
  // and only support equal sign = as the value maker in group option till now
  AIR_ASSERT_MSG(group->Separator() == COLON_SIGN,
                 "error: only colon : as group option separator is supported");
  AIR_ASSERT_MSG(
      group->Val_maker() == V_EQUAL,
      "error: only equal sign = as group option value maker is supported");
  OPTION_DESC_HANDLE* group_option = group->Options();
  for (int i = 0; i < group_option->Size(); i++) {
    if (group_option->Option(i)->Val_maker() != V_NONE) {
      group_option->Option(i)->Val_maker(V_EQUAL);
    }
  }
}

void OPTION_MGR::Group_option_rule_checker() {
  vector<OPTION_GRP*>   option_grps = Group_option();
  unordered_set<string> name_set;
  for (auto option_group : option_grps) {
    string grp_name = option_group->Name();
    if (name_set.find(grp_name) == name_set.end()) {
      name_set.insert(grp_name);
    } else {
      AIR_DEBUG("group name %s is duplicated", grp_name.c_str());
      CMPLR_USR_MSG(U_CODE::Incorrect_Option, grp_name.c_str());
      exit(1);
    }
  }
}

template <typename T>
pair<T, bool> OPTION_MGR::String_to_integer(
    const char*                                                 str_val,
    function<T(const char* __str, char** __endptr, int __base)> convert_func) {
  int base = 10;
  if ((strncmp(str_val, "0x", 2) == 0) || (strncmp(str_val, "0X", 2) == 0)) {
    base = 16;
  }
  pair<T, bool> result;
  result.second = false;
  errno         = 0;
  char* p_end{};
  T     value = convert_func(str_val, &p_end, base);
  if (str_val == p_end) {
    AIR_DEBUG("%s is invalid number", str_val);
  } else if (errno == ERANGE) {
    AIR_DEBUG("%s is out of range", str_val);
  } else if (*p_end == '\0') {
    result.second = true;
  }
  result.first = value;
  return result;
}

string OPTION_MGR::Get_option_value(const string& cml_option,
                                    OPTION_DESC*  op_desc,
                                    bool          match_with_name) {
  // string_view cml_option_name;
  string cml_option_value;
  switch (op_desc->Val_maker()) {
    case V_NONE: {
      size_t name_size = 0;
      if (match_with_name) {
        name_size = strlen(op_desc->Name());
      } else {
        name_size = strlen(op_desc->Abbrev_name());
      }
      // cml_option_name = cml_option.substr(0, name_size);
      cml_option_value =
          cml_option.substr(name_size, cml_option.size() - name_size);
      break;
    }
    case V_SPACE: {
      Set_option_increment(2);
      cml_option_value = Get_next_option();
      break;
    }
    case V_EQUAL: {
      size_t sep_pos = cml_option.find(EQUAL_SIGN);
      if (sep_pos != string::npos) {
        // cml_option_name = cml_option.substr(0, sep_pos);
        cml_option_value =
            cml_option.substr(sep_pos + 1, cml_option.size() - sep_pos);
      }
      break;
    }
    case V_NONE_SPACE: {
      string next_option = Get_next_option();
      // when -I/xxx/xxx is the last option, next_option is empty
      if (next_option.empty() ||
          (!next_option.empty() && (next_option.rfind(DASH_SIGN, 0) == 0))) {
        AIR_DEBUG("no separator between option and value");
        size_t name_size = 0;
        if (match_with_name) {
          name_size = strlen(op_desc->Name());
        } else {
          name_size = strlen(op_desc->Abbrev_name());
        }
        // cml_option_name = cml_option.substr(0, name_size);
        cml_option_value =
            cml_option.substr(name_size, cml_option.size() - name_size);
      } else {
        AIR_DEBUG("has space separator between option and value");
        Set_option_increment(2);
        cml_option_value = Get_next_option();
      }
      break;
    }
    default:
      AIR_ASSERT_MSG(false, "unsupported separator in option: ", cml_option);
  }
  return cml_option_value;
}

// used for parse top level option
bool OPTION_MGR::Option_match_and_update(
    string cml_option, vector<OPTION_DESC_HANDLE*>& builtin_options) {
  // the most long matched option should be the real option
  // complete match has highest priority
  OPTION_DESC* matched_op_desc = nullptr;
  string       matched_name;
  bool         complete_match  = false;
  bool         match_with_name = false;
  for (auto builtin_option : builtin_options) {
    for (int i = 0; i < builtin_option->Size(); i++) {
      string_view option_name  = builtin_option->Option(i)->Name();
      string_view option_aname = builtin_option->Option(i)->Abbrev_name();

      // if match completely, then jump out the loop directly
      if ((cml_option == option_name) || (cml_option == option_aname)) {
        matched_op_desc = builtin_option->Option(i);
        complete_match  = true;
        if (cml_option == option_name) {
          match_with_name = true;
        }
        break;
      }

      // TODO: need to consider more complex situation
      if (builtin_option->Option(i)->Kind() != K_NONE) {
        if (!option_name.empty() && (cml_option.rfind(option_name, 0) == 0)) {
          match_with_name = true;
          if (option_name.size() > matched_name.size()) {
            matched_name    = option_name;
            matched_op_desc = builtin_option->Option(i);
          }
        } else if (!option_aname.empty() &&
                   (cml_option.rfind(option_aname, 0) == 0)) {
          if (option_aname.size() > matched_name.size()) {
            matched_name    = option_aname;
            matched_op_desc = builtin_option->Option(i);
          }
        }
      }
    }
    if (complete_match) {
      break;
    }
  }

  if (matched_op_desc) {
    switch (matched_op_desc->Kind()) {
      case K_NONE: {
        AIR_ASSERT(
            (strcmp(cml_option.c_str(), matched_op_desc->Name()) == 0) ||
            (strcmp(cml_option.c_str(), matched_op_desc->Abbrev_name()) == 0));
        *((bool*)matched_op_desc->_option_var) = true;
        break;
      }
      case K_STR: {
        string cml_option_value =
            Get_option_value(cml_option, matched_op_desc, match_with_name);
        if (cml_option_value.empty()) {
          AIR_DEBUG("%s option missing value", cml_option);
          CMPLR_USR_MSG(U_CODE::Incorrect_Option, cml_option.c_str());
          return false;
        }
        *((string*)matched_op_desc->_option_var) = cml_option_value;
        break;
      }
      case K_INT64: {
        string cml_option_value =
            Get_option_value(cml_option, matched_op_desc, match_with_name);
        if (cml_option_value.empty()) {
          AIR_DEBUG("%s option missing value", cml_option);
          CMPLR_USR_MSG(U_CODE::Incorrect_Option, cml_option.c_str());
          return false;
        }

        pair<int64_t, bool> result =
            String_to_integer<int64_t>(cml_option_value.c_str(), &strtoll);
        if (!result.second) {
          CMPLR_USR_MSG(U_CODE::Incorrect_Option, cml_option.c_str());
          return false;
        }
        *((int64_t*)matched_op_desc->_option_var) = result.first;
        break;
      }
      case K_UINT64: {
        string cml_option_value =
            Get_option_value(cml_option, matched_op_desc, match_with_name);
        if (cml_option_value.empty()) {
          AIR_DEBUG("%s option missing value", cml_option);
          CMPLR_USR_MSG(U_CODE::Incorrect_Option, cml_option.c_str());
          return false;
        }

        pair<uint64_t, bool> result =
            String_to_integer<uint64_t>(cml_option_value.c_str(), &strtoull);
        if (!result.second) {
          CMPLR_USR_MSG(U_CODE::Incorrect_Option, cml_option.c_str());
          return false;
        }
        *((uint64_t*)matched_op_desc->_option_var) = result.first;
        break;
      }
      case K_DOUBLE: {
        string cml_option_value =
            Get_option_value(cml_option, matched_op_desc, match_with_name);
        if (cml_option_value.empty()) {
          AIR_DEBUG("%s option missing value", cml_option);
          CMPLR_USR_MSG(U_CODE::Incorrect_Option, cml_option.c_str());
          return false;
        }

        const char* p_start = cml_option_value.c_str();
        char*       p_end   = nullptr;
        double      result  = strtod(p_start, &p_end);
        if (errno == ERANGE || (p_end != p_start + cml_option_value.size())) {
          CMPLR_USR_MSG(U_CODE::Incorrect_Option, cml_option.c_str());
          return false;
        }
        *((double*)matched_op_desc->_option_var) = result;
        break;
      }
      default:
        AIR_ASSERT_MSG(false, "not implement option kind: %s", cml_option);
    }
    return true;
  }
  return false;
}

// used for process option group
bool OPTION_MGR::Option_match_and_update(string      cml_option,
                                         OPTION_GRP* option_grp) {
  string cml_option_name = cml_option;
  string cml_option_value;
  int    equal_pos = cml_option.find(EQUAL_SIGN);
  if (equal_pos != string::npos) {
    cml_option_name = cml_option.substr(0, equal_pos);
    cml_option_value =
        cml_option.substr(equal_pos + 1, cml_option.size() - equal_pos);
  }

  OPTION_DESC_HANDLE* builtin_option = option_grp->Options();
  for (int i = 0; i < builtin_option->Size(); i++) {
    OPTION_DESC* option_tbl   = builtin_option->Option_table();
    string_view  option_name  = (option_tbl + i)->Name();
    string_view  option_aname = (option_tbl + i)->Abbrev_name();

    if ((cml_option_name == option_name) or (cml_option_name == option_aname)) {
      switch ((option_tbl + i)->Kind()) {
        case K_NONE:
          AIR_ASSERT(cml_option_value.empty());
          AIR_ASSERT(builtin_option->Option(i)->Val_maker() == V_NONE);
          *((bool*)(option_tbl + i)->_option_var) = true;
          break;
        case K_STR:
          AIR_ASSERT(!cml_option_value.empty());
          AIR_ASSERT(builtin_option->Option(i)->Val_maker() == V_EQUAL);
          *((string*)(option_tbl + i)->_option_var) = cml_option_value;
          break;
        case K_INT64: {
          AIR_ASSERT(!cml_option_value.empty());
          AIR_ASSERT(builtin_option->Option(i)->Val_maker() == V_EQUAL);
          pair<int64_t, bool> result =
              String_to_integer<int64_t>(cml_option_value.c_str(), &strtoll);
          if (!result.second) {
            CMPLR_USR_MSG(U_CODE::Incorrect_Option, cml_option_name.c_str());
            return false;
          }
          *((int64_t*)(option_tbl + i)->_option_var) = result.first;
          break;
        }
        case K_UINT64: {
          AIR_ASSERT(!cml_option_value.empty());
          AIR_ASSERT(builtin_option->Option(i)->Val_maker() == V_EQUAL);
          pair<uint64_t, bool> result =
              String_to_integer<uint64_t>(cml_option_value.c_str(), &strtoull);
          if (!result.second) {
            CMPLR_USR_MSG(U_CODE::Incorrect_Option, cml_option_name.c_str());
            return false;
          }
          *((uint64_t*)(option_tbl + i)->_option_var) = result.first;
          break;
        }
        case K_DOUBLE: {
          AIR_ASSERT(!cml_option_value.empty());
          AIR_ASSERT(builtin_option->Option(i)->Val_maker() == V_EQUAL);
          const char* p_start = cml_option_value.c_str();
          char*       p_end   = nullptr;
          double      result  = strtod(p_start, &p_end);
          if (errno == ERANGE || (p_end != p_start + cml_option_value.size())) {
            CMPLR_USR_MSG(U_CODE::Incorrect_Option, cml_option_name.c_str());
            return false;
          }
          *((double*)(option_tbl + i)->_option_var) = result;
          break;
        }
        default:
          AIR_ASSERT_MSG(false, "not implement option kind: %s",
                         cml_option_name);
      }
      return true;
    }
  }
  return false;
}

OPTION_GRP* OPTION_MGR::Group_name_match(string_view cml_group_name) {
  vector<OPTION_GRP*> builtin_groups = this->Group_option();
  for (auto builtin_group : builtin_groups) {
    if (builtin_group->Name() == cml_group_name) {
      return builtin_group;
    }
  }
  return nullptr;
}

R_CODE OPTION_MGR::Parse_options(int argc, char** options) {
  _exe_name = options[0];
  if (argc < 2) {
    CMPLR_USR_MSG(U_CODE::No_Input_File, Exe_name());
    AIR_DEBUG("Please try %s -help/-h to see how to use", Exe_name());
    return R_CODE::USER;
  }

  vector<OPTION_DESC_HANDLE*> builtin_options = this->Top_level_option();
  for (int i = 1; i < argc; i += Get_option_increment()) {
    Set_option_increment(1);
    if (options[i][0] == DASH_SIGN) {
      if (i + 1 < argc) {
        Set_next_option(options[i + 1]);
      } else {
        Set_next_option("");
      }
      int number_of_dash = 1;
      if (options[i][1] == DASH_SIGN) number_of_dash = 2;
      string option = options[i];
      // remove dash in the prefix
      option.erase(0, number_of_dash);

      // group name all uppercase letter? only group name contains colon:?
      int colon_pos = option.find(COLON_SIGN);
      if (colon_pos != string::npos) {
        AIR_DEBUG("processing group option");
        string_view cml_grp_name = option.substr(0, colon_pos);
        OPTION_GRP* option_grp   = Group_name_match(cml_grp_name);
        if (option_grp == nullptr) {
          CMPLR_USR_MSG(U_CODE::Incorrect_Option, option.c_str());
          return R_CODE::USER;
        }

        // remove groupname and colon
        option.erase(0, cml_grp_name.size() + 1);

        vector<string> grp_options;
        for (colon_pos = option.find(option_grp->Separator());
             colon_pos != string::npos;
             colon_pos = option.find(option_grp->Separator())) {
          string grp_option_name = option.substr(0, colon_pos);
          grp_options.push_back(grp_option_name);
          // remove option and colon
          option.erase(0, grp_option_name.size() + 1);
        }
        grp_options.push_back(option);

        for (auto grp_option : grp_options) {
          if (!Option_match_and_update(grp_option, option_grp)) {
            CMPLR_USR_MSG(U_CODE::Incorrect_Option, grp_option.c_str());
            return R_CODE::USER;
          }
        }
      } else {
        AIR_DEBUG("processing top level option");
        if (!Option_match_and_update(option, builtin_options)) {
          CMPLR_USR_MSG(U_CODE::Incorrect_Option, option.c_str());
          return R_CODE::USER;
        }
      }
    } else {
      // positional parameter
      // check whether file exists
      // replace suffix with output file suffix if no -o filename provided
      if (!filesystem::exists(options[i])) {
        CMPLR_USR_MSG(U_CODE::Srcfile_Not_Found, options[i]);
        return R_CODE::USER;
      }
      _ifile                 = options[i];
      filesystem::path fname = filesystem::path(_ifile).filename();
      _ofile                 = fname.replace_extension(BFILE_SUFFIX).string();
      _tfile                 = fname.replace_extension(TFILE_SUFFIX).string();
      _pfile                 = fname.replace_extension(PFILE_SUFFIX).string();
      AIR_DEBUG(Ifile());
      AIR_DEBUG(Ofile());
      AIR_DEBUG(Tfile());
      AIR_DEBUG(Pfile());
    }
  }
  return R_CODE::NORMAL;
}

}  // namespace util
}  // namespace air
