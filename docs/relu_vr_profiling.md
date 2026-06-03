# 基于 CIFAR 数据集的 ReLU 拟合范围 (relu_vr) Profile 方案

> **注意**：本文档中的 `profile_relu_torch.py`（Hook 方案）和 `profile_relu.py`（ONNX 方案）已被弃用。
> 请使用新的 FX Interpreter 方案：
> - **API**：`ReLUProfiler` (`ace.fhe.config.profiler`) 或 `CompileOptions(profile_relu=True)`
> - **CLI**：`python -m ace.model.relu_profile`
> - **设计文档**：`docs/design/relu-vr-profiling.md`
>
> 新方案的优势：per-call-site VR 值（ResNet-20: 19 节点 vs 旧方案 10 节点），与 AIR IR 节点名完全匹配。

## 1. 背景

在 FHE（全同态加密）推理中，ReLU 函数需要通过 SIHE 多项式拟合来近似。拟合精度取决于 `relu_vr`（Value Range）参数的设置——它定义了每个 ReLU 节点的输入值范围。

**核心问题**：如果 `relu_vr` 设置过小，实际激活值超出拟合范围，导致多项式近似误差急剧增大，推理结果偏差甚至失败；如果设置过大，则浪费同态计算资源，降低精度。

早期做法是针对固定 ONNX 文件手工调试 VR 值（见 `docs/resnet20_relu_issue.md`），但当 PyTorch 模型重新导出 ONNX 时，节点命名和数值分布可能变化，手工配置不再适用。

**本方案**：使用真实 CIFAR 测试数据，自动 profile 每个 ReLU 节点的激活值范围，生成精确的 `relu_vr` 配置。

提供两种 Profile 实现：
- **PyTorch 原生方案**（推荐）：直接在 PyTorch 模型上用 `register_forward_hook` 统计 ReLU 激活范围，与 Torch → AIR 编译路径对齐
- **ONNX 方案**：通过导出 ONNX 并修改计算图来 profile，适用于 ONNX → AIR 编译路径

## 2. 方案设计

### 2.1 核心思路

**原则：Profile 的对象必须与编译的对象一致。** FHE 编译路径决定了应该选择哪种 profile 方式。

#### 方案 A：PyTorch 原生 Profile（推荐，Torch → AIR 路径）

实际编译路径为 `Torch → lowering（BN 折叠等）→ AIR`，ReLU 是 PyTorch 模型中的原始 `nn.ReLU` 算子，不是 BN lowering 生成的。因此直接对 PyTorch 模型做 profile 更合理：

1. 用 `model.named_modules()` 找到所有 `nn.ReLU` 模块
2. 通过 `register_forward_hook` 注册钩子，在每个 ReLU 的输出上统计 abs_max
3. 输入真实 CIFAR 测试图片，逐样本推理
4. 计算 `VR = ceil(abs_max) + margin`，生成 `sihe.relu_vr` 配置
5. ReLU 节点名通过 `name_fn` 参数自定义，默认映射为 AIR 风格路径（如 `layer1.0.relu` → `/layer1/layer1.0/relu/Relu`）

**优势**：
- 无需 ONNX 中间步骤，profile 的模型和编译的模型是同一个 PyTorch 对象
- 不存在 ONNX 导出引入图变换导致节点名/数量不匹配的风险
- 节点命名由 `name_fn` 参数控制，可灵活适配不同前端命名规则

#### 方案 B：ONNX Profile（ONNX → AIR 路径）

1. 将 PyTorch 模型导出为 ONNX
2. 修改 ONNX 计算图，将所有 ReLU 中间输出暴露为模型输出
3. 用 ONNX Runtime 加载修改后的模型，输入真实 CIFAR 测试图片
4. 逐样本统计每个 ReLU 节点的 `abs_max`
5. 计算 `VR = ceil(abs_max) + margin`，生成 `sihe.relu_vr` 配置

### 2.2 为什么使用真实数据而非随机数据

早期的 `analyze_onnx_relu.py` 使用随机输入（ones、randn、uniform）来估算 ReLU 范围，存在以下问题：

- 随机输入的分布与真实数据分布差异大，估算的 abs_max 偏差大
- 无法反映归一化后的真实输入范围
- 对于深层网络，随机噪声逐层放大/衰减，与真实激活分布偏离

使用 CIFAR 测试集可以：
- 反映模型在真实推理场景下的激活值分布
- 经过归一化（mean/std），与训练/推理时一致
- 覆盖足够多的样本，提高 abs_max 估计的可靠性

### 2.3 VR 计算公式

```
VR = ceil(abs_max) + margin
```

- `abs_max`：所有测试样本中该 ReLU 节点输出值的绝对最大值
- `ceil`：向上取整，确保覆盖非整数范围
- `margin`：安全边际（默认 1），防止未覆盖的极端值

`relu_vr_def` 为未在 `relu_vr` 中显式列出的 ReLU 节点的默认 VR 值，通常设为 3。

## 3. 实现架构

### 3.1 PyTorch 原生方案：`fhe_dsl/python/model/resnet/profile_relu_torch.py`

核心函数：

```python
result = profile_relu_vr_torch(model, images, margin=1, relu_vr_def=3, name_fn=None)
```

**参数**：
| 参数 | 类型 | 说明 |
|------|------|------|
| `model` | `nn.Module` | PyTorch 模型（eval 模式） |
| `images` | `Tensor` | 输入图片 `(N, C, H, W)`，已归一化 |
| `margin` | `int` | 安全边际，默认 1 |
| `relu_vr_def` | `int` | 未列出 ReLU 的默认 VR，默认 3 |
| `name_fn` | `Callable[[str], str]` | 自定义命名函数，输入 torch 模块路径，返回 AIR 节点名 |

**默认命名规则** `default_relu_name_fn`：

```python
# torch module path -> AIR node name
"layer1.0.relu"   -> "/layer1/layer1.0/relu/Relu"
"relu"            -> "/relu/Relu"
```

**自定义命名示例**：

```python
# 自定义命名：不加 /Relu 后缀
result = profile_relu_vr_torch(model, images, name_fn=lambda n: f"/{n.replace('.', '/')}")

# 自定义命名：使用扁平名
result = profile_relu_vr_torch(model, images, name_fn=lambda n: n)
```

**内部流程**：

1. 遍历 `model.named_modules()`，找到所有 `isinstance(module, nn.ReLU)` 的模块
2. 为每个 ReLU 模块注册 `register_forward_hook`，在 hook 中计算 `output.abs().max()` 并更新全局 abs_max
3. 逐样本推理（`with torch.no_grad()`），hook 自动收集统计值
4. 推理结束后移除所有 hook
5. 计算 VR 值并格式化

**返回值**与 ONNX 方案一致：

```python
{
    "relu_vr_def": 3,
    "relu_vr": "/relu/Relu=4;/layer1/layer1.0/relu/Relu=4;...",
    "per_node": {
        "/relu/Relu": {"abs_max": 3.21, "vr": 4},
        "/layer1/layer1.0/relu/Relu": {"abs_max": 3.12, "vr": 4},
        ...
    }
}
```

