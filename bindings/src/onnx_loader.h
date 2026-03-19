// onnx_loader.h - ONNX model loading interface for Python bindings

#ifndef ONNX_LOADER_H
#define ONNX_LOADER_H

#include <string>

#ifdef ACE_BINDINGS_ENABLED
#include "air/base/st.h"

namespace ace_bindings {

struct OnnxLoadResult {
    bool success;
    std::string message;
    air::base::GLOB_SCOPE* glob;
    std::string ir_dump;
};

// Load an ONNX model and convert to AIR (nn::core level)
OnnxLoadResult load_onnx_model_impl(const std::string& onnx_path);

} // namespace ace_bindings

#endif // ACE_BINDINGS_ENABLED

#endif // ONNX_LOADER_H

