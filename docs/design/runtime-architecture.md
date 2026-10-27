# ACE Runtime — Four-Layer Architecture

> Status: living document · reflects the codebase as of mid-July 2026
> Scope: the inference-time runtime (`ace.runtime` / `fhe_lib/`), **not** the compiler pipeline
> Companion: [runtime-vendor-integration.md](runtime-vendor-integration.md) — how a new backend is wired in, layer by layer

This document describes the ACE Runtime as a **four-layer architecture**: the responsibilities and API
surface of each layer, the contracts between layers, and how the current implementation maps to the
target design. It is the "what and why". The companion integration guide is the "which file, which
symbol".

---

## 1. Design Principles

1. **Layer names describe responsibilities, not implementations.**
   "Execution Engine", not `FheContext`. A reader should understand the layer from its name alone.

2. **Architecture diagrams express API surface and responsibilities, not class decomposition.**
   `runtime.run(inputs)` rather than `FHERuntime (CompiledProgram / KernelExecutor)`.

3. **The implementation moves toward the architecture, not the reverse.**
   Where the current code does not yet satisfy the target, it is called out as an evolution item
   (Section 8) rather than silently back-fit into the diagram.

4. **Strict unidirectional dependency: L1 → L2 → L3 → L4.**
   L1 may declare policy *intent* across layers, but execution flows only through L2.

---

## 2. Four-Layer Overview

```
┌──────────────────────────────────────────────────────────────────────┐
│  L1  User API Layer  (Python)                                        │
│      FHERuntime · CompiledProgram · DatasetEvaluator · KernelExecutor · policy settings │
│      — inference primitives · evaluation orchestration · session lifecycle · policy config │
├──────────────────────────────────────────────────────────────────────┤
│  L2  Execution Engine Layer  (C++)                                    │
│      FheContext<Backend>  (FheSession / KeyMgr / Evaluator / EncMgr)  │
│      — crypto context lifecycle · HE operations · kernel execution     │
├──────────────────────────────────────────────────────────────────────┤
│  L3  Data Management Layer  (KMS ∥ ENC)                               │
│      KMS: KeySerializer / KeyIndex / KeyIO                            │
│      ENC: EncMgr / EncPipeline / EncIo                                │
│      — key & weight lifecycles · serialization · on/offline encoding   │
├──────────────────────────────────────────────────────────────────────┤
│  L4  Hardware Abstraction Layer                                       │
│      BackendProvider (VendorTraits) · IoTransport / GdsTransport ·     │
│      IoBulkReader · ResourcePool                                      │
│      — compute backend · IO transport routing · buffer registration    │
└──────────────────────────────────────────────────────────────────────┘
```

**Dependency rule:** Layer N depends only on Layer N+1 and below. Python (L1) crosses into C++ (L2) via
the pybind11 `_C` module. Business layers (L2/L3) speak to L4 through **intent declarations**
(`IoPolicy`) rather than concrete transports (`IoConfig`).

---

## 3. L1 — User API Layer (Python)

### 3.1 Responsibilities

The only surface a Python user touches. Holds two orthogonal concerns:

- **Inference execution** — running a compiled FHE program (`CompiledProgram`), managing a runtime
  (`FHERuntime`), driving kernel lifecycle (`KernelExecutor`).
- **Orchestration & policy** — choosing key-load mode, encoding strategy, IO transport, and passing
  the compiled artifact + runtime policy together via a *package descriptor*.

L1 never touches ciphertext bytes, key material, or GPU memory directly.

### 3.2 Core API

The L1 surface separates **inference primitives + lifecycle** (`FHERuntime`, `KernelExecutor`) from
**evaluation orchestration** (`DatasetEvaluator`). `FHERuntime` owns computation; `DatasetEvaluator`
owns accuracy/metric protocol.

#### FHERuntime — inference primitives + lifecycle

