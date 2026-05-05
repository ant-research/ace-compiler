# Overall Design

## Overview

ANT-ACE is a unified Fully Homomorphic Encryption (FHE) framework that compiles high-level Python code (PyTorch models, ONNX) into optimized encrypted execution. It supports multiple librarys (CPU/GPU) and encryption schemes (CKKS, TFHE).

## Framework Overview

```
+--------------------------------------------------------------------------------------------------+
|                                  User Code (Python)                                              |
|  +----------------+  +----------------+  +----------------+  +--------------------------------+  |
|  | @compile()     |  | @compute()     |  | @export()      |  | Direct Driver API              |  |
|  | def func(..)   |  | def func(..)   |  | def func(..)   |  | compiler.compile(model,..)     |  |
|  +-------+--------+  +-------+--------+  +-------+--------+  +---------------+----------------+  |
+----------+------------------+------------------+-------------------+------------------------------+
           |                  |                  |                   |
           v                  v                  v                   v
+--------------------------------------------------------------------------------------------------+
|                                Decorators Layer                                                  |
|          - @compile: FHE compilation decorator (returns compiled program)                        |
|          - @compute: Compilation + immediate execution (transparent FHE)                         |
|          - @export: Frontend IR export (returns AIR IR file)                                     |
|          - Validates Frontend/Backend/Library, resolves encrypted inputs                         |
+--------------------------------------------------------------------------------------------------+
                                        |
                                        v
+--------------------------------------------------------------------------------------------------+
|                                   Driver Layer                                                   |
|                            Driver (driver.py)                                                    |
|            - Orchestrates Frontend -> Backend -> Runtime pipeline                                |
|            - Uses registry pattern for Frontend/Library selection                                |
+--------------------------------------------------------------------------------------------------+
                                        |
            +---------------------------+---------------------------+
            v                           v                           v
    +-----------------------+ +-----------------------+ +-----------------------+
    |     Frontend Layer    | |     Backend Layer     | |    FHELibrary Layer   |
    |                       | |  (5-Level Lowering)   | |                       |
    |  5 Frontends:         | |                       | |  5 Library:           |
    |  - onnx               | |  TENSOR -> VECTOR     | |  - antlib (CPU)       |
    |  - torch              | |     |                 | |  - phantom (GPU)      |
    |  - torch-via-onnx     | |  SIHE -> CKKS -> POLY | |  - hyperfhe (GPU)     |
    |  - ast(TBD)           | |     |                 | |  - seal (CPU)         |
    |  - ast-via-onnx(TBD)  | |  C/C++ -> library     | |  - openfhe (CPU)      |
    +-----------------------+ +-----------------------+ +-----------------------+
```

## Compilation Pipeline

```
+--------------------------------------------------------------------------------+
|                              Frontend Layer                                    |
+--------------------------------------------------------------------------------+
|            onnx                   torch                   ast                  |
|           (file)               (FX trace)                (AST)                 |
|             |                      |                       |                   |
|             v                      v                       v                   |
|          ONNXIR               Model Graph             FHEProgram               |
|             |                      |                       |                   |
|             v                      v                       v                   |
+--------------------------------------------------------------------------------+
                                     |
                                     v
+--------------------------------------------------------------------------------+
|                         Backend Input Formats                                  |
+--------------------------------------------------------------------------------+
|  AIR IR (.B file)    |      ONNXIR (.onnx file)   |  FHEProgram (in-memory)    |
+--------------------------------------------------------------------------------+
                                     |
                                     v
+--------------------------------------------------------------------------------+
|                              Backend Layer                                     |
+--------------------------------------------------------------------------------+
|                                                                                |
|        TENSOR -> VECTOR -> SIHE -> CKKS -> POLY  ->  C/C++(...)                |
|                                                   |                            |
|                                          library + FHEProgram                  |
|                                                                                |
|                                  {antlib,phantom,hyperfhe,seal,openfhe,...}    |
|                                                                                |
+--------------------------------------------------------------------------------+
```

## Input Types and Conversion Strategies

The framework supports **three input types** with **two conversion strategies**:

