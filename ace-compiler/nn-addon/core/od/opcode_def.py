#!/usr/bin/env python3 # noqa T499
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

'''
Operator description for neural network domain.
'''
import sys
import os
import argparse

OD_COMPONENT_PATH = os.getenv('IRGEN_OD_PATH')
if OD_COMPONENT_PATH:
    sys.path.append(OD_COMPONENT_PATH)
    from od import CREATE_DOMAIN, REGISTER_OP  # noqa T484
    from od.property import OprProp  # noqa T484
    from od.cg.opcode import (OpcodeHeaderConfig, OpcodeHeaderFileCG,  # noqa T484
                              OpcodeCXXConfig, OpcodeCXXFileCG, OpcodeWrite)
else:
    print(f'The IRGEN_OD_PATH component directory cannot be found!!!')  # noqa F541

DOMAIN = CREATE_DOMAIN('NN', 1)


def register_opcodes(output_dir: str):
    ''' Register all operators for neural network domain. '''
    (REGISTER_OP('invalid')
     .description('Invalid operator')
     .set_arg_num(0)
     .add_prop(OprProp.NONE))

    (REGISTER_OP('add')
     .description('Add two tensors element-wise')
     .set_arg_num(2)
     .add_arg('a', 'Tensor', 'Left-hand side tensor')
     .add_arg('b', 'Tensor', 'Right-hand side tensor')
     .add_prop(OprProp.EXPR)
     .add_prop(OprProp.ATTR))

    (REGISTER_OP('average_pool')
     .description('Average pooling operation')
     .set_arg_num(1)
     .add_arg('t', 'Tensor', 'Input tensor')
     .add_prop(OprProp.EXPR)
     .add_prop(OprProp.ATTR))

    (REGISTER_OP('conv')
     .description('2D convolution operation')
     .set_arg_num(3)
     .add_arg('x', 'Tensor', 'Input tensor')
     .add_arg('w', 'Tensor', 'Filter tensor')
     .add_arg('b', 'Tensor', 'Bias tensor')
     .add_prop(OprProp.EXPR)
     .add_prop(OprProp.ATTR))

    (REGISTER_OP('flatten')
     .description('Flatten the input tensor')
     .set_arg_num(1)
     .add_arg('t', 'Tensor', 'Input tensor')
     .add_prop(OprProp.EXPR)
     .add_prop(OprProp.ATTR))

    (REGISTER_OP('gemm')
     .description('General matrix multiplication')
     .set_arg_num(3)
     .add_arg('a', 'Tensor', 'Left-hand side tensor')
     .add_arg('b', 'Tensor', 'Right-hand side tensor')
     .add_arg('c', 'Tensor', 'Bias tensor')
     .add_prop(OprProp.EXPR)
     .add_prop(OprProp.ATTR))

    (REGISTER_OP('global_average_pool')
     .description('Global average pooling operation')
     .set_arg_num(1)
     .add_arg('t', 'Tensor', 'Input tensor')
     .add_prop(OprProp.EXPR)
     .add_prop(OprProp.ATTR))

    (REGISTER_OP('max_pool')
     .description('Max pooling operation')
     .set_arg_num(1)
     .add_arg('t', 'Tensor', 'Input tensor')
     .add_prop(OprProp.EXPR)
     .add_prop(OprProp.ATTR))

    (REGISTER_OP('mul')
     .description('Multiply two tensors element-wise')
     .set_arg_num(2)
     .add_arg('a', 'Tensor', 'Left-hand side tensor')
     .add_arg('b', 'Tensor', 'Right-hand side tensor')
     .add_prop(OprProp.EXPR)
     .add_prop(OprProp.ATTR))

    (REGISTER_OP('relu')
     .description('Rectified linear unit activation function')
     .set_arg_num(1)
     .add_arg('t', 'Tensor', 'Input tensor')
     .add_prop(OprProp.EXPR)
     .add_prop(OprProp.ATTR))

    (REGISTER_OP('reshape')
     .description('Reshape the input tensor')
     .set_arg_num(2)
     .add_arg('t', 'Tensor', 'Input tensor')
     .add_arg('shape', 'Shape', 'Target shape')
     .add_prop(OprProp.EXPR)
     .add_prop(OprProp.ATTR))

    (REGISTER_OP('strided_slice')
     .description('Extract a strided slice of a tensor')
     .set_arg_num(4)
     .add_arg('t', 'Tensor', 'Input tensor')
     .add_arg('begin', 'Shape', 'Begin index')
     .add_arg('end', 'Shape', 'End index')
     .add_arg('stride', 'Shape', 'Stride')
     .add_prop(OprProp.EXPR)
     .add_prop(OprProp.ATTR))

    (REGISTER_OP('sub')
     .description('Subtract two tensors element-wise')
     .set_arg_num(2)
     .add_arg('a', 'Tensor', 'Left-hand side tensor')
     .add_arg('b', 'Tensor', 'Right-hand side tensor')
     .add_prop(OprProp.EXPR))

    (REGISTER_OP('rmsnorm')
     .description('Root mean square normalization')
     .set_arg_num(3)
     .add_arg('t', 'Tensor', 'Input tensor')
     .add_arg('dim', int, 'Dimension')
     .add_prop(OprProp.EXPR))

    (REGISTER_OP('matmul')
     .description('Matrix multiplication')
     .set_arg_num(2)
     .add_arg('a', 'Tensor', 'Left-hand side tensor')
     .add_arg('b', 'Tensor', 'Right-hand side tensor')
     .add_prop(OprProp.EXPR))

    (REGISTER_OP('rope_rotary')
     .description('Rope rotary operation')
     .set_arg_num(5)
     .add_arg('t', 'Tensor', 'Input tensor')
     .add_arg('real_vec0', 'Tensor', 'real_vec0 tensor')
     .add_arg('real_vec1', 'Tensor', 'real_vec1 tensor')
     .add_arg('imag_vec0', 'Tensor', 'imag_vec0 tensor')
     .add_arg('imag_vec1', 'Tensor', 'imag_vec1 tensor')
     .add_prop(OprProp.EXPR))

    (REGISTER_OP('reshape_kv')
     .set_arg_num(3)
     .add_prop(OprProp.EXPR))

    (REGISTER_OP('repeat_kv')
     .set_arg_num(3)
     .add_arg('t', 'Tensor', 'Input tensor')
     .add_arg('pos', int, 'Pos')
     .add_arg('dim', int, 'Dim')
     .add_prop(OprProp.EXPR))

    (REGISTER_OP('transpose')
     .description('Transpose the input tensor')
     .set_arg_num(3)
     .add_arg('t', 'Tensor', 'Input tensor')
     .add_arg('a', int, 'Axis A')
     .add_arg('b', int, 'Axis B')
     .add_prop(OprProp.EXPR))

    (REGISTER_OP('sqrt')
     .description('Square root operation')
     .set_arg_num(1)
     .add_arg('t', 'Tensor', 'Input tensor')
     .add_prop(OprProp.EXPR))

    (REGISTER_OP('softmax')
     .description('Softmax operation')
     .set_arg_num(4)
     .add_arg('t', 'Tensor', 'Input tensor')
     .add_arg('pos', int, 'Pos')
     .add_arg('n_heads', int, 'N_heads')
     .add_arg('len_pad', int, 'Len_pad')
     .add_prop(OprProp.EXPR))

    (REGISTER_OP('divide')
     .description('Divide two tensors element-wise')
     .set_arg_num(2)
     .add_arg('a', 'Tensor', 'Left-hand side tensor')
     .add_arg('b', 'Tensor', 'Right-hand side tensor')
     .add_prop(OprProp.EXPR))

    (REGISTER_OP('concat')
     .description('Concatenate tensors along an axis')
     .set_arg_num(2)
     .add_arg('a', 'Tensor', 'Left-hand side tensor')
     .add_arg('b', 'Tensor', 'Right-hand side tensor')
     .add_prop(OprProp.EXPR)
     .add_prop(OprProp.ATTR))

    (REGISTER_OP('update_kcache')
     .description('Update_kcache operation')
     .set_arg_num(3)
     .add_arg('t', 'Tensor', 'Input tensor')
     .add_arg('pos', int, 'Pos')
     .add_arg('dim', int, 'Dim')
     .add_prop(OprProp.EXPR))

    (REGISTER_OP('update_vcache')
     .description('Update_vcache operation')
     .set_arg_num(5)
     .add_arg('t', 'Tensor', 'Input tensor')
     .add_arg('pos', int, 'Pos')
     .add_arg('dim', int, 'Dim')
     .add_arg('n_heads', int, 'N_heads')
     .add_arg('len_pad', int, 'Len_pad')
     .add_prop(OprProp.EXPR))

    (REGISTER_OP('bmm')
     .description('Bmm operation')
     .set_arg_num(5)
     .add_arg('inp_tensor', 'Tensor', 'Input tensor')
     .add_arg('cache_tensor', 'Tensor', 'Cache tensor')
     .add_arg('pos', int, 'Pos')
     .add_arg('n_heads', int, 'N_heads')
     .add_arg('len_pad', int, 'Len_pad')
     .add_prop(OprProp.EXPR))

    (REGISTER_OP('swiglu')
     .description('Swiglu operation')
     .set_arg_num(2)
     .add_arg('inp_tensor_1', 'Tensor', 'Input tensor 1')
     .add_arg('inp_tensor_2', 'Tensor', 'Input tensor 2')
     .add_prop(OprProp.EXPR))

    header_config = (OpcodeHeaderConfig()
                     .add_namespace('nn')
                     .add_namespace('core')
                     .set_macro_guard('NN_CORE_OPCODE_H'))
    header_cg = OpcodeHeaderFileCG(header_config, DOMAIN)

    cxx_config = (OpcodeCXXConfig()
                  .add_namespace('nn')
                  .add_namespace('core'))
    cxx_cg = OpcodeCXXFileCG(cxx_config, DOMAIN)

    (OpcodeWrite(output_dir)
     .set_header_cg(header_cg)
     .set_cxx_cg(cxx_cg)
     .write_file())


if __name__ == '__main__':
    PARSER = argparse.ArgumentParser(description='Neural network domain')
    PARSER.add_argument('-o', '--output-dir',
                        help='Output directory', required=True)
    register_opcodes(PARSER.parse_args().output_dir)
