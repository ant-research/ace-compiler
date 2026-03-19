"""
This module provides a NumPy related utility functions that are needed for
the DSL.
"""

from dataclasses import dataclass
import numpy as np

from .logger import log as logger

# =============================================================================
# NumPy extended data types
# =============================================================================


class f8E4M3FN(np.floating):
    pass


class f8E5M2(np.floating):
    pass


class f8E8M0FNU(np.floating):
    pass


class f6E3M2FN(np.floating):
    pass


class f6E2M3FN(np.floating):
    pass


class f4E2M1FN(np.floating):
    pass


# =============================================================================
# NumPy Utilities
# =============================================================================


@dataclass
class IncorrectResultError(RuntimeError):
    log: str
    diff: np.array
    verbose: bool = False

    def __str__(self):
        incorrect_prefix = (
            f"{self.log} : ❌ INCORRECT RESULT, max_diff = {self.diff.max()}"
        )

        if not self.verbose or len(self.diff.shape) != 2:
            return incorrect_prefix

        # Determine target size for downsampling
        down_shape = 8
        rows, cols = self.diff.shape
        new_rows = (
            rows // down_shape + (1 if rows % down_shape != 0 else 0)
        ) * down_shape
        new_cols = (
            cols // down_shape + (1 if cols % down_shape != 0 else 0)
        ) * down_shape
        diff_resized = np.zeros((new_rows, new_cols))
        diff_resized[:rows, :cols] = self.diff

        # Downscale the resized result array by averaging over target size blocks
        new_shape = (
            new_rows // down_shape,
            down_shape,
            new_cols // down_shape,
            down_shape,
        )
        downscale = diff_resized.reshape(new_shape).mean(axis=(1, 3))
        error_map = (downscale > 0) | np.isnan(downscale)

        return (
            incorrect_prefix
            + "\nError map:\n"
            + "\n".join(
                "".join("🟥" if error else "⬛" for error in row) for row in error_map
            )
        )


def compare_result(src, dst, log, rtol=1e-2, atol=1e-3, verbose=False):
    """Compare two numpy arrays."""
    # Deduce the element type from src/dst, assuming that at least one of them
    # is a NumPy array.
    dtype = None
    if isinstance(src, np.ndarray):
        dtype = src.dtype
    elif isinstance(dst, np.ndarray):
        dtype = dst.dtype
    else:
        assert False, "one of src and dst must be a NumPy array"

    # Ensure src and dst are numpy arrays and cast to high precision for comparison
    # The casting would also facilitate comparison across bool data types
    src = np.asarray(src, dtype=dtype).astype(np.float64)
    dst = np.asarray(dst, dtype=dtype).astype(np.float64)

    # Check for NaN consistency, meaning src values are NaN if and only if
    # dst values are NaN.
    nan_consistent = np.isnan(src) == np.isnan(dst)

    # Check for inf consistency, meaning src values are +/- infinity if and
    # only if dst values are +/- infinity.
    inf_consistent = (np.isposinf(src) == np.isposinf(dst)) & (
        np.isneginf(src) == np.isneginf(dst)
    )

    # Check if maximum difference is within tolerance
    diff = np.abs(dst - src)
    difference_tolerated = diff <= (rtol * np.maximum(np.abs(dst), np.abs(src)) + atol)

    if nan_consistent.all() and inf_consistent.all() and difference_tolerated.all():
        logger().info(f"{log} : 🚀🚀🚀 PASS 🚀🚀🚀")
        return

    raise IncorrectResultError(log=log, diff=diff, verbose=verbose)


def numpy_type_to_string(type):
    if type == np.float64:
        return "f64"
    if type == np.float32:
        return "f32"
    elif type == np.float16:
        return "f16"
    # todo NumPy doesn't have them
    elif type == f8E4M3FN:
        return "f8E4M3FN"
    elif type == f8E5M2:
        return "f8E5M2"
    elif type == f8E8M0FNU:
        return "f8E8M0FNU"
    elif type == f6E2M3FN:
        return "f6E2M3FN"
    elif type == f6E3M2FN:
        return "f6E3M2FN"
    elif type == f4E2M1FN:
        return "f4E2M1FN"
    elif type == np.int64:
        return "i64"
    elif type == np.int32:
        return "i32"
    elif type == np.int16:
        return "i16"
    elif type == np.int8:
        return "i8"
    elif type == np.uint64:
        return "ui64"
    elif type == np.uint32:
        return "ui32"
    elif type == np.uint16:
        return "ui16"
    elif type == np.uint8:
        return "ui8"
    assert False, f"Unknown type {type}"
