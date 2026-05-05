# frontend 框架重构计划

## 一、背景与目标

### 1.1 当前状态

frontend 框架负责将 PyTorch 模型或Python 函数转换为 AIR IR。当前架构包含以下核心组件：

**C++ 层 (`csrc/frontend/`)**:
- `ir_builder.h/cxx` - AIR IR 构建器，封装 AIR base API
- `frontend.h/cxx` - 前端管理器，管理不同 level 的 handler
- `tensor.h/cxx` - 自定义 PyTorch 算子实现（torch.ops.tensor.xxx）
- `levels/tensor_level.h/cxx` - Tensor 级别的算子映射
- `pybind_extension.cxx` - Python 绑定

**Python 层 (`python/ace/fhe/frontend/`)**:
- `torch_frontend.py` - Torch FX 直接 trace 方案
- `torch_via_onnx.py` - Torch → ONNX → AIR 方案
- `decorators.py` - `@compile`和`@compute` 装饰器

### 1.2 当前架构问题

通过实现 conv2d 和 gemm 算子支持的过程中，发现以下问题：

1. **算子属性处理分散**:
   - `tensor.cxx`中硬编码属性处理逻辑（如 padding 格式转换）
   - `torch_frontend.py`中 FX 图改写逻辑复杂（gemm/conv 需要特殊处理 weight/bias）
   - 缺少统一的属性传递机制

2. **Tensor Name Registry 设计局限**:
   - 当前使用数据指针映射名称 (`TensorNameRegistry`)
   - 对于 weight/bias 等参数需要额外通过 get_attr 节点存储
   - 名称解析逻辑分散在多处

3. **Level 架构未充分利用**:
   - `LevelBase`设计为可扩展多级别表示（如 AST 级、Tensor 级）
   - 但目前只有 `TensorLevel` 一个实现
   - `Frontend::AddOperation` 中硬编码了 TensorLevel 的 opcode 映射

4. **IRBuilder 状态管理复杂**:
   - 单例模式 (`Instance()`) 导致状态难以管理
   - `BeginFunction` 时需要清理和重建 GLOB_SCOPE
   - `_last_node` 用于属性设置，但不支持多节点并行构建

5. **Python 到 C++ 的参数传递复杂**:
   - FX 图的 kwargs 需要传递到 C++ 算子
   - 当前 conv 算子有 7 个参数（x, w, b, stride, padding, dilation, groups）
   - 缺少标准化的属性包传递机制

### 1.3 重构目标

支持 ResNet 等更大模型转换，需要：

1. **可扩展的算子注册机制**: 支持自定义算子及其属性，我的自定义算子有多层，当前只处理了Tensor层
2. **统一的属性传递框架**: 从 Python FX 图或者AST Tree到 C++ AIR IR 的属性透传
3. **更清晰的层级设计**: 分离 IR 生成逻辑和算子语义逻辑
4. **更好的状态管理**: 支持多函数并行构建
5. **更简洁的 Python-C++ 接口**: 减少硬编码的图改写逻辑


---

## 二、架构设计

### 2.1 整体架构

**前端路径**:
- `torch` - PyTorch 模型 → FX Trace → C++ TensorLevel → AIR
- `torch-via-onnx` - PyTorch 模型 → ONNX → fhe_cmplr → AIR
- `ast` - Python 函数 → AST Trace → C++ ASTLevel → AIR
- `ast-via-onnx` - Python 函数 → ONNX → fhe_cmplr → AIR
- `onnx` - ONNX 文件 → fhe_cmplr → AIR