| Method | Signature | Note |
|--------|-----------|------|
| `__init__` | `(package, *, auto_init=False, validate_atol=None, validate_rtol=None)` | Process-wide singleton — constructing one auto-finalizes the previous |
| `init` | `() → None` | Initialize the FHE context; idempotent; auto-called on first `run`/`run_batch` |
| `finalize` | `() → None` | Release FHE resources; reset C++ global state (`key_path`, `key_mode`); idempotent |
| `run` | `(*inputs) → Tensor` | Single FHE inference (auto-cast to 4D) |
| `run_batch` | `(batch, parallel, num_threads, verbose) → BatchResult` | Batched inference |
| `validate` | `(result, expected) → bool` | FHE-vs-plaintext comparison |
| `__enter__` / `__exit__` | — | Deterministic scoping — `__exit__` calls `finalize()` |

`auto_init=True` eager-initializes at construction; default `False` defers to first use.
`validate_atol` / `validate_rtol` are reserved for a pending C++ `Validate_result` binding extension
— thresholds are env-var driven today (`ABS_ERROR` / `REL_ERROR`).

#### DatasetEvaluator — evaluation orchestration

| Method | Signature | Note |
|--------|-----------|------|
| `__init__` | `(runtime: FHERuntime)` | Wraps a live runtime; the caller owns its lifecycle |
| `evaluate` | `(images, labels, *, top_k, parallel, num_threads, verbose, plaintext_predictions) → DatasetResult` | Dataset inference + accuracy metrics |

Routes through `runtime.run_batch(...)` and computes argmax / top-k / FHE-vs-plaintext match rate.
Context lifecycle is the caller's — the evaluator never calls `init()`/`finalize()`.

**Package descriptor:**

```python
{
    "model": "Add",                   # model name (derives config path)
    "kernel": "/path/to/kernel.so",   # compiled FHE shared library
    "config_path": "/path/to.conf",   # optional: explicit config
    "key_path": "/path/to/keys",      # optional: pre-generated key directory
    "key_mode": "preload_full",       # optional: key-load mode
    "load_full_keys": True,           # deprecated: → preload_full
}
```

#### CompiledProgram — callable compiled artifact

Thin lazy wrapper around `FHERuntime`: `__call__` allocates a runtime and runs `run`;
`runtime()` yields the underlying `FHERuntime`; `validate()` is a zero-arg self-check against
compile-time example inputs; `profile()` runs a perf trace.

#### KernelExecutor — kernel lifecycle

`init()` / `finalize()` / `execute(*tensors)` / `execute_batch(…)` / `capture_graph()` (ACE backend
only). Owns the phase-split execution state machine (Section 6.4).

### 3.3 Result types

| Type | Fields | Produced by |
|------|--------|-------------|
| `BatchTiming` | `total_ms`, `avg_per_image_ms`, `min_image_ms`, `max_image_ms`, `num_images` | `run_batch` / `evaluate` |
| `BatchResult` | `outputs`, `timing`, `num_success`, `num_failure` | `run_batch` |
| `DatasetResult` | `predictions`, `labels`, `top1_accuracy`, `top5_accuracy`, `fhe_match_rate`, `timing` | `DatasetEvaluator.evaluate` |

### 3.4 Process-wide singleton & scoping

`FHERuntime` is a **process-wide singleton** — the C++ globals (`CONFIG_MANAGER`, `key_path`,
`key_mode`) admit exactly one live instance at a time. Cold-start benchmarks therefore use a
subprocess per configuration. Within a single process, the context manager owns deterministic release:

```python
with FHERuntime(pkg) as rt:
    out = rt.run(x)
    result = DatasetEvaluator(rt).evaluate(images, labels)
# __exit__ finalizes the context
```

---

## 4. L2 — Execution Engine Layer (C++)

### 4.1 Responsibilities

Owns the FHE compute context and drives the computation:

- Create/destroy the cryptosystem context.
- Dispatch HE operations (Add, Mul, Rotate, Rescale, Relin, Bootstrap).
- Schedule kernel execution — load the compiled `.so`, split it into prepare/execute/output phases.
- Bind a compute context to the calling thread (thread-local `Current()`).

L2 does **not** manage key/weight *storage* (that is L3) or *transport* (that is L4); it obtains data
from L3 and moves bytes through L4.

### 4.2 Core structure

