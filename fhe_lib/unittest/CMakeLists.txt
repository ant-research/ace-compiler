#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

# fhe_lib/unittest — C++ unit tests organized by public API surface.
#
# Directory layout mirrors the public header structure:
#   common/io/     — Tests for lib/common/io/*.h
#   common/kms/    — Tests for lib/common/kms/*.h
#   common/data/   — Tests for lib/common/data/*.h
#   common/bench/  — Performance benchmarks (GPU required, not in default ctest)
#
# Targets:
#   ut_common       — FHErt_io correctness tests
#   ut_common_bench — FHErt_io GPU benchmarks (separate binary)
#   ut_phantom      — Phantom GPU backend (FHErt_phantom, GPU required)
#   ut_ace          — ACE runtime (FHErt_ace, placeholder)
#
# Build:
#   cmake -DBUILD_UNITTEST=ON ..
#   make ut_common
#   ctest -L common_unit -V

enable_testing()

# ---------------------------------------------------------------------------
# ACE_PHANTOM_GPU_TESTS — enable GPU-dependent tests
# ---------------------------------------------------------------------------
option(ACE_PHANTOM_GPU_TESTS "Enable GPU-dependent tests" OFF)

# ---------------------------------------------------------------------------
# nlohmann/json — needed by common/bench/io_perf.cpp for JSON benchmark output
# ---------------------------------------------------------------------------
find_package(nlohmann_json REQUIRED)

# ---------------------------------------------------------------------------
# gtest — reuse the gtest built by rtlib's ExternalProject, or fetch
# ---------------------------------------------------------------------------
if(TARGET gtest)
    set(GTEST_INCLUDE_DIR $ENV{UNITTEST_INCLUDE_DIR})
    set(GTEST_LIBS gtest gtest_main pthread)
    set(GTEST_LIBRARY_DIR "${CMAKE_BINARY_DIR}/external/src/unittest-build/lib")
else()
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.14.0
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    set(GTEST_INCLUDE_DIR "")
    set(GTEST_LIBS GTest::gtest_main)
    set(GTEST_LIBRARY_DIR "")
endif()

# ---------------------------------------------------------------------------
# Common includes for all targets
# ---------------------------------------------------------------------------
set(FHELIB_UNITTEST_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}                  # for test_util.h
)

if(GTEST_INCLUDE_DIR)
    list(APPEND FHELIB_UNITTEST_INCLUDE_DIRS ${GTEST_INCLUDE_DIR})
endif()

# ---------------------------------------------------------------------------
# ut_common — FHErt_io correctness tests
# ---------------------------------------------------------------------------
# Shared test utility (compiled into ut_common directly)
set(FHE_TEST_UTIL_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/test_util.cpp
)

