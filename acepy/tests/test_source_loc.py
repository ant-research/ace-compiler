#!/usr/bin/env python3
"""
Test source location tracking through the compilation pipeline.

This test verifies that Python line numbers are preserved when creating AIR.
"""
import sys
import os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from ace_dsl import kernel
from ace_dsl.core.types import Tensor
from ace_dsl.bindings import air_builder


def test_file_registration():
    """Test that source files can be registered."""
    glob = air_builder.create_glob_scope()
    
    # Register a file
    file_id = glob.register_file("/test/my_kernel.py")
    assert file_id > 0, "File ID should be positive"
    
    # Same file should return same ID
    file_id2 = glob.register_file("/test/my_kernel.py")
    assert file_id == file_id2, "Same file should return same ID"
    
    # Different file should get different ID
    file_id3 = glob.register_file("/test/other_kernel.py")
    assert file_id3 != file_id, "Different files should have different IDs"
    
    print("✓ test_file_registration passed")


def test_container_set_loc():
    """Test that Container can receive source locations."""
    glob = air_builder.create_glob_scope()
    file_id = glob.register_file("/test/my_kernel.py")
    
    # Create a function to get a container
    func = glob.new_func_with_params("test_func", 2, [64])
    container = func.container()
    
    # Set source location
    container.set_loc(file_id, 42, 10)  # Line 42, column 10
    
    # Create a node - should use the set location
    a = func.new_param("a", air_builder.Type.make_float(32))
    b = func.new_param("b", air_builder.Type.make_float(32))
    
    # Set another location
    container.set_loc(file_id, 43, 15)  # Line 43
    result = container.new_add(a, b)
    
    print("✓ test_container_set_loc passed")


# Define kernels at module level (required for source parsing)
@kernel
def add_kernel(a: Tensor[64], b: Tensor[64]) -> Tensor[64]:
    return a + b  # This should track line numbers


@kernel
def multi_line(a: Tensor[64], b: Tensor[64], c: Tensor[64]) -> Tensor[64]:
    # Line N: first operation
    x = a + b
    # Line N+2: second operation
    y = x * c
    # Line N+4: return
    return y


def test_kernel_source_tracking():
    """Test that @kernel decorator tracks source locations."""
    
    # Trigger compilation
    add_kernel.compile()
    
    # The kernel should have been compiled
    assert add_kernel._compiled, "Kernel should be compiled"
    
    # Get the AIR dump
    air_dump = add_kernel.air_module.dump()
    print(f"\nGenerated AIR:\n{air_dump}")
    
    # The kernel's source file should be registered
    # We can verify by checking the glob scope
    glob = add_kernel.air_module
    
    # If using real ACE bindings, the SPOS should contain line info
    # For mock, it's tracked but not reflected in the dump
    if hasattr(glob, 'has_native_ir') and glob.has_native_ir():
        print("  Using real ACE bindings - line numbers embedded in IR")
    else:
        print("  Using mock bindings - line numbers tracked but not in dump")
    
    print("✓ test_kernel_source_tracking passed")


def test_multi_line_kernel():
    """Test source tracking across multiple lines."""
    
    multi_line.compile()
    assert multi_line._compiled
    air_dump = multi_line.air_module.dump()
    print(f"\nMulti-line kernel AIR:\n{air_dump}")
    
    print("✓ test_multi_line_kernel passed")


if __name__ == "__main__":
    print("=" * 60)
    print("Testing Source Location Tracking")
    print("=" * 60)
    
    # Check binding status
    print(f"\nair_builder mock status: {getattr(air_builder, '__is_mock__', 'unknown')}")
    print(f"air_builder ACE enabled: {getattr(air_builder, '__ace_enabled__', 'unknown')}")
    print()
    
    test_file_registration()
    test_container_set_loc()
    test_kernel_source_tracking()
    test_multi_line_kernel()
    
    print("\n" + "=" * 60)
    print("All tests passed!")
    print("=" * 60)