```
FheContext<Backend>                 ← top-level: shared compute resources
  ├── _crypto_ctx                   ← Backend::Context (e.g. PhantomContext)
  ├── _session (FheSession)         ← primary session: keys + encrypt/decrypt
  │   ├── _key_mgr (KeyMgr)         ← key lifecycle (SK/PK/RLK/GK)
  │   └── _crypto (Crypto)          ← Encryptor / Decryptor
  ├── _active_session → _session    ← active-session pointer (switch_session)
  ├── _evaluator (Evaluator)        ← shared HE compute engine
  ├── _enc_mgr (EncMgr)             ← weight encoding manager
  └── _bootstrap (BtsMgr)           ← bootstrap manager
```

`FheContext` is a **template over `Backend`** (a `VendorTraits` struct, see L4 / companion guide).
There is no factory and no virtual interface — dispatch is resolved entirely at compile time via
templates. A `static_assert(is_valid_vendor_traits_v<Backend>)` enforces that the backend provides
every required type alias and operation.

### 4.3 Lifecycle

```cpp
auto ctx = FheContext<Backend>::Create(
    KeyMode::PRELOAD_FULL,    // how keys are obtained
    EncPolicy::ON_DEMAND);    // when weights are encoded  (default)
ctx->MakeCurrent();           // install thread-local current

Ciphertext res;
ctx->Add(res, ct1, ct2);      // HE ops resolve via FheContext<Backend>::Current()
```

Destruction order (RAII custom deleter): `_evaluator` → `_session` (KeyMgr release) → `_crypto_ctx`.
`Finalize_context` always calls `cudaDeviceSynchronize()` before tearing down, so it does not rely on
the Python side to drain the device.

**Thread-local `Current()`:** the codegen dispatch layer (`rt_phantom.h` / `rt_ace.h`) resolves the
active context via `FheContext::Current()` rather than a process singleton. This is what would let
different threads use different contexts, though the architecture is single-user today.

### 4.4 KeyMode and EncPolicy

#### KeyMode — how keys are obtained

| Mode | C++ value | SK/PK | RLK/GK | `can_verify()` |
|------|-----------|-------|--------|----------------|
| `ONLINE` | 0 | GPU-generated | GPU-generated | ✓ |
| `PRELOAD` | 1 | dummy | disk-loaded | ✗ |
| `ON_DEMAND` | 2 | dummy | on-demand (reserved) | ✗ |
| `PRELOAD_FULL` | 3 | disk-loaded | disk-loaded | ✓ |
| `ON_DEMAND_FULL` | 4 | on-demand | on-demand (reserved) | ✓ |

> Aliases: `key_offline` → `PRELOAD_FULL`, `server_only` → `PRELOAD`.

In `PRELOAD`, RLK/GK come from disk but SK/PK are placeholders produced by
`generate_dummy_keys_for_evaluator()`. That mode cannot encrypt/decrypt — the client owns those
steps — so `Prepare_input()` / `Handle_output()` raise.

#### EncPolicy — when weights are encoded

| Policy | Behavior | init time | per-inference cost |
|--------|----------|-----------|--------------------|
| `ON_DEMAND` (default) | weights stay on disk; encoded as used | short | encoding per inference |
| `PRELOAD` | all weights encoded into HBM at init | longer | zero encoding |
| `BYPASS` | skip encoding mgr (encoder + params only) | shortest | none (offline-encoded path) |

> Default is `ON_DEMAND` to avoid init-time OOM on large models with limited HBM. Switch to `PRELOAD`
> when HBM permits and latency matters.

### 4.5 Encoding operations (L2 surface)

| API | Note |
|-----|------|
| `Encode_float(pt, input, len, scale, level)` | encode a float array to Plaintext |
| `Encode_float_cst_lvl(pt, input, len, scale, level)` | encode to a specific level |
| `Encode_float_mask(pt, input, len, scale, level)` | encode a scalar-broadcast mask |

`EncMgr::Get_or_encode()` unifies the three sources of a plaintext (preload cache hit ·
`DE_PLAINTEXT` file · `DE_MSG_F32` file) — see L3.

### 4.6 KernelRunner — compiled-kernel execution

```
KERNEL_RUNNER
  ├── dlopen(kernel.so)        ← RTLD_LOCAL | RTLD_NODELETE (isolates _cst_N symbols)
  ├── Prepare_context()        ← FheContext::Create + MakeCurrent
  ├── Prepare_input()          ← encode input data
  ├── Main_graph()             ← HE computation
  ├── Handle_output()          ← decode output
  └── Finalize_context()
```

