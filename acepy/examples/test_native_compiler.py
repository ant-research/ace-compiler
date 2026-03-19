"""
Test Native Compiler - Full Pipeline via Subprocess
====================================================

This script calls the native fhe_cmplr compiler via subprocess to 
generate C code. Use this for production C code generation.

For Python DSL lowering validation (without C code), see:
  test_python_lowering_only.py

Run with:
    cd acepy
    python examples/test_native_compiler.py
"""

import subprocess
import os
import sys
import time


def run_native_compiler(
    model_path: str,
    output_dir: str,
    poly_degree: int = 16384,
    scaling_factor_bits: int = 56,
    first_prime_bits: int = 60,
    hamming_weight: int = 192,
    relu_vr_def: float = 3.0,
    relu_vr: str = None,
    conv_fast: bool = True,
    verbose: bool = True
) -> dict:
    """
    Run the native FHE compiler via subprocess.
    
    Args:
        model_path: Path to ONNX model
        output_dir: Directory for output files
        poly_degree: Polynomial degree (N)
        scaling_factor_bits: Scale factor bits (sf)
        first_prime_bits: First prime bits (q0)
        hamming_weight: Hamming weight (sk_hw)
        relu_vr_def: Default ReLU value range
        relu_vr: Per-layer ReLU value ranges (semicolon-separated)
        conv_fast: Enable fast convolution
        verbose: Print progress
        
    Returns:
        dict with 'success', 'c_file', 'data_file', 'message'
    """
    # Find the compiler
    script_dir = os.path.dirname(os.path.abspath(__file__))
    compiler_dir = os.path.join(script_dir, "../../release")
    compiler_path = os.path.join(compiler_dir, "driver/fhe_cmplr")
    
    if not os.path.exists(compiler_path):
        return {"success": False, "message": f"Compiler not found: {compiler_path}"}
    
    if not os.path.exists(model_path):
        return {"success": False, "message": f"Model not found: {model_path}"}
    
    os.makedirs(output_dir, exist_ok=True)
    
    # Build output file paths
    model_name = os.path.basename(model_path).replace('.onnx', '')
    c_file = os.path.join(output_dir, f"{model_name}.c")
    data_file = os.path.join(output_dir, f"{model_name}.msg")
    
    # Build command
    cmd = [compiler_path, model_path]
    
    # VEC options
    vec_opts = []
    if conv_fast:
        vec_opts.append("conv_parl")  # renamed from conv_fast to conv_parl in newer versions
    vec_opts.append("rtt")  # runtime timing
    if vec_opts:
        cmd.append(f"-VEC:{':'.join(vec_opts)}")
    
    # SIHE options
    sihe_opts = [f"relu_vr_def={relu_vr_def}"]
    if relu_vr:
        sihe_opts.append(f"relu_vr={relu_vr}")
    cmd.append(f"-SIHE:{':'.join(sihe_opts)}")
    
    # FHE_SCHEME options (CKKS parameters moved to FHE_SCHEME group in newer versions)
    fhe_opts = [
        f"sk_hw={hamming_weight}",
        f"q0={first_prime_bits}",
        f"sf={scaling_factor_bits}"
    ]
    cmd.append(f"-FHE_SCHEME:{':'.join(fhe_opts)}")
    
    # P2C options
    p2c_opts = [
        f"df={data_file}",
        "fp"  # free_poly
    ]
    cmd.append(f"-P2C:{':'.join(p2c_opts)}")
    
    # Output file
    cmd.extend(["-o", c_file])
    
    if verbose:
        print(f"  Command: {' '.join(cmd)}")
    
    # Run compiler
    try:
        result = subprocess.run(
            cmd,
            cwd=compiler_dir,
            capture_output=True,
            text=True,
            timeout=600  # 10 minute timeout
        )
        
        if result.returncode != 0:
            return {
                "success": False, 
                "message": f"Compiler failed with code {result.returncode}",
                "stderr": result.stderr,
                "stdout": result.stdout
            }
        
        # Check output files exist
        # Note: output file may have .onnx.c suffix
        actual_c_file = c_file
        if not os.path.exists(c_file):
            # Try alternative naming
            alt_c_file = os.path.join(compiler_dir, f"{os.path.basename(model_path)}.c")
            if os.path.exists(alt_c_file):
                actual_c_file = alt_c_file
            else:
                return {"success": False, "message": f"C file not generated: {c_file}"}
        
        c_size = os.path.getsize(actual_c_file)
        data_size = os.path.getsize(data_file) if os.path.exists(data_file) else 0
        
        return {
            "success": True,
            "c_file": actual_c_file,
            "c_size": c_size,
            "data_file": data_file,
            "data_size": data_size,
            "message": "Success"
        }
        
    except subprocess.TimeoutExpired:
        return {"success": False, "message": "Compiler timed out (10 min)"}
    except Exception as e:
        return {"success": False, "message": str(e)}


def run_test():
    print("=" * 70)
    print("Native Compiler Test - Full Pipeline")
    print("=" * 70)
    
    script_dir = os.path.dirname(os.path.abspath(__file__))
    model_path = os.path.join(script_dir, "../../model/resnet20_cifar10_pre.onnx")
    output_dir = os.path.join(script_dir, "output_native")
    
    # Per-layer ReLU value ranges (from build_resnet20_cifar10.sh)
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
    
    print(f"\n[Input]")
    print(f"  Model: {model_path}")
    print(f"  Output: {output_dir}/")
    
    print(f"\n[Running native compiler...]")
    t0 = time.time()
    
    result = run_native_compiler(
        model_path=model_path,
        output_dir=output_dir,
        poly_degree=16384,
        scaling_factor_bits=56,
        first_prime_bits=60,
        hamming_weight=192,
        relu_vr_def=3.0,
        relu_vr=relu_vr,
        conv_fast=True,
        verbose=True
    )
    
    elapsed = time.time() - t0
    
    if result["success"]:
        print(f"\n  ✓ Compilation successful ({elapsed:.2f}s)")
        print(f"  ✓ C file: {result['c_file']} ({result['c_size']:,} bytes)")
        print(f"  ✓ Data file: {result['data_file']} ({result['data_size']:,} bytes)")
    else:
        print(f"\n  ✗ Compilation failed: {result['message']}")
        if "stderr" in result:
            print(f"  stderr: {result['stderr'][:500]}")
        return False
    
    print("\n" + "=" * 70)
    print("Summary")
    print("=" * 70)
    print(f"""
Native compiler successfully generated C code:

  Input:  {model_path}
  Output: {result['c_file']} ({result['c_size']:,} bytes)
          {result['data_file']} ({result['data_size']:,} bytes)
  
  Time: {elapsed:.2f}s

For Python DSL lowering validation, see:
  python examples/test_python_lowering_only.py
""")
    return True


if __name__ == "__main__":
    success = run_test()
    sys.exit(0 if success else 1)

