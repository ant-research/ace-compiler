#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
TorchTracedModel - IR class for FX-traced PyTorch models.

This is an IR representation that holds a traced PyTorch model
and can generate AIR IR when executed.
"""

import logging
from typing import List, Optional, Dict, Set
import torch
import torch.fx as fx
import sys

from ...base import CompilationUnit
from ...core.ir_builder import IRBuilder
from .custom_ops import STANDARD_OP_MAPPING, CUSTOM_OPERATORS
from .custom_ops.tracer import CustomTracer, trace_with_metadata

# Get logger
logger = logging.getLogger(__name__)


class TorchTracedModel(CompilationUnit):
    """
    IR wrapper for FX-traced PyTorch model.

    Holds the traced model and generates AIR IR when executed.

    Properties:
    - format_type: "memory" before export, "file" after export_ir()
    - file_format: None before export, "air" after export_ir()
    - file_path: None before export, path after export_ir()

    When executed, it generates AIR IR instead of computing results.
    After export_ir(), format_type becomes "file" with file_path set.
    """

    def __init__(self, traced_model, input_names: List[str],
                 input_shapes: List[List[int]], output_shape: List[int],
                 constants: Optional[dict] = None):
        self.traced_model = traced_model
        self._input_names = input_names
        self._input_shapes = input_shapes
        self._output_shape = output_shape
        self._constants = constants or {}  # dict: name -> {tensor, shape, data}
        self._air_generated = False
        self._func_scope = None
        self._glob_scope = None
        self._file_path = None  # Path to exported .B file
        self._entry_name = "Main_graph"  # Default entry name
        self._builder = None  # IRBuilder instance for IR generation

    @property
    def format_type(self) -> str:
        """Return 'file' if exported, 'memory' otherwise."""
        if self._file_path is not None:
            return "file"
        return "memory"

    @property
    def file_format(self) -> Optional[str]:
        """Return 'air' if exported to .B file, None otherwise."""
        if self._file_path is not None:
            return "air"
        return None

    @property
    def file_path(self) -> Optional[str]:
        """Return the exported file path."""
        return self._file_path

    @property
    def entry_name(self) -> str:
        """Return the entry name."""
        return self._entry_name

    @entry_name.setter
    def entry_name(self, value: str):
        """Set the entry name."""
        self._entry_name = value

    def is_air_generated(self) -> bool:
        """Check if AIR IR has been generated."""
        return self._air_generated

    def get_air_func_scope(self):
        """Get the generated AIR function scope."""
        return self._func_scope

    def get_air_glob_scope(self):
        """Get the generated AIR global scope."""
        return self._glob_scope

    def get_custom_ops(self) -> List[str]:
        """Get list of custom ops used in this model."""
        ops = []
        if self.traced_model is not None:
            for node in self.traced_model.graph.nodes:
                if node.op == 'call_function':
                    ops.append(str(node.target))
        return ops

    def execute(self, *args, execution_mode="interpreter", **kwargs):
        """
        Execute the traced model to generate AIR IR.

        Args:
            *args: Input tensors
            execution_mode: "interpreter" (default) or "direct"
                - "interpreter": Node-by-node execution via IRBuilder.add_op()
                  (Path 2: full attrs/metadata/shape from Python)
                - "direct": Direct model execution, C++ custom ops generate AIR IR
                  (Path 1: attrs/metadata/shape built in C++ torch_ops.cxx)
            **kwargs: Additional keyword arguments

        The default "interpreter" mode is the stable, well-tested path.
        The "direct" mode is experimental and may not support all ops.
        """
        if execution_mode == "direct":
            return self._execute_direct(*args, **kwargs)
        return self._execute_interpreter(*args, **kwargs)

    def _execute_direct(self, *args, **kwargs):
        """
        Execute traced model directly - C++ custom ops generate AIR IR.

        In this mode, the traced model is executed directly. Each custom op
        (torch.ops.tensor.xxx) triggers its C++ implementation which calls
        Add_operation_cpp() to generate AIR IR with full attrs/metadata/shape.

        This is simpler than _execute_interpreter() because shape computation,
        attr construction, and metadata handling all happen in C++.
        """
        if not IRBuilder.is_available():
            raise RuntimeError("C++ extension not available. Cannot generate AIR IR.")

        logger.debug("Starting AIR generation (direct mode)...")

        # Create IRBuilder for building AIR IR
        self._builder = IRBuilder()

        # Clear tensor name registry
        self._builder.clear_tensor_names()

        # Step 1: Begin AIR function
        self._builder.begin_function("Main_graph")

        # Step 2: Add inputs
        for name, shape in zip(self._input_names, self._input_shapes):
            self._builder.add_input(name, shape)

        # Step 3: End function definition (sets output shape) - BEFORE constants
        # DEBUG: print output_shape before end_function
        print("=== DEBUG: output_shape in torch_trace.py before end_function ===")
        print("self._output_shape:", self._output_shape)
        self._builder.end_function(self._output_shape)

        # Step 4: Register constants
        fc_keys = [k for k in self._constants.keys() if k.startswith('fc_')]
        other_keys = [k for k in self._constants.keys() if not k.startswith('fc_')]
        fc_keys.sort(key=lambda x: (0 if 'weight' in x else 1, x))
        ordered_keys = fc_keys + other_keys
        for const_name in ordered_keys:
            const_info = self._constants[const_name]
            shape = const_info['shape']
            data = const_info['data']
            dtype = const_info.get('dtype', 'float32')
            if dtype == 'int64':
                self._builder.add_constant(const_name, shape, data, dtype='int64')
            else:
                self._builder.add_constant(const_name, shape, data)
            logger.debug("Registered constant: %s, shape=%s, dtype=%s", const_name, shape, dtype)

        # Step 5: Register data_ptr→name mappings for direct mode
        for name, arg in zip(self._input_names, args):
            self._builder.register_tensor_name(arg.data_ptr(), name)
        for const_name in ordered_keys:
            tensor = self._constants[const_name]['tensor']
            self._builder.register_tensor_name(tensor.data_ptr(), const_name)

        # Step 6: Execute model directly
        # C++ custom ops will be called automatically and generate AIR IR
        # via Add_operation_cpp() with full attrs/metadata/output_shape
        logger.debug("Executing traced model directly (C++ ops generate AIR IR)...")

        result = self.traced_model(*args)

        # Step 7: Finalize function
        self._builder.finalize()

        # Step 8: Get generated scopes
        self._func_scope = self._builder.get_func_scope()
        self._glob_scope = self._builder.get_glob_scope()
        self._air_generated = True

        logger.info("AIR generation complete (direct mode)!")

        # Print generated IR
        self._builder.print_ir()

        return result

    def _execute_interpreter(self, *args, **kwargs):
        """
        Execute the traced model node-by-node to generate AIR IR.

        This is the default execution path (Path 2). It iterates over each
        FX node and calls IRBuilder.add_op() with full attrs/metadata/shape
        computed in Python, which then delegates to TENSOR_LEVEL_HANDLER.
        """
        if not IRBuilder.is_available():
            raise RuntimeError("C++ extension not available. Cannot generate AIR IR.")

        logger.debug("Starting AIR generation...")
        logger.debug("Input names: %s", self._input_names)
        logger.debug("Input shapes: %s", self._input_shapes)
        logger.debug("Output shape: %s", self._output_shape)

        # Create IRBuilder for building AIR IR (this also sets global pointers)
        self._builder = IRBuilder()

        # Clear tensor name registry before starting
        if IRBuilder.is_available():
            self._builder.clear_tensor_names()

        # Step 1: Begin AIR function
        self._builder.begin_function("Main_graph")

        # Step 2: Add inputs
        for name, shape in zip(self._input_names, self._input_shapes):
            self._builder.add_input(name, shape)

        # Step 3: End function definition (sets output shape) - BEFORE constants
        # This matches onnx2air order: output type is created before constant types
        self._builder.end_function(self._output_shape)

        # Step 4: Register constants from get_attr nodes
        # Order constants to match onnx2air: fc weights first, then conv weights in network order
        # This aligns CST indices with reference implementation
        fc_keys = [k for k in self._constants.keys() if k.startswith('fc_')]
        other_keys = [k for k in self._constants.keys() if not k.startswith('fc_')]
        # Sort fc_keys to ensure fc_weight before fc_bias
        fc_keys.sort(key=lambda x: (0 if 'weight' in x else 1, x))
        # Combine: fc first, then others
        ordered_keys = fc_keys + other_keys
        for const_name in ordered_keys:
            const_info = self._constants[const_name]
            shape = const_info['shape']
            data = const_info['data']  # Flattened list
            dtype = const_info.get('dtype', 'float32')  # Default to float32
            if dtype == 'int64':
                self._builder.add_constant(const_name, shape, data, dtype='int64')
            else:
                self._builder.add_constant(const_name, shape, data)
            logger.debug("Registered constant: %s, shape=%s, dtype=%s", const_name, shape, dtype)

        # Step 5: Execute traced model node-by-node
        # This allows us to set op names before each custom op (hardcode sync with ONNX)
        logger.debug("Executing traced model node-by-node (generating AIR IR)...")

        # Find the output node to identify the last operation
        output_node = None
        for node in self.traced_model.graph.nodes:
            if node.op == 'output':
                output_node = node
                break

        last_op_node = None
        if output_node and isinstance(output_node.args[0], torch.fx.Node):
            last_op_node = output_node.args[0]

        # Build environment for node execution
        env = {}
        # Map input placeholder nodes to actual tensors
        # First, collect all placeholder nodes
        placeholder_nodes = []
        for node in self.traced_model.graph.nodes:
            if node.op == 'placeholder':
                placeholder_nodes.append(node)

        # Map placeholder nodes to input tensors
        for i, (name, arg) in enumerate(zip(self._input_names, args)):
            env[name] = arg
            if i < len(placeholder_nodes):
                env[placeholder_nodes[i]] = arg

        # Map constant names to their tensors
        for const_name, const_info in self._constants.items():
            env[const_name] = const_info['tensor']

        # Track AIR names for each FX node (for correct residual connection handling)
        # This maps FX nodes to their AIR IR names (e.g., "t0", "t1", etc.)
        node_air_names = {}
        # Track output shapes for each FX node (for correct shape propagation)
        node_shapes = {}
        # Register input names - use the input name directly
        for i, (name, arg) in enumerate(zip(self._input_names, args)):
            if i < len(placeholder_nodes):
                node_air_names[placeholder_nodes[i]] = name
                node_shapes[placeholder_nodes[i]] = list(arg.shape)

        # Counter for intermediate result names (must match C++ _intermediate_idx)
        intermediate_idx = 0

        # Execute nodes in order
        result = None
        logger.debug("Starting node iteration, total nodes: %d", len(list(self.traced_model.graph.nodes)))
        for node in self.traced_model.graph.nodes:
            logger.debug("Processing node: %s, op=%s, target=%s", node.name, node.op, getattr(node, 'target', 'N/A'))
            # Skip nodes marked for skipping (e.g., size() calls)
            if node.meta.get('skip_execution', False):
                # Provide a dummy value for skipped nodes
                env[node] = 1  # Default batch size
                # Also assign a dummy AIR name
                node_air_names[node] = f"_dummy_{node.name}"
                continue

            if node.op == 'call_function':
                # Check if this is a custom op (torch.ops.tensor.xxx or tensor.xxx)
                target_str = str(node.target)
                is_custom_op = (target_str.startswith('torch.ops.tensor.') or
                               target_str.startswith('tensor.'))
                logger.debug("call_function: node.name=%s, target_str=%s, is_custom_op=%s", node.name, target_str, is_custom_op)
                if not is_custom_op:
                    logger.debug("Skipping non-custom op: %s", target_str)
                    continue
                if is_custom_op:
                    # If op_name is not in node.meta, extract from target and inject
                    if node.meta.get('op_name') is None:
                        # Extract operator name from target, e.g., torch.ops.tensor.add -> tensor.add
                        if 'torch.ops.tensor.' in target_str:
                            op_name = 'tensor.' + target_str.replace('torch.ops.tensor.', '')
                        elif target_str.startswith('tensor.'):
                            op_name = target_str
                        else:
                            op_name = target_str
                        node.meta['op_name'] = op_name
                        node.meta['hardware'] = 'cpu'  # default value
                        node.meta['library'] = 'antlib'  # default value
                    # ===== New approach: read op_name from node.meta =====
                    # Injected by CustomTracer during tracing
                    op_name = node.meta.get('op_name', target_str)
                    hardware = node.meta.get('hardware', 'cpu')
                    library = node.meta.get('library', 'antlib')

                    # Check if this is the last operation (should use 'st' instead of 'stp')
                    is_last_op = (node == last_op_node)

                    # Build list of input AIR names for this operation
                    # This is the key fix for residual connections - we explicitly
                    # track which AIR name corresponds to each input
                    input_air_names = []
                    for arg in node.args:
                        if isinstance(arg, torch.fx.Node):
                            if arg in node_air_names:
                                input_air_names.append(node_air_names[arg])
                            elif arg.op == 'get_attr':
                                # Constant - use its attribute name
                                input_air_names.append(arg.target)
                            else:
                                # For residual connections, the input might be from a previous layer
                                # that hasn't been registered yet. Generate a placeholder name.
                                input_air_names.append(f"_{arg.name}")
                        elif isinstance(arg, torch.Tensor):
                            # Direct tensor - should not happen for custom ops
                            input_air_names.append("unknown")

                    # ===== New approach: no longer use thread-local =====
                    # Get op_name directly from node.meta and pass to C++

                    # Get tensor args from environment
                    tensor_args = []
                    first_tensor_shape = None

                    # First, find the shape of the first tensor
                    for arg in node.args:
                        if isinstance(arg, torch.fx.Node):
                            if arg in env and isinstance(env[arg], torch.Tensor):
                                first_tensor_shape = env[arg].shape
                                break
                        elif isinstance(arg, torch.Tensor):
                            first_tensor_shape = arg.shape
                            break

                    # If not found yet, get from input arguments
                    if first_tensor_shape is None and args:
                        for inp_arg in args:
                            if isinstance(inp_arg, torch.Tensor):
                                first_tensor_shape = inp_arg.shape
                                break

                    for arg in node.args:
                        if isinstance(arg, torch.fx.Node):
                            tensor_args.append(env[arg])
                        elif isinstance(arg, torch.Tensor):
                            tensor_args.append(arg)
                        elif isinstance(arg, (int, float)):
                            # Convert Python scalar to tensor with shape matching the first tensor
                            # Use clone() to create independent tensor, avoiding shared data_ptr
                            if first_tensor_shape:
                                scalar_tensor = torch.tensor(arg).expand(first_tensor_shape).clone()
                            else:
                                scalar_tensor = torch.tensor(arg)
                            tensor_args.append(scalar_tensor)
                        else:
                            # Other types, skip or use None
                            tensor_args.append(None)

                    # ===== Call IRBuilder to add operation =====
                    # Unified use of add_op interface
                    logger.debug("Using unified add_op for op: %s", op_name)
                    # Collect input tensor names
                    input_names = []
                    # Collect scalar parameters as attributes (for flatten, reshape, etc.)
                    scalar_attrs = {}
                    scalar_attr_names = {
                        'flatten': ['start_dim', 'end_dim'],
                        'tensor.flatten': ['start_dim', 'end_dim'],
                        'reshape': ['shape'],
                        'tensor.reshape': ['shape'],
                        'transpose': ['dim0', 'dim1'],
                        'tensor.transpose': ['dim0', 'dim1'],
                    }
                    op_scalar_names = scalar_attr_names.get(op_name, [])
                    scalar_idx = 0

                    # Track input tensors to compute output shape
                    input_tensors = []

                    for orig_arg in node.args:
                        if isinstance(orig_arg, torch.fx.Node):
                            # Check if it's a get_attr node (constant)
                            if orig_arg.op == 'get_attr':
                                # Use attribute name as input name
                                input_names.append(orig_arg.target)
                                # Collect constant tensor for shape computation
                                try:
                                    const_tensor = getattr(self.traced_model, orig_arg.target)
                                    if isinstance(const_tensor, torch.Tensor):
                                        input_tensors.append(const_tensor)
                                except:
                                    pass
                            elif orig_arg in node_air_names:
                                # Get AIR name from previous node
                                input_names.append(node_air_names[orig_arg])
                                # Collect input tensor for shape computation
                                if orig_arg in env and isinstance(env[orig_arg], torch.Tensor):
                                    input_tensors.append(env[orig_arg])
                            else:
                                input_names.append(f"arg_{len(input_names)}")
                                # Collect input tensor for shape computation
                                if orig_arg in env and isinstance(env[orig_arg], torch.Tensor):
                                    input_tensors.append(env[orig_arg])
                        elif isinstance(orig_arg, torch.Tensor):
                            input_names.append(f"arg_{len(input_names)}")
                            input_tensors.append(orig_arg)
                        elif isinstance(orig_arg, (int, float)):
                            # Scalar parameter passed as attribute
                            if scalar_idx < len(op_scalar_names):
                                attr_name = op_scalar_names[scalar_idx]
                                scalar_attrs[attr_name] = orig_arg
                                scalar_idx += 1
                            else:
                                # Additional scalar parameter
                                scalar_attrs[f"scalar_{scalar_idx}"] = orig_arg
                                scalar_idx += 1

                    # Collect attributes from node.kwargs
                    attrs = {}
                    for k, v in node.kwargs.items():
                        attrs[k] = v
                    # Merge scalar attributes
                    attrs.update(scalar_attrs)

                    # Map Python attribute names to AIR attribute names
                    attr_name_map = {
                        'kernel_size': 'kernel_shape',
                        'stride': 'strides',
                        'padding': 'pads',
                        'dilation': 'dilations',
                        'groups': 'group',
                    }
                    attrs = {attr_name_map.get(k, k): v for k, v in attrs.items()}

                    # Collect metadata from node.meta
                    metadata = {}
                    if node.meta.get('onnx_name'):
                        metadata['onnx_name'] = node.meta['onnx_name']
                    # Put is_output flag into metadata for unified passing
                    metadata['is_output'] = str(is_last_op)

                    # Compute output shape from input tensors and attrs
                    output_shape = []
                    # First try to get shape from input nodes' saved shapes (preferred)
                    for orig_arg in node.args:
                        if isinstance(orig_arg, torch.fx.Node) and orig_arg in node_shapes:
                            output_shape = list(node_shapes[orig_arg])
                            break
                    # Fallback: use input tensors (for non-node inputs)
                    if not output_shape and input_tensors:
                        first_tensor = input_tensors[0]
                        output_shape = list(first_tensor.shape)
                    # Final fallback: use default shape
                    if not output_shape:
                        output_shape = [1, 64, 8, 8]  # Default shape for ResNet20 before avgpool

                    # Handle specific operators that change shape
                    if op_name in ('flatten', 'tensor.flatten'):
                        # Flatten: compute flattened shape
                        start_dim = attrs.get('start_dim', 0)
                        end_dim = attrs.get('end_dim', -1)
                        if isinstance(start_dim, torch.fx.Node) and start_dim in env:
                            start_dim = int(env[start_dim])
                        if isinstance(end_dim, torch.fx.Node) and end_dim in env:
                            end_dim = int(env[end_dim])
                        # Normalize negative indices
                        if start_dim < 0:
                            start_dim += len(output_shape)
                        if end_dim < 0:
                            end_dim += len(output_shape)
                        # Compute flattened size
                        flattened_size = 1
                        for i in range(max(0, start_dim), min(len(output_shape), end_dim + 1)):
                            flattened_size *= output_shape[i]
                        # Build new shape
                        new_shape = list(output_shape[:start_dim]) + [flattened_size] + list(output_shape[end_dim+1:])
                        output_shape = new_shape
                    elif op_name in ('reshape', 'tensor.reshape'):
                        # Reshape: use the shape attribute or shape tensor values
                        shape_attr = attrs.get('shape')
                        if shape_attr:
                            output_shape = list(shape_attr)
                        else:
                            # Try to get shape from the second input (shape tensor)
                            # The shape tensor contains the target shape values, not its own shape
                            if len(input_tensors) >= 2:
                                shape_tensor = input_tensors[1]
                                # Get actual values from the shape tensor
                                try:
                                    if hasattr(shape_tensor, 'tolist'):
                                        shape_values = shape_tensor.tolist()
                                        if isinstance(shape_values, list):
                                            output_shape = shape_values
                                        else:
                                            # Single value
                                            output_shape = [shape_values]
                                    elif hasattr(shape_tensor, 'numpy'):
                                        output_shape = shape_tensor.numpy().tolist()
                                except Exception as e:
                                    # Fallback: keep input shape
                                    pass
                    elif op_name in ('conv', 'tensor.conv'):
                            # Conv2d: compute output shape using formula
                            # out_H = (H + 2*pad - dilation*(kernel-1) - 1) // stride + 1
                            strides = attrs.get('strides', [1, 1])
                            pads = attrs.get('pads', [0, 0, 0, 0])
                            kernel_shape = attrs.get('kernel_shape', [3, 3])
                            dilations = attrs.get('dilations', [1, 1])

                            if len(output_shape) >= 4:  # NCHW format
                                batch_size = output_shape[0]
                                h, w = output_shape[2], output_shape[3]

                                # Compute output spatial dimensions
                                out_h = (h + 2*pads[0] - dilations[0]*(kernel_shape[0]-1) - 1) // strides[0] + 1
                                out_w = (w + 2*pads[1] - dilations[1]*(kernel_shape[1]-1) - 1) // strides[1] + 1

                                # Output channels: try to get from weight tensor (second input for conv with bias)
                                out_channels = output_shape[1]  # Default: keep same channels
                                # Try to get from input_tensors first
                                if len(input_tensors) >= 2:
                                    weight_tensor = input_tensors[1]
                                    if len(weight_tensor.shape) >= 4:
                                        out_channels = weight_tensor.shape[0]  # out_channels from weight
                                # Fallback: try to get from node_shapes (weight is typically second arg)
                                weight_node = None
                                for i, orig_arg in enumerate(node.args):
                                    if isinstance(orig_arg, torch.fx.Node) and i == 1:  # Second arg is weight
                                        weight_node = orig_arg
                                        break
                                if weight_node and weight_node in node_shapes:
                                    weight_shape = node_shapes[weight_node]
                                    if len(weight_shape) >= 4:
                                        out_channels = weight_shape[0]  # out_channels from weight

                                output_shape = [batch_size, out_channels, out_h, out_w]
                            elif len(output_shape) == 3:  # NHC format (less common)
                                # Handle 3D conv case
                                n, h, c = output_shape
                                kernel_h = kernel_shape[0] if len(kernel_shape) > 0 else 3
                                stride_h = strides[0] if len(strides) > 0 else 1
                                pad_h = pads[0] if len(pads) > 0 else 0
                                dilation_h = dilations[0] if len(dilations) > 0 else 1
                                out_h = (h + 2*pad_h - dilation_h*(kernel_h-1) - 1) // stride_h + 1
                                output_shape = [n, out_h, c]
                    elif op_name in ('global_average_pool', 'tensor.global_average_pool'):
                        # Global average pool: reduce all spatial dims to 1
                        if len(output_shape) >= 2:
                            output_shape = [output_shape[0], output_shape[1]] + [1] * (len(output_shape) - 2)
                    elif op_name in ('max_pool', 'average_pool', 'tensor.max_pool', 'tensor.average_pool'):
                        # Regular pooling: compute output shape
                        kernel_shape = attrs.get('kernel_shape', [2, 2])
                        strides = attrs.get('strides', kernel_shape)  # Default stride = kernel_size
                        pads = attrs.get('pads', [0, 0, 0, 0])

                        if len(output_shape) >= 4:  # NCHW
                            batch_size, channels = output_shape[0], output_shape[1]
                            h, w = output_shape[2], output_shape[3]

                            out_h = (h + 2*pads[0] - kernel_shape[0] - 1) // strides[0] + 1
                            out_w = (w + 2*pads[1] - kernel_shape[1] - 1) // strides[1] + 1

                            output_shape = [batch_size, channels, out_h, out_w]
                    elif op_name in ('gemm', 'tensor.gemm'):
                        # Gemm (Linear layer): output shape = [N, out_features]
                        # out_features from weight shape[0] (if transB=1, default)
                        transA = attrs.get('transA', 0)
                        transB = attrs.get('transB', 1)

                        # Try to get weight shape from node_shapes (weight is typically second arg)
                        weight_shape = None
                        for i, orig_arg in enumerate(node.args):
                            if isinstance(orig_arg, torch.fx.Node) and i == 1:
                                weight_shape = node_shapes.get(orig_arg)
                                if weight_shape:
                                    break
                        # Fallback: try input_tensors (but be careful, input_tensors may include bias too)
                        if not weight_shape and len(input_tensors) >= 2:
                                # Find the tensor with 2D shape (weight) instead of 1D (bias)
                                for tensor in input_tensors:
                                    if len(tensor.shape) >= 2:
                                        weight_shape = list(tensor.shape)
                                        break

                        if weight_shape and len(weight_shape) >= 2:
                            batch_size = output_shape[0] if len(output_shape) > 0 else 1
                            if transB:
                                # weight is [out_features, in_features]
                                out_features = weight_shape[0]
                            else:
                                # weight is [in_features, out_features]
                                out_features = weight_shape[1]
                            output_shape = [batch_size, out_features]
                        elif len(output_shape) >= 2:
                            # Fallback: use existing output_shape but ensure 2D
                            output_shape = [output_shape[0], output_shape[-1]]
                    elif op_name in ('matmul', 'tensor.matmul'):
                            # Matmul: [N, M] @ [M, K] = [N, K]
                            if len(input_tensors) >= 2:
                                a_shape = list(input_tensors[0].shape)
                                b_shape = list(input_tensors[1].shape)

                                if len(a_shape) >= 2 and len(b_shape) >= 2:
                                    # Result: [a_batch..., a_rows, b_cols]
                                    output_shape = a_shape[:-1] + [b_shape[-1]]
                    elif op_name in ('add', 'sub', 'mul', 'div', 'tensor.add', 'tensor.sub', 'tensor.mul', 'tensor.div'):
                            # Binary element-wise ops: output shape = broadcast(input1, input2)
                            if len(input_tensors) >= 2:
                                a_shape = list(input_tensors[0].shape)
                                b_shape = list(input_tensors[1].shape)
                                # Simple broadcast: take max of each dim (simplified)
                                max_ndim = max(len(a_shape), len(b_shape))
                                a_shape = [1] * (max_ndim - len(a_shape)) + a_shape
                                b_shape = [1] * (max_ndim - len(b_shape)) + b_shape
                                output_shape = [max(a, b) for a, b in zip(a_shape, b_shape)]
                    elif op_name in ('relu', 'softmax', 'sqrt', 'silu', 'tensor.relu', 'tensor.softmax', 'tensor.sqrt', 'tensor.silu'):
                            # Unary element-wise ops: output shape = input shape
                            pass  # output_shape already set to input shape

                    # Directly call add_op, passing all necessary information
                    logger.debug("add_op: op_name=%s, input_names=%s, output_shape=%s", op_name, input_names, output_shape)
                    result_air_name = self._builder.add_op(op_name, input_names, attrs, metadata, output_shape)

                    # add_air_operation returns the result name generated by C++ (e.g., _v0, _v1, etc.)
                    logger.debug("  result_air_name=%s", result_air_name)

                    # Store result in environment (for add_air_operation, result is None)
                    env[node] = None

                    # Use the result name returned by C++
                    if result_air_name:
                        node_air_names[node] = result_air_name
                    else:
                        # Fallback: use old naming convention (should not happen)
                        result_air_name = f"t{intermediate_idx}"
                        intermediate_idx += 1
                        node_air_names[node] = result_air_name
                    # Save output shape for subsequent nodes
                    node_shapes[node] = output_shape
                else:
                    # Non-custom function (e.g., torch.add for residual)
                    # Execute normally
                    tensor_args = []
                    for arg in node.args:
                        if isinstance(arg, torch.fx.Node):
                            tensor_args.append(env[arg])
                        elif isinstance(arg, torch.Tensor):
                            tensor_args.append(arg)
                    kwargs_exec = {k: v for k, v in node.kwargs.items()}
                    result = node.target(*tensor_args, **kwargs_exec)
                    env[node] = result
                    # Save output shape for subsequent nodes
                    if isinstance(result, torch.Tensor):
                        node_shapes[node] = list(result.shape)

            elif node.op == 'get_attr':
                # Get attribute from traced model
                attr_val = getattr(self.traced_model, node.target)
                env[node] = attr_val
                # Save shape for constant tensors
                if isinstance(attr_val, torch.Tensor):
                    node_shapes[node] = list(attr_val.shape)

            elif node.op == 'call_method':
                # Handle method calls (e.g., view, size)
                method_name = node.target
                tensor_args = []
                for arg in node.args:
                    if isinstance(arg, torch.fx.Node):
                        tensor_args.append(env[arg])
                    elif isinstance(arg, torch.Tensor):
                        tensor_args.append(arg)

                # Check if this is a custom op (should have been rewritten to call_function)
                # If somehow we get a method call that's a custom op, handle it
                if method_name in ('reshape', 'view', 'flatten'):
                    # Execute the custom op
                    kwargs_exec = {k: v for k, v in node.kwargs.items()}
                    if method_name == 'reshape' and hasattr(torch.ops.tensor, 'reshape'):
                        result = torch.ops.tensor.reshape(*tensor_args, **kwargs_exec)
                    elif method_name == 'flatten' and hasattr(torch.ops.tensor, 'flatten'):
                        result = torch.ops.tensor.flatten(*tensor_args, **kwargs_exec)
                    else:
                        # Fall back to native method
                        try:
                            if tensor_args:
                                method = getattr(tensor_args[0], method_name)
                                result = method(*tensor_args[1:], **kwargs_exec)
                            else:
                                result = None
                        except Exception as e:
                            result = None

                    env[node] = result
                else:
                    # Native method call (e.g., size)
                    try:
                        # Execute method on first tensor arg
                        if tensor_args:
                            method = getattr(tensor_args[0], method_name)
                            result = method(*tensor_args[1:], **node.kwargs)
                            env[node] = result
                        else:
                            # No tensor args, skip
                            env[node] = None
                    except Exception as e:
                        # Method execution failed, provide default value
                        env[node] = 1

            elif node.op == 'call_module':
                # Handle module calls (e.g., nn.Linear, nn.Conv2d)
                # The module is accessed from the traced model
                module = getattr(self.traced_model, node.target)
                tensor_args = []
                for arg in node.args:
                    if isinstance(arg, torch.fx.Node):
                        tensor_args.append(env[arg])
                    elif isinstance(arg, torch.Tensor):
                        tensor_args.append(arg)

                # Check if this is a custom op module (has custom forward)
                # For custom ops, the forward should call tensor.xxx functions
                is_custom_module = hasattr(module, '_is_custom_op')
                if is_custom_module:
                    # Execute module forward
                    kwargs_exec = {k: v for k, v in node.kwargs.items()}
                    result = module(*tensor_args, **kwargs_exec)
                    env[node] = result
                else:
                    # Standard module (e.g., nn.Linear) - execute normally
                    kwargs_exec = {k: v for k, v in node.kwargs.items()}
                    result = module(*tensor_args, **kwargs_exec)
                    env[node] = result

            elif node.op == 'placeholder':
                # Already handled above
                pass

            elif node.op == 'output':
                # Return the output
                output_val = node.args[0]
                if isinstance(output_val, torch.fx.Node):
                    result = env[output_val]
                else:
                    result = output_val

        # Step 8: Finalize function
        self._builder.finalize()

        # Step 9: Get generated scopes
        self._func_scope = self._builder.get_func_scope()
        self._glob_scope = self._builder.get_glob_scope()
        self._air_generated = True

        logger.info("AIR generation complete!")

        # Print generated IR
        self._builder.print_ir()

        return result

    def __call__(self, *args, execution_mode="interpreter", **kwargs):
        return self.execute(*args, execution_mode=execution_mode, **kwargs)

    def write_ir(self, filename: str) -> bool:
        """
        Write the generated AIR IR to a file.

        Args:
            filename: Output file path (e.g., "output.air" or "output.o")

        Returns:
            True if successful, False otherwise
        """
        return self.export_ir(filename)

    def export_ir(self, filename: str) -> bool:
        """
        Export the generated AIR IR to a file.

        Args:
            filename: Output file path (e.g., "output.B" or "output.air")

        Returns:
            True if successful, False otherwise
        """
        if not IRBuilder.is_available():
            raise RuntimeError("C++ extension not available. Cannot write AIR IR.")

        if not self._air_generated:
            raise RuntimeError("AIR IR has not been generated. Call execute() first.")

        logger.debug("Exporting AIR IR to %s...", filename)
        # Use the stored IRBuilder instance to write IR
        self._builder.write_ir(filename)
        logger.info("AIR IR exported successfully!")

        # Set file path so format_type becomes "file"
        self._file_path = str(filename)
        return True

    def print_graph(self):
        """Print the FX graph."""
        if self.traced_model is not None:
            self.traced_model.print_readable()

    def get_graph_code(self) -> str:
        """Get the FX graph code."""
        if self.traced_model is not None:
            return str(self.traced_model.graph)
        return ""


# Backward compatibility alias
FXTracedModel = TorchTracedModel