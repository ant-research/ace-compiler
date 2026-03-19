"""
ACE DSL Bindings - Backward compatibility wrapper

This module re-exports from ace_bindings for backward compatibility.
New code should import directly from ace_bindings:

    from ace_bindings import air_builder, nn_addon, fhe_cmplr, passmanager
"""

# Re-export everything from the shared ace_bindings package
from ace_bindings import air_builder, nn_addon, fhe_cmplr, passmanager

__all__ = [
    'air_builder',
    'nn_addon', 
    'fhe_cmplr',
    'passmanager',
]
