#include "vector_ops.h"

#include "nn/vector/vector_gen.h"
#include "nn/vector/vector_utils.h"

using namespace nn::vector;

namespace air::dsl {
VECTOR_API::VECTOR_API(PY_AIRGEN& dsl) : _dsl(dsl) {}

NODE_PTR VECTOR_API::Add(NODE_PTR a, NODE_PTR b, const SPOS& spos) {
  VECTOR_GEN vgen(&_dsl.Get_cur_func_scope()->Container());
  NODE_PTR   node = vgen.New_add(a, b, spos);
  return node;
}

NODE_PTR VECTOR_API::Mul(NODE_PTR a, NODE_PTR b, const SPOS& spos) {
  VECTOR_GEN vgen(&_dsl.Get_cur_func_scope()->Container());
  NODE_PTR   node = vgen.New_mul(a, b, spos);
  return node;
}

NODE_PTR VECTOR_API::Roll(NODE_PTR vec, NODE_PTR offset, const SPOS& spos) {
  VECTOR_GEN vgen(&_dsl.Get_cur_func_scope()->Container());
  NODE_PTR   node = vgen.New_roll(vec, offset, spos);
  return node;
}

NODE_PTR VECTOR_API::Slice(NODE_PTR vec, NODE_PTR begin, NODE_PTR slice_size,
                           const SPOS& spos) {
  VECTOR_GEN vgen(&_dsl.Get_cur_func_scope()->Container());
  NODE_PTR   node = vgen.New_slice(vec, begin, slice_size, spos);
  return node;
}

NODE_PTR VECTOR_API::Reshape(NODE_PTR op0, std::vector<int64_t> shape,
                             const SPOS& spos) {
  VECTOR_GEN vgen(&_dsl.Get_cur_func_scope()->Container());
  NODE_PTR   reshape_node = vgen.New_reshape(op0, shape, spos);
  return reshape_node;
}

}  // namespace air::dsl
