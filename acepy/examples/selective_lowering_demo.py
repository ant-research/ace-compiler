#!/usr/bin/env python3
"""
Selective Lowering Demo

This example demonstrates the selective lowering feature where:
1. Python registers custom lowerings with skip_cpp=True
2. C++ compiler (nn-addon) skips lowering for registered ops
3. Python post-lowering pass inlines the custom lowerings

Run with:
    cd acepy
    PYTHONPATH=. python examples/selective_lowering_demo.py
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from ace_dsl.frontend.lowering_registry import (
    register_lowering, 
    get_lowering, 
    has_lowering,
    list_lowerings, 
    clear_lowerings, 
    get_ops_to_skip,
    sync_skip_ops_to_cpp,
)
from ace_dsl.frontend.domain_kernels import (
    vector_kernel,
    VectorTensor,
)
from ace_bindings import passmanager, nn_addon


# ═══════════════════════════════════════════════════════════════════════════════
# Custom Lowering Implementation
# ═══════════════════════════════════════════════════════════════════════════════

# Define conv2d lowering at module level (required for AST parsing)
@vector_kernel
def my_conv2d_lowering(
    input: VectorTensor,
    weight: VectorTensor,
    bias: VectorTensor
) -> VectorTensor:
    """
    Custom conv2d lowering using vector operations.
    
    This demonstrates a simplified conv2d using multiply-accumulate
    with a loop over kernel positions.
    """
    result = bias
    kernel_hw = 9  # 3x3 kernel
    
    for khw in range(kernel_hw):
        # In real implementation:
        # - roll(input, ra[khw]) to align input with kernel position
        # - slice(weight, khw) to get weight for this position
        aligned = input * input    # Placeholder for roll
        sliced = weight * weight   # Placeholder for slice
        result = result + aligned * sliced
    
    return result


# ═══════════════════════════════════════════════════════════════════════════════
# Demo Functions
# ═══════════════════════════════════════════════════════════════════════════════

def demo_basic_registration():
    """Demonstrate basic selective lowering registration."""
    print("\n" + "=" * 70)
    print("Demo 1: Basic Selective Lowering Registration")
    print("=" * 70)
    
    clear_lowerings()
    
    # Register conv2d with skip_cpp=True
    # This tells C++ (nn-addon) to skip lowering this op
    register_lowering(
        "nn::core", "conv2d",
        skip_cpp=True,
        description="Custom conv2d using Python"
    )(my_conv2d_lowering)
    
    print("\nRegistered lowerings:")
    print("-" * 50)
    for info in list_lowerings():
        skip_str = "SKIP C++" if info.skip_cpp else "C++ handles"
        print(f"  {info.source_domain}::{info.op_name}")
        print(f"    -> {info.target_domain} [{skip_str}]")
        print(f"    {info.description}")
        print()
    
    print("Ops that C++ should skip:")
    for op in get_ops_to_skip():
        print(f"  - {op}")


def demo_cpp_bridge():
    """Demonstrate Python-C++ bridge communication."""
    print("\n" + "=" * 70)
    print("Demo 2: Python-C++ Bridge Communication")
    print("=" * 70)
    
    # Check available bindings
    print("\nBinding status:")
    print(f"  passmanager.set_skip_ops: {hasattr(passmanager, 'set_skip_ops')}")
    print(f"  nn_addon.set_skip_ops: {hasattr(nn_addon, 'set_skip_ops')}")
    
    # Sync Python registry to C++ (passmanager bridge)
    print("\n1. Syncing to passmanager bridge:")
    sync_skip_ops_to_cpp()
    if hasattr(passmanager, 'get_skip_ops'):
        cpp_ops = passmanager.get_skip_ops()
        print(f"   Skip list: {cpp_ops}")
    
    # Sync to nn_addon (real ACE nn-addon)
    print("\n2. Syncing to nn_addon (real compiler):")
    if hasattr(nn_addon, 'set_skip_ops'):
        ops = get_ops_to_skip()
        nn_addon.set_skip_ops(ops)
        print(f"   Set ops: {ops}")
        
        # Verify
        should_skip = nn_addon.should_skip_lowering("nn::core", "conv2d")
        print(f"   nn_addon.should_skip_lowering('nn::core', 'conv2d') = {should_skip}")


def demo_nn_addon_integration():
    """Demonstrate nn-addon selective lowering."""
    print("\n" + "=" * 70)
    print("Demo 3: nn-addon Selective Lowering")
    print("=" * 70)
    
    # Clear and set up fresh
    clear_lowerings()
    if hasattr(nn_addon, 'clear_skip_ops'):
        nn_addon.clear_skip_ops()
    
    # Register conv to skip
    register_lowering("nn::core", "conv", skip_cpp=True)(my_conv2d_lowering)
    
    # Sync to nn-addon
    ops = get_ops_to_skip()
    if hasattr(nn_addon, 'set_skip_ops'):
        nn_addon.set_skip_ops(ops)
    
    print(f"\n  Registered: nn::core::conv (skip_cpp=True)")
    print(f"  nn_addon skip list: {ops}")
    
    # Show how nn-addon checks skip
    print("\n  nn-addon skip checks:")
    print(f"    should_skip('nn::core', 'conv') = {nn_addon.should_skip_lowering('nn::core', 'conv')}")
    print(f"    should_skip('nn::core', 'relu') = {nn_addon.should_skip_lowering('nn::core', 'relu')}")
    print(f"    should_skip('nn::core', 'add')  = {nn_addon.should_skip_lowering('nn::core', 'add')}")
    
    print("\n  When running real tensor2vector pass:")
    print("    - conv will be SKIPPED (kept as nn::core::conv)")
    print("    - relu will be LOWERED (transformed to nn::vector::relu)")
    print("    - add will be LOWERED (transformed to nn::vector::add)")


def demo_supported_ops():
    """Show all ops that support selective lowering in nn-addon."""
    print("\n" + "=" * 70)
    print("Demo 4: Supported Ops for Selective Lowering")
    print("=" * 70)
    
    print("\nThe following nn::core ops support selective lowering in nn-addon:")
    print("-" * 50)
    
    supported_ops = [
        ("add", "Handle_add in tensor2vector_handler.h"),
        ("mul", "Handle_mul in tensor2vector_handler.h"),
        ("conv", "Handle_conv in tensor2vector_handler.h"),
        ("relu", "Handle_relu in tensor2vector_handler.h"),
        ("gemm", "Handle_gemm in tensor2vector_handler.h"),
        ("average_pool", "Handle_average_pool in tensor2vector_handler.h"),
        ("max_pool", "Handle_max_pool in tensor2vector_handler.h"),
        ("flatten", "Handle_flatten in tensor2vector_handler.h"),
        ("reshape", "Handle_reshape in tensor2vector_handler.h"),
        ("global_average_pool", "Handle_global_average_pool in tensor2vector_handler.h"),
        ("strided_slice", "Handle_strided_slice in tensor2vector_handler.h"),
    ]
    
    for op, handler in supported_ops:
        print(f"  nn::core::{op:<20} - {handler}")
    
    print("\nTo skip any of these ops, register a lowering with skip_cpp=True")


def demo_lowering_ir():
    """Show the lowering kernel IR."""
    print("\n" + "=" * 70)
    print("Demo 5: Lowering Kernel IR")
    print("=" * 70)
    
    clear_lowerings()
    register_lowering("nn::core", "conv", skip_cpp=True)(my_conv2d_lowering)
    
    print("\n1. Compile the custom conv lowering kernel:")
    my_conv2d_lowering.compile()
    lowering_ir = my_conv2d_lowering.dump_ir()
    
    print("   Lowering IR (what gets inlined for conv):")
    for i, line in enumerate(lowering_ir.split('\n')[:30]):
        if line.strip():
            print(f"   {line}")
    print("   ...")
    
    print("\n2. This IR will be inlined when Python post-lowering pass finds")
    print("   an nn::core::conv node that was skipped by C++.")


# ═══════════════════════════════════════════════════════════════════════════════
# Main
# ═══════════════════════════════════════════════════════════════════════════════

def main():
    print("=" * 70)
    print("          Selective Lowering Demo")
    print("=" * 70)
    print("""
This demo shows how to use selective lowering to:
- Register custom Python lowerings for specific ops
- Have C++ (nn-addon) skip lowering for those ops
- Apply Python lowerings after C++ passes
""")
    
    demo_basic_registration()
    demo_cpp_bridge()
    demo_nn_addon_integration()
    demo_supported_ops()
    demo_lowering_ir()
    
    print("\n" + "=" * 70)
    print("          Summary")
    print("=" * 70)
    print("""
Key APIs:
---------
  register_lowering(domain, op, skip_cpp=True)  # Register Python lowering
  get_ops_to_skip()                              # Get ops C++ should skip
  sync_skip_ops_to_cpp()                         # Sync to passmanager bridge
  nn_addon.set_skip_ops(ops)                     # Sync to real nn-addon

nn-addon Integration:
---------------------
  The skip list is checked in tensor2vector_handler.h:
  
    template <typename RETV, typename VISITOR>
    RETV Handle_conv(VISITOR* visitor, air::base::NODE_PTR node) {
        if (should_skip_op("nn::core", "conv")) {
            // Skip lowering - keep node as-is for Python pass
            return clone_node_with_visited_children<RETV>(visitor, node);
        }
        // ... original lowering logic ...
    }

Workflow:
---------
  1. @register_lowering("nn::core", "conv", skip_cpp=True)
     @vector_kernel
     def my_conv_lowering(...): ...
     
  2. nn_addon.set_skip_ops(get_ops_to_skip())  # Before running C++ passes
  
  3. Run tensor2vector pass  # C++ skips registered ops
  
  4. run_python_lowering_pass(kernel.air_module)  # Python fills in
""")
    print("=" * 70)
    print("✓ All demos completed successfully!")
    print("=" * 70)


if __name__ == "__main__":
    main()
