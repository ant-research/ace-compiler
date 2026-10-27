# Test Coverage Summary

This document summarizes **what is actually covered** by the existing test cases
in `tests/` and `benchmark/`, organized by layer (unit → integration →
regression → benchmark). It is an inventory of the current test surface, not a
spec of what should be tested. Reference: [testing.md](testing.md) for how to
run the tests.

Structure of this document:

- `tests/unit/` — component layer: which modules and what scope
- `tests/integration/` — end-to-end chain: which vertical themes
- `tests/regression/` — regression baselines: op library and model family
- `benchmark/` — performance contracts: ResNet + IO + runtime modes
- Key facts (what is / isn't covered)

---

## 1. tests/unit/ — component layer

### 1.1 frontend (4 frontends: torch / ast / onnx / torch-via-onnx)

| File | Covered scope |
|------|---------------|
| `test_torch.py` | torch trace: prepare → compile → export lifecycle; IR properties (entry name, scopes, graph); **standard PyTorch models**: add_module / relu_module / linear / conv2d+relu / global_avg_pool; graph transforms (add/relu rewrite); placeholder inputs; auto-generated input names |
| `test_ast.py` | AST frontend: prepare → compile → export; memory-mode properties; placeholder inputs; export to valid path; IR properties (entry name, main graph, input/output nodes) |
| `test_onnx.py` | ONNX-file frontend: file-mode properties; example_inputs ignored; prepare/compile; export onnx/air; **edge cases**: nonexistent file, invalid onnx file, export to valid path |
| `test_torch_via_onnx.py` | torch→onnx: file-mode, multi-input function, wrong-input branches (None / empty / wrong count) |

Scope covered: 4 input forms × lifecycle API × 5 representative models × error branches.

### 1.2 IR (`ir/`)

- `test_structure.py` — IR object skeleton: `CompilationUnit` (abstract base constraints), `IRNode`, `BasicBlock`, `FHEGraph`, `FHEProgram`; creation / attributes / add-remove / serialize (dict, export). Uses hand-built fixtures only — **does not exercise the real compiler-produced IR graph**.
- `test_format.py` / `test_export.py` — ONNXFileIR / AIRFileIR format contracts; op-type mapping table; export produces valid onnx / air files.

### 1.3 driver / pipeline (`driver/`)

- `test_registry.py` — frontend registry + library registry: enumeration, duplicate-registration errors, method contracts.
- `test_pipeline.py` — **four compilation pipelines** (torch→air, torch-via-onnx→air/onnx, ast, onnx→air/onnx) run end-to-end on ant-cpu / phantom-cuda / ace-cuda, sampled with **2 ops (LINEAR / ADD)**; invalid frontend / backend errors.

Scope covered: compilation main paths × backends, at a small op sample.

### 1.4 config / runtime (`config/`, `runtime/`)

- `test_option.py` — CompileOptions / ComputeOptions / BaseOption: defaults, field extraction, None exclusion, interop.
- `test_key_mode.py` — the **5 key modes** (online / preload / preload_full / on_demand / on_demand_full): validate / apply / pybind roundtrip; `needs_full_keys` / `requires_key_path` derivation; alias compatibility; `encode_only` removed.
- `test_key_path.py` — key_path set / clear / overwrite; empty-string clears; spaces / special chars / Chinese / relative / trailing slash / very long / unicode / nesting; multi set-clear cycles.

### 1.5 KMS (`kms/`)

| File | Covered scope |
|------|---------------|
| `test_config_reader.py` | conf-file discovery; CKKS param parsing (required/optional fields, defaults, roundtrip); edge branches: missing fields, bad JSON, extreme N / mul_depth / rotation indices |
| `test_generator.py` | encryption-key generation (metadata, auto key_id, optional params); **evaluation-key generation** (rotation indices, hamming weight, bootstrap on/off driven by conf) |
| `test_key_bundle.py` | KeyBundle path layout, metadata serialization, has-checks, summary, missing files |
| `test_storage.py` | bundle save / load / roundtrip; list / get_info; output-dir resolution; corrupted metadata branches |
| `test_offline_encode.py` | OfflinePrep init (library validation); update_config (entry_type / file_name / backup / preserves other fields); encode error guards; save_keys interface disabled |

### 1.6 IO transport API (`io_transport/`)

- `test_io_policy.py` (~40 cases) — IoPolicy constants; **5 IO profiles** (galois_key_load / key_persistence / checkpoint / high_throughput) defaults; backward-compat mapping to auto/gds/staged/mapped; apply; interaction with IoConfig; pybind.
- `test_io_config.py` (~30 cases) — IoConfig validation and defaults; chunk_size / threads / disk_paths; legacy fields; exec_mode; deprecated `DataPath` aliases; serialization to C++; GDS availability probe.
- `test_io_bench_util.py` — policy → expected-transport mapping table (staged→staged, gds→cuFile, auto→probe); BenchResult dataclass; aggregation; report writing.

### 1.7 backend (`backend/`)

- `test_provider.py` — provider properties / options; build command (basic / with flags / with ace_root); unsupported-IR error; openfhe AIR-not-implemented branch.

---

## 2. tests/integration/ — end-to-end chain

Chain exercised: **frontend → compile → key handling → run → export**, through
real op models. Op samples: `LINEAR_OP`, `ADD_OP` (model level), `ADD_FUNC`
(function level).

| File | Covered scope |
|------|---------------|
| `test_decorator.py` | **@compile / @compute / @export** decorator API: model torch / torch-via-onnx; function torch / torch-via-onnx; across the 3 backends |
| `test_fhe_compile.py` | fhe.compile on model class / instance / function; torch-via-onnx; invalid frontend / library errors |
| `test_fhe_compute.py` | **run consistency**: ciphertext result vs plaintext, for model torch, model torch-via-onnx, function AST, function torch-via-onnx |
| `test_fhe_export.py` | export model / function to AIR file |
| `test_inference_modes.py` | key-mode validation integration: key_offline / server_only require key_path; valid / invalid / roundtrip; encode_only removed |
| `test_kms_key_save_load.py` | **full KMS flow**: save → load → run; missing-key-path fallback; eval-only; works under staged IO policy; key_offline results == online results; repeated save/load cycles; repeated run on same context |
| `test_mode_combinations.py` | **key_mode × io_policy matrix**: online; key_offline × (staged/auto); server_only × staged; offline_encode × (key_offline / server_only) |
| `test_offline_encode.py` | offline-encode roundtrip; combination with key_offline / server_only; package rewrites config and preserves the original |
| `test_gpu_batch_parallel.py` | **run_batch_parallel**: intra-batch consistency; batch vs serial agreement; num_sessions property; single image == serial |
| `test_mode_b_session.py` | Mode B: serial **run_session** across sessions agrees |
| `test_multi_user.py` | multi-user independent-process results agree |
| `test_on_demand_keys.py` | **on-demand key loading**: lazy-load results == full preload results |

Scope covered: 5 vertical themes — ① compile/compute/export main path,
② key lifecycle & inference modes, ③ IO policy participation, ④ multi-user /
session reuse, ⑤ GPU batch parallelism.

---

## 3. tests/regression/ — regression baselines

### 3.1 Op library (`sample/`) — data source: 30 op specs (yml goldens)

Op coverage (from the 30 trace yml files under `test_sample_compile/`):

| Category | Ops |
|----------|-----|
| Basic arithmetic | add / sub / mul / div / mult_const / add_const |
| Activations | relu / sigmoid / tanh / sqrt / softmax |
| Linear | linear / linear_bias / linear_relu / mlp / gemm_49x3 / relu_gemm |
| Conv / pool | conv2d / conv2d_relu / depthwise_conv2d / avg_pool2d / avg_pool_2d / avg_pool_2d_with_stride / avg_pool_flatten / global_avg_pool / global_avg_pool_op / max_pool2d / relu_avg_pool |
| Structure | flatten |

Per op: trace (IR-structure golden), model (creation / forward / determinism /
ops), smoke (compile + run on ant-cpu / phantom-cuda / ace-cuda).

### 3.2 ResNet model family (`resnet/`) — 6 model variants

- Models: resnet20 / resnet32 / resnet44 / resnet56 / resnet110 (cifar10)
  + resnet32 cifar100.
- `test_resnet_compile.py` — trace / bn_folded (golden IR structure) →
  to_air (torch, torch-via-onnx, onnx) → export (air / onnx) + BN-folding
  equivalence; 12 yml baselines.
- `test_resnet_model.py` — model creation / forward / expected ops /
  determinism / batch-size consistency.
- `test_resnet_smoke.py` — full-model smoke on ant / phantom / ace.

### 3.3 Multi-user concurrency (`test_mu_*`)

- `test_mu_serial.py` — serial session switching (preload_full / on_demand_full)
- `test_mu_parallel.py` — concurrent run_session (key_mode parametrized)
- `test_mu_multiprocess.py` — independent-process results agree

---

## 4. benchmark/ — performance contracts

### 4.1 `benchmark/resnet/` — ResNet performance regression

| File | Covered scope |
|------|---------------|
| `test_accuracy.py` | resnet20/32/44/56/110 (cifar10) + resnet32 cifar100; image counts 1/10/100/1000 (phantom), 1/10 (ace); accuracy data_regression |
| `test_latency.py` | same 6 models × phantom/ace latency baselines; A100 + H20 csv goldens |
| `test_memory.py` | same 6 models × phantom/ace peak-HBM baselines |

### 4.2 `benchmark/io/` — IO & key-load performance contracts

Pytest assertion tests (CI-runnable):

- `test_io_datapath_bench.py` — **108-combination matrix**:
  io_policy (staged/mapped/gds) × threads (1/2/4) × disks (1/2/4) ×
  file_mode (single/multi) × data_size (10/20 GiB); read/write verification +
  policy→transport mapping assertions.
- `test_io_policy_bench.py` — 11 representative subsets
  (auto/gds/staged/mapped × threads/disks/data_size), avoiding the full-108 cost.

Report-style scripts (manual inspection, not CI):

- `bench_read.py` — read-bandwidth across 3 datapaths × D1/D4
- `bench_key_load.py` — ResNet key-load IO latency
- `gds_key_read_bench.py` — GDS 4T4D galois-key throughput
- `run_io_baseline.py` — full 108-combination baseline runner
- `data_prep/` — key data prep: merge_galois_keys / repack_keys_v3 / split_merged_to_v3
- `validation/` — IO smoke / quick validation

### 4.3 `benchmark/runtime/` — inference-mode runtime overhead

| File | Covered scope |
|------|---------------|
| `test_key_mode_latency.py` | same model × key mode (online / key_offline / server_only) init-latency baselines |
| `test_io_policy_latency.py` | key_offline init × io_policy (staged / mapped / auto) |
| `test_offline_loading.py` | 4 groups: offline-key loading, offline-encode loading, server-only, offline_encode × key_mode; each × io_mode (staged / compatible_gds) |

---

## 5. Key facts (what is / isn't covered today)

1. **unit** walks only 2 ops (ADD / LINEAR) through the pipeline — the full
   30-op coverage lives in the **regression** layer.
2. **integration** uses a 2-model + 1-function sample but has the widest
   vertical-theme coverage (key lifecycle, multi-user, batch parallelism,
   mode combinations).
3. **regression** is the widest layer: 30 ops + 6 ResNet model variants, all
   backed by golden data.
4. GPU paths (phantom/ace) execute only when `gpu_available()`; multi-disk
   combinations depend on how many NVMe disks the host exposes.
5. The benchmark layer distinguishes **assertion benchmarks** (CI-runnable)
   from **report benchmarks** (require manual judgment).