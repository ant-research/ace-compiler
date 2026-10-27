# ResNet Performance: ace-cuda

Performance benchmark results for ResNet-CIFAR models using the ACE (acelib) GPU backend.

> **Note**: This document contains latency and memory data for the `ace-cuda` backend.
> For phantom-cuda results, see [resnet-phantom.md](resnet-phantom.md).

---

## Inference Latency (ace-cuda)

### Test Configuration

- Backend: ace-cuda
- Device: CUDA
- Test images: 10 images per model

### Latency Results — NVIDIA A100 (10 images)

- GPU: NVIDIA A100

| Model | Dataset | Total (s) | Min (s) | Max (s) | Avg/Image (s) |
|-------|---------|-----------|---------|---------|---------------|
| ResNet-20 | CIFAR-10 | 46.91 | 4.67 | 4.74 | 4.69 |
| ResNet-32 | CIFAR-10 | 74.87 | 7.45 | 7.54 | 7.49 |
| ResNet-32 | CIFAR-100 | 74.87 | 7.45 | 7.53 | 7.49 |
| ResNet-44 | CIFAR-10 | 103.07 | 10.25 | 10.37 | 10.31 |
| ResNet-56 | CIFAR-10 | 131.44 | 13.01 | 13.23 | 13.14 |
| ResNet-110 | CIFAR-10 | 256.22 | 25.40 | 25.71 | 25.62 |

### Latency Analysis (A100)

| Model | Avg Time (s/img) | Delta from ResNet-20 |
|-------|------------------|---------------------|
| ResNet-20 | 4.69 | baseline |
| ResNet-32 | 7.49 | +2.80 (+60%) |
| ResNet-44 | 10.31 | +5.62 (+120%) |
| ResNet-56 | 13.14 | +8.45 (+180%) |
| ResNet-110 | 25.62 | +20.93 (+447%) |

### Latency Results — NVIDIA H20 (10 images)

- GPU: NVIDIA H20 (95 GB)

| Model | Dataset | Total (s) | Min (s) | Max (s) | Avg/Image (s) |
|-------|---------|-----------|---------|---------|---------------|
| ResNet-20 | CIFAR-10 | 44.98 | 4.46 | 4.54 | 4.50 |
| ResNet-32 | CIFAR-10 | 81.53 | 7.07 | 12.71 | 8.15 |
| ResNet-32 | CIFAR-100 | 71.15 | 7.09 | 7.16 | 7.11 |
| ResNet-44 | CIFAR-10 | 97.73 | 9.71 | 9.82 | 9.77 |
| ResNet-56 | CIFAR-10 | 125.69 | 12.49 | 12.73 | 12.57 |
| ResNet-110 | CIFAR-10 | 241.43 | 24.05 | 24.26 | 24.14 |

### Latency Analysis (H20)

| Model | Avg Time (s/img) | Delta from ResNet-20 |
|-------|------------------|---------------------|
| ResNet-20 | 4.50 | baseline |
| ResNet-32 | 8.15 | +3.65 (+81%) |
| ResNet-44 | 9.77 | +5.27 (+117%) |
| ResNet-56 | 12.57 | +8.07 (+179%) |
| ResNet-110 | 24.14 | +19.64 (+436%) |

### Latency Comparison: A100 vs H20

| Model | Avg A100 (s/img) | Avg H20 (s/img) | A100 / H20 |
|-------|-----------------|-----------------|------------|
| ResNet-20 | 4.69 | 4.50 | 1.04× |
| ResNet-32 | 7.49 | 8.15 | 0.92× |
| ResNet-32 CIFAR-100 | 7.49 | 7.11 | 1.05× |
| ResNet-44 | 10.31 | 9.77 | 1.06× |
| ResNet-56 | 13.14 | 12.57 | 1.05× |
| ResNet-110 | 25.62 | 24.14 | 1.06× |

**Observations:**
- A100 and H20 latency is within ~10% — FHE inference is compute-bound by
  polynomial arithmetic (NTT, multiplication), not GPU raw FLOPS.
- Per-image latency is dominated by CKKS parameters (N, multiplicative depth)
  rather than GPU clock speed.

### Latency Comparison: Phantom vs ACE (H20)

| Model | Dataset | Avg Phantom (s/img) | Avg ACE (s/img) | Speedup |
|-------|---------|--------------------|----------------|---------|
| ResNet-20 | CIFAR-10 | 8.65 | 4.50 | 1.92× |
| ResNet-32 | CIFAR-10 | 13.85 | 8.15 | 1.70× |
| ResNet-32 | CIFAR-100 | 13.90 | 7.11 | 1.95× |
| ResNet-44 | CIFAR-10 | 19.91 | 9.77 | 2.04× |
| ResNet-56 | CIFAR-10 | 25.79 | 12.57 | 2.05× |
| ResNet-110 | CIFAR-10 | 49.53 | 24.14 | 2.05× |

### Latency Comparison: Phantom vs ACE (A100)

| Model | Dataset | Avg Phantom (s/img) | Avg ACE (s/img) | Speedup |
|-------|---------|--------------------|----------------|---------|
| ResNet-20 | CIFAR-10 | 8.36 | 4.69 | 1.78× |
| ResNet-32 | CIFAR-10 | 13.71 | 7.49 | 1.83× |
| ResNet-32 | CIFAR-100 | 13.66 | 7.49 | 1.82× |
| ResNet-44 | CIFAR-10 | 19.03 | 10.31 | 1.85× |
| ResNet-56 | CIFAR-10 | 24.65 | 13.14 | 1.88× |
| ResNet-110 | CIFAR-10 | 48.62 | 25.62 | 1.90× |

