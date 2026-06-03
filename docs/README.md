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

## dev/ — Development Guide

Guides for developers working on ACE.

| Document | Description |
|----------|-------------|
| [develop.md](dev/develop.md) | Environment setup and development workflow |
| [compile_options.md](dev/compile_options.md) | FHE compiler options reference (vec, ckks, sihe, p2c) |
| [package.md](dev/package.md) | Package structure and module organization |

### dev/tests/ — Testing

| Document | Description |
|----------|-------------|
| [index.md](dev/tests/index.md) | Testing strategy and guidelines |
| [unit-backend.md](dev/tests/unit-backend.md) | Backend module unit tests |
| [unit-config.md](dev/tests/unit-config.md) | Config options unit tests |
| [unit-driver.md](dev/tests/unit-driver.md) | Driver module unit tests |
| [unit-frontend.md](dev/tests/unit-frontend.md) | Frontend module unit tests |
| [unit-ir.md](dev/tests/unit-ir.md) | IR module unit tests |
| [integration-api.md](dev/tests/integration-api.md) | Top-level API integration tests |
| [regression-resnet.md](dev/tests/regression-resnet.md) | ResNet regression tests |
| [regression-sample.md](dev/tests/regression-sample.md) | Sample ops/funcs regression tests |

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