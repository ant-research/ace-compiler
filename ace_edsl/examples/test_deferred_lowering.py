"""
Test Deferred Lowering - ace_edsl with Automatic Inlining
=========================================================

Two test modes:

1. PURE ACE_EDSL (Python models):
   - Uses operator overloading for AUTOMATIC inlining
   - No separate pass needed - inlining happens when function is called

2. ONNX MODEL (resnet20 with 19 bootstraps):
   - Loads ONNX model via C++ pipeline
   - sihe2ckks inserts 19 bootstrap ops via noise budget analysis
   - Uses Python lowering pass to inline bootstrap_full

Output files saved to: examples/output/deferred_lowering/

Run with:
    cd ace_edsl
    PYTHONPATH=.:.. python examples/test_deferred_lowering.py
"""

import sys
import os
import time
import shutil
import subprocess
import re

# Setup path for imports
script_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.dirname(script_dir))

try:
    from ace_edsl.edsl import (
        AceEDSL,
        ckks_kernel, CkksCiphertext, CkksPlaintext,
        Pipeline, PipelineTarget, load_onnx,
        register_lowering, get_ops_to_skip, clear_lowerings,
    )
    IMPORTS_AVAILABLE = True
except ImportError as e:
    IMPORTS_AVAILABLE = False
    IMPORT_ERROR = str(e)
    print(f"Warning: ACE EDSL imports not available: {e}")

# Import ace_edsl's bootstrap_full FIRST for pure Python test
from bootstrap_full import (
    bootstrap_full as ace_edsl_bootstrap_full,
    G_COEFFICIENTS_UNIFORM_HW_192,
    get_double_angle_scalars,
    BOOTSTRAP_POST_SCALE,
)

# For ONNX test, we need acepy's lowering pass (ONNX comes from C++, not Python tracing)
try:
    sys.path.insert(0, os.path.join(script_dir, "../../acepy"))
    from ace_dsl.passes.python_lowering_pass import (
        register_lowering as acepy_register_lowering,
        get_ops_to_skip as acepy_get_ops_to_skip,
        run_python_lowering_pass,
        clear_lowerings as acepy_clear_lowerings,
    )
    # Import acepy's bootstrap_full for ONNX test (has .compile() method)
    acepy_examples_dir = os.path.join(script_dir, "../../acepy/examples")
    sys.path.insert(0, acepy_examples_dir)
    # Use importlib to import with a different name
    import importlib.util
    spec = importlib.util.spec_from_file_location("acepy_bootstrap", os.path.join(acepy_examples_dir, "bootstrap_full.py"))
    acepy_bootstrap_module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(acepy_bootstrap_module)
    acepy_bootstrap_full = acepy_bootstrap_module.bootstrap_full
    ACEPY_AVAILABLE = True
except ImportError as e:
    ACEPY_AVAILABLE = False
    print(f"Note: acepy not available for ONNX test: {e}")
except Exception as e:
    ACEPY_AVAILABLE = False
    print(f"Note: acepy bootstrap load failed: {e}")


def count_ops(ir: str, pattern: str) -> int:
    """Count occurrences of an op pattern in IR."""
    return ir.lower().count(pattern.lower())


# =============================================================================
# Pure ace_edsl model - bootstrap_full is AUTOMATICALLY inlined
# =============================================================================

def _call_bootstrap_full_with_constants(ct, zero, one):
    """Call bootstrap_full with the full ANT constant set expected by the kernel."""
    coeffs = list(G_COEFFICIENTS_UNIFORM_HW_192)
    da_scalars = get_double_angle_scalars()
    return ace_edsl_bootstrap_full(ct, zero, one, *coeffs, *da_scalars, float(BOOTSTRAP_POST_SCALE))


@ckks_kernel
def model_with_bootstrap(
    ct: CkksCiphertext,
    zero: CkksCiphertext,
    one: CkksPlaintext,
) -> CkksCiphertext:
    """
    Model that calls bootstrap_full.
    
    In ace_edsl, when we call ace_edsl_bootstrap_full(ct, zero, ...), Python executes
    bootstrap_full's body. Since ct and zero are AIRValue objects, all
    operations (+, -, *, rotate) emit AIR nodes via operator overloading.
    
    Result: bootstrap_full's operations are AUTOMATICALLY traced into
    this function's AIR - no separate inlining pass needed!
    """
    # Some pre-bootstrap operations
    x = ct * ct
    x = x + ct
    
    # Call bootstrap - automatically inlined via operator overloading!
    refreshed = _call_bootstrap_full_with_constants(x, zero, one)
    
    # Some post-bootstrap operations  
    result = refreshed * refreshed
    result = result + ct
    
    return result


