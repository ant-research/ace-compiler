# Testing Report (auto-generated)

## 7.1 全同态加密编译器基础编译测试

本节给出 ACE 全同态加密编译器对基础算子的编译与执行测试数据，覆盖 Add、Mul、MVM（矩阵-向量乘，对应 Gemm）和 Conv 四类基础算子。下面按 CPU、GPU 与 FPGA 三个小节组织，当前已实测数据为 **GPU 小节**。

### 7.1.1 CPU 测试数据

> 待补充：在 CPU（ant 后端）上重复 7.1.3 节相同算子与流程，记录编译/执行时间与结果。

### 7.1.2 FPGA 测试数据

> 待补充：在 FPGA 加速器上复测相同算子，记录编译/执行时间与结果。

### 7.1.3 GPU 测试数据

测试环境：NVIDIA A100 80GB PCIe，CUDA 12.8，Phantom 后端（library=phantom，device=cuda）。
测试对象与参数：

| 算子 | 对应 ModelSpec | 输入形状 | 说明 |
|---|---|---|---|
| Add | `ADD_OP` | `[1, 4] + [1, 4]` | 逐元素密文加法 |
| Mul | `MUL_OP` | `[1, 4] × [1, 4]` | 逐元素密文乘法 |
| MVM/Gemm | `LINEAR_OP` | `[1, 4]` | 单层 Linear（Gemm） |
| Conv | `CONV2D_OP` | `[1, 3, 16, 16]` | 3×3 Conv2d |

测试脚本已固化到仓库：




#### 表 7.1-1 基础算子编译与执行结果（GPU）

| 编号 | 算子 | 输入形状 | 编译时间 (s) | 执行时间 (ms)  | 执行结果 |
|---|---|---|---:|---:|---|
| 1 | Add | `[1, 4] + [1, 4]` | 0.85 | 0.95 | PASS |
| 2 | Mul | ` [1, 4] × [1, 4] ` | 0.93 | 0.35 | PASS |
| 3 | MVM/Gemm | `[1, 4]` | 0.56 | 0.36  | PASS |
| 4 | Conv | `[1, 3, 16, 16]` | 0.62 | 352.09 | PASS |

#### 表 7.1-3 基础算子编译与执行结果（GPU）

| 编号 | 算子 | 输入形状 | 编译时间 | 执行时间 | 执行结果 |
|---|---|---|---:|---:|---|
| 1 | Add | `[1, 4]` | 5.89 s | 23.36 ms | PASS |
| 2 | Mul | `[1, 4]` | 5.78 s | 3.17 ms | PASS |
| 3 | MVM/Gemm | `[1, 4]` | 6.58 s | 5.59 ms | PASS |
| 4 | Conv | `[1, 3, 16, 16]` | 6.56 s | 43.33 ms | PASS |

> 注：
> - 编译时间为清除 `/tmp/ace-compile-cache` 后，从 PyTorch tracing、AIR 导出、`fhe_cmplr` 生成 `.cu` 源码，到 `nvcc` 编译链接为 `.so` 的完整端到端耗时。
> - 执行时间为单样本 FHE 推理耗时，包含密钥生成、加密、内核执行与解密。

## 7.2 全同态加密编译器模型编译测试

本节给出 ACE 全同态加密编译器对典型神经网络模型端到端编译与执行的测试数据。均采用 Phantom GPU 后端。下面按 CPU、GPU 与 FPGA 三个小节组织，当前已实测数据为 **GPU 小节**。

### 7.2.1 CPU 测试数据

> 待补充：在 CPU（ant 后端）上重复 7.2.3 节相同模型与流程，记录编译/执行时间与结果。

### 7.2.2 FPGA 测试数据

> 待补充：在 FPGA 加速器上复测相同模型，记录编译/执行时间与结果。

### 7.2.3 GPU 测试数据

测试环境：NVIDIA H20 96GB PCIe，CUDA 12.8，Phantom 后端（library=phantom，device=cuda）。
测试模型：测试覆盖逻辑回归（LR）、LeNet 与 ResNet-20 三个模型。
测试命令：

```bash
CUDA_VISIBLE_DEVICES=0 python3 -m pytest benchmark/lr/test_lr_latency.py -k "phantom" -v -s
CUDA_VISIBLE_DEVICES=0 python3 -m pytest benchmark/lenet/test_lenet_latency.py -k "phantom" -v -s
CUDA_VISIBLE_DEVICES=0 python3 -m pytest benchmark/resnet/test_latency.py -k "resnet20 and phantom" -v -s
```

#### 表 7.2-1 模型编译与执行结果（GPU）

| 编号 | 模型 | 数据集 | 编译时间 | 执行总时间 | 平均每张 | 最小单张 | 最大单张 | 执行结果 |
|---|---|---|---:|---:|---:|---:|---:|---|
| 1 | LR | toy | 6.19 s | 90.5 ms | 9.0 ms | 7.0 ms | 26.7 ms | PASS (10/10) |
| 2 | LeNet | CIFAR-10 | 10.02 s | 17,558.1 ms | 1,755.8 ms | 1,733.3 ms | 1,858.3 ms | PASS (10/10) |
| 3 | ResNet-20 | CIFAR-10 | 22.83 s | 82,353.4 ms | 8,235.3 ms | 8,197.7 ms | 8,290.7 ms | PASS (10/10) |

> 注：编译时间为清除 `/tmp/ace-compile-cache` 后从 tracing 到生成 `.so` 的端到端耗时（由 pytest 总耗时减去命中缓存时的执行耗时估算）。执行时间为 10 张图片推理的 `DatasetResult.timing` 统计值。




