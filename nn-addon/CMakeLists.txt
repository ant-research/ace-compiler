cmake_minimum_required (VERSION 3.13)

project (nn-addon C CXX)

set (NN_ADDON_VERSION_MAJOR 0)
set (NN_ADDON_VERSION_MINOR 0)
set (NN_ADDON_VERSION_PATCH 1)
set (NN_ADDON_VERSION ${NN_ADDON_VERSION_MAJOR}.${NN_ADDON_VERSION_MINOR}.${NN_ADDON_VERSION_PATCH})

set (CMAKE_CXX_STANDARD 17)
set (CXX_STANDARD_REQUIRED ON)

#--------------------------------------------------------------------
# Build options
#--------------------------------------------------------------------
# -DCMAKE_BUILD_TYPE=Debug/Release
if (CMAKE_BUILD_TYPE)
  set (RELEASE_TYPES Debug Release)
  list (FIND RELEASE_TYPES ${CMAKE_BUILD_TYPE} INDEX_FOUND)
  if (${INDEX_FOUND} EQUAL -1)
    message (FATAL_ERROR "CMAKE_BUILD_TYPE must be one of Debug, Release")
  endif ()
else ()
    # if no build type is chosen, default to Debug mode
	set (CMAKE_BUILD_TYPE Debug CACHE STRING
		"Choose the type of build, options are: None, Debug, Release." FORCE)
endif ()

string (TIMESTAMP BUILD_TIMESTAMP "%Y-%m-%d %H:%M:%S")

option (BUILD_STATIC      "Set to ON to build static versions of the library"             ON)
option (BUILD_SHARED      "Set to OFF to disable build shared versions of the library"   OFF)
option (BUILD_UNITTEST    "Set to ON to build third-party unittests default"              ON)
option (BUILD_BENCH       "Set to ON to build third-party benmark default"                ON)
option (BUILD_DOC         "Set to OFF to disable Build documentation default"            OFF)
option (NN_CODE_CHECK     "Set to ON to check C/C++ code style default"                   ON)
option (NN_WITH_SRC       "Set to ON to build with air-infra default"                     ON)
option (NN_BUILD_ONNX     "Set to ON to build test example defualt"                       ON)
option (NN_BUILD_TEST     "Set to ON to build test example defualt"                       ON)
option (NN_BUILD_EXAMPLE  "Set to ON to build test example defualt"                       ON)
option (NN_INSTALL_APP    "Set to ON to build install package defualt"                    ON)

### Print options
message (STATUS "CMAKE_CURRENT_SOURCE_DIR     : ${CMAKE_CURRENT_SOURCE_DIR}")
message (STATUS "CMAKE_BUILD_TYPE             : ${CMAKE_BUILD_TYPE}")
message (STATUS "BUILD_STATIC                 : ${BUILD_STATIC}")
message (STATUS "BUILD_SHARED                 : ${BUILD_SHARED}")
message (STATUS "BUILD_UNITTEST               : ${BUILD_UNITTEST}")
message (STATUS "BUILD_BENCH                  : ${BUILD_BENCH}")
message (STATUS "BUILD_DOC                    : ${BUILD_DOC}")
message (STATUS "NN_CODE_CHECK                : ${NN_CODE_CHECK}")
message (STATUS "NN_WITH_SRC                  : ${NN_WITH_SRC}")
message (STATUS "NN_BUILD_ONNX                : ${NN_BUILD_ONNX}")
message (STATUS "NN_BUILD_TEST                : ${NN_BUILD_TEST}")
message (STATUS "NN_BUILD_EXAMPLE             : ${NN_BUILD_EXAMPLE}")
message (STATUS "NN_INSTALL_APP               : ${NN_INSTALL_APP}")
message (STATUS "BUILD_TIMESTAMP              : ${BUILD_TIMESTAMP}")

#--------------------------------------------------------------------
# Logic: Configure
#--------------------------------------------------------------------
# for build with src/libs
# -DNN_WITH_SRC="air-infra"
if (NN_WITH_SRC)
  string (FIND "${NN_WITH_SRC}" "air-infra" CONF_AIR_INFRA)
  if (${CONF_AIR_INFRA} GREATER "-1")
    set (AIR_INFRA_PROJECTS ${CMAKE_SOURCE_DIR}/../air-infra)
    if (IS_DIRECTORY ${AIR_INFRA_PROJECTS})
      add_subdirectory (${AIR_INFRA_PROJECTS} air-infra)
      include_directories (${AIR_INFRA_PROJECTS}/include)
      include_directories (${CMAKE_BINARY_DIR}/include)
    else ()
      message (FATAL_ERROR "MUST MANUALLY CLONE REPO 'AIR_INFRA_PROJECTS'=${AIR_INFRA_PROJECTS}")
    endif ()
  endif ()
else ()
  find_library (libvar  AIRutil)
  if (${libvar} STREQUAL "libvar-NOTFOUND")
    message (FATAL_ERROR "required AIRutil library but not found!")
  else ()
    message (STATUS "AIRutil library found in ${libvar}")
  endif ()
endif ()

# for check coding style
if (NN_CODE_CHECK)
  include (cmake/external_regex.cmake)
endif ()

# for generate onnx.proto
include (cmake/external_protogen.cmake)
#--------------------------------------------------------------------
# Logic: Target
#--------------------------------------------------------------------
# build nn
add_custom_target (allnn)

if (BUILD_STATIC)
  set (AIR_LIBS ${AIR_LIBS} PUBLIC AIRdriver PUBLIC AIRopt PUBLIC AIRcg PUBLIC AIRcore PUBLIC AIRbase PUBLIC AIRutil)
  set (NN_LIBS ${NN_LIBS} PUBLIC NNdriver PUBLIC NNvector PUBLIC NNonnx2air PUBLIC NNopt PUBLIC NNonnx PUBLIC NNcore PUBLIC NNutil ${AIR_LIBS})
  add_dependencies (allnn NNdriver NNopt NNvector NNonnx2air NNonnx NNcore NNutil)
endif ()

# build depend libs for test
include (cmake/external_testdep.cmake)

if (NN_BUILD_TEST)
  enable_testing()
endif ()

# build component libs
add_subdirectory (util)
add_subdirectory (core)
add_subdirectory (onnx)
add_subdirectory (onnx2air)
add_subdirectory (llama)
add_subdirectory (vector)
add_subdirectory (opt)

# build component executable
add_subdirectory (driver)

# build benchmark
if (BUILD_BENCH)
  add_subdirectory (benchmark)
endif ()

# build e2e test
if (NN_BUILD_TEST)
  add_subdirectory (test)
endif ()

# build docs
if (BUILD_DOC)
  include (cmake/external_doxygen.cmake)
endif ()

#--------------------------------------------------------------------
# Logic: Package
#--------------------------------------------------------------------
# install header file
install (DIRECTORY include/ DESTINATION include)

# install testcase dict for cti
install (FILES ${PROJECT_SOURCE_DIR}/test/data_avhc_nn.yml DESTINATION test)
