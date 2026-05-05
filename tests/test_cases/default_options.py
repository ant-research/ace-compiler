# test_cases/default_options.py
"""
Default compiler options for test cases.

This module provides:
1. DEFAULT_COMPILE_OPTIONS: Default options for all tests
2. MODEL_SPECIFIC_OPTIONS: Per-model options override
3. Environment variable support for engineer tuning

Environment Variables:
    ACE_COMPILE_OPTIONS: JSON string to override compile options
        Example: ACE_COMPILE_OPTIONS='{"vec": {"ms": 256}, "ckks": {"N": 4096}}'

Usage in test cases:
    from test_cases.default_options import get_compile_options

    options = get_compile_options(model_name)
    fhe.compute(..., **options)
"""

import os
import json
from typing import Dict, Any, Optional


# Default compiler options for all tests
# These are applied when no model-specific options are defined
DEFAULT_COMPILE_OPTIONS: Dict[str, Any] = {
    # Example default options (uncomment to enable):
    # "vec": {"ms": 256},
    # "ckks": {"N": 8192, "hw": 192},
}


# Model-specific compiler options
# Key: model name (matches ModelTestCase.name)
# Value: dict of compiler options
MODEL_SPECIFIC_OPTIONS: Dict[str, Dict[str, Any]] = {
    # GEMM models
    "gemm_49x3": {
        "vec": {"ms": 128},
        "ckks": {"N": 256},
    },
    # Conv2d models need smaller N for faster testing
    "conv2d": {
        "vec": {"ms": 256},
        "ckks": {"N": 4096},
    },
    # conv2d_relu: skipped due to runtime ReLU issue
    # "conv2d_relu": {
    #     "vec": {"ms": 2048},
    #     "ckks": {"N": 2048},
    # },
    # AvgPool + Conv2d
    # avg_pool_conv2d: skipped due to runtime accuracy issue (mul_depth=8)
    # "avg_pool_conv2d": {
    #     "vec": {"ms": 1024},
    #     "ckks": {"q0": 40, "sf": 37, "N": 2048},
    # },
    # conv2d_bn_relu: skipped due to runtime ReLU issue
    # "conv2d_bn_relu": {
    #     "vec": {"ms": 256},
    #     "ckks": {"N": 4096},
    # },
    # Add more model-specific options as needed
    # "resnet20_cifar10": {
    #     "vec": {"ms": 256},
    #     "ckks": {"N": 8192, "hw": 192},
    # },
}


def get_compile_options(
    model_name: str,
    case_options: Optional[Dict[str, Any]] = None
) -> Dict[str, Any]:
    """
    Get compile options for a model, with priority:
    1. Environment variable ACE_COMPILE_OPTIONS (highest priority)
    2. ModelTestCase.compile_options (case_options)
    3. MODEL_SPECIFIC_OPTIONS[model_name]
    4. DEFAULT_COMPILE_OPTIONS (lowest priority)

    Args:
        model_name: Name of the model (ModelTestCase.name)
        case_options: Options from ModelTestCase.compile_options

    Returns:
        Dict of compile options ready for fhe.compile/compute
    """
    # Start with defaults
    options = DEFAULT_COMPILE_OPTIONS.copy()

    # Apply model-specific options
    if model_name in MODEL_SPECIFIC_OPTIONS:
        options = _merge_options(options, MODEL_SPECIFIC_OPTIONS[model_name])

    # Apply case-specific options
    if case_options:
        options = _merge_options(options, case_options)

    # Apply environment variable override (highest priority)
    env_options = _get_env_options()
    if env_options:
        options = _merge_options(options, env_options)

    return options


def _merge_options(base: Dict[str, Any], override: Dict[str, Any]) -> Dict[str, Any]:
    """Merge override options into base options (deep merge for nested dicts)."""
    result = base.copy()
    for key, value in override.items():
        if key in result and isinstance(result[key], dict) and isinstance(value, dict):
            result[key] = {**result[key], **value}
        else:
            result[key] = value
    return result


def _get_env_options() -> Optional[Dict[str, Any]]:
    """Get compile options from environment variable."""
    env_str = os.environ.get("ACE_COMPILE_OPTIONS", "")
    if not env_str:
        return None

    try:
        return json.loads(env_str)
    except json.JSONDecodeError as e:
        print(f"Warning: Failed to parse ACE_COMPILE_OPTIONS: {e}")
        return None


def set_env_options(options: Dict[str, Any]) -> None:
    """
    Set compile options via environment variable.
    Useful for programmatic control in tests or scripts.

    Args:
        options: Dict of compile options

    Example:
        set_env_options({"vec": {"ms": 256}, "ckks": {"N": 4096}})
    """
    os.environ["ACE_COMPILE_OPTIONS"] = json.dumps(options)


def clear_env_options() -> None:
    """Clear environment variable compile options."""
    os.environ.pop("ACE_COMPILE_OPTIONS", None)