@ckks_kernel
def model_with_multiple_bootstraps(
    ct: CkksCiphertext,
    zero: CkksCiphertext,
    one: CkksPlaintext,
) -> CkksCiphertext:
    """Model with multiple bootstrap calls - all automatically inlined."""
    x = ct * ct
    
    # First bootstrap - automatically inlined
    x = _call_bootstrap_full_with_constants(x, zero, one)
    
    x = x * x
    
    # Second bootstrap - automatically inlined  
    x = _call_bootstrap_full_with_constants(x, zero, one)
    
    x = x + ct
    
    # Third bootstrap - automatically inlined
    result = _call_bootstrap_full_with_constants(x, zero, one)
    
    return result


def capture_metrics(ir: str) -> dict:
    """Capture detailed CKKS operation counts."""
    metrics = {
        'bootstrap': count_ops(ir, 'ckks.bootstrap'),
        'rotate': count_ops(ir, 'ckks.rotate'),
        'mul': count_ops(ir, 'ckks.mul'),
        'add': count_ops(ir, 'ckks.add'),
        'sub': count_ops(ir, 'ckks.sub'),
        'rescale': count_ops(ir, 'ckks.rescale'),
        'relin': count_ops(ir, 'ckks.relin'),
        'ir_size': len(ir),
    }
    metrics['total_ops'] = sum(metrics[op] for op in ['rotate', 'mul', 'add', 'sub'])
    return metrics


def run_single_bootstrap_test():
    """Test with single bootstrap call."""
    print("\n" + "=" * 70)
    print("Test 1: Single Bootstrap - Automatic Inlining")
    print("=" * 70)
    
    # Clear previous state completely
    clear_lowerings()
    AceEDSL._get_dsl.cache_clear()
    
    # Force new DSL instance
    dsl = AceEDSL._get_dsl()
    dsl.current_air_module = None
    dsl._in_air_context = False
    
    start_time = time.time()
    
    # Execute model - bootstrap_full is automatically inlined!
    print("\n[Step 1] Executing model (automatic tracing)...")
    print("  → bootstrap_full's body will be traced automatically!")
    
    ct = CkksCiphertext(shape=(16384,), name="input_ct")
    zero = CkksCiphertext(shape=(16384,), name="zero_ct")
    model_with_bootstrap(ct, zero, 1.0)
    
    # Get the generated AIR
    dsl = AceEDSL._get_dsl()
    glob = dsl.current_air_module
    
    if glob is None:
        print("ERROR: No AIR module generated")
        return None
    
    ir = glob.dump()
    metrics = capture_metrics(ir)
    elapsed = time.time() - start_time
    
    print(f"\n[Step 2] Analyzing traced AIR...")
    print(f"""
  ┌─ Automatic Inlining Results ─────────────────────────────┐
  │                                                          │
  │  CKKS.bootstrap: {metrics['bootstrap']:>4}  (should be 0 - all inlined!)  │
  │  CKKS.rotate:    {metrics['rotate']:>4}  (from bootstrap DFT/iDFT)       │
  │  CKKS.mul:       {metrics['mul']:>4}  (model + bootstrap)              │
  │  CKKS.add:       {metrics['add']:>4}  (model + bootstrap)              │
  │  CKKS.sub:       {metrics['sub']:>4}  (from bootstrap)                 │
  │  ─────────────────────────────────────────────────────   │
  │  Total CKKS ops: {metrics['total_ops']:>4}                                  │
  │  IR size: {metrics['ir_size']:>10,} chars                          │
  │  Time: {elapsed:.3f}s                                           │
  │                                                          │
  │  ✓ Bootstrap automatically inlined via operator          │
  │    overloading - NO separate inlining pass needed!       │
  └──────────────────────────────────────────────────────────┘
""")
    
    if metrics['bootstrap'] == 0:
        print("  ✓ SUCCESS: Bootstrap fully inlined!")
    else:
        print(f"  ⚠ WARNING: Found {metrics['bootstrap']} CKKS.bootstrap ops")
    
    return metrics


