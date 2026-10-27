# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
ctypes bindings for cuFile (GPUDirect Storage) and minimal CUDA helpers.

Provides low-level FFI wrappers used by check_io and check_env.
No pycuda or cupy dependency — only ctypes + libcudart.so + libcufile.so.

References:
    - cufile.h (CUDA 12.x GDS SDK)
    - https://docs.nvidia.com/gpudirect-storage/
"""

import ctypes
import os


# ── cuFile constants ──────────────────────────────────────────────
CU_FILE_HANDLE_TYPE_OPAQUE_FD = 1  # matches cufile.h CUfileFileHandleType

# O_DIRECT: required by cuFile for GDS path
try:
    O_DIRECT = os.O_DIRECT
except AttributeError:
    O_DIRECT = 0o40000  # x86_64 / aarch64 fallback

# cuFileOpError codes — from cufile.h CUFILEOP_STATUS_ENTRIES
CUFILE_ERRORS = {
    0: "CU_FILE_SUCCESS",
    5001: "CU_FILE_DRIVER_NOT_INITIALIZED",
    5002: "CU_FILE_DRIVER_INVALID_PROPS",
    5003: "CU_FILE_DRIVER_UNSUPPORTED_LIMIT",
    5004: "CU_FILE_DRIVER_VERSION_MISMATCH",
    5005: "CU_FILE_DRIVER_VERSION_READ_ERROR",
    5006: "CU_FILE_DRIVER_CLOSING",
    5007: "CU_FILE_PLATFORM_NOT_SUPPORTED",
    5008: "CU_FILE_IO_NOT_SUPPORTED",
    5009: "CU_FILE_DEVICE_NOT_SUPPORTED",
    5010: "CU_FILE_NVFS_DRIVER_ERROR",
    5011: "CU_FILE_CUDA_DRIVER_ERROR",
    5012: "CU_FILE_CUDA_POINTER_INVALID",
    5013: "CU_FILE_CUDA_MEMORY_TYPE_INVALID",
    5014: "CU_FILE_CUDA_POINTER_RANGE_ERROR",
    5015: "CU_FILE_CUDA_CONTEXT_MISMATCH",
    5016: "CU_FILE_INVALID_MAPPING_SIZE",
    5017: "CU_FILE_INVALID_MAPPING_RANGE",
    5018: "CU_FILE_INVALID_FILE_TYPE",
    5019: "CU_FILE_INVALID_FILE_OPEN_FLAG",
    5020: "CU_FILE_DIO_NOT_SET",
    5022: "CU_FILE_INVALID_VALUE",
    5023: "CU_FILE_MEMORY_ALREADY_REGISTERED",
    5024: "CU_FILE_MEMORY_NOT_REGISTERED",
    5025: "CU_FILE_PERMISSION_DENIED",
    5026: "CU_FILE_DRIVER_ALREADY_OPEN",
    5027: "CU_FILE_HANDLE_NOT_REGISTERED",
    5028: "CU_FILE_HANDLE_ALREADY_REGISTERED",
    5029: "CU_FILE_DEVICE_NOT_FOUND",
    5030: "CU_FILE_INTERNAL_ERROR",
    5031: "CU_FILE_GETNEWFD_FAILED",
    5033: "CU_FILE_NVFS_SETUP_ERROR",
    5034: "CU_FILE_IO_DISABLED",
    5035: "CU_FILE_BATCH_SUBMIT_FAILED",
    5036: "CU_FILE_GPU_MEMORY_PINNING_FAILED",
    5037: "CU_FILE_BATCH_FULL",
    5038: "CU_FILE_ASYNC_NOT_SUPPORTED",
    5039: "CU_FILE_IO_MAX_ERROR",
}


def cufile_strerror(code: int) -> str:
    """Return human-readable string for a cuFileOpError code."""
    return CUFILE_ERRORS.get(code, f"CU_FILE_ERROR_UNKNOWN({code})")


# ── ctypes structs matching cufile.h ──────────────────────────────
class CUfileError(ctypes.Structure):
    """Matches: typedef struct CUfileError { CUfileOpError err; CUresult cu_err; }"""

    _fields_ = [("err", ctypes.c_int), ("cu_err", ctypes.c_int)]

    @property
    def ok(self):
        return self.err == 0

    def __str__(self):
        if self.ok:
            return "CU_FILE_SUCCESS"
        s = cufile_strerror(self.err)
        if self.err == 5011 and self.cu_err != 0:  # CU_FILE_CUDA_DRIVER_ERROR
            s += f" (cuda_err={self.cu_err})"
        return s


class CUfileDescrHandle(ctypes.Union):
    """Matches: union { int fd; void *handle; }"""

    _fields_ = [("fd", ctypes.c_int), ("handle", ctypes.c_void_p)]


class CUfileDescr(ctypes.Structure):
    """Matches: typedef struct CUfileDescr_t { type; handle; fs_ops; }"""

    _fields_ = [
        ("type", ctypes.c_int),
        ("handle", CUfileDescrHandle),
        ("fs_ops", ctypes.c_void_p),
    ]


class CUfileDrvProps_nvfs(ctypes.Structure):
    """Matches: struct CUfileDrvProps.nvfs sub-struct."""

    _fields_ = [
        ("major_version", ctypes.c_uint),
        ("minor_version", ctypes.c_uint),
        ("poll_thresh_size", ctypes.c_size_t),
        ("max_direct_io_size", ctypes.c_size_t),
        ("dstatusflags", ctypes.c_uint),
        ("dcontrolflags", ctypes.c_uint),
    ]


class CUfileDrvProps(ctypes.Structure):
    """Matches: typedef struct CUfileDrvProps."""

    _fields_ = [
        ("nvfs", CUfileDrvProps_nvfs),
        ("fflags", ctypes.c_uint),
        ("max_device_cache_size", ctypes.c_uint),
        ("per_buffer_cache_size", ctypes.c_uint),
        ("max_device_pinned_mem_size", ctypes.c_uint),
        ("max_batch_io_size", ctypes.c_uint),
        ("max_batch_io_timeout_msecs", ctypes.c_uint),
    ]


# ── Library loaders ───────────────────────────────────────────────
def load_cufile():
    """Load libcufile.so with correct ctypes signatures.

    Returns the loaded library handle.

    Raises OSError if libcufile.so cannot be found.
    """
    cufile = ctypes.CDLL("libcufile.so")

    cufile.cuFileDriverOpen.restype = CUfileError
    cufile.cuFileDriverOpen.argtypes = []

    cufile.cuFileDriverClose.restype = CUfileError
    cufile.cuFileDriverClose.argtypes = []

    cufile.cuFileHandleRegister.restype = CUfileError
    cufile.cuFileHandleRegister.argtypes = [
        ctypes.POINTER(ctypes.c_void_p),
        ctypes.POINTER(CUfileDescr),
    ]

    cufile.cuFileHandleDeregister.restype = None
    cufile.cuFileHandleDeregister.argtypes = [ctypes.c_void_p]

    cufile.cuFileBufRegister.restype = CUfileError
    cufile.cuFileBufRegister.argtypes = [ctypes.c_void_p, ctypes.c_size_t, ctypes.c_int]

    cufile.cuFileBufDeregister.restype = CUfileError
    cufile.cuFileBufDeregister.argtypes = [ctypes.c_void_p]

    cufile.cuFileRead.restype = ctypes.c_ssize_t
    cufile.cuFileRead.argtypes = [
        ctypes.c_void_p,
        ctypes.c_void_p,
        ctypes.c_size_t,
        ctypes.c_longlong,
        ctypes.c_longlong,
    ]

    cufile.cuFileWrite.restype = ctypes.c_ssize_t
    cufile.cuFileWrite.argtypes = [
        ctypes.c_void_p,
        ctypes.c_void_p,
        ctypes.c_size_t,
        ctypes.c_longlong,
        ctypes.c_longlong,
    ]

    cufile.cuFileDriverGetProperties.restype = CUfileError
    cufile.cuFileDriverGetProperties.argtypes = [ctypes.POINTER(CUfileDrvProps)]

    return cufile


def load_cuda():
    """Load libcudart.so.

    Returns the loaded library handle.

    Raises OSError if libcudart.so cannot be found.
    """
    return ctypes.CDLL("libcudart.so")


# ── CUDA helpers (minimal, no pycuda needed) ──────────────────────
def cuda_malloc(cuda, size):
    """Allocate device memory. Returns a c_void_p."""
    ptr = ctypes.c_void_p()
    rc = cuda.cudaMalloc(ctypes.byref(ptr), size)
    if rc != 0:
        raise RuntimeError(f"cudaMalloc failed: cudaError {rc}")
    return ptr


def cuda_free(cuda, ptr):
    """Free device memory."""
    rc = cuda.cudaFree(ptr)
    if rc != 0:
        print(f"  ⚠ cudaFree failed: cudaError {rc}")


def cuda_memcpy_h2d(cuda, dst, src_bytes, size):
    """Copy from host to device. cudaMemcpyHostToDevice=1."""
    rc = cuda.cudaMemcpy(dst, src_bytes, size, 1)
    if rc != 0:
        raise RuntimeError(f"cudaMemcpy H2D failed: cudaError {rc}")


def cuda_memcpy_d2h(cuda, dst_buf, src, size):
    """Copy from device to host. cudaMemcpyDeviceToHost=2."""
    rc = cuda.cudaMemcpy(dst_buf, src, size, 2)
    if rc != 0:
        raise RuntimeError(f"cudaMemcpy D2H failed: cudaError {rc}")


def cuda_memset(cuda, ptr, value, size):
    """Fill device memory with a byte value."""
    rc = cuda.cudaMemset(ptr, value, size)
    if rc != 0:
        print(f"  ⚠ cudaMemset returned {rc}")
