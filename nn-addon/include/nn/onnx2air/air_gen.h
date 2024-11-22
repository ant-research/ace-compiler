//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_GEN_H
#define AIR_GEN_H

#include "air_sym.h"
#include "air_type.h"
#include "onnx.pb.h"

namespace nn {
namespace onnx2air {
/**
 * @brief Interface of converting the onnx graph to the AIR.
 *
 */
class AIRGEN {
public:
  AIRGEN(GLOB_SCOPE* glob) : _sym_gen(this), _ty_gen(this), _glob(glob) {
    _func_scope = nullptr;
  }
  ~AIRGEN();

  bool Process_graph(onnx::ModelProto& onnx_model);

  AIRSYMGEN&  Sg() { return _sym_gen; }
  AIRTYGEN&   Tg() { return _ty_gen; }
  STR_PTR     Enter_string(const char* str);
  GLOB_SCOPE* Get_glob() { return _glob; }
  FUNC_SCOPE* Get_func_scope() { return _func_scope; }
  void        Set_func_scope(FUNC_SCOPE* fs) { _func_scope = fs; }

  void        Directory_path_set(std::string path) { _directory_path = path; }
  std::string Directory_path() { return _directory_path; }

private:
  AIRGEN(void);                      // REQUIRED UNDEFINED UNWANTED methods
  AIRGEN(const AIRGEN&);             // REQUIRED UNDEFINED UNWANTED methods
  AIRGEN& operator=(const AIRGEN&);  // REQUIRED UNDEFINED UNWANTED methods

  AIRSYMGEN   _sym_gen;
  AIRTYGEN    _ty_gen;
  GLOB_SCOPE* _glob;
  FUNC_SCOPE* _func_scope;
  std::string _directory_path;
};
}  // namespace onnx2air
}  // namespace nn

#endif /* AIR_GEN_H */
