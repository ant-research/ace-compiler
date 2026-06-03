# Constant Comparison: onnx2air vs Torch Frontend

## Overview

This document compares how constants (weights and biases) are handled between:
1. **Reference**: `onnx2air` (fhe_cmplr O2A path)
2. **Ours**: Torch Frontend (FX trace → AIR IR)

---

## 差异二：常量添加顺序不同

### 现象

第一个 NN.conv 算子的常量索引不同：

| 实现 | 第一个 conv 的常量 | fc/gemm 的常量 |
|------|-------------------|----------------|
| 参考 | CST[0x2], CST[0x3] | CST[0], CST[0x1] |
| 我们 | CST[0], CST[0x1] | CST[0x2b], CST[0x2c] |

### 详细对比

**参考实现 (onnx2air) 常量顺序：**
```
CST[0]    -> fc_weight      (最后添加，最先使用)
CST[0x1]  -> fc_bias
CST[0x2]  -> conv1_weight   (按ONNX节点顺序)
CST[0x3]  -> conv1_bias
CST[0x4]  -> layer1_0_conv1_weight
...
```

**我们的实现 (Torch Frontend) 常量顺序：**
```
CST[0]    -> conv1_weight   (按网络执行顺序)
CST[0x1]  -> conv1_bias
CST[0x2]  -> layer1_0_conv1_weight
...
CST[0x2b] -> fc_weight      (最后)
CST[0x2c] -> fc_bias
```

### 原因

- **参考实现 (onnx2air)**：ONNX模型中常量按特定顺序遍历，fc权重在前
- **我们的实现**：按PyTorch模型执行顺序遍历，conv在前，fc在后

### 功能正确性

**这是差异，不是问题：**

1. 两种方式都正确地加载了所有权重和偏置
2. 每个操作都能正确引用对应的常量
3. 不影响计算结果
4. 常量索引只是内部标识符，只要引用关系正确即可

### 已修复 ✅

2024-04-15: 已修改 `torch_trace.py` 中的常量添加顺序，先添加 fc 权重，再按网络顺序添加 conv 权重。现在 CST 索引与参考实现完全对齐：
- `CST[0], CST[0x1]` -> fc_weight, fc_bias
- `CST[0x2], CST[0x3]` -> conv1_weight, conv1_bias
- ...

---

## Type ID Allocation Comparison

### Reference (onnx2air)
From `resnet20_cifar10_pre.t`:

```
TYP[0x17] - Input type (FML)
TYP[0x18] - Output type (VAR)
TYP[0x19-0x45] - Constant types (weights/biases)
TYP[0x46-0x7a] - PREG types (intermediate results)
```

**Key observations:**
- First PREG type: `TYP[0x46]` (array, size:65536)
- PREG range: 0x46 - 0x7a (52 PREGs)
- Constants are created BEFORE PREGs

### Torch Frontend (Ours)
From `torch_trace.t` (generated):

```
TYP[0x17] - Input type (FML)
TYP[0x18] - Output type (VAR)
TYP[0x19-0x46] - Constant types (weights/biases)
TYP[0x47-0x7a] - PREG types (intermediate results)
```

**Key observations:**
- First PREG type: `TYP[0x47]` (array, size:65536)
- PREG range: 0x47 - 0x7a (51 PREGs, missing one)
- Constants seem to use ONE extra type ID

## Constant Count Comparison

### ResNet20 Constants

| Constant Type | onnx2air | Torch Frontend | Match |
|--------------|----------|----------------|-------|
| Conv weights | 21 | 21 | ✓ |
| Conv biases | 21 | 21 | ✓ |
| FC weight | 1 | 1 | ✓ |
| FC bias | 1 | 1 | ✓ |
| **Total** | **44** | **44** | ✓ |

### Type ID Assignment

| Item | onnx2air | Torch Frontend | Diff |
|------|----------|----------------|------|
| Input type | 0x17 | 0x17 | ✓ |
| Output type | 0x18 | 0x18 | ✓ |
| First constant | 0x19 | 0x19 | ✓ |
| Last constant | 0x44 | 0x45 | **+1** |
| First PREG | 0x46 | 0x47 | **+1** |
| Last PREG | 0x7a | 0x7a | ✓ |

## Root Cause Analysis

### Key Finding: Type ID Shift

