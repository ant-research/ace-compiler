# Python-C++ 接口规范 (air_gen 模块)

## 一、接口概览

Python 端提供模块化的 API 设计：

| 模块 | 类 | 职责 | 说明 |
|------|-----|------|------|
| `ace.fhe.ir` | `IRBuilder` | IR 构建 | 构建 AIR IR，方法链式调用 |
| `ace.fhe.ir` | `TensorRegistry` | 张量名称注册 | 管理张量名称映射 |
| `ace.air_gen` | `Frontend` | C++ 绑定 | 内部实现，不推荐直接使用 |

### 推荐：IRBuilder + TensorRegistry

```python
from ace.fhe.ir import IRBuilder, TensorRegistry
import torch

# 创建 builder（方法链式调用）
builder = IRBuilder()
builder.begin_function("Main_graph") \
      .add_input("x", [1, 3, 32, 32]) \
      .add_constant("weight", [16, 3, 3, 3], weight_data) \
      .end_function([1, 16, 30, 30])

# 使用便捷方法构建网络
v0 = builder.conv("x", "weight", stride=[1, 1], padding=[1, 1, 1, 1],
                  onnx_name="/conv1/Conv", output_shape=[1, 16, 32, 32])
v1 = builder.relu(v0, output_shape=[1, 16, 32, 32])
v2 = builder.max_pool(v1, kernel_size=[2, 2], stride=[2, 2],
                      output_shape=[1, 16, 16, 16])
v3 = builder.gemm(v2, "fc_weight", "fc_bias",
                  trans_b=1, output_shape=[1, 10])

# 完成并输出
builder.finalize().write_ir("model.B")

# 张量名称注册（用于自定义算子）
x = torch.randn(1, 3, 32, 32)
TensorRegistry.register(x, "input_x")
TensorRegistry.clear()  # 清空注册表
```

### Frontend 类 (内部实现)

```python
from ace import air_gen

# 获取单例（内部实现，不推荐直接使用）
frontend = air_gen.Frontend.instance

# 使用（适合动态 op 名）
frontend.begin_function("Main_graph")
frontend.add_input("x", [1, 3, 32, 32])
frontend.add_constant("weight", [16, 3, 3, 3], weight_data)
frontend.end_function([1, 16, 30, 30])

# 统一操作接口（op 名动态决定）
v0 = frontend.add_operation("conv", ["x", "weight"], 
                            {"strides": [1, 1], "pads": [1, 1, 1, 1]},
                            {"onnx_name": "/conv1/Conv"},
                            [1, 16, 32, 32])
v1 = frontend.add_operation("relu", [v0], {}, {}, [1, 16, 32, 32])

frontend.finalize()
frontend.write_ir("model.B")
```

### 旧 API (deprecated)

```python
import air_gen

# 已废弃，请使用 IRBuilder 或 Frontend.instance
air_gen.begin_air_function("Main_graph")
air_gen.add_air_input("x", [1, 3, 32, 32])
# ...
```

```
┌─────────────────────────────────────────────────────────────────┐
│                    Python (air_gen module)                       │
├─────────────────────────────────────────────────────────────────┤
│  函数级操作                                                       │
│  ├── begin_air_function(func_name)                               │
│  ├── add_air_input(name, shape)                                  │
│  ├── add_air_constant(name, shape, data)                         │
│  ├── add_air_constant_int64(name, shape, data)                   │
│  ├── end_air_function(output_shape)                              │
│  └── finalize_air_function()                                     │
│                                                                   │
│  操作级操作                                                       │
│  └── add_air_operation(op_name, input_names, attrs, metadata)    │
│                                                                   │
│  张量名称管理                                                     │
│  ├── register_tensor_name(data_ptr, name)                        │
│  └── clear_tensor_name_registry()                                │
│                                                                   │
│  输出与调试                                                       │
│  ├── write_air_ir(filename, phase)                               │
│  ├── print_air_ir()                                              │
│  └── print_bfile_ir(filename)                                    │
│                                                                   │
│  状态查询                                                         │
│  ├── is_air_building() → bool                                    │
│  ├── get_output_shape() → List[int]                              │
│  ├── get_air_func_scope() → int                                  │
│  └── get_air_glob_scope() → int                                  │
│                                                                   │
│  Level 管理                                                       │
│  ├── set_air_level(level_name)                                   │
│  └── get_air_level() → str                                       │
│                                                                   │
│  元数据设置                                                       │
│  ├── set_current_op_name(name)                                   │
│  └── set_is_output_op(is_output)                                 │
└─────────────────────────────────────────────────────────────────┘
                               │
                               ▼
┌─────────────────────────────────────────────────────────────────┐
│                    C++ (Frontend / IRBuilder)                     │
└─────────────────────────────────────────────────────────────────┘
```

