# Client/Server FHE 推理（Step 1：明文 HTTP 雏形）

服务化路线第 1 步（见 `.claude/plans/multi-user/fhe-service-roadmap.md`）。
把现有 `run_session()` K-pool 模式（`examples/10_multi_user/resnet20/06_server_parallel_pool.py`）
套一层 FastAPI HTTP，让 client 过网络请求 resnet20 推理。**明文进明文出**——
密文序列化 + eval-only server 是第 2 步。

- `server.py`：启动编译一次 + 建 K session（预热，`Ensure_bootstrap_keys` 在
  `create_session`/`Init` 内自动触发，sync-hoist）+ K 个 `ThreadPoolExecutor(1)` slot pool。
  `POST /predict` 按 `user_id%K` 路由到 slot，`run_in_threadpool` 包 `run_session`（不阻塞
  event loop），返回 1×10 logits 为 float32 bytes。`GET /health`。
- `client.py`：`load_cifar10_images` → `numpy.tobytes` → `POST` → `frombuffer` logits；
  支持 `--concurrent/--requests` 吞吐测。

## 前置

1. 已用 `examples/10_multi_user/resnet20/generate_keys.py` 生成多用户 key（含 `meta.json`）：
   ```bash
   python3 examples/10_multi_user/resnet20/generate_keys.py \
     --num-users 3 --key-root /mnt/nvme0/mu_keys_resnet20
   ```
2. 装依赖（仓库默认无）：
   ```bash
   pip install fastapi uvicorn requests
   ```
3. GPU 空闲（cuFile/HBM 竞争会让计时失真）：
   ```bash
   nvidia-smi --query-compute-apps=pid,used_memory --format=csv,noheader
   ```

## 启动 server

```bash
CUDA_VISIBLE_DEVICES=<空闲卡> \
ACE_GDS_ENABLED=0 FHERT_GALOIS_LOAD_THREADS=1 \
python3 examples/11_client_server/server.py \
  --key-root /mnt/nvme0/mu_keys_resnet20 \
  --port 8000 --num-users 3 \
  --io staged --key-mode on_demand_full
```

启动日志应依次出现：`session 0 init=`、`session N (kN) create=`、`[hbm] after K=3 sessions:
resident ≈ 70528 MiB`、`warmup session N: 8.xs`、`ready: 3 slot pools online`、
`Uvicorn running on http://0.0.0.0:8000`。预热冷缓存约 2-4 分钟（含 prefetch 21.4G
bootstrap key/用户）。**重新编译 codegen 后务必先清 kernel cache**
（`rm -rf /tmp/ace-compile-cache/resnet20_cifar10-torch-phantom-cuda/`），否则命中旧行为。

参数：
- `--key-root`：含 `meta.json` 的 key 根目录（必填）。
- `--port`：HTTP 端口（默认 8000）。
- `--num-users`：K，常驻 session 数 = 并发上限（默认 3；H20 96G 下 K≤3，HBM≈K×21.4G）。
- `--io`：`staged`/`gds`/`mapped`/`auto`（默认 staged；SSD+cuFile 环境用 gds，prefetch ~1-2s）。
- `--key-mode`：`on_demand_full`/`preload_full`（默认 on_demand_full）。
- `--host`：绑定地址（默认 0.0.0.0）。

## 跑 client

```bash
# 单请求（user_id 路由到 session user_id%K）
python3 examples/11_client_server/client.py \
  --url http://localhost:8000 --user-id 0

# 并发吞吐测（K 路并行，请求 user_id = i%concurrent）
python3 examples/11_client_server/client.py \
  --url http://localhost:8000 --concurrent 3 --requests 6

# 超限并发（K=3 时 5 路排队 + 25s 超时兜底）
python3 examples/11_client_server/client.py \
  --url http://localhost:8000 --concurrent 5 --requests 5
```

参数：
- `--url`：server 地址（默认 `http://localhost:8000`）。
- `--user-id`：用户 id，决定路由 slot（默认 0）。
- `--image`：CIFAR-10 测试图索引（默认 0）。
- `--concurrent`：并发 worker 数（默认 1）。
- `--requests`：总请求数（默认 1）。

