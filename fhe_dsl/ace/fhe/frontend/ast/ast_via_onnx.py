#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
AST Via ONNX Frontend - Converts Python functions to AIR via ONNX.

Pipeline:
1. prepare()  - Python function → ONNX file
2. compile()  - ONNX → AIR IR (memory) - NOT IMPLEMENTED
3. export()   - Export ONNX file or .B file

Output Modes:
- Bypass: prepare() → ONNXFileIR → backend (ONNX file passed directly)
- AIR file: export(format="air") → .B file → backend
- Memory: compile() → FHEProgram → backend (NOT IMPLEMENTED)
"""

import os
import torch
import torch.nn as nn
from pathlib import Path
from typing import Union, Optional, List, Callable
import shutil

from ...driver import Frontend
from ...ir import CompilationUnit, ONNXFileIR, export_function_to_onnx, export_model_to_onnx, convert_onnx_to_air


class ASTViaOnnx(Frontend):
    """Python Function/Model → ONNX → AIR frontend.

    Pipeline:
    1. prepare()  - Python → ONNX file
    2. compile()  - ONNX → AIR IR (memory) - NOT IMPLEMENTED
    3. export()   - Export ONNX file or .B file

    Output Modes:
    - Bypass: prepare() → ONNXFileIR → backend (ONNX file passed directly)
    - AIR file: export(format="air") → .B file → backend
    - Memory: compile() → FHEProgram → backend (NOT IMPLEMENTED)
    """

    @classmethod
    def name(cls) -> str:
        return "ast-via-onnx"

    def prepare(
        self,
        source: Union[nn.Module, Callable],
        example_inputs: List[torch.Tensor],
        input_names: Optional[List[str]] = None,
        build_dir: Optional[str] = None,
        **kwargs
    ) -> ONNXFileIR:
        """Export Python function/model to ONNX file.

        This is the "bypass" mode - ONNX file is passed directly to backend.

        Args:
            source: Python function or PyTorch model
            example_inputs: Example input tensors
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
    ) -> CompilationUnit:
        """Export Python to AIR IR in memory.

        NOT IMPLEMENTED - Memory IR compilation is not yet supported.

        Args:
            source: Python function or PyTorch model
            example_inputs: Example input tensors
            input_names: Optional list of input names
            build_dir: Not used (for API consistency with other frontends)
            **kwargs: Additional keyword arguments (ignored)

        Returns:
            FHEProgram (NOT IMPLEMENTED)

        Raises:
            NotImplementedError: Always raised for now
        """
        # Step 1: Python -> ONNX file
        onnx_model = self.prepare(source, example_inputs, input_names, build_dir, **kwargs)

        # Step 2: ONNX -> AIR IR (memory)
        # This would require fhe_cmplr to output in-memory IR representation
        # Currently not implemented
        raise NotImplementedError(
            "ast-via-onnx 'memory' output is not implemented. "
            "Use export(format='air') for .B file output, "
            "or prepare() for ONNX bypass mode."
        )

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
            source: Python function or model
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
            source: Python function or model
            example_inputs: Example inputs
            input_names: Optional input names
            output_path: Output .B file path

        Returns:
            Path to written .B file
        """
        # Step 1: Python -> ONNX file
        onnx_model = self.prepare(source, example_inputs, input_names)

        # Step 2: ONNX -> AIR binary using fhe_cmplr
        air_path = convert_onnx_to_air(onnx_model.onnx_path, output_path)

        return air_path