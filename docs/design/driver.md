# Driver Design

## Overview

The driver module orchestrates the FHE compilation pipeline, connecting frontends, IR, and backends.

## Architecture

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                              Driver Layer                                    │
│                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                         Driver                                       │   │
│  │                                                                      │   │
│  │  Responsibilities:                                                   │   │
│  │  - Select frontend by name (via registry)                            │   │
│  │  - Select backend by name (via registry)                             │   │
│  │  - Orchestrate frontend.compile() → backend.build() pipeline         │   │
│  │  - Handle IR format conversion                                       │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                      │                                      │
│            ┌─────────────────────────┼─────────────────────────┐            │
│            ▼                         ▼                         ▼            │
│  ┌───────────────────────┐ ┌───────────────────────┐ ┌───────────────────┐  │
│  │   Frontend Registry   │ │    Backend Registry   │ │   Builder         │  │
│  │   (registry.py)       │ │   (registry.py)       │ │   (builder.py)    │  │
│  │   - Lazy loading      │ │   - Strategy pattern  │ │   - Build .so     │  │
│  │   - Plugin support    │ │   - Device mapping    │ │   - Config mgmt   │  │
│  └───────────────────────┘ └───────────────────────┘ └───────────────────┘  │
└─────────────────────────────────────────────────────────────────────────────┘
```

## Driver Class

The main class for orchestrating FHE compilation.

**Files**: `driver/driver.py`

```python
class Driver:
    """FHE Compiler Driver.

    Orchestrates frontend → IR → backend pipeline.

    Usage:
        compiler = Driver(
            frontend_name="torch-via-onnx",
            backend_name="antlib",
            backend_config={"device": "cpu"}
        )

        # Compile model
        result = compiler.compile(model, inputs, input_names)

        # Build library
        lib_path = compiler.build(ir, output_dir)
    """

    def __init__(
        self,
        frontend_name: str,
        backend_name: str,
        backend_config: Optional[dict] = None
    ):
        """Initialize compiler with frontend and backend.

        Args:
            frontend_name: Name of registered frontend
            backend_name: Name of registered backend
            backend_config: Backend configuration options

        Raises:
            ValueError: If frontend or backend not found
        """
        self.frontend = get_frontend(frontend_name)
        self.backend_impl = get_backend_strategy(backend_name, backend_config or {})
        self.backend_config = backend_config or {}

    def compile(self, source, example_inputs, input_names=None) -> Any:
        """Compile source to IR.

        Args:
            source: Model, function, or file path
            example_inputs: Example inputs for tracing
            input_names: Optional input names

        Returns:
            IR object (FHEProgram, ONNXFileIR, or AIRFileIR)
        """
        return self.frontend.compile(source, example_inputs, input_names)

    def build(self, ir, output_dir: str) -> str:
        """Build IR to shared library.

        Args:
            ir: IR object from compile()
            output_dir: Output directory for generated files

        Returns:
            Path to generated .so file
        """
        return self.backend_impl.compile_to_lib(ir, output_dir)
```

## Registry Pattern

The registry provides lazy loading and plugin support for frontends and backends.

**Files**: `compiler/registry.py`

### Frontend Registry

```python
# Registration
def register_frontend(name: str, class_path: str) -> None:
    """Register a frontend class by import path.

    Args:
        name: Unique identifier for the frontend
        class_path: Full import path (e.g., "ace.fhe.frontend.torch_frontend.TorchFrontend")
    """
    _FRONTEND_REGISTRY[name] = class_path

# Retrieval
def get_frontend(name: str) -> Frontend:
    """Get frontend instance by name.

    Lazily imports and instantiates the frontend class.

    Args:
        name: Registered frontend name

    Returns:
        Frontend instance

    Raises:
        ValueError: If frontend not found
    """
    if name not in _FRONTEND_REGISTRY:
        raise ValueError(f"Unknown frontend: {name}. Available: {list_frontends()}")

    class_path = _FRONTEND_REGISTRY[name]
    module_path, class_name = class_path.rsplit(".", 1)

    module = importlib.import_module(module_path)
    cls = getattr(module, class_name)
    return cls()

def list_frontends() -> List[str]:
    """List all registered frontend names."""
    return list(_FRONTEND_REGISTRY.keys())
```

### Backend Registry

```python
# Registration
def register_backend(name: str, device: str, backend_class: type) -> None:
    """Register a backend class.

    Args:
        name: Backend name (e.g., "antlib")
        device: Device name (e.g., "cpu", "cuda")
        backend_class: Backend class
    """
    key = (name, device)
    _BACKEND_REGISTRY[key] = backend_class

# Retrieval
def get_backend_strategy(name: str, config: dict) -> Backend:
    """Get backend instance by name and config.

    Args:
        name: Backend name
        config: Configuration dict (must include "device")

    Returns:
        Backend instance

    Raises:
        ValueError: If backend not found
    """
    device = config.get("device", "cpu")
    key = (name, device)

    if key not in _BACKEND_REGISTRY:
        raise ValueError(f"Unknown backend: {name}/{device}")

    return _BACKEND_REGISTRY[key](**config)

def list_backends() -> List[Tuple[str, str]]:
    """List all registered (backend, device) pairs."""
    return list(_BACKEND_REGISTRY.keys())
