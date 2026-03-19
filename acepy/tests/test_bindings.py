"""
Test C++ bindings (or mock fallbacks).

These tests verify that the binding layer works correctly,
whether using actual C++ bindings or Python mocks.
"""

import unittest
import sys
import os

# Add PyACE to path
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))


class TestAirBuilder(unittest.TestCase):
    """Test air_builder bindings."""
    
    def test_import(self):
        """Test that air_builder can be imported."""
        from ace_dsl.bindings import air_builder
        self.assertIsNotNone(air_builder)
    
    def test_create_glob_scope(self):
        """Test creating a global scope."""
        from ace_dsl.bindings import air_builder
        
        glob = air_builder.create_glob_scope()
        self.assertIsNotNone(glob)
    
    def test_create_function(self):
        """Test creating a function."""
        from ace_dsl.bindings import air_builder
        
        glob = air_builder.create_glob_scope()
        func = glob.new_func("test_func")
        self.assertEqual(func.name, "test_func")
    
    def test_create_params(self):
        """Test creating function parameters."""
        from ace_dsl.bindings import air_builder
        
        glob = air_builder.create_glob_scope()
        func = glob.new_func("test_func")
        
        param_type = air_builder.Type.make_float(32)
        param = func.new_param("x", param_type)
        
        self.assertIsNotNone(param)
    
    def test_arithmetic_ops(self):
        """Test arithmetic operations."""
        from ace_dsl.bindings import air_builder
        
        glob = air_builder.create_glob_scope()
        func = glob.new_func("test_func")
        container = func.container()
        
        # Create params
        a = func.new_param("a", air_builder.Type.make_float(32))
        b = func.new_param("b", air_builder.Type.make_float(32))
        
        # Test operations
        add_result = container.new_add(a, b)
        self.assertIsNotNone(add_result)
        
        mul_result = container.new_mul(a, b)
        self.assertIsNotNone(mul_result)
        
        sub_result = container.new_sub(a, b)
        self.assertIsNotNone(sub_result)
    
    def test_dump(self):
        """Test dumping IR."""
        from ace_dsl.bindings import air_builder
        
        glob = air_builder.create_glob_scope()
        func = glob.new_func("test_func")
        container = func.container()
        
        a = func.new_param("a", air_builder.Type.make_float(32))
        b = func.new_param("b", air_builder.Type.make_float(32))
        result = container.new_add(a, b)
        container.new_retv(result)
        
        ir = glob.dump()
        self.assertIn("test_func", ir)


class TestNNAddon(unittest.TestCase):
    """Test nn_addon bindings."""
    
    def test_import(self):
        """Test that nn_addon can be imported."""
        from ace_dsl.bindings import nn_addon
        self.assertIsNotNone(nn_addon)
    
    def test_opcodes(self):
        """Test opcode constants."""
        from ace_dsl.bindings import nn_addon
        
        self.assertEqual(nn_addon.core.ADD, "nn::core::ADD")
        self.assertEqual(nn_addon.vector.MUL, "nn::vector::MUL")
    
    def test_container_ops(self):
        """Test container operations."""
        from ace_dsl.bindings import nn_addon
        
        # Check if using C++ bindings (loaded from .so file) vs Python mocks
        is_cpp_binding = hasattr(nn_addon, '__file__') and '.so' in str(getattr(nn_addon, '__file__', ''))
        
        container = nn_addon.Container()
        self.assertIsNotNone(container)
        
        if is_cpp_binding:
            # For C++ bindings, just verify methods exist
            self.assertTrue(hasattr(container, 'new_core_add'))
            self.assertTrue(hasattr(container, 'new_vec_mul'))
            return
        
        # Python mock tests
        from ace_dsl.bindings.mock_nn_addon import Node
        a = Node("nn::core", "PARAM")
        b = Node("nn::core", "PARAM")
        
        # Test nn::core ops
        add_node = container.new_core_add(a, b)
        self.assertIn("nn::core::ADD", add_node.full_opcode())
        
        # Test nn::vector ops
        vec_mul = container.new_vec_mul(a, b)
        self.assertIn("nn::vector::MUL", vec_mul.full_opcode())