---

## 二、函数级操作

### 2.1 begin_air_function

**签名**:
```python
def begin_air_function(func_name: str) -> None
```

**功能**: 开始构建一个新的 AIR 函数。

**参数**:
| 参数 | 类型 | 说明 |
|------|------|------|
| `func_name` | `str` | 函数名称，将作为入口点名称 |

**行为**:
1. 清理之前的构建状态
2. 创建新的 `GLOB_SCOPE`
3. 注册所有域 (air::core, nn::core, nn::vector, fhe::sihe, fhe::ckks, fhe::poly)
4. 创建函数对象

**示例**:
```python
air_gen.begin_air_function("Main_graph")
```

---

### 2.2 add_air_input

**签名**:
```python
def add_air_input(name: str, shape: List[int]) -> None
```

**功能**: 添加函数输入参数。

**参数**:
| 参数 | 类型 | 说明 |
|------|------|------|
| `name` | `str` | 输入参数名称 |
| `shape` | `List[int]` | 输入张量形状 |

**行为**:
1. 记录输入名称和形状
2. 在 `EndFunction` 时统一创建参数

**示例**:
```python
air_gen.add_air_input("x", [1, 3, 32, 32])  # NCHW 格式
```

---

### 2.3 add_air_constant

**签名**:
```python
def add_air_constant(name: str, shape: List[int], data: List[float]) -> None
```

**功能**: 添加 float32 常量张量。

**参数**:
| 参数 | 类型 | 说明 |
|------|------|------|
| `name` | `str` | 常量名称 |
| `shape` | `List[int]` | 常量张量形状 |
| `data` | `List[float]` | 常量数据（float32） |

**示例**:
```python
# 添加权重常量
weight_data = [0.1, 0.2, 0.3, 0.4]  # 展平的数据
air_gen.add_air_constant("conv1_weight", [16, 3, 3, 3], weight_data)
```

---

### 2.4 add_air_constant_int64

**签名**:
```python
def add_air_constant_int64(name: str, shape: List[int],  List[int]) -> None
```

**功能**: 添加 int64 常量张量（用于 shape 张量）。

**参数**:
| 参数 | 类型 | 说明 |
|------|------|------|
| `name` | `str` | 常量名称 |
| `shape` | `List[int]` | 常量张量形状 |
| `data` | `List[int]` | 常量数据（int64） |

**示例**:
```python
# 添加 shape 常量（用于 reshape）
air_gen.add_air_constant_int64("reshape_shape", [4], [1, 64, 8, 8])
```

---

### 2.5 end_air_function

**签名**:
```python
def end_air_function(output_shape: List[int]) -> None
```

**功能**: 结束函数定义，创建输出变量。

**参数**:
| 参数 | 类型 | 说明 |
|------|------|------|
| `output_shape` | `List[int]` | 输出张量形状 |

**行为**:
1. 创建所有输入参数（按添加顺序）
2. 创建签名
3. 创建函数作用域
4. 创建输出变量

**示例**:
```python
air_gen.end_air_function([1, 10])  # 输出形状 [batch, num_classes]
```

---

### 2.6 finalize_air_function

**签名**:
```python
def finalize_air_function() -> None
```

**功能**: 完成函数构建，添加返回语句。

**行为**:
1. 创建 `retv` 语句，返回输出变量
2. 将函数标记为程序入口

**示例**:
```python
air_gen.finalize_air_function()
```

---

## 三、操作级操作

