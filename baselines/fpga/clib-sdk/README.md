# clib-sdk

用 C++ 手写同态计算，ANT 负责参数、密钥、离线编码、输入加密和输出解密。**你只维护 `main.cpp` 和 `config.mk`，改完执行 `make`。**

## 一、目录组成

```text
clib-sdk/
├── include/clib.h         原生 clib 公开接口
├── include/seal/          SEAL API 子集（可选）
├── lib/                  libclib.a，以及可选的 libclib_seal.a
├── tools/                所有应用共用的 ANT 工具和运行规则
├── apps/new-app/         MVM → Bootstrap 新应用模板
├── apps/resnet20/        conv → Bootstrap → ReLU → conv，数据在自己的 data/ 中
├── apps/lenet/           CIFAR-10 LeNet（AvgPool），完整网络应用
├── apps/fhe-mp-cnn/      SEAL 转接的完整 ResNet20/ResNet32 应用
├── cmake/SEAL/           SEAL::seal CMake 导入目标
├── licenses/             第三方许可和来源说明
└── docs/                 API、应用开发和 ANT 说明
```

应用模板中有 `main.cpp`、`config.mk`、`Makefile` 和 `README.md`。Makefile 已经处理编译、静态库链接和工具调用，无需修改。

应用专用的源码、数据、工具和测试放在对应 `apps/<应用>/` 中。源码仓库顶层的 `tests/` 只维护 SDK 公共测试及统一入口，`tools/relu/` 是多个应用共享的 ReLU 开发工具；应用的训练、导出和专用验收位于自己的 `tools/`、`tests/`。发行包只带运行必需的脚本，不带训练工具、测试代码和测试专用数据。`cmake/SEAL/` 是所有外部 CMake 应用共用的接口，`licenses/` 随相关代码交付；普通 `make` 无需操作这两个目录。

运行环境：Linux x86-64，C++17 编译器、Make、Python 3、OpenMP 和 GMP。随包二进制使用 GCC 12 构建。仓库内的 `sdk/` 另外包含内部 `src/`、`tests/` 和构建入口；维护者先在该目录执行 `make build`，再按下文运行应用。发布包已构建完成。

## 二、使用流程

### 1. 跑现有例子

打开终端，进入解压后的 `clib-sdk` 目录，再执行：

```bash
cd apps/new-app
make
```

这个例子通过旋转、密文乘明文和加法计算一个 MVM，再执行 Bootstrap。成功后显示 `PASS mvm` 和 `PASS bootstrap`，结果在当前目录的 `result/`。

要跑两层卷积，从 `clib-sdk` 根目录执行：

```bash
cd apps/resnet20
make
```

该例使用 **N=1024、Q=62**，执行 ResNet20 `layer3.1` 的 `conv1＋BN → Bootstrap → 多项式 ReLU → conv2＋BN`，输出停在残差相加之前。输入、输出均为 64×8×8，各阶段会打印验证结果。

该旧例在本次新生成材料的回归中出现自举精度失败，尚未修复；历史材料通过不代表新材料必然通过。具体结果见 [验证记录](docs/VALIDATION.md)。

完整 LeNet 应用使用 **N=65536、Q=58、P=62**，包括两层卷积、两次 AvgPool、三层全连接和四次多项式 ReLU。从 SDK 根目录运行：

```bash
cd apps/lenet
make
```

网络代码全部在 [apps/lenet/main.cpp](apps/lenet/main.cpp)，只调用现有 `clib.h`。训练权重和固定样本已经提供；完整测试集明文准确率为 62.26%。当前 Q58 版本的三个固定样本均已通过完整密文验收；每个样本检查 19 组输出的全部 32768 槽。实测耗时、短链选择和八次自举的安排见 [LeNet 说明](apps/lenet/README.md)。

通过 SEAL 转接运行完整 ResNet20（N65536/Q58），从 SDK 根目录执行：

```bash
cd apps/fhe-mp-cnn
make
```

支持 ResNet32/CIFAR-100，选择方法见 [FHE-MP-CNN 说明](apps/fhe-mp-cnn/README.md)。
该应用还需要 NTL/GMP 开发库和 NumPy。其真实分类结果由 ANT 解密后报告，SEAL 的 shadow 输出仅作参考。

原生 clib 的另一种卷积打包示例在 `apps/resnet20_eva`，进入目录执行 `make`。
它使用 N8192/Q62，计算止于 layer0 + stage1，具体见 [EVA 说明](apps/resnet20_eva/README.md)。
SEAL 接口边界见 [转接说明](docs/SEAL.md)，远程代码差异见 [迁移记录](docs/MIGRATION.md)。

第一次 `make` 会准备 ANT 材料；后续 `make` 会复用未改变的材料，重新计算并验证。

### 2. 创建自己的应用

从 `clib-sdk` 根目录执行，创建一个名为 `my-app` 的应用目录：

```bash
mkdir apps/my-app
cp apps/new-app/{main.cpp,config.mk,Makefile,README.md} apps/my-app/
cd apps/my-app
```