```
┌─────────────────────────────────────────────────────────────┐
│                    Python Frontend Layer                    │
│  ┌──────────────┐ ┌──────────────┐ ┌──────────────┐        │
│  │TorchFrontend │ │TorchViaOnnx  │ │ ASTFrontend  │        │
│  │ - FX trace   │ │ - ONNX export│ │ - AST trace  │        │
│  │ - Graph rewrite│ │ - ONNX→AIR │ │ - Graph build│        │
│  └──────────────┘ └──────────────┘ └──────────────┘        │
│         │                  │                   │            │
│         │                  ▼                   │            │
│         │          ┌──────────────┐           │            │
│         │          │ fhe_cmplr    │ ◄─────────┘            │
│         │          │ (ONNX→AIR)   │                        │
│         │          └──────────────┘                        │
│         ▼                                                   │
│  ┌────────────────────────┐                                │
│  │   Driver (driver) │                                │
│  └───────────┬────────────┘                                │
└──────────────────┼─────────────────────────────────────────┘
                   │
                   ▼
┌─────────────────────────────────────────────────────────────┐
│                    C++ AIR Gen Layer                        │
│  ┌─────────────────────────────────────────────────────┐    │
│  │                    Frontend                         │    │
│  │  - SetLevel()                                       │    │
│  │  - Begin/EndFunction()                              │    │
│  │  - AddOperation()                                   │    │
│  └─────────────────────┬───────────────────────────────┘    │
│                        │                                     │
│         ┌──────────────┼──────────────┐                     │
│         ▼              ▼              ▼                      │
│  ┌────────────┐ ┌────────────┐ ┌────────────┐               │
│  │TensorLevel │ │  ASTLevel  │ │CustomLevel │               │
│  │(opcode map)│ │ (future)   │ │ (user-def) │               │
│  └────────────┘ └────────────┘ └────────────┘               │
│                        │                                     │
│                        ▼                                     │
│  ┌─────────────────────────────────────────────────────┐    │
│  │                  IRBuilder                          │    │
│  │  - CreateTensorType()                               │    │
│  │  - AddOperation(opcode, inputs, attrs)              │    │
│  │  - SetNodeAttr()                                    │    │
│  └─────────────────────┬───────────────────────────────┘    │
│                        │                                    │
│                        ▼                                    │
│  ┌─────────────────────────────────────────────────────┐    │
│  │               AIR Base API                          │    │
│  │  - GLOB_SCOPE, FUNC_SCOPE, CONTAINER                │    │
│  │  - NODE_PTR, STMT_PTR                               │    │
│  └─────────────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────────┘
```

### 2.1.1 Python 与 C++ 配合设计

**Torch FX Trace 路径**:
1. Python 侧 `torch_frontend.py`:
   - `fx.symbolic_trace(model)` 捕获 FX 图
   - `rewrite_graph_to_custom_ops()` 改写节点为自定义算子
   - 自定义算子 (`torch.ops.tensor.xxx`) 触发 C++ AIR IR 生成

2. C++ 侧 `tensor.cxx`:
   - 自定义算子实现 (`tensor_conv_impl`, `tensor_gemm_impl` 等)
   - 通过 `IR_BUILDER::Instance()` 获取构建器
   - 调用 `Add_Operation()` 生成 AIR IR

3. 属性传递:
   - Python FX 图的 `node.kwargs` → C++ `OP_CONTEXT._attrs`
   - 通过 pybind11 绑定传递 `**kwargs`

**AST Trace 路径**:
1. Python 侧 `ast_frontend.py`:
   - `ast.parse(source)` 解析 Python 函数
   - `ASTTransformer` 遍历 AST 节点
   - 映射 AST 节点到 AIR 算子

2. C++ 侧 `ast_level.cxx` (未来实现):
   - 接收 Python AST 节点信息
   - 生成对应的 AIR IR

3. 配合方式:
   - AST 路径适合简单函数（加减乘除、控制流）
   - Tensor 路径适合复杂模型（Conv, Linear 等）

### 2.2 核心设计决策

#### 2.2.1 算子注册与属性定义

**当前问题**: 每个算子的属性处理逻辑硬编码在 `tensor.cxx` 中

**新设计**: 引入 `OP_SCHEMA` 定义算子签名和属性

```cpp
// include/air/op_schema.h

// 属性类型枚举
enum class ATTR_TYPE {
    INT,      // 单整数
    INTS,     // 整数数组
    FLOAT,    // 单浮点数
    FLOATS,   // 浮点数数组
    STRING,   // 字符串
    TENSOR    // 张量
};

// 算子属性定义
struct OP_ATTRIBUTE {
    std::string           _name;          // 属性名
    ATTR_TYPE             _type;          // 属性类型
    bool                  _required;      // 是否必需
    std::any              _default_value; // 默认值（可选）
    
    OP_ATTRIBUTE(const std::string& name, ATTR_TYPE type, 
                 bool required = false, std::any default_val = {});
};

// 算子 Schema 定义
class OP_SCHEMA {
public:
    OP_SCHEMA(const std::string& name, air::base::OPCODE opcode);
    
    // 链式 API 用于定义 Schema
    OP_SCHEMA& Attr(const std::string& name, ATTR_TYPE type, 
                    bool required = false, std::any default_val = {});
    OP_SCHEMA& Input(const std::string& name, bool required = true);
    OP_SCHEMA& Output(const std::string& name);
    
    // 验证并解析属性
    bool Validate_And_Parse(const pybind11::kwargs& kwargs, 
                            std::map<std::string, std::any>& out_attrs) const;
    
    // Accessors
    const std::string& Name() const { return _name; }
    air::base::OPCODE  Opcode() const { return _opcode; }
    
private:
    std::string                  _name;    // 算子名
    air::base::OPCODE            _opcode;  // 算子 Opcode
    std::vector<OP_ATTRIBUTE>    _attrs;   // 属性列表
    std::vector<std::string>     _inputs;  // 输入列表
    std::string                  _output;  // 输出名
};

// 算子 Schema 注册表（单例）
class OP_SCHEMA_REGISTRY {
public:
    static OP_SCHEMA_REGISTRY& Instance();
    
    void Register(const std::string& name, OP_SCHEMA schema);
    const OP_SCHEMA* Get(const std::string& name) const;
    bool Has(const std::string& name) const;
    
private:
    OP_SCHEMA_REGISTRY() = default;
    std::map<std::string, OP_SCHEMA> _schema_map;
};
```

