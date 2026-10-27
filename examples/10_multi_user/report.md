# 多用户推理性能报告 · resnet20(单卡)

单卡约束下,服务多个 FHE 用户的 resnet20 推理延迟 / 吞吐数据。

---

## 测试配置

- **模型**:resnet20_cifar10(CKKS,N=65536,每用户 67 个 galois key,~22 GiB)
- **后端**:phantom-cuda
- **GPU**:单卡 NVIDIA H20 ~96 GiB HBM(采集时全空)
- **单 context 显存**:实测 ~23.8 GiB(galois 22G + context ~1.8G)
- **key**:3 份独立 bundle(`/mnt/nvme0/mu_keys_resnet20/user_{0,1,2}`,每份 ~23 GiB),N>3 时循环复用(`user i → key i%3`)
- **IO**:staged(`ACE_GDS_ENABLED=0`,本机 cuFile 假可用;galois 单线程 `FHERT_GALOIS_LOAD_THREADS=1`)
- **输入**:每用户 1 张归一化 CIFAR-10 图

## 三段时间口径

每个用户拆三段墙钟:**load**(22 GiB galois 读进 HBM)+ **init**(PhantomContext 构建:NTT/sin-cos 表/CKKS 参数,A 还含切换 teardown)+ **infer**(稳态推理一张图)。`load + init + infer = 单用户服务成本`。

## 四种模式(单卡)

| # | 模式 | 脚本 | 进程 / context | 切用户 / 推理执行 | 现状 |
|---|------|------|----------------|-------------------|------|
| **A** | **Batch** | `01_batch.py` | 1 进程;每用户重建独立 context | 切换=finalize+重建 context;推理串行 | ✅ 实测 |
| **B** | **Dedicated** | `02_dedicated.py` | N 独立进程,各 1 context+key | 各进程独立推理;单卡/多卡均可 | ✅ 实测 |
| **C** | **Server-Serial** | `03_server_serial.py` | 1 进程 1 共享 context,N key 常驻 | `switch_session` 重绑 key(µs);推理串行 | ✅ 实测 |
| **D** | **Server-Parallel** | `04_server_parallel.py` | 1 进程 1 共享 context,N key 常驻 | 每 session 独立 evaluator/encoder/stream/BtsMgr/GaloisTool,推理**并行** | ✅ 2 路并发稳;3 路多轮崩(race);4 路并发推理 OOM |

> 命名按部署拓扑,简称 ABCD 与脚本文件一一对应:Batch=每用户重建(批处理/非 server);Dedicated=每用户独占进程;Server-Serial/Parallel=共享 context 的 server 模式(串行/并行调度)。脚本见 `resnet20/`(大模型)与 `small/`(小算子,`04_server_parallel.py` 为串行 correctness 驱动,并发验证用 resnet20 版)。

补充:

- **HBM 约束**:单 context ~23.8 GiB。B-S 共享 1 context + N 常驻 key → N≤3(3 用户峰 ~71.9G)。B-P 每 extra session 增量 ~23.4G → 4 用户常驻 93.9G(< 96G);但并发推理受 race/scratch 限制(见下)。C 每 worker 完整 context+key → 单卡 N≤3。
- **SM 约束**:resnet20 单用户推理 SM 仅 ~60%、峰值 79%(非打满)→ 有 ~30-40% 余量,这是 B-P 并行的吞吐空间来源。
- **init 大头一次性**:PhantomContext 构建(NTT/sin-cos 表、encoder、evaluator)全在 context 创建时一次性做好、只付一次;B-S/B-P extra session 只 `Load_keys`,不重算任何表。

---

## 实测数据

### Batch —— 单进程串行(3 用户)

| 用户 | load(s) | init(s) | infer(s) |
|------|---------|---------|----------|
| user_0 | 4.74 | 11.91 | 8.24 |
| user_1 | 5.03 | 11.43 | 8.32 |
| user_2 | 5.05 | 11.04 | 8.49 |
| **均值** | **4.94** | **11.46** | **8.35** |

- total wall 74.4s(纯服务 3 用户,≈ 24.7 × 3)。`init`(context 构建+小 key)是单用户成本大头,每次切换重付。

### Dedicated —— 单卡多进程并行(3 worker 同卡)

| 用户 | load(s) | init(s) | infer(s) |
|------|---------|---------|----------|
| user_0 | 6.18 | 14.17 | 21.86 |
| user_1 | 4.95 | 12.58 | 23.11 |
| user_2 | 4.64 | 12.64 | 22.70 |
| **均值** | **5.26** | **13.13** | **22.56** |

- total wall 44.9s(并行):setup 重叠,wall ≈ 最慢一个的 init+infer。
- infer ~22.6s:3 进程抢 SM,推理近线性退化(单人 8.3 ×~2.7),吞吐不增。单卡并行上限 N=3。

### Server-Serial —— 共享 context + 串行热切(3 用户)

| 阶段 | load(s) | init(s) | infer(s) | switch(µs) |
|------|---------|---------|----------|------------|
| **create(一次性)**¹ | **33.87** | **46.63** | — | — |
| **稳态每轮(3 用户)**² | — | — | **9.27** | **34.3**(23.3–71.3) |

- 稳态每轮 = 27.8s(3×infer + µs 级 switch);冷启动头一轮 ~108s。
- switch ~34µs vs Batch 每次 ~24s 切换;infer ~9.3s 独占 SM,与 Batch 同、不退化。
- HBM 峰值 71.9 GiB(3 份常驻 key + 1 context)< 96G ✓;跨用户输出一致。

¹ create 一次性:load=3 用户 galois 各读一次;init=1 次 context 构建(primary 独有)+ key 绑定。  
² 稳态无 IO、无 context 重建,故无 load/init。

### Server-Parallel —— 共享 context + 并行推理

每 session 独立 evaluator + PhantomCKKSEncoder + CUDA stream + BtsMgr + GaloisTool(per-session,thread-local override),`run_session(idx)` 绑定调用线程后并行执行 kernel。

**Setup(一次性,4 session 常驻)**:

| 步骤 | HBM | 耗时 | 增量 |
|---|---:|---:|---|
| primary init(idx=0) | 23.8 GiB | 18.2s | 共享 context(~1.8G)+ user_0 key(22G)+ primary BtsMgr |
| create_session user_1 | 47.2 GiB | 14.4s | +23.3 GiB |
| create_session user_2 | 70.5 GiB | 15.2s | +23.4 GiB |
| create_session user_3(复用 user_0 key) | 93.9 GiB | 15.4s | +23.4 GiB |
| **合计** | **93.9 GiB** | **~63s** | 每 extra session ~23.4G = key 22G + per-session BtsMgr+GaloisTool ~1.4G |

**串行推理**:稳态 ~95.4 GiB,8.7–8.9s/图,4 session 输出跨 key 6 位一致。

**并发推理**(wall = N 图全部完成):

| 并发宽度 | HBM 峰值 | wall | 吞吐(图/s) | 结果 |
|---:|---:|---:|---:|---|
| 1(串行) | 95.4 GiB | 8.8s | 0.114 | 基线 |
| 2 | 96.6 GiB | 12.1s | 0.165 | all agree ✓(多轮稳) |
| 3 | 97.2 GiB | 16.2s(round 0) | 0.185 | round 0 ✓、**round 1 崩**(race) |
| 4 | >96 GiB | — | — | OOM(常驻 93.9G + 4 路 scratch 突破 96G) |

- 小算子(add_op/gemm_49x3)2 路并发 10 轮全过、输出一致。
- B-P 把每 extra session 增量从 regen 路径的 45.8G 降到 23.4G → 4 用户可常驻;**并发推理 2 路稳,3 路存在 race(单轮可能侥幸、多轮必崩),4 路 OOM**。

### 四模式对比(3 用户单卡)

| # | 模式 | context | 切换成本/次 | 推理执行 | setup 形态 | 单卡吞吐(图/s) | 现状 |
|---|------|---------|-------------|----------|------------|------------------|------|
| **A** | **Batch** | 每用户重建 | ~24s | 串行 | 每次建 ctx | ~0.04 | ✅ 实测 |
| **B** | **Dedicated** | N 独立 | —(各占 ctx) | 并行 | N 进程 setup 重叠 | ~0.13(单卡抢退) | ✅ 实测 |
| **C** | **Server-Serial** | 1 共享 | **~34µs** | **串行** | 1×ctx+N×load | ~0.11 | ✅ 实测 |
| **D** | **Server-Parallel** | 1 共享 | ~34µs(并行不需切) | **并行** | 1×ctx+N×load+N×~1.4G | 0.165(2路) | ✅ 2路稳;3路race;4路OOM |

---

## 简单分析

- **Server 模式族价值(Shared-Serial/Parallel)**:共享 context 消除 per-user init(sin/cos/NTT 表只建一次),µs 切换,每用户稳态只剩推理(infer ~9.3s);代价 N×key 常驻 HBM(3 用户 71.9G)。
- **Batch 价值**:最省 HBM(单用户),但每次切换 rebuild ~24s 且 flaky(phantom rebuild 残留)。
- **Dedicated 价值**:独立进程稳定、可跨卡真线性扩吞吐;单卡受 SM/HBM 抢占退化(并行只省 setup、不增吞吐)。Dedicated 单卡并行 wall(44.9s)< Batch 串行(74.4s)靠 setup 重叠,非推理提速。
- **Server-Serial vs Server-Parallel**:Serial 稳态吞吐=单卡推理极限(串行);Parallel 用空闲 SM 提并发吞吐,2 路实测 0.165 图/s(wall 12.1s 跑 2 图 vs 串行排队 17.6s)。Parallel 单图未占满 SM,并发把空闲资源利用起来。
- **B-P 并发上限**:2 路稳;3 路 race(根因待查,疑 3 路并发暴露的另一共享 mutable 点,非 GaloisTool);4 路 OOM(显存天花板)。

## 复现

```bash
cd /work/ace-fhe/ace-compiler

# 环境默认:脚本内 os.environ.setdefault 已设 ACE_GDS_ENABLED=0(staged)、
# FHERT_GALOIS_LOAD_THREADS=1;CUDA_VISIBLE_DEVICES 按需自行设置(选一张空闲卡)。
# 参数默认:--num-users 3、--rounds 3(02 --inputs 每用户图数,默认 1);下列命令
# 走默认,需要时显式覆盖。

# 0. 生成 3 份 resnet20 key(每用户独立进程,~23 GiB,存 NVMe)
#    默认:--gpus 省略 → 复用 CUDA_VISIBLE_DEVICES 的卡(单卡并行,~75G 峰,96G 卡可行);
#    --io staged / --galois-format per-key 为默认(本机 cuFile 假可用 + 避 merged stall)。
#    多卡更快:加 --gpus 4 5 7(每用户一卡)。
python3 examples/10_multi_user/resnet20/generate_keys.py \
  --num-users 3 --key-root /mnt/nvme0/mu_keys_resnet20

# 模式 A (Batch) —— 单进程串行(默认 3 用户 × 3 轮)
python3 examples/10_multi_user/resnet20/01_batch.py --key-root /mnt/nvme0/mu_keys_resnet20

# 模式 B (Dedicated) —— 单卡多进程并行(3 worker 同卡;--gpus 指定卡)
python3 examples/10_multi_user/resnet20/02_dedicated.py --key-root /mnt/nvme0/mu_keys_resnet20

# 模式 C (Server-Serial) —— 共享 context + 串行热切(默认 3 用户 × 3 轮)
python3 examples/10_multi_user/resnet20/03_server_serial.py --key-root /mnt/nvme0/mu_keys_resnet20

# 模式 D (Server-Parallel) —— 共享 context + 并行推理
# 注:3 用户当前 round 1 有 race(见未完成事项),这里显式 --num-users 2 稳跑
python3 examples/10_multi_user/resnet20/04_server_parallel.py \
  --key-root /mnt/nvme0/mu_keys_resnet20 --num-users 2

# 模式 E (Server-Serial + KEY POOL) —— K 份 key 内存服务 N>K 逻辑用户
python3 examples/10_multi_user/resnet20/05_server_serial_pool.py \
  --key-root /mnt/nvme0/mu_keys_resnet20 --pool-size 3 --num-logical-users 8 --rounds 2

# 模式 F (Server-Parallel + KEY POOL) —— K 路并发、N>K 逻辑用户(per-session lock)
python3 examples/10_multi_user/resnet20/06_server_parallel_pool.py \
  --key-root /mnt/nvme0/mu_keys_resnet20 --pool-size 2 --num-logical-users 4 --rounds 1
```

