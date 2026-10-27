# ResNet Scripts

Benchmark, training, profiling, and data utilities for ResNet FHE models.

## Benchmark

Benchmarks live in `benchmark/resnet/` and use pytest with pytest-regressions
for baseline management. Three dimensions are tracked:

### Prerequisites

Before running any ResNet benchmark, make sure the environment is ready:

| Requirement | Details |
|-------------|---------|
| Build | ACE built and installed (`./scripts/dev-build.sh`); `from ace import ...` works. |
| Dataset | CIFAR-10 / CIFAR-100. The loader resolves the data root as `ACE_DATA_DIR` env var → `/opt/dataset` → `<repo>/data` (auto-download). On benchmark hosts datasets live at `/opt/dataset`; internal setup is in [.aci/ONBOARDING.md](../../.aci/ONBOARDING.md#2-pull-and-start-the-internal-dev-image). |
| KMS keys | Large ResNet keys are pre-generated via `examples/09_kms/*.py` or `ace_tool kms`. ResNet-20 galois keys (~22 GiB) live on NVMe (`/mnt/nvme*`) on benchmark hosts; latency/offline-loading tests need them. |
| GPU | The GPU **must be idle** — cuFile Bar1 mapping fails if other GPU processes are running. Check with `nvidia-smi --query-compute-apps=pid,used_memory --format=csv,noheader` and clear stragglers (`pkill -9 -f bench_read`). |
| Runtime libs | If `libFHErt_common.so not found` appears, set `LD_LIBRARY_PATH=$(python -c "import sysconfig; print(sysconfig.get_path('platlib'))")/ace/lib:$LD_LIBRARY_PATH`. |

| Dimension | Test File | What it measures |
|-----------|-----------|------------------|
| Accuracy  | `test_accuracy.py` | FHE vs plaintext predictions at 1/10/100/1000 images |
| Latency   | `test_latency.py`  | Inference timing (total, avg/min/max per image) |
| Memory    | `test_memory.py`   | Peak GPU memory from profiling snapshots |
| Offline Loading | `test_offline_loading.py` | Offline key/weight loading latency (staged vs GDS) |

### Running Benchmarks

```bash
# Run all ResNet benchmarks
pytest benchmark/resnet/ -v

# Run accuracy only
pytest benchmark/resnet/test_accuracy.py -v

# Run latency only
pytest benchmark/resnet/test_latency.py -v

# Run memory only
pytest benchmark/resnet/test_memory.py -v

# Quick test (skip 1000-image slow tests)
pytest benchmark/resnet/ -v -m "not slow"

# Specify GPU
CUDA_VISIBLE_DEVICES=6 pytest benchmark/resnet/ -v
```

### Selecting Backends

All benchmarks are parameterized across three backends via `TARGET_PARAMS`:

| ID | Library | Device |
|----|---------|--------|
| `ant-cpu` | ant | CPU |
| `phantom-cuda` | phantom | CUDA (auto-skip if no GPU) |
| `ace-cuda` | ace | CUDA (auto-skip if no GPU) |

Use `-k` to select:

```bash
# Only CPU backend
pytest benchmark/resnet/test_accuracy.py -k "ant-cpu" -v

# Only GPU backends (phantom + ace)
pytest benchmark/resnet/test_accuracy.py -k "cuda" -v

# Only ace backend
pytest benchmark/resnet/test_accuracy.py -k "ace" -v

# Only phantom backend
pytest benchmark/resnet/test_accuracy.py -k "phantom" -v

# Only ant (CPU) backend
pytest benchmark/resnet/test_accuracy.py -k "ant" -v -m slow

# Specific model + backend
pytest benchmark/resnet/test_accuracy.py -k "resnet20 and ace" -v
```

### Running Accuracy by Model

Tests are parameterized by `(backend, image_count)`. The `-k` filter and
`-m slow` control which combinations run:

```bash
# All models, all backends, quick (1/10/100 images)
pytest benchmark/resnet/test_accuracy.py -v -m "not slow"

# All models, all backends, including 1000-image slow tests
pytest benchmark/resnet/test_accuracy.py -v

# Single model, all backends, quick
pytest benchmark/resnet/test_accuracy.py -k "resnet20" -v -m "not slow"

# Single model on a specific backend
pytest benchmark/resnet/test_accuracy.py -k "resnet20 and ace" -v -m "not slow"

# Single model, single backend, single image count
#   Use [param-id] for exact match — avoids substring ambiguity
#   (e.g. -k "100" matches "1000" too)
pytest benchmark/resnet/test_accuracy.py -k "resnet20 and [ace-cuda-1]" -v

# Include the slow 1000-image tests
pytest benchmark/resnet/test_accuracy.py -k "resnet20 and ace" -v -m slow

# Only the 1000-image slow test
pytest benchmark/resnet/test_accuracy.py -k "resnet20 and [ace-cuda-1000]" -v -m slow
```

### Running Latency by Model

```bash
# All models, quick
pytest benchmark/resnet/test_latency.py -v -m "not slow"

# Single model on ace
pytest benchmark/resnet/test_latency.py -k "resnet20 and ace" -v -m "not slow"

# Specific image count
pytest benchmark/resnet/test_latency.py -k "resnet20 and [phantom-cuda-10]" -v
```

### Running Memory by Model

```bash
# Single model
pytest benchmark/resnet/test_memory.py -k "resnet20" -v

# All models
pytest benchmark/resnet/test_memory.py -v
```

### Running Offline Loading Benchmark

Offline loading benchmark measures key loading latency (online key generation
vs offline key loading from disk) across I/O transport modes (staged, auto, gds).

**Standalone benchmark script** (subprocess-per-scenario, avoids singleton contamination):

```bash
# All I/O modes, 1 image (default)
python3 docs/benchmarks/resnet_offline_benchmark.py \
    --key-dir /mnt/nvme0/kms_demo_keys_resnet20_phantom

# Single I/O mode
python3 docs/benchmarks/resnet_offline_benchmark.py --io staged --key-dir ./keys

# Auto-save keys if missing
python3 docs/benchmarks/resnet_offline_benchmark.py --save-keys

# ResNet-110
python3 docs/benchmarks/resnet_offline_benchmark.py --model resnet110 \
    --key-dir /path/to/keys
```

**pytest integration** (parameterized across backends and I/O modes):

```bash
# All GPU backends, all I/O modes
pytest benchmark/resnet/test_offline_loading.py -v

# Phantom only
pytest benchmark/resnet/test_offline_loading.py -k phantom -v

# Staged I/O only
pytest benchmark/resnet/test_offline_loading.py -k staged -v
```

See [ResNet Offline Loading Results](resnet-offline-loading.md) for benchmark data
and [GDS I/O Transport Design](../design/gds-io-transport.md) for architecture details.

### Baseline Management

Benchmarks use `pytest-regressions` (`data_regression` / `num_regression`) to
store expected results. Baseline files live in directories named after each test
module:

- `benchmark/resnet/test_accuracy/` — YAML files
- `benchmark/resnet/test_latency/` — CSV files
- `benchmark/resnet/test_memory/` — CSV files

#### GPU-Specific Latency Baselines

Latency baselines are GPU-dependent — different GPUs produce different timings.
Baseline filenames include a GPU tag (auto-detected from `torch.cuda.get_device_name`):

```
resnet20_cifar10_ace_cuda_h20_latency.csv     ← NVIDIA H20
resnet20_cifar10_ace_cuda_a100_latency.csv    ← NVIDIA A100
resnet20_cifar10_phantom_cuda_h20_latency.csv ← NVIDIA H20
```

When running on a new GPU, `--regen-all` creates a new set of baselines with
that GPU's tag, without overwriting existing GPU-specific baselines.

```bash
# On H20: matches *_h20_*.csv baselines
CUDA_VISIBLE_DEVICES=0 pytest benchmark/resnet/test_latency.py -k ace --regen-all -v

# On A100: matches *_a100_*.csv baselines
CUDA_VISIBLE_DEVICES=0 pytest benchmark/resnet/test_latency.py -k ace --regen-all -v
```

Memory baselines do **not** include a GPU tag — peak GPU memory is determined
by CKKS parameters and model structure, not by GPU model.

#### `--regen-all` vs `--force-regen`

| Flag | Behavior | Use case |
|------|----------|----------|
| `--regen-all` | Overwrite all baselines unconditionally; tests **PASS** | **Performance baselines** (numbers naturally vary; you want to update them) |
| `--force-regen` | Overwrite only baselines that differ; those tests **FAIL** (forces review) | **Correctness baselines** (structural changes need human confirmation) |

**For FHE benchmarks, use `--regen-all`**: latency and memory numbers fluctuate
across runs due to GPU scheduling and thermal throttling. You want new numbers
written and tests to pass so the run completes cleanly.

```bash
# Generate or refresh all baselines (recommended for benchmarks)
pytest benchmark/resnet/ --regen-all -v

# Generate baselines for a specific model + backend
pytest benchmark/resnet/test_accuracy.py -k "resnet20 and ace" --regen-all -v

# Generate baselines for a specific dimension
pytest benchmark/resnet/test_latency.py --regen-all -v
pytest benchmark/resnet/test_memory.py --regen-all -v
```

Use `--force-regen` for IR structure regression tests where changes require
human review:

```bash
# IR structure tests: fail on diff so you can review changes
pytest tests/regression/resnet/test_resnet_compile.py --force-regen -v
```

### Accuracy Baselines

Each baseline YAML contains:
- `fhe_correct`: number of images where FHE matches ground truth
- `plaintext_correct`: number of images where plaintext matches ground truth
- `fhe_match_count`: number of images where FHE matches plaintext
- `top1_accuracy`: FHE top-1 accuracy ratio
- `num_images`: number of test images
- `fhe_mismatch`: sorted list of image indices where FHE diverges from plaintext

### Latency Baselines

Each baseline YAML contains:
- `num_images`: number of test images
- `total_ms`: total inference time
- `avg_per_image_ms`: average time per image
- `min_image_ms`: fastest single image
- `max_image_ms`: slowest single image

Note: latency values vary across runs. Use `--force-regen` to update after
compiler/runtime changes. Values are rounded to 1 decimal to reduce noise.

### Memory Baselines

Each baseline YAML contains:
- `peak_gpu_used_mb`: peak GPU memory usage
- `snapshots`: list of `{phase, gpu_used_mb, gpu_free_mb}` memory snapshots

## Training (Optional)

Pre-trained weights are included in `ace/model/resnet/weights/`. Only needed
when retraining models or training new variants.

```bash
# Train ResNet-20 on CIFAR-10
python -m ace.model.train_resnet --model 20 --epochs 200 --dataset cifar10

# Train ResNet-110 on CIFAR-10
python -m ace.model.train_resnet --model 110 --epochs 200 --dataset cifar10 --batch-size 128

# Train ResNet-32 on CIFAR-100
python -m ace.model.train_resnet --model 32 --epochs 200 --dataset cifar100 --batch-size 128
```

## ReLU VR Profiling (Optional)

Pre-profiled VR data is included in `ace/model/resnet/profiles/`. Only needed
after retraining (weights change) or when adding new model variants.

```bash
# Regenerate all ResNet profiles (FX Interpreter, 10000 images)
python -m ace.model.relu_profile

# Profile specific model
python -m ace.model.relu_profile --model resnet20 resnet56

# Profile with user-provided data
python -m ace.model.relu_profile --model resnet20 --inputs my_data.pt

# Compare old vs new profiles
python -m ace.model.relu_profile --compare

# Or use ReLUProfiler API directly:
#   from ace.fhe.config import ReLUProfiler
#   profiler = ReLUProfiler(spec)
#   vr_data = profiler.profile(inputs=images, margin=1, save=True)
```

### When to re-profile

- After model retraining (weights change → activation ranges change)
- After normalization changes (mean/std affect activation distributions)
- When adding new model variants