#### 2.2.2 统一的属性传递机制

**当前问题**: Python kwargs → C++ 参数通过函数签名硬编码传递

**新设计**: 使用 `OP_CONTEXT` 统一传递

```cpp
// include/air/op_context.h

// 算子上下文 - 统一传递输入和属性
struct OP_CONTEXT {
    // 输入张量
    std::vector<at::Tensor> _inputs;
    
    // 属性（从 Python kwargs 解析）
    std::map<std::string, std::any> _attrs;
    
    // Accessors - 单值属性
    template<typename T>
    T Get_Attr(const std::string& key, T default_val) const {
        auto it = _attrs.find(key);
        if (it != _attrs.end()) {
            return std::any_cast<T>(it->second);
        }
        return default_val;
    }
    
    // Accessors - 数组属性
    template<typename T>
    std::vector<T> Get_Attr_Vec(const std::string& key, 
                                 std::vector<T> default_val = {}) const {
        auto it = _attrs.find(key);
        if (it != _attrs.end()) {
            return std::any_cast<std::vector<T>>(it->second);
        }
        return default_val;
    }
    
    // 辅助方法
    bool Has_Attr(const std::string& key) const {
        return _attrs.find(key) != _attrs.end();
    }
    
    size_t Input_Count() const { return _inputs.size(); }
    
    const at::Tensor& Input_At(size_t index) const {
        AIR_ASSERT(index < _inputs.size());
        return _inputs[index];
    }
};

// 算子实现函数类型
using OP_IMPL_FN = std::function<at::Tensor(const OP_CONTEXT& ctx)>;
```

#### 2.2.3 IRBuilder 多上下文支持

**当前问题**: 
- Tensor 层目前只支持单函数构建
- 后续 Level（CKKS、SIHE 等）可能需要多函数并行构建

**新设计**: 支持多上下文，为多 Level 扩展做准备

```cpp
// include/air/ir_builder.h

// 构建上下文 - 封装单个函数构建所需的全部状态
class BUILD_CONTEXT {
public:
    BUILD_CONTEXT();
    ~BUILD_CONTEXT();
    
    // AIR 核心对象
    air::base::GLOB_SCOPE*      _glob;          // 全局作用域
    air::base::FUNC_SCOPE*      _func_scope;    // 函数作用域
    air::base::SPOS             _spos;          // 源位置
    std::unique_ptr<fhe::core::LOWER_CTX> _lower_ctx;
    
    // 符号表
    std::map<std::string, air::base::ADDR_DATUM_PTR> _st_map;    // 变量表
    std::map<std::string, air::base::CONSTANT_PTR>     _cst_map; // 常量表
    
    // 构建状态
    int _stmt_count;        // 语句计数
    int _op_idx;            // 算子索引
    int _intermediate_idx;  // 中间变量索引
    air::base::NODE_PTR _last_node;  // 最后创建的节点（用于设置属性）
    
    // 重置上下文状态
    void Reset();
};

// IR 构建器 - 支持多上下文并行构建
class IR_BUILDER {
public:
    IR_BUILDER();
    ~IR_BUILDER();
    
    // 上下文管理
    BUILD_CONTEXT* New_Context();
    void Set_Current_Context(BUILD_CONTEXT* ctx);
    BUILD_CONTEXT* Get_Current_Context() const;
    void Release_Context(BUILD_CONTEXT* ctx);
    
    // 函数构建 API
    void Begin_Function(const std::string& func_name);
    void Add_Input(const std::string& name, const std::vector<int64_t>& shape);
    void Add_Constant(const std::string& name, const std::vector<int64_t>& shape,
                      const void* data, size_t byte_len);
    void End_Function(const std::vector<int64_t>& output_shape);
    void Finalize_Function();
    
    // 算子构建 API
    std::string Add_Operation(const std::string& op_name,
                              const std::vector<std::string>& input_names,
                              air::base::OPCODE opcode,
                              const std::map<std::string, std::any>& attrs = {});
    
    // 属性设置 API
    void Set_Last_Node_Attr_Int(const std::string& key, int value);
    void Set_Last_Node_Attr_Ints(const std::string& key, const std::vector<int>& values);
    void Set_Last_Node_Attr_Float(const std::string& key, float value);
    void Set_Last_Node_Attr_Floats(const std::string& key, const std::vector<float>& values);
    
    // 输出 API
    void Print_IR();
    void Write_IR(const std::string& filename, const std::string& phase = "ONNX2AIR");
    
    // Accessors
    air::base::FUNC_SCOPE* Get_Func_Scope();
    air::base::GLOB_SCOPE* Get_Glob_Scope();
    bool Is_Building() const;
    std::vector<int64_t> Get_Output_Shape() const;
    
private:
    BUILD_CONTEXT* _current_ctx;
    std::vector<std::unique_ptr<BUILD_CONTEXT>> _contexts;
    
    // 辅助方法
    air::base::TYPE_PTR Create_Tensor_Type(const std::vector<int64_t>& shape);
};
```

