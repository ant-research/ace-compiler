# Testing Report (auto-generated)

## 7.1 全同态加密编译器基础编译测试

本节给出 ACE 全同态加密编译器对基础算子的编译与执行测试数据，覆盖 Add、Mul、MVM（矩阵-向量乘，对应 Gemm）和 Conv 四类基础算子。下面按 CPU、GPU 与 FPGA 三个小节组织，当前已实测数据为 **GPU 小节**。

### 7.1.1 CPU 测试数据

测试环境：CPU-only，Linux x86_64，ACE `ant` 后端（library=ant，device=cpu）。
测试对象与参数：

| 算子 | 对应 ModelSpec | 输入形状 | 说明 |
|---|---|---|---|
| Add | `ADD_OP` | `[1, 4] + [1, 4]` | 逐元素密文加法 |
| Mul | `MUL_OP` | `[1, 4] × [1, 4]` | 逐元素密文乘法 |
| MVM/Gemm | `LINEAR_OP` | `[1, 4]` | 单层 Linear（Gemm） |
| Conv | `CONV2D_OP` | `[1, 3, 16, 16]` | 3×3 Conv2d |

测试命令：

```bash
python3 benchmark/basic_ops/basic_ops.py --library ant --device cpu
```

#### 表 7.1-1 基础算子编译与执行结果（CPU）

| 编号 | 算子 | 输入形状 | 编译时间 (s) | 执行时间 (ms) | 执行结果 |
|---|---|---:|---:|---:|---|
| 1 | Add | `[1, 4] + [1, 4]` | 0.86 | 1.89 | PASS |
| 2 | Mul | `[1, 4] × [1, 4]` | 0.95 | 0.40 | PASS |
| 3 | MVM/Gemm | `[1, 4]` | 0.56 | 0.35 | PASS |
| 4 | Conv | `[1, 3, 16, 16]` | 0.63 | 351.91 | PASS |

> 注：编译时间为清除 `/tmp/ace-compile-cache` 后，从 PyTorch tracing、AIR 导出、`fhe_cmplr` 生成源码到本地 `.so` 编译链接的完整端到端耗时；执行时间为单样本 FHE 推理耗时，包含密钥生成、加密、计算与解密。

当前已实测数据为 **CPU 与 GPU 小节**，FPGA 数据待后续在支持加速器的测试环境中补充。

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

本节给出 ACE 全同态加密编译器对典型神经网络模型端到端编译与执行的测试数据。下面按 CPU、GPU 与 FPGA 三个小节组织，当前已实测数据为 **CPU 与 GPU 小节**。

### 7.2.1 CPU 测试数据

测试环境：CPU-only，Linux x86_64，ACE `ant` 后端（library=ant，device=cpu）。
测试模型：LR、LeNet x²、AlexNet、ResNet-20、VGG16；x2 精度冒烟测试（平方激活，纯密文乘法链验证 CKKS 精度）。
测试命令：

```bash
# CPU ant 后端
pytest tests/regression/lr/test_lr_smoke.py -k ant -v -s
pytest tests/regression/lenet_x2/test_lenet_x2_smoke.py -k ant -v -s
pytest tests/regression/alexnet_x2/test_alexnet_x2_smoke.py -k ant -v -s
pytest tests/regression/resnet_x2/test_resnet20_x2_smoke.py -k ant -v -s
pytest tests/regression/vgg16_x2/test_vgg16_x2_smoke.py -k ant -v -s
```

#### 表 7.2-1 模型编译与执行结果（CPU）

| 编号 | 模型 | 数据集 | 端到端耗时 | 最大绝对误差 | 执行结果 |
|---|---|---:|---:|---|---|
| 1 | LR | smoke | 0.80 s | — | PASS（标准 smoke） |
| 2 | LeNet x² | smoke | — | 1.53e-07 | PASS (abs err<=0.001) |
| 3 | AlexNet | smoke | 1087.98 s (18:07) | 2.33e-08 | PASS (abs err<=0.001) |
| 4 | ResNet-20 | smoke | 439.36 s (07:19) | 5.44e-07 | PASS (abs err<=0.001) |
| 5 | VGG16 | smoke | 1120.37 s (18:40) | 5.67e-05 | PASS (abs err<=0.001) |

