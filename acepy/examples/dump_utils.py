"""
Dump Utilities for ACE FHE Compiler Tests
==========================================

Provides standardized utilities to dump AIR (intermediate representation) at 
each phase of the FHE compilation pipeline, plus final C code output.

Usage (Simple):
    from dump_utils import Pipeline
    
    # Load model and run full pipeline
    pipeline = Pipeline("my_test")
    pipeline.load_onnx("model.onnx")
    pipeline.configure_fhe(scaling_factor_bits=56)
    result = pipeline.run()
    
    if result.success:
        print(f"Generated {len(result.c_code)} bytes of C code")

Usage (Manual control):
    from dump_utils import PipelineDumper

    dumper = PipelineDumper("my_test")
    
    # After loading ONNX model
    dumper.dump_air(glob, "initial", "After ONNX loading (nn::core)")
    
    # After each pass
    glob.run_cpp_pass("tensor2vector")
    dumper.dump_air(glob, "tensor2vector", "After tensor2vector (nn::vector)")
    
    # After final C code generation
    dumper.dump_c_code(c_code)
    
    # Print summary of all dumps
    dumper.print_summary()
"""

import os
import time
from dataclasses import dataclass, field
from typing import Optional, Dict, List, Any, Callable
from enum import Enum


class PipelineDumper:
    """
    Utility class to dump AIR at each phase of the FHE compilation pipeline.
    
    Stores all dumps in:  {script_dir}/output/{prefix}/
    
    Standard phases:
        initial        - After ONNX loading (nn::core level)
        tensor2vector  - After tensor2vector pass (nn::vector level)
        vector2sihe    - After vector2sihe pass (fhe::sihe level)
        sihe2ckks      - After sihe2ckks pass (fhe::ckks level)
        python_lower   - After Python lowering pass (fhe::ckks level)
        ckks_driver    - After CKKS driver (scale management)
        poly_driver    - After Poly driver (fhe::poly level)
        poly2c         - After poly2c (final AIR state)
    """
    
    # Standard phase descriptions
    PHASE_DESCRIPTIONS = {
        "initial": "Initial AIR after ONNX loading (nn::core)",
        "tensor2vector": "After tensor2vector (nn::vector)",
        "vector2sihe": "After vector2sihe (fhe::sihe)",
        "sihe2ckks": "After sihe2ckks (fhe::ckks)",
        "python_lower": "After Python lowering (fhe::ckks)",
        "ckks_driver": "After CKKS driver (fhe::ckks, scaled)",
        "poly_driver": "After Poly driver (fhe::poly)",
        "poly2c": "After poly2c (final)",
    }
    
    def __init__(self, prefix: str, output_dir: Optional[str] = None):
        """
        Initialize the dumper.
        
        Args:
            prefix: Prefix for output files (e.g., "resnet20_test")
            output_dir: Output directory (default: {script_dir}/output/{prefix}/)
        """
        self.prefix = prefix
        self.dumps: Dict[str, str] = {}  # phase -> file path
        self.stats: Dict[str, Dict[str, Any]] = {}  # phase -> stats
        self.timings: Dict[str, float] = {}  # phase -> time taken
        
        # Determine output directory
        if output_dir is None:
            # Default: examples/output/{prefix}/
            script_dir = os.path.dirname(os.path.abspath(__file__))
            output_dir = os.path.join(script_dir, "output", prefix)
        
        self.output_dir = output_dir
        os.makedirs(self.output_dir, exist_ok=True)
    
    def dump_air(self, glob, phase: str, description: Optional[str] = None, 
                 verbose: bool = True) -> str:
        """
        Dump AIR at a specific phase.
        
        Args:
            glob: GlobScope object with dump() method
            phase: Phase name (e.g., "tensor2vector")
            description: Human-readable description (auto-generated if None)
            verbose: Print progress
            
        Returns:
            Path to the dump file
        """
        if description is None:
            description = self.PHASE_DESCRIPTIONS.get(phase, f"After {phase}")
        
        # Get IR dump
        ir = glob.dump()
        
        # Compute stats
        stats = self._compute_stats(ir)
        self.stats[phase] = stats
        
        # Generate filename
        filename = f"{self.prefix}_air_{phase}.txt"
        filepath = os.path.join(self.output_dir, filename)
        
        # Write to file with header
        with open(filepath, "w") as f:
            f.write("=" * 80 + "\n")
            f.write(f"{description}\n")
            f.write(f"Phase: {phase}\n")
            f.write(f"Size: {len(ir):,} chars\n")
            f.write("-" * 80 + "\n")
            f.write("Op counts:\n")
            for op, count in sorted(stats.items()):
                if count > 0:
                    f.write(f"  {op}: {count}\n")
            f.write("=" * 80 + "\n\n")
            f.write(ir)
        
        self.dumps[phase] = filepath
        
        if verbose:
            print(f"  ✓ AIR dump ({phase}): {filepath}")
            if stats:
                top_ops = sorted(stats.items(), key=lambda x: -x[1])[:5]
                top_ops_str = ", ".join(f"{op}={cnt}" for op, cnt in top_ops if cnt > 0)
                if top_ops_str:
                    print(f"    Top ops: {top_ops_str}")
        
        return filepath
    
    def dump_c_code(self, c_code: str, filename: Optional[str] = None,
                    verbose: bool = True) -> str:
        """
        Dump generated C code.
        
        Args:
            c_code: The generated C source code
            filename: Output filename (default: {prefix}_output.c)
            verbose: Print progress
            
        Returns:
            Path to the C file
        """
        if filename is None:
            filename = f"{self.prefix}_output.c"
        
        filepath = os.path.join(self.output_dir, filename)
        
        with open(filepath, "w") as f:
            f.write(c_code)
        
        self.dumps["c_code"] = filepath
        
        if verbose:
            print(f"  ✓ C code: {filepath} ({len(c_code):,} bytes)")
        
        return filepath
    
    def dump_data_file(self, data_path: str, verbose: bool = True) -> str:
        """
        Record the data file path (for .msg files).
        
        Args:
            data_path: Path to the data file
            verbose: Print progress
            
        Returns:
            The data file path
        """
        self.dumps["data_file"] = data_path
        
        if verbose and os.path.exists(data_path):
            size = os.path.getsize(data_path)
            print(f"  ✓ Data file: {data_path} ({size:,} bytes)")
        
        return data_path
    
    def record_timing(self, phase: str, elapsed: float):
        """Record timing for a phase."""
        self.timings[phase] = elapsed
    
    def _compute_stats(self, ir: str) -> Dict[str, int]:
        """Compute operation counts from IR."""
        ops_to_count = [
            # NN ops
            "NN.conv", "NN.relu", "NN.add", "NN.mul", "NN.gemm",
            "NN.average_pool", "NN.global_average_pool", "NN.flatten",
            # Vector ops
            "VECTOR.roll", "VECTOR.mul", "VECTOR.add", "VECTOR.sub",
            # SIHE ops
            "SIHE.add", "SIHE.mul", "SIHE.sub", "SIHE.bootstrap",
            # CKKS ops
            "CKKS.add", "CKKS.mul", "CKKS.sub", "CKKS.rotate", "CKKS.bootstrap",
            # Poly ops
            "POLY.add", "POLY.mul", "POLY.ntt", "POLY.intt",
        ]
        
        ir_lower = ir.lower()
        stats = {}
        for op in ops_to_count:
            count = ir_lower.count(op.lower())
            if count > 0:
                stats[op] = count
        
        return stats
    
    def print_summary(self):
        """Print a summary of all dumps."""
        print("\n" + "=" * 70)
        print(f"Pipeline Dump Summary: {self.prefix}")
        print("=" * 70)
        
        print(f"\nOutput directory: {self.output_dir}")
        
        # List all dump files
        print("\nDump files:")
        for phase, filepath in self.dumps.items():
            if os.path.exists(filepath):
                size = os.path.getsize(filepath)
                timing = self.timings.get(phase, None)
                timing_str = f" ({timing:.2f}s)" if timing else ""
                print(f"  {phase}: {os.path.basename(filepath)} ({size:,} bytes){timing_str}")
        
        # Show stats progression
        if self.stats:
            print("\nOperation counts by phase:")
            all_ops = set()
            for stats in self.stats.values():
                all_ops.update(stats.keys())
            
            # Header
            phases = list(self.stats.keys())
            header = "Op".ljust(20) + " | ".join(p.ljust(12) for p in phases)
            print(f"  {header}")
            print(f"  {'-' * len(header)}")
            
            # Rows
            for op in sorted(all_ops):
                row = op.ljust(20)
                for phase in phases:
                    count = self.stats.get(phase, {}).get(op, 0)
                    row += str(count).rjust(12) + " | "
                print(f"  {row}")
        
        print("=" * 70)
    
    def get_output_dir(self) -> str:
        """Get the output directory path."""
        return self.output_dir