### 3.1 add_air_operation

**签名**:
```python
def add_air_operation(
    op_name: str,
    input_names: List[str],
    attrs: Dict[str, Any] = {},
    meta Dict[str, str] = {},
    output_shape: List[int] = []
) -> str
```

**功能**: 添加一个操作到当前函数。

**参数**:
| 参数 | 类型 | 说明 |
|------|------|------|
| `op_name` | `str` | 操作名称（如 "conv", "relu", "add"） |
| `input_names` | `List[str]` | 输入变量名称列表 |
| `attrs` | `Dict[str, Any]` | 操作属性（int, float, List[int], List[float], str） |
| `metadata` | `Dict[str, str]` | 元数据（如 "onnx_name", "is_output"） |
| `output_shape` | `List[int]` | 输出形状（可选，用于类型推断） |

**返回值**: 结果变量名称（如 `"_v0"`, `"_v1"`）

**支持的属性类型**:
| Python 类型 | C++ 类型 |
|-------------|----------|
| `int` | `int` |
| `float` | `float` |
| `List[int]` | `std::vector<int>` |
| `List[float]` | `std::vector<float>` |
| `str` | `std::string` |

**metadata 键**:
| 键 | 说明 |
|----|------|
| `onnx_name` | ONNX 风格的节点名称（用于 pragma 生成） |
| `is_output` | 是否为输出操作（"True" / "False"） |

**示例**:
```python
# 添加 Conv 操作
result = air_gen.add_air_operation(
    op_name="conv",
    input_names=["x", "conv1_weight", "conv1_bias"],
    attrs={
        "stride": [1, 1],
        "padding": [0, 0, 0, 0],
        "dilation": [1, 1],
        "groups": 1
    },
    metadata={"onnx_name": "/conv1/Conv"},
    output_shape=[1, 16, 30, 30]
)

# 添加 ReLU 操作
result = air_gen.add_air_operation(
    op_name="relu",
    input_names=[result],
    metadata={"onnx_name": "/relu1/Relu"}
)

# 添加输出操作
air_gen.add_air_operation(
    op_name="add",
    input_names=["residual", result],
    metadata={"onnx_name": "/add/Add", "is_output": "True"}
)
```

---

## 四、张量名称管理

### 4.1 register_tensor_name

**签名**:
```python
def register_tensor_name(data_ptr: int, name: str) -> None
```

**功能**: 注册张量数据指针到名称的映射。

**参数**:
| 参数 | 类型 | 说明 |
|------|------|------|
| `data_ptr` | `int` | 张量数据指针（`tensor.data_ptr()`） |
| `name` | `str` | 张量名称 |

**用途**: 用于自定义算子执行时查找张量名称。

**示例**:
```python
x = torch.randn(1, 3, 32, 32)
air_gen.register_tensor_name(x.data_ptr(), "input_x")
```

---

### 4.2 clear_tensor_name_registry

**签名**:
```python
def clear_tensor_name_registry() -> None
```

**功能**: 清空张量名称注册表。

**用途**: 在开始新函数构建前调用。

---

## 五、输出与调试

### 5.1 write_air_ir

**签名**:
```python
def write_air_ir(filename: str, phase: str = "ONNX2AIR") -> None
```

**功能**: 将生成的 AIR IR 写入文件。

**参数**:
| 参数 | 类型 | 说明 |
|------|------|------|
| `filename` | `str` | 输出文件路径（如 `"model.B"`） |
| `phase` | `str` | 阶段名称（默认 `"ONNX2AIR"`） |

**示例**:
```python
air_gen.write_air_ir("output/model.B")
```

---

### 5.2 print_air_ir

**签名**:
```python
def print_air_ir() -> None
```

**功能**: 打印生成的 AIR IR 到标准输出。

---

### 5.3 print_bfile_ir

**签名**:
```python
def print_bfile_ir(filename: str) -> str
```

**功能**: 读取 .B 文件并返回 AIR IR 文本。

**参数**:
| 参数 | 类型 | 说明 |
|------|------|------|
| `filename` | `str` | .B 文件路径 |

**返回值**: AIR IR 文本字符串

