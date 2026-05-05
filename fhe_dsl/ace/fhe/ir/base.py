from abc import ABC, abstractmethod
from typing import Optional


class CompilationUnit(ABC):
    """
    Base class for all compilation units produced by frontends.

    This is the unified output type that backends consume.

    Properties:
    - format_type: "memory" or "file"
    - file_format: None, "onnx", or "air"
    - file_path: None or file path string
    - entry_name: Entry point name for config generation
    """
    @property
    @abstractmethod
    def format_type(self) -> str:
        """Return 'memory' or 'file'."""
        pass

    @property
    def file_format(self) -> Optional[str]:
        """Return file format: None, 'onnx', or 'air'."""
        return None

    @property
    def file_path(self) -> Optional[str]:
        """Return file path if exported, None otherwise."""
        return None

    @property
    @abstractmethod
    def entry_name(self) -> str:
        """Return the entry name (model/function/operator) for config file generation."""
        pass
