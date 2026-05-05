# Torch Frontend Passes Refactor

## Background

`torch_frontend.py` has too many responsibilities mixed together:
- Model preprocessing (BN folding)
- FX graph transformation (custom op replacement)
- Constant extraction
- Frontend framework API

## Refactor Goals

Separate different responsibilities into independent pass modules, making `torch_frontend.py` only contain the framework API.

## New Architecture

```
python/ace/fhe/frontend/torch/
├── torch_frontend.py      # High-level API (prepare/compile/export)
├── torch_ops/             # Custom operator mapping
│   ├── __init__.py
│   └── tensor.py
└── passes/                # NEW: Transformation passes (class-encapsulated)
    ├── __init__.py
    ├── model_prepare.py        # Model prepare pass (ModelPreparePass class)
    ├── graph_transform.py      # FX graph transform pass (GraphTransformPass class)
    └── constant_extraction.py  # Constant extraction pass (ConstantExtractionPass class)
```

## Pass Layers

```
┌─────────────────────────────────────────────────────────────┐
│                    TorchFrontend                            │
│  (High-level API: prepare/compile/export)                   │
└─────────────────────────────────────────────────────────────┘
                              │
            ┌─────────────────┼─────────────────┐
            ▼                 ▼                 ▼
┌───────────────────┐ ┌─────────────────┐ ┌─────────────────┐
│ ModelPreparePass  │ │GraphTransformPass│ │ConstantExtractionPass│
│  (Model-level     │ │  (FX Graph      │ │  (Constant      │
│   preprocessing)  │ │   Transform)    │ │   Extraction)   │
│  - eval mode      │ │  - remove_id    │ │  - get_attr     │
│  - fuse_bn        │ │  - rewrite_ops  │ │  - tensor meta  │
└───────────────────┘ └─────────────────┘ └───────────────────┘
```

## Pass Class Design

### ModelPreparePass

**File:** `passes/model_prepare.py`

```python
class ModelPreparePass:
    """Model preparation pass for FHE compilation."""
    
    def __init__(self, inplace: bool = False):
        """
        Initialize the model prepare pass.
        
        Args:
            inplace: If True, modify the model in-place
        """
        self.inplace = inplace
    
    def apply(self, model: nn.Module) -> nn.Module:
        """Apply model preparation transformations."""
    
    def _fuse_batchnorm(self, model: nn.Module) -> nn.Module:
        """Fuse BatchNorm layers into preceding Conv/Linear layers."""
    
    def _fuse_conv_bn(self, conv: nn.Conv2d, bn: nn.BatchNorm2d):
        """Fuse Conv2d + BatchNorm2d into a single Conv2d."""
    
    def _fuse_linear_bn(self, linear: nn.Linear, bn: nn.BatchNorm1d):
        """Fuse Linear + BatchNorm1d into a single Linear."""


# Convenience function
def prepare_model_for_fhe(model: nn.Module, inplace: bool = False) -> nn.Module:
    """Prepare a PyTorch model for FHE compilation."""
```

**Key Features:**
- Sets model to eval mode (disables dropout, fixes BN statistics)
- Fuses BatchNorm parameters into preceding Conv/Linear layers
- Replaces folded BN modules with nn.Identity
- Handles both Conv-BN and Linear-BN pairs

---

### GraphTransformPass

**File:** `passes/graph_transform.py`

```python
class GraphTransformPass:
    """Graph transformation pass for FHE compilation."""
    
    def __init__(self):
        """Initialize the graph transform pass."""
        self.rewritten_count = 0
    
    def apply(self, traced_model: torch.fx.GraphModule) -> torch.fx.GraphModule:
        """Apply graph transformations."""
    
    def _remove_identity_nodes(self, traced_model):
        """Remove Identity module calls from FX graph."""
    
    def _rewrite_to_custom_ops(self, traced_model):
        """Rewrite FX graph to use custom tensor ops."""
    
    def _handle_call_function(self, node, graph):
        """Handle call_function nodes."""
    
    def _handle_call_module(self, node, graph, traced_model):
        """Handle call_module nodes (nn.Module instances)."""
    
    def _handle_call_method(self, node, graph, traced_model):
        """Handle call_method nodes (tensor methods)."""
    
    def _filter_tensor_args(self, args) -> tuple:
        """Filter arguments to keep only tensor-producing nodes and scalar constants."""
    
    def _generate_onnx_name(self, node, op_name) -> str:
        """Generate ONNX-style node name."""
    
    def _generate_module_onnx_name(self, node, module_target, op_name) -> str:
        """Generate ONNX-style node name for nn.Module calls."""
    
    def _handle_pool_node(self, node, module, kernel_attr, stride_attr, padding_attr):
        """Handle pooling operation nodes."""
    
    def _handle_gemm_node(self, node, module, traced_model, attr_prefix, graph):
        """Handle GEMM (Linear) operation nodes."""
    
    def _handle_conv_node(self, node, module, traced_model, attr_prefix, graph):
        """Handle Conv2d operation nodes."""
    
    def _handle_reshape_node(self, node, traced_model, graph, method_name):
        """Handle reshape/view operation nodes."""


# Convenience function
def rewrite_graph_to_custom_ops(traced_model):
    """Rewrite FX graph to use custom tensor ops."""
```

