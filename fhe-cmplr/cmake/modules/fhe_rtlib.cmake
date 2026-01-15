#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

# Build FHE_RTLIB project dependent function
function(build_rtlib)
  include(ExternalProject)

  # Passing a list of include files requires a custom transformation
  string(JOIN "|" PACKAGE_INC_DIR_TF ${PACKAGE_INC_DIR})

  ExternalProject_Add(
    fhe_rtlib
    PREFIX ${CMAKE_BINARY_DIR}/rtlib
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/rtlib
    BINARY_DIR ${CMAKE_BINARY_DIR}/rtlib/build
    CMAKE_ARGS -G "Unix Makefiles"
              -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
              -DBUILD_STATIC=${BUILD_STATIC}
              -DBUILD_UNITTEST=${BUILD_UNITTEST}
              -DBUILD_BENCH=${BUILD_BENCH}
              -DRTLIB_BUILD_TEST=${FHE_BUILD_TEST}
              -DRTLIB_BUILD_EXAMPLE=${FHE_BUILD_EXAMPLE}
              -DRTLIB_INSTALL_APP=${FHE_INSTALL_APP}
              -DRTLIB_ENABLE_SEAL=${FHE_ENABLE_SEAL}
              -DRTLIB_ENABLE_SEAL_BTS=${FHE_ENABLE_SEAL_BTS}
              -DRTLIB_ENABLE_OPENFHE=${FHE_ENABLE_OPENFHE}
              -DRTLIB_ENABLE_CUDA=${FHE_ENABLE_CUDA}
              -DRTLIB_SUPPORT_HPU=${FHE_SUPPORT_HPU}
              -DRTLIB_SEED_MODE=${FHE_SEED_MODE}
              -DBUILD_WITH_OPENMP=${BUILD_WITH_OPENMP}
              -DPACKAGE_BASE_DIR=$ENV{PACKAGE_BASE_DIR}
              -DPACKAGE_INC_DIR_TF=${PACKAGE_INC_DIR_TF}
              -DEXTERNAL_URL_SSH=${EXTERNAL_URL_SSH}
              -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    UPDATE_COMMAND ""
    BUILD_ALWAYS ON
    BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --config ${CMAKE_BUILD_TYPE}
    TEST_COMMAND  "" # ${CMAKE_CTEST_COMMAND} --output-on-failure
    INSTALL_COMMAND ""
    BUILD_BYPRODUCTS  <BINARY_DIR>/common/libFHErt_common.a
                      <BINARY_DIR>/ant/libFHErt_ant.a
                      <BINARY_DIR>/ant/libFHErt_ant_encode.a
                      <BINARY_DIR>/seal/libFHErt_seal.a
                      <BINARY_DIR>/openfhe/libFHErt_openfhe.a
                      <BINARY_DIR>/phantom/libFHErt_phantom.a
  )
  ExternalProject_Get_Property(fhe_rtlib SOURCE_DIR BINARY_DIR)

  add_library(FHErt_common STATIC IMPORTED GLOBAL)
  set_target_properties(FHErt_common PROPERTIES
    IMPORTED_LOCATION "${BINARY_DIR}/common/libFHErt_common.a"
  )
  add_library(FHErt_ant STATIC IMPORTED GLOBAL)
  set_target_properties(FHErt_ant PROPERTIES
    IMPORTED_LOCATION "${BINARY_DIR}/ant/libFHErt_ant.a"
  )
  add_library(FHErt_ant_encode STATIC IMPORTED GLOBAL)
  set_target_properties(FHErt_ant_encode PROPERTIES
    IMPORTED_LOCATION "${BINARY_DIR}/ant/libFHErt_ant_encode.a"
  )

  # for seal & openfhe & phantom
  if(FHE_ENABLE_SEAL)
    add_library(FHErt_seal STATIC IMPORTED GLOBAL)
    set_target_properties(FHErt_seal PROPERTIES
      IMPORTED_LOCATION "${BINARY_DIR}/seal/libFHErt_seal.a"
    )
    install(FILES ${BINARY_DIR}/seal/libFHErt_seal.a DESTINATION rtlib/lib)
  endif()
  if(FHE_ENABLE_OPENFHE)
    add_library(FHErt_openfhe STATIC IMPORTED GLOBAL)
    set_target_properties(FHErt_openfhe PROPERTIES
      IMPORTED_LOCATION "${BINARY_DIR}/openfhe/libFHErt_openfhe.a"
    )
    install(FILES ${BINARY_DIR}/openfhe/libFHErt_openfhe.a DESTINATION rtlib/lib)
  endif()
  if(FHE_ENABLE_PHANTOM)
    add_library(FHErt_phantom STATIC IMPORTED GLOBAL)
    set_target_properties(FHErt_phantom PROPERTIES
      IMPORTED_LOCATION "${BINARY_DIR}/phantom/libFHErt_phantom.a"
    )
    install(FILES ${BINARY_DIR}/phantom/libFHErt_phantom.a DESTINATION rtlib/lib)
  endif()

  add_test(NAME test_fhe_rtlib COMMAND ${CMAKE_COMMAND} --build ${BINARY_DIR} --target test)

  install(DIRECTORY ${SOURCE_DIR}/include/ DESTINATION rtlib/include)
  install(DIRECTORY ${SOURCE_DIR}/ant/include/ DESTINATION rtlib/include/ant)

  # put it here now, rm it wait for refactor private key
  install(FILES ${SOURCE_DIR}/cmake/modules/uthash/src/uthash.h DESTINATION rtlib/include/ant)

  install(FILES ${BINARY_DIR}/common/libFHErt_common.a DESTINATION rtlib/lib)
  install(FILES ${BINARY_DIR}/ant/libFHErt_ant.a DESTINATION rtlib/lib)
  install(FILES ${BINARY_DIR}/ant/libFHErt_ant_encode.a DESTINATION rtlib/lib)

endfunction()

if(NOT TARGET fhe_rtlib)
  build_rtlib()
  add_dependencies(fhe_depend fhe_rtlib)
  add_dependencies(fhe_rtlib ${NN_LIBS})

  find_library(M_LIBRARY NAMES m)
  find_library(GMP_LIBRARY NAMES gmp libgmp)
  set(FHERT_ANT_LIBS ${FHERT_ANT_LIBS} FHErt_ant_encode FHErt_common ${M_LIBRARY} ${GMP_LIBRARY})
endif()
