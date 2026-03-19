"""
CKKS extended-op rewrite pass for ACE EDSL.

This pass rewrites CKKS extended ops in-place via bindings:
  - `fhe::ckks::raise_mod`
  - `fhe::ckks::conjugate`
  - `fhe::ckks::mul_mono`

The rewrite runs before CKKS driver and avoids cross-scope inlining.
"""

from typing import Any, Dict


def rewrite_extended_ckks_ops_to_primitives(
    glob_scope, verbose: bool = False
) -> Dict[str, Any]:
    """
    Rewrite extended CKKS ops in `glob_scope` to primitive CKKS ops.

    Returns:
        Dict with fields:
          - success: bool
          - changed: bool
          - replaced_count: int
          - errors: [str]
    """
    if glob_scope is None:
        return {
            "success": False,
            "changed": False,
            "replaced_count": 0,
            "errors": ["glob_scope is None"],
        }

    if not hasattr(glob_scope, "rewrite_ckks_extended_ops"):
        return {
            "success": False,
            "changed": False,
            "replaced_count": 0,
            "errors": ["glob_scope does not support rewrite_ckks_extended_ops"],
        }

    try:
        replaced_count = int(glob_scope.rewrite_ckks_extended_ops(verbose))
        return {
            "success": True,
            "changed": replaced_count > 0,
            "replaced_count": replaced_count,
            "errors": [],
        }
    except Exception as exc:
        return {
            "success": False,
            "changed": False,
            "replaced_count": 0,
            "errors": [str(exc)],
        }

