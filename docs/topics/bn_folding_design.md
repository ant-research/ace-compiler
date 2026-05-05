# BatchNorm Folding Design Document

## Overview

This document describes the BatchNorm folding strategy in the Torch Frontend for FHE compilation.

## Background

### The Problem

PyTorch models commonly use BatchNormalization layers after Conv2d/Linear layers:

```python
class ResNetBlock(nn.Module):
    def __init__(self):
        self.conv1 = nn.Conv2d(64, 64, 3, padding=1)
        self.bn1 = nn.BatchNorm2d(64)
        self.relu = nn.ReLU()
    
    def forward(self, x):
        out = self.conv1(x)
        out = self.bn1(out)
        out = self.relu(out)
        return out
```

In **evaluation mode** (`model.eval()`), BatchNorm uses fixed running statistics:
- `running_mean (μ)` - accumulated mean during training
- `running_var (σ²)` - accumulated variance during training

The BN computation becomes a **linear transformation**:
```
y = γ * (x - μ) / √(σ² + ε) + β
```

Where:
- `γ` (weight) and `β` (bias) are learned parameters
- `μ` and `σ²` are fixed constants
- `ε` is a small constant for numerical stability

### Why Fold BN?

**1. FHE Inefficiency**

FHE (Fully Homomorphic Encryption) cannot efficiently compute:
- Division operations (`/ √(σ² + ε)`)
- Square root operations
- Non-polynomial functions

**2. Redundant Computation**

Since BN parameters are known constants at inference time, there's no need to perform this computation separately - it can be **pre-computed** and merged into the preceding layer.

**3. ONNX Compatibility**

When exporting to ONNX in eval mode, PyTorch **automatically folds** BatchNorm into Conv:

```python
model.eval()
torch.onnx.export(model, ...)  # BN is auto-folded
```

Our direct FX-based frontend should match this behavior.

---

## Solution: BN Folding

### Mathematical Formulation

#### Conv2d + BatchNorm2d

Given:
```
# Conv output
y = conv(x, W, b=0)

# BN output  
z = γ * (y - μ) / √(σ² + ε) + β
```

Folded computation:
```
# Pre-compute scaling factor
scale = γ / √(σ² + ε)

# Fold weights
W' = W * scale.view(-1, 1, 1, 1)

# Fold bias
b' = (b - μ) * scale + β
     = -μ * scale + β  (when b=0)

# Result
z = conv(x, W', b')
```

#### Linear + BatchNorm1d

Same formula, different tensor shapes:
```
# scale = γ / √(σ² + ε)  [shape: (out_features,)]
W' = W * scale.view(-1, 1)  # [out_features, in_features]
b' = (b - μ) * scale + β
```

---

## Implementation

### Architecture

```
python/ace/fhe/frontend/
├── bn_folding.py          # BN folding pass implementation
├── torch_frontend.py      # Main frontend (integrates bn_folding)
└── torch_ops.py           # Custom operator definitions
```

### Pipeline

```
┌─────────────────────────────────────────────────────────────┐
│                    PyTorch Model                            │
│              (Conv -> BN -> ReLU blocks)                    │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│              prepare_model_for_fhe()                        │
│  1. model.eval() - Set evaluation mode                     │
│  2. fuse_modules() - Pattern matching & fusion             │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                    Fused Model                              │
│              (Conv -> ReLU blocks only)                     │
│   - BN parameters absorbed into Conv weights/bias           │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│              fx.symbolic_trace()                            │
│  - Trace fused model to FX GraphModule                     │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│              rewrite_graph_to_custom_ops()                  │
│  - Replace torch ops with custom tensor ops                │
│  - Custom ops generate AIR IR when executed                │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                    AIR IR Generation                        │
│  - Execute traced model with encrypted inputs              │
│  - Custom ops emit AIR operations to IRBuilder             │
└─────────────────────────────────────────────────────────────┘
```

### Supported Patterns

| Pattern | Modules | Fused To |
|---------|---------|----------|
| Conv-BN | `Conv2d -> BatchNorm2d` | `Conv2d` (folded) |
| Conv-BN-ReLU | `Conv2d -> BatchNorm2d -> ReLU` | `Conv2d (folded) -> ReLU` |
| Linear-BN | `Linear -> BatchNorm1d` | `Linear` (folded) |

### Code Example

```python
from .bn_folding import fuse_conv_bn, fuse_modules

# Pattern 1: Single Conv-BN fusion
conv = nn.Conv2d(64, 64, 3, padding=1, bias=False)
bn = nn.BatchNorm2d(64)
fused_conv = fuse_conv_bn(conv, bn)

# Pattern 2: Full model fusion
model = ResNet20()
model.eval()
fused_model = fuse_modules(model)
# All Conv->BN sequences are now fused
```

