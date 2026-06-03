import logging
from typing import Union, List, Callable, Any, Dict, Optional
from pathlib import Path

from ..base import CompilationUnit
from .graph import FHEGraph
from ..export.onnx_export import export_fhe_program_to_onnx
from ..export.air_export import export_fhe_program_to_air

logger = logging.getLogger(__name__)

class FHEProgram(CompilationUnit):
    """
    Internal FHE-specific intermediate representation.

    Contains one or more computation graphs with FHE-aware metadata.
    """

    def __init__(self, name: str = "default_module"):
        self._name = name
        self.graphs: Dict[str, FHEGraph] = {}
        self.global_vars: Dict[str, Any] = {}
        self.meta: Dict[str, Any] = {}
        self._file_path: Optional[str] = None

    @property
    def format_type(self) -> str:
        """Return 'file' if exported, 'memory' otherwise."""
        return "file" if self._file_path is not None else "memory"

    @property
    def file_format(self) -> Optional[str]:
        """Return 'air' if exported to .B file, None otherwise."""
        if self._file_path is not None:
            return "air"
        return None

    @property
    def file_path(self) -> Optional[str]:
        """Return the exported file path if available."""
        return self._file_path

    @property
    def entry_name(self) -> str:
        """Return the entry name."""
        return self._name

    @property
    def name(self) -> str:
        """Return the program name (alias for model_name)."""
        return self._name

    @name.setter
    def name(self, value: str):
        """Set the program name."""
        self._name = value

    def add_graph(self, name: str, graph: "FHEGraph"):
        self.graphs[name] = graph

    def add_function(self, func_name: str, graph: FHEGraph):
        if func_name in self.graphs:
            raise ValueError(f"Function '{func_name}' already exists")
        self.graphs[func_name] = graph

    def get_main_graph(self) -> FHEGraph:
        """Get the primary computation graph (usually 'forward')"""
        if "forward" in self.graphs:
            return self.graphs["forward"]
        elif len(self.graphs) == 1:
            return next(iter(self.graphs.values()))
        else:
            raise ValueError("No main function found")

    # Backward compatibility properties
    @property
    def nodes(self) -> List[Dict[str, Any]]:
        main_graph = self.get_main_graph()
        if main_graph.entry_block:
            return main_graph.entry_block.nodes
        # Fallback: collect from all blocks
        all_nodes = []
        for block in main_graph.blocks.values():
            all_nodes.extend(block.nodes)
        return all_nodes
    
    @property
    def inputs(self) -> List[str]:
        return self.get_main_graph().input_nodes
    
    @property
    def outputs(self) -> List[str]:
        return self.get_main_graph().output_nodes
    
    def get_function(self, func_name: str) -> Optional[FHEGraph]:
        return self.graphs.get(func_name)

    def list_functions(self) -> List[str]:
        return list(self.graphs.keys())

    def export_ir(self, filename: str, use_air_format: bool = True) -> bool:
        """
        Export the FHEProgram IR to a file.

        Args:
            filename: Output file path (e.g., "program.B", "program.onnx", "program.pkl")
            use_air_format: If True, generate AIR binary format (.B file) via IRBuilder.
                           If False, serialize as pickle.

        Returns:
            True if successful, False otherwise
        """
        filepath = Path(filename)
        success = False
        if filepath.suffix in ['.B', '.b', '.air'] and use_air_format:
            success = export_fhe_program_to_air(self, filename)
        elif filepath.suffix in ['.onnx']:
            success = export_fhe_program_to_onnx(self, filename)
        else:
            success = self._export_as_pickle(filename)

        if success:
            self._file_path = str(filepath.absolute())

        return success

    def _export_as_pickle(self, filename: str) -> bool:
        """Export FHEProgram as pickle file."""
        try:
            import pickle
            filepath = Path(filename)
            filepath.parent.mkdir(parents=True, exist_ok=True)
            with open(filepath, 'wb') as f:
                pickle.dump(self, f)
            logger.info(f"FHEProgram pickled to: {filename}")
            return True
        except Exception as e:
            logger.error(f"Failed to export FHEProgram as pickle: {e}")
            return False

    def write_ir(self, filename: str) -> bool:
        """
        Write the FHEProgram IR to a file (alias for export_ir).

        Args:
            filename: Output file path (e.g., "program.air" or "program.pkl")

        Returns:
            True if successful, False otherwise
        """
        return self.export_ir(filename)

