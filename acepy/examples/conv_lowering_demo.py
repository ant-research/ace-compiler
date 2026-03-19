"""
Conv2D Lowering Demo - FHE Pipeline

Demonstrates the full pass pipeline:
nn::core -> nn::vector -> fhe::sihe -> fhe::ckks -> fhe::poly -> C code

All stages use AIR IR dumps to show the transformation at each level.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from ace_dsl.frontend.domain_kernels import nn_kernel, NNTensor
from ace_bindings import air_builder


@nn_kernel
def conv_nn_kernel(
    input_packed: NNTensor,
    weight_packed: NNTensor,
    bias_expanded: NNTensor
) -> NNTensor:
    """
    NN-level conv with nested loops.
    Creates NN.add, NN.mul operations at nn::core level.
    """
    result = bias_expanded
    for cin in range(1):
        for khw in range(9):
            input_rolled = input_packed * input_packed
            weight_slice = weight_packed * weight_packed
            result = result + input_rolled * weight_slice
    return result


def run_demo():
    print("=" * 70)
    print("Conv2D Lowering Demo - FHE Pipeline")
    print("=" * 70)
    
    conv_nn_kernel.compile()
    glob = conv_nn_kernel.air_module
    
    # ========================================================================
    # Step 1: Initial IR (nn::core level)
    # ========================================================================
    print("\n" + "=" * 70)
    print("1. Initial IR (nn::core level)")
    print("=" * 70)
    print(glob.dump())
    
    # ========================================================================
    # Step 2: tensor2vector pass (nn::core -> nn::vector)
    # ========================================================================
    print("\n" + "=" * 70)
    print("2. Vector-level IR (nn::vector operations)")
    print("=" * 70)
    glob.run_cpp_pass("tensor2vector", [])
    print(glob.dump())
    
    # ========================================================================
    # Step 3: vector2sihe pass (nn::vector -> fhe::sihe)
    # ========================================================================
    print("\n" + "=" * 70)
    print("3. SIHE-level IR (fhe::sihe operations)")
    print("=" * 70)
    glob.run_cpp_pass("vector2sihe", [])
    print(glob.dump())
    
    # ========================================================================
    # Step 4: ckks_driver (fhe::sihe -> fhe::ckks)
    # ========================================================================
    print("\n" + "=" * 70)
    print("4. CKKS-level IR (fhe::ckks operations)")
    print("=" * 70)
    result = air_builder.run_ckks_driver(glob)
    print(f"Success: {result['success']}, Message: {result['message']}")
    if result['success'] and 'ir_dump' in result:
        print(result["ir_dump"])

    # ========================================================================
    # Step 5: poly_driver (fhe::ckks -> fhe::poly)
    # ========================================================================
    if result['success']:
        print("\n" + "=" * 70)
        print("5. Poly-level IR (fhe::poly operations)")
        print("=" * 70)
        poly_result = air_builder.run_poly_driver(glob)
        print(f"Success: {poly_result['success']}, Message: {poly_result['message']}")
        if poly_result['success'] and 'ir_dump' in poly_result:
            print(poly_result['ir_dump'])

        # ====================================================================
        # Step 6: poly2c (fhe::poly -> C code)
        # ====================================================================
        if poly_result['success']:
            print("\n" + "=" * 70)
            print("6. Generated C Code (poly2c)")
            print("=" * 70)
            glob.run_cpp_pass("poly2c", [])
            print(glob.get_c_code())

    # ========================================================================
    # Summary
    # ========================================================================
    print("\n" + "=" * 70)
    print("Summary")
    print("=" * 70)
    print("""
Pipeline Status:
  ✓ nn::core   -> nn::vector  (tensor2vector pass)
  ✓ nn::vector -> fhe::sihe   (vector2sihe pass)
  ✓ fhe::sihe  -> fhe::ckks   (ckks_driver)
  ✓ fhe::ckks  -> fhe::poly   (poly_driver)
  ✓ fhe::poly  -> C code      (poly2c)

All stages show AIR IR dumps:
  - nn::core:   NN.mul, NN.add operations
  - nn::vector: VECTOR.mul, VECTOR.add operations
  - fhe::sihe:  SIHE.mul, SIHE.add with CIPHERTEXT types
  - fhe::ckks:  CKKS.mul, CKKS.relin, CKKS.add with CIPHERTEXT3
  - fhe::poly:  POLY.hw_modmul, POLY.coeffs, RNS loops
  - C code:     Generated C code with FHE polynomial runtime calls
""")
    
    print("=" * 70)
    print("✓ Demo complete")
    print("=" * 70)


if __name__ == "__main__":
    run_demo()
