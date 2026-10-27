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
compiled = fhe.compile(frontend="torch", library="ant", options=options)(model)

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
compiled = fhe.compile(frontend="torch", library="ant", options=options)(model)
```

### Mode 3: Built-in profiling (compile-time)

Profile during compilation using the same `example_inputs` passed to `compile()`. Convenient but only profiles a single batch — less accurate than Mode 2 for production.

```python
from ace import fhe
from ace.fhe.config import CompileOptions

options = CompileOptions(profile_relu=True)
compiled = fhe.compile(
    frontend="torch",
    library="ant",
    options=options,
)(model)

# Or via Driver directly
driver = Driver(frontend="torch", library="ant", options=options)
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

## ReLU VR and FHE Inference Accuracy

Correct `relu_vr` is a **necessary** but not **sufficient** condition for
plaintext/ciphertext inference agreement. Even with exact VR coverage and a
bug-free compiler, results may still differ.

### How VR Affects FHE Accuracy

In FHE, ReLU is approximated by SIHE polynomial fitting. Multiple error sources
stack up:

1. **Polynomial fitting error**: Even within `[-vr, vr]`, the polynomial
   approximation (e.g., Chebyshev) has bounded error. Higher polynomial degree
   reduces error but is limited by CKKS multiplication depth.
2. **CKKS encoding noise**: Each homomorphic operation introduces encryption
   noise that accumulates across dozens of conv+ReLU layers.
3. **Error amplification across layers**: Small errors in shallow ReLU layers
   get amplified by subsequent convolution weights. Deeper networks suffer more.

| `relu_vr` setting | Consequence |
|---|---|
| **Too small** (activations exceed range) | Polynomial diverges outside range — **catastrophic error** |
| **Exact coverage** | Polynomial fits within range, but approximation + CKKS noise may cause **small shifts**; borderline samples may flip |
| **Too large** | Scaling factor `1/vr` shrinks, compressing input to narrower `[-1,1]` interval — **reduced polynomial precision** |

Final agreement rate is determined by:

| Factor | Impact |
|---|---|
| Polynomial degree | Higher → more accurate ReLU, but consumes more multiplication depth |
| CKKS parameters (sf, N, q0) | Higher sf → more precision but less multiplication depth; N/q0 affect noise budget |
| Network depth | Deeper → more error accumulation |
| `relu_vr` precision | Exact VR avoids out-of-range divergence and over-compression |
| Activation distribution | Samples near argmax decision boundary are more likely to flip from small errors |

**Conclusion**: Profiling exact `relu_vr` is only the first step — it eliminates
the largest error source (out-of-range divergence), but achieving high
plaintext/ciphertext agreement also requires proper polynomial degree and CKKS
parameter configuration.

### Normalization Parameter Correction

#### Mismatch Issue

