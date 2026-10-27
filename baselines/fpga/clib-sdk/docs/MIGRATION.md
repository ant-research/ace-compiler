# delivery/final-202610 迁移记录

来源仓库：https://github.com/ant-research/ace-compiler
分支：`delivery/final-202610`
固定提交：`7d6166fa56edb05fffeedc06d0b571264189e4f7`

## 相对现有 SDK 的差异

| 远程内容 | 检查结果与本次处理 |
| --- | --- |
| `include/clib.h` | 与本地相同，继续使用本地版本 |
| 标准应用工具、API/APPLICATION 文档、new-app/resnet20 应用 | 与本地基线相同；本次仅扩展多源码应用的构建入口 |
| `libclib.a`、ANT 可执行工具、build.id | 二进制不同，未导入；远程发行包无法证明其 clib 源码版本 |
| SDK 外部的 `clib-seal` | 纳入 SDK，单独生成 `libclib_seal.a`，提供 SEAL CMake target |
| `fhe-mp-cnn` | 迁入完整应用及必要源码；Q58 调度，第二次自举移入 ReLU，并在刷新前后做范围缩放及恢复 |
| `resnet20_eva` | 迁入当前 layer0 + stage1 片段；移除未使用的后续层数据 |
| LeNet/Q58 及本地验证文档 | 保留我们的版本 |
| 外部 build/run/perf 包装脚本 | 不迁入，统一使用应用目录的 `make` |

逐文件核对以本地 clib 提交 `864862f` 为基线：远程 SDK 中能对应上的 29 个已跟踪文件
全部逐字节相同，包括 `clib.h`、原生应用和 ANT 流程脚本。远程 SDK 没有本地的
核心构建源码与测试目录，不能用其随附静态库替代我们的源码构建。
本地未提交的 LeNet/Q58 工作另行保留，不把它误算为远程删除。

FHE-MP-CNN 来源实现保留原始 15/15/27 系数。delivery 中缺少的 `d13.txt`
来自 https://github.com/snu-ccl/FHE-MP-CNN 的固定提交
`a752630b1da3c7ed9e5f7f7a7f9aa817db720fdd`，路径 `cnn_ckks/result/d13.txt`。
随包只携带 CIFAR-10/CIFAR-100 各前三张已有预处理样本，不携带完整测试集。
修正了 CIFAR-100 的标签文件名、权重路径、数值读取检查和系数计数中的数组释放。

迁移保留已有文件中的作者/来源声明。来源仓库可用的法律说明和 Microsoft MIT 文本在 `licenses/`。
这些文本按原文保留，不把 SEAL 兼容实现称为 Microsoft SEAL 本身，也不推断其他来源文件采用同一许可证。

EVA 原片段的输入和首层折叠权重都含 `1/B`（B=40），而完整 FHE-MP-CNN
只有输入及 BN 偏置按此范围归一化。本次保留两者原有数值行为，分别建立独立参考，
不把 EVA 当作完整 FHE-MP-CNN 的等值前缀。
