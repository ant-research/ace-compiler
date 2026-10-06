#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

set (TEST_IRGEN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/od/test")
# add_test(NAME test_irgen
#         COMMAND ${PYTHON_EXECUTABLE} -m unittest discover -s ${TEST_IRGEN_DIR}
#         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" DESTINATION share/${PACKAGE_NAME})