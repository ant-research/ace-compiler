#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
ONNX to IR conversion utilities.
"""

import logging
import os
import subprocess
from pathlib import Path
from typing import Union, Optional

import onnx

from ...representations.fhe_program import FHEProgram
from ...representations.graph import FHEGraph, BasicBlock

# Get logger
logger = logging.getLogger(__name__)


def convert_onnx_to_fhe_program(onnx_path: Union[str, Path]) -> FHEProgram:
    """
    Convert an ONNX model to FHEProgram representation.

    Args:
        onnx_path: Path to ONNX model file

    Returns:
        FHEProgram instance containing a main graph with single basic block.
    """
    onnx_path = str(onnx_path)
    _validate_onnx_model(onnx_path)
    model = onnx.load(onnx_path)
    graph = model.graph

    # Create the main computation graph
    fhe_graph = FHEGraph(name="forward")

    # Create a single basic block for all ONNX nodes
    main_block = BasicBlock(name="entry")

    # Convert ONNX nodes to our node format
    for i, node in enumerate(graph.node):
        attrs = _extract_attributes(node)
        node_dict = {
            "index": i,
            "op_type": node.op_type,
            "inputs": list(node.input),
            "outputs": list(node.output),
            "attributes": attrs
        }
        main_block.add_node(node_dict)

    # Add the basic block to the graph
    fhe_graph.add_block(main_block)
    fhe_graph.entry_block = main_block

    # Set input/output nodes
    fhe_graph.input_nodes = [inp.name for inp in graph.input]
    fhe_graph.output_nodes = [out.name for out in graph.output]

    # Create FHEProgram
    program_name = model.graph.name if model.graph.name else "main"
    fhe_program = FHEProgram(name=program_name)
    fhe_program.add_graph("forward", fhe_graph)

    return fhe_program


def convert_onnx_to_air_binary(onnx_path: Union[str, Path], output_path: str) -> str:
    """
    Convert ONNX to AIR binary format (.B file) using fhe_cmplr.

    Args:
        onnx_path: Path to ONNX model file
        output_path: Output path for AIR .B file

    Returns:
        Path to the generated .B file
    """
    onnx_path = str(onnx_path)

    # Ensure output directory exists
    Path(output_path).parent.mkdir(parents=True, exist_ok=True)

    # Use fhe_cmplr to convert ONNX to AIR .B file
    # Run in output directory to avoid generating intermediate files
    # (.c, .conf, .json, .t) in the current working directory
    onnx_path_abs = str(Path(onnx_path).resolve())
    output_dir = str(Path(output_path).parent.resolve())
    output_filename = Path(output_path).name

    # Get absolute path to fhe_cmplr from installed package
    import sysconfig
    platlib = sysconfig.get_path("platlib")
    fhe_cmplr_path = str(Path(platlib) / "ace/bin/fhe_cmplr")

    cmd = [
        fhe_cmplr_path,
        onnx_path_abs,
        f"-O2A:ir2b={output_filename}"
    ]

    try:
        subprocess.run(cmd, check=True, capture_output=True, text=True, cwd=output_dir)
        logger.info("ONNX → AIR conversion succeeded: %s", output_path)
    except subprocess.CalledProcessError as e:
        raise RuntimeError(f"ONNX to AIR conversion failed: {e.stderr}")
    except FileNotFoundError:
        raise FileNotFoundError(f"fhe_cmplr not found at {fhe_cmplr_path}")

    return output_path


def _validate_onnx_model(onnx_path: str) -> None:
    """Validate an ONNX model file."""
    model = onnx.load(onnx_path)
    onnx.checker.check_model(model)


def _extract_attributes(node) -> dict:
    """Extract attributes from ONNX node."""
    attrs = {}
    for attr in node.attribute:
        if attr.type == onnx.AttributeProto.FLOAT:
            val = attr.f
        elif attr.type == onnx.AttributeProto.INT:
            val = attr.i
        elif attr.type == onnx.AttributeProto.STRING:
            val = attr.s.decode('utf-8')
        elif attr.type == onnx.AttributeProto.TENSOR:
            val = onnx.numpy_helper.to_array(attr.t).tolist()
        else:
            val = str(attr)
        attrs[attr.name] = val
    return attrs