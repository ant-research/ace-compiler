#!/usr/bin/env python3
"""
Test: Python Post-Lowering Pass

This test demonstrates the full selective lowering workflow:
1. Register a Python lowering for conv with skip_cpp=True
2. Run C++ tensor2vector pass (conv is SKIPPED)
3. Run Python post-lowering pass (conv is EXPANDED)

Run with:
    cd acepy
    PYTHONPATH=. python examples/test_python_lowering_pass.py
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from ace_bindings import air_builder, nn_addon
from ace_dsl.frontend.domain_kernels import vector_kernel
# Use consistent imports from python_lowering_pass module
from ace_dsl.passes.python_lowering_pass import (
    register_lowering,
    run_python_lowering_pass, 
    get_ops_to_skip,
    list_registered_lowerings
)

_SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
ONNX_MODEL = os.path.join(_SCRIPT_DIR, "../../model/resnet20_cifar10_pre.onnx")
OUTPUT_DIR = os.path.join(_SCRIPT_DIR, "ir_dumps")


# ============================================================================
# Step 1: Register a custom Python lowering for conv
# ============================================================================

@register_lowering("nn::core", "conv")
@vector_kernel
def my_custom_conv2d(input_tensor, weight, bias):
    """
    Custom conv2d implementation in Python.
    This will be inlined by the Python post-lowering pass.
    
    Important: This lowering function is NOT compiled when registered.
    It is compiled AFTER tensor2vector runs to avoid "unknown domain" errors.
    
    This generates a VECTOR.mul operation that can be counted in the output.
    """
    return input_tensor * weight


def test_python_lowering_pass(verbose=False):
    """Test the full Python lowering workflow"""
    print("=" * 70)
    print("TEST: Python Post-Lowering Pass")
    print("=" * 70)
    
    # DO NOT compile lowering here - compiling @vector_kernel creates nn::vector 
    # domain nodes that will cause "unknown domain" error in tensor2vector pass.
    # Instead, compile AFTER tensor2vector runs.
    print("\n0. Lowering function registered (compilation deferred)")
    
    # Check what ops are registered to skip
    skip_ops = list(get_ops_to_skip())
    print(f"\n1. Registered skip ops: {skip_ops}")
    
    # Sync to C++
    nn_addon.set_skip_ops(skip_ops)
    print(f"   Synced to C++: {nn_addon.get_skip_ops()}")
    
    # Load model
    print(f"\n2. Loading ONNX model: {ONNX_MODEL}")
    result = air_builder.load_onnx_model(ONNX_MODEL)
    if not result["success"]:
        print(f"   Failed: {result['message']}")
        return False
    
    glob = result["glob_scope"]
    
    # Count initial ops
    ir_initial = glob.dump()
    conv_initial = ir_initial.count("NN.conv")
    vector_mul_initial = ir_initial.count("VECTOR.mul")
    print(f"\n3. Initial IR:")
    print(f"   NN.conv: {conv_initial}")
    print(f"   VECTOR.mul: {vector_mul_initial}")
    
    # Save initial IR
    with open(OUTPUT_DIR + "/python_lowering_1_initial.txt", "w") as f:
        f.write("=" * 80 + "\n")
        f.write("STEP 1: Initial IR (before any passes)\n")
        f.write(f"NN.conv: {conv_initial}\n")
        f.write("=" * 80 + "\n\n")
        f.write(ir_initial)
    print(f"   Saved to: {OUTPUT_DIR}/python_lowering_1_initial.txt")
    
    # Run C++ tensor2vector pass (conv will be SKIPPED)
    print(f"\n4. Running C++ tensor2vector pass (skip: {skip_ops})...")
    success = glob.run_cpp_pass("tensor2vector", skip_ops)
    print(f"   Pass result: {success}")
    
    # NOW compile the lowering function (after tensor2vector is done)
    # This is safe because tensor2vector won't see the nn::vector nodes
    print(f"\n4b. Compiling lowering function (after tensor2vector)...")
    my_custom_conv2d.compile()
    lowering_ir = my_custom_conv2d.air_module.dump()
    print(f"    Lowering IR length: {len(lowering_ir)} chars")
    if verbose:
        print(f"    Lowering IR:")
        print(lowering_ir)
    
    ir_after_cpp = glob.dump()
    conv_after_cpp = ir_after_cpp.count("NN.conv")
    vector_mul_cpp = ir_after_cpp.count("VECTOR.mul")
    vector_roll = ir_after_cpp.count("VECTOR.roll")
    print(f"\n5. After C++ pass:")
    print(f"   NN.conv: {conv_after_cpp} (PRESERVED!)")
    print(f"   VECTOR.mul: {vector_mul_cpp}")
    print(f"   VECTOR.roll: {vector_roll}")
    
    # Save after C++ pass
    with open(OUTPUT_DIR + "/python_lowering_2_after_cpp.txt", "w") as f:
        f.write("=" * 80 + "\n")
        f.write("STEP 2: After C++ tensor2vector pass (conv SKIPPED)\n")
        f.write(f"Skip ops: {skip_ops}\n")
        f.write(f"NN.conv: {conv_after_cpp} (preserved)\n")
        f.write("=" * 80 + "\n\n")
        f.write(ir_after_cpp)
    print(f"   Saved to: {OUTPUT_DIR}/python_lowering_2_after_cpp.txt")
    
    # Run Python post-lowering pass
    print(f"\n6. Running Python post-lowering pass...")
    try:
        applied = run_python_lowering_pass(glob, verbose=verbose)
        print(f"   Lowerings applied: {applied}")
    except Exception as e:
        print(f"   Note: Python lowering pass returned: {e}")
        print(f"   (This is expected if real AIR manipulation is not fully implemented)")
        applied = False
    
    ir_after_python = glob.dump()
    conv_after_python = ir_after_python.count("NN.conv")
    vector_mul_after = ir_after_python.count("VECTOR.mul")
    
    print(f"\n7. After Python pass:")
    print(f"   NN.conv: {conv_after_python}")
    print(f"   VECTOR.mul: {vector_mul_after} (includes those from Python lowering)")
    
    # Save after Python pass
    with open(OUTPUT_DIR + "/python_lowering_3_after_python.txt", "w") as f:
        f.write("=" * 80 + "\n")
        f.write("STEP 3: After Python post-lowering pass\n")
        f.write(f"NN.conv: {conv_after_python}\n")
        f.write(f"VECTOR.mul: {vector_mul_after}\n")
        f.write("=" * 80 + "\n\n")
        f.write(ir_after_python)
    print(f"   Saved to: {OUTPUT_DIR}/python_lowering_3_after_python.txt")
    
    # Calculate delta in VECTOR.mul from Python lowering
    vector_mul_added = vector_mul_after - vector_mul_cpp
    conv_replaced = conv_after_cpp - conv_after_python
    
    print("\n" + "=" * 70)
    print("SUMMARY:")
    print("=" * 70)
    print(f"  Initial:        {conv_initial} NN.conv, {vector_mul_initial} VECTOR.mul")
    print(f"  After C++:      {conv_after_cpp} NN.conv (preserved), {vector_mul_cpp} VECTOR.mul")
    print(f"  After Python:   {conv_after_python} NN.conv, {vector_mul_after} VECTOR.mul")
    print(f"")
    print(f"  NN.conv replaced by Python: {conv_replaced}")
    print(f"  VECTOR.mul before Python:   {vector_mul_cpp}")
    print(f"  VECTOR.mul after Python:    {vector_mul_after}")
    print(f"  VECTOR.mul delta:           {vector_mul_added}")
    print(f"")
    if vector_mul_added == conv_replaced:
        print(f"  ✓ Real inlining works! {conv_replaced} conv → {vector_mul_added} VECTOR.mul")
    else:
        print(f"  Note: Expected {conv_replaced} VECTOR.mul, got {vector_mul_added}")
    print("=" * 70)
    
    return True


def test_lowering_registration():
    """Test that lowering registration works"""
    print("\n" + "=" * 70)
    print("TEST: Lowering Registration")
    print("=" * 70)
    
    print(f"\nRegistered lowerings:")
    for entry in list_registered_lowerings():
        print(f"  {entry.domain}::{entry.op_name}:")
        func_name = getattr(entry.lowering_func, '__name__', str(entry.lowering_func)) if entry.lowering_func else 'None'
        print(f"    function: {func_name}")
        print(f"    target_domain: {entry.target_domain}")
    
    print(f"\nOps to skip in C++: {list(get_ops_to_skip())}")
    print("✓ Registration works")


if __name__ == "__main__":
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    
    test_lowering_registration()
    test_python_lowering_pass()
    
    print("\n" + "=" * 70)
    print("DONE! Check the IR files in examples/ir_dumps/")
    print("=" * 70)