**Phase-split execution** (for OpenMP-parallel batching):

```
IDLE → Prepare_input_only() → PREPARED
PREPARED → Execute() → EXECUTED      ← parallelizable
EXECUTED → Get_output() → IDLE       ← serial
```

The runtime dlopen's the kernel `.so` and resolves 11 `extern "C"` symbols via `dlsym` (5 lifecycle +
6 KMS — fully specified in the companion guide, Section 4). This is the **single contract** between
the Python `_C` extension and any backend; the extension itself is backend-agnostic.

---

## 5. L3 — Data Management Layer (KMS ∥ ENC)

KMS and ENC are **peer sub-modules**: neither depends on the other; both depend on L4 (IO) and are
both invoked by L2 (`FheContext`).

There are exactly **three kinds of data** in an FHE runtime — keys, encoded weights, and encrypted
I/O. L3 owns the lifecycle and *orchestration* of keys and weights; L2 owns their *consumption*.

### 5.1 KMS — Key Management

#### Key directory layout

```
key_dir/
├── metadata.json                 # key-bundle metadata
├── meta.json                     # inference-info written by sample scripts
├── global/                       # encryption keys (SK + PK) — absent when eval_only
│   ├── secret_key.bin
│   └── public_key.bin
└── model/                        # evaluation keys (RLK + GK + BK)
    ├── relin_key.bin
    ├── galois_key.index.json     # v3 multi-file index
    ├── galois_key.merged.*.bin   # merged sub-files (GDS-friendly)
    ├── galois_key.NN.bin         # …or per-key format
    └── bootstrap_key.bin         # optional
```

- `global/` is omitted under `eval_only=True` (`SERVER_ONLY`).
- `model/` is the minimum set loaded under `PRELOAD` / `PRELOAD_FULL`.

#### C++ key management

| Class | Header | Responsibility |
|-------|--------|----------------|
| `KeyMgr<Backend>` | `fhe/kms/key_mgr.h` | key lifecycle: generate / KMS-load / dummy / encode-only |
| `KeySerializer<Backend>` | `fhe/kms/key_serializer.h` | serialize galois keys (per-key / merged) |
| `GaloisKeyIndex` | `fhe/kms/key_index.h` | v3 multi-file JSON index |
| `KeyIO` | `fhe/kms/key_io.h` | low-level key file IO |

#### Python API (`ace.kms`)

`OfflinePrep` — offline preprocessing: `encode()` (DE_MSG_F32 → DE_PLAINTEXT), `encode_package()`
(end-to-end), `update_config()` (point `.conf` at encoded weights), `save_keys()`. `OfflineEncoder`
is a backward-compat alias.

`generate_encryption_keys(params, dir)` / `generate_evaluation_keys(compiled_dir, dir)` /
`save_key_bundle` / `load_key_bundle` / `list_keys` / `get_key_info` / `CKKSParams` / `read_conf`.

> Note: the Python key-generation bridge is partial — directory structures and metadata are created in
> Python, but the binary key blobs must still be produced via `OfflinePrep.save_keys()` or
> `FHERuntime.init()` + the backend `Kms_save_keys` symbol.

### 5.2 ENC — Encoding Management

#### Weight file formats

| Format | Constant | Meaning | Encode cost at inference |
|--------|----------|---------|--------------------------|
| `DE_MSG_F32` | 1 | raw float32 | NTT-encode on use |
| `DE_PLAINTEXT` | 2 | pre-encoded CKKS Plaintext | just deserialize |

#### EncMgr — the encoding manager

```
EncMgr::Init(weight_file, ctx, enc_policy)
  ├── DE_MSG_F32  + ON_DEMAND → Load_encode() per use
  ├── DE_MSG_F32  + PRELOAD   → Preload_encode() at init
  ├── DE_PLAINTEXT           → Load_plaintext() (pread-deserialize) — policy-independent
  └── BYPASS                 → no EncMgr (encode-only context)

EncMgr::Get_or_encode(pt, index, …)
  ├── preload cache hit? → return cached Plaintext (D2D copy)
  ├── DE_PLAINTEXT?       → pread + deserialize
  └── DE_MSG_F32?         → Load_encode (NTT + encode)
```

