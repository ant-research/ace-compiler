# 跑 FHE-MP-CNN 全同态 ResNet

在包含本仓库的 SDK 目录中执行：

```bash
cd apps/fhe-mp-cnn
make
```

程序在 clib 真实后端上完整执行 FHE-MP-CNN 的同态 ResNet 推理：明文密文声明
（`Encrypt_input` / `Encode_plain`）、卷积、多项式近似 ReLU、Bootstrap、批归一化和
残差相加，最后把每张图的 logits 声明为可验证输出（`Save_output`），由 ANT 解密并
按声明的 2.0 容差验证。看到 `PASS fc_logits_*` 即通过，文件在 `result/`。

首次运行会依次执行编译、导出需求、准备 ANT 上下文、编码加密、计算与验证；
再次 `make` 复用未改变的材料重新计算。**N=65536 时 ANT 上下文准备（258 个旋转
密钥的自举/求值密钥）是单线程长任务，可能耗时数小时到数天**，中途不要打断
（打断后需从头开始）；`make` 各步骤没有设置超时。

## 与其它应用的不同点

其它应用在 `main.cpp` 里手写 clib 算子；本应用包的是既有的 SEAL 3.6 代码
（`baselines/fpga/cnn_ckks`）。`config.mk` 在标准配置之外多做两件事：

1. **源码装配**：`scripts/build_cnn_clib.py --stage-only` 把 cnn_ckks 暂存并在
   暂存副本上打齐 clib 适配补丁（每逻辑层双素数链、逐周期第二次 Bootstrap、
   `save_output`、clib 版 Bootstrapper、数据根宏）。`config.mk` 把这些源文件
   连同 SEAL 兼容层（`baselines/fpga/clib-seal`）一起编进本应用的构建；
   `main.cpp` 只负责按应用约定把图交给 `Run_application`。
2. **数据布局**：权重和测试图片不在本目录，而是 `DATA_DIR` 指向仓库中 gpu
   路线携带的原始数据（`pretrained_parameters/`、`testFile/`、
   `testFile_cifar100/`），编译期以 `-DCNN_DATA_ROOT` 传入绝对路径（作用同其它
   应用的 `CLIB_APP_DATA_DIR`）；CNN 自己的报告文件写入本应用 `result/`
   （`-DCNN_RESULT_DIR`）。ReLU 的 minimax 组合多项式系数文件 `d13.txt`
   （上游 FHE-MP-CNN 随库携带的离线产物，缺失时 CNN 会静默退化成无 ReLU 的
   线性网络）也在 `$(DATA_DIR)/result/` 下，`DATA_FILES` 会检查它存在。

## 模型与图片选择

`config.mk` 中导出以下变量（也可在 make 命令行覆盖）：

```make
export CNN_LAYERS ?= 20     # ResNet 深度：2020/32100 两种见下
export CNN_DATASET ?= 10     # 数据集：10 或 100
export CNN_START ?= 0        # 起始图片 id
export CNN_END ?= 0          # 结束图片 id（每张图片一次完整推理）
```

仅支持有预训练数据的组合：**ResNet-20/CIFAR-10**（默认）与
**ResNet-32/CIFAR-100**（对应 `CNN_LAYERS=32 CNN_DATASET=100`，注意此时
旋转/自举需求不同，ANT 会准备新的上下文）。切换组合后建议 `make distclean`。

## 分步命令与数据检查

```bash
make build     # 编译 .build/app（首次会自动暂存并适配 CNN 源码）
make export    # 导出明文/输入/旋转与计算需求（2351 objects, 258 rotations @ resnet20）
make context   # ANT 准备参数与密钥（长任务，勿设超时）
make encode    # ANT 编码与加密
make run       # 真实后端计算
make verify    # 解密 logits 并按 2.0 容差验证
```

本应用的导出与 cmake 旁路（`scripts/run_cnn_clib.py`，`build/cnn-ckks/cnn`）
逐对象一致，两条路线可共享同一套导出结果。

每一步自动补齐前置步骤。`DATA_FILES` 在导出前检查权重/测试文件是否存在，
缺文件时 Make 直接报错；换模型时它与 `DATA_DIR` 一起在 `config.mk` 的
`ifeq` 分支中选择。

`make clean` 只删除编译产物，保留暂存源码（在
`build/cnn-ckks-src`，为 cmake 旁路共用）与密钥材料；`make distclean`
额外删除本应用的 `work/` 与 `result/`。

## 层级调度（为什么每个周期有两次 Bootstrap）

FHE-MP-CNN 一个周期在两次刷新间消耗 16 个逻辑层级（近似 ReLU 14 + 卷积 2），
而 clib 的 Q=62 Bootstrap 最多刷新到 32 limbs（15 个逻辑层级）。暂存补丁在
周期内 ReLU 后再自举一次，把周期拆成 14 | 2 两个都能放下的口袋——纯层级
重排，数值不变（对应的 SEAL 式 scale 账本也会在每次 Bootstrap 处重置）。细节见
`scripts/build_cnn_clib.py` 与 `baselines/fpga/clib-seal/Bootstrapper.cpp`。

仓库侧的 `scripts/run_cnn_clib.py` 驱动的是同一套导出/上下文/计算/验证流程的
cmake 构建版本（`build/cnn-ckks/cnn`），两者的暂存源码完全一致；本应用目录
是 SDK 约定下的正式入口。