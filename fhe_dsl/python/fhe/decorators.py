#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
FHE decorators for compiling and executing PyTorch models/functions.

Three public decorators are provided:
- @compile: Compile a function/model to FHE, returns a program object
- @compute: Compile and immediately execute in FHE
- @export: Export frontend IR to file (ONNX/AIR) without full compilation
"""

import functools
import inspect
from collections.abc import Callable
from typing import Any, Union

import torch
import torch.nn as nn

from .config import CompileOptions, ComputeOptions
from .driver import (
    Driver,
    _raise_library_error,
    is_library_supported,
    list_devices_for_library,
    list_frontends,
    list_libraries,
)
from .runtime import CompiledProgram, FHERuntime
from .util import setup_fhe_logger


# ======================
# Module-level constants
# ======================

_COMPILER_KWARGS = set(CompileOptions.__dataclass_fields__.keys())
_COMPUTE_KWARGS = set(ComputeOptions.__dataclass_fields__.keys())

# Safety check: ComputeOptions must include all CompileOptions fields
assert _COMPILER_KWARGS.issubset(_COMPUTE_KWARGS), "ComputeOptions must be a superset of CompileOptions"

# ======================
# Internal helper functions
# ======================


def _validate_frontend_library(frontend: str, library: str):
    """Validate that frontend and library are available (sourced from the live registry)."""
    available_frontends = list_frontends()
    if frontend not in available_frontends:
        raise ValueError(f"Unknown frontend '{frontend}'. Available frontends: {available_frontends}")

    available_libraries = list_libraries()
    if library not in available_libraries:
        raise ValueError(f"Unknown library '{library}'. Available libraries: {available_libraries}")


def _resolve_device(library: str, device: str | None) -> str:
    """
    Resolve the target device for a library.

    ``device`` is optional: when ``None`` it is derived from the library's
    registered devices. Most libraries register exactly one device (e.g.
    ``phantom -> cuda``, ``ant -> cpu``), so it can be auto-selected. When the
    user supplies ``device`` explicitly it is only validated against the
    library's supported devices.

    Args:
        library: Registered library name.
        device: Device string or ``None`` for auto-derive.

    Returns:
        The resolved (lowercased) device string.

    Raises:
        ValueError: If the (library, device) combo is not supported, or if the
            library supports multiple devices and none was specified.
    """
    if device is None:
        supported = list_devices_for_library(library)
        if len(supported) == 1:
            return supported[0]
        if not supported:
            raise ValueError(f"Library '{library}' has no registered devices.")
        # Ambiguous: ask the user to disambiguate explicitly.
        raise ValueError(f"Library '{library}' supports multiple devices {supported}; specify device= explicitly.")

    device = device.lower()
    if not is_library_supported(library, device):
        _raise_library_error(library, device)
    return device


# Map export format → default output file extension.
_FORMAT_EXT = {"air": "B", "onnx": "onnx"}


def _default_output_path(format: str) -> str:
    """Generate a default output path from the export format (e.g. air → exported.B)."""
    ext = _FORMAT_EXT.get(format, format)
    return f"exported.{ext}"


def _validate_kwargs(kwargs: dict, allowed_keys: set, decorator_name: str):
    """Validate that kwargs only contains allowed keys."""
    unknown = set(kwargs.keys()) - allowed_keys
    if unknown:
        raise TypeError(f"{decorator_name}() got unexpected keyword arguments: {sorted(unknown)}")


def _resolve_encrypted_inputs(encrypt_inputs: list[str] | list[int] | None, param_names: list[str]) -> list[str]:
    """
    Resolve encrypted input specification to a list of parameter names.

    Args:
        encrypt_inputs: Either None, list of names, or list of indices.
        param_names: Ordered list of function parameter names.

    Returns:
        list of parameter names to encrypt.
    """
    if encrypt_inputs is None:
        return param_names[:]

    if not encrypt_inputs:
        return []

    if isinstance(encrypt_inputs[0], int):
        try:
            return [param_names[i] for i in encrypt_inputs]
        except IndexError as e:
            raise ValueError(
                f"Parameter index out of range. Function has {len(param_names)} parameters, "
                f"but got index {max(encrypt_inputs)}."
            ) from e

    for name in encrypt_inputs:
        if name not in param_names:
            raise ValueError(f"Parameter '{name}' not found in function signature.")

    return list(encrypt_inputs)


def _get_param_names(target: Callable | nn.Module) -> list[str]:
    """Get parameter names from a function or model."""
    if isinstance(target, nn.Module):
        sig = inspect.signature(target.forward)
        params = list(sig.parameters.keys())
        return params[1:] if params and params[0] == "self" else params
    else:
        sig = inspect.signature(target)
        return list(sig.parameters.keys())


def _create_wrapped_function(func: Callable, model: nn.Module = None):
    """Create a wrapped function for compilation."""
    if model is not None:
        model.eval()

        @functools.wraps(model.forward)
        def wrapped_func(*args, **kwargs):
            with torch.no_grad():
                return model(*args, **kwargs)

        wrapped_func._original_model = model
        return wrapped_func

    if callable(func) and not isinstance(func, nn.Module):
        func_sig = inspect.signature(func)
        param_names = list(func_sig.parameters.keys())
        num_params = len(param_names)

        if num_params == 1:

            def forward_method(self, x):
                return func(x)
        elif num_params == 2:

            def forward_method(self, x, y):
                return func(x, y)
        elif num_params == 3:

            def forward_method(self, x, y, z):
                return func(x, y, z)
        elif num_params == 4:

            def forward_method(self, a, b, c, d):
                return func(a, b, c, d)
        else:

            def forward_method(self, *args):
                return func(*args)

        wrapper_class_name = func.__name__.title().replace("_", "")
        FunctionWrapper = type(
            wrapper_class_name,
            (nn.Module,),
            {
                "__init__": lambda self, f: setattr(self, "_func", f) or super(FunctionWrapper, self).__init__(),
                "forward": forward_method,
            },
        )

        wrapper = FunctionWrapper(func)
        wrapper.eval()

        @functools.wraps(func)
        def wrapped_func(*args, **kwargs):
            with torch.no_grad():
                return wrapper(*args)

        wrapped_func._original_model = wrapper
        return wrapped_func

    return func


def _process_target(target, options):
    """
    Process target (function/model) and return common metadata.

    Args:
        target: Function, model class, or model instance
        options: CompileOptions or ComputeOptions

    Returns:
        tuple of (wrapped_func, model, param_names, encrypt_inputs)
    """
    if isinstance(target, type) and issubclass(target, nn.Module):
        model = target()
    elif isinstance(target, nn.Module):
        model = target
    else:
        model = None

    wrapped_func = _create_wrapped_function(target if model is None else None, model)
    param_names = _get_param_names(model if model else target)
    encrypt_inputs = _resolve_encrypted_inputs(options.encrypt_inputs, param_names)

    return wrapped_func, model, param_names, encrypt_inputs


def _attach_metadata(target, compiler, options, param_names, encrypt_inputs, extra=None):
    """
    Attach common metadata to target.

    Args:
        target: Target to attach metadata to
        compiler: FHE compiler/driver
        options: CompileOptions or ComputeOptions
        param_names: list of parameter names
        encrypt_inputs: list of encrypted input names
        extra: Optional additional attributes to attach
    """
    target._fhe_compiler = compiler
    target._fhe_options = options
    target._fhe_param_names = param_names
    target._fhe_encrypt_inputs = encrypt_inputs

    if extra:
        for k, v in extra.items():
            setattr(target, k, v)


# ======================
# Public decorators
# ======================


def compile(
    frontend: str = "torch", library: str = "ant", device: str | None = None, **kwargs
) -> Callable[[Callable | type[nn.Module] | nn.Module], Any]:
    """
    FHE compilation decorator - compiles a function/model to an FHE program.

    Args:
        frontend: Frontend strategy name (default: "torch")
        library: Library name (default: "ant")
        device: Device name (default: None — auto-derived from ``library``;
            only validated when given explicitly)
        **kwargs: Configuration options (see CompileOptions).

    Returns:
        Decorated target with `.compile(inputs)` method attached.

    Example:
        @compile(encrypt_inputs=["x"])
        def square(x):
            return x * x

        prog = square.compile([input_tensor])
    """
    setup_fhe_logger()
    _validate_frontend_library(frontend, library)
    device = _resolve_device(library, device)
    _validate_kwargs(kwargs, _COMPILER_KWARGS, "compile")

    options = CompileOptions(**kwargs)

    def decorator(target: Callable | type[nn.Module] | nn.Module) -> Any:
        wrapped_func, model, param_names, encrypt_inputs = _process_target(target, options)

        compiler = Driver(frontend=frontend, library=library, device=device, options=options)

        def compile_method(example_inputs):
            return compiler.compile(wrapped_func, example_inputs, input_names=encrypt_inputs)

        def compile_public(self_or_inputs, example_inputs=None):
            """Compile the decorated function/model with example inputs."""
            # Handle both class instance (self_or_inputs is self) and direct call
            inputs = self_or_inputs if example_inputs is None else example_inputs

            package = compile_method(inputs)
            return CompiledProgram(package, func=target, example_inputs=inputs, model=model)

        # Attach metadata and compile method
        _attach_metadata(
            target,
            compiler,
            options,
            param_names,
            encrypt_inputs,
            {
                "fhe_compile": compile_public,
            },
        )

        # For functions (non-Module), attach compile method directly
        if not isinstance(target, nn.Module):
            target.compile = compile_public

        # For nn.Module classes, attach both compile and fhe_compile
        if isinstance(target, type) and issubclass(target, nn.Module):
            target.fhe_compile = compile_public
            target.compile = compile_public

        # For nn.Module instances, only attach fhe_compile to avoid overriding Module.compile
        if isinstance(target, nn.Module) and not (isinstance(target, type) and issubclass(target, nn.Module)):
            target.fhe_compile = compile_public

        return target

    return decorator


def compute(frontend: str = "torch", library: str = "ant", device: str | None = None, **kwargs) -> Callable:
    """
    FHE compilation and immediately execute decorator.

    The decorated function behaves like the original, but runs encrypted:
    - Inputs are automatically encrypted
    - Computation is performed homomorphically
    - Output is decrypted; by default also validated against a plaintext run
      (``validate=True``). For batch/production workloads where the plaintext
      is unavailable or a double run is too costly, prefer ``@fhe.compile`` or
      pass ``validate=False``.

    Args:
        frontend: Frontend strategy name (default: "torch")
        library: Library name (default: "ant")
        device: Device name (default: None — auto-derived from ``library``;
            only validated when given explicitly)
        **kwargs: Configuration options (see ComputeOptions).

    Returns:
        A wrapped function that runs in FHE.

    Example:
        @compute(encrypt_inputs=[0])
        def add(a, b):
            return a + b

        result = add(input0, input1)  # Runs in FHE, returns plaintext result
    """
    setup_fhe_logger()
    _validate_frontend_library(frontend, library)
    device = _resolve_device(library, device)
    _validate_kwargs(kwargs, _COMPUTE_KWARGS, "compute")

    options = ComputeOptions(**kwargs)

    def decorator(target: Callable | type[nn.Module] | nn.Module) -> Callable:
        wrapped_func, _model, param_names, encrypt_inputs = _process_target(target, options)

        compiler = Driver(frontend=frontend, library=library, device=device, options=options)

        _compiled_cache = {}

        @functools.wraps(wrapped_func)
        def wrapper(*args, **kwds):
            inputset = list(args)
            cache_key = tuple(t.shape for t in inputset)

            if cache_key not in _compiled_cache:
                package = compiler.compile(wrapped_func, inputset, input_names=encrypt_inputs)
                runner = FHERuntime(package)
                _compiled_cache[cache_key] = runner
            else:
                runner = _compiled_cache[cache_key]

            result = runner.run(*inputset)

            if options.validate:
                with torch.no_grad():
                    expected = wrapped_func(*inputset)
                is_valid = runner.validate(result, expected)
                if is_valid:
                    print("Validation: PASSED")
                else:
                    raise RuntimeError("Result validation failed")

            return result

        _attach_metadata(wrapper, compiler, options, param_names, encrypt_inputs)
        return wrapper

    return decorator


def export(
    frontend: str = "torch",
    library: str = "ant",
    device: str | None = None,
    format: str = "air",
    output_path: str | None = None,
    **kwargs,
) -> Callable[[Callable | type[nn.Module] | nn.Module], Any]:
    """
    FHE export decorator - exports frontend IR to file without full compilation.

    Args:
        frontend: Frontend strategy name (default: "torch")
        library: Library name (default: "ant")
        device: Device name (default: None — auto-derived from ``library``;
            only validated when given explicitly)
        format: Output format - "air" for .B file, "onnx" for .onnx file
        output_path: Output file path (default: None — auto-derived from
            ``format`` as "exported.B" / "exported.onnx")
        **kwargs: Configuration options (see CompileOptions).

    Returns:
        Decorated target with `.export(inputs, output_path, format)` method attached.

    Example:
        @export(frontend="torch", format="air")
        def model(x):
            return x * x

        result = model.export([input_tensor])
    """
    setup_fhe_logger()
    _validate_frontend_library(frontend, library)
    device = _resolve_device(library, device)
    _validate_kwargs(kwargs, _COMPILER_KWARGS, "export")

    options = CompileOptions(**kwargs)

    def decorator(target: Callable | type[nn.Module] | nn.Module) -> Any:
        wrapped_func, model, param_names, encrypt_inputs = _process_target(target, options)

        compiler = Driver(frontend=frontend, library=library, device=device, options=options)

        # Use model instance for nn.Module, otherwise use wrapped_func or target
        if model is not None:
            export_target = model
        elif wrapped_func is not None:
            export_target = wrapped_func
        else:
            export_target = target

        def export_method(example_inputs, output_path=output_path, format=format):
            """Export the frontend IR to file."""
            if output_path is None:
                output_path = _default_output_path(format)
            return compiler.export(
                example_inputs, input_names=encrypt_inputs, format=format, output_path=output_path, source=export_target
            )

        # For functions, attach export to wrapped_func and return it
        if callable(target) and not isinstance(target, nn.Module):
            wrapped_func.export = export_method
            _attach_metadata(wrapped_func, compiler, options, param_names, encrypt_inputs)
            return wrapped_func

        # For model classes/instances, attach to target
        _attach_metadata(
            target,
            compiler,
            options,
            param_names,
            encrypt_inputs,
            {
                "export": export_method,
            },
        )

        if isinstance(target, type) and issubclass(target, nn.Module):
            target._fhe_export = export_method
            target.export = export_method

        return target

    return decorator
