# ACE Compiler Documentation

## design/ — Architecture & Module Design

Stable design documents describing the architecture and module interfaces.

| Document | Description |
|----------|-------------|
| [overall.md](design/overall.md) | Overall architecture of the ACE compiler |
| [frontend.md](design/frontend.md) | Frontend: Python/PyTorch/ONNX to IR |
| [ir.md](design/ir.md) | IR: intermediate representation formats |
| [backend.md](design/backend.md) | Backend: IR to executable shared libraries |
| [driver.md](design/driver.md) | Driver: compilation pipeline orchestration |
| [decorators.md](design/decorators.md) | User-facing decorator API (@compile, @compute, @export) |
| [cli.md](design/cli.md) | CLI tools design (ace_tool subcommands, architecture, adding new commands) |
| [relu-vr-profiling.md](design/relu-vr-profiling.md) | ReLU VR profiling system (FX Interpreter-based) |
| [runtime-profiling.md](design/runtime-profiling.md) | Runtime profiling: per-phase timing, memory, Chrome Trace |
| [bn-folding.md](design/bn-folding.md) | BatchNorm folding strategy in Torch Frontend |
| [rtlib-api-alignment.md](design/rtlib-api-alignment.md) | RTLib API alignment across antlib/phantom providers |

### Compiler IR & Code Generation (submodule docs)

The compiler's IR design and code generation are documented in the three submodules under `compiler/`. These are the authoritative references
for the lowering pipeline (`AIR IR → FHE IR → C/CU`):

| Submodule | Entry point | Topics |
|-----------|-------------|--------|
| air-infra | [air-infra/doc/README.md](../compiler/air-infra/doc/README.md) | AIR IR design: container, type, attributes, traversal, codegen, passes, serialization |
| nn-addon | [nn-addon/doc/README.md](../compiler/nn-addon/doc/README.md) | NN IR design: vector IR, tensor sharding, GEMV |
| fhe-cmplr | [fhe-cmplr/doc/README.md](../compiler/fhe-cmplr/doc/README.md) | FHE IR design: CKKS/poly op descriptors, pipeline, region scheduling, runtime debug |

## api/ — User API Reference

Quick-reference documentation for user-facing APIs.

| Document | Description |
|----------|-------------|
| [decorators.md](api/decorators.md) | `@fhe.compile`, `@fhe.compute`, `@fhe.export` API reference |
| [compile-options.md](api/compile-options.md) | Compile options quick reference (ckks, vec, p2c, sihe) |
| [runtime.md](api/runtime.md) | `CompiledProgram` API: inference, validation, profiling |

## dev/ — Development Guide

Guides for developers working on ACE.

| Document | Description |
|----------|-------------|
| [develop.md](dev/develop.md) | Environment setup and development workflow |
| [testing.md](dev/testing.md) | Testing strategy, unit/integration/regression test guide |
| [compile-options.md](dev/compile-options.md) | FHE compiler options design (vec, ckks, sihe, p2c) |
| [package.md](dev/package.md) | Package structure and module organization |
| [release.md](dev/release.md) | Versioning strategy and release process |
| [ci.md](dev/ci.md) | CI/CD architecture (ACI + GitHub Actions) |

## benchmarks/ — Performance Benchmarks

| Document | Description |
|----------|-------------|
| [resnet-scripts.md](benchmarks/resnet-scripts.md) | How to run ResNet benchmarks |
| [resnet-accuracy.md](benchmarks/resnet-accuracy.md) | ResNet FHE inference accuracy results |
| [resnet-phantom.md](benchmarks/resnet-phantom.md) | ResNet performance on the phantom-cuda backend |
| [resnet-ace.md](benchmarks/resnet-ace.md) | ResNet performance on the ace-cuda backend |
| [resnet-training.md](benchmarks/resnet-training.md) | ResNet training configuration and plaintext baselines |
| [io/resnet-io-benchmark.md](benchmarks/io/resnet-io-benchmark.md) | ResNet key/weight I/O benchmark (staged vs GDS) |
| [io/io-read-baseline.md](benchmarks/io/io-read-baseline.md) | Raw NVMe / GDS read bandwidth baselines |

## updates/ — Development Progress

Dated progress records following the convention `YYYY-MM-DD-topic-name.md`. See [updates/README.md](updates/README.md) for the full index.
