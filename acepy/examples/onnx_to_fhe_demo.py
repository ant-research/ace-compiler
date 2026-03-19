#!/usr/bin/env python3
"""
ONNX to FHE Demo
================

This example demonstrates loading an ONNX model and compiling it through
the full FHE pipeline using Python bindings.

Usage:
    cd ace-compiler/acepy
    PYTHONPATH=. python examples/onnx_to_fhe_demo.py [path/to/model.onnx]

If no model path is provided, it will try to load the ResNet20 CIFAR10 model.
"""

import sys
import os

# Add acepy to path
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from ace_bindings import air_builder


def main():
    # Default to ResNet20 CIFAR10 model
    if len(sys.argv) > 1:
        onnx_path = sys.argv[1]
    else:
        onnx_path = "../model/resnet20_cifar10_pre.onnx"
    
    print("=" * 70)
    print("ONNX to FHE Demo")
    print("=" * 70)
    print(f"\nLoading ONNX model: {onnx_path}")
    
    # Step 1: Load ONNX model
    print("\n[Step 1] Loading ONNX model...")
    result = air_builder.load_onnx_model(onnx_path)
    
    if not result["success"]:
        print(f"ERROR: Failed to load ONNX model: {result['message']}")
        return 1
    
    glob = result["glob_scope"]
    print(f"  ✓ Model loaded successfully")
    print(f"  ✓ IR at nn::core level")
    
    # Show first few lines of IR
    ir_lines = result["ir_dump"].split("\n")
    print(f"\n  First 10 lines of IR:")
    for line in ir_lines[:10]:
        print(f"    {line}")
    print("    ...")
    
    # Step 2: Run tensor2vector pass (nn::core -> nn::vector)
    print("\n[Step 2] Running tensor2vector pass (nn::core -> nn::vector)...")
    try:
        glob.run_cpp_pass("tensor2vector", [])
        print(f"  ✓ Tensor to vector lowering complete")
    except Exception as e:
        print(f"  ✗ tensor2vector failed: {e}")
        # Continue anyway - some models may not need this pass
    
    # Step 3: Run vector2sihe pass (nn::vector -> fhe::sihe)
    print("\n[Step 3] Running vector2sihe pass (nn::vector -> fhe::sihe)...")
    try:
        glob.run_cpp_pass("vector2sihe", [])
        print(f"  ✓ Vector to SIHE lowering complete")
    except Exception as e:
        print(f"  ✗ vector2sihe failed: {e}")
    
    # Step 4: Run CKKS driver (fhe::sihe -> fhe::ckks)
    print("\n[Step 4] Running CKKS driver (fhe::sihe -> fhe::ckks)...")
    ckks_result = air_builder.run_ckks_driver(glob)
    
    if ckks_result["success"]:
        print(f"  ✓ CKKS lowering successful")
    else:
        print(f"  ✗ CKKS lowering failed: {ckks_result['message']}")
        print("  Note: CKKS lowering requires proper type registration")
    
    # Step 5: Run Poly driver (fhe::ckks -> fhe::poly)
    print("\n[Step 5] Running Poly driver (fhe::ckks -> fhe::poly)...")
    poly_result = air_builder.run_poly_driver(glob)
    
    if poly_result["success"]:
        print(f"  ✓ Poly lowering successful")
    else:
        print(f"  ✗ Poly lowering failed: {poly_result['message']}")
    
    # Step 6: Generate C code
    print("\n[Step 6] Generating C code...")
    try:
        glob.run_cpp_pass("poly2c", [])
        c_code = glob.get_c_code()
        if c_code:
            print(f"  ✓ C code generated ({len(c_code)} bytes)")
            print(f"\n  First 500 characters of C code:")
            print("  " + "-" * 50)
            for line in c_code[:500].split("\n"):
                print(f"    {line}")
            print("    ...")
        else:
            print(f"  ✗ No C code generated")
    except Exception as e:
        print(f"  ✗ poly2c failed: {e}")
    
    # Summary
    print("\n" + "=" * 70)
    print("Pipeline Summary:")
    print("=" * 70)
    print(f"  ONNX Loading:    {'✓' if result['success'] else '✗'}")
    print(f"  CKKS Lowering:   {'✓' if ckks_result.get('success') else '✗'}")
    print(f"  Poly Lowering:   {'✓' if poly_result.get('success') else '✗'}")
    print("=" * 70)
    
    return 0


if __name__ == "__main__":
    sys.exit(main())

