"""
Domain Registry for ace_edsl

Maps domain names to AIR pass pipelines.
AIR passes are strings that correspond to C++ pass names.

Matches acepy's domain structure:
- air::core / tensor: High-level tensor operations
- nn::core: Neural network operations
- nn::vector: Vectorized operations
- fhe::sihe: Scheme-independent FHE
- fhe::ckks: CKKS-specific FHE
- fhe::poly: Low-level polynomial operations

Additional domains:
- compute: Compute operations
- memory: Memory operations
"""

DOMAIN_PIPELINES = {
    # acepy-compatible domains
    "air::core": [],  # Just AIR, no lowering
    "tensor": [],  # Alias for air::core
    "nn::core": [
        "tensor2vector",      # Lower nn::core ops to vector ops
        "vector2sihe",        # Lower vector ops to SIHE
        "sihe2ckks",          # Lower SIHE to CKKS
        "ckks2poly",          # Lower CKKS to polynomial
        "poly2c",             # Generate C code
    ],
    "nn::vector": [
        "vector2sihe",        # Lower vector ops to SIHE
        "sihe2ckks",          # Lower SIHE to CKKS
        "ckks2poly",          # Lower CKKS to polynomial
        "poly2c",             # Generate C code
    ],
    "fhe::sihe": [
        "sihe2ckks",          # Lower SIHE to CKKS
        "ckks2poly",          # Lower CKKS to polynomial
        "poly2c",             # Generate C code
    ],
    "fhe::ckks": [
        "ckks2poly",          # Lower CKKS to polynomial
        "poly2c",             # Generate C code
    ],
    "fhe::poly": [
        "poly2c",             # Generate C code
    ],
    
    # Additional domains (not in acepy)
    "compute": [
        "compute2cuda",       # Lower compute ops to CUDA
        "cuda2c",             # Generate C code
    ],
    "memory": [
        "memory2cuda",        # Lower memory ops to CUDA
        "cuda2c",             # Generate C code
    ],
}