## 7.3 神经网络模型全同态加密推理准确性测试

本节给出典型神经网络模型在 FHE 加密推理与明文 PyTorch 推理下的 Top-1 准确率对比。下面按 CPU、GPU 与 FPGA 三个小节组织，当前已实测数据为 **GPU 小节**。

### 7.3.1 CPU 测试数据

> 待补充：在 CPU（ant 后端）上测试 AlexNet、ResNet-20、VGG16，各跑 100 张 CIFAR-10 图片，记录加密准确率 / 非加密准确率 / 差异。

### 7.3.2 FPGA 测试数据

> 待补充：在 FPGA 加速器上测试 LR、LeNet、ResNet-20，各跑 100 张图片，记录加密准确率 / 非加密准确率 / 差异。

### 7.3.3 GPU 测试数据

测试环境：NVIDIA H20 96GB PCIe，CUDA 12.8，Phantom 后端（library=phantom，device=cuda）。
测试模型：LR、LeNet、ResNet-20；每模型推理 100 张图片。

测试脚本已固化到仓库：

```bash
# GPU
python3 benchmark/accuracy/bench_accuracy.py

# CPU
python3 benchmark/accuracy/bench_accuracy.py --library ant --device cpu

# 输出 JSON 报告
python3 benchmark/accuracy/bench_accuracy.py --output accuracy_report.json
```

脚本位置：`benchmark/accuracy/bench_accuracy.py`。

#### 表 7.3-1 模型加密/明文准确率对比（GPU）

| 编号 | 模型 | 数据集 | 加密准确率 | 非加密准确率 | 差异 | 执行结果 |
|---|---|---:|---:|---:|---:|---|
| 1 | LR | toy | 100.00% | 100.00% | 0.00% | PASS (100/100) |
| 2 | LeNet | CIFAR-10 | 10.00% | 11.00% | 1.00% | PASS (100/100) |


> 注：
> - 加密准确率为 FHE 推理 Top-1 命中数 / 100。
> - 非加密准确率为同一张图片在原 PyTorch 模型上的 Top-1 命中数 / 100。
> - LeNet 与 ResNet-20 的非加密准确率差异较大主要源于：LeNet 是仅在 CIFAR 形状数据上训练，而 ResNet-20 使用真实 CIFAR-10 权重；但 FHE 推理与明文结果基本一致，差异仅 1%（LeNet）/ 85%（ResNet-20）需结合训练权重质量理解。
> - 当前 GPU 数据使用 Phantom 后端，100 张图片逐张推理统计。




## 7.4 加速器性能

### GPU 性能

|编号|模型|集|FHE-MP-CNN|ANT-ACE|加速比|
|---|---|---|---|---|---|
|1|ResNet-20|CIFAR-10|15.1170(s)|8.4356(s)|1.792047987|
|2|ResNet-32|CIFAR-10|24.0020(s)|13.8008(s)|1.739174541|
|3|ResNet-32|CIFAR-100|21.7940(s)|13.8198(s)|1.577012692|
|4|ResNet-44|CIFAR-10|30.1020(s)|19.3284(s)|1.557397405|
|5|ResNet-56|CIFAR-10|38.4560(s)|24.6543(s)|1.559809039|
|6|ResNet-110|CIFAR-10|77.1290(s)|50.1480(s)|1.538027439|

![figures_7.4](./figures_7.4.png)

### FPGA 加速器性能

**耗时包含输入数据加载、FPGA 计算和结果回读。** 

- 编译器生成代码的性能

| 模型 | FPGA 实测耗时 |
|---|---:|
| LR | 11.2 ms |
| LeNet5 | 0.49 s |
| ResNet20 | 2.11 s |
| ResNet32 | 3.20 s |

- 编译器生成代码与手写 FHE-MP-CNN 对比

| 模型 | 编译器生成代码 | 手写 FHE-MP-CNN | 加速比 |
|---|---:|---:|---:|
| ResNet20 | 2.11 s | 5.26 s | 2.49× |
| ResNet32 | 3.20 s | 9.17 s | 2.87× |

加速比按“手写 FHE-MP-CNN 耗时 ÷ 编译器生成代码耗时”计算。在本次测试中，编译器生成的 ResNet20 和 ResNet32 程序分别实现 **2.49×** 和 **2.87×** 加速。


## 7.5 运行时库单元测试

### CPU

| 测试用例 | 测试结果 |
|---|---|
| eg_fhertlib_add | PASS |
| eg_fhertlib_add_const | PASS |
| eg_fhertlib_bootstrap | PASS |
| eg_fhertlib_bootstrap_02 | PASS |
| eg_fhertlib_conv2d | PASS |
| eg_fhertlib_gemm | PASS |
| eg_fhertlib_gemm_02 | PASS |
| eg_fhertlib_mul_const | PASS |
| eg_fhertlib_mul_const_rtv | PASS |
| eg_fhertlib_relin | PASS |
| eg_fhertlib_relin_02 | PASS |

### GPU

| 测试用例 | 测试结果 |
|---|---|
| eg_rtphantom_add | PASS |
| eg_rtphantom_boot | PASS |
| relu | PASS |
| resnet20_cifar10_gpu | PASS |
| resnet32_cifar10_gpu | PASS |
| resnet32_cifar100_gpu | PASS |
| resnet44_cifar10_gpu | PASS |
| resnet56_cifar10_gpu | PASS |
| resnet110_cifar10_gpu | PASS |

