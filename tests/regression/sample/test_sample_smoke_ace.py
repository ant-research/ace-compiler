# tests/regression/sample/test_smoke_ace.py
"""
Acelib (CUDA) FHE smoke tests: compilation and inference.

Tests FHE compilation and inference for built-in ops and functions
on the acelib-cuda backend. These tests are slow — use -m "not slow" to skip.

Run examples:
    pytest tests/regression/sample/test_smoke_ace.py            # all acelib tests
    pytest tests/regression/sample/test_smoke_ace.py -k "op"    # ops only
    pytest tests/regression/sample/test_smoke_ace.py -k "func"  # funcs only
    pytest tests/regression/sample/ -m "not slow"                   # skip all FHE tests
"""
import pytest
import torch

from ace import fhe
from ace.sample.ops.specs import ALL_OPS_SPECS
from ace.sample.funcs.specs import ALL_FUNCS_SPECS
from utils import requires_torch, requires_gpu, HAS_FRONTEND


def _acelib_available():
    try:
        from ace.fhe.backend import get_library_impl
        return get_library_impl("acelib", device="cuda").check_available()
    except Exception:
        return False

# ============================================================================
# Ops
# ============================================================================

# Ops that fail at FHE compile time:
#   sub_op — fhe_cmplr backend does not support NN SUB opcode
#   div_op — AIR IR generation fails (Op_num_child assertion)
#   sigmoid_op — torch frontend missing sigmoid custom op registration (KeyError)
#   tanh_op — torch frontend missing tanh custom op registration (KeyError)
#   sqrt_op — torch frontend missing abs custom op registration (KeyError)
#   softmax_op — fhe_cmplr backend does not support NN SOFTMAX
BROKEN_COMPILE_OPS = {
    "sub_op", "div_op", "sigmoid_op", "tanh_op", "sqrt_op", "softmax_op",
}

# Ops that compile OK but fail at FHE inference:
#   conv2d_relu_op — segfaults in executor
#   linear_op — ace-library inference error
#   linear_relu_op — segfaults in ace-library executor
#   relu_linear_op — segfaults in ace-library executor
#   mlp_op — segfaults in ace-library executor
#   flatten_op — ace-library inference error
#   conv2d_op — ace-library inference error
#   depthwise_conv2d_op — segfaults in ace-library executor
#   avg_pool2d_op — segfaults in ace-library executor
#   max_pool2d_op — segfaults in ace-library executor
#   global_avg_pool_op — segfaults in ace-library executor
#   relu_op — ace-library inference error
#   gemm_49x3 — ace-library inference error
#   relu_gemm — ace-library inference error
#   conv2d — ace-library inference error
#   avg_pool_2d — segfaults in ace-library executor
#   avg_pool_2d_with_stride — segfaults in ace-library executor
#   global_avg_pool — segfaults in ace-library executor
#   relu_avg_pool — segfaults in ace-library executor
#   avg_pool_flatten — segfaults in ace-library executor
BROKEN_SMOKE_OPS = {
    "conv2d_relu_op",
    "linear_op", "linear_relu_op", "relu_linear_op", "mlp_op",
    "flatten_op", "conv2d_op", "depthwise_conv2d_op",
    "avg_pool2d_op", "max_pool2d_op", "global_avg_pool_op",
    "relu_op", "gemm_49x3", "relu_gemm", "conv2d",
    "avg_pool_2d", "avg_pool_2d_with_stride", "global_avg_pool",
    "relu_avg_pool", "avg_pool_flatten",
}

COMPILE_OPS = [s for s in ALL_OPS_SPECS if s.name not in BROKEN_COMPILE_OPS]
SMOKE_OPS = [s for s in ALL_OPS_SPECS
             if s.name not in BROKEN_COMPILE_OPS
             and s.name not in BROKEN_SMOKE_OPS]

# ============================================================================
# Funcs
# ============================================================================