ResNet-20 pretrained weights come from
[chenyaofo/pytorch-cifar-models](https://github.com/chenyaofo/pytorch-cifar-models),
which uses CIFAR-10's own statistics during training, not ImageNet's:

| Parameter set | mean | std | Source |
|---|---|---|---|
| **CIFAR-10** (training) | [0.4914, 0.4822, 0.4465] | [0.2023, 0.1994, 0.2010] | chenyaofo/pytorch-cifar-models |
| **CIFAR-100** (training) | [0.5070, 0.4865, 0.4409] | [0.2673, 0.2564, 0.2761] | chenyaofo/pytorch-cifar-models |
| ImageNet (previously used) | [0.485, 0.456, 0.406] | [0.229, 0.224, 0.225] | — |

The mismatch between training and inference normalization caused ~2% accuracy
loss. After fixing `cifar10.py` and `cifar100.py` to use CIFAR statistics:

| Condition | Accuracy |
|---|---|
| Training source reported (full 10000) | 92.60% |
| After fix, plaintext (first 1000, CIFAR-10 norm) | 92.5% |
| Before fix, plaintext (ImageNet norm) | ~90.2% |
| Before fix, ciphertext (100, ImageNet norm) | ~88% |

#### Normalization Impact on VR Values

Re-profiling with CIFAR-10 normalization (10000 samples) vs. ImageNet
normalization:

| Node | ImageNet norm VR | CIFAR-10 norm VR | Change |
|---|---|---|---|
| /relu/Relu | 4 | **5** | +1 |
| layer1.0.relu | 4 | 4 | — |
| layer1.0.relu_1 | 7 | 7 | — |
| layer1.1.relu | 4 | 4 | — |
| layer1.1.relu_1 | 7 | 7 | — |
| layer1.2.relu | 4 | 4 | — |
| layer1.2.relu_1 | 7 | 7 | — |
| layer2.0.relu | 5 | 5 | — |
| layer2.0.relu_1 | 6 | 6 | — |
| layer2.1.relu | 3 | 3 | — |
| layer2.1.relu_1 | 6 | **7** | +1 |
| layer2.2.relu | 4 | 4 | — |
| layer2.2.relu_1 | 9 | 9 | — |
| layer3.0.relu | 4 | 4 | — |
| layer3.0.relu_1 | 6 | 6 | — |
| layer3.1.relu | 5 | 5 | — |
| layer3.1.relu_1 | 9 | 9 | — |
| layer3.2.relu | 4 | 4 | — |
| layer3.2.relu_1 | 20 | 20 | — |

2 nodes have larger VR with CIFAR-10 normalization: stem `/relu/Relu` 4→5,
`layer2.1.relu_1` 6→7.

### ONNX Baseline

#### Why ONNX is the Correct Baseline

The FHE compilation path is `Torch → ONNX → AIR`. Ciphertext inference uses
the ONNX model (after BN folding etc.), so **ONNX plaintext inference is the
correct baseline**, not PyTorch plaintext inference.

#### ONNX Model Weight Consistency

Early experiments used a stale ONNX model with inconsistent weights (max_diff
up to 1.3 vs PyTorch). Re-exporting ONNX from current PyTorch weights resolved
this. New ONNX matches PyTorch accuracy exactly (94/100), confirming BN folding
has no impact on inference accuracy.

### Systematic Experiments

#### Experiment Design

| ID | VR config | Normalization | Description |
|---|---|---|---|
| A | profiled (margin=1, CIFAR-10 norm) | CIFAR-10 mean + std | Correct config + auto VR |
| B | Old manual VR | CIFAR-10 mean + std | Auto VR vs manual VR |
| C | profiled (margin=1, ImageNet norm) | ImageNet mean + std | Normalization impact on FHE |
| E | profiled (margin=1, std=0.225) | CIFAR-10 mean + std=0.225 | std sweep |
| F | profiled (margin=1, CIFAR-10 mean + ImageNet std) | CIFAR-10 mean + ImageNet std | Mixed normalization |

#### Ciphertext Results (100 samples)

| ID | VR | Normalization | Ciphertext accuracy | Failed count | Plaintext baseline | FHE overhead |
|---|---|---|---|---|---|---|
| A | profiled(margin=1) | CIFAR-10 (std=0.201) | 86/100 | 14 | 94% | **8%** |
| B | Old manual | CIFAR-10 (std=0.201) | 85/100 | 15 | 94% | **9%** |
| C | profiled(margin=1) | ImageNet | 87/100 | 13 | 91% | **4%** |
| E | profiled(margin=1) | CIFAR-10 mean + std=0.225 | 86/100 | 14 | 94% | **8%** |
| F | profiled(margin=1) | CIFAR-10 mean + ImageNet std | ~86/100 | ~14 | 94% | **~8%** |

#### Key Findings

1. **VR configuration has minimal impact on ciphertext accuracy**: A vs B differ
   by only 1 sample. VR is not the bottleneck.
2. **Normalization is the main FHE overhead driver**: C (ImageNet norm) has only
   4% FHE overhead vs A (CIFAR-10 norm) at 8%. ImageNet's larger std (0.229 vs
   0.202) normalizes inputs to smaller values, narrowing activation ranges and
   improving polynomial fitting.
