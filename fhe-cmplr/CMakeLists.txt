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
option (BUILD_BENCH         "Set to ON to build third-party benmark default"                ON)
option (BUILD_DOC           "Set to OFF to disable Build documentation default"            OFF)
option (FHE_CODE_CHECK      "Set to ON to check C/C++ code style default"                   ON)
option (FHE_WITH_SRC        "Set to ON to build with fhe-cmplr default"                     ON)
option (FHE_BUILD_TEST      "Set to ON to build test example default"                       ON)
option (FHE_BUILD_EXAMPLE   "Set to ON to build test example default"                       ON)
option (FHE_INSTALL_APP     "Set to ON to build install package default"                    ON)
option (BUILD_WITH_OPENMP   "Set to ON to enable build with OpenMP"                        OFF)
option (BUILD_WITH_SEAL     "Set to ON to enable build to support SEAL"                    OFF)
option (BUILD_WITH_OPENFHE  "Set to ON to enable build to support OpenFHE"                 OFF)

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
message (STATUS "BUILD_TIMESTAMP              : ${BUILD_TIMESTAMP}")
message (STATUS "BUILD_WITH_OPENMP            : ${BUILD_WITH_OPENMP}")
message (STATUS "BUILD_WITH_SEAL              : ${BUILD_WITH_SEAL}")
message (STATUS "BUILD_WITH_OPENFHE           : ${BUILD_WITH_OPENFHE}")

#--------------------------------------------------------------------
# Logic: Configure
#--------------------------------------------------------------------
# for build with src/libs
# -DFHE_WITH_SRC="air-infra;nn-addon"
set (NN_ADDON_PROJECTS "")
if (FHE_WITH_SRC)
  string (FIND "${FHE_WITH_SRC}" "air-infra" CONF_AIR_INFRA)
  string (FIND "${FHE_WITH_SRC}" "nn-addon" CONF_NN_ADDON)
  if (${CONF_AIR_INFRA} GREATER "-1")
    set (NN_WITH_SRC "air-infra" CACHE BOOL "Enable build air-infra with src." FORCE)
    set (AIR_INFRA_PROJECTS ${CMAKE_SOURCE_DIR}/../air-infra)
    include_directories (${AIR_INFRA_PROJECTS}/include)
    include_directories (${CMAKE_BINARY_DIR}/include)
  endif ()
  if (${CONF_NN_ADDON} GREATER "-1")
    set (NN_ADDON_PROJECTS ${CMAKE_SOURCE_DIR}/../nn-addon)
    if (IS_DIRECTORY ${NN_ADDON_PROJECTS})
      add_subdirectory (${NN_ADDON_PROJECTS} nn-addon)
      include_directories (${NN_ADDON_PROJECTS}/include)
    else ()
        message (FATAL_ERROR "MUST MANUALLY CLONE REPO 'NN_ADDON_PROJECTS'=${NN_ADDON_PROJECTS}")
    endif ()
  endif ()
else ()
  find_library (libvar AIRutil)
  if (${libvar} STREQUAL "libvar-NOTFOUND")
    message (FATAL_ERROR "required AIRutil library but not found!")
  else ()
    message (STATUS "AIRutil library found in ${libvar}")
  endif ()
  find_library (libvar  NNutil)
  if (${libvar} STREQUAL "libvar-NOTFOUND")
    message (FATAL_ERROR "required NNutil library but not found!")
  else ()
    message (STATUS "NNutil library found in ${libvar}")
  endif ()
endif ()

# for check coding style
if (FHE_CODE_CHECK)
  include (cmake/external_regex.cmake)
endif ()

if (BUILD_WITH_OPENMP)
  find_package(OpenMP)
	# OpenMP_CXX_FOUND was added in cmake 3.9.x so we are also checking
  # the OpenMP_FOUND flag
	if (OpenMP_CXX_FOUND OR OpenMP_FOUND)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
	else()
		message(SEND_ERROR "** ERROR ** OpenMP is not installed")
	endif()

	if (OpenMP_C_FOUND OR OpenMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
	endif()
else()
	# Disable unknown #pragma omp warning
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas")
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unknown-pragmas")
endif()

#--------------------------------------------------------------------
# Logic: Target
#--------------------------------------------------------------------
# build fhe
add_custom_target (allfhe)

# find depend libs for protobuf, gmp, m
include (cmake/external_extralib.cmake)

if (BUILD_STATIC)
  set (AIR_LIBS ${AIR_LIBS} PUBLIC AIRdriver PUBLIC AIRopt PUBLIC AIRcg PUBLIC AIRcore PUBLIC AIRbase PUBLIC AIRutil)
  set (NN_LIBS ${NN_LIBS} PUBLIC NNdriver PUBLIC NNvector PUBLIC NNonnx2air PUBLIC NNopt PUBLIC NNonnx PUBLIC NNcore PUBLIC NNutil)
  set (NN_LIBS ${NN_LIBS} ${AIR_LIBS})
  set (FHE_LIBS ${FHE_LIBS} PUBLIC FHEdriver PUBLIC FHEpoly PUBLIC FHEckks PUBLIC FHEsihe PUBLIC FHEcg PUBLIC FHEopt PUBLIC FHEcore PUBLIC FHEutil)
  set (FHE_LIBS ${FHE_LIBS} ${NN_LIBS})
  set (FHE_LIBS ${FHE_LIBS} PUBLIC FHErt_ant_encode FHErt_common ${MATH_LIBS})
  add_dependencies (allfhe FHEdriver FHEpoly FHEckks FHEsihe FHEcg FHEopt FHEcore FHEutil)
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
