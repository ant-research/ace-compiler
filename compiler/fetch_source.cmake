#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================


include(FetchContent)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${DEP_AIR_NAME}/CMakeLists.txt")
  FetchContent_Declare(
    ${DEP_AIR_NAME}
    SOURCE_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/${DEP_AIR_NAME}
  )
else()
  FetchContent_Declare(
    ${DEP_AIR_NAME}
    GIT_REPOSITORY  ${DEP_AIR_URL}
    GIT_TAG         ${DEP_AIR_REF}
    SOURCE_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/${DEP_AIR_NAME}
  )
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${DEP_NN_NAME}/CMakeLists.txt")
  FetchContent_Declare(
    ${DEP_NN_NAME}
    SOURCE_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/${DEP_NN_NAME}
  )
else()
  FetchContent_Declare(
    ${DEP_NN_NAME}
    GIT_REPOSITORY  ${DEP_NN_URL}
    GIT_TAG         ${DEP_NN_REF}
    SOURCE_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/${DEP_NN_NAME}
  )
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${DEP_FHE_NAME}/CMakeLists.txt")
  FetchContent_Declare(
    ${DEP_FHE_NAME}
    SOURCE_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/${DEP_FHE_NAME}
  )
else()
  FetchContent_Declare(
    ${DEP_FHE_NAME}
    GIT_REPOSITORY  ${DEP_FHE_URL}
    GIT_TAG         ${DEP_FHE_REF}
    SOURCE_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/${DEP_FHE_NAME}
  )
endif()

FetchContent_Declare(
  ${DEP_RISCV_NAME}
  GIT_REPOSITORY  ${DEP_RISCV_URL}
  GIT_TAG         ${DEP_RISCV_REF}
  SOURCE_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/${DEP_RISCV_NAME}
)

FetchContent_Declare(
  ${DEP_HPU_NAME}
  GIT_REPOSITORY  ${DEP_HPU_URL}
  GIT_TAG         ${DEP_HPU_REF}
  SOURCE_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/${DEP_HPU_NAME}
)


if(COMPILE_MODULE STREQUAL ${DEP_AIR_NAME})
  set(AIR_CODE_CHECK      OFF CACHE BOOL "Disable coding conventions checking")

  FetchContent_MakeAvailable(${DEP_AIR_NAME})
elseif(COMPILE_MODULE STREQUAL ${DEP_NN_NAME})
  FetchContent_Populate(${DEP_AIR_NAME})
  
  set(NN_WITH_SRC "air-infra" CACHE STRING "Set source dependencies")
  set(AIR_CODE_CHECK      OFF CACHE BOOL "Disable coding conventions checking")
  set(NN_CODE_CHECK       OFF CACHE BOOL "Disable coding conventions checking")

  FetchContent_MakeAvailable(${DEP_NN_NAME})
elseif(COMPILE_MODULE STREQUAL ${DEP_FHE_NAME})
  FetchContent_Populate(${DEP_AIR_NAME})
  FetchContent_Populate(${DEP_NN_NAME})

  set(FHE_WITH_SRC "air-infra;nn-addon" CACHE STRING "Set source dependencies")
  set(AIR_CODE_CHECK      OFF CACHE BOOL "Disable coding conventions checking")
  set(NN_CODE_CHECK       OFF CACHE BOOL "Disable coding conventions checking")
  set(FHE_CODE_CHECK      OFF CACHE BOOL "Disable coding conventions checking")

  set(FHE_ENABLE_SEAL     OFF CACHE BOOL "Disable coding conventions checking")
  set(FHE_ENABLE_OPENFHE  OFF CACHE BOOL "Disable coding conventions checking")

  set(BUILD_UNITTEST      OFF CACHE BOOL "Disable unittest build")
  set(BUILD_BENCH         OFF CACHE BOOL "Disable benchmark build")
  set(FHE_BUILD_TEST      OFF CACHE BOOL "Disable FHE test build")
  set(FHE_BUILD_EXAMPLE   OFF CACHE BOOL "Disable FHE example build")

  FetchContent_MakeAvailable(${DEP_FHE_NAME})
elseif(COMPILE_MODULE STREQUAL ${DEP_HPU_NAME})
  FetchContent_Populate(${DEP_AIR_NAME})
  FetchContent_Populate(${DEP_NN_NAME})
  FetchContent_Populate(${DEP_FHE_NAME})
  FetchContent_Populate(${DEP_RISCV_NAME})

  set(HPU_WITH_SRC "air-infra;nn-addon;fhe-cmplr;riscv-cg" CACHE STRING "Set source dependencies")
  set(AIR_CODE_CHECK    OFF CACHE BOOL "Disable coding conventions checking")
  set(NN_CODE_CHECK     OFF CACHE BOOL "Disable coding conventions checking")
  set(FHE_CODE_CHECK    OFF CACHE BOOL "Disable coding conventions checking")
  set(RISCV_CODE_CHECK  OFF CACHE BOOL "Disable coding conventions checking")
  set(HPU_CODE_CHECK    OFF CACHE BOOL "Disable coding conventions checking")

  FetchContent_MakeAvailable(${DEP_HPU_NAME})
else()
  message(FATAL_ERROR "Invalid COMPILE_MODULE='${COMPILE_MODULE}'. Use fhe-cmplr or hpu-cg.")
endif()

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
