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

## dev/ — Development Guide

Guides for developers working on ACE.

| Document | Description |
|----------|-------------|
| [develop.md](dev/develop.md) | Environment setup and development workflow |
| [compile_options.md](dev/compile_options.md) | FHE compiler options reference (vec, ckks, sihe, p2c) |
| [package.md](dev/package.md) | Package structure and module organization |

### dev/testing/ — Testing

| Document | Description |
|----------|-------------|
| [index.md](dev/testing/index.md) | Testing strategy and guidelines |
| [backend_unittest.md](dev/testing/backend_unittest.md) | Backend module unit tests |
| [driver_unittest.md](dev/testing/driver_unittest.md) | Driver module unit tests |
| [frontend_unittest.md](dev/testing/frontend_unittest.md) | Frontend module unit tests |
| [ir_unittest.md](dev/testing/ir_unittest.md) | IR module unit tests |

## release/ — Release & Operations

| Document | Description |
|----------|-------------|
| [release.md](release/release.md) | Versioning strategy and release process |

## topics/ — In-depth Analysis & Issue Tracking

Deep-dive analyses, profiling reports, and issue investigations.

| Document | Description |
|----------|-------------|
| [resnet20_relu_issue.md](topics/resnet20_relu_issue.md) | ResNet20 ReLU value range accuracy problem |
| [bn_folding_design.md](topics/bn_folding_design.md) | BatchNorm folding strategy in Torch Frontend |
| [constant_comparison.md](topics/constant_comparison.md) | Constant handling: onnx2air vs Torch Frontend |
| [resnet20_torch_frontend_test.md](topics/resnet20_torch_frontend_test.md) | ResNet20 Torch Frontend test walkthrough |