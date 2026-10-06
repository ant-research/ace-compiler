# baselines/ — 三个 FHE 推理后端基线

三个后端基线的源码 / 数据 / 构建产物的统一容器：

```
baselines/
  cpu/   # CPU baseline —— FHE-MP-CNN（Microsoft SEAL 3.6.6 OOP API）
  fpga/  # FPGA baseline —— cpu 的副本 + fhe_ops 接口适配 rt_iie.h（CPU 验证路径）
  gpu/   # GPU baseline —— phantom-fhe（CUDA RNS-CKKS + bootstrapping）
  README.md   # 本文档
```

---

## CPU baseline（`baselines/cpu/`）

**来源**：[snu-ccl/FHE-MP-CNN](https://github.com/snu-ccl/FHE-MP-CNN) `main-3.6.6` 分支 + `baselines/0001-capture-keymem-support-resnet32_cifar10.patch`（capture keymem + support resnet32_cifar100）。源码 tree 已 tracked（与 fpga baseline 同形式）；上游升级时手工同步 + 重新 apply 0001 patch。

**派生步骤（provenance — 本仓库的 `baselines/cpu/cnn_ckks/` 内容如何从上游得到，仅作记录，clean checkout 不必重跑）**：
```bash
git clone -b main-3.6.6 https://github.com/snu-ccl/FHE-MP-CNN.git /tmp/FHE-MP-CNN
# 顶层 flat layout: 取 cnn_ckks / pretrained_parameters / testFile* 平铺
mv /tmp/FHE-MP-CNN/{cnn_ckks,pretrained_parameters,testFile,testFile_cifar100} baselines/cpu/
# apply 0001 patch（patch 内 a/cnn_ckks/... 相对 baselines/cpu/ 解析）
cd baselines/cpu && git apply ../0001-capture-keymem-support-resnet32_cifar10.patch
```

**构建**：
```bash
python3 /app/scripts/build_cnn.py
# 等价于:
#   cmake -S baselines/cpu/cnn_ckks/cpu-ckks/single-key/seal-modified-3.6.6 -B baselines/cpu/build_seal -DCMAKE_BUILD_TYPE=Release -DCMAKE_POLICY_VERSION_MINIMUM=3.5; cmake --build ... -- -j; cmake --install ... # 装 SEAL 3.6.6 → /usr/local
#   cmake -S baselines/cpu/cnn_ckks -B baselines/cpu/cnn_ckks/build_cnn -DCNN=ON -DCMAKE_BUILD_TYPE=Release; cmake --build ... -- -j  # 编 cnn 可执行
```

**运行**：
```bash
cd /app/baselines/cpu/cnn_ckks/build_cnn
./cnn <layer> <dataset> <start> <end>
# 支持: layer ∈ {20,32,44,56,110} (CIFAR-10), 32 (CIFAR-100); image label ∈ [0, 9999]
# 例: ./cnn 20 10 0 0   # ResNet-20, CIFAR-10, image 0
```

**构建产物（gitignored）**：
- `baselines/cpu/build_seal/` — modified SEAL 3.6.6 cmake build（install 到 `/usr/local/lib/libseal-3.6.a`+`/usr/local/include/seal`）
- `baselines/cpu/cnn_ckks/build_cnn/cnn` — 可执行
- `baselines/cpu/cnn_ckks/cpu-ckks/single-key/seal-modified-3.6.6/` — modified SEAL 源 tree（gitignored；用 `/usr/local` 装好的 SEAL 编译）
- `baselines/cpu/pretrained_parameters/`、`testFile/`、`testFile_cifar100/`、`result/` — 数据 + 推理输出（gitignored）

---

## FPGA baseline（`baselines/fpga/`）

**来源**：CPU baseline 的副本，把底层 `seal::Evaluator` 调用替换为 `fhe::*` 接口层，契约对齐 `rt_iie.h` 上层 wrapper，用于后续接 FPGA 加速器。适配规划与硬件接通过程见 [`.claude/plans/fpga-adaptation/`](../.claude/plans/fpga-adaptation/)（[PLAN.md](../.claude/plans/fpga-adaptation/PLAN.md) + [FPGA-INTEGRATION.md](../.claude/plans/fpga-adaptation/FPGA-INTEGRATION.md)）。

**构建**：
```bash
python3 /app/scripts/build_cnn_fpga.py
# 等价于:
#   cmake -S baselines/fpga/cnn_ckks -B baselines/fpga/cnn_ckks/build_fpga -DCNN=ON -DCMAKE_BUILD_TYPE=Release
#   cmake --build baselines/fpga/cnn_ckks/build_fpga -j
# 注意: build_fpga/ 内层目录名采用 fpga（不再用 iie）
```

**运行**（CPU 路径，fhe::* 内部转发 SEAL）：
```bash
cd /app/baselines/fpga/cnn_ckks/build_fpga
./cnn 20 10 0 0   # ResNet-20, CIFAR-10, image 0
```

**构建产物（gitignored）**：
- `baselines/fpga/cnn_ckks/build_fpga/cnn` — 可执行
- `baselines/fpga/cnn_ckks/build_cnn/`、`build/` — 历史/备用 build dir
- `baselines/fpga/cnn_ckks/cpu-ckks/single-key/seal-modified-3.6.6/` — 内嵌的 SEAL 源（gitignored；用 `/usr/local` 装好的 SEAL 编译）
- `baselines/fpga/result/` — 推理结果输出

---

## GPU baseline（`baselines/gpu/`）

**来源**：[phantom-fhe](https://github.com/encryptorion-lab/phantom-fhe) fork（含 bootstrapping + 手写 FHE ResNet 示例），phantom-fhe 上游版本 + 少量适配。

GPU-only（CUDA RNS-CKKS），需 CUDA arch 90（H100/H20）。

**Bootstrap**：clone 上游或直接用本仓库的 phantom-fhe 源码树（已 tracked 在 `baselines/gpu/`）。

**构建**：
```bash
python3 /app/scripts/build_cnn_gpu.py
# 等价于:
#   cmake -S /app/baselines/gpu -B /app/baselines/gpu/build -DCMAKE_BUILD_TYPE=Release
#   cmake --build /app/baselines/gpu/build -- -j
```

**运行**：
```bash
cd /app/baselines/gpu/build/bin
./cnn_depth_30 <rn> <cfar> <start> <end>
# 例: ./cnn_depth_30 20 10 0 0   # ResNet-20, CIFAR-10, image 0
# 也含 cnn_depth_34、cnn_2_n_key_30、cnn_2_n_key_34、boot_test 等
```

---

## 编译脚本统一在 `scripts/`

- `scripts/build_cnn.py` — cpu baseline 构建（build_seal + build_cnn）
- `scripts/build_cnn_fpga.py` — fpga baseline 构建（cmake + build_fpga）
- `scripts/build_cnn_gpu.py` — gpu baseline 构建
- `scripts/build_cmplr.sh` / `build_cmplr_omp.sh` / `build_cmplr_gpu.sh` — 不同：上面三条 `build_cnn*.py` 是 **手写 FHE 推理基线** 的编译（直接对三个 baseline 的源码），这三条 `.sh` 是 **ace-compiler 编译器** 的三后端构建（ONNX → FHE 可执行自动生成），与基线独立。

**经此布局**：三个 baseline 构建脚本不再分散在各自 baseline 目录里，统一由 `scripts/` 管理；baseline 目录只保留源码 + 数据 + 构建产物。
