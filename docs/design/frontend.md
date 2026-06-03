# Frontend Design

## Overview

The frontend module converts Python functions, PyTorch models, and ONNX files into intermediate representations (IR) for FHE compilation.

## Implementation Status Matrix

### Output Modes

| Frontend | prepare() | compile() | export(onnx) | export(air) | AIR Generation |
|----------|-----------|-----------|--------------|-------------|----------------|
| **torch** | TorchTracedModel | TorchTracedModel (AIR生成) | ONNX 文件 | .B 文件 | air_gen (C++ extension) |
| **torch-via-onnx** | ONNXFileIR | NotImplementedError | ONNX 文件 | .B 文件 | fhe_cmplr (external) |
| **ast** | FHEProgram | FHEProgram | ONNX 文件 | .B 文件 | air_gen (add_air_operation) |
| **ast-via-onnx** | ONNXFileIR | NotImplementedError | ONNX 文件 | .B 文件 | fhe_cmplr (external) |
| **onnx** | ONNXFileIR | NotImplementedError | ONNX 文件 | .B 文件 | fhe_cmplr (external) |

### IR Properties

| Frontend | format_type | file_format | Memory Mode |
|----------|-------------|-------------|-------------|
| torch | "memory" / "file" | None / "air" | ✓ Implemented |
| torch-via-onnx | "file" | "onnx" | ✗ Not implemented |
| ast | "memory" | None | ✓ Implemented |
| ast-via-onnx | "file" | "onnx" | ✗ Not implemented |
| onnx | "file" | "onnx" | ✗ Not implemented |

### Supported Operations (air_gen API)

| Operation | OPCODE | torch | ast |
|-----------|--------|-------|-----|
| add | NN::ADD | ✓ | ✓ |
| sub | NN::SUB | ✓ | ✓ |
| mul | NN::MUL | ✓ | ✓ |
| div | NN::DIVIDE | ✓ | ✓ |
| relu | NN::RELU | ✓ | ✓ |
| silu | NN::SILU | ✓ | ✓ |
| matmul | NN::MATMUL | ✓ | ✓ |
| conv | NN::CONV | ✓ | ✓ |
| gemm | NN::GEMM | ✓ | ✓ |
| max_pool | NN::MAX_POOL | ✓ | ✓ |
| avg_pool | NN::AVERAGE_POOL | ✓ | ✓ |
| global_avg_pool | NN::GLOBAL_AVERAGE_POOL | ✓ | ✓ |
| flatten | NN::FLATTEN | ✓ | ✓ |
| concat | NN::CONCAT | ✓ | ✓ |
| softmax | NN::SOFTMAX | ✓ | ✓ |
| sqrt | NN::SQRT | ✓ | ✓ |
| transpose | NN::TRANSPOSE | ✓ | ✓ |
| reshape | NN::RESHAPE | ✓ | ✓ |

### Architecture

```
torch 前端：torch.ops.tensor.xxx → C++ kernel → AddOperation() → AIR
AST 前端：  AST 分析 → FHEProgram → add_air_operation() → AIR
ONNX 前端：ONNX → fhe_cmplr → AIR

所有前端现在使用统一的 air_gen API！
```

## Directory Structure

```
ace/fhe/
├── __init__.py             # Package exports
├── decorators.py           # @compile, @compute, @export decorators
│
├── driver/                 # Pipeline orchestration
│   ├── __init__.py
│   ├── base.py             # Frontend, Backend ABCs
│   ├── pipeline.py         # Pipeline main class
│   ├── registry.py         # Frontend/Bckend registration
│   └── builder.py          # FHELibraryBuilder
│
├── frontend/               # Input conversion
│   ├── __init__.py
│   ├── torch2air.py        # Model/Function → AIR direct (via FX)
│   ├── torch2onnx.py       # Model/Function → ONNX → AIR
│   ├── ast2air.py          # Python AST → AIR
│   ├── ast2onnx.py         # Python Function → ONNX → AIR
│   └── onnx.py             # ONNX File → AIR
│
├── backend/                # Backend compilation
│   ├── __init__.py
│   ├── antlib.py           # Antlib backend (CPU)
│   ├── phantom.py          # Phantom backend (CUDA)
│   ├── acelib.py         # Acelib backend (CUDA)
│   ├── seal.py             # Microsoft SEAL backend (CPU)
│   └── openfhe.py          # OpenFHE backend (CPU)
│
├── runtime/
│   └── runtime.py          # FHERuntime for execution
│
├── config/                 # Configuration options
│   └── ...
│
└── util/                   # Utilities
```

## Registered Frontends

| Frontend Name | Input Type | Intermediate IR | Description |
|---------------|------------|-----------------|-------------|
| `torch` | Model/Function | TorchTracedModel → AIR | Direct FX tracing to AIR |
| `torch-via-onnx` | Model/Function | ONNXFileIR → AIR | Via ONNX export (stable) |
| `ast` | Function | FHEProgram | Python AST analysis |
| `ast-via-onnx` | Function | ONNXFileIR → AIR | Python AST via ONNX export |
| `onnx` | ONNX File | ONNXFileIR | Direct ONNX input |

