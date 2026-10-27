#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
IO Module

File-based IR formats and ONNX tools.
"""

from .file_ir import AIRFileIR, AIRModel, FileIR, FileModel, ONNXFileIR, ONNXModel
from .onnx_tools import (
    convert_onnx_to_air,
    export_function_to_onnx,
    export_model_to_onnx,
    inspect_onnx_model,
    validate_onnx_model,
)


__all__ = [
    "AIRFileIR",
    "AIRModel",
    # File formats
    "FileIR",
    "FileModel",
    "ONNXFileIR",
    "ONNXModel",
    "convert_onnx_to_air",
    "export_function_to_onnx",
    # ONNX tools
    "export_model_to_onnx",
    "inspect_onnx_model",
    "validate_onnx_model",
]
