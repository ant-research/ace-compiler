"""
This module provides a runtime utility functions that are needed for
the DSL.
"""

import ctypes
import abc


class Argument:
    """
    Abstract Runtime C Object of DSL dialect data types which interoperate with JIT functions.
    """

    def __init__(self):
        pass

    @abc.abstractmethod
    def c_pointer(self) -> ctypes.c_void_p:
        """
        Get raw pointer to underlying plain data

        Returns
        -------
        pointer
            Raw pointer to underlying plain data pass to JIT engine
        """
        raise NotImplementedError

    @abc.abstractmethod
    def size_in_bytes(self) -> int:
        """
        Size of underlying plain data structure pass to JIT engine

        Returns
        -------
        size in bytes
            number of bytes of raw data
        """
        raise NotImplementedError

    @property
    @abc.abstractmethod
    def mlir_type(self):  # todo(guray) how to return type? # -> ir.Type:
        """
        Corresponding MLIR Type of runtime argument

        Returns
        -------
        Type
            MLIR type of runtime argument
        """
        raise NotImplementedError

    @abc.abstractmethod
    def verify(self, expected_py_type) -> bool:
        """
        Verify if argument matches expected python type
        """
        return True


from . import device_tensor
# dlpack_types removed - ace_edsl uses AIR, not MLIR/GPU runtime
dlpack_types = None
#from . import cuda
from . import tensor_descriptor

__all__ = ["Argument", "device_tensor", "dlpack_types", "cuda", "tensor_descriptor"]