#### 2.2.4 Level 架构增强

**当前问题**: Level 只负责 opcode 映射，职责单一

**新设计**: Level 负责算子语义和 AIR IR 生成的映射

```cpp
// include/air/levels/level_base.h

// Level 类型枚举
// AIR 层次定义：tensor, vector, ckks, sihe, poly 等
enum class LEVEL_TYPE {
    TENSOR,     // Tensor 层 - 处理 torch.Tensor 操作 (对应 nn::core)
    VECTOR,     // Vector 层 - 处理向量操作 (对应 nn::vector)
    CKKS,       // CKKS 层 - CKKS 同态加密操作 (对应 fhe::ckks)
    SIHE,       // SIHE 层 - SIHE 同态加密操作 (对应 fhe::sihe)
    POLY        // Polynomial 层 - 多项式操作 (对应 fhe::poly)
};

inline std::string Level_Type_To_String(LEVEL_TYPE type) {
    switch (type) {
        case LEVEL_TYPE::TENSOR:  return "tensor";
        case LEVEL_TYPE::VECTOR:  return "vector";
        case LEVEL_TYPE::CKKS:    return "ckks";
        case LEVEL_TYPE::SIHE:    return "sihe";
        case LEVEL_TYPE::POLY:    return "poly";
        default: return "unknown";
    }
}

// 算子构建信息
struct OP_BUILD_INFO {
    std::string                        _op_name;      // 算子名
    air::base::OPCODE                  _opcode;       // Opcode
    std::vector<std::string>           _input_names;  // 输入名称列表
    std::map<std::string, std::any>    _attrs;        // 属性
};

// Level 基类接口
class LEVEL_BASE {
public:
    virtual ~LEVEL_BASE() = default;
    
    // Level 信息
    virtual std::string Get_Level_Name() const = 0;
    virtual LEVEL_TYPE Get_Level_Type() const = 0;
    
    // 算子支持
    virtual bool Has_Op(const std::string& op_name) const = 0;
    virtual std::vector<std::string> Get_Supported_Ops() const = 0;
    
    // 获取算子构建信息
    virtual bool Get_Op_Build_Info(const std::string& op_name,
                                   const OP_CONTEXT& ctx,
                                   OP_BUILD_INFO& out_info) const = 0;
    
    // Pybind11 注册
    virtual void Register_Py_Ops(pybind11::module& m) = 0;
};

// Level 工厂函数类型
using LEVEL_FACTORY = std::function<std::unique_ptr<LEVEL_BASE>()>;
```

### 2.3 文件结构

```
csrc/frontend/
├── CMakeLists.txt
├── pybind_extension.cxx          # Python 绑定（更新）
├── include/air/
│   ├── ir_builder.h              # IR 构建器（更新：支持多上下文）
│   ├── frontend.h                # 前端管理器（更新：使用新 Level 接口）
│   ├── tensor.h                  # Tensor 算子（更新：使用 OP_CONTEXT）
│   ├── op_schema.h               # 新增：算子 Schema 定义
│   ├── op_context.h              # 新增：算子上下文
│   ├── op_registry.h             # 新增：算子注册表
│   └── levels/
│       ├── level_base.h          # Level 基类（更新：新接口）
│       ├── tensor_level.h        # Tensor Level（更新：新接口）
│       ├── vector_level.h        # Vector Level（未来）
│       ├── ckks_level.h          # CKKS Level（未来）
│       └── poly_level.h          # Poly Level（未来）
└── src/
    ├── ir_builder.cxx            # IR 构建器实现（更新）
    ├── frontend.cxx              # 前端实现（更新）
    ├── tensor.cxx                # Tensor 算子实现（更新）
    ├── op_schema.cxx             # 新增：OpSchema 实现
    ├── op_registry.cxx           # 新增：算子注册表实现
    └── levels/
        ├── level_base.cxx        # Level 基类实现
        ├── tensor_level.cxx      # Tensor Level 实现（更新）
        ├── vector_level.cxx      # Vector Level 实现（未来）
        ├── ckks_level.cxx        # CKKS Level 实现（未来）
        └── poly_level.cxx        # Poly Level 实现（未来）
```

