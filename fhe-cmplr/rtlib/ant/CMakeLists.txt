file (GLOB_RECURSE RT_ANT_SRC_FILES CONFIGURE_DEPENDS
      src/ckks/*.c
      src/poly/*.c
      src/rtlib/*.c
      src/util/*.c )
file (GLOB_RECURSE RT_ANT_ENCODE_FILES CONFIGURE_DEPENDS
      src/ckks/plain_eval.c
      src/encode/encode_context.c
      src/util/bit_operations.c
      src/util/ckks_encoder.c
      src/util/ckks_parameters.c
      src/util/crt.c
      src/util/fhe_std_parms.c
      src/util/fhe_types.c
      src/util/ntt.c
      src/util/number_theory.c
      src/util/polynomial.c
      src/util/random_sample.c
      src/util/prng.c )

set (BLAKE2_DIR "${PROJECT_SOURCE_DIR}/third-party/BLAKE2/ref/")
file (GLOB_RECURSE BLAKE2_SRC_FILES CONFIGURE_DEPENDS ${BLAKE2_DIR}/blake2b-ref.c ${BLAKE2_DIR}/blake2xb-ref.c)

set (RT_ANT_INCLUDE_DIRS "")
list (APPEND RT_ANT_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
list (APPEND RT_ANT_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/third-party/uthash/include")
list (APPEND RT_ANT_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")
list (APPEND RT_ANT_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/third-party/BLAKE2/ref")
include_directories (${RT_ANT_INCLUDE_DIRS})
set (RT_ANT_INCLUDE_DIRS "${RT_ANT_INCLUDE_DIRS}" CACHE INTERNAL "")

add_library (fhert_ant_obj OBJECT ${RT_ANT_SRC_FILES} ${BLAKE2_SRC_FILES})
set_property (TARGET fhert_ant_obj PROPERTY POSITION_INDEPENDENT_CODE 1)

if (BUILD_STATIC)
	add_library (FHErt_ant STATIC $<TARGET_OBJECTS:fhert_ant_obj>)
	set_property (TARGET FHErt_ant PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/rtlib/lib)
	install (TARGETS FHErt_ant EXPORT FHETargets DESTINATION rtlib/lib)

	add_library (FHErt_ant_encode STATIC ${RT_ANT_ENCODE_FILES} ${BLAKE2_SRC_FILES})
	set_property (TARGET FHErt_ant_encode PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
	install (TARGETS FHErt_ant_encode EXPORT FHETargets DESTINATION lib)
endif ()

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION rtlib/include/rt_ant)
install(FILES ${CMAKE_SOURCE_DIR}/third-party/uthash/include/uthash.h DESTINATION rtlib/include)

add_custom_target (fhert_ant_all)

if (BUILD_STATIC)
	set (FHERT_ANT_LIB ${FHERT_ANT_LIB} PUBLIC FHErt_ant FHErt_common)
	add_dependencies (fhert_ant_all FHErt_ant)
endif()

set (FHERT_ANT_UTAPP "")
if (BUILD_UNITTEST)
	file (GLOB FHERT_ANT_UNITTEST_SRC_FILES CONFIGURE_DEPENDS unittest/*.cxx)
	set (FHERT_ANT_UNITTEST_SRC_FILES ${FHERT_ANT_UNITTEST_SRC_FILES})
	add_executable (ut_fhert_ant ${FHERT_ANT_UNITTEST_SRC_FILES} ${UNITTESTMAIN})
	set_property (TARGET ut_fhert_ant PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/unittest)
	target_link_libraries (ut_fhert_ant ${FHERT_ANT_LIB} ${FHE_UTLIBS} ${MATH_LIBS})
	set (FHERT_ANT_UTAPP ${FHERT_ANT_UTAPP} ut_fhert_ant)

	add_dependencies (fhert_ant_all ut_fhert_ant)

	add_custom_command (OUTPUT run_fhert_ant_utapp WORKING_DIRECTORY ${CMAKE_BINARY_DIR} COMMAND ${CMAKE_BINARY_DIR}/unittest/ut_fhert_ant)
	add_custom_target (test_fhert_ant_ut DEPENDS ut_fhert_ant run_fhert_ant_utapp)
	add_test( NAME ut_fhert_ant COMMAND ${CMAKE_BINARY_DIR}/unittest/ut_fhert_ant)
endif()

if (FHE_BUILD_TEST)
        add_executable (test_ant_encode ${CMAKE_CURRENT_SOURCE_DIR}/test/test_ant_encode.c)
        target_include_directories (test_ant_encode PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ant/include)
        target_link_libraries (test_ant_encode FHErt_ant_encode FHErt_common ${MATH_LIBS})
        set_property (TARGET test_ant_encode PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test)
        add_test (NAME test_ant_encode COMMAND ${CMAKE_BINARY_DIR}/test/test_ant_encode)
endif ()

set (FHERT_ANT_EGAPP "")
if (FHE_BUILD_EXAMPLE)
	file (GLOB FHERT_ANT_EXAMPLE_SRC_FILES CONFIGURE_DEPENDS example/*.c example/*.cxx)
	foreach (app ${FHERT_ANT_EXAMPLE_SRC_FILES})
		get_filename_component (exe ${app} NAME_WE)
		add_executable (${exe} ${app})
		set_property (TARGET ${exe} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/example)
		set (FHERT_ANT_EGAPP ${FHERT_ANT_EGAPP} ${exe})
		target_link_libraries (${exe} ${FHERT_ANT_LIB} ${NN_LIBS} ${MATH_LIBS})
		add_test( NAME ${exe} COMMAND ${CMAKE_BINARY_DIR}/example/${exe})
	endforeach ()

	add_custom_target (fhert_ant_example)
	add_dependencies (fhert_ant_example ${FHERT_ANT_EGAPP})
	add_dependencies (fhert_ant_all fhert_ant_example)
endif ()

if (FHE_BUILD_EXAMPLE)
        file (GLOB FHERT_ANT_DATASET_SRC_FILES CONFIGURE_DEPENDS dataset/*.cxx)
        foreach (app ${FHERT_ANT_DATASET_SRC_FILES})
                get_filename_component (exe ${app} NAME_WE)
		set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fopenmp")
		set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
                add_executable (${exe} ${app})
                set_property (TARGET ${exe} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/dataset)
		target_link_options (${exe} BEFORE PUBLIC -fopenmp)
                target_link_libraries (${exe} ${FHERT_ANT_LIB} ${NN_LIBS} ${MATH_LIBS})
        endforeach ()
endif ()

if (FHE_INSTALL_APP)
	install (TARGETS ${FHERT_ANT_UTAPP} RUNTIME DESTINATION unittest)
	install (TARGETS ${FHERT_ANT_EGAPP} RUNTIME DESTINATION example)
endif ()