| | Reference (onnx2air) | Ours (Torch Frontend) | Diff |
|---|---|---|---|
| First PREG type | TYP[0x46] | TYP[0x47] | +1 |
| Last PREG type | TYP[0x7a] | TYP[0x79] | -1 |
| Total PREGs | 52 | 51 | -1 |
| Missing PREG type | TYP[0x78] not used for PREG | TYP[0x46] not used | - |

### Detailed Comparison

**Reference PREG Types:**
- First 10: 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f
- Last 10: 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x79, 0x7a
- Note: TYP[0x78] is NOT used for PREG (used for constant CST[0x2c])

**Our PREG Types:**
- First 10: 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50
- Last 10: 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79
- Note: TYP[0x46] is NOT used at all

### Root Cause: Missing Reshape Intermediate PREG

The reference has an extra PREG:
- `PREG[0x10000032] TYP[0x79](array,"_noname",size:16)` - intermediate reshape

This PREG is used in the reshape operation between avgpool and fc:
```
// Reference:
stp PREG[0x10000033]  // reshape result [1, 64]
  NN.reshape RTYPE[0x7a]
    ldp PREG[0x10000031] RTYPE[0x77]  // avgpool output [1, 64, 1, 1]
    ldp PREG[0x10000033] RTYPE[0x7a]  // reshape output [1, 64]
```

Our implementation:
```
// Ours:
stp PREG[0x10000032]  // reshape result [1, 64]
  NN.reshape RTYPE[0x79]
    ldp PREG[0x10000031] RTYPE[0x78]  // avgpool output [1, 64, 1, 1]
```

We're missing the second input to reshape (the intermediate PREG with size:16).

### Type Creation Order (onnx2air)

From debug output:
```
[onnx2air] Convert_io_tensor_type: input dims=[1,3,32,32]     -> TYP[0x17]
[onnx2air] Convert_io_tensor_type: output dims=[1,10]         -> TYP[0x18]
[onnx2air] Convert_tensor_type: fc.weight dims=[10,64]           -> TYP[0x19]
[onnx2air] Convert_tensor_type: fc.bias dims=[10]               -> TYP[0x1a]
[onnx2air] Convert_tensor_type: onnx::Conv_208 dims=[16,3,3,3]  -> TYP[0x1b]
...
[onnx2air] Convert_tensor_type: onnx::Conv_269 dims=[64]        -> TYP[0x44]
[onnx2air] Get_tensor_sym_or_preg: /conv1/Conv_output_0        -> TYP[0x46] (PREG)
```

### Type Creation Order (Torch Frontend)

From `IRBuilder`:
1. `BeginFunction` - creates new GLOB_SCOPE
2. `AddInput` - creates input types (TYP[0x17])
3. `AddConstant` - creates constant types (TYP[0x19-0x44])
4. `AddOperation` - creates PREG types during operation processing

## Root Cause Identified

### The Missing PREG

The reference creates a PREG for the reshape shape tensor, but we use a constant:

**Reference reshape:**
```
NN.reshape
  ldp PREG[0x10000031] RTYPE[0x77]  // avgpool output
  ldp PREG[0x10000033] RTYPE[0x7a]  // shape tensor as PREG!
```

**Our reshape:**
```
NN.reshape
  ldp PREG[0x10000031] RTYPE[0x78]  // avgpool output
  ldc CST[0x2a] RTYPE[0x44]         // shape tensor as constant
```

### Why This Causes the Type Shift

1. The reference creates 52 PREGs (including one for the reshape shape)
2. We create only 51 PREGs (shape is a constant, not a PREG)
3. All PREG type IDs after the reshape are shifted by 1

### The Sequence

1. First PREG in reference: TYP[0x46]
2. First PREG in ours: TYP[0x47] (shifted by 1)
3. Last PREG in reference: TYP[0x7a]
4. Last PREG in ours: TYP[0x79]

## 结论：这是差异，不是问题

### 功能正确性

两种实现方式在功能上是等价的：

| 方面 | 参考实现 (onnx2air) | 我们的实现 |
|------|---------------------|------------|
| reshape输入 | `ldp PREG + ldp PREG` | `ldp PREG + ldc CST` |
| 语义 | 从PREG读取shape | 从常量表读取shape |
| 结果 | 正确传递shape信息 | 正确传递shape信息 |

**两种方式都是合理的实现选择，功能上没有问题。**

### 差异原因

- **onnx2air**：将shape tensor作为中间结果（PREG）处理
- **我们的实现**：将shape tensor作为常量（CST）处理

这是设计选择的不同，不是bug。

