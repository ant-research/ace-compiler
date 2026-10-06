#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
Provides an example to lower tensor add op.
"""
import pydsl.vector as vector
import sys
from pathlib import Path
from air_dsl import *  # noqa F403
from pydsl.compiler import py_compile, FunctionScopeManager
sys.path.append(str(Path(__file__).parent / '..'))


def kernel_impl():
    '''
    implementation of the kernel
    '''
    def __main__(input_a, input_b):  # noqa N807
        '''
        implementation of the add
        '''
        out = vector.Add(input_a, input_b)
        return out


def add(func_scope, ctx, node, rhs_1, rhs_2, out):
    '''
    compile the Kernel_impl
    '''
    scope_manager = FunctionScopeManager()
    scope_manager.fs = func_scope
    scope_manager.node_operator = node
    scope_manager.add_exprs([rhs_1, rhs_2])
    scope_manager.output_var = out
    scope_manager.ctx = ctx
    decorator_f = py_compile(locals(), globals(), scope_manager, dump_air=True)(kernel_impl)
    decorator_f()


def add_py_api(f_s, ctx, node, lhs, rhs, out):
    '''
    create VECTOR::ADD
    '''
    dsl = DSL(f_s, ctx)
    vec = VectorAPI(dsl)
    spos = node.Spos()
    vec_st = dsl.St(vec.add(lhs, rhs, spos), out, spos)
    dsl.Prepend(vec_st)
