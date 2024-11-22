# codes from other parties are excluded from coding style checks
set (CODEING_STYLE_EXCLUDES "third-party")
# exclude unittest source from coding style check if not build
if (NOT BUILD_UNITTEST)
  set (CODEING_STYLE_EXCLUDES ${CODEING_STYLE_EXCLUDES} "unittest")
endif()
# exclude benchmark source from coding style check if not build
if (NOT BUILD_BENCH)
  set (CODEING_STYLE_EXCLUDES ${CODEING_STYLE_EXCLUDES} "benchmark")
endif()
# exclude tests source from coding style check if not build
if (NOT NN_BUILD_TEST)
  set (CODEING_STYLE_EXCLUDES ${CODEING_STYLE_EXCLUDES} "test")
endif()
if (NN_CODE_CHECK)
  set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
  set (CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
  add_custom_target (nn_check_coding_style ALL
                    COMMAND 
                      python3 devtools/check_coding_style.py -b ${CMAKE_BINARY_DIR} -x ${CODEING_STYLE_EXCLUDES}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif ()