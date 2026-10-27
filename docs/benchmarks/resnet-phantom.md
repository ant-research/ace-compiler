# ResNet Performance: phantom-cuda

Performance benchmark results for ResNet-CIFAR models using phantom-cuda backend.

> **Note**: This document contains latency and memory data for the `phantom-cuda` backend.
> For accuracy results (independent of backend), see [resnet-accuracy.md](resnet-accuracy.md).

---

## Inference Latency (phantom-cuda)

### Test Configuration

- Backend: phantom-cuda
- Device: CUDA
- Test images: 10 images per model

### Latency Results — NVIDIA A100 (10 images)

- GPU: NVIDIA A100

| Model | Dataset | Total (s) | Min (s) | Max (s) | Avg/Image (s) |
|-------|---------|-----------|---------|---------|---------------|
| ResNet-20 | CIFAR-10 | 83.59 | 8.34 | 8.42 | 8.36 |
| ResNet-32 | CIFAR-10 | 137.08 | 13.63 | 13.75 | 13.71 |
| ResNet-32 | CIFAR-100 | 136.60 | 13.63 | 13.75 | 13.66 |
| ResNet-44 | CIFAR-10 | 190.31 | 18.96 | 19.12 | 19.03 |
| ResNet-56 | CIFAR-10 | 246.46 | 24.51 | 24.75 | 24.65 |
| ResNet-110 | CIFAR-10 | 486.16 | 48.36 | 48.77 | 48.62 |

### Latency Results — NVIDIA H20 (10 images)

- GPU: NVIDIA H20 (95 GB)

| Model | Dataset | Total (s) | Min (s) | Max (s) | Avg/Image (s) |
|-------|---------|-----------|---------|---------|---------------|
| ResNet-20 | CIFAR-10 | 86.46 | 8.37 | 9.10 | 8.65 |
| ResNet-32 | CIFAR-10 | 138.49 | 13.78 | 13.95 | 13.85 |
| ResNet-32 | CIFAR-100 | 139.01 | 13.74 | 14.14 | 13.90 |
| ResNet-44 | CIFAR-10 | 199.07 | 18.83 | 24.18 | 19.91 |
| ResNet-56 | CIFAR-10 | 257.86 | 24.89 | 31.01 | 25.79 |
| ResNet-110 | CIFAR-10 | 495.31 | 48.69 | 53.29 | 49.53 |

### Latency Analysis (H20)

| Model | Avg Time (s/img) | Delta from ResNet-20 |
|-------|------------------|---------------------|
| ResNet-20 | 8.65 | baseline |
| ResNet-32 | 13.85 | +5.20 (+60%) |
| ResNet-44 | 19.91 | +11.26 (+130%) |
| ResNet-56 | 25.79 | +17.14 (+198%) |
| ResNet-110 | 49.53 | +40.88 (+473%) |

**Observations:**
- Latency scales approximately linearly with model depth
- ResNet-110 is ~5.7× slower than ResNet-20

---

## GPU Memory Usage (phantom-cuda)

### Test Configuration

- Backend: phantom-cuda
- GPU: NVIDIA H20 (95 GB)
- CKKS parameters: N=65536, q0=60, sf=56

> Note: Data from `ace_tool bench latency` (peak memory during inference).

### Peak Memory by Model

| Model | Dataset | GPU Memory Used |
|-------|---------|-----------------|
| ResNet-20 | CIFAR-10 | 21,478 MiB |
| ResNet-32 | CIFAR-10 | 21,508 MiB |
| ResNet-44 | CIFAR-10 | 21,700 MiB |
| ResNet-56 | CIFAR-10 | 21,732 MiB |
| ResNet-110 | CIFAR-10 | 21,924 MiB |

> Note: ResNet-32 CIFAR-100 uses the same memory as ResNet-32 CIFAR-10.

### Memory Breakdown

GPU memory usage is decomposed into offline (pre-generated, loadable via KMS) and
runtime components. Offline data (encryption keys) can be saved to disk and reused
across runs, while runtime data is created fresh each time.