---

## Comparison: ONNX Path vs Direct Frontend

### ONNX Path (torch-via-onnx)

```
PyTorch Model (eval mode)
       │
       ▼
torch.onnx.export()
  - Auto-folds BN during export
       │
       ▼
ONNX Model (no BN nodes)
       │
       ▼
onnx2air
  - Converts ONNX ops to AIR
       │
       ▼
AIR IR
```

### Direct Frontend (torch)

```
PyTorch Model
       │
       ▼
prepare_model_for_fhe()
  - Explicit BN folding
       │
       ▼
Fused Model (no BN modules)
       │
       ▼
fx.symbolic_trace()
       │
       ▼
FX GraphModule
       │
       ▼
rewrite_graph_to_custom_ops()
       │
       ▼
Custom Op Graph
       │
       ▼
Execute with inputs → AIR IR
```

### Key Differences

| Aspect | ONNX Path | Direct Frontend |
|--------|-----------|-----------------|
| BN Folding | Automatic (in torch.onnx.export) | Explicit (in prepare_model_for_fhe) |
| IR Generation | onnx2air (C++ parser) | Custom ops (Python → C++) |
| Control | Black box | Full control in Python |
| Extensibility | Limited | Easy to add new patterns |

---

## Benefits

### 1. Consistency with ONNX

Both frontends now produce equivalent AIR IR:
- Same folded Conv parameters
- Same operator sequence
- Same lowering requirements

### 2. No Lowering Changes Required

By folding BN **before** IR generation:
- Lowering passes see only Conv/Linear/ReLU
- No need to implement BN lowering
- No need to modify existing optimization passes

### 3. FHE Efficiency

Folded computation reduces:
- Number of operations (fewer IR nodes)
- Multiplicative depth (no separate BN scaling)
- Runtime overhead

### 4. Extensibility

The pattern-based design makes it easy to add:
- Conv1d + BatchNorm1d
- Conv3d + BatchNorm3d
- InstanceNorm folding
- LayerNorm folding (for transformers)

---

## Testing

### Unit Tests

```python
def test_fuse_conv_bn():
    conv = nn.Conv2d(3, 16, 3, padding=1, bias=False)
    bn = nn.BatchNorm2d(16)
    
    # Set BN running stats to known values
    bn.running_mean.fill_(0.5)
    bn.running_var.fill_(1.0)
    bn.weight.fill_(2.0)
    bn.bias.fill_(0.0)
    
    fused = fuse_conv_bn(conv, bn)
    
    # Verify folded weights
    expected_scale = 2.0 / sqrt(1.0 + 1e-5)
    assert torch.allclose(fused.weight, conv.weight * expected_scale)
```

### Integration Tests

```python
def test_resnet20_folding():
    model = ResNet20()
    model.eval()
    
    # Count BN modules before folding
    bn_count_before = sum(
        isinstance(m, nn.BatchNorm2d) 
        for m in model.modules()
    )
    assert bn_count_before > 0
    
    # Fold
    fused_model = fuse_modules(model)
    
    # Count BN modules after folding
    bn_count_after = sum(
        isinstance(m, nn.BatchNorm2d) 
        for m in fused_model.modules()
    )
    assert bn_count_after == 0
    
    # Verify output equivalence
    x = torch.randn(1, 3, 32, 32)
    with torch.no_grad():
        y1 = model(x)
        y2 = fused_model(x)
    assert torch.allclose(y1, y2, rtol=1e-4)
```

---

## Future Extensions

### 1. Quantization-Aware Folding

For models that will be quantized:
```python
# Fold with quantization parameters
fused_conv = fuse_conv_bn_quant(conv, bn, qparams)
```

### 2. Training Mode Support

Currently only supports eval mode. For training:
```python
# Keep BN separate but mark for later folding
model = prepare_model_for_fhe(model, mode='training')
# BN remains, but metadata indicates it should be folded
```

### 3. Additional Patterns

```python
# Conv + GroupNorm (for vision transformers)
class ConvGnPattern(ModulePattern):
    match_types = (nn.Conv2d, nn.GroupNorm)

# Linear + LayerNorm (for language models)
class LinearLnPattern(ModulePattern):
    match_types = (nn.Linear, nn.LayerNorm)
```

---

## References

- [PyTorch ONNX Export - BatchNorm Handling](https://pytorch.org/docs/stable/onnx.html)
- [torch.ao.quantization.fuse_modules](https://pytorch.org/docs/stable/ao_quantization.html)
- [FHE-friendly Neural Network Operators](https://eprint.iacr.org/2021/1577.pdf)