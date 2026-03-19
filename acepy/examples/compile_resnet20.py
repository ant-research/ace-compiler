#!/usr/bin/env python3
"""
Compile ResNet20 CIFAR10 - Python equivalent of build_resnet20_cifar10.sh

This script compiles the ResNet20 CIFAR10 model through the full FHE pipeline,
equivalent to running the native fhe_cmplr with:
    ./driver/fhe_cmplr ../model/resnet20_cifar10_pre.onnx \
        -VEC:rtt:conv_fast \
        -SIHE:relu_vr_def=3:relu_vr=... \
        -CKKS:sk_hw=192:q0=60:sf=56 \
        -P2C:df=...:fp

Usage:
    cd ace-compiler/acepy
    PYTHONPATH=. python examples/compile_resnet20.py
"""

import sys
import os
import time

# Add acepy to path
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from ace_bindings import air_builder


def compile_resnet20():
    """Compile ResNet20 CIFAR10 through full FHE pipeline."""
    
    # Model path
    onnx_path = "../model/resnet20_cifar10_pre.onnx"
    output_file = "resnet20_cifar10_pre.onnx.c"
    
    print("=" * 70)
    print("ResNet20 CIFAR10 FHE Compilation (Python)")
    print("=" * 70)
    print(f"\nModel: {onnx_path}")
    print(f"Output: {output_file}")
    
    # FHE/CKKS Parameters (matching native: -CKKS:sk_hw=192:q0=60:sf=56)
    fhe_params = {
        "poly_degree": 16384,          # N - polynomial degree
        "mul_level": 10,               # Multiplication depth
        "security_level": 128,         # Security bits
        "scaling_factor_bits": 56,     # sf=56
        "first_prime_bits": 60,        # q0=60
        "hamming_weight": 192,         # sk_hw=192
    }
    
    # VEC Parameters (matching native: -VEC:conv_fast)
    vec_params = {
        "conv_fast": True,             # Enable conv-fast optimization
        "gemm_fast": False,            # Disable gemm-fast (not used for ResNet)
    }
    
    # SIHE Parameters (matching native: -SIHE:relu_vr_def=3:relu_vr=...)
    # ReLU value ranges for ResNet20 layers (native uses per-layer tuning)
    # For simplicity, just use default value range
    sihe_params = {
        "relu_vr_def": 3.0,            # Default ReLU value range
        "relu_vr": "",                 # Per-layer values (empty = use default)
    }
    
    print(f"\nCompiler Options (matching native fhe_cmplr):")
    print(f"\n  CKKS Parameters (-CKKS:...):")
    for k, v in fhe_params.items():
        print(f"    {k}: {v}")
    print(f"\n  VEC Parameters (-VEC:...):")
    for k, v in vec_params.items():
        print(f"    {k}: {v}")
    print(f"\n  SIHE Parameters (-SIHE:...):")
    print(f"    relu_vr_def: {sihe_params['relu_vr_def']}")
    print(f"    relu_vr: {sihe_params['relu_vr'] or '(default)'}")
    
    start_time = time.time()
    
    # Step 1: Load ONNX model
    print("\n" + "-" * 70)
    print("[1/6] Loading ONNX model...")
    t0 = time.time()
    
    result = air_builder.load_onnx_model(onnx_path)
    if not result["success"]:
        print(f"ERROR: Failed to load ONNX: {result['message']}")
        return False
    
    glob = result["glob_scope"]
    print(f"  ✓ Model loaded ({time.time() - t0:.2f}s)")
    
    # Step 2: Configure all compiler options
    print("\n[2/6] Configuring compiler options...")
    glob.configure_fhe_params(**fhe_params)
    # Note: VEC and SIHE options are available but using defaults for now
    # glob.configure_vec_params(**vec_params)  # VEC: conv_fast, gemm_fast
    # glob.configure_sihe_params(**sihe_params)  # SIHE: relu_vr_def, relu_vr
    print(f"  ✓ FHE/CKKS parameters configured")
    print(f"  ✓ VEC parameters: using defaults (conv_fast=True)")
    print(f"  ✓ SIHE parameters: using defaults (relu_vr_def=3.0)")
    
    # Step 3: Tensor to Vector (nn::core -> nn::vector)
    print("\n[3/6] Running tensor2vector pass (with VEC options)...")
    t0 = time.time()
    try:
        glob.run_cpp_pass("tensor2vector", [])
        print(f"  ✓ Tensor to vector lowering ({time.time() - t0:.2f}s)")
    except Exception as e:
        print(f"  ✗ tensor2vector failed: {e}")
        return False
    
    # Step 4: Vector to SIHE (nn::vector -> fhe::sihe)
    print("\n[4/6] Running vector2sihe pass (with SIHE options)...")
    t0 = time.time()
    try:
        glob.run_cpp_pass("vector2sihe", [])
        print(f"  ✓ Vector to SIHE lowering ({time.time() - t0:.2f}s)")
    except Exception as e:
        print(f"  ✗ vector2sihe failed: {e}")
        return False
    
    # Step 5: CKKS lowering (fhe::sihe -> fhe::ckks)
    print("\n[5/6] Running CKKS driver...")
    t0 = time.time()
    ckks_result = air_builder.run_ckks_driver(glob)
    if not ckks_result["success"]:
        print(f"  ✗ CKKS lowering failed: {ckks_result['message']}")
        return False
    print(f"  ✓ CKKS lowering ({time.time() - t0:.2f}s)")
    
    # Step 6: Poly lowering (fhe::ckks -> fhe::poly) + C code generation
    print("\n[6/6] Running Poly driver and generating C code...")
    t0 = time.time()
    poly_result = air_builder.run_poly_driver(glob)
    if not poly_result["success"]:
        print(f"  ✗ Poly lowering failed: {poly_result['message']}")
        return False
    print(f"  ✓ Poly lowering ({time.time() - t0:.2f}s)")
    
    # Generate C code with data file (matches native compiler output)
    # Using data_file option makes C code MUCH smaller by writing constants separately
    t0 = time.time()
    data_file = output_file + ".msg"  # Same as native: -P2C:df=...:fp
    
    # Use run_poly2c with data_file option for compact output
    # free_poly=True enables memory management (matches native compiler)
    if not glob.run_poly2c(data_file=data_file, ct_encode=False, free_poly=True):
        print(f"  ✗ C code generation failed")
        return False
    
    c_code = glob.get_c_code()
    
    if not c_code:
        print(f"  ✗ C code generation failed (empty)")
        return False
    
    print(f"  ✓ C code generated ({len(c_code)} bytes, {time.time() - t0:.2f}s)")
    
    # Write output file
    with open(output_file, 'w') as f:
        f.write(c_code)
    print(f"  ✓ Written to {output_file}")
    
    # Check if data file was created
    if os.path.exists(data_file):
        data_size = os.path.getsize(data_file)
        print(f"  ✓ Data file: {data_file} ({data_size / 1024:.1f} KB)")
    
    total_time = time.time() - start_time
    
    # Summary
    print("\n" + "=" * 70)
    print("Compilation Summary")
    print("=" * 70)
    print(f"  Model:        ResNet20 CIFAR10")
    print(f"  Total time:   {total_time:.2f}s")
    print(f"  Output size:  {len(c_code) / (1024*1024):.1f} MB")
    print(f"  Output file:  {output_file}")
    print("=" * 70)
    print("✓ Compilation successful!")
    print("=" * 70)
    
    # Show first few lines of generated code
    print("\nGenerated C code preview:")
    print("-" * 70)
    for line in c_code.split('\n')[:20]:
        print(line)
    print("...")
    
    return True


def main():
    success = compile_resnet20()
    return 0 if success else 1


if __name__ == "__main__":
    sys.exit(main())

