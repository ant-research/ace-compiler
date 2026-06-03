# CLI Tools Design

## Overview

`ace_tool` is an auxiliary CLI for ACE FHE Compiler & Runtime, providing convenience subcommands for model preparation workflows — ReLU profiling, sample data export, and model training. The primary interface for ACE is the Python API (`ace.fhe`); `ace_tool` extends it with common operational tasks that are easier to run from the shell.

## Invocation

After `pip install ace`, the `ace_tool` command is available on `PATH`:

```bash
ace_tool <subcommand> [options]
```

Alternatively, run as a Python module:

```bash
python -m ace.cli <subcommand> [options]
```

## Architecture

```
ace_tool (pyproject.toml: ace.cli:main)
  │
  ▼
fhe_dsl/python/cli/
  ├── __init__.py        # Re-exports main()
  ├── __main__.py        # python -m ace.cli entry
  ├── main.py            # argparse dispatcher — defines subcommands
  ├── relu_profile.py    # relu-profile  →  ace.model.relu_profile
  ├── dump_sample.py     # dump-sample   →  ace.model.dump_sample
  └── train_resnet.py    # train-resnet  →  ace.model.train_resnet
```

Each subcommand module contains a single `run(args)` function that parses the
`argparse.Namespace` and delegates to the corresponding implementation in
`ace.model.*`.  The CLI layer is intentionally thin — it handles argument
parsing and I/O formatting, while business logic lives in the models package.

## Subcommands

### relu-profile

Profile per-call-site ReLU value ranges for FHE polynomial approximation.
Uses FX Interpreter to track pre-ReLU activation ranges across a dataset,
producing VR values that match AIR IR node names exactly.

There are two profiling scenarios depending on the model source:

| Scenario | Interface | Description |
|----------|-----------|-------------|
| **Built-in models** | CLI (`ace_tool`) or Python API | ResNet models shipped with ACE, with pretrained weights and built-in datasets |
| **User models** | Python API only | Custom PyTorch models defined by the user, with user-provided weights and data |

#### Built-in models (CLI)

Profile the ResNet models bundled with ACE. The CLI handles model
construction, weight loading, and dataset preparation automatically.

```bash
# Profile all built-in models
ace_tool relu-profile

# Profile specific models
ace_tool relu-profile --model resnet20 resnet110

# Use custom input data (overrides built-in dataset)
ace_tool relu-profile --model resnet20 --inputs my_data.pt

# Adjust sample count and safety margin
ace_tool relu-profile --model resnet20 --num-samples 1000 --margin 1

# Dry run (show what would be done)
ace_tool relu-profile --dry-run

# Compare new profiles with existing ones
ace_tool relu-profile --compare

# Write profiles to a custom directory
ace_tool relu-profile --model resnet20 --output /path/to/profiles
```

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `--model` | str (repeatable) | all | Built-in model names to profile (substring match) |
| `--inputs` | path | built-in dataset | Path to `.pt` file with input tensor `(N, C, H, W)` |
| `--num-samples` | int | 10000 | Number of dataset samples |
| `--margin` | int | 1 | Safety margin for VR calculation |
| `--dry-run` | flag | off | Show results without writing files |
| `--compare` | flag | off | Compare new profiles with existing ones |
| `--output`, `-o` | path | auto | Output directory for profile JSON files |

**Output directory resolution** (when `--output` is not specified):

| Environment | Default output path |
|-------------|-------------------|
| Git checkout | `fhe_dsl/python/model/resnet/profiles/` (source tree) |
| Pip install | `./profiles/` (current working directory) |

Writing to the install directory (site-packages) is intentionally avoided —
it may lack write permissions and is managed by the package manager.

**Delegates to:** `ace.model.relu_profile.profile_spec()`, `write_profile()`, `compare_profiles()`

#### Built-in models (Python API)

The same built-in models can also be profiled programmatically:

```python
from ace.model.relu_profile import profile_spec, write_profile, _get_builtin_specs

# Get built-in specs
specs = _get_builtin_specs()

# Profile a specific built-in model
spec = [s for s in specs if "resnet20" in s.name][0]
result = profile_spec(spec, num_samples=10000, margin=1)
write_profile(spec, result, output_dir="my_profiles/")
```

#### User / custom models (Python API)

For user-defined models, use the `ReLUProfiler` class with a `ModelSpec`
that describes the custom model. The CLI does not support custom models
directly — there is no `--model-class` or `--weights` flag.

