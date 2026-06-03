# ReLU VR Profiling Design

## Overview

In FHE inference, ReLU is approximated by SIHE polynomial fitting. The fitting accuracy depends on `relu_vr` (Value Range) — the input value range for each ReLU node. If `relu_vr` is too small, actual activations exceed the fitting range and polynomial approximation error explodes; if too large, the scaling factor `1/vr` shrinks, reducing polynomial precision across the range.

This document describes the unified ReLU VR profiling system that replaces the previous ad-hoc approaches (ONNX-based `profile_relu.py` and hook-based `profile_relu_torch.py`) with a single FX Interpreter-based profiler integrated into the compilation pipeline.

## Architecture

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                         ReLU VR Profiling System                            │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌─────────────┐   ┌──────────────────────────────────────────────────┐    │
│  │  ModelSpec   │   │             ReLUProfiler                        │    │
│  │  FuncSpec    │   │                                                  │    │
│  │              │──▶│  Mode 1: load()     → Load from JSON file       │    │
│  │  resolve_    │   │  Mode 2: profile()  → FX Interpreter profiling  │    │
│  │  relu_vr_    │   │  Mode 3: built-in   → Driver._resolve_relu_vr  │    │
│  │  file()      │   │                                                  │    │
│  └─────────────┘   └──────────────────────┬───────────────────────────┘    │
│                                              │                              │
│                                              ▼                              │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                     CompileOptions                                  │   │
│  │  relu_vr_data: Dict[str, float]  ← explicit VR values (highest)    │   │
│  │  relu_vr_file: str               ← JSON file path                  │   │
│  │  profile_relu: bool              ← built-in profiling flag          │   │
│  └──────────────────────────────┬──────────────────────────────────────┘   │
│                                  │                                          │
│                                  ▼                                          │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                     Driver._resolve_relu_vr_data()                  │   │
│  │  Priority: relu_vr_data > relu_vr_file > profile_relu > None       │   │
│  │  Resolved VR data → frontend_kwargs['relu_vr_data']                │   │
│  └──────────────────────────────┬──────────────────────────────────────┘   │
│                                  │                                          │
│                                  ▼                                          │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                     TorchFrontend → torch_trace.py                  │   │
│  │  relu_vr_data injected into AIR IR as relu_vr attribute on ReLU   │   │
│  │  nodes during FX graph → AIR conversion                           │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────────────────┘
```

## Three Profiling Modes

### Mode 1: Load from file (pre-profiled)

Load VR values from a previously saved JSON profile. Best for production use — profile once, reuse across compilations.

```python
from ace import fhe
from ace.fhe.config import CompileOptions, ModelSpec

# Option A: Via CompileOptions
options = CompileOptions(relu_vr_file="profiles/resnet20_cifar10.json")
compiled = fhe.compile(frontend="torch", library="antlib", options=options)(model)

# Option B: Via ModelSpec auto-discovery
spec = ModelSpec(
    name="resnet20_cifar10",
    model_class=ResNet_CIFAR,
    weights_required=True,  # auto-discovers profiles/resnet20_cifar10.json
    ...
)
profiler = ReLUProfiler(spec)
vr_data = profiler.load()  # loads from spec.get_vr_profile()
```

### Mode 2: Pre-pipeline profiling (dataset/specified inputs)

Profile with a full dataset before compilation. Produces the most accurate VR values since it covers many samples.

```python
from ace.fhe.config import ReLUProfiler, ModelSpec

spec = RESNET20_CIFAR10  # pre-defined spec
profiler = ReLUProfiler(spec)

# Profile with full CIFAR-10 test set (10000 images)
vr_data = profiler.profile(margin=1, save=True)

# Or profile with specific inputs
images = load_cifar10_images(100)
vr_data = profiler.profile(inputs=images, margin=1)

# Use the result in compilation
options = CompileOptions(relu_vr_data=vr_data)
compiled = fhe.compile(frontend="torch", library="antlib", options=options)(model)
```

### Mode 3: Built-in profiling (compile-time)

Profile during compilation using the same `example_inputs` passed to `compile()`. Convenient but only profiles a single batch — less accurate than Mode 2 for production.

```python
from ace import fhe
from ace.fhe.config import CompileOptions

options = CompileOptions(profile_relu=True)
compiled = fhe.compile(
    frontend="torch",
    library="antlib",
    options=options,
)(model)

