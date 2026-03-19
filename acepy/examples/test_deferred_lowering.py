"""
Test Deferred Lowering - Full Pipeline to C Code
=================================================

Demonstrates the selective lowering mechanism with full function inlining,
going through the complete FHE compilation pipeline to generate C code.

Equivalent to native compiler command:
    fhe_cmplr model/resnet20_cifar10_pre.onnx \
        -CKKS:hw=192:q0=60:sf=56:sbm \
        -SIHE:relu_vr_def=3:relu_vr=/relu/Relu=4;... \
        -P2C:fp -P2C:lib=ant \
        -P2C:df=resnet20_cifar10_pre.weight \
        -o resnet20_cifar10_pre.c

Python API equivalent:
    configure_fhe(
        scaling_factor_bits=56,   # -CKKS:sf=56
        first_prime_bits=60,      # -CKKS:q0=60
        hamming_weight=192,       # -CKKS:hw=192
        relu_vr_def=3.0,          # -SIHE:relu_vr_def=3
        relu_vr="...",            # -SIHE:relu_vr=...
        data_file="...weight",    # -P2C:df=...
        free_poly=True,           # -P2C:fp
    )

Note: Some native options are not yet exposed in Python API:
    - -CKKS:sbm (scale by modulus)
    - -P2C:lib=ant (library selection)
    - Trace options (-O2A:ts, -VEC:ts:rtt, etc.)

The pipeline:
  1. Register lowering (NOT compiled yet)
  2. Load ONNX model
  3. Run C++ passes with skip_ops
  4. Python lowering pass compiles and inlines bootstrap
  5. Run CKKS driver → Poly driver → poly2c

Output files saved to: examples/output/deferred_lowering/

Run with:
    cd acepy
    PYTHONPATH=.:examples python examples/test_deferred_lowering.py
"""

import sys
import os
import time
import shutil
import subprocess
import re
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from ace_dsl.passes.python_lowering_pass import (
    register_lowering, 
    get_ops_to_skip,
    run_python_lowering_pass,
    clear_lowerings,
)
from ace_dsl.frontend.domain_kernels import ckks_kernel, CkksCiphertext, ckks
from dump_utils import Pipeline, PipelineTarget

# Import the full bootstrap implementation
from bootstrap_full import bootstrap_full


def count_ops(ir: str, pattern: str) -> int:
    return ir.lower().count(pattern.lower())


def deduplicate_c_functions(c_code: str) -> str:
    """Remove duplicate function definitions from generated C code."""
    func_pattern = re.compile(
        r'^((?:CIPHERTEXT|PLAINTEXT|void|int|uint32_t)\s+(\w+)\s*\([^)]*\)\s*\{)',
        re.MULTILINE
    )
    
    seen_funcs = {}
    lines = c_code.split('\n')
    result_lines = []
    skip_until_close = False
    brace_count = 0
    
    i = 0
    while i < len(lines):
        line = lines[i]
        
        if skip_until_close:
            brace_count += line.count('{') - line.count('}')
            if brace_count <= 0:
                skip_until_close = False
            i += 1
            continue
        
        match = func_pattern.match(line)
        if match:
            func_name = match.group(2)
            if func_name in seen_funcs:
                skip_until_close = True
                brace_count = line.count('{') - line.count('}')
                if brace_count <= 0:
                    skip_until_close = False
                i += 1
                continue
            else:
                seen_funcs[func_name] = True
        
        decl_pattern = re.compile(r'^((?:CIPHERTEXT|PLAINTEXT|void|int|uint32_t)\s+(\w+)\s*\([^)]*\)\s*;)')
        decl_match = decl_pattern.match(line)
        if decl_match:
            func_name = decl_match.group(2)
            decl_key = f"decl_{func_name}"
            if decl_key in seen_funcs:
                i += 1
                continue
            seen_funcs[decl_key] = True
        
        result_lines.append(line)
        i += 1
    
    return '\n'.join(result_lines)


