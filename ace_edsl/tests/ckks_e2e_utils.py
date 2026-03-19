#!/usr/bin/env python3
"""Helpers to compile and run generated CKKS C kernels end-to-end via rtlib."""

import json
import os
import subprocess
import sys
from typing import List, Sequence, Tuple


TESTS_DIR = os.path.dirname(os.path.abspath(__file__))
ACE_EDSL_DIR = os.path.abspath(os.path.join(TESTS_DIR, ".."))
REPO_ROOT = os.path.abspath(os.path.join(ACE_EDSL_DIR, ".."))
ACE_CMPLR_DIR = os.path.join(REPO_ROOT, "ace_cmplr")


def find_rtlib_build():
    """Return include/lib paths for ANT rtlib, or None when unavailable."""
    include_dir = os.path.join(ACE_CMPLR_DIR, "rtlib", "include")
    if not os.path.isdir(include_dir):
        return None

    lib_ant = None
    lib_common = None
    rtlib_lib = os.path.join(ACE_CMPLR_DIR, "rtlib", "lib")
    if os.path.isdir(rtlib_lib):
        ant = os.path.join(rtlib_lib, "libFHErt_ant.a")
        if not os.path.isfile(ant):
            ant = os.path.join(rtlib_lib, "libFHErt_ant_encode.a")
        common = os.path.join(rtlib_lib, "libFHErt_common.a")
        if os.path.isfile(ant) and os.path.isfile(common):
            lib_ant, lib_common = ant, common

    if not lib_ant or not lib_common:
        for build_name in ("debug", "release", "debug_openmp", "release_openmp"):
            build_rtlib = os.path.join(REPO_ROOT, build_name, "rtlib", "build")
            ant_dir = os.path.join(build_rtlib, "ant")
            common_dir = os.path.join(build_rtlib, "common")
            ant = os.path.join(ant_dir, "libFHErt_ant.a")
            if not os.path.isfile(ant):
                ant = os.path.join(ant_dir, "libFHErt_ant_encode.a")
            common = os.path.join(common_dir, "libFHErt_common.a")
            if os.path.isfile(ant) and os.path.isfile(common):
                lib_ant, lib_common = ant, common
                break

    if not lib_ant or not lib_common:
        return None

    lib_airutil = os.path.join(ACE_CMPLR_DIR, "lib", "libAIRutil.a")
    if not os.path.isfile(lib_airutil):
        lib_airutil = None
        for build_name in ("debug", "release", "debug_openmp", "release_openmp"):
            cand = os.path.join(
                REPO_ROOT,
                build_name,
                "nn-addon",
                "air-infra",
                "util",
                "libAIRutil.a",
            )
            if os.path.isfile(cand):
                lib_airutil = cand
                break
    if not lib_airutil:
        return None

    return {
        "include_dir": include_dir,
        "ant_include_dir": os.path.join(ACE_CMPLR_DIR, "rtlib", "include", "ant"),
        "ace_cmplr_include": os.path.join(ACE_CMPLR_DIR, "include"),
        "lib_ant": lib_ant,
        "lib_common": lib_common,
        "lib_airutil": lib_airutil,
    }


