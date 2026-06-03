#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Torch Frontend - Traces PyTorch models and generates AIR IR.

Pipeline:
1. prepare()  - FX trace PyTorch model → TorchTracedModel
2. compile()  - Execute traced model → generates AIR IR
3. export()   - Export AIR IR to .B file
"""

import logging
import torch
import torch.nn as nn
from typing import Dict, List, Optional
from pathlib import Path

# Setup logging for this module
from ...util import setup_logging
setup_logging()

# Get logger
logger = logging.getLogger(__name__)

# Check if torch.fx is available
try:
    import torch.fx as fx
    HAS_TORCH_FX = True
except ImportError:
    HAS_TORCH_FX = False

# Import CustomTracer for enhanced tracing with metadata
try:
    from ...ir.frontends.torch import CustomTracer, trace_with_metadata, STANDARD_OP_MAPPING
except ImportError:
    CustomTracer = None
    trace_with_metadata = None
    STANDARD_OP_MAPPING = {}

from ...driver import Frontend
from ...ir import TorchTracedModel, export_model_to_onnx, IRBuilder

# Import passes
from .passes import (
    ModelPreparePass,
    GraphTransformPass,
    ConstantExtractionPass,
)


class Torch(Frontend):
    """
    Torch Frontend for tracing PyTorch models via FX.

    Pipeline:
    1. prepare()  - PyTorch → FX Trace → TorchTracedModel
    2. compile()  - Execute traced model → generates AIR IR (via C++ kernels)
    3. export()   - Export AIR IR to .B file

    Note: Does NOT go through ONNX - use torch-via-onnx for ONNX path.
    """

    @classmethod
    def name(cls) -> str:
        return "torch"

    def prepare(
        self,
        model: nn.Module,
        inputs: List[torch.Tensor],
        input_names: Optional[List[str]] = None,
        build_dir: Optional[str] = None,
        library: str = "antlib",
        device: str = "cpu",
        relu_vr_data: Optional[Dict[str, float]] = None
    ) -> TorchTracedModel:
        """FX trace PyTorch model.

        Pipeline:
        1. Prepare model for FHE (fuse BatchNorm, eval mode)
        2. FX trace with CustomTracer
        3. Rewrite graph to use custom ops
        4. Extract constants

        Args:
            model: PyTorch model to trace (or wrapped function with _original_model)
            inputs: List of input tensors for shape derivation
            input_names: Optional list of input names
            library: Library name (antlib/phantom/acelib)
            device: Device name (cpu/cuda)
            relu_vr_data: Optional dict mapping AIR node names to ReLU VR values
                e.g. {"relu_Relu": 4.0, "layer1_0_relu_Relu": 5.0}

        Returns:
            TorchTracedModel wrapper
        """
        if not HAS_TORCH_FX:
            raise RuntimeError("torch.fx is not available (requires PyTorch 1.8+)")

        if not IRBuilder.is_available():
            raise RuntimeError("C++ extension not available. Cannot generate AIR IR.")

        # If model is a wrapped function, get the original model
        if hasattr(model, '_original_model'):
            model = model._original_model

        # Generate input names if not provided
        if input_names is None:
            input_names = [f"input_{i}" for i in range(len(inputs))]

        # Get input shapes
        input_shapes = [list(inp.shape) for inp in inputs]

        # Pass 1: Prepare model for FHE (fuse BatchNorm layers)
        # Only apply to nn.Module, skip for plain functions
        if isinstance(model, nn.Module):
            logger.info("Fusing BatchNorm layers...")
            model_prepare = ModelPreparePass(inplace=False)
            model = model_prepare.apply(model)

        # Pass 2: FX trace using CustomTracer
        logger.info("Tracing model with %d inputs using CustomTracer...", len(inputs))
        if CustomTracer is not None:
            tracer = CustomTracer(library=library, device=device)
            traced_model = fx.GraphModule(model, tracer.trace(model))
        else:
            traced_model = fx.symbolic_trace(model)

        # Pass 3: Rewrite graph to use custom ops
        graph_transform = GraphTransformPass()
        traced_model = graph_transform.apply(traced_model)

        # Pass 4: Extract constants from get_attr nodes
        constant_extraction = ConstantExtractionPass()
        constants = constant_extraction.apply(traced_model, model)
        if constants:
            logger.info("Found %d constants: %s", len(constants), list(constants.keys()))

        # Store weight and bias tensors in traced model for gemm/conv ops
        for const_name, const_info in constants.items():
            if not hasattr(traced_model, const_name):
                setattr(traced_model, const_name, const_info['tensor'])

        # Run once to get output shape
        with torch.no_grad():
            output = model(*inputs)
            if isinstance(output, (tuple, list)):
                output_shape = list(output[0].shape)
            else:
                output_shape = list(output.shape)

        logger.info("Traced model successfully")
        logger.info("Output shape: %s", output_shape)

        # DEBUG: print output_shape before returning
        print("=== DEBUG: output_shape in prepare() ===")
        print("output_shape:", output_shape)

        return TorchTracedModel(traced_model, input_names, input_shapes, output_shape, constants, relu_vr_data=relu_vr_data)

    def compile(
        self,
        model: nn.Module,
        inputs: List[torch.Tensor],
        input_names: Optional[List[str]] = None,
        build_dir: Optional[str] = None,
        library: str = "antlib",
        device: str = "cpu",
        execution_mode: str = "interpreter",
        relu_vr_data: Optional[Dict[str, float]] = None
    ) -> TorchTracedModel:
        """FX trace, execute, and export to .B file.

        Args:
            model: PyTorch model to trace
            inputs: List of input tensors for shape derivation
            input_names: Optional list of input names
            build_dir: Optional directory for intermediate files
            library: Library name (antlib/phantom/acelib)
            device: Device name (cpu/cuda)
            execution_mode: "interpreter" (default) or "direct"
                - "interpreter": Node-by-node via IRBuilder.add_op() (stable)
                - "direct": C++ custom ops generate AIR IR (experimental)
            relu_vr_data: Optional dict mapping AIR node names to ReLU VR values

        Returns:
            TorchTracedModel with AIR IR exported to .B file (format_type="file")
        """
        traced = self.prepare(model, inputs, input_names, library=library, device=device, relu_vr_data=relu_vr_data)
        logger.debug("prepare returned: %s", type(traced))
        logger.debug("traced has execute: %s", hasattr(traced, 'execute'))
        traced.execute(*inputs, execution_mode=execution_mode)  # Generate AIR IR

        # Export to .B file - only if build_dir is provided
        if build_dir:
            import time
            build_path = Path(build_dir)
            build_path.mkdir(parents=True, exist_ok=True)
            unique_id = f"{id(model)}_{int(time.time() * 1000) % 10000}"
            air_path = str(build_path / f"model_{unique_id}.B")
            traced.export_ir(air_path)
            logger.debug("AIR IR exported to: %s", air_path)
        # else: skip internal temp file creation - caller should provide build_dir if needed

        return traced

    def _convert_to_air(self, intermediate: TorchTracedModel) -> TorchTracedModel:
        """Execute traced model to generate AIR IR."""
        return intermediate

    def _export_to_onnx_file(
        self,
        model: nn.Module,
        inputs: List[torch.Tensor],
        input_names: Optional[List[str]] = None,
        output_path: str = None
    ) -> str:
        """Export PyTorch model to ONNX file.

        Args:
            model: PyTorch model to export
            inputs: List of input tensors for tracing
            input_names: Optional list of input names
            output_path: Output ONNX file path

        Returns:
            Path to written ONNX file
        """
        export_model_to_onnx(model, inputs, output_path, input_names)
        return output_path

    def _export_to_air_file(
        self,
        model: nn.Module,
        inputs: List[torch.Tensor],
        input_names: Optional[List[str]] = None,
        output_path: str = None,
        library: str = "antlib",
        device: str = "cpu",
        **kwargs
    ) -> str:
        """FX trace, execute, and write AIR IR to .B file.

        Args:
            model: PyTorch model to trace
            inputs: List of input tensors
            input_names: Optional list of input names
            output_path: Output .B file path
            library: Library name (antlib/phantom/acelib)
            device: Device name (cpu/cuda)
            **kwargs: Additional arguments (ignored)

        Returns:
            Path to written file
        """
        traced = self.compile(model, inputs, input_names, library=library, device=device)
        traced.export_ir(output_path)
        return output_path