## Frontend Base Class

```python
class Frontend(ABC):
    """Convert input to AIR IR via three-stage pipeline.

    Pipeline:
    1. prepare()   - Convert input to intermediate format (ONNX/FX/AST)
    2. compile()   - Convert intermediate to AIR IR (memory)
    3. export()    - Export output to file (.onnx or .B)
    """

    @classmethod
    @abstractmethod
    def name(cls) -> str:
        """Unique identifier for the frontend."""
        pass

    @abstractmethod
    def prepare(self, *args, **kwargs) -> Any:
        """Prepare intermediate format.

        Returns:
            Intermediate format object:
            - TorchFrontend: TorchTracedModel
            - TorchViaOnnxFrontend: ONNXFileIR
            - ASTFrontend: FHEProgram (already AIR)
            - ASTViaOnnxFrontend: ONNXFileIR
            - OnnxFrontend: ONNXFileIR
        """
        pass

    def compile(self, *args, **kwargs) -> Any:
        """Convert input to AIR IR (in memory).

        This is the main entry point for FHE compilation.
        """
        intermediate = self.prepare(*args, **kwargs)
        return self._convert_to_air(intermediate)

    def _convert_to_air(self, intermediate: Any) -> Any:
        """Convert intermediate format to AIR IR."""
        raise NotImplementedError(f"{self.name()} must implement _convert_to_air()")

    def export(self, *args, format: str, output_path: str, **kwargs) -> str:
        """Export output to file."""
        if format == "onnx":
            return self._export_to_onnx_file(*args, output_path=output_path, **kwargs)
        else:  # format == "air"
            return self._export_to_air_file(*args, output_path=output_path, **kwargs)

    def _export_to_onnx_file(self, *args, output_path: str, **kwargs) -> str:
        """Export to ONNX file. Only -via-onnx frontends support this."""
        raise NotImplementedError(f"{self.name()} doesn't support ONNX file output")

    def _export_to_air_file(self, *args, output_path: str, **kwargs) -> str:
        """Export to .B file (AIR serialized ELF)."""
        air = self.compile(*args, **kwargs)
        if hasattr(air, "export_ir"):
            air.export_ir(output_path)
        else:
            raise NotImplementedError(
                f"{type(air).__name__} doesn't support export_ir()"
            )
        return output_path
```

## Frontend Implementations

### torch Frontend

Direct FX tracing to AIR without ONNX intermediate.

**Files**: `frontend/torch_frontend.py`

**Flow**:
1. PyTorch model traced using `torch.fx.symbolic_trace()`
2. Graph nodes rewritten to custom ops (`torch.ops.tensor.xxx`)
3. Traced model executed to generate AIR IR
4. Output: TorchTracedModel (memory) or .B file

**Key Components**:
- `TORCH_OP_TO_CUSTOM_OP`: Mapping from standard torch ops to custom ops
- `rewrite_graph_to_custom_ops()`: Rewrites FX graph to use custom ops
- `TorchTracedModel`: Wrapper for traced model with AIR generation

**Usage**:
```python
from ace.fhe.frontend.torch_frontend import TorchFrontend

frontend = TorchFrontend()

# Memory output
traced = frontend.compile(model, inputs, input_names)
traced.execute(*inputs)  # Generates AIR IR

# File output
traced.export_ir("model.B")
```

### torch-via-onnx Frontend

PyTorch to AIR via ONNX export.

**Files**: `frontend/torch_via_onnx.py`

**Output Modes**:
1. **Bypass**: `prepare()` → ONNXFileIR → backend (ONNX file passed directly)
2. **AIR file**: `export(format="air")` → .B file → backend
3. **Memory**: `compile()` → FHEProgram → backend (**NOT IMPLEMENTED**)

**Flow**:
1. PyTorch model exported to ONNX via `torch.onnx.export()`
2. ONNX file stored as ONNXFileIR (for bypass) or converted to AIR .B file

**Usage**:
```python
from ace.fhe.frontend.torch_via_onnx import TorchViaOnnxFrontend

frontend = TorchViaOnnxFrontend()

# Mode 1: Bypass - ONNX file passed directly to backend
onnx_model = frontend.prepare(model, inputs, input_names)
# onnx_model.format_type = "file", file_format = "onnx"

# Mode 2: AIR file - Convert to .B file
frontend.export(model, inputs, format="air", output_path="model.B")

# Mode 3: Memory - NOT IMPLEMENTED
# air = frontend.compile(model, inputs, input_names)  # Raises NotImplementedError

# Export to ONNX file
frontend.export(model, inputs, format="onnx", output_path="model.onnx")
```

### ast Frontend

Pure Python AST analysis to AIR.

**Files**: `frontend/ast_frontend.py`, `ir/ast_conversion.py`

**Flow**:
1. Python function source parsed to AST
2. AST nodes converted to AIR IR nodes
3. Control flow (if/for) converted to CFG with BasicBlocks
4. Output: FHEProgram (memory)