---

## 三、实施计划

### 阶段 1: 基础设施（1天）

**目标**: 建立新的基础架构，不影响现有功能

1. **创建 `op_schema.h/cxx`**
   - 定义 `ATTR_TYPE` 枚举
   - 定义 `OP_ATTRIBUTE` 结构
   - 实现 `OP_SCHEMA` 类（链式 API）
   - 实现 `OP_SCHEMA_REGISTRY` 单例

2. **创建 `op_context.h`**
   - 定义 `OP_CONTEXT` 结构
   - 提供模板方法 `Get_Attr<T>()` 和 `Get_Attr_Vec<T>()`
   - 提供 `Has_Attr()`, `Input_Count()`, `Input_At()` 辅助方法

3. **更新 `ir_builder.h/cxx`**
   - 引入 `BUILD_CONTEXT` 类
   - 实现多上下文管理 (`New_Context`, `Set_Current_Context`, `Release_Context`)
   - 更新所有方法名为下划线风格 (`Begin_Function`, `Add_Operation` 等)
   - 保持向后兼容的 C-style API

4. **创建 `op_registry.h/cxx`**
   - 实现 `Register_All_Ops()` 函数
   - 为每个算子创建独立的注册函数
   - 注册 NN 层所有算子 (add, sub, mul, div, matmul, conv, gemm, relu, softmax, pool 等)

### 阶段 2: Level 架构重构（1 天）

**目标**: 更新 Level 接口，将算子语义与 IR 生成分离

1. **更新 `levels/level_base.h`**
   - 定义 `LEVEL_TYPE` 枚举 (TENSOR, VECTOR, CKKS, POLY, AST)
   - 新增 `OP_BUILD_INFO` 结构
   - 修改虚接口为下划线风格 (`Get_Level_Name`, `Get_Op_Build_Info` 等)

2. **更新 `levels/tensor_level.h/cxx`**
   - 继承自新的 `LEVEL_BASE` 接口
   - 为每个算子实现 `Get_Op_Build_Info` 方法
   - 使用 `OP_SCHEMA` 验证属性

3. **更新 `frontend.cxx`**
   - 使用新的 Level 接口
   - 将 `OP_BUILD_INFO` 传递给 `IR_BUILDER`
   - 更新函数名为下划线风格

### 阶段 3: 属性传递框架（1-2 天）

**目标**: 实现从 Python 到 C++ 的统一属性传递

1. **更新 `pybind_extension.cxx`**
   - 修改 Python 绑定以接受 `**kwargs`
   - 将 kwargs 转换为 `std::map<std::string, std::any>` 传递给 `OP_CONTEXT`
   - 更新所有绑定函数名为下划线风格

2. **更新 `tensor.cxx`**
   - 每个算子实现从 `OP_CONTEXT` 读取属性
   - 移除硬编码的参数列表
   - 更新函数名为下划线风格 (`Tensor_Conv_Impl`, `Tensor_Gemm_Impl` 等)

3. **更新 `torch_frontend.py`**
   - 简化 `Rewrite_Graph_To_Custom_Ops` 逻辑
   - 统一将属性作为 kwargs 传递
   - 规范化函数命名 (下划线风格)

4. **Python 与 C++ 配合**
   - FX 图的 `node.kwargs` → pybind11 → `OP_CONTEXT._attrs`
   - Tensor 名称注册：`register_tensor_name(data_ptr, name)` → `Get_Tensor_Name()`
   - 支持多层级算子调用（Tensor → Vector → CKKS 等）

### 阶段 4: 清理与优化（1 天）

**目标**: 移除冗余代码，优化架构

1. **移除旧的宏定义**
   - `GEN_AIR_OP1`, `GEN_AIR_OP2`, `GEN_AIR_OP3`
   - 替换为统一的 `Build_Op` 调用

2. **简化 TensorNameRegistry**
   - 与 `OP_CONTEXT` 整合
   - 提供统一的 `Get_Tensor_Name` API