3. **CIFAR-10 norm: higher plaintext but lower ciphertext**: 94% - 8% = 86%;
   ImageNet norm: lower plaintext but higher ciphertext: 91% - 4% = 87%.
4. **Systematically flipped samples overlap heavily**: A/B/C share failures at
   indices 15, 52, 57, 58, 59, 61, 78, 87 — these are systematic flips from
   FHE error.
5. **E/F groups match A group results**: Adjusting std alone (while keeping
   CIFAR-10 mean) doesn't reduce FHE overhead. C's advantage comes from
   ImageNet's smaller mean (smaller inputs → narrower activation ranges).
6. **Dataset profiling is necessary**: It ensures safe VR coverage, preventing
   polynomial divergence outside range (catastrophic error). But VR itself is
   not the FHE accuracy bottleneck — optimization should focus on normalization
   strategy and CKKS parameters.

### Normalization Parameter Sweep

Fixed mean=CIFAR-10 mean, sweeping unified 3-channel std:

**Plaintext accuracy** (new ONNX, 100 samples):

| std | Accuracy | Failed indices |
|---|---|---|
| 0.250 | 93/100 | 3,15,52,59,61,78,87 |
| 0.225 | 94/100 | 15,37,52,61,78,87 |
| 0.201 (CIFAR-10) | 94/100 | 15,37,52,61,78,87 |
| 0.180 | 93/100 | 15,35,37,52,61,78,87 |

**ReLU abs_max** (10000 samples, PyTorch model, combined relu/relu2):

| Layer | std=0.250 | std=0.225 | std=0.201 | std=0.180 |
|---|---|---|---|---|
| relu | 2.71 | 2.98 | 3.31 | 3.66 |
| layer1.0.relu | 4.86 | 5.25 | 5.71 | 6.20 |
| layer1.1.relu | 5.27 | 5.49 | 5.94 | 6.43 |
| layer1.2.relu | 5.19 | 5.57 | 6.02 | 6.50 |
| layer2.0.relu | 4.30 | 4.58 | 4.91 | 5.24 |
| layer2.1.relu | 4.63 | 4.86 | 5.16 | 5.48 |
| layer2.2.relu | 7.12 | 7.35 | 7.52 | 7.70 |
| layer3.0.relu | 4.05 | 4.12 | 4.20 | 4.23 |
| layer3.1.relu | 6.98 | 7.08 | 7.17 | 7.28 |
| layer3.2.relu | 19.11 | 18.72 | 18.68 | 18.24 |

**VR summary** (margin=1):

| Layer | std=0.250 | std=0.225 | std=0.201 | std=0.180 |
|---|---|---|---|---|
| relu | 4 | 4 | 5 | 5 |
| layer1.0.relu | 6 | 7 | 7 | 8 |
| layer1.1.relu | 7 | 7 | 7 | 8 |
| layer1.2.relu | 7 | 7 | 8 | 8 |
| layer2.0.relu | 6 | 6 | 6 | 7 |
| layer2.1.relu | 6 | 6 | 7 | 7 |
| layer2.2.relu | 9 | 9 | 9 | 9 |
| layer3.0.relu | 6 | 6 | 6 | 6 |
| layer3.1.relu | 8 | 9 | 9 | 9 |
| layer3.2.relu | 21 | 20 | 20 | 20 |
| **VR_sum** | **80** | **81** | **84** | **87** |

Analysis:
- Larger std → smaller activation range → smaller VR_sum → more precise
  polynomial fitting → less FHE error
- std=0.225 matches std=0.201 plaintext accuracy (94%) but reduces VR_sum from
  84 to 81
- std=0.250 drops plaintext by 1% (93%) but VR_sum=80, potentially further
  reducing FHE overhead
