cmake_minimum_required (VERSION 3.13)

project (air-infra C CXX)

set (AIR_VERSION_MAJOR 0)
set (AIR_VERSION_MINOR 0)
set (AIR_VERSION_PATCH 1)
set (AIR_VERSION ${AIR_VERSION_MAJOR}.${AIR_VERSION_MINOR}.${AIR_VERSION_PATCH})

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

option (BUILD_STATIC        "Set to ON to build static versions of the library"             ON)
option (BUILD_SHARED        "Set to OFF to disable build shared versions of the library"   OFF)
option (BUILD_UNITTEST      "Set to ON to disable build third-party unittest default"       ON)
option (BUILD_BENCH         "Set to ON to disable build third-party benmark default"        ON)
option (BUILD_DOC           "Set to OFF to disable Build documentation default"            OFF)
option (AIR_CODE_CHECK      "Set to ON to check C/C++ code style default"                   ON)
option (AIR_BUILD_TEST      "Set to ON to build test example default"                       ON)
option (AIR_BUILD_EXAMPLE   "Set to ON to build test example default"                       ON)
option (AIR_INSTALL_APP     "Set to ON to build install package default"                    ON)
option (BUILD_TCM           "Activate tcmalloc by setting BUILD_TCM to ON"                 OFF)

### Print options
message (STATUS "CMAKE_CURRENT_SOURCE_DIR     : ${CMAKE_CURRENT_SOURCE_DIR}")
message (STATUS "CMAKE_BUILD_TYPE             : ${CMAKE_BUILD_TYPE}")
message (STATUS "BUILD_STATIC                 : ${BUILD_STATIC}")
message (STATUS "BUILD_SHARED                 : ${BUILD_SHARED}")
message (STATUS "BUILD_UNITTEST               : ${BUILD_UNITTEST}")
message (STATUS "BUILD_BENCH                  : ${BUILD_BENCH}")
message (STATUS "BUILD_DOC                    : ${BUILD_DOC}")
message (STATUS "AIR_CODE_CHECK               : ${AIR_CODE_CHECK}")
message (STATUS "AIR_BUILD_TEST               : ${AIR_BUILD_TEST}")
message (STATUS "AIR_BUILD_EXAMPLE            : ${AIR_BUILD_EXAMPLE}")
message (STATUS "AIR_INSTALL_APP              : ${AIR_INSTALL_APP}")
message (STATUS "BUILD_TCM                    : ${BUILD_TCM}")
message (STATUS "BUILD_TIMESTAMP              : ${BUILD_TIMESTAMP}")

#--------------------------------------------------------------------
# Logic: Configure
#--------------------------------------------------------------------
# for generate configure
configure_file (
  "${CMAKE_CURRENT_SOURCE_DIR}/config.in"
  "${CMAKE_BINARY_DIR}/include/config.h"
)

# for check coding style
if (AIR_CODE_CHECK)
  include (cmake/external_regex.cmake)
endif ()

# for generate user message
include (cmake/external_errmsg.cmake)

#--------------------------------------------------------------------
# Logic: Target
#--------------------------------------------------------------------
# build air
add_custom_target (allair)

if (BUILD_STATIC)
  set (AIR_LIBS ${AIR_LIBS} PUBLIC AIRplugin AIRcg PUBLIC AIRopt PUBLIC AIRdriver PUBLIC AIRcore PUBLIC AIRbase PUBLIC AIRutil)
	add_dependencies (allair AIRplugin AIRcg AIRopt AIRdriver AIRcore AIRbase AIRutil)
endif ()

# build dependent lib
include (cmake/external_testdep.cmake)
include (cmake/external_jsoncpp.cmake)

if (AIR_BUILD_TEST)
  enable_testing()
endif ()

# build component
add_subdirectory (util)
add_subdirectory (base)
add_subdirectory (core)
add_subdirectory (driver)
add_subdirectory (opt)
add_subdirectory (cg)
add_subdirectory (plugin)

# build benchmark
if (BUILD_BENCH)
  add_subdirectory (benchmark)
endif ()

# build e2e test
if (AIR_BUILD_TEST)
  add_subdirectory (test)
endif ()

# build doc
if (BUILD_DOC)
  include (cmake/external_doxygen.cmake)
endif ()

#--------------------------------------------------------------------
# Logic: Package
#--------------------------------------------------------------------
# install header file
install (DIRECTORY include/ DESTINATION include)

# install testcase dict for cti
install (FILES ${PROJECT_SOURCE_DIR}/test/data_avhc_air.yml DESTINATION test)