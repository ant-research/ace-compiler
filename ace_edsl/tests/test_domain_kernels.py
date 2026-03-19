"""
Test cases for domain kernel decorators.

Tests all domain kernel decorators:
- @tensor_kernel / @kernel
- @nn_kernel
- @vector_kernel
- @sihe_kernel
- @ckks_kernel
- @poly_kernel
- @compute_kernel
- @memory_kernel
"""

import sys
import os
import unittest

# Add ace-compiler directory to path for imports (needed for ace_edsl package)
current_dir = os.path.dirname(os.path.abspath(__file__))
ace_compiler_root = os.path.abspath(os.path.join(current_dir, '..', '..'))
if ace_compiler_root not in sys.path:
    sys.path.insert(0, ace_compiler_root)

# Import with error handling (may fail if dependencies not available)
try:
    from ace_edsl.edsl.domain_kernels import (
        tensor_kernel,
        kernel,  # Alias for tensor_kernel
        nn_kernel,
        vector_kernel,
        sihe_kernel,
        ckks_kernel,
        poly_kernel,
        compute_kernel,
        memory_kernel,
    )
    IMPORTS_AVAILABLE = True
except ImportError as e:
    IMPORTS_AVAILABLE = False
    IMPORT_ERROR = str(e)
    # Create dummy decorators for testing structure
    def dummy_decorator(func=None, **kwargs):
        def decorator(f):
            f._py_domain = "test"
            return f
        return decorator if func is None else decorator(func)
    tensor_kernel = kernel = nn_kernel = vector_kernel = sihe_kernel = \
        ckks_kernel = poly_kernel = compute_kernel = memory_kernel = dummy_decorator