- std=0.180 increases VR_sum to 87, worst for FHE — not viable
- However, experiment E (std=0.225) still shows 86% ciphertext accuracy,
  meaning the VR_sum reduction (84→81) is insufficient to significantly improve
  FHE overhead

### Optimization Directions

| Priority | Direction | Expected effect | Complexity |
|---|---|---|---|
| 1 | Per-layer error analysis | Identify error amplification layers | Medium (requires dumping intermediate values) |
| 2 | Increase CKKS sf | Improve per-layer computation precision | Medium (requires recompilation) |
| 3 | ReLU polynomial degree optimization | Use higher-degree polynomials for large-VR nodes | High (requires compiler support) |
| 4 | Normalization strategy optimization | Explore joint mean/std impact on FHE | Low (no compilation chain changes) |

## CPU (ant) vs GPU (phantom) Accuracy Analysis

### Problem

ResNet-20 with identical CKKS parameters (`q0=60, sf=56, N=65536`) and `relu_vr`
configuration:

- **CPU (ant lib)**: ~91.2% accuracy
- **GPU (phantom lib)**: ~86% accuracy
- **Difference**: ~5%

### Confirmed Identical Aspects

Comparing generated `resnet20_cpu.cu` (~7M lines) and `resnet20_gpu.cu` (~7M
lines):

| Dimension | Consistency |
|---|---|
| Network topology | Identical |
| relu_vr values | Identical |
| 1/VR scaling constants | Identical (0.25, 0.142857, 0.2, 0.1667, 0.333, 0.1111, 0.05, 0.015625) |
| Bootstrap call count | Both 17 |
| Bootstrap level/slot parameters | Identical (level=15/18, slot=16384/8192/4096) |
| Weight constants (float32_t) | Identical |
| ReLU polynomial structure | Both 3-polynomial composition |

### Key Differences

#### Difference 1: Bootstrap Implementation (Highest Suspect)

The two libraries use fundamentally different bootstrap algorithms.

**CPU (ant) Bootstrap** (`Eval_bootstrap` in `ant/ckks/src/bootstrap.c`):
- Uses pre-computed `G_coefficients_*` coefficient tables
- Evaluation via `Eval_chebyshev` (Paterson-Stockmeyer algorithm)
- Precisely tracks `_scaling_factor` and `_sf_degree`, never force-sets
- Polynomial selection based on secret key hamming weight
  (SPARSE/UNIFORM/UNIFORM_EVEN)

**GPU (phantom) Bootstrap** (`bootstrap_3` in `phantom-src/src/boot/Bootstrapper.cu`):
- Uses Remez exchange algorithm for online polynomial generation
  (`RemezCos` + `RemezArcsin`)
- Evaluation via `homomorphic_poly_evaluation`
- Force-sets `rtncipher.scale() = final_scale` after bootstrap

Accuracy difference sources:
1. **Polynomial precision**: CPU uses pre-optimized coefficients; GPU generates
   online via Remez — different approximation accuracy
2. **Evaluation algorithm**: CPU's Paterson-Stockmeyer may be better at
   depth-precision tradeoffs
3. **Scale force-reset** (see Difference 2)
4. **17 bootstrap calls** amplify accumulated errors

#### Difference 2: Post-Bootstrap Scale Force-Reset (High Suspect)

GPU bootstrap ends with:
```cpp
rtncipher.scale() = final_scale;  // Force-set to 2^56
```

CPU bootstrap does not force-set scale — it mathematically tracks through
operations and only rescales:
```c
while (Get_ciph_sf_degree(res) > 1) {
    Rescale_ciphertext(res, res, eval);
}
```