**示例**:
```python
ir_text = air_gen.print_bfile_ir("model.B")
print(ir_text)
```

---

## 六、状态查询

### 6.1 is_air_building

**签名**:
```python
def is_air_building() -> bool
```

**功能**: 检查是否正在构建 AIR 函数。

---

### 6.2 get_output_shape

**签名**:
```python
def get_output_shape() -> List[int]
```

**功能**: 获取当前函数的输出形状。

---

### 6.3 get_air_func_scope / get_air_glob_scope

**签名**:
```python
def get_air_func_scope() -> int
def get_air_glob_scope() -> int
```

**功能**: 获取函数作用域/全局作用域的指针（用于底层操作）。

---

## 七、Level 管理

### 7.1 set_air_level

**签名**:
```python
def set_air_level(level_name: str) -> None
```

**功能**: 设置当前 Level。

**支持的 Level**:
| Level | 说明 |
|-------|------|
| `"tensor"` | Tensor 层（默认） |
| `"vector"` | Vector 层（未来） |
| `"ckks"` | CKKS 层（未来） |
| `"sihe"` | SIHE 层（未来） |
| `"poly"` | Polynomial 层（未来） |

---

### 7.2 get_air_level

**签名**:
```python
def get_air_level() -> str
```

**功能**: 获取当前 Level 名称。

---

## 八、元数据设置

### 8.1 set_current_op_name

**签名**:
```python
def set_current_op_name(name: str) -> None
```

**功能**: 设置当前操作的名称（用于 ONNX 兼容的 pragma 生成）。

**示例**:
```python
air_gen.set_current_op_name("/conv1/Conv")
# 后续操作将使用此名称生成 pragma
```

---

### 8.2 set_is_output_op

**签名**:
```python
def set_is_output_op(is_output: bool) -> None
```

**功能**: 设置下一个操作是否为输出操作。

**行为**:
- `True`: 结果存储到输出变量（`st` 指令）
- `False`: 结果存储到 PREG（`stp` 指令）

---

## 九、算子属性规范

### 9.1 算子列表

| 算子 | 输入数 | 必需属性 | 可选属性 |
|------|--------|----------|----------|
| `add` | 2 | - | - |
| `sub` | 2 | - | - |
| `mul` | 2 | - | - |
| `div` | 2 | - | - |
| `matmul` | 2 | - | - |
| `concat` | N | `axis` | - |
| `relu` | 1 | - | - |
| `softmax` | 1 | - | `axis` |
| `sqrt` | 1 | - | - |
| `silu` | 1 | - | - |
| `flatten` | 1 | - | `axis` |
| `reshape` | 2 | - | - |
| `max_pool` | 1 | `kernel_size` | `stride`, `padding` |
| `average_pool` | 1 | `kernel_size` | `stride`, `padding` |
| `global_average_pool` | 1 | - | - |
| `conv` | 2-3 | - | `stride`, `padding`, `dilation`, `groups` |
| `gemm` | 2-3 | - | `alpha`, `beta`, `transA`, `transB` |

### 9.2 属性详细说明

#### conv 属性

| 属性 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| `stride` | `List[int]` | `[1, 1]` | 步长 [H, W] |
| `padding` | `List[int]` | `[0, 0, 0, 0]` | 填充 [top, left, bottom, right] |
| `dilation` | `List[int]` | `[1, 1]` | 膨胀率 [H, W] |
| `groups` | `int` | `1` | 分组数 |

#### gemm 属性

| 属性 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| `alpha` | `float` | `1.0` | 缩放因子 A |
| `beta` | `float` | `1.0` | 缩放因子 B |
| `transA` | `int` | `0` | 是否转置 A |
| `transB` | `int` | `0` | 是否转置 B |

#### pooling 属性

| 属性 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| `kernel_size` | `List[int]` | 必需 | 核大小 [H, W] |
| `stride` | `List[int]` | `kernel_size` | 步长 |
| `padding` | `List[int]` | `[0, 0, 0, 0]` | 填充 |

---

## 十、完整使用示例

### 10.1 基本流程