### 3.2 PyTorch CLI 脚本：`scripts/profile_relu_vr_torch.py`

与 ONNX 版 CLI 参数完全一致，加 `[PyTorch native]` 标识区分：

```bash
# Profile ResNet-20
python scripts/profile_relu_vr_torch.py --model resnet20

# Profile ResNet-110，使用 500 个样本
python scripts/profile_relu_vr_torch.py --model resnet110 --num-samples 500

# 与现有配置对比
python scripts/profile_relu_vr_torch.py --model resnet20 --compare

# 输出 fhe_cmplr CLI 格式
python scripts/profile_relu_vr_torch.py --model resnet20 --format cli
```

### 3.3 ONNX 方案：`fhe_dsl/python/model/resnet/profile_relu.py`

提供可编程的 API，核心函数：

```python
result = profile_relu_vr(model, images, margin=1, relu_vr_def=3)
```

**参数**：
| 参数 | 类型 | 说明 |
|------|------|------|
| `model` | `nn.Module` | PyTorch 模型（eval 模式） |
| `images` | `Tensor` | 输入图片 `(N, C, H, W)`，已归一化 |
| `margin` | `int` | 安全边际，默认 1 |
| `relu_vr_def` | `int` | 未列出 ReLU 的默认 VR，默认 3 |

**返回值**：
```python
{
    "relu_vr_def": 3,
    "relu_vr": "/relu/Relu=4;/layer1/layer1.0/relu_1/Relu=7;...",  # 分号分隔
    "per_node": {
        "/relu/Relu": {"abs_max": 3.21, "vr": 4},
        "/layer1/layer1.0/relu_1/Relu": {"abs_max": 6.45, "vr": 7},
        ...
    }
}
```

**内部流程**：

1. `_export_to_onnx()`：将 PyTorch 模型导出为 ONNX（内存中，opset=13）
2. `_find_relu_nodes()`：遍历 ONNX 图，找到所有 `op_type == "Relu"` 的节点，返回 `(node_name, output_name)` 列表
3. `_modify_onnx_for_relu_outputs()`：深拷贝 ONNX 模型，将所有 ReLU 输出添加为模型输出
4. 使用 ONNX Runtime 逐样本推理，跟踪每个 ReLU 节点的全局 `abs_max`
5. 计算 VR 值并格式化

**辅助函数**：

- `format_sihe_config(result)`：输出 `fhe_cmplr` 命令行格式
  ```
  -SIHE:relu_vr_def=3:relu_vr=/relu/Relu=4;/layer1/...
  ```
- `format_python_config(result)`：输出 Python dict 格式，可直接粘贴到 `config.py`
- `compare_with_config(result, config_relu_vr, config_relu_vr_def)`：与现有配置对比，标记 `TOO LOW`/`EXCESS`/`OK`

### 3.4 ONNX CLI 脚本：`scripts/profile_relu_vr.py`

命令行入口，支持以下参数：

| 参数 | 默认值 | 说明 |
|------|--------|------|
| `--model` | `resnet20` | 模型变体：resnet20/32/44/56/110/resnet32_cifar100 |
| `--num-samples` | `100` | 使用的 CIFAR 测试图片数量 |
| `--margin` | `1` | VR 安全边际 |
| `--relu-vr-def` | `3` | 默认 VR 值 |
| `--format` | `python` | 输出格式：python / cli / table |
| `--compare` | `False` | 与现有 config.py 中的值对比 |

使用示例：

```bash
# Profile ResNet-20，输出 Python dict 格式
python scripts/profile_relu_vr.py --model resnet20

# Profile ResNet-110，使用 500 个样本
python scripts/profile_relu_vr.py --model resnet110 --num-samples 500

# 与现有配置对比
python scripts/profile_relu_vr.py --model resnet20 --compare

# 输出 fhe_cmplr CLI 格式
python scripts/profile_relu_vr.py --model resnet20 --format cli

# 表格形式查看每个 ReLU 的 abs_max 和 VR
python scripts/profile_relu_vr.py --model resnet20 --format table
```

### 3.5 数据加载

CIFAR 数据通过以下模块加载：

- `ace.model.cifar10.load_cifar10_images(n, offset)`：加载 CIFAR-10 测试集
- `ace.model.cifar100.load_cifar100_images(n, offset)`：加载 CIFAR-100 测试集

归一化参数（与训练源 chenyaofo/pytorch-cifar-models 一致）：
```python
# cifar10.py
CIFAR10_MEAN = [0.4914, 0.4822, 0.4465]
CIFAR10_STD  = [0.229, 0.224, 0.225]    # 当前使用 ImageNet std（见 5.4 实验 F）

# cifar100.py
CIFAR100_MEAN = [0.5070, 0.4865, 0.4409]
CIFAR100_STD  = [0.2673, 0.2564, 0.2761]
```

> **注意**：CIFAR-10 训练时的原始 std 为 `[0.2023, 0.1994, 0.2010]`，当前 `cifar10.py` 使用 ImageNet std 是为实验 F 配置的，实际部署需根据最优归一化方案确定。

数据预处理流程：原始 uint8 → float32 / 255.0 → (x - mean) / std

## 4. Profile 结果

各模型变体的 `relu_vr` 配置已写入 `fhe_dsl/python/model/resnet/config.py`：

| 模型 | 数据集 | ReLU 节点数 | relu_vr_def | VR 范围 |
|------|--------|-------------|-------------|---------|
| ResNet-20 | CIFAR-10 | 19 | 3 | 3 ~ 20 |
| ResNet-32 | CIFAR-10 | ~20 | 2 | 3 ~ 11 |
| ResNet-32 | CIFAR-100 | ~22 | 3 | 4 ~ 46 |
| ResNet-44 | CIFAR-10 | ~22 | 2 | 2 ~ 16 |
| ResNet-56 | CIFAR-10 | ~25 | 2 | 3 ~ 12 |
| ResNet-110 | CIFAR-10 | ~53 | 3 | 3 ~ 33 |

### 4.1 关键观察

1. **VR 值随网络深度递增**：浅层 ReLU（layer1）的 VR 较小（3~7），深层（layer3）的 VR 显著增大，最后一层可达到 20~46
2. **残差分支 vs 主分支**：每个残差块有两个 ReLU（`relu` 和 `relu_1`），`relu_1`（残差相加后的 ReLU）通常 VR 更大
3. **CIFAR-100 vs CIFAR-10**：CIFAR-100 的 VR 值整体更大，最后一层 `relu_1` 达到 46，反映 100 类分类任务激活值范围更广
4. **relu_vr_def 的选择**：大部分未列出的 ReLU 节点 abs_max 在 2~3 之间，因此 `relu_vr_def=3` 是合理的保守默认值

### 4.2 ResNet-20 Profile 验证（PyTorch 原生方案 vs Config）

使用 `profile_relu_torch.py` 对 ResNet-20 CIFAR-10 全量测试集（10000 张）进行 profile，验证与 `RESNET20_COMPILE_OPTIONS` 中手工配置的 VR 值是否一致。