这里只复制开发文件；即使之前跑过模板，也不会复制它生成的密钥和结果。接下来只改两个文件：

| 文件 | 你要写什么 |
| --- | --- |
| `config.mk` | N、上下文 Q、SDK 路径；有外部数据时再设置数据位置 |
| `main.cpp` | 数据读取与槽位打包、输入和权重声明、算子调用、输出及明文参考 |

`config.mk` 的最小配置如下，直接保留模板默认值即可：

```make
SDK_DIR ?= ../..
N = 1024
Q = 38
DATA_FILES =
```

`N` 是环维度，每个密文有 `N/2` 个槽位。`Q` 是整套上下文的物理 Q limbs 数。把应用目录放到 SDK 外时，将 `SDK_DIR` 改为 SDK 的实际路径，无需设置环境变量。

### 3. 在 main.cpp 中写计算

下面是一个完整的小应用，可直接替换模板的 `main.cpp`。它计算每个槽位的 `y = x × 0.125`，并检查所有槽位的误差：

```cpp
#include "clib.h"
#include <vector>
using namespace clib;

void Main_graph(CONTEXT ctx) {
    const size_t slots = Context_info(ctx).slots;
    std::vector<double> input(slots), weight(slots, 0.125);
    std::vector<double> reference(slots);
    for (size_t j = 0; j < slots; ++j) {
        input[j] = (int(j % 31) - 15) / 64.0;
        reference[j] = input[j] * weight[j];
    }

    CIPHER x = Encrypt_input(ctx, "input", input.data(), slots, 4);
    PLAIN w = Encode_plain(ctx, "weight", weight.data(), slots, 4);
    CIPHER product = New_ciph(ctx);
    CIPHER y = New_ciph(ctx);

    Begin_stage(ctx, "multiply");
    Mul_plain(ctx, product, x, w);
    Rescale_ciph(ctx, y, product);  // Q4 → Q2
    End_stage(ctx);

    Save_output(ctx, "answer", y, reference.data(), slots, 1e-8);

    Free_plain(w);
    Free_ciph(x);
    Free_ciph(product);
    Free_ciph(y);
}

int main(int argc, char** argv) {
    return Run_application(argc, argv, Main_graph);
}
```

在 `my-app` 目录执行：

```bash
make
```

成功后显示 `PASS answer`。结果密文为 `result/answer.ct`，解密数值为 `result/answer.json`，验证报告为 `result/verification.json`。这些文件由工具生成，不需要手写 JSON。

这里 `Encrypt_input`、`Encode_plain` 最后的 `4` 表示**该对象使用 Q4**；`config.mk` 的 `Q=38` 表示**整套上下文的 Q**。两者不同。改变上下文参数或计算公式时，要同时检查对象层级与 rescale、Bootstrap 的安排。

写卷积、MVM 或 ReLU 时，在 `Main_graph` 中组合基础算子即可。槽位布局、卷积分块和边界掩码由应用编写，SDK 不会根据模型形状自动生成。实际卷积打包和 ReLU 算子写法见 [ResNet20 main.cpp](apps/resnet20/main.cpp)，旋转和自举写法见 [新应用模板](apps/new-app/main.cpp)。修改计算后，也要同步修改独立的明文参考。

有外部权重或输入时，在 `config.mk` 中增加：

```make
DATA_DIR = ./data
DATA_FILES = $(wildcard $(DATA_DIR)/*.txt)
```

文件模式按你的数据文件修改。`main.cpp` 可通过 `CLIB_APP_DATA_DIR` 取得数据目录的绝对路径，再用普通 C++ 文件读取代码加载数据。具体写法见 [应用指南](docs/APPLICATION.md)。

## 三、每一步做什么

### main.cpp 如何生成离线编码需求

`make` 自动完成以下流程：

```text
编译 main.cpp
    ↓
同一份程序运行导出模式，生成 encode.inc 和打包数值
    ↓
ANT 准备参数、密钥、编码上下文和自举材料
    ↓
ANT 按 encode.inc 编码明文、加密输入
    ↓
程序运行计算模式，加载对应材料并执行 clib 算子
    ↓
ANT 解密输出，与应用给出的明文参考比较
```

例如，应用只在使用权重的位置写一次：

```cpp
PLAIN w = Encode_plain(ctx, "weight", weight.data(), slots, 4);
```

| 模式 | 这一行做什么 |
| --- | --- |
| 导出模式 | 记录名称 `weight`、打包数值、槽位数和 Q4，生成 ANT 编码需求 |
| 计算模式 | 检查当前声明与材料是否一致，加载 ANT 编好的对应明文 |

导出时会执行普通 C++ 数据读取、循环和槽位打包；同态算子只记录计算需求，不真正执行卷积或 Bootstrap。`Run_application` 负责模式切换和上下文管理，应用无需另写导出程序。

`work/export/encode.inc` 是自动生成、供 SDK 配套 ANT 工具读取的任务清单，无需手写，也不是编译器生成的 C++ 编码源码。`Encrypt_input` 同样声明输入加密需求，`Save_output` 指定输出及验证参考。旋转需求也由算子调用自动收集。