def run_test():
    print("=" * 70)
    print("Deferred Lowering Test - Full Pipeline with Pipeline Class")
    print("=" * 70)
    
    script_dir = os.path.dirname(os.path.abspath(__file__))
    model_path = os.path.join(script_dir, "../../model/resnet20_cifar10_pre.onnx")
    
    if not os.path.exists(model_path):
        print(f"ERROR: Model not found at {model_path}")
        return False
    
    start_time = time.time()
    
    # Clear any previous registrations
    clear_lowerings()
    
    # =========================================================================
    # Step 1: Register bootstrap lowering (NOT compiled yet)
    # =========================================================================
    print("\n[Step 1] Registering bootstrap lowering...")
    
    register_lowering(
        "fhe::ckks", "bootstrap", 
        target_domain="fhe::poly",
        description="Full CKKS bootstrap with DFT/EvalMod/iDFT"
    )(bootstrap_full)
    
    skip_ops = get_ops_to_skip()
    print(f"  Registered skip ops: {skip_ops}")
    
    # =========================================================================
    # Step 2: Track bootstrap counts at each phase
    # =========================================================================
    bootstrap_counts = {}
    
    def track_bootstrap(phase, ir):
        bootstrap_counts[phase] = count_ops(ir, 'ckks.bootstrap')
        rotate_count = count_ops(ir, 'ckks.rotate')
        if bootstrap_counts[phase] > 0 or rotate_count > 100:
            print(f"  [{phase}] CKKS.bootstrap: {bootstrap_counts[phase]}, CKKS.rotate: {rotate_count}")
    
    # =========================================================================
    # Step 3: Define Python lowering callback
    # =========================================================================
    def python_lowering(glob):
        """Run Python lowering pass after sihe2ckks."""
        print("\n[Python Lowering] Running Python lowering pass...")
        print("  (This compiles and inlines bootstrap_full)")
        run_python_lowering_pass(glob, verbose=True)
    
    # =========================================================================
    # Step 4: Create pipeline and run
    # =========================================================================
    print("\n[Step 2] Creating pipeline and running...")
    
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
    
    pipeline = Pipeline(
        "deferred_lowering",
        verbose=True,
        on_phase_complete=track_bootstrap,
    )
    
    # Data file path (matches native compiler: -P2C:df=resnet20_cifar10_pre.weight)
    output_dir = pipeline.dumper.get_output_dir() if pipeline.dumper else "."
    data_file = os.path.join(output_dir, "resnet20_cifar10_pre.weight")
    
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
            data_file=data_file,
            free_poly=True,
        )
        .set_skip_ops(list(skip_ops))
        .set_python_lowering(python_lowering)
        .run(target=PipelineTarget.C)
    )
    
    if not result.success:
        print(f"\n✗ Pipeline failed: {result.error}")
        return False
    
    # =========================================================================
    # Step 5: Post-process C code
    # =========================================================================
    print("\n[Step 3] Post-processing C code...")
    
    output_dir = pipeline.dumper.get_output_dir()
    c_file = os.path.join(output_dir, "resnet20_with_custom_bootstrap.c")
    
    original_size = len(result.c_code)
    c_code = deduplicate_c_functions(result.c_code)
    dedup_size = len(c_code)
    
    print(f"  Deduplicated: {original_size:,} → {dedup_size:,} bytes ({original_size - dedup_size:,} removed)")
    
    with open(c_file, 'w') as f:
        f.write(c_code)
    print(f"  Written: {c_file}")
    
    # =========================================================================
    # Step 6: Summary
    # =========================================================================
    total_time = time.time() - start_time
    
    pipeline.print_summary()
    
    bootstrap_before = bootstrap_counts.get("sihe2ckks", 0)
    bootstrap_after = bootstrap_counts.get("python_lower", bootstrap_counts.get("ckks_driver", 0))
    
    print(f"""
Full pipeline completed in {total_time:.2f}s:

  Phases completed: {result.phases_completed}
  
  Bootstrap replacement:
    - Before Python lowering: {bootstrap_before} CKKS.bootstrap
    - After Python lowering:  {bootstrap_after} CKKS.bootstrap
    - Inlined: {bootstrap_before - bootstrap_after} × bootstrap_full

  Output:
    - C code: {len(c_code):,} bytes
    - Data file: {result.data_file}
""")
    
    # =========================================================================
    # Step 7: Build executable (optional)
    # =========================================================================
    print("[Step 4] Building executable...")
    
    release_dir = os.path.join(script_dir, "../../release")
    inc_dest = os.path.join(script_dir, "../../fhe-cmplr/rtlib/ant/dataset/resnet20_cifar10_pre.onnx.inc")
    
    try:
        shutil.copy2(c_file, inc_dest)
        print(f"  ✓ Copied to {inc_dest}")
        
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
    
    print("\n" + "=" * 70)
    print("✓ Test completed successfully!")
    print("=" * 70)
    return True


if __name__ == "__main__":
    success = run_test()
    sys.exit(0 if success else 1)
