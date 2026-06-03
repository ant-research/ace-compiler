#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
FHE Compilation Cache.

Three-level cache structure:
- /tmp/ace-compile-cache/<entity>-<frontend>-<library>-<device>/   # Level 1: 基础模型
    ├── meta.json                                                    # 主记录
    └── configs/
        └── input_<shape>_<dtype>/                                   # Level 2: 输入配置
            └── full_<config_hash>/                                   # Level 3: 完整配置hash
                ├── kernel.so
                ├── model.conf
                └── meta.json

Level 1: entity-frontend-library-device - 标识同一个模型
Level 2: input shape/dtype - 固定输入，尝试不同编译参数
Level 3: fhe_cmplr参数 + gcc/nvcc参数 合并后的hash

Usage:
    cache_key = generate_cache_key(model, frontend, library, device)
    config_key = get_config_key(compile_options, build_options, input_tensors)
    if is_cache_valid(cache_key, config_key):
        return load_cached(cache_key, config_key)

    package = compile(...)
    save_cache(cache_key, config_key, package)
    return package
"""
import hashlib
import json
import logging
import time
from pathlib import Path
from typing import Optional, List, Dict, Any

logger = logging.getLogger(__name__)

# Default directories
DEFAULT_CACHE_DIR = "/tmp/ace-compile-cache"

# Global configuration
_config = {
    "cache_dir": DEFAULT_CACHE_DIR,
    "force_rebuild": False,
}

# Original project directory (set when cache operations start)
_project_root = None


def _get_project_root() -> Path:
    """Get project root directory (where cache records are stored)."""
    global _project_root
    if _project_root is not None:
        return _project_root
    return Path.cwd()


def _set_project_root(path: Path) -> None:
    """Set project root directory."""
    global _project_root
    _project_root = path


def configure_cache(**kwargs) -> None:
    """Configure cache settings.

    Args:
        cache_dir: Directory for cache records (default: "/tmp/ace-compile-cache")
        force_rebuild: Force recompilation (default: False)
    """
    global _config
    for key in kwargs:
        if key in _config:
            _config[key] = kwargs[key]


def get_cache_dir() -> Path:
    """Get cache records directory."""
    return Path(_config["cache_dir"])


def is_force_rebuild() -> bool:
    return _config.get("force_rebuild", False)


# ============================================================================
# Cache Key Generation
# ============================================================================

def generate_cache_key(target, frontend: str, library: str, device: str) -> str:
    """Generate Level 1 cache key from target and basic parameters.

    Format: <entity>-<frontend>-<library>-<device>

    Priority for entity name:
    1. _fhe_name on the original model (set by CompileSpec.create)
    2. Function name (for plain functions)
    3. Class name (fallback)
    """
    entity_name = "unknown"
    if target is not None:
        if hasattr(target, "_original_model"):
            # Wrapped model from @compile decorator
            model = target._original_model
            if hasattr(model, "_fhe_name"):
                entity_name = model._fhe_name
            else:
                entity_name = model.__class__.__name__
        elif hasattr(target, "__name__"):
            entity_name = target.__name__
        elif hasattr(target, "name"):
            entity_name = target.name
        elif hasattr(target, "__class__"):
            entity_name = target.__class__.__name__

    return f"{entity_name}-{frontend}-{library}-{device}"


def get_input_tensors_hash(input_tensors) -> str:
    """Get hash of input tensor shapes and dtypes for Level 2 config key.

    Returns a directory-safe string: "shape_HxW_dtype_float32"
    """
    if not input_tensors:
        return ""

    tensor_info = []
    for t in input_tensors:
        if hasattr(t, 'shape') and hasattr(t, 'dtype'):
            shape_str = "x".join(str(d) for d in t.shape)
            dtype_str = str(t.dtype).replace('.', '_')
            tensor_info.append(f"shape_{shape_str}_dtype_{dtype_str}")

    if not tensor_info:
        return ""

    # For directory name, use first tensor's shape/dtype
    # Multiple inputs would need more complex handling
    return tensor_info[0]


def get_input_tensors_config_hash(input_tensors) -> str:
    """Get hash of input tensor info for Level 3 config validation."""
    if not input_tensors:
        return ""

    tensor_info = []
    for t in input_tensors:
        if hasattr(t, 'shape') and hasattr(t, 'dtype'):
            tensor_info.append({
                "shape": list(t.shape),
                "dtype": str(t.dtype)
            })
    if not tensor_info:
        return ""

    opts_str = json.dumps(tensor_info, sort_keys=True, default=str)
    return hashlib.sha256(opts_str.encode()).hexdigest()[:16]


def get_compile_options_hash(compile_options: dict) -> str:
    """Get hash of fhe_cmplr compile options."""
    if not compile_options:
        return ""
    opts_str = json.dumps(compile_options, sort_keys=True, default=str)
    return hashlib.sha256(opts_str.encode()).hexdigest()[:16]


def get_relu_vr_hash(relu_vr_data: dict) -> str:
    """Get hash of ReLU VR data for cache key generation.

    VR data affects compilation output (polynomial degree, scale management),
    so different VR values must produce different cache keys.
    """
    if not relu_vr_data:
        return ""
    vr_str = json.dumps(relu_vr_data, sort_keys=True)
    return hashlib.sha256(vr_str.encode()).hexdigest()[:16]


def get_build_options_hash(build_options: dict) -> str:
    """Get hash of gcc/nvcc build options."""
    if not build_options:
        return ""
    opts_str = json.dumps(build_options, sort_keys=True, default=str)
    return hashlib.sha256(opts_str.encode()).hexdigest()[:16]


def get_full_config_hash(compile_options: dict, build_options: dict,
                         relu_vr_data: dict = None) -> str:
    """Get combined hash of compile options + build options + VR data (Level 3)."""
    full_config = {
        "compile": compile_options or {},
        "build": build_options or {},
    }
    if relu_vr_data:
        full_config["relu_vr"] = get_relu_vr_hash(relu_vr_data)
    opts_str = json.dumps(full_config, sort_keys=True, default=str)
    return hashlib.sha256(opts_str.encode()).hexdigest()[:16]


# ============================================================================
# Cache Path Utilities
# ============================================================================

def get_cache_path(cache_key: str, input_tensors=None,
                   compile_options: dict = None, build_options: dict = None,
                   relu_vr_data: dict = None) -> Path:
    """Get full cache path for a specific configuration.

    Three-level structure (no configs layer):
    - Level 1: entity-frontend-library-device
    - Level 2: input_shape_dtype (固定输入，尝试不同编译参数)
    - Level 3: compile_options_hash (fhe_cmplr参数)

    Returns: Path to the config directory
    """
    base_dir = _get_project_root() / get_cache_dir() / cache_key

    # Level 2: input shape/dtype
    input_str = get_input_tensors_hash(input_tensors) or "default"
    level2_dir = base_dir / input_str

    # Level 3: compile_options + relu_vr hash
    vr_hash = get_relu_vr_hash(relu_vr_data) if relu_vr_data else ""
    compile_hash = get_compile_options_hash(compile_options) or "default"
    if vr_hash:
        compile_hash = compile_hash + "_vr" + vr_hash
    level3_dir = level2_dir / compile_hash

    return level3_dir


# ============================================================================
# Cache Operations
# ============================================================================

def has_cache(cache_key: str) -> bool:
    """Check if cache with given key exists (Level 1)."""
    cache_dir = _get_project_root() / get_cache_dir() / cache_key
    return cache_dir.exists()


def load_cache(cache_key: str, config_path: Path = None) -> Optional[dict]:
    """Load cache record from specific config path."""
    if config_path is None:
        # Try to find any config directory
        cache_dir = _get_project_root() / get_cache_dir() / cache_key
        configs_dir = cache_dir / "configs"
        if not configs_dir.exists():
            return None
        # Find first config directory
        for input_dir in configs_dir.iterdir():
            if input_dir.is_dir():
                for full_dir in input_dir.iterdir():
                    if full_dir.is_dir():
                        config_path = full_dir
                        break
                if config_path:
                    break

    if config_path is None:
        return None

    meta_file = config_path / "meta.json"
    if not meta_file.exists():
        return None

    try:
        with open(meta_file, "r") as f:
            return json.load(f)
    except (json.JSONDecodeError, IOError):
        return None


def save_cache(cache_key: str, config_path: Path, record: dict) -> None:
    """Save cache record to specific config path."""
    config_path.mkdir(parents=True, exist_ok=True)

    record["timestamp"] = time.strftime("%Y-%m-%dT%H:%M:%S")

    with open(config_path / "meta.json", "w") as f:
        json.dump(record, f, indent=2)


def is_cache_valid(cache_key: str, input_tensors=None,
                   compile_options: dict = None, build_options: dict = None,
                   relu_vr_data: dict = None) -> bool:
    """Check if cache record is valid for given configuration."""
    config_path = get_cache_path(cache_key, input_tensors, compile_options, build_options,
                                 relu_vr_data)
    record = load_cache(cache_key, config_path)

    if not record:
        return False

    # Validate compile_options hash (Level 3)
    if compile_options:
        cached_hash = record.get("compile_options_hash", "")
        current_hash = get_compile_options_hash(compile_options)
        if cached_hash != current_hash:
            return False

    # Validate relu_vr hash
    if relu_vr_data:
        cached_vr_hash = record.get("relu_vr_hash", "")
        current_vr_hash = get_relu_vr_hash(relu_vr_data)
        if cached_vr_hash != current_vr_hash:
            return False

    # Note: build_options is not used for cache key (can be changed without recompiling IR)

    return True


# ============================================================================
# High-level compile interface
# ============================================================================

def compile_with_cache(cache_key: str, compile_options: dict, build_options: dict,
                       input_tensors, compile_func, *args, relu_vr_data=None, **kwargs):
    """Execute compilation with automatic cache management.

    Three-level cache structure:
    - Level 1: entity-frontend-library-device
    - Level 2: input_shape_dtype
    - Level 3: full_config_hash (compile_options + build_options + relu_vr_data)

    Args:
        cache_key: Level 1 cache key
        compile_options: fhe_cmplr options
        build_options: gcc/nvcc options
        input_tensors: Input tensors for shape/dtype info
        compile_func: Function to call for actual compilation
        relu_vr_data: ReLU VR data dict (affects compilation output)
        *args, **kwargs: Arguments passed to compile_func

    Returns:
        Compiled package dict
    """
    # Get full config path
    config_path = get_cache_path(cache_key, input_tensors, compile_options, build_options,
                                 relu_vr_data)

    # Check cache first
    if not is_force_rebuild() and is_cache_valid(cache_key, input_tensors,
                                                  compile_options, build_options,
                                                  relu_vr_data):
        record = load_cache(cache_key, config_path)
        if record and record.get("kernel_exists"):
            kernel_path = record.get("kernel_path", "")
            if kernel_path and Path(kernel_path).exists():
                logger.info(f"[Cache] HIT: Loading from {kernel_path}")
                return {
                    "model": record.get("model", "unknown"),
                    "kernel": kernel_path,
                    "config_path": record.get("config_path", ""),
                    "input_info": record.get("input_info", []),
                    "output_info": record.get("output_info", []),
                }
            logger.info(f"[Cache] STALE: kernel file missing, recompiling")

    # Need to compile
    _set_project_root(Path.cwd().resolve())

    # Execute compilation (Driver has already set build_dir to the cache directory)
    package = compile_func(*args, **kwargs)

    # Save cache record to config path (including full config for reference)
    record = {
        "model": package.get("model", "unknown"),
        "kernel_path": package.get("kernel", ""),
        "config_path": package.get("config_path", ""),
        "input_info": package.get("input_info", []),
        "output_info": package.get("output_info", []),
        "compile_options_hash": get_compile_options_hash(compile_options),
        "compile_options": compile_options or {},
        "build_options_hash": get_build_options_hash(build_options),
        "build_options": build_options or {},
        "input_tensors_hash": get_input_tensors_config_hash(input_tensors),
        "relu_vr_hash": get_relu_vr_hash(relu_vr_data) if relu_vr_data else "",
        "kernel_exists": True,
    }
    save_cache(cache_key, config_path, record)
    logger.info(f"[Cache] MISS: Compiled to {package.get('kernel', '')}")

    return package


# Backwards compatibility
generate_scope = generate_cache_key
generate_cache_key_with_options = generate_cache_key  # Deprecated