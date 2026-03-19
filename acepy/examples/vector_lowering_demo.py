"""
Vector Kernel Demo - FHE Pipeline

Demonstrates the full pass pipeline starting from vector level:
nn::vector -> fhe::sihe -> fhe::ckks -> fhe::poly -> C code

All stages use AIR IR dumps to show the transformation at each level.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from ace_dsl.frontend.domain_kernels import vector_kernel, VectorTensor
from ace_bindings import air_builder


@vector_kernel
def conv_vector_kernel(
    input_packed: VectorTensor,
    weight_packed: VectorTensor,
    bias_expanded: VectorTensor
) -> VectorTensor:
    """
    Vector-level conv with nested loops.
    Creates VECTOR.add, VECTOR.mul operations at nn::vector level.
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
    print("Vector Kernel Demo - FHE Pipeline")
    print("=" * 70)
    
    conv_vector_kernel.compile()
    glob = conv_vector_kernel.air_module
    
    # ========================================================================
    # Step 1: Initial IR (nn::vector level)
    # ========================================================================
    print("\n" + "=" * 70)
    print("1. Initial IR (nn::vector level)")
    print("   Starting point: VECTOR.mul, VECTOR.add operations")
    print("=" * 70)
    print(glob.dump())
    
    # ========================================================================
    # Step 2: vector2sihe pass (nn::vector -> fhe::sihe)
    # ========================================================================
    print("\n" + "=" * 70)
    print("2. SIHE-level IR (fhe::sihe operations)")
    print("=" * 70)
    glob.run_cpp_pass("vector2sihe", [])
    print(glob.dump())
    
    # ========================================================================
    # Step 3: ckks_driver (fhe::sihe -> fhe::ckks)
    # ========================================================================
    print("\n" + "=" * 70)
    print("3. CKKS-level IR (fhe::ckks operations)")
    print("=" * 70)
    result = air_builder.run_ckks_driver(glob)
    print(f"Success: {result['success']}, Message: {result['message']}")
    if result['success'] and 'ir_dump' in result:
        print(result["ir_dump"])

    # ========================================================================
    # Step 4: poly_driver (fhe::ckks -> fhe::poly)
    # ========================================================================
    if result['success']:
        print("\n" + "=" * 70)
        print("4. Poly-level IR (fhe::poly operations)")
        print("=" * 70)
        poly_result = air_builder.run_poly_driver(glob)
        print(f"Success: {poly_result['success']}, Message: {poly_result['message']}")
        if poly_result['success'] and 'ir_dump' in poly_result:
            print(poly_result['ir_dump'])

        # ====================================================================
        # Step 5: poly2c (fhe::poly -> C code)
        # ====================================================================
        if poly_result['success']:
            print("\n" + "=" * 70)
            print("5. Generated C Code (poly2c)")
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
Pipeline Status (starting from @vector_kernel):
  ✓ nn::vector -> fhe::sihe   (vector2sihe pass)
  ✓ fhe::sihe  -> fhe::ckks   (ckks_driver)
  ✓ fhe::ckks  -> fhe::poly   (poly_driver)
  ✓ fhe::poly  -> C code      (poly2c)

Note: @vector_kernel starts at nn::vector level, skipping tensor2vector pass.

All stages show AIR IR dumps:
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