3. **添加单元测试**
   - `test_op_schema.cpp` - 算子 Schema 验证
   - `test_op_context.cpp` - 属性解析测试
   - `test_ir_builder_multi_context.cpp` - 多上下文构建测试
   - `test_level_interface.cpp` - Level 接口测试

4. **小算子验证方法**
   - 执行 `./build/_deps/fhe-cmplr-build/driver/fhe_cmplr /work/model/op/*.onnx -O2A:tia`
   - 生成 `*.t` 文件用于检查 AIR 是否正确
   - 对比生成的 `.t` 文件与预期输出

5. **回归测试**
   - 运行 `pytest -m "TestCompileModels not cuda" tests/test_regression/test_torch_frontend.py`
   - 验证 torch frontend 编译的模型（Conv2d, GEMM, Pooling 等）
   - 确保重构后所有测试通过

### 阶段 5: ResNet 支持验证（1 天）

**目标**: 验证重构后的架构支持复杂模型

1. **添加 ResNet20 模型测试**
   - 验证 Conv2d, BatchNorm, ReLU, AvgPool, Linear 等算子
   - 验证 stride, padding, dilation 等属性正确传递
   - 验证多函数并行构建

2. **性能测试**
   - 比较重构前后构建时间
   - 验证内存使用
   - 确保无性能回归

---

## 四、代码规范

### 4.1 头文件规范

所有头文件必须使用 `#ifdef` 保护：

```cpp
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OP_SCHEMA_H
#define AIR_OP_SCHEMA_H

// ... 头文件内容

#endif  // AIR_OP_SCHEMA_H
```

### 4.2 源代码文件规范

所有源文件使用两层 namespace 包裹：

```cpp
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/op_schema.h"

// ... 其他头文件

namespace ace {
namespace frontend {

// ... 实现代码

}  // namespace frontend
}  // namespace ace
```

### 4.3 命名规范

| 类型 | 风格 | 示例 |
|------|------|------|
| 类名 | 全大写，下划线分隔 | `OP_SCHEMA`, `BUILD_CONTEXT` |
| 结构体 | 全大写，下划线分隔 | `OP_ATTRIBUTE`, `OP_CONTEXT` |
| 枚举类 | 全大写，下划线分隔 | `ATTR_TYPE`, `LEVEL_TYPE` |
| 成员函数 | 首字母大写，下划线分隔 | `Get_Attr`, `Register_Op` |
| 成员变量 | 下划线开头 | `_name`, `_attrs` |
| 全局函数 | 下划线分隔 | `Register_All_Ops`, `Convert_Padding_To_Onnx` |
| 宏定义 | 全大写，下划线分隔 | `DEF_OPCODE`, `AIR_ASSERT` |

---

## 五、关键代码示例

### 5.1 新算子注册示例

```cpp
// src/op_registry.cxx

// 注册 Conv 算子
void Register_Conv_Op() {
    OP_SCHEMA schema("conv", air::base::OPCODE(nn::core::NN, nn::core::OPCODE::CONV));
    
    // 定义输入
    schema.Input("x", true)   // 输入张量
          .Input("w", true)   // weight
          .Input("b", false); // bias (可选)
    
    // 定义属性
    schema.Attr("stride", ATTR_TYPE::INTS, false, 
                std::vector<int>{1, 1});
    schema.Attr("padding", ATTR_TYPE::INTS, false, 
                std::vector<int>{0, 0});
    schema.Attr("dilation", ATTR_TYPE::INTS, false, 
                std::vector<int>{1, 1});
    schema.Attr("groups", ATTR_TYPE::INT, false, 1);
    schema.Attr("kernel_shape", ATTR_TYPE::INTS, false); // 从 weight 推导
    
    // 注册到全局注册表
    OP_SCHEMA_REGISTRY::Instance().Register("conv", schema);
}

// 注册所有算子
void Register_All_Ops() {
    // Binary ops
    Register_Add_Op();
    Register_Sub_Op();
    Register_Mul_Op();
    Register_Div_Op();
    Register_Matmul_Op();
    Register_Concat_Op();
    
    // Unary ops
    Register_Relu_Op();
    Register_Softmax_Op();
    Register_Sqrt_Op();
    Register_Silu_Op();
    Register_Flatten_Op();
    
    // Pooling ops
    Register_Max_Pool_Op();
    Register_Average_Pool_Op();
    Register_Global_Average_Pool_Op();
    
    // Linear ops
    Register_Gemm_Op();
    Register_Conv_Op();
}
```

### 5.2 新算子实现示例

