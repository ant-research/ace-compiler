# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
GDS I/O round-trip verification.

Tests the full cuFile lifecycle: driver open → GPU alloc → buffer register →
file handle register → write (GPU→file) → read (file→GPU) → data verify.

Usage (via CLI):
    ace_tool gds check
    ace_tool gds check --size 1024 --dir /data
    ace_tool gds check --keep

Usage (programmatic):
    from ace.gds.check_io import run_io_check
    exit_code = run_io_check(size_kb=4, test_dir="/tmp", keep=False)
"""

import ctypes
import os

from .cufile import (
    CU_FILE_HANDLE_TYPE_OPAQUE_FD,
    O_DIRECT,
    CUfileDescr,
    CUfileDrvProps,
    CUfileError,
    cuda_free,
    cuda_malloc,
    cuda_memcpy_d2h,
    cuda_memcpy_h2d,
    cuda_memset,
    cufile_strerror,
    load_cuda,
    load_cufile,
)


def run_io_check(size_kb=4, test_dir="/tmp", keep=False):
    """Run the GDS cuFile I/O round-trip test.

    Args:
        size_kb: Test buffer size in KB (will be rounded up to 4KB alignment).
        test_dir: Directory for the temporary test file.
        keep: If True, keep the test file after the run.

    Returns:
        0 on success, 1 on failure.
    """
    size_bytes = size_kb * 1024
    if size_bytes % 4096 != 0:
        size_bytes = ((size_bytes + 4095) // 4096) * 4096
        print(f"  [i] Rounded size up to {size_bytes // 1024} KB for O_DIRECT alignment")

    all_ok = True
    d_ptr = None
    d_ptr2 = None
    fd = -1
    cf_handle = ctypes.c_void_p()
    cufile = None
    cuda = None

    # ── Step 0: Load libraries ────────────────────────────────────
    print("=" * 56)
    print("  GDS (cuFile) I/O Verification")
    print("=" * 56)

    print("\n[0] Loading libraries")
    try:
        cufile = load_cufile()
        print("  ✓ libcufile.so loaded")
    except OSError as e:
        print(f"  ✗ libcufile.so not found: {e}")
        return 1

    try:
        cuda = load_cuda()
        print("  ✓ libcudart.so loaded")
    except OSError as e:
        print(f"  ✗ libcudart.so not found: {e}")
        return 1

    # ── Step 1: cuFileDriverOpen ──────────────────────────────────
    print("\n[1] cuFileDriverOpen")
    rc = cufile.cuFileDriverOpen()
    if not rc.ok:
        print(f"  ✗ cuFileDriverOpen: {rc}")
        return 1
    print("  ✓ cuFileDriverOpen")

    # ── Step 2: Allocate GPU buffer ───────────────────────────────
    print(f"\n[2] GPU buffer allocation ({size_bytes // 1024} KB)")
    try:
        d_ptr = cuda_malloc(cuda, size_bytes)
        print(f"  ✓ cudaMalloc: ptr={hex(d_ptr.value)}")
    except RuntimeError as e:
        print(f"  ✗ {e}")
        cufile.cuFileDriverClose()
        return 1

    src_data = b"\xab" * size_bytes
    cuda_memcpy_h2d(cuda, d_ptr, src_data, size_bytes)
    print("  ✓ Uploaded test pattern (0xAB) to GPU")

    # ── Step 3: cuFileBufRegister ─────────────────────────────────
    print("\n[3] cuFileBufRegister")
    rc = cufile.cuFileBufRegister(d_ptr, size_bytes, 0)
    if not rc.ok:
        print(f"  ✗ cuFileBufRegister: {rc}")
        all_ok = False
    else:
        print(f"  ✓ cuFileBufRegister(ptr={hex(d_ptr.value)}, size={size_bytes})")

    # ── Step 4: cuFileHandleRegister ──────────────────────────────
    test_file = os.path.join(test_dir, "gds_check_test.bin")
    print(f"\n[4] cuFileHandleRegister: {test_file}")

    fd = os.open(test_file, os.O_RDWR | os.O_CREAT | os.O_TRUNC | O_DIRECT, 0o644)
    print(f"  ✓ POSIX open(fd={fd}, O_DIRECT)")

    if all_ok:
        descr = CUfileDescr()
        descr.type = CU_FILE_HANDLE_TYPE_OPAQUE_FD
        descr.handle.fd = fd
        descr.fs_ops = None

        cf_handle = ctypes.c_void_p()
        rc = cufile.cuFileHandleRegister(ctypes.byref(cf_handle), ctypes.byref(descr))
        if not rc.ok:
            print(f"  ✗ cuFileHandleRegister: {rc}")
            if rc.err == 5019:
                print("    → CU_FILE_INVALID_FILE_OPEN_FLAG: file opened without O_DIRECT or unsupported flags")
            elif rc.err == 5008:
                print("    → CU_FILE_IO_NOT_SUPPORTED: filesystem does not support GDS")
                print("    → If /tmp is tmpfs, try: --dir /data")
            elif rc.err == 5020:
                print("    → CU_FILE_DIO_NOT_SET: file descriptor missing O_DIRECT")
            elif rc.err == 5010:
                print("    → CU_FILE_NVFS_DRIVER_ERROR: check nvidia-fs driver")
            all_ok = False
        else:
            print(f"  ✓ cuFileHandleRegister(fd={fd}) → handle={hex(cf_handle.value or 0)}")
    else:
        print("  ⏭ Skipped (prior step failed)")

    # ── Step 5: cuFileWrite (GPU → file) ─────────────────────────
    print(f"\n[5] cuFileWrite (GPU → file, {size_bytes // 1024} KB)")
    if all_ok:
        bytes_written = cufile.cuFileWrite(cf_handle, d_ptr, size_bytes, 0, 0)
        if bytes_written < 0:
            print(f"  ✗ cuFileWrite failed: {cufile_strerror(int(bytes_written))} ({bytes_written})")
            all_ok = False
        elif bytes_written != size_bytes:
            print(f"  ✗ cuFileWrite short write: {bytes_written}/{size_bytes}")
            all_ok = False
        else:
            print(f"  ✓ cuFileWrite: {bytes_written} bytes written")
    else:
        print("  ⏭ Skipped (prior step failed)")

    # ── Step 6: cuFileRead (file → GPU) ──────────────────────────
    print(f"\n[6] cuFileRead (file → GPU, {size_bytes // 1024} KB)")
    if all_ok:
        d_ptr2 = cuda_malloc(cuda, size_bytes)
        cuda_memset(cuda, d_ptr2, 0, size_bytes)

        bytes_read = cufile.cuFileRead(cf_handle, d_ptr2, size_bytes, 0, 0)
        if bytes_read < 0:
            print(f"  ✗ cuFileRead failed: {cufile_strerror(int(bytes_read))} ({bytes_read})")
            all_ok = False
        elif bytes_read != size_bytes:
            print(f"  ✗ cuFileRead short read: {bytes_read}/{size_bytes}")
            all_ok = False
        else:
            print(f"  ✓ cuFileRead: {bytes_read} bytes read")
    else:
        print("  ⏭ Skipped (prior step failed)")

    # ── Step 7: Verify data ──────────────────────────────────────
    print("\n[7] Verify round-trip data")
    if all_ok and d_ptr2:
        read_buf = ctypes.create_string_buffer(size_bytes)
        cuda_memcpy_d2h(cuda, read_buf, d_ptr2, size_bytes)
        read_data = read_buf.raw
        if read_data == src_data:
            print(f"  ✓ Data matches! {size_bytes} bytes verified (0xAB pattern)")
        else:
            mismatch = sum(a != b for a, b in zip(read_data, src_data))
            print(f"  ✗ Data mismatch: {mismatch}/{size_bytes} bytes differ")
            for i, (a, b) in enumerate(zip(read_data, src_data)):
                if a != b:
                    print(f"    First diff at offset {i}: got 0x{a:02X}, expected 0x{b:02X}")
                    break
            all_ok = False
    else:
        print("  ⏭ Skipped (prior step failed)")

    # ── Step 8: Driver properties ────────────────────────────────
    print("\n[8] Driver properties")
    try:
        props = CUfileDrvProps()
        rc = cufile.cuFileDriverGetProperties(ctypes.byref(props))
        if rc.ok:
            print(f"  ✓ nvidia-fs version: {props.nvfs.major_version}.{props.nvfs.minor_version}")
            print(f"    poll_thresh_size:        {props.nvfs.poll_thresh_size}")
            print(f"    max_direct_io_size:      {props.nvfs.max_direct_io_size}")
            print(f"    max_device_cache_size:   {props.max_device_cache_size} KB")
            print(f"    max_device_pinned_mem:   {props.max_device_pinned_mem_size} KB")
        else:
            print(f"  ⚠ cuFileDriverGetProperties: {rc}")
    except Exception as e:
        print(f"  ⚠ Could not query driver properties: {e}")

    # ── Cleanup ──────────────────────────────────────────────────
    print("\n[9] Cleanup")

    if d_ptr and d_ptr.value:
        rc = cufile.cuFileBufDeregister(d_ptr)
        print(f"  {'✓' if rc.ok else '✗'} cuFileBufDeregister(write_buf): {rc}")
    if d_ptr2 and d_ptr2.value:
        rc = cufile.cuFileBufDeregister(d_ptr2)
        print(f"  {'✓' if rc.ok else '✗'} cuFileBufDeregister(read_buf): {rc}")

    if cf_handle.value:
        cufile.cuFileHandleDeregister(cf_handle)
        print("  ✓ cuFileHandleDeregister")

    if cufile:
        cufile.cuFileDriverClose()
        print("  ✓ cuFileDriverClose")

    if fd >= 0:
        os.close(fd)
        print("  ✓ POSIX close(fd)")

    if cuda:
        if d_ptr and d_ptr.value:
            cuda_free(cuda, d_ptr)
        if d_ptr2 and d_ptr2.value:
            cuda_free(cuda, d_ptr2)
        print("  ✓ cudaFree")

    if not keep:
        try:
            os.unlink(test_file)
            print(f"  ✓ Removed {test_file}")
        except OSError:
            pass
    else:
        print(f"  [i] Kept test file: {test_file}")

    # ── Result ────────────────────────────────────────────────────
    print("\n" + "=" * 56)
    if all_ok:
        print("  RESULT: GDS WORKING ✓")
        print(f"  Successfully wrote & read {size_bytes // 1024} KB via cuFile")
    else:
        print("  RESULT: GDS NOT WORKING ✗")
        print("  Run 'ace_tool gds env-check' to diagnose environment issues.")
    print("=" * 56)

    return 0 if all_ok else 1
