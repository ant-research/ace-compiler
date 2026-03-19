"""
Domain Kernel Decorators for ACE EDSL

Provides decorators for different domains.
Mimics acepy's domain kernel pattern but for AIR generation.

Domains (matching acepy):
- @kernel/@tensor_kernel: Tensor domain (air::core)
- @nn_kernel: Neural network domain (nn::core)
- @vector_kernel: Vector domain (nn::vector)
- @sihe_kernel: SIHE domain (fhe::sihe)
- @ckks_kernel: CKKS domain (fhe::ckks)
- @poly_kernel: Polynomial domain (fhe::poly)

Additional domains:
- @compute_kernel: Compute domain
- @memory_kernel: Memory domain

Important: These decorators look up the DSL singleton at CALL time (not decoration time).
This allows tests to clear the DSL cache and get fresh instances.
"""

import inspect
from functools import wraps


def _get_ace_edsl():
    """Get the DSL singleton. Called at runtime to ensure we get the current instance."""
    from .edsl import AceEDSL
    return AceEDSL._get_dsl()


def _make_kernel_decorator(domain: str):
    """
    Create a kernel decorator for a specific domain.
    
    The decorator looks up the DSL singleton at CALL time, enabling tests to
    clear the singleton cache and get fresh instances.
    """
    def kernel_decorator(func=None, **kwargs):
        def decorator(f):
            # Store domain on function
            f._py_domain = domain
            
            # Store the original function for later
            original_func = f
            
            # Preprocess once at decoration time (for AST transformation)
            # But use a wrapper that looks up DSL at call time
            preprocessed_func = None
            try:
                dsl = _get_ace_edsl()
                frame = inspect.currentframe().f_back.f_back  # Skip _make_kernel_decorator and decorator frames
                dsl.frame = frame
                if dsl.enable_preprocessor:
                    import os
                    if os.environ.get('ACE_DEBUG_PREPROCESS'):
                        print(f"[DEBUG] Running preprocessor for {f.__name__}...")
                    preprocessed_func = dsl.run_preprocessor(f)
                    if preprocessed_func is not None:
                        preprocessed_func._py_domain = domain
                        if os.environ.get('ACE_DEBUG_PREPROCESS'):
                            print(f"[DEBUG] Preprocessing succeeded for {f.__name__}")
                    else:
                        if os.environ.get('ACE_DEBUG_PREPROCESS'):
                            print(f"[DEBUG] Preprocessor returned None for {f.__name__}")
            except Exception as e:
                # Preprocessing failed, use original function
                import os
                if os.environ.get('ACE_DEBUG_PREPROCESS'):
                    print(f"[DEBUG] AST preprocessing EXCEPTION for {f.__name__}: {e}")
                    import traceback
                    traceback.print_exc()
                preprocessed_func = None
            
            func_to_use = preprocessed_func if preprocessed_func is not None else original_func
            
            @wraps(original_func)
            def kernel_wrapper(*args, **inner_kwargs):
                """
                Wrapper that looks up DSL at CALL time.
                This is the key difference from the standard jit_runner pattern.
                """
                # Get DSL at call time (not decoration time!)
                dsl = _get_ace_edsl()
                
                # Set domain on DSL
                dsl.current_domain = domain
                dsl._original_funcBody = original_func
                
                # Call the kernel helper
                return dsl._kernel_helper(func_to_use, *args, **inner_kwargs)
            
            return kernel_wrapper
        
        if func is None:
            return decorator
        return decorator(func)
    
    return kernel_decorator


# Create domain-specific decorators
tensor_kernel = _make_kernel_decorator("air::core")
tensor_kernel.__doc__ = """
Decorator for tensor-domain kernels (air::core).

Mimics acepy's @kernel decorator.
"""

vector_kernel = _make_kernel_decorator("nn::vector")
vector_kernel.__doc__ = """Decorator for vector-domain kernels (nn::vector)"""

compute_kernel = _make_kernel_decorator("compute")
compute_kernel.__doc__ = """Decorator for compute-domain kernels"""

memory_kernel = _make_kernel_decorator("memory")
memory_kernel.__doc__ = """Decorator for memory-domain kernels"""

nn_kernel = _make_kernel_decorator("nn::core")
nn_kernel.__doc__ = """
Decorator for neural network domain kernels (nn::core).

Similar to @tensor_kernel but uses nn::core operations.
"""

sihe_kernel = _make_kernel_decorator("fhe::sihe")
sihe_kernel.__doc__ = """
Decorator for SIHE (Scheme-Independent Homomorphic Encryption) domain kernels (fhe::sihe).

Works with any FHE scheme (CKKS, BFV, BGV).
"""

ckks_kernel = _make_kernel_decorator("fhe::ckks")
ckks_kernel.__doc__ = """
Decorator for CKKS domain kernels (fhe::ckks).

CKKS-specific operations with scale management.
"""

poly_kernel = _make_kernel_decorator("fhe::poly")
poly_kernel.__doc__ = """
Decorator for polynomial domain kernels (fhe::poly).

Low-level polynomial operations (NTT, etc.).
"""

# Alias for backward compatibility and acepy compatibility
kernel = tensor_kernel  # @kernel is alias for @tensor_kernel