#### 1. Offline Data -- Key Composition

Keys are generated once and can be persisted via KMS. All ResNet-CIFAR models share
the same key sizes due to identical CKKS parameters (N=65536, L=36, q0=60, sf=56)
and the same rotation index set (100 indices, deduplicating to 67 Galois keys with
bootstrap).

| Model | Dataset | Galois Keys | Secret Key | Public Key | Relin Key | Galois Key | **Total** |
|-------|---------|-------------|------------|------------|-----------|------------|-----------|
| ResNet-20 | CIFAR-10 | 67 | 54 MiB | 36 MiB | 288 MiB | 19,296 MiB | **19,674 MiB (~19.2 GB)** |
| ResNet-32 | CIFAR-10 | 67 | 54 MiB | 36 MiB | 288 MiB | 19,296 MiB | **19,674 MiB (~19.2 GB)** |
| ResNet-44 | CIFAR-10 | 67 | 54 MiB | 36 MiB | 288 MiB | 19,296 MiB | **19,674 MiB (~19.2 GB)** |
| ResNet-56 | CIFAR-10 | 67 | 54 MiB | 36 MiB | 288 MiB | 19,296 MiB | **19,674 MiB (~19.2 GB)** |
| ResNet-110 | CIFAR-10 | 67 | 54 MiB | 36 MiB | 288 MiB | 19,296 MiB | **19,674 MiB (~19.2 GB)** |

> Note: Key sizes are measured after `Phantom_save_keys()`. The disk format includes a
> per-file header (`PhantomKeyHeader`, 64 bytes) and CRC32 checksum.

#### 2. Runtime Memory per Model

| Model | Keys (offline) | Context Overhead | CUDA Baseline | Inference Intermediates | **Peak Total** |
|-------|---------------|-----------------|---------------|------------------------|---------------|
| ResNet-20 | 19,674 MiB | 252 MiB | 343 MiB | 1,209 MiB | **21,478 MiB** |
| ResNet-32 | 19,674 MiB | 252 MiB | 343 MiB | 1,239 MiB | **21,508 MiB** |
| ResNet-44 | 19,674 MiB | 252 MiB | 343 MiB | 1,431 MiB | **21,700 MiB** |
| ResNet-56 | 19,674 MiB | 252 MiB | 343 MiB | 1,463 MiB | **21,732 MiB** |
| ResNet-110 | 19,674 MiB | 252 MiB | 343 MiB | 1,655 MiB | **21,924 MiB** |

- **Keys (offline)**: Encryption keys (Galois 19,296 MiB + Relin 288 MiB + Secret
  54 MiB + Public 36 MiB = 19,674 MiB) loaded from KMS or generated at runtime.
  Identical across all models (see table 1 above).
- **Context Overhead**: PhantomContext, CKKS parameters, NTT tables, bootstrapper
  polynomials. Identical across models (same CKKS params).
- **CUDA Baseline**: GPU driver and CUDA runtime overhead before FHE initialization.
- **Inference Intermediates**: Ciphertexts allocated during FHE execution.
  Grows with model depth -- deeper networks require more intermediate ciphertexts
  in the computation graph. This is the only component that varies by model.

Keys usage at runtime (all four keys are currently loaded into GPU memory):

| Key | Runtime Usage | Required |
|-----|--------------|----------|
| Secret Key | Decrypt output (`decryptor.decrypt`) | Yes |
| Public Key | Encrypt input (`encryptor.encrypt`) | Yes |
| Relin Key | Relinearization after multiplication | Yes |
| Galois Key | Rotations (convolution) and bootstrapping | Yes |

> Note: Encryption (input) and decryption (output) are not yet separated in the
> current architecture, so both Secret Key and Public Key must be loaded at runtime.
> Once they are decoupled, the offline-only decryption scenario would only need
> the Secret Key, and the online-only encryption scenario would only need the
> Public Key.

### Memory Analysis