# Or via Driver directly
driver = Driver(frontend="torch", library="antlib", options=options)
driver.compile(model, example_inputs)
```

### Priority Resolution

When multiple sources are specified, `Driver._resolve_relu_vr_data()` resolves in priority order:

| Priority | Source | Field | Use Case |
|----------|--------|-------|----------|
| 1 (highest) | Explicit dict | `CompileOptions.relu_vr_data` | Override specific values |
| 2 | JSON file | `CompileOptions.relu_vr_file` | Production: pre-profiled file |
| 3 | Built-in profiling | `CompileOptions.profile_relu` | Development: quick iteration |
| 4 (lowest) | None | — | Fallback to CLI defaults |

## FX Interpreter-Based Profiling

### Why FX Interpreter (not hooks)

The previous hook-based approach (`register_forward_hook`) has a fundamental limitation: it profiles per **module instance**, not per **call site**. In ResNet's `BasicBlock`, a single `self.relu` module is called twice in `forward()`:

```python
class BasicBlock(nn.Module):
    def __init__(self, ...):
        self.relu = nn.ReLU()  # one module instance

    def forward(self, x):
        out = self.relu(self.bn1(self.conv1(x)))  # call 1
        out = self.relu(out + identity)            # call 2 (same module!)
        return out
```

Hook-based profiling sees 10 ReLU modules for ResNet-20 (one per `BasicBlock` plus the stem `relu`), capturing the **combined** `abs_max` of both calls. This doesn't match the AIR IR, which has 19 ReLU nodes (one per call site).

FX Interpreter traces the model's symbolic graph and creates separate nodes for each call:

```
layer1_0_relu     → call_module (1st call)
layer1_0_relu_1   → call_module (2nd call, FX adds _1 suffix)
```

This produces per-call-site VR values that match AIR IR node names exactly.

### AIR Node Naming Convention

FX node names follow the pattern `{fx_node_name}_{OpType}`:

| FX Node Name | AIR IR Name | Meaning |
|---|---|---|
| `relu` | `relu_Relu` | Stem ReLU |
| `layer1_0_relu` | `layer1_0_relu_Relu` | BasicBlock 1st ReLU call |
| `layer1_0_relu_1` | `layer1_0_relu_1_Relu` | BasicBlock 2nd ReLU call |

The `_Relu` suffix is the AIR OpType, applied consistently to all operations (e.g., `conv1_Conv`, `bn1_BatchNorm`, `add_Add`). This is not specific to ReLU — it's how AIR names all operations.

### Profile Result Format

```python
# ReLUProfiler.profile() returns a structured result:
{
    "relu_vr_def": 3,                    # default VR for unlisted nodes
    "relu_vr": "relu_Relu=3;layer1_0_relu_Relu=3;...",  # CLI-format string
    "per_node": {
        "relu_Relu": {"abs_max": 2.597, "vr": 3},
        "layer1_0_relu_Relu": {"abs_max": 2.400, "vr": 3},
        "layer1_0_relu_1_Relu": {"abs_max": 3.640, "vr": 4},
        ...
    }
}

# _vr_result_to_flat_dict() converts to flat dict for AIR IR embedding:
{
    "relu_Relu": 3.0,
    "layer1_0_relu_Relu": 3.0,
    "layer1_0_relu_1_Relu": 4.0,
    ...
}
```

### VR Calculation

```
VR = ceil(abs_max) + margin    if abs_max > 0
VR = relu_vr_def               if abs_max == 0
```

- `abs_max`: Maximum absolute value of the **pre-ReLU input** across all samples for that ReLU call site. Tracking the input (not the post-ReLU output) is critical because the polynomial approximation must cover the full input interval `[-abs_max, abs_max]`. The post-ReLU output `max(0, x)` clips negative values, so tracking the output would underestimate the range when inputs have large negative activations.
- `margin`: Safety margin (default 1) to cover values slightly beyond the observed range
- `relu_vr_def`: Default VR for ReLU nodes with zero activation (default 3)

## ModelSpec and FuncSpec

### ModelSpec

Unified descriptor for nn.Module compilation targets, replacing `ModelTestCase` and `CompileSpec.ModelEntity`.

```python
@dataclass
class ModelSpec:
    name: str                                    # "resnet20_cifar10"
    model_class: type                            # ResNet_CIFAR
    example_inputs: Tuple[torch.Tensor, ...]     # (torch.randn(1, 3, 32, 32),)
    encrypt_inputs: List[str]                    # ["x"]
    model_init_args: tuple = ()                  # positional args for constructor
    model_init_kwargs: dict = {}                 # keyword args: {"n_layers": 20}
    model_post_init: Optional[Callable] = None   # callback for weight loading
    compile_options: Optional[Dict] = None       # {"sihe": {"relu_vr_def": 3}, ...}
    expected_ops: Optional[List[str]] = None     # ["Conv", "Relu", "Add"]
    weights_required: bool = False               # triggers VR file auto-discovery
    dataset: Optional[str] = None                # "cifar10", "cifar100"
    relu_vr_file: Optional[str] = None           # explicit path to VR profile
