# Updates & Progress

This directory tracks development progress, design decisions, and implementation updates.

## Directory Structure

```
updates/
├── README.md                              # This file - index and guidelines
├── 2026-04-08-ir-formats-refactor.md      # IR format classes refactoring
├── 2026-04-08-torch-via-onnx-review.md    # torch-via-onnx frontend review
└── ...
```

## Naming Convention

Files are named by date and topic: `YYYY-MM-DD-topic-name.md`

## Update Categories

| Category | Description |
|----------|-------------|
| **refactor** | Code restructuring, renaming, cleanup |
| **feature** | New functionality added |
| **fix** | Bug fixes and corrections |
| **design** | Design decisions and architecture changes |
| **docs** | Documentation updates |

## Index

### 2026-04-08

| File | Category | Description |
|------|----------|-------------|
| [ir-formats-refactor.md](2026-04-08-ir-formats-refactor.md) | refactor | Renamed IR format classes, added `file_format` property |
| [torch-via-onnx-review.md](2026-04-08-torch-via-onnx-review.md) | fix | Fixed torch-via-onnx output modes, clarified memory path not implemented |
| [torch-via-onnx-tests.md](2026-04-08-torch-via-onnx-tests.md) | test | Rewrote unit tests with method-based organization and parametrization |
| [ast-via-onnx-review.md](2026-04-08-ast-via-onnx-review.md) | fix, test | Fixed ast-via-onnx to match torch-via-onnx behavior, rewrote tests |
| [onnx-frontend-review.md](2026-04-08-onnx-frontend-review.md) | fix, test | Fixed onnx frontend, reorganized test files, rewrote tests |
| [torch-frontend-review.md](2026-04-08-torch-frontend-review.md) | fix, refactor, test | Added `file_format` property, ONNX export, rewrote tests |
| [ast-frontend-review.md](2026-04-08-ast-frontend-review.md) | fix, refactor, test | Added `file_format` to FHEProgram, fixed AST converter, rewrote tests |
| [add-air-operation-api.md](2026-04-08-add-air-operation-api.md) | feature, refactor | Added `add_air_operation()` API for unified IR generation |

## Writing Guidelines

Each update file should include:

1. **Summary** - Brief description of changes
2. **Changes** - Detailed list of modified files and key changes
3. **Technical Details** - Implementation notes and decisions
4. **Impact** - What this affects (API, tests, docs)
5. **Related** - Links to related docs or issues