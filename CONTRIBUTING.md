# Contributing to ACE

Thank you for your interest in contributing to ACE! This guide covers the essentials for code quality and development workflow.

## Code Style

We use automated tools to enforce consistent code style:

| Language | Tool | Config |
|----------|------|--------|
| Python | [ruff](https://docs.astral.sh/ruff/) (lint + format) | `ruff.toml` |
| C/C++/CUDA | [clang-format](https://clang.llvm.org/docs/ClangFormat.html) | `.clang-format` |

## Code Checks

### Local Checks

```bash
# Check all code (Python + C/C++)
make lint

# Check Python only
make lint-python

# Check C/C++ only
make lint-cpp

# Auto-fix Python issues (ruff --fix + ruff format)
make lint-fix
```

### Pre-commit Hooks (Recommended)

Install pre-commit to automatically check code before each commit:

```bash
pip install pre-commit
pre-commit install
```

This runs ruff, clang-format, and file checks (trailing whitespace, YAML validation, etc.) on every `git commit`. To skip hooks for an urgent fix:

```bash
git commit --no-verify
```

### CI Enforcement

All pull requests must pass the lint CI check before merge. This is the hard gate — even if you skip pre-commit locally, CI will catch issues.

## Development Setup

See [Developer Guide](docs/dev/develop.md) for build instructions and development workflow.

## Pull Request Process

1. Fork the repository and create a feature branch
2. Make your changes
3. Run `make lint` locally to catch issues early
4. Ensure all tests pass: `make test`
5. Submit your PR — CI will run lint and tests automatically
