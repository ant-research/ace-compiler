# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
ACE FHE environment diagnostics.

Checks Python, compiled libraries, GPU/CUDA, backends,
model assets, and dataset readiness — no side effects.

Usage (via CLI):
    ace_tool doctor

Usage (programmatic):
    from ace.fhe.check_env import run_doctor
    exit_code = run_doctor()
"""

import logging
import os
import subprocess
import sys
import sysconfig
from pathlib import Path


# ── Helpers ───────────────────────────────────────────────────────


def _ace_root() -> Path:
    """Get the installed ace root directory (same as Backend._get_ace_root)."""
    platlib = sysconfig.get_path("platlib")
    return Path(platlib) / "ace"


def _ok(msg):
    print(f"  ✓ {msg}")


def _fail(msg):
    print(f"  ✗ {msg}")


def _warn(msg):
    print(f"  ⚠ {msg}")


def _hint(msg):
    print(f"    → {msg}")


def _info(msg):
    print(f"    {msg}")


# ── Check 1: Python Environment ──────────────────────────────────


def _check_python_env() -> dict[str, bool]:
    """Check Python version, ace package, and key dependencies."""
    results = {}

    # Python version
    py_ver = f"{sys.version_info.major}.{sys.version_info.minor}.{sys.version_info.micro}"
    if sys.version_info >= (3, 10):  # noqa: UP036
        _ok(f"Python {py_ver} (≥ 3.10)")
        results["python_version"] = True
    else:
        _fail(f"Python {py_ver} (< 3.10)")
        _hint("Upgrade Python to >= 3.10")
        results["python_version"] = False

    # ace package
    try:
        import ace

        _ok(f"ace {ace.__version__}")
        results["ace_package"] = True
    except ImportError:
        _fail("ace package not importable")
        _hint("Install ace-fhe: pip install -e . --no-build-isolation")
        results["ace_package"] = False
        # Cannot check further — return early
        results["ace_frontend_ext"] = False
        results["ace_runtime_ext"] = False
        results["torch"] = False
        results["numpy"] = False
        results["onnx"] = False
        return results

    # C++ extensions
    if ace.HAS_FRONTEND:
        _ok("ace.frontend C++ extension")
    else:
        _fail("ace.frontend C++ extension not loaded")
        _hint("Reinstall ace-fhe (C++ frontend extension missing)")
    results["ace_frontend_ext"] = ace.HAS_FRONTEND

    if ace.HAS_RUNTIME:
        _ok("ace.runtime C++ extension")
    else:
        _fail("ace.runtime C++ extension not loaded")
        _hint("Reinstall ace-fhe (C++ runtime extension missing)")
    results["ace_runtime_ext"] = ace.HAS_RUNTIME

    # torch
    try:
        import torch

        cuda_suffix = f"+{torch.version.cuda}" if torch.version.cuda else ""
        _ok(f"torch {torch.__version__}{cuda_suffix}")
        results["torch"] = True
    except ImportError:
        _fail("torch not installed")
        _hint("Install PyTorch: pip install torch")
        results["torch"] = False

    # numpy
    try:
        import numpy as np

        _ok(f"numpy {np.__version__}")
        results["numpy"] = True
    except ImportError:
        _fail("numpy not installed")
        _hint("Install numpy: pip install numpy")
        results["numpy"] = False

    # onnx (optional)
    try:
        import onnx

        _warn(f"onnx {onnx.__version__} (optional)")
        results["onnx"] = True
    except ImportError:
        _warn("onnx not installed (optional)")
        _hint("pip install onnx  (for ONNX model import)")
        results["onnx"] = True  # optional, don't fail

    return results


# ── Check 2: Compiled Libraries ──────────────────────────────────


def _check_compiled_libs() -> dict[str, bool]:
    """Check compiled binaries and shared libraries."""
    results = {}
    root = _ace_root()

    # fhe_cmplr binary
    cmplr_path = root / "bin" / "fhe_cmplr"
    if cmplr_path.exists():
        try:
            subprocess.run(
                [str(cmplr_path), "-h"],
                capture_output=True,
                timeout=5,
            )
            _ok("fhe_cmplr binary")
            results["fhe_cmplr"] = True
        except (subprocess.TimeoutExpired, OSError) as e:
            _fail(f"fhe_cmplr binary (run failed: {e})")
            _hint("Reinstall ace-fhe or rebuild from source")
            results["fhe_cmplr"] = False
    else:
        _fail(f"fhe_cmplr binary not found ({cmplr_path})")
        _hint("Reinstall ace-fhe or rebuild from source")
        results["fhe_cmplr"] = False

    # Runtime shared libraries
    rt_libs = [
        ("libFHErt_common.so", True),  # critical
        ("libFHErt_ant.so", False),  # important
        ("libFHErt_phantom.so", False),  # important
        ("libFHErt_ace.so", False),  # important
    ]
    for lib_name, is_critical in rt_libs:
        lib_path = root / "lib" / lib_name
        if lib_path.exists():
            size_mb = lib_path.stat().st_size / (1024 * 1024)
            _ok(f"{lib_name} ({size_mb:.1f} MB)")
            results[lib_name] = True
        else:
            level = _fail if is_critical else _warn
            level(f"{lib_name} not found")
            _hint("Reinstall ace-fhe or rebuild from source")
            results[lib_name] = not is_critical  # non-critical libs don't fail

    return results


# ── Check 3: GPU & CUDA ──────────────────────────────────────────


def _check_gpu_cuda() -> dict[str, bool]:
    """Check GPU and CUDA availability. All informational — CPU works without GPU."""
    results = {}

    try:
        import torch
    except ImportError:
        _warn("torch not installed — skipping GPU/CUDA checks")
        results["torch_cuda"] = False
        return results

    if torch.cuda.is_available():
        count = torch.cuda.device_count()
        _ok(f"torch.cuda available ({count} device{'s' if count != 1 else ''})")
        results["torch_cuda"] = True

        # Device info
        try:
            for i in range(min(count, 4)):  # Show first 4 devices max
                name = torch.cuda.get_device_name(i)
                mem_gb = torch.cuda.get_device_properties(i).total_memory / (1024**3)
                _info(f"Device {i}: {name} ({mem_gb:.0f} GB)")
            if count > 4:
                _info(f"... and {count - 4} more")
        except Exception:
            pass

        # CUDA version
        if torch.version.cuda:
            _ok(f"CUDA {torch.version.cuda}")
        results["cuda_version"] = True
    else:
        _warn("torch.cuda not available — GPU backends will not work")
        _hint("Install CUDA-enabled PyTorch or check GPU driver")
        results["torch_cuda"] = False
        results["cuda_version"] = False

    # CUDA_VISIBLE_DEVICES
    cuda_vis = os.environ.get("CUDA_VISIBLE_DEVICES", "")
    if cuda_vis:
        _info(f"CUDA_VISIBLE_DEVICES={cuda_vis}")
    else:
        _info("CUDA_VISIBLE_DEVICES=(not set)")

    return results


# ── Check 4: Backend Availability ────────────────────────────────


def _check_backends() -> dict[str, bool]:
    """Check FHE backend availability using the registry."""
    results = {}

    # Labels for display
    _COMBO_LABELS = {
        ("ant", "cpu"): "ant (CPU) — always built",
        ("phantom", "cuda"): "phantom (CUDA)",
        ("ace", "cuda"): "ace (CUDA)",
        ("seal", "cpu"): "seal (CPU)",
        ("openfhe", "cpu"): "openfhe (CPU)",
    }

    try:
        from ace.fhe.driver.registry import get_library_impl, list_supported_combos
    except ImportError:
        _fail("Backend registry not importable")
        results["backend_registry"] = False
        return results

    combos = list_supported_combos()
    if not combos:
        _warn("No backends registered")
        results["backend_registry"] = False
        return results

    results["backend_registry"] = True

    for lib, dev in combos:
        label = _COMBO_LABELS.get((lib, dev), f"{lib} ({dev})")
        try:
            impl = get_library_impl(lib, device=dev)
            available = impl.check_available()
            implemented = getattr(impl, "implemented", True)

            if not implemented:
                _warn(f"{label} — not implemented")
                results[f"backend_{lib}"] = False
            elif available:
                _ok(label)
                results[f"backend_{lib}"] = True
            else:
                _warn(f"{label} — not available on this system")
                results[f"backend_{lib}"] = lib == "ant"  # ant failure = important
        except Exception as e:
            _warn(f"{label} — check failed ({type(e).__name__})")
            results[f"backend_{lib}"] = False

    return results


# ── Check 5: Model Assets ────────────────────────────────────────


def _check_model_assets() -> dict[str, bool]:
    """Check ResNet model weights and VR profiles."""
    results = {}

    try:
        from ace.model.spec_resnet import ALL_RESNET_SPECS
    except ImportError:
        _warn("ResNet specs not importable — skipping model asset checks")
        results["model_assets"] = False
        return results

    # Weights directory
    try:
        from ace.model.spec_resnet import _WEIGHTS_DIR

        if os.path.isdir(_WEIGHTS_DIR):
            weight_files = [f for f in os.listdir(_WEIGHTS_DIR) if f.endswith(".pt")]
            _ok(f"Weights directory ({len(weight_files)} .pt files)")
            results["weights_dir"] = True
        else:
            _fail(f"Weights directory not found ({_WEIGHTS_DIR})")
            _hint("Reinstall ace-fhe (model weights missing)")
            results["weights_dir"] = False
    except ImportError:
        _fail("Cannot locate weights directory")
        results["weights_dir"] = False

    # Per-model checks
    all_profiles_ok = True
    for spec in ALL_RESNET_SPECS:
        # Weight file: spec.name is e.g. "resnet20_cifar10" → "resnet20_cifar10.pt"
        weight_file = f"{spec.name}.pt"
        weight_path = os.path.join(_WEIGHTS_DIR, weight_file) if results.get("weights_dir") else ""

        weight_ok = os.path.exists(weight_path) if weight_path else False

        # VR profile
        profile_ok = False
        try:
            vr_file = spec.get_vr_profile()
            profile_ok = vr_file is not None
        except FileNotFoundError:
            pass
        except Exception:
            pass

        if weight_ok and profile_ok:
            _ok(f"{spec.name}: weights + VR profile")
        elif weight_ok:
            _warn(f"{spec.name}: weights OK, VR profile missing")
            _hint(f"Run: ace_tool relu-profile --model {spec.name}")
            all_profiles_ok = False
        else:
            _fail(f"{spec.name}: weights missing")
            results["weights_dir"] = False

    results["vr_profiles"] = all_profiles_ok
    return results


# ── Check 6: Dataset ─────────────────────────────────────────────


def _check_dataset() -> dict[str, bool]:
    """Check CIFAR-10/100 dataset availability (read-only, no downloads)."""
    results = {}

    # Data root detection (mirrors _get_data_root from dataset.py, no side effects)
    env_data_dir = os.environ.get("ACE_DATA_DIR")
    system_data_dir = Path("/opt/dataset")
    project_root = Path(__file__).resolve().parent.parent.parent.parent

    if env_data_dir:
        data_root = Path(env_data_dir)
        source = f"ACE_DATA_DIR={env_data_dir}"
    elif system_data_dir.exists():
        data_root = system_data_dir
        source = "/opt/dataset (system)"
    else:
        data_root = project_root / "data"
        source = f"{data_root} (project fallback)"

    _info(f"Data root: {data_root}  ({source})")

    # CIFAR-10
    cifar10_test = data_root / "cifar10" / "cifar-10-batches-py" / "test_batch"
    if cifar10_test.exists():
        size_mb = cifar10_test.stat().st_size / (1024 * 1024)
        _ok(f"CIFAR-10 test data ({size_mb:.0f} MB)")
        results["cifar10"] = True
    else:
        _warn("CIFAR-10 test data not found")
        _hint("Dataset will auto-download on first use, or set ACE_DATA_DIR")
        results["cifar10"] = True  # auto-download available, don't fail

    # CIFAR-100
    cifar100_test = data_root / "cifar100" / "cifar-100-python" / "test"
    if cifar100_test.exists():
        _ok("CIFAR-100 test data")
        results["cifar100"] = True
    else:
        _warn("CIFAR-100 test data not found (optional)")
        _hint("Dataset will auto-download on first use, or set ACE_DATA_DIR")
        results["cifar100"] = True  # optional, auto-download available

    return results


# ── Check 7: Include Headers ─────────────────────────────────────


def _check_include_headers() -> dict[str, bool]:
    """Check include headers directory for kernel compilation."""
    results = {}
    root = _ace_root()
    include_dir = root / "include"

    if not include_dir.is_dir():
        _warn(f"Include directory not found ({include_dir})")
        _hint("Only needed for kernel recompilation; safe to ignore for binary installs")
        results["include_dir"] = False
        return results

    subdirs = sorted(d.name for d in include_dir.iterdir() if d.is_dir())
    _ok(f"include/ ({len(subdirs)} subdirectories)")

    expected_subdirs = ["lib", "lib_ace", "lib_phantom", "rt_ant", "common"]
    all_ok = True
    for sub in expected_subdirs:
        sub_path = include_dir / sub
        if sub_path.is_dir():
            _ok(f"include/{sub}/")
        else:
            _warn(f"include/{sub}/ not found")
            all_ok = False

    results["include_dir"] = all_ok
    return results


# ── Main entry point ──────────────────────────────────────────────


def run_doctor() -> int:
    """Run all ACE FHE environment diagnostic checks.

    Returns:
        0 if all critical checks pass, 1 otherwise.
    """
    # Suppress INFO-level log output from backend registration during checks
    logging.getLogger("ace.fhe.backend").setLevel(logging.WARNING)

    print("=" * 56)
    print("  ACE FHE Environment Diagnostics")
    print("=" * 56)

    all_results: dict[str, bool] = {}

    print("\n── Python Environment ──")
    all_results.update(_check_python_env())

    print("\n── Compiled Libraries ──")
    all_results.update(_check_compiled_libs())

    print("\n── GPU & CUDA ──")
    all_results.update(_check_gpu_cuda())

    print("\n── Backend Availability ──")
    all_results.update(_check_backends())

    print("\n── Model Assets (ResNet) ──")
    all_results.update(_check_model_assets())

    print("\n── Dataset ──")
    all_results.update(_check_dataset())

    print("\n── Include Headers ──")
    all_results.update(_check_include_headers())

    # ── Summary ───────────────────────────────────────────────────
    print("\n" + "=" * 56)
    print("  Summary")
    print("=" * 56)

    critical = {
        "python_version": "Python ≥ 3.10",
        "ace_package": "ace package",
        "ace_frontend_ext": "ace.frontend C++ extension",
        "ace_runtime_ext": "ace.runtime C++ extension",
        "fhe_cmplr": "fhe_cmplr binary",
        "libFHErt_common.so": "libFHErt_common.so",
    }
    important = {
        "torch": "torch installed",
        "torch_cuda": "torch.cuda available",
        "backend_ant": "ant backend (CPU)",
        "weights_dir": "ResNet weights directory",
        "cifar10": "CIFAR-10 dataset",
    }
    optional = {
        "onnx": "onnx package",
        "numpy": "numpy",
        "libFHErt_phantom.so": "phantom GPU library",
        "libFHErt_ace.so": "ace GPU library",
        "backend_phantom": "phantom backend (CUDA)",
        "backend_ace": "ace backend (CUDA)",
        "cifar100": "CIFAR-100 dataset",
        "vr_profiles": "VR profiles",
        "include_dir": "Include headers",
    }

    for label, checks in [("Critical", critical), ("Important", important), ("Optional", optional)]:
        print(f"\n  {label}:")
        for key, desc in checks.items():
            val = all_results.get(key)
            if val is None:
                _info(f"  {desc} (not checked)")
            elif val:
                _ok(desc)
            else:
                _fail(desc) if label == "Critical" else _warn(desc)

    # Final verdict
    critical_ok = all(all_results.get(k) for k in critical)
    important_ok = all(all_results.get(k) for k in important)

    print()
    if critical_ok and important_ok:
        print("  RESULT: Environment READY ✓")
    elif critical_ok:
        print("  RESULT: Critical checks pass, but some issues remain ⚠")
    else:
        print("  RESULT: Environment NOT ready ✗")
        print("  Fix critical issues above before running FHE inference.")
    print("=" * 56)

    return 0 if critical_ok else 1
