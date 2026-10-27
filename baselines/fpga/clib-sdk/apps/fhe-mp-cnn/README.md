# FHE-MP-CNN：完整 ResNet 推理

在仓库根目录执行：

```bash
cd soft/clib/sdk/apps/fhe-mp-cnn
make
```

如果使用解压后的发行包，进入 `clib-sdk/apps/fhe-mp-cnn` 执行 `make` 即可。
需要 C++17 编译器、OpenMP、Python 3、NumPy，以及 NTL/GMP 开发库（Ubuntu：`libntl-dev libgmp-dev`）。

默认运行 **ResNet20 / CIFAR-10 / 样本 0 / N65536 / Q58**。
`make` 依次完成编译、导出、ANT context、编码/加密、密文计算、ANT 解密核对和分类报告。

这是完整 CPU 同态推理，运行成本较高。本机默认 ResNet20 的 context 约 45 GiB、
编码材料约 23 GiB、导出数据约 0.9 GiB；计算进程观测内存约 44 GiB。
这些材料由首次 `make` 生成，不包含在发行 ZIP 中。实测时间见 SDK 的 `docs/VALIDATION.md`。
ResNet32 的对应材料约为 52.7 GiB context、36.0 GiB 编码数据和 1.4 GiB 导出数据。

另外两个固定样本及第二个模型：

```bash
make SAMPLE=1
make SAMPLE=2
make MODEL=32              # ResNet32 / CIFAR-100 / 样本 0
make MODEL=32 SAMPLE=1
make MODEL=32 SAMPLE=2
```

## 输入与输出

`data/` 已包含两个模型的完整权重、每个数据集前三个样本和原始 `d13.txt` ReLU 系数。
无需外部数据目录，不在构建时下载或修改源代码。
`main.cpp` 是应用入口，`src/` 是迁入并适配的 FHE-MP-CNN 实现。

中间材料在 `work/resnet<模型>/sample<编号>/`，最终结果在对应的 `result/` 目录：

- `fc_logits_<编号>.json`：ANT 实际解密的槽位，前 10 或 100 项是 logits。
- `verification.json`：真实密文与图中参考值的误差，最终容限为绝对误差 2.0。
- `classification.json`：真实解密 top-1、标签、独立 NumPy 参考 top-1 及三者误差。
- `layer0_relu_<编号>.json`：首层激活的实际解密检查点。
- `runtime.csv`、`breakdown.csv`、`stages.csv`：SDK 实际耗时，单位秒；context 加载与计算分开记录。
- 上游文本报告里的 shadow/reference 是明文镜像，不是真实解密结果。

只查看导出内容可运行 `make export`。重新运行 `make` 会检查并复用匹配的材料。
`make clean` 删除编译产物；`make distclean` 删除所选模型/样本的材料与结果。

## Q 与自举

保留原来的 15/15/27 ReLU 多项式和系数。
Q58 的自举出口是 Q28。循环内先自举，再计算前两段 15 次多项式，
在第三段 27 次多项式之前执行第二次自举，最后完成 ReLU。
第二段多项式后先乘 `1/256` 并 rescale，再降到 Q2 自举，最后用整数乘法恢复 256 倍。
该归一化控制当前 ANT EvalMod 的输入幅度；恢复不消耗层级，原多项式和数学输出不变。
原来 ReLU 后的第二次自举已移到这里，没有增加第三次。
单张图 ResNet20 为 36 次自举，ResNet32 为 60 次。

此迁移保留上游的数学行为：全连接实现读取 bias，但没有把它加到结果中。
独立参考也按实际代码核对；它不等同于补上 bias 后的训练框架模型。
分类是否正确以实际报告为准，数值核对通过不代表一定分类正确。

使用方法和已验证范围见 SDK 的 `docs/VALIDATION.md`；接口说明见 `docs/SEAL.md`。


维护者检查（仅源码仓库提供）：在本应用目录执行 `python3 tests/run.py`，
默认只导出并检查独立网络参考。`--activation` 运行小规模 ReLU 密文回归；`--full` 才执行完整密文验收。
测试使用 `soft/clib/build/sdk/` 下的独立工作副本，不覆盖本应用的材料和结果。