class TestFHECmplr(unittest.TestCase):
    """Test fhe_cmplr bindings."""
    
    def test_import(self):
        """Test that fhe_cmplr can be imported."""
        from ace_dsl.bindings import fhe_cmplr
        self.assertIsNotNone(fhe_cmplr)
    
    def test_opcodes(self):
        """Test opcode constants."""
        from ace_dsl.bindings import fhe_cmplr
        
        self.assertEqual(fhe_cmplr.sihe.ENCODE, "fhe::sihe::ENCODE")
        self.assertEqual(fhe_cmplr.ckks.RESCALE, "fhe::ckks::RESCALE")
        self.assertEqual(fhe_cmplr.poly.NTT, "fhe::poly::NTT")
    
    def test_container_ops(self):
        """Test FHE container operations."""
        from ace_dsl.bindings import fhe_cmplr
        
        # Check if using C++ bindings (loaded from .so file) vs Python mocks
        is_cpp_binding = hasattr(fhe_cmplr, '__file__') and '.so' in str(getattr(fhe_cmplr, '__file__', ''))
        
        container = fhe_cmplr.Container()
        self.assertIsNotNone(container)
        
        if is_cpp_binding:
            # For C++ bindings, just verify methods exist
            self.assertTrue(hasattr(container, 'new_sihe_encode'))
            self.assertTrue(hasattr(container, 'new_ckks_add'))
            return
        
        # Python mock tests
        from ace_dsl.bindings.mock_fhe_cmplr import Node
        plaintext = Node("fhe::sihe", "CONST")
        
        # Test sihe ops
        encoded = container.new_sihe_encode(plaintext)
        self.assertIn("ENCODE", encoded.full_opcode())
        
        # Test ckks ops
        ct = container.new_ckks_add(encoded, encoded)
        rescaled = container.new_ckks_rescale(ct)
        self.assertIn("RESCALE", rescaled.full_opcode())


class TestPassManager(unittest.TestCase):
    """Test passmanager bindings."""
    
    def test_import(self):
        """Test that passmanager can be imported."""
        from ace_dsl.bindings import passmanager
        self.assertIsNotNone(passmanager)
    
    def test_parse_pipeline(self):
        """Test parsing a pipeline string."""
        from ace_dsl.bindings import passmanager
        
        pm = passmanager.PassManager.parse("vector-pass,sihe-pass,ckks-pass")
        self.assertEqual(pm.num_passes(), 3)
    
    def test_run_pipeline(self):
        """Test running a simple pipeline."""
        from ace_dsl.bindings import passmanager
        
        pm = passmanager.PassManager.parse("vector-pass,sihe-pass")
        
        module = passmanager.Module("test")
        module.set_ir("%1 = nn::core::ADD(%a, %b)")
        
        pm.run(module)
        
        ir = module.dump()
        # After sihe-pass, nn::vector should become fhe::sihe
        self.assertIn("fhe::sihe", ir)
    
    def test_ir_printing(self):
        """Test enabling IR printing."""
        from ace_dsl.bindings import passmanager
        
        pm = passmanager.PassManager.parse("vector-pass")
        pm.enable_ir_printing()
        
        module = passmanager.Module("test")
        module.set_ir("test IR")
        
        # This should print IR (manually verify in output)
        pm.run(module)


class TestBindingsIntegration(unittest.TestCase):
    """Integration tests using multiple bindings."""
    
    def test_full_pipeline(self):
        """Test a complete compilation pipeline."""
        from ace_dsl.bindings import passmanager
        
        # Create a simple IR string with nn::core ops
        ir_string = "%1 = nn::core::ADD(%a, %b)\n%2 = nn::core::MUL(%1, %c)"
        
        # Run through passes
        pm = passmanager.PassManager.parse("vector-pass,sihe-pass,ckks-pass,poly-pass")
        
        module = passmanager.Module("kernel")
        module.set_ir(ir_string)
        
        pm.run(module)
        
        # Check final IR has been transformed through all passes
        final_ir = module.dump()
        # After poly-pass, nn::core should become fhe::poly
        self.assertIn("fhe::poly", final_ir)
    
    def test_air_builder_integration(self):
        """Test air_builder operations."""
        from ace_dsl.bindings import air_builder
        
        # Create AIR
        glob = air_builder.create_glob_scope()
        func = glob.new_func("add_kernel")
        container = func.container()
        
        # Create params with proper type (C++ requires elem type for make_array)
        float_type = air_builder.Type.make_float(32)
        array_type = air_builder.Type.make_array([64], float_type)
        
        a = func.new_param("a", array_type)
        b = func.new_param("b", array_type)
        result = container.new_add(a, b)
        container.new_retv(result)
        
        # Verify IR contains expected elements
        ir = glob.dump()
        self.assertIn("add_kernel", ir)
        self.assertIn("ADD", ir)  # May be air::core::ADD or just ADD


class TestMockStatus(unittest.TestCase):
    """Test mock vs real binding status."""
    
    def test_using_mocks(self):
        """Test the using_mocks() function."""
        from ace_dsl.bindings import using_mocks
        
        # Should return True when using mock implementations
        result = using_mocks()
        self.assertIsInstance(result, bool)
        print(f"Using mocks: {result}")


if __name__ == '__main__':
    unittest.main(argv=['first-arg-is-ignored'], exit=False, verbosity=2)

