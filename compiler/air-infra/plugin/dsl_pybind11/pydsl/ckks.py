#!/usr/bin/env python3 # noqa T499
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
"""
Define the CKKS API in Python dsl.
"""

import ast
from abc import abstractmethod
from pydsl.macro import ArgRep, CallMacro  # noqa T400
from air_dsl import PrimTypeEnum, CKKSAPI  # noqa T484


class CKKSCallMacro(CallMacro):
    @classmethod
    def _on_Call(cls, visitor: ast.NodeVisitor, args):  # noqa: N802
        return cls._ckks_on_Call(visitor, args)  # noqa T484

    @abstractmethod
    def argreps(cls, args):  # noqa T484
        pass

    @abstractmethod
    def _ckks_on_Call(visitor, args):  # noqa N802
        pass


class Type(CKKSCallMacro):
    """
    Handle the function Rotate.
    """
    def argreps():  # noqa T484
        return [ArgRep.UNCOMPILED]

    def _ckks_on_Call(visitor, args):  # noqa N802
        dsl = visitor._air
        node = visitor.visit(args[0])
        return dsl.get_rtype(node)


class Zero(CKKSCallMacro):
    """
    Handle the function Rotate.
    """
    def argreps():  # noqa T484
        return [ArgRep.UNCOMPILED]

    def _ckks_on_Call(visitor, args):  # noqa N802
        spos = visitor._scope_manager.exprs[0].Spos()
        dsl = visitor._air
        api = CKKSAPI(dsl)
        ty = visitor.visit(args[0])
        return dsl.zero(ty, spos)


class Rotate(CKKSCallMacro):
    """
    Handle the function Rotate.
    """
    def argreps():  # noqa T484
        return [ArgRep.UNCOMPILED, ArgRep.UNCOMPILED]

    def _ckks_on_Call(visitor, args):  # noqa N802
        spos = visitor._scope_manager.exprs[0].Spos()
        dsl = visitor._air
        api = CKKSAPI(dsl)
        res_0 = visitor.visit(args[0])
        res_1 = visitor.visit(args[1])
        if isinstance(res_1, int):
            res_1 = dsl.intconst(PrimTypeEnum.INT_S32, res_1, spos)
        return api.rotate(res_0, res_1, spos)


class Add(CKKSCallMacro):
    """
    Handle the function Add.
    """
    def argreps():   # noqa T484
        return [ArgRep.UNCOMPILED, ArgRep.UNCOMPILED]

    def _ckks_on_Call(visitor, args):  # noqa N802
        spos = visitor._scope_manager.exprs[0].Spos()
        dsl = visitor._air
        api = CKKSAPI(dsl)
        res_0 = visitor.visit(args[0])
        res_1 = visitor.visit(args[1])
        return api.add(res_0, res_1, spos)


class Sub(CKKSCallMacro):
    """
    Handle the function Sub.
    """
    def argreps():   # noqa T484
        return [ArgRep.UNCOMPILED, ArgRep.UNCOMPILED]

    def _ckks_on_Call(visitor, args):  # noqa N802
        spos = visitor._scope_manager.exprs[0].Spos()
        dsl = visitor._air
        api = CKKSAPI(dsl)
        res_0 = visitor.visit(args[0])
        res_1 = visitor.visit(args[1])
        return api.sub(res_0, res_1, spos)


class Mul(CKKSCallMacro):
    """
    Handle the function Mul.
    """
    def argreps():   # noqa T484
        return [ArgRep.UNCOMPILED, ArgRep.UNCOMPILED]

    def _ckks_on_Call(visitor, args):  # noqa N802
        spos = visitor._scope_manager.exprs[0].Spos()
        dsl = visitor._air
        api = CKKSAPI(dsl)
        res_0 = visitor.visit(args[0])
        res_1 = visitor.visit(args[1])
        return api.mul(res_0, res_1, spos)


class Neg(CKKSCallMacro):
    """
    Handle the function Neg.
    """
    def argreps():   # noqa T484
        return [ArgRep.UNCOMPILED]

    def _ckks_on_Call(visitor, args):  # noqa N802
        spos = visitor._scope_manager.exprs[0].Spos()
        dsl = visitor._air
        api = CKKSAPI(dsl)
        res_0 = visitor.visit(args[0])
        return api.neg(res_0, spos)


class Encode(CKKSCallMacro):
    """
    Handle the function Encode.
    """
    def argreps():   # noqa T484
        return [ArgRep.UNCOMPILED, ArgRep.UNCOMPILED, ArgRep.UNCOMPILED, ArgRep.UNCOMPILED]

    def _ckks_on_Call(visitor, args):  # noqa N802
        spos = visitor._scope_manager.exprs[0].Spos()
        dsl = visitor._air
        api = CKKSAPI(dsl)
        res_0 = visitor.visit(args[0])
        res_1 = visitor.visit(args[1])
        res_1 = dsl.intconst(PrimTypeEnum.INT_S32, res_1, spos)
        res_2 = visitor.visit(args[2])
        res_2 = dsl.intconst(PrimTypeEnum.INT_S32, res_2, spos)
        res_3 = visitor.visit(args[3])
        res_3 = dsl.intconst(PrimTypeEnum.INT_S32, res_3, spos)
        return api.encode(res_0, res_1, res_2, res_3, spos)