```cpp
// src/tensor.cxx

// Conv 算子实现
at::Tensor Tensor_Conv_Impl(const OP_CONTEXT& ctx) {
    IR_BUILDER& builder = IR_BUILDER::Instance();
    
    // 非构建模式，直接返回
    if (!builder.Is_Building()) {
        return ctx.Input_At(0);
    }
    
    // 从 context 读取属性
    auto stride = ctx.Get_Attr_Vec<int>("stride", {1, 1});
    auto padding = ctx.Get_Attr_Vec<int>("padding", {0, 0});
    auto dilation = ctx.Get_Attr_Vec<int>("dilation", {1, 1});
    auto groups = ctx.Get_Attr<int>("groups", 1);
    
    // 获取输入名称
    std::string x_name = Get_Tensor_Name(ctx.Input_At(0), "x");
    std::string w_name = Get_Tensor_Name(ctx.Input_At(1), "w");
    std::string b_name = ctx.Input_Count() > 2 
                         ? Get_Tensor_Name(ctx.Input_At(2), "b") 
                         : "";
    
    // 添加操作
    std::string result = builder.Add_Operation(
        "conv",
        {x_name, w_name, b_name},
        air::base::OPCODE(nn::core::NN, nn::core::OPCODE::CONV)
    );
    
    // 设置属性
    builder.Set_Last_Node_Attr_Ints("strides", stride);
    builder.Set_Last_Node_Attr_Ints("pads", Convert_Padding_To_Onnx(padding));
    builder.Set_Last_Node_Attr_Ints("dilations", dilation);
    builder.Set_Last_Node_Attr_Int("group", groups);
    
    return ctx.Input_At(0);
}

// Padding 格式转换：PyTorch [pad_h, pad_w] → ONNX [top, left, bottom, right]
std::vector<int> Convert_Padding_To_Onnx(const std::vector<int>& padding) {
    std::vector<int> pads_onnx;
    for (int p : padding) {
        pads_onnx.push_back(p);  // top/left
        pads_onnx.push_back(p);  // bottom/right
    }
    return pads_onnx;
}
```

### 5.3 算子签名设计规范

**Tensor 层算子签名与 ONNX 保持一致**:

Tensor 层 (`nn::core`) 的算子签名设计参考 ONNX 标准，原因：
1. 与 `torch-via-onnx` 路径保持一致性
2. 便于从 ONNX 模型迁移到直接 FX trace
3. 属性命名和语义与 ONNX 对齐

例如 Conv 算子：
- ONNX: `Y = conv(X, W, B, strides, pads, dilations, group, kernel_shape)`
- Tensor 层：签名与 ONNX 一致

**其他 Level 完全自定义**:

- `VECTOR` 层：向量操作，自定义签名
- `CKKS` 层：CKKS 同态加密操作，自定义签名
- `SIHE` 层：SIHE 同态加密操作，自定义签名
- `POLY` 层：多项式操作，自定义签名

### 5.4 Python 端简化示例

```python
# python/ace/fhe/frontend/torch_frontend.py

def Rewrite_Graph_To_Custom_Ops(traced_model):
    """
    重写 FX 图以使用自定义算子。
    
    将标准 torch 操作替换为自定义算子 (torch.ops.tensor.xxx)，
    这些算子在执行时生成 AIR IR 而非计算结果。
    """
    graph = traced_model.graph
    rewritten = 0
    
    for node in graph.nodes:
        if node.op == 'call_module':
            module = traced_model.get_submodule(node.target)
            op_name = Get_Module_Op_Name(module)
            
            if op_name is not None:
                # 保存原始模块路径用于属性名前缀
                original_module_target = str(node.target)
                
                # 获取自定义算子
                custom_op = Get_Custom_Op_For_Module(module)
                if custom_op is not None:
                    node.target = custom_op
                    node.op = 'call_function'
                    
                    # 根据算子类型处理属性
                    if op_name == 'conv':
                        # Conv2d: 需要 weight, bias 和卷积属性
                        attr_prefix = original_module_target.replace('.', '_')
                        
                        # 存储 weight/bias 到 traced_model
                        setattr(traced_model, f'{attr_prefix}_weight', module.weight)
                        if module.bias is not None:
                            setattr(traced_model, f'{attr_prefix}_bias', module.bias)
                        
                        # 创建 get_attr 节点
                        with graph.inserting_before(node):
                            weight_node = graph.create_node('get_attr', 
                                                           f'{attr_prefix}_weight', 
                                                           kwargs={})
                            bias_node = graph.create_node('get_attr', 
                                                         f'{attr_prefix}_bias', 
                                                         kwargs={}) if module.bias else None
                        
                        # 构建新的参数列表
                        new_args = list(node.args) + [weight_node]
                        if bias_node:
                            new_args.append(bias_node)
                        node.args = tuple(new_args)
                        
                        # 设置卷积属性
                        node.kwargs = {
                            'stride': list(module.stride),
                            'padding': list(module.padding),
                            'dilation': list(module.dilation),
                            'groups': module.groups
                        }
                    
                    elif op_name == 'gemm':
                        # Linear: 需要 weight 和 bias
                        _Add_Weight_Bias_Attrs(graph, node, module, 
                                              original_module_target)
                    
                    else:
                        # 其他算子：只保留 tensor 参数
                        node.args = tuple(arg for arg in node.args 
                                         if isinstance(arg, (torch.fx.Node, torch.Tensor)))
                        node.kwargs = {}
                    
                    rewritten += 1
    
    if rewritten > 0:
        traced_model.recompile()
    
    return traced_model
```


