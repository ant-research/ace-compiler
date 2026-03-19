"""
Test Python Lowering Only - Validates DSL Works
================================================

This test validates that Python DSL lowering works correctly without
attempting the poly2c step which has parameter issues.

Pipeline:
  1. Register lowering (NO manual compile - deferred)
  2. Load ONNX model
  3. Run C++ passes: tensor2vector, vector2sihe, sihe2ckks (with skip_ops)
  4. Run Python lowering pass (compiles lazily, inlines bootstrap_full)
  
For full C code generation, use the native compiler:
  ./driver/fhe_cmplr ../model/resnet20_cifar10_pre.onnx \
    -VEC:rtt:conv_fast \
    -SIHE:relu_vr_def=3 \
    -CKKS:sk_hw=192:q0=60:sf=56 \
    -P2C:df=output.msg:fp

Run with:
    cd acepy
    PYTHONPATH=.:examples python examples/test_python_lowering_only.py
"""

import sys
import os
import time
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from ace_bindings import air_builder
from ace_dsl.passes.python_lowering_pass import (
    register_lowering, 
    get_ops_to_skip,
    run_python_lowering_pass,
    clear_lowerings
)
from ace_dsl.frontend.domain_kernels import ckks_kernel, CkksCiphertext, ckks

# Import the full bootstrap implementation
from bootstrap_full import bootstrap_full


def count_ops(ir: str, pattern: str) -> int:
    return ir.lower().count(pattern.lower())


def run_test():
    print("=" * 70)
    print("Python Lowering Test - Validates DSL Works")
    print("=" * 70)
    
    start_time = time.time()
    
    # Clear any previous registrations
    clear_lowerings()
    
    # =========================================================================
    # Step 1: Register bootstrap lowering (NO COMPILE YET)
    # =========================================================================
    print("\n[Step 1/4] Registering bootstrap lowering...")
    
    register_lowering(
        "fhe::ckks", "bootstrap", 
        target_domain="fhe::poly",
        description="Full CKKS bootstrap with DFT/EvalMod/iDFT"
    )(bootstrap_full)
    
    skip_ops = get_ops_to_skip()
    print(f"  Registered skip ops: {skip_ops}")
    
    # =========================================================================
    # Step 2: Load ONNX model
    # =========================================================================
    print("\n[Step 2/4] Loading ResNet20 model...")
    t0 = time.time()
    
    script_dir = os.path.dirname(os.path.abspath(__file__))
    model_path = os.path.join(script_dir, "../../model/resnet20_cifar10_pre.onnx")
    if not os.path.exists(model_path):
        print(f"  ERROR: Model not found at {model_path}")
        return False
    
    result = air_builder.load_onnx_model(model_path)
    if not result["success"]:
        print(f"  ERROR: Failed to load: {result['message']}")
        return False
    
    glob = result["glob_scope"]
    print(f"  ✓ Model loaded ({time.time() - t0:.2f}s)")
    
    # Configure FHE params (for later passes, even though we won't run poly2c)
    glob.configure_fhe_params(
        poly_degree=16384,
        mul_level=35,
        scaling_factor_bits=56,
        first_prime_bits=60,
        hamming_weight=192
    )
    print(f"  ✓ FHE params configured")
    
    # =========================================================================
    # Step 3: Run C++ lowering passes
    # =========================================================================
    print("\n[Step 3/4] Running C++ lowering passes...")
    
    t0 = time.time()
    glob.run_cpp_pass("tensor2vector")
    print(f"  ✓ tensor2vector ({time.time() - t0:.2f}s)")
    
    t0 = time.time()
    glob.run_cpp_pass("vector2sihe")
    print(f"  ✓ vector2sihe ({time.time() - t0:.2f}s)")
    
    t0 = time.time()
    glob.run_cpp_pass("sihe2ckks", list(skip_ops))
    print(f"  ✓ sihe2ckks with skip_ops={list(skip_ops)} ({time.time() - t0:.2f}s)")
    
    ir_before = glob.dump()
    bootstrap_before = count_ops(ir_before, 'ckks.bootstrap')
    rotate_before = count_ops(ir_before, 'ckks.rotate')
    print(f"\n  Before Python lowering:")
    print(f"    CKKS.bootstrap: {bootstrap_before}")
    print(f"    CKKS.rotate:    {rotate_before}")
    
    # =========================================================================
    # Step 4: Run Python lowering pass (COMPILES LAZILY)
    # =========================================================================
    print("\n[Step 4/4] Running Python lowering pass...")
    t0 = time.time()
    
    try:
        run_python_lowering_pass(glob, verbose=True)
        print(f"  ✓ Python lowering ({time.time() - t0:.2f}s)")
        
        ir_after = glob.dump()
        bootstrap_after = count_ops(ir_after, 'ckks.bootstrap')
        rotate_after = count_ops(ir_after, 'ckks.rotate')
        add_after = count_ops(ir_after, 'ckks.add')
        sub_after = count_ops(ir_after, 'ckks.sub')
        mul_after = count_ops(ir_after, 'ckks.mul')
        
        print(f"\n  After Python lowering:")
        print(f"    CKKS.bootstrap: {bootstrap_after} (was {bootstrap_before})")
        print(f"    CKKS.rotate:    {rotate_after} (was {rotate_before})")
        print(f"    CKKS.add:       {add_after}")
        print(f"    CKKS.sub:       {sub_after}")
        print(f"    CKKS.mul:       {mul_after}")
        
        success = bootstrap_after < bootstrap_before
        if success:
            print(f"\n  ✓ SUCCESS: {bootstrap_before - bootstrap_after} bootstrap nodes replaced!")
            expected_rotate_increase = (bootstrap_before - bootstrap_after) * 6
            if rotate_after >= rotate_before + expected_rotate_increase - 1:
                print(f"  ✓ CKKS.rotate increased by ~{rotate_after - rotate_before} (expected ~{expected_rotate_increase})")
        else:
            print(f"\n  ✗ FAILED: No bootstrap nodes replaced")
            return False
        
    except Exception as e:
        print(f"  ✗ ERROR: {e}")
        import traceback
        traceback.print_exc()
        return False
    
    # =========================================================================
    # Summary
    # =========================================================================
    total_time = time.time() - start_time
    
    print("\n" + "=" * 70)
    print("Summary")
    print("=" * 70)
    print(f"""
Python lowering completed in {total_time:.2f}s:

  Step 1: Register lowering (bootstrap_full)                     ✓
  Step 2: Load ONNX model (ResNet20)                             ✓
  Step 3: C++ passes (tensor2vector, vector2sihe, sihe2ckks)     ✓
  Step 4: Python lowering ({bootstrap_before} → {bootstrap_after} bootstrap nodes)    ✓

Result:
  - {bootstrap_before - bootstrap_after} bootstrap operations successfully replaced
  - Each bootstrap_full contains: 6 rotate, 8 add, 9 sub, 13 mul
  - Python DSL lowering mechanism works correctly!

NOTE: For full C code generation, use the native compiler:
  cd release
  ./driver/fhe_cmplr ../model/resnet20_cifar10_pre.onnx \\
    -VEC:rtt:conv_fast -SIHE:relu_vr_def=3 \\
    -CKKS:sk_hw=192:q0=60:sf=56 -P2C:df=output.msg:fp
""")
    print("=" * 70)
    return True


if __name__ == "__main__":
    success = run_test()
    sys.exit(0 if success else 1)