```python
import torch
import torch.nn as nn
from ace.fhe.config import ReLUProfiler
from ace.fhe.config.spec import ModelSpec

# 1. Define or import your model
class MyModel(nn.Module):
    def __init__(self):
        super().__init__()
        self.conv = nn.Conv2d(3, 16, 3, padding=1)
        self.relu = nn.ReLU()
        self.fc = nn.Linear(16 * 32 * 32, 10)

    def forward(self, x):
        x = self.relu(self.conv(x))
        x = x.flatten(1)
        return self.fc(x)

# 2. Create a ModelSpec
spec = ModelSpec(
    name="my_model",
    model_class=MyModel,
    example_inputs=(torch.randn(1, 3, 32, 32),),
)

# 3. Profile with your own data
my_data = torch.randn(1000, 3, 32, 32)  # (N, C, H, W)
profiler = ReLUProfiler(spec)
vr_data = profiler.profile(inputs=my_data, margin=1, save=True)

# vr_data is a dict: {air_node_name: vr_float_value}
print(vr_data)
```

Key differences from built-in model profiling:

| Aspect | Built-in models | User models |
|--------|----------------|-------------|
| Model source | Predefined `ModelSpec` in `ace.model` | User-created `ModelSpec` |
| Weights | Auto-loaded from `model/resnet/weights/` | User loads weights before profiling |
| Dataset | Built-in CIFAR-10/100 | User provides `inputs` tensor |
| BN folding | Not applied in CLI path | Applied automatically in `ReLUProfiler.profile()` |
| Output path | Auto-detected (source tree / CWD) | Auto-saved to model package `profiles/` or `{name}_vr.json` |
| CLI support | Full | Not available — Python API only |

The profiled VR data can also be supplied at compile time via
`CompileOptions`:

```python
from ace import fhe

# Option 1: Pass VR data dict directly
prog = fhe.compile(
    my_model,
    relu_vr_data=vr_data,
    ...
)

# Option 2: Pass VR profile JSON file
prog = fhe.compile(
    my_model,
    relu_vr_file="my_model_vr.json",
    ...
)

# Option 3: Profile at compile time (uses example_inputs)
prog = fhe.compile(
    my_model,
    profile_relu=True,
    ...
)
```

### dump-sample

Export sample images from CIFAR-10/100 datasets to `.npz` files for testing
and benchmarking.

```bash
# Dump 1 CIFAR-10 image (default)
ace_tool dump-sample

# Dump 5 images
ace_tool dump-sample --num 5

# Dump from CIFAR-100
ace_tool dump-sample --dataset cifar100

# Specify output path
ace_tool dump-sample -o /path/to/sample.npz

# Dump starting from the 100th image
ace_tool dump-sample --offset 99
```

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `--dataset` | `cifar10` \| `cifar100` | `cifar10` | Dataset to sample from |
| `--num`, `-n` | int | 1 | Number of images to dump |
| `--offset` | int | 0 | Start index in the dataset |
| `--output`, `-o` | path | `<dataset>_sample.npz` | Output npz path |

**Delegates to:** `ace.model.dump_sample.dump_sample()`

**Output format:** npz file with keys `image` `(N, C, H, W)` uint8 and `label` `(N,)` int64.

### train-resnet

Train ResNet models (20/32/44/56/110) on CIFAR-10/100 with SGD and learning
rate scheduling.

```bash
# Train ResNet-20 on CIFAR-10
ace_tool train-resnet --model 20 --epochs 200 --dataset cifar10

# Train ResNet-110 on CIFAR-100
ace_tool train-resnet --model 110 --epochs 200 --dataset cifar100

# Quick test run
ace_tool train-resnet --model 32 --epochs 10 --batch-size 256
```

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `--model` | 20 \| 32 \| 44 \| 56 \| 110 | required | ResNet depth |
| `--dataset` | `cifar10` \| `cifar100` | `cifar10` | Dataset |
| `--epochs` | int | 200 | Training epochs |
| `--batch-size` | int | 128 | Batch size |
| `--lr` | float | 0.1 | Initial learning rate |
| `--momentum` | float | 0.9 | SGD momentum |
| `--weight-decay` | float | 5e-4 | Weight decay |
| `--lr-schedule` | `standard` \| `cosine` \| `step` | `standard` | LR schedule |
| `--device` | str | `cuda` | Device (`cuda` or `cpu`) |
| `--num-workers` | int | 4 | Data loading workers |
| `--save-dir` | path | `weights` | Directory to save checkpoints |
| `--resume` | path | — | Resume from checkpoint |

**Delegates to:** `ace.model.train_resnet` (`get_model`, `train_epoch`, `test_epoch`, etc.)

## Adding a New Subcommand

1. Create `fhe_dsl/python/cli/<subcommand>.py` with a `run(args)` function.
2. Register the subparser in `fhe_dsl/python/cli/main.py`.
3. Add the dispatch case in `main()`'s if-elif chain.
4. Update `__init__.py` docstring.
5. Add documentation in this file.

## Entry Point Registration

The console script is registered in `pyproject.toml`:

```toml
[project.scripts]
ace_tool = "ace.cli:main"
```

This creates the `ace_tool` command on `PATH` after `pip install`.
