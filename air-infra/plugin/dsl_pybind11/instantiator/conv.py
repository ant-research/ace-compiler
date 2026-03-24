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
from pydsl.vector import ElemType, Zero, Roll, Vslice
sys.path.append(str(Path(__file__).parent / '..'))


def kernel_impl():
    '''
    implementation of the kernel
    '''
    def __main__(input_tensor, weight, bias, ra, channel_in, channel_out,  # noqa N807
                 output_height, output_width, kernel_size):
        '''
        implementation of the conv
        '''
        ele_type = ElemType(input_tensor)
        shape_type = [1, channel_out, output_height, output_width]
        tmp_result_n1 = Zero(shape_type, ele_type)
        input_dup_n1 = vector.Add(input_tensor, Roll(input_tensor,
                                                     -channel_in * output_height * output_width))
        for index_cin_n1 in arange(channel_in):
            for index_khw_n1 in arrange(kernel_size):
                input_roll = Roll(input_dup_n1, ra[index_khw_n1])
                weight_slice = Vslice(weight, index_khw_n1 +
                                      index_cin_n1 * kernel_size, kernel_size)
                tmp_result_n1 = vector.Mul(input_roll, weight_slice)
            input_dup_n1 = Roll(input_dup_n1, output_height * output_width)
        tmp_result_n1 = vector.Add(tmp_result_n1, bias)
        out = tmp_result_n1
        return out


def conv(fs, ctx, node, input_tensor, weight_1, weight_2,
         out, ra, channel_in, channel_out, output_height,
         output_width, kernel_size):
    '''
    compile the kernel_impl
    '''
    scope_manager = FunctionScopeManager()
    scope_manager.fs = fs
    scope_manager.node_operator = node
    scope_manager.add_exprs([input_tensor, weight_1, weight_2, ra, channel_in,
                             channel_out, output_height, output_width, kernel_size])
    scope_manager.output_var = out
    scope_manager.ctx = ctx
    decorator_f = py_compile(locals(), globals(), scope_manager, dump_air=True)(kernel_impl)
    decorator_f()