```

Key methods:
- `create_model()` — instantiate `model_class(*args, **kwargs)`, then call `model_post_init(model)` if set
- `get_vr_profile()` — return `self.relu_vr_file` if set, else auto-discover `profiles/{name}.json` if `weights_required=True`

### FuncSpec

Unified descriptor for Python function compilation targets, replacing `FunctionTestCase` and `CompileSpec.FuncEntity`.

```python
@dataclass
class FuncSpec:
    name: str                                    # "add_func"
    func: Callable                               # the function itself
    example_inputs: Tuple[torch.Tensor, ...]     # (torch.randn(1, 10),)
    compile_options: Optional[Dict] = None
    expected_ops: Optional[List[str]] = None
```

## ReLUProfiler API

```python
class ReLUProfiler:
    """ReLU Value Range profiler for FHE polynomial approximation."""

    def __init__(self, model_spec: ModelSpec):
        """Initialize with a ModelSpec."""

    def load(self) -> Dict[str, float]:
        """Mode 1: Load pre-profiled VR from file.

        Uses model_spec.get_vr_profile() to find the profile JSON.
        Returns flat dict: {"relu_Relu": 3.0, "layer1_0_relu_Relu": 4.0, ...}
        """

    def profile(self, inputs=None, margin=1, save=False) -> Dict[str, float]:
        """Mode 2: Pre-pipeline profiling with dataset or specified inputs.

        Uses FX Interpreter to profile per-call-site ReLU activation ranges.

        Args:
            inputs: Input tensor (N, C, H, W). If None, loads from spec.dataset.
            margin: Safety margin added to ceil(abs_max). Default 1.
            save: If True, save result to relu_vr_file (or auto-discovered path).

        Returns flat dict for AIR IR embedding.
        """
```

### Module-Level Functions

```python
def profile_relu_vr_fx(model, inputs, margin=1, relu_vr_def=3) -> Dict:
    """Profile ReLU VR using FX Interpreter (primary method).

    Returns structured result with relu_vr_def, relu_vr string, and per_node dict.
    """

def _profile_relu_vr_hooks(model, images, margin=1, relu_vr_def=3) -> Dict:
    """Profile ReLU VR using forward hooks (fallback when FX trace fails).

    Produces per-module VR values (10 for ResNet-20) instead of per-call-site (19).
    """

def _vr_result_to_flat_dict(result: Dict) -> Dict[str, float]:
    """Convert structured profile result to flat dict for AIR IR embedding."""

def _load_vr_file(path: str) -> Dict[str, float]:
    """Load VR profile from JSON file and return flat dict."""

def _save_vr_file(result: Dict, path: str) -> None:
    """Save profile result to JSON file."""
```

## Integration with Compilation Pipeline

### Data Flow

```
User specifies VR data
        │
        ▼
CompileOptions (relu_vr_data / relu_vr_file / profile_relu)
        │
        ▼
Driver._resolve_relu_vr_data(source, input_tensors)
        │
        ├── Priority 1: options.relu_vr_data (explicit dict)
        ├── Priority 2: options.relu_vr_file → _load_vr_file()
        └── Priority 3: options.profile_relu → _profile_relu_builtin()
        │
        ▼
Dict[str, float] or None
        │
        ▼
TorchFrontend.to_ir(..., relu_vr_data=vr_data)
        │
        ▼
torch_trace.py: inject relu_vr attribute on ReLU AIR nodes
        │
        ├── Exact name match: layer1_0_relu_1_Relu → vr_value
        └── Fallback: strip _1 suffix → layer1_0_relu_Relu → vr_value
        │
        ▼
AIR IR with embedded relu_vr attributes
        │
        ▼