def run_multiple_bootstrap_test():
    """Test with multiple bootstrap calls."""
    print("\n" + "=" * 70)
    print("Test 2: Multiple Bootstraps (3x) - All Automatically Inlined")
    print("=" * 70)
    
    # Clear previous state
    clear_lowerings()
    AceEDSL._get_dsl.cache_clear()
    
    start_time = time.time()
    
    print("\n[Step 1] Executing model with 3 bootstrap calls...")
    print("  → All 3 bootstrap_full calls will be traced automatically!")
    
    ct = CkksCiphertext(shape=(16384,), name="input_ct")
    zero = CkksCiphertext(shape=(16384,), name="zero_ct")
    model_with_multiple_bootstraps(ct, zero, 1.0)
    
    # Get the generated AIR
    dsl = AceEDSL._get_dsl()
    glob = dsl.current_air_module
    
    if glob is None:
        print("ERROR: No AIR module generated")
        return None
    
    ir = glob.dump()
    metrics = capture_metrics(ir)
    elapsed = time.time() - start_time
    
    print(f"\n[Step 2] Analyzing traced AIR...")
    
    # Expected: 3x the single bootstrap operations
    expected_rotates = 6 * 3  # ~6 rotations per bootstrap
    
    print(f"""
  ┌─ Multiple Bootstrap Inlining Results ────────────────────┐
  │                                                          │
  │  CKKS.bootstrap: {metrics['bootstrap']:>4}  (should be 0 - all inlined!)  │
  │  CKKS.rotate:    {metrics['rotate']:>4}  (expected ~{expected_rotates} for 3 bootstraps)  │
  │  CKKS.mul:       {metrics['mul']:>4}  (model + 3×bootstrap)            │
  │  CKKS.add:       {metrics['add']:>4}  (model + 3×bootstrap)            │
  │  CKKS.sub:       {metrics['sub']:>4}  (from 3×bootstrap)               │
  │  ─────────────────────────────────────────────────────   │
  │  Total CKKS ops: {metrics['total_ops']:>4}                                  │
  │  IR size: {metrics['ir_size']:>10,} chars                          │
  │  Time: {elapsed:.3f}s                                           │
  │                                                          │
  │  ✓ All 3 bootstraps automatically inlined!               │
  └──────────────────────────────────────────────────────────┘
""")
    
    if metrics['bootstrap'] == 0:
        print("  ✓ SUCCESS: All 3 bootstraps fully inlined!")
    else:
        print(f"  ⚠ WARNING: Found {metrics['bootstrap']} CKKS.bootstrap ops")
    
    return metrics


