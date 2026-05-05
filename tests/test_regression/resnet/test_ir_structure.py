# test_regression/resnet/test_ir_structure.py
"""
Regression tests for ResNet AIR IR structure validation.

Validates that the torch frontend generates the expected IR structure
for ResNet-20/32/44/56 models. Baseline data is in data/ir_baselines.py.
"""
import pytest

from test_regression.resnet.data import RESNET_MODEL_TEST_CASES, IR_BASELINES
from test_regression.resnet.data.ir_baselines import (
    extract_ir_structure,
    validate_resnet_ir,
)
from test_utils import TORCH_FX_AVAILABLE, HAS_FRONTEND


requires_torch_fx = pytest.mark.skipif(
    not TORCH_FX_AVAILABLE,
    reason="torch.fx not available"
)

requires_frontend = pytest.mark.skipif(
    not HAS_FRONTEND,
    reason="C++ frontend extension not available"
)


@requires_torch_fx
@requires_frontend
class TestResNetIRStructure:
    """Test ResNet IR structure against baseline for all variants."""

    def test_ir_structure_baseline(self, resnet_case):
        """Validate IR structure matches the baseline for the given variant."""
        from ace.fhe.frontend import get_frontend

        model = resnet_case.create_model()
        model_inputs = resnet_case.example_inputs

        frontend = get_frontend("torch")
        traced = frontend.prepare(model, list(model_inputs))
        traced.execute(*model_inputs)

        baseline = IR_BASELINES.get(resnet_case.name)
        assert baseline is not None, f"No IR baseline for {resnet_case.name}"

        is_valid, differences = validate_resnet_ir(traced, traced._constants, baseline)
        if not is_valid:
            pytest.fail(f"IR structure validation failed:\n" + "\n".join(differences))

    def test_operation_counts(self, resnet_case):
        """Test that operation counts match baseline."""
        from ace.fhe.frontend import get_frontend

        model = resnet_case.create_model()
        model_inputs = resnet_case.example_inputs

        frontend = get_frontend("torch")
        traced = frontend.prepare(model, list(model_inputs))
        traced.execute(*model_inputs)

        baseline = IR_BASELINES[resnet_case.name]
        structure = extract_ir_structure(traced, traced._constants)

        for op, expected_count in baseline["operations"].items():
            actual_count = structure["operations"].get(op, 0)
            assert actual_count == expected_count, \
                f"Operation '{op}' count mismatch: expected {expected_count}, got {actual_count}"

    def test_conv_layer_count(self, resnet_case):
        """Test that conv layer count matches baseline."""
        from ace.fhe.frontend import get_frontend

        model = resnet_case.create_model()
        model_inputs = resnet_case.example_inputs

        frontend = get_frontend("torch")
        traced = frontend.prepare(model, list(model_inputs))
        traced.execute(*model_inputs)

        baseline = IR_BASELINES[resnet_case.name]
        structure = extract_ir_structure(traced, traced._constants)

        expected_conv = len(baseline["conv_layers"])
        actual_conv = len(structure["conv_layers"])
        assert actual_conv == expected_conv, \
            f"Conv layer count mismatch: expected {expected_conv}, got {actual_conv}"

    def test_constant_count(self, resnet_case):
        """Test that constant count matches baseline."""
        from ace.fhe.frontend import get_frontend

        model = resnet_case.create_model()
        model_inputs = resnet_case.example_inputs

        frontend = get_frontend("torch")
        traced = frontend.prepare(model, list(model_inputs))

        baseline = IR_BASELINES[resnet_case.name]
        actual_constants = len(traced._constants)
        expected_constants = baseline["total_constants"]
        assert actual_constants == expected_constants, \
            f"Constant count mismatch: expected {expected_constants}, got {actual_constants}"

    def test_input_output_shapes(self, resnet_case):
        """Test that input/output shapes match baseline."""
        from ace.fhe.frontend import get_frontend

        model = resnet_case.create_model()
        model_inputs = resnet_case.example_inputs

        frontend = get_frontend("torch")
        traced = frontend.prepare(model, list(model_inputs))

        baseline = IR_BASELINES[resnet_case.name]
        assert traced._input_shapes[0] == baseline["input_shape"], \
            f"Input shape mismatch: expected {baseline['input_shape']}, got {traced._input_shapes[0]}"
        assert traced._output_shape == baseline["output_shape"], \
            f"Output shape mismatch: expected {baseline['output_shape']}, got {traced._output_shape}"


if __name__ == "__main__":
    pytest.main([__file__, "-v"])