def run_full_pipeline_with_dumps(glob, dumper: PipelineDumper, 
                                  skip_ops: Optional[List[str]] = None,
                                  verbose: bool = True) -> Optional[str]:
    """
    Run the full FHE compilation pipeline with dumps at each stage.
    
    Args:
        glob: GlobScope with the loaded model
        dumper: PipelineDumper instance
        skip_ops: Operations to skip in sihe2ckks (for Python lowering)
        verbose: Print progress
        
    Returns:
        Generated C code, or None if failed
    """
    from ace_bindings import air_builder
    
    # Dump initial state
    dumper.dump_air(glob, "initial", verbose=verbose)
    
    # tensor2vector
    if verbose:
        print("\nRunning tensor2vector...")
    t0 = time.time()
    glob.run_cpp_pass("tensor2vector")
    dumper.record_timing("tensor2vector", time.time() - t0)
    dumper.dump_air(glob, "tensor2vector", verbose=verbose)
    
    # vector2sihe
    if verbose:
        print("\nRunning vector2sihe...")
    t0 = time.time()
    glob.run_cpp_pass("vector2sihe")
    dumper.record_timing("vector2sihe", time.time() - t0)
    dumper.dump_air(glob, "vector2sihe", verbose=verbose)
    
    # sihe2ckks (optionally with skip_ops)
    if verbose:
        print(f"\nRunning sihe2ckks{' with skip_ops=' + str(skip_ops) if skip_ops else ''}...")
    t0 = time.time()
    glob.run_cpp_pass("sihe2ckks", skip_ops or [])
    dumper.record_timing("sihe2ckks", time.time() - t0)
    dumper.dump_air(glob, "sihe2ckks", verbose=verbose)
    
    # CKKS driver
    if verbose:
        print("\nRunning CKKS driver...")
    t0 = time.time()
    ckks_result = air_builder.run_ckks_driver(glob)
    dumper.record_timing("ckks_driver", time.time() - t0)
    if not ckks_result["success"]:
        print(f"  ✗ CKKS driver failed: {ckks_result['message']}")
        return None
    dumper.dump_air(glob, "ckks_driver", verbose=verbose)
    
    # Poly driver
    if verbose:
        print("\nRunning Poly driver...")
    t0 = time.time()
    poly_result = air_builder.run_poly_driver(glob)
    dumper.record_timing("poly_driver", time.time() - t0)
    if not poly_result["success"]:
        print(f"  ✗ Poly driver failed: {poly_result['message']}")
        return None
    dumper.dump_air(glob, "poly_driver", verbose=verbose)
    
    # poly2c
    if verbose:
        print("\nRunning poly2c...")
    t0 = time.time()
    
    data_file = os.path.join(dumper.get_output_dir(), f"{dumper.prefix}_data.msg")
    
    if not glob.run_poly2c(output_file="", data_file=data_file, ct_encode=False, free_poly=True):
        print("  ✗ poly2c failed")
        return None
    
    c_code = glob.get_c_code()
    dumper.record_timing("poly2c", time.time() - t0)
    
    if c_code:
        dumper.dump_air(glob, "poly2c", verbose=verbose)
        dumper.dump_c_code(c_code, verbose=verbose)
        dumper.dump_data_file(data_file, verbose=verbose)
    
    return c_code


