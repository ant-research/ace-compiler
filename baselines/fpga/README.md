# baselines/fpga — FHE ResNet on clib SDK

基于 clib-sdk（RNS-CKKS 全同态加密）的 ResNet 推理，共三部分：

| # | 路径 | N | 特点 |
|---|---|---|---|
| 1 | `clib-seal/` | 65536 | SEAL 3.6 兼容层，把 SEAL API 调用转到 clib 后端 |
| 2 | `clib-sdk/apps/fhe-mp-cnn/` | 65536 | clib-seal 桥接版 ResNet（复现原 cnn_ckks，全网络） |
| 3 | `clib-sdk/apps/resnet20_eva/` | 8192 | clib 原生 ResNet-20（按 clib 应用范式重写，无 SEAL 依赖） |

---

## 1. clib-seal（SEAL 兼容层）

把 Microsoft SEAL 3.6 API 调用转发到 clib SDK 后端，使得原 cnn_ckks 的 SEAL ResNet 程序可以不改源码直接跑在 clib 上。RNS-CKKS 参数固定 N=65536、Q=62。

```bash
# 编译
python3 /app/scripts/build_cnn_clib.py
# 运行
python3 /app/scripts/perf_cnn_clib.py
```

---

## 2. fhe-mp-cnn（clib-seal 桥接版，N=65536）

复现原 cnn_ckks 的 SEAL ResNet 推理，通过 `clib-seal/` 兼容层把 SEAL 3.6 API 调用转到 clib 后端。支持 ResNet-{20,32,44,56,110} on CIFAR-10，ResNet-32 on CIFAR-100。N=65536，Q=62。

```bash
# 编译
python3 scripts/build_cnn_clib.py --stage-only
# 运行
cd baselines/fpga/clib-sdk/apps/fhe-mp-cnn
make
```

---

## 3. resnet20_eva（clib 评估，N=8192）

```bash
# 编译 & 运行 & 验证
cd baselines/fpga/clib-sdk/apps/resnet20_eva
make
```