def _gen_wrapper_code(kernel_name: str, input_names: Sequence[str]) -> str:
    arg_decl = ", ".join(f"CIPHERTEXT {name}" for name in input_names)
    call_args = ", ".join(input_names)

    desc_defs = []
    scheme_defs = []
    scheme_refs = []
    load_lines = []
    for idx, name in enumerate(input_names):
        desc_defs.append(f"  static MAP_DESC desc_{idx}[] = {{ {{NORMAL, 0, 0, 0, 0}} }};")
        scheme_defs.append(
            f'  static DATA_SCHEME scheme_{idx} = {{ "{name}", {{0, 0, 0, 0}}, 1, desc_{idx} }};'
        )
        scheme_refs.append(f"&scheme_{idx}")
        load_lines.append(f'  CIPHERTEXT {name} = Get_input_data("{name}", 0);')

    return f"""//-*-c-*-
#include <stddef.h>
#include <string.h>

#include "rt_ant/rt_ant.h"

extern CIPHERTEXT {kernel_name}({arg_decl});

DATA_SCHEME* Get_encode_scheme(int idx) {{
{os.linesep.join(desc_defs)}
{os.linesep.join(scheme_defs)}
  static DATA_SCHEME* scheme[] = {{ {", ".join(scheme_refs)} }};
  if (idx < 0 || idx >= {len(input_names)}) {{
    return NULL;
  }}
  return scheme[idx];
}}

DATA_SCHEME* Get_decode_scheme(int idx) {{
  static MAP_DESC desc[] = {{ {{NORMAL, 0, 0, 0, 0}} }};
  static DATA_SCHEME scheme = {{ "output", {{0, 0, 0, 0}}, 1, desc }};
  (void)idx;
  return &scheme;
}}

int Get_input_count() {{
  return {len(input_names)};
}}

int Get_output_count() {{
  return 1;
}}

bool Main_graph() {{
{os.linesep.join(load_lines)}
  CIPHERTEXT result;
  memset(&result, 0, sizeof(result));
  result = {kernel_name}({call_args});
  Set_output_data("output", 0, &result);
  return true;
}}
"""


def _build_shared_lib(
    output_dir: str,
    generated_c_path: str,
    kernel_name: str,
    input_names: Sequence[str],
) -> str:
    paths = find_rtlib_build()
    if not paths:
        raise RuntimeError(
            "ANT rtlib not found. Build compiler runtime first "
            "(e.g. scripts/build_cmplr_omp.sh Debug; cmake --install debug_openmp/ --prefix ace_cmplr)."
        )

    wrapper_c = os.path.join(output_dir, f"{kernel_name}_wrapper.c")
    with open(wrapper_c, "w") as f:
        f.write(_gen_wrapper_code(kernel_name, input_names))

    obj_gen = os.path.join(output_dir, f"{kernel_name}_gen.o")
    obj_wrap = os.path.join(output_dir, f"{kernel_name}_wrap.o")
    so_path = os.path.join(output_dir, f"lib{kernel_name}.so")

    inc = [
        "-I",
        paths["ace_cmplr_include"],
        "-I",
        paths["include_dir"],
        "-I",
        paths["ant_include_dir"],
    ]
    cflags = ["-std=gnu11", "-O0", "-g", "-DNDEBUG", "-DRTLIB_SUPPORT_LINUX", "-fopenmp", "-fPIC"]

    compile_gen = ["gcc", "-c"] + cflags + inc + [generated_c_path, "-o", obj_gen]
    compile_wrap = ["gcc", "-c"] + cflags + inc + [wrapper_c, "-o", obj_wrap]
    link_so = [
        "g++",
        "-shared",
        "-fPIC",
        obj_gen,
        obj_wrap,
        paths["lib_ant"],
        paths["lib_common"],
        paths["lib_airutil"],
        "-lgmp",
        "-lm",
        "-lgomp",
        "-o",
        so_path,
    ]

    try:
        subprocess.run(compile_gen, check=True, capture_output=True, cwd=REPO_ROOT)
    except subprocess.CalledProcessError as e:
        stderr = e.stderr.decode(errors="ignore") if e.stderr else str(e)
        raise RuntimeError(f"Compile generated C failed:\n{stderr}") from e

    try:
        subprocess.run(compile_wrap, check=True, capture_output=True, cwd=REPO_ROOT)
    except subprocess.CalledProcessError as e:
        stderr = e.stderr.decode(errors="ignore") if e.stderr else str(e)
        raise RuntimeError(f"Compile wrapper C failed:\n{stderr}") from e

    try:
        subprocess.run(link_so, check=True, capture_output=True, cwd=REPO_ROOT)
    except subprocess.CalledProcessError as e:
        stderr = e.stderr.decode(errors="ignore") if e.stderr else str(e)
        raise RuntimeError(f"Link shared library failed:\n{stderr}") from e

    return so_path