**前提**：`BasicBlock` 中 `self.relu` 在 `forward()` 中被调用两次（bn1 后、residual add 后），PyTorch `named_modules()` 只能识别一个 `nn.ReLU` 实例。为分别采集两个 ReLU 的激活范围，将 `self.relu` 拆分为 `self.relu`（bn1 后）和 `self.relu2`（residual add 后）。由于 `nn.ReLU` 无可学习参数，不影响预训练权重加载。

#### 拆分前（10 个 ReLU，合并统计）

| ReLU Node | abs_max | VR |
|---|---|---|
| /relu/Relu | 2.9473 | 4 |
| /layer1/0/relu/Relu | 5.2018 | 7 |
| /layer1/1/relu/Relu | 5.4973 | 7 |
| /layer1/2/relu/Relu | 5.5082 | 7 |
| /layer2/0/relu/Relu | 4.5392 | 6 |
| /layer2/1/relu/Relu | 4.8548 | 6 |
| /layer2/2/relu/Relu | 7.1927 | 9 |
| /layer3/0/relu/Relu | 4.1094 | 6 |
| /layer3/1/relu/Relu | 7.0333 | 9 |
| /layer3/2/relu/Relu | 18.6785 | 20 |

> 拆分前每个 block 只有一个 `relu`，hook 捕获的是两次使用的合并 max，无法区分 `relu`（bn1 后）和 `relu_1`（residual add 后）。

#### 拆分后（19 个 ReLU，独立统计）

| PyTorch 模块名 | Profiled 节点名 | abs_max | VR | Config 中的对应名 | Config VR | 匹配 |
|---|---|---|---|---|---|---|
| relu | /relu/Relu | 2.9473 | 4 | /relu/Relu | 4 | ✓ |
| layer1.0.relu | /layer1/0/relu/Relu | 2.3557 | 4 | /layer1/layer1.0/relu/Relu | 4 | ✓ |
| layer1.0.relu2 | /layer1/0/relu2/Relu | 5.2018 | 7 | /layer1/layer1.0/relu_1/Relu | 7 | ✓ |
| layer1.1.relu | /layer1/1/relu/Relu | 2.3808 | 4 | /layer1/layer1.1/relu/Relu | 4 | ✓ |
| layer1.1.relu2 | /layer1/1/relu2/Relu | 5.4973 | 7 | /layer1/layer1.1/relu_1/Relu | 7 | ✓ |
| layer1.2.relu | /layer1/2/relu/Relu | 2.1007 | 4 | /layer1/layer1.2/relu/Relu | 4 | ✓ |
| layer1.2.relu2 | /layer1/2/relu2/Relu | 5.5082 | 7 | /layer1/layer1.2/relu_1/Relu | 7 | ✓ |
| layer2.0.relu | /layer2/0/relu/Relu | 3.0914 | 5 | /layer2/layer2.0/relu/Relu | 5 | ✓ |
| layer2.0.relu2 | /layer2/0/relu2/Relu | 4.5392 | 6 | /layer2/layer2.0/relu_1/Relu | 6 | ✓ |
| layer2.1.relu | /layer2/1/relu/Relu | 1.7576 | 3 | /layer2/layer2.1/relu/Relu | 3 | ✓ |
| layer2.1.relu2 | /layer2/1/relu2/Relu | 4.8548 | 6 | /layer2/layer2.1/relu_1/Relu | 6 | ✓ |
| layer2.2.relu | /layer2/2/relu/Relu | 2.6838 | 4 | /layer2/layer2.2/relu/Relu | 4 | ✓ |
| layer2.2.relu2 | /layer2/2/relu2/Relu | 7.1927 | 9 | /layer2/layer2.2/relu_1/Relu | 9 | ✓ |
| layer3.0.relu | /layer3/0/relu/Relu | 2.6673 | 4 | /layer3/layer3.0/relu/Relu | 4 | ✓ |
| layer3.0.relu2 | /layer3/0/relu2/Relu | 4.1094 | 6 | /layer3/layer3.0/relu_1/Relu | 6 | ✓ |
| layer3.1.relu | /layer3/1/relu/Relu | 3.0124 | 5 | /layer3/layer3.1/relu/Relu | 5 | ✓ |
| layer3.1.relu2 | /layer3/1/relu2/Relu | 7.0333 | 9 | /layer3/layer3.1/relu_1/Relu | 9 | ✓ |
| layer3.2.relu | /layer3/2/relu/Relu | 2.1954 | 4 | /layer3/layer3.2/relu/Relu | 4 | ✓ |
| layer3.2.relu2 | /layer3/2/relu2/Relu | 18.6785 | 20 | /layer3/layer3.2/relu_1/Relu | 20 | ✓ |

**结论**：19 个 ReLU 节点的 VR 值全部吻合，`profile_relu_torch.py` + 拆分 `relu2` 的方案验证通过。

**命名映射说明**：
- PyTorch 模块路径 `layer1.0.relu2` 对应 AIR IR 中的 `relu_1`（ONNX 导出时对同名 ReLU 自动编号）
- PyTorch 路径中的 `layer1.0` 对应 AIR IR 中的 `layer1/layer1.0`（AIR 命名含父级前缀）
- 这两个映射差异仅影响节点名字符串，不影响 VR 数值

### 4.3 relu_vr 由模型和数据集共同决定

`relu_vr` 是**模型（架构+权重）× 数据集**共同决定的，不是数据集单独决定的。ReLU 的输出是前面所有层（conv + BN + 权重）计算的结果，同样的 CIFAR 图片送进 ResNet-20 和 ResNet-110，每个 ReLU 的激活值分布完全不同。上表中 ResNet-20 最后一层 VR=20 而 ResNet-110 最后一层 VR=33 也印证了这一点。

因此：**对于一个确定的模型（架构+权重）和一个确定的数据集，relu_vr 通过 profile 可以固定下来。** 换模型或重新训练后权重变化，都需要重新 profile。

## 5. ReLU VR 与 FHE 推理精度分析

`relu_vr` 正确是明文/密文推理结果一致的**必要条件**，但不是**充分条件**。即使 VR 精确覆盖了实际激活范围、编译器没有 bug，两者仍可能不一致。

### 5.1 VR 对 FHE 精度的影响机制

ReLU 在 FHE 中不是精确计算，而是**多项式近似**，存在多层误差叠加：

1. **多项式拟合误差**：即使输入在 `[-vr, vr]` 范围内，多项式（如 Chebyshev）对 ReLU 的近似仍有有界误差。多项式阶数越高误差越小，但受 CKKS 乘法深度限制。
2. **CKKS 编码噪声**：同态运算每一步都引入加密噪声，经过几十层卷积+ReLU 后噪声逐步累积。
3. **误差逐层放大**：浅层 ReLU 的微小误差，经过后续卷积（乘以大量权重）会被放大。网络越深，误差传播越严重。

VR 大小的影响：

| relu_vr 设置 | 后果 |
|-------------|------|
| **过小**（实际激活值超出范围） | 多项式在范围外发散，结果**灾难性错误** |
| **恰好覆盖** | 多项式在范围内近似，但近似误差+CKKS噪声仍可能导致**小幅偏移**，少数边界样本可能翻转 |
| **过大** | 缩放因子 `1/vr` 变小，输入被压缩到更窄的 `[-1,1]` 区间，多项式拟合精度反而下降 |

