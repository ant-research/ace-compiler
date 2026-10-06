cmake_minimum_required(VERSION 3.20)

project(phantom 
    LANGUAGES CXX CUDA 
    VERSION 2.0 
    DESCRIPTION "CUDA-Accelerated Homomorphic Encryption Library"
)

# Options
option(PHANTOM_USE_CUDA_PTX "Use CUDA PTX Assembly" ON)
option(PHANTOM_BUILD_EXAMPLES "Build examples" ON)
option(PHANTOM_BUILD_TESTS "Build tests" OFF)

if (PHANTOM_USE_CUDA_PTX)
    add_compile_definitions(PHANTOM_USE_CUDA_PTX)
endif ()

# CUDA settings
if (NOT DEFINED CMAKE_CUDA_ARCHITECTURES)
    set(CMAKE_CUDA_ARCHITECTURES 90)
endif ()
set(CMAKE_CUDA_SEPARABLE_COMPILATION ON)

# C++ and CUDA standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_STANDARD 17)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

# Global settings
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

# Add cmake modules path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Find NTL and GMP
find_package(NTL REQUIRED)
find_package(GMP REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include ${NTL_INCLUDE_DIRS} ${GMP_INCLUDE_DIRS})

# Find all source files for phantom (including bootstrapping)
file(GLOB_RECURSE PHANTOM_SOURCES "src/*.cu")

# Main phantom library
add_library(phantom STATIC ${PHANTOM_SOURCES})

target_compile_options(phantom PRIVATE 
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CUDA>>:-G;-src-in-ptx>"
)
target_compile_features(phantom PUBLIC cxx_std_17 cuda_std_17)
target_link_libraries(phantom PRIVATE ${NTL_LIBRARIES} ${GMP_LIBRARIES})

# Installation
install(TARGETS phantom
    EXPORT phantomConfig
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)
install(EXPORT phantomConfig 
    NAMESPACE phantom:: 
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/phantom
)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ 
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/phantom
)

# Conditionally build examples
if(PHANTOM_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Conditionally build tests
if(PHANTOM_BUILD_TESTS)
    # Google Test setup
    include(FetchContent)
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(googletest)
    # Enable testing
    enable_testing()

    # Add the tests subdirectory
    add_subdirectory(tests)
endif()
