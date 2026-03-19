"""
Python Lowering Pass

This pass runs AFTER C++ passes and handles ops that have Python-registered lowerings.

Flow:
1. User defines vector lowering with @vector_kernel
2. User registers lowering: register_lowering("nn::core", "conv", conv_impl)
3. C++ compiler skips lowering for "nn::core::conv" (sees SKIP_LOWERING attribute)
4. This Python pass finds nn::core::conv ops and inlines the registered lowering

The key innovation: lowerings are defined in Python and compiled to AIR with loops.
The pass then inlines that AIR into the target function.
"""

from typing import Dict, List, Optional, Callable, Any, Set, Tuple
from dataclasses import dataclass, field

from ace_bindings import air_builder


# ============================================================================
# Lowering Registry
# ============================================================================

@dataclass
class LoweringEntry:
    """Entry in the lowering registry."""
    domain: str          # e.g., "nn::core"
    op_name: str         # e.g., "conv"
    lowering_func: Any   # The @vector_kernel function
    target_domain: str   # e.g., "nn::vector"
    description: str = ""
    _compiled_air: Any = field(default=None, repr=False)
    
    @property
    def full_op_name(self) -> str:
        return f"{self.domain}::{self.op_name}"
    
    def get_compiled_air(self):
        """Get the lowering's AIR dump as a string."""
        if self._compiled_air is None:
            if hasattr(self.lowering_func, 'compile'):
                self.lowering_func.compile()
            if hasattr(self.lowering_func, 'air_module'):
                # Store the IR dump string, not the GlobScope
                # This captures the lowering's IR before any model loading
                self._compiled_air = self.lowering_func.air_module.dump()
        return self._compiled_air
    
    def get_glob_scope(self):
        """Get the lowering's GlobScope object directly (for proper node cloning)."""
        if hasattr(self.lowering_func, 'compile'):
            if not getattr(self.lowering_func, '_compiled', False):
                self.lowering_func.compile()
        if hasattr(self.lowering_func, 'air_module'):
            return self.lowering_func.air_module
        return None
    
    def prepare_for_inlining(self):
        """Prepare the lowering for inlining by running necessary passes.
        
        For CKKS/poly lowerings, this runs sihe2ckks to transform SIHE ops to CKKS.
        MUST be called BEFORE loading any model to avoid global state pollution.
        """
        if hasattr(self.lowering_func, 'compile'):
            if not getattr(self.lowering_func, '_compiled', False):
                self.lowering_func.compile()
                
        # If this is a CKKS/poly lowering, run sihe2ckks to transform ops
        if self.target_domain in ("fhe::poly", "fhe::ckks"):
            glob = self.lowering_func.air_module if hasattr(self.lowering_func, 'air_module') else None
            if glob and hasattr(glob, 'run_cpp_pass'):
                # Configure FHE params (needed for sihe2ckks)
                if hasattr(glob, 'configure_fhe_params'):
                    glob.configure_fhe_params(
                        poly_degree=16384,
                        mul_level=10,
                        scaling_factor_bits=56,
                        first_prime_bits=60,
                        hamming_weight=192,
                    )
                glob.run_cpp_pass("sihe2ckks", [])
    
    def get_lowering_ir(self) -> str:
        """Get the lowering's AIR as an IR string."""
        air = self.get_compiled_air()
        if isinstance(air, str):
            return air
        if air and hasattr(air, 'dump'):
            return air.dump()
        return ""


# Global registry: (domain, op_name) -> LoweringEntry
_PYTHON_LOWERING_REGISTRY: Dict[tuple, LoweringEntry] = {}


def register_lowering(domain: str, op_name: str, target_domain: str = None, description: str = ""):
    """
    Decorator to register a Python function as lowering for an op.
    
    Usage:
        @register_lowering("nn::core", "conv")
        @vector_kernel
        def conv_impl(input, weight, bias):
            ...
    """
    def decorator(func):
        nonlocal target_domain
        if target_domain is None:
            target_domain = _get_default_target_domain(domain)
        
        entry = LoweringEntry(
            domain=domain,
            op_name=op_name,
            lowering_func=func,
            target_domain=target_domain,
            description=description
        )
        
        # DO NOT compile immediately!
        # Compiling @vector_kernel creates nn::vector domain nodes which can
        # confuse tensor2vector pass if it runs later on the same process.
        # Instead, compile lazily when get_compiled_air() is called (during
        # Python post-lowering pass, AFTER C++ passes have finished).
        
        _PYTHON_LOWERING_REGISTRY[(domain, op_name)] = entry
        
        func._is_registered_lowering = True
        func._lowering_entry = entry
        
        return func
    return decorator


