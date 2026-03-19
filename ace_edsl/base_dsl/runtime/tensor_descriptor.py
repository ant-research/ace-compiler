import itertools, operator
import ctypes

# dlpack removed - ace_edsl uses AIR, not MLIR/GPU runtime
# TensorDescriptor is stubbed out for compatibility but not functional

from ..utils.logger import log
from ..common import *


class TensorDescriptor:
    """Stub class - dlpack removed. ace_edsl uses AIR, not MLIR/GPU runtime."""
    
    def __init__(self, tensor):
        """TensorDescriptor is not functional - dlpack removed."""
        raise NotImplementedError("TensorDescriptor requires dlpack - removed for ace_edsl (uses AIR, not MLIR/GPU runtime)")

    @staticmethod
    def can_transformed_to_dlpack(dl_tensor):
        """Always returns False - dlpack removed."""
        return False

    @property
    def is_in_device(self):
        """Stub - dlpack removed."""
        raise NotImplementedError("TensorDescriptor requires dlpack - removed for ace_edsl")

    @property
    def device_id(self):
        """Stub - dlpack removed."""
        raise NotImplementedError("TensorDescriptor requires dlpack - removed for ace_edsl")

    @property
    def element_type(self):
        """Stub - dlpack removed."""
        raise NotImplementedError("TensorDescriptor requires dlpack - removed for ace_edsl")

    @property
    def shape(self):
        """Stub - dlpack removed."""
        raise NotImplementedError("TensorDescriptor requires dlpack - removed for ace_edsl")

    @property
    def rank(self):
        """Stub - dlpack removed."""
        raise NotImplementedError("TensorDescriptor requires dlpack - removed for ace_edsl")

    @property
    def strides(self):
        """Stub - dlpack removed."""
        raise NotImplementedError("TensorDescriptor requires dlpack - removed for ace_edsl")

    @property
    def element_size_in_bytes(self):
        """Stub - dlpack removed."""
        raise NotImplementedError("TensorDescriptor requires dlpack - removed for ace_edsl")

    @property
    def size_in_bytes(self):
        """Stub - dlpack removed."""
        raise NotImplementedError("TensorDescriptor requires dlpack - removed for ace_edsl")

    def __str__(self):
        """Stub - dlpack removed."""
        return "TensorDescriptor<dlpack_removed>"

    def _check_is_managed_by_framework(self):
        """Stub - dlpack removed."""
        raise NotImplementedError("TensorDescriptor requires dlpack - removed for ace_edsl")


def from_tensor(tensor) -> TensorDescriptor:
    """Create a TensorDescriptor from a tensor object."""
    return TensorDescriptor(tensor)


def to_tensor(tensor_descriptor: TensorDescriptor):
    """Return tensor object from tensor descriptor."""
    raise NotImplementedError("to_tensor requires dlpack - removed for ace_edsl")
