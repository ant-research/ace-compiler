"""
Compiler Infrastructure
=======================

Base compiler class with PassManager integration for MLIR/AIR passes.
"""

from typing import Optional, Dict, Any, Sequence
import os


class CompilationError(RuntimeError):
    """Custom error class for compilation failures with formatted output."""
    
    def __init__(self, message: str, ir_context: Optional[str] = None):
        self.ir_context = ir_context
        super().__init__(message)
    
    def __str__(self) -> str:
        if self.ir_context:
            return f"{self.args[0]}\n\nIR Context:\n{self.ir_context}"
        return str(self.args[0])


class Compiler:
    """
    Compiler class for compiling and building MLIR/AIR modules.
    
    This class provides a uniform interface for:
    1. Running pass pipelines on IR modules
    2. JIT compilation to target backends
    3. Debug output and IR printing
    """
    
    def __init__(self, passmanager=None, execution_engine=None):
        """
        Initialize the compiler.
        
        Args:
            passmanager: PassManager module with parse() method
            execution_engine: Optional execution engine for JIT
        """
        self.passmanager = passmanager
        self.execution_engine = execution_engine
    
    def __call__(self, module):
        """Convenience method to compile with default pipeline."""
        return self.compile(module)
    
    def compile(self, module, pipeline: str = "", enable_ir_printing: bool = False):
        """
        Compile the module by invoking the pass pipeline.
        
        Args:
            module: The IR module to compile
            pipeline: Pass pipeline string (e.g., "vector-pass,sihe-pass,ckks-pass")
            enable_ir_printing: Print IR after each pass for debugging
        
        Raises:
            CompilationError: If compilation fails
        """
        if not pipeline:
            return module
        
        try:
            if self.passmanager is None:
                raise CompilationError("PassManager not configured")
            
            pm = self.passmanager.PassManager.parse(pipeline)
            
            if enable_ir_printing:
                # Disable multithreading for deterministic IR printing
                if hasattr(module, 'context'):
                    module.context.enable_multithreading(False)
                pm.enable_ir_printing()
            
            pm.run(module.operation)
            return module
            
        except Exception as e:
            error_msg = str(e)
            ir_context = self._extract_ir_context(error_msg)
            raise CompilationError(error_msg, ir_context=ir_context) from e
    
    def _extract_ir_context(self, error_msg: str) -> Optional[str]:
        """Extract IR context from error message for debugging."""
        if "see current operation:" in error_msg:
            ir_section = error_msg.split("see current operation:")[1].strip()
            ir_lines = ir_section.split("\n")
            if len(ir_lines) > 10:
                return "\n".join(ir_lines[:5] + ["  ..."] + ir_lines[-5:])
            return ir_section
        return None
    
    def jit(self, module, func_name: str = "kernel", 
            target_options: Dict[str, Any] = None):
        """
        JIT compile a function using the backend compilation pipeline.
        
        Args:
            module: The compiled IR module
            func_name: Name of the function to compile
            target_options: Backend-specific options
        
        Returns:
            ExecutionEngine or compiled artifact
        """
        if target_options is None:
            target_options = {}
        
        if self.execution_engine is None:
            raise CompilationError("Execution engine not configured")
        
        return self.execution_engine.create(module, func_name, target_options)
    
    def compile_and_jit(self, module, pipeline: str, func_name: str = "kernel",
                        target_options: Dict[str, Any] = None,
                        enable_ir_printing: bool = False):
        """
        Compile with passes and then JIT compile.
        
        Args:
            module: The IR module to compile
            pipeline: Pass pipeline string
            func_name: Name of the function to JIT compile
            target_options: Backend-specific options
            enable_ir_printing: Print IR after each pass
        
        Returns:
            JIT compiled artifact
        """
        self.compile(module, pipeline, enable_ir_printing)
        return self.jit(module, func_name, target_options)


def get_tmpdir() -> str:
    """Get temporary directory for compilation artifacts."""
    tmp_dir = os.environ.get('ACE_DSL_WORK_DIR')
    if tmp_dir is None:
        import tempfile
        tmp_dir = tempfile.mkdtemp(prefix="ace_dsl_")
    os.makedirs(tmp_dir, exist_ok=True)
    return tmp_dir


# Environment variable documentation
ENV_VARS = {
    'ACE_DSL_DRYRUN': 'Generate IR only, skip compilation',
    'ACE_DSL_WORK_DIR': 'Directory for compilation artifacts',
    'ACE_DSL_TRACE_IR': 'Print IR after each pass',
    'ACE_DSL_TRACE_STAT': 'Print pass statistics',
}


def is_dryrun() -> bool:
    """Check if running in dry-run mode."""
    return os.environ.get('ACE_DSL_DRYRUN', '').lower() in ('1', 'true', 'yes')


def is_trace_ir() -> bool:
    """Check if IR tracing is enabled."""
    return os.environ.get('ACE_DSL_TRACE_IR', '').lower() in ('1', 'true', 'yes')

