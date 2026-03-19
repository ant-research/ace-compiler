// onnx_loader.cpp - ONNX model loading for Python bindings
//
// Separate file to avoid namespace conflicts with air::base::OPCODE

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>

#ifdef ACE_BINDINGS_ENABLED

#include "air/base/meta_info.h"
#include "air/base/st.h"
#include "air/core/opcode.h"
#include "nn/core/opcode.h"
#include "nn/vector/vector_opcode.h"
#include "nn/onnx2air/air_gen.h"
#include "nn/onnx2air/config.h"
#include "nn/util/copy_prop.h"
#include "air/driver/driver_ctx.h"
#include "onnx.pb.h"

namespace py = pybind11;

namespace ace_bindings {

// Sanitize string for UTF-8 compatibility
// Replaces invalid byte sequences with hex escape sequences
static std::string sanitize_utf8(const std::string& input) {
    std::string result;
    result.reserve(input.size());
    
    for (size_t i = 0; i < input.size(); ) {
        unsigned char c = static_cast<unsigned char>(input[i]);
        
        // ASCII range (0x00-0x7F) - always valid
        if (c <= 0x7F) {
            result += input[i];
            i++;
            continue;
        }
        
        // Check for valid multi-byte UTF-8 sequences
        size_t seq_len = 0;
        if ((c & 0xE0) == 0xC0) seq_len = 2;      // 110xxxxx - 2 byte sequence
        else if ((c & 0xF0) == 0xE0) seq_len = 3; // 1110xxxx - 3 byte sequence
        else if ((c & 0xF8) == 0xF0) seq_len = 4; // 11110xxx - 4 byte sequence
        
        // Check if sequence is complete and valid
        bool valid = (seq_len >= 2) && (i + seq_len <= input.size());
        if (valid) {
            for (size_t j = 1; j < seq_len; j++) {
                unsigned char cont = static_cast<unsigned char>(input[i + j]);
                if ((cont & 0xC0) != 0x80) {  // Must be 10xxxxxx
                    valid = false;
                    break;
                }
            }
        }
        
        if (valid) {
            // Copy valid multi-byte sequence
            for (size_t j = 0; j < seq_len; j++) {
                result += input[i + j];
            }
            i += seq_len;
        } else {
            // Replace invalid byte with hex escape
            char hex[8];
            snprintf(hex, sizeof(hex), "\\x%02x", c);
            result += hex;
            i++;
        }
    }
    
    return result;
}

// Forward declaration - GlobScope is defined in air_builder_bindings.cpp
// We'll use a void* to pass the glob scope pointer
struct OnnxLoadResult {
    bool success;
    std::string message;
    air::base::GLOB_SCOPE* glob;
    std::string ir_dump;
};

// Helper function to get directory path from file path
static std::string get_directory_path(const std::string& file_name) {
    std::filesystem::path abs_path = file_name;
    std::filesystem::path dir_path = abs_path.parent_path();
    return dir_path.lexically_normal().string();
}

// Ensure AIR is initialized
static bool s_onnx_air_initialized = false;

void ensure_onnx_air_initialized() {
    if (!s_onnx_air_initialized) {
        air::base::META_INFO::Remove_all();
        air::core::Register_core();
        nn::core::Register_nn();
        nn::vector::Register_vector_domain();
        s_onnx_air_initialized = true;
    }
}

// Load an ONNX model and convert to AIR (nn::core level)
// Returns result struct with glob scope pointer
OnnxLoadResult load_onnx_model_impl(const std::string& onnx_path) {
    OnnxLoadResult result;
    result.success = false;
    result.glob = nullptr;
    
    // Check if file exists
    if (!std::filesystem::exists(onnx_path)) {
        result.message = "ONNX file not found: " + onnx_path;
        return result;
    }
    
    try {
        // Ensure AIR is initialized
        ensure_onnx_air_initialized();
        
        // Create a new glob scope for the model
        air::base::GLOB_SCOPE* glob = air::base::GLOB_SCOPE::Get();
        
        // Parse the ONNX model
        onnx::ModelProto onnx_model;
        std::ifstream input(onnx_path, std::ios::binary);
        if (!input.is_open()) {
            result.message = "Failed to open ONNX file: " + onnx_path;
            return result;
        }
        
        if (!onnx_model.ParseFromIstream(&input)) {
            result.message = "Failed to parse ONNX model";
            return result;
        }
        input.close();
        
        // Create AIR generator and process the model
        nn::onnx2air::AIRGEN air_gen(glob);
        air_gen.Directory_path_set(get_directory_path(onnx_path));
        
        if (!air_gen.Process_graph(onnx_model)) {
            result.message = "Failed to convert ONNX to AIR";
            return result;
        }
        
        // Run copy propagation optimization
        air::driver::DRIVER_CTX driver_ctx;
        air::base::GLOB_SCOPE* optimized_glob = nn::opt::Opt_perform_copy_propagation(glob, &driver_ctx);
        if (optimized_glob) {
            glob = optimized_glob;
        }
        
        result.success = true;
        result.message = "ONNX model loaded successfully";
        result.glob = glob;
        
        // Dump the IR for debugging (sanitize for UTF-8 compatibility)
        std::ostringstream oss;
        glob->Print(oss);
        result.ir_dump = sanitize_utf8(oss.str());
        
    } catch (const std::exception& e) {
        result.message = std::string("Exception loading ONNX: ") + e.what();
    } catch (...) {
        result.message = "Unknown error loading ONNX model";
    }
    
    return result;
}

} // namespace ace_bindings

// Export function for use by air_builder_bindings.cpp
extern "C" {
    ace_bindings::OnnxLoadResult ace_load_onnx_model(const char* path) {
        return ace_bindings::load_onnx_model_impl(std::string(path));
    }
}

#endif // ACE_BINDINGS_ENABLED

