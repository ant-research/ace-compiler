# ResNet20从Torch生成，推理精度问题分析

## 问题概述

在将 ResNet20 PyTorch 模型转换为 AIR 的过程中，ReLU层的输出范围会发生变化，导致 SIHE (Scale-Invariant Homomorphic Encryption) 拟合失败，最终引起推理失败。

## 问题根因

### 随机数来源

ResNet20 模型的输入，存在一个随机数来源：

1. **输入数据随机性**：每次推理时输入数据不同，导致 ReLU 激活值分布变化

### ReLU 拟合问题

在 FHE (全同态加密) 推理中，ReLU 函数需要使用 SIHE (Scale-Invariant Homomorphic Encryption) 技术进行多项式拟合。拟合的精度取决于 `relu_vr` (Virtual ReLU Value Range) 参数的设置。

**关键问题**：如果 ReLU 输出的实际范围超过预设的 VR 值，会导致：
- 多项式拟合精度下降
- 推理结果严重偏差
- 最终推理失败

## 问题背景

### 之前的正常流程

1. 使用固定的 ONNX 模型文件 `resnet20_cifar10_pre.onnx` 进行推理
2. VR (Virtual Range) 参数是**手工调试**出来的，针对这个固定ONNX
3. 推理结果正常

### 现在的问题

当使用 PyTorch 模型重新生成 ONNX 时：
- 输入数据存在随机性
- ReLU 的输出范围发生变化
- 原有的手工 VR 配置不再适用
- 导致推理失败

## 可能存在的解决方案

### 方案一：固定随机数种子（推荐）

通过固定 PyTorch 的随机种子，确保每次生成的 ONNX 模型一致：

```python
import torch
import numpy as np

# 固定所有随机种子
torch.manual_seed(42)
torch.cuda.manual_seed_all(42)
np.random.seed(42)

# 确保模型在 eval 模式
model = resnet20()
model.load_state_dict(torch.load('resnet20_cifar10_pre.pth'))
model.eval()

# 导出 ONNX
torch.onnx.export(
    model,
    example_input,
    "resnet20_cifar10_pre.onnx",
    input_names=["input"],
    output_names=["output"],
    opset_version=11,
)
```

这样可以保证重新生成的 ONNX 与原始文件完全一致，原有的 VR 配置仍然有效，但需要重新调试出一组Relu设置参数。

### 方案二：可以考虑使用工具做自动分析 VR

开发一个 `analyze_onnx_relu.py` 工具，自动分析 ONNX 模型中每个 ReLU 层的输出范围，并生成对应的 VR 配置：

```bash
# 分析 ONNX 模型
python python/ace/fhe/frontend/analyze_onnx_relu.py resnet20_cifar10_pre.onnx -n 100

# 输出示例
relu_vr:
  "/relu/Relu": 4
  "/layer1/layer1.0/relu_1/Relu": 6
  "/layer1/layer1.1/relu_1/Relu": 8
  # ...
```

**工具特点**：
- 支持多次采样，捕获最大激活值
- 自动计算 VR 值：`VR = ceil(abs_max) + 1`
- 支持输出 JSON 格式便于程序读取
- 可配置采样数量

### 方案三：动态 VR 分析

在推理时实时分析 ReLU 范围并动态调整 VR 配置（需要前端和拟合器支持）。

## VR 计算公式

```
VR = ceil(abs_max_value) + 1
```

其中：
- `abs_max_value` = ReLU 输出值的绝对最大值（多次采样取最大值）
- `ceil` = 向上取整
- `+ 1` = 安全边际

## 验证步骤

- **模型**: ResNet20 (CIFAR-10)
- **测试用例**: 重新生成Resnet20.onnx，调试Relu范围
- **相关文件**:
  - `git clone https://code.alipay.com/air-infra/cti.git` - 已经验证过的`resnet20_cifar10_pre.onnx`生成办法
  - `cd cti/model/cifar/pytorch-cifar-models`
  - `python resnet20_cifar10_pre_trained.py -o resnet20_tun.onnx` - 生成新的测试ONNX 模型
  - `/opt/avhc/bin/fhe_cmplr resnet20_tun.onnx -CKKS:hw=192:q0=60:sf=56:sbm -P2C:fp -SIHE:relu_vr_def=3:relu_vr=/relu/Relu=100;/layer1/layer1.0/relu_1/Relu=100;/layer1/layer1.1/relu/Relu=100;/layer1/layer1.1/relu_1/Relu=100;/layer1/layer1.2/relu_1/Relu=100;/layer2/layer2.0/relu_1/Relu=100;/layer2/layer2.1/relu_1/Relu=100;/layer2/layer2.2/relu_1/Relu=100;/layer3/layer3.0/relu_1/Relu=100;/layer3/layer3.1/relu_1/Relu=100;/layer3/layer3.2/relu/Relu=100;/layer3/layer3.2/relu_1/Relu=100 -P2C:lib=ant -O2A:ts -FHE_SCHEME:ts -VEC:ts:rtt:rtv:rtd -SIHE:ts:rtt -CKKS:ts:rtt -POLY:ts:rtt -P2C:ts -P2C:df=resnet20_cifar10_pre.weight -o resnet20_tun.c` - 编译生成，需要调试这里的relu范围
  - `cp resnet20_tun.c /app/fhe-cmplr/rtlib/ant/dataset/resnet20_cifar10_pre.onnx.inc`
  - `c++ /app/fhe-cmplr/rtlib/ant/dataset/resnet20_cifar10.cxx -DRTLIB_SUPPORT_LINUX -I /opt/avhc/include -I /opt/avhc/rtlib/include/ -I /opt/avhc/rtlib/include/ant/ -O3 -DNDEBUG -std=gnu++17 -fopenmp /opt/avhc/rtlib/lib/libFHErt_ant.a /opt/avhc/rtlib/lib/libFHErt_common.a /opt/avhc/lib/libAIRutil.a -lgmp -lm -o /app/resnet20_cifar10_pre.ace -lgomp` - 编译生成可执行程序
  - `/app/resnet20_cifar10_pre.ace /opt/datasets/cifar-10-batches-bin/test_batch.bin 0 0` - 推理验证

## 验证方案

1. **方案一验证**：
   - 固定随机种子后重新生成 ONNX
   - 使用 `diff` 对比生成的 ONNX 与原始文件是否完全一致

2. **方案二验证**：
   - 使用 `analyze_onnx_relu.py` 分析新生成的 ONNX
   - 将生成的 VR 配置与原有配置对比
   - 如有差异，更新配置文件

## 附录：现有 VR 配置

基于原始 `resnet20_cifar10_pre.onnx` 分析的 VR 配置（手工调试）：

```yaml
    '-SIHE:relu_vr_def=3:relu_vr=' +
    '/relu/Relu=4' +  # 1
    ';/layer1/layer1.0/relu_1/Relu=4' +  # 3
    ';/layer1/layer1.1/relu/Relu=4' +  # 4
    ';/layer1/layer1.1/relu_1/Relu=5' +  # 5
    ';/layer1/layer1.2/relu_1/Relu=5' +  # 7
    ';/layer2/layer2.0/relu_1/Relu=5' +  # 9
    ';/layer2/layer2.1/relu_1/Relu=5' +  # 11
    ';/layer2/layer2.2/relu_1/Relu=7' +  # 13
    ';/layer3/layer3.0/relu_1/Relu=4' +  # 15
    ';/layer3/layer3.1/relu_1/Relu=6' +  # 17
    ';/layer3/layer3.2/relu/Relu=4' +  # 18
    ';/layer3/layer3.2/relu_1/Relu=20'  # 19
```