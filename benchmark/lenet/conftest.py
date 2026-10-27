#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# LeNet-specific fixtures for FHE benchmark tests.
#
# LeNet is a small CNN with ReLUs, so it must be compiled with built-in
# profile_relu=True (the bundled VR file's node names do not match the
# compiled IR — see tests/regression/lenet). Compiled once per
# (library, device) with session scope because FHE compilation is slow.

import copy

import pytest

from ace import fhe
from ace.model.spec_lenet import LENET_CIFAR10

_gpu_available = fhe.gpu_available()

# LeNet is covered on ant + phantom only. ace-CUDA is deliberately excluded:
# like conv/GEMM ops it hits the known ace-library inference error (see
# tests/regression/sample/test_sample_smoke_ace.py).
SMALL_TARGET_PARAMS = [
    pytest.param(("ant", "cpu"), id="ant-cpu"),
    pytest.param(
        ("phantom", "cuda"),
        marks=pytest.mark.skipif(not _gpu_available, reason="GPU not available"),
        id="phantom-cuda",
    ),
]


def _compile_lenet(spec, library, device):
    """Compile LeNet with built-in ReLU VR profiling."""
    from ace import fhe

    model = spec.create_model()
    model.eval()

    compile_opts = dict(spec.compile_options.get(library, {})) if spec.compile_options else {}
    compile_opts["profile_relu"] = True

    compiled = fhe.compile(
        frontend="torch",
        library=library,
        device=device,
        encrypt_inputs=spec.encrypt_inputs,
        **compile_opts,
    )(model)

    return compiled.fhe_compile(spec.example_inputs)


def _make_compiled_fixture(spec):
    """Factory for compiled LeNet fixture. Returns (program, library, device)."""

    def fixture(request):
        library, device = request.param
        program = _compile_lenet(spec, library, device)
        return program, library, device

    return fixture


compiled_lenet = pytest.fixture(scope="session")(_make_compiled_fixture(LENET_CIFAR10))


def pytest_configure(config):
    config.addinivalue_line("markers", "lenet: LeNet benchmark tests")