**Key Features:**
- Removes Identity nodes (from folded BN)
- Replaces torch functions with custom ops (torch.ops.tensor.xxx)
- Replaces nn.Module calls with custom ops
- Generates ONNX-style node names for debugging
- Handles argument normalization (removes unsupported kwargs)
- Supports Conv, GEMM, Pool, Reshape, and other operations

---

### ConstantExtractionPass

**File:** `passes/constant_extraction.py`

```python
class ConstantExtractionPass:
    """Constant extraction pass for FHE compilation."""
    
    def __init__(self):
        """Initialize the constant extraction pass."""
        self.constants = {}
    
    def apply(self, traced_model, original_model=None) -> Dict[str, Dict[str, Any]]:
        """Extract constants from FX graph get_attr nodes."""
    
    def get_constants(self) -> Dict[str, Dict[str, Any]]:
        """Get extracted constants."""
    
    def get_constant_names(self) -> List[str]:
        """Get names of extracted constants."""


# Convenience function
def get_graph_constants(traced_model, original_model=None):
    """Extract constants from FX graph get_attr nodes."""
```

**Constant Metadata Format:**
```python
{
    'tensor': torch.Tensor,      # Original tensor
    'shape': List[int],          # Tensor shape
    'data': List[float|int],     # Flattened data
    'dtype': str                 # 'float32' or 'int64'
}
```

**Key Features:**
- Extracts weights and biases from Conv/Linear layers
- Extracts shape constants from Reshape/View operations
- Handles both float32 and int64 tensors
- Returns structured metadata for IR generation

## File Changes

### New Files

| File | Responsibility |
|------|----------------|
| `passes/__init__.py` | Unified export for pass modules |
| `passes/model_prepare.py` | `ModelPreparePass` class encapsulating BN folding logic |
| `passes/graph_transform.py` | `GraphTransformPass` class encapsulating graph transform logic |
| `passes/constant_extraction.py` | `ConstantExtractionPass` class encapsulating constant extraction logic |

### Simplified Files

| File | Change |
|------|--------|
| `torch_frontend.py` | Remove pass functions, only keep `TorchFrontend` class, use pass class instances |
| `bn_folding.py` | Deleted, functionality integrated into `model_prepare.py` |

## Import Relationships

```python
# torch_frontend.py imports
from .passes import (
    ModelPreparePass,       # BN folding
    GraphTransformPass,     # Graph transform
    ConstantExtractionPass, # Constant extraction
)

# Or use convenience functions
from .passes import (
    prepare_model_for_fhe,
    rewrite_graph_to_custom_ops,
    get_graph_constants,
)

# External usage
from ace.fhe.frontend.torch import TorchFrontend
from ace.fhe.frontend.torch.passes import ModelPreparePass
```

## Verification

Run tests after refactoring:
```bash
pytest tests/test_unit/test_frontend/test_torch_frontend.py -v
pytest tests/test_regression/test_resnet20_torch.py -v
```

## Future Optimizations

1. Integrate `torch_ops/` into `passes/` directory
2. Add pass composition functionality (pipeline)
3. Support custom pass injection

---

## Pipeline Usage Example

```python
from ace.fhe.frontend.torch.passes import (
    ModelPreparePass,
    GraphTransformPass,
    ConstantExtractionPass,
)

# Create pass instances
model_prepare = ModelPreparePass(inplace=False)
graph_transform = GraphTransformPass()
constant_extraction = ConstantExtractionPass()

# Apply passes in sequence
model = model_prepare.apply(model)
traced_model = fx.symbolic_trace(model)
traced_model = graph_transform.apply(traced_model)
constants = constant_extraction.apply(traced_model, model)
```

## Testing

All tests pass after refactoring:
```bash
# Unit tests
pytest tests/test_unit/test_frontend/test_torch_frontend.py -v
# Result: 38 passed, 1 skipped

# Regression tests
pytest tests/test_regression/test_resnet20_torch.py -v
# Result: ResNet20 compilation successful
```