**Impact**: After GPU bootstrap, the ciphertext's `scale()` is forced to `2^56`,
but the actual mathematical scale may differ slightly (since CKKS modulus primes
don't exactly equal `2^56`). Subsequent `Mul_const` uses `op1->scale()` to
encode constants, creating a systematic encoding scale mismatch. This accumulates
across 17 bootstrap calls.

#### Difference 3: 1/VR Scaling Implementation (Medium Suspect)

GPU uses `Mul_const` which encodes with `op1->scale()` (potentially inaccurate
after bootstrap). CPU uses `Encode_float` with fixed `scale=1`, plus explicit
`up_scale`/`down_scale` metadata management, and separate c0/c1 rescale.

#### Difference 4: Rescale Implementation (Low Suspect)

Both are mathematically equivalent but use different NTT transform and rounding
implementations. Individual differences are small but may accumulate over 17
bootstrap calls and many convolutions.

### Root Cause Ranking

| Rank | Factor | Impact | Verification |
|---|---|---|---|
| 1 | Bootstrap algorithm difference | Highest | Test without bootstrap: compare CPU/GPU on shallow network |
| 2 | Post-bootstrap scale force-reset | High | Remove `rtncipher.scale() = final_scale`, track actual scale |
| 3 | Mul_const scale encoding bias | Medium | Use fixed `2^56` encoding instead of `op1->scale()` |
| 4 | Rescale rounding difference | Low | Benchmark rescale precision independently |

### Key Source Locations

| File | Location | Description |
|---|---|---|
| `backend/resnet20_cpu.cu` | Full file | CPU-generated FHE code |
| `backend/resnet20_gpu.cu` | Full file | GPU-generated FHE code |
| `rtlib/ant/ckks/src/bootstrap.c:1549` | `Eval_bootstrap` | CPU bootstrap main function |
| `rtlib/ant/ckks/src/chebyshev_impl.c:275` | `Eval_chebyshev` | CPU Chebyshev evaluation |
| `rtlib/ant/ckks/src/evaluator.c:342` | `Rescale_ciphertext` | CPU rescale |
| `rtlib/phantom/src/phantom_lib.cu:213` | `Mul_const` | GPU constant multiplication |
| `rtlib/phantom/src/phantom_lib.cu:279` | `Bootstrap` | GPU bootstrap wrapper |
| `phantom-src/src/boot/Bootstrapper.cu:3105` | `bootstrap_full_3` | GPU bootstrap implementation |
| `phantom-src/src/boot/Bootstrapper.cu:3124` | `rtncipher.scale()=final_scale` | **GPU scale force-reset** |
| `phantom-src/src/boot/ModularReducer.cu:53` | `modular_reduction` | GPU modular reduction (sin/cos + double-angle) |

## Legacy Approaches (Deprecated)

The profiling system described in this document replaces the following deprecated
approaches:

| Old API | New API | Notes |
|---|---|---|
| `profile_relu_vr_torch()` (hooks) | `ReLUProfiler.profile()` | FX Interpreter, per-call-site |
| `profile_relu_vr()` (ONNX) | `ReLUProfiler.profile()` | FX Interpreter replaces ONNX approach |
| `analyze_onnx_relu.py` (random input) | `ReLUProfiler.profile()` | Real data replaces random input |
| `ace.model.resnet.config` VR strings | `profiles/*.json` files | JSON format with per-node VR data |
| `sihe.relu_vr` CLI string | `relu_vr_data` dict | Driver converts dict → CLI args via `_dict_to_cmd_args()` |

Key differences between legacy and current approaches:

| Dimension | Hook-based (old) | FX Interpreter (current) |
|---|---|---|
| Granularity | Per module instance | Per call site |
| ResNet-20 nodes | 10 | 19 |
| AIR IR match | No (combined abs_max) | Yes (exact name match) |
| Shared ReLU handling | Combined max of both calls | Separate VR per call |
| Fallback | N/A | Hooks used if FX trace fails |
| Naming | Module path: `layer1.0.relu` | FX node: `layer1_0_relu_1` |
| AIR name | Custom `name_fn` mapping | `{fx_node_name}_Relu` (automatic) |

> **Note**: The legacy Hook-based approach required splitting `self.relu` into
> `self.relu` and `self.relu2` in `BasicBlock` to profile each call separately.
> The FX Interpreter approach handles this automatically without model
> modifications.