| Model | Peak (MiB) | Keys | Context + CUDA | Inference | Delta from ResNet-20 |
|-------|-----------|------|----------------|-----------|---------------------|
| ResNet-20 | 21,478 | 91.6% | 2.8% | 5.6% | baseline |
| ResNet-32 | 21,508 | 91.5% | 2.8% | 5.8% | +30 MiB |
| ResNet-44 | 21,700 | 90.7% | 2.7% | 6.6% | +222 MiB |
| ResNet-56 | 21,732 | 90.5% | 2.7% | 6.7% | +254 MiB |
| ResNet-110 | 21,924 | 89.7% | 2.7% | 7.5% | +446 MiB |

**Observations:**
- Encryption keys dominate GPU memory usage (~90%), with Galois keys alone accounting for
  ~98% of all key memory
- Memory usage increases with model depth, but the increase comes entirely from inference
  intermediates (ciphertexts), not from keys or context
- ResNet-20 to ResNet-110 shows only ~2% total memory increase because the key component
  (which does not vary by model depth) is so large
- KMS can offload ~19.2 GB of keys to disk, leaving only ~1.8 GB of runtime overhead
  (context + CUDA + intermediates) that must be resident during inference
- All models fit comfortably within H20's 95 GB GPU memory

---

## FHE Key Sizes (phantom-cuda)

### Test Configuration

- Backend: phantom-cuda
- Device: CUDA (NVIDIA H20)
- CKKS parameters: N=65536, q0=60, sf=56 (shared across all ResNet-CIFAR models)

> Note: Key sizes are measured after `Phantom_save_keys()`. The disk format
> includes a per-file header (`PhantomKeyHeader`, 64 bytes) and CRC32 checksum.
> Total on disk = header + CRC + raw key data.

### CKKS Parameters per Model

| Model | Dataset | N | L | Mul Depth | Galois Keys | **Total Key Size** |
|-------|---------|---|---|-----------|-------------|-------------------|
| ResNet-20 | CIFAR-10 | 65536 | 36 | 31 | 67 | **19,674 MB (~19.2 GB)** |
| ResNet-32 | CIFAR-10 | 65536 | 36 | 31 | 67 | **19,674 MB (~19.2 GB)** |
| ResNet-44 | CIFAR-10 | 65536 | 36 | 31 | 67 | **19,674 MB (~19.2 GB)** |
| ResNet-56 | CIFAR-10 | 65536 | 36 | 31 | 67 | **19,674 MB (~19.2 GB)** |
| ResNet-110 | CIFAR-10 | 65536 | 36 | 31 | 67 | **19,674 MB (~19.2 GB)** |

All ResNet-CIFAR models share the same key sizes due to identical CKKS parameters
and the same rotation index set (67 Galois keys after deduplication). For per-key-type
breakdown (Galois/Relin/Secret/Public), see the
[Offline Data table](#1-offline-data----key-composition) in Memory Breakdown.

**Observations:**
- Key size is primarily determined by N (poly modulus degree) and the number of
  Galois keys (rotation indices), not model depth directly
- Galois keys dominate key size (>98%), driven by the 67 rotation indices required
  by the convolution/pooling pattern and bootstrap steps
- Key generation time is significant for large models -- KMS enables pay-once,
  reuse-many workflows

### KMS Key Save/Load Timing (ResNet-20)

| Operation | Time |
|-----------|------|
| Key generation (during inference) | ~36s |
| Key save to disk | ~86s |
| Key load from disk (cold start) | ~115s (includes context init + bootstrap) |
| Inference with generated keys | ~36s |
| Inference with loaded keys | ~115s (dominated by context + bootstrap init) |

> Note: Key *loading* time is dominated by PhantomContext initialization and
> bootstrapper setup, not the key deserialization itself. Galois key deserialization
> (19 GB) accounts for a significant portion. Future optimizations could reduce
> this by streaming keys directly to GPU rather than through host memory.

---

## Test Details

### Run Commands

```bash
# Single model
ace_tool bench latency --model resnet20 --num-images 10

# All models
ace_tool bench latency --model all --num-images 10

# Other examples
ace_tool bench latency --model resnet32 --num-images 10
ace_tool bench latency --model resnet56 --num-images 5
```
