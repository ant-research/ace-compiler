#!/usr/bin/env python3
"""
Test: Selective Conv Lowering with Pipeline
============================================

This test demonstrates selective lowering where:
- NN.conv operations are SKIPPED by the C++ tensor2vector pass
- NN.conv nodes are PRESERVED for Python to handle later

Uses the Pipeline class for clean, chainable API.

Output files saved to: examples/output/selective_conv_*/

Run with:
    cd acepy
    PYTHONPATH=. python examples/test_selective_conv.py
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from ace_bindings import nn_addon
from dump_utils import Pipeline, PipelineTarget


_SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
ONNX_MODEL = os.path.join(_SCRIPT_DIR, "../../model/resnet20_cifar10_pre.onnx")


def count_ops(ir: str, pattern: str) -> int:
    """Count occurrences of an operation in IR."""
    return ir.lower().count(pattern.lower())


def test_no_skip():
    """Test 1: Normal lowering - all conv ops are lowered by C++"""
    print("=" * 70)
    print("TEST 1: No Skip Ops (Normal Lowering) - Full Pipeline")
    print("=" * 70)
    
    # Clear any skip ops
    nn_addon.clear_skip_ops()
    print(f"Skip ops: {nn_addon.get_skip_ops()}")
    
    # Create pipeline and run
    pipeline = Pipeline("selective_conv_test1_no_skip", verbose=True)
    
    result = (pipeline
        .load_onnx(ONNX_MODEL)
        .configure_fhe(
            scaling_factor_bits=56,
            first_prime_bits=60,
            hamming_weight=192,
            conv_fast=True,
            gemm_fast=False,
            relu_vr_def=3.0,
        )
        .run(target=PipelineTarget.C)
    )
    
    # Check results
    if not result.success:
        print(f"\n✗ Pipeline failed: {result.error}")
        return False
    
    # Verify all conv ops were lowered
    final_ir = pipeline.get_ir()
    conv_after = count_ops(final_ir, "NN.conv")
    
    if conv_after == 0:
        print(f"\n✓ TEST 1 PASSED: All conv ops lowered (C code: {len(result.c_code):,} bytes)")
        pipeline.print_summary()
        return True
    else:
        print(f"\n✗ TEST 1 FAILED: {conv_after} NN.conv ops remain")
        return False


def test_skip_conv():
    """Test 2: Selective lowering - conv ops are SKIPPED"""
    print("\n" + "=" * 70)
    print("TEST 2: Skip nn::core::conv (Selective Lowering)")
    print("=" * 70)
    
    # Set skip ops to skip conv
    nn_addon.clear_skip_ops()
    nn_addon.add_skip_op("nn::core::conv")
    print(f"Skip ops: {nn_addon.get_skip_ops()}")
    
    # Create pipeline - only run up to tensor2vector to demonstrate skip
    pipeline = Pipeline("selective_conv_test2_skip_conv", verbose=True)
    
    # Load and get initial count
    pipeline.load_onnx(ONNX_MODEL)
    initial_ir = pipeline.get_ir()
    conv_before = count_ops(initial_ir, "NN.conv")
    print(f"\nBefore tensor2vector: NN.conv = {conv_before}")
    
    # Run only tensor2vector with skip
    result = (pipeline
        .configure_fhe(conv_fast=True, gemm_fast=False, relu_vr_def=3.0)
        .set_skip_ops(["nn::core::conv"])
        .run(target=PipelineTarget.VECTOR)  # Stop at vector level
    )
    
    if not result.success:
        print(f"\n✗ Pipeline failed: {result.error}")
        return False
    
    # Check that conv ops are preserved
    final_ir = pipeline.get_ir()
    conv_after = count_ops(final_ir, "NN.conv")
    vector_roll = count_ops(final_ir, "VECTOR.roll")
    
    print(f"\nAfter tensor2vector (with skip conv):")
    print(f"  NN.conv: {conv_after} (was {conv_before}, should be PRESERVED)")
    print(f"  VECTOR.roll: {vector_roll} (from other ops)")
    
    if conv_after == conv_before:
        print(f"\n✓ TEST 2 PASSED: Conv ops preserved ({conv_after} NN.conv)")
        pipeline.print_summary()
        return True
    else:
        print(f"\n✗ TEST 2 FAILED: Expected {conv_before} NN.conv, got {conv_after}")
        return False


def test_skip_multiple():
    """Test 3: Skip multiple ops"""
    print("\n" + "=" * 70)
    print("TEST 3: Skip Multiple Ops (conv and relu)")
    print("=" * 70)
    
    # Set multiple skip ops
    nn_addon.clear_skip_ops()
    nn_addon.add_skip_op("nn::core::conv")
    nn_addon.add_skip_op("nn::core::relu")
    print(f"Skip ops: {nn_addon.get_skip_ops()}")
    
    # Create pipeline
    pipeline = Pipeline("selective_conv_test3_skip_multi", verbose=True)
    
    # Load and get initial counts
    pipeline.load_onnx(ONNX_MODEL)
    initial_ir = pipeline.get_ir()
    conv_before = count_ops(initial_ir, "NN.conv")
    relu_before = count_ops(initial_ir, "NN.relu")
    print(f"\nBefore tensor2vector:")
    print(f"  NN.conv: {conv_before}")
    print(f"  NN.relu: {relu_before}")
    
    # Run with skip_ops
    result = (pipeline
        .set_skip_ops(["nn::core::conv", "nn::core::relu"])
        .run(target=PipelineTarget.VECTOR)
    )
    
    if not result.success:
        print(f"\n✗ Pipeline failed: {result.error}")
        return False
    
    # Check that both ops are preserved
    final_ir = pipeline.get_ir()
    conv_after = count_ops(final_ir, "NN.conv")
    relu_after = count_ops(final_ir, "NN.relu")
    
    print(f"\nAfter tensor2vector (skip conv and relu):")
    print(f"  NN.conv: {conv_after} (was {conv_before})")
    print(f"  NN.relu: {relu_after} (was {relu_before})")
    
    if conv_after == conv_before and relu_after == relu_before:
        print(f"\n✓ TEST 3 PASSED: Multiple ops preserved")
        pipeline.print_summary()
        return True
    else:
        print(f"\n✗ TEST 3 FAILED: Ops not preserved correctly")
        return False


def main():
    print("\n" + "=" * 70)
    print("Selective Lowering Tests with Pipeline")
    print("=" * 70)
    print(f"\nModel: {ONNX_MODEL}")
    
    all_passed = True
    
    try:
        if not test_no_skip():
            all_passed = False
    except Exception as e:
        print(f"\n✗ TEST 1 FAILED: {e}")
        all_passed = False
    
    try:
        if not test_skip_conv():
            all_passed = False
    except Exception as e:
        print(f"\n✗ TEST 2 FAILED: {e}")
        all_passed = False
    
    try:
        if not test_skip_multiple():
            all_passed = False
    except Exception as e:
        print(f"\n✗ TEST 3 FAILED: {e}")
        all_passed = False
    
    # Clean up
    nn_addon.clear_skip_ops()
    
    print("\n" + "=" * 70)
    if all_passed:
        print("ALL TESTS PASSED")
    else:
        print("SOME TESTS FAILED")
    print("=" * 70)
    
    return all_passed


if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)