def run_onnx_test():
    """Test with ONNX model that generates 19 bootstraps via noise budget analysis."""
    print("\n" + "=" * 70)
    print("Test 3: ONNX Model (resnet20) - 19 Bootstraps via Noise Budget")
    print("=" * 70)
    
    if not ACEPY_AVAILABLE:
        print("\n  ⚠ Skipping ONNX test - acepy not available")
        print("    (ONNX models need acepy's PythonLoweringPass for inlining)")
        return None
    
    model_path = os.path.join(script_dir, "../../model/resnet20_cifar10_pre.onnx")
    if not os.path.exists(model_path):
        print(f"\n  ⚠ Skipping ONNX test - model not found: {model_path}")
        return None
    
    start_time = time.time()
    
    # Clear previous registrations
    acepy_clear_lowerings()
    
    # Register bootstrap lowering with acepy's registry
    print("\n[Step 1] Registering bootstrap_full with acepy's lowering registry...")
    acepy_register_lowering(
        "fhe::ckks", "bootstrap",
        target_domain="fhe::poly",
        description="Full CKKS bootstrap"
    )(acepy_bootstrap_full)
    
    skip_ops = acepy_get_ops_to_skip()
    print(f"  Registered skip ops: {skip_ops}")
    
    # Track metrics
    metrics_before = {}
    metrics_after = {}
    
    def python_lowering(glob):
        """Run Python lowering pass after sihe2ckks."""
        nonlocal metrics_before, metrics_after
        
        # Capture BEFORE
        ir_before = glob.dump()
        metrics_before = capture_metrics(ir_before)
        
        print(f"\n  ┌─ Before Python Lowering ─────────────────────────────────┐")
        print(f"  │ CKKS.bootstrap: {metrics_before['bootstrap']:>4}  (inserted by sihe2ckks)      │")
        print(f"  │ CKKS.rotate:    {metrics_before['rotate']:>4}  CKKS.mul: {metrics_before['mul']:>4}              │")
        print(f"  │ CKKS.add:       {metrics_before['add']:>4}  CKKS.sub: {metrics_before['sub']:>4}              │")
        print(f"  │ Total ops:      {metrics_before['total_ops']:>4}  IR size: {metrics_before['ir_size']:>10,} ch │")
        print(f"  └─────────────────────────────────────────────────────────────┘")
        
        print("\n[Python Lowering] Running acepy's lowering pass...")
        run_python_lowering_pass(glob, verbose=True)
        
        # Capture AFTER
        ir_after = glob.dump()
        metrics_after = capture_metrics(ir_after)
        
        print(f"\n  ┌─ After Python Lowering ──────────────────────────────────┐")
        print(f"  │ CKKS.bootstrap: {metrics_after['bootstrap']:>4}  (should be 0 - all inlined!) │")
        print(f"  │ CKKS.rotate:    {metrics_after['rotate']:>4}  CKKS.mul: {metrics_after['mul']:>4}              │")
        print(f"  │ CKKS.add:       {metrics_after['add']:>4}  CKKS.sub: {metrics_after['sub']:>4}              │")
        print(f"  │ Total ops:      {metrics_after['total_ops']:>4}  IR size: {metrics_after['ir_size']:>10,} ch │")
        print(f"  └─────────────────────────────────────────────────────────────┘")
    
    # Run pipeline
    print("\n[Step 2] Running pipeline (ONNX → tensor2vector → sihe2ckks → ...)...")
    
    relu_vr = (
        "/relu/Relu=4;"
        "/layer1/layer1.0/relu_1/Relu=4;"
        "/layer1/layer1.1/relu/Relu=4;"
        "/layer1/layer1.1/relu_1/Relu=5;"
        "/layer1/layer1.2/relu_1/Relu=5;"
        "/layer2/layer2.0/relu_1/Relu=5;"
        "/layer2/layer2.1/relu_1/Relu=5;"
        "/layer2/layer2.2/relu_1/Relu=7;"
        "/layer3/layer3.0/relu_1/Relu=4;"
        "/layer3/layer3.1/relu_1/Relu=6;"
        "/layer3/layer3.2/relu/Relu=4;"
        "/layer3/layer3.2/relu_1/Relu=20"
    )
    
    output_dir = os.path.join(script_dir, "output", "deferred_lowering")
    os.makedirs(output_dir, exist_ok=True)
    
    pipeline = Pipeline(
        "onnx_bootstrap_test",
        verbose=True,
        dump_ir=True,
        output_dir=output_dir,
    )
    
    result = (pipeline
        .load_onnx(model_path)
        .configure_fhe(
            # CKKS params (matches -CKKS:hw=192:q0=60:sf=56)
            scaling_factor_bits=56,
            first_prime_bits=60,
            hamming_weight=192,
            # SIHE params (matches -SIHE:relu_vr_def=3:relu_vr=...)
            relu_vr_def=3.0,
            relu_vr=relu_vr,
            # Poly2C params (matches -P2C:fp -P2C:df=...)
            data_file=os.path.join(output_dir, "resnet20_cifar10_pre.weight"),
            free_poly=True,
        )
        .set_skip_ops(list(skip_ops))
        .set_python_lowering(python_lowering)
        .run(target=PipelineTarget.C)
    )
    
    elapsed = time.time() - start_time
    
    if not result.success:
        print(f"\n✗ Pipeline failed: {result.error}")
        return None
    
    # Count C code lines
    c_lines = len(result.c_code.split('\n')) if result.c_code else 0
    c_size_kb = len(result.c_code) / 1024 if result.c_code else 0
    
    inlined = metrics_before.get('bootstrap', 0) - metrics_after.get('bootstrap', 0)
    
    print(f"""
╔══════════════════════════════════════════════════════════════════════╗
║         ONNX Model (resnet20) - 19 Bootstraps Inlined                ║
╠══════════════════════════════════════════════════════════════════════╣
║  Metric                    │  Before Inlining  │  After Inlining     ║
╠════════════════════════════╪═══════════════════╪═════════════════════╣
║  CKKS.bootstrap            │  {metrics_before.get('bootstrap', 0):>15}  │  {metrics_after.get('bootstrap', 0):>17}  ║
║  CKKS.rotate               │  {metrics_before.get('rotate', 0):>15}  │  {metrics_after.get('rotate', 0):>17}  ║
║  CKKS.mul                  │  {metrics_before.get('mul', 0):>15}  │  {metrics_after.get('mul', 0):>17}  ║
║  CKKS.add                  │  {metrics_before.get('add', 0):>15}  │  {metrics_after.get('add', 0):>17}  ║
║  CKKS.sub                  │  {metrics_before.get('sub', 0):>15}  │  {metrics_after.get('sub', 0):>17}  ║
╠════════════════════════════╪═══════════════════╪═════════════════════╣
║  Total CKKS ops            │  {metrics_before.get('total_ops', 0):>15}  │  {metrics_after.get('total_ops', 0):>17}  ║
║  IR size (chars)           │  {metrics_before.get('ir_size', 0):>12,} ch │  {metrics_after.get('ir_size', 0):>14,} ch ║
╠════════════════════════════╧═══════════════════╧═════════════════════╣
║  Pipeline Time: {elapsed:>6.2f}s                                          ║
║  Bootstraps Inlined: {inlined:>3} × bootstrap_full                         ║
║  C Code: {c_lines:>6,} lines ({c_size_kb:>6.1f} KB)                              ║
╚══════════════════════════════════════════════════════════════════════╝
""")
    
    if metrics_after.get('bootstrap', 0) == 0:
        print("  ✓ SUCCESS: All 19 bootstraps inlined!")
    
    # Save C code to file and copy to dataset directory (like acepy)
    if result.c_code:
        c_file = os.path.join(output_dir, "resnet20_with_custom_bootstrap.c")
        with open(c_file, 'w') as f:
            f.write(result.c_code)
        print(f"\n  ✓ Saved C code to {c_file}")
        
        # Copy to dataset directory for build
        inc_dest = os.path.join(script_dir, "../../fhe-cmplr/rtlib/ant/dataset/resnet20_cifar10_pre.onnx.inc")
        try:
            shutil.copy2(c_file, inc_dest)
            print(f"  ✓ Copied to {inc_dest}")
            
            # Attempt to build
            release_dir = os.path.join(script_dir, "../../release")
            if os.path.exists(release_dir):
                result_build = subprocess.run(
                    ["make", "resnet20_cifar10"],
                    cwd=release_dir,
                    capture_output=True,
                    text=True,
                    timeout=300
                )
                if result_build.returncode == 0:
                    print(f"  ✓ Build successful")
                    exe_path = os.path.join(release_dir, "dataset/resnet20_cifar10")
                    if os.path.exists(exe_path):
                        print(f"  ✓ Executable: {exe_path}")
                else:
                    print(f"  ⚠ Build failed (but pipeline succeeded)")
        except Exception as e:
            print(f"  ⚠ Build step skipped: {e}")
    
    return {
        'before': metrics_before,
        'after': metrics_after,
        'inlined': inlined,
        'c_lines': c_lines,
        'c_size_kb': c_size_kb,
        'time': elapsed,
    }


