# IR Design

## Overview

The IR (Intermediate Representation) module defines the data structures used to represent FHE programs during compilation. It supports three formats: memory, ONNX file, and AIR file.

## IR Format Types

| Format | Class | format_type | file_format | Use Case |
|--------|-------|-------------|-------------|----------|
| **Memory** | `FHEProgram` | "memory" | - | In-memory IR for direct backend consumption |
| **Memory** | `TorchTracedModel` | "memory"/"file" | None/"air" | FX-traced PyTorch model |
| **ONNX File** | `ONNXFileIR` | "file" | "onnx" | ONNX file for backend bypass |
| **AIR File** | `AIRFileIR` | "file" | "air" | Serialized AIR binary (.B file) |

## Class Hierarchy

```
CompilationUnit (ABC)
├── FHEProgram          # Memory IR (AST frontend)
│   └── FHEGraph
│       └── BasicBlock
│           └── IRNode
├── TorchTracedModel    # Memory IR (Torch frontend)
├── FileIR (ABC)        # File-based IR base
│   ├── ONNXFileIR      # ONNX file
│   └── AIRFileIR       # AIR binary file
```

## Directory Structure

```
python/ace/fhe/ir/
├── __init__.py          # Module entry, exports
├── base.py              # CompilationUnit base class
├── fhe_program.py       # FHEProgram (memory IR)
├── graph.py             # FHEGraph, BasicBlock, IRNode
├── torch_trace.py       # TorchTracedModel (FX traced IR)
├── ir_formats.py        # FileIR, ONNXFileIR, AIRFileIR
├── onnx_tools.py        # ONNX export/convert utilities
├── export/              # Export utilities
│   ├── __init__.py
│   ├── onnx_export.py   # FHEProgram → ONNX export
│   ├── air_export.py    # FHEProgram → AIR export
│   └── serializer.py    # Pickle serialization
└── conversion/          # Conversion utilities
    ├── __init__.py
    ├── ast_converter.py # AST → IR conversion
    └── onnx_converter.py # ONNX → IR conversion
```

## Base Class

```python
class CompilationUnit(ABC):
    """Base class for all IR formats."""

    @property
    @abstractmethod
    def format_type(self) -> str:
        """Return 'file' or 'memory'.

        Used by backend to determine compilation strategy.
        """
        pass

    @property
    def file_format(self) -> Optional[str]:
        """Return file format: None, 'onnx', or 'air'.

        Default implementation returns None.
        """
        return None

    @property
    def file_path(self) -> Optional[str]:
        """Return file path if format_type is 'file'.

        Returns None for memory IR.
        """
        return None

    @property
    @abstractmethod
    def entry_name(self) -> str:
        """Return the entry function name.

        Used for symbol naming in generated code.
        """
        pass
```

## FHEProgram (Memory IR)

In-memory representation of FHE programs.

**Files**: `ir/fhe_program.py`

```python
class FHEProgram(CompilationUnit):
    """Internal FHE-specific intermediate representation.

    Contains one or more computation graphs with FHE-aware metadata.
    """

    def __init__(self, name: str = "default_module"):
        self._name = name
        self.graphs: Dict[str, FHEGraph] = {}
        self.global_vars: Dict[str, Any] = {}
        self.meta: Dict[str, Any] = {}

    @property
    def format_type(self) -> str:
        """Return 'memory' for in-memory IR."""
        return "memory"

    @property
    def file_format(self) -> Optional[str]:
        """Return None for memory IR."""
        return None

    @property
    def file_path(self) -> Optional[str]:
        """Return None for memory IR."""
        return None

    @property
    def entry_name(self) -> str:
        """Return the entry name."""
        return self._name

    def add_graph(self, name: str, graph: "FHEGraph"):
        """Add a computation graph."""
        self.graphs[name] = graph

    def get_main_graph(self) -> "FHEGraph":
        """Get the primary computation graph (usually 'forward')."""
        if "forward" in self.graphs:
            return self.graphs["forward"]
        elif len(self.graphs) == 1:
            return next(iter(self.graphs.values()))
        else:
            raise ValueError("No main function found")

    def export_ir(self, filename: str) -> bool:
        """Export IR to file.

        Supports:
        - .B / .air: AIR binary format via air_gen
        - .onnx: ONNX format
        - .pkl: Pickle serialization
        """
        pass
```

## TorchTracedModel (FX Traced IR)

IR wrapper for FX-traced PyTorch models.

**Files**: `ir/torch_trace.py`

