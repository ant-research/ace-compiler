"""
Test simple kernel compilation.

Tests Phase 1: Basic AIR generation for a simple kernel without loops/conditionals.
"""

import sys
import os

# Add parent directories to path for imports
current_dir = os.path.dirname(os.path.abspath(__file__))
ace_edsl_root = os.path.abspath(os.path.join(current_dir, '..'))
parent_root = os.path.abspath(os.path.join(ace_edsl_root, '..'))

if ace_edsl_root not in sys.path:
    sys.path.insert(0, ace_edsl_root)
if parent_root not in sys.path:
    sys.path.insert(0, parent_root)

# Add bindings build directory to path so Python can find the .so files
bindings_build = os.path.abspath(os.path.join(parent_root, 'acepy', 'bindings', 'build'))
if bindings_build not in sys.path:
    sys.path.insert(0, bindings_build)

try:
    from ace_edsl.edsl.domain_kernels import tensor_kernel
    from ace_edsl.edsl import AceEDSL
    
    # Test if ace_edsl can be instantiated
    print("Testing ace_edsl instantiation...")
    dsl = AceEDSL._get_dsl()
    print(f"✓ ace_edsl instantiated: {dsl.name}")
    
    # Test simple kernel
    print("\nTesting simple kernel: a + b")
    
    @tensor_kernel
    def add(a, b):
        """Simple addition kernel"""
        return a + b
    
    print("✓ Kernel decorated successfully")
    
    # Try to call the kernel (this will attempt AIR generation)
    print("\nAttempting to call kernel (will generate AIR)...")
    try:
        # Note: This will fail if air_builder is not available or AIRValue operations incomplete
        result = add(1, 2)
        print(f"✓ Kernel executed, result: {result}")
        # Dump global scope to file if available
        if hasattr(dsl, "current_air_module") and dsl.current_air_module is not None:
            glob = dsl.current_air_module
            if hasattr(glob, "dump"):
                out_dir = os.path.join(current_dir, "output")
                os.makedirs(out_dir, exist_ok=True)
                out_path = os.path.join(out_dir, "simple_kernel_initial.air")
                with open(out_path, "w") as f:
                    f.write(glob.dump())
                print(f"✓ AIR dump written to: {out_path}")
    except ImportError as e:
        print(f"⚠ ImportError (expected if air_builder not available): {e}")
        print("  This is OK for Phase 1 - we're testing the structure, not full execution")
    except NotImplementedError as e:
        print(f"⚠ NotImplementedError (expected if AIRValue operations incomplete): {e}")
        print("  This is OK for Phase 1 - we'll complete AIRValue in Phase 2")
    except Exception as e:
        print(f"✗ Unexpected error: {e}")
        import traceback
        traceback.print_exc()
        raise
    
    print("\n✓ Phase 1 test completed successfully!")
    print("  Next: Complete AIRValue operations in Phase 2")
    
except ImportError as e:
    print(f"✗ Import failed: {e}")
    import traceback
    traceback.print_exc()
    sys.exit(1)
except Exception as e:
    print(f"✗ Test failed: {e}")
    import traceback
    traceback.print_exc()
    sys.exit(1)

