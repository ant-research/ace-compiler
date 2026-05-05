# test_regression/resnet/cases/ir_baselines.py
"""
ResNet AIR IR structure baselines and validation utilities.

All baselines are generated from ResNet with BatchNorm folding.
Input shape: [1, 3, 32, 32], Output shape: [1, 10]
"""


# ============================================================================
# Shared validation utilities
# ============================================================================

def extract_ir_structure(traced_model, constants):
    """Extract IR structure from a TorchTracedModel."""
    structure = {
        "model": "unknown",
        "input_shape": traced_model._input_shapes[0] if traced_model._input_shapes else [],
        "output_shape": traced_model._output_shape,
        "operations": {},
        "total_ops": 0,
        "constants": {},
        "total_constants": len(constants),
        "conv_layers": [],
        "add_layers": [],
    }

    op_count = {}
    for node in traced_model.traced_model.graph.nodes:
        if node.op == 'call_function':
            target_str = str(node.target)
            if 'torch.ops.tensor.' in target_str:
                op_name = target_str.replace('torch.ops.tensor.', '')
            elif target_str.startswith('tensor.'):
                op_name = target_str.replace('tensor.', '')
            else:
                continue

            op_name_normalized = op_name.lower()
            if op_name_normalized not in op_count:
                op_count[op_name_normalized] = 0
            op_count[op_name_normalized] += 1

            if op_name_normalized == 'conv':
                structure["conv_layers"].append({"name": node.name})
            elif op_name_normalized == 'add':
                structure["add_layers"].append({"name": node.name})

    structure["operations"] = op_count
    structure["total_ops"] = sum(op_count.values())

    for name, info in constants.items():
        shape = info.get('shape', [])
        dtype = info.get('dtype', 'float32')
        structure["constants"][name] = {"shape": shape, "dtype": dtype}

    return structure


def compare_ir_structure(actual, baseline):
    """Compare actual IR structure with baseline. Returns (is_match, differences)."""
    differences = []

    if actual["input_shape"] != baseline["input_shape"]:
        differences.append(f"Input shape mismatch: expected {baseline['input_shape']}, got {actual['input_shape']}")

    if actual["output_shape"] != baseline["output_shape"]:
        differences.append(f"Output shape mismatch: expected {baseline['output_shape']}, got {actual['output_shape']}")

    for op, expected_count in baseline["operations"].items():
        actual_count = actual["operations"].get(op, 0)
        if actual_count != expected_count:
            differences.append(f"Operation '{op}' count mismatch: expected {expected_count}, got {actual_count}")

    for op, actual_count in actual["operations"].items():
        if op not in baseline["operations"]:
            differences.append(f"Unexpected operation '{op}' with count {actual_count}")

    if actual["total_ops"] != baseline["total_ops"]:
        differences.append(f"Total ops mismatch: expected {baseline['total_ops']}, got {actual['total_ops']}")

    if actual["total_constants"] != baseline["total_constants"]:
        differences.append(f"Constant count mismatch: expected {baseline['total_constants']}, got {actual['total_constants']}")

    if len(actual["conv_layers"]) != len(baseline["conv_layers"]):
        differences.append(f"Conv layer count mismatch: expected {len(baseline['conv_layers'])}, got {len(actual['conv_layers'])}")

    if len(actual["add_layers"]) != len(baseline["add_layers"]):
        differences.append(f"Add layer count mismatch: expected {len(baseline['add_layers'])}, got {len(actual['add_layers'])}")

    return len(differences) == 0, differences


def validate_resnet_ir(traced_model, constants, baseline):
    """Validate ResNet IR structure against a baseline dict."""
    actual = extract_ir_structure(traced_model, constants)
    actual["model"] = baseline["model"]
    return compare_ir_structure(actual, baseline)


# ============================================================================
# ResNet-20 IR Baseline
# ============================================================================