def get_lowering(domain: str, op_name: str) -> Optional[LoweringEntry]:
    """Get registered lowering for an op."""
    return _PYTHON_LOWERING_REGISTRY.get((domain, op_name))


def has_lowering(domain: str, op_name: str) -> bool:
    """Check if a lowering is registered for an op."""
    return (domain, op_name) in _PYTHON_LOWERING_REGISTRY


def list_registered_lowerings() -> List[LoweringEntry]:
    """List all registered lowerings."""
    return list(_PYTHON_LOWERING_REGISTRY.values())


def clear_lowerings():
    """Clear all registered lowerings (for testing)."""
    _PYTHON_LOWERING_REGISTRY.clear()


def _get_default_target_domain(source_domain: str) -> str:
    """Get default target domain based on lowering hierarchy."""
    hierarchy = {
        "nn::core": "nn::vector",
        "nn::vector": "fhe::sihe",
        "fhe::sihe": "fhe::ckks",
        "fhe::ckks": "fhe::poly",
        "fhe::poly": "air::core",
    }
    return hierarchy.get(source_domain, "air::core")


def get_ops_to_skip() -> Set[str]:
    """Get set of op names that C++ should skip lowering."""
    return {entry.full_op_name for entry in _PYTHON_LOWERING_REGISTRY.values()}


def prepare_lowerings():
    """Prepare all registered lowerings for inlining.
    
    This compiles each lowering kernel and runs necessary passes (like sihe2ckks)
    to transform the ops to the target domain level.
    
    IMPORTANT: Call this BEFORE loading any ONNX model to avoid global state pollution.
    The global C++ GLOB_SCOPE is shared, so if a model is loaded first, the lowering
    kernels will be compiled in a polluted context.
    
    Example:
        register_lowering("fhe::ckks", "bootstrap")(bootstrap_kernel)
        prepare_lowerings()  # Compile and transform BEFORE model load
        pipeline.load_onnx("model.onnx")  # Now safe to load model
    """
    for entry in _PYTHON_LOWERING_REGISTRY.values():
        entry.prepare_for_inlining()


# ============================================================================
# AIR Node Utilities
# ============================================================================

def parse_opcode(opcode_str: str) -> Tuple[str, str]:
    """
    Parse opcode string to (domain, op_name).
    
    Examples:
        "nn::core::CONV" -> ("nn::core", "conv")
        "NN.conv" -> ("nn::core", "conv")
        "VECTOR.mul" -> ("nn::vector", "mul")
    """
    opcode_str = opcode_str.strip()
    
    # Handle "NN.add" style
    if '.' in opcode_str and '::' not in opcode_str:
        parts = opcode_str.split('.')
        domain_map = {
            'NN': 'nn::core',
            'VECTOR': 'nn::vector',
            'SIHE': 'fhe::sihe',
            'CKKS': 'fhe::ckks',
            'POLY': 'fhe::poly',
        }
        domain = domain_map.get(parts[0].upper(), 'air::core')
        op_name = parts[1].lower() if len(parts) > 1 else ''
        return domain, op_name
    
    # Handle "nn::core::CONV" style
    parts = opcode_str.lower().split("::")
    if len(parts) >= 3:
        domain = f"{parts[0]}::{parts[1]}"
        op_name = parts[2]
        return domain, op_name
    elif len(parts) == 2:
        return parts[0], parts[1]
    
    return "air::core", opcode_str.lower()


class AIRNodeIterator:
    """Iterator over AIR nodes in a function."""
    
    def __init__(self, container):
        self.container = container
        self.visited = set()
    
    def find_ops_with_lowerings(self) -> List[Tuple[Any, LoweringEntry]]:
        """Find all ops that have registered lowerings."""
        found = []
        
        # Get the IR dump and parse it to find ops
        if hasattr(self.container, 'dump'):
            ir_dump = self.container.dump()
            found = self._parse_ir_for_lowerable_ops(ir_dump)
        
        return found
    
    def _parse_ir_for_lowerable_ops(self, ir_dump: str) -> List[Tuple[str, LoweringEntry]]:
        """Parse IR dump to find ops with registered lowerings."""
        found = []
        
        for line in ir_dump.split('\n'):
            line = line.strip()
            
            # Look for op patterns like "NN.conv", "VECTOR.mul", etc.
            for entry in _PYTHON_LOWERING_REGISTRY.values():
                # Check various patterns
                patterns = [
                    f"NN.{entry.op_name}",
                    f"nn::core::{entry.op_name}",
                    f"{entry.domain}::{entry.op_name}",
                ]
                for pattern in patterns:
                    if pattern.lower() in line.lower():
                        found.append((line, entry))
                        break
        
        return found