```python
class TorchTracedModel(CompilationUnit):
    """IR wrapper for FX-traced PyTorch model.

    Properties:
    - format_type: "memory" before export, "file" after export_ir()
    - file_format: None before export, "air" after export_ir()
    - file_path: None before export, path after export_ir()
    """

    def __init__(self, traced_model, input_names, input_shapes, output_shape):
        self.traced_model = traced_model
        self._input_names = input_names
        self._input_shapes = input_shapes
        self._output_shape = output_shape
        self._air_generated = False
        self._file_path = None

    @property
    def format_type(self) -> str:
        """Return 'file' if exported, 'memory' otherwise."""
        if self._file_path is not None:
            return "file"
        return "memory"

    @property
    def file_format(self) -> Optional[str]:
        """Return 'air' if exported to .B file, None otherwise."""
        if self._file_path is not None:
            return "air"
        return None

    def execute(self, *args, **kwargs):
        """Execute traced model to generate AIR IR."""
        pass

    def export_ir(self, filename: str) -> bool:
        """Export generated AIR IR to .B file."""
        pass
```

## FHEGraph and BasicBlock

Graph structure for representing computation.

**Files**: `ir/graph.py`

```python
class IRNode:
    """IR node representing a single operation."""

    def __init__(self, name: str):
        self.name = name
        self.op_type = ""
        self.inputs: List[str] = []
        self.outputs: List[str] = []
        self.attributes: Dict[str, Any] = {}
        self.dtype = None
        self.shape = None


class BasicBlock:
    """Basic block containing sequential IR nodes."""

    def __init__(self, name: str):
        self.name = name
        self.nodes: List[IRNode] = []
        self.predecessors: List["BasicBlock"] = []
        self.successors: List["BasicBlock"] = []

    def add_node(self, node: Dict[str, Any]):
        """Add an IR node to this block."""
        self.nodes.append(node)


class FHEGraph:
    """Computation graph for FHE operations."""

    def __init__(self, name: str):
        self.name = name
        self.blocks: Dict[str, BasicBlock] = {}
        self.entry_block: Optional[BasicBlock] = None
        self.input_nodes: List[str] = []
        self.output_nodes: List[str] = []
        self.metadata: Dict[str, Any] = {}

    def add_block(self, block: BasicBlock):
        """Add a basic block to the graph."""
        self.blocks[block.name] = block
```

## FileIR (File-based IR)

Base class for file-based IR formats.

**Files**: `ir/ir_formats.py`

```python
class FileIR(CompilationUnit):
    """Base class for file-based IR formats.

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
```

## ONNX Tools

ONNX export and conversion utilities.

**Files**: `ir/onnx_tools.py`

```python
# PyTorch → ONNX export
def export_model_to_onnx(model, example_inputs, output_path, ...) -> Path:
    """Export PyTorch model to ONNX format."""
    pass

def export_function_to_onnx(func, example_inputs, output_path, ...) -> Path:
    """Export Python function to ONNX format."""
    pass

# ONNX → AIR conversion
def convert_onnx_to_air(onnx_path, output_path=None) -> Union[FHEProgram, str]:
    """Convert ONNX to AIR IR or .B file."""
    pass

# Validation
def validate_onnx_model(onnx_path) -> None:
    """Validate ONNX model file."""
    pass

def inspect_onnx_model(onnx_path, verbose=True) -> Dict:
    """Inspect ONNX model structure."""
    pass
```

## Export Utilities

### ONNX Export

**Files**: `ir/export/onnx_export.py`

```python
# IR op_type to ONNX op_type mapping
OP_TYPE_TO_ONNX = {
    "add": "Add",
    "sub": "Sub",
    "mul": "Mul",
    "div": "Div",
    "relu": "Relu",
    "matmul": "MatMul",
    "conv": "Conv",
    "gemm": "Gemm",
    # ...
}

def export_fhe_program_to_onnx(fhe_program: FHEProgram, filename: str) -> bool:
    """Export FHEProgram as ONNX format."""
    pass
```

### AIR Export

**Files**: `ir/export/air_export.py`

```python
# Supported operations for AIR export
SUPPORTED_AIR_OPS = {
    "add", "sub", "mul", "div", "relu", "silu", "matmul", "conv",
    "max_pool", "avg_pool", "global_avg_pool", "flatten", "concat",
    "softmax", "sqrt", "gemm", "transpose", "reshape",
}

def export_fhe_program_to_air(fhe_program: FHEProgram, filename: str) -> bool:
    """Export FHEProgram as AIR binary format (.B file)."""
    pass
```

## Conversion Utilities

### AST Converter

**Files**: `ir/conversion/ast_converter.py`

```python
class ASTToIRConverter:
    """Convert Python AST to IR."""

    def convert_function(self, func, graph_name=None) -> FHEGraph:
        """Convert a Python function to FHEGraph."""
        pass

    def convert_module_from_source(self, source_code, filename="<string>") -> FHEProgram:
        """Convert source code string to FHEProgram."""
        pass
```