### 5.5 Resnet20有关信息
- 生成的AIR文本，可以参考文件： @resnet20_cifar10_pre.t
- resnet20 的onnx文件： @/work/model/ace/resnet20_cifar10_pre.onnx
---

## 六、成功标准

1. **代码质量**:
   - 算子属性定义集中化（`op_registry.cxx`）
   - 移除所有硬编码的参数传递
   - 每个算子有明确的 Schema 定义
   - 头文件使用 `#ifdef` 保护
   - 源文件使用两层 namespace 包裹 (`namespace air { namespace gen { ... }}`)

2. **代码风格** (参考 `compiler/air-infra/include/air/base/st.h`):
   - 类/结构体/枚举：全大写，下划线分隔 (`OP_SCHEMA`, `BUILD_CONTEXT`, `ATTR_TYPE`)
   - 方法/函数：首字母大写，下划线分隔 (`Get_Attr`, `Register_Op`, `Convert_Padding_To_Onnx`)
   - 成员变量：下划线开头 (`_name`, `_attrs`, `_contexts`)

3. **功能完整性**:
   - 支持 NN 层所有算子 (参考 `compiler/nn-addon/include/nn/core/opcode_def.inc`):
     - Binary: ADD, SUB, MUL, DIVIDE, MATMUL, CONCAT
     - Unary: RELU, SOFTMAX, SQRT, SILU, FLATTEN
     - Pooling: MAX_POOL, AVERAGE_POOL, GLOBAL_AVERAGE_POOL
     - Linear: GEMM, CONV
   - 正确传递所有属性 (stride, padding, dilation, groups, kernel_shape 等)
   - 支持多函数并行构建
   - 支持多层级扩展 (Tensor, Vector, CKKS, SIHE, Poly 等)

4. **可扩展性**:
   - 添加新算子只需注册 Schema + 实现函数
   - 添加新 Level 只需继承 `LEVEL_BASE` 并实现接口
   - 无需修改 `frontend.cxx` 或 `ir_builder.cxx`

5. **测试覆盖**:
   - 所有现有测试通过
   - ResNet20 模型成功编译和运行 (参考 `resnet20_cifar10_pre.t` 和 `resnet20_cifar10_pre.onnx`)
   - 新增单元测试覆盖新架构
   - 小算子验证：通过 `fhe_cmplr /work/model/op/*.onnx -O2A:tia` 生成 `.t` 文件验证 AIR 正确性
---

## 七、风险与缓解

| 风险 | 影响 | 缓解措施 |
|------|------|----------|
| API 变更影响现有代码 | 高 | 保持向后兼容的 API，逐步迁移 |
| 性能回归 | 中 | 基准测试，优化热点路径 |
| 属性解析开销 | 低 | 使用缓存，避免重复解析 |
| Python-C++ 边界复杂度 | 中 | 清晰的文档和示例 |

---

## 八、参考文件

### 已阅读的代码
- `csrc/frontend/include/air/ir_builder.h`
- `csrc/frontend/src/ir_builder.cxx`
- `csrc/frontend/include/air/frontend.h`
- `csrc/frontend/src/frontend.cxx`
- `csrc/frontend/include/air/tensor.h`
- `csrc/frontend/src/tensor.cxx`
- `csrc/frontend/include/air/levels/tensor_level.h`
- `csrc/frontend/src/levels/tensor_level.cxx`
- `csrc/frontend/pybind_extension.cxx`
- `python/ace/fhe/frontend/torch_frontend.py`
- `python/ace/fhe/frontend/torch_via_onnx.py`
- `python/ace/fhe/decorators.py`
- `python/ace/fhe/compiler/driver.py`
- `tests/test_cases/models/conv.py`
- `tests/test_cases/models/gemm.py`

### 相关架构参考
- `compiler/nn-addon/onnx2air/src/air_stmt.cxx` - ONNX 属性处理参考
- `compiler/nn-addon/include/nn/onnx2air/air_stmt.h`