class Rescale(CKKSCallMacro):
    """
    Handle the function Rescale.
    """
    def argreps():   # noqa T484
        return [ArgRep.UNCOMPILED]

    def _ckks_on_Call(visitor, args):  # noqa N802
        spos = visitor._scope_manager.exprs[0].Spos()
        dsl = visitor._air
        api = CKKSAPI(dsl)
        res_0 = visitor.visit(args[0])
        return api.rescale(res_0, spos)


class Upscale(CKKSCallMacro):
    """
    Handle the function Upscale.
    """
    def argreps():   # noqa T484
        return [ArgRep.UNCOMPILED, ArgRep.UNCOMPILED]

    def _ckks_on_Call(visitor, args):  # noqa N802
        spos = visitor._scope_manager.exprs[0].Spos()
        dsl = visitor._air
        api = CKKSAPI(dsl)
        res_0 = visitor.visit(args[0])
        res_1 = visitor.visit(args[1])
        res_1 = dsl.intconst(PrimTypeEnum.INT_S32, res_1, spos)
        return api.upscale(res_0, res_1, spos)


class Modswitch(CKKSCallMacro):
    """
    Handle the function Modswitch.
    """
    def argreps():   # noqa T484
        return [ArgRep.UNCOMPILED]

    def _ckks_on_Call(visitor, args):  # noqa N802
        spos = visitor._scope_manager.exprs[0].Spos()
        dsl = visitor._air
        api = CKKSAPI(dsl)
        res_0 = visitor.visit(args[0])
        return api.modswitch(res_0, spos)


class Relin(CKKSCallMacro):
    """
    Handle the function Relin.
    """
    def argreps():   # noqa T484
        return [ArgRep.UNCOMPILED]

    def _ckks_on_Call(visitor, args):  # noqa N802
        spos = visitor._scope_manager.exprs[0].Spos()
        dsl = visitor._air
        api = CKKSAPI(dsl)
        res_0 = visitor.visit(args[0])
        return api.relin(res_0, spos)


class Bootstrap(CKKSCallMacro):
    """
    Handle the function Bootstrap.
    """
    def argreps():   # noqa T484
        return [ArgRep.UNCOMPILED, ArgRep.UNCOMPILED]

    def _ckks_on_Call(visitor, args):  # noqa N802
        spos = visitor._scope_manager.exprs[0].Spos()
        dsl = visitor._air
        api = CKKSAPI(dsl)
        res_0 = visitor.visit(args[0])
        res_1 = visitor.visit(args[1])
        res_1 = dsl.intconst(PrimTypeEnum.INT_S32, res_1, spos)
        return api.bootstrap(res_0, res_1, spos)


class Scale(CKKSCallMacro):
    """
    Handle the function Scale.
    """
    def argreps():   # noqa T484
        return [ArgRep.UNCOMPILED]

    def _ckks_on_Call(visitor, args):  # noqa N802
        spos = visitor._scope_manager.exprs[0].Spos()
        dsl = visitor._air
        api = CKKSAPI(dsl)
        res_0 = visitor.visit(args[0])
        return api.scale(res_0, spos)


class Level(CKKSCallMacro):
    """
    Handle the function Level.
    """
    def argreps():   # noqa T484
        return [ArgRep.UNCOMPILED]

    def _ckks_on_Call(visitor, args):  # noqa N802
        spos = visitor._scope_manager.exprs[0].Spos()
        dsl = visitor._air
        api = CKKSAPI(dsl)
        res_0 = visitor.visit(args[0])
        return api.level(res_0, spos)


class BatchSize(CKKSCallMacro):
    """
    Handle the function Batch_size.
    """
    def argreps():   # noqa T484
        return [ArgRep.UNCOMPILED]

    def _ckks_on_Call(visitor, args):  # noqa N802
        spos = visitor._scope_manager.exprs[0].Spos()
        dsl = visitor._air
        api = CKKSAPI(dsl)
        res_0 = visitor.visit(args[0])
        return api.batch_size(res_0, spos)


class RaiseMod(CKKSCallMacro):
    """
    Handle the function Raise_mod.
    """
    def argreps():   # noqa T484
        return [ArgRep.UNCOMPILED, ArgRep.UNCOMPILED]

    def _ckks_on_Call(visitor, args):  # noqa N802
        spos = visitor._scope_manager.exprs[0].Spos()
        dsl = visitor._air
        api = CKKSAPI(dsl)
        res_0 = visitor.visit(args[0])
        res_1 = visitor.visit(args[1])
        res_1 = dsl.intconst(PrimTypeEnum.INT_S32, res_1, spos)
        return api.raise_mod(res_0, res_1, spos)


class MulMono(CKKSCallMacro):
    """
    Handle the function Mul_mono.
    """
    def argreps():   # noqa T484
        return [ArgRep.UNCOMPILED, ArgRep.UNCOMPILED]

    def _ckks_on_Call(visitor, args):  # noqa N802
        spos = visitor._scope_manager.exprs[0].Spos()
        dsl = visitor._air
        api = CKKSAPI(dsl)
        res_0 = visitor.visit(args[0])
        res_1 = visitor.visit(args[1])
        res_1 = dsl.intconst(PrimTypeEnum.INT_S32, res_1, spos)
        return api.mul_mono(res_0, res_1, spos)


class Conjugate(CKKSCallMacro):
    """
    Handle the function Conjugate.
    """
    def argreps():   # noqa T484
        return [ArgRep.UNCOMPILED]

    def _ckks_on_Call(visitor, args):  # noqa N802
        spos = visitor._scope_manager.exprs[0].Spos()
        dsl = visitor._air
        api = CKKSAPI(dsl)
        res_0 = visitor.visit(args[0])
        return api.conjugate(res_0, spos)