class TestDomainKernelDecorators(unittest.TestCase):
    """Test that domain kernel decorators work correctly."""
    
    @unittest.skipIf(not IMPORTS_AVAILABLE, f"Imports not available: {IMPORT_ERROR if not IMPORTS_AVAILABLE else ''}")
    def test_tensor_kernel_without_parens(self):
        """Test @tensor_kernel decorator without parentheses."""
        @tensor_kernel
        def add(a, b):
            return a + b
        
        self.assertTrue(hasattr(add, '_py_domain'))
        self.assertEqual(add._py_domain, "air::core")
        self.assertTrue(callable(add))
    
    @unittest.skipIf(not IMPORTS_AVAILABLE, f"Imports not available: {IMPORT_ERROR if not IMPORTS_AVAILABLE else ''}")
    def test_tensor_kernel_with_parens(self):
        """Test @tensor_kernel() decorator with parentheses."""
        @tensor_kernel()
        def add(a, b):
            return a + b
        
        self.assertTrue(hasattr(add, '_py_domain'))
        self.assertEqual(add._py_domain, "air::core")
        self.assertTrue(callable(add))
    
    @unittest.skipIf(not IMPORTS_AVAILABLE, f"Imports not available: {IMPORT_ERROR if not IMPORTS_AVAILABLE else ''}")
    def test_kernel_alias(self):
        """Test that @kernel is an alias for @tensor_kernel."""
        @kernel
        def add(a, b):
            return a + b
        
        self.assertTrue(hasattr(add, '_py_domain'))
        self.assertEqual(add._py_domain, "air::core")
        self.assertEqual(kernel, tensor_kernel)  # Should be the same function
    
    @unittest.skipIf(not IMPORTS_AVAILABLE, f"Imports not available: {IMPORT_ERROR if not IMPORTS_AVAILABLE else ''}")
    def test_nn_kernel(self):
        """Test @nn_kernel decorator."""
        @nn_kernel
        def nn_add(a, b):
            return a + b
        
        self.assertTrue(hasattr(nn_add, '_py_domain'))
        self.assertEqual(nn_add._py_domain, "nn::core")
        self.assertTrue(callable(nn_add))
    
    @unittest.skipIf(not IMPORTS_AVAILABLE, f"Imports not available: {IMPORT_ERROR if not IMPORTS_AVAILABLE else ''}")
    def test_vector_kernel(self):
        """Test @vector_kernel decorator."""
        @vector_kernel
        def vec_add(a, b):
            return a + b
        
        self.assertTrue(hasattr(vec_add, '_py_domain'))
        self.assertEqual(vec_add._py_domain, "nn::vector")
        self.assertTrue(callable(vec_add))
    
    @unittest.skipIf(not IMPORTS_AVAILABLE, f"Imports not available: {IMPORT_ERROR if not IMPORTS_AVAILABLE else ''}")
    def test_sihe_kernel(self):
        """Test @sihe_kernel decorator."""
        @sihe_kernel
        def sihe_add(a, b):
            return a + b
        
        self.assertTrue(hasattr(sihe_add, '_py_domain'))
        self.assertEqual(sihe_add._py_domain, "fhe::sihe")
        self.assertTrue(callable(sihe_add))
    
    @unittest.skipIf(not IMPORTS_AVAILABLE, f"Imports not available: {IMPORT_ERROR if not IMPORTS_AVAILABLE else ''}")
    def test_ckks_kernel(self):
        """Test @ckks_kernel decorator."""
        @ckks_kernel
        def ckks_mul(a, b):
            return a * b
        
        self.assertTrue(hasattr(ckks_mul, '_py_domain'))
        self.assertEqual(ckks_mul._py_domain, "fhe::ckks")
        self.assertTrue(callable(ckks_mul))
    
    @unittest.skipIf(not IMPORTS_AVAILABLE, f"Imports not available: {IMPORT_ERROR if not IMPORTS_AVAILABLE else ''}")
    def test_poly_kernel(self):
        """Test @poly_kernel decorator."""
        @poly_kernel
        def poly_mul(a, b):
            return a * b
        
        self.assertTrue(hasattr(poly_mul, '_py_domain'))
        self.assertEqual(poly_mul._py_domain, "fhe::poly")
        self.assertTrue(callable(poly_mul))
    
    @unittest.skipIf(not IMPORTS_AVAILABLE, f"Imports not available: {IMPORT_ERROR if not IMPORTS_AVAILABLE else ''}")
    def test_compute_kernel(self):
        """Test @compute_kernel decorator."""
        @compute_kernel
        def compute_op(a, b):
            return a + b
        
        self.assertTrue(hasattr(compute_op, '_py_domain'))
        self.assertEqual(compute_op._py_domain, "compute")
        self.assertTrue(callable(compute_op))
    
    @unittest.skipIf(not IMPORTS_AVAILABLE, f"Imports not available: {IMPORT_ERROR if not IMPORTS_AVAILABLE else ''}")
    def test_memory_kernel(self):
        """Test @memory_kernel decorator."""
        @memory_kernel
        def memory_op(a, b):
            return a + b
        
        self.assertTrue(hasattr(memory_op, '_py_domain'))
        self.assertEqual(memory_op._py_domain, "memory")
        self.assertTrue(callable(memory_op))
    
    @unittest.skipIf(not IMPORTS_AVAILABLE, f"Imports not available: {IMPORT_ERROR if not IMPORTS_AVAILABLE else ''}")
    def test_all_domains_unique(self):
        """Test that all domains have unique domain strings."""
        domains = {}
        
        @tensor_kernel
        def t1(a): pass
        domains['tensor'] = t1._py_domain
        
        @nn_kernel
        def n1(a): pass
        domains['nn'] = n1._py_domain
        
        @vector_kernel
        def v1(a): pass
        domains['vector'] = v1._py_domain
        
        @sihe_kernel
        def s1(a): pass
        domains['sihe'] = s1._py_domain
        
        @ckks_kernel
        def c1(a): pass
        domains['ckks'] = c1._py_domain
        
        @poly_kernel
        def p1(a): pass
        domains['poly'] = p1._py_domain
        
        @compute_kernel
        def co1(a): pass
        domains['compute'] = co1._py_domain
        
        @memory_kernel
        def m1(a): pass
        domains['memory'] = m1._py_domain
        
        # All domains should be unique
        domain_values = list(domains.values())
        self.assertEqual(len(domain_values), len(set(domain_values)), 
                        f"Duplicate domains found: {domains}")
    
    @unittest.skipIf(not IMPORTS_AVAILABLE, f"Imports not available: {IMPORT_ERROR if not IMPORTS_AVAILABLE else ''}")
    def test_decorator_with_kwargs(self):
        """Test that decorators accept keyword arguments."""
        @tensor_kernel(verbose=True)
        def add(a, b):
            return a + b
        
        self.assertTrue(hasattr(add, '_py_domain'))
        self.assertEqual(add._py_domain, "air::core")
    
    @unittest.skipIf(not IMPORTS_AVAILABLE, f"Imports not available: {IMPORT_ERROR if not IMPORTS_AVAILABLE else ''}")
    def test_multiple_kernels_same_domain(self):
        """Test that multiple kernels can use the same domain."""
        @tensor_kernel
        def add1(a, b):
            return a + b
        
        @tensor_kernel
        def add2(a, b):
            return a + b
        
        self.assertEqual(add1._py_domain, add2._py_domain)
        self.assertEqual(add1._py_domain, "air::core")
    
    @unittest.skipIf(not IMPORTS_AVAILABLE, f"Imports not available: {IMPORT_ERROR if not IMPORTS_AVAILABLE else ''}")
    def test_kernel_preserves_function_metadata(self):
        """Test that decorator preserves function name and docstring."""
        @tensor_kernel
        def my_function(a, b):
            """This is a test function."""
            return a + b
        
        self.assertEqual(my_function.__name__, "my_function")
        self.assertEqual(my_function.__doc__, "This is a test function.")


