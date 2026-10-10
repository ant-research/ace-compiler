# 写自己的应用

[返回 README](../README.md) · [API](API.md) · [ANT](ANT.md)

复制 `apps/new-app`，只维护 `main.cpp` 和 `config.mk`。SDK 提供 Makefile 和全部工具。

## 1. main.cpp 的结构

```cpp
#include "clib.h"
using namespace clib;

void Main_graph(CONTEXT ctx) {
    // 1. 读取原始数值，按自己的布局生成槽位向量。
    // 2. Encrypt_input 声明加密输入，Encode_plain 声明编码明文。
    // 3. 调用 Rotate_ciph、Mul_plain、Bootstrap 等基础算子。
    // 4. Save_output 指定结果和明文参考。
    // 5. Free_* 释放全部句柄。
}

int main(int argc, char** argv) {
    return Run_application(argc, argv, Main_graph);
}
```

完整可编译代码见 [模板 main.cpp](../apps/new-app/main.cpp)。`Run_application` 负责创建/关闭上下文、选择导出或计算模式，以及异常清理。函数内不再调用 Open_context、Close_context 或 Enable_profile；需要统计阶段时直接使用 Begin_stage/End_stage。

## 2. 声明与使用放在同一处

下面假设 values 已装入恰好 N/2 个实数，x 与各输出句柄已创建：

```cpp
PLAIN w = Encode_plain(ctx, "conv1/weight", values.data(), values.size(), 4);
Mul_plain(ctx, product, x, w);
Rescale_ciph(ctx, output, product);  // Q4 -> Q2
Free_plain(w);
```

- 导出时，Encode_plain 记录名称、数值、槽位数、Q 和 degree 1；乘法和 rescale 只传播元数据。
- ANT 按导出的记录产生真正编码明文。
- 计算时，同一个 Encode_plain 调用核对声明，然后加载对应 ANT 对象；不在计算阶段调用 ANT。

对象名称由应用命名；文件路径由工具管理。相同名称、相同需求可以重复声明；相同名称对应不同数值或 Q 会报错。例如同一系数需在 Q30、Q28 使用，分别命名 `minus_one_q30`、`minus_one_q28`。

输入写法：

```cpp
CIPHER x = Encrypt_input(ctx, "input", input.data(), input.size(), 4);
```

输出和验证写法：

```cpp
Save_output(ctx, "answer", output, reference.data(), reference.size(), 1e-4);
```

得到 `result/answer.ct`，make verify 后有 `answer.json` 和验证报告。参考也必须恰好 N/2 个实数；所有槽位都会比较。若只需保存和解密，不做数值比较，传 `nullptr, 0, 0`。

原始数值、系数与参考均由应用提供；SDK 不会从矩阵形状自动猜测打包方式，也不会自动生成新的模型算子。

## 3. config.mk

```make
SDK_DIR ?= ../..
N = 1024
Q = 38
DATA_DIR = ./data
DATA_FILES = $(wildcard $(DATA_DIR)/*.txt)
```

模板没有外部数据时 DATA_FILES 留空。使用数据目录时，Makefile 把绝对路径传给 C++ 宏 `CLIB_APP_DATA_DIR`，main.cpp 可用它读取文件。移动应用到 SDK 外时，只需调整 SDK_DIR。

每次 make export 都执行同一份 main.cpp 的普通数据读取/打包，因此原始数据改变会反映到新的需求中。DATA_FILES 还可以列出必须存在的输入文件，让 Make 在缺文件时直接报错。

## 4. 矩阵和卷积怎样打包

模板用三个循环对角线实现 MVM：

```text
y[j] = w0[j]*x[j] + w1[j]*x[(j+1) mod slots] + w2[j]*x[(j+17) mod slots]
```

对可放入 slots×slots 的矩阵，补零后取 `d_s[j] = A[j,(j+s) mod slots]`。每条非零对角线声明一个明文，旋转输入、逐槽相乘，再把同层级乘积累加，最后一次 rescale。超过一个块的矩阵按输入列/输出行分块。

卷积采用相同组合，但打包权重还必须包含 padding 掩码。槽位旋转跨全部 slots 循环，不会自动理解图像行或通道。空间越界位置必须将相应明文权重置零。

[ResNet20 main.cpp](../apps/resnet20/main.cpp) 给出具体实现：

- 每块 8 个通道 × 8×8 = 512 槽；64 个通道分为 8 块。
- 旋转步数为 `(64*k + 8*dy + dx) mod 512`，k=0…7，dy/dx=-1…1。
- `pack_weight` 读取原始 W，按输出块、输入块、旋转填充每槽权重并处理边界。
- 在使用处调用 Encode_plain；导出自动收集这份权重及实际旋转，不维护另一份 Python 实现。

修改形状时，要一起修改打包、分块循环和明文参考。完整网络还需要其它卷积、下采样与残差分支，当前示例止于残差相加之前。

## 5. 深度与导出限制

公开乘法要求同 Q、degree 1 输入，产生 degree 2；rescale 消耗两个物理 limbs，回到 degree 1。加法前需要显式对齐分支，Bootstrap 输入为 Q2/degree 1。

两层示例的预算：

| 位置 | Q |
| --- | ---: |
| 输入 | 4 |
| conv1＋BN 后 | 2 |
| Bootstrap 后 | 32 |
| 7 次多项式后 | 26 |
| 15 次多项式后 | 18 |
| 13 次多项式后 | 10 |
| ReLU 合成后 | 8 |
| conv2＋BN 后 | 6 |

ReLU 深度为 `3+4+4+1=12`，消耗 24 个物理 limbs。R=9 归一化已融合到 conv1 权重。

导出无需模数或密钥，不创建系数。Context_info/Cipher_info 的 N、slots、Q 以及 Scale_degree 可用于调度；导出时实际 scale 不可用，INFO.scale 为 NaN，不能据此决定执行分支。普通循环和条件必须在两种模式下产生一致的计算需求。库会核对每次逻辑值更新，发现新增/缺失操作或依赖改变时报错。

Run_application 目前在两个模式下都执行应用的数值生成与打包，以核对原始槽位数值。因此执行新应用时也需保留它读取的数据。仅加载密文的既有应用继续使用 Open_context/Load_ciph 等直接接口。

## 6. 哪些东西自动检查

- 对象名称、类型、N/2 槽数、Q、degree、实际槽位数值。
- 应用可执行文件与导出记录是否对应。
- 算子顺序、参数、输入依赖与输出层级；复用句柄时按逻辑值版本区分。
- ANT 上下文标识、已编码对象的 N/Q/类型、成功运行标记。
- 图结束后是否遗漏操作或未释放对象。

声明和缓存的槽位数值直接比较；上下文标识用于识别不同材料。工具不检测任意材料内容修改。布局公式和计算结果由 Save_output 的独立明文参考检查。建议从小尺寸和全部槽位验证开始，再增加层数。
