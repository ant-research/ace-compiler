# 测试内容

## 准备 Docker 构建与测试环境

从 Docker Hub 拉取预构建镜像（与 `delivery/final-202610-v2` GitHub Actions 工作流构建并推送的镜像一致，详见 `.github/workflows/build-image.yml`）：

```bash
docker pull opencc/ace:final-202610-v2
```

每个测试项启动一个独立容器，通过 `--name` 区分以避免冲突。CPU 测试项（§1 / §2）：

```bash
docker run -it -d --name test_<n> opencc/ace:final-202610-v2 bash
docker exec -it test_<n> bash
```

- GPU 测试项（§3）——透传 GPU：`--gpus '"device=1"'`
- FPGA 测试项（§4）——宿主机需配备 FPGA 板卡并准备 `fheacc-software-platform` 仓库，详见 `baselines/fpga/README.md`。

```bash
cd ace-compiler
./scripts/dev-build.sh
```

## 1. 面向 CPU 全同态加密编译原型系统

对应 `testing/report.md` §7.1（CPU）/ §7.2（CPU）/ §7.3（CPU）。覆盖 CPU 后端的基础算子编译执行、神经网络模型端到端编译与 FHE 推理精度测试。

- 环境准备

```bash
docker run -it -d --name test_1 opencc/ace:final-202610-v2 bash
docker exec -it test_1 bash
```

- 构建

```bash
cd ace-compiler
./scripts/dev-build.sh
```

- 测试

```bash
# 基础算子（report 7.1）：Add / Mul / MVM / Conv2d
python3 benchmark/basic_ops/basic_ops.py --library ant --device cpu
```

## 2. CPU 运行时原型系统

对应 `testing/report.md` §7.4。覆盖 ResNet 系列（ResNet-20/32/44/56/110）在 CPU 上的 FHE 推理性能，对比 FHE-MP-CNN 基线与 ANT-ACE 预编译产物。

- 环境准备

```bash
docker run -it -d --name test_2 opencc/ace:final-202610-v2 bash
docker exec -it test_2 bash
```

- 构建

```bash
cd ace-compiler
./scripts/dev-build.sh
```

- 测试

```bash
# 模型编译冒烟测试（report 7.2）：LR / LeNet x² / AlexNet / ResNet-20 / VGG16
pytest tests/regression/lr/test_lr_smoke.py -k ant -v -s
pytest tests/regression/lenet_x2/test_lenet_x2_smoke.py -k ant -v -s
pytest tests/regression/alexnet_x2/test_alexnet_x2_smoke.py -k ant -v -s
pytest tests/regression/resnet_x2/test_resnet20_x2_smoke.py -k ant -v -s
pytest tests/regression/vgg16_x2/test_vgg16_x2_smoke.py -k ant -v -s

# 推理准确率（report 7.3，每模型 100 张 CIFAR-10）
pytest benchmark/alexnet/test_accuracy.py -k "ant-cpu-100 and not 1000" -v -s
pytest benchmark/resnet/test_accuracy.py -k "ant-cpu-100 and resnet20 and not 1000" -v -s
pytest benchmark/vgg16/test_accuracy.py -k "ant-cpu-100 and not 1000" -v -s
```

## 3. 面向加速器全同态加密编译原型系统

对应 `testing/report.md` §7.1（GPU）/ §7.2（GPU）/ §7.3（GPU）。覆盖 GPU（Phantom）后端的基础算子、模型端到端编译与 FHE 推理精度测试。

- 环境准备

```bash
docker run -it -d --name test_3 --gpus '"device=1"' opencc/ace:final-202610-v2 bash
docker exec -it test_3 bash
```

- 构建

```bash
cd ace-compiler
./scripts/dev-build.sh
```

- 测试