RESNET20_IR_BASELINE = {
    "model": "resnet20_cifar10",
    "input_shape": [1, 3, 32, 32],
    "output_shape": [1, 10],
    "operations": {"conv": 21, "relu": 19, "add": 9, "global_average_pool": 1, "gemm": 1, "reshape": 1},
    "total_ops": 52,
    "constants": {
        "fc_weight": {"shape": [10, 64]}, "fc_bias": {"shape": [10]},
        "conv_weights": {"count": 21}, "conv_biases": {"count": 21},
        "reshape_shape": {"shape": [2], "dtype": "int64"},
    },
    "total_constants": 45,
    "conv_layers": [
        {"name": "conv1", "in_channels": 3, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.0.conv1", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.0.conv2", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.1.conv1", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.1.conv2", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.2.conv1", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.2.conv2", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer2.0.conv1", "in_channels": 16, "out_channels": 32, "kernel_size": 3, "stride": 2},
        {"name": "layer2.0.conv2", "in_channels": 32, "out_channels": 32, "kernel_size": 3},
        {"name": "layer2.0.downsample", "in_channels": 16, "out_channels": 32, "kernel_size": 1, "stride": 2},
        {"name": "layer2.1.conv1", "in_channels": 32, "out_channels": 32, "kernel_size": 3},
        {"name": "layer2.1.conv2", "in_channels": 32, "out_channels": 32, "kernel_size": 3},
        {"name": "layer2.2.conv1", "in_channels": 32, "out_channels": 32, "kernel_size": 3},
        {"name": "layer2.2.conv2", "in_channels": 32, "out_channels": 32, "kernel_size": 3},
        {"name": "layer3.0.conv1", "in_channels": 32, "out_channels": 64, "kernel_size": 3, "stride": 2},
        {"name": "layer3.0.conv2", "in_channels": 64, "out_channels": 64, "kernel_size": 3},
        {"name": "layer3.0.downsample", "in_channels": 32, "out_channels": 64, "kernel_size": 1, "stride": 2},
        {"name": "layer3.1.conv1", "in_channels": 64, "out_channels": 64, "kernel_size": 3},
        {"name": "layer3.1.conv2", "in_channels": 64, "out_channels": 64, "kernel_size": 3},
        {"name": "layer3.2.conv1", "in_channels": 64, "out_channels": 64, "kernel_size": 3},
        {"name": "layer3.2.conv2", "in_channels": 64, "out_channels": 64, "kernel_size": 3},
    ],
    "add_layers": [
        {"name": f"layer{l}.{b}.add"}
        for l in [1, 2, 3] for b in range(3)
    ],
}

# ============================================================================
# ResNet-32 IR Baseline
# ============================================================================

RESNET32_IR_BASELINE = {
    "model": "resnet32_cifar10",
    "input_shape": [1, 3, 32, 32],
    "output_shape": [1, 10],
    "operations": {"conv": 33, "relu": 31, "add": 15, "global_average_pool": 1, "gemm": 1, "reshape": 1},
    "total_ops": 82,
    "constants": {
        "fc_weight": {"shape": [10, 64]}, "fc_bias": {"shape": [10]},
        "conv_weights": {"count": 33}, "conv_biases": {"count": 33},
        "reshape_shape": {"shape": [2], "dtype": "int64"},
    },
    "total_constants": 69,
    "conv_layers": [
        {"name": "conv1", "in_channels": 3, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.0.conv1", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.0.conv2", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.1.conv1", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.1.conv2", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.2.conv1", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.2.conv2", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.3.conv1", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.3.conv2", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.4.conv1", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.4.conv2", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer2.0.conv1", "in_channels": 16, "out_channels": 32, "kernel_size": 3, "stride": 2},
        {"name": "layer2.0.conv2", "in_channels": 32, "out_channels": 32, "kernel_size": 3},
        {"name": "layer2.0.downsample", "in_channels": 16, "out_channels": 32, "kernel_size": 1, "stride": 2},
        {"name": "layer2.1.conv1", "in_channels": 32, "out_channels": 32, "kernel_size": 3},
        {"name": "layer2.1.conv2", "in_channels": 32, "out_channels": 32, "kernel_size": 3},
        {"name": "layer2.2.conv1", "in_channels": 32, "out_channels": 32, "kernel_size": 3},
        {"name": "layer2.2.conv2", "in_channels": 32, "out_channels": 32, "kernel_size": 3},
        {"name": "layer2.3.conv1", "in_channels": 32, "out_channels": 32, "kernel_size": 3},
        {"name": "layer2.3.conv2", "in_channels": 32, "out_channels": 32, "kernel_size": 3},
        {"name": "layer2.4.conv1", "in_channels": 32, "out_channels": 32, "kernel_size": 3},
        {"name": "layer2.4.conv2", "in_channels": 32, "out_channels": 32, "kernel_size": 3},
        {"name": "layer3.0.conv1", "in_channels": 32, "out_channels": 64, "kernel_size": 3, "stride": 2},
        {"name": "layer3.0.conv2", "in_channels": 64, "out_channels": 64, "kernel_size": 3},
        {"name": "layer3.0.downsample", "in_channels": 32, "out_channels": 64, "kernel_size": 1, "stride": 2},
        {"name": "layer3.1.conv1", "in_channels": 64, "out_channels": 64, "kernel_size": 3},
        {"name": "layer3.1.conv2", "in_channels": 64, "out_channels": 64, "kernel_size": 3},
        {"name": "layer3.2.conv1", "in_channels": 64, "out_channels": 64, "kernel_size": 3},
        {"name": "layer3.2.conv2", "in_channels": 64, "out_channels": 64, "kernel_size": 3},
        {"name": "layer3.3.conv1", "in_channels": 64, "out_channels": 64, "kernel_size": 3},
        {"name": "layer3.3.conv2", "in_channels": 64, "out_channels": 64, "kernel_size": 3},
        {"name": "layer3.4.conv1", "in_channels": 64, "out_channels": 64, "kernel_size": 3},
        {"name": "layer3.4.conv2", "in_channels": 64, "out_channels": 64, "kernel_size": 3},
    ],
    "add_layers": [
        {"name": f"layer{l}.{b}.add"}
        for l in [1, 2, 3] for b in range(5)
    ],
}

# ============================================================================
# ResNet-44 IR Baseline
# ============================================================================

RESNET44_IR_BASELINE = {
    "model": "resnet44_cifar10",
    "input_shape": [1, 3, 32, 32],
    "output_shape": [1, 10],
    "operations": {"conv": 45, "relu": 43, "add": 21, "global_average_pool": 1, "gemm": 1, "reshape": 1},
    "total_ops": 112,
    "constants": {
        "fc_weight": {"shape": [10, 64]}, "fc_bias": {"shape": [10]},
        "conv_weights": {"count": 45}, "conv_biases": {"count": 45},
        "reshape_shape": {"shape": [2], "dtype": "int64"},
    },
    "total_constants": 93,
    "conv_layers": [
        {"name": "conv1", "in_channels": 3, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.0.conv1", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.0.conv2", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.1.conv1", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.1.conv2", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.2.conv1", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.2.conv2", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.3.conv1", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.3.conv2", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.4.conv1", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.4.conv2", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.5.conv1", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.5.conv2", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.6.conv1", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.6.conv2", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer2.0.conv1", "in_channels": 16, "out_channels": 32, "kernel_size": 3, "stride": 2},
        {"name": "layer2.0.conv2", "in_channels": 32, "out_channels": 32, "kernel_size": 3},
        {"name": "layer2.0.downsample", "in_channels": 16, "out_channels": 32, "kernel_size": 1, "stride": 2},
        {"name": "layer2.1.conv1", "in_channels": 32, "out_channels": 32, "kernel_size": 3},
        {"name": "layer2.1.conv2", "in_channels": 32, "out_channels": 32, "kernel_size": 3},
        {"name": "layer2.2.conv1", "in_channels": 32, "out_channels": 32, "kernel_size": 3},
        {"name": "layer2.2.conv2", "in_channels": 32, "out_channels": 32, "kernel_size": 3},
        {"name": "layer2.3.conv1", "in_channels": 32, "out_channels": 32, "kernel_size": 3},
        {"name": "layer2.3.conv2", "in_channels": 32, "out_channels": 32, "kernel_size": 3},
        {"name": "layer2.4.conv1", "in_channels": 32, "out_channels": 32, "kernel_size": 3},
        {"name": "layer2.4.conv2", "in_channels": 32, "out_channels": 32, "kernel_size": 3},
        {"name": "layer2.5.conv1", "in_channels": 32, "out_channels": 32, "kernel_size": 3},
        {"name": "layer2.5.conv2", "in_channels": 32, "out_channels": 32, "kernel_size": 3},
        {"name": "layer2.6.conv1", "in_channels": 32, "out_channels": 32, "kernel_size": 3},
        {"name": "layer2.6.conv2", "in_channels": 32, "out_channels": 32, "kernel_size": 3},
        {"name": "layer3.0.conv1", "in_channels": 32, "out_channels": 64, "kernel_size": 3, "stride": 2},
        {"name": "layer3.0.conv2", "in_channels": 64, "out_channels": 64, "kernel_size": 3},
        {"name": "layer3.0.downsample", "in_channels": 32, "out_channels": 64, "kernel_size": 1, "stride": 2},
        {"name": "layer3.1.conv1", "in_channels": 64, "out_channels": 64, "kernel_size": 3},
        {"name": "layer3.1.conv2", "in_channels": 64, "out_channels": 64, "kernel_size": 3},
        {"name": "layer3.2.conv1", "in_channels": 64, "out_channels": 64, "kernel_size": 3},
        {"name": "layer3.2.conv2", "in_channels": 64, "out_channels": 64, "kernel_size": 3},
        {"name": "layer3.3.conv1", "in_channels": 64, "out_channels": 64, "kernel_size": 3},
        {"name": "layer3.3.conv2", "in_channels": 64, "out_channels": 64, "kernel_size": 3},
        {"name": "layer3.4.conv1", "in_channels": 64, "out_channels": 64, "kernel_size": 3},
        {"name": "layer3.4.conv2", "in_channels": 64, "out_channels": 64, "kernel_size": 3},
        {"name": "layer3.5.conv1", "in_channels": 64, "out_channels": 64, "kernel_size": 3},
        {"name": "layer3.5.conv2", "in_channels": 64, "out_channels": 64, "kernel_size": 3},
        {"name": "layer3.6.conv1", "in_channels": 64, "out_channels": 64, "kernel_size": 3},
        {"name": "layer3.6.conv2", "in_channels": 64, "out_channels": 64, "kernel_size": 3},
    ],
    "add_layers": [
        {"name": f"layer{l}.{b}.add"}
        for l in [1, 2, 3] for b in range(7)
    ],
}

# ============================================================================
# ResNet-56 IR Baseline
# ============================================================================

RESNET56_IR_BASELINE = {
    "model": "resnet56_cifar10",
    "input_shape": [1, 3, 32, 32],
    "output_shape": [1, 10],
    "operations": {"conv": 57, "relu": 55, "add": 27, "global_average_pool": 1, "gemm": 1, "reshape": 1},
    "total_ops": 142,
    "constants": {
        "fc_weight": {"shape": [10, 64]}, "fc_bias": {"shape": [10]},
        "conv_weights": {"count": 57}, "conv_biases": {"count": 57},
        "reshape_shape": {"shape": [2], "dtype": "int64"},
    },
    "total_constants": 117,
    "conv_layers": [
        {"name": "conv1", "in_channels": 3, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.0.conv1", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.0.conv2", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.1.conv1", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.1.conv2", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.2.conv1", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.2.conv2", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.3.conv1", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.3.conv2", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.4.conv1", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.4.conv2", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.5.conv1", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.5.conv2", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.6.conv1", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.6.conv2", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.7.conv1", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.7.conv2", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.8.conv1", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer1.8.conv2", "in_channels": 16, "out_channels": 16, "kernel_size": 3},
        {"name": "layer2.0.conv1", "in_channels": 16, "out_channels": 32, "kernel_size": 3, "stride": 2},
        {"name": "layer2.0.conv2", "in_channels": 32, "out_channels": 32, "kernel_size": 3},
        {"name": "layer2.0.downsample", "in_channels": 16, "out_channels": 32, "kernel_size": 1, "stride": 2},
        {"name": "layer2.1.conv1", "in_channels": 32, "out_channels": 32, "kernel_size": 3},
        {"name": "layer2.1.conv2", "in_channels": 32, "out_channels": 32, "kernel_size": 3},
        {"name": "layer2.2.conv1", "in_channels": 32, "out_channels": 32, "kernel_size": 3},
        {"name": "layer2.2.conv2", "in_channels": 32, "out_channels": 32, "kernel_size": 3},
        {"name": "layer2.3.conv1", "in_channels": 32, "out_channels": 32, "kernel_size": 3},
        {"name": "layer2.3.conv2", "in_channels": 32, "out_channels": 32, "kernel_size": 3},
        {"name": "layer2.4.conv1", "in_channels": 32, "out_channels": 32, "kernel_size": 3},
        {"name": "layer2.4.conv2", "in_channels": 32, "out_channels": 32, "kernel_size": 3},
        {"name": "layer2.5.conv1", "in_channels": 32, "out_channels": 32, "kernel_size": 3},
        {"name": "layer2.5.conv2", "in_channels": 32, "out_channels": 32, "kernel_size": 3},
        {"name": "layer2.6.conv1", "in_channels": 32, "out_channels": 32, "kernel_size": 3},
        {"name": "layer2.6.conv2", "in_channels": 32, "out_channels": 32, "kernel_size": 3},
        {"name": "layer2.7.conv1", "in_channels": 32, "out_channels": 32, "kernel_size": 3},
        {"name": "layer2.7.conv2", "in_channels": 32, "out_channels": 32, "kernel_size": 3},
        {"name": "layer2.8.conv1", "in_channels": 32, "out_channels": 32, "kernel_size": 3},
        {"name": "layer2.8.conv2", "in_channels": 32, "out_channels": 32, "kernel_size": 3},
        {"name": "layer3.0.conv1", "in_channels": 32, "out_channels": 64, "kernel_size": 3, "stride": 2},
        {"name": "layer3.0.conv2", "in_channels": 64, "out_channels": 64, "kernel_size": 3},
        {"name": "layer3.0.downsample", "in_channels": 32, "out_channels": 64, "kernel_size": 1, "stride": 2},
        {"name": "layer3.1.conv1", "in_channels": 64, "out_channels": 64, "kernel_size": 3},
        {"name": "layer3.1.conv2", "in_channels": 64, "out_channels": 64, "kernel_size": 3},
        {"name": "layer3.2.conv1", "in_channels": 64, "out_channels": 64, "kernel_size": 3},
        {"name": "layer3.2.conv2", "in_channels": 64, "out_channels": 64, "kernel_size": 3},
        {"name": "layer3.3.conv1", "in_channels": 64, "out_channels": 64, "kernel_size": 3},
        {"name": "layer3.3.conv2", "in_channels": 64, "out_channels": 64, "kernel_size": 3},
        {"name": "layer3.4.conv1", "in_channels": 64, "out_channels": 64, "kernel_size": 3},
        {"name": "layer3.4.conv2", "in_channels": 64, "out_channels": 64, "kernel_size": 3},
        {"name": "layer3.5.conv1", "in_channels": 64, "out_channels": 64, "kernel_size": 3},
        {"name": "layer3.5.conv2", "in_channels": 64, "out_channels": 64, "kernel_size": 3},
        {"name": "layer3.6.conv1", "in_channels": 64, "out_channels": 64, "kernel_size": 3},
        {"name": "layer3.6.conv2", "in_channels": 64, "out_channels": 64, "kernel_size": 3},
        {"name": "layer3.7.conv1", "in_channels": 64, "out_channels": 64, "kernel_size": 3},
        {"name": "layer3.7.conv2", "in_channels": 64, "out_channels": 64, "kernel_size": 3},
        {"name": "layer3.8.conv1", "in_channels": 64, "out_channels": 64, "kernel_size": 3},
        {"name": "layer3.8.conv2", "in_channels": 64, "out_channels": 64, "kernel_size": 3},
    ],
    "add_layers": [
        {"name": f"layer{l}.{b}.add"}
        for l in [1, 2, 3] for b in range(9)
    ],
}

# ============================================================================
# Baseline lookup by model name
# ============================================================================

IR_BASELINES = {
    "resnet20_cifar10": RESNET20_IR_BASELINE,
    "resnet32_cifar10": RESNET32_IR_BASELINE,
    "resnet44_cifar10": RESNET44_IR_BASELINE,
    "resnet56_cifar10": RESNET56_IR_BASELINE,
}