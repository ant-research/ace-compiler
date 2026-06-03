# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [0.2.0] - 2025-05-02

### Added
- Initial release of ANT-ACE FHE compiler
- Support for multiple frontends: onnx, torch, ast
- Support for multiple fhe libraries: antlib (CPU), phantom (GPU), acelib (GPU)
- CKKS encryption scheme support
- High-level Python API: @compile, @compute, @export decorators

### Fixed
- Directory structure refactored
- Wheel package structure fixed to avoid nested directories

### Changed
- Package name changed to `ace`
- Version path updated from `python/ace/_version.py` to `fhe_dsl/python/_version.py`

## [0.1.0] - 2025-01-01

### Added
- Project initialization
- Basic FHE compilation pipeline