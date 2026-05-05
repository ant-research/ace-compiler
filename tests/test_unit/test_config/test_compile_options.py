# tests/test_unit/test_config/test_compile_options.py
import pytest
from ace.fhe.config import CompileOptions


class TestCompileOptions:
    """Unit tests for CompileOptions."""

    def test_default_values(self):
        """Test default values are set correctly."""
        options = CompileOptions()
        assert options.encrypt_inputs is None
        assert options.config.scheme == "CKKS"
        assert options.config.poly_modulus_degree == 8192

    def test_custom_encrypt_inputs(self):
        """Test setting custom encrypt_inputs."""
        options = CompileOptions(encrypt_inputs=["x", "y"])
        assert options.encrypt_inputs == ["x", "y"]

    def test_encrypt_inputs_as_indices(self):
        """Test setting encrypt_inputs as indices."""
        options = CompileOptions(encrypt_inputs=[0, 1])
        assert options.encrypt_inputs == [0, 1]

    def test_custom_fhe_config(self):
        """Test setting custom FHE config."""
        from ace.fhe.config.compile_options import FHEConfig

        config = FHEConfig(
            scheme="TFHE",
            poly_modulus_degree=4096,
            multiplication_depth=3
        )
        options = CompileOptions(config=config)

        assert options.config.scheme == "TFHE"
        assert options.config.poly_modulus_degree == 4096
        assert options.config.multiplication_depth == 3

    def test_dataclass_fields(self):
        """Test that expected fields exist."""
        fields = CompileOptions.__dataclass_fields__.keys()
        assert "encrypt_inputs" in fields
        assert "config" in fields