//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_LLAMA_LLAMA_H
#define NN_LLAMA_LLAMA_H

#include <vector>

#include "air/base/container.h"
#include "air/base/st.h"
#include "air/driver/driver_ctx.h"
#include "config.h"

using namespace air::base;

namespace nn {
namespace llama {

class LLAMA {
public:
  LLAMA(GLOB_SCOPE* glob) : _glob(glob) {
    _spos = _glob->Unknown_simple_spos();
  }
  ~LLAMA() {}
  void           Create_entry_func();
  ADDR_DATUM_PTR Create_rmsnorm(std::string output_name);
  ADDR_DATUM_PTR Create_matmul(ADDR_DATUM_PTR input, std::string weight_input,
                               std::string output_name);
  ADDR_DATUM_PTR Create_matmul(ADDR_DATUM_PTR input_var_1,
                               ADDR_DATUM_PTR input_var_2,
                               std::string    output_name);
  ADDR_DATUM_PTR Create_matmul(NODE_PTR input_1, NODE_PTR input_2,
                               TYPE_PTR array_type, std::string output_name);
  std::pair<ADDR_DATUM_PTR, ADDR_DATUM_PTR> Create_rope_rotary(
      ADDR_DATUM_PTR input_1, ADDR_DATUM_PTR input_2, std::string weight_input,
      std::string output);
  std::pair<ADDR_DATUM_PTR, ADDR_DATUM_PTR> Create_kv_cache(
      ADDR_DATUM_PTR input_1, ADDR_DATUM_PTR input_2, ADDR_DATUM_PTR input_3,
      std::string start_pos, std::string output);
  void         Create_return(ADDR_DATUM_PTR input);
  FIELD_PTR    Get_fld(ADDR_DATUM_PTR var, uint32_t fld_id);
  CONSTANT_PTR Read_value_from_file(std::string file_name);
  std::pair<ADDR_DATUM_PTR, ADDR_DATUM_PTR> Create_record_return(
      NODE_PTR nn_node, TYPE_PTR array_type, std::string output_name);
  ADDR_DATUM_PTR Create_repeat_kv(ADDR_DATUM_PTR, int32_t times,
                                  std::string output_name);
  ADDR_DATUM_PTR Create_transpose(ADDR_DATUM_PTR input, int32_t val_1,
                                  int32_t val_2, std::string result);
  ADDR_DATUM_PTR Create_sqrt(std::string weight_input, std::string result);
  ADDR_DATUM_PTR Create_divide(ADDR_DATUM_PTR input_1, ADDR_DATUM_PTR input_2,
                               std::string result);
  ADDR_DATUM_PTR Create_softmax(ADDR_DATUM_PTR input, std::string result);

private:
  LLAMA(void);                          // REQUIRED UNDEFINED UNWANTED methods
  LLAMA(const LLAMA&);                  // REQUIRED UNDEFINED UNWANTED methods
  LLAMA&      operator=(const LLAMA&);  // REQUIRED UNDEFINED UNWANTED methods
  GLOB_SCOPE* _glob;
  FUNC_SCOPE* _func_scope;
  SPOS        _spos;
};

GLOB_SCOPE* Llama_driver(GLOB_SCOPE*                    glob,
                         const air::driver::DRIVER_CTX* driver_ctx,
                         const nn::llama::LLAMA_CONFIG& cfg);

}  // namespace llama
}  // namespace nn

#endif  // NN_LLAMA_LLAMA_H
