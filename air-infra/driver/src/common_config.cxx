//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/driver/common_config.h"

namespace air {
namespace util {

void COMMON_CONFIG::Print(std::ostream& os) const {
  os << "  Enable: " << (_enable ? "Yes" : "No") << std::endl;
  os << "  Show: " << (_show ? "Yes" : "No") << std::endl;
  os << "  Runtime Timing: " << (_rt_timing ? "Yes" : "No") << std::endl;
  os << "  Runtime Validate: " << (_rt_validate ? "Yes" : "No") << std::endl;
  os << "  Trace stat: " << (_trace_stat ? "Yes" : "No") << std::endl;
  os << "  Trace IR before: " << (_trace_ir_before ? "Yes" : "No") << std::endl;
  os << "  Trace IR after: " << (_trace_ir_after ? "Yes" : "No") << std::endl;
  os << "  Trace Symtab before: " << (_trace_st_before ? "Yes" : "No")
     << std::endl;
  os << "  Trace Symtab after: " << (_trace_st_after ? "Yes" : "No")
     << std::endl;
  os << "  Trace detail: " << _trace_detail << std::endl;
  os << "  Skip before: " << _skip_before << std::endl;
  os << "  Skip after: " << _skip_after << std::endl;
  os << "  Skip equal: " << _skip_equal << std::endl;
  os << "  Read IR from: " << _read_ir << std::endl;
  os << "  Write IR to: " << _write_ir << std::endl;
}

}  // namespace util
}  // namespace air
