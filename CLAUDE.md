# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

> 本文只给概要与索引。构建命令、架构细节、测试方法、脚本说明见各目录下的专题文档（见末尾「索引」）。

## 项目概要

蚂蚁集团 ACE FHE 评测仓库：验证「对 ONNX 神经网络做全同态加密（RNS-CKKS）推理」的端到端链路。顶层 `README.md` 是 4 个测试项的操作权威手册；本文件只做路线概要与导航。

## 三条技术路线

- **ace-compiler**（CPU 编译器，`/app/ace-compiler`）— `air-infra` + `nn-addon` + `fhe-cmplr` 三子项目 cmake 聚合，把 ONNX 编译成 FHE 程序，装到 `/app/ace_cmplr`（`bin/fhe_cmplr`）。后端由 `FHE_ENABLE_CUDA/SEAL/OPENFHE` 选。
- **baselines/gpu**（GPU 库，`/app/baselines/gpu`）— CUDA RNS-CKKS + bootstrapping 的 phantom fork，含手写 FHE ResNet 示例（`examples/cnn_depth_30.cu` 等）。
- **baselines/cpu**（CPU SEAL，`/app/baselines/cpu`）— 改版 Microsoft SEAL 3.6.6 的单密钥 CKKS ResNet。

## 五个测试项（对应顶层 README §1–4 + 7.5 runtime）

1. baselines/cpu 推理性能（CPU 6 模型）— `python3 /app/scripts/perf.py -e` + `python3 /app/scripts/cgo25_ae.py -a`
2. 编译器泛化能力 — `python3 /app/scripts/resbm.py -n 10`
3. 推理精度 — `resbm_expert.py -n 100` 后 `resbm.py -n 100`
4. 硬件加速性能（GPU）— `python3 /app/scripts/perf_gpu.py -a`（Expert 套件：`-e`）
5. runtime testcase — `python3 /app/scripts/run_rtlib.py`（跑 `eg_fhertlib_*` 14 个）

## 关键约束

- GPU 路线需 CUDA arch 90（H100/H20），编译器 GPU 构建 ≈ 3 小时。
- baselines/cpu 单图需 384GB RAM，50 图并发需 512GB。
- 环境见 `Dockerfile`（`nvidia/cuda:12.2.0-devel-ubuntu22.04`，依赖 NTL/GMP/protobuf/pybind11）；Python 依赖见 `requirements.txt`。

## 索引（细节文档）

| 内容 | 位置 |
|---|---|
| 构建命令 + 架构要点 | `/app/.claude/ARCHITECTURE.md` |
| 测试方法 | `/app/.claude/TESTING.md` |
| 测试口径（模型/图数/指标/对比） | `/app/.claude/Scope.md` |
| 脚本 option 用法手册 | `/app/.claude/Option.md` |
| 脚本说明 | `/app/scripts/README.md` |
| 5 测试项操作权威（README 一键跑） | `/app/README.md` |
| FPGA 接口适配规划 | `/app/.claude/plans/fpga-adaptation/PLAN.md` |
