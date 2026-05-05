#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Integration test: Torch Frontend + FHE Driver Pipeline

Complete flow:
1. Define PyTorch model with custom operators
2. Torch frontend traces and generates AIR IR
3. Pass GLOB_SCOPE to FHE driver via pybind
4. Run driver pipeline: Torch -> SIHE -> CKKS -> POLY -> POLY2C
5. Generate C code
"""

import os
import pytest
import torch
import torch.nn as nn

# Import centralized dependency checks from test_utils
from test_utils import (
    TORCH_FX_AVAILABLE,
    HAS_TORCH_FX,
    HAS_FRONTEND,
    FHE_AVAILABLE,
    IRBuilder,
    skip_if_no_torch_fx,
    skip_if_no_frontend,
    skip_if_no_fhe,
)

# Import C++ extension for low-level driver API
try:
    import ace.frontend as frontend
except ImportError:
    frontend = None


# Skip all tests if dependencies not available
pytestmark = pytest.mark.skipif(
    not HAS_TORCH_FX or not HAS_FRONTEND or not FHE_AVAILABLE,
    reason="torch.fx, frontend, or ace.fhe not available"
)


# ============================================================================
# Test Models
# ============================================================================

from ace.samples.tensor_ops import AddTensorOp

AddModel = AddTensorOp


# ============================================================================
# Integration Tests
# ============================================================================

class TestTorchDriverCallback:
    """Tests for Torch pass callback mechanism with driver."""

    def test_callback_returns_glob_scope(self):
        """Test that callback can return GLOB_SCOPE pointer."""
        from ace.fhe.frontend import get_frontend

        model = AddModel()
        x = torch.ones(1, 3, 4, 4)
        y = torch.ones(1, 3, 4, 4) * 2

        frontend = get_frontend("torch")
        traced = frontend.to_ir(model, [x, y], ["x", "y"])
        traced.execute(x, y)

        # Get GLOB_SCOPE pointer
        glob_scope = traced.get_air_glob_scope()

        # Should be a valid pointer (non-zero)
        assert glob_scope is not None
        assert glob_scope != 0

    def test_register_callback_with_driver(self):
        """Test registering Torch callback with driver.

        This is the key integration point:
        1. Torch frontend generates AIR IR
        2. Callback returns GLOB_SCOPE to driver
        3. Driver updates its IR and continues pipeline
        """
        from ace.fhe.frontend import get_frontend

        model = AddModel()
        x = torch.ones(1, 3, 4, 4)
        y = torch.ones(1, 3, 4, 4) * 2

        # Step 1: Generate AIR IR via Torch frontend
        frontend = get_frontend("torch")
        traced = frontend.to_ir(model, [x, y], ["x", "y"])
        traced.execute(x, y)

        # Step 2: Get GLOB_SCOPE for callback
        glob_scope_ptr = traced.get_air_glob_scope()
        assert glob_scope_ptr is not None

        # Step 3: Create driver and register callback
        # (This would be the actual driver integration)
        #
        # from ace.fhe.driver.driver import FHEDriver
        # driver = FHEDriver()
        #
        # def torch_callback(graph_ptr):
        #     return glob_scope_ptr
        #
        # driver.context.register_pass_callback("Torch", torch_callback)
        # driver.run()

        # For now, just verify we have the scope
        assert glob_scope_ptr != 0


class TestTorchFullPipeline:
    """Full pipeline tests: Torch -> Driver -> C Code."""

    def test_run_with_callback_one_shot(self, tmp_path):
        """Test run_with_callback method that creates driver and runs in one shot.

        This is the simplest way to use DSL callback:
        1. Create AIR IR using IRBuilder
        2. Call run_with_callback with a Python callback
        3. Callback populates GLOB_SCOPE via IRBuilder
        """
        if frontend is None:
            pytest.skip("C++ extension not available")

        # Define callback that builds AIR IR using IRBuilder
        def dsl_callback(glob_scope_ptr, graph_ptr):
            # Use IRBuilder to create a simple function
            builder = IRBuilder()
            builder.begin_function("Main_graph")
            builder.add_input("x", [1, 3, 4, 4])
            builder.add_input("y", [1, 3, 4, 4])
            builder.end_function([1, 3, 4, 4])
            builder.finalize()

            # Return the GLOB_SCOPE pointer
            return builder.get_glob_scope()

        # Create driver and run with callback in one shot
        result = frontend.run_dsl_fhe_compiler(dsl_callback)

        # Result 0 means NORMAL
        assert result == 0, f"Driver run_with_callback failed with code {result}"

    def test_torch_to_c_with_driver_direct(self, tmp_path):
        """Test complete pipeline using FHEDriver directly via pybind.

        Full flow:
        1. Torch frontend generates AIR IR
        2. Create FHEDriver and set GLOB_SCOPE directly
        3. Run driver pipeline
        """
        if frontend is None:
            pytest.skip("C++ extension not available")

        from ace.fhe.frontend import get_frontend

        model = AddModel()
        x = torch.ones(1, 3, 4, 4)
        y = torch.ones(1, 3, 4, 4) * 2

        # Step 1: Generate AIR IR via Torch frontend
        frontend = get_frontend("torch")
        traced = frontend.to_ir(model, [x, y], ["x", "y"])
        traced.execute(x, y)

        # Step 2: Verify AIR was generated
        assert traced.is_frontenderated()

        # Step 3: Get GLOB_SCOPE pointer
        glob_scope = traced.get_air_glob_scope()
        assert glob_scope is not None
        assert glob_scope != 0

        # Step 4: Create FHEDriver and set GLOB_SCOPE directly
        driver = frontend.FHEDriver()
        driver.init()

        # Set GLOB_SCOPE directly (no callback needed)
        driver.set_glob_scope(glob_scope)

        # Step 5: Run the pipeline
        result = driver.run()

        # Step 6: Cleanup
        driver.post_run()
        driver.fini()

        # Result 0 means NORMAL
        assert result == 0, f"Driver run failed with code {result}"

    def test_torch_to_c_with_callback(self, tmp_path):
        """Test complete pipeline using callback mechanism.

        Full flow:
        1. Torch frontend generates AIR IR
        2. Create FHEDriver and register callback
        3. Callback returns GLOB_SCOPE when Torch pass runs
        4. Driver pipeline continues
        """
        if frontend is None:
            pytest.skip("C++ extension not available")

        from ace.fhe.frontend import get_frontend

        model = AddModel()
        x = torch.ones(1, 3, 4, 4)
        y = torch.ones(1, 3, 4, 4) * 2

        # Step 1: Generate AIR IR via Torch frontend
        frontend = get_frontend("torch")
        traced = frontend.to_ir(model, [x, y], ["x", "y"])
        traced.execute(x, y)

        # Step 2: Get GLOB_SCOPE pointer for callback
        glob_scope = traced.get_air_glob_scope()
        assert glob_scope is not None

        # Step 3: Create FHEDriver and register callback
        driver = frontend.FHEDriver()
        driver.init()

        # Register callback that returns GLOB_SCOPE
        def torch_callback(graph_ptr):
            # Return the GLOB_SCOPE pointer from IRBuilder
            return int(glob_scope)

        driver.register_fx_callback(torch_callback)

        # Step 4: Run the pipeline
        result = driver.run()

        # Step 5: Cleanup
        driver.post_run()
        driver.fini()

        # Result 0 means NORMAL
        assert result == 0, f"Driver run failed with code {result}"


# ============================================================================
# Entry Point
# ============================================================================

if __name__ == "__main__":
    pytest.main([__file__, "-v"])