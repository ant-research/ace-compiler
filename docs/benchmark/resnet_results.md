# ResNet FHE Inference Results

FHE inference results for ResNet-CIFAR models.

See [ResNet Training](resnet_training.md) for training configuration and results.
See [Benchmark Design](../design/benchmark.md) for the benchmark framework design
and how to run benchmarks. See [resnet_scripts.md](resnet_scripts.md) for script
usage.

## Plaintext Baseline

| Model | Dataset | Best Test Accuracy | Weight File |
|-------|---------|--------------------|-------------|
| ResNet-20 | CIFAR-10 | 92.40% | resnet20_cifar10.pt |
| ResNet-32 | CIFAR-10 | 93.07% | resnet32_cifar10.pt |
| ResNet-44 | CIFAR-10 | 93.59% | resnet44_cifar10.pt |
| ResNet-56 | CIFAR-10 | 93.38% | resnet56_cifar10.pt |
| ResNet-110 | CIFAR-10 | 94.11% | resnet110_cifar10.pt |
| ResNet-32 | CIFAR-100 | 71.23% | resnet32_cifar100.pt |

## FHE Inference Results (phantom-cuda)

### Test Configuration

- Backend: phantom-cuda
- Profiling: FX Interpreter-based per-call-site VR data from `ace/model/resnet/profiles/`
- CKKS parameters: q0=60, sf=56, N=65536
- Test images: 1/10/100/1000 CIFAR-10/100 test images

### CIFAR-10 Results Summary (1000 images)

| Model | Plaintext Acc | FHE Acc | Consistency | Mismatches |
|-------|---------------|---------|-------------|------------|
| ResNet-20 | 92.4% (924/1000) | 92.4% (924/1000) | 99.1% (991/1000) | 9 |
| ResNet-32 | 92.0% (920/1000) | 91.7% (917/1000) | 99.7% (997/1000) | 3 |
| ResNet-44 | 92.7% (927/1000) | 92.4% (924/1000) | 99.0% (990/1000) | 10 |
| ResNet-56 | 94.8% (948/1000) | 94.6% (946/1000) | 98.9% (989/1000) | 11 |
| ResNet-110 | 94.4% (944/1000) | 93.2% (932/1000) | 96.2% (962/1000) | 38 |

### CIFAR-100 Results Summary (1000 images)

| Model | Plaintext Acc | FHE Acc | Consistency | Mismatches |
|-------|---------------|---------|-------------|------------|
| ResNet-32 | 70.4% (704/1000) | 68.8% (688/1000) | 92.7% (927/1000) | 73 |

### Multi-Scale Results

#### ResNet-20 / CIFAR-10

| Images | Plaintext Acc | FHE Acc | Consistency | Mismatches |
|--------|---------------|---------|-------------|------------|
| 1 | 100.0% (1/1) | 100.0% (1/1) | 100.0% (1/1) | 0 |
| 10 | 100.0% (10/10) | 100.0% (10/10) | 100.0% (10/10) | 0 |
| 100 | 95.0% (95/100) | 95.0% (95/100) | 98.0% (98/100) | 2 |
| 1000 | 92.4% (924/1000) | 92.4% (924/1000) | 99.1% (991/1000) | 9 |

#### ResNet-32 / CIFAR-10

| Images | Plaintext Acc | FHE Acc | Consistency | Mismatches |
|--------|---------------|---------|-------------|------------|
| 1 | 100.0% (1/1) | 100.0% (1/1) | 100.0% (1/1) | 0 |
| 10 | 100.0% (10/10) | 100.0% (10/10) | 100.0% (10/10) | 0 |
| 100 | 95.0% (95/100) | 94.0% (94/100) | 99.0% (99/100) | 1 |
| 1000 | 92.0% (920/1000) | 91.7% (917/1000) | 99.7% (997/1000) | 3 |

#### ResNet-32 / CIFAR-100

| Images | Plaintext Acc | FHE Acc | Consistency | Mismatches |
|--------|---------------|---------|-------------|------------|
| 1 | 0.0% (0/1) | 0.0% (0/1) | 100.0% (1/1) | 0 |
| 10 | 40.0% (4/10) | 40.0% (4/10) | 100.0% (10/10) | 0 |
| 100 | 69.0% (69/100) | 66.0% (66/100) | 94.0% (94/100) | 6 |
| 1000 | 70.4% (704/1000) | 68.8% (688/1000) | 92.7% (927/1000) | 73 |

#### ResNet-44 / CIFAR-10

| Images | Plaintext Acc | FHE Acc | Consistency | Mismatches |
|--------|---------------|---------|-------------|------------|
| 1 | 100.0% (1/1) | 100.0% (1/1) | 100.0% (1/1) | 0 |
| 10 | 100.0% (10/10) | 100.0% (10/10) | 100.0% (10/10) | 0 |
| 100 | 95.0% (95/100) | 94.0% (94/100) | 99.0% (99/100) | 1 |
| 1000 | 92.7% (927/1000) | 92.4% (924/1000) | 99.0% (990/1000) | 10 |

#### ResNet-56 / CIFAR-10

| Images | Plaintext Acc | FHE Acc | Consistency | Mismatches |
|--------|---------------|---------|-------------|------------|
| 1 | 100.0% (1/1) | 100.0% (1/1) | 100.0% (1/1) | 0 |
| 10 | 100.0% (10/10) | 100.0% (10/10) | 100.0% (10/10) | 0 |
| 100 | 95.0% (95/100) | 94.0% (94/100) | 99.0% (99/100) | 1 |
| 1000 | 94.8% (948/1000) | 94.6% (946/1000) | 98.9% (989/1000) | 11 |

#### ResNet-110 / CIFAR-10

| Images | Plaintext Acc | FHE Acc | Consistency | Mismatches |
|--------|---------------|---------|-------------|------------|
| 1 | 100.0% (1/1) | 100.0% (1/1) | 100.0% (1/1) | 0 |
| 10 | 100.0% (10/10) | 100.0% (10/10) | 100.0% (10/10) | 0 |
| 100 | 96.0% (96/100) | 94.0% (94/100) | 97.0% (97/100) | 3 |
| 1000 | 94.4% (944/1000) | 93.2% (932/1000) | 96.2% (962/1000) | 38 |

## Analysis

### Consistency vs Model Depth

On CIFAR-10 (1000 images), consistency degrades with model depth:

| Model | Depth | ReLU Nodes | Consistency | Mismatches |
|-------|-------|------------|-------------|------------|
| ResNet-20 | 20 | 19 | 99.1% | 9 |
| ResNet-32 | 32 | 31 | 99.7% | 3 |
| ResNet-44 | 44 | 43 | 99.0% | 10 |
| ResNet-56 | 56 | 55 | 98.9% | 11 |
| ResNet-110 | 110 | 109 | 96.2% | 38 |

ResNet-32 shows the best consistency (99.7%), while ResNet-110 drops to 96.2%
due to deeper networks amplifying CKKS noise through more bootstrap operations.

### CIFAR-10 vs CIFAR-100

ResNet-32 on CIFAR-100 has significantly lower consistency (92.7%) than the same
model on CIFAR-10 (99.7%). The 100-class classification task amplifies FHE
approximation errors — small perturbations near decision boundaries are more
likely to flip the top-1 prediction.

### FHE Accuracy Gap

The gap between FHE and plaintext accuracy is typically 0-1.2 percentage points
on CIFAR-10. Notable exceptions:
- ResNet-110/CIFAR-10: 1.2pp gap (94.4% vs 93.2%)
- ResNet-32/CIFAR-100: 1.6pp gap (70.4% vs 68.8%)

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