## small / ace 用法

`examples/10_multi_user/small/` 是快速验证组(小 case `gemm_49x3`/`linear_bias`/`add_op`,
秒级),脚本结构与 resnet20 组一致(`generate_keys.py` → `01_batch`/`03_server_serial`/
`04_server_parallel`/`06_server_parallel_pool`,及 on-demand 的 `05_on_demand_keys`)。
推理脚本不重新编译,直接复用 `generate_keys` 产出的 `.so` + per-user key,故**后端由
generate 阶段的 `--backend` 决定**。

```bash
cd /work/ace-fhe/ace-compiler

# 0. 生成 2 份 gemm key(默认 phantom;每用户独立进程,单卡可放)
CUDA_VISIBLE_DEVICES=<空闲卡> python3 examples/10_multi_user/small/generate_keys.py \
  --num-users 2 --spec gemm_49x3 --galois-format per-key --key-root /tmp/mu_keys_gemm

# 模式 C (Server-Serial) —— 共享 context + 串行热切
CUDA_VISIBLE_DEVICES=<空闲卡> python3 examples/10_multi_user/small/03_server_serial.py \
  --key-root /tmp/mu_keys_gemm --rounds 3

# 模式 D (Server-Parallel) —— 共享 context + 并行推理
CUDA_VISIBLE_DEVICES=<空闲卡> python3 examples/10_multi_user/small/04_server_parallel.py \
  --key-root /tmp/mu_keys_gemm --num-users 2

# 模式 F (Server-Parallel + KEY POOL) —— K 份 key 服务 N>K 逻辑用户
CUDA_VISIBLE_DEVICES=<空闲卡> python3 examples/10_multi_user/small/06_server_parallel_pool.py \
  --key-root /tmp/mu_keys_gemm --pool-size 2 --num-logical-users 4 --rounds 1
```

### ace 后端

`generate_keys.py --backend {phantom,ace,ant}` 选择编译/运行后端(默认 phantom)。ace
后端把同样的 spec 编成 `libace_cuda_*.so` 并链接 ace 运行库;推理脚本因复用 `.so`+key,
**无需改命令**,只是 `--key-root` 指向 ace 生成的目录:

```bash
# ace:生成 2 份 gemm key(注意 ace 单次 alloc ~26 GiB,需一张 >30G 空闲的卡)
CUDA_VISIBLE_DEVICES=<空闲大卡> python3 examples/10_multi_user/small/generate_keys.py \
  --backend ace --num-users 2 --spec gemm_49x3 --galois-format per-key --key-root /tmp/mu_ace_keys

# 推理脚本照常(后端已由 .so 决定):
CUDA_VISIBLE_DEVICES=<空闲大卡> python3 examples/10_multi_user/small/03_server_serial.py \
  --key-root /tmp/mu_ace_keys --rounds 3
```

**ace 注意事项**:
- 显存:ace(rmm cuda_async)gemm_49x3 key 生成峰值 ~26 GiB/进程,**必须一张足够空的卡**
  (>30G 空闲);同卡并发 2 worker 需 ~60G 空闲。本机 8 卡被占满时(每卡仅剩 ~28G)ace 会
  OOM(`failed to allocate ~26G`),需等卡空旷或用 `--gpus` 分卡。
