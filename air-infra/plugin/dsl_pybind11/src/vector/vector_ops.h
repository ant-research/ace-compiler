#pragma once

#include "dsl.hpp"
#include "nn/vector/vector_opcode.h"

using namespace air::base;
using namespace air::util;

namespace air::dsl {

class VECTOR_API {
public:
  VECTOR_API(DSL &dsl);

  // TODO : The operator API should be defined
  NODE_PTR Add(NODE_PTR a, NODE_PTR b, const SPOS &spos);

private:
  DSL &_dsl;
};

} // namespace air::dsl