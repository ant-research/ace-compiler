#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
AIR binary export utilities for IR.
"""

import logging
from pathlib import Path
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from ..representations.fhe_program import FHEProgram

logger = logging.getLogger(__name__)

# Supported operations for AIR export
SUPPORTED_AIR_OPS = {
    "add", "sub", "mul", "div", "relu", "silu", "matmul", "conv",
    "max_pool", "avg_pool", "average_pool", "global_avg_pool", "global_average_pool",
    "flatten", "concat", "softmax", "sqrt", "gemm", "transpose", "reshape",
}


def export_fhe_program_to_air(fhe_program: "FHEProgram", filename: str) -> bool:
    """
    Export FHEProgram as AIR binary format (.B file) via IRBuilder.

    Args:
        fhe_program: FHEProgram instance to export
        filename: Output .B file path

    Returns:
        True if successful, False otherwise
    """
    try:
        from ..core.ir_builder import IRBuilder

        if not IRBuilder.is_available():
            logger.error("IRBuilder C++ extension not available")
            return False

        # Ensure directory exists
        filepath = Path(filename)
        filepath.parent.mkdir(parents=True, exist_ok=True)

        # Get the main graph
        main_graph = fhe_program.get_main_graph()

        # Create IRBuilder for building AIR IR
        builder = IRBuilder()

        # Build AIR IR using IRBuilder API
        builder.begin_function(fhe_program.name)

        # Add inputs
        input_shapes = main_graph.metadata.get("input_shapes", {})
        for input_name in main_graph.input_nodes:
            shape = input_shapes.get(input_name, [1, 4])
            builder.add_input(input_name, shape)

        # Set output shape
        output_shape = main_graph.metadata.get("output_shape", [1, 4])
        builder.end_function(output_shape)

        # Process IR nodes
        _process_ir_nodes(main_graph, builder)

        # Finalize and write
        builder.finalize()
        builder.write_ir(filename)

        logger.info(f"AIR binary written to: {filename}")
        return True

    except ImportError as e:
        logger.error(f"IRBuilder not available: {e}")
        return False
    except Exception as e:
        logger.error(f"Failed to export AIR binary: {e}")
        return False


def _process_ir_nodes(main_graph, builder):
    """Process IR nodes and generate AIR operations."""
    symbol_table = {name: name for name in main_graph.input_nodes}

    for block in main_graph.blocks.values():
        for node in block.nodes:
            if node.op_type == "input":
                continue
            elif node.op_type == "output":
                continue
            elif node.op_type in ("constant", "ref"):
                continue
            elif node.op_type in SUPPORTED_AIR_OPS:
                # Resolve input names
                resolved_inputs = []
                for inp in node.inputs:
                    resolved_inputs.append(symbol_table.get(inp, inp))

                result_name = builder.add_op(node.op_type, resolved_inputs)

                # Update symbol table
                if node.outputs:
                    for out in node.outputs:
                        symbol_table[out] = result_name
            else:
                logger.warning(f"Unknown operation type: {node.op_type}")