The preload cache stores encoded `Plaintext` objects in a vector; because the backend's GPU pointer
type uses deep-copy semantics, reading from cache still costs a D2D copy — but far cheaper than
re-encoding.

#### EncPipeline — offline encoding

`EncPipeline<Backend>::encode_file(in, out)` spins up an encode-only context
(`KeyMode::ONLINE` + `EncPolicy::BYPASS`), encodes each weight entry to a Plaintext, and serializes
it. The encode-only context has only an encoder + CKKS parameters — no evaluator, keys, or weights.

#### EncIo — plaintext serialization

`serialize(pt, ctx)` / `deserialize(buf, pt, ctx)`, with backend-specific memory layouts (Phantom
and ACE differ).

---

## 6. L4 — Hardware Abstraction Layer

L4 hides all hardware specifics behind a uniform capability interface. Two concerns live here:
**IO transport** and **compute backend**.

### 6.1 IoPolicy vs IoConfig — intent vs tuning

```
business layer declares intent:  IoPolicy::auto   "I want the fastest path"
                                         ↓
IO layer resolves intent:        cuFile available? → GdsTransport
                                 else?             → StagedTransport
```

Business layers (`KeySerializer`, `KeyMgr`) include only `io_policy.h` and never name a transport
class. Tuning knobs (`IoConfig`: chunk_size, threads, disk_paths) are expert overrides, not the
default API.

| Policy | Transport | GDS? |
|--------|-----------|------|
| `auto` (default) | GDS if cuFile available, else staged | auto-detect |
| `gds` | cuFile DMA, fall back to staged on error | yes |
| `staged` | fread + cudaMemcpy (safest) | no |
| `mapped` | mmap + cudaMemcpy (one fewer copy) | no |

| `IoConfig` param | Default | Note |
|------------------|---------|------|
| `fallback_on_error` | `true` | silent GDS→staged fallback |
| `chunk_size` | 8 MiB | optimal GDS DMA granularity |
| `num_io_threads` | 4 | parallel GDS read threads |
| `disk_paths` | `[]` | multi-disk paths (`[]` = single-disk) |
| `galois_merged_format` | `true` | merged galois-key file format |
| `exec_mode` | `sync` | async/pipeline reserved |

### 6.2 C++ IO transport hierarchy

```
IoTransport (abstract)
  ├── read / read_gpu           ├── write / write_gpu
  ├── register_buffer(gpu, sz)  └── deregister_all
      ├── StagedTransport       — fread + cudaMemcpy
      ├── MappedTransport       — mmap + cudaMemcpy
      └── GdsTransport          — cuFile DMA + auto-fallback
            └── gds_parallel_read()  — multi-handle atomic chunk-steal engine
```

`GdsTransport` auto-manages `cuFileBufRegister` lifetime, reads in `chunk_size`-granular atomic
chunk-stealing, and transparently falls back to `StagedTransport` on cuFile error. The single IO
entry point for business code is the factory:

```cpp
auto reader = rtio::open_reader(path, rtio::IoLoc::Device, policy);
auto writer = rtio::open_writer(path, rtio::IoLoc::Device, policy);
```

### 6.3 IoBulkReader — multi-file parallel read

Groups files by disk (path-prefix match), launches one read thread per disk group, and reads files on
the same disk serially. Modes: independent buffers `read(files, buffers)`, single file
`read(path, buf, size)`, or VRAM-constrained `read_reuse_buffer(…)`.

### 6.4 BackendProvider — compute backend

The compute backends (GPU: Phantom/ACE; CPU: ANT, with OpenFHE/SEAL stubbed) are abstracted by a
**`VendorTraits` struct** consumed as a C++ template parameter of `FheContext<Backend>`. Each backend
provides type aliases (keys, context, plaintext, ciphertext) and static methods (the HE ops) that
satisfy a compile-time concept (`concept.h`). The codegen dispatch header `rt_<backend>.h` routes
inline op calls to `FheContext<VendorTraits>::Current()`.

This is the mechanism a new vendor plugs into — it is the entire subject of the companion guide.

---

## 7. Inter-Layer Contracts and Data Flow

### 7.1 Three data kinds across layers