```python
import torch
import ace.air_gen as air_gen

# 1. 开始函数
air_gen.begin_air_function("Main_graph")

# 2. 添加输入
air_gen.add_air_input("x", [1, 3, 32, 32])

# 3. 添加常量（权重）
weight = torch.randn(16, 3, 3, 3)
bias = torch.randn(16)
air_gen.add_air_constant("conv_weight", [16, 3, 3, 3], weight.flatten().tolist())
air_gen.add_air_constant("conv_bias", [16], bias.flatten().tolist())

# 4. 结束函数定义
air_gen.end_air_function([1, 16, 30, 30])

# 5. 添加操作
v0 = air_gen.add_air_operation(
    "conv",
    ["x", "conv_weight", "conv_bias"],
    {"stride": [1, 1], "padding": [1, 1, 1, 1]},
    {"onnx_name": "/conv/Conv"},
    [1, 16, 32, 32]
)

v1 = air_gen.add_air_operation(
    "relu",
    [v0],
    {},
    {"onnx_name": "/relu/Relu"}
)

# 6. 完成函数
air_gen.finalize_air_function()

# 7. 写入文件
air_gen.write_air_ir("model.B")
```

### 10.2 与 TorchTracedModel 集成

```python
# torch_frontend.py 中的使用方式
def execute(self, *inputs):
    # 注册输入张量名称
    for name, tensor in zip(self._input_names, inputs):
        air_gen.register_tensor_name(tensor.data_ptr(), name)
    
    # 开始 AIR 构建
    air_gen.begin_air_function(self._entry_name)
    for name, shape in zip(self._input_names, self._input_shapes):
        air_gen.add_air_input(name, list(shape))
    
    # 添加常量
    for name, (shape, data) in self._constants.items():
        air_gen.add_air_constant(name, shape, data)
    
    air_gen.end_air_function(list(self._output_shape))
    
    # 执行模型（触发自定义算子）
    with torch.no_grad():
        self.traced_model(*inputs)
    
    air_gen.finalize_air_function()
    air_gen.write_air_ir(self._file_path)
```

---

## 十一、设计原则

### 11.1 统一接口原则

**原则**: 所有算子通过 `add_air_operation` 统一接口，不提供单独的算子接口。

**理由**:
1. 易于扩展新算子
2. Python 端代码简洁
3. C++ 端维护成本低

### 11.2 属性类型安全

**原则**: 属性在 C++ 端进行类型转换，支持 int, float, List[int], List[float], str。

**实现**:
```cpp
// pybind_extension.cxx
try {
    ctx._attrs[key] = py::cast<int>(value);
} catch (const py::cast_error&) {
    try {
        ctx._attrs[key] = py::cast<float>(value);
    } catch (...) {
        // ...
    }
}
```

### 11.3 名称传递机制

**问题**: 自定义算子执行时，如何知道张量的名称？

**方案**: 使用 `register_tensor_name` + thread-local 存储。

```cpp
// pybind_extension.cxx
static thread_local std::vector<std::string> g_pending_input_names;

// Python 端在调用算子前设置
air_gen.register_tensor_name(x.data_ptr(), "input_x");
air_gen.set_pending_input_names(["input_x", "weight"]);  // 内部调用

// C++ 端在算子实现中读取
std::string x_name = ctx.Get_Tensor_Name(x.data_ptr(), "x");
```

### 11.4 元数据传递

**用途**: 用于 ONNX 兼容的 pragma 生成。

```python
air_gen.add_air_operation(
    "conv",
    ["x", "w", "b"],
    attrs={"stride": [1, 1]},
    metadata={"onnx_name": "/conv1/Conv", "is_output": "False"}
)
```

**C++ 端处理**:
```cpp
auto it = metadata.find("onnx_name");
if (it != metadata.end()) {
    IRBuilder::Instance().SetOpName(it->second);
    IRBuilder::Instance().EnablePragma(true);
}
```

---

## 十二、待完善项

