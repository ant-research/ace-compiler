set (JSONCPP_WITH_TESTS OFF CACHE BOOL "Disable tests of the jsoncpp library." FORCE)
set (JSONCPP_WITH_POST_BUILD_UNITTEST OFF CACHE BOOL "Disable unittest of the jsoncpp library." FORCE)
add_subdirectory (${PROJECT_SOURCE_DIR}/third-party/jsoncpp EXCLUDE_FROM_ALL)
include_directories (${PROJECT_SOURCE_DIR}/third-party/jsoncpp/include)