#!/usr/bin/env python3
"""
Analyze ONNX model to find abs max values for each ReLU layer.

This script uses ONNX Runtime's ability to extract intermediate outputs
by modifying the model's graph outputs.
"""

import argparse
import logging
import numpy as np
import onnx
import onnxruntime as ort
from collections import OrderedDict
import copy

# Get logger
logger = logging.getLogger(__name__)


def load_onnx_model(onnx_path):
    """Load ONNX model."""
    logger.info("Loading ONNX model: %s", onnx_path)
    model = onnx.load(onnx_path)
    return model


def find_relu_nodes(model):
    """Find all ReLU nodes in the model."""
    relu_nodes = []
    for node in model.graph.node:
        if node.op_type == "Relu":
            relu_nodes.append(node)
            logger.debug("  Found ReLU: %s (output: %s)", node.name, node.output[0])

    logger.info("\nTotal ReLU nodes: %d", len(relu_nodes))
    return relu_nodes


def modify_model_outputs(model, output_names):
    """Modify model to output intermediate layers."""
    from onnx import helper
    
    # Create a copy to avoid modifying original
    modified_model = copy.deepcopy(model)
    
    # Clear existing outputs
    del modified_model.graph.output[:]
    
    # Add new outputs for each ReLU layer we want to monitor
    for output_name in output_names:
        # Try to find tensor type info from nodes
        found = False
        
        # First check value_info
        for vi in model.graph.value_info:
            if vi.name == output_name:
                new_output = modified_model.graph.output.add()
                new_output.CopyFrom(vi)
                found = True
                break
        
        # If not found in value_info, try to create one
        if not found:
            # Create a simple output info
            output_tensor = helper.make_tensor_value_info(
                output_name,
                onnx.TensorProto.FLOAT,
                None  # Unknown shape
            )
            modified_model.graph.output.append(output_tensor)
            found = True
            logger.debug("  Added output (created): %s", output_name)

    logger.debug("  Modified model has %d outputs", len(modified_model.graph.output))
    return modified_model


