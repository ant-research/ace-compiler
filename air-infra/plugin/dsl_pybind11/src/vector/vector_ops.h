#pragma once

#include "nn/vector/vector_opcode.h"
#include "py_airgen.h"

using namespace air::base;
using namespace air::util;

namespace air::dsl {

class VECTOR_API {
public:
  VECTOR_API(PY_AIRGEN& dsl);

  // TODO : The operator API should be defined
  NODE_PTR Add(NODE_PTR a, NODE_PTR b, const SPOS& spos);
  NODE_PTR Mul(NODE_PTR a, NODE_PTR b, const SPOS& spos);
  NODE_PTR Roll(NODE_PTR vec, NODE_PTR offset, const SPOS& spos);
  NODE_PTR Reshape(NODE_PTR op0, std::vector<int64_t> shape, const SPOS& spos);
  NODE_PTR Slice(NODE_PTR vec, NODE_PTR begin, NODE_PTR slice_size,
                 const SPOS& spos);

private:
  PY_AIRGEN& _dsl;
};

}  // namespace air::dsl
