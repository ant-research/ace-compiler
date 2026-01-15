#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

# Set the working environment of the package
function(set_package_env base conf devtools)
  assert("${base}" "project base path can't be empty")
  assert("${conf}" "configure path can't be empty")
  assert("${devtools}" "devtools path can't be empty")

  set(ENV{PACKAGE_BASE_DIR} ${base})
  set(ENV{PACKAGE_CONF_DIR} ${conf})
  set(ENV{PACKAGE_DEVTOOLS_DIR} ${devtools})

  # Check common program & libraries
  if("${PYTHON_EXECUTABLE}" STREQUAL "")
    check_program("python3" PYTHON_EXECUTABLE)
  endif()
endfunction()

# Check Configure dependent function
function(read_package_config path)
  assert("${path}" "path can't be empty")

  file(READ "${path}/config.txt" CONFIG_CONTENT)

  string(REGEX MATCHALL "([A-Za-z0-9_]+)=[^ \n]+" CONFIG_PAIRS "${CONFIG_CONTENT}")
  foreach(PAIR ${CONFIG_PAIRS})
    string(REGEX REPLACE "([^=]+)=([^ \n]+)" "\\1" VAR_NAME ${PAIR})
    string(REGEX REPLACE "([^=]+)=([^ \n]+)" "\\2" VAR_VALUE ${PAIR})

    set(${VAR_NAME} ${VAR_VALUE} PARENT_SCOPE)
  endforeach()
endfunction()

# Check Package information dependent function
function(check_package_info name)
  assert("${name}" "package name can't found")
  message(STATUS "Found Package name            : ${name}")
endfunction()

# Check that the program is available
function(check_program PROGRAM_NAME VARIABLE_NAME)
  assert("${PROGRAM_NAME}" "PROGRAM_NAME can't be empty")

  find_program(${VARIABLE_NAME} ${PROGRAM_NAME})
  if(NOT ${VARIABLE_NAME})
    message(FATAL_ERROR "${PROGRAM_NAME} not found!")
  else()
    message(STATUS "Found ${PROGRAM_NAME}                  : ${${VARIABLE_NAME}}")
    set(PYTHON_EXECUTABLE ${PYTHON_EXECUTABLE} CACHE STRING "Global common execute of python")
  endif()
endfunction()

# Setup git hooks function
function(install_hooks path)
  assert("${path}" "path can't be empty")
  set(INSTALL_HOOK "${path}/install-hooks.sh")

  execute_process(
    COMMAND ${INSTALL_HOOK}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  )
endfunction()

