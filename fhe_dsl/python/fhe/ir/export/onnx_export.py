#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
ONNX export utilities for IR.
"""

import logging
from pathlib import Path
from typing import TYPE_CHECKING

import onnx
from onnx import helper, TensorProto, numpy_helper

if TYPE_CHECKING:
    from ..representations.fhe_program import FHEProgram
    from ..representations.graph import FHEGraph

logger = logging.getLogger(__name__)


# IR op_type to ONNX op_type mapping
OP_TYPE_TO_ONNX = {
    "add": "Add",
    "sub": "Sub",
    "mul": "Mul",
    "div": "Div",
    "relu": "Relu",
    "silu": "SiLU",
    "matmul": "MatMul",
    "conv": "Conv",
    "gemm": "Gemm",
    "max_pool": "MaxPool",
    "avg_pool": "AveragePool",
    "average_pool": "AveragePool",
    "global_avg_pool": "GlobalAveragePool",
    "global_average_pool": "GlobalAveragePool",
    "flatten": "Flatten",
    "concat": "Concat",
    "softmax": "Softmax",
    "sqrt": "Sqrt",
    "transpose": "Transpose",
    "reshape": "Reshape",
    "abs": "Abs",
    "neg": "Neg",
    "clamp": "Clip",
    "log": "Log",
    "exp": "Exp",
}


def export_fhe_program_to_onnx(fhe_program: "FHEProgram", filename: str) -> bool:
    """
    Export FHEProgram as ONNX format.

    Args:
        fhe_program: FHEProgram instance to export
        filename: Output .onnx file path

    Returns:
        True if successful, False otherwise
    """
    try:
        import numpy as np

        # Ensure directory exists
        filepath = Path(filename)
        filepath.parent.mkdir(parents=True, exist_ok=True)

        # Get the main graph
        main_graph = fhe_program.get_main_graph()

        # Build ONNX nodes and initializers
        onnx_nodes, initializer = _build_onnx_nodes(main_graph)

        # Build input/output tensors
        graph_inputs, graph_outputs = _build_io_tensors(main_graph)

        # Create ONNX graph
        onnx_graph = helper.make_graph(
            nodes=onnx_nodes,
            name=fhe_program.name,
            inputs=graph_inputs,
            outputs=graph_outputs,
            initializer=initializer,
        )

        # Create ONNX model
        onnx_model = helper.make_model(onnx_graph)
        onnx_model.opset_import[0].version = 13

        # Validate and save
        onnx.checker.check_model(onnx_model)
        onnx.save(onnx_model, filename)

        logger.info(f"ONNX model written to: {filename}")
        return True

    except Exception as e:
        logger.error(f"Failed to export ONNX: {e}")
        return False


def _build_onnx_nodes(main_graph: "FHEGraph"):
    """Build ONNX nodes and initializers from FHEGraph."""
    import numpy as np

    onnx_nodes = []
    initializer = []

    # First pass: collect constants
    for block in main_graph.blocks.values():
        for node in block.nodes:
            if node.op_type == "constant":
                const_value = node.attributes.get("value", 0)
                if isinstance(const_value, (int, float)):
                    const_array = np.array([const_value], dtype=np.float32)
                else:
                    const_array = np.array(const_value, dtype=np.float32)
                const_tensor = numpy_helper.from_array(const_array, name=node.name)
                initializer.append(const_tensor)

    # Second pass: create operation nodes
    for block in main_graph.blocks.values():
        for node in block.nodes:
            if node.op_type in ("input", "output", "constant", "ref"):
                continue

            onnx_op_type = OP_TYPE_TO_ONNX.get(node.op_type)
            if onnx_op_type is None:
                logger.warning(f"Unknown op_type '{node.op_type}', skipping")
                continue

            onnx_node = helper.make_node(
                onnx_op_type,
                inputs=node.inputs,
                outputs=node.outputs if node.outputs else [f"{node.name}_out"],
                name=node.name,
            )

            # Add attributes
            _add_attributes(onnx_node, node.attributes)
            onnx_nodes.append(onnx_node)

    return onnx_nodes, initializer


def _add_attributes(onnx_node, attributes: dict):
    """Add attributes to ONNX node."""
    if not attributes:
        return

    for attr_name, attr_value in attributes.items():
        if attr_name == "value":
            continue

        if isinstance(attr_value, int):
            onnx_node.attribute.append(helper.make_attribute(attr_name, attr_value))
        elif isinstance(attr_value, float):
            onnx_node.attribute.append(helper.make_attribute(attr_name, attr_value))
        elif isinstance(attr_value, str):
            onnx_node.attribute.append(helper.make_attribute(attr_name, attr_value))
        elif isinstance(attr_value, list) and all(isinstance(x, int) for x in attr_value):
            onnx_node.attribute.append(helper.make_attribute(attr_name, attr_value))


def _build_io_tensors(main_graph: "FHEGraph"):
    """Build input/output tensor info."""
    input_shapes = main_graph.metadata.get("input_shapes", {})
    output_shape = main_graph.metadata.get("output_shape", [1, 4])

    graph_inputs = []
    for inp_name in main_graph.input_nodes:
        shape = input_shapes.get(inp_name, [1, 4])
        graph_inputs.append(
            helper.make_tensor_value_info(inp_name, TensorProto.FLOAT, shape)
        )

    graph_outputs = []
    for out_name in main_graph.output_nodes:
        graph_outputs.append(
            helper.make_tensor_value_info(out_name, TensorProto.FLOAT, output_shape)
        )

    return graph_inputs, graph_outputs