```

## Builder Module

The builder handles the final compilation step to shared library.

**Files**: `compiler/builder.py`

```python
class FHELibraryBuilder:
    """Build FHE shared library from IR."""

    def __init__(self, backend: Backend):
        self.backend = backend

    def build(self, ir, output_dir: str) -> dict:
        """Build shared library from IR.

        Args:
            ir: IR object (FHEProgram, ONNXFileIR, or AIRFileIR)
            output_dir: Output directory

        Returns:
            Dict with paths to generated files:
            - "library": Path to .so file
            - "config": Path to config file
            - "source": Path to .cpp file
        """
        # Step 1: Compile IR to C++ code
        cpp_path = self.backend.compile_to_lib(ir, output_dir)

        # Step 2: Compile C++ to shared library
        so_path = self._compile_cpp_to_so(cpp_path, output_dir)

        return {
            "library": so_path,
            "source": cpp_path,
            "config": getattr(ir, "_config_path", None)
        }

    def _compile_cpp_to_so(self, cpp_path: str, output_dir: str) -> str:
        """Compile C++ source to shared library."""
        # Build g++ command via backend
        so_path = cpp_path.replace(".cpp", ".so")
        cmd = self.backend.build_command(cpp_path, so_path, ...)

        subprocess.run(cmd, check=True)
        return so_path
```

## Decorators API

The decorators layer provides user-friendly API for FHE compilation.

**Files**: `decorators.py`

### @compile Decorator

```python
def compile(
    frontend: str = "torch-via-onnx",
    backend: str = "antlib",
    device: str = "cpu",
    **kwargs
) -> Callable:
    """FHE compilation decorator.

    Args:
        frontend: Frontend strategy name
        backend: Backend name
        device: Device name
        **kwargs: Configuration options (see CompileOptions)

    Returns:
        A compiled FHE program object

    Example:
        @compile(encrypt_inputs=["x"])
        def square(x):
            return x * x

        prog = square.compile([input_tensor])
    """
    def decorator(target):
        # Create compiler
        compiler = Driver(
            frontend_name=frontend,
            backend_name=backend,
            backend_config={"device": device}
        )

        # Attach compile method
        def compile_method(example_inputs):
            return compiler.compile(target, example_inputs, ...)

        target.compile = compile_method
        target._fhe_compiler = compiler

        return target

    return decorator
```

### @compute Decorator

```python
def compute(
    frontend: str = "torch-via-onnx",
    backend: str = "antlib",
    device: str = "cpu",
    **kwargs
) -> Callable:
    """FHE compilation and immediate execution decorator.

    The decorated function runs in FHE transparently:
    - Inputs are automatically encrypted
    - Computation is performed homomorphically
    - Output is decrypted and returned

    Example:
        @compute(encrypt_inputs=[0])
        def add(a, b):
            return a + b

        result = add(input0, input1)  # Runs in FHE
    """
    def decorator(target):
        compiler = Driver(...)

        @functools.wraps(target)
        def wrapper(*args, **kwargs):
            # Compile
            package = compiler.compile(target, list(args), ...)

            # Create runtime
            runner = FHERuntime(package)

            # Run inference
            result = runner.inference(*args)

            return result

        return wrapper

    return decorator
```

## Error Handling

```python
class FHECompilationError(Exception):
    """Base exception for FHE compilation errors."""
    pass

class FrontendNotFoundError(FHECompilationError):
    """Raised when frontend is not found in registry."""
    pass

class BackendNotFoundError(FHECompilationError):
    """Raised when backend is not found in registry."""
    pass

class IRFormatError(FHECompilationError):
    """Raised when IR format is not supported."""
    pass
```

## Configuration Options

```python
@dataclass
class CompileOptions:
    """Options for FHE compilation."""
    encrypt_inputs: Optional[List[Union[str, int]]] = None
    output_dir: Optional[str] = None
    keep_intermediate: bool = False
    verbose: bool = False

@dataclass
class ComputeOptions(CompileOptions):
    """Options for FHE compute (compile + run)."""
    validate: bool = True
    compare_plaintext: bool = False
```

## Usage Examples

### Basic Compilation

```python
from ace.fhe.driver import Driver

# Create compiler
compiler = Driver(
    frontend_name="torch-via-onnx",
    backend_name="antlib",
    backend_config={"device": "cpu"}
)

# Compile model
ir = compiler.compile(model, [example_input], ["x"])

# Build library
result = compiler.build(ir, output_dir="./output")
```

### Using Decorators

```python
from ace import fhe

@fhe.compile(frontend="torch-via-onnx", library="antlib", encrypt_inputs=["x"])
def process(x):
    return x * 2

# Compile
prog = process.compile([example_input])

# Use with runtime
runner = fhe.JITRunner(prog)
result = runner.inference(encrypted_input)
```

### Direct Frontend/Backend Access

```python
from ace.fhe.frontend import get_frontend
from ace.fhe.backend import get_backend_strategy

# Get frontend directly
frontend = get_frontend("onnx")
ir = frontend.compile("model.onnx")

# Get backend directly
backend = get_backend_strategy("antlib", {"device": "cpu"})
lib_path = backend.compile_to_lib(ir, "./output")
```