# Check Build Version function
function(check_code_revision project commit)
  assert("${project}" "project can't be empty")

  return()
  execute_process(
    COMMAND git rev-parse HEAD
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  assert("${GIT_COMMIT}" "can't get commit id")

  set(${commit} ${GIT_COMMIT} PARENT_SCOPE)
  file(APPEND "${CMAKE_BINARY_DIR}/git_commit.txt" "${project} Commit: ${GIT_COMMIT}\n")
endfunction()

# Add glob include list dependent function
function(add_inc_list path)
  set(current_list ${PACKAGE_INC_DIR})
  list(FIND current_list "${path}" index)
  if(index EQUAL -1)
    list(APPEND current_list "${path}")
    set(PACKAGE_INC_DIR "${current_list}" CACHE STRING "A list of glob include" FORCE)
  endif()
endfunction()

# Check Build Type dependent function
function(check_build_type type)
  set(TYPES Debug Release RelWithDebInfo MinSizeRel)
  list(FIND TYPES ${type} INDEX_FOUND)

  if(${INDEX_FOUND} EQUAL -1)
    message(FATAL_ERROR "** ERROR ** CMAKE_BUILD_TYPE must be : Debug|Release|RelWithDebInfo|MinSizeRel")
  endif()
endfunction()

# Check Coding Style dependent function
function(check_coding_style path regex)
  assert("${path}" "path can't found")

  get_filename_component(last_dir ${CMAKE_CURRENT_SOURCE_DIR} NAME)
  set(target_name "check_coding_style_${last_dir}")

  add_custom_target(${target_name} ALL
    COMMAND python3 ${path}/check_coding_style.py -b ${CMAKE_BINARY_DIR} -x ${regex} -c ${CMAKE_CURRENT_SOURCE_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )

  if(BUILD_UNITTEST AND NOT DEFINED LIBRARY_GTEST)
    add_dependencies(${target_name} gtest)
  endif()
  if(BUILD_BENCH AND NOT DEFINED LIBRARY_BENCHMARK)
    add_dependencies(${target_name} benchmark)
  endif()
  # if(BUILD_TCM AND NOT DEFINED LIBRARY_TCMALLOC)
  #   add_dependencies(${target_name} gperftools)
  # endif()
endfunction()

# Check String not empty dependent function
function(assert string message)
  if("${string}" STREQUAL "")
    message(FATAL_ERROR "ASSERTION FAILED: ${message}")
  endif()
endfunction()

# Pregen errmsg dependent function
function(gen_errmsg program)
  set(USER_MSG_PATH ${CMAKE_CURRENT_SOURCE_DIR}/devtools/err_msg)
  set(INC_H_DIR ${CMAKE_BINARY_DIR}/include/air/util)
  set(INC_C_DIR ${CMAKE_BINARY_DIR}/include/air/util)

  add_custom_command(
    OUTPUT ${INC_H_DIR}/err_msg.h ${INC_C_DIR}/err_msg.inc
    PRE_BUILD
    WORKING_DIRECTORY ${USER_MSG_PATH}
    COMMAND ${program} err_msg.py -i ${INC_H_DIR}  -s ${INC_C_DIR}
    DEPENDS ${USER_MSG_PATH}/err_msg.yml ${USER_MSG_PATH}/user_define.yml
    COMMENT "Generating files for 'customized error message'"
  )
  include_directories(${CMAKE_BINARY_DIR}/include)

  add_custom_target(errmsg DEPENDS ${INC_H_DIR}/err_msg.h ${INC_C_DIR}/err_msg.inc)
  add_dependencies(air_depend errmsg)

  install(DIRECTORY ${CMAKE_BINARY_DIR}/include/ DESTINATION include)
endfunction()

# Pregen option dependent function
function(gen_option program target OUTPUT_DIR INPUT_DIR)
  assert("${INPUT_DIR}" "INPUT_DIR can't be empty")
  assert("${OUTPUT_DIR}" "OUTPUT_DIR can't be empty")

  if(NOT EXISTS "${OUTPUT_DIR}")
    file(MAKE_DIRECTORY "${OUTPUT_DIR}")
  endif()

  set(OPTION_PATH $ENV{PACKAGE_DEVTOOLS_DIR}/option)
  set(INC_H_DIR ${OUTPUT_DIR})
  set(INC_CXX_DIR ${OUTPUT_DIR})
  set(OPTION_CONFIG_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${INPUT_DIR})

  add_custom_command(
    OUTPUT ${INC_H_DIR}/option_config.h ${INC_CXX_DIR}/option_config.inc
    WORKING_DIRECTORY ${INC_CXX_DIR}
    COMMAND ${program} ${OPTION_PATH}/option_generator.py -ip ${OPTION_CONFIG_PATH}/option.yml
    DEPENDS ${OPTION_CONFIG_PATH}/option.yml
    COMMENT "Generating files for 'customized option'"
  )

  add_custom_target(${target} ALL DEPENDS ${INC_H_DIR}/option_config.h ${INC_CXX_DIR}/option_config.inc)
  set(${target} ${target} PARENT_SCOPE)

  install(DIRECTORY ${CMAKE_BINARY_DIR}/include/ DESTINATION include)
endfunction()

# Pregen odgen dependent function
function(gen_domain_od GENERATE_SCRIPT INPUT_FILE OUTPUT_DIR DOMAIN)
  assert("${GENERATE_SCRIPT}" "GENERATE_SCRIPT can't be empty")
  assert("${INPUT_FILE}" "INPUT_FILE can't be empty")
  assert("${OUTPUT_DIR}" "OUTPUT_DIR can't be empty")

  set(OUTPUT_FILE "${OUTPUT_DIR}/opcode.cxx;${OUTPUT_DIR}/opcode.h;${OUTPUT_DIR}/${DOMAIN}_binding.h")
  string(REPLACE ";" " " VAR_OUTPUT "${OUTPUT_FILE}")

  add_custom_command(
      OUTPUT ${OUTPUT_FILE}
      COMMAND ${CMAKE_COMMAND} -E echo "Generating files : ${VAR_OUTPUT}"
      COMMAND ${PYTHON_EXECUTABLE} ${GENERATE_SCRIPT} -i ${INPUT_FILE} -o ${OUTPUT_DIR}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      DEPENDS ${INPUT_FILE} ${GENERATE_SCRIPT}
      COMMENT "Running customized domain opcode generation script for ${DOMAIN}"
  )
  add_custom_target(${DOMAIN}_od ALL DEPENDS "${OUTPUT_FILE}")

  install(DIRECTORY ${CMAKE_BINARY_DIR}/include/ DESTINATION include)
endfunction()

# Pregen type checkers dependent function
function(gen_type_checkers program INPUT_FILE OUTPUT_DIR)
  assert("${program}" "program can't be empty")
  assert("${INPUT_FILE}" "INPUT_FILE can't be empty")
  assert("${OUTPUT_DIR}" "OUTPUT_DIR can't be empty")

  set(IRGEN_OD_PATH "$ENV{PACKAGE_BASE_DIR}/irgen")
  set(CMD ${program} ${INPUT_FILE} -o ${OUTPUT_DIR})
  message(STATUS "Generating type checkers ...")
  execute_process(
    COMMAND ${CMAKE_COMMAND} -E env IRGEN_OD_PATH=${IRGEN_OD_PATH} ${CMD}
    OUTPUT_VARIABLE output_var
    RESULT_VARIABLE result_var
    ERROR_VARIABLE error_var
  )
endfunction()

# Pregen irgen dependent function
function(tabgen program INPUT_DIR OUTPUT_DIR STAMP)
  assert("${program}" "program can't be empty")
  assert("${INPUT_DIR}" "INPUT_DIR can't be empty")
  assert("${OUTPUT_DIR}" "OUTPUT_DIR can't be empty")

  set(COMMANDS_LIST "")
  file(GLOB_RECURSE TD_FILES "${INPUT_DIR}/*.yml")
  foreach(TD_FILE IN LISTS TD_FILES)
    list(APPEND COMMANDS_LIST "${PYTHON_EXECUTABLE} ${program} -t -i ${TD_FILE} -o ${OUTPUT_DIR}")
  endforeach()

  set(IRGEN_OD_PATH "$ENV{PACKAGE_BASE_DIR}/irgen")
  add_custom_target(${STAMP} ALL
    COMMAND /bin/bash -c ${COMMANDS_LIST}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generate customized tabgen files ..."
    VERBATIM
  )

  install(DIRECTORY ${CMAKE_BINARY_DIR}/include/ DESTINATION include)
endfunction()
