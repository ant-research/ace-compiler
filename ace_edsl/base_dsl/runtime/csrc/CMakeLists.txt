project(dlpack_runtime LANGUAGES CXX)

# Find Python and add the nanobind package
find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)
find_package(nanobind CONFIG REQUIRED)

include_directories(${dlpack_SOURCE_DIR}/include)

# Create the extension module
nanobind_add_module(
    dlpack_runtime
    dlpack_runtime.cpp
)

set_target_properties(dlpack_runtime PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/base_dsl_extension/dlpack_runtime"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/base_dsl_extension/dlpack_runtime"
  OUTPUT_NAME "dlpack_runtime"
)