| Data | Produced by | Persisted by | Consumed at inference |
|------|-------------|--------------|-----------------------|
| Keys | L2 (KeyMgr generate) | L3 (KeySerializer → disk via L4 IO) | L2 (KeyMgr holds; Evaluator uses) |
| Encoded weights | L2 (EncMgr encode) | L3 (EncIo serialize → disk via L4 IO) | L2 (FheContext uses) |
| Encrypted I/O | L2 (encrypt/decrypt) | not persisted | L2 (inference input/output) |

L3 is the **persistence + orchestration** layer; L2 is the **consumption** layer. L2 hands data to
L3 for management and asks it back on demand. The split between L3 and L4 is: L3 decides *what to
read and in what order* (orchestration); L4 decides *how to move the bytes* (transport).

### 7.2 Typical inference (ONLINE mode)

```
Python                              C++ Runtime
───────                             ──────────
FHERuntime(package)
  ConfigLoader → _C.set_config() ──→ CONFIG_MANAGER.Set_config() / Freeze_config()

KernelExecutor.init()
  _C.ProviderManager.init() ──────→ KERNEL_RUNNER.Init()
                                     Prepare_context()
                                       FheContext::Create(ONLINE, ON_DEMAND)
                                         KeyMgr::Init() → generate SK/PK/RLK/GK
                                         EncMgr::Init() → load weights
                                       FheContext::MakeCurrent()

FHERuntime.run(x)
  KernelExecutor.execute() ────────→ KERNEL_RUNNER.Run()
                                     Prepare_input() → encode input
                                     Main_graph()    → HE compute
                                     Handle_output() → decode output
```

### 7.3 Key-load flow (PRELOAD mode)

```
_C.set_key_path(dir) ─────────────→ CONFIG_MANAGER.Set_key_path()
KeyMode.apply("preload") ─────────→ Set_key_mode(PRELOAD)

KernelExecutor.init() ────────────→ KERNEL_RUNNER.Init()
                                     Prepare_context() → FheContext::Create(PRELOAD, …)
                                       KeyMgr::set_key_mode(PRELOAD)
                                       KeyMgr::Init() → Load_keys(dir)
                                         KeySerializer::Load_keys()
                                           GaloisKeyIndex::load()
                                           IoBulkReader::read() → multi-disk GDS
                                         generate_dummy_keys_for_evaluator()  (dummy SK/PK)
                                       EncMgr::Init()
                                       MakeCurrent()
```

### 7.4 Offline encoding flow

```
OfflinePrep(kernel)
  ctypes.CDLL(kernel, RTLD_GLOBAL)        # resolve kernel symbols
  ctypes.CDLL(libFHErt_<backend>.so)      # load backend lib
prep.encode_package(package)
  ├── prep.encode(weight_file) ────→ Kms-less encode path (EncPipeline)
  │                                    encode-only context (no keys)
  │                                    per LUT entry: Encode_float → serialize → DE_PLAINTEXT
  └── OfflinePrep.update_config()
      config["rt_data_info"]["_entry_type"] = 2    # DE_PLAINTEXT
      config["rt_data_info"]["_file_name"]   = encoded_weight

runner = FHERuntime(encoded_package)
runner.run(x) → EncMgr detects DE_PLAINTEXT → Load_plaintext()  (zero NTT)
```

### 7.5 IO policy configuration flow

```
IoPolicySettings(policy="auto", disk_paths=[...]).apply()
  _C.set_io_policy_with_disk_paths(auto, paths) ─→ rtio::set_global_io_policy(auto)
                                                    rtio::set_global_io_config({disk_paths})

# Subsequent IO (KeySerializer::Load_keys → IoBulkReader::read → open_reader(..., auto))
# picks GdsTransport iff cuFile is available.
```

---

## 8. Current Implementation vs Target (Gaps & Evolution)

The architecture names and layering above are the **target**. Gaps against today's code are listed
here as evolution items, not silently rewritten into the diagrams.

