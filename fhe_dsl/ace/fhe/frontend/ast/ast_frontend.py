#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
AST frontend for converting Python functions to IR using AST analysis.

Pipeline:
1. prepare()  - Python → AST Analysis → FHEProgram
2. compile()  - Use IRBuilder to generate AIR IR → .B file
3. export()   - Export .B file
"""

from typing import Union, List, Callable, Optional
import torch
import torch.nn as nn

from ...driver import Frontend
from ...ir import FHEProgram, ASTToIRConverter
from ...ir.core.ir_builder import IRBuilder


class AST(Frontend):
    """Python Function/Model → AIR frontend using AST analysis.

    Pipeline:
    1. prepare()  - Python → AST Analysis → FHEProgram
    2. compile()  - Use IRBuilder to generate AIR IR → .B file
    3. export()   - Export .B file

    Note: Does NOT go through ONNX - use ast-via-onnx for ONNX path.
    """

    @classmethod
    def name(cls) -> str:
        return "ast"

    def prepare(
        self,
        model_or_func: Union[nn.Module, Callable],
        example_inputs: List[torch.Tensor],
        input_names: List[str]
    ) -> FHEProgram:
        """Convert Python function/model to FHEProgram via AST analysis.

        Args:
            model_or_func: PyTorch model or Python function
            example_inputs: Example inputs (not used for AST)
            input_names: Names of input parameters

        Returns:
            FHEProgram with AIR IR
        """
        if isinstance(model_or_func, nn.Module):
            return self._model_to_air(model_or_func, input_names)
        else:
            return self._function_to_air(model_or_func, input_names)

    def compile(
        self,
        model_or_func: Union[nn.Module, Callable],
        example_inputs: List[torch.Tensor],
        input_names: List[str],
        build_dir: Optional[str] = None,
        backend: Optional[str] = None,
        device: Optional[str] = None,
        **kwargs
    ) -> FHEProgram:
        """Convert Python function/model to AIR via AST using IRBuilder.

        Args:
            model_or_func: PyTorch model or Python function
            example_inputs: Example inputs (used for shape inference)
            input_names: Names of input parameters
            build_dir: Optional directory for intermediate files
            backend: Backend name (ignored for AST frontend)
            device: Device name (ignored for AST frontend)
            **kwargs: Additional arguments (ignored)

        Returns:
            FHEProgram with AIR IR exported to file (format_type="file")
        """
        import os

        # Get FHEProgram from prepare()
        fhe_program = self.prepare(model_or_func, example_inputs, input_names)

        # Use provided build_dir for AIR IR generation, or create temp dir
        if build_dir:
            temp_dir = build_dir
        else:
            import tempfile
            temp_dir = tempfile.mkdtemp()
        temp_file = os.path.join(temp_dir, "ast_export.B")

        # Generate AIR IR using IRBuilder
        self._generate_air_ir(fhe_program, example_inputs, input_names, temp_file)

        # Update fhe_program with file path
        fhe_program._file_path = temp_file

        return fhe_program

    def _generate_air_ir(
        self,
        fhe_program: FHEProgram,
        example_inputs: List[torch.Tensor],
        input_names: List[str],
        output_path: str
    ):
        """Generate AIR IR using IRBuilder (same approach as Torch frontend)."""
        if not IRBuilder.is_available():
            raise RuntimeError("C++ extension not available. Cannot generate AIR IR.")

        # Get the main graph
        main_graph = fhe_program.get_main_graph()

        # Create IRBuilder
        builder = IRBuilder()

        # Clear tensor name registry
        builder.clear_tensor_names()

        # Step 1: Begin AIR function with "Main_graph" entry point
        builder.begin_function("Main_graph")

        # Step 2: Add inputs with shapes from example_inputs
        input_shapes = []
        for i, name in enumerate(input_names):
            if example_inputs and i < len(example_inputs) and example_inputs[i] is not None:
                shape = list(example_inputs[i].shape)
            else:
                shape = [1, 4]  # Default shape
            input_shapes.append(shape)
            builder.add_input(name, shape)
            # Register tensor name for later reference
            if example_inputs and i < len(example_inputs) and example_inputs[i] is not None:
                builder.register_tensor_name(example_inputs[i].data_ptr(), name)

        # Step 3: Determine output shape (assume same as input for simple functions)
        if example_inputs and example_inputs[0] is not None:
            output_shape = list(example_inputs[0].shape)
        else:
            output_shape = [1, 4]

        # Step 4: End function definition
        builder.end_function(output_shape)

        # Step 5: Process IR nodes and add operations
        self._process_ast_nodes(main_graph, builder, input_names, example_inputs)

        # Step 6: Finalize and write IR
        builder.finalize()
        builder.write_ir(output_path)

    def _process_ast_nodes(self, main_graph, builder: IRBuilder, input_names: List[str], example_inputs: List[torch.Tensor] = None):
        """Process AST-generated nodes and add operations via IRBuilder."""
        # Symbol table: maps variable names to AIR result names
        symbol_table = {name: name for name in input_names}

        # Determine output shape for constants
        if example_inputs and example_inputs[0] is not None:
            const_shape = [1]  # Scalar constant
        else:
            const_shape = [1, 4]

        # Find the last operation node (not input/output/ref/constant)
        last_op_node = None
        for block in main_graph.blocks.values():
            for node in block.nodes:
                if node.op_type not in ("input", "output", "constant", "ref"):
                    last_op_node = node

        # Process nodes
        for block in main_graph.blocks.values():
            for node in block.nodes:
                if node.op_type == "input":
                    continue
                elif node.op_type == "output":
                    continue
                elif node.op_type == "constant":
                    # Handle constants - add as constant tensor
                    # Extract constant value from attributes
                    const_value = node.attributes.get("value", 0)
                    const_name = node.outputs[0] if node.outputs else node.name

                    # Add constant to IRBuilder
                    if isinstance(const_value, (int, float)):
                        # For numeric constants, convert to list
                        data = [float(const_value)]
                        builder.add_constant(const_name, const_shape, data, dtype="float32")
                        # Register in symbol table
                        symbol_table[const_name] = const_name
                    continue
                elif node.op_type == "ref":
                    # Reference to existing variable - already in symbol table
                    continue
                else:
                    # Process operation node
                    # Resolve input names from symbol table
                    resolved_inputs = []
                    for inp in node.inputs:
                        resolved_inputs.append(symbol_table.get(inp, inp))

                    # Generate metadata with onnx-style name for function references
                    # Set is_output flag for the last operation
                    is_last_op = (node == last_op_node)
                    meta = {
                        "onnx_name": f"/{node.op_type}/{node.op_type.title()}",
                        "is_output": str(is_last_op)
                    }

                    # Determine output shape (assume same as first input for simple element-wise ops)
                    if example_inputs and example_inputs[0] is not None:
                        output_shape = list(example_inputs[0].shape)
                    else:
                        output_shape = [1, 4]

                    # Add operation
                    result_name = builder.add_op(
                        node.op_type,
                        resolved_inputs,
                        attrs={},
                        meta=meta,
                        output_shape=output_shape
                    )

                    # Update symbol table
                    if node.outputs:
                        for out in node.outputs:
                            symbol_table[out] = result_name

    def _convert_to_air(self, intermediate: FHEProgram) -> FHEProgram:
        """Already AIR, return as-is."""
        return intermediate

    def _model_to_air(self, model: nn.Module, input_names: List[str]) -> FHEProgram:
        """Convert a PyTorch model to AIR using AST analysis."""
        # Get the forward method
        forward_method = getattr(model, 'forward', None)
        if forward_method is None:
            raise ValueError("Model must have a forward method")

        # Use AST converter to convert the forward method
        converter = ASTToIRConverter()
        graph = converter.convert_function(forward_method, f"{model.__class__.__name__}_graph")

        # Create FHEProgram
        fhe_program = FHEProgram(name=model.__class__.__name__)
        fhe_program.add_graph("forward", graph)

        return fhe_program

    def _function_to_air(self, func: Callable, input_names: List[str]) -> FHEProgram:
        """Convert a Python function to AIR using AST analysis."""
        converter = ASTToIRConverter()
        graph = converter.convert_function(func, f"{func.__name__}_graph")

        # Create FHEProgram
        fhe_program = FHEProgram(name=func.__name__)
        fhe_program.add_graph("forward", graph)

        return fhe_program

    def _export_to_air_file(
        self,
        model_or_func: Union[nn.Module, Callable],
        example_inputs: List[torch.Tensor],
        input_names: List[str],
        output_path: str
    ) -> str:
        """Convert to AIR and write to .B file.

        Args:
            model_or_func: PyTorch model or Python function
            example_inputs: Example inputs
            input_names: Names of input parameters
            output_path: Output .B file path

        Returns:
            Path to written file
        """
        air = self.compile(model_or_func, example_inputs, input_names)
        if hasattr(air, "export_ir"):
            air.export_ir(output_path)
        else:
            raise NotImplementedError(
                f"{type(air).__name__} doesn't support export_ir()"
            )
        return output_path

    def _export_to_onnx_file(
        self,
        model_or_func: Union[nn.Module, Callable],
        example_inputs: List[torch.Tensor],
        input_names: List[str],
        output_path: str
    ) -> str:
        """Export to ONNX file via FHEProgram._export_as_onnx().

        Args:
            model_or_func: PyTorch model or Python function
            example_inputs: Example inputs (used for shape inference)
            input_names: Names of input parameters
            output_path: Output .onnx file path

        Returns:
            Path to written file
        """
        # Get FHEProgram with IR
        fhe_program = self.compile(model_or_func, example_inputs, input_names)

        # Get main graph and set input shapes from example_inputs
        main_graph = fhe_program.get_main_graph()
        input_shapes = {}
        for i, inp_name in enumerate(main_graph.input_nodes):
            if i < len(example_inputs):
                input_shapes[inp_name] = list(example_inputs[i].shape)
        main_graph.metadata["input_shapes"] = input_shapes

        # Set output shape (assume same as last input for simple cases)
        if example_inputs:
            main_graph.metadata["output_shape"] = list(example_inputs[0].shape)

        # Export to ONNX
        if hasattr(fhe_program, "export_ir"):
            fhe_program.export_ir(output_path)
        else:
            raise NotImplementedError(
                f"{type(fhe_program).__name__} doesn't support export_ir()"
            )
        return output_path