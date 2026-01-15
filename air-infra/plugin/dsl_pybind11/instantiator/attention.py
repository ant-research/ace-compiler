#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
Provides an example to lower tensor add op.
"""
import sys
from pathlib import Path
from air_dsl import *  # noqa F403
from pydsl.compiler import py_compile, FunctionScopeManager
import pydsl.tensor as tensor
sys.path.append(str(Path(__file__).parent / '..'))


def kernel_impl():
    '''
    implementation of the kernel
    '''
    def attention_impl(
        input_x, wq, wk, wv, wo, w1, w2, w3,
        rms_att_weight, rms_ffn_weight,
        real_vec0, real_vec1, image_vec0, image_vec1,
        dim, pos, len_pad, n_heads
    ):
        """
        Implements the forward pass for a transformer block with rotary position embeddings.

        Parameters:
            input_x: Input tensor.
            wq, wk, wv, wo: Weight matrices for query, key, value, and output projections.
            w1, w2, w3: Weight matrices for the feedforward network.
            rms_att_weight, rms_ffn_weight: Weights for RMS normalization layers.
            real_vec0, real_vec1, image_vec0, image_vec1: Rotary embedding vectors.
            dim: Dimensionality of the model.
            pos: Current position in the sequence.
            len_pad: Length of padding in the sequence.
            n_heads: Number of attention heads.

        Returns:
            Final output tensor after attention and feedforward layers.
        """
        # Apply RMS normalization to the input
        rms_out_1 = tensor.Rmsnorm(input_x, rms_att_weight)

        # Compute query, key, and value matrices
        # Computes the query matrix by multiplying the input with the query weight matrix.
        q = tensor.Matmul(input_x, wq)
        k = tensor.Matmul(input_x, wk)
        v = tensor.Matmul(input_x, wv)

        # Apply rotary position embeddings to query and key
        q_rotary = tensor.RopeRotary(q, real_vec0, real_vec1, image_vec0, image_vec1)
        k_rotary = tensor.RopeRotary(k, real_vec0, real_vec1, image_vec0, image_vec1)

        # Updates the key cache with the current key values.
        k_cache = tensor.UpdateKcache(k_rotary, pos, dim)
        # Updates the value cache with the current value vectors.
        v_cache = tensor.UpdateVcache(v, pos, dim, n_heads, len_pad)

        # Calculates the attention scores by multiplying the query with the cached keys.
        attention_scores = tensor.Bmm(q_rotary, k_cache, n_heads, dim, len_pad)
        # Applies softmax to the scores to obtain attention probabilities.
        attention_probs = tensor.Softmax(attention_scores, pos, n_heads, len_pad)
        # Calculates the weighted sum of values based on the attention probabilities.
        attention_output = tensor.Bmm(attention_probs, v_cache, n_heads, dim, len_pad)

        # Projects the attention output to the model's dimension.
        projected_output = tensor.Matmul(attention_output, wo)
        # Adds the projected output to the original input (residual connection).
        residual_output_1 = tensor.Add(projected_output, input_x)

        # Apply RMS normalization to the output of the attention layer.
        rms_out_2 = tensor.Rmsnorm(residual_output_1, rms_ffn_weight)

        # Feedforward network with Swiglu activation
        # First linear transformation in the feedforward network.
        ff_intermediate_1 = tensor.Matmul(rms_out_2, w1)
        # Second linear transformation used in the Swiglu activation function.
        ff_intermediate_2 = tensor.Matmul(rms_out_2, w3)
        # Applies the Swiglu activation function.
        ff_activated = tensor.Swiglu(ff_intermediate_1, ff_intermediate_2)
        # Final linear transformation in the feedforward network.
        ff_output = tensor.Matmul(ff_activated, w2)

        # Adds the output of the feedforward network to the output of the attention layer.
        final_output = tensor.Add(ff_output, residual_output_1)

        return final_output


def attention(func_scope, ctx, node, inp_x, wq, wk, wv, wo, w1, w2, w3, rms_att_weight, rms_ffn_weight,
              real_vec0, real_vec1, imag_vec0, imag_vec1, dim, pos,
              len_pad, n_heads, out):
    '''
    compile the kernel_impl
    '''
    scope_manager = FunctionScopeManager()
    scope_manager.fs = func_scope
    scope_manager.node_operator = node
    scope_manager.add_exprs([inp_x, wq, wk, wv, wo, w1, w2, w3, rms_att_weight, rms_ffn_weight,
                             real_vec0, real_vec1, imag_vec0,
                             imag_vec1, dim, pos, len_pad, n_heads])
    scope_manager.output_var = out
    scope_manager.ctx = ctx
    decorator_f = py_compile(locals(), globals(), scope_manager, dump_air=True)(kernel_impl)
    decorator_f()
