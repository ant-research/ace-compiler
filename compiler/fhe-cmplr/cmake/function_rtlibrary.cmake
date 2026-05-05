#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

##############################################################################
# Runtime libraries FHErt_ant
##############################################################################

# Found libraries to import component rtlib_ant
function(find_fhert_ant_lib)
  find_library(FHERT_ANT NAMES FHErt_ant)
  find_library(FHERT_COMMON NAMES FHErt_common)
  # find_extralib_math(MATH_LIBS)

  if(FHERT_ANT AND FHERT_COMMON)
    set(FHERT_ANT_LIBS ${FHERT_ANT_LIBS} ${FHERT_ANT} ${FHERT_COMMON})
    set(FHERT_ANT_LIBS ${FHERT_ANT_LIBS} CACHE STRING "Global libs of rtlib-ant")
  else()
    message(FATAL_ERROR "** ERROR ** fhert_ant libs is not installed")
  endif()
endfunction()

##############################################################################
# Runtime libraries FHErt_seal
##############################################################################

# Set the list of libraries to build component rtlib_seal
function(find_fhert_seal_lib)

  # Put it here for now
  set (FHERT_SEAL_LIBS "")
  assert("${FHERT_SEAL_LIBS}" "FHERT_SEAL_LIBS can't be empty")
endfunction()

##############################################################################
# Runtime libraries FHErt_openfhe
##############################################################################

# Set the list of libraries to build component rtlib_openfhe
function(find_fhert_openfhe_lib)

  # Put it here for now
  set (FHERT_OPENFHE_LIBS "")
  assert("${FHERT_OPENFHE_LIBS}" "FHERT_OPENFHE_LIBS can't be empty")
endfunction()

