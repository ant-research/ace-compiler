# build unittest
if (BUILD_UNITTEST)
  if(NOT TARGET gtest)
    add_subdirectory (${PROJECT_SOURCE_DIR}/third-party/googletest EXCLUDE_FROM_ALL)
    include_directories (${PROJECT_SOURCE_DIR}/third-party/googletest/googletest)
    include_directories (${PROJECT_SOURCE_DIR}/third-party/googletest/googletest/include)
  endif ()
endif()

# build benchmark
if (BUILD_BENCH)
  if(NOT TARGET benchmark)
    set (BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Enable testing of the benchmark library." FORCE)
    set (BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "Enable installation of benchmark. (Projects embedding benchmark may want to turn this OFF.)" FORCE)
    set (BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "Enable building the unit test which depend on gtest" FORCE)
    add_subdirectory (${PROJECT_SOURCE_DIR}/third-party/benchmark EXCLUDE_FROM_ALL)
  endif ()
endif ()

if (BUILD_TCM)
  set (BUILD_TESTING OFF CACHE BOOL "Disable build test of the gperftools library." FORCE)
  set (GPERFTOOLS_BUILD_STATIC OFF CACHE BOOL "Disable build static lib of the gperftools library." FORCE)
  add_subdirectory (${PROJECT_SOURCE_DIR}/third-party/gperftools)

  install (
    FILES ${PROJECT_SOURCE_DIR}/third-party/gperftools/src/pprof
    PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
    DESTINATION ${CMAKE_INSTALL_BINDIR})
endif ()

if (BUILD_STATIC)
	set (AIR_UTLIBS ${AIR_BMLIBS} PUBLIC ${AIR_LIBS} PUBLIC gtest)
	set (AIR_BMLIBS ${AIR_BMLIBS} PUBLIC ${AIR_LIBS} PUBLIC benchmark)
endif()