| 项目 | 状态 | 说明 |
|------|------|------|
| 多 Level 支持 | TODO | vector, ckks, sihe, poly |
| 错误处理 | 待完善 | Python 异常传递 |
| 类型检查 | 待完善 | 属性类型验证 |
| 文档生成 | TODO | 自动生成 API 文档 |
| 单元测试 | 待完善 | 接口测试覆盖 |

---

## 十三、IRBuilder Python API (高级封装)

`IRBuilder` 是对 `Frontend` 的 Pythonic 封装，提供方法链式调用和便捷方法。

### 13.1 导入与创建

```python
from ace.fhe.ir import IRBuilder, TensorInfo

# 创建实例
builder = IRBuilder()

# 检查 C++ 扩展是否可用
if IRBuilder.is_available():
    print("air_gen C++ extension is available")
```

### 13.2 函数级操作

```python
# 开始函数（返回 self，支持方法链）
builder.begin_function("Main_graph")

# 添加输入
builder.add_input("x", [1, 3, 32, 32])
builder.add_input("bias", [64])

# 添加常量
builder.add_constant("weight", [64, 3, 7, 7], weight_data, dtype="float32")
builder.add_constant("shape_tensor", [4], [1, 64, 56, 56], dtype="int64")

# 结束函数定义
builder.end_function([1, 64, 56, 56])

# 完成函数
builder.finalize()
```

### 13.3 操作级操作

#### 通用接口

```python
# add_op: 统一操作接口
result = builder.add_op(
    op_name="conv",
    inputs=["x", "weight", "bias"],
    attrs={"strides": [1, 1], "pads": [3, 3, 3, 3], "group": 1},
    meta={"onnx_name": "/conv1/Conv"},
    output_shape=[1, 64, 56, 56]
)
```

#### 便捷方法

| 方法 | 说明 | 示例 |
|------|------|------|
| `conv()` | 卷积操作 | `builder.conv("x", "weight", "bias", stride=[1,1], padding=[1,1,1,1])` |
| `gemm()` | 矩阵乘法 | `builder.gemm("a", "b", "c", trans_b=1)` |
| `matmul()` | 矩阵乘法 | `builder.matmul("a", "b")` |
| `add()` | 加法 | `builder.add("a", "b")` |
| `sub()` | 减法 | `builder.sub("a", "b")` |
| `mul()` | 乘法 | `builder.mul("a", "b")` |
| `div()` | 除法 | `builder.div("a", "b")` |
| `relu()` | ReLU 激活 | `builder.relu("x")` |
| `silu()` | SiLU 激活 | `builder.silu("x")` |
| `softmax()` | Softmax | `builder.softmax("x", axis=-1)` |
| `sqrt()` | 平方根 | `builder.sqrt("x")` |
| `max_pool()` | 最大池化 | `builder.max_pool("x", kernel_size=[2,2], stride=[2,2])` |
| `average_pool()` | 平均池化 | `builder.average_pool("x", kernel_size=[3,3])` |
| `global_average_pool()` | 全局平均池化 | `builder.global_average_pool("x")` |
| `flatten()` | 展平 | `builder.flatten("x", start_dim=1, end_dim=-1)` |
| `reshape()` | 形状变换 | `builder.reshape("x", "shape_tensor")` |
| `concat()` | 拼接 | `builder.concat(["a", "b"], axis=1)` |

#### 便捷方法示例

```python
# 卷积
v1 = builder.conv("x", "conv1_weight", "conv1_bias",
                  stride=[1, 1], padding=[1, 1, 1, 1],
                  dilation=[1, 1], groups=1,
                  onnx_name="/conv1/Conv",
                  output_shape=[1, 64, 56, 56])

# ReLU
v2 = builder.relu(v1, output_shape=[1, 64, 56, 56])

# 最大池化
v3 = builder.max_pool(v2, kernel_size=[3, 3], stride=[2, 2],
                      padding=[1, 1, 1, 1],
                      output_shape=[1, 64, 28, 28])

# 全连接层 (GEMM)
v4 = builder.gemm(v3, "fc_weight", "fc_bias",
                  alpha=1.0, beta=1.0, trans_a=0, trans_b=1,
                  output_shape=[1, 1000])

# Softmax
output = builder.softmax(v4, axis=1, output_shape=[1, 1000])
```

