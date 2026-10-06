# scripts/ 说明

> ACE FHE 评测的全部构建与测试脚本。权威测试项定义见 `/app/README.md`（7.1–7.4）+ `result/report.md`（7.1–7.5）。本文件按测试项结构化，给出每脚本的层次、option、嵌套、独立可跑性与已知点。

## 1. 三层架构

```
L1 工具底座                ace_util.py  (12 helper + paper 配置 CGO25/ASPLOS25/OOPSLA25)
        ↑ import
L2 编译/运行半成品          ace_compile.py   (run_ace_compile: onnx→fhe_cmplr→链接)
                           run_perf.py     (run_perf: 串行跑 .ace exec)
                           run_accuracy.py (run_accuracy: 并行 Pool 跑 .ace exec)
        ↑ import
L3 顶层入口                perf.py(perf_gpu.py)  resbm.py  cgo25_ae.py  resbm_expert.py  run_rtlib.py
                           │  CPU perf(baselines/cpu+ANT-ACE) │ ASPLOS25 AE │ CGO25 AE │ 明文基线 │ 7.5 rtlib
                           └─→ 直接跑二进制 ./cnn (./cnn_depth_30) 或 fhe_cmplr/c++ 链 .ace
```

构建脚本（独立，不在三层 import 链内）：`build_cmplr.sh` / `build_cmplr_omp.sh` / `build_cmplr_gpu.sh` / `build_cnn.py` / `build_cnn_fpga.py` / `build_cnn_gpu.py` / `build_target_gpu.sh` / `build_alexnet/resnet20/squeezenet/vgg16_cifar10.sh`。

## 2. 五个测试项 → 脚本链

数据权威 `/work/ace-compiler-iiecas/result/report.md`。每项命令以 `/app` 为工作目录。

### 7.1 CPU 6 模型推理性能（baselines/cpu vs ANT-ACE 加速比）
- **构建**：`python3 /app/scripts/build_cnn.py` + `/app/scripts/build_cmplr.sh Release`
- **跑**：`python3 /app/scripts/perf.py -e`（baselines/cpu 侧，跑 `./cnn <rn> <cfar> 5 5`）+ `python3 /app/scripts/cgo25_ae.py -a`（ANT-ACE 侧，编译+跑 .ace）
- **链**：`perf.py -e` → `test_perf_fhe_mp_cnn` → `./cnn`（baselines/cpu 二进制）；`cgo25_ae.py -a` → `ace_compile.run_ace_compile`（fhe_cmplr）→ `run_perf`/`run_accuracy` → .ace exec。

### 7.2 编译器泛化能力（AlexNet/ResNet-20/SqueezeNet/VGG16，准确率 10 图）
- **构建**：`/app/scripts/build_cmplr_omp.sh Release`
- **跑**：`python3 /app/scripts/resbm.py -n 10`
- **链**：`resbm.py` → `asplos25_ae()` → `ace_compile.run_ace_compile`（编译，OpenMP）+ `run_accuracy`（并行跑）。perf 分支**已禁用**（见 §6）。

### 7.3 推理精度（4 模型，加密 vs 非加密准确率差）
- **构建**：`/app/scripts/build_cmplr_omp.sh Release`
- **跑**：`python3 /app/scripts/resbm_expert.py -n 100`（明文基线，跑 `infer_model.py`）→ `python3 /app/scripts/resbm.py -n 100`（加密精度）
- **链**：`resbm_expert.py` → `infer_model.py`（ONNXRuntime 明文）；`resbm.py` → 同 7.2。

### 7.4 GPU 硬件加速性能（A100/H20 六模型加速比）
- **构建**：`python3 /app/scripts/build_cnn_gpu.py` + `/app/scripts/build_cmplr_gpu.sh Release`（约 3 小时；末尾 chain `ace_compile_gpu.py` → `build_target_gpu.sh` 链 CUDA target）
- **跑**：`python3 /app/scripts/perf_gpu.py -a`
- **链**：`perf_gpu.py -a` → `test_perf_fhe_mp_cnn`（跑 `./cnn_depth_30 <rn> <cfar> 0 0`，baselines/gpu GPU）+ `test_perf_ace`（跑预建 `<model>_gpu` exec）。

### 7.5 runtime testcase（14 个 eg_fhertlib_* + 模型 PASS）
- **构建**：`/app/scripts/build_cmplr.sh Release`（`RTLIB_BUILD_EXAMPLE` 默认 ON，产 `release/rtlib/build/example/eg_fhertlib_*`）
- **跑**：`python3 /app/scripts/run_rtlib.py`
- **链**：`run_rtlib.py` → 跑 `eg_fhertlib_*` + 模型二进制，抓 exit code 产 `.rtlib.log`（带 `#### log for:` 头）。

