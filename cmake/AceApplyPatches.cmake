#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

# AceApplyPatches.cmake — Apply *.patch files to a source tree.
#
# ace_apply_patches(<patch_dir> <source_dir>)
#   Applies every *.patch file found in <patch_dir> to <source_dir> via
#   `git apply --ignore-whitespace`.  Patches that are already applied or
#   not applicable are silently skipped (dry-run check fails gracefully).
#
function(ace_apply_patches PATCH_DIR SOURCE_DIR)
  if(NOT EXISTS "${PATCH_DIR}")
    return()
  endif()

  file(GLOB _PATCH_FILES "${PATCH_DIR}/*.patch")
  foreach(_PF ${_PATCH_FILES})
    # Dry-run: check if the patch can be applied
    execute_process(
      COMMAND git apply --ignore-whitespace --check "${_PF}"
      WORKING_DIRECTORY "${SOURCE_DIR}"
      RESULT_VARIABLE _RC
      OUTPUT_QUIET ERROR_QUIET
    )
    if(_RC EQUAL 0)
      message(STATUS "Applying patch: ${_PF}")
      execute_process(
        COMMAND git apply --ignore-whitespace "${_PF}"
        WORKING_DIRECTORY "${SOURCE_DIR}"
        RESULT_VARIABLE _RC
      )
      if(NOT _RC EQUAL 0)
        message(WARNING "Failed to apply patch: ${_PF}")
      endif()
    else()
      message(STATUS "Patch already applied or not applicable, skipping: ${_PF}")
    endif()
  endforeach()
endfunction()
