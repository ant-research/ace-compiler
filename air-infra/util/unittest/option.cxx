//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/util/option.h"

#include <fstream>

#include "air/driver/global_config.h"
#include "gtest/gtest.h"

using namespace air::util;

namespace {

struct CONFIG_OPTION {
  CONFIG_OPTION(void)
      : _alias("off"),
        _analysis_enable(false),
        _show(false),
        _trace(false),
        _skip_before(0) {}
  ~CONFIG_OPTION(void) {}

  std::string_view Alias(void) { return _alias; }
  bool             Analysis_enable(void) { return _analysis_enable; }
  bool             Show(void) { return _show; }
  bool             Trace(void) { return _trace; }
  int64_t          Skip_before(void) { return _skip_before; }
  std::string_view Output_file(void) { return _o_file; }
  std::string_view Include_file(void) { return _include_file; }

  std::string _alias;
  bool        _analysis_enable;
  bool        _show;
  bool        _trace;
  int64_t     _skip_before;
  std::string _o_file;
  std::string _include_file;
};  // struct CONFIG_OPTION

class OPTION_TEST : public ::testing::Test {
protected:
  void SetUp() override {
    // Create a temporary file
    _filename = "hello.c";
    std::ofstream outfile(_filename);
    outfile << "#include<stdio.h>" << std::endl;
    outfile << "int main() {" << std::endl;
    outfile << "  printf(\"hello world\\n\");" << std::endl;
    outfile << "}" << std::endl;
    outfile.close();

    // use static to make this variable alive after leave SetUp function
    static OPTION_DESC top_options[] = {
        {"show",        "s", "Show the progress of nn2a", &_config_option._show,
         air::util::K_NONE,                                                                                  0, air::util::V_NONE      },
        {"trace",       "t", "Enable trace in nn2a",      &_config_option._trace,
         air::util::K_NONE,                                                                                  0, air::util::V_NONE      },
        {"skip_before", "",  "Skip item before given id",
         &_config_option._skip_before,                                                   air::util::K_INT64, 0,
         air::util::V_EQUAL                                                                                                            },
        {"o",           "",  "ouput file name",           &_config_option._o_file,       air::util::K_STR,
         0,                                                                                                     air::util::V_SPACE     },
        {"I",           "",  "include file name",         &_config_option._include_file,
         air::util::K_STR,                                                                                   0, air::util::V_NONE_SPACE},
    };

    static OPTION_DESC group_options[] = {
        {"alias",           "a",  "alias analysis open", &_config_option._alias,
         air::util::K_STR,                                                                          0, air::util::V_SPACE},
        {"analysis_enable", "ae", "analysis enable",
         &_config_option._analysis_enable,                                       air::util::K_NONE, 0,
         air::util::V_NONE                                                                                               },
    };

    _top_option_handle   = {sizeof(top_options) / sizeof(top_options[0]),
                            top_options};
    _group_option_handle = {sizeof(group_options) / sizeof(group_options[0]),
                            group_options};
    _option_group = {"NN2A", "nn2a group options", ':', air::util::V_EQUAL,
                     &_group_option_handle};
    _option_mgr.Register_top_level_option(&_top_option_handle);
    _option_mgr.Register_option_group(&_option_group);
  }

  void TearDown() override {
    // Remove the temporary file
    std::remove(_filename.c_str());
  }

