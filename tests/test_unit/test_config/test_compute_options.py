# tests/test_unit/test_config/test_compute_options.py
import pytest
from ace.fhe.config import ComputeOptions


class TestComputeOptions:
    """Unit tests for ComputeOptions."""

    def test_default_values(self):
        """Test default values are set correctly."""
        options = ComputeOptions()
        assert options.encrypt_inputs is None
        assert options.validate is True
        assert options.server_url is None

    def test_custom_validate(self):
        """Test setting custom validate."""
        options = ComputeOptions(validate=False)
        assert options.validate is False

    def test_custom_server_url(self):
        """Test setting custom server_url."""
        options = ComputeOptions(server_url="http://localhost:8080")
        assert options.server_url == "http://localhost:8080"

    def test_inherits_from_compile_options(self):
        """Test that ComputeOptions inherits from CompileOptions."""
        options = ComputeOptions(encrypt_inputs=["x"])
        assert options.encrypt_inputs == ["x"]
        assert options.config.scheme == "CKKS"

    def test_all_options(self):
        """Test all options together."""
        options = ComputeOptions(
            encrypt_inputs=["x", "y"],
            validate=False,
            server_url="http://example.com"
        )

        assert options.encrypt_inputs == ["x", "y"]
        assert options.validate is False
        assert options.server_url == "http://example.com"