> 注：
> - 最大绝对误差 = FHE 推理 logit 与明文 logits 之差的绝对值上限（\|fhe_logits - plain_logits\|），判断标准为 ≤ 0.001。


### 7.2.2 FPGA 测试数据

> 待补充：在 FPGA 加速器上复测相同模型，记录编译/执行时间与结果。

### 7.2.3 GPU 测试数据

测试环境：NVIDIA H20 96GB PCIe，CUDA 12.8，Phantom 后端（library=phantom，device=cuda）。
测试模型：LR（无激活函数，按标准 smoke 通过）、LeNet x²、ResNet-20 x²。
测试命令：

```bash
# LR 标准 smoke
CUDA_VISIBLE_DEVICES=0 python3 -m pytest tests/regression/lr/test_lr_smoke.py -k "phantom" -v -s

# LeNet x² GPU（spec 已更新：输入 /10、phantom icl=18）
CUDA_VISIBLE_DEVICES=0 python3 -m pytest tests/regression/lenet_x2/test_lenet_x2_smoke.py -k "phantom" -v -s

# ResNet-20 x² GPU（spec 已更新：phantom icl=17 + sbm）
CUDA_VISIBLE_DEVICES=0 python3 -m pytest tests/regression/resnet_x2/test_resnet20_x2_smoke.py -k "phantom" -v -s
```

#### 表 7.2-3 模型编译与执行结果（GPU，x²）

| 编号 | 模型 | 数据集 | 编译时间 | 总耗时 | 最大绝对误差 | 执行结果 |
|---|---|---:|---:|---:|---:|---|
| 1 | LR | toy | 6.4 s | 12.8 s | 2.38e-07 | PASS |
| 2 | LeNet x² | CIFAR-10 | 8.0 s | 16.0 s | 8.61e-06 | PASS |
| 3 | ResNet-20 x² | CIFAR-10 | 18.1 s | 39.1 s | 7.45e-09 | PASS |

> 注：
> - 总耗时 = 上述编译 + FHE 推理（密钥生成、加密、计算、解密）的完整 pytest 用例耗时。
> - 最大绝对误差 = FHE 推理 logit 与明文 logits 之差的绝对值上限，判定标准 ≤ 1e-3。


原始日志保存位置：

| 模型 | 日志文件 |
|---|---|
| LR | [`testing/7.2_gpu_lr.log`](7.2_gpu_lr.log) |
| LeNet x² | [`testing/7.2_gpu_lenet_x2.log`](7.2_gpu_lenet_x2.log) |
| ResNet-20 x² | [`testing/7.2_gpu_resnet20_x2.log`](7.2_gpu_resnet20_x2.log) |


## 7.3 神经网络模型全同态加密推理准确性测试

本节给出典型神经网络模型在 FHE 加密推理与明文 PyTorch 推理下的 Top-1 准确率对比。下面按 CPU、GPU 与 FPGA 三个小节组织，当前已实测数据为 **GPU 小节**。

### 7.3.1 CPU 测试数据

测试环境：CPU-only，Linux x86_64，ACE `ant` 后端（library=ant，device=cpu）。
测试模型：AlexNet、ResNet-20、VGG16；每模型推理 100 张 CIFAR-10 图片。
测试命令：

```bash
/app/scripts/resbm_expert.py -n 100
```

（历史日志：`testing/7.3_2026_10_04_15_05.asplos25.log`、`testing/7.3_2026_10_04_15_05_acc_100.log`。）

#### 表 7.3-1 模型加密/明文准确率对比（CPU）

