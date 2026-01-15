#!/usr/bin/env python3 # noqa T499
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
"""
Defines the interface of compile time function call.
"""
import ast
from ast import NodeVisitor
from typing import (
    Optional,
    List,
)
from pydsl.scope import ScopeStack  # noqa T484


def handle_compile_time_callable(visitor, node: ast.Call, prefix_args: List[ast.AST] = []):  # noqa B006
    """
    Handle the call node
    """
    attr_chain = visitor._scope_stack.resolve_attr_chain(node.func)
    while True:
        x = attr_chain[-1]
        # Determine on_Call
        if issubclass(type(x), type):
            on_Call = x.on_Call  # noqa N806
        else:
            on_Call = type(x).on_Call  # noqa N806

        # Check if on_Call matches CompileTimeCallable or Callable
        # if isinstance(on_Call, CompileTimeCallable):
        #    attr_chain.append(on_Call)
        #    continue
        if callable(on_Call):
            return on_Call(attr_chain, visitor, node, prefix_args=prefix_args)


class ToAIRBase(NodeVisitor):
    """
    This is an empty class which ToAIR inherits. It serves no
    purpose other than to enable type-hinting ToAIR without risking
    cyclic import.
    """

    _scope_stack: Optional[ScopeStack] = None