## 2b. 自动生成报告（gen_report.py）

跑完测试后，从 `.log` 提取数据 → `data 7.X.csv` + `report.md` + `figures_7.X.png`（落 `testing/`），对齐 `testing/report.md` 已有手供格式。

```bash
python3 /app/scripts/gen_report.py -L <log_dir> -o /app/testing
python3 /app/scripts/gen_report.py -L <log_dir> -o /app/testing -t 7.2    # 仅 7.2
```

- 输入（按 log 名后缀自动识别）：`*.cgo25.log`（7.1 ANT-ACE 侧 `<model>:` + `Exec: Time=Y(s), ,Accuracy=X%`）、`*.asplos25.log`（7.2/7.3 resbm 产 `<model>: Accuracy=X%, Time=Y(s)`）、非加密基线 log（`<Model>: Unencrypted Accuracy = X%`）、`*perf_gpu*` / `*H20*`（7.4 `Inference Time = X(s)`）。
- 输出：`data 7.{1,2,3,4}.csv`（utf-8-sig BOM，匹配现有 csv 格式）+ `figures_7.X.png`（matplotlib 柱状图，缺失列自动跳过）+ `report.md`（汇总表 + figure 嵌入）。
- 缺 log 的测试项对应列为 '-'（不崩）。

## 3. 脚本清单（22 个）

| 脚本 | 层 | 独立可跑 | 主要 option | 外部依赖 |
|---|---|---|---|---|
| `ace_util.py` | L1 | 否（库） | — | — |
| `ace_compile.py` | L2 | 是 | `-a -b -c -d -e -f -k -l -m -p -s -t` | fhe_cmplr + src + model |
| `ace_compile_gpu.py` | ? | 是（hardcoded /app 路径） | 无 argparse | fhe_cmplr + CUDA build + model |
| `run_perf.py` | L2 | 是 | `-e -i -t -d -cf10 -cf100` | 预建 .ace exec |
| `run_accuracy.py` | L2 | 是 | `-e -i -n -t -d -cf10 -cf100` | 预建 .ace exec |
| `perf.py` | L3 | 是 | `-a -e -f -k -c -m -d` | ./cnn + fhe_cmplr + onnx |
| `perf_gpu.py` | L3 | 是 | `-a -e -f -k -c -m -d` | ./cnn_depth_30 + ace_cmplr + onnx |
| `resbm.py` | L3 | 是 | `-a -c -co -cf10 -cf100 -d -f -i -m -n -p -s -t` | ace_cmplr_omp + onnx + cifar |
| `cgo25_ae.py` | L3 | 是 | 同 resbm（默认 ./ 相对路径） | ace_cmplr + ace_cmplr_omp |
| `resbm_expert.py` | L3 | 是 | `-n` | infer_model.py + onnx + cifar |
| `run_rtlib.py` | L3 | 是 | `-e -l -o -d` | eg_fhertlib_* 二进制 |
| `gen_report.py` | L3 报告 | 是 | `-L -o -t` | .log 文件（matplotlib 可选） |
| `build_cmplr.sh` | 构建 | 是 | `$1=Release\|Debug` | ace-compiler src |
| `build_cmplr_omp.sh` | 构建 | 是 | 同上 | src（OpenMP） |
| `build_cmplr_gpu.sh` | 构建 | 是 | 同上 | src + CUDA |
| `build_cnn.py` | 构建 | 是 | 无 | baselines/cpu (build_seal + build_cnn) |
| `build_cnn_fpga.py` | 构建 | 是 | 无 | baselines/fpga (build_fpga) |
| `build_cnn_gpu.py` | 构建 | 是 | 无 | baselines/gpu + CUDA |
| `build_target_gpu.sh` | 构建 | 是 | `<cu> <inc>` 位置参 | release/rtlib build |
| `build_resnet20_cifar10.sh` | 构建/手工 | 需 `WRK_DIR`+`$2` | `$1=ID $2=cmplr_dir` | ace_cmplr_omp + model |
| `build_alexnet_cifar10.sh` | 同 | 同 | 同 | 同 |
| `build_squeezenet_cifar10.sh` | 同 | 同 | 同 | 同 |
| `build_vgg16_cifar10.sh` | 同 | 同 | 同 | 同 |
| `accuracy.sh` | 手工/备选 | 是 | `$1=model $2=start $3=end` | ace_cmplr_omp + build_<model>.sh |
| `infer_model.py` | L3 | 是 | `--loader_onnx --datasets --images` | onnxruntime + torchvision |

