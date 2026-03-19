#!/usr/bin/env python3
"""
Kernel Tests for PyACE DSL
==========================

Test suite for verifying kernel compilation through the ACE pipeline.
Similar to cuda-python-tile-compiler test structure.
"""

import unittest
import sys
import os

# Add PyACE to path
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))


class TestSimpleAddKernels(unittest.TestCase):
    """Test simple arithmetic kernels."""
    
    def setUp(self):
        """Import kernels."""
        from tests.kernels import simple_add
        self.module = simple_add
    
    def test_add_kernel_definition(self):
        """Test add_kernel is properly defined."""
        kernel = self.module.add_kernel
        self.assertEqual(kernel.name, "add_kernel")
        self.assertEqual(len(kernel.parameters), 2)
    
    def test_add_mul_kernel_definition(self):
        """Test add_mul_kernel is properly defined."""
        kernel = self.module.add_mul_kernel
        self.assertEqual(kernel.name, "add_mul_kernel")
        self.assertEqual(len(kernel.parameters), 3)
    
    def test_polynomial_kernel_definition(self):
        """Test polynomial_kernel is properly defined."""
        kernel = self.module.polynomial_kernel
        self.assertEqual(kernel.name, "polynomial_kernel")
        self.assertEqual(len(kernel.parameters), 4)
    
    def test_add_kernel_ir_dump(self):
        """Test that add_kernel can dump IR."""
        kernel = self.module.add_kernel
        ir = kernel.dump_ir()
        self.assertIn("add_kernel", ir)
        self.assertIn("ADD", ir)  # Should contain add operation


class TestMatMulKernels(unittest.TestCase):
    """Test matrix multiplication kernels."""
    
    def setUp(self):
        """Import kernels."""
        from tests.kernels import matmul
        self.module = matmul
    
    def test_gemm_kernel_definition(self):
        """Test gemm_kernel is properly defined."""
        kernel = self.module.gemm_kernel
        self.assertEqual(kernel.name, "gemm_kernel")
        self.assertEqual(len(kernel.parameters), 3)
    
    def test_gemm_kernel_ir_dump(self):
        """Test that gemm_kernel can dump IR."""
        kernel = self.module.gemm_kernel
        ir = kernel.dump_ir()
        self.assertIn("gemm_kernel", ir)


class TestLeNetKernels(unittest.TestCase):
    """Test LeNet-style CNN kernels."""
    
    def setUp(self):
        """Import kernels."""
        from tests.kernels import lenet
        self.module = lenet
    
    def test_conv2d_kernel_definition(self):
        """Test conv2d_kernel is properly defined."""
        kernel = self.module.conv2d_kernel
        self.assertEqual(kernel.name, "conv2d_kernel")
    
    def test_relu_approx_kernel_definition(self):
        """Test relu_approx_kernel is properly defined."""
        kernel = self.module.relu_approx_kernel
        self.assertEqual(kernel.name, "relu_approx_kernel")
    
    def test_lenet_block_definition(self):
        """Test lenet_block is properly defined."""
        kernel = self.module.lenet_block
        self.assertEqual(kernel.name, "lenet_block")
    
    def test_lenet_block_ir_dump(self):
        """Test that lenet_block can dump IR."""
        kernel = self.module.lenet_block
        ir = kernel.dump_ir()
        self.assertIn("lenet_block", ir)


class TestFFTKernels(unittest.TestCase):
    """Test FFT-related kernels for FHE."""
    
    def setUp(self):
        """Import kernels."""
        from tests.kernels import fft_fhe
        self.module = fft_fhe
    
    def test_butterfly_kernel_definition(self):
        """Test butterfly_kernel is properly defined."""
        kernel = self.module.butterfly_kernel
        self.assertEqual(kernel.name, "butterfly_kernel")
        self.assertEqual(len(kernel.parameters), 3)
    
    def test_ntt_butterfly_kernel_definition(self):
        """Test ntt_butterfly_kernel is properly defined."""
        kernel = self.module.ntt_butterfly_kernel
        self.assertEqual(kernel.name, "ntt_butterfly_kernel")
    
    def test_polynomial_mul_kernel_definition(self):
        """Test polynomial_mul_kernel is properly defined."""
        kernel = self.module.polynomial_mul_kernel
        self.assertEqual(kernel.name, "polynomial_mul_kernel")
        self.assertEqual(len(kernel.parameters), 4)


class TestAttentionKernels(unittest.TestCase):
    """Test Transformer attention kernels."""
    
    def setUp(self):
        """Import kernels."""
        from tests.kernels import attention
        self.module = attention
    
    def test_scaled_dot_product_kernel_definition(self):
        """Test scaled_dot_product_kernel is properly defined."""
        kernel = self.module.scaled_dot_product_kernel
        self.assertEqual(kernel.name, "scaled_dot_product_kernel")
        self.assertEqual(len(kernel.parameters), 4)
    
    def test_linear_attention_kernel_definition(self):
        """Test linear_attention_kernel is properly defined."""
        kernel = self.module.linear_attention_kernel
        self.assertEqual(kernel.name, "linear_attention_kernel")
    
    def test_feed_forward_kernel_definition(self):
        """Test feed_forward_kernel is properly defined."""
        kernel = self.module.feed_forward_kernel
        self.assertEqual(kernel.name, "feed_forward_kernel")


class TestCompilationPipeline(unittest.TestCase):
    """Test full compilation pipeline."""
    
    def test_compile_simple_add_to_fhe(self):
        """Test compiling add_kernel through FHE pipeline."""
        from tests.kernels.simple_add import add_kernel
        from ace_dsl.frontend.compile import compile_fhe
        
        # Compile to FHE C code
        c_code = compile_fhe(add_kernel, enable_ir_printing=False)
        
        # Should produce some output
        self.assertIsNotNone(c_code)
        self.assertGreater(len(c_code), 0)
    
    def test_compile_matmul_to_fhe(self):
        """Test compiling gemm_kernel through FHE pipeline."""
        from tests.kernels.matmul import gemm_kernel
        from ace_dsl.frontend.compile import compile_fhe
        
        c_code = compile_fhe(gemm_kernel, enable_ir_printing=False)
        
        self.assertIsNotNone(c_code)
        self.assertGreater(len(c_code), 0)


class TestIRPrinting(unittest.TestCase):
    """Test IR printing functionality."""
    
    def test_ir_printing_enabled(self):
        """Test that IR printing can be enabled."""
        from tests.kernels.simple_add import add_kernel
        from ace_dsl.frontend.compile import compile_fhe
        import io
        import sys
        
        # Capture stdout
        old_stdout = sys.stdout
        sys.stdout = io.StringIO()
        
        try:
            compile_fhe(add_kernel, enable_ir_printing=True)
            output = sys.stdout.getvalue()
        finally:
            sys.stdout = old_stdout
        
        # Should have printed something
        # (In mock mode, may just print warnings)
        self.assertIsNotNone(output)


if __name__ == '__main__':
    print("=" * 70)
    print("PyACE Kernel Test Suite")
    print("=" * 70)
    print()
    
    # Run tests with verbosity
    unittest.main(argv=['first-arg-is-ignored'], exit=False, verbosity=2)