**Supported Constructs**:
- Binary operations: `+`, `-`, `*`, `/`
- Comparison operations: `>`, `<`, `>=`, `<=`, `==`, `!=`
- Assignments
- If/else statements (with phi nodes)
- For loops over `range(n)`
- Function calls (torch.relu, etc.)

**Usage**:
```python
from ace.fhe.frontend.ast_frontend import ASTFrontend

frontend = ASTFrontend()

@compile(frontend="ast", library="antlib")
def compute(x, y):
    result = x + y
    if result > 0:
        return result * 2
    return result

prog = compute.compile([input_x, input_y])
```

### ast-via-onnx Frontend

Python function to AIR via ONNX export.

**Files**: `frontend/ast_via_onnx.py`

**Flow**:
1. Python function wrapped and exported to ONNX
2. ONNX file stored as ONNXFileIR
3. ONNX converted to AIR when needed

**Usage**:
```python
from ace.fhe.frontend.ast_via_onnx import ASTViaOnnxFrontend

frontend = ASTViaOnnxFrontend()

# Get ONNXFileIR
onnx_model = frontend.prepare(func, inputs, input_names)

# Export to ONNX file
frontend.export(func, inputs, format="onnx", output_path="func.onnx")
```

### onnx Frontend

Direct ONNX file input.

**Files**: `frontend/onnx_frontend.py`, `frontend/onnx_tools.py`

**Flow**:
1. ONNX file loaded and validated
2. Stored as ONNXFileIR
3. Can bypass to backend or convert to AIR

**Paths**:
1. **Bypass**: ONNX file → ONNXFileIR → backend (direct)
2. **AIR file**: ONNX file → AIR .B file → backend
3. **Memory**: ONNX file → FHEProgram (not yet implemented)

**Usage**:
```python
from ace.fhe.frontend.onnx_frontend import OnnxFrontend
from ace.fhe.ir import ONNXFileIR, AIRFileIR

frontend = OnnxFrontend()

# Bypass: ONNX file → backend
onnx_model = frontend.prepare("model.onnx")  # ONNXFileIR

# Convert: ONNX → AIR file
frontend.export("model.onnx", format="air", output_path="model.B")
air_model = AIRFileIR("model.B")
```

## ONNX Tools

Utility functions for ONNX operations.

**Files**: `frontend/onnx_tools.py`

### Export Functions

```python
def export_model_to_onnx(
    model: nn.Module,
    example_inputs: Union[torch.Tensor, Tuple[torch.Tensor, ...]],
    output_path: Union[str, Path],
    input_names: Optional[List[str]] = None,
    ...
) -> Path:
    """Export a PyTorch nn.Module to ONNX format."""

def export_function_to_onnx(
    func: callable,
    example_inputs: Union[torch.Tensor, Tuple[torch.Tensor, ...]],
    output_path: Union[str, Path],
    ...
) -> Path:
    """Export a standalone PyTorch function to ONNX."""
```

### Conversion Functions

```python
def convert_onnx_to_air(
    onnx_path: Union[str, Path],
    output_path: Optional[Union[str, Path]] = None
) -> Union[FHEProgram, str]:
    """Convert an ONNX model to AIR IR.

    Args:
        onnx_path: Path to ONNX model file
        output_path: Optional output path for AIR .B file.

    Returns:
        If output_path is None: FHEProgram instance.
        If output_path is provided: Path to the generated .B file.
    """
```

### Utility Functions

```python
def validate_onnx_model(onnx_path: Union[str, Path]) -> None:
    """Validate an ONNX model file."""

def inspect_onnx_model(onnx_path: Union[str, Path], verbose: bool = True) -> Dict:
    """Load and inspect an ONNX model, returning structured info."""
```

## Registry Pattern

Frontends are registered lazily to avoid import overhead:

```python
# Registration (in frontend/__init__.py)
register_frontend("torch", "ace.fhe.frontend.torch_frontend.TorchFrontend")
register_frontend("torch-via-onnx", "ace.fhe.frontend.torch_via_onnx.TorchViaOnnxFrontend")
register_frontend("ast", "ace.fhe.frontend.ast_frontend.ASTFrontend")
register_frontend("ast-via-onnx", "ace.fhe.frontend.ast_via_onnx.ASTViaOnnxFrontend")
register_frontend("onnx", "ace.fhe.frontend.onnx_frontend.OnnxFrontend")

# Retrieval (on demand)
frontend = get_frontend("torch-via-onnx")
```

## Extension Points

### Adding a New Frontend

1. Create a new frontend class implementing `Frontend`:
```python
class MyFrontend(Frontend):
    @classmethod
    def name(cls) -> str:
        return "my-frontend"

    def prepare(self, source, example_inputs, input_names):
        # Return intermediate format
        return intermediate

    def _convert_to_air(self, intermediate):
        # Convert to AIR IR
        return fhe_program
```

2. Register in `frontend/__init__.py`:
```python
register_frontend("my-frontend", "ace.fhe.frontend.my_frontend.MyFrontend")
```