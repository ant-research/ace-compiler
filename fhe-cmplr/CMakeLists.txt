cmake_minimum_required(VERSION 3.13)

project (fhe-cmplr C CXX)

set (FHE_CMPLR_VERSION_MAJOR 0)
set (FHE_CMPLR_VERSION_MINOR 0)
set (FHE_CMPLR_VERSION_PATCH 1)
set (FHE_CMPLR_VERSION ${FHE_CMPLR_VERSION_MAJOR}.${FHE_CMPLR_VERSION_MINOR}.${FHE_CMPLR_VERSION_PATCH})

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
    message (FATAL_ERROR "** ERROR ** CMAKE_BUILD_TYPE must be : Debug|Release")
  endif ()
else ()
    # if no build type is chosen, default to Debug mode
	set (CMAKE_BUILD_TYPE Debug CACHE STRING
		"Choose the type of build, options are: None, Debug, Release." FORCE)
endif ()

string (TIMESTAMP BUILD_TIMESTAMP "%Y-%m-%d %H:%M:%S")
include (cmake/revision.cmake)

option (BUILD_STATIC        "Set to ON to build static versions of the library"             ON)
option (BUILD_SHARED        "Set to OFF to disable build shared versions of the library"   OFF)
option (BUILD_UNITTEST      "Set to ON to disable build third-party unittest default"       ON)
option (BUILD_BENCH         "Set to ON to build third-party benmark default"                ON)
option (BUILD_DOC           "Set to OFF to disable Build documentation default"            OFF)
option (FHE_CODE_CHECK      "Set to ON to check C/C++ code style default"                   ON)
option (FHE_WITH_SRC        "Set to OFF to build with fhe-cmplr default"                   OFF)
option (FHE_BUILD_TEST      "Set to ON to build test example default"                       ON)
option (FHE_BUILD_EXAMPLE   "Set to ON to build test example default"                       ON)
option (FHE_INSTALL_APP     "Set to ON to build install package default"                    ON)
option (BUILD_WITH_OPENMP   "Set to ON to enable build with OpenMP"                        OFF)

### Print options
message (STATUS "CMAKE_CURRENT_SOURCE_DIR     : ${CMAKE_CURRENT_SOURCE_DIR}")
message (STATUS "CMAKE_BUILD_TYPE             : ${CMAKE_BUILD_TYPE}")
message (STATUS "BUILD_STATIC                 : ${BUILD_STATIC}")
message (STATUS "BUILD_SHARED                 : ${BUILD_SHARED}")
message (STATUS "BUILD_UNITTEST               : ${BUILD_UNITTEST}")
message (STATUS "BUILD_BENCH                  : ${BUILD_BENCH}")
message (STATUS "BUILD_DOC                    : ${BUILD_DOC}")
message (STATUS "FHE_CODE_CHECK               : ${FHE_CODE_CHECK}")
message (STATUS "FHE_WITH_SRC                 : ${FHE_WITH_SRC}")
message (STATUS "FHE_BUILD_TEST               : ${FHE_BUILD_TEST}")
message (STATUS "FHE_BUILD_EXAMPLE            : ${FHE_BUILD_EXAMPLE}")
message (STATUS "FHE_INSTALL_APP              : ${FHE_INSTALL_APP}")
message (STATUS "BUILD_WITH_OPENMP            : ${BUILD_WITH_OPENMP}")
message (STATUS "BUILD_TIMESTAMP              : ${BUILD_TIMESTAMP}")
message (STATUS "BUILD_GIT_COMMIT             : ${FHE_GIT_COMMIT}")

#--------------------------------------------------------------------
# Logic: Configure
#--------------------------------------------------------------------
# for configure options

# for check coding style
if (FHE_CODE_CHECK)
  include (cmake/external_regex.cmake)
endif ()

# find depend libs for air, nn
include (cmake/external_deplib.cmake)

# find extra libs for protobuf, gmp, m, openmp
include (cmake/external_extralib.cmake)

#--------------------------------------------------------------------
# Logic: Target
#--------------------------------------------------------------------
# build fhe

# add target fhe_depend for generated source
add_custom_target (fhe_depend DEPENDS ${FHE_DEP_LIBS})

# add glob depends for allfhe
add_custom_target (allfhe DEPENDS fhe_depend)

if (BUILD_STATIC)
  set (FHE_LIBS ${FHE_LIBS} PUBLIC FHEdriver FHEpoly FHEckks FHEsihe FHEcg FHEopt FHEcore FHEutil)
  set (FHE_LIBS ${FHE_LIBS} ${FHE_DEP_LIBS})
  set (FHE_LIBS ${FHE_LIBS} FHErt_ant_encode FHErt_common ${MATH_LIBS})
endif()

# build depend libs for test
include (cmake/external_testdep.cmake)

if (FHE_BUILD_TEST)
  enable_testing()
endif ()

# build component
add_subdirectory (util)
add_subdirectory (core)
add_subdirectory (opt)
add_subdirectory (cg)
add_subdirectory (sihe)
add_subdirectory (ckks)
add_subdirectory (poly)

# build executable
add_subdirectory (driver)

# build runtime lib
add_subdirectory (rtlib)

# build benchmark
if (BUILD_BENCH)
  add_subdirectory (benchmark)
endif ()

# build e2e test
if (FHE_BUILD_TEST)
  add_subdirectory (test)
endif ()

# build doc
if (BUILD_DOC)
  include (cmake/external_doxgen.cmake)
endif ()

#--------------------------------------------------------------------
# Logic: Package
#--------------------------------------------------------------------
# install header file
install(DIRECTORY include/ DESTINATION include)

# install testcase dict for cti
install(FILES ${PROJECT_SOURCE_DIR}/test/data_avhc_fhe.yml DESTINATION test)