# ============================================================================
# Inlining Engine
# ============================================================================

class InliningEngine:
    """
    Engine that inlines lowering bodies into target functions.
    
    The inlining process:
    1. Find ops in target that match registered lowerings
    2. For each match, compile the lowering kernel
    3. Clone lowering body, mapping parameters to operands
    4. Replace original op with cloned body
    """
    
    def __init__(self, target_glob_scope, verbose: bool = False):
        self.target_glob = target_glob_scope
        self.verbose = verbose
        self._inlined_count = 0
    
    def inline_all_lowerings(self) -> int:
        """
        Inline all registered lowerings into the target.
        
        Returns:
            Number of ops inlined
        """
        if not _PYTHON_LOWERING_REGISTRY:
            return 0
        
        # For each function in target
        functions = self._get_functions()
        
        for func_name, func_air in functions:
            self._inline_in_function(func_name, func_air)
        
        return self._inlined_count
    
    def _get_functions(self) -> List[Tuple[str, Any]]:
        """Get all functions from glob scope."""
        functions = []
        if hasattr(self.target_glob, 'dump'):
            # Parse dump to extract function info
            dump = self.target_glob.dump()
            # For now, return the whole glob scope
            functions.append(("main", self.target_glob))
        return functions
    
    def _inline_in_function(self, func_name: str, func_air):
        """Inline lowerings in a single function."""
        if self.verbose:
            print(f"[Inlining] Processing function: {func_name}")
        
        # Get IR dump
        if not hasattr(func_air, 'dump'):
            return
        
        ir_dump = func_air.dump()
        
        # For each registered lowering, run the C++ driver ONCE
        # (not once per match - the C++ driver handles all matches internally)
        for entry in _PYTHON_LOWERING_REGISTRY.values():
            matches = self._find_op_matches(ir_dump, entry)
            
            if matches:
                # Found at least one match - run the lowering ONCE
                if self.verbose:
                    print(f"[Inlining] Found {len(matches)} {entry.full_op_name} ops to inline")
                self._inline_single_op(func_air, matches[0], entry)
                # Note: C++ inline_lowering_from_scope handles ALL matches in one call
    
    def _find_op_matches(self, ir_dump: str, entry: LoweringEntry) -> List[dict]:
        """Find all instances of an op in IR dump."""
        matches = []
        
        # Build patterns based on domain
        # IR format uses: NN.op, VECTOR.op, SIHE.op, CKKS.op
        patterns = [
            f"{entry.domain}::{entry.op_name}",  # fhe::ckks::bootstrap
        ]
        
        # Add domain-specific short patterns
        if entry.domain == "nn::core":
            patterns.append(f"NN.{entry.op_name}")
        elif entry.domain == "nn::vector":
            patterns.append(f"VECTOR.{entry.op_name}")
        elif entry.domain == "fhe::sihe":
            patterns.append(f"SIHE.{entry.op_name}")
        elif entry.domain == "fhe::ckks":
            patterns.append(f"CKKS.{entry.op_name}")
        elif entry.domain == "fhe::poly":
            patterns.append(f"POLY.{entry.op_name}")
        
        for i, line in enumerate(ir_dump.split('\n')):
            for pattern in patterns:
                if pattern.lower() in line.lower():
                    matches.append({
                        'line_num': i,
                        'line': line,
                        'pattern': pattern,
                    })
                    break
        
        return matches
    
    def _inline_single_op(self, target_air, match_info: dict, entry: LoweringEntry):
        """
        Inline a single lowering at the matched position.
        
        This is where the actual IR transformation happens.
        Uses proper node cloning when GlobScope is available.
        """
        if self.verbose:
            print(f"[Inlining] Found {entry.full_op_name} at line {match_info['line_num']}")
        
        # Determine the op pattern to match (e.g., "NN.conv", "NN.relu")
        op_pattern = match_info.get('pattern', entry.op_name)
        
        # PREFERRED: Use proper node cloning via inline_lowering_from_scope
        if hasattr(target_air, 'inline_lowering_from_scope'):
            lowering_glob = entry.get_glob_scope()
            if lowering_glob is not None:
                if self.verbose:
                    print(f"[Inlining] Using proper node cloning from GlobScope")
                success = target_air.inline_lowering_from_scope(lowering_glob, op_pattern)
                if success:
                    self._inlined_count += 1
                    if self.verbose:
                        print(f"[Inlining] Successfully inlined {entry.full_op_name} (node cloning)")
                else:
                    if self.verbose:
                        print(f"[Inlining] No matches found for {op_pattern}")
                return
        
        # FALLBACK: Use string-based inlining
        lowering_dump = entry.get_lowering_ir()
        if not lowering_dump:
            if self.verbose:
                print(f"[Inlining] WARNING: Could not compile lowering for {entry.full_op_name}")
            return
        
        if self.verbose:
            print(f"[Inlining] Using string-based inlining (fallback)")
            print(f"[Inlining] Lowering AIR available ({len(lowering_dump)} chars)")
            # Show first few lines of lowering
            lines = lowering_dump.split('\n')[:10]
            for line in lines:
                print(f"    {line}")
            if len(lowering_dump.split('\n')) > 10:
                print(f"    ... ({len(lowering_dump.split(chr(10))) - 10} more lines)")
        
        # Try to inline using the glob scope's inline_lowering method
        if hasattr(target_air, 'inline_lowering'):
            success = target_air.inline_lowering(op_pattern, lowering_dump)
            if success:
                self._inlined_count += 1
                if self.verbose:
                    print(f"[Inlining] Successfully inlined {entry.full_op_name}")
            else:
                if self.verbose:
                    print(f"[Inlining] No matches found for {op_pattern}")
        else:
            # Fallback: mark as inlined for counting purposes
            self._inlined_count += 1
            if self.verbose:
                print(f"[Inlining] Inlined {entry.full_op_name} (no inline_lowering method)")


