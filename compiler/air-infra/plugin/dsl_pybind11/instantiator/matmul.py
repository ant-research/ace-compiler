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
import numpy as np
from air_dsl import *  # noqa F403
from pydsl.compiler import py_compile, FunctionScopeManager
from pydsl.vector import ElemType, Zero, Roll, Vslice, Shape
sys.path.append(str(Path(__file__).parent / '..'))


def gen_diag(mat):
    '''
    generate diagonal matrix from a matrix
    '''
    shape = mat.shape
    k = shape[1]
    n_size = shape[0]
    shape = mat.shape
    diag = np.zeros(shape, dtype=float)

    for iter_r in range(0, n_size):
        i = 0
        j = iter_r
        for iter_c in range(0, k):
            diag[iter_r][iter_c] = mat[i][j]
            i = (i + 1) % n_size
            j = (j + 1) % k
    return diag


def kernel_impl():
    '''
    implementation of the kernel
    '''
    def __main__(input_tensor, kernel, bias):  # pylint:disable=W0612 # noqa N807
        '''
        implementation of the gemm
        '''
        ishape_0 = Shape(input_tensor, 0)
        kshape_0 = Shape(kernel, 0)
        kshape_1 = Shape(kernel, 1)
        ele_type = ElemType(input_tensor)
        shape_type = [1, ishape_0 * kshape_1]
        tmp_result = Zero(shape_type, ele_type)
        tmp_dup = vector.Add(input_tensor, Roll(input_tensor, -kshape_0))  # pylint:disable=E1130
        for iter_iv in arange(kshape_0):
            roll_res = Roll(tmp_dup, iter_iv)
            slice_res = Vslice(kernel, iter_iv, kshape_1)
            mul_res = vector.Mul(roll_res, slice_res)
            tmp_result = vector.Add(tmp_result, mul_res)

        tmp_result = vector.Add(tmp_result, bias)
        out = tmp_result
        return out


def gemm(func_scope, ctx, node, input_tensor, kernel, bias, out):
    '''
    compile the kernel_impl
    '''
    scope_manager = FunctionScopeManager()
    scope_manager.fs = func_scope
    scope_manager.node_operator = node
    scope_manager.add_exprs([input_tensor, kernel, bias])
    scope_manager.output_var = out
    scope_manager.ctx = ctx
    decorator_f = py_compile(locals(), globals(), scope_manager, dump_air=True)(kernel_impl)
    decorator_f()


def gemm_py_api(func_scope, ctx, node, input_tensor, kernel, bias, out):  # pylint:disable=R0914,R0915
    '''
    convert TENSOR::MATMUL to VECTOR OPs
    '''
    # the four lines of code should be hidden by the DSL compiler
    dsl = DSL(func_scope, ctx)
    vec = VectorAPI(dsl)
    pyctx = PyContext(node)

    spos = node.Spos()

    ishape = pyctx.rtype(input_tensor)
    kshape = pyctx.rtype(kernel)
    print(f'ishape: {ishape}')
    print(f'kshape: {kshape}')

    # input_vec = v.flatten(input)

    # get matrix from context
    kernel_array = np.reshape(np.array(pyctx.getFloatConstArray(kernel)),
                              kshape)
    kernel_array = np.pad(kernel_array, ((0, 0), (0, 6)), 'constant')
    print("kernel_array")
    print(kernel_array)
    np_diag = gen_diag(kernel_array)
    print("np_diag")
    print(np_diag)

    kernel_elem_ty = pyctx.getElemType(kernel)

    # create return variable and init it with 0
    res_ty = dsl.getArrayType("res_ty", kernel_elem_ty, [1, ishape[0] * kshape[1]], spos)
    res_var = dsl.newVar("tmp_result", res_ty, spos)
    vec_st = dsl.St(dsl.Zero(res_ty, spos), res_var, spos)
    dsl.append(vec_st)

    # create duplicate tmp variable
    dup_shape = [1, ishape[0] * ishape[1] * 2]
    dup_ty = dsl.getArrayType("dup_ty", kernel_elem_ty, dup_shape, spos)
    dup_var = dsl.newVar("tmp_dup", dup_ty, spos)
    # INFO: the const value here must be a uint64!!!!!
    offset = dsl.IntConst(PrimTypeEnum.INT_S32, np.uint64(-kshape[0]), spos)
    roll_input = vec.roll(input_tensor, offset, spos)
    vec_st = dsl.St(vec.add(dsl.clone_exp(input_tensor), roll_input, spos), dup_var, spos)
    dsl.append(vec_st)

    elem_ty = pyctx.getElemConstType(kernel)
    diag0 = dsl.newFloatArrayConst('diag0', len(np_diag[0]), elem_ty, np_diag[0].shape,
                                   np_diag[0], spos)
    ldc_diag0 = dsl.Ldc(diag0, spos)

    # create do loop
    int32_ty = dsl.getPrimType(PrimTypeEnum.INT_S32)
    iter_iv = dsl.newVar("iv", int32_ty, spos)
    init = dsl.IntConst(PrimTypeEnum.INT_S32, 0, spos)
    int_tmp = dsl.IntConst(PrimTypeEnum.INT_S32, kshape[0], spos)
    comp = dsl.Lt(dsl.Ld(iter_iv, spos), int_tmp, int32_ty, spos)
    incr = v.add(dsl.Ld(iter_iv, spos), dsl.IntConst(PrimTypeEnum.INT_S32, 1, spos), spos)
    block = dsl.Block(spos)
    roll = v.roll(dsl.Ld(dup_var, spos), dsl.Ld(iter_iv, spos), spos)
    slice_size = dsl.IntConst(PrimTypeEnum.INT_S32, kshape[0], spos)
    slice_ptr = vec.slice(ldc_diag0, dsl.Ld(iter_iv, spos), slice_size, spos)
    mul = v.mul(roll, slice_ptr, spos)
    add_tmp = v.add(dsl.Ld(res_var, spos), mul, spos)
    vec_st = dsl.St(add_tmp, res_var, spos)
    dsl.BlockAppend(block, vec_st)
    loop = dsl.DoLoop(iter_iv, init, comp, incr, block, spos)
    dsl.append(loop)

    # add bias
    vec_st = dsl.St(v.add(dsl.Ld(res_var, spos), bias, spos), res_var, spos)
    dsl.append(vec_st)

    ld_res = dsl.Ld(res_var, spos)
    vec_st = dsl.St(ld_res, out, spos)
    dsl.append(vec_st)
