# 跑 ResNet20 两层片段

在解压后的 SDK 目录中执行：

```bash
cd apps/resnet20
make
```

程序执行 `layer3.1.conv1＋BN → Bootstrap → 多项式 ReLU → layer3.1.conv2＋BN`，输出停在残差相加之前。各阶段验证结果会打印为 PASS/FAIL，文件在 `result/`。

用户只维护 `main.cpp` 和 `config.mk`。前者包含原始权重打包、明文声明、计算与参考；后者指定 N1024、Q62 和本应用的 data/ 目录。Makefile 自动处理 ANT 和编译流程。

- 输入、输出：64×8×8，8 个密文，每个密文 512 槽。
- 每层卷积：64→64、3×3、stride=1、padding=1。
- 层级：输入 Q4 → conv1 Q2 → Bootstrap Q32 → ReLU Q8 → conv2 Q6。
- ReLU 深度 12，消耗 24 个物理 Q limbs，R=9 归一化已经融合在第一层权重中。
- 普通阶段输出与多项式参考比较；`exact_relu_*`、`exact_conv2_*` 同时检查激活近似误差。

`pack_weight` 只做普通数值排列及边界置零。实际编码由 ANT 根据 main.cpp 导出的需求完成。改变卷积形状时，需同时修改打包、分块、算子循环和参考；不能只替换原始权重。

分步操作与新应用模板相同：`make export`、`make context`、`make encode`、`make run`、`make verify`。

## 本应用的数据

`data/` 是该应用自己的原始权重、输入和验证参考；配置为 `DATA_DIR = ./data`。复制应用目录时一起复制它。新应用模板不依赖这些文件。

- `conv1.weight`、`conv1.bias`、`conv2.weight`、`conv2.bias`：真实 ResNet20 layer3.1 的两层权重与偏置，已融合 BN。
- `sample0.input`：CIFAR-10 测试样本经过前面网络层后得到的 64×8×8 输入。
- `sample0.conv1`、`sample0.relu`、`sample0.conv2`：相同多项式计算的明文参考。
- `sample0.exact_relu`、`sample0.exact_conv2`：使用精确 ReLU 的参考，用于检查近似误差。

R=9 来自 CIFAR-10 训练集前 32 个样本的范围校准。第一层权重和偏置除以 R，第二层权重乘以 R。运行现有示例不需要 PyTorch，也不需要重新导出模型。


维护者文件（仅源码仓库提供）：权重导出位于 `tools/export_resnet20.py`；
独立参考、第二样本和边界验收位于 `tests/`。在本应用目录执行
`python3 tests/run.py --case numeric` 准备并验证基础样本，随后可执行
`python3 tests/run.py --case second-sample` 或 `--case boundary`。
共享 ReLU 调度工具及系数位于 SDK 的 `tools/relu/`，不参与普通应用的 `make`。
