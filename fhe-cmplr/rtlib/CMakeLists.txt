file (GLOB_RECURSE RTLIB_COMMON_SRC_FILES CONFIGURE_DEPENDS common/src/*.c)

set (FHERTLIB_INCLUDE_DIRS "")
list (APPEND FHERTLIB_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
list (APPEND FHERTLIB_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories (${FHERTLIB_INCLUDE_DIRS})
set (FHERTLIB_INCLUDE_DIRS "${FHERTLIB_INCLUDE_DIRS}" CACHE INTERNAL "")

add_definitions(-DOPENFHE_COMPAT)
add_library (fhert_common_obj OBJECT ${RTLIB_COMMON_SRC_FILES})
set_property (TARGET fhert_common_obj PROPERTY POSITION_INDEPENDENT_CODE 1)

if (BUILD_STATIC)
	add_library (FHErt_common STATIC $<TARGET_OBJECTS:fhert_common_obj>)
	set_property (TARGET FHErt_common PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/rtlib/lib)
	install (TARGETS FHErt_common EXPORT FHETargets DESTINATION rtlib/lib)
endif ()

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION rtlib/include)

set (FHERT_COMMON_UTAPP "")
if (BUILD_UNITTEST)
	file (GLOB FHERT_COMMON_UNITTEST_SRC_FILES CONFIGURE_DEPENDS common/unittest/*.cxx)
	set (FHERT_COMMON_UNITTEST_SRC_FILES ${FHERT_COMMON_UNITTEST_SRC_FILES})
	add_executable (ut_fhert_common ${FHERT_COMMON_UNITTEST_SRC_FILES} ${UNITTESTMAIN})
	set_property (TARGET ut_fhert_common PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/unittest)
	target_link_libraries (ut_fhert_common PUBLIC FHErt_common ${FHE_UTLIBS} ${MATH_LIBS})
	set (FHERT_COMMON_UTAPP ${FHERT_COMMON_UTAPP} ut_fhert_common)

	add_custom_command (OUTPUT run_fhert_common_utapp WORKING_DIRECTORY ${CMAKE_BINARY_DIR} COMMAND ${CMAKE_BINARY_DIR}/unittest/ut_fhert_common)
	add_custom_target (test_fhert_common_ut DEPENDS ut_fhert_common run_fhert_common_utapp)
	# add_test (NAME ut_fhert_common COMMAND ${CMAKE_BINARY_DIR}/unittest/ut_fhert_common)		# temp rm with FHERT_COMMON.PT_MGR
endif ()

add_subdirectory (ant)

if (BUILD_WITH_SEAL)
  add_subdirectory (seal)
endif ()

if (BUILD_WITH_OPENFHE)
  add_subdirectory (openfhe)
endif ()

if (FHE_INSTALL_APP)
	install (TARGETS ${FHERT_COMMON_UTAPP} RUNTIME DESTINATION unittest)
endif ()