def run_kernel_e2e(
    output_dir: str,
    generated_c_path: str,
    kernel_name: str,
    input_tensors: Sequence[Tuple[str, Sequence[float]]],
    output_len: int,
) -> List[float]:
    """Compile generated kernel + wrapper and return decoded output values."""
    so_path = _build_shared_lib(output_dir, generated_c_path, kernel_name, [n for n, _ in input_tensors])

    tensor_data = [[name, [float(x) for x in values]] for name, values in input_tensors]
    child_code = f"""
import ctypes
import json

so_path = {so_path!r}
input_tensors = {tensor_data!r}
output_len = int({int(output_len)})

class Shape(ctypes.Structure):
    _fields_ = [("_n", ctypes.c_size_t), ("_c", ctypes.c_size_t), ("_h", ctypes.c_size_t), ("_w", ctypes.c_size_t)]

class TensorHeader(ctypes.Structure):
    _fields_ = [("_shape", Shape)]

def _make_tensor(vals):
    n, c, h, w = 1, 1, 1, len(vals)
    count = n * c * h * w
    sz = ctypes.sizeof(TensorHeader) + ctypes.sizeof(ctypes.c_double) * count
    buf = ctypes.create_string_buffer(sz)
    header = ctypes.cast(buf, ctypes.POINTER(TensorHeader))
    header.contents._shape = Shape(n, c, h, w)
    arr_t = ctypes.c_double * count
    arr = arr_t.from_buffer(buf, ctypes.sizeof(TensorHeader))
    for i, v in enumerate(vals):
        arr[i] = float(v)
    return buf, header

lib = ctypes.CDLL(so_path)
lib.Prepare_context.argtypes = []
lib.Prepare_context.restype = None
lib.Finalize_context.argtypes = []
lib.Finalize_context.restype = None
lib.Prepare_input.argtypes = [ctypes.POINTER(TensorHeader), ctypes.c_char_p]
lib.Prepare_input.restype = None
lib.Main_graph.argtypes = []
lib.Main_graph.restype = ctypes.c_bool
lib.Handle_output.argtypes = [ctypes.c_char_p]
lib.Handle_output.restype = ctypes.POINTER(ctypes.c_double)

buffers = []
out_ptr = None
try:
    lib.Prepare_context()
    for name, vals in input_tensors:
        buf, header = _make_tensor(vals)
        buffers.append(buf)
        lib.Prepare_input(header, name.encode("utf-8"))
    ok = lib.Main_graph()
    if not ok:
        raise RuntimeError("Main_graph() returned false")
    out_ptr = lib.Handle_output(b"output")
    if not bool(out_ptr):
        raise RuntimeError("Handle_output returned null")
    values = [float(out_ptr[i]) for i in range(output_len)]
    print("C_API_VALUES_JSON=" + json.dumps(values))
finally:
    if out_ptr:
        ctypes.CDLL("libc.so.6").free(ctypes.cast(out_ptr, ctypes.c_void_p))
    lib.Finalize_context()
"""

    result = subprocess.run(
        [sys.executable, "-c", child_code],
        cwd=REPO_ROOT,
        capture_output=True,
        text=True,
        timeout=240,
    )
    if result.returncode != 0:
        raise RuntimeError(
            "End-to-end run failed.\n"
            f"stdout:\n{result.stdout}\n"
            f"stderr:\n{result.stderr}"
        )

    values_line = None
    for line in reversed((result.stdout or "").splitlines()):
        if line.startswith("C_API_VALUES_JSON="):
            values_line = line
            break
    if values_line is None:
        raise RuntimeError(
            "End-to-end run did not emit output values.\n"
            f"stdout:\n{result.stdout}\n"
            f"stderr:\n{result.stderr}"
        )
    return json.loads(values_line.split("=", 1)[1])


def compare_with_tolerance(actual: Sequence[float], expected: Sequence[float], tol: float = 1e-2):
    if len(actual) != len(expected):
        return False, f"length mismatch: actual={len(actual)} expected={len(expected)}"
    if not actual:
        return True, "empty output"

    max_err = -1.0
    max_idx = -1
    for i, (a, e) in enumerate(zip(actual, expected)):
        err = abs(float(a) - float(e))
        if err > max_err:
            max_err = err
            max_idx = i
    if max_err > tol:
        return (
            False,
            f"max_abs_err={max_err:.6g} at idx={max_idx}, actual={actual[max_idx]:.6g}, expected={expected[max_idx]:.6g}, tol={tol}",
        )
    return True, f"max_abs_err={max_err:.6g} (tol={tol})"
