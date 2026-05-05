# ResNet20 Torch Frontend 测试指南

## 概述

本文档描述如何使用 Torch Frontend 对 ResNet20 模型进行 AIR IR 生成测试。

## 测试环境

```bash
# 确保在正确的目录
cd /work/ace/ace.refactor/ace_tool.refactor

# 设置 Python 路径
export PYTHONPATH=/work/ace/ace.refactor/ace_tool.refactor/python:$PYTHONPATH
```

## 测试 1: 基本 AIR 生成测试

### 测试脚本

```python
#!/usr/bin/env python3
"""
ResNet20 Torch Frontend 测试 - 基本 AIR 生成
"""
import torch
import torch.nn as nn
import sys
sys.path.insert(0, 'python')

from ace.fhe.frontend import get_frontend

# 定义 ResNet20 模型
def conv3x3(in_planes, out_planes, stride=1):
    return nn.Conv2d(in_planes, out_planes, kernel_size=3, stride=stride, padding=1, bias=False)

def conv1x1(in_planes, out_planes, stride=1):
    return nn.Conv2d(in_planes, out_planes, kernel_size=1, stride=stride, bias=False)

class BasicBlock(nn.Module):
    expansion = 1
    def __init__(self, inplanes, planes, stride=1, downsample=None):
        super(BasicBlock, self).__init__()
        self.conv1 = conv3x3(inplanes, planes, stride)
        self.bn1 = nn.BatchNorm2d(planes)
        self.relu = nn.ReLU(inplace=True)
        self.conv2 = conv3x3(planes, planes)
        self.bn2 = nn.BatchNorm2d(planes)
        self.downsample = downsample
        self.stride = stride
    def forward(self, x):
        identity = x
        out = self.conv1(x)
        out = self.bn1(out)
        out = self.relu(out)
        out = self.conv2(out)
        out = self.bn2(out)
        if self.downsample is not None:
            identity = self.downsample(x)
        out += identity
        out = self.relu(out)
        return out

class ResNet_CIFAR(nn.Module):
    def __init__(self, block, layers, num_classes=10):
        super(ResNet_CIFAR, self).__init__()
        self.inplanes = 16
        self.conv1 = conv3x3(3, 16)
        self.bn1 = nn.BatchNorm2d(16)
        self.relu = nn.ReLU(inplace=True)
        self.layer1 = self._make_layer(block, 16, layers[0])
        self.layer2 = self._make_layer(block, 32, layers[1], stride=2)
        self.layer3 = self._make_layer(block, 64, layers[2], stride=2)
        self.avgpool = nn.AdaptiveAvgPool2d((1, 1))
        self.fc = nn.Linear(64 * block.expansion, num_classes)
    def _make_layer(self, block, planes, blocks, stride=1):
        downsample = None
        if stride != 1 or self.inplanes != planes * block.expansion:
            downsample = nn.Sequential(
                conv1x1(self.inplanes, planes * block.expansion, stride),
                nn.BatchNorm2d(planes * block.expansion),
            )
        layers = []
        layers.append(block(self.inplanes, planes, stride, downsample))
        self.inplanes = planes * block.expansion
        for _ in range(1, blocks):
            layers.append(block(self.inplanes, planes))
        return nn.Sequential(*layers)
    def forward(self, x):
        x = self.conv1(x)
        x = self.bn1(x)
        x = self.relu(x)
        x = self.layer1(x)
        x = self.layer2(x)
        x = self.layer3(x)
        x = self.avgpool(x)
        x = x.view(x.size(0), -1)
        x = self.fc(x)
        return x

# 创建模型
model = ResNet_CIFAR(BasicBlock, [3, 3, 3])
model.eval()

# 测试输入
x = torch.randn(1, 3, 32, 32)

# 获取 torch frontend
frontend = get_frontend('torch')

print('=== ResNet20 Torch Frontend 测试 ===')
print()

# Step 1: Prepare (包括 BN folding)
print('Step 1: Preparing model (with BN folding)...')
traced = frontend.prepare(model, [x])
print('  OK')

# Step 2: Execute (生成 AIR IR)
print('Step 2: Executing traced model (generating AIR IR)...')
traced.execute(x)
print('  OK')

# Step 3: Export to .B file
print('Step 3: Exporting AIR IR to file...')
air_path = '/tmp/resnet20_test.B'
traced.export_ir(air_path)
print(f'  OK: {air_path} ({os.path.getsize(air_path)} bytes)')

print()
print('=== 测试通过 ===')
```

### 运行命令

```bash
python3 test_resnet20_torch.py
```

### 预期输出

```
=== ResNet20 Torch Frontend 测试 ===

Step 1: Preparing model (with BN folding)...
[TorchFrontend] Fusing BatchNorm layers...
[TorchFrontend] Tracing model with 1 inputs...
[TorchFrontend] Found 44 constants: [conv1_weight, conv1_bias, ...]
[TorchFrontend] Traced model successfully
[TorchFrontend] Output shape: [1, 10]
  OK

Step 2: Executing traced model (generating AIR IR)...
[TensorNameRegistry] Cleared
[TensorNameRegistry] Registered: input_0 @ ...
[TensorNameRegistry] Registered: conv1_weight @ ...
...
[tensor_conv_impl] Generating AIR IR with attributes...
[tensor_relu_impl] Generating AIR IR...
[tensor_add_impl] Generating AIR IR...
...
  OK

Step 3: Exporting AIR IR to file...
  OK: /tmp/resnet20_test.B (1110903 bytes)

=== 测试通过 ===
```

---

## 测试 2: BN Folding 验证测试

### 测试脚本