**Observations:**
- ACE is consistently ~1.7–2.1× faster than Phantom across all model depths.
- The speedup is stable on both H20 and A100, driven by ACE's optimized GPU
  kernel implementations (4-step NTT, Montgomery reduction) and memory pool strategy.

### Latency Comparison: FHE-MP-CNN vs Phantom vs ACE (H20)

| Model | Dataset | FHE-MP-CNN (s/img) | Phantom (s/img) | ACE (s/img) | Phantom Speedup | ACE Speedup |
|-------|---------|--------------------|-----------------|-------------|-----------------|-------------|
| ResNet-20 | CIFAR-10 | 13.97 | 8.65 | 4.50 | 1.62× | 3.10× |
| ResNet-32 | CIFAR-10 | 21.36 | 13.85 | 8.15 | 1.54× | 2.62× |
| ResNet-32 | CIFAR-100 | 21.06 | 13.90 | 7.11 | 1.52× | 2.96× |
| ResNet-44 | CIFAR-10 | 29.37 | 19.91 | 9.77 | 1.48× | 3.01× |
| ResNet-56 | CIFAR-10 | 37.16 | 25.79 | 12.57 | 1.44× | 2.96× |
| ResNet-110 | CIFAR-10 | 74.38 | 49.53 | 24.14 | 1.50× | 3.08× |

**Observations:**
- Phantom is consistently ~1.4–1.6× faster than FHE-MP-CNN on H20.
- ACE is consistently ~2.6–3.1× faster than FHE-MP-CNN on H20.

### Latency Comparison: FHE-MP-CNN vs Phantom vs ACE (A100)

| Model | Dataset | FHE-MP-CNN (s/img) | Phantom (s/img) | ACE (s/img) | Phantom Speedup | ACE Speedup |
|-------|---------|--------------------|-----------------|-------------|-----------------|-------------|
| ResNet-20 | CIFAR-10 | 17.30 | 8.36 | 4.69 | 2.07× | 3.69× |
| ResNet-32 | CIFAR-10 | 26.34 | 13.71 | 7.49 | 1.92× | 3.52× |
| ResNet-32 | CIFAR-100 | 26.17 | 13.66 | 7.49 | 1.92× | 3.49× |
| ResNet-44 | CIFAR-10 | 34.89 | 19.03 | 10.31 | 1.83× | 3.38× |
| ResNet-56 | CIFAR-10 | 44.79 | 24.65 | 13.14 | 1.82× | 3.41× |
| ResNet-110 | CIFAR-10 | 87.91 | 48.62 | 25.62 | 1.81× | 3.43× |

**Observations:**
- Phantom is consistently ~1.8–2.1× faster than FHE-MP-CNN on A100.
- ACE is consistently ~3.4–3.7× faster than FHE-MP-CNN on A100.

---

## GPU Memory Usage (ace-cuda)

### Test Configuration

- Backend: ace-cuda
- GPU: NVIDIA H20 (95 GB)

> Note: Data from `ace_tool bench memory` (peak memory during inference).

### Peak Memory by Model

| Model | Dataset | GPU Memory Used |
|-------|---------|-----------------|
| ResNet-20 | CIFAR-10 | 54,969 MiB |
| ResNet-32 | CIFAR-10 | 54,969 MiB |
| ResNet-32 | CIFAR-100 | 54,969 MiB |
| ResNet-44 | CIFAR-10 | 54,969 MiB |
| ResNet-56 | CIFAR-10 | 54,969 MiB |
| ResNet-110 | CIFAR-10 | 54,969 MiB |

> Note: All models show the same peak GPU memory usage, indicating ACE pre-allocates
> its full memory pool at context initialization regardless of model complexity.

### Memory Comparison: ACE vs Phantom (H20)

| Model | ACE Peak (MiB) | Phantom Peak (MiB) | ACE / Phantom |
|-------|---------------|-------------------|---------------|
| ResNet-20 | 54,969 | 21,478 | 2.56× |
| ResNet-32 | 54,969 | 21,508 | 2.56× |
| ResNet-44 | 54,969 | 21,700 | 2.53× |
| ResNet-56 | 54,969 | 21,732 | 2.53× |
| ResNet-110 | 54,969 | 21,924 | 2.51× |

**Observations:**
- ACE uses ~2.5× more GPU memory than Phantom consistently across all model depths.
  This is primarily due to ACE's memory pool pre-allocation strategy, which reserves
  the full pool at context initialization.
- Unlike Phantom (whose memory is dominated by Galois keys shared across models),
  ACE's peak memory is a fixed constant independent of model depth.
- All tested models fit within H20's 95 GB GPU memory.

---

## Test Details

### Run Commands

```bash
# Single model
ace_tool bench latency --model resnet20 --backend ace --num-images 10
ace_tool bench memory --model resnet20 --backend ace

# All models
ace_tool bench latency --model all --backend ace --num-images 10
ace_tool bench memory --model all --backend ace

# Update latency baselines
pytest benchmark/resnet/test_latency.py -k ace --regen-all -v
```

### Baseline Regeneration

Latency baselines are GPU-specific — each GPU gets its own set of CSV files
(e.g. `*_h20_latency.csv`, `*_a100_latency.csv`). Running `--regen-all` on
a new GPU creates new baselines without overwriting existing ones.

```bash
# Regenerate ace-cuda latency baselines (auto-detects GPU tag)
CUDA_VISIBLE_DEVICES=0 pytest benchmark/resnet/test_latency.py -k ace --regen-all -v

# Regenerate phantom-cuda baselines
CUDA_VISIBLE_DEVICES=0 pytest benchmark/resnet/test_latency.py -k phantom --regen-all -v
```

See [resnet-scripts.md](resnet-scripts.md) for details on `--regen-all`
vs `--force-regen` and GPU-specific baseline file naming.