```bash
# 基础算子（report 7.1）
python3 benchmark/basic_ops/basic_ops.py --library phantom --device cuda

# 模型编译冒烟测试（report 7.2）：LR / LeNet x² / ResNet-20 x²
CUDA_VISIBLE_DEVICES=0 pytest tests/regression/lr/test_lr_smoke.py -k phantom -v -s
CUDA_VISIBLE_DEVICES=0 pytest tests/regression/lenet_x2/test_lenet_x2_smoke.py -k phantom -v -s
CUDA_VISIBLE_DEVICES=0 pytest tests/regression/resnet_x2/test_resnet20_x2_smoke.py -k phantom -v -s

# 推理准确率（report 7.3，每模型 100 张图片）
CUDA_VISIBLE_DEVICES=0 pytest benchmark/lr/test_accuracy.py -k "[phantom-cuda-100]" -v -s
CUDA_VISIBLE_DEVICES=0 pytest benchmark/resnet/test_accuracy.py -k "resnet20 and [phantom-cuda-100]" -v -s
CUDA_VISIBLE_DEVICES=0 pytest benchmark/resnet/test_accuracy.py -k "resnet32 and [phantom-cuda-100]" -v -s
CUDA_VISIBLE_DEVICES=0 pytest benchmark/resnet/test_accuracy.py -k "resnet44 and [phantom-cuda-100]" -v -s
CUDA_VISIBLE_DEVICES=0 pytest benchmark/resnet/test_accuracy.py -k "resnet56 and [phantom-cuda-100]" -v -s
CUDA_VISIBLE_DEVICES=0 pytest benchmark/resnet/test_accuracy.py -k "resnet110 and [phantom-cuda-100]" -v -s
```

## 4. 加速器运行时原型系统

对应 `testing/report.md` §7.5。在 FPGA 加速器（AMD Alveo V80 / Versal VPK180）上运行编译器生成代码与手写 FHE-MP-CNN 程序，对比实测耗时。

### 4.2 FPGA 加速器（baselines/fpga，接口适配 / CPU 验证路径）

IIE FPGA 加速器支持运行编译器生成的程序与手写 FHE-MP-CNN 程序。编译器测试模型包括 LR、LeNet5、ResNet20、ResNet32；手写实现测试模型包括 ResNet20、ResNet32。

- 基线验证

按交付仓库中 `baselines/fpga/README.md` 的说明完成板卡配置与 FPGA 加载。在 `fheacc-software-platform` 仓库根目录执行以下命令，生成编译器测试模型的硬件指令与数据：

```bash
make soft TEST_NAME=lr       RELEASE_MODE=2
make soft TEST_NAME=LeNet5   RELEASE_MODE=2
make soft TEST_NAME=resnet20 RELEASE_MODE=2
make soft TEST_NAME=resnet32 RELEASE_MODE=2
```

各模型的产物位于 `soft/clib/tests/tests/<TEST_NAME>/`，其中 `data/` 包含指令与 HBM 镜像，`nttw/ntt_intt_merged/` 包含 NTT 窗口数据。`RELEASE_MODE=2` 还会运行 CPU Golden model 进行数值验证。

通过 SDK app 生成手写 FHE-MP-CNN 测试模型的硬件包：

```bash
cd soft/clib/sdk/apps/resnet20
make hardware MODEL=20 SAMPLE=0 RELEASE_MODE=2
# 先加载并测试生成的 resnet20/ 硬件包，再生成下一个模型
make hardware MODEL=32 SAMPLE=0 RELEASE_MODE=2
```

两个模型都输出到 app 目录下的 `resnet20/` 硬件包目录；第二次导出会覆盖第一次。硬件包名由 app 目录名决定，实际网络由 `MODEL=20` 或 `MODEL=32` 选择。若仅需硬件产物与 cycle 估算，可使用 `RELEASE_MODE=1`，该模式不执行 Golden 数值验证。

使用上板 loader 加载对应的指令流、初始 HBM 数据与 NTT 窗口数据。运行加速器，读回输出并与对应 Golden 结果比对。进行解密比对时，若输出仍为 Montgomery 表示，需先做去 Montgomery 转换；SDK 硬件包的解密工具会根据包内记录的格式处理输出。

- FPGA 性能测试

**耗时包含输入数据加载、FPGA 计算与结果回读。** 编译器生成代码与手写实现采用相同的计时口径。将上板运行日志与计时数据保存到交付仓库的 `testing/` 目录。

实测结果见随附的 `report.md`，其内容对应仓库内的 `testing/report.md`。这些数据为 FPGA 实测耗时；`make soft` 的运行时间与 `clib_profile.md` 中的 cycle 估算是不同的指标。

## 5. 生成测试报告

任一/全部测试项运行完成后，使用 `gen_report.py` 从 `.log` 文件提取数据，生成 `data 7.X.csv` + `figures_7.X.png` + `report.md`（位于 `/app/testing/`）。脚本通过日志文件名前缀（`7.1_` / `7.2_` / `7.3_` / `7.4_` / `7.5_`，由各测试脚本写入）识别其所属测试项。

- 单项：

```bash
python3 /app/scripts/gen_report.py -L /app -o /app/testing -t 7.4
```

- 全部五项：

```bash
python3 /app/scripts/gen_report.py -L /app -o /app/testing
```
