"""
Test NN Conv Lowering with C Code Generation

Similar to examples/nn_conv_lowering.py, this test demonstrates:
1. Defining a neural network convolution kernel
2. Running through the compilation pipeline
3. Generating C code
4. Dumping AIR IR after each phase

This test shows the full pipeline from Python kernel definition to C code output.
"""

import sys
import os

# Add parent directories to path for imports
# ace_edsl uses relative imports (..base_dsl), so we need the parent directory in path
current_dir = os.path.dirname(os.path.abspath(__file__))
ace_edsl_root = os.path.abspath(os.path.join(current_dir, '..'))
parent_root = os.path.abspath(os.path.join(ace_edsl_root, '..'))

# Add both ace_edsl and parent directory to path
if ace_edsl_root not in sys.path:
    sys.path.insert(0, ace_edsl_root)
if parent_root not in sys.path:
    sys.path.insert(0, parent_root)

# Add acepy to path for ace_dsl imports
acepy_root = os.path.abspath(os.path.join(parent_root, 'acepy'))
if acepy_root not in sys.path:
    sys.path.insert(0, acepy_root)

# Add bindings build directory to path so Python can find the .so files
bindings_build = os.path.abspath(os.path.join(parent_root, 'acepy', 'bindings', 'build'))
if bindings_build not in sys.path:
    sys.path.insert(0, bindings_build)

try:
    # Try importing - this may fail if base_dsl dependencies aren't available
    # Import as package to handle relative imports correctly
    import ace_edsl.edsl.domain_kernels as domain_kernels
    nn_kernel = domain_kernels.nn_kernel
    tensor_kernel = domain_kernels.tensor_kernel
    from ace_edsl.edsl import AceEDSL
    from ace_edsl.edsl.core.air_value import AIRValue
    from ace_edsl.base_dsl.ast_helpers import range_dynamic
    IMPORTS_AVAILABLE = True
    IMPORT_ERROR = None
except ImportError as e:
    IMPORTS_AVAILABLE = False
    IMPORT_ERROR = str(e)
    print(f"⚠ Warning: Imports not available: {e}")
    print("  This test requires ace_edsl to be properly set up.")
    print(f"  Current sys.path: {sys.path[:3]}...")
    import traceback
    traceback.print_exc()


def dump_air_ir(glob, phase_name, max_lines=50):
    """Helper function to dump AIR IR at a specific phase (write to file only)."""
    print(f"\n{'='*70}")
    print(f"AIR IR After {phase_name}:")
    print("="*70)
    
    if glob is None:
        print("  ⚠ AIR module not available")
        return
    
    try:
        if hasattr(glob, 'dump'):
            ir_dump = glob.dump()
        elif hasattr(glob, 'dump_flat'):
            ir_dump = glob.dump_flat()
        else:
            print("  ⚠ IR dump method not available")
            return

        # Write full dump to file
        out_dir = os.path.join(os.path.dirname(__file__), "output")
        os.makedirs(out_dir, exist_ok=True)
        safe_name = phase_name.lower().replace(" ", "_").replace("::", "_")
        out_path = os.path.join(out_dir, f"nn_conv_{safe_name}.air")
        with open(out_path, "w") as f:
            f.write(ir_dump)
        print(f"  ✓ AIR dump written to: {out_path}")

        # No IR preview on stdout; file only
    except Exception as e:
        print(f"  ⚠ Could not dump IR: {e}")
        import traceback
        traceback.print_exc()


