#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

include(FetchContent)
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/AceFetchContent.cmake)

# ---------------------------------------------------------------------------
# Fetch compiler toolchain dependencies
# ---------------------------------------------------------------------------

# air-infra (populate only — built inline by fhe-cmplr via FHE_WITH_SRC)
ace_fetch_content(${DEP_AIR_NAME}
  GIT_REPOSITORY ${DEP_AIR_URL}
  GIT_TAG        ${DEP_AIR_REF}
  LOCAL_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/${DEP_AIR_NAME}
  SOURCE_DIR     ${CMAKE_CURRENT_SOURCE_DIR}/${DEP_AIR_NAME}
  POPULATE_ONLY
)

# nn-addon (populate only — built inline by fhe-cmplr via FHE_WITH_SRC)
ace_fetch_content(${DEP_NN_NAME}
  GIT_REPOSITORY ${DEP_NN_URL}
  GIT_TAG        ${DEP_NN_REF}
  LOCAL_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/${DEP_NN_NAME}
  SOURCE_DIR     ${CMAKE_CURRENT_SOURCE_DIR}/${DEP_NN_NAME}
  POPULATE_ONLY
)

# fhe-cmplr (declare only; MakeAvailable is called after option forwarding
# below, because fhe-cmplr's CMakeLists.txt reads FHE_WITH_SRC and
# FHE_ENABLE_* at add_subdirectory time)
ace_fetch_content(${DEP_FHE_NAME}
  GIT_REPOSITORY ${DEP_FHE_URL}
  GIT_TAG        ${DEP_FHE_REF}
  LOCAL_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/${DEP_FHE_NAME}
  SOURCE_DIR     ${CMAKE_CURRENT_SOURCE_DIR}/${DEP_FHE_NAME}
  DECLARE_ONLY
)

# ---------------------------------------------------------------------------
# fhe-cmplr: air-infra + nn-addon + fhe-cmplr built together
# ---------------------------------------------------------------------------
set(FHE_WITH_SRC "air-infra;nn-addon" CACHE STRING "Set source dependencies")

# Code style checks — always off in integrated build
set(AIR_CODE_CHECK       OFF CACHE BOOL "Disable coding conventions checking")
set(NN_CODE_CHECK        OFF CACHE BOOL "Disable coding conventions checking")
set(FHE_CODE_CHECK       OFF CACHE BOOL "Disable coding conventions checking")

# Example builds — inherit from top-level BUILD_EXAMPLE
# FORCE is needed because sub-projects declare option(ON) which takes precedence
# over a CACHE variable that was already set by a parent project.
set(AIR_BUILD_EXAMPLE   ${BUILD_EXAMPLE} CACHE BOOL "Build air-infra examples" FORCE)
set(NN_BUILD_EXAMPLE    ${BUILD_EXAMPLE} CACHE BOOL "Build nn-addon examples" FORCE)
set(FHE_BUILD_EXAMPLE   ${BUILD_EXAMPLE} CACHE BOOL "Build fhe-cmplr examples" FORCE)

# Build options — inherit from top-level if defined, default OFF
if(NOT DEFINED BUILD_UNITTEST)
  set(BUILD_UNITTEST    OFF CACHE BOOL "Build unit tests")
endif()
if(NOT DEFINED BUILD_BENCH)
  set(BUILD_BENCH       OFF CACHE BOOL "Build benchmarks")
endif()
if(NOT DEFINED BUILD_EXAMPLE)
  set(BUILD_EXAMPLE     OFF CACHE BOOL "Build examples")
endif()
# BUILD_WITH_OPENMP: inherit top-level default=ON (pyproject.toml cmake.define).
# FORCE is required because fhe-cmplr/CMakeLists.txt declares
# option(BUILD_WITH_OPENMP "..." OFF) whose default would otherwise override
# the inherited value. When OFF, libFHErt_ant.so / libFHErt_common.so compile
# Rtm_stack as a plain global (non-TLS), which then mismatches the
# -fopenmp TLS reference emitted by the JIT-compiled kernel .so (antlib.py),
# failing the kernel build. Keep ON so the backend libs and the kernel agree
# on TLS.
if(NOT DEFINED BUILD_WITH_OPENMP)
  set(BUILD_WITH_OPENMP ON CACHE BOOL "Build with OpenMP" FORCE)
else()
  set(BUILD_WITH_OPENMP ${BUILD_WITH_OPENMP} CACHE BOOL "Build with OpenMP" FORCE)
endif()
set(FHE_BUILD_TEST      OFF CACHE BOOL "Disable FHE test build")

# Backend enable — map from top-level WITH_* to fhe-cmplr FHE_ENABLE_*
# fhe_lib/ backends (phantom, ace) replace the corresponding rtlib backends,
# so when WITH_PHANTOM=ON we set FHE_ENABLE_PHANTOM=OFF to avoid symbol conflicts.
# FORCE is needed because fhe-cmplr/CMakeLists.txt declares option() with
# different defaults that would otherwise take precedence.
set(FHE_ENABLE_SEAL     OFF CACHE BOOL "Disable SEAL in integrated build" FORCE)
set(FHE_ENABLE_OPENFHE  OFF CACHE BOOL "Disable OpenFHE in integrated build" FORCE)
set(FHE_ENABLE_PHANTOM  OFF CACHE BOOL "Disable Phantom rtlib (replaced by fhe_lib/phantom)" FORCE)
set(FHE_ENABLE_ACE      OFF CACHE BOOL "Disable ACE rtlib (replaced by fhe_lib/ace)" FORCE)

FetchContent_MakeAvailable(${DEP_FHE_NAME})

# ---------------------------------------------------------------------------
# Install headers
# ---------------------------------------------------------------------------
install(DIRECTORY ${air-infra_SOURCE_DIR}/include/
        DESTINATION ace/include/backend
        COMPONENT core
)

install(DIRECTORY ${nn-addon_SOURCE_DIR}/include/
        DESTINATION ace/include/backend
        COMPONENT core
)

install(DIRECTORY ${fhe-cmplr_SOURCE_DIR}/include/
        DESTINATION ace/include/backend
        COMPONENT core
)

install(DIRECTORY ${fhe-cmplr_SOURCE_DIR}/rtlib/include/
        DESTINATION ace/include/lib_ant
        COMPONENT core
)

install(DIRECTORY ${fhe-cmplr_SOURCE_DIR}/rtlib/ant/include/
        DESTINATION ace/include/lib_ant/ant
        COMPONENT core
)

install(FILES ${CMAKE_BINARY_DIR}/rtlib/build/_deps/uthash-src/src/uthash.h
        DESTINATION ace/include/lib_ant/ant
        COMPONENT core
)

# install(TARGETS FHErt_common LIBRARY DESTINATION ${ACE_EXTENSION}/lib)
# install(TARGETS FHErt_ant LIBRARY DESTINATION ${ACE_EXTENSION}/lib)