| Gap | Today | Target | Priority |
|-----|-------|--------|----------|
| L2 naming | "Server Context Management" | "Execution Engine" | High — rename only |
| Architecture diagrams | list impl classes | express API surface & responsibilities | High — redraw |
| `FheContext` visibility in L2 diagram | class decomposition | "compute context" responsibility only | Medium |
| `CompiledProgram` role | thin lazy `FHERuntime` wrapper (L1) | true DAG executor in L2 | Medium — refactor |
| `EncMgr` ownership | L2 (`FheContext` holds it) | L3 (data management) | Medium — short-term impl compromise |
| Compute backend placement | implicit template param, no arch home | explicit L4 (backend provider) | Low — `VendorTraits` already sits at L4 in fact |
| Encrypted I/O data mgmt | scattered in `Prepare_input` / `Handle_output` | unified L3 abstraction | Low |
| `KeyCache` | does not exist | L3 | Low — perf phase |
| `GpuMemoryPool` / `ResourcePool` | partial (`cuFileBufRegister`) | L4 | Low — resource phase |
| SPDK integration | absent | L4 optional | Low — on demand |
| `prefetch` key/load strategy | reserved, unimplemented | L4 async IO + L3 slot pool | Low — see `runtime_loading/` sketches |

---

## 9. Constraints and Design Decisions

### 9.1 Locked decisions

| Decision | Choice | Rationale |
|----------|--------|-----------|
| `IoPolicy` / `IoConfig` split | decouple intent from tuning | business layer describes the scenario, IO layer picks the transport |
| Business header `io_policy.h` only | never depend on `transport.h` | hide transport class names, lower coupling |
| Process-wide `FHERuntime` singleton | C++ globals are process-scoped | `CONFIG_MANAGER` / `key_path` / `key_mode` |
| `FheSession` holds `KeyMgr` via `unique_ptr` | break header dependency chain | `session.h` need not pull in `key_mgr.h → spdlog` |
| `PRELOAD` dummy SK/PK | `generate_dummy_keys_for_evaluator` | Evaluator needs SK/PK to construct; PRELOAD must not require them |
| Merged galois-key format default | `galois_merged_format=true` | eliminate per-file `cuFileHandle` registration, maximize GDS throughput |
| Backend dispatch as templates | `VendorTraits` + `FheContext<Backend>` | compile-time, zero virtual overhead, concept-checked |
| L1 API layering split | `FHERuntime` owns primitives (`run`/`run_batch`) + lifecycle; dataset evaluation lives in `DatasetEvaluator` | single-responsibility — the compute layer must not encode dataset/accuracy protocol |
| Deterministic runtime scoping | `FHERuntime` context manager (`__enter__`/`__exit__`); lazy `auto_init=False` default | RAII-equivalent for the heavy native FHE context — release must not depend on GC ordering |

### 9.2 Current constraints

| Constraint | Impact | Mitigation |
|------------|--------|------------|
| `FHERuntime` process singleton | no concurrent runtimes in one process | subprocess-per-config benchmarks |
| GPU pointer deep-copy destructor (`cudaFreeAsync`) | contiguous-buffer sub-ranges unusable; D2D scatter unavoidable | accept ~42 ms D2D for 22 GiB |
| GPU must be idle to benchmark | cuFile Bar1 mapping fails if other GPU procs exist | `nvidia-smi` check beforehand |
| Python key-gen bridge partial | `generate_*_keys` makes dirs+metadata only | use `OfflinePrep.save_keys()` instead |
| `ON_DEMAND` / `prefetch` reserved | on-demand load not implemented | use `PRELOAD` |

---

## 10. Glossary

| Term | Expansion |
|------|-----------|
| FHE | Fully Homomorphic Encryption |
| CKKS | Cheon–Kim–Kim–Song (approximate FHE scheme) |
| SK / PK | Secret / Public Key |
| RLK / GK / BK | Relinearization / Galois / Bootstrap Key |
| GDS / cuFile | GPU Direct Storage / its user-space API |
| HBM | High Bandwidth Memory (GPU) |
| NTT | Number Theoretic Transform (core FHE encode op) |
| DE_MSG_F32 / DE_PLAINTEXT | weight-file formats: raw float32 / pre-encoded plaintext |
| KMS | Key Management (Service) |
| EncMgr / EncPolicy | Encoding Manager / Encoding Policy |
| IoPolicy / IoConfig | IO path policy (intent) / IO tuning (params) |
| `VendorTraits` | per-backend struct of type aliases + HE-op static methods, consumed as a template param |