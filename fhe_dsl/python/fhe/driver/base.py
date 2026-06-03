#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

from abc import ABC, abstractmethod
from typing import List, Any, Optional, Literal, Union
from pathlib import Path

try:
    import torch
except ImportError:
    torch = None


# ===== Frontend interface =====

class Frontend(ABC):
    """Convert input to IR via three-stage pipeline.

    Pipeline:
    1. prepare()   - Convert input to intermediate format (ONNX/FX/AST)
    2. compile()   - Convert intermediate to AIR IR (memory)
    3. export()    - Export output to file (.onnx or .B)

    Registered Frontends:
    - torch: Model/Function → FX Trace → AIR
    - torch-via-onnx: Model/Function → ONNX → AIR
    - ast: Python Function → AST → AIR
    - ast-via-onnx: Python Function → ONNX → AIR
    - onnx: ONNX File → AIR
    """

    @classmethod
    @abstractmethod
    def name(cls) -> str:
        """Unique identifier for the frontend."""
        pass

    @abstractmethod
    def prepare(self, *args, **kwargs) -> Any:
        """Prepare intermediate format.

        Args:
            *args: Frontend-specific arguments
            **kwargs: Frontend-specific keyword arguments

        Returns:
            Intermediate format object:
            - Torch: TorchTracedModel
            - TorchViaOnnx: ONNXFileIR
            - AST: FHEProgram (already AIR)
            - ASTViaOnnx: ONNXFileIR
            - Onnx: ONNXFileIR
        """
        pass

    def compile(self, *args, **kwargs) -> Any:
        """Convert input to AIR IR (in memory).

        This is the main entry point for FHE compilation.

        Args:
            *args: Passed to prepare()
            **kwargs: Passed to prepare()

        Returns:
            AIR IR object (CompilationUnit or equivalent)
        """
        intermediate = self.prepare(*args, **kwargs)
        return self._convert_to_air(intermediate)

    def _convert_to_air(self, intermediate: Any) -> Any:
        """Convert intermediate format to AIR IR.

        Subclasses override this to implement conversion logic.
        Some frontends (ast) may return intermediate as-is if already AIR.

        Args:
            intermediate: Output from prepare()

        Returns:
            AIR IR object
        """
        raise NotImplementedError(f"{self.name()} must implement _convert_to_air()")

    def export(
        self,
        *args,
        format: Literal["onnx", "air"] = "air",
        output_path: str,
        **kwargs
    ) -> str:
        """Export output to file.

        Args:
            *args: Passed to prepare()/compile()
            format: Output format - "onnx" or "air" (.B file)
            output_path: Output file path
            **kwargs: Passed to prepare()/compile()

        Returns:
            Path to written file
        """
        if format == "onnx":
            return self._export_to_onnx_file(*args, output_path=output_path, **kwargs)
        else:  # format == "air"
            return self._export_to_air_file(*args, output_path=output_path, **kwargs)

    def _export_to_onnx_file(self, *args, output_path: str, **kwargs) -> str:
        """Export to ONNX file.

        Only -via-onnx frontends support this.
        """
        raise NotImplementedError(f"{self.name()} doesn't support ONNX file output")

    def _export_to_air_file(self, *args, output_path: str, **kwargs) -> str:
        """Export to .B file (AIR serialized ELF)."""
        air = self.compile(*args, **kwargs)
        if hasattr(air, "export_ir"):
            air.export_ir(output_path)
        else:
            # FHEProgram or similar - may need different API
            raise NotImplementedError(
                f"{type(air).__name__} doesn't support export_ir()"
            )
        return output_path

    # ===== Legacy API (deprecated) =====

    def to_ir(self, *args, build_dir=None, **kwargs) -> Any:
        """Legacy API - use compile() instead.

        For backward compatibility, delegates to compile().

        Args:
            build_dir: Optional directory for intermediate files
        """
        return self.compile(*args, build_dir=build_dir, **kwargs)


# ===== Backend interface =====

class Backend(ABC):
    """Abstract base class for backend execution engines."""

    # Subclasses set to False if compile_to_lib is not yet implemented
    implemented: bool = True

    @classmethod
    @abstractmethod
    def backend_name(cls) -> str:
        """Return the FHE backend name, e.g., 'antlib'."""
        pass

    @classmethod
    @abstractmethod
    def device_name(cls) -> str:
        """Return the hardware device name, e.g., 'cpu'."""
        pass

    @classmethod
    @abstractmethod
    def supported_format_types(cls) -> List[str]:
        """List of IR types this backend can compile (e.g., ['air', 'onnx'])."""
        pass

    @abstractmethod
    def check_available(self) -> bool:
        """Check if this backend is available on the current system."""
        pass

    @abstractmethod
    def build_command(
        self,
        source: str,
        output: str,
        ace_root: Optional[str],
        extra_flags: Optional[List[str]]
    ) -> List[str]:
        """Construct the build command."""
        pass