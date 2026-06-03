#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

import logging
from pathlib import Path
from typing import Callable, List, Any, Dict, Optional
import torch
from .registry import get_frontend, get_library_impl
from .builder import FHELibraryBuilder
from ..config.compile_options import CompileOptions


def _profile_relu_builtin(source, input_tensors) -> Optional[Dict[str, float]]:
    """Built-in ReLU VR profiling using FX Interpreter.

    Applies BN folding first (same as torch_frontend.prepare), then traces
    the fused model and runs input_tensors through FX Interpreter to collect
    per-call-site ReLU input activation ranges (pre-ReLU abs_max).

    The VR range must cover the full input interval [-max_abs, max_abs] for
    accurate polynomial approximation.  Tracking the pre-ReLU input (not the
    post-ReLU output) ensures the range covers negative values that ReLU
    clips to zero.

    Args:
        source: PyTorch nn.Module model.
        input_tensors: List of input tensors.

    Returns:
        Dict mapping AIR node names to VR float values, or None.
    """
    try:
        import torch.fx as fx
        from ..config.profiler import _vr_result_to_flat_dict
    except ImportError:
        logger.warning("torch.fx not available for built-in ReLU profiling")
        return None

    model = source
    if hasattr(model, '_original_model'):
        model = model._original_model
    model.eval()

    # Apply BN folding before profiling, same as torch_frontend.prepare
    try:
        from ..frontend.torch.passes.model_prepare import ModelPreparePass
        model_prepare = ModelPreparePass(inplace=False)
        model = model_prepare.apply(model)
        logger.info("Applied BN folding before built-in ReLU profiling")
    except ImportError:
        logger.warning("ModelPreparePass not available, profiling without BN folding")

    try:
        traced = fx.symbolic_trace(model)
    except Exception as e:
        logger.warning("FX trace failed for built-in ReLU profiling: %s", e)
        return None

    # Find ReLU nodes in FX graph
    relu_nodes = []
    for node in traced.graph.nodes:
        if node.op == 'call_module':
            submod = _get_module(traced, node.target)
            if isinstance(submod, torch.nn.ReLU):
                relu_nodes.append(node)
        elif node.op == 'call_function' and node.target is torch.relu:
            relu_nodes.append(node)

    if not relu_nodes:
        return None

    # Track abs_max per node
    abs_max_values = {}
    for node in relu_nodes:
        abs_max_values[f"{node.name}_Relu"] = 0.0

    # Run through FX Interpreter
    sample = input_tensors[0] if len(input_tensors) == 1 else input_tensors
    with torch.no_grad():
        _run_fx_relu_tracker(traced, sample, relu_nodes, abs_max_values)

    # Compute VR values
    margin = 1
    result = {}
    for node in relu_nodes:
        air_name = f"{node.name}_Relu"
        abs_max = abs_max_values[air_name]
        vr = int(abs_max) + margin if abs_max > 0 else 3
        result[air_name] = {"abs_max": abs_max, "vr": vr}

    return _vr_result_to_flat_dict({"per_node": result})


def _get_module(model, target: str):
    """Get a submodule by dot-separated target path."""
    atoms = target.split('.')
    mod = model
    for atom in atoms:
        if not hasattr(mod, atom):
            return None
        mod = getattr(mod, atom)
    return mod


def _run_fx_relu_tracker(traced, sample, relu_nodes, abs_max_values):
    """Run one sample through FX Interpreter, tracking ReLU input activations.

    Tracks the pre-ReLU input abs_max (not the output), because the VR range
    must cover the full input interval [-max_abs, max_abs] for polynomial
    approximation.  Tracking the output max(0, x) underestimates the range
    when the input has large negative values.
    """
    import torch.fx as fx

    relu_node_ids = set(id(n) for n in relu_nodes)

    class ReLUTracker(fx.Interpreter):
        def run_node(self, n):
            # Capture pre-ReLU input abs_max before the node executes
            if id(n) in relu_node_ids:
                air_name = f"{n.name}_Relu"
                input_val = None
                if n.args:
                    arg = n.args[0]
                    if isinstance(arg, torch.fx.Node):
                        input_val = self.env.get(arg)
                if isinstance(input_val, torch.Tensor):
                    val = float(input_val.abs().max())
                    if val > abs_max_values[air_name]:
                        abs_max_values[air_name] = val
            result = super().run_node(n)
            return result

    tracker = ReLUTracker(traced)
    if isinstance(sample, (list, tuple)):
        tracker.run(*sample)
    else:
        tracker.run(sample)

