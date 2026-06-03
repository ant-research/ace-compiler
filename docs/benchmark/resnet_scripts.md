# ResNet Scripts

Benchmark, training, profiling, and data utilities for ResNet FHE models.

## Benchmark

Benchmarks live in `benchmark/resnet/` and use pytest with pytest-regressions
for baseline management. Three dimensions are tracked:

| Dimension | Test File | What it measures |
|-----------|-----------|------------------|
| Accuracy  | `test_accuracy.py` | FHE vs plaintext predictions at 1/10/100/1000 images |
| Latency   | `test_latency.py`  | Inference timing (total, avg/min/max per image) |
| Memory    | `test_memory.py`   | Peak GPU memory from profiling snapshots |

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

### Running Accuracy by Model

```bash
# ResNet-20 (1/10/100 images, phantom-cuda)
pytest benchmark/resnet/test_accuracy.py -k "resnet20 and phantom" -v -m "not slow"

# ResNet-32 CIFAR-10
pytest benchmark/resnet/test_accuracy.py -k "resnet32_cifar10 and phantom" -v -m "not slow"

# ResNet-44
pytest benchmark/resnet/test_accuracy.py -k "resnet44 and phantom" -v -m "not slow"

# ResNet-56
pytest benchmark/resnet/test_accuracy.py -k "resnet56 and phantom" -v -m "not slow"

# ResNet-110
pytest benchmark/resnet/test_accuracy.py -k "resnet110 and phantom" -v -m "not slow"

# ResNet-32 CIFAR-100
pytest benchmark/resnet/test_accuracy.py -k "resnet32_cifar100 and phantom" -v -m "not slow"
```

#### Selecting Specific Image Counts

Tests are parameterized by image count (1/10/100/1000). Use `[param-id]` in
`-k` to select a specific count — brackets match the full parameter ID, avoiding
substring ambiguity (e.g. `-k "100"` also matches `phantom-1000`):

```bash
# Only 1 image
pytest benchmark/resnet/test_accuracy.py -k "resnet20 and [phantom-1]" -v

# Only 10 images
pytest benchmark/resnet/test_accuracy.py -k "resnet20 and [phantom-10]" -v

# Only 100 images
pytest benchmark/resnet/test_accuracy.py -k "resnet20 and [phantom-100]" -v

# Only 1000 images (slow)
pytest benchmark/resnet/test_accuracy.py -k "resnet20 and [phantom-1000]" -v -m slow
```

Include 1000-image tests (slow):

```bash
# ResNet-20 full (1/10/100/1000 images)
pytest benchmark/resnet/test_accuracy.py -k "resnet20 and phantom" -v -m slow
```

### Running Latency by Model

```bash
# ResNet-20 latency (1/10/100 images)
pytest benchmark/resnet/test_latency.py -k "resnet20 and phantom" -v -m "not slow"

# ResNet-110 latency with 1000 images
pytest benchmark/resnet/test_latency.py -k "resnet110 and [phantom-1000]" -v -m slow

# Specific count (e.g., only 10 images)
pytest benchmark/resnet/test_latency.py -k "resnet20 and [phantom-10]" -v
```

### Running Memory by Model

```bash
# ResNet-20 memory
pytest benchmark/resnet/test_memory.py -k "resnet20 and phantom" -v

# All models memory
pytest benchmark/resnet/test_memory.py -v -m slow
```

### Baseline Management

Benchmarks use `pytest-regressions` (`data_regression`) to store expected
results as YAML files. On first run or after changes, regenerate baselines:

```bash
# First run: generate all baselines
pytest benchmark/resnet/ --force-regen -v

# Generate baselines for a specific model
pytest benchmark/resnet/test_accuracy.py -k "resnet20 and phantom" --force-regen -v

# Generate baselines for a specific dimension
pytest benchmark/resnet/test_latency.py --force-regen -v
pytest benchmark/resnet/test_memory.py --force-regen -v

# Baseline files are auto-generated in:
#   benchmark/resnet/test_accuracy/
#   benchmark/resnet/test_latency/
#   benchmark/resnet/test_memory/
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