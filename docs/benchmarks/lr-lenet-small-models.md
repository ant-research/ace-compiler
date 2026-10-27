# Small-Model FHE Performance: LR & LeNet

Benchmark results for the two small FHE models added alongside ResNet:
**Logistic Regression (LR)** — a single GEMM with no activation — and
**LeNet** — a compact CNN (stride-2 conv → relu ×2 → flatten → linear).

Covered backends: **phantom-cuda** (GPU) and **ant-cpu** (CPU), matching
the smoke-test scope for these models (ace-CUDA is excluded: known
single-GEMM/conv inference error, see test_sample_smoke_ace.py).

> **Note:** This report covers latency/memory from the `benchmark/lr/` and
> `benchmark/lenet/` pytest baselines. Accuracy vs CIFAR labels is
> intentionally not benchmarked: both models are trained on deterministic
> synthetic CIFAR-shaped data (see model specs), so real-CIFAR accuracy is
> not a meaningful contract.
>
> Baseline files (pytest-regressions):
> - `benchmark/lr/test_lr_latency/lr_1d_{library}_{device}_h20_latency.csv`
> - `benchmark/lenet/test_lenet_latency/lenet_cifar10_{library}_{device}_h20_latency.csv`
> - `benchmark/lenet/test_lenet_memory/lenet_cifar10_{library}_{device}_memory.csv`

---

## Test Configuration

- Backend: phantom-cuda (NVIDIA H20, 95 GB) / ant-cpu
- Device: CUDA (H20) / CPU
- CKKS parameters: N=65536, q0=60, sf=56 (mirrors the ResNet-family options)
- LR: single `nn.Linear(16 → 10)`; input `[1, 16]`, no ReLU, no VR profile
- LeNet: `conv(3→8,s2)→relu → conv(8→16,s2)→relu → flatten → fc(1024→32)→relu → fc(32→10)`;
  input `[1, 3, 32, 32]`, 3 ReLUs, built-in `profile_relu=True`

---

## Inference Latency (10 images)

### phantom-cuda

| Model | Total (ms) | Min (ms) | Max (ms) | Avg/Image (ms) | Avg/Image (s) |
|-------|-----------|----------|----------|----------------|---------------|
| LR 1D (16→10) | 90.9 | 7.1 | 26.9 | 9.1 | 0.009 |
| LeNet CIFAR-10 | 17,839.8 | 1,723.8 | 2,053.2 | 1,784.0 | 1.78 |

### ant-cpu

| Model | Total (ms) | Min (ms) | Max (ms) | Avg/Image (ms) | Avg/Image (s) |
|-------|-----------|----------|----------|----------------|---------------|
| LR 1D (16→10) | 4.2 | 1.9 | 4.1 | 0.4 | 0.0004 |
| LeNet CIFAR-10 | 182,861.4 | 171,524.1 | 182,861.1 | 18,286.1 | 18.29 |

**Observations:**
- LR is a single ciphertext GEMM — sub-ms on CPU (~0.4 ms/image), ~9
  ms/image on GPU. The GPU cost is dominated by one encrypt + multiply +
  relinearize.
- LeNet: **GPU 1784 ms/image vs CPU 18,286 ms/image (CPU ~10× slower).**
  Each conv layer performs many ciphertext multiplications plus
  rotation-based convolution, and the 3 ReLU sites each need a polynomial
  approximation (bootstrap-backed at this CKKS depth).
- On phantom, LeNet is **~196× slower** than LR; on ant-cpu, ~46,000×
  (mostly key-gen amortization + no GPU datapath).
- Both are deterministic crypto compute; the min/max spread reflects
  per-frame scheduling / PCIe contention rather than algorithm variance.

---

## GPU Memory Usage

| Backend | Model | Peak Memory (MiB) |
|---------|-------|-------------------|
| phantom-cuda | LeNet CIFAR-10 | 24,497 (~26% of H20 95 GB) |
| ant-cpu | LeNet CIFAR-10 | 343 (CPU runtime, no GPU) |

**Observations:**
- LeNet at 24.5 GiB on phantom is dominated by the **Galois key set**
  (rotation indices for convolutions) + relin/secret/public keys — the
  same CKKS parameter family shared with ResNet, so key memory leans on
  the same large component (~19+ GiB).
- Run-time intermediates for LeNet are modest (~1-2 GiB) because the
  network is shallow; the big cost is the key set.
- LR memory is not captured on either backend (single-GEMM profile returns
  no memory snapshots), and is trivially small.

---

## How to Reproduce

```bash
# phantom-cuda latency baselines (regenerate)
pytest benchmark/lr/test_lr_latency.py -k phantom --force-regen
pytest benchmark/lenet/test_lenet_latency.py -k phantom --force-regen

# ant-cpu latency baselines (regenerate)
pytest benchmark/lr/test_lr_latency.py -k ant --force-regen
pytest benchmark/lenet/test_lenet_latency.py -k ant --force-regen

# Memory baselines (regenerate in isolation — phantom profile snapshot is
# only taken at runtime init, so do NOT run alongside latency)
pytest benchmark/lenet/test_lenet_memory.py -k phantom --force-regen
pytest benchmark/lenet/test_lenet_memory.py -k ant --force-regen

# Run (verify against baselines)
pytest benchmark/lr/test_lr_latency.py
pytest benchmark/lenet/test_lenet_latency.py
pytest benchmark/lenet/test_lenet_memory.py
```

> **Performance tests must be run in isolation — never parallel, never
> back-to-back in the same pytest process.** The phantom runtime is a
> process-wide singleton and takes its memory snapshot pair (init_before /
> init_after) only when the runtime context is created. A prior latency
> run_dataset in the same process leaves no fresh snapshot, so the memory
> benchmark reads 0 MB unless run on its own. The benchmark helper in
> `benchmark/lenet/test_lenet_memory.py` additionally builds a fresh
> CompiledProgram per profiling pass to guarantee a new context init.