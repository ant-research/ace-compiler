cmake_minimum_required(VERSION 3.16)

project(cnn_ckks_project)
set(CMAKE_CXX_COMPILER g++)

option(COMP "Build comparison operation" OFF)
option(BOOT "Build comparison operation" OFF)
option(CNN "Build comparison operation" OFF)
option(TEST "Build comparison operation" OFF)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ./run)
find_package(SEAL 3.6 REQUIRED)


if(COMP)
	add_subdirectory(common)
	add_subdirectory(cpu-ckks/single-key/comp)
	add_executable(compare)
	target_sources(compare
		PRIVATE
			./run/run_compare.cpp
			${COMMON_SOURCE_FILES}
			${COMP_SOURCE_FILES}
	)
	target_compile_options(compare PRIVATE -Wall -O3 -std=c++11 -pthread -march=native)
	target_link_libraries(compare PRIVATE ntl gmp m pthread SEAL::seal)
	target_include_directories(compare PUBLIC /usr/local/include /usr/local/include/NTL ${COMMON_HEADER_DIR} ${COMP_HEADER_DIR})
endif()

if(BOOT)
	add_subdirectory(common)
	add_subdirectory(cpu-ckks/single-key/ckks_bootstrapping)

	add_executable(bootstrapping)
	target_sources(bootstrapping
		PRIVATE
			./run/run_bootstrapping.cpp
			${COMMON_SOURCE_FILES}
			${BOOTSTRAPPING_SOURCE_FILES}
	)
	target_compile_options(bootstrapping PRIVATE -Wall -O0 -std=c++17 -pthread -march=native)
	target_link_libraries(bootstrapping PRIVATE ntl gmp m pthread SEAL::seal)
	target_include_directories(bootstrapping PUBLIC /usr/local/include /usr/local/include/NTL ${COMMON_HEADER_DIR} ${BOOTSTRAPPING_HEADER_DIR})
endif()

if(CNN)
	add_subdirectory(common)
	add_subdirectory(cpu-ckks/single-key/cnn)
	add_subdirectory(cpu-ckks/single-key/comp)
	add_subdirectory(cpu-ckks/single-key/ckks_bootstrapping)
	add_executable(cnn)
	target_sources(cnn
		PRIVATE
			./run/run_cnn.cpp
			${COMMON_SOURCE_FILES}
			${CNN_SOURCE_FILES}
			${COMP_SOURCE_FILES}
			${BOOTSTRAPPING_SOURCE_FILES}
	)
	target_compile_options(cnn PRIVATE -w -O0 -std=c++17 -pthread -march=native -fopenmp)
	target_link_libraries(cnn PRIVATE ntl gmp m pthread SEAL::seal gomp)
	target_include_directories(cnn PUBLIC /usr/include /usr/local/include /usr/local/include/NTL ${COMMON_HEADER_DIR} ${CNN_HEADER_DIR} ${COMP_HEADER_DIR} ${BOOTSTRAPPING_HEADER_DIR})
	# target_include_directories(cnn PUBLIC /usr/include /usr/local/include /usr/local/include/NTL ${COMMON_HEADER_DIR} ${CNN_HEADER_DIR} ${COMP_HEADER_DIR})
endif()