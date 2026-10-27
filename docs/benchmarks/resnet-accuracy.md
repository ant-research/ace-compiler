# ResNet FHE Inference Accuracy

FHE inference accuracy results for ResNet-CIFAR models.

> **Note**: Accuracy results are independent of the backend library (phantom, antlib, acelib).
> They depend only on CKKS parameters and ReLU VR profiling data.

See [ResNet Training](resnet-training.md) for training configuration and results.
See [Benchmark Scripts](resnet-scripts.md) for script usage.

---

## Plaintext Baseline

| Model | Dataset | Best Test Accuracy | Weight File |
|-------|---------|--------------------|-------------|
| ResNet-20 | CIFAR-10 | 92.40% | resnet20_cifar10.pt |
| ResNet-32 | CIFAR-10 | 93.07% | resnet32_cifar10.pt |
| ResNet-44 | CIFAR-10 | 93.59% | resnet44_cifar10.pt |
| ResNet-56 | CIFAR-10 | 93.38% | resnet56_cifar10.pt |
| ResNet-110 | CIFAR-10 | 94.11% | resnet110_cifar10.pt |
| ResNet-32 | CIFAR-100 | 71.23% | resnet32_cifar100.pt |

---

## Test Configuration

- CKKS parameters: q0=60, sf=56, N=65536
- Profiling: FX Interpreter-based per-call-site VR data from `ace/model/resnet/profiles/`
- Test images: 1/10/100/1000 CIFAR-10/100 test images

---

## CIFAR-10 Results Summary (1000 images)

| Model | Plaintext Acc | FHE Acc | Accuracy Loss | Consistency | Mismatches |
|-------|---------------|---------|---------------|-------------|------------|
| ResNet-20 | 92.4% (924/1000) | 92.5% (925/1000) | -0.1% | 99.7% (997/1000) | 3 |
| ResNet-32 | 92.0% (920/1000) | 92.3% (923/1000) | -0.3% | 99.5% (995/1000) | 4 |
| ResNet-44 | 92.7% (927/1000) | 92.5% (925/1000) | 0.2% | 98.8% (988/1000) | 9 |
| ResNet-56 | 94.8% (948/1000) | 94.7% (947/1000) | 0.1% | 99.1% (991/1000) | 6 |
| ResNet-110 | 94.4% (944/1000) | 94.3% (943/1000) | 0.1% | 97.8% (978/1000) | 16 |

---

## CIFAR-100 Results Summary (1000 images)

| Model | Plaintext Acc | FHE Acc | Accuracy Loss | Consistency | Mismatches |
|-------|---------------|---------|---------------|-------------|------------|
| ResNet-32 | 70.4% (704/1000) | 68.9% (689/1000) | 1.5% | 94.1% (941/1000) | 59 |

---

## Multi-Scale Results

### ResNet-20 / CIFAR-10

| Images | Plaintext Acc | FHE Acc | Consistency | Mismatches |
|--------|---------------|---------|-------------|------------|
| 1 | 100.0% (1/1) | 100.0% (1/1) | 100.0% (1/1) | 0 |
| 10 | 100.0% (10/10) | 100.0% (10/10) | 100.0% (10/10) | 0 |
| 100 | 95.0% (95/100) | 95.0% (95/100) | 100.0% (100/100) | 0 |
| 1000 | 92.4% (924/1000) | 92.5% (925/1000) | 99.7% (997/1000) | 3 |

### ResNet-32 / CIFAR-10

| Images | Plaintext Acc | FHE Acc | Consistency | Mismatches |
|--------|---------------|---------|-------------|------------|
| 1 | 100.0% (1/1) | 100.0% (1/1) | 100.0% (1/1) | 0 |
| 10 | 100.0% (10/10) | 100.0% (10/10) | 100.0% (10/10) | 0 |
| 100 | 95.0% (95/100) | 94.0% (94/100) | 99.0% (99/100) | 1 |
| 1000 | 92.0% (920/1000) | 92.3% (923/1000) | 99.5% (995/1000) | 4 |

### ResNet-32 / CIFAR-100

| Images | Plaintext Acc | FHE Acc | Consistency | Mismatches |
|--------|---------------|---------|-------------|------------|
| 1 | 100.0% (1/1) | 100.0% (1/1) | 100.0% (1/1) | 0 |
| 10 | 40.0% (4/10) | 40.0% (4/10) | 100.0% (10/10) | 0 |
| 100 | 69.0% (69/100) | 69.0% (69/100) | 96.0% (96/100) | 3 |
| 1000 | 70.4% (704/1000) | 68.9% (689/1000) | 94.1% (941/1000) | 59 |

