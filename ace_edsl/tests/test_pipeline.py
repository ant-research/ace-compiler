#!/usr/bin/env python3
"""
Test the AcePipeline class.
"""

import os
import sys
import pytest

def _setup_sys_path():
    repo_root = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
    parent_root = os.path.abspath(os.path.join(repo_root, ".."))
    for path in (repo_root, parent_root):
        if path not in sys.path:
            sys.path.insert(0, path)

_setup_sys_path()

try:
    from ace_edsl.edsl import nn_kernel, AceEDSL, AcePipeline, compile_to_c, FHEConfig
    IMPORTS_AVAILABLE = True
    IMPORT_ERROR = None
except ImportError as e:
    IMPORTS_AVAILABLE = False
    IMPORT_ERROR = str(e)


@pytest.mark.skipif(not IMPORTS_AVAILABLE, reason=f"Imports not available: {IMPORT_ERROR}")
def test_pipeline_class():
    """Test AcePipeline with nn_kernel."""
    @nn_kernel
    def simple_add(a, b):
        return a + b

    # Trigger AIR generation
    simple_add(None, None)

    # Get the AIR module
    dsl = AceEDSL._get_dsl()
    glob = dsl.current_air_module

    print("Testing AcePipeline class...")

    # Create pipeline with custom FHE config
    pipeline = AcePipeline(glob)
    pipeline.configure_fhe(
        scaling_factor_bits=56,
        first_prime_bits=60,
        hamming_weight=192,
        data_file="test_pipeline_data.msg",
    )

    # Run the full pipeline
    result = pipeline.run(verbose=True, dump_stages=True)

    assert result.success, f"Pipeline failed: {result.error}"
    assert result.c_code is not None, "No C code generated"
    assert len(result.c_code) > 0, "C code is empty"
    
    print(f"\n✓ Pipeline succeeded!")
    print(f"  Stages completed: {result.stages_completed}")
    print(f"  C code size: {len(result.c_code)} bytes")
    
    # Write C code to output
    out_dir = os.path.join(os.path.dirname(__file__), "output")
    os.makedirs(out_dir, exist_ok=True)
    out_path = os.path.join(out_dir, "pipeline_test_output.c")
    with open(out_path, "w") as f:
        f.write(result.c_code)
    print(f"  C code written to: {out_path}")


@pytest.mark.skipif(not IMPORTS_AVAILABLE, reason=f"Imports not available: {IMPORT_ERROR}")
def test_compile_to_c_function():
    """Test the compile_to_c convenience function."""
    @nn_kernel
    def mul_add(a, b):
        return (a * b) + a

    # Trigger AIR generation
    mul_add(None, None)

    # Get the AIR module
    dsl = AceEDSL._get_dsl()
    glob = dsl.current_air_module

    print("\nTesting compile_to_c() function...")

    out_dir = os.path.join(os.path.dirname(__file__), "output")
    os.makedirs(out_dir, exist_ok=True)
    out_path = os.path.join(out_dir, "compile_to_c_test.c")

    c_code = compile_to_c(
        glob,
        output_path=out_path,
        verbose=True,
    )

    assert c_code is not None, "compile_to_c() returned None"
    assert len(c_code) > 0, "compile_to_c() returned empty code"
    print(f"✓ compile_to_c() succeeded! ({len(c_code)} bytes)")


@pytest.mark.skipif(not IMPORTS_AVAILABLE, reason=f"Imports not available: {IMPORT_ERROR}")
def test_pipeline_with_fhe_config():
    """Test AcePipeline with FHEConfig dataclass."""
    @nn_kernel
    def fhe_test(x, y):
        return x + y

    fhe_test(None, None)
    dsl = AceEDSL._get_dsl()
    glob = dsl.current_air_module

    print("\nTesting AcePipeline with FHEConfig...")

    # Create FHEConfig directly
    config = FHEConfig(
        poly_degree=0,
        mul_level=0,
        scaling_factor_bits=56,
        first_prime_bits=60,
        hamming_weight=192,
    )

    pipeline = AcePipeline(glob, fhe_config=config)
    result = pipeline.run(verbose=False)

    assert result.success, f"FHEConfig test failed: {result.error}"
    print(f"✓ FHEConfig test passed! Stages: {result.stages_completed}")


if __name__ == "__main__":
    # When run directly (not via pytest), use traditional approach
    all_passed = True
    
    if not IMPORTS_AVAILABLE:
        print(f"⚠ Imports not available: {IMPORT_ERROR}")
        sys.exit(1)
    
    try:
        test_pipeline_class()
    except AssertionError as e:
        print(f"✗ test_pipeline_class failed: {e}")
        all_passed = False
    
    try:
        test_compile_to_c_function()
    except AssertionError as e:
        print(f"✗ test_compile_to_c_function failed: {e}")
        all_passed = False
    
    try:
        test_pipeline_with_fhe_config()
    except AssertionError as e:
        print(f"✗ test_pipeline_with_fhe_config failed: {e}")
        all_passed = False
    
    print("\n" + "=" * 50)
    if all_passed:
        print("✓ All pipeline tests passed!")
    else:
        print("✗ Some tests failed")
    
    sys.exit(0 if all_passed else 1)
