# tests/regression/sample/test_sample_compile.py
"""
Sample compilation pipeline tests.

Frontend coverage:
- ops: torch frontend (FX trace -> AIR) + IR structure regression
- funcs: ast frontend (AST -> FHEProgram)

Run with: pytest tests/regression/sample/test_compile.py
"""
import pytest

from ace.fhe.frontend import get_frontend
from ace.fhe.ir import extract_ir_structure
from ace.sample.ops.specs import ALL_OPS_SPECS
from ace.sample.funcs.specs import ALL_FUNCS_SPECS
from utils import requires_torch


# ============================================================================
# Ops: Torch Frontend Tests (nn.Module)
# ============================================================================

@requires_torch
class TestOpFrontendTorch:
    """Test torch frontend for ops (FX trace -> AIR)."""

    @pytest.mark.parametrize("spec", ALL_OPS_SPECS, ids=lambda s: s.name)
    def test_trace(self, spec, data_regression):
        """Test IR structure after torch frontend tracing."""
        model = spec.create_model()
        model.eval()

        frontend = get_frontend("torch")
        traced = frontend.prepare(model, list(spec.example_inputs))

        structure = extract_ir_structure(traced)
        data_regression.check(structure)


# ============================================================================
# Funcs: AST Frontend Tests (Python function)
# ============================================================================

# Funcs that fail at FHE compile time (fhe_cmplr does not support these ops):
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
BROKEN_COMPILE_FUNCS = {
    "sub_func", "div_func", "abs_func", "neg_func",
    "sqrt_func", "clamp_func", "log_func", "exp_func",
    "sigmoid_func", "tanh_func", "softmax_func",
    "conditional_add_func", "conditional_relu_func",
    "loop_multiply_func", "loop_add_func", "nested_loop_func",
    "while_loop_func", "branch_execution_func",
}

COMPILE_FUNCS = [s for s in ALL_FUNCS_SPECS if s.name not in BROKEN_COMPILE_FUNCS]


@requires_torch
class TestFuncFrontendAST:
    """Test ast frontend for functions (AST -> FHEProgram)."""

    @pytest.mark.parametrize("spec", COMPILE_FUNCS, ids=lambda s: s.name)
    def test_trace(self, spec):
        """Test ast frontend tracing for functions."""
        func = spec.func
        encrypt_inputs = spec.encrypt_inputs

        from ace import fhe
        compiled = fhe.compile(
            frontend="ast",
            library="antlib",
            device="cpu",
            encrypt_inputs=encrypt_inputs,
        )(func)

        # Verify compilation succeeds (produces a valid program)
        prog = compiled.compile(spec.example_inputs)
        assert prog is not None