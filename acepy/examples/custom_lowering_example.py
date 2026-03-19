"""
Custom Lowering Example

This example shows how to:
1. Define a custom op lowering in Python
2. Register it so C++ skips lowering
3. Use Python pass to apply the lowering

This approach allows users to customize lowerings without modifying C++ code.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from ace_dsl.frontend.domain_kernels import (
    vector_kernel, nn_kernel, 
    VectorTensor, NNTensor
)
from ace_dsl.passes import (
    register_lowering, 
    list_registered_lowerings,
    get_ops_to_skip,
    run_python_lowering_pass,
)


# ============================================================================
# Step 1: Define your vector lowering implementation
# ============================================================================

@register_lowering("nn::core", "conv", description="Custom conv2d lowering")
@vector_kernel
def conv2d_lowering(
    input: VectorTensor,
    weight: VectorTensor,
    bias: VectorTensor
) -> VectorTensor:
    """
    Vector implementation of conv2d.
    
    This is what the user writes to define HOW conv should be lowered.
    The nested loops generate the correct AIR structure.
    """
    # Initialize result with bias
    result = bias
    
    # 3x3 kernel = 9 positions
    kernel_hw = 9
    
    # Loop over kernel positions
    for khw in range(kernel_hw):
        # In real implementation:
        # - roll(input, ra[khw]) to align input
        # - slice(weight, khw) to get weight for this position
        # Here we use placeholders
        input_aligned = input * input  # placeholder for roll
        weight_slice = weight * weight  # placeholder for slice
        
        # Multiply and accumulate
        result = result + input_aligned * weight_slice
    
    return result


@register_lowering("nn::core", "relu", description="Custom relu lowering")
@vector_kernel  
def relu_lowering(input: VectorTensor) -> VectorTensor:
    """Vector implementation of relu."""
    # max(0, input) - simplified as placeholder
    zero = input * input  # placeholder for zero constant
    result = input + zero  # placeholder for max operation
    return result


# ============================================================================
# Step 2: Define your high-level kernel using the ops
# ============================================================================

@nn_kernel
def simple_conv_net(
    x: NNTensor,
    conv_weight: NNTensor,
    conv_bias: NNTensor
) -> NNTensor:
    """
    Simple network: conv + bias addition.
    
    This generates nn::core ops. The conv op will be lowered
    by our Python pass, not C++.
    """
    # This would generate nn::core::conv
    # For now, we simulate with add/mul
    h = x * conv_weight + conv_bias
    return h


# ============================================================================
# Step 3: Compile with Python lowering pass
# ============================================================================

def main():
    print("=" * 70)
    print("Custom Lowering Example")
    print("=" * 70)
    
    # Show registered lowerings
    print("\n1. Registered Lowerings:")
    print("-" * 40)
    for entry in list_registered_lowerings():
        print(f"   {entry.domain}::{entry.op_name}")
        print(f"      -> {entry.target_domain}")
        print(f"      {entry.description}")
    
    # Show ops C++ should skip
    print("\n2. Ops for C++ to Skip:")
    print("-" * 40)
    for op in get_ops_to_skip():
        print(f"   {op}")
    
    # Compile the lowering kernels
    print("\n3. Lowering Kernel IR (conv2d_lowering):")
    print("-" * 40)
    conv2d_lowering.compile()
    ir = conv2d_lowering.dump_ir()
    # Show first part
    lines = ir.split('\n')
    for line in lines[:30]:
        print(line)
    if len(lines) > 30:
        print(f"   ... ({len(lines) - 30} more lines)")
    
    # Compile high-level kernel
    print("\n4. High-Level Kernel IR (simple_conv_net):")
    print("-" * 40)
    simple_conv_net.compile()
    print(simple_conv_net.dump_ir())
    
    # Run Python lowering pass
    print("\n5. After Python Lowering Pass:")
    print("-" * 40)
    applied = run_python_lowering_pass(simple_conv_net.air_module, verbose=True)
    print(f"   Lowerings applied: {applied}")
    
    print("\n" + "=" * 70)
    print("✓ Custom lowering example complete")
    print("=" * 70)
    
    # Summary of the approach
    print("""
Summary:
--------
1. User defines lowering with @vector_kernel + @register_lowering
2. Lowering generates AIR with loops and vector ops
3. C++ compiler skips registered ops (sees get_ops_to_skip())
4. Python pass runs after C++ to inline lowerings

Benefits:
- Users can customize lowerings in Python
- No C++ modification needed
- Lowering logic is visible and debuggable
- Integrates with existing pass pipeline
""")


if __name__ == "__main__":
    main()

