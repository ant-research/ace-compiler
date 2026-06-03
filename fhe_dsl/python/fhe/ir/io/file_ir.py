from pathlib import Path
from ..base import CompilationUnit


class FileIR(CompilationUnit):
    """Base class for file-based IR formats.

    Used to represent file inputs (ONNX, AIR .B) for backend compilation.

    format_type: "file" - indicates file-based input
    file_format: "onnx" or "air" - specific file format
    """

    @property
    def format_type(self) -> str:
        """Return 'file' to indicate file-based input."""
        return "file"

    @property
    def file_format(self) -> str:
        """Return the specific file format: 'onnx' or 'air'."""
        raise NotImplementedError("Subclasses must implement file_format")

    def __init__(self, file_path: str):
        self._file_path = str(file_path)

    @property
    def file_path(self) -> str:
        """Return the file path."""
        return self._file_path

    @property
    def entry_name(self) -> str:
        """Return the entry name (basename without extension)."""
        return Path(self._file_path).stem


class ONNXFileIR(FileIR):
    """ONNX model file as a compilation unit."""

    @property
    def file_format(self) -> str:
        return "onnx"

    def __init__(self, onnx_path: str):
        super().__init__(onnx_path)
        self.onnx_path = onnx_path  # Keep for backward compatibility


class AIRFileIR(FileIR):
    """AIR binary file (.B) as a compilation unit."""

    @property
    def file_format(self) -> str:
        return "air"

    def __init__(self, air_path: str):
        super().__init__(air_path)
        self.air_path = air_path


# Backward compatibility aliases
ONNXModel = ONNXFileIR
AIRModel = AIRFileIR
FileModel = FileIR