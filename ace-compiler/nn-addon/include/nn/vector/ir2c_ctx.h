//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_VECTOR_IR2C_CTX_H
#define NN_VECTOR_IR2C_CTX_H

#include "air/core/ir2c_ctx.h"
#include "nn/vector/vec2c_config.h"

namespace nn {

namespace vector {

class IR2C_CTX : public air::core::IR2C_CTX {
public:
  IR2C_CTX(std::ostream& os, const VEC2C_CONFIG& cfg)
      : air::core::IR2C_CTX(os), _config(cfg) {}

  DECLARE_VEC2C_CONFIG_ACCESS_API(_config)

  void Emit_global_include() {
    _ir2c_util << "// external header files" << std::endl;
    _ir2c_util << "#include \"rt_nn.h\"" << std::endl << std::endl;
  }

private:
  const VEC2C_CONFIG& _config;
};

}  // namespace vector

}  // namespace nn

#endif  // NN_VECTOR_IR2C_CTX_H