单请求返回示例：`wall 8.74s`，`logits: [-1.46, -2.69, ..., 10.94, ...]`（1×10 float32）。

## 停 server

```bash
pkill -9 -f "11_client_server/server.py"
```

## 实测基线（resnet20, H20, on_demand_full, staged, K=3）

| 场景 | 结果 |
|------|------|
| 单请求（user_id=0） | wall 8.74s，logits 多次完全一致，正确性稳定（守 8s SLA） |
| K=3 session 建成 | HBM ~70.5 GiB，warmup 8.8s/session |
| 3 路并行单图 | ~12-16s/图（GPU contention，详见 `examples/10_multi_user/report.md` K=3 wall 16.2s） |

`INFER_TIMEOUT_S=25`（8.5s 单图 / ~16s 3 路竞争 + 余量），超时返 503。

## 已知限制

- **并发 ≥2 可能触发 phase race**：executor session 状态机在并发异常路径下不回
  `IDLE`（`Prepare_input_only: invalid phase transition (expected IDLE, got 2)`），
  可能卡死 session。与 `examples/10_multi_user/report.md #1` 的 B-P K≥2 race 同根
  （`Io_get_input` 全局单例 + `cuda_auto_ptr` copy stream）。**单请求路径稳定**；
  并发服务需先修底层 phase race（根治方向在 fhe-cmplr MFREE borrowed 或 per-session Io）。
- 第 1 步明文进明文出，server 持 SK/PK（`can_verify()==true`）。第 2 步见下方。

---

## 第 2 步：密文进密文出（`--mode ciphertext`）

client 用 PK 加密 → server 只 eval（只持 RLK+GK，**不带私钥**）→ client 用 SK 解密。

**密钥分工**：

| 角色 | 模式 | 密钥 | 能力 |
|------|------|------|------|
| server | `preload`（`--mode ciphertext` 强制） | RLK+GK，**SK/PK = nullptr** | 只能 eval（`run_cipher`），不能 encrypt/decrypt |
| client | `on_demand_full`（`--mode ciphertext` 自动） | SK+PK+RLK（galois **仅 index**，不加载 21 GiB 字节） | encrypt input（PK）/ decrypt output（SK） |

> Client 用 `on_demand_full` 而非 `preload_full`：client 只加密/解密、从不 eval，所以**永远不碰 21 GiB galois key**
>（galois 只读 index，shell 留空，从不 Ensure）。实测 client HBM ≈ **930 MiB**（context + SK/PK/RLK），相比
> `preload_full` 的 ~22 GiB 砍掉 95%。

`KeyMode::PRELOAD`（`fhe_lib/phantom/src/kms/key_mgr.cu:90-118`）只载 eval keys，`_sk`/`_pk` 保持 `nullptr` ——
server 真正不带私钥。`EncryptInput`/`DecryptOutput`（`fhe_lib/include/fhe/kms/kms_api.h:349,563`）在 session 无
SK/PK 时直接抛异常，故 encrypt/decrypt 必须在 client 侧完成。

### 启动 server（ciphertext，eval-only）

```bash
CUDA_VISIBLE_DEVICES=<空闲卡> \
ACE_GDS_ENABLED=0 FHERT_GALOIS_LOAD_THREADS=1 \
python3 examples/11_client_server/server.py \
  --key-root /mnt/nvme0/mu_keys_resnet20 \
  --port 8000 --num-users 1 \
  --io staged --mode ciphertext
```

日志应见 `KeyMode::PRELOAD — loading eval keys (RLK+GK)`，**不**出现 `Loaded secret_key.bin`。ciphertext 模式
server **不在启动时 run_session warmup**（无 SK 会抛异常），改由 client 的 `/warmup_cipher` 握手驱动预热
（见下）。端点：`POST /predict_cipher`（密文进、密文出）、`POST /warmup_cipher`（握手预热，触发 lazy setup +
bootstrap ensure）、`GET /health`。

### 跑 client（ciphertext）

```bash
python3 examples/11_client_server/client.py \
  --url http://localhost:8000 \
  --mode ciphertext --key-root /mnt/nvme0/mu_keys_resnet20 --user-id 0
```