fhe_cmplr uses relu_vr for SIHE polynomial fitting
```

### Built-in Profiling (Mode 3) Implementation

`Driver._profile_relu_builtin()` in `driver.py`:

1. Unwrap `model._original_model` if present (e.g., DataParallel wrapper)
2. `model.eval()` and `fx.symbolic_trace(model)`
3. Find ReLU nodes: `call_module` (nn.ReLU) and `call_function` (torch.relu)
4. Create `ReLUTracker(fx.Interpreter)` that tracks `abs_max` per ReLU call site
5. Run `example_inputs[0]` through the tracker
6. Compute `VR = ceil(abs_max) + margin` (or `relu_vr_def=3` if zero)
7. Return flat dict via `_vr_result_to_flat_dict()`

This runs only on the first example input (not a full dataset), so it's suitable for development iteration but less accurate than pre-pipeline profiling (Mode 2) for production.

## File Index

| File | Description |
|------|-------------|
| `fhe_dsl/python/fhe/config/spec.py` | `ModelSpec`, `FuncSpec` dataclasses |
| `fhe_dsl/python/fhe/config/profiler.py` | `ReLUProfiler`, `profile_relu_vr_fx()`, helper functions |
| `fhe_dsl/python/fhe/config/compile_options.py` | `CompileOptions` with `relu_vr_data`, `relu_vr_file`, `profile_relu` |
| `fhe_dsl/python/fhe/config/__init__.py` | Exports: `ModelSpec`, `FuncSpec`, `ReLUProfiler` |
| `fhe_dsl/python/fhe/driver/driver.py` | `Driver._resolve_relu_vr_data()`, `_profile_relu_builtin()` |
| `fhe_dsl/python/fhe/frontend/torch/torch_frontend.py` | Passes `relu_vr_data` to `TorchTracedModel` |
| `fhe_dsl/python/fhe/ir/frontends/torch/torch_trace.py` | Injects `relu_vr` attribute on ReLU AIR nodes |
| `fhe_dsl/python/model/resnet/specs.py` | ResNet ModelSpec instances |
| `fhe_dsl/python/sample/ops/specs.py` | Op ModelSpec instances |
| `fhe_dsl/python/sample/funcs/specs.py` | FuncSpec instances |

## Migration from Old Systems

| Old API | New API | Notes |
|---------|---------|-------|
| `ModelTestCase` / `FunctionTestCase` | `ModelSpec` / `FuncSpec` | test_cases backward-compat shim wraps new classes |
| `CompileSpec` + `ModelEntity` | `ModelSpec` | `spec.compile.input_spec` → `spec.example_inputs` |
| `CompileSpec` + `FuncEntity` | `FuncSpec` | `spec.entity.func` → `spec.func` |
| `spec.create()` | `spec.create_model()` | Direct instantiation |
| `profile_relu_vr_torch()` (hooks) | `ReLUProfiler.profile()` | FX Interpreter, per-call-site |
| `profile_relu_vr()` (ONNX) | `ReLUProfiler.profile()` | FX Interpreter replaces ONNX approach |
| `ace.model.resnet.config` VR strings | `profiles/*.json` files | JSON format with per-node VR data |
| `sihe.relu_vr` CLI string | `relu_vr_data` dict | Driver converts dict → CLI args via `_dict_to_cmd_args()` |

## Comparison: FX Interpreter vs Hook-Based Profiling

| Dimension | Hook-Based (old) | FX Interpreter (new) |
|-----------|-------------------|----------------------|
| Granularity | Per module instance | Per call site |
| ResNet-20 nodes | 10 | 19 |
| AIR IR match | No (combined abs_max) | Yes (exact name match) |
| Shared ReLU handling | Combined max of both calls | Separate VR per call |
| Fallback | N/A | Hooks used if FX trace fails |
| Naming | Module path: `layer1.0.relu` | FX node: `layer1_0_relu_1` |
| AIR name | Custom `name_fn` mapping | `{fx_node_name}_Relu` (automatic) |

## ResNet-20 Example

FX Interpreter profiling of ResNet-20 on CIFAR-10 produces 19 ReLU nodes:

```
Node                          abs_max    VR
relu_Relu                      2.597     3
layer1_0_relu_Relu             2.400     3
layer1_0_relu_1_Relu           3.640     4
layer1_1_relu_Relu             2.381     3
layer1_1_relu_1_Relu           5.497     6
layer1_2_relu_Relu             2.101     3
layer1_2_relu_1_Relu           5.508     6
layer2_0_relu_Relu             3.091     4
layer2_0_relu_1_Relu           4.539     5
layer2_1_relu_Relu             1.758     3
layer2_1_relu_1_Relu           4.855     5
layer2_2_relu_Relu             2.684     3
layer2_2_relu_1_Relu           7.193     8
layer3_0_relu_Relu             2.667     3
layer3_0_relu_1_Relu           4.109     5
layer3_1_relu_Relu             3.012     4
layer3_1_relu_1_Relu           7.033     8
layer3_2_relu_Relu             2.195     3
layer3_2_relu_1_Relu          18.679    19
```

Key observations:
- `_relu` nodes (after BN1) have smaller VR (3-4) than `_relu_1` nodes (after residual add, 4-19)
- VR increases with network depth — layer3's last ReLU has VR=19
- The `_1` suffix is FX's disambiguation for the second call to a shared ReLU module