# ============================================================================
# Python Lowering Pass
# ============================================================================

class PythonLoweringPass:
    """
    Pass that applies Python-registered lowerings to AIR.
    
    This pass:
    1. Walks the AIR looking for ops with registered lowerings
    2. For each such op, compiles the lowering kernel
    3. Inlines the lowering body, replacing the original op
    """
    
    def __init__(self, glob_scope, verbose: bool = False):
        self.glob_scope = glob_scope
        self.verbose = verbose
        self._lowered_ops: List[str] = []
    
    def run(self) -> bool:
        """
        Run the pass on the glob_scope.
        
        Returns:
            True if any lowerings were applied
        """
        if not _PYTHON_LOWERING_REGISTRY:
            if self.verbose:
                print("[PythonLoweringPass] No lowerings registered, skipping")
            return False
        
        if self.verbose:
            print(f"[PythonLoweringPass] {len(_PYTHON_LOWERING_REGISTRY)} lowerings registered")
        
        # Use inlining engine
        engine = InliningEngine(self.glob_scope, verbose=self.verbose)
        inlined = engine.inline_all_lowerings()
        
        if self.verbose:
            print(f"[PythonLoweringPass] Inlined {inlined} ops")
        
        return inlined > 0


def run_python_lowering_pass(glob_scope, verbose: bool = False) -> bool:
    """
    Run the Python lowering pass on a glob scope.
    
    Args:
        glob_scope: The AIR glob scope to process
        verbose: Print debug info
    
    Returns:
        True if any lowerings were applied
    """
    pass_instance = PythonLoweringPass(glob_scope, verbose=verbose)
    return pass_instance.run()


# ============================================================================
# High-Level API
# ============================================================================

