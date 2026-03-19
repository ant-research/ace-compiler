#!/usr/bin/env python3
"""
Test: Load ONNX and Run Full Pipeline
======================================

Demonstrates loading an ONNX model and running the complete FHE pipeline
using the Pipeline class with automatic AIR dumps at each phase.

Output files are saved to: examples/output/onnx_pipeline/

Run with:
    cd acepy
    PYTHONPATH=. python examples/test_with_load_onnx.py
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from dump_utils import Pipeline, PipelineTarget


def run_test():
    print("=" * 70)
    print("Test: Load ONNX and Run Full Pipeline (using Pipeline class)")
    print("=" * 70)
    
    # Get model path
    script_dir = os.path.dirname(os.path.abspath(__file__))
    model_path = os.path.join(script_dir, "../../model/resnet20_cifar10_pre.onnx")
    
    if not os.path.exists(model_path):
        print(f"ERROR: Model not found at {model_path}")
        return False
    
    # Create pipeline with ResNet20-specific ReLU config
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
    
    # Build and run pipeline using fluent API
    pipeline = Pipeline("onnx_pipeline", verbose=True)
    
    result = (pipeline
        .load_onnx(model_path)
        .configure_fhe(
            scaling_factor_bits=56,
            first_prime_bits=60,
            hamming_weight=192,
            conv_fast=True,
            gemm_fast=False,
            relu_vr_def=3.0,
            relu_vr=relu_vr,
        )
        .run(target=PipelineTarget.C)
    )
    
    # Print summary
    pipeline.print_summary()
    
    if result.success:
        print(f"\nTotal pipeline time: {result.total_time:.2f}s")
        print(f"C code size: {len(result.c_code):,} bytes")
        print(f"Data file: {result.data_file}")
        print("\n✓ Test passed!")
        return True
    else:
        print(f"\n✗ Pipeline failed: {result.error}")
        print(f"Completed phases: {result.phases_completed}")
        return False


if __name__ == "__main__":
    success = run_test()
    sys.exit(0 if success else 1)
