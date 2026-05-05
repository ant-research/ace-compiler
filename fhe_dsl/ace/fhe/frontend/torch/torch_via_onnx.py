# Supports:
#   - Path 1: torch model → ONNX file → backend (bypass)
#   - Path 2: torch model → ONNX → AIR file (.B) → backend
#   - Path 3: torch model → ONNX → AIR memory → backend (not implemented)
#
# Pipeline:
# 1. prepare()  - PyTorch → ONNX file
# 2. compile()  - ONNX → AIR IR (memory) - NOT IMPLEMENTED
# 3. export()   - Export ONNX file or .B file

import os
import torch
import torch.nn as nn
from pathlib import Path
from typing import Union, Optional, List, Callable
import shutil

from ...driver import Frontend
from ...ir import CompilationUnit, ONNXFileIR, AIRFileIR, FHEProgram, export_model_to_onnx, export_function_to_onnx, convert_onnx_to_air


class TorchViaOnnx(Frontend):
    """PyTorch Model/Function → ONNX → AIR frontend.

    Pipeline:
    1. prepare()  - PyTorch → ONNX file
    2. compile()  - ONNX → AIR IR (memory) - NOT IMPLEMENTED
    3. export()   - Export ONNX file or .B file

    Output Modes:
    - Bypass: prepare() → ONNXFileIR → backend (ONNX file passed directly)
    - AIR file: export(format="air") → .B file → backend
    - Memory: compile() → FHEProgram → backend (NOT IMPLEMENTED)
    """

    @classmethod
    def name(cls) -> str:
        return "torch-via-onnx"

    def prepare(
        self,
        source: Union[nn.Module, Callable],
        example_inputs: List[torch.Tensor],
        input_names: Optional[List[str]] = None,
        build_dir: Optional[str] = None,
        **kwargs
    ) -> ONNXFileIR:
        """Export PyTorch model/function to ONNX file.

        This is the "bypass" mode - ONNX file is passed directly to backend.

        Args:
            source: PyTorch model or Python function
            example_inputs: Example input tensors for tracing
            input_names: Optional list of input names
            build_dir: Not used (for API consistency with other frontends)
            **kwargs: Additional keyword arguments (ignored)

        Returns:
            ONNXFileIR wrapping the ONNX file (format_type="file", file_format="onnx")
        """
        # Use provided build_dir for temporary ONNX files, or create temp dir
        if build_dir:
            onnx_path = str(Path(build_dir) / f"tmp_{os.getpid()}.onnx")
        else:
            import tempfile
            tmpdir = tempfile.mkdtemp()
            onnx_path = str(Path(tmpdir) / f"tmp_{os.getpid()}.onnx")

        if isinstance(source, nn.Module):
            export_model_to_onnx(source, example_inputs, onnx_path, input_names)
        else:
            export_function_to_onnx(source, example_inputs, onnx_path, input_names)

        return ONNXFileIR(onnx_path)

    def compile(
        self,
        source: Union[nn.Module, Callable],
        example_inputs: List[torch.Tensor],
        input_names: Optional[List[str]] = None,
        build_dir: Optional[str] = None,
        **kwargs
    ) -> ONNXFileIR:
        """Export PyTorch to ONNX for compilation.

        For torch-via-onnx frontend, this returns ONNXFileIR directly (bypass mode).
        The backend will handle the ONNX file directly.

        Args:
            source: PyTorch model or Python function
            example_inputs: Example input tensors
            input_names: Optional list of input names
            build_dir: Not used (for API consistency with other frontends)
            **kwargs: Additional keyword arguments (ignored)

        Returns:
            ONNXFileIR wrapping the exported ONNX file
        """
        # Bypass mode: export to ONNX and return ONNXFileIR
        return self.prepare(source, example_inputs, input_names, build_dir, **kwargs)

    def _convert_to_air(self, intermediate: ONNXFileIR) -> CompilationUnit:
        """Convert ONNX to AIR IR (if needed by backend).

        NOT IMPLEMENTED - Memory IR conversion is not supported.
        """
        raise NotImplementedError(
            "AIR memory IR conversion is not implemented. "
            "Use _export_to_air_file() for .B file output."
        )

    def _export_to_onnx_file(
        self,
        source: Union[nn.Module, Callable],
        example_inputs: List[torch.Tensor],
        input_names: Optional[List[str]] = None,
        output_path: str = None
    ) -> str:
        """Export to ONNX and save to file.

        Args:
            source: PyTorch model or function
            example_inputs: Example inputs
            input_names: Optional input names
            output_path: Output ONNX file path

        Returns:
            Path to written ONNX file
        """
        onnx = self.prepare(source, example_inputs, input_names)
        shutil.copy(onnx.onnx_path, output_path)
        return output_path

    def _export_to_air_file(
        self,
        source: Union[nn.Module, Callable],
        example_inputs: List[torch.Tensor],
        input_names: Optional[List[str]] = None,
        output_path: str = None
    ) -> str:
        """Export to ONNX, convert to AIR binary, and save to .B file.

        Uses fhe_cmplr to convert ONNX to AIR binary format.

        Args:
            source: PyTorch model or function
            example_inputs: Example inputs
            input_names: Optional input names
            output_path: Output .B file path

        Returns:
            Path to written .B file
        """
        # Step 1: PyTorch -> ONNX file
        onnx_model = self.prepare(source, example_inputs, input_names)

        # Step 2: ONNX -> AIR binary using fhe_cmplr
        air_path = convert_onnx_to_air(onnx_model.onnx_path, output_path)

        return air_path