### 13.4 输出操作

```python
# 写入文件
builder.write_ir("model.B")
builder.write_ir("model.B", phase="ONNX2AIR")

# 打印到标准输出
builder.print_ir()

# 读取 .B 文件并打印
ir_text = IRBuilder.print_bfile_ir("model.B")
print(ir_text)
```

### 13.5 状态查询

```python
# 检查是否正在构建
if builder.is_building():
    print("Currently building a function")

# 获取输出形状
output_shape = builder.get_output_shape()

# 获取作用域句柄
func_scope = builder.get_func_scope()
glob_scope = builder.get_glob_scope()

# 获取/设置 Level
current_level = builder.get_level()
builder.set_level("tensor")

# 获取函数信息
print(f"Function: {builder.function_name}")
print(f"Inputs: {builder.input_names}")
print(f"Input shapes: {builder.input_shapes}")
print(f"Constants: {builder.constants}")
```

### 13.6 张量名称注册

```python
import torch
from ace.fhe.ir import TensorRegistry

x = torch.randn(1, 3, 32, 32)

# 注册张量名称（用于自定义算子）
TensorRegistry.register(x, "input_x")  # 直接传入 Tensor
TensorRegistry.register(x.data_ptr(), "input_x")  # 或传入 data_ptr

# 清空注册表
TensorRegistry.clear()

# 检查是否可用
if TensorRegistry.is_available():
    print("C++ extension is available")
```

### 13.7 完整示例

```python
import torch
from ace.fhe.ir import IRBuilder, TensorRegistry

# 创建权重数据
conv_weight = torch.randn(64, 3, 7, 7)
conv_bias = torch.randn(64)
fc_weight = torch.randn(1000, 64 * 56 * 56)
fc_bias = torch.randn(1000)

# 构建 IR
builder = IRBuilder()

# 函数定义
builder.begin_function("SimpleNet") \
      .add_input("x", [1, 3, 224, 224]) \
      .add_constant("conv_weight", [64, 3, 7, 7], conv_weight.flatten().tolist()) \
      .add_constant("conv_bias", [64], conv_bias.flatten().tolist()) \
      .add_constant("fc_weight", [1000, 64 * 56 * 56], fc_weight.flatten().tolist()) \
      .add_constant("fc_bias", [1000], fc_bias.flatten().tolist()) \
      .end_function([1, 1000])

# 构建网络
v1 = builder.conv("x", "conv_weight", "conv_bias",
                  stride=[2, 2], padding=[3, 3, 3, 3],
                  output_shape=[1, 64, 112, 112])

v2 = builder.relu(v1, output_shape=[1, 64, 112, 112])

v3 = builder.max_pool(v2, kernel_size=[3, 3], stride=[2, 2],
                      padding=[1, 1, 1, 1],
                      output_shape=[1, 64, 56, 56])

v4 = builder.flatten(v3, start_dim=1, end_dim=-1,
                     output_shape=[1, 64 * 56 * 56])

v5 = builder.gemm(v4, "fc_weight", "fc_bias",
                  trans_b=1, output_shape=[1, 1000])

output = builder.softmax(v5, axis=1, output_shape=[1, 1000])

# 完成并输出
builder.finalize().write_ir("simple_net.B")

print(f"Generated IR for {builder.function_name}")
print(f"Inputs: {builder.input_names}")
print(f"Output shape: {builder.get_output_shape()}")
```

### 13.8 模块化 API 设计

Python 端采用模块化设计，职责清晰：

| 模块 | 类 | 职责 | 使用场景 |
|------|-----|------|---------|
| `ace.fhe.ir` | `IRBuilder` | IR 构建 | 构建 AIR IR |
| `ace.fhe.ir` | `TensorRegistry` | 张量名称注册 | 自定义算子执行时查找张量名称 |
| `ace.air_gen` | `Frontend` | C++ 绑定 | 内部实现，不推荐直接使用 |

**使用建议：**
- 用户代码：使用 `IRBuilder` 和 `TensorRegistry`
- 内部实现：可以使用 `Frontend`，但应逐步迁移到 `IRBuilder`