#!/usr/bin/env python3 # noqa T499
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
"""
Define the vector API in Python dsl.
"""
import ast
from abc import abstractmethod
from pydsl.macro import ArgRep, CallMacro  # noqa T484
import numpy as np  # noqa T484
from air_dsl import *  # noqa T484


class VectorCallMacro(CallMacro):
    """
    A CallMacro that can only be called in an vector context
    """

    @classmethod
    def _on_Call(cls, visitor: ast.NodeVisitor, args):  # noqa N802
        return cls._vector_on_Call(visitor, args)  # noqa T484

    @abstractmethod
    def argreps():  # noqa T484
        pass

    @abstractmethod
    def _vector_on_Call(visitor, args):  # noqa N802
        pass


class Add(VectorCallMacro):
    """
    Handle the function Add.
    """
    def argreps():  # noqa T484
        return [ArgRep.UNCOMPILED, ArgRep.UNCOMPILED]

    def _vector_on_Call(visitor, args):  # noqa N802
        spos = visitor._scope_manager.exprs[0].Spos()
        dsl = visitor._air
        vec = VectorAPI(dsl)
        res_0 = visitor.visit(args[0])
        res_1 = visitor.visit(args[1])
        return vec.add(res_0, res_1, spos)


class Mul(VectorCallMacro):
    """
    Handle the function Mul.
    """
    def argreps():  # noqa T484
        return [ArgRep.UNCOMPILED, ArgRep.UNCOMPILED]

    def _vector_on_Call(visitor, args):  # noqa N802
        spos = visitor._scope_manager.exprs[0].Spos()
        dsl = visitor._air
        vec = VectorAPI(dsl)
        res_0 = visitor.visit(args[0])
        res_1 = visitor.visit(args[1])
        return vec.mul(res_0, res_1, spos)


class Zero(VectorCallMacro):
    """
    Handles the zero function.
    """
    def argreps():  # noqa T484
        return [ArgRep.UNCOMPILED, ArgRep.UNCOMPILED]

    def _vector_on_Call(visitor, args):  # noqa N802
        tensor_size = visitor.visit(args[0])
        elem_ty = visitor.visit(args[1])
        dsl = visitor._air
        spos = visitor._scope_manager.exprs[0].Spos()
        res_ty = dsl.get_array_type("res_ty", elem_ty, tensor_size, spos)
        return dsl.zero(res_ty, spos)


class Shape(VectorCallMacro):
    """
    Handles the shape size
    """
    def argreps():  # noqa T484
        return [ArgRep.UNCOMPILED, ArgRep.UNCOMPILED]

    def _vector_on_Call(visitor, args):  # noqa N802
        shape_array = visitor.visit(args[0])
        shape_dim = visitor.visit(args[1])
        pyctx = PyContext(visitor._scope_manager.exprs[0])
        ishape_size = pyctx.rtype(shape_array)[shape_dim]
        return ishape_size


class ElemType(VectorCallMacro):
    """
    Handles the element type function.
    """
    def argreps():  # noqa T484
        return [ArgRep.UNCOMPILED]

    def _vector_on_Call(visitor, args):  # noqa N802
        shape_array = visitor.visit(args[0])
        pyctx = PyContext(visitor._scope_manager.exprs[0])
        return pyctx.get_elem_type(shape_array)


class Reshape(VectorCallMacro):
    """
    Handles the reshape function.
    """
    def argreps():  # noqa T484
        return [ArgRep.UNCOMPILED, ArgRep.UNCOMPILED]

    def _vector_on_Call(visitor, args):  # noqa N802
        tensor_var = visitor.visit(args[0])
        reshape_size = visitor.visit(args[1])
        dsl = visitor._air
        spos = visitor._scope_manager.exprs[0].Spos()
        vec = VectorAPI(dsl)
        return vec.Reshape(tensor_var, [reshape_size], spos)


class Nchw(VectorCallMacro):
    """
    Handles the nchw function.
    """
    def argreps():  # noqa T484
        return [ArgRep.UNCOMPILED]

    def _vector_on_Call(visitor, args):  # noqa N802
        input_tensor = visitor.visit(args[0])
        dsl = visitor._air
        pyctx = PyContext(visitor._scope_manager.exprs[0])
        input_ty = dsl.get_rtype(input_tensor)
        return pyctx.Get_array_nchw(input_ty)


class Roll(VectorCallMacro):
    """
    Handles the roll function.
    """
    def argreps():  # noqa T484
        return [ArgRep.UNCOMPILED, ArgRep.UNCOMPILED]

    def _vector_on_Call(visitor, args):  # noqa N802
        input_tensor = visitor.visit(args[0])
        arg = visitor.visit(args[1])
        dsl = visitor._air
        spos = visitor._scope_manager.exprs[0].Spos()
        vec = VectorAPI(dsl)
        if isinstance(arg, int):
            offset = dsl.intconst(PrimTypeEnum.INT_S32, np.uint64(arg), spos)
        else:
            offset = arg
        roll_input = vec.roll(input_tensor, offset, spos)
        return roll_input


class Vslice(VectorCallMacro):
    """
    Handles the slice function.
    """
    def argreps():  # noqa T484
        return [ArgRep.UNCOMPILED, ArgRep.UNCOMPILED, ArgRep.UNCOMPILED]

    def _vector_on_Call(visitor, args):  # noqa N802
        input_tensor = visitor.visit(args[0])
        offset = visitor.visit(args[1])
        slice_sz = visitor.visit(args[2])
        dsl = visitor._air
        spos = visitor._scope_manager.exprs[0].Spos()
        vec = VectorAPI(dsl)
        slice_size = dsl.intconst(PrimTypeEnum.INT_S32, slice_sz, spos)
        roll_input = vec.slice(input_tensor, offset, slice_size, spos)
        return roll_input