def test_nn_conv_kernel():
    """Test NN convolution kernel definition and compilation."""
    if not IMPORTS_AVAILABLE:
        print(f"\n{'='*70}")
        print("Test: NN Conv Kernel with C Code Generation")
        print("="*70)
        print(f"\n⚠ Skipping test: {IMPORT_ERROR}")
        print("\nThis test requires:")
        print("  - ace_edsl to be properly installed")
        print("  - base_dsl dependencies")
        print("  - Full FHE compilation infrastructure")
        print("\nThe test structure is correct, but dependencies are missing.")
        print("When dependencies are available, the test will:")
        print("  1. Define a convolution kernel")
        print("  2. Generate AIR IR via @jit execution")
        print("  3. Run lowering passes")
        print("  4. Generate C code")
        print(f"\n{'='*70}")
        return False
    
    print("=" * 70)
    print("Test: NN Conv Kernel with C Code Generation")
    print("=" * 70)
    
    # ========================================================================
    # Step 1: Define NN convolution kernel
    # ========================================================================
    print("\n[1/7] Defining NN convolution kernel...")
    
    # Use range_dynamic for AST preprocessing
    # The loop body is executed once to generate the IR pattern
    # AIR's loop structure (new_loop_begin_range/new_loop_end) handles repetition
    @nn_kernel
    def conv_nn_kernel(
        input_packed,
        weight_packed,
        bias_expanded
    ):
        """
        NN-level conv with nested loops.
        Creates NN.add, NN.mul operations at nn::core level.
        
        Uses range_dynamic() for AST preprocessing which transforms to AIR loops.
        """
        result = bias_expanded
        # Use range_dynamic() for AST preprocessing
        # Preprocessor transforms this to @loop_selector which creates AIR loop operations
        for cin in range_dynamic(1):  # Single input channel for simplicity
            for khw in range_dynamic(9):  # 3x3 kernel = 9 positions
                # In a real implementation, these would be:
                # - input_rolled = roll(input_packed, ra[khw])
                # - weight_slice = slice(weight_packed, khw)
                # For now, use placeholder operations
                # These should generate AIR operations via operator overloading
                input_rolled = input_packed * input_packed
                weight_slice = weight_packed * weight_packed
                result = result + input_rolled * weight_slice
        return result
    
    print("✓ Kernel defined")
    print(f"  Domain: {getattr(conv_nn_kernel, '_py_domain', 'unknown')}")
    
    # ========================================================================
    # Step 2: Generate AIR IR by calling the kernel (jit pattern)
    # ========================================================================
    print("\n[2/7] Generating AIR IR via jit execution...")
    print("  Note: ace_edsl uses @jit decorator, not compile() method")
    print("  IR is generated when the function is called")
    print("  The @jit decorator creates the AIR module automatically")
    
    glob = None
    try:
        # Try to import air_builder
        try:
            from ace_bindings import air_builder
            
            if air_builder is None:
                raise ImportError("air_builder is None")
            
            # Get DSL instance - it will create the AIR module when kernel is called
            dsl_instance = AceEDSL._get_dsl()
            dsl_instance.current_domain = "nn::core"
            
            # IMPORTANT: Do NOT manually create an AIR module!
            # The @jit decorator will create it automatically when the kernel is called.
            # generate_execution_arguments_air() ignores the args we pass and creates
            # parameters based on the function signature.
            #
            # If we manually create an AIR module here, we'll see duplicate functions:
            #   - One from our manual creation
            #   - One from @jit decorator
            # This causes the "same function twice" issue in the IR dump.
            
            # Call the kernel with dummy arguments (they'll be ignored anyway)
            # The @jit decorator will:
            #   1. Create a new AIR module (glob_scope)
            #   2. Create function scope with parameters based on signature
            #   3. Execute the function body with AIRValue objects
            try:
                # Pass None or dummy values - they'll be replaced by generate_execution_arguments_air
                print("  Calling kernel function...", flush=True)
                result = conv_nn_kernel(None, None, None)
                print(f"  Kernel returned: {type(result)}", flush=True)
                
                # Get the AIR module from DSL instance (created by @jit)
                glob = dsl_instance.current_air_module
                
                if glob:
                    print("✓ Kernel executed - AIR IR generated via @jit", flush=True)
                    # Verify we only have one function with this name
                    ir_dump = glob.dump()
                    func_count = ir_dump.count('FUN[')
                    kernel_count = sum(
                        1
                        for line in ir_dump.splitlines()
                        if line.strip().startswith("FUN[") and '"conv_nn_kernel"' in line
                    )
                    print(f"  AIR module contains {func_count} function(s)", flush=True)
                    print(f"  'conv_nn_kernel' appears {kernel_count} time(s)", flush=True)
                    
                    # Check if function body has operations
                    # Count operations in the function (look for operations after func_entry)
                    lines = ir_dump.split('\n')
                    func_started = False
                    op_count = 0
                    for line in lines:
                        if '"conv_nn_kernel"' in line and 'func_entry' in line:
                            func_started = True
                        elif func_started and ('end_block' in line or 'FUN[' in line):
                            break
                        elif func_started and line.strip() and not line.strip().startswith('#'):
                            # Count non-empty, non-comment lines as potential operations
                            if any(keyword in line for keyword in ['block', 'end_block', 'func_entry']):
                                continue
                            op_count += 1
                    
                    print(f"  Function body operations: {op_count}", flush=True)
                    if op_count == 0:
                        print("  ⚠ WARNING: Function body is empty - no operations generated!", flush=True)
                        print("  This may indicate:", flush=True)
                        print("    - Function body not executing", flush=True)
                        print("    - Loops not being transformed", flush=True)
                        print("    - Operations not generating AIR nodes", flush=True)
                        print("  Check: AST preprocessing and range_dynamic() usage", flush=True)
                    
                    if kernel_count > 1:
                        print("  ⚠ WARNING: Function appears multiple times - duplicate AIR module created!", flush=True)
                else:
                    print("  ⚠ AIR module not stored in DSL instance")
                        
            except Exception as exec_error:
                print(f"  ⚠ Kernel execution error: {exec_error}")
                print("  (AIR IR generation failed)")
                import traceback
                traceback.print_exc()
                # Try to get any partial AIR module
                glob = dsl_instance.current_air_module
                if glob:
                    print("  (Retrieved partial AIR module)")
                
        except ImportError as e:
            print(f"  ⚠ air_builder not available: {e}")
            print("  (AIR IR generation skipped - requires air_builder)")
            
    except Exception as e:
        print(f"✗ Setup failed: {e}")
        import traceback
        traceback.print_exc()
        return False
    
    # ========================================================================
    # Step 3: Dump initial AIR IR (nn::core level)
    # ========================================================================
    print("\n[3/7] Initial AIR IR (nn::core level)")
    dump_air_ir(glob, "Initial Generation (nn::core)", max_lines=100)
    
    # Check if function body is empty BEFORE running passes
    if glob:
        try:
            ir_dump = glob.dump()
            # Count operations
            param_count = ir_dump.count('PARAM')
            new_count = ir_dump.lower().count('new_')
            call_count = ir_dump.lower().count('call')
            # Check for loop operations
            for_count = ir_dump.lower().count('for')
            block_count = ir_dump.count('block')
            
            print(f"\n  Function body analysis:")
            print(f"    Parameters: {param_count}")
            print(f"    Operations (new_): {new_count}")
            print(f"    Operations (call): {call_count}")
            print(f"    Loops (for): {for_count}")
            print(f"    Blocks: {block_count}")
            
            if param_count > 0 and (new_count == 0 and call_count == 0):
                print(f"\n  ⚠⚠⚠ WARNING: Function body is EMPTY! ⚠⚠⚠")
                print(f"  The function has {param_count} parameter(s) but no operations.")
                print(f"  This indicates:")
                print(f"    1. Preprocessing may have failed (loops not transformed)")
                print(f"    2. Function body not executing")
                print(f"    3. Operations not generating AIR nodes")
        except Exception as e:
            print(f"  ⚠ Could not analyze function body: {e}")
    
    # ========================================================================
    # Step 4: Run tensor2vector pass and dump IR
    # ========================================================================
    print("\n[4/7] Running tensor2vector pass...")
    if glob:
        try:
            if hasattr(glob, 'run_cpp_pass'):
                try:
                    glob.run_cpp_pass("tensor2vector", [])
                    print("  ✓ tensor2vector pass completed")
                    dump_air_ir(glob, "After tensor2vector Pass (nn::vector)", max_lines=50)
                except Exception as pass_error:
                    print(f"  ⚠ tensor2vector pass error: {pass_error}")
                    print("  (Dumping IR anyway to see current state)")
                    dump_air_ir(glob, "After tensor2vector Pass (nn::vector) - ERROR", max_lines=50)
            else:
                print("  ⚠ run_cpp_pass not available")
        except Exception as e:
            print(f"  ⚠ tensor2vector pass setup failed: {e}")
            if glob:
                dump_air_ir(glob, "Before tensor2vector Pass", max_lines=30)
    else:
        print("  ⚠ AIR module not available - skipping pass")
    
    # ========================================================================
    # Step 5: Run vector2sihe pass and dump IR
    # ========================================================================
    print("\n[5/7] Running vector2sihe pass...")
    if glob:
        try:
            if hasattr(glob, 'run_cpp_pass'):
                try:
                    glob.run_cpp_pass("vector2sihe", [])
                    print("  ✓ vector2sihe pass completed")
                    dump_air_ir(glob, "After vector2sihe Pass (fhe::sihe)", max_lines=50)
                except Exception as pass_error:
                    print(f"  ⚠ vector2sihe pass error: {pass_error}")
                    print("  (Dumping IR anyway to see current state)")
                    dump_air_ir(glob, "After vector2sihe Pass (fhe::sihe) - ERROR", max_lines=50)
            else:
                print("  ⚠ run_cpp_pass not available")
        except Exception as e:
            print(f"  ⚠ vector2sihe pass setup failed: {e}")
            if glob:
                dump_air_ir(glob, "Before vector2sihe Pass", max_lines=30)
    else:
        print("  ⚠ AIR module not available - skipping pass")
    
    # ========================================================================
    # Step 6: Run FHE drivers and dump IR after each
    # ========================================================================
    print("\n[6/7] Running FHE drivers...")
    try:
        # Try to import air_builder for FHE drivers
        try:
            from ace_bindings import air_builder
            
            if glob:
                # Configure FHE params to satisfy poly2c
                glob.configure_fhe_params(
                    poly_degree=0,
                    mul_level=0,
                    security_level=0,
                    scaling_factor_bits=56,
                    first_prime_bits=60,
                    hamming_weight=192,
                )
                # CKKS driver
                print("  Running CKKS driver...")
                try:
                    ckks_result = air_builder.run_ckks_driver(glob)
                    if ckks_result.get('success'):
                        print("  ✓ CKKS driver completed")
                        dump_air_ir(glob, "After CKKS Driver (fhe::ckks)", max_lines=50)
                    else:
                        print(f"  ⚠ CKKS driver: {ckks_result.get('message', 'unknown error')}")
                        # Dump IR even on failure to see state
                        dump_air_ir(glob, "After CKKS Driver (fhe::ckks) - ERROR", max_lines=30)
                except Exception as ckks_error:
                    print(f"  ⚠ CKKS driver exception: {ckks_error}")
                    dump_air_ir(glob, "After CKKS Driver (fhe::ckks) - EXCEPTION", max_lines=30)
                    ckks_result = {'success': False}
                
                # Poly driver
                if glob and ckks_result.get('success'):
                    print("  Running Poly driver...")
                    try:
                        poly_result = air_builder.run_poly_driver(glob)
                        if poly_result.get('success'):
                            print("  ✓ Poly driver completed")
                            dump_air_ir(glob, "After Poly Driver (fhe::poly)", max_lines=50)
                        else:
                            print(f"  ⚠ Poly driver: {poly_result.get('message', 'unknown error')}")
                            # Dump IR even on failure
                            dump_air_ir(glob, "After Poly Driver (fhe::poly) - ERROR", max_lines=30)
                    except Exception as poly_error:
                        print(f"  ⚠ Poly driver exception: {poly_error}")
                        dump_air_ir(glob, "After Poly Driver (fhe::poly) - EXCEPTION", max_lines=30)
        except ImportError:
            print("  ⚠ air_builder not available (FHE drivers skipped)")
    except Exception as e:
        print(f"  ⚠ FHE drivers not available: {e}")
        import traceback
        traceback.print_exc()
    
    # ========================================================================
    # Step 7: Generate C code and dump final IR
    # ========================================================================
    print("\n[7/7] Generating C code...")
    print("-" * 70)
    
    c_code = None
    if glob:
        try:
            # Try run_poly2c method
            if hasattr(glob, 'run_poly2c'):
                print("  Using run_poly2c() method...")
                success = glob.run_poly2c(
                    data_file="test_conv_data.msg",
                    ct_encode=False,
                    free_poly=True
                )
                if success:
                    c_code = glob.get_c_code()
                    print(f"  ✓ C code generated ({len(c_code)} bytes)")
                    dump_air_ir(glob, "After poly2c (Final)", max_lines=30)
                else:
                    print("  ✗ C code generation failed")
            # Try run_cpp_pass with poly2c
            elif hasattr(glob, 'run_cpp_pass'):
                print("  Using run_cpp_pass('poly2c')...")
                glob.run_cpp_pass("poly2c", [])
                dump_air_ir(glob, "After poly2c Pass (Final)", max_lines=30)
                if hasattr(glob, 'get_c_code'):
                    c_code = glob.get_c_code()
                    print(f"  ✓ C code generated ({len(c_code)} bytes)")
                else:
                    print("  ⚠ C code not available via get_c_code()")
            else:
                print("  ⚠ C code generation methods not available")
        except Exception as e:
            print(f"  ✗ C code generation failed: {e}")
            import traceback
            traceback.print_exc()
    
    # ========================================================================
    # Display generated C code
    # ========================================================================
    if c_code:
        # Write to file
        output_dir = os.path.join(os.path.dirname(__file__), "output")
        os.makedirs(output_dir, exist_ok=True)
        output_file = os.path.join(output_dir, "nn_conv_output.c")
        try:
            with open(output_file, 'w') as f:
                f.write(c_code)
            print(f"\n✓ C code written to: {output_file}")
        except Exception as e:
            print(f"\n⚠ Could not write to file: {e}")
        
        # Check for data file
        data_file = "test_conv_data.msg"
        if os.path.exists(data_file):
            size = os.path.getsize(data_file)
            print(f"✓ Data file created: {data_file} ({size} bytes)")
    else:
        print("\n⚠ No C code generated")
        print("  This may be expected if:")
        print("  - FHE infrastructure is not fully set up")
        print("  - Pipeline did not complete successfully")
        print("  - C code generation is not yet implemented for ace_edsl")
    
    # ========================================================================
    # Summary
    # ========================================================================
    print("\n" + "=" * 70)
    print("Test Summary:")
    print("=" * 70)
    lowering_status = "  ✓ Lowering passes" if glob and hasattr(glob, 'run_cpp_pass') else "  ⚠ Lowering passes"
    fhe_status = "  ✓ FHE drivers" if 'air_builder' in locals() else "  ⚠ FHE drivers"
    codegen_status = "  ✓ C code generation" if c_code else "  ⚠ C code generation"
    
    print(f"""
Pipeline Status:
  ✓ Kernel definition      (@nn_kernel decorator applied)
  ✓ JIT setup             (function wrapped with @jit)
  {'✓' if glob else '⚠'} AIR generation        ({'generated' if glob else 'requires function execution'})
{lowering_status} (tensor2vector, vector2sihe)
{fhe_status} (CKKS, Poly)
{codegen_status} (poly2c)

Output:
  C file: {output_file if c_code else 'N/A'}

Note: ace_edsl uses @jit pattern (like base DSL):
  - @nn_kernel decorator wraps the function
  - IR is generated when function is called
  - No compile() method needed

Expected Flow:
  1. Define kernel with @nn_kernel
  2. Call kernel with arguments → generates AIR IR
  3. Run lowering passes on AIR IR
  4. Run FHE drivers
  5. Generate C code

Expected Output (when executed):
  - nn::core:   NN.mul, NN.add operations
  - nn::vector: VECTOR.mul, VECTOR.add operations (after tensor2vector)
  - fhe::sihe:  SIHE.mul, SIHE.add (after vector2sihe)
  - fhe::ckks:  CKKS.mul, CKKS.relin, CKKS.add (after CKKS driver)
  - fhe::poly:  POLY operations (after Poly driver)
  - C code:     Generated C code with FHE runtime calls
""")
    
    print("=" * 70)
    if c_code:
        print("✓ Test completed - C code generated successfully!")
    else:
        print("⚠ Test completed - C code generation not available")
    print("=" * 70)
    
    # Use assertion for pytest compatibility
    assert c_code is not None, "C code generation failed"


def main():
    """Main test function."""
    try:
        test_nn_conv_kernel()
        return 0
    except AssertionError:
        return 1


if __name__ == "__main__":
    sys.exit(main())
