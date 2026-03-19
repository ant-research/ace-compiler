#!/usr/bin/env python3
"""
Real tensor2vector Pass Example

This example runs the ACTUAL C++ tensor2vector pass from libNNvector
and shows the AIR dump from actual ACE compiler.

Run with:
    cd acepy
    PYTHONPATH=. python examples/real_tensor2vector.py
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from ace_bindings import air_builder, nn_addon

_SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
ONNX_MODEL = os.path.join(_SCRIPT_DIR, "../../model/resnet20_cifar10_pre.onnx")


def main():
    print("=" * 70)
    print("tensor2vector Pass")
    print("=" * 70)
    
    # 1. Clear skip ops
    nn_addon.clear_skip_ops()
    print(f"\nSkip ops: {nn_addon.get_skip_ops()}")
    
    # 2. Load real ONNX model
    print(f"\n1. Loading ONNX model: {ONNX_MODEL}")
    result = air_builder.load_onnx_model(ONNX_MODEL)
    
    if not result["success"]:
        print(f"   Failed: {result['message']}")
        return
    
    print(f"   Success: {result['message']}")
    glob = result["glob_scope"]
    
    # 3. Print initial IR (nn::core level)
    print("\n2. Initial IR (nn::core level) - first 1500 chars:")
    print("-" * 70)
    initial_ir = glob.dump()
    print(initial_ir[:1500])
    print("...")
    print("-" * 70)
    
    # Count ops before
    nn_conv_before = initial_ir.count("NN.conv")
    nn_relu_before = initial_ir.count("NN.relu")
    print(f"\n   NN.conv count: {nn_conv_before}")
    print(f"   NN.relu count: {nn_relu_before}")
    
    # 4. Run the REAL tensor2vector pass
    print("\n3. Running REAL tensor2vector pass...")
    pass_result = glob.run_cpp_pass("tensor2vector", [])
    print(f"   Pass result: {pass_result}")
    
    # 5. Print transformed IR (nn::vector level)
    print("\n4. Transformed IR (nn::vector level) - first 2000 chars:")
    print("-" * 70)
    transformed_ir = glob.dump()
    print(transformed_ir[:2000])
    print("...")
    print("-" * 70)
    
    # Count ops after
    nn_conv_after = transformed_ir.count("NN.conv")
    vector_roll = transformed_ir.count("VECTOR.roll")
    vector_slice = transformed_ir.count("VECTOR.slice")
    vector_mul = transformed_ir.count("VECTOR.mul")
    vector_add = transformed_ir.count("VECTOR.add")
    
    print(f"\n   NN.conv count: {nn_conv_after} (was {nn_conv_before})")
    print(f"   VECTOR.roll count: {vector_roll}")
    print(f"   VECTOR.slice count: {vector_slice}")
    print(f"   VECTOR.mul count: {vector_mul}")
    print(f"   VECTOR.add count: {vector_add}")
    
    print("\n" + "=" * 70)
    print("Generated AIR from the ACE compiler")
    print("=" * 70)


if __name__ == "__main__":
    main()