# Funcs that fail at FHE compile time:
#   sub_func — fhe_cmplr does not support Sub opcode
#   div_func — fhe_cmplr does not support Div opcode
#   abs_func — fhe_cmplr does not support Abs opcode
#   neg_func — fhe_cmplr does not support Neg opcode
#   sqrt_func — fhe_cmplr does not support Sqrt opcode from AST
#   clamp_func — fhe_cmplr does not support Clamp/Clip opcode
#   log_func — fhe_cmplr does not support Log opcode
#   exp_func — fhe_cmplr does not support Exp opcode
#   sigmoid_func — fhe_cmplr Op_num_child assertion failure
#   tanh_func — fhe_cmplr Op_num_child assertion failure
#   softmax_func — AST generates "F.softmax" (not normalized to "softmax")
#   conditional_add_func — fhe_cmplr does not support control flow ops
#   conditional_relu_func — fhe_cmplr does not support control flow ops
#   loop_multiply_func — fhe_cmplr does not support control flow ops
#   loop_add_func — fhe_cmplr does not support control flow ops
#   nested_loop_func — fhe_cmplr does not support control flow ops
#   while_loop_func — AST converter does not support while loops
#   branch_execution_func — fhe_cmplr does not support control flow ops
BROKEN_COMPILE_FUNCS: set = {
    "sub_func", "div_func", "abs_func", "neg_func",
    "sqrt_func", "clamp_func", "log_func", "exp_func",
    "sigmoid_func", "tanh_func", "softmax_func",
    "conditional_add_func", "conditional_relu_func",
    "loop_multiply_func", "loop_add_func", "nested_loop_func",
    "while_loop_func", "branch_execution_func",
}

# Funcs that compile OK but fail at FHE inference:
#   square_func — segfaults (unsupported unary op)
#   relu_func — ace-library inference error (passes on antlib)
#   conditional_chain_func — Invalid level error (CKKS mul_depth=0 insufficient)
BROKEN_SMOKE_FUNCS = {
    "square_func", "relu_func", "conditional_chain_func",
}

COMPILE_FUNCS = [s for s in ALL_FUNCS_SPECS if s.name not in BROKEN_COMPILE_FUNCS]
SMOKE_FUNCS = [s for s in ALL_FUNCS_SPECS
               if s.name not in BROKEN_COMPILE_FUNCS
               and s.name not in BROKEN_SMOKE_FUNCS]


# ============================================================================
# Helpers
# ============================================================================

def _compile_op(spec, device="cuda"):
    """Compile an op model with built-in ReLU VR profiling."""
    model = spec.create_model()

    compiled_model = fhe.compile(
        frontend="torch",
        library="acelib",
        device=device,
        encrypt_inputs=spec.encrypt_inputs,
        profile_relu=True,
    )(model)

    return compiled_model.fhe_compile(spec.example_inputs)


def _compile_func(spec, device="cuda"):
    """Compile a function with built-in ReLU VR profiling."""
    compiled = fhe.compile(
        frontend="ast",
        library="acelib",
        device=device,
        encrypt_inputs=spec.encrypt_inputs,
        profile_relu=True,
    )(spec.func)

    return compiled.compile(spec.example_inputs)


# ============================================================================
# Ops: FHE Compilation (nn.Module)
# ============================================================================

@requires_torch
@requires_gpu
@pytest.mark.parametrize("spec", COMPILE_OPS, ids=lambda s: s.name)
def test_op_compile(spec):
    """Op FHE compilation on acelib-cuda."""
    if not _acelib_available():
        pytest.skip("acelib-cuda compiler not available")
    prog = _compile_op(spec)
    assert prog is not None


# ============================================================================
# Ops: FHE Inference (nn.Module)
# ============================================================================

@requires_torch
@requires_gpu
@pytest.mark.parametrize("spec", SMOKE_OPS, ids=lambda s: f"{s.name}_cuda")
def test_op_smoke_cuda(spec):
    """Op FHE inference on acelib-cuda."""
    if not _acelib_available():
        pytest.skip("acelib-cuda compiler not available")
    prog = _compile_op(spec)
    assert prog is not None

    runner = fhe.FHERuntime(prog.package)
    runner.inference(*spec.example_inputs)
    runner.finalize()
    if torch.cuda.is_available():
        torch.cuda.empty_cache()

@requires_torch
@requires_gpu
@pytest.mark.parametrize("spec", COMPILE_FUNCS, ids=lambda s: s.name)
def test_func_compile(spec):
    """Function FHE compilation on acelib-cuda."""
    if not _acelib_available():
        pytest.skip("acelib-cuda compiler not available")
    prog = _compile_func(spec)
    assert prog is not None


# ============================================================================
# Funcs: FHE Inference (Python function)
# ============================================================================

@requires_torch
@requires_gpu
@pytest.mark.parametrize("spec", SMOKE_FUNCS, ids=lambda s: f"{s.name}_cuda")
def test_func_smoke_cuda(spec):
    """Function FHE inference on acelib-cuda."""
    if not _acelib_available():
        pytest.skip("acelib-cuda compiler not available")
    prog = _compile_func(spec)
    assert prog is not None

    runner = fhe.FHERuntime(prog.package)
    runner.inference(*spec.example_inputs)
    runner.finalize()
    if torch.cuda.is_available():
        torch.cuda.empty_cache()