# CIFAR-100 Workflow

This document describes how to run the HPAO inference pipeline with a CIFAR-100
input image. CIFAR-10 and CIFAR-100 share the same image dimensions
(3 × 32 × 32) and model architectures.

## Step-by-step

### 1. Obtain the CIFAR-100 dataset

The script below downloads it automatically. To download manually:

```bash
wget http://www.cs.toronto.edu/~kriz/cifar-100-binary.tar.gz
tar -xzf cifar-100-binary.tar.gz
# This creates cifar-100-binary/ containing test.bin and train.bin
```

### 2. Apply CIFAR-100 input data to test drivers

```bash
# If you already have the dataset extracted (test.bin):
/app/scripts/apply_cifar100.sh --data-dir /path/to/cifar-100-binary

# Or let the script download it automatically:
/app/scripts/apply_cifar100.sh

# Preview what will be changed:
/app/scripts/apply_cifar100.sh --dry-run
```

This script:

- Extracts an image from the CIFAR-100 test set
- Normalizes it using the same parameters as `cifar_reader.h`
- Replaces the `input1[]` array in each model test driver with the
  normalized CIFAR-100 values
- Saves the originals as `*.c.cifar10_bak`

The following test drivers are updated:

| File | Model |
|---|---|
| `lenet.c`, `lenet_x2.c` | LeNet |
| `resnet20_cifar10_pre.c`, `resnet20_cifar10_x2.c` | ResNet-20 |
| `resnet32_cifar10_pre.c`, `resnet32_cifar10_x2.c` | ResNet-32 |
| `resnet44_cifar10_pre.c`, `resnet44_cifar10_x2.c` | ResNet-44 |
| `vgg11_cifar10.c`, `vgg11_cifar10_x2.c` | VGG-11 |
| `vgg13_cifar10.c`, `vgg13_cifar10_x2.c` | VGG-13 |
| `vgg16_cifar10.c`, `vgg16_cifar10_x2.c` | VGG-16 |

### 3. Run the inference workflow

Microbenchmarks use workload-derived operator shapes (convolution, gemm,
average-pooling) and are not dataset-dependent. To save time, comment out
the micro line in `hpao.sh`:

```bash
#/usr/bin/time -f "%e %M" /app/scripts/run_micro.sh --model-first 2>&1 | tee /app/micro.log
```

If the compiler was already built in a previous run, skip the build step
as well. Edit `run_full.sh` to comment out the build line:

```bash
# ./build_cmplr.sh Release || err ${LINENO} "build_cmplr.sh failed"
```

Then run the pipeline. From the `/app` directory:

```bash
/app/scripts/run_full.sh > hpao_cifar100.log 2>&1 &
```

### 4. Revert to original CIFAR-10 inputs (if desired)

```bash
/app/scripts/apply_cifar100.sh --restore
```