logger = logging.getLogger(__name__)



class Driver:
    """
    Unified compiler that delegates to registered backends.

    Supports multiple compilation strategies while maintaining a single interface.
    """

    def __init__(
            self,
            frontend: str,
            library: str,
            device: str = "cpu",
            options: Any = None,
            **kwargs
    ):
        """
        Initialize compiler with specified library.

        Args:
            frontend: Frontend name (torch/ast/onnx)
            library: Library name (antlib/phantom/acelib/seal)
            device: Device name (cpu/cuda)
            options: CompileOptions or ComputeOptions instance
            **kwargs: Library configuration (ckks/vec/sihe/p2c etc.)
        """
        self.func = None
        self.input_names = None
        self._options = options

        # Build library config from options
        self.library_config = {"device": device}
        if options is not None:
            # Use to_compiler_options() method from BaseOption
            compiler_options = options.to_compiler_options()
            self.library_config.update(compiler_options)

        # Frontends no longer accept constructor arguments
        self.frontend_impl = get_frontend(frontend)
        self.backend_impl = FHELibraryBuilder(library, **self.library_config)

    def compile(self, source, input_tensors, input_names=None):
        from ..cache import (
            compile_with_cache, generate_cache_key, get_cache_dir, _get_project_root,
            _set_project_root, get_input_tensors_hash, get_full_config_hash, get_cache_path
        )

        self.func = source
        self.input_names = input_names

        # Get parameters
        device = getattr(self.backend_impl, 'device', 'cpu')
        library = self.backend_impl.library
        frontend = self.frontend_impl.name() if hasattr(self.frontend_impl, 'name') else 'torch'

        # Generate Level 1 cache key (entity-frontend-library-device)
        cache_key = generate_cache_key(source, frontend, library, device)

        # Get build options from backend (for Level 3 hash)
        build_options = self._get_build_options()

        # Resolve relu_vr_data BEFORE cache lookup so different VR values
        # produce different cache keys (prevents stale cache hits)
        relu_vr_data = self._resolve_relu_vr_data(source=source, input_tensors=input_tensors)

        # Set build_dir to the full config path (Level 3) so all compilation outputs land there
        _set_project_root(Path.cwd().resolve())
        config_path = get_cache_path(cache_key, input_tensors, self.library_config, build_options,
                                     relu_vr_data)
        config_path.mkdir(parents=True, exist_ok=True)
        self.backend_impl.build_dir = config_path
        self.backend_impl.so_file = config_path / f"kernel.so"

        # Define actual compilation
        def do_compile():
            build_dir = getattr(self.backend_impl, 'build_dir', None)
            frontend_kwargs = {'library': library, 'device': device}
            if build_dir is not None:
                # Convert Path to str for frontend compatibility
                frontend_kwargs['build_dir'] = str(build_dir)
            # Pass pre-resolved relu_vr_data to frontend (for AIR IR embedding)
            if relu_vr_data is not None:
                frontend_kwargs['relu_vr_data'] = relu_vr_data
            self.ir = self.frontend_impl.to_ir(source, input_tensors, input_names, **frontend_kwargs)
            self.so_file = self.backend_impl.build(ir=self.ir)
            self._package = self._build_package(input_tensors)
            return self._package

        # Use cache wrapper with three-level cache
        self._package = compile_with_cache(
            cache_key=cache_key,
            compile_options=self.library_config,
            build_options=build_options,
            input_tensors=input_tensors,
            compile_func=do_compile,
            relu_vr_data=relu_vr_data,
        )
        return self._package

    def _get_build_options(self) -> dict:
        """Get build options from backend for cache key generation."""
        # Get optimization level and other build options from backend
        # Default to empty dict - can be extended to capture actual build flags
        return {
            "optimization_level": "O2",  # Default, can be extended
        }

    def _resolve_relu_vr_data(self, source=None, input_tensors=None) -> Optional[Dict[str, float]]:
        """Resolve ReLU VR data from CompileOptions.

        Priority:
        1. CompileOptions.relu_vr_data (explicit dict) — highest
        2. CompileOptions.relu_vr_file (JSON file path)
        3. CompileOptions.profile_relu (built-in profiling with input_tensors)
        4. None (skip embedding, fall back to CLI defaults)

        Args:
            source: The model/function being compiled (needed for profile_relu).
            input_tensors: Example input tensors (needed for profile_relu).

        Returns:
            Dict mapping AIR node names to VR float values, or None.
        """
        options = self._get_options()
        if not isinstance(options, CompileOptions):
            return None

        # Priority 1: explicit dict
        if options.relu_vr_data is not None:
            logger.info("Using explicit relu_vr_data (%d nodes)", len(options.relu_vr_data))
            return options.relu_vr_data

        # Priority 2: JSON file
        if options.relu_vr_file is not None:
            from ace.fhe.config.profiler import _load_vr_file
            logger.info("Loading relu_vr_data from file: %s", options.relu_vr_file)
            return _load_vr_file(options.relu_vr_file)

        # Priority 3: built-in profiling with example_inputs
        if options.profile_relu and source is not None and input_tensors is not None:
            logger.info("Profiling ReLU VR from example_inputs (built-in mode)")
            vr_data = _profile_relu_builtin(source, input_tensors)
            if vr_data:
                logger.info("Built-in profiling found %d ReLU nodes", len(vr_data))
                return vr_data
            logger.warning("Built-in profiling found no ReLU nodes")

        return None

    def _get_options(self):
        """Get the CompileOptions or ComputeOptions instance."""
        return getattr(self, '_options', None)

    def export(self, input_tensors, input_names=None, format="air", output_path="exported.ir", source=None):
        """
        Export frontend IR to file without full compilation.

        Delegates to the frontend's export method for consistency.

        Args:
            input_tensors: List of input tensors
            input_names: List of input names
            format: Output format - "air" or "onnx"
            output_path: Output file path
            source: Source model/function to export (optional, uses stored func if not provided)

        Returns:
            Path to exported file
        """
        # Use provided source or fall back to stored func
        source_to_export = source if source is not None else self.func
        self.input_names = input_names

        # Get device from backend_impl
        device = getattr(self.backend_impl, 'device', 'cpu')
        library = self.backend_impl.library

        # Delegate to frontend's export method for consistency
        return self.frontend_impl.export(
            source_to_export,
            input_tensors,
            input_names,
            format=format,
            output_path=output_path
        )


    def _build_package(self, input_tensors: List[torch.Tensor]) -> Dict[str, Any]:
        """Generate package metadata from input tensors and function signature."""
        if len(input_tensors) != len(self.input_names):
            raise ValueError(
                f"Expected {len(self.input_names)} input tensors, got {len(input_tensors)}"
            )

        input_info = []
        for name, tensor in zip(self.input_names, input_tensors):
            input_info.append({
                "name": name,
                "shape": list(tensor.shape),
                "type": f"tensor({self._get_tensor_dtype(tensor)})",
            })

        # Run the original function to infer output (if callable)
        if callable(self.func):
            output_tensors = self.func(*input_tensors)
            if not isinstance(output_tensors, (list, tuple)):
                output_tensors = [output_tensors]
        else:
            # For file-based frontends (ONNX), we don't have a callable
            # Output info will be inferred at runtime
            output_tensors = None

        output_info = []
        if output_tensors is not None:
            for i, out in enumerate(output_tensors):
                output_info.append({
                    "name": "output",
                    "shape": list(out.shape),
                    "type": f"tensor({self._get_tensor_dtype(out)})",
                })

        package = {
            "model": self.ir.entry_name,
            "kernel": str(self.so_file),
            "input_info": input_info,
            "output_info": output_info,
        }

        # Include config path if available (for ONNX-based compilation)
        if hasattr(self.ir, '_config_path'):
            package["config_path"] = self.ir._config_path

        logger.debug("Package built: model=%s, kernel=%s, input_info=%s, output_info=%s, config_path=%s",
                     package.get("model"), package.get("kernel"), package.get("input_info"),
                     package.get("output_info"), package.get("config_path"))

        return package

    def _get_tensor_dtype(self, tensor: torch.Tensor) -> str:
        """Map PyTorch dtype to string representation."""
        dtype_map = {
            torch.float32: "float",
            torch.float64: "double",
            torch.int32: "int32",
            torch.int64: "int64",
        }
        return dtype_map.get(tensor.dtype, "float")