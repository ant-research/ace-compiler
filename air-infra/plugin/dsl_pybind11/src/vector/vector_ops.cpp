#include "vector_ops.h"

namespace air::dsl {
VECTOR_API::VECTOR_API(DSL &dsl) : _dsl(dsl) {
  bool ret = nn::vector::Register_vector_domain();
  AIR_ASSERT(ret);
}

NODE_PTR VECTOR_API::Add(NODE_PTR a, NODE_PTR b, const SPOS &spos) {
  NODE_PTR node = _dsl.getCurFuncScope()->Container().New_bin_arith(
      OPCODE(nn::vector::VECTOR, nn::vector::VECTOR_OPCODE::ADD), a, b, spos);
  return node;
}
} // namespace air::dsl