# =============================================================================
# Pipeline Class - High-level API for FHE compilation
# =============================================================================

class PipelineTarget(Enum):
    """Compilation target level."""
    AIR = "air"          # Stop at initial AIR
    VECTOR = "vector"    # Stop at nn::vector
    SIHE = "sihe"        # Stop at fhe::sihe
    CKKS = "ckks"        # Stop at fhe::ckks
    POLY = "poly"        # Stop at fhe::poly
    C = "c"              # Generate C code (full pipeline)


@dataclass
class FHEConfig:
    """FHE parameter configuration."""
    poly_degree: int = 0          # 0 = auto
    mul_level: int = 0            # 0 = auto
    security_level: int = 0       # 0 = auto
    scaling_factor_bits: int = 56
    first_prime_bits: int = 60
    hamming_weight: int = 192
    
    # Vector params
    conv_fast: bool = True
    gemm_fast: bool = False
    
    # SIHE params
    relu_vr_def: float = 3.0
    relu_vr: str = ""
    
    # Poly2C params
    data_file: str = ""           # Custom data file name (empty = auto-generate)
    free_poly: bool = True        # Insert Free_poly_data calls (-P2C:fp)
    ct_encode: bool = False       # Encode constants at compile time


@dataclass
class PipelineResult:
    """Result of pipeline execution."""
    success: bool
    c_code: Optional[str] = None
    data_file: Optional[str] = None
    error: Optional[str] = None
    phases_completed: List[str] = field(default_factory=list)
    timings: Dict[str, float] = field(default_factory=dict)
    total_time: float = 0.0
    
    def __bool__(self) -> bool:
        return self.success