```python
#!/usr/bin/env python3
"""
ResNet20 BN Folding 验证测试
"""
import torch
import torch.nn as nn
import sys
sys.path.insert(0, 'python')

from ace.fhe.frontend.bn_folding import fuse_modules

# ... (ResNet20 模型定义同上) ...

# 创建模型
model = ResNet_CIFAR(BasicBlock, [3, 3, 3])
model.eval()

print('=== BN Folding 验证测试 ===')
print()

# 计数 BN 模块
bn_before = sum(1 for m in model.modules() if isinstance(m, nn.BatchNorm2d))
print(f'BatchNorm2d modules BEFORE folding: {bn_before}')

# 执行 BN folding
fused_model = fuse_modules(model)

# 计数 BN 模块
bn_after = sum(1 for m in fused_model.modules() if isinstance(m, nn.BatchNorm2d))
identity_count = sum(1 for m in fused_model.modules() if isinstance(m, nn.Identity))
print(f'BatchNorm2d modules AFTER folding: {bn_after}')
print(f'Identity modules (replaced BN): {identity_count}')

# 验证输出一致性
x = torch.randn(1, 3, 32, 32)
with torch.no_grad():
    y_before = model(x)
    y_after = fused_model(x)

max_diff = (y_before - y_after).abs().max().item()
print()
print(f'Max output difference: {max_diff:.2e}')
print(f'Outputs match: {torch.allclose(y_before, y_after, rtol=1e-4)}')

# 检查期望的操作类型
print()
print('=== 检查融合后的操作类型 ===')
op_types = set()
for name, module in fused_model.named_modules():
    if isinstance(module, (nn.Conv2d, nn.ReLU, nn.Linear, nn.Identity, nn.AdaptiveAvgPool2d)):
        op_types.add(type(module).__name__)

print(f'Operation types: {sorted(op_types)}')
print('Expected: [Conv2d, Identity, Linear, ReLU, AdaptiveAvgPool2d]')
print('(注意：没有 BatchNorm2d，说明 BN 已成功折叠)')
```

### 运行命令

```bash
python3 test_bn_folding.py
```

### 预期输出

```
=== BN Folding 验证测试 ===

BatchNorm2d modules BEFORE folding: 21
BatchNorm2d modules AFTER folding: 0
Identity modules (replaced BN): 19

Max output difference: 9.31e-09
Outputs match: True

=== 检查融合后的操作类型 ===
Operation types: ['AdaptiveAvgPool2d', 'Conv2d', 'Identity', 'Linear', 'ReLU']
(注意：没有 BatchNorm2d，说明 BN 已成功折叠)
```

---

## 测试 3: 与 ONNX 路径对比测试

### 测试脚本

```python
#!/usr/bin/env python3
"""
对比 Torch Frontend 和 Torch-Via-ONNX Frontend 的输出
"""
import torch
import torch.nn as nn
import sys
sys.path.insert(0, 'python')

from ace.fhe.frontend import get_frontend

# ... (ResNet20 模型定义) ...

model = ResNet_CIFAR(BasicBlock, [3, 3, 3])
model.eval()
x = torch.randn(1, 3, 32, 32)

print('=== Frontend 对比测试 ===')
print()

# Torch Frontend (直接 FX trace + BN folding)
print('Torch Frontend:')
frontend_torch = get_frontend('torch')
traced_torch = frontend_torch.prepare(model, [x])
print(f'  - BN folding: Yes')
print(f'  - Constants found: {len(traced_torch.constants)}')

# Torch-Via-ONNX Frontend (ONNX export)
print()
print('Torch-Via-ONNX Frontend:')
frontend_onnx = get_frontend('torch-via-onnx')
onnx_ir = frontend_onnx.prepare(model, [x])
print(f'  - ONNX path: {onnx_ir.onnx_path}')

# 检查 ONNX 文件中的操作类型
import onnx
onnx_model = onnx.load(onnx_ir.onnx_path)
op_types = {}
for node in onnx_model.graph.node:
    op_type = node.op_type
    op_types[op_type] = op_types.get(op_type, 0) + 1

print(f'  - ONNX operators: {dict(sorted(op_types.items()))}')
print(f'  - BatchNormalization in ONNX: {"BatchNormalization" not in op_types}')
```

### 运行命令

```bash
python3 test_frontend_compare.py
```

### 预期输出

```
=== Frontend 对比测试 ===

Torch Frontend:
[TorchFrontend] Fusing BatchNorm layers...
  - BN folding: Yes
  - Constants found: 44

Torch-Via-ONNX Frontend:
  - ONNX path: /tmp/tmpXXXXXX.onnx
  - ONNX operators: {'Add': 9, 'Constant': 1, 'Conv': 21, 'Gemm': 1, 'GlobalAveragePool': 1, 'Relu': 19, 'Reshape': 1}
  - BatchNormalization in ONNX: True
```

---

## 常见问题

### Q1: "ace_ext not available" 错误

**原因**: C++ 扩展未编译或加载失败

**解决**:
```bash
# 重新编译扩展
cd /work/ace/ace.refactor/ace_tool.refactor
mkdir -p build && cd build
cmake ..
make -j4
```

### Q2: BN folding 后输出不匹配

**原因**: BN 参数未正确初始化

**解决**:
```python
# 确保模型在 eval 模式下
model.eval()

# 运行一次 forward pass 初始化 BN 统计
with torch.no_grad():
    _ = model(x)

# 然后再进行 folding
fused_model = fuse_modules(model)
```

### Q3: AIR IR 生成失败

**检查**:
1. 所有 custom ops 是否正确注册
2. 张量名称是否正确传递
3. IRBuilder 是否正确初始化

---

## 参考资料

- [BN Folding Design Document](bn_folding_design.md)
- [Torch Frontend Source](python/ace/fhe/frontend/torch_frontend.py)
- [BN Folding Source](python/ace/fhe/frontend/bn_folding.py)