### ResNet-44 / CIFAR-10

| Images | Plaintext Acc | FHE Acc | Consistency | Mismatches |
|--------|---------------|---------|-------------|------------|
| 1 | 100.0% (1/1) | 100.0% (1/1) | 100.0% (1/1) | 0 |
| 10 | 100.0% (10/10) | 100.0% (10/10) | 100.0% (10/10) | 0 |
| 100 | 95.0% (95/100) | 93.0% (93/100) | 98.0% (98/100) | 2 |
| 1000 | 92.7% (927/1000) | 92.5% (925/1000) | 98.8% (988/1000) | 9 |

### ResNet-56 / CIFAR-10

| Images | Plaintext Acc | FHE Acc | Consistency | Mismatches |
|--------|---------------|---------|-------------|------------|
| 1 | 100.0% (1/1) | 100.0% (1/1) | 100.0% (1/1) | 0 |
| 10 | 100.0% (10/10) | 100.0% (10/10) | 100.0% (10/10) | 0 |
| 100 | 95.0% (95/100) | 94.0% (94/100) | 99.0% (99/100) | 1 |
| 1000 | 94.8% (948/1000) | 94.7% (947/1000) | 99.1% (991/1000) | 6 |

### ResNet-110 / CIFAR-10

| Images | Plaintext Acc | FHE Acc | Consistency | Mismatches |
|--------|---------------|---------|-------------|------------|
| 1 | 100.0% (1/1) | 100.0% (1/1) | 100.0% (1/1) | 0 |
| 10 | 100.0% (10/10) | 100.0% (10/10) | 100.0% (10/10) | 0 |
| 100 | 96.0% (96/100) | 96.0% (96/100) | 99.0% (99/100) | 1 |
| 1000 | 94.4% (944/1000) | 94.3% (943/1000) | 97.8% (978/1000) | 16 |

---

## Analysis

### Consistency vs Model Depth

On CIFAR-10 (1000 images), consistency degrades with model depth:

| Model | Depth | ReLU Nodes | Consistency | Mismatches |
|-------|-------|------------|-------------|------------|
| ResNet-20 | 20 | 19 | 99.7% | 3 |
| ResNet-32 | 32 | 31 | 99.5% | 4 |
| ResNet-44 | 44 | 43 | 98.8% | 9 |
| ResNet-56 | 56 | 55 | 99.1% | 6 |
| ResNet-110 | 110 | 109 | 97.8% | 16 |

ResNet-20 shows the best consistency (99.7%), while ResNet-110 drops to 97.8%
due to deeper networks amplifying CKKS noise through more bootstrap operations.

### CIFAR-10 vs CIFAR-100

ResNet-32 on CIFAR-100 has lower consistency (94.1%) than the same
model on CIFAR-10 (99.7%). The 100-class classification task amplifies FHE
approximation errors — small perturbations near decision boundaries are more
likely to flip the top-1 prediction.

### FHE Accuracy Gap

The gap between FHE and plaintext accuracy is typically 0-0.4 percentage points
on CIFAR-10. Notable cases:
- ResNet-44/CIFAR-10: 0.2pp gap (92.7% vs 92.5%)
- ResNet-32/CIFAR-100: 1.5pp gap (70.4% vs 68.9%)

After ReLU VR profiling optimization (2026-06-04), the FHE accuracy gap has been
significantly reduced. Key improvements:
- ResNet-110: gap reduced from 1.2pp to 0.1pp (93.2%→94.3%)
- ResNet-32: gap reduced from 0.3pp to -0.3pp (91.7%→92.3%, FHE exceeds plaintext)
- ResNet-20: consistency improved from 99.1% to 99.7%, mismatches from 9 to 3

### ReLU VR Profiling

All models use FX Interpreter-based per-call-site VR profiling (e.g., 19 nodes
for ResNet-20, 109 nodes for ResNet-110), matching AIR IR node names exactly.
Profile data is stored in `ace/model/resnet/profiles/`.

To regenerate profiles:

```bash
# Regenerate all ResNet profiles
python -m ace.model.relu_profile

# Regenerate specific model
python -m ace.model.relu_profile --model resnet20

# Compare old vs new profiles
python -m ace.model.relu_profile --compare

# Profile with user-provided data
python -m ace.model.relu_profile --model resnet20 --inputs my_data.pt
```
