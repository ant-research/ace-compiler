# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
GDS (GPUDirect Storage / cuFile) environment diagnostics.

Checks runtime prerequisites for GDS — no GPU I/O required.
Useful inside Docker containers to quickly identify missing devices,
kernel modules, permissions, or configuration.

Usage (via CLI):
    ace_tool gds env-check
    ace_tool gds env-check --dir /data

Usage (programmatic):
    from ace.gds.check_env import run_env_check
    exit_code = run_env_check(test_dir="/tmp")
"""

import os
import subprocess

from .cufile import O_DIRECT


# ── Helpers ───────────────────────────────────────────────────────
def _run_cmd(cmd):
    """Run a shell command, return stripped stdout or None on failure."""
    try:
        out = subprocess.check_output(cmd, shell=True, stderr=subprocess.STDOUT, text=True, timeout=5).strip()
        return out if out else None
    except (subprocess.CalledProcessError, subprocess.TimeoutExpired, FileNotFoundError):
        return None


def _check_file(path, need_rw=False):
    """Check file existence and optionally read/write access. Returns True if OK."""
    if not os.path.exists(path):
        print(f"    ✗ {path} does NOT exist")
        return False
    print(f"    ✓ {path} exists")
    if need_rw:
        try:
            fd = os.open(path, os.O_RDWR)
            os.close(fd)
            print(f"    ✓ {path} is accessible (read+write)")
            return True
        except OSError as e:
            print(f"    ✗ {path} NOT accessible: {e}")
            return False
    return True


# ── Individual checks ─────────────────────────────────────────────
def _check_kernel_module():
    """Check nvidia-fs kernel module and /dev/nvidia-fs device."""
    print("  [1] nvidia-fs kernel module")
    out = _run_cmd("lsmod | grep nvidia_fs")
    if out:
        for line in out.splitlines():
            print(f"    {line}")
        print("    ✓ nvidia_fs module loaded")
    else:
        print("    ✗ nvidia_fs module NOT loaded")
        print("      → Install on host: apt install nvidia-gds")
        print("      → Or: modprobe nvidia_fs")
    return _check_file("/dev/nvidia-fs", need_rw=True)


def _check_nvidia_devices():
    """Check NVIDIA device nodes."""
    print("  [2] NVIDIA device nodes")
    out = _run_cmd("ls -la /dev/nvidia* 2>/dev/null")
    if out:
        for line in out.splitlines():
            print(f"    {line}")
    else:
        print("    ✗ No /dev/nvidia* devices found")
        print("      → Docker: add --gpus all")
        return False
    required = ["/dev/nvidiactl", "/dev/nvidia-uvm"]
    missing = [d for d in required if not os.path.exists(d)]
    if missing:
        print(f"    ✗ Missing required devices: {', '.join(missing)}")
        print("      → Docker: add --device <device> for each missing device")
        return False
    print("    ✓ Required devices present (nvidiactl, nvidia-uvm)")
    return True


def _check_nvidia_fs_version():
    """Check nvidia-fs driver version."""
    print("  [3] nvidia-fs driver version")
    try:
        with open("/proc/driver/nvidia-fs/version") as f:
            print(f"    {f.read().strip()}")
        return True
    except Exception:
        pass
    out = _run_cmd("modinfo nvidia_fs 2>/dev/null | grep -E '^(version|description):'")
    if out:
        for line in out.splitlines():
            print(f"    {line}")
        return True
    print("    ⚠ nvidia-fs version not available")
    return False


def _check_cufile_config():
    """Check cuFile configuration file."""
    print("  [4] cuFile configuration")
    config_paths = ["/etc/cufile.json", "/etc/cufile-1.7.2.json"]
    found = False
    for path in config_paths:
        if os.path.exists(path):
            try:
                with open(path) as f:
                    content = f.read().strip()
                print(f"    ✓ {path}")
                for line in content.splitlines():
                    print(f"      {line}")
                found = True
            except Exception as e:
                print(f"    ⚠ {path} exists but not readable: {e}")
    if not found:
        print("    ⚠ No cufile.json found (optional; defaults will be used)")
        print("      → Docker: add -v /etc/cufile.json:/etc/cufile.json:ro")
    return found


def _check_container():
    """Detect if running inside a container, print Docker flags if so."""
    print("  [5] Container detection")
    is_container = False
    if os.path.exists("/.dockerenv"):
        print("    ✓ Running inside Docker")
        is_container = True
    elif os.path.exists("/run/.containerenv"):
        print("    ✓ Running inside Podman/container")
        is_container = True
    else:
        try:
            with open("/proc/1/cgroup") as f:
                cgroup = f.read()
            if "docker" in cgroup or "containerd" in cgroup or "kubepods" in cgroup:
                print("    ✓ Running inside container (detected via cgroup)")
                is_container = True
            else:
                print("    Running on host (not container)")
        except Exception:
            print("    Unknown")

    if is_container:
        print("    Required Docker flags for GDS:")
        print("      --gpus all")
        print("      --device /dev/nvidia-fs")
        print("      --device /dev/nvidia0          (nvidia1, nvidia2... for multi-GPU)")
        print("      --device /dev/nvidiactl")
        print("      --device /dev/nvidia-uvm")
        print("      --device /dev/nvidia-uvm-tools")
        print("      --cap-add CAP_SYS_ADMIN")
        print("      -v /etc/cufile.json:/etc/cufile.json:ro")
    return is_container


def _check_filesystem(test_dir):
    """Check filesystem type and O_DIRECT support for test directory."""
    print(f"  [6] Filesystem info (for {test_dir})")
    out = _run_cmd(f"df -Th {test_dir} 2>/dev/null | tail -1")
    if out:
        parts = out.split()
        if len(parts) >= 2:
            fs_type = parts[1]
            mount_point = parts[-1] if len(parts) >= 6 else test_dir
            print(f"    Filesystem: {fs_type}, Mount: {mount_point}")
            if fs_type in ("tmpfs", "overlay", "devtmpfs"):
                print(f"    ✗ {fs_type} does NOT support O_DIRECT — cuFile will fail!")
                print("    → Use --dir to point to an ext4/xfs mount (e.g. --dir /data)")
                return False
            else:
                print(f"    ✓ {fs_type} should support O_DIRECT for cuFile")
        else:
            print(f"    {out}")
    else:
        print("    ⚠ Could not determine filesystem type")

    # Actually test O_DIRECT
    try:
        test_path = os.path.join(test_dir, ".gds_o_direct_test")
        fd = os.open(test_path, os.O_RDWR | os.O_CREAT | os.O_TRUNC | O_DIRECT, 0o644)
        os.close(fd)
        os.unlink(test_path)
        print(f"    ✓ O_DIRECT works on {test_dir}")
        return True
    except OSError as e:
        print(f"    ✗ O_DIRECT failed on {test_dir}: {e}")
        print("    → Use --dir to point to a disk-backed filesystem")
        return False


def _check_gds_lib():
    """Check libcufile.so availability."""
    print("  [7] libcufile.so library")
    out = _run_cmd("ldconfig -p 2>/dev/null | grep libcufile")
    if out:
        for line in out.splitlines():
            print(f"    {line.strip()}")
        print("    ✓ libcufile.so found in library cache")
        return True
    search_paths = [
        "/usr/local/cuda/targets/x86_64-linux/lib/",
        "/usr/local/cuda/lib64/",
        "/usr/lib/x86_64-linux-gnu/",
    ]
    for p in search_paths:
        full = os.path.join(p, "libcufile.so")
        if os.path.exists(full):
            print(f"    ✓ {full}")
            return True
    print("    ✗ libcufile.so NOT found")
    print("      → Install nvidia-gds package on host")
    print("      → Docker: mount the library from host (-v /usr/local/cuda:/usr/local/cuda)")
    return False


def _check_gpu():
    """Check GPU visibility."""
    print("  [8] GPU visibility")
    out = _run_cmd("nvidia-smi --query-gpu=name,pci.bus_id,driver_version --format=csv,noheader 2>/dev/null")
    if out:
        for line in out.splitlines():
            print(f"    {line}")
        return True
    cuda_dev = os.environ.get("CUDA_VISIBLE_DEVICES", "")
    if cuda_dev:
        print(f"    CUDA_VISIBLE_DEVICES={cuda_dev}")
    print("    ⚠ nvidia-smi not available or no GPU visible")
    print("      → Docker: add --gpus all")
    return False


def _check_nvidia_fs_proc():
    """Check nvidia-fs procfs entries."""
    print("  [9] nvidia-fs procfs entries")
    entries = [
        "/proc/driver/nvidia-fs/version",
        "/proc/driver/nvidia-fs/config",
        "/proc/driver/nvidia-fs/devices",
    ]
    found_any = False
    for entry in entries:
        if os.path.exists(entry):
            try:
                with open(entry) as f:
                    content = f.read().strip()
                name = os.path.basename(entry)
                if len(content) < 200:
                    print(f"    ✓ {name}: {content}")
                else:
                    print(f"    ✓ {name}: ({len(content)} bytes)")
                found_any = True
            except Exception:
                print(f"    ⚠ {entry} not readable")
        else:
            print(f"    ✗ {entry} not found")
    if not found_any:
        print("      → nvidia-fs procfs missing; likely module not loaded or not mounted in container")
        print("      → Docker: ensure host has nvidia_fs loaded, add --device /dev/nvidia-fs")
    return found_any


def _check_capabilities():
    """Check Linux capabilities (for GDS in containers)."""
    print("  [10] Linux capabilities")
    out = _run_cmd("cat /proc/self/status | grep Cap")
    if out:
        for line in out.splitlines():
            print(f"    {line}")
        cap_out = _run_cmd("capsh --print 2>/dev/null | grep Current")
        if cap_out:
            if "cap_sys_admin" in cap_out.lower():
                print("    ✓ CAP_SYS_ADMIN present")
            else:
                print("    ⚠ CAP_SYS_ADMIN may be missing")
                print("      → Docker: add --cap-add CAP_SYS_ADMIN")
    else:
        print("    ⚠ Cannot determine capabilities")
    return True


# ── Main entry point ──────────────────────────────────────────────
def run_env_check(test_dir="/tmp"):
    """Run all GDS environment diagnostic checks.

    Args:
        test_dir: Directory to check for O_DIRECT support.

    Returns:
        0 if critical checks pass, 1 otherwise.
    """
    print("=" * 56)
    print("  GDS Environment Diagnostics")
    print("=" * 56)

    results = {}

    print("\n─ Kernel & Driver ─")
    results["kernel_module"] = _check_kernel_module()
    results["nvidia_devices"] = _check_nvidia_devices()
    results["nvidia_fs_version"] = _check_nvidia_fs_version()
    results["nvidia_fs_proc"] = _check_nvidia_fs_proc()

    print("\n─ Configuration ─")
    results["cufile_config"] = _check_cufile_config()
    results["gds_lib"] = _check_gds_lib()

    print("\n─ Container ─")
    results["container"] = _check_container()
    results["capabilities"] = _check_capabilities()

    print("\n─ Filesystem & GPU ─")
    results["filesystem"] = _check_filesystem(test_dir)
    results["gpu"] = _check_gpu()

    # ── Summary ───────────────────────────────────────────────────
    print("\n" + "=" * 56)
    print("  Summary")
    print("=" * 56)

    critical = {
        "kernel_module": "nvidia-fs kernel module + /dev/nvidia-fs",
        "nvidia_devices": "NVIDIA device nodes (/dev/nvidia*)",
        "gds_lib": "libcufile.so library",
    }
    important = {
        "filesystem": "O_DIRECT support on test directory",
        "gpu": "GPU visibility (nvidia-smi)",
    }
    optional = {
        "nvidia_fs_version": "nvidia-fs driver version info",
        "cufile_config": "/etc/cufile.json",
        "nvidia_fs_proc": "nvidia-fs procfs entries",
        "container": "Container detection",
        "capabilities": "Linux capabilities",
    }

    for label, checks in [("Critical", critical), ("Important", important), ("Optional", optional)]:
        print(f"\n  {label}:")
        for key, desc in checks.items():
            status = "✓" if results.get(key) else "✗"
            print(f"    {status} {desc}")

    # Final verdict
    critical_ok = all(results.get(k) for k in critical)
    important_ok = all(results.get(k) for k in important)

    print()
    if critical_ok and important_ok:
        print("  RESULT: Environment looks READY for GDS ✓")
        print("  Run 'ace_tool gds check' to verify full I/O round-trip.")
    elif critical_ok:
        print("  RESULT: Critical checks pass, but some issues remain ⚠")
        print("  GDS may work with limitations; run 'ace_tool gds check' to test.")
    else:
        print("  RESULT: Environment NOT ready for GDS ✗")
        if results.get("container"):
            print("  Docker fix (add these flags):")
            print("    --gpus all")
            print("    --device /dev/nvidia-fs")
            print("    --device /dev/nvidia0 --device /dev/nvidiactl")
            print("    --device /dev/nvidia-uvm --device /dev/nvidia-uvm-tools")
            print("    --cap-add CAP_SYS_ADMIN")
            print("    -v /etc/cufile.json:/etc/cufile.json:ro")
        else:
            print("  Host fix:")
            print("    1. Install nvidia-gds: apt install nvidia-gds")
            print("    2. Load module: modprobe nvidia_fs")
            print("    3. Verify: lsmod | grep nvidia_fs && ls -la /dev/nvidia-fs")
    print("=" * 56)

    return 0 if critical_ok else 1
