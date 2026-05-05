# ir_graph.py
import logging
from typing import List, Dict, Optional, Any

# Get logger
logger = logging.getLogger(__name__)

# No internal dependencies in this file

class IRNode:
    """Keep original definition (compatibility)"""
    def __init__(self, name: str):
        self.name = name
        self.op_type = ""
        self.inputs: List[str] = []
        self.outputs: List[str] = []
        self.attributes: Dict[str, Any] = {}
        self.dtype = None
        self.shape = None

class BasicBlock:
    """Basic block: A sequence of instructions executed sequentially"""
    def __init__(self, name: str):
        self.name = name
        self.nodes: List[IRNode] = []  # Nodes within a block (order)
        self.successors: List['BasicBlock'] = []  # Successor block
        self.predecessors: List['BasicBlock'] = []  # Precursor block

    def add_node(self, node: Dict[str, Any]):
        self.nodes.append(node)

class FHEGraph:
    """Upgrade to CFG structure"""
    def __init__(self, name: str):
        self.name = name
        # Added: basic block collection
        self.blocks: Dict[str, BasicBlock] = {}
        self.entry_block: Optional[BasicBlock] = None
        
        # Preserve the original field (compatibility)
        self.input_nodes: List[str] = []
        self.output_nodes: List[str] = []
        self.metadata: Dict[str, Any] = {}
        self._name_counter: Dict[str, int] = {}

    def generate_unique_name(self, prefix: str = "tmp") -> str:
        count = self._name_counter.get(prefix, 0) + 1
        self._name_counter[prefix] = count
        return f"{prefix}_{count}"

    # Added: Helper methods
    def add_block(self, block: BasicBlock):
        self.blocks[block.name] = block
    
    def get_all_nodes(self) -> Dict[str, dict]:
        """Get all nodes (for compatibility with old interfaces)"""
        all_nodes = {}
        for block in self.blocks.values():
            for node in block.nodes:
                all_nodes[node.name] = node
        return all_nodes
    
    def print_tabular(self):
        """Print IR in tabular form (similar to print_tabular in PyTorch)"""
        from tabulate import tabulate  # You need to install the tabulate library

        logger.info("=== IR Graph: %s ===", self.name)
        logger.info("Inputs: %s", self.input_nodes)
        logger.info("Outputs: %s", self.output_nodes)

        for block_name, block in self.blocks.items():
            if not block.nodes:
                continue

            logger.info("Block '%s':", block_name)

            # Prepare tabular data
            table_data = []
            headers = ["Index", "Name", "Op Type", "Inputs", "Shape", "Dtype"]

            for i, node in enumerate(block.nodes):
                inputs_str = ", ".join(node.inputs) if node.inputs else "-"
                shape_str = str(node.shape) if node.shape is not None else "-"
                dtype_str = str(node.dtype) if node.dtype is not None else "-"

                table_data.append([
                    i,
                    node.name,
                    node.op_type,
                    inputs_str,
                    shape_str,
                    dtype_str
                ])

            # Print the form
            logger.info("\n%s", tabulate(table_data, headers=headers, tablefmt="grid"))

        logger.info("=" * 80)
  
    def print_ir(self):
        """Print the IR in a readable format"""
        logger.info("=== IR Graph: %s ===", self.name)
        logger.info("Inputs: %s", self.input_nodes)
        logger.info("Outputs: %s", self.output_nodes)
        logger.info("Blocks: %s", list(self.blocks.keys()))

        for block_name, block in self.blocks.items():
            logger.info("Block '%s':", block_name)
            for i, node in enumerate(block.nodes):
                inputs_str = ", ".join(node.inputs) if node.inputs else ""
                outputs_str = ", ".join(node.outputs) if node.outputs else ""
                attrs_str = f" {{{node.attributes}}}" if node.attributes else ""
                logger.info("  %d: %s = %s(%s)%s", i, node.name, node.op_type, inputs_str, attrs_str)
                if node.shape or node.dtype:
                    meta_str = []
                    if node.shape:
                        meta_str.append(f"shape={node.shape}")
                    if node.dtype:
                        meta_str.append(f"dtype={node.dtype}")
                    logger.info("      [%s]", ', '.join(meta_str))

        logger.info("=" * 50)

    def to_dict(self):
        """Convert to dictionary format for easy debugging"""
        return {
            'name': self.name,
            'input_nodes': self.input_nodes,
            'output_nodes': self.output_nodes,
            'blocks': {
                name: {
                    'nodes': [
                        {
                            'name': node.name,
                            'op_type': node.op_type,
                            'inputs': node.inputs,
                            'outputs': node.outputs,
                            'attributes': node.attributes,
                            'shape': node.shape,
                            'dtype': node.dtype
                        }
                        for node in block.nodes
                    ]
                }
                for name, block in self.blocks.items()
            }
        }

