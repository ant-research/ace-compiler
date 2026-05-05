#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Base test classes for ANT-ACE testing framework.

Provides reusable test base classes for:
- CompileSpec validation
- Frontend tests
- Driver tests
- Backend tests

Usage:
    from tests.base import BaseCompileSpecTest, BaseFrontendTest

    class TestMyFeature(BaseCompileSpecTest):
        def test_something(self, spec):
            # Your test logic here
            pass
"""
import pytest
from typing import Optional, List


# =============================================================================
# Base class for CompileSpec tests
# =============================================================================

class BaseCompileSpecTest:
    """Base class for all CompileSpec-based tests.

    Provides common validation logic for:
    - Entity validation (name, structure)
    - CompileConfig validation (input_spec, options)
    - RuntimeConfig validation (optional)
    """

    def test_entity_has_name(self, spec):
        """Test that entity has a valid name."""
        assert spec.entity.name is not None
        assert len(spec.entity.name) > 0

    def test_compile_config_has_input_spec(self, spec):
        """Test that compile config has input_spec."""
        assert spec.compile.input_spec is not None
        assert len(spec.compile.input_spec) > 0

    def test_input_spec_has_shape_and_dtype(self, spec):
        """Test that input_spec has valid shape and dtype."""
        for inp in spec.compile.input_spec:
            assert inp.shape is not None
            assert len(inp.shape) > 0
            assert inp.dtype is not None

    def test_compile_config_has_frontend(self, spec):
        """Test that compile config has frontend specified."""
        assert spec.compile.frontend is not None

    def test_compile_config_has_library(self, spec):
        """Test that compile config has library specified."""
        assert spec.compile.library is not None

    def test_compile_config_has_device(self, spec):
        """Test that compile config has device specified."""
        assert spec.compile.device is not None

    def test_entity_has_ir_ops(self, spec):
        """Test that entity has ir_ops field (optional)."""
        assert hasattr(spec.entity, 'ir_ops')


# =============================================================================
# Base class for ModelEntity tests
# =============================================================================

class BaseModelEntityTest(BaseCompileSpecTest):
    """Base class for ModelEntity-based tests.

    Provides common validation logic for model entities.
    """

    def test_entity_has_model_class(self, spec):
        """Test that entity has model_class."""
        assert spec.entity.model_class is not None

    def test_entity_has_weights_required_flag(self, spec):
        """Test that entity has weights_required flag."""
        assert hasattr(spec.entity, 'weights_required')

    def test_entity_has_constants_required_flag(self, spec):
        """Test that entity has constants_required flag."""
        assert hasattr(spec.entity, 'constants_required')


# =============================================================================
# Base class for FuncEntity tests
# =============================================================================

class BaseFuncEntityTest(BaseCompileSpecTest):
    """Base class for FuncEntity-based tests.

    Provides common validation logic for function entities.
    """

    def test_entity_has_func(self, spec):
        """Test that entity has func."""
        assert spec.entity.func is not None
        assert callable(spec.entity.func)


# =============================================================================
# Base class for IR validation tests
# =============================================================================

class BaseIRValidationTest:
    """Base class for IR validation tests.

    Provides common validation logic for IR structure.
    """

    def test_ir_ops_match_entity(self, spec, ir_ops):
        """Test that IR ops match entity's expected ops.

        Args:
            spec: CompileSpec instance
            ir_ops: List of actual IR ops from generated IR
        """
        if spec.entity.ir_ops:
            for expected_op in spec.entity.ir_ops:
                assert expected_op in ir_ops, f"Expected op '{expected_op}' not found in IR"

    def test_ir_has_entry_point(self, ir):
        """Test that IR has valid entry point."""
        assert hasattr(ir, 'entry_name')
        assert ir.entry_name is not None

    def test_ir_has_graphs(self, ir):
        """Test that IR has graphs."""
        assert hasattr(ir, 'graphs')
        assert len(ir.graphs) > 0


# =============================================================================
# Base class for Frontend tests
# =============================================================================

class BaseFrontendTest:
    """Base class for frontend tests.

    Provides common validation logic for frontend implementations.
    """

    def test_frontend_has_name(self, frontend):
        """Test that frontend has name."""
        assert hasattr(frontend, 'name')
        assert frontend.name() is not None

    def test_frontend_prepare_returns_ir(self, frontend, model, inputs, input_names):
        """Test that prepare() returns IR object."""
        result = frontend.prepare(model, inputs, input_names)
        assert result is not None

    def test_frontend_compile_returns_ir(self, frontend, model, inputs, input_names):
        """Test that compile() returns IR object."""
        result = frontend.compile(model, inputs, input_names)
        assert result is not None


# =============================================================================
# Base class for Backend tests
# =============================================================================

class BackendTestBase:
    """Base class for backend tests.

    Provides common validation logic for backend implementations.
    """

    def test_backend_has_name(self, backend):
        """Test that backend has name."""
        assert hasattr(backend, 'name')
        assert backend.name() is not None

    def test_backend_has_supported_format_types(self, backend):
        """Test that backend has supported_format_types."""
        assert hasattr(backend, 'supported_format_types')
        assert callable(backend.supported_format_types)


# =============================================================================
# Base class for Runtime tests
# =============================================================================

class BaseRuntimeTest:
    """Base class for runtime tests.

    Provides common validation logic for runtime implementations.
    """

    def test_runtime_can_execute(self, runtime):
        """Test that runtime can execute."""
        assert hasattr(runtime, 'execute')
        assert callable(runtime.execute)

    def test_runtime_has_encrypt_method(self, runtime):
        """Test that runtime has encrypt method."""
        assert hasattr(runtime, 'encrypt')
        assert callable(runtime.encrypt)

    def test_runtime_has_decrypt_method(self, runtime):
        """Test that runtime has decrypt method."""
        assert hasattr(runtime, 'decrypt')
        assert callable(runtime.decrypt)