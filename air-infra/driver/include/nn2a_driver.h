//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_DRIVER_NN2A_DRIVER_H
#define NN_DRIVER_NN2A_DRIVER_H

#include "config_nn2a.inc"

namespace nn {

namespace driver {

class NN2A_DRIVER {
public:
  NN2A_DRIVER(bool standalone, int argc, char** argv)
      : _standalone(standalone), _argc(argc), _argv(argv) {}

  int Init() {  // allocate resource and open input file
    Register_options_config_nn2a(_option_mgr, _standalone);
    R_CODE ret_code = _option_mgr.Parse_options(_argc, _argv);
    if (ret_code == R_CODE::NORMAL) {
      // after parse success, every pass(or other type) who do register should
      // also do update
      Update_options(_config_nn2a);
    } else {
      exit(int(ret_code));
    }
    return int(ret_code);
  }

  int Run();  // do the translation

  int Fini();  // write to binary file and free up resource

  ~NN2A_DRIVER(void) {}

  bool             Trace_enabled(void) { return _config_nn2a.Trace(); }
  bool             Show(void) { return _config_nn2a.Show(); }
  std::string_view Alias(void) {
    return _config_nn2a.Alias();
    ;
  }
  std::string_view Analysis_enable(void) {
    return _config_nn2a.Analysis_enable();
  }
  int64_t          Skip_before(void) { return _config_nn2a.Skip_before(); }
  std::string_view Output_file(void) { return _config_nn2a.Output_file(); }
  std::string_view Include_file(void) { return _config_nn2a.Include_file(); }

  air::util::OPTION_MGR& Option_mgr(void) { return _option_mgr; }

private:
  air::util::OPTION_MGR _option_mgr;
  CONFIG_NN2A           _config_nn2a;
  bool                  _standalone;
  int                   _argc;
  char**                _argv;
};

#ifdef BUILD_STANDALONE_NN2A
int main(int argc, char** argv) {
  NN2A_DRIVER driver(true, argv);

  // to collect from the argv, TBD, what is the suffix of the input file
  char* ifile;

  air::util::TFILE tfile;

  Config_nn2a.Print(tfile);
}
#endif

}  // namespace driver

}  // namespace nn

#endif  // NN_DRIVER_NN2A_DRIVER_H