最终一致率由以下因素共同决定：

| 因素 | 影响 |
|------|------|
| 多项式阶数 | 阶数越高，ReLU 近似越精确，但消耗更多乘法深度 |
| CKKS 参数（sf、N、q0） | sf 越高精度越高但乘法深度越受限；N/q0 影响噪声预算 |
| 网络深度 | 深层网络误差累积更严重 |
| relu_vr 精确度 | 精确的 VR 避免范围外发散和过度压缩 |
| 激活值分布 | 靠近 argmax 决策边界的样本更容易因微小误差翻转 |

**结论**：profile 得到精确的 `relu_vr` 只是第一步——它消除了"范围外发散"这个最大的误差源，但要让明文/密文推理达到高一致率，还需要合理配置多项式阶数和 CKKS 参数。

### 5.2 归一化参数修正

#### 5.2.1 归一化参数不匹配问题

ResNet-20 的预训练权重来自 [chenyaofo/pytorch-cifar-models](https://github.com/chenyaofo/pytorch-cifar-models)，该仓库训练时使用的归一化参数为 CIFAR-10 自身的统计量，而非 ImageNet 的：

| 参数集 | mean | std | 来源 |
|--------|------|-----|------|
| **CIFAR-10**（训练时使用） | [0.4914, 0.4822, 0.4465] | [0.2023, 0.1994, 0.2010] | chenyaofo/pytorch-cifar-models |
| **CIFAR-100**（训练时使用） | [0.5070, 0.4865, 0.4409] | [0.2673, 0.2564, 0.2761] | chenyaofo/pytorch-cifar-models |
| ImageNet（之前推理时误用） | [0.485, 0.456, 0.406] | [0.229, 0.224, 0.225] | — |

之前 `cifar10.py` 和 `cifar100.py` 使用了 ImageNet 归一化参数，与训练时不一致，导致输入数据分布偏移，影响明文和密文推理精度。

**修复**：已将 `cifar10.py` 和 `cifar100.py` 的归一化参数改为与训练一致的 CIFAR 统计量（`IMAGENET_MEAN/STD` → `CIFAR10_MEAN/STD`），C++ 端 FHE runtime 也已同步修改：
- `backend/fhe-cmplr/rtlib/phantom/example/resnet_cifar.main.inc`
- `backend/fhe-cmplr/rtlib/ant/dataset/resnet_cifar.main.inc`

#### 5.2.2 归一化修正后的明文精度

| 测试条件 | 精度 |
|----------|------|
| 训练源报告精度（全量 10000 张） | 92.60% |
| 修正后明文推理（前 1000 张，CIFAR-10 归一化） | 92.5% |
| 修正前明文推理（ImageNet 归一化） | ~90.2% |
| 修正前密文推理（100 张，ImageNet 归一化） | ~88% |

修正归一化后明文精度从 ~90.2% 提升到 92.5%，确认归一化不匹配是精度损失的重要来源。

#### 5.2.3 归一化修正对 relu_vr 的影响

使用 CIFAR-10 归一化重新 profile 全量测试集（10000 张），与 ImageNet 归一化的 profile 结果对比：

| 节点 | ImageNet 归一化 VR | CIFAR-10 归一化 VR | 变化 |
|------|-------------------|-------------------|------|
| /relu/Relu | 4 | **5** | +1 |
| layer1.0.relu | 4 | 4 | - |
| layer1.0.relu_1 | 7 | 7 | - |
| layer1.1.relu | 4 | 4 | - |
| layer1.1.relu_1 | 7 | 7 | - |
| layer1.2.relu | 4 | 4 | - |
| layer1.2.relu_1 | 7 | 7 | - |
| layer2.0.relu | 5 | 5 | - |
| layer2.0.relu_1 | 6 | 6 | - |
| layer2.1.relu | 3 | 3 | - |
| layer2.1.relu_1 | 6 | **7** | +1 |
| layer2.2.relu | 4 | 4 | - |
| layer2.2.relu_1 | 9 | 9 | - |
| layer3.0.relu | 4 | 4 | - |
| layer3.0.relu_1 | 6 | 6 | - |
| layer3.1.relu | 5 | 5 | - |
| layer3.1.relu_1 | 9 | 9 | - |
| layer3.2.relu | 4 | 4 | - |
| layer3.2.relu_1 | 20 | 20 | - |

2 个节点 VR 值变大：stem 的 `/relu/Relu` 从 4→5，`layer2.1.relu_1` 从 6→7。已更新 `RESNET20_COMPILE_OPTIONS` 中对应的值。

### 5.3 ONNX Baseline 确立

#### 5.3.1 为什么 ONNX 是正确 baseline

FHE 编译路径为 `Torch → ONNX → AIR`，密文推理使用的是 ONNX 模型（经过 BN 折叠等变换），因此 **ONNX 明文推理才是密文推理的正确 baseline**，而非 PyTorch 明文推理。

#### 5.3.2 旧 ONNX 模型权重不一致问题

早期实验使用 `/app/model/resnet20_cifar10_pre.onnx`，发现其与当前 PyTorch 模型权重不一致：
- 旧 ONNX + CIFAR-10 归一化：90/100
- PyTorch + CIFAR-10 归一化：94/100
- 两者输出 max_diff 高达 1.3（正常 BN 折叠浮点误差应 <0.01）

重新从当前 PyTorch 权重导出 ONNX 模型：`fhe_dsl/python/model/resnet/weights/resnet20_cifar10.onnx`

新 ONNX 模型验证结果：

| 推理方式 | 归一化 | 100 张精度 | 1000 张精度 |
|----------|--------|-----------|------------|
| PyTorch | CIFAR-10 | 94% | 92.5% |
| 新 ONNX | CIFAR-10 | 94% | 92.5% |
| 新 ONNX | ImageNet | 91% | — |

新 ONNX 与 PyTorch 精度完全一致，确认 BN 折叠对推理精度无影响。**后续所有实验均使用新 ONNX 模型。**

#### 5.3.3 ONNX Profile 验证

使用 `profile_relu.py` 对新 ONNX 模型 profile（CIFAR-10 归一化，10000 张，margin=1），结果与 PyTorch profile 完全一致，确认 BN 折叠不影响激活值范围。

### 5.4 系统化对照实验

#### 5.4.1 实验设计

| 编号 | VR 配置 | 归一化 | 说明 |
|------|---------|--------|------|
| A | profiled（margin=1, CIFAR-10 norm） | CIFAR-10 mean + std | 正确配置 + 自动 VR |
| B | 旧手工 VR | CIFAR-10 mean + std | 自动 VR vs 手工 VR |
| C | profiled（margin=1, ImageNet norm） | ImageNet mean + std | 归一化对 FHE 的影响 |
| E | profiled（margin=1, std=0.225） | CIFAR-10 mean + std=0.225 | std 扫描 |
| F | profiled（margin=1, CIFAR-10 mean + ImageNet std） | CIFAR-10 mean + ImageNet std | 混合归一化 |

#### 5.4.2 明文 Baseline（新 ONNX，100 张）

| 归一化 | 精度 | 失败 index |
|--------|------|-----------|
| CIFAR-10 (mean=[0.4914,0.4822,0.4465], std=[0.2023,0.1994,0.2010]) | 94/100 | 15,37,52,61,78,87 |
| ImageNet (mean=[0.485,0.456,0.406], std=[0.229,0.224,0.225]) | 91/100 | 3,15,37,52,58,61,78,87,95 |
| CIFAR-10 mean + std=0.225 | 94/100 | 15,37,52,61,78,87 |
| CIFAR-10 mean + ImageNet std | 94/100 | 15,37,52,61,78,87 |

#### 5.4.3 密文实验结果（100 张）

| 编号 | VR | 归一化 | 密文精度 | 失败数 | 明文 baseline | FHE 开销 |
|------|-----|--------|---------|--------|-------------|---------|
| A | profiled(margin=1) | CIFAR-10 (std=0.201) | 86/100 | 14 | 94% | **8%** |
| B | 旧手工 | CIFAR-10 (std=0.201) | 85/100 | 15 | 94% | **9%** |
| C | profiled(margin=1) | ImageNet | 87/100 | 13 | 91% | **4%** |
| E | profiled(margin=1) | CIFAR-10 mean + std=0.225 | 86/100 | 14 | 94% | **8%** |
| F | profiled(margin=1) | CIFAR-10 mean + ImageNet std | ~86/100 | ~14 | 94% | **~8%** |

A 组失败 index：15,25,35,47,52,57,58,59,61,70,78,86,87,95
B 组失败 index：15,25,35,47,52,57,58,59,61,70,78,86,87,95,98
C 组失败 index：15,25,47,52,57,58,59,61,70,78,86,87,95

#### 5.4.4 C 组扩展实验（500 张）

C 组（ImageNet 归一化）扩展到 500 张图片测试：

- **密文精度**：434/500 = **86.8%**
- **失败数**：66 张
- **失败 index**：15, 25, 47, 52, 57, 58, 59, 61, 70, 78, 86, 87, 95, 118, 125, 128, 134, 147, 155, 158, 164, 169, 171, 183, 193, 201, 206, 213, 214, 221, 226, 228, 232, 247, 258, 259, 269, 271, 275, 277, 287, 305, 312, 313, 324, 325, 332, 342, 346, 352, 355, 357, 378, 384, 388, 412, 421, 426, 428, 430, 433, 439, 456, 478, 485, 491, 497

**单张GPU，因内存限制，最大能跑500张图片推理**

C 组完整配置：

- **归一化**：mean=[0.485, 0.456, 0.406], std=[0.229, 0.224, 0.225]
- **VR 配置**（ImageNet 归一化 profile）：
  ```
  relu_vr_def=3
  relu_vr=/relu/Relu=4;/layer1/layer1.0/relu/Relu=4;/layer1/layer1.0/relu_1/Relu=7;/layer1/layer1.1/relu/Relu=4;/layer1/layer1.1/relu_1/Relu=7;/layer1/layer1.2/relu/Relu=4;/layer1/layer1.2/relu_1/Relu=7;/layer2/layer2.0/relu/Relu=5;/layer2/layer2.0/relu_1/Relu=6;/layer2/layer2.1/relu/Relu=3;/layer2/layer2.1/relu_1/Relu=6;/layer2/layer2.2/relu/Relu=4;/layer2/layer2.2/relu_1/Relu=9;/layer3/layer3.0/relu/Relu=4;/layer3/layer3.0/relu_1/Relu=6;/layer3/layer3.1/relu/Relu=5;/layer3/layer3.1/relu_1/Relu=9;/layer3/layer3.2/relu/Relu=4;/layer3/layer3.2/relu_1/Relu=20
  ```
- **C++ runtime 参数**：
  ```c
  double mean[]  = {0.485, 0.456, 0.406};
  double stdev[] = {0.229, 0.224, 0.225};
  ```

> 注：原计划跑 1000 张，因内存不足在 500 张左右终止。

#### 5.4.5 F 组配置

F 组（CIFAR-10 mean + ImageNet std）完整配置：

- **归一化**：mean=[0.4914, 0.4822, 0.4465], std=[0.229, 0.224, 0.225]
- **VR 配置**（CIFAR-10 mean + ImageNet std profile）：
  ```
  relu_vr_def=3
  relu_vr=/relu/Relu=4;/layer1/layer1.0/relu/Relu=4;/layer1/layer1.0/relu_1/Relu=7;/layer1/layer1.1/relu/Relu=4;/layer1/layer1.1/relu_1/Relu=7;/layer1/layer1.2/relu/Relu=4;/layer1/layer1.2/relu_1/Relu=7;/layer2/layer2.0/relu/Relu=5;/layer2/layer2.0/relu_1/Relu=6;/layer2/layer2.1/relu/Relu=3;/layer2/layer2.1/relu_1/Relu=6;/layer2/layer2.2/relu/Relu=4;/layer2/layer2.2/relu_1/Relu=9;/layer3/layer3.0/relu/Relu=4;/layer3/layer3.0/relu_1/Relu=6;/layer3/layer3.1/relu/Relu=4;/layer3/layer3.1/relu_1/Relu=9;/layer3/layer3.2/relu/Relu=4;/layer3/layer3.2/relu_1/Relu=20
  ```
- **C++ runtime 参数**：
  ```c
  double mean[]  = {0.4914, 0.4822, 0.4465};
  double stdev[] = {0.229, 0.224, 0.225};
  ```

F 组 VR 与 C 组仅有 1 个节点不同：`/layer3/layer3.1/relu/Relu`，F=4 vs C=5。

#### 5.4.6 VR 大小对 FHE 精度的影响

CIFAR-10 归一化下 profile 的 VR 值与旧配置对比：

| 节点 | 旧配置 (手工调优) | margin=0 | margin=1 (当前) | 备注 |
|------|-------------|----------|----------------|------|
| /relu/Relu | 4 | 4 | 5 | |
| layer1.0.relu | 3(def) | 3 | 4 | |
| layer1.0.relu_1 | **4** | **6** | **7** | 差距大 |
| layer1.1.relu | 4 | 3 | 4 | |
| layer1.1.relu_1 | 5 | 6 | 7 | |
| layer1.2.relu | 3(def) | 3 | 4 | |
| layer1.2.relu_1 | 5 | 6 | 7 | |
| layer2.0.relu | 3(def) | 4 | 5 | |
| layer2.0.relu_1 | 5 | 5 | 6 | |
| layer2.1.relu | 3(def) | 2 | 3 | |
| layer2.1.relu_1 | 5 | 6 | 7 | |
| layer2.2.relu | 3(def) | 3 | 4 | |
| layer2.2.relu_1 | 7 | 8 | 9 | |
| layer3.0.relu | 3(def) | 3 | 4 | |
| layer3.0.relu_1 | 4 | 5 | 6 | |
| layer3.1.relu | 3(def) | 4 | 5 | |
| layer3.1.relu_1 | **6** | **8** | **9** | 差距大 |
| layer3.2.relu | 4 | 3 | 4 | |
| layer3.2.relu_1 | 20 | 19 | 20 | |

**观察**：

1. **CIFAR-10 归一化下 `relu_1` 的 VR 显著增大**：如 `layer1.0.relu_1` 旧配置=4，profile=6（margin=0）。CIFAR-10 的 std 更小（0.20 vs 0.23），归一化后输入值更大，激活值范围更宽。
2. **margin=0 vs margin=1 对密文精度几乎无影响**：实验证实三组 VR 配置的密文结果一致。
3. **旧配置部分节点 VR 低于实际 abs_max**：旧配置中很多节点使用了 `relu_vr_def=3`，而实际 abs_max 可能超过 3。但旧配置仍达到 ~87% 密文精度，说明 **VR 偏小但未严重超出时，多项式在范围外的截断误差有限；VR 偏大则系统性地降低多项式拟合精度**。
4. **VR 值不是密文精度的瓶颈**：三组 VR 配置结果一致，说明 FHE 开销主要来自多项式近似误差和 CKKS 噪声累积，而非 VR 配置。

#### 5.4.7 关键发现

1. **VR 配置对密文精度几乎无影响**：A vs B 仅差 1 张图片，VR 不是瓶颈。
2. **归一化是 FHE 开销的主因**：C（ImageNet 归一化）FHE 开销仅 4%，A（CIFAR-10 归一化）FHE 开销 8%。ImageNet 的 std 更大（0.229 vs 0.202），归一化后输入值更小，整网激活值范围更窄，多项式拟合更精确。
3. **CIFAR-10 归一化明文高但密文低**：94% - 8% = 86%；ImageNet 归一化明文低但密文高：91% - 4% = 87%。最终密文精度 ImageNet 反而更优。
4. **密文额外翻转的图片高度重叠**：A/B/C 三组共同失败的图片为 15,52,57,58,59,61,78,87，这些是 FHE 误差导致的系统性翻转样本。
5. **E/F 组与 A 组结果相近**：std=0.225 和 CIFAR-10 mean + ImageNet std 的密文精度均为 ~86%，与 A 组（CIFAR-10 归一化）一致。说明仅调整 std 而保持 CIFAR-10 mean，对 FHE 开销改善有限——C 组的优势来自 ImageNet mean 更小（输入值更小 → 激活值范围更窄）。
6. **数据集 profiling 是必要的**：保证 VR 安全覆盖，防止多项式在范围外发散（灾难性错误）。但 VR 本身不是 FHE 精度瓶颈，FHE 开销需要从归一化策略和 CKKS 参数方向优化。

### 5.5 归一化参数扫描

为了找到明文精度和 FHE 开销的最优平衡点，对 std 参数进行扫描（固定 mean=CIFAR-10 mean，统一三通道 std）：

**明文精度扫描**（新 ONNX，100 张）：

| std | 精度 | 失败 index |
|-----|------|-----------|
| 0.250 | 93/100 | 3,15,52,59,61,78,87 |
| 0.225 | 94/100 | 15,37,52,61,78,87 |
| 0.201 (CIFAR-10) | 94/100 | 15,37,52,61,78,87 |
| 0.180 | 93/100 | 15,35,37,52,61,78,87 |

**ReLU abs_max 扫描**（10000 张，PyTorch 模型，合并统计 relu/relu2）：

| 层 | std=0.250 | std=0.225 | std=0.201 | std=0.180 |
|----|-----------|-----------|-----------|-----------|
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

**VR 汇总**（margin=1）：

| 层 | std=0.250 | std=0.225 | std=0.201 | std=0.180 |
|----|-----------|-----------|-----------|-----------|
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

**分析**：

- std 越大，激活值范围越小，VR_sum 越小，多项式拟合越精确，FHE 误差越小
- std=0.225 与 std=0.201 明文精度相同（94%），但 VR_sum 从 84 降到 81，FHE 误差预期更小
- std=0.250 明文精度下降 1%（93%），但 VR_sum 降到 80，可能进一步降低 FHE 开销
- std=0.180 明文精度也下降，且 VR_sum 最大（87），FHE 误差最大，不可取
- 但 E 组实验（std=0.225）密文精度仍为 86%，说明 VR_sum 的减小（84→81）不足以显著改善 FHE 开销

### 5.6 下一步优化方向

| 优先级 | 方向 | 预期效果 | 复杂度 |
|--------|------|---------|--------|
| 1 | 逐层误差分析 | 定位误差放大层 | 中（需 dump 中间值） |
| 2 | 提高 CKKS sf | 提高每层计算精度 | 中（需重新编译） |
| 3 | ReLU 多项式阶数优化 | 大 VR 节点用高阶多项式 | 高（需编译器支持） |
| 4 | 归一化策略优化 | 探索 mean/std 对 FHE 的联合影响 | 低（不改编译链） |

## 6. 方案对比

| 维度 | 早期方案 (analyze_onnx_relu.py) | ONNX 方案 (profile_relu.py) | PyTorch 方案 (profile_relu_torch.py) |
|------|------|------|------|
| 输入来源 | 随机数据 (ones, randn, uniform) | 真实 CIFAR 测试集 | 真实 CIFAR 测试集 |
| Profile 对象 | 预先导出的 .onnx 文件 | PyTorch → ONNX 导出后 profile | 直接对 PyTorch 模型 profile |
| 适用编译路径 | ONNX → AIR | ONNX → AIR | Torch → AIR |
| 归一化 | 无 | CIFAR-10/100 归一化 | CIFAR-10/100 归一化 |
| 样本量 | 默认 10 | 默认 100 | 默认 100 |
| VR 公式 | `ceil(abs_max) + 1` | `ceil(abs_max) + margin` | `ceil(abs_max) + margin` |
| 节点命名 | ONNX 节点名 | ONNX 节点名 | 自定义 `name_fn`，默认对齐 AIR 命名 |
| 输出格式 | 日志 + 可选 JSON | Python dict / CLI / table | Python dict / CLI / table |
| 配置对比 | 无 | `compare_with_config()` | `compare_with_config()` |
| 集成 | 独立脚本 | 库 API + CLI | 库 API + CLI |

**推荐选择**：根据编译路径选择——Torch → AIR 用 `profile_relu_torch.py`，ONNX → AIR 用 `profile_relu.py`。

## 7. 使用流程

### 7.1 为新模型生成 relu_vr 配置（PyTorch 原生）

```python
from ace.model.resnet import create_pretrained_resnet
from ace.model.resnet.profile_relu_torch import profile_relu_vr_torch
from ace.model.resnet.profile_relu import format_python_config
from ace.model.cifar10 import load_cifar10_images

# 1. 加载模型和数据
model = create_pretrained_resnet(n_layers=20, num_classes=10)
images, labels = load_cifar10_images(100)

# 2. Profile（使用默认命名）
result = profile_relu_vr_torch(model, images, margin=1, relu_vr_def=3)

# 3. 自定义命名（如需适配不同前端）
# result = profile_relu_vr_torch(model, images, name_fn=lambda n: f"/{n.replace('.', '/')}")

# 4. 输出配置
print(format_python_config(result))
```

### 7.2 为新模型生成 relu_vr 配置（ONNX 方案）

```python
from ace.model.resnet import create_pretrained_resnet
from ace.model.resnet.profile_relu import profile_relu_vr, format_python_config
from ace.model.cifar10 import load_cifar10_images

# 1. 加载模型和数据
model = create_pretrained_resnet(n_layers=20, num_classes=10)
images, labels = load_cifar10_images(100)

# 2. Profile
result = profile_relu_vr(model, images, margin=1, relu_vr_def=3)

# 3. 输出配置
print(format_python_config(result))
```

### 7.3 验证现有配置是否安全

```bash
# PyTorch 原生
python scripts/profile_relu_vr_torch.py --model resnet20 --compare

# ONNX 方案
python scripts/profile_relu_vr.py --model resnet20 --compare
```

输出示例：
```
ReLU Node                                           abs_max  Profiled     Config    Status
-------------------------------------------------------------------------------------------
/relu/Relu                                            3.2143         4         4        OK
/layer1/layer1.0/relu/Relu                            3.1234         4         4        OK
/layer1/layer1.0/relu_1/Relu                          6.4521         7         7        OK
...
/layer3/layer3.2/relu_1/Relu                         19.8734        20        20        OK

Summary: 19 OK, 0 TOO LOW, 0 EXCESS
```

如果有 `TOO LOW` 状态的节点，说明现有配置的 VR 值不够大，需要更新。

### 7.4 更新 config.py

将 profile 结果直接更新到 `fhe_dsl/python/model/resnet/config.py` 中对应的 `RESNET*_COMPILE_OPTIONS` 字典的 `sihe.relu_vr` 字段。

## 8. CPU (ant) vs GPU (phantom) 精度差分析

### 8.1 问题描述

ResNet20 在相同 CKKS 参数（`q0=60, sf=56, N=65536`）和相同 `relu_vr` 配置下：
- **CPU (ant lib)**: ~91.2% 准确率
- **GPU (phantom lib)**: ~86% 准确率
- **精度差**: ~5%

编译命令分别为：
```bash
# CPU
./fhe_cmplr resnet20.onnx -CKKS:q0=60:sf=56:N=65536 -P2C:lib=ant
# GPU
./fhe_cmplr resnet20.onnx -CKKS:q0=60:sf=56:N=65536 -P2C:lib=phantom
```

### 8.2 相同点

通过对比生成的 `resnet20_cpu.cu`（~7M 行）和 `resnet20_gpu.cu`（~7M 行），确认以下方面完全一致：

| 维度 | 一致性 |
|------|--------|
| 网络拓扑 | 完全一致 |
| relu_vr 值 | 完全一致 |
| 1/VR 缩放常量 | 完全一致（0.25, 0.142857, 0.2, 0.1667, 0.333, 0.1111, 0.05, 0.015625） |
| Bootstrap 调用次数 | 均为 17 次 |
| Bootstrap level/slot 参数 | 完全一致（level=15/18, slot=16384/8192/4096） |
| 权重常量 (float32_t) | 完全一致 |
| ReLU 多项式结构 | 均为 3-polynomial composition |

### 8.3 关键差异分析

#### 差异 1：Bootstrap 实现（最高嫌疑）

这是最可能导致 ~5% 精度差的原因。两个库使用了完全不同的 bootstrap 算法。

**CPU (ant) Bootstrap** (`Eval_bootstrap` in `ant/ckks/src/bootstrap.c`):

```
流程: Raise level → Coeffs_to_slots → Eval_approx_mod → Slots_to_coeffs → Mul_integer
多项式: 使用预计算的 G_coefficients_* 系数表
求值方法: Eval_chebyshev (Paterson-Stockmeyer 算法)
Scale 跟踪: 精确跟踪 _scaling_factor 和 _sf_degree，不强制设置
```

- 多项式选择基于密钥 hamming weight（SPARSE/UNIFORM/UNIFORM_EVEN）
- 深度表：degree 28-59 → depth 7, degree 60-119 → depth 8
- 使用 `AUTO_SCALE` 模式：仅在 `sf_degree > 1` 时 rescale
- 支持 `CONF_BTS_CLEAR_IMAG` 选项清除虚部

**GPU (phantom) Bootstrap** (`bootstrap_3` in `phantom-src/src/boot/Bootstrapper.cu`):

```
流程: modraise → coefftoslot_full_3 → modular_reduction → slottocoeff_full_3
多项式: 使用 Remez 交换算法在线生成（RemezCos + RemezArcsin）
求值方法: homomorphic_poly_evaluation
Scale 跟踪: 强制设置 rtncipher.scale() = final_scale
```

- 参数：`boundary_K=25, deg=59, scale_factor=2, inverse_deg=1, loge=10`
- modular_reduction 分两步：
  1. `sin_cos_polynomial.homomorphic_poly_evaluation`（余弦多项式求值）
  2. 2 次 double_angle_formula（`inverse_deg=1` 时使用 `scale_inverse_coeff` 方式）
- **关键问题**：`rtncipher.scale() = final_scale` 强制将输出 scale 设为 `2^56`

**精度差异来源**：

1. **多项式精度**：CPU 使用预优化系数，GPU 使用 Remez 算法在线生成。两者的多项式近似精度不同。
2. **求值算法**：CPU 的 Paterson-Stockmeyer 算法在深度-精度权衡上可能更优。
3. **Scale 强制重置**（见差异 2）。
4. **17 次 bootstrap 调用**使误差累积放大。

#### 差异 2：Bootstrap 后 Scale 强制重置（高嫌疑）

**GPU bootstrap 结束时**（`bootstrap_full_3`，第 3124 行）：
```cpp
rtncipher.scale() = final_scale;  // 强制设为 2^56
```

**CPU bootstrap 结束时**：不强制设置 scale，而是通过数学运算自然追踪：
```c
// bootstrap.c 末尾只做 rescale，不强制 scale
while (Get_ciph_sf_degree(res) > 1) {
    Rescale_ciphertext(res, res, eval);
}
```

**影响**：GPU bootstrap 后，密文的 `scale()` 被强制设为 `2^56`，但实际数学 scale 可能略有偏差（因为 CKKS 的模数素数不精确等于 `2^56`）。后续 `Mul_const` 用 `op1->scale()` 编码常量时，编码 scale 与实际 scale 不匹配，引入系统性误差。17 次 bootstrap 后累积效应显著。

#### 差异 3：1/VR 缩放实现（中嫌疑）

**GPU** (`Mul_scalar` → `Mul_const` in `phantom_lib.cu`):
```cpp
void Mul_const(const Ciphertext *op1, double op2, Ciphertext *res) {
    Plaintext pt;
    _evaluator->encoder.encode(op2, op1->chain_index(), op1->scale(), pt);
    _evaluator->evaluator.multiply_plain_inplace(*res, pt);
}
// 然后 Rescale_ciph
```

编码常量时使用 `op1->scale()`。乘法后 scale 变为 `scale^2`，rescale 后约为 `scale^2 / p_L`（`p_L` 为最顶层素数模值）。

**CPU** (`Encode_float` + `Init_ciph_up_scale_plain` + `Hw_modmul` + `Init_ciph_down_scale` + `Rescale`):
```c
Encode_float(&plain, &cst, 1, 1, Level(&ciph));          // 编码常量，scale=1 → 编码 scale=2^56
Init_ciph_up_scale_plain(&tmp, &ciph, &plain);            // sfactor *= plain.sf, sf_degree++
Hw_modmul(tmp.c0, ciph.c0, plain.poly, ...);              // 逐 RNS 分量乘法
Hw_modmul(tmp.c1, ciph.c1, plain.poly, ...);
Init_ciph_down_scale(&result, &tmp);                       // sfactor /= default_sc, sf_degree--
Rescale(&result.c0, &tmp.c0);                              // 分别 rescale c0, c1
Rescale(&result.c1, &tmp.c1);
```

CPU 使用显式的 up_scale/down_scale 机制管理 scale 元数据，并分别对 c0/c1 做 rescale。GPU 使用 `multiply_plain` + `Rescale_ciph` 封装。

**差异**：
1. GPU 的 `Mul_const` 依赖 `op1->scale()`（bootstrap 后可能不准），CPU 的 `Encode_float` 使用固定 `scale=1`
2. CPU 分别 rescale c0/c1，GPU 通过 `rescale_to_next_inplace` 一次性处理
3. GPU 的 `multiply_plain` 内部可能有额外的 NTT 变换，引入更多舍入

#### 差异 4：Rescale 实现差异（低嫌疑）

**CPU Rescale**（`Rescale_ciphertext` in `evaluator.c`）：
```c
double new_factor = old_factor / eval->_params->_scaling_factor;  // 除以 2^56
Init_ciphertext_from_ciph(res, ciph, new_factor, ciph->_sf_degree - 1);
Rescale(Get_c0(res), Get_c0(ciph));  // 分别 rescale c0, c1
Rescale(Get_c1(res), Get_c1(ciph));
```

**GPU Rescale**（`rescale_to_next_inplace` in Phantom 库）：
- 调用 `mod_switch_scale_to_next` → `divide_and_round_q_last_ntt`
- 内部实现可能使用不同的舍入算法

两者在数学上等价，但具体实现（NTT 变换、舍入方式）可能略有差异。单次差异小，但 17 次 bootstrap + 大量卷积运算后可能累积。

### 8.4 根因排序与验证建议

| 排序 | 嫌疑因素 | 影响程度 | 验证方法 |
|------|---------|---------|---------|
| 1 | Bootstrap 算法差异 | 最高 | 单独测试：跳过 bootstrap，对比 CPU/GPU 在浅层网络上的精度 |
| 2 | Bootstrap 后 Scale 强制重置 | 高 | 修改 GPU bootstrap：不强制设 `final_scale`，改为追踪实际 scale |
| 3 | Mul_const Scale 编码偏差 | 中 | 修改 `Mul_const`：使用固定 `2^56` 编码而非 `op1->scale()` |
| 4 | Rescale 舍入差异 | 低 | 单独 benchmark rescale 精度 |

### 8.5 快速验证实验建议

1. **Bootstrap 精度单测**：构造一个低 level 密文，分别用 CPU 和 GPU bootstrap，对比输出明文差异。这可以直接量化单次 bootstrap 的精度差。

2. **无 Bootstrap 对比**：构造一个不需要 bootstrap 的浅层网络（如单层卷积+ReLU），对比 CPU/GPU 输出。如果精度一致，则确认 bootstrap 是根因。

3. **Scale 追踪修复**：修改 `phantom_lib.cu` 的 `bootstrap_full_3`，移除 `rtncipher.scale() = final_scale`，改为追踪实际 scale。观察精度是否改善。

4. **Mul_const 编码修复**：修改 `Mul_const`，将 `op1->scale()` 替换为固定 `pow(2.0, _scaling_mod_size)`，观察精度是否改善。

### 8.6 关键源码位置

| 文件 | 位置 | 说明 |
|------|------|------|
| `backend/resnet20_cpu.cu` | 全文 | CPU 生成的 FHE 代码 |
| `backend/resnet20_gpu.cu` | 全文 | GPU 生成的 FHE 代码 |
| `rtlib/ant/ckks/src/bootstrap.c:1549` | `Eval_bootstrap` | CPU bootstrap 主函数 |
| `rtlib/ant/ckks/src/chebyshev_impl.c:275` | `Eval_chebyshev` | CPU Chebyshev 求值 |
| `rtlib/ant/ckks/src/evaluator.c:342` | `Rescale_ciphertext` | CPU rescale |
| `rtlib/ant/ckks/src/cipher.c:74` | `Init_ciph_up_scale_plain` | CPU up-scale 元数据 |
| `rtlib/ant/ckks/src/cipher.c:94` | `Init_ciph_down_scale` | CPU down-scale 元数据 |
| `rtlib/phantom/src/phantom_lib.cu:213` | `Mul_const` | GPU 常量乘法 |
| `rtlib/phantom/src/phantom_lib.cu:279` | `Bootstrap` | GPU bootstrap 封装 |
| `rtlib/phantom/src/phantom_lib.cu:136` | `Equal_level` | GPU level 匹配 |
| `phantom-src/src/boot/Bootstrapper.cu:3105` | `bootstrap_full_3` | GPU bootstrap 实现 |
| `phantom-src/src/boot/Bootstrapper.cu:3124` | `rtncipher.scale()=final_scale` | **GPU scale 强制重置** |
| `phantom-src/src/boot/ModularReducer.cu:53` | `modular_reduction` | GPU 模归约（sin/cos+双角） |

## 9. 相关文件

| 文件 | 用途 |
|------|------|
| `fhe_dsl/python/model/resnet/profile_relu_torch.py` | PyTorch 原生 Profile 库模块（推荐） |
| `scripts/profile_relu_vr_torch.py` | PyTorch 原生 CLI 入口脚本 |
| `fhe_dsl/python/model/resnet/profile_relu.py` | ONNX 方案 Profile 库模块 |
| `scripts/profile_relu_vr.py` | ONNX 方案 CLI 入口脚本 |
| `fhe_dsl/python/model/resnet/config.py` | 各模型变体的 relu_vr 配置 |
| `fhe_dsl/python/model/resnet/__init__.py` | `create_pretrained_resnet()` 模型创建 |
| `fhe_dsl/python/model/cifar10.py` | CIFAR-10 数据加载 |
| `fhe_dsl/python/model/cifar100.py` | CIFAR-100 数据加载 |
| `fhe_dsl/python/fhe/frontend/onnx/analyze_onnx_relu.py` | 早期方案（随机输入，已弃用） |
| `docs/resnet20_relu_issue.md` | 原始问题分析文档 |
| `backend/resnet20_cpu.cu` | CPU (ant) 生成的 FHE 推理代码 |
| `backend/resnet20_gpu.cu` | GPU (phantom) 生成的 FHE 推理代码 |