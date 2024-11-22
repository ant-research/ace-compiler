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

if (BUILD_STATIC)
	set (NN_UTLIBS ${NN_UTLIBS} PUBLIC ${NN_LIBS} PUBLIC gtest)
	set (NN_BMLIBS ${NN_BMLIBS} PUBLIC ${NN_LIBS} PUBLIC benchmark)
endif()