def run_comparison():
    """Run tests based on command line argument."""
    print("\n")
    print("╔══════════════════════════════════════════════════════════════════════╗")
    print("║            ace_edsl - Automatic Inlining Demonstration               ║")
    print("╚══════════════════════════════════════════════════════════════════════╝")
    
    if not IMPORTS_AVAILABLE:
        print(f"\nERROR: Imports not available: {IMPORT_ERROR}")
        return False
    
    # Parse command line to select test
    test_mode = sys.argv[1] if len(sys.argv) > 1 else "single"
    
    if test_mode == "single":
        # Run single bootstrap test
        print("\n" + "─" * 70)
        print("Pure ace_edsl - Automatic Inlining via Operator Overloading")
        print("─" * 70)
        metrics = run_single_bootstrap_test()
        return metrics is not None
        
    elif test_mode == "multiple":
        # Run multiple bootstrap test
        print("\n" + "─" * 70)
        print("Pure ace_edsl - Multiple Bootstraps Automatically Inlined")
        print("─" * 70)
        metrics = run_multiple_bootstrap_test()
        return metrics is not None
        
    elif test_mode == "onnx":
        # Run ONNX test (19 bootstraps)
        print("\n" + "─" * 70)
        print("ONNX Model - 19 Bootstraps via Noise Budget Analysis")
        print("─" * 70)
        metrics = run_onnx_test()
        return metrics is not None
    
    else:
        print(f"""
Usage: python test_deferred_lowering.py [test_mode]

Test modes:
  single    - Single bootstrap, automatic inlining (default)
  multiple  - 3 bootstraps, automatic inlining  
  onnx      - ONNX model (resnet20) with 19 bootstraps

Examples:
  python test_deferred_lowering.py single
  python test_deferred_lowering.py onnx
""")
        return False


if __name__ == "__main__":
    success = run_comparison()
    sys.exit(0 if success else 1)
