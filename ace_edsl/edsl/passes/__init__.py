"""
Domain-specific pass configurations for ace_edsl
"""

from .ckks_extended_ops_rewrite import rewrite_extended_ckks_ops_to_primitives

__all__ = ["rewrite_extended_ckks_ops_to_primitives"]
