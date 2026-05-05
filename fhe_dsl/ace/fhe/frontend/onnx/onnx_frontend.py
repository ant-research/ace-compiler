# Supports:
#   - Path 1: ONNX file → backend (bypass)
#   - Path 2: ONNX → AIR file (.B) → backend
#   - Path 3: ONNX → AIR memory → backend (not implemented)
#
# Pipeline:
# 1. prepare()  - Load ONNX file → ONNXFileIR
# 2. compile()  - ONNX → AIR IR (memory) - NOT IMPLEMENTED
# 3. export()   - Export ONNX file (copy) or .B file

from typing import List, Any, Optional
from pathlib import Path
import shutil

from ...driver import Frontend
from ...ir import CompilationUnit, ONNXFileIR, convert_onnx_to_air


class Onnx(Frontend):
    """ONNX File → AIR frontend.

    Pipeline:
    1. prepare()  - Load ONNX file → ONNXFileIR
    2. compile()  - ONNX → AIR IR (memory) - NOT IMPLEMENTED
    3. export()   - Export ONNX file (copy) or .B file

    Output Modes:
    - Bypass: prepare() → ONNXFileIR → backend (ONNX file passed directly)
    - AIR file: export(format="air") → .B file → backend
    - Memory: compile() → FHEProgram → backend (NOT IMPLEMENTED)
    """

    @classmethod
    def name(cls) -> str:
        return "onnx"

    def prepare(
        self,
        onnx_path: str,
        example_inputs: Optional[List[Any]] = None,
        input_names: Optional[List[str]] = None,
        build_dir: Optional[str] = None,
        **kwargs
    ) -> ONNXFileIR:
        """Load ONNX file.

        This is the "bypass" mode - ONNX file is passed directly to backend.

        Args:
            onnx_path: Path to ONNX model file
            example_inputs: Not used (for API consistency)
            input_names: Not used (for API consistency)
            build_dir: Not used (for API consistency with other frontends)
            **kwargs: Additional keyword arguments (ignored)

        Returns:
            ONNXFileIR wrapping the ONNX file (format_type="file", file_format="onnx")
        """
        return ONNXFileIR(onnx_path)

    def compile(
        self,
        onnx_path: str,
        example_inputs: Optional[List[Any]] = None,
        input_names: Optional[List[str]] = None,
        build_dir: Optional[str] = None,
        **kwargs
    ) -> CompilationUnit:
        """Load ONNX file for compilation.

        For ONNX frontend, this returns ONNXFileIR directly (bypass mode).
        The backend will handle the ONNX file directly.

        Args:
            onnx_path: Path to ONNX model file
            example_inputs: Not used (for API consistency)
            input_names: Not used (for API consistency)
            build_dir: Not used (for API consistency with other frontends)
            **kwargs: Additional keyword arguments (ignored)

        Returns:
            ONNXFileIR wrapping the ONNX file
        """
        # Bypass mode: return ONNXFileIR directly
        return self.prepare(onnx_path, example_inputs, input_names)

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
        *args,
        output_path: str = None,
        **kwargs
    ) -> str:
        """Bypass ONNX file to backend (identity operation).

        Args:
            *args: First arg is onnx_path
            output_path: Optional destination path (if provided, copy file)
            **kwargs: Additional keyword arguments

        Returns:
            Path to ONNX file (original or copied)
        """
        onnx_path = args[0] if args else kwargs.get('onnx_path')

        # If output_path specified, copy the file; otherwise return original
        if output_path:
            shutil.copy(onnx_path, output_path)
            return output_path
        return onnx_path

    def _export_to_air_file(
        self,
        *args,
        output_path: str,
        **kwargs
    ) -> str:
        """Convert ONNX to AIR binary and save to .B file.

        Uses fhe_cmplr to convert ONNX to AIR binary format.

        Args:
            *args: First arg is onnx_path
            output_path: Destination .B file path

        Returns:
            Path to written file
        """
        onnx_path = args[0] if args else kwargs.get('onnx_path')
        return convert_onnx_to_air(onnx_path, output_path=output_path)