### ONNX Converter

**Files**: `ir/conversion/onnx_converter.py`

```python
def convert_onnx_to_fhe_program(onnx_path) -> FHEProgram:
    """Convert ONNX model to FHEProgram."""
    pass

def convert_onnx_to_air_binary(onnx_path, output_path) -> str:
    """Convert ONNX to AIR binary using fhe_cmplr."""
    pass
```

## Module Exports

```python
# ir/__init__.py

# Base class
from .base import CompilationUnit

# Memory IR
from .fhe_program import FHEProgram
from .graph import BasicBlock, FHEGraph, IRNode

# Torch traced IR
from .torch_trace import TorchTracedModel, FXTracedModel

# File IR formats
from .ir_formats import FileIR, ONNXFileIR, AIRFileIR
from .ir_formats import ONNXModel, AIRModel, FileModel  # backward compat

# ONNX tools
from .onnx_tools import (
    export_model_to_onnx,
    export_function_to_onnx,
    convert_onnx_to_air,
    convert_onnx_to_fhe_program,
    validate_onnx_model,
    inspect_onnx_model,
)

# Conversion utilities
from .conversion import ASTToIRConverter

# Export utilities
from .export import IRSerializer, export_fhe_program_to_onnx, export_fhe_program_to_air
```

## Test Structure

```
tests/test_unit/test_ir/
├── test_base.py              # CompilationUnit base class tests
├── test_fhe_program.py       # FHEProgram tests
├── test_graph.py             # IRNode, BasicBlock, FHEGraph tests
├── test_torch_trace.py       # TorchTracedModel tests
├── test_ir_formats.py        # FileIR, ONNXFileIR, AIRFileIR tests
├── test_onnx_tools.py        # ONNX tools tests
├── test_export/
│   ├── __init__.py
│   ├── test_onnx_export.py   # ONNX export tests
│   └── test_air_export.py    # AIR export tests
└── test_conversion/
    ├── __init__.py
    ├── test_ast_converter.py # AST converter tests
    └── test_onnx_converter.py # ONNX converter tests
```

## Backend Integration

Backends use `format_type` and `file_format` to determine compilation strategy:

```python
# In backend
def compile_to_lib(self, ir, output_dir: str) -> str:
    # Handle string/Path directly
    if isinstance(ir, (str, Path)):
        return self._compile_file(ir, output_dir)

    # Get format type
    format_type = ir.format_type

    if format_type == "file":
        # Check file format
        file_format = ir.file_format
        if file_format == "onnx":
            # Compile ONNX file
            return self._compile_file(ir.file_path, output_dir)
        elif file_format == "air":
            # Compile AIR file
            return self._compile_file(ir.file_path, output_dir)
    elif format_type == "memory":
        # Memory IR (not yet implemented)
        raise NotImplementedError("Memory IR compilation not yet implemented")
```

## Usage Examples

### Creating FHEProgram

```python
from ace.fhe.ir import FHEProgram, FHEGraph, BasicBlock, IRNode

# Create program
program = FHEProgram(name="my_program")

# Create graph
graph = FHEGraph(name="forward")

# Create basic block
block = BasicBlock(name="entry")

# Create IR node
node = IRNode("add_node")
node.op_type = "add"
node.inputs = ["x", "y"]
node.outputs = ["result"]
block.nodes.append(node)

# Assemble
graph.add_block(block)
graph.entry_block = block
graph.input_nodes = ["x", "y"]
graph.output_nodes = ["result"]
program.add_graph("forward", graph)

# Properties
print(program.format_type)  # "memory"
print(program.entry_name)   # "my_program"
```

### Creating FileIR

```python
from ace.fhe.ir import ONNXFileIR, AIRFileIR

# ONNX file
onnx_ir = ONNXFileIR("model.onnx")
print(onnx_ir.format_type)   # "file"
print(onnx_ir.file_format)   # "onnx"
print(onnx_ir.file_path)     # "model.onnx"
print(onnx_ir.entry_name)    # "model"

# AIR file
air_ir = AIRFileIR("model.B")
print(air_ir.format_type)    # "file"
print(air_ir.file_format)    # "air"
print(air_ir.file_path)      # "model.B"
print(air_ir.entry_name)     # "model"
```

### Exporting IR

```python
from ace.fhe.ir import FHEProgram, IRSerializer

# Create program
program = FHEProgram(name="test")

# Export to different formats
program.export_ir("model.B")      # AIR binary
program.export_ir("model.onnx")   # ONNX
program.export_ir("model.pkl")    # Pickle

# Or use specific export functions
from ace.fhe.ir.export import export_fhe_program_to_onnx
export_fhe_program_to_onnx(program, "model.onnx")
```