## 4. 嵌套调用图

```
7.1:  perf.py -e ──→ ./cnn (baselines/cpu)
      perf.py -a → test_perf_ace → subprocess cgo25_ae.py ──→ ace_compile.run_ace_compile
                                          └→ run_perf/run_accuracy ─→ .ace exec
      cgo25_ae.py -a ──→ ace_compile.run_ace_compile + run_perf + run_accuracy
7.2:  resbm.py -n 10 ──→ asplos25_ae() → ace_compile.run_ace_compile(omp) + run_accuracy
7.3:  resbm_expert.py -n 100 ──→ infer_model.py (明文 ONNXRuntime)
      resbm.py -n 100 ──→ (同 7.2，加密精度)
7.4:  build_cmplr_gpu.sh ──→ ace_compile_gpu.py ──→ build_target_gpu.sh ──→ cmake build target
      perf_gpu.py -a ──→ ./cnn_depth_30 + test_perf_ace <model>_gpu
7.5:  run_rtlib.py ──→ eg_fhertlib_* (release/rtlib/build/example/)

库:   ace_compile / run_perf / run_accuracy / cgo25_ae / resbm / asplos25_ae(已删)
      共用 from ace_util import *
      perf.py / perf_gpu.py / resbm_expert.py 不 import ace_util（各自本地 write_log）
```

## 5. 独立运行矩阵

| 脚本 | 仅 --help 可跑 | 需前置构建 | 需硬件 |
|---|---|---|---|
| `ace_compile.py` / `run_perf.py` / `run_accuracy.py` | ✓ | fhe_cmplr + .ace | — |
| `perf.py` | ✓ | ./cnn + fhe_cmplr + onnx | CPU ≥320GB |
| `perf_gpu.py` | ✓ | cnn_depth_30 + ace_cmplr + onnx | GPU arch 90 |
| `resbm.py` / `cgo25_ae.py` | ✓ | ace_cmplr_omp + onnx + cifar | — |
| `resbm_expert.py` | ✓（仅 -n） | onnx + cifar | — |
| `run_rtlib.py` | ✓ | eg_fhertlib_* 二进制 | — |
| `build_*.sh` | ✓（bash -n） | src tree / CUDA | — |

## 6. 已知点

- **`perf_gpu.py -f` 不支持**：原 `ace_compile_and_run_onnx` 路径调用未 import 的 `process_model`/`build_target`（实为 `ace_compile_gpu.py` 的 `compile_model`/`link_target`，路径也错），跑 `-f` 必 NameError。README 7.4 只用 `-a`/`-e`；`-f` 现清错拒绝（argparse 保留语法兼容）。
- **`resbm.py` perf 分支禁用**：`asplos25_ae()` 的 perf 分支（L25-28）注释，`-p` 失效；7.2/7.3 实测只产精度数据（report.md §7.2 证实）。不复原以保 result 可复现。
- **`accuracy.sh` + 4 个 `build_<model>.sh` 为手工备选链**：`accuracy.sh` 旧时被 `resbm_ae.py`（已删）调用；现无现役调用者，但保留作 7.2/7.3 手工复现 alexnet/squeezenet/vgg16/resnet20 单模型的备选（`accuracy.sh <model> <start> <end>`）。
- **本地 helper 副本**：`perf.py`/`perf_gpu.py` 的 `write_log`/`time_and_memory`/`check_required_memory` 与 `ace_util.py` 同名但语义不同（无 None-guard、`/proc/meminfo` vs psutil、返回 str vs float），**不可并入 ace_util**——会改 log 输出、破坏 result 可复现。
- **删孤儿**：`asplos25_ae.py`（≡ resbm.py 重复）、`resbm_ae.py`（README 不用，用 resbm_expert）、`perf.sh`（README 不依赖）已删。

## 7. 常用参数（resbm.py）

- `-n/--num` 图数 [0,10000]（默认 10）
- `-a` 关精度 | `-p` 关性能（注：perf 分支禁用，无效）| `-t` 关 trace（默认全开）
- `-c/--cmplr` 默认 `/app/ace_cmplr`；`-co/--cmplr_omp` 默认 `/app/ace_cmplr_omp`
- `-cf10/--cifar10` 默认 `/app/cifar/cifar-10-batches-bin`；`-cf100` 默认 `/app/cifar/cifar-100-binary`
- `-m/--model` 默认 `/app/model`；`-s/--src` 默认 `/app/ace-compiler`
