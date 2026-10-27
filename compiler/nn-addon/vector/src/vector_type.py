#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

'''
Typing rules for the VECTOR domain
'''

import os
import sys
import argparse

OD_COMPONENT_PATH = os.getenv('IRGEN_OD_PATH')
if OD_COMPONENT_PATH:
    sys.path.append(OD_COMPONENT_PATH)
    from od.type import ATTRS, TYPE_TRAIT, ARRAY_TYPE_TRAIT, VERIFIER
else:
    print(f'The IRGEN_OD_PATH component directory cannot be found!!!')

def Conv2d_verifier(types: list, attrs: ATTRS) -> TYPE_TRAIT:
    '''
    Rules for verifying/infering the type of a conv2d operator
    '''
    # types of data/kernel/bias should be the same
    assert types[0].Elem_type() == types[1].Elem_type(
    ), "types of data/kernel are mismatched"
    assert types[0].Elem_type() == types[2].Elem_type(
    ), "types of data/bias are mismatched"

    # dimensions of data should be 4 (N/C/H/W)
    d_shape = types[0].Shape()
    assert len(d_shape) == 4, "dimensions of data should be 4"

    # dimiensions of kernel should be 4 (out channels, in channels, height, width)
    k_shape = types[1].Shape()
    assert len(k_shape) == 4, "dimensions of kernel should be 4"

    # C == in_channels
    assert d_shape[1] == k_shape[
        1], "in_channels of data/kernel are mismatched"

    assert len(types[2].Shape()) == k_shape[0], "shapes of bias are mismatched"

    # inference rules
    in_height = d_shape[2]
    in_width = d_shape[3]
    kernel_height = k_shape[2]
    kernel_width = k_shape[3]

    # retreive attributes
    # dilation is hard coded for now
    dilation = [1, 1]
    cnt = 0
    padding = attrs.Get('pads', cnt)
    assert (padding is not None) and (cnt == 4), "padding should be 4"

    stride = attrs.Get('strides', cnt)
    assert (stride is not None) and (cnt == 2), "stride should be 2"

    out_height = (in_height + padding[0] + padding[1] - dilation[0] *
                  (kernel_height - 1) - 1) // stride[0] + 1
    out_width = (in_width + padding[2] + padding[3] - dilation[1] *
                 (kernel_width - 1) - 1) // stride[1] + 1
    out_shape = [d_shape[0], k_shape[0], out_height, out_width]

    # what about the name and spos?
    return ARRAY_TYPE_TRAIT(types[0].Elem_type(), out_shape)


if __name__ == "__main__":
    PARSE = argparse.ArgumentParser()
    # add arguments -o
    PARSE.add_argument('-o', "--outdir", type=str, required=True)
    ARGS = PARSE.parse_args()
    VERIFIER(Conv2d_verifier).Set_namespaces(["nn", "vector"]).Set_outdir(
        ARGS.outdir).Gen_verifier("vector_type")
