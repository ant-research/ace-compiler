#if(CUSTOM_FETCHCONTENT_DIR)
#    set(CUSTOM_HYPERFHE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${DEP_HYPERFHE_NAME})
#    file(MAKE_DIRECTORY ${CUSTOM_HYPERFHE_DIR})
#    set(FETCHCONTENT_SOURCE_DIR_CKKS_INFRA ${CUSTOM_HYPERFHE_DIR})
#endif()

include(FetchContent)

set(CMAKE_CUDA_ARCHITECTURES 80)
FetchContent_Declare(
  ckks_infra
  GIT_REPOSITORY  ${DEP_HYPERFHE_URL}
  GIT_TAG         ${DEP_HYPERFHE_REF}
  # SOURCE_SUBDIR   ckks-gpu
  CMAKE_ARGS
    -DCMAKE_CUDA_FLAGS="LIBCUDACXX_ENABLE_EXPERIMENTAL_MEMORY_RESOURCE"
    -DCMAKE_CXX_FLAGS="LIBCUDACXX_ENABLE_EXPERIMENTAL_MEMORY_RESOURCE"
  #CMAKE_ARGS
  #  -DBUILD_SHARED_LIBS=ON
  #  -DBUILD_BENCH=OFF
  #  -DNTT_ALG="4StepNTTMont"
  #  -DDATA_TYPE=64
)

include_directories(${CMAKE_INSTALL_PREFIX}/rapids_logger/include)

FetchContent_Populate(ckks_infra)

message(STATUS "ckks_infra source: ${ckks_infra_SOURCE_DIR}")
#FetchContent_MakeAvailable(ckks_infra)
add_compile_definitions(LIBCUDACXX_ENABLE_EXPERIMENTAL_MEMORY_RESOURCE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

set(BUILD_STATIC_LIB ON)
set(BUILD_TESTING OFF)
add_subdirectory(${ckks_infra_SOURCE_DIR}/ckks-gpu
	                ${CMAKE_CURRENT_BINARY_DIR}/ckks_gpu
)
add_subdirectory(${ckks_infra_SOURCE_DIR}/ckks-app
	                ${CMAKE_CURRENT_BINARY_DIR}/ckks_app
)

# get_property(ckks_infra_BINARY_DIR DIRECTORY ${ckks_infra_SOURCE_DIR} PROPERTY BINARY_DIR)

set(RMM_SOURCE_DIR ${CMAKE_BINARY_DIR}/_deps/rmm-src)
set(RMM_BINARY_DIR ${CMAKE_BINARY_DIR}/_deps/rmm-build)

install(DIRECTORY ${RMM_SOURCE_DIR}/cpp/include
        DESTINATION ace/include/lib_hyperfhe
)

install(DIRECTORY ${RMM_BINARY_DIR}/include
        DESTINATION ace/include/lib_hyperfhe
)

install(DIRECTORY ${ckks_infra_SOURCE_DIR}/ckks-app/src
        DESTINATION ace/include/lib_hyperfhe
)

install(DIRECTORY ${ckks_infra_SOURCE_DIR}/ckks-app/src/common
        DESTINATION ace/include/lib_hyperfhe
)

install(DIRECTORY ${ckks_infra_SOURCE_DIR}/ckks-gpu/include/public
        DESTINATION ace/include/lib_hyperfhe
)

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/ckks_gpu/include/public
        DESTINATION ace/include/lib_hyperfhe
)
