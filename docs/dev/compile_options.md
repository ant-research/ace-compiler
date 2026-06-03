# FHE 编译器选项设计与使用指南

## 概述

FHE 编译器选项（`vec`, `ckks`, `sihe`, `p2c`）用于控制 `fhe_cmplr` 的编译行为，如多项式模数度（N）、最大槽数（ms）、硬件参数（hw）等。

本文档描述了选项的设计架构、传递路径和使用方式。

## 选项类型

| 选项 | 说明 | 常用参数 |
|------|------|---------|
| `vec` | 向量化选项 | `ms` (max_slots) |
| `ckks` | CKKS 方案参数 | `N` (poly_modulus_degree), `hw` (hamming_weight), `q0`, `sf` |
| `sihe` | SIHE 选项 | `relu_vr_def`, `relu_vr`, `relu_mul_depth` |
| `p2c` | Poly2C 选项 | `fp` (float_precision), `df` (data_file) |

## 架构设计

### 选项传递路径

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           用户调用层                                          │
├─────────────────────────────────────────────────────────────────────────────┤
│  fhe.compile(encrypt_inputs=["x"], vec={"ms":256}, ckks={"N":4096})         │
│                              │                                               │
│                              ▼                                               │
│  decorators.py:                                                             │
│    options = CompileOptions(**kwargs)                     ← 创建 options     │
│    backend_config = {"device": device, **compiler_options} ← 传递选项        │
└─────────────────────────────────────────────────────────────────────────────┘
                               │
                               ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                           Driver 层                                          │
├─────────────────────────────────────────────────────────────────────────────┤
│  driver.py:                                                                 │
│    FHEDriverCompiler(frontend_name, backend_name, backend_config)           │
│    self.backend_impl = FHELibraryBuilder(backend_name, **backend_config)    │
└─────────────────────────────────────────────────────────────────────────────┘
                               │
                               ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                           Backend 层                                         │
├─────────────────────────────────────────────────────────────────────────────┤
│  antlib.py:                                                                 │
│    def __init__(self, device, **kwargs):                                    │
│        self._options = kwargs  ← 存储编译选项                                 │
│                                                                             │
│    def _compile_file(self, ir, output_dir):                                 │
│        option_args = _dict_to_cmd_args(self._options)  ← 转换为命令行        │
│        full_cmd = base_cmd + option_args                                    │
│        # fhe_cmplr input.onnx -VEC:ms=256 -CKKS:N=4096 ...                  │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 核心组件

| 组件 | 文件 | 职责 |
|------|------|------|
| `CompileOptions` | `fhe_dsl/python/fhe/config/compile_options.py` | 定义选项字段，验证参数 |
| `_dict_to_cmd_args()` | `fhe_dsl/python/fhe/config/compile_options.py` | 将字典转换为命令行参数 |
| `decorators.py` | `fhe_dsl/python/fhe/decorators.py` | 接收用户选项，传递给 backend |
| `Backend` | `fhe_dsl/python/fhe/backend/antlib.py` | 存储选项，调用编译器 |

## 使用方式

### 1. fhe.compile / fhe.compute

直接在 API 调用时传递选项：

```python
import ace.fhe as fhe

# 使用 fhe.compute
compute_model = fhe.compute(
    frontend='torch',
    library='antlib',
    device='cpu',
    encrypt_inputs=['x'],
    vec={'ms': 256},
    ckks={'N': 4096, 'hw': 192}
)(model)

result = compute_model(*inputs)

# 使用 fhe.compile
compiled = fhe.compile(
    frontend='torch',
    library='antlib',
    encrypt_inputs=['x'],
    sihe={'relu_vr_def': 2, 'relu_mul_depth': 13}
)(model)

prog = compiled.compile(inputs)
```

### 2. 测试用例默认选项

为测试用例配置默认编译选项，支持三种方式：

#### 方式一：测试用例定义时设置（推荐用于特殊模型）

```python
# tests/test_cases/models/conv.py
from test_cases.base import ModelTestCase

MODEL_CONV_TEST_CASES = [
    ModelTestCase(
        name="conv2d",
        model_class=Conv2dModel,
        example_inputs=(torch.randn(1, 3, 16, 16),),
        encrypt_inputs=["x"],
        compile_options={
            "vec": {"ms": 256},
            "ckks": {"N": 4096}
        }
    ),
]
```

#### 方式二：集中配置文件（推荐用于批量配置）

```python
# tests/test_cases/default_options.py

# 全局默认选项
DEFAULT_COMPILE_OPTIONS = {
    # "vec": {"ms": 256},
    # "ckks": {"N": 8192},
}

# 模型专属选项
MODEL_SPECIFIC_OPTIONS = {
    "conv2d": {
        "vec": {"ms": 256},
        "ckks": {"N": 4096},
    },
    "conv2d_relu": {
        "vec": {"ms": 256},
        "ckks": {"N": 4096},
    },
    "resnet20_cifar10": {
        "ckks": {"N": 8192, "hw": 192},
        "sihe": {"relu_vr_def": 2},
    },
}
```