# IO transport tests
file(GLOB FHE_COMMON_IO_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/common/io/*.cpp
)
# KMS tests
file(GLOB FHE_COMMON_KMS_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/common/kms/*.cpp
)
# Data tests
file(GLOB FHE_COMMON_DATA_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/common/data/*.cpp
)

set(FHE_COMMON_UNITTEST_SOURCES
    ${FHE_TEST_UTIL_SOURCES}
    ${FHE_COMMON_IO_SOURCES}
    ${FHE_COMMON_KMS_SOURCES}
    ${FHE_COMMON_DATA_SOURCES}
)

add_executable(ut_common ${FHE_COMMON_UNITTEST_SOURCES})
set_property(TARGET ut_common PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/unittest)
target_include_directories(ut_common PRIVATE ${FHELIB_UNITTEST_INCLUDE_DIRS})
if(GTEST_LIBRARY_DIR)
    target_link_directories(ut_common PRIVATE ${GTEST_LIBRARY_DIR})
endif()
target_link_libraries(ut_common PRIVATE FHErt_io ${GTEST_LIBS} CUDA::cudart)
if(ACE_PHANTOM_GPU_TESTS)
    target_compile_definitions(ut_common PRIVATE ACE_PHANTOM_GPU_TESTS)
endif()
set_target_properties(ut_common PROPERTIES
    BUILD_RPATH "${CMAKE_BINARY_DIR}/fhe_lib/common"
    INSTALL_RPATH "$ORIGIN"
)
add_test(NAME ut_common COMMAND ut_common)
set_tests_properties(ut_common PROPERTIES LABELS "common_unit")

# ---------------------------------------------------------------------------
# ut_common_bench — GPU IO performance benchmarks
# ---------------------------------------------------------------------------
file(GLOB FHE_BENCH_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/common/bench/*.cpp
)

if(FHE_BENCH_SOURCES)
    add_executable(ut_common_bench ${FHE_BENCH_SOURCES} ${FHE_TEST_UTIL_SOURCES})
    set_property(TARGET ut_common_bench PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/unittest)
    target_include_directories(ut_common_bench PRIVATE ${FHELIB_UNITTEST_INCLUDE_DIRS})
    if(GTEST_LIBRARY_DIR)
        target_link_directories(ut_common_bench PRIVATE ${GTEST_LIBRARY_DIR})
    endif()
    target_link_libraries(ut_common_bench PRIVATE FHErt_io ${GTEST_LIBS} CUDA::cudart nlohmann_json::nlohmann_json)
    if(ACE_PHANTOM_GPU_TESTS)
        target_compile_definitions(ut_common_bench PRIVATE ACE_PHANTOM_GPU_TESTS)
    endif()
    set_target_properties(ut_common_bench PROPERTIES
        BUILD_RPATH "${CMAKE_BINARY_DIR}/fhe_lib/common"
        INSTALL_RPATH "$ORIGIN"
    )
    # NOT added to ctest by default — these are GPU benchmarks
    # Run manually: ./unittest/ut_common_bench --gtest_filter="IoPerf*"
endif()

# ---------------------------------------------------------------------------
# ut_phantom — Phantom GPU backend tests
# ---------------------------------------------------------------------------
if(WITH_PHANTOM)
    file(GLOB FHE_PHANTOM_UNITTEST_SOURCES CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/phantom/*.cpp
    )

    if(FHE_PHANTOM_UNITTEST_SOURCES)
        add_executable(ut_phantom ${FHE_PHANTOM_UNITTEST_SOURCES})
        set_property(TARGET ut_phantom PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/unittest)
        target_include_directories(ut_phantom PRIVATE
            ${FHELIB_UNITTEST_INCLUDE_DIRS}
            ${phantom_SOURCE_DIR}/include
        )
        if(GTEST_LIBRARY_DIR)
            target_link_directories(ut_phantom PRIVATE ${GTEST_LIBRARY_DIR})
        endif()
        target_link_libraries(ut_phantom PRIVATE FHErt_phantom FHErt_io ${GTEST_LIBS} CUDA::cudart)
        set_target_properties(ut_phantom PROPERTIES
            BUILD_RPATH "${CMAKE_BINARY_DIR}/fhe_lib/common;${CMAKE_BINARY_DIR}/fhe_lib/phantom"
            INSTALL_RPATH "$ORIGIN"
        )
        add_test(NAME ut_phantom COMMAND ut_phantom)
        set_tests_properties(ut_phantom PROPERTIES LABELS "phantom_unit")
    else()
        message(STATUS "No phantom unit test files found — skipping ut_phantom")
    endif()
endif()

# ---------------------------------------------------------------------------
# ut_ace — ACE runtime tests (placeholder)
# ---------------------------------------------------------------------------
file(GLOB FHE_ACE_UNITTEST_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/ace/*.cpp
)

if(FHE_ACE_UNITTEST_SOURCES)
    add_executable(ut_ace ${FHE_ACE_UNITTEST_SOURCES})
    set_property(TARGET ut_ace PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/unittest)
    target_include_directories(ut_ace PRIVATE ${FHELIB_UNITTEST_INCLUDE_DIRS})
    if(GTEST_LIBRARY_DIR)
        target_link_directories(ut_ace PRIVATE ${GTEST_LIBRARY_DIR})
    endif()
    target_link_libraries(ut_ace PRIVATE FHErt_ace FHErt_io ${GTEST_LIBS} CUDA::cudart)
    set_target_properties(ut_ace PROPERTIES
        BUILD_RPATH "${CMAKE_BINARY_DIR}/fhe_lib/common;${CMAKE_BINARY_DIR}/fhe_lib/ace"
        INSTALL_RPATH "$ORIGIN"
    )
    add_test(NAME ut_ace COMMAND ut_ace)
    set_tests_properties(ut_ace PROPERTIES LABELS "ace_unit")
endif()
