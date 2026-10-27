# ACE Examples

This directory contains example scripts demonstrating the ACE FHE compiler framework.

## Directory Structure

```
examples/
|-- 01_quickstart/                  # Getting started
|   |-- 01_compute.py               # @fhe.compute -- compile + run in one step
|   `-- 02_compile.py               # @fhe.compile -- compile first, then run
|
|-- 02_frontend/                    # Frontend examples
|   |-- 01_torch_function.py        # torch frontend + function
|   |-- 02_torch_model.py           # torch frontend + nn.Module + encrypt_inputs
|   |-- 03_torch_via_onnx.py        # torch-via-onnx frontend
|   |-- 04_ast_function.py          # ast frontend + Python function
|   `-- 05_onnx_file.py             # onnx frontend + ONNX file
|
|-- 03_provider/                    # Provider examples (with per-provider params)
|   |-- 01_antlib_cpu.py            # antlib CPU + CKKS parameters
|   |-- 02_phantom_cuda.py          # phantom CUDA
|   `-- 03_acelib_cuda.py           # acelib CUDA + provider-specific parameters
|
|-- 04_ir_and_export/               # IR formats and export
|   |-- 01_export_air.py            # @fhe.export -> AIR (.B file)
|   |-- 02_export_onnx.py           # @fhe.export -> ONNX
|   |-- 03_onnx_file_compile.py     # ONNX file -> compile
|   `-- 04_memory_compile.py        # In-memory IR compilation
|
|-- 05_models/                      # Real-world models
|   |-- 01_linear_regression.py
|   |-- 02_logistic_regression.py
|   `-- 03_mlp_classifier.py
|
|-- 06_advanced/                    # Advanced usage
|   |-- 01_encryption_params.py     # Custom CKKS encryption parameters
|   |-- 02_partial_encryption.py    # Partial input encryption (encrypt_inputs)
|   |-- 03_batch_inference.py       # Batch and dataset inference
|   |-- 04_runtime_api.py           # FHERuntime low-level API
|   |-- 05_export_only.py           # Export AIR/ONNX without compilation
|   |-- 06_compile_options.py       # Compiler options (ckks/vec/sihe/p2c/env override)
|   |-- 07_cache_control.py         # Compile cache (3-level key, force rebuild, custom dir)
|   `-- 08_offline_encode.py        # Offline weight pre-encoding (OfflinePrep)
|
|-- 07_profiling/                   # FHE profiling
|   |-- 01_quick_profile.py         # Quick profiling on a simple model
|   `-- 02_resnet_profile.py        # ResNet profiling (resnet20/32/44/56/110, --model all)
|
|-- 08_relu_vr_profiling/           # ReLU value range profiling
|   `-- resnet20_relu_vr.py
|
|-- 09_kms/                         # Key Management (save/load pre-generated keys)
|   |-- 01_save_keys.py             # Step 1: Generate & save FHE keys (add model)
|   |-- 02_load_keys.py             # Step 2: Load saved keys & run inference (add model)
|   |-- 03_resnet_save_keys.py      # Step 1: Generate & save FHE keys (ResNet, --model)
|   |-- 04_resnet_load_keys.py      # Step 2: Load saved keys & run inference (ResNet)
|   `-- 05_inference_modes.py       # Selective key loading (online / key_offline / server_only)
|
`-- samples/                        # Shared sample models, functions & input generators
    |-- models.py                   # NOTE: not yet imported by the example scripts above;
    |-- functions.py                # each script self-defines its model. Kept as a shared
    `-- input_generators.py         # utility for future example reuse.
```
```

## Quick Start

```bash
# Run a quickstart example
python examples/01_quickstart/01_compute.py

# Run with a specific provider
python examples/03_provider/01_antlib_cpu.py
```

## Provider Libraries

| Library   | Device | Description              |
|-----------|--------|--------------------------|
| `antlib`  | CPU    | Default CPU backend      |
| `phantom` | CUDA   | GPU-accelerated FHE      |
| `acelib`  | CUDA   | Alternative GPU library  |
| `seal`    | CPU    | Microsoft SEAL (limited) |
| `openfhe` | CPU    | OpenFHE library          |
