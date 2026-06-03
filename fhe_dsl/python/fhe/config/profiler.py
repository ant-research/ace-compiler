#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
ReLUProfiler: ReLU Value Range profiling for FHE polynomial approximation.

Three usage modes:
1. Load pre-profiled VR from file:  profiler.load()
2. Pre-pipeline profiling:          profiler.profile(inputs=..., margin=1)
3. Built-in profiling during compile (via fhe.compile(..., profile_relu=True))

The profiler uses FX Interpreter-based profiling that produces per-call-site
VR values (e.g. 19 nodes for ResNet-20), matching AIR IR node names exactly.
"""
import json
import logging
import os
from pathlib import Path
from typing import Callable, Dict, List, Optional

import torch
import torch.nn as nn

logger = logging.getLogger(__name__)


class ReLUProfiler:
    """ReLU Value Range profiler for FHE polynomial approximation.

    Uses FX Interpreter to profile per-call-site ReLU activation ranges,
    producing VR values that match AIR IR node names exactly.

    Usage:
        # Mode 1: Load from file
        profiler = ReLUProfiler(model_spec)
        vr_data = profiler.load()

        # Mode 2: Profile with dataset or specified inputs
        vr_data = profiler.profile(images, margin=1, save=True)

        # Mode 3: Built-in profiling (during compile)
        #   Handled by fhe.compile(..., profile_relu=True)
        #   Uses example_inputs from ModelSpec
    """

    def __init__(self, model_spec):
        """Initialize profiler with a ModelSpec.

        Args:
            model_spec: A ModelSpec instance describing the model to profile.
        """
        from .spec import ModelSpec
        if not isinstance(model_spec, ModelSpec):
            raise TypeError(f"Expected ModelSpec, got {type(model_spec)}")
        self._spec = model_spec

    def load(self) -> Dict[str, float]:
        """Mode 1: Load pre-profiled VR from file.

        Uses model_spec.get_vr_profile() to find the profile JSON.

        Returns:
            Dict mapping AIR node name to VR float value,
            e.g. {"relu_Relu": 5.0, "layer1_0_relu_Relu": 4.0, ...}

        Raises:
            FileNotFoundError: If no VR profile file is found.
        """
        path = self._spec.get_vr_profile()
        if path is None:
            raise FileNotFoundError(
                f"No ReLU VR profile found for {self._spec.name}. "
                f"Set relu_vr_file or ensure weights_required=True with a profiles/ directory."
            )
        return _load_vr_file(path)

    def profile(
        self,
        inputs: Optional[torch.Tensor] = None,
        margin: int = 1,
        save: bool = False,
    ) -> Dict[str, float]:
        """Mode 2: Pre-pipeline profiling with dataset or specified inputs.

        Uses FX Interpreter to profile per-call-site ReLU activation ranges.
        Each ReLU call in the FX graph gets its own VR value, producing
        names that match AIR IR exactly (e.g. layer1_0_relu_Relu and
        layer1_0_relu_1_Relu for a shared ReLU module called twice).

        Args:
            inputs: Input tensor of shape (N, C, H, W) with normalization applied.
                If None, loads from model_spec.dataset (full dataset profiling).
            margin: Safety margin added to ceil(abs_max). Default 1.
            save: If True, save result to relu_vr_file (or auto-discovered path).

        Returns:
            Dict mapping AIR node name to VR float value,
            e.g. {"relu_Relu": 5.0, "layer1_0_relu_Relu": 4.0, ...}
        """
        if inputs is None:
            inputs = self._load_dataset()

        model = self._spec.create_model()
        model.eval()

        # Apply BN folding before profiling, same as torch_frontend.prepare
        try:
            from ..frontend.torch.passes.model_prepare import ModelPreparePass
            model_prepare = ModelPreparePass(inplace=False)
            model = model_prepare.apply(model)
            logger.info("Applied BN folding before ReLU profiling")
        except ImportError:
            logger.warning("ModelPreparePass not available, profiling without BN folding")

        result = profile_relu_vr_fx(model, inputs, margin=margin)

        if save:
            path = self._spec.get_vr_profile()
            if path is None:
                path = self._default_save_path()
            _save_vr_file(result, path)

        return _vr_result_to_flat_dict(result)

    def _load_dataset(self) -> torch.Tensor:
        """Load dataset images for profiling."""
        dataset = self._spec.dataset
        if dataset is None:
            raise ValueError(
                f"ModelSpec '{self._spec.name}' has no dataset specified. "
                f"Pass inputs= explicitly to profile()."
            )

        if dataset in ("cifar10", "cifar100"):
            mod_name = f"ace.model.{dataset}"
            import importlib
            mod = importlib.import_module(mod_name)
            load_fn = getattr(mod, f"load_{dataset}_images")
            images, _ = load_fn(10000)
            return images

        raise ValueError(f"Unknown dataset: {dataset}")

    def _default_save_path(self) -> str:
        """Compute default save path for VR profile."""
        module = self._spec.model_class.__module__
        if module:
            pkg = module.rsplit(".", 1)[0] if "." in module else module
            try:
                import importlib
                mod = importlib.import_module(pkg)
                pkg_dir = os.path.dirname(mod.__file__)
                return os.path.join(pkg_dir, "profiles", f"{self._spec.name}.json")
            except (ImportError, AttributeError):
                pass
        return f"{self._spec.name}_vr.json"


# ---------------------------------------------------------------------------
# FX Interpreter-based profiling (produces per-call-site VR values)
# ---------------------------------------------------------------------------

def profile_relu_vr_fx(
    model: nn.Module,
    inputs: torch.Tensor,
    margin: int = 1,
    relu_vr_def: int = 3,
) -> Dict:
    """Profile ReLU input activation ranges using FX Interpreter.

    Traces the model, then runs data through an FX Interpreter to collect
    per-call-site pre-ReLU abs_max values for every ReLU node.  Tracking
    the input (not the output) ensures the VR range covers the full input
    interval [-max_abs, max_abs], including negative values that ReLU clips
    to zero.  This produces VR values for each FX node (e.g. 19 nodes for
    ResNet-20 with shared ReLU), matching AIR IR node names exactly.

    Args:
        model: PyTorch nn.Module in eval mode.
        inputs: Input tensor of shape (N, C, H, W) with normalization applied.
        margin: Safety margin added to ceil(abs_max). Default 1.
        relu_vr_def: Default VR value for unlisted ReLU nodes. Default 3.

    Returns:
        Dict with keys:
        - "relu_vr_def": int, default VR value
        - "relu_vr": str, semicolon-delimited config
        - "per_node": dict mapping AIR name to {"abs_max": float, "vr": int}
    """
    try:
        import torch.fx as fx
    except ImportError:
        raise RuntimeError("torch.fx is required for FX-based ReLU profiling")

    model.eval()

    # FX trace the model
    try:
        traced = fx.symbolic_trace(model)
    except Exception as e:
        logger.warning("FX trace failed, falling back to hook-based profiling: %s", e)
        return _profile_relu_vr_hooks(model, inputs, margin=margin, relu_vr_def=relu_vr_def)

    # Find all ReLU call_module and call_function nodes
    relu_nodes = []
    for node in traced.graph.nodes:
        if node.op == 'call_module':
            module = _get_module(traced, node.target)
            if isinstance(module, nn.ReLU):
                relu_nodes.append(node)
        elif node.op == 'call_function' and node.target is torch.relu:
            relu_nodes.append(node)

    if not relu_nodes:
        logger.warning("No ReLU nodes found in FX graph")
        return {"relu_vr_def": relu_vr_def, "relu_vr": "", "per_node": {}}

    logger.info("Found %d ReLU nodes in FX graph", len(relu_nodes))

    # Track abs_max per FX node using AIR naming convention
    abs_max_values: Dict[str, float] = {}
    for node in relu_nodes:
        air_name = f"{node.name}_Relu"
        abs_max_values[air_name] = 0.0

    # Run FX Interpreter to collect activation ranges
    n_samples = inputs.shape[0]
    with torch.no_grad():
        for i in range(n_samples):
            _run_fx_with_relu_tracking(traced, inputs[i:i + 1], relu_nodes, abs_max_values)
            if (i + 1) % 50 == 0:
                logger.info("Processed %d/%d samples", i + 1, n_samples)

    # Compute VR values
    per_node = {}
    vr_entries = []
    for node in relu_nodes:
        air_name = f"{node.name}_Relu"
        abs_max = abs_max_values[air_name]
        vr = int(abs_max) + margin if abs_max > 0 else relu_vr_def
        per_node[air_name] = {"abs_max": abs_max, "vr": vr}
        vr_entries.append(f"{air_name}={vr}")

    relu_vr_str = ";".join(vr_entries)

    return {
        "relu_vr_def": relu_vr_def,
        "relu_vr": relu_vr_str,
        "per_node": per_node,
    }


def _run_fx_with_relu_tracking(
    traced: 'fx.GraphModule',
    sample: torch.Tensor,
    relu_nodes: list,
    abs_max_values: Dict[str, float],
):
    """Run one sample through FX Interpreter, tracking ReLU input activations.

    Tracks the pre-ReLU input abs_max (not the output), because the VR range
    must cover the full input interval [-max_abs, max_abs] for polynomial
    approximation.  Tracking the output max(0, x) underestimates the range
    when the input has large negative values.
    """
    import torch.fx as fx

    class ReLUTracker(fx.Interpreter):
        def __init__(self, module, relu_nodes, abs_max_values):
            super().__init__(module)
            self._relu_nodes = set(id(n) for n in relu_nodes)
            self._abs_max_values = abs_max_values

        def run_node(self, n):
            # Capture pre-ReLU input abs_max before the node executes
            if id(n) in self._relu_nodes:
                air_name = f"{n.name}_Relu"
                input_val = None
                if n.args:
                    arg = n.args[0]
                    if isinstance(arg, torch.fx.Node):
                        input_val = self.env.get(arg)
                if isinstance(input_val, torch.Tensor):
                    val = float(input_val.abs().max())
                    if val > self._abs_max_values[air_name]:
                        self._abs_max_values[air_name] = val
            result = super().run_node(n)
            return result

    tracker = ReLUTracker(traced, relu_nodes, abs_max_values)
    tracker.run(sample)


def _get_module(model: nn.Module, target: str) -> nn.Module:
    """Get a submodule by dot-separated target path."""
    atoms = target.split('.')
    mod = model
    for atom in atoms:
        if not hasattr(mod, atom):
            return None
        mod = getattr(mod, atom)
    return mod


# ---------------------------------------------------------------------------
# Hook-based profiling (fallback when FX trace fails)
# ---------------------------------------------------------------------------

def _default_relu_name_fn(module_name: str) -> str:
    """Map torch module path to AIR node name (hook-based fallback)."""
    fx_name = module_name.replace(".", "_")
    return f"{fx_name}_Relu"


def _profile_relu_vr_hooks(
    model: nn.Module,
    images: torch.Tensor,
    margin: int = 1,
    relu_vr_def: int = 3,
) -> Dict:
    """Profile ReLU input activation ranges using forward hooks (fallback).

    Tracks pre-ReLU input abs_max (not the output) to ensure the VR range
    covers the full input interval [-max_abs, max_abs].  This produces
    per-module VR values (e.g. 10 for ResNet-20) rather than per-call-site
    (19). Used when FX trace is not available.
    """
    name_fn = _default_relu_name_fn
    abs_max_values: Dict[str, float] = {}
    relu_modules: List[tuple] = []

    for mod_name, module in model.named_modules():
        if isinstance(module, nn.ReLU):
            air_name = name_fn(mod_name)
            relu_modules.append((air_name, module))
            abs_max_values[air_name] = 0.0

    if not relu_modules:
        logger.warning("No ReLU modules found in model")
        return {"relu_vr_def": relu_vr_def, "relu_vr": "", "per_node": {}}

    logger.info("Found %d ReLU modules (hook-based profiling)", len(relu_modules))

    def make_hook(air_name: str):
        def hook(module, input, output):
            # Track pre-ReLU input abs_max, not output
            if isinstance(input, tuple) and len(input) > 0:
                inp = input[0]
                if isinstance(inp, torch.Tensor):
                    val = float(inp.abs().max())
                    if val > abs_max_values[air_name]:
                        abs_max_values[air_name] = val
        return hook

    handles = []
    for air_name, module in relu_modules:
        handles.append(module.register_forward_hook(make_hook(air_name)))

    try:
        model.eval()
        n_samples = images.shape[0]
        with torch.no_grad():
            for i in range(n_samples):
                model(images[i:i + 1])
                if (i + 1) % 50 == 0:
                    logger.info("Processed %d/%d samples", i + 1, n_samples)
    finally:
        for h in handles:
            h.remove()

    per_node = {}
    vr_entries = []
    for air_name, _ in relu_modules:
        abs_max = abs_max_values[air_name]
        vr = int(abs_max) + margin if abs_max > 0 else relu_vr_def
        per_node[air_name] = {"abs_max": abs_max, "vr": vr}
        vr_entries.append(f"{air_name}={vr}")

    relu_vr_str = ";".join(vr_entries)

    return {
        "relu_vr_def": relu_vr_def,
        "relu_vr": relu_vr_str,
        "per_node": per_node,
    }


# ---------------------------------------------------------------------------
# File I/O utilities
# ---------------------------------------------------------------------------

def _vr_result_to_flat_dict(result: Dict) -> Dict[str, float]:
    """Convert profile result to flat dict for AIR IR embedding.

    FX-based profiling already uses AIR-style names (e.g. "relu_Relu"),
    so no name mapping is needed.
    """
    flat = {}
    for name, info in result.get("per_node", {}).items():
        flat[name] = float(info["vr"])
    return flat


def _load_vr_file(path: str) -> Dict[str, float]:
    """Load VR profile from JSON file and return flat dict."""
    with open(path) as f:
        result = json.load(f)
    return _vr_result_to_flat_dict(result)


def _save_vr_file(result: Dict, path: str) -> None:
    """Save profile result to JSON file."""
    path = Path(path)
    path.parent.mkdir(parents=True, exist_ok=True)
    with open(path, "w") as f:
        json.dump(result, f, indent=2)
    logger.info("Saved VR profile to %s (%d nodes)", path, len(result.get("per_node", {})))