# _types.py
"""
Type aliases for C++ extension objects.
Only used during type checking (mypy, IDE).
"""

from typing import TYPE_CHECKING

if TYPE_CHECKING:
    # Type hints for C++ runtime extension
    from ace import runtime as _C
    DataScheme = _C.DataScheme
    Mapdesc = _C.Mapdesc
    Shape = _C.Shape
    GlobalConfig = _C.GlobalConfig
    RtDataInfo = _C.RtDataInfo
else:
    # Runtime: use Any to avoid ImportError
    from typing import Any
    DataScheme = Any
    Mapdesc = Any
    Shape = Any
    GlobalConfig = Any
    RtDataInfo = Any