导出和运行直接比较对象名称、类型、实际槽位数值、槽位数、Q 和计算依赖，并检查对象所属上下文。相同名称重复声明时，数值和层级必须一致。这些检查用于发现声明和上下文混用；不提供对材料任意内容修改的完整性检测。数学公式、槽位布局和实际计算结果由明文参考验证。

**当前导出和计算模式都会执行应用的数据读取与打包，因此运行时仍需保留 main.cpp 读取的原始数据文件。** 计算模式加载 ANT 已生成的材料，不在算子内部调用 ANT。

### 分步命令与输入输出

平时只需 `make`。需要检查某一步时，在应用目录使用：

| 命令 | 输入 | 输出或行为 |
| --- | --- | --- |
| `make build` | `main.cpp`、配置、头文件和静态库 | 编译 `.build/app` |
| `make export` | 应用程序、配置、原始输入和权重 | `work/export/` 中的 `encode.inc`、槽位数值、参考和计算记录 |
| `make context` | 导出的 N/Q、旋转需求 | ANT 生成或复用参数、密钥、编码上下文和自举材料 |
| `make encode` | 导出清单、槽位数值、ANT 上下文 | ANT 编码的 `.pt` 明文和加密的 `.ct` 输入 |
| `make run` | 应用程序、原始数据、ANT 材料 | clib 计算结果密文及性能记录 |
| `make verify` | 本次计算输出、ANT 私有材料、明文参考 | 解密数值、误差报告及 PASS/FAIL |
| `make` | 上述输入 | 自动完成全部步骤，直到验证结束 |

每一步自动执行前置步骤。例如 `make encode` 会先导出需求并准备上下文；`make verify` 会先执行本次计算，再解密验证。

ANT 的调用已由 Makefile 处理。需要单独了解工具接口时，见 [ANT 工具说明](docs/ANT.md)。

### 生成的文件在哪里

所有生成文件都在当前应用目录内：

| 位置 | 内容 |
| --- | --- |
| `.build/` | 应用可执行文件 |
| `work/export/` | `encode.inc`、`raw/` 打包数值、`refs/` 明文参考和计算记录 |
| `work/context/` | 当前 ANT 参数、密钥及自举材料；含 evaluation/private 目录 |
| `work/encoded/` | 当前编码明文、输入密文和用于直接比较的槽位数值 |
| `result/` | 输出密文、解密数值、验证报告和性能 CSV |

FHE-MP-CNN 将 `work/`、`result/` 再按模型和样本分目录，具体路径见应用 README。

`Save_output` 中的名称决定输出名称。例如 `"answer"` 对应 `answer.ct` 和 `answer.json`；原始模板则输出 `mvm` 和 `bootstrap` 两组结果。

## 四、其它

### 参数、公开接口和线程

原生 clib 应用只需包含 `clib.h`，链接 `libclib.a`；模板 Makefile 已处理。不需要内部 `rt_iie.hh`，也不需要自己维护 `param.hh`。参数由 ANT 材料加载，同一份静态库支持运行时 N=1024、8192、65536，无需按应用重新编译库。当前实测结果及新材料的精度限制见 [验证记录](docs/VALIDATION.md)。

当前上下文 Q 支持 32～62 的偶数，Q/P 各不超过 62，DS=1。每次 `Rescale_ciph` 消耗两个物理 Q limbs。ResNet20 示例的层级为 `输入 Q4 → conv1 Q2 → Bootstrap Q32 → ReLU Q8 → conv2 Q6`，其中 ReLU 深度为 12，消耗 24 个物理 limbs。

应用顺序调用 API，同一进程一次使用一个计算上下文；较大算子内部使用 32 线程。`Run_application` 管理上下文，`Main_graph` 中不再手动调用 `Open_context`、`Close_context` 或 `Enable_profile`；申请的明文和密文仍需用 `Free_*` 释放。

### 耗时、复用与清理

SDK 性能 CSV 的时间单位是**秒**（FHE-MP-CNN 来源文本日志中的 `ms` 是毫秒）：`stages.csv` 记录阶段墙钟时间，`breakdown.csv` 记录公开 API 的调用次数和累计耗时，`runtime.csv` 记录参数与线程信息。`end_to_end_without_open` 不包含上下文加载，也不包含导出和 ANT 材料准备时间。

改完应用或数据后重新 `make`。只改权重、输入数值会复用上下文；修改 N/Q、旋转集合或库构建版本后，会准备新上下文，成功后替换本应用的旧上下文。工作目录只保留当前版本。

- `make clean`：只删除 `.build/`，保留已准备材料和结果。
- `make distclean`：同时删除 `work/` 和 `result/`，包括本应用生成的密钥。

更多内容见 [应用指南](docs/APPLICATION.md)、[API 条件与函数说明](docs/API.md) 和 [ANT 工具说明](docs/ANT.md)。旧版材料需重新生成，直接加载接口仍可使用本版本工具生成的材料。
