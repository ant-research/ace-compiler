#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
ModelSpec and FuncSpec: Unified FHE compilation target descriptors.

ModelSpec describes an nn.Module model for FHE compilation.
FuncSpec describes a Python function for FHE compilation.

These replace the older ModelTestCase/FunctionTestCase (test_cases) and
CompileSpec (fhe.spec) systems with a single, simpler class per domain.
"""
import os
import logging
from dataclasses import dataclass, field
from typing import Any, Callable, Dict, List, Optional, Tuple

import torch
import torch.nn as nn

logger = logging.getLogger(__name__)


@dataclass
class ModelSpec:
    """FHE compilation target: nn.Module model.

    Attributes:
        name: Unique identifier (e.g. "resnet20_cifar10", "conv2d_relu_op").
        model_class: PyTorch nn.Module subclass.
        example_inputs: Tuple of example input tensors for tracing.
        encrypt_inputs: List of input names to encrypt (e.g. ["x"]).
        model_init_args: Positional args for model_class constructor.
        model_init_kwargs: Keyword args for model_class constructor.
        model_post_init: Optional callback(model) for custom init (e.g. load weights).
        compile_options: Optional dict of compiler options (vec, ckks, sihe, p2c, ...).
        expected_ops: Optional list of expected IR operator types.
        weights_required: Whether pretrained weights are required.
        dataset: Dataset name for profiling/validation (e.g. "cifar10", "cifar100").
        relu_vr_file: Explicit path to ReLU VR profile JSON. Auto-discovered if None
            and weights_required=True (looks in profiles/{name}.json).
    """

    name: str
    model_class: type
    example_inputs: Tuple[torch.Tensor, ...]
    encrypt_inputs: List[str]
    model_init_args: tuple = ()
    model_init_kwargs: dict = field(default_factory=dict)
    model_post_init: Optional[Callable[[nn.Module], None]] = None
    compile_options: Optional[Dict[str, Any]] = None
    expected_ops: Optional[List[str]] = None
    weights_required: bool = False
    dataset: Optional[str] = None
    relu_vr_file: Optional[str] = None

    def create_model(self) -> nn.Module:
        """Instantiate and return the model.

        Creates model_class(*model_init_args, **model_init_kwargs), then calls
        model_post_init(model) if set. Sets _fhe_name for cache key generation.
        """
        model = self.model_class(*self.model_init_args, **self.model_init_kwargs)
        if self.model_post_init is not None:
            self.model_post_init(model)
        model._fhe_name = self.name
        return model

    def get_vr_profile(self) -> Optional[str]:
        """Get the VR (value range) profile file path for ReLU approximation.

        Returns self.relu_vr_file if set, otherwise auto-discovers
        profiles/{name}.json relative to the model package directory
        if weights_required is True.

        Returns:
            Path to VR profile JSON, or None if not found and weights_required=False.

        Raises:
            FileNotFoundError: If weights_required=True but no profile found.
        """
        if self.relu_vr_file is not None:
            path = self.relu_vr_file
            if os.path.exists(path):
                return path
            raise FileNotFoundError(f"Explicit relu_vr_file not found: {path}")

        if not self.weights_required:
            return None

        # Auto-discover: look for profiles/{name}.json relative to model_class's package
        module = self.model_class.__module__
        if module:
            pkg = module.rsplit(".", 1)[0] if "." in module else module
            try:
                import importlib
                mod = importlib.import_module(pkg)
                pkg_dir = os.path.dirname(mod.__file__)
            except (ImportError, AttributeError):
                pkg_dir = None

            if pkg_dir:
                candidate = os.path.join(pkg_dir, "profiles", f"{self.name}.json")
                if os.path.exists(candidate):
                    return candidate

        raise FileNotFoundError(
            f"VR profile not found for {self.name}. "
            f"Generate it: python -m ace.model.relu_profile --model {self.name}"
        )


@dataclass
class FuncSpec:
    """FHE compilation target: Python function.

    Attributes:
        name: Unique identifier (e.g. "add_func", "relu_func").
        func: Python function to compile.
        example_inputs: Tuple of example input tensors for tracing.
        encrypt_inputs: List of input names to encrypt (e.g. ["x"] or ["x", "y"]).
        compile_options: Optional dict of compiler options.
        expected_ops: Optional list of expected IR operator types.
    """

    name: str
    func: Callable
    example_inputs: Tuple[torch.Tensor, ...]
    encrypt_inputs: List[str] = field(default_factory=lambda: ["x"])
    compile_options: Optional[Dict[str, Any]] = None
    expected_ops: Optional[List[str]] = None