### 何时需要修复

只有当目标是**完全对齐onnx2air输出格式**时才需要修改：
- 需要与某个下游工具链完全兼容
- 需要做精确的diff比较
- 有明确的格式对齐要求

如果只是功能正确性，这个差异可以保留。

---

## Solution (可选，仅当需要完全对齐时)

Modify the reshape handling to create a PREG for the shape tensor instead of using a constant. This requires changes in:

1. **Python side (torch_trace.py)**: Pass the shape tensor as an intermediate result, not a constant
2. **C++ side (frontend.cxx/tensor_level_handler.cxx)**: Handle reshape to create a PREG for the shape input

## Action Items (可选，仅当需要完全对齐时)

1. **Modify reshape handling**: Create a PREG for the shape tensor instead of using a constant
2. **Verify PREG count**: After fix, should have 52 PREGs matching reference
3. **Verify type alignment**: PREGs should start at TYP[0x46] and end at TYP[0x7a]

## Files Involved

- `csrc/frontend/src/ir_builder.cxx` - Type creation logic
- `csrc/frontend/src/frontend.cxx` - Frontend type coordination
- `csrc/frontend/src/tensor_level_handler.cxx` - PREG type creation

---

## Pragma Comment ID 对比

### 发现

pragma comment_id 是基于字符串表中评论文本的字符串ID，而不是一个计数器。

**参考实现 (onnx2air):**
```cpp
STMT_PTR stmt = cntr->New_comment(node->name().c_str(), spos);
uint32_t op_name = stmt->Node()->Comment_id().Value();  // 使用字符串ID
stmt = cntr->New_pragma(core::PRAGMA_OP_START, op_code, op_name, spos);
```

**我们之前的实现:**
```cpp
comment_id = IRBuilder::Instance().GetNextCommentId();  // 使用计数器（错误）
```

**修复后的实现:**
```cpp
air::base::STMT_PTR comment_stmt = cntr->New_comment(op_comment.c_str(), spos);
uint32_t comment_id = comment_stmt->Node()->Comment_id().Value();  // 使用字符串ID（正确）
```

### 当前状态

修复后，comment_id 现在正确使用字符串ID。但是：

1. **第一个 comment_id 差异**: 我们的第一个 comment_id 是 0x26，参考是 0x21。差异为 5，表示字符串表中有 5 个额外的字符串在第一个评论之前。

2. **重复的评论文本**: 某些 relu 操作的评论文本重复（例如 `/layer1/layer1.0/relu/Relu` 出现两次），这是因为 torch_frontend.py 中的节点命名问题。

### 参考 comment_id 模式

```
conv1:  0x21 (字符串 "/conv1/Conv" 的ID)
relu:   0x27 (字符串 "/relu/Relu" 的ID)
conv:   0x28 (字符串 "/layer1/layer1.0/conv1/Conv" 的ID)
relu:   0x29 (字符串 "/layer1/layer1.0/relu/Relu" 的ID)
conv:   0x2a (字符串 "/layer1/layer1.0/conv2/Conv" 的ID)
add:    0x2b (字符串 "/layer1/layer1.0/Add" 的ID)
relu:   0x2c (字符串 "/layer1/layer1.0/relu_1/Relu" 的ID)
```

### 待修复

1. **字符串表顺序对齐**: 需要确保字符串以相同的顺序添加到字符串表中。
2. ~~**节点命名修复**: 需要修复 torch_frontend.py 中的节点命名，确保每个操作有唯一的名称（例如 `relu` vs `relu_1`）。~~ ✅ 已修复

### 已修复 ✅

1. **Pragma comment_id 机制**: 已修复为使用字符串ID而不是计数器，与 onnx2air 行为一致。
2. **ReLU 节点命名**: 已修复，现在正确使用 `relu` 和 `relu_1` 区分同一模块的多次调用。
3. **常量添加顺序**: 已修复，fc 权重先于 conv 权重添加。

### 当前状态

**完全对齐的部分:**
- ATTR 属性顺序和内容
- 常量 (CST) 索引
- ReLU 节点命名 (relu vs relu_1)
- Pragma comment_id 模式（从第二个操作开始完全匹配）

**剩余差异:**
- 第一个操作的 comment_id: 我们是 0x26，参考是 0x21（差值为 5，由于字符串表初始化顺序不同）
- reshape 操作的 shape tensor 处理方式不同（我们使用 CST，参考使用 PREG）- 这是可接受的差异