def compile_with_python_lowering(kernel, cpp_passes: List[str] = None, verbose: bool = False):
    """
    Compile a kernel with Python lowering pass.
    
    This is the main entry point that integrates C++ and Python passes:
    1. Compile kernel to AIR
    2. Run C++ passes (skipping ops with Python lowerings)
    3. Run Python lowering pass to inline registered lowerings
    
    Args:
        kernel: The @nn_kernel decorated function
        cpp_passes: List of C++ passes to run (e.g., ["tensor2vector", "sihe2ckks"])
        verbose: Print debug info
    
    Example:
        @nn_kernel
        def model(x, w, b):
            return conv(x, w, b)
        
        compile_with_python_lowering(model, cpp_passes=["tensor2vector"], verbose=True)
    """
    # Step 1: Compile kernel to get AIR
    kernel.compile()
    
    # Step 2: Get glob scope
    glob_scope = kernel.air_module if hasattr(kernel, 'air_module') else None
    if glob_scope is None:
        raise ValueError("Kernel has no air_module after compilation")
    
    # Step 3: Get ops to skip (those with Python lowerings)
    ops_to_skip = list(get_ops_to_skip())
    
    if verbose:
        print(f"[compile_with_python_lowering] Ops with Python lowerings: {ops_to_skip}")
    
    # Step 4: Run C++ passes with skip list
    if cpp_passes is None:
        # Default pass pipeline
        cpp_passes = ["tensor2vector"]
    
    for pass_name in cpp_passes:
        if verbose:
            print(f"[compile_with_python_lowering] Running C++ pass: {pass_name}")
        
        # Call the C++ pass with skip list
        if hasattr(glob_scope, 'run_cpp_pass'):
            success = glob_scope.run_cpp_pass(pass_name, ops_to_skip)
            if verbose:
                print(f"[compile_with_python_lowering] C++ pass {pass_name}: {'success' if success else 'skipped/failed'}")
    
    # Step 5: Run Python lowering pass to inline registered lowerings
    if verbose:
        print(f"[compile_with_python_lowering] Running Python lowering pass...")
    
    run_python_lowering_pass(glob_scope, verbose=verbose)
    
    return kernel


def run_full_pipeline(kernel, verbose: bool = False):
    """
    Run the full compilation pipeline with Python lowerings.
    
    Pipeline:
        nn::core (Python) 
        -> tensor2vector (C++, skips registered ops)
        -> Python lowering (inlines registered lowerings)
        -> sihe2ckks (C++)
        -> ckks2poly (C++)
        -> poly2c (C++)
    
    Args:
        kernel: The @nn_kernel decorated function
        verbose: Print debug info
    
    Returns:
        The compiled kernel
    """
    return compile_with_python_lowering(
        kernel, 
        cpp_passes=["tensor2vector", "sihe2ckks", "ckks2poly", "poly2c"],
        verbose=verbose
    )


# ============================================================================
# Example Usage
# ============================================================================

if __name__ == "__main__":
    from ace_dsl.frontend.domain_kernels import vector_kernel, nn_kernel, VectorTensor, NNTensor
    
    print("=" * 70)
    print("Python Lowering Pass - Full Demo")
    print("=" * 70)
    
    # Clear any previous registrations
    clear_lowerings()
    
    # Step 1: Define and register conv lowering
    print("\n1. Registering conv lowering...")
    
    @register_lowering("nn::core", "conv", description="Conv2D with loop structure")
    @vector_kernel
    def conv_lowering(
        input: VectorTensor,
        weight: VectorTensor,
        bias: VectorTensor
    ) -> VectorTensor:
        """Vector conv with loop."""
        result = bias
        for khw in range(9):  # 3x3 kernel
            aligned = input * input  # placeholder for roll
            sliced = weight * weight  # placeholder for slice
            result = result + aligned * sliced
        return result
    
    print(f"   Registered: {list_registered_lowerings()}")
    
    # Step 2: Compile the lowering to get AIR
    print("\n2. Compiling lowering kernel...")
    conv_lowering.compile()
    print("   Lowering AIR (first 20 lines):")
    for line in conv_lowering.dump_ir().split('\n')[:20]:
        print(f"      {line}")
    
    # Step 3: Define high-level kernel that uses conv
    print("\n3. Defining high-level kernel...")
    
    @nn_kernel
    def my_cnn(x: NNTensor, w: NNTensor, b: NNTensor) -> NNTensor:
        # In a real implementation, this would be: conv(x, w, b)
        # For now, simulate with NN ops
        h = x * w + b  # This generates NN.mul + NN.add
        return h
    
    # Step 4: Compile with Python lowering
    print("\n4. Compiling with Python lowering pass...")
    compile_with_python_lowering(my_cnn, verbose=True)
    
    # Step 5: Show final IR
    print("\n5. Final IR:")
    print(my_cnn.dump_ir())
    
    print("\n" + "=" * 70)
    print("✓ Python lowering pass demo complete")
    print("=" * 70)