- B-P 对称性:ace 的 `Encode_float*`/`Prepare_input`/`Bootstrap` 仍走 primary 非 per-session
  路由(见"未完成事项"#3),故 ace 上 Server-Parallel/Serial 多 session 推理尚未对称,建议先用
  `01_batch`/`05_on_demand_keys` 等单 session 路径验证 ace 正确性。
- key 管理:ace 的 `Ensure_galois_key`/`Prepare_on_demand_keys` 目前是 no-op stub,**ace 不
  支持 ON_DEMAND 模式**;ace 只走 ONLINE/PRELOAD(_FULL)。

与上面 A-F 多用户拓扑**正交**的机制:把"init 时全量加载 galois key(~22 GiB/用户)常驻 HBM"
改成"用到某个 key 前 ensure 它已在 HBM"。`KeyMode::ON_DEMAND_FULL`(SK/PK/RLK 预加载、
galois 空壳)由 FheContext::Rotate 前的 `Ensure_galois_key(step)` 触发按需 fetch;`on_demand`
(eval-only,无 SK/PK)与之同名分支但 `run()` 不能端到端,故对照用 `on_demand_full`。

完整设计/调查/实现证据见 `.claude/plans/multi-user/on-demand-key-loading.md`(三路代码调查 +
file:line + stage-1/2 进展)。两条 feature 分支:主仓 `feat/on-demand-key-loading`(runtime) +
子仓 fhe-cmplr `feat/on-demand-galois-key-load-emit`(codegen emit)。

### 机制(两阶段)

- **Stage-1(已验证)**:codegen 在每个 rotate 前 emit `Load_galois_key(step);`;
  runtime `Ensure_galois_key` miss 时 lazy load 单 key(`Read_one_relin_key` +
  `populate_one_relin_key` 灌 slot)。resnet20 的 rotate 经 phantom NAF 分解,大量
  step 的 elt 不在直接 key 集 → stage-1 用"全集 prefetch fallback"兜底(等价延迟 PRELOAD:
  init 省 HBMbubble首图触发 22 GiB 全量加载)。
- **Stage-2 全量 prefetch → NAF-aware 单 key prefetch(已实现)**:复现 phantom
  `naf(step)`(±2^i 分解),只 prefetch 该 step 的 NAF 分量 elt(typically log|step|
  个上线 key 的 5^k primitive),非全 67。意图是稳态只持 working-set 而非全量。
  **但 resnet20 实测**:稳态仍 ≈25 GiB(全集)——因 resnet20 的 rotation-step 集合
  NAF 分解后覆盖几乎所有 5^k primitive,working-set = full-set。on-demand 对 resnet20
  的真实价值在 **init 省 22 GiB/用户**(多用户 transient peak 不撞卡),稳态不省。

### 实测(gemm_49x3 + resnet20)

gemm_49x3(20 个 galois key,小 working-set 真子集):

| 模式 | init HBM | infer | max_diff ON_DEMAND vs PRELOAD |
|------|---------:|------:|------------------------------:|
| PRELOAD_FULL | ~1028 MiB | <1s | — |
| ON_DEMAND_FULL | ~788 MiB | <1s | 0.000e+00(PASS) |

resnet20(67 个 galois key,working-set=full-set),单卡 `03_server_serial.py --key-mode on_demand_full`:

| 模式 | init HBM | 首图 infer | 稳态 infer | all correct |
|------|---------:|-----------:|-----------:|:-----------:|
| PRELOAD_FULL | ~23804 MiB | 9.55s | ~9.5s | ✓ |
| ON_DEMAND_FULL | **~890 MiB** | 32.4s(cold:含 NAF 全集 prefetch ~24s) | ~8.5s | ✓ |

(resnet20 cold-cache 首图多 ~24s 是 NAF 全集 prefetch 的 staged IO;warm page cache
下首图几乎无额外延迟。)

### 运行

```bash
# gemm small demo(05_on_demand_keys.py,ON_DEMAND vs PRELOAD 对照 + HBM 打印)
ACE_GDS_ENABLED=0 FHERT_GALOIS_LOAD_THREADS=1 \
python3 examples/10_multi_user/small/05_on_demand_keys.py

# resnet20 on-demand(03/05 加 --key-mode on_demand_full)
CUDA_VISIBLE_DEVICES=0 ACE_GDS_ENABLED=0 FHERT_GALOIS_LOAD_THREADS=1 \
python3 examples/10_multi_user/resnet20/03_server_serial.py \
  --key-root /mnt/nvme0/mu_keys_resnet20 --num-users 1 --rounds 1 \
  --io staged --key-mode on_demand_full --hbm

# pool + on-demand(K 份 on-demand 空 shell 服务 N>K 用户,init peak 进一步降)
python3 examples/10_multi_user/resnet20/05_server_serial_pool.py \
  --key-root /mnt/nvme0/mu_keys_resnet20 --pool-size 3 --num-logical-users 8 \
  --rounds 1 --key-mode on_demand_full --hbm
```

### 验证脚本(临时)

`/tmp/on_demand_step1_verify.py`(gemm 三步:emit 检查 + JIT 符号 + ON_DEMAND vs PRELOAD
diff)、`/tmp/on_demand_step1_verify_resnet20.py`(resnet20 同三步,删 cache 强制 fresh compile)。

### 当前状态 / 待办

- ✅ gemm 端到端验证(ON_DEMAND == PRELOAD,max_diff=0)
- ✅ resnet20 on-demand 跑通(init 省 22 GiB,输出一致)
- ✅ codegen `Load_galois_key` emit 已在子仓 fhe-cmplr feature 分支
- ✅ **create_session 多用户 on-demand 修复**(039b264):create_session 原走 `Load_keys`
  无视 key_mode 全量加载 galois(3 用户 init 涨到 ~70 GiB,与 PRELOAD 无异);现按 key_mode
  走 `Prepare_on_demand_keys` 空壳,extra session 也省 galois(3 用户 init 仅 ~3 GiB,create
  ~9s/用户含 SK/PK/RLK+evaluator/stream/BtsMgr,无 galois IO,galois_load=-1)。GEMM 回归仍绿。
- ⚠️ 06_server_parallel_pool 未能验证:04_server_parallel 原版 K=2/K=3 并发在当前 phantom
  faa6ba2 下崩 illegal memory access(见未完成事项 #1);06 并发模型照 04,pool wiring 正确
  但待 04 并发路径修复后才能验证。
- ⏳ stage-2 step-2(load 插入算法 / prefetch 时机):codegen emit 现为同步,待改"提前 N 步
  async"以重叠 IO 与 compute。对 resnet20 稳态 HBM 不省(working-set=full),价值在首图延迟摊销。

## 未完成事项

1. **B-P 3 路并发 race(2026-07-29 gdb 查清,两层根因)**:2 路稳、3 路崩。gdb
   `thread apply all bt` 抓到 abort 线程(Thread 68)栈:`free()←Main_graph()←Run_session()`。
   小 case(gemm)K=3 复现稳定(`free(): unaligned chunk in tcache 2` / illegal ckks.cu:185)。
   深挖 phantom + rtlib 7 层后定位**两层根因**(均在 input 取用路径,非 phantom encoder
   内部——后者经查 rns_tool/fft/ntt_tables/context_data/stream-TLS 全 const/own-scratch/
   read-only-table,理论线程安全):

   **(a) `Get_input_data` 在全局 Input_data 上 `std::move + delete` → double-free**。
   `Io_get_input`(`io_lib.c:109`)存于 `static IO_DATA** Input_data` + `#pragma omp
   threadprivate`——**threadprivate 只对 OpenMP 线程生效,ThreadPool worker 不继承** →
   3 worker 共享同一 Input_data。`Get_input_data`(`phantom_lib.cu:78`)
   `std::move(*data); delete data;` 从该全局 move 出 + delete 容器 → 3 路并发各 move+delete
   同一存储 → 第 2/3 move 悬空 + 3× delete → tcache 腐败 abort。
   尝试修(返回 copy `*data` 不 move+delete)→ **修了 layer a,K=3 N=6 rounds=3 通**;但
   **layer b 暴露**→ copy 不可靠,revert。

   **(b) copy 路径 `cuda_auto_ptr` const `operator=` 用源 stream → 大压力 race**。
   `Ciphertext ret = *data` 走 `cuda_auto_ptr::operator=(const&)`(`cuda_wrapper.cuh:99`)
   `cudaMallocAsync(...,obj.cudaStream_) + cudaMemcpyAsync(...,obj.cudaStream_)`——
   **用源 data 的 stream(全局 Input_data 存的 stream,default 共享)非当前 thread stream。
   3 worker 并发 copy 同一 *data → 同 default stream 上 cudaMallocAsync/Memcpy 交错 +
   各 worker cudaStreamSynchronize 打破顺序 → illegal ckks.cu:185。K=3 N≥8 / rounds≥5
   必崩。

   **根治方向(二选一,fhe-cmplr 子仓)**:
   ① MFREE pass 标 `Get_input_data` 返回变量 `borrowed`(`poly2c_mfree.h:122 Mark_borrowed`
   机制现成):kernel 不再 emit `Free_cipher(&x_0)` → 不 copy 不 move,no race。input 本就是
   caller(Prepare_input)拥有,kernel 仅 view——borrowed 语义正确。~1 天,子仓。
   ② `Io_get_input`/`Io_set_input` 改 per-session 存储(`Input_data[session_idx]`):彻底隔离,
   但侵入 io_lib + 所有 rtlib backend。~2-3 天。

   **现状**:copy 修法 revert(不可靠);K=2 是侥幸稳定上限(small gemm K=2 偶崩,resnet20
   K=2 曾稳跑 47s 待重测确证);K=3 需根治(layer a 或 b 任一)。phantom ckks encoder 内部已
   查清线程安全,race 全在 rtlib Io input 全局单例 + cuda_auto_ptr copy stream。
2. **B-P 4 路并发 OOM**:常驻 93.9G + 4 路推理 scratch 突破 96G。需更大卡或推理 scratch 优化。
3. **ace 后端对称修复**:`fhe_lib/ace/src/context.cu` 的 `Encode_float*`/`Prepare_input`/`Bootstrap` 仍用 primary 非 per-session 路由(phantom 已修)。B-P 默认 phantom 不受影响。
4. **服务型 create_session 拆分 + prefetch**(client-server 演进):当前 create_session 把"建壳 + load key(22G)+ 建 BtsMgr"绑死。服务型需拆为:① create_session 只建壳(不 load key,HBM~0);② 推理时 `prefetch_key` 走 GDS 直读(1.2s)异步重叠进推理窗口 → 用户上线建壳零显存、首请求无感 load。per-session 隔离已为并发铺好基础。**部分已实现**(ON_DEMAND_FULL: galois 空壳 init + 推理时 lazy fetch,见上"On-Demand Key Loading";残项是 SK/PK/RLK 仍在 init 全量 + async prefetch 重叠)。
5. **B-S/B-P key load 走 GDS**:当前 staged(~11s),真实部署应 GDS 直读(~1.2s)才能完全重叠进推理窗口。需修本机 cuFile 假可用或部署用 PSDK。
6. **push phantom 分支 + pin DEP_PHANTOM_REF**:phantom `feat/per-op-stream`(含 per-session GaloisTool)尚未 push,远端/URL 待定。

## 硬件 / 环境注记

- 主机 NVIDIA H20 ~96 GiB HBM(8 卡),2× NVMe,key 在 `/mnt/nvme0`。
- 本机 cuFile 假可用 → 强制 staged IO;当前为模拟测试多用户,真实部署下 `load` 应走 GDS/PSDK(NVMe→GPU 直读)。


### 暂存测试结果：
- Branch : feat/eva-multi-user, commit id : 51da15b

- 3用户串行 H20

```bash
# 3用户串行 H20
root@b74ab3125f8b:/work/ace-fhe/ace-compiler# CUDA_VISIBLE_DEVICES=6 python3 examples/10_multi_user/resnet20/03_server_serial.py     --key-root /mnt/nvme0/mu_keys_resnet20 --num-users 3  --rounds 1 --io staged --key-mode on_demand_full --hbm
  key_mode = on_demand_full
======================================================================
ResNet-20 Multi-User (Server-Serial) — 3 users, 1 rounds
======================================================================
  compiling resnet20 ...
[INFO] ace.fhe.backend.phantom: Library: using phantom for build...
[INFO] ace.fhe.driver.driver: Loading relu_vr_data from file: /opt/ace-env/lib/python3.12/site-packages/ace/model/resnet/profiles/resnet20_cifar10.json
[INFO] ace.fhe.cache: [Cache] HIT: Loading from /tmp/ace-compile-cache/resnet20_cifar10-torch-phantom-cuda/shape_1x3x32x32_dtype_torch_float32/7ec3f2afaeed67f7_vr8c6d9768e27cd706/libphantom_cuda_836272.so
  compiled (0.0s)
  kernel: /tmp/ace-compile-cache/resnet20_cifar10-torch-phantom-cuda/shape_1x3x32x32_dtype_torch_float32/7ec3f2afaeed67f7_vr8c6d9768e27cd706/libphantom_cuda_836272.so
[INFO] ace.fhe.runtime.runtime: Key path set: /mnt/nvme0/mu_keys_resnet20/user_0 (pre-generated keys will be loaded)
[INFO] ace.fhe.runtime.runtime: Key mode set: on_demand_full
[INFO] ace.runtime: [phantom] PhantomContext created (N=65536) [8.86s]
[INFO] ace.runtime: [kms] PhantomContext received (N=65536)
[*** LOG ERROR #0001 ***] [2026-08-04 15:51:07] [ace.runtime] argument not found
[INFO] ace.runtime: [kms] Loaded secret_key.bin via staged (N=65536, L=37, max_pow=2)
[INFO] ace.runtime: [kms] Loaded public_key.bin via staged (N=65536, L=37, pk_size=2)
[INFO] ace.runtime: [kms] Loaded relin_key.bin via staged (N=65536, L=37, 9 keys)
[INFO] ace.runtime: [kms] SK+PK+RLK preloaded (ON_DEMAND_FULL) from /mnt/nvme0/mu_keys_resnet20/user_0
[INFO] ace.runtime: [kms] Read /mnt/nvme0/mu_keys_resnet20/user_0/model/galois_key.index.json (version=1, N=65536, L=37, 67 relin keys, 1 disks)
[INFO] ace.runtime: [kms] Loaded galois-key index only: 67 relin keys, 67 elts (ON_DEMAND, no key bytes read)
[INFO] ace.runtime: [kms] ON_DEMAND_FULL ready: 67 galois slots (67 elts) deferred, SK/PK/RLK preloaded (from /mnt/nvme0/mu_keys_resnet20/user_0)
[INFO] ace.runtime: [kms] Rebuilt GaloisTool with 67 loaded galois elements
[INFO] ace.runtime: [kms] Skipping galois key generation in bootstrap — using keys loaded from disk
[INFO] ace.runtime: [kms] bootstrap key prefetch (hoist) 25.404s
  primary user_0   build(ctx+load)=50.55s load=-1.00s init=51.55s  (idx=0)
  [hbm] after primary init: resident ≈ 23804 MiB (galois lazy-empty)
[*** LOG ERROR #0002 ***] [2026-08-04 15:51:49] [ace.runtime] argument not found
[INFO] ace.runtime: [kms] Loaded secret_key.bin via staged (N=65536, L=37, max_pow=2)
[INFO] ace.runtime: [kms] Loaded public_key.bin via staged (N=65536, L=37, pk_size=2)
[INFO] ace.runtime: [kms] Loaded relin_key.bin via staged (N=65536, L=37, 9 keys)
[INFO] ace.runtime: [kms] SK+PK+RLK preloaded (ON_DEMAND_FULL) from /mnt/nvme0/mu_keys_resnet20/user_1
[INFO] ace.runtime: [kms] Read /mnt/nvme0/mu_keys_resnet20/user_1/model/galois_key.index.json (version=1, N=65536, L=37, 67 relin keys, 1 disks)
[INFO] ace.runtime: [kms] Loaded galois-key index only: 67 relin keys, 67 elts (ON_DEMAND, no key bytes read)
[INFO] ace.runtime: [kms] ON_DEMAND_FULL ready: 67 galois slots (67 elts) deferred, SK/PK/RLK preloaded (from /mnt/nvme0/mu_keys_resnet20/user_1)
  create  user_1   idx=1 total= 8.87s galois_load=-1.00s
[*** LOG ERROR #0003 ***] [2026-08-04 15:51:57] [ace.runtime] argument not found
[INFO] ace.runtime: [kms] Loaded secret_key.bin via staged (N=65536, L=37, max_pow=2)
[INFO] ace.runtime: [kms] Loaded public_key.bin via staged (N=65536, L=37, pk_size=2)
[INFO] ace.runtime: [kms] Loaded relin_key.bin via staged (N=65536, L=37, 9 keys)
[INFO] ace.runtime: [kms] SK+PK+RLK preloaded (ON_DEMAND_FULL) from /mnt/nvme0/mu_keys_resnet20/user_2
[INFO] ace.runtime: [kms] Read /mnt/nvme0/mu_keys_resnet20/user_2/model/galois_key.index.json (version=1, N=65536, L=37, 67 relin keys, 1 disks)
[INFO] ace.runtime: [kms] Loaded galois-key index only: 67 relin keys, 67 elts (ON_DEMAND, no key bytes read)
[INFO] ace.runtime: [kms] ON_DEMAND_FULL ready: 67 galois slots (67 elts) deferred, SK/PK/RLK preloaded (from /mnt/nvme0/mu_keys_resnet20/user_2)
  create  user_2   idx=2 total= 9.79s galois_load=-1.00s
  round 0 user_0   switch=   33.1µs infer= 8.42s
  round 0 user_1   switch=   13.2µs infer= 8.48s
  round 0 user_2   switch=   31.5µs infer= 8.40s
  round 0: all users agree ✓

======================================================================
Summary
======================================================================
  users / rounds:  3 / 1
  serve wall:      25.30s  (switches + infers only; setup excluded)
  all correct:     True

  user      create_s  switch_us   infer_s
  user_0       0.00s      33.1µs     8.42s
  user_1       8.87s      13.2µs     8.48s
  user_2       9.79s      31.5µs     8.40s
  mean                    25.9µs (min=13.2µs max=33.1µs)

  primary build (one-time):     50.55s  (context + key load)
  per-user create (one-time):   9.33s avg (22 GiB galois load; resident thereafter)
  switch_session latency:       ~25.9µs  (key rebind, no IO)
======================================================================
```

- 3用户串行 A100, staged & gds(sync)
```bash
root@4c228eb24ce5:/work/ace-fhe/ace-compiler# CUDA_VISIBLE_DEVICES=0 python3 examples/10_multi_user/resnet20/03_server_serial.py     --key-root /mnt/nvme0/mu_keys_resnet20 --num-users 3  --rounds 1 --io staged --key-mode on_demand_full --hbm
  key_mode = on_demand_full
======================================================================
ResNet-20 Multi-User (Server-Serial) — 3 users, 1 rounds
======================================================================
  compiling resnet20 ...
[INFO] ace.fhe.backend.phantom: Library: using phantom for build...
[INFO] ace.fhe.driver.driver: Loading relu_vr_data from file: /opt/ace-env/lib/python3.12/site-packages/ace/model/resnet/profiles/resnet20_cifar10.json
[INFO] ace.fhe.cache: [Cache] HIT: Loading from /tmp/ace-compile-cache/resnet20_cifar10-torch-phantom-cuda/shape_1x3x32x32_dtype_torch_float32/7ec3f2afaeed67f7_vr8c6d9768e27cd706/libphantom_cuda_209106.so
  compiled (0.0s)
  kernel: /tmp/ace-compile-cache/resnet20_cifar10-torch-phantom-cuda/shape_1x3x32x32_dtype_torch_float32/7ec3f2afaeed67f7_vr8c6d9768e27cd706/libphantom_cuda_209106.so
[INFO] ace.fhe.runtime.runtime: Key path set: /mnt/nvme0/mu_keys_resnet20/user_0 (pre-generated keys will be loaded)
[INFO] ace.fhe.runtime.runtime: Key mode set: on_demand_full
[INFO] ace.runtime: [phantom] PhantomContext created (N=65536) [2.53s]
[INFO] ace.runtime: [kms] PhantomContext received (N=65536)
[*** LOG ERROR #0001 ***] [2026-08-04 15:55:23] [ace.runtime] argument not found
[INFO] ace.runtime: [kms] Loaded secret_key.bin via staged (N=65536, L=37, max_pow=2)
[INFO] ace.runtime: [kms] Loaded public_key.bin via staged (N=65536, L=37, pk_size=2)
[INFO] ace.runtime: [kms] Loaded relin_key.bin via staged (N=65536, L=37, 9 keys)
[INFO] ace.runtime: [kms] SK+PK+RLK preloaded (ON_DEMAND_FULL) from /mnt/nvme0/mu_keys_resnet20/user_0
[INFO] ace.runtime: [kms] Read /mnt/nvme0/mu_keys_resnet20/user_0/model/galois_key.index.json (version=1, N=65536, L=37, 67 relin keys, 1 disks)
[INFO] ace.runtime: [kms] Loaded galois-key index only: 67 relin keys, 67 elts (ON_DEMAND, no key bytes read)
[INFO] ace.runtime: [kms] ON_DEMAND_FULL ready: 67 galois slots (67 elts) deferred, SK/PK/RLK preloaded (from /mnt/nvme0/mu_keys_resnet20/user_0)
[INFO] ace.runtime: [kms] Rebuilt GaloisTool with 67 loaded galois elements
[INFO] ace.runtime: [kms] Skipping galois key generation in bootstrap — using keys loaded from disk
[INFO] ace.runtime: [kms] bootstrap key prefetch (hoist) 2.500s
  primary user_0   build(ctx+load)=11.57s load=-1.00s init=12.57s  (idx=0)
  [hbm] after primary init: resident ≈ 24334 MiB (galois lazy-empty)
[*** LOG ERROR #0002 ***] [2026-08-04 15:55:32] [ace.runtime] argument not found
[INFO] ace.runtime: [kms] Loaded secret_key.bin via staged (N=65536, L=37, max_pow=2)
[INFO] ace.runtime: [kms] Loaded public_key.bin via staged (N=65536, L=37, pk_size=2)
[INFO] ace.runtime: [kms] Loaded relin_key.bin via staged (N=65536, L=37, 9 keys)
[INFO] ace.runtime: [kms] SK+PK+RLK preloaded (ON_DEMAND_FULL) from /mnt/nvme0/mu_keys_resnet20/user_1
[INFO] ace.runtime: [kms] Read /mnt/nvme0/mu_keys_resnet20/user_1/model/galois_key.index.json (version=1, N=65536, L=37, 67 relin keys, 1 disks)
[INFO] ace.runtime: [kms] Loaded galois-key index only: 67 relin keys, 67 elts (ON_DEMAND, no key bytes read)
[INFO] ace.runtime: [kms] ON_DEMAND_FULL ready: 67 galois slots (67 elts) deferred, SK/PK/RLK preloaded (from /mnt/nvme0/mu_keys_resnet20/user_1)
  create  user_1   idx=1 total= 3.67s galois_load=-1.00s
[*** LOG ERROR #0003 ***] [2026-08-04 15:55:36] [ace.runtime] argument not found
[INFO] ace.runtime: [kms] Loaded secret_key.bin via staged (N=65536, L=37, max_pow=2)
[INFO] ace.runtime: [kms] Loaded public_key.bin via staged (N=65536, L=37, pk_size=2)
[INFO] ace.runtime: [kms] Loaded relin_key.bin via staged (N=65536, L=37, 9 keys)
[INFO] ace.runtime: [kms] SK+PK+RLK preloaded (ON_DEMAND_FULL) from /mnt/nvme0/mu_keys_resnet20/user_2
[INFO] ace.runtime: [kms] Read /mnt/nvme0/mu_keys_resnet20/user_2/model/galois_key.index.json (version=1, N=65536, L=37, 67 relin keys, 1 disks)
[INFO] ace.runtime: [kms] Loaded galois-key index only: 67 relin keys, 67 elts (ON_DEMAND, no key bytes read)
[INFO] ace.runtime: [kms] ON_DEMAND_FULL ready: 67 galois slots (67 elts) deferred, SK/PK/RLK preloaded (from /mnt/nvme0/mu_keys_resnet20/user_2)
  create  user_2   idx=2 total= 3.63s galois_load=-1.00s
  round 0 user_0   switch=   34.7µs infer= 8.29s
  round 0 user_1   switch=   20.6µs infer= 8.30s
  round 0 user_2   switch=   19.7µs infer= 8.30s
  round 0: all users agree ✓

======================================================================
Summary
======================================================================
  users / rounds:  3 / 1
  serve wall:      24.89s  (switches + infers only; setup excluded)
  all correct:     True

  user      create_s  switch_us   infer_s
  user_0       0.00s      34.7µs     8.29s
  user_1       3.67s      20.6µs     8.30s
  user_2       3.63s      19.7µs     8.30s
  mean                    25.0µs (min=19.7µs max=34.7µs)

  primary build (one-time):     11.57s  (context + key load)
  per-user create (one-time):   3.65s avg (22 GiB galois load; resident thereafter)
  switch_session latency:       ~25.0µs  (key rebind, no IO)
======================================================================
root@4c228eb24ce5:/work/ace-fhe/ace-compiler# CUDA_VISIBLE_DEVICES=0 python3 examples/10_multi_user/resnet20/03_server_serial.py     --key-root /mnt/nvme0/mu_keys_resnet20 --num-users 3  --rounds 1 --io gds --key-mode on_demand_full --hbm
  key_mode = on_demand_full
======================================================================
ResNet-20 Multi-User (Server-Serial) — 3 users, 1 rounds
======================================================================
  compiling resnet20 ...
[INFO] ace.fhe.backend.phantom: Library: using phantom for build...
[INFO] ace.fhe.driver.driver: Loading relu_vr_data from file: /opt/ace-env/lib/python3.12/site-packages/ace/model/resnet/profiles/resnet20_cifar10.json
[INFO] ace.fhe.cache: [Cache] HIT: Loading from /tmp/ace-compile-cache/resnet20_cifar10-torch-phantom-cuda/shape_1x3x32x32_dtype_torch_float32/7ec3f2afaeed67f7_vr8c6d9768e27cd706/libphantom_cuda_209106.so
  compiled (0.0s)
  kernel: /tmp/ace-compile-cache/resnet20_cifar10-torch-phantom-cuda/shape_1x3x32x32_dtype_torch_float32/7ec3f2afaeed67f7_vr8c6d9768e27cd706/libphantom_cuda_209106.so
[INFO] ace.fhe.runtime.runtime: Key path set: /mnt/nvme0/mu_keys_resnet20/user_0 (pre-generated keys will be loaded)
[INFO] ace.fhe.runtime.runtime: Key mode set: on_demand_full
[INFO] ace.runtime: [phantom] PhantomContext created (N=65536) [2.52s]
[INFO] ace.runtime: [kms] PhantomContext received (N=65536)
[*** LOG ERROR #0001 ***] [2026-08-04 15:56:39] [ace.runtime] argument not found
[INFO] ace.runtime: [kms] Loaded secret_key.bin via staged (N=65536, L=37, max_pow=2)
[INFO] ace.runtime: [kms] Loaded public_key.bin via staged (N=65536, L=37, pk_size=2)
[INFO] ace.runtime: [kms] Loaded relin_key.bin via staged (N=65536, L=37, 9 keys)
[INFO] ace.runtime: [kms] SK+PK+RLK preloaded (ON_DEMAND_FULL) from /mnt/nvme0/mu_keys_resnet20/user_0
[INFO] ace.runtime: [kms] Read /mnt/nvme0/mu_keys_resnet20/user_0/model/galois_key.index.json (version=1, N=65536, L=37, 67 relin keys, 1 disks)
[INFO] ace.runtime: [kms] Loaded galois-key index only: 67 relin keys, 67 elts (ON_DEMAND, no key bytes read)
[INFO] ace.runtime: [kms] ON_DEMAND_FULL ready: 67 galois slots (67 elts) deferred, SK/PK/RLK preloaded (from /mnt/nvme0/mu_keys_resnet20/user_0)
[INFO] ace.runtime: [kms] Rebuilt GaloisTool with 67 loaded galois elements
[INFO] ace.runtime: [kms] Skipping galois key generation in bootstrap — using keys loaded from disk
[INFO] ace.runtime: [kms] bootstrap key prefetch (hoist) 2.567s
  primary user_0   build(ctx+load)=11.59s load=-1.00s init=12.59s  (idx=0)
  [hbm] after primary init: resident ≈ 31448 MiB (galois lazy-empty)
[*** LOG ERROR #0002 ***] [2026-08-04 15:56:48] [ace.runtime] argument not found
[INFO] ace.runtime: [kms] Loaded secret_key.bin via staged (N=65536, L=37, max_pow=2)
[INFO] ace.runtime: [kms] Loaded public_key.bin via staged (N=65536, L=37, pk_size=2)
[INFO] ace.runtime: [kms] Loaded relin_key.bin via staged (N=65536, L=37, 9 keys)
[INFO] ace.runtime: [kms] SK+PK+RLK preloaded (ON_DEMAND_FULL) from /mnt/nvme0/mu_keys_resnet20/user_1
[INFO] ace.runtime: [kms] Read /mnt/nvme0/mu_keys_resnet20/user_1/model/galois_key.index.json (version=1, N=65536, L=37, 67 relin keys, 1 disks)
[INFO] ace.runtime: [kms] Loaded galois-key index only: 67 relin keys, 67 elts (ON_DEMAND, no key bytes read)
[INFO] ace.runtime: [kms] ON_DEMAND_FULL ready: 67 galois slots (67 elts) deferred, SK/PK/RLK preloaded (from /mnt/nvme0/mu_keys_resnet20/user_1)
  create  user_1   idx=1 total= 3.66s galois_load=-1.00s
[*** LOG ERROR #0003 ***] [2026-08-04 15:56:51] [ace.runtime] argument not found
[INFO] ace.runtime: [kms] Loaded secret_key.bin via staged (N=65536, L=37, max_pow=2)
[INFO] ace.runtime: [kms] Loaded public_key.bin via staged (N=65536, L=37, pk_size=2)
[INFO] ace.runtime: [kms] Loaded relin_key.bin via staged (N=65536, L=37, 9 keys)
[INFO] ace.runtime: [kms] SK+PK+RLK preloaded (ON_DEMAND_FULL) from /mnt/nvme0/mu_keys_resnet20/user_2
[INFO] ace.runtime: [kms] Read /mnt/nvme0/mu_keys_resnet20/user_2/model/galois_key.index.json (version=1, N=65536, L=37, 67 relin keys, 1 disks)
[INFO] ace.runtime: [kms] Loaded galois-key index only: 67 relin keys, 67 elts (ON_DEMAND, no key bytes read)
[INFO] ace.runtime: [kms] ON_DEMAND_FULL ready: 67 galois slots (67 elts) deferred, SK/PK/RLK preloaded (from /mnt/nvme0/mu_keys_resnet20/user_2)
  create  user_2   idx=2 total= 3.61s galois_load=-1.00s
  round 0 user_0   switch=   37.0µs infer= 8.37s
  round 0 user_1   switch=   20.3µs infer= 8.31s
  round 0 user_2   switch=   19.4µs infer= 8.31s
  round 0: all users agree ✓

======================================================================
Summary
======================================================================
  users / rounds:  3 / 1
  serve wall:      24.99s  (switches + infers only; setup excluded)
  all correct:     True

  user      create_s  switch_us   infer_s
  user_0       0.00s      37.0µs     8.37s
  user_1       3.66s      20.3µs     8.31s
  user_2       3.61s      19.4µs     8.31s
  mean                    25.6µs (min=19.4µs max=37.0µs)

  primary build (one-time):     11.59s  (context + key load)
  per-user create (one-time):   3.63s avg (22 GiB galois load; resident thereafter)
  switch_session latency:       ~25.6µs  (key rebind, no IO)
======================================================================
```