class TestDomainKernelExamples(unittest.TestCase):
    """Test realistic kernel examples."""
    
    @unittest.skipIf(not IMPORTS_AVAILABLE, f"Imports not available: {IMPORT_ERROR if not IMPORTS_AVAILABLE else ''}")
    def test_tensor_add_kernel(self):
        """Test a simple tensor addition kernel."""
        @tensor_kernel
        def tensor_add(a, b):
            """Add two tensors."""
            return a + b
        
        self.assertEqual(tensor_add._py_domain, "air::core")
        self.assertEqual(tensor_add.__name__, "tensor_add")
        self.assertEqual(tensor_add.__doc__, "Add two tensors.")
    
    @unittest.skipIf(not IMPORTS_AVAILABLE, f"Imports not available: {IMPORT_ERROR if not IMPORTS_AVAILABLE else ''}")
    def test_nn_matmul_kernel(self):
        """Test a neural network matmul kernel."""
        @nn_kernel
        def nn_matmul(a, b):
            """Matrix multiplication for neural networks."""
            return a @ b
        
        self.assertEqual(nn_matmul._py_domain, "nn::core")
    
    @unittest.skipIf(not IMPORTS_AVAILABLE, f"Imports not available: {IMPORT_ERROR if not IMPORTS_AVAILABLE else ''}")
    def test_vector_fused_kernel(self):
        """Test a vector fused operation kernel."""
        @vector_kernel
        def vec_fused_mul_add(a, b, c):
            """Fused multiply-add: a * b + c."""
            return a * b + c
        
        self.assertEqual(vec_fused_mul_add._py_domain, "nn::vector")
    
    @unittest.skipIf(not IMPORTS_AVAILABLE, f"Imports not available: {IMPORT_ERROR if not IMPORTS_AVAILABLE else ''}")
    def test_sihe_add_kernel(self):
        """Test a SIHE addition kernel."""
        @sihe_kernel
        def sihe_add(a, b):
            """SIHE ciphertext addition."""
            return a + b
        
        self.assertEqual(sihe_add._py_domain, "fhe::sihe")
    
    @unittest.skipIf(not IMPORTS_AVAILABLE, f"Imports not available: {IMPORT_ERROR if not IMPORTS_AVAILABLE else ''}")
    def test_ckks_mul_kernel(self):
        """Test a CKKS multiplication kernel."""
        @ckks_kernel
        def ckks_mul(a, b):
            """CKKS ciphertext multiplication with scale management."""
            return a * b
        
        self.assertEqual(ckks_mul._py_domain, "fhe::ckks")
    
    @unittest.skipIf(not IMPORTS_AVAILABLE, f"Imports not available: {IMPORT_ERROR if not IMPORTS_AVAILABLE else ''}")
    def test_poly_ntt_kernel(self):
        """Test a polynomial NTT kernel."""
        @poly_kernel
        def poly_ntt(p):
            """Number Theoretic Transform."""
            return p  # Simplified
        
        self.assertEqual(poly_ntt._py_domain, "fhe::poly")