#### 方式三：环境变量覆盖（工程师调试用）

```bash
# 命令行设置环境变量
ACE_COMPILE_OPTIONS='{"vec": {"ms": 512}, "ckks": {"N": 8192}}' \
    pytest -v tests/test_regression/test_torch_frontend.py -k "conv2d"

# 或在 Python 代码中设置
from test_cases import set_env_options, clear_env_options

# 设置选项
set_env_options({"vec": {"ms": 256}, "ckks": {"N": 4096}})

# 运行测试...

# 清除选项
clear_env_options()
```

### 选项优先级

```
环境变量 ACE_COMPILE_OPTIONS  (最高优先级)
              ↓
ModelTestCase.compile_options
              ↓
MODEL_SPECIFIC_OPTIONS[model_name]
              ↓
DEFAULT_COMPILE_OPTIONS  (最低优先级)
```

## 测试框架集成

### 测试代码使用方式

```python
# tests/test_regression/test_torch_frontend.py
from test_cases import get_compile_options

def test_compute_success(self, model_case, model_inputs, backend, device):
    model = model_case.create_model()
    encrypt_inputs = model_case.encrypt_inputs

    # 获取编译选项（自动合并优先级）
    compile_opts = get_compile_options(model_case.name, model_case.compile_options)

    # 传递给 fhe.compute
    compute_model = fhe.compute(
        frontend="torch",
        library=backend,
        device=device,
        encrypt_inputs=encrypt_inputs,
        **compile_opts
    )(model)

    result = compute_model(*model_inputs)
```

### 添加新模型测试

1. 在 `tests/test_cases/models/` 中定义测试用例
2. 如需特殊选项，在 `default_options.py` 中添加：

```python
MODEL_SPECIFIC_OPTIONS = {
    # 现有选项...
    "my_new_model": {
        "vec": {"ms": 256},
        "ckks": {"N": 4096},
    },
}
```

## 选项参数参考

### vec (向量化选项)

| 参数 | 类型 | 说明 |
|------|------|------|
| `ms` | int | 最大槽数 (max_slots) |

### ckks (CKKS 方案选项)

| 参数 | 类型 | 说明 |
|------|------|------|
| `N` | int | 多项式模数度 (poly_modulus_degree)，常用值：512, 1024, 2048, 4096, 8192, 16384 |
| `hw` | int | Hamming 权重，影响旋转密钥数量 |
| `q0` | int | 首个素数比特数 |
| `sf` | int | 缩放因子比特数 |
| `sbm` | bool | 启用子批量乘法 |

### sihe (SIHE 选项)

| 参数 | 类型 | 说明 |
|------|------|------|
| `relu_vr_def` | int | 默认 ReLU 值范围 |
| `relu_vr` | str | 按操作指定 ReLU 值范围，格式：`/path/op=val;/path/op2=val2` |
| `relu_mul_depth` | int | ReLU 多项式乘法深度 |

### p2c (Poly2C 选项)

| 参数 | 类型 | 说明 |
|------|------|------|
| `fp` | bool | 启用浮点精度 |
| `df` | str | 数据文件路径 |

## 常见问题

### Q: 如何确定模型需要的 N 值？

A: N 值取决于：
1. 输入数据大小
2. 模型复杂度
3. 精度要求

常见配置：
- 小模型（conv2d, linear）：N=4096
- 中等模型（resnet20）：N=8192
- 大模型：N=16384

### Q: 编译时报 "slot size is too small" 错误？

A: N 值太小，需要增大。尝试 N=4096 或 N=8192。

### Q: 如何调试编译选项？

A: 使用环境变量快速测试：

```bash
ACE_COMPILE_OPTIONS='{"ckks": {"N": 8192}}' pytest -v tests/... -s
```

编译日志会显示实际使用的命令行参数：
```
[SUCCESS] Compilation succeeded: fhe_cmplr ... -VEC:ms=256 -CKKS:N=8192
```

## 相关文件

| 文件 | 说明 |
|------|------|
| `fhe_dsl/python/fhe/config/compile_options.py` | CompileOptions 定义和转换函数 |
| `fhe_dsl/python/fhe/decorators.py` | fhe.compile/compute 装饰器 |
| `fhe_dsl/python/fhe/backend/antlib.py` | AntLIB backend 实现 |
| `tests/test_cases/base.py` | ModelTestCase 定义 |
| `tests/test_cases/default_options.py` | 默认选项配置 |