  CONFIG_OPTION      _config_option;
  std::string        _filename;
  OPTION_MGR         _option_mgr;
  OPTION_DESC_HANDLE _top_option_handle;
  OPTION_DESC_HANDLE _group_option_handle;
  OPTION_GRP         _option_group;
};

TEST_F(OPTION_TEST, RegisterTopLevelOption) {
  std::vector<OPTION_DESC_HANDLE*> op_desc_handle =
      _option_mgr.Top_level_option();
  EXPECT_EQ(op_desc_handle.size(), 1);
  EXPECT_EQ(op_desc_handle[0]->Size(), _top_option_handle.Size());
}

TEST_F(OPTION_TEST, RegisterTopLevelOptionWithDuplicateOptionName) {
  OPTION_DESC duplicate_top_level_options[] = {
      {"show", "", "test show", &_config_option._show, air::util::K_NONE, 0,
       air::util::V_NONE}
  };

  OPTION_DESC_HANDLE duplicate_option_handle = {
      sizeof(duplicate_top_level_options) /
          sizeof(duplicate_top_level_options[0]),
      duplicate_top_level_options};

  EXPECT_EXIT(_option_mgr.Register_top_level_option(&duplicate_option_handle),
              testing::ExitedWithCode(1), ".*");
}

TEST_F(OPTION_TEST, RegisterTopLevelOptionWithDuplicateAbbrevOptionName1) {
  OPTION_DESC duplicate_top_level_options[] = {
      {"show-test", "s", "test show", &_config_option._show, air::util::K_NONE,
       0, air::util::V_NONE}
  };

  OPTION_DESC_HANDLE duplicate_option_handle = {
      sizeof(duplicate_top_level_options) /
          sizeof(duplicate_top_level_options[0]),
      duplicate_top_level_options};

  EXPECT_EXIT(_option_mgr.Register_top_level_option(&duplicate_option_handle),
              testing::ExitedWithCode(1), ".*");
}

TEST_F(OPTION_TEST, RegisterTopLevelOptionWithDuplicateAbbrevOptionName2) {
  OPTION_DESC duplicate_top_level_options[] = {
      {"show-test", "show", "test show", &_config_option._show,
       air::util::K_NONE, 0, air::util::V_NONE}
  };

  OPTION_DESC_HANDLE duplicate_option_handle = {
      sizeof(duplicate_top_level_options) /
          sizeof(duplicate_top_level_options[0]),
      duplicate_top_level_options};

  EXPECT_EXIT(_option_mgr.Register_top_level_option(&duplicate_option_handle),
              testing::ExitedWithCode(1), ".*");
}

TEST_F(OPTION_TEST, RegisterOptionGroupOption) {
  std::vector<OPTION_GRP*> option_grp = _option_mgr.Group_option();
  EXPECT_EQ(option_grp.size(), 1);
  EXPECT_EQ(option_grp[0]->Options()->Size(), _group_option_handle.Size());
  bool result = false;
  for (uint32_t i = 0; i < option_grp[0]->Options()->Size(); i++) {
    if (strcmp(option_grp[0]->Options()->Option(i)->Name(), "alias") == 0) {
      EXPECT_EQ(option_grp[0]->Options()->Option(i)->Val_maker(), V_EQUAL);
      result = true;
      break;
    }
  }
  EXPECT_TRUE(result);
}

TEST_F(OPTION_TEST, RegisterOptionGroupWithUnsupportSeperator) {
  OPTION_GRP test_option_group = {"TEST", "nn2a group options", '|',
                                  air::util::V_EQUAL, &_group_option_handle};
  EXPECT_EXIT(_option_mgr.Register_option_group(&test_option_group),
              testing::ExitedWithCode(1), "only colon");
}

TEST_F(OPTION_TEST, RegisterOptionGroupWithUnsupportValueMaker) {
  OPTION_DESC invalid_value_maker_group_options[] = {
      {"test", "t", "test open", &_config_option._alias, air::util::K_STR, 0,
       air::util::V_SPACE}
  };

  OPTION_DESC_HANDLE test_group_option_handle = {
      sizeof(invalid_value_maker_group_options) /
          sizeof(invalid_value_maker_group_options[0]),
      invalid_value_maker_group_options};
  OPTION_GRP test_option_group = {"TEST", "test group options", ':',
                                  air::util::V_SPACE,
                                  &test_group_option_handle};
  EXPECT_EXIT(_option_mgr.Register_option_group(&test_option_group),
              testing::ExitedWithCode(1), "equal sign =");
}

TEST_F(OPTION_TEST, RegisterOptionGroupWithDuplicateGroupName) {
  OPTION_GRP test_option_group = {"NN2A", "nn2a group options", ':',
                                  air::util::V_EQUAL, &_group_option_handle};
  EXPECT_EXIT(_option_mgr.Register_option_group(&test_option_group),
              testing::ExitedWithCode(1), ".*");
}

TEST_F(OPTION_TEST, ParseOnlyInputFile) {
  const char* argv[] = {"./compiler", "hello.c"};
  int         argc   = sizeof(argv) / sizeof(argv[0]);
  EXPECT_EQ(_option_mgr.Parse_options(argc, const_cast<char**>(argv)),
            R_CODE::NORMAL);
}

TEST_F(OPTION_TEST, ParseNoInputAndOption) {
  const char* argv[] = {"./compiler"};
  int         argc   = sizeof(argv) / sizeof(argv[0]);
  EXPECT_EXIT(_option_mgr.Parse_options(argc, const_cast<char**>(argv)),
              testing::ExitedWithCode(1), "no input files");
}

TEST_F(OPTION_TEST, ParseOuputOption) {
  const char* argv[] = {"./compiler", "hello.c", "-o", "hello.x"};
  int         argc   = sizeof(argv) / sizeof(argv[0]);
  EXPECT_EQ(_option_mgr.Parse_options(argc, const_cast<char**>(argv)),
            R_CODE::NORMAL);
  EXPECT_EQ(_config_option.Output_file(), "hello.x");
}

TEST_F(OPTION_TEST, ParseEnableTrace) {
  EXPECT_FALSE(_config_option.Trace());
  const char* argv[] = {"./compiler", "-trace"};
  int         argc   = sizeof(argv) / sizeof(argv[0]);
  EXPECT_EQ(_option_mgr.Parse_options(argc, const_cast<char**>(argv)),
            R_CODE::NORMAL);
  EXPECT_TRUE(_config_option.Trace());
  EXPECT_FALSE(_config_option.Show());
}

TEST_F(OPTION_TEST, ParseSkipBefore) {
  const char* argv[] = {"./compiler", "-skip_before=20"};
  int         argc   = sizeof(argv) / sizeof(argv[0]);
  EXPECT_EQ(_option_mgr.Parse_options(argc, const_cast<char**>(argv)),
            R_CODE::NORMAL);
  EXPECT_EQ(_config_option.Skip_before(), 20);
}

TEST_F(OPTION_TEST, ParseOptionWithInvalidValue) {
  const char* argv[] = {"./compiler", "-skip_before=x20"};
  int         argc   = sizeof(argv) / sizeof(argv[0]);
  EXPECT_EXIT(_option_mgr.Parse_options(argc, const_cast<char**>(argv)),
              testing::ExitedWithCode(1), "Incorrect option");
}

TEST_F(OPTION_TEST, ParseOptionWithVeryLargeValue) {
  const char* argv[] = {"./compiler",
                        "-skip_before=2000000000000000000000000000000"};
  int         argc   = sizeof(argv) / sizeof(argv[0]);
  EXPECT_EXIT(_option_mgr.Parse_options(argc, const_cast<char**>(argv)),
              testing::ExitedWithCode(1), "Incorrect option");
}

TEST_F(OPTION_TEST, ParseOnlyNoneKindGroupOption) {
  EXPECT_FALSE(_config_option.Analysis_enable());
  const char* argv[] = {"./compiler", "-NN2A:ae"};
  int         argc   = sizeof(argv) / sizeof(argv[0]);
  EXPECT_EQ(_option_mgr.Parse_options(argc, const_cast<char**>(argv)),
            R_CODE::NORMAL);
  EXPECT_TRUE(_config_option.Analysis_enable());
}

TEST_F(OPTION_TEST, ParseOnlyStrKindGroupOption) {
  EXPECT_EQ(_config_option.Alias(), "off");
  const char* argv[] = {"./compiler", "-NN2A:a=on"};
  int         argc   = sizeof(argv) / sizeof(argv[0]);
  EXPECT_EQ(_option_mgr.Parse_options(argc, const_cast<char**>(argv)),
            R_CODE::NORMAL);
  EXPECT_EQ(_config_option.Alias(), "on");
}

TEST_F(OPTION_TEST, ParseMixKindGroupOption) {
  EXPECT_FALSE(_config_option.Analysis_enable());
  EXPECT_EQ(_config_option.Alias(), "off");
  const char* argv[] = {"./compiler", "-NN2A:ae:a=on"};
  int         argc   = sizeof(argv) / sizeof(argv[0]);
  EXPECT_EQ(_option_mgr.Parse_options(argc, const_cast<char**>(argv)),
            R_CODE::NORMAL);
  EXPECT_TRUE(_config_option.Analysis_enable());
  EXPECT_EQ(_config_option.Alias(), "on");
}

}  // namespace