class Pipeline:
    """
    High-level API for ACE FHE compilation pipeline.
    
    This class provides a clean, chainable interface for running the FHE
    compilation pipeline with automatic IR dumping at each phase.
    
    Example:
        pipeline = Pipeline("my_test")
        result = (pipeline
            .load_onnx("model.onnx")
            .configure_fhe(scaling_factor_bits=56)
            .run())
        
        if result.success:
            print(f"Generated {len(result.c_code)} bytes of C code")
    
    Example with callbacks:
        def on_phase(phase, ir):
            print(f"After {phase}: {len(ir)} chars")
        
        pipeline = Pipeline("my_test", on_phase_complete=on_phase)
        result = pipeline.load_onnx("model.onnx").run()
    """
    
    # Standard FHE pipeline phases
    PHASES = [
        "tensor2vector",
        "vector2sihe", 
        "sihe2ckks",
        "ckks_driver",
        "poly_driver",
        "poly2c",
    ]
    
    # Map target to phases to run
    TARGET_PHASES = {
        PipelineTarget.AIR: [],
        PipelineTarget.VECTOR: ["tensor2vector"],
        PipelineTarget.SIHE: ["tensor2vector", "vector2sihe"],
        PipelineTarget.CKKS: ["tensor2vector", "vector2sihe", "sihe2ckks"],
        PipelineTarget.POLY: ["tensor2vector", "vector2sihe", "sihe2ckks", "ckks_driver", "poly_driver"],
        PipelineTarget.C: PHASES,
    }
    
    def __init__(
        self, 
        name: str,
        output_dir: Optional[str] = None,
        verbose: bool = True,
        dump_ir: bool = True,
        on_phase_complete: Optional[Callable[[str, str], None]] = None,
    ):
        """
        Initialize the pipeline.
        
        Args:
            name: Name for output files (e.g., "resnet20_test")
            output_dir: Output directory (default: examples/output/{name}/)
            verbose: Print progress messages
            dump_ir: Dump IR to files after each phase
            on_phase_complete: Callback(phase_name, ir_dump) called after each phase
        """
        self.name = name
        self.verbose = verbose
        self.dump_ir = dump_ir
        self.on_phase_complete = on_phase_complete
        
        # State
        self.glob = None
        self.config = FHEConfig()
        self.skip_ops: List[str] = []
        self.python_lowering_func: Optional[Callable] = None
        
        # Dumper for IR output
        self.dumper = PipelineDumper(name, output_dir) if dump_ir else None
        
    def load_onnx(self, model_path: str) -> "Pipeline":
        """
        Load an ONNX model.
        
        Args:
            model_path: Path to .onnx file
            
        Returns:
            self for chaining
        """
        from ace_bindings import air_builder
        
        if not os.path.exists(model_path):
            raise FileNotFoundError(f"Model not found: {model_path}")
        
        if self.verbose:
            print(f"[Pipeline] Loading ONNX model: {model_path}")
        
        t0 = time.time()
        result = air_builder.load_onnx_model(model_path)
        load_time = time.time() - t0
        
        if not result["success"]:
            raise RuntimeError(f"Failed to load ONNX model: {result['message']}")
        
        self.glob = result["glob_scope"]
        
        if self.verbose:
            print(f"  ✓ Loaded ({load_time:.2f}s)")
        
        return self
    
    def set_glob(self, glob) -> "Pipeline":
        """
        Set the GlobScope directly (for programmatic IR).
        
        Args:
            glob: GlobScope object
            
        Returns:
            self for chaining
        """
        self.glob = glob
        return self
    
    def configure_fhe(self, **kwargs) -> "Pipeline":
        """
        Configure FHE parameters.
        
        Args:
            **kwargs: FHE config parameters (see FHEConfig)
            
        Returns:
            self for chaining
        """
        for key, value in kwargs.items():
            if hasattr(self.config, key):
                setattr(self.config, key, value)
            else:
                raise ValueError(f"Unknown FHE config parameter: {key}")
        return self
    
    def set_skip_ops(self, ops: List[str]) -> "Pipeline":
        """
        Set operations to skip during lowering.
        
        Args:
            ops: List of op names to skip (e.g., ["nn::core::conv"])
            
        Returns:
            self for chaining
        """
        self.skip_ops = ops
        return self
    
    def set_python_lowering(self, func: Callable) -> "Pipeline":
        """
        Set a Python lowering function to run after sihe2ckks.
        
        Args:
            func: Function that takes glob_scope as argument
            
        Returns:
            self for chaining
        """
        self.python_lowering_func = func
        return self
    
    def run(
        self, 
        target: PipelineTarget = PipelineTarget.C,
        phases: Optional[List[str]] = None,
    ) -> PipelineResult:
        """
        Run the compilation pipeline.
        
        Args:
            target: Target level to compile to (default: C)
            phases: Override automatic phase selection
            
        Returns:
            PipelineResult with success status, C code, etc.
        """
        from ace_bindings import air_builder
        
        if self.glob is None:
            return PipelineResult(
                success=False, 
                error="No model loaded. Call load_onnx() or set_glob() first."
            )
        
        result = PipelineResult(success=True)
        total_start = time.time()
        
        # Determine phases to run
        if phases is None:
            phases = self.TARGET_PHASES.get(target, self.PHASES)
        
        if self.verbose:
            print(f"[Pipeline] Running {len(phases)} phases: {phases}")
            print(f"[Pipeline] Skip ops: {self.skip_ops}")
        
        # Apply FHE configuration
        self._apply_fhe_config()
        
        # Dump initial state
        if self.dumper:
            self.dumper.dump_air(self.glob, "initial", verbose=self.verbose)
        if self.on_phase_complete:
            self.on_phase_complete("initial", self.glob.dump())
        
        # Run each phase
        try:
            for phase in phases:
                t0 = time.time()
                success = self._run_phase(phase)
                elapsed = time.time() - t0
                
                result.timings[phase] = elapsed
                
                if not success:
                    result.success = False
                    result.error = f"Phase '{phase}' failed"
                    break
                
                result.phases_completed.append(phase)
                
                # Dump IR after phase
                if self.dumper:
                    self.dumper.record_timing(phase, elapsed)
                    self.dumper.dump_air(self.glob, phase, verbose=self.verbose)
                if self.on_phase_complete:
                    self.on_phase_complete(phase, self.glob.dump())
                
                # Run Python lowering after sihe2ckks if configured
                if phase == "sihe2ckks" and self.python_lowering_func:
                    if self.verbose:
                        print("[Pipeline] Running Python lowering...")
                    t0 = time.time()
                    self.python_lowering_func(self.glob)
                    elapsed = time.time() - t0
                    result.timings["python_lower"] = elapsed
                    result.phases_completed.append("python_lower")
                    
                    if self.dumper:
                        self.dumper.record_timing("python_lower", elapsed)
                        self.dumper.dump_air(self.glob, "python_lower", verbose=self.verbose)
                    if self.on_phase_complete:
                        self.on_phase_complete("python_lower", self.glob.dump())
            
            # Get C code if we completed poly2c
            if result.success and "poly2c" in result.phases_completed:
                result.c_code = self.glob.get_c_code()
                
                if self.dumper and result.c_code:
                    self.dumper.dump_c_code(result.c_code, verbose=self.verbose)
                    # Use custom data_file if specified, otherwise auto-generate
                    result.data_file = self.config.data_file
                    if not result.data_file:
                        result.data_file = os.path.join(
                            self.dumper.get_output_dir(), 
                            f"{self.name}_data.msg"
                        )
                    if os.path.exists(result.data_file):
                        self.dumper.dump_data_file(result.data_file, verbose=self.verbose)
        
        except Exception as e:
            result.success = False
            result.error = str(e)
            if self.verbose:
                print(f"[Pipeline] ERROR: {e}")
        
        result.total_time = time.time() - total_start
        
        if self.verbose:
            if result.success:
                print(f"[Pipeline] ✓ Completed in {result.total_time:.2f}s")
                if result.c_code:
                    print(f"[Pipeline] Generated {len(result.c_code):,} bytes of C code")
            else:
                print(f"[Pipeline] ✗ Failed: {result.error}")
        
        return result
    
    def print_summary(self):
        """Print a summary of the pipeline run."""
        if self.dumper:
            self.dumper.print_summary()
    
    def get_ir(self) -> str:
        """Get current IR dump."""
        return self.glob.dump() if self.glob else ""
    
    def _apply_fhe_config(self):
        """Apply FHE configuration to glob scope."""
        if self.glob is None:
            return
        
        # FHE params
        self.glob.configure_fhe_params(
            poly_degree=self.config.poly_degree,
            mul_level=self.config.mul_level,
            scaling_factor_bits=self.config.scaling_factor_bits,
            first_prime_bits=self.config.first_prime_bits,
            hamming_weight=self.config.hamming_weight,
        )
        
        # Vector params
        self.glob.configure_vec_params(
            conv_fast=self.config.conv_fast, 
            gemm_fast=self.config.gemm_fast
        )
        
        # SIHE params
        if self.config.relu_vr:
            self.glob.configure_sihe_params(
                relu_vr_def=self.config.relu_vr_def,
                relu_vr=self.config.relu_vr
            )
        else:
            self.glob.configure_sihe_params(relu_vr_def=self.config.relu_vr_def)
    
    def _run_phase(self, phase: str) -> bool:
        """Run a single pipeline phase."""
        from ace_bindings import air_builder
        
        if self.verbose:
            print(f"[Pipeline] Running {phase}...")
        
        if phase == "tensor2vector":
            return self.glob.run_cpp_pass("tensor2vector", self.skip_ops)
        
        elif phase == "vector2sihe":
            return self.glob.run_cpp_pass("vector2sihe", self.skip_ops)
        
        elif phase == "sihe2ckks":
            return self.glob.run_cpp_pass("sihe2ckks", self.skip_ops)
        
        elif phase == "ckks_driver":
            result = air_builder.run_ckks_driver(self.glob)
            if not result["success"]:
                if self.verbose:
                    print(f"  ✗ CKKS driver failed: {result['message']}")
                return False
            return True
        
        elif phase == "poly_driver":
            result = air_builder.run_poly_driver(self.glob)
            if not result["success"]:
                if self.verbose:
                    print(f"  ✗ Poly driver failed: {result['message']}")
                return False
            return True
        
        elif phase == "poly2c":
            # Use custom data_file if specified, otherwise auto-generate
            data_file = self.config.data_file
            if not data_file and self.dumper:
                data_file = os.path.join(
                    self.dumper.get_output_dir(), 
                    f"{self.name}_data.msg"
                )
            return self.glob.run_poly2c(
                output_file="", 
                data_file=data_file, 
                ct_encode=self.config.ct_encode, 
                free_poly=self.config.free_poly
            )
        
        else:
            # Generic C++ pass
            return self.glob.run_cpp_pass(phase, self.skip_ops)


# Helper function for simple use cases
def compile_onnx_to_c(
    model_path: str,
    output_name: str,
    fhe_config: Optional[Dict[str, Any]] = None,
    verbose: bool = True,
) -> PipelineResult:
    """
    Compile an ONNX model to C code.
    
    Args:
        model_path: Path to .onnx file
        output_name: Name for output files
        fhe_config: FHE parameters (optional)
        verbose: Print progress
        
    Returns:
        PipelineResult with success status and C code
        
    Example:
        result = compile_onnx_to_c("model.onnx", "my_model")
        if result.success:
            with open("output.c", "w") as f:
                f.write(result.c_code)
    """
    pipeline = Pipeline(output_name, verbose=verbose)
    pipeline.load_onnx(model_path)
    
    if fhe_config:
        pipeline.configure_fhe(**fhe_config)
    
    return pipeline.run()