class TestDomainKernelEdgeCases(unittest.TestCase):
    """Test edge cases and error conditions."""
    
    @unittest.skipIf(not IMPORTS_AVAILABLE, f"Imports not available: {IMPORT_ERROR if not IMPORTS_AVAILABLE else ''}")
    def test_empty_function(self):
        """Test decorator on empty function."""
        @tensor_kernel
        def empty():
            pass
        
        self.assertEqual(empty._py_domain, "air::core")
    
    @unittest.skipIf(not IMPORTS_AVAILABLE, f"Imports not available: {IMPORT_ERROR if not IMPORTS_AVAILABLE else ''}")
    def test_function_with_no_args(self):
        """Test decorator on function with no arguments."""
        @tensor_kernel
        def no_args():
            return 42
        
        self.assertEqual(no_args._py_domain, "air::core")
    
    @unittest.skipIf(not IMPORTS_AVAILABLE, f"Imports not available: {IMPORT_ERROR if not IMPORTS_AVAILABLE else ''}")
    def test_function_with_many_args(self):
        """Test decorator on function with many arguments."""
        @tensor_kernel
        def many_args(a, b, c, d, e, f, g, h):
            return a + b + c + d + e + f + g + h
        
        self.assertEqual(many_args._py_domain, "air::core")
    
    @unittest.skipIf(not IMPORTS_AVAILABLE, f"Imports not available: {IMPORT_ERROR if not IMPORTS_AVAILABLE else ''}")
    def test_function_with_kwargs(self):
        """Test decorator on function with keyword arguments."""
        @tensor_kernel
        def with_kwargs(a, b, c=10):
            return a + b + c
        
        self.assertEqual(with_kwargs._py_domain, "air::core")
    
    @unittest.skipIf(not IMPORTS_AVAILABLE, f"Imports not available: {IMPORT_ERROR if not IMPORTS_AVAILABLE else ''}")
    def test_function_with_star_args(self):
        """Test decorator on function with *args."""
        @tensor_kernel
        def with_star_args(*args):
            return sum(args)
        
        self.assertEqual(with_star_args._py_domain, "air::core")
    
    @unittest.skipIf(not IMPORTS_AVAILABLE, f"Imports not available: {IMPORT_ERROR if not IMPORTS_AVAILABLE else ''}")
    def test_function_with_star_kwargs(self):
        """Test decorator on function with **kwargs."""
        @tensor_kernel
        def with_star_kwargs(**kwargs):
            return sum(kwargs.values())
        
        self.assertEqual(with_star_kwargs._py_domain, "air::core")


def run_tests():
    """Run all tests and print summary."""
    loader = unittest.TestLoader()
    suite = unittest.TestSuite()
    
    # Add all test classes
    suite.addTests(loader.loadTestsFromTestCase(TestDomainKernelDecorators))
    suite.addTests(loader.loadTestsFromTestCase(TestDomainKernelIntegration))
    suite.addTests(loader.loadTestsFromTestCase(TestDomainKernelExamples))
    suite.addTests(loader.loadTestsFromTestCase(TestDomainKernelEdgeCases))
    
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    
    # Print summary
    print("\n" + "=" * 70)
    if result.wasSuccessful():
        print("✓ All domain kernel tests passed!")
    else:
        print(f"✗ Tests failed: {len(result.failures)} failures, {len(result.errors)} errors")
    print("=" * 70)
    
    return result.wasSuccessful()


if __name__ == "__main__":
    success = run_tests()
    sys.exit(0 if success else 1)

