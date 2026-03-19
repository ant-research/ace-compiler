"""
Lowering Function Registry
===========================

Registries for lowering functions at each compilation phase:
- nn_to_vector: nn::core → nn::vector
- vector_to_sihe: nn::vector → fhe::sihe
- sihe_to_ckks: fhe::sihe → fhe::ckks
- ckks_to_poly: fhe::ckks → fhe::poly
"""

from typing import Dict, Callable, Any, Optional
from dataclasses import dataclass


@dataclass
class LoweringInfo:
    """Information about a registered lowering function."""
    name: str
    func: Callable
    source_domain: str
    target_domain: str
    description: str = ""


# Global registries for each domain transition
_nn_to_vector_registry: Dict[str, LoweringInfo] = {}
_vector_to_sihe_registry: Dict[str, LoweringInfo] = {}
_sihe_to_ckks_registry: Dict[str, LoweringInfo] = {}
_ckks_to_poly_registry: Dict[str, LoweringInfo] = {}


def nn_to_vector(op_name: str, description: str = ""):
    """
    Decorator to register a lowering function for nn::core → nn::vector.
    
    Usage:
        @nn_to_vector("add")
        def add_to_vector(a, b):
            return vec_add(a, b)
    
        @nn_to_vector("conv")
        def conv_to_vector(x, w, b, **attrs):
            # im2col lowering implementation
            ...
    """
    def decorator(func: Callable) -> Callable:
        info = LoweringInfo(
            name=op_name,
            func=func,
            source_domain="nn::core",
            target_domain="nn::vector",
            description=description or func.__doc__ or ""
        )
        _nn_to_vector_registry[op_name] = info
        return func
    return decorator


def vector_to_sihe(op_name: str, description: str = ""):
    """
    Decorator to register a lowering function for nn::vector → fhe::sihe.
    
    Usage:
        @vector_to_sihe("add")
        def add_to_sihe(a, b):
            return sihe_add(a, b)
    """
    def decorator(func: Callable) -> Callable:
        info = LoweringInfo(
            name=op_name,
            func=func,
            source_domain="nn::vector",
            target_domain="fhe::sihe",
            description=description or func.__doc__ or ""
        )
        _vector_to_sihe_registry[op_name] = info
        return func
    return decorator


def sihe_to_ckks(op_name: str, description: str = ""):
    """
    Decorator to register a lowering function for fhe::sihe → fhe::ckks.
    
    Usage:
        @sihe_to_ckks("sihe_mul")
        def sihe_mul_to_ckks(ct_a, ct_b):
            # CKKS multiplication with relinearization
            ...
    """
    def decorator(func: Callable) -> Callable:
        info = LoweringInfo(
            name=op_name,
            func=func,
            source_domain="fhe::sihe",
            target_domain="fhe::ckks",
            description=description or func.__doc__ or ""
        )
        _sihe_to_ckks_registry[op_name] = info
        return func
    return decorator


def ckks_to_poly(op_name: str, description: str = ""):
    """
    Decorator to register a lowering function for fhe::ckks → fhe::poly.
    
    Usage:
        @ckks_to_poly("ckks_add")
        def ckks_add_to_poly(p1, p2):
            return poly_add(p1, p2)
    """
    def decorator(func: Callable) -> Callable:
        info = LoweringInfo(
            name=op_name,
            func=func,
            source_domain="fhe::ckks",
            target_domain="fhe::poly",
            description=description or func.__doc__ or ""
        )
        _ckks_to_poly_registry[op_name] = info
        return func
    return decorator


def get_lowering_function(op_name: str, domain_transition: str) -> Optional[Callable]:
    """
    Get a registered lowering function.
    
    Args:
        op_name: Operation name (e.g., "add", "conv", "mul")
        domain_transition: One of "nn_to_vector", "vector_to_sihe", 
                          "sihe_to_ckks", "ckks_to_poly"
    
    Returns:
        The registered lowering function, or None if not found
    """
    registries = {
        "nn_to_vector": _nn_to_vector_registry,
        "vector_to_sihe": _vector_to_sihe_registry,
        "sihe_to_ckks": _sihe_to_ckks_registry,
        "ckks_to_poly": _ckks_to_poly_registry,
    }
    
    registry = registries.get(domain_transition)
    if registry is None:
        raise ValueError(f"Unknown domain transition: {domain_transition}")
    
    info = registry.get(op_name)
    return info.func if info else None


def list_lowering_functions(domain_transition: str) -> Dict[str, str]:
    """
    List all registered lowering functions for a domain transition.
    
    Args:
        domain_transition: One of "nn_to_vector", "vector_to_sihe", 
                          "sihe_to_ckks", "ckks_to_poly"
    
    Returns:
        Dict mapping operation names to descriptions
    """
    registries = {
        "nn_to_vector": _nn_to_vector_registry,
        "vector_to_sihe": _vector_to_sihe_registry,
        "sihe_to_ckks": _sihe_to_ckks_registry,
        "ckks_to_poly": _ckks_to_poly_registry,
    }
    
    registry = registries.get(domain_transition)
    if registry is None:
        raise ValueError(f"Unknown domain transition: {domain_transition}")
    
    return {name: info.description for name, info in registry.items()}


# Convenience functions to access registries directly
def get_nn_to_vector_registry() -> Dict[str, LoweringInfo]:
    return _nn_to_vector_registry.copy()


def get_vector_to_sihe_registry() -> Dict[str, LoweringInfo]:
    return _vector_to_sihe_registry.copy()


def get_sihe_to_ckks_registry() -> Dict[str, LoweringInfo]:
    return _sihe_to_ckks_registry.copy()


def get_ckks_to_poly_registry() -> Dict[str, LoweringInfo]:
    return _ckks_to_poly_registry.copy()

