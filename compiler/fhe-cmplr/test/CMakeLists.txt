#=============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
#=============================================================================

set (FHE_TESTAPP_INCLUDE_DIRS "")
list (APPEND FHE_TESTAPP_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
include_directories (${FHE_TESTAPP_INCLUDE_DIRS})
set (FHE_TESTAPP_INCLUDE_DIRS "${FHE_TESTAPP_INCLUDE_DIRS}" CACHE INTERNAL "")

build_app_test (FHE_INSTALL_APP "${FHE_LIBS}")

add_test( NAME test_fhe_cmplr COMMAND ${PROJECT_SOURCE_DIR}/test/test_fhe_cmplr.sh "${fhe-cmplr_BINARY_DIR}")
add_test( NAME test_bts_rescale COMMAND ${PROJECT_SOURCE_DIR}/test/test_bts_rescale.sh "${fhe-cmplr_BINARY_DIR}")

# --- rtlib-py / onnx e2e gate (the main test path, under test/rtlib_py/).
# The onnx-models corpus sweep is the main test path; the bootstrap smoke +
# the allops C++ emit/exec test are registered here; py_verify lives under
# test/rtlib_py/.
if(DEFINED PYTHON_EXECUTABLE AND PYTHON_EXECUTABLE)
  set(_FHE_PY ${PYTHON_EXECUTABLE})
else()
  set(_FHE_PY python3)
endif()

# onnx-models/ corpus workloads (rtlib_py/py_verify/workloads.py) driven by
# run_py_verify.py over rt_ref then rt_openfhe. Skips (77) when onnx/pytest
# (rt_ref) or openfhe-python (rt_openfhe) is absent.
add_test( NAME test_fhe_cmplr_py_verify_onnx
  COMMAND ${PROJECT_SOURCE_DIR}/test/rtlib_py/test_fhe_cmplr_py_verify_onnx.sh
          "${fhe-cmplr_BINARY_DIR}" "${_FHE_PY}")
add_test( NAME test_fhe_cmplr_py_verify_onnx_openfhe
  COMMAND ${PROJECT_SOURCE_DIR}/test/rtlib_py/test_fhe_cmplr_py_verify_onnx_openfhe.sh
          "${fhe-cmplr_BINARY_DIR}" "${_FHE_PY}")
set_tests_properties(test_fhe_cmplr_py_verify_onnx
  PROPERTIES SKIP_RETURN_CODE 77 LABELS phase1_onnx_models TIMEOUT 1800)
set_tests_properties(test_fhe_cmplr_py_verify_onnx_openfhe
  PROPERTIES SKIP_RETURN_CODE 77 LABELS phase3_onnx_models TIMEOUT 1800)

# rt_openfhe bootstrap smoke (rtpy-bts/rtpy-lvl lazy-bridge gate): real
# EvalBootstrap under a toy Set-A context (~6s; wide timeout for slow CI).
add_test( NAME test_openfhe_bootstrap
  COMMAND ${PROJECT_SOURCE_DIR}/test/rtlib_py/test_openfhe_bootstrap.sh
          "${fhe-cmplr_BINARY_DIR}" "${_FHE_PY}")
set_tests_properties(test_openfhe_bootstrap
  PROPERTIES SKIP_RETURN_CODE 77 LABELS phase3_openfhe TIMEOUT 600)

# rt_fhert host bootstrap smoke (F4.9 host gate): real EvalBootstrap under a
# toy ring-2^14 context (FIXEDMANUAL, SPARSE_TERNARY + correction factor,
# level_budget [4,4]); prints the measured max_abs against FHE_BTS_TOL
# (default 1e-1 lossy refresh). Skips (77) when the fhert binding/pytest
# absent. Slow under the host binding -- wide timeout.
add_test( NAME test_fhert_bootstrap_host
  COMMAND ${PROJECT_SOURCE_DIR}/test/rtlib_py/test_fhert_bootstrap_host.sh
          "${fhe-cmplr_BINARY_DIR}" "${_FHE_PY}")
set_tests_properties(test_fhert_bootstrap_host
  PROPERTIES SKIP_RETURN_CODE 77 LABELS phase3_fhert TIMEOUT 1800)

# rt_fhert resnet20 device e2e (F4.9 resnet20-device SKIP-77): default OFF --
# the test script self-SKIPs (exit 77) unless FHE_RUN_RESNET20_DEVICE=1 is
# set. resnet20 device is a known-limited feature (per-cycle HBM
# accumulation; fix B1 future); this refactor records the device suite as
# NOT-RUN-in-this-env (F4.9/F4.12, no cmodel symlink present).
add_test( NAME test_fhert_device_resnet20
  COMMAND ${PROJECT_SOURCE_DIR}/test/rtlib_py/test_fhert_device_resnet20.sh
          "${fhe-cmplr_BINARY_DIR}" "${_FHE_PY}")
set_tests_properties(test_fhert_device_resnet20
  PROPERTIES SKIP_RETURN_CODE 77 LABELS phase3_fhert TIMEOUT 7200)

# The kept C++ poly2py emit+exec test (allops) execs the emitted .py via
# run_py_exec.py; bake the harness path + interpreter at compile time so the
# C++ std::system invocations agree with the .sh-driven tests.
if(TARGET test_ckks2py_allops)
  target_compile_definitions(test_ckks2py_allops PRIVATE
    "FHE_PY_VERIFY_DIR=\"${PROJECT_SOURCE_DIR}/test/rtlib_py/py_verify\""
    "FHE_PYTHON=\"${_FHE_PY}\"")
endif()

# Phase 1 verify label: the kept poly2py C++ emit+exec test only.
# F5.3: SKIP_RETURN_CODE 77 + TIMEOUT so a binding-absent / harness-absent env
# SKIPs (not FAILs) -- the C++ harness returns 0 on install-only (harness file
# absent) and run_py_exec.py reserves 77 for a missing provider binding (rt_ref
# default needs no binding; an install-only container skips cleanly). Mirrors
# the .sh-driven py_verify tests' SKIP gate.
foreach(_t test_ckks2py_allops)
  if(TEST ${_t})
    set_tests_properties(${_t} PROPERTIES SKIP_RETURN_CODE 77
      LABELS phase1_verify TIMEOUT 300)
  endif()
endforeach()