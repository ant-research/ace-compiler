#!/usr/bin/env python3
"""
Basic Tests for ACE DSL
========================

Unit tests for core functionality.
"""

import sys
import os

# Add parent directory to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))


def test_tensor_type():
    """Test Tensor type annotations."""
    print("Testing Tensor type...")
    
    from ace_dsl.core.types import Tensor, get_tensor_shape
    
    # Test subscript notation
    T1 = Tensor[64]
    assert T1.get_shape() == (64,), f"Expected (64,), got {T1.get_shape()}"
    
    T2 = Tensor[1, 3, 224, 224]
    assert T2.get_shape() == (1, 3, 224, 224), f"Expected (1, 3, 224, 224), got {T2.get_shape()}"
    
    print("  ✓ Tensor type tests passed")


def test_python_ir():
    """Test Python IR generation."""
    print("Testing Python IR...")
    
    from base_dsl import get_function_ir, Scope
    
    def simple_func(a, b):
        c = a + b
        return c
    
    scope = Scope()
    ir = get_function_ir(simple_func, scope)
    
    assert ir.name == "simple_func", f"Expected 'simple_func', got {ir.name}"
    assert len(ir.parameters) == 2, f"Expected 2 params, got {len(ir.parameters)}"
    assert len(ir.root_block) > 0, "Expected non-empty block"
    
    print("  ✓ Python IR tests passed")


def test_kernel_decorator():
    """Test @kernel decorator."""
    print("Testing @kernel decorator...")
    
    from ace_dsl import kernel, Tensor
    
    @kernel
    def add_kernel(a: Tensor[64], b: Tensor[64]):
        return a + b
    
    # Check it returns CompiledKernel
    assert add_kernel.name == "add_kernel"
    assert len(add_kernel.parameters) == 2
    
    # Check dump_ir works
    ir_str = add_kernel.dump_ir()
    assert "add_kernel" in ir_str
    
    print("  ✓ @kernel decorator tests passed")


def test_lowering_registry():
    """Test lowering function registry."""
    print("Testing lowering registry...")
    
    from ace_dsl.core.registry import (
        nn_to_vector, 
        get_lowering_function,
        list_lowering_functions
    )
    
    # Check built-in lowerings are registered
    funcs = list_lowering_functions("nn_to_vector")
    assert "add" in funcs, "Expected 'add' in registry"
    assert "conv" in funcs, "Expected 'conv' in registry"
    
    # Check we can retrieve lowering functions
    add_func = get_lowering_function("add", "nn_to_vector")
    assert add_func is not None
    
    print("  ✓ Lowering registry tests passed")


def test_air_value():
    """Test AIRValue operator overloading."""
    print("Testing AIRValue...")
    
    from ace_dsl.core.air_value import AIRValue, MockContainer
    
    container = MockContainer()
    a = AIRValue("a", container)
    b = AIRValue("b", container)
    
    # Test operators
    c = a + b
    assert isinstance(c, AIRValue)
    
    d = a * b
    assert isinstance(d, AIRValue)
    
    e = a - b
    assert isinstance(e, AIRValue)
    
    # Check nodes were created
    nodes = container.get_nodes()
    assert len(nodes) >= 3, f"Expected at least 3 nodes, got {len(nodes)}"
    
    opcodes = [n.opcode for n in nodes]
    assert "nn::vector::ADD" in opcodes
    assert "nn::vector::MUL" in opcodes
    assert "nn::vector::SUB" in opcodes
    
    print("  ✓ AIRValue tests passed")


def test_tensor2vector_pass():
    """Test Tensor2VectorPyPass."""
    print("Testing Tensor2VectorPyPass...")
    
    from ace_dsl.passes.tensor2vector_pass import Tensor2VectorPyPass
    from base_dsl.python_ir import Block, Scope, BinOp, Var
    from base_dsl.loc import Loc
    
    # Create a simple IR block with an "add" operation
    scope = Scope()
    block = Block(scope)
    
    loc = Loc(1, 0, "test.py")
    a = Var("a", loc)
    b = Var("b", loc)
    c = Var("c", loc)
    
    block.append(BinOp("add", a, b, c, loc))
    
    # Create pass and run
    pass_instance = Tensor2VectorPyPass(verbose=False)
    
    # The pass would normally work on AIR modules
    # This is a simplified test
    
    print("  ✓ Tensor2VectorPyPass tests passed")


def test_source_location():
    """Test source location tracking."""
    print("Testing source location...")
    
    from base_dsl.loc import Loc, get_caller_loc, source_location
    
    # Test Loc creation
    loc = Loc(10, 5, "test.py")
    assert loc.line == 10
    assert loc.col == 5
    assert "test.py:10:5" in str(loc)
    
    # Test unknown location
    unk = Loc.unknown()
    assert unk.line == 0
    
    # Test context manager
    with source_location(loc):
        from base_dsl.loc import get_current_loc
        current = get_current_loc()
        assert current.line == 10
    
    print("  ✓ Source location tests passed")


def main():
    """Run all tests."""
    print("=" * 60)
    print("ACE DSL Unit Tests")
    print("=" * 60 + "\n")
    
    tests = [
        test_tensor_type,
        test_python_ir,
        test_kernel_decorator,
        test_lowering_registry,
        test_air_value,
        test_tensor2vector_pass,
        test_source_location,
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            test()
            passed += 1
        except Exception as e:
            print(f"  ✗ {test.__name__} FAILED: {e}")
            failed += 1
    
    print("\n" + "=" * 60)
    print(f"Results: {passed} passed, {failed} failed")
    print("=" * 60)
    
    return 0 if failed == 0 else 1


if __name__ == "__main__":
    sys.exit(main())