| Input Type | Strategy A: Direct AIR | Strategy B: Via ONNX |
|------------|---------------------|------------------------|
| **ONNX File (.onnx)** | N/A (already ONNX) | `onnx` |
| **PyTorch Model (nn.Module)** | `torch` | `torch-via-onnx` |
| **Python Function (callable)** | `torch`, `ast` | `torch-via-onnx`, `ast-via-onnx` |

## Backend Lowering Strategies

| Backend Type | Backends | Lowering Entry |
|--------------|----------|----------------|
| **CPU** | antlib, seal, openfhe | POLY -> C/C++ -> library |
| **GPU** | phantom, hyperfhe | CKKS -> C/C++ -> library |

## Package Module Summary

| Module | Directory | Purpose |
|--------|----------|---------|
| Decorators | `ace/fhe/decorators.py` | User-facing API (@compile, @compute, @export) |
| Driver | `ace/fhe/driver/` | Orchestrate compilation and computation pipeline |
| Frontend | `ace/fhe/frontend/` | Convert Python/PyTorch/ONNX to AIR |
| Backend | `ace/fhe/backend/` | 5-level IR lowering (TENSOR->POLY), generates code for different libraries |
| Runtime | `ace/fhe/runtime/` | FHE execution (encrypt/run/decrypt) for CPU/GPU |


## Test Coverage Matrix

```
+---------------------+-------------+-------------+-------------+
| Frontend            | file air    | file onnx   | memory      |
+---------------------+-------------+-------------+-------------+
| onnx                |     na      |     +       |     -       |
| torch               |     +       |     na      |     -       |
| ast                 |     -       |     na      |     -       |
+---------------------+-------------+-------------+-------------+

Legend: + = implemented, - = not implemented
```

## File Structure

```
ace-compiler/                 # Repository root
+-- fhe_dsl/  
|   +-- ace/fhe/                  # Python source (pip install ace-fhe)
|   |   +-- decorators.py         # @compile, @compute, @export
|   |   +-- driver/               # Pipeline orchestration
|   |   +-- frontend/             # Python/PyTorch/ONNX -> AIR
|   |   +-- backend/              # Lowering and generate FHE code for different libraries
|   |   +-- runtime/              # encrypted execution orchestration for CPU/GPU
|   |   +-- config/, util/        # Configuration and utilities
|   +-- csrc/                     # C++ PyTorch extension
|   |   +-- frontend/             # C++ Frontend: PyTorch graph -> AIR conversion
|   |   +-- runtime/              # C++ Runtime: low-level FHE computation kernels
|
+-- compiler/                   # FHE Backend (5-level IR lowering)
|   +-- air-infra/              # AIR infrastructure
|   +-- fhe-bedriver/           # AIR infrastructure
|   |   +-- nn-addon/           # Neural network extensions
|   |   +-- fhe-cmplr/          # IR lowering and C/C++ code generation
|   +-- airtool/                # AIR tools
|   +-- driver/                 # Compiler Driver
|
+-- fhe_lib/                    # External FHE library integration (phantom, hyperfhe)
|   +-- antlib/                 # CPU FHE library
|   +-- seal/                   # CPU FHE library (Microsoft SEAL)
|   +-- openfhe/                # CPU FHE library (OpenFHE)
|   +-- phantom/                # GPU FHE library (CUDA)
|   +-- hyperfhe/               # GPU FHE library (CUDA)
|
+-- docs/                       # Documents
|   +-- design/                 # Architecture & module design
|   +-- dev/                    # Development guides
|   +-- release/                # Release & operations
|   +-- topics/                 # In-depth analysis & issue tracking
|   +-- plans/                  # Implementation plans
|   +-- updates/                # Development updates (date-stamped)
|
+-- tests/                      # Test suite
+-- benchmarks/                 # Benchmarks suite
+-- examples/                   # Example code
+-- scripts/                    # Build and development scripts
```

## Related Documents

- [Decorators Design](decorators.md) - Decorators and user-facing API
- [Frontend Design](frontend.md) - Frontend module details
- [Backend Design](backend.md) - Backend implementations
- [Development Guide](../dev/develop.md) - Setup and workflow
- [All Documents](../README.md) - Documentation index
- [Driver Design](driver.md) - Driver and compiler orchestration