| 编号 | 模型 | 数据集 | 加密准确率 | 非加密准确率 | 差异 | 执行结果 |
|---|---|---:|---:|---:|---:|---|
| 1 | AlexNet | CIFAR-10 | 85.00% | 86.00% | 1.00% | PASS |
| 2 | ResNet-20 | CIFAR-10 | 89.00% | 89.00% | 0.00% | PASS |
| 3 | VGG16 | CIFAR-10 | 96.00% | 96.00% | 0.00% | PASS |

### 7.3.2 FPGA 测试数据

> 待补充：在 FPGA 加速器上测试 LR、LeNet、ResNet-20，各跑 100 张图片，记录加密准确率 / 非加密准确率 / 差异。

### 7.3.3 GPU 测试数据

测试环境：NVIDIA H20 96GB PCIe，CUDA 12.8，Phantom 后端（library=phantom，device=cuda）。
测试模型：LR、LeNet、ResNet-20；每模型推理 100 张图片。

测试脚本已固化到仓库：

```bash
# LR
CUDA_VISIBLE_DEVICES=0 python3 -m pytest benchmark/lr/test_accuracy.py -k "phantom-cuda-100" -v -s

# LeNet
CUDA_VISIBLE_DEVICES=0 python3 -m pytest benchmark/lenet/test_accuracy.py -k "phantom-cuda-100" -v -s

# ResNet-20
CUDA_VISIBLE_DEVICES=0 python3 -m pytest benchmark/resnet/test_accuracy.py::TestResNet20Accuracy::test_accuracy -k "phantom-cuda-100" -v -s
```

#### 表 7.3-2 模型加密/明文准确率对比（GPU）

| 编号 | 模型 | 数据集 | 加密准确率 | 非加密准确率 | 差异 | 执行结果 |
|---|---|---:|---:|---:|---:|---|
| 1 | LR | toy | 100.00% | 100.00% | 0.00% | PASS |
| 2 | LeNet | CIFAR-10 | 58.00% | 74.00% | 16.00% | PASS |
| 3 | ResNet-20 | CIFAR-10 | 95.00% | 95.00% | 0.00% | PASS |

> 注：
> - 加密准确率为 FHE 推理 Top-1 命中数 / 100。
> - 非加密准确率为同一张图片在原 PyTorch 模型上的 Top-1 命中数 / 100。
> - LR 使用固定 toy 权重，准确率 100%。
> - LeNet 使用固定 `lenet_cifar10.pt` 训练权重：100 张 CIFAR-10 图片上明文准确率 74/100、密文准确率 58/100，相差 16 张；两者预测结果一致的为 59/100。

原始日志保存位置：

| 模型 | 日志文件 |
|---|---|
| LR | [`testing/7.3_gpu_lr_accuracy.log`](7.3_gpu_lr_accuracy.log) |
| LeNet | [`testing/7.3_gpu_lenet_accuracy.log`](7.3_gpu_lenet_accuracy.log) |
| ResNet-20 | [`testing/7.3_gpu_resnet20_accuracy.log`](7.3_gpu_resnet20_accuracy.log) |


## 7.4 面向CPU平台的全同态编译系统性能测试

|编号|模型|集|FHE-MP-CNN|ANT-ACE|加速比|
|---|---|---|---|---|---|
|1|ResNet-20|CIFAR-10|2997.94(s)|1476.2(s)|2.030849|
|2|ResNet-32|CIFAR-10|4851.45(s)|2213.3(s)|2.191953|
|3|ResNet-44|CIFAR-10|6657.36(s)|3022.2(s)|2.202819|
|4|ResNet-56|CIFAR-10|8494.23(s)|3813.2(s)|2.227586|
|5|ResNet-110|CIFAR-10|17056.50(s)|7317.0(s)|2.331078|


## 7.5 面向硬件加速器的全同态编译系统性能测试

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