def analyze_relu_max_values(onnx_path, num_samples=10):
    """Analyze ReLU layers and find max values."""
    logger.info("=" * 70)
    logger.info("ONNX Model ReLU Analysis")
    logger.info("=" * 70)

    # Load original model
    original_model = load_onnx_model(onnx_path)

    # Find ReLU nodes
    relu_nodes = find_relu_nodes(original_model)

    if not relu_nodes:
        logger.warning("No ReLU nodes found in the model!")
        return {}

    # Get ReLU output names
    relu_output_names = [node.output[0] for node in relu_nodes]

    # Get input info
    input_info = original_model.graph.input[0]
    input_shape = []
    for dim in input_info.type.tensor_type.shape.dim:
        if dim.HasField('dim_value'):
            input_shape.append(dim.dim_value)
        else:
            input_shape.append(1)  # Default batch size

    logger.info("\nInput shape: %s", input_shape)

    # Modify model to output all ReLU layers
    logger.info("Modifying model to output intermediate ReLU layers...")
    modified_model = modify_model_outputs(original_model, relu_output_names)

    # Save modified model temporarily
    import os
    import tempfile
    # Use temp file instead of workspace
    with tempfile.NamedTemporaryFile(suffix='.onnx', delete=False) as tmp:
        tmp_path = tmp.name
    onnx.save(modified_model, tmp_path)
    logger.info("  Saved modified model to: %s", tmp_path)
    
    try:
        # Create session with modified model
        logger.info("Creating ONNX Runtime session...")
        session = ort.InferenceSession(
            tmp_path,
            providers=["CPUExecutionProvider"]
        )

        # Get output names
        output_names = [output.name for output in session.get_outputs()]
        logger.debug("  Session outputs: %d", len(output_names))

        # Run multiple samples and track max values
        relu_max_values = {}

        logger.info("Running %d samples to find max values...", num_samples)
        logger.info("-" * 70)

        for sample_idx in range(num_samples):
            # Generate random input
            if sample_idx == 0:
                input_data = np.ones(input_shape, dtype=np.float32)
            elif sample_idx == 1:
                input_data = np.random.randn(*input_shape).astype(np.float32)
            else:
                input_data = np.random.uniform(-1, 1, input_shape).astype(np.float32)

            # Get input name
            input_name = original_model.graph.input[0].name

            # Run inference
            ort_inputs = {input_name: input_data}
            outputs = session.run(output_names, ort_inputs)

            # Analyze each ReLU output
            for i, relu_node in enumerate(relu_nodes):
                if i < len(outputs):
                    relu_output = outputs[i]
                    abs_max = np.abs(relu_output).max()

                    if relu_node.name not in relu_max_values:
                        relu_max_values[relu_node.name] = []

                    relu_max_values[relu_node.name].append(abs_max)

                    if sample_idx == 0:
                        logger.debug("  Sample 0 - %s: %.6f", relu_node.name, abs_max)
        
        # Compute final max values across all samples
        logger.info("\n" + "=" * 70)
        logger.info("ReLU Abs Max Values (across all samples)")
        logger.info("=" * 70)

        final_max_values = {}
        for relu_name, values in relu_max_values.items():
            max_val = max(values)
            final_max_values[relu_name] = max_val
            logger.info("%s: %.6f", relu_name, max_val)

        # Print summary statistics
        logger.info("\n" + "=" * 70)
        logger.info("Summary Statistics")
        logger.info("=" * 70)

        all_max_values = list(final_max_values.values())
        if all_max_values:
            logger.info("  Min abs max: %.6f", min(all_max_values))
            logger.info("  Max abs max: %.6f", max(all_max_values))
            logger.info("  Mean abs max: %.6f", np.mean(all_max_values))
            logger.info("  Median abs max: %.6f", np.median(all_max_values))

        # Output in format suitable for SIHE configuration
        logger.info("\n" + "=" * 70)
        logger.info("SIHE Configuration Format (relu_vr)")
        logger.info("=" * 70)
        logger.info("relu_vr:")
        for relu_name, max_val in final_max_values.items():
            # Round up to nearest integer for safety margin
            vr = int(np.ceil(max_val)) + 1  # Add 1 for safety margin
            logger.info('  "%s": %d', relu_name, vr)
        
        return final_max_values
    
    finally:
        pass


def main():
    parser = argparse.ArgumentParser(
        description="Analyze ONNX model ReLU layers for abs max values"
    )
    parser.add_argument(
        "onnx_path",
        help="Path to ONNX model file"
    )
    parser.add_argument(
        "-n", "--num-samples",
        type=int,
        default=10,
        help="Number of random samples to run (default: 10)"
    )
    parser.add_argument(
        "-o", "--output",
        help="Output file for results (optional)"
    )
    
    args = parser.parse_args()
    
    # Analyze model
    results = analyze_relu_max_values(args.onnx_path, args.num_samples)
    
    # Save results if output file specified
    if args.output and results:
        import json
        # Convert numpy types to Python types for JSON serialization
        all_max_values = list(results.values())
        json_results = {
            "model_path": args.onnx_path,
            "num_samples": args.num_samples,
            "relu_max_values": {k: float(v) for k, v in results.items()},
            "relu_vr_config": {k: int(np.ceil(v)) + 1 for k, v in results.items()},
            "summary": {
                "min_abs_max": float(min(all_max_values)),
                "max_abs_max": float(max(all_max_values)),
                "mean_abs_max": float(np.mean(all_max_values)),
                "median_abs_max": float(np.median(all_max_values))
            }
        }
        with open(args.output, "w") as f:
            json.dump(json_results, f, indent=2)
        logger.info("\nResults saved to: %s", args.output)


if __name__ == "__main__":
    main()