client 流程：起 `on_demand_full` 1-session runtime（持 SK/PK，galois 仅 index）→ ctypes 调 `Kms_encrypt_input` 加密一张 CIFAR
图（~17.8 MiB 密文）→ `POST /warmup_cipher` 握手预热 → `POST /predict_cipher` 发密文 → 收 output 密文
（~1 MiB）→ ctypes 调 `Kms_decrypt_output` 解码 logits。`--no-warmup` 跳过握手（首个请求会冷启慢）。

> C ABI（`Kms_encrypt_input`/`Kms_decrypt_output`）暂无 Python 绑定，client 走 ctypes 直调
> `libFHErt_phantom.so`（复用 `test_ciphertext_e2e.py` 的绑定模式）。client 也有冷启成本：首次编译 resnet20
> kernel（之后走 `/tmp/ace-compile-cache` 命中、跳过编译）+ 载 SK/PK/RLK ~20s。此后每次请求只花
> encrypt（~0.1s）+ 网络 + server eval（~17s）+ decrypt（~0.01s）。

### client 足迹（实测，CUDA_VISIBLE_DEVICES=0，server 同卡）

| 指标 | 值 |
|------|------|
| client HBM 增量（稳态，runtime 持有） | **~930 MiB**（vs preload_full ~22 GiB） |
| galois key 字节加载 | **0**（ON_DEMAND 只读 index，client 不 eval → 不 Ensure） |
| client runtime 冷启 | ~20s（编译走 cache + 载 SK/PK/RLK） |
| encrypt 单图 | ~0.1s → 17.8 MiB 密文 |
| 单请求端到端（warm） | server `/predict_cipher` ~17s（含 eval） |

### 后续：真正的最小化 client（尚未实现）

当前 client 仍有两处"不够轻"，是**后续 C++ 工作**，不在本次范围：

1. **依赖 kernel .so dlopen**：`Get_context_params()`（N/L/scale）是 code-gen 时实例化进 kernel.so 的全局符号，
   `Kms_encrypt_input` 要读它 → client 必须加载那个 .so（cache 命中后只读、不重编，但仍要 dlopen + 起 session）。
2. **仍占少量 GPU**：phantom 后端的 `encode`/`encrypt` 跑 NTT，是 GPU kernel，所以 HBM ~930 MiB + encrypt 时瞬时 NTT。
   完全"无 GPU client"需要 C++ 侧一个 standalone **encrypt-only API**（context params 从 sidecar json 读、不靠
   dlopen kernel.so、不依赖 eval session），目前代码里不存在。这是最小化 client 的实现路径，记为后续项。

### 密文 vs 明文 payload

| 方向 | plaintext | ciphertext |
|------|-----------|------------|
| 上行（input） | ~12 KB（float32 1×3×32×32） | ~17.8 MiB（序列化密文） |
| 下行（output） | ~40 B（10×float32 logits） | ~1 MiB（序列化密文） |

ciphertext 模式网络传输量大三个数量级，`requests.post` timeout 已放宽（predict 120s / warmup 120s），
uvicorn 默认不限 body 大小。

### 预期结果

ciphertext 模式 logits 应与 e2e 基线一致（`[-1.457, -2.691, -1.091, 10.939, -3.762, 8.02, -0.245, -2.88,
-1.968, -4.865]`，atol=1.0），与 plaintext 模式 logits 逐位一致（同 CIFAR 图、同密钥）。

### 退路

若 server 在 `preload` 下 `Ensure_bootstrap_keys`（`create_session`/`Init` 内 sync-hoist）报错（理论上
bootstrap key 是 galois key 子集，`preload` 已载全部 GK，应能完成，但需实测）→ 退回 `--key-mode
on_demand_full`（server 带 SK，但 `run_cipher` 不碰 SK），先跑通密文链路，再单独剥离 SK。



### Web client（`web_client.py`）

浏览器 UI：选 server URL → 从 100 张 CIFAR-10 预设图选一张 → 点「Infer」→ 显示 logits + 分段计时。
本地 web server（FastAPI）做加密代理：浏览器不能跑 phantom GPU 加密，所以「点推理」→ 本地后端
encrypt → 转发 server `/warmup_cipher`+`/predict_cipher` → decrypt → 回 logits。复用 `client.py` 的
加密/解密逻辑（`on_demand_full`，galois 不加载，`FHERT_SKIP_BOOTSTRAP_HOIST=1` 跳过 21 GiB bootstrap hoist）。

计时分两栏：
- **Main flow**：`client: encrypt` / `server: warmup(一次性)` / `server: eval` / `network` / `client: decrypt`
- **server eval detail**：server 把 `run_cipher` 的内部分段（deserialize / launder / **pure GPU eval (kernel)**
  / serialize）通过 `X-RC-*` HTTP header 回传，本地后端展开显示，末尾标「pure eval is X% of the server wall」。
  `run_cipher` 单 pass（kernel 只跑 1 次），稳态单请求 ≈ 8.9s（其中纯 GPU eval ≈ 8.8s）。

```bash
# 1. server（ciphertext / eval-only / 无 SK），另一终端
CUDA_VISIBLE_DEVICES=1 ACE_GDS_ENABLED=0 FHERT_GALOIS_LOAD_THREADS=1 \
  python3 examples/11_client_server/server.py \
    --key-root /mnt/nvme0/mu_keys_resnet20 --port 8000 --num-users 1 \
    --io staged --mode ciphertext

# 2. web client（本地，需 GPU 加密代理）
CUDA_VISIBLE_DEVICES=0 ACE_GDS_ENABLED=0 FHERT_GALOIS_LOAD_THREADS=1 \
  python3 examples/11_client_server/web_client.py \
    --key-root /mnt/nvme0/mu_keys_resnet20 --port 5500 \
    --server-url http://localhost:8000

# 3. 浏览器开 http://localhost:5500
```

> server 终端可加 `ACE_RUNCIPHER_TIMING=1`，额外打印 `[runcipher] split KERNEL(eval)=Xs` 行
> （网页已自带分段计时，不设也行）。

### 构建（重要：acelib 依赖钉版本）

上游 `acelib`（`DEP_HYPERFHE_REF`，`cmake/AceDependencies.cmake` 默认 `master`）是浮动 ref。
当前 master（`6717c5e`）改了 `bootstrap::CryptoParameters` 接口（删 `logQ0` 等），与
`fhe_lib/ace/src/ace_config.cu` 不兼容，**`dev-build.sh` 会编不过**。需钉到 `9ef4769`：
```bash
DEP_HYPERFHE_REF=9ef4769 CI_TOKEN=f8fee7370ee5492eb79244d0a2aca4c4 ./scripts/dev-build.sh
```
（若要持久化，把 `set(DEP_HYPERFHE_REF "9ef4769")` 加进 `.aci/config-internal.cmake`。本轮改动未改该文件。）

### Test docker shaw-dev-client

```bash
docker run -it --name shaw-dev-client -v ~/work:/work -v /opt/dataset:/opt/dataset \
  -p 5500:5500 -p 8000:8000 --gpus all \
  reg.docker.alibaba-inc.com/ace/dev-cuda:12.8.0-devel-ubuntu24.04 bash

# generate keys
python3 examples/10_multi_user/resnet20/generate_keys.py \
  --num-users 3 --key-root /mnt/nvme0/mu_keys_resnet20

# build (pin acelib)
DEP_HYPERFHE_REF=9ef4769 CI_TOKEN=f8fee7370ee5492eb79244d0a2aca4c4 ./scripts/dev-build.sh

# server
CUDA_VISIBLE_DEVICES=1 ACE_GDS_ENABLED=0 FHERT_GALOIS_LOAD_THREADS=1 \
  python3 examples/11_client_server/server.py \
    --key-root /mnt/nvme0/mu_keys_resnet20 --port 8000 --num-users 1 \
    --io staged --mode ciphertext

# web client
CUDA_VISIBLE_DEVICES=0 ACE_GDS_ENABLED=0 FHERT_GALOIS_LOAD_THREADS=1 \
  python3 examples/11_client_server/web_client.py \
    --key-root /mnt/nvme0/mu_keys_resnet20 --port 5500 \
    --server-url http://localhost:8000

# 浏览器开 http://localhost:5500
```