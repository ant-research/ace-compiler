#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
Provides an example to lower tensor add op.
"""
from typing import List
import struct
import math
import sys
from pathlib import Path
sys.path.append(str(Path(__file__).parent / '..'))
from pydsl.compiler import py_script  # noqa T484
from air_dsl import *  # noqa F403
sys.path.append('/usr/local/include/nn/core')  # noqa T484
# autopep8: off
from NN import Rmsnorm, Matmul, Rope, UpdKv, CompMha, Softmax, UpdMha, Accum, Silu, Mul # noqa T484
# autopep8: on


class Config:
    dim: int
    hidden_dim: int
    n_layers: int
    n_heads: int
    n_kv_heads: int
    vocab_size: int
    seq_len: int

    def __init__(self, dim, hidden_dim, n_layers, n_heads, n_kv_heads, vocab_size, seq_len):
        self.dim = dim
        self.hidden_dim = hidden_dim
        self.n_layers = n_layers
        self.n_heads = n_heads
        self.n_kv_heads = n_kv_heads
        self.vocab_size = vocab_size
        self.seq_len = seq_len


class TransformerWeights:
    token_embedding_table: List[float]
    rms_att_weight: List[float]
    wq: List[float]
    wk: List[float]
    wv: List[float]
    wo: List[float]
    rms_ffn_weight: List[float]
    w1: List[float]
    w3: List[float]
    w2: List[float]
    rms_final_weight: List[float]
    freq_cis_real: List[float]
    freq_cis_imag: List[float]
    wcls: List[float]

# ----------------------------------------------------------------------------
# initialization: read from checkpoint


def checkpoint_init_weights(weights: TransformerWeights,
                            conf: Config,
                            file,
                            shared_weights: int) -> None:
    def read_floats(count):
        values = struct.unpack(str(count) + 'f', file.read(count * 4 if count > 0 else count))
        return values

    weights.token_embedding_table = read_floats(conf.vocab_size * conf.dim)
    weights.rms_att_weight = read_floats(conf.n_layers * conf.dim)
    weights.wq = read_floats(conf.n_layers * conf.dim * conf.dim)
    weights.wk = read_floats(conf.n_layers * conf.dim * conf.dim)
    weights.wv = read_floats(conf.n_layers * conf.dim * conf.dim)
    weights.wo = read_floats(conf.n_layers * conf.dim * conf.dim)
    weights.rms_ffn_weight = read_floats(conf.n_layers * conf.dim)
    weights.w1 = read_floats(conf.n_layers * conf.dim * conf.hidden_dim)
    weights.w2 = read_floats(conf.n_layers * conf.hidden_dim * conf.dim)
    weights.w3 = read_floats(conf.n_layers * conf.dim * conf.hidden_dim)
    weights.rms_final_weight = read_floats(conf.dim)
    weights.freq_cis_real = read_floats(conf.seq_len * (conf.dim // conf.n_heads) // 2)
    weights.freq_cis_imag = read_floats(conf.seq_len * (conf.dim // conf.n_heads) // 2)
    weights.wcls = weights.token_embedding_table if shared_weights else read_floats(-1)


def tokenizer_init(conf: Config, file):
    vocab, vocab_scores, max_token_length = [], [], 0

    max_token_length = struct.unpack('i', file.read(4))[0]
    for i in range(0, conf.vocab_size):
        vocab_scores.append(struct.unpack('f', file.read(4))[0])
        len = struct.unpack('i', file.read(4))[0]
        bstr = file.read(len)
        if type(bstr) is not str:
            bstr = bstr.decode('utf8')
        vocab.append(bstr)
    return vocab, vocab_scores, max_token_length


def accum(a, b):
    for i in range(len(a)):
        a[i] += b[i]
    return a


def rmsnorm(x, weight):
    size = len(x)
    out = [0.0] * (size)
    # calculate sum of squares
    ss = 0.0
    for j in range(size):
        ss += x[j] * x[j]
    ss /= size
    ss += 1e-5
    ss = 1.0 / math.sqrt(ss)
    # normalize and scale
    for j in range(size):
        out[j] = weight[j] * (ss * x[j])
    return out


def softmax(x, size):
    # find max value (for numerical stability)
    max_val = x[0]
    for i in range(1, size):
        if x[i] > max_val:
            max_val = x[i]
    # exp and sum
    exp_sum = 0.0
    for i in range(size):
        x[i] = math.exp(x[i] - max_val)
        exp_sum += x[i]
    # normalize
    for i in range(size):
        x[i] /= exp_sum
    return x


def matmul(x, w):
    # W (d,n) @ x (n,) -> xout (d,)
    # by far the most amount of time is spent inside this little function
    n = len(x)  # Number of columns in the matrix
    d = len(w) // n
    xout = [0.0] * (d)
    for i in range(d):
        val = 0.0
        for j in range(n):
            val += w[i * n + j] * x[j]
        xout[i] = val
    return xout


def apply_rope_to_tensor(state_q, n_heads, real_vec0, real_vec1, imag_vec0, imag_vec1):
    dim = len(state_q)
    head_size = dim // n_heads
    for h in range(n_heads):
        q = state_q[h * head_size: (h + 1) * head_size]
        for i in range(0, head_size, 2):
            q[i] = q[i] * real_vec0[h * head_size + i] - q[i + 1] * imag_vec1[h * head_size + i + 1]
            q[i + 1] = q[i] * imag_vec0[h * head_size + i] + \
                q[i + 1] * real_vec1[h * head_size + i + 1]

        state_q[h * head_size: (h + 1) * head_size] = q
    return state_q

# Func apply_rope_to_tensor
# Input:
#     state.q    [config.dim]
#     conf.n_heads
#     head_size
#     real_vec0  [conf.n_heads x head_size]
#     real_vec1  [conf.n_heads x head_size + 1]
#     imag_vec0  [conf.n_heads x head_size]
#     imag_vec1  [conf.n_heads x head_size + 1]
# Output:
#     state.q    [config.dim]


def apply_rope_to_tensor_2(state_q, n_heads, head_size, freq_cis_real_row, freq_cis_imag_row):
    for h in range(n_heads):
        # Get the q vector for this head
        q = state_q[h * head_size: (h + 1) * head_size]

        # Rotate q by the freq_cis_real and freq_cis_imag
        for i in range(0, head_size, 2):
            q0, q1 = q[i], q[i + 1]
            fcr = freq_cis_real_row[i // 2]
            fci = freq_cis_imag_row[i // 2]
            q[i] = q0 * fcr - q1 * fci
            q[i + 1] = q0 * fci + q1 * fcr

        # Reassign back to state.q
        state_q[h * head_size: (h + 1) * head_size] = q
    return state_q

# def update_cache(cache, state_value, dim, pos):


def update_cache(cache, state_value, pos):
    dim = len(state_value)
    cache[pos * dim: (pos + 1) * dim] = state_value
    return cache

# Multihead attention
# Func compute_multihead_attention
# Input:
#     state.q        [config.dim]
#     key_cache      [(conf.seq_len + 1) * conf.dim]
#     n_heads
#     head_size
#     seq_len
#     dim
#     pos
# Output:
#     state.att      [config.n_heads x config.seq_len]


def compute_multihead_attention(state_att, state_q, key_cache, n_heads, pos):
    dim = len(state_q)
    head_size = dim // n_heads
    seq_len = len(state_att) // n_heads
    for h in range(n_heads):
        q = state_q[h * head_size: (h + 1) * head_size]
        att = state_att[h * seq_len: (h + 1) * seq_len]

        for t in range(pos + 1):
            # Extract the key vector for head h at position t

            k = key_cache[t * dim + h * head_size: (t + 1) * dim + h * head_size]
            # 1/sqrt(head_size=64) = 0.125
            # score = sum(q[i] * k[i] for i in range(head_size)) / math.sqrt(head_size)
            score = sum(q[i] * k[i] for i in range(head_size)) * 0.125
            att[t] = score
        state_att[h * seq_len: (h + 1) * seq_len] = att
    return state_att

# Func mha_softmax
# Input:
#     state.att        [config.n_heads x config.seq_len]
#     n_heads
#     pos
# Output
#     state.att        [config.n_heads x config.seq_len]


def mha_softmax(state_att, n_heads, pos):
    seq_len = len(state_att) // n_heads

    for h in range(n_heads):
        att = state_att[h * seq_len: (h + 1) * seq_len]
        # Softmax attention
        state_att[h * seq_len: (h + 1) * seq_len] = softmax(att, pos + 1)
    return state_att

# Func update_multihead_attention
# Input:
#     state.xb        [config.n_heads x config.seq_len]
#     value_cache      [(conf.seq_len + 1) * conf.dim]
#     n_heads
#     head_size
#     seq_len
#     dim
#     pos
# Output:
#     state.xb         [config.dim]


def update_multihead_attention(state_xb, state_att, value_cache, n_heads, pos):
    dim = len(state_xb)
    head_size = dim // n_heads
    seq_len = (len(value_cache) - dim) // dim
    for h in range(n_heads):
        state_xb[h * head_size: (h + 1) * head_size] = [0.0] * head_size
        att = state_att[h * seq_len: (h + 1) * seq_len]
        for t in range(pos + 1):
            v = value_cache[t * dim + h * head_size: (t + 1) * dim + h * head_size]
            a = att[t]
            for i in range(head_size):
                state_xb[h * head_size + i] += a * v[i]
    return state_xb


def mul(state_hb, state_hb2):
    hidden_dim = len(state_hb)
    state_hb = [state_hb[i] * state_hb2[i] for i in range(hidden_dim)]
    return state_hb


def silu(state_hb):
    hidden_dim = len(state_hb)
    for i in range(hidden_dim):
        state_hb[i] = state_hb[i] * (1.0 / (1.0 + math.exp(-state_hb[i])))
    return state_hb


def transformer_func(x, state_q, state_k, state_v, state_att, state_xb, state_xb2,
                     state_hb, state_hb2, key_cache, value_cache, rms_weights, weights_rms_ffn_weight,
                     weights_wq, weights_wk, weights_wv, weights_wo, weights_w1, weights_w2,
                     weights_w3, real_vec0, real_vec1, imag_vec0, imag_vec1, n_heads,
                     head_size, seq_len, pos, dim):
    # Func rmsorm
    # Input:
    #     state.x     [config.dim]
    #     rms_weight  [config.dim]
    # Output:
    #     state.xb    [config.dim]
    state_xb = rmsnorm(x, rms_weights)

    # QKV matmuls for this position

    # Func matmul
    # Input:
    #     state.xb   [config.dim]
    #     weight_wq  [config.dim]
    # Output:
    #     state.q    [config.dim]
    state_q = matmul(state_xb, weights_wq)

    # Func matmul
    # Input:
    #     state.xb   [config.dim]
    #     weight_wk  [config.dim]
    # Output:
    #     state.k    [config.dim]
    state_k = matmul(state_xb, weights_wk)

    # Func matmul
    # Input:
    #     state.xb   [config.dim]
    #     weight_wv  [config.dim]
    # Output:
    #     state.v    [config.dim]
    state_v = matmul(state_xb, weights_wv)

    # Attention
    # Apply RoPE rotation

    # Func apply_rope_to_tensor
    # Input:
    #     state.q    [config.dim]
    #     conf.n_heads
    #     head_size
    #     real_vec0  [conf.n_heads x head_size]
    #     real_vec1  [conf.n_heads x head_size + 1]
    #     imag_vec0  [conf.n_heads x head_size]
    #     imag_vec1  [conf.n_heads x head_size + 1]
    # Output:
    #     state.q    [config.dim]
    state_q = apply_rope_to_tensor(state_q, n_heads, real_vec0, real_vec1, imag_vec0, imag_vec1)

    # Func apply_rope_to_tensor
    # Input:
    #     state.k    [config.dim]
    #     conf.n_heads
    #     head_size
    #     real_vec0  [conf.n_heads x head_size]
    #     real_vec1  [conf.n_heads x head_size + 1]
    #     imag_vec0  [conf.n_heads x head_size]
    #     imag_vec1  [conf.n_heads x head_size + 1]
    # Output:
    #     state.k    [config.dim]
    state_k = apply_rope_to_tensor(state_k, n_heads, real_vec0, real_vec1, imag_vec0, imag_vec1)

    # Save key, value to cache

    # Func update_cache
    # Input:
    #     state.k    [config.dim]
    #     pos
    # Ouput:
    #     key_cache  [pos * dim: (pos + 1) * dim]
    # key_cache = update_cache(key_cache, state_k, dim, pos)
    key_cache = update_cache(key_cache, state_k, pos)

    # Func update_cache
    # Input:
    #     state.v      [config.dim]
    #     pos
    # Ouput:
    #     value_cache  [pos * dim: (pos + 1) * dim]
    # value_cache = update_cache(value_cache, state_v, dim, pos)
    value_cache = update_cache(value_cache, state_v, pos)

    # Multihead attention
    # Func compute_multihead_attention
    # Input:
    #     state.q        [config.dim]
    #     key_cache      [(conf.seq_len + 1) * conf.dim]
    #     n_heads
    #     head_size
    #     seq_len
    #     dim
    #     pos
    # Output:
    #     state.att      [config.n_heads x config.seq_len]
    state_att = compute_multihead_attention(state_att, state_q, key_cache, n_heads, pos)

    # Func mha_softmax
    # Input:
    #     state.att        [config.n_heads x config.seq_len]
    #     n_heads
    #     seq_len
    #     pos
    # Output
    #     state.att        [config.n_heads x config.seq_len]
    state_att = mha_softmax(state_att, n_heads, pos)

    # Func update_multihead_attention
    # Input:
    #     state.xb        [config.n_heads x config.seq_len]
    #     value_cache      [(conf.seq_len + 1) * conf.dim]
    #     n_heads
    #     head_size
    #     seq_len
    #     dim
    #     pos
    # Output:
    #     state.xb         [config.dim]
    state_xb = update_multihead_attention(state_xb, state_att, value_cache, n_heads, pos)

    # Output projection
    # Func matmul
    # Input:
    #     state.xb   [config.dim]
    #     weights_wo [config.dim]
    # Output:
    #     state.xb2   [config.dim]
    state_xb2 = matmul(state_xb, weights_wo)
    # Func accum
    # Input:
    #     state.xb2   [config.dim]
    # Output:
    #     x           [config.dim]
    x = accum(x, state_xb2)

    # FFN
    #
    # Func rmsnorm
    # Input:
    #     state.x     [config.dim]
    #     rms_ffn_weight  [config.dim]
    # Output:
    #     state.xb    [config.dim]
    state_xb = rmsnorm(x, weights_rms_ffn_weight)

    # Calculate self.w1(x) and self.w3(x)
    # Func matmul
    # Input:
    #     state.xb   [config.dim]
    #     weights_w1  [config.hidden_dim]
    # Output:
    #     state.hb    [config.hidden_dim]
    state_hb = matmul(state_xb, weights_w1)
    # Func matmul
    # Input:
    #     state.xb   [config.dim]
    #     weight_w3  [config.hidden_dim]
    # Output:
    #     state.hb2    [config.hidden_dim]
    state_hb2 = matmul(state_xb, weights_w3)

    # Apply SiLU activation function
    state_hb = silu(state_hb)

    # Elementwise multiply with w3(x)
    # Func mul
    # Input:
    #     state.hb     [config.hidden_dim]
    #     state.hb2    [config.hidden_dim]
    # Output:
    #     state.hb     [config.hidden_dim]
    state_hb = mul(state_hb, state_hb2)

    # Final matrix multiplication
    # Func matmul
    # Input:
    #     state.hb   [config.hidden_dim]
    #     weight_w2  [config.dim]
    # Output:
    #     state.xb    [config.dim]
    state_xb = matmul(state_hb, weights_w2)

    # Func accum
    # Input:
    #     state.xb   [config.dim]
    # Output:
    #     x          [config.dim]
    x = accum(x, state_xb)
    return x


def transformer_func_kernel(x, state_q, state_k, state_v, state_att, state_xb, state_xb2,
                            state_hb, state_hb2, key_cache, value_cache, rms_weights, weights_rms_ffn_weight,
                            weights_wq, weights_wk, weights_wv, weights_wo, weights_w1, weights_w2,
                            weights_w3, real_vec0, real_vec1, imag_vec0, imag_vec1, n_heads, head_size,
                            seq_len, pos, dim):
    # Func Rmsorm
    # Input:
    #     state.x     [config.dim]
    #     rms_weight  [config.dim]
    # Output:
    #     state.xb    [config.dim]
    state_xb = Rmsnorm(x, rms_weights)

    # QKV matmuls for this position

    # Func Matmul
    # Input:
    #     state.xb   [config.dim]
    #     weight_wq  [config.dim]
    # Output:
    #     state.q    [config.dim]
    state_q = Matmul(state_xb, weights_wq)

    # Func Matmul
    # Input:
    #     state.xb   [config.dim]
    #     weight_wk  [config.dim]
    # Output:
    #     state.k    [config.dim]
    state_k = Matmul(state_xb, weights_wk)

    # Func matmul
    # Input:
    #     state.xb   [config.dim]
    #     weight_wv  [config.dim]
    # Output:
    #     state.v    [config.dim]
    state_v = Matmul(state_xb, weights_wv)

    # Attention
    # Apply RoPE rotation

    # Func Rope
    # Input:
    #     state.q    [config.dim]
    #     conf.n_heads
    #     head_size
    #     real_vec0  [conf.n_heads x head_size]
    #     real_vec1  [conf.n_heads x head_size + 1]
    #     imag_vec0  [conf.n_heads x head_size]
    #     imag_vec1  [conf.n_heads x head_size + 1]
    # Output:
    #     state.q    [config.dim]
    state_q = Rope(state_q, n_heads, real_vec0, real_vec1, imag_vec0, imag_vec1)

    # Func Rope
    # Input:
    #     state.k    [config.dim]
    #     conf.n_heads
    #     head_size
    #     real_vec0  [conf.n_heads x head_size]
    #     real_vec1  [conf.n_heads x head_size + 1]
    #     imag_vec0  [conf.n_heads x head_size]
    #     imag_vec1  [conf.n_heads x head_size + 1]
    # Output:
    #     state.k    [config.dim]
    state_k = Rope(state_k, n_heads, real_vec0, real_vec1, imag_vec0, imag_vec1)

    # Save key, value to cache

    # Func UpdKv
    # Input:
    #     state.k    [config.dim]
    #     pos
    # Ouput:
    #     key_cache  [pos * dim: (pos + 1) * dim]
    # key_cache = update_cache(key_cache, state_k, dim, pos)
    key_cache = UpdKv(key_cache, state_k, pos)

    # Func update_cache
    # Input:
    #     state.v      [config.dim]
    #     pos
    # Ouput:
    #     value_cache  [pos * dim: (pos + 1) * dim]
    # value_cache = update_cache(value_cache, state_v, dim, pos)
    value_cache = UpdKv(value_cache, state_v, pos)

    # Multihead attention
    # Func CompMha
    # Input:
    #     state.q        [config.dim]
    #     key_cache      [(conf.seq_len + 1) * conf.dim]
    #     n_heads
    #     head_size
    #     seq_len
    #     dim
    #     pos
    # Output:
    #     state.att      [config.n_heads x config.seq_len]
    state_att = CompMha(state_att, state_q, key_cache, n_heads, pos)

    # Funcm Softmax
    # Input:
    #     state.att        [config.n_heads x config.seq_len]
    #     n_heads
    #     seq_len
    #     pos
    # Output
    #     state.att        [config.n_heads x config.seq_len]
    state_att = Softmax(state_att, n_heads, pos)

    # Func UpdMha
    # Input:
    #     state.xb        [config.n_heads x config.seq_len]
    #     value_cache      [(conf.seq_len + 1) * conf.dim]
    #     n_heads
    #     head_size
    #     seq_len
    #     dim
    #     pos
    # Output:
    #     state.xb         [config.dim]
    state_xb = UpdMha(state_xb, state_att, value_cache, n_heads, pos)

    # Output projection
    # Func Matmul
    # Input:
    #     state.xb   [config.dim]
    #     weights_wo [config.dim]
    # Output:
    #     state.xb2   [config.dim]
    state_xb2 = Matmul(state_xb, weights_wo)
    # Func Accum
    # Input:
    #     state.xb2   [config.dim]
    # Output:
    #     x           [config.dim]
    x = Accum(x, state_xb2)

    # FFN
    #
    # Func Rmsnorm
    # Input:
    #     state.x     [config.dim]
    #     rms_ffn_weight  [config.dim]
    # Output:
    #     state.xb    [config.dim]
    state_xb = Rmsnorm(x, weights_rms_ffn_weight)

    # Calculate self.w1(x) and self.w3(x)
    # Func Matmul
    # Input:
    #     state.xb   [config.dim]
    #     weights_w1  [config.hidden_dim]
    # Output:
    #     state.hb    [config.hidden_dim]
    state_hb = Matmul(state_xb, weights_w1)
    # Func matmul
    # Input:
    #     state.xb   [config.dim]
    #     weight_w3  [config.hidden_dim]
    # Output:
    #     state.hb2    [config.hidden_dim]
    state_hb2 = Matmul(state_xb, weights_w3)

    # Apply SiLU activation function
    state_hb = Silu(state_hb)

    # Elementwise multiply with w3(x)
    # Func mul
    # Input:
    #     state.hb     [config.hidden_dim]
    #     state.hb2    [config.hidden_dim]
    # Output:
    #     state.hb     [config.hidden_dim]
    state_hb = Mul(state_hb, state_hb2)

    # Final matrix multiplication
    # Func Matmul
    # Input:
    #     state.hb   [config.hidden_dim]
    #     weight_w2  [config.dim]
    # Output:
    #     state.xb    [config.dim]
    state_xb = Matmul(state_hb, weights_w2)

    # Func Accum
    # Input:
    #     state.xb   [config.dim]
    # Output:
    #     x          [config.dim]
    x = Accum(x, state_xb)
    return x


class RunState:
    x: List[float]
    xb: List[float]
    q: List[float]
    k: List[float]
    v: List[float]
    att: List[float]
    key_cache: List[float]
    value_cache: List[float]
    xb2: List[float]
    hb: List[float]
    hb2: List[float]
    logits: List[float]


# token, pos, config, state, weights
def transformer(token: int, pos: int, conf: Config, state: RunState, weights: TransformerWeights) -> None:
    # A few convenience variables
    x = state.x
    dim = conf.dim
    hidden_dim = conf.hidden_dim
    head_size = dim // conf.n_heads

    # Copy the token embedding into x
    content_row = weights.token_embedding_table[token * dim: (token + 1) * dim]
    x[:] = content_row

    # Pluck out the "pos" row of freq_cis_real and freq_cis_imag
    freq_cis_real_row = weights.freq_cis_real[pos *
                                              head_size // 2: (pos + 1) * head_size // 2]
    freq_cis_imag_row = weights.freq_cis_imag[pos *
                                              head_size // 2: (pos + 1) * head_size // 2]

    # Forward all the layers
    for l in range(conf.n_layers):
        # Attention rmsnorm
        rms_weights = list(weights.rms_att_weight[l * dim: (l + 1) * dim])
        weights_wq = list(weights.wq[l * dim * dim: (l + 1) * dim * dim])
        weights_wk = list(weights.wk[l * dim * dim: (l + 1) * dim * dim])
        weights_wv = list(weights.wv[l * dim * dim: (l + 1) * dim * dim])
        weights_wo = list(weights.wo[l * dim * dim: (l + 1) * dim * dim])
        weights_rms_ffn_weight = list(weights.rms_ffn_weight[l * dim: (l + 1) * dim])
        weights_w1 = list(weights.w1[l * dim * hidden_dim: (l + 1) * dim * hidden_dim])
        weights_w2 = list(weights.w2[l * dim * hidden_dim: (l + 1) * dim * hidden_dim])
        weights_w3 = list(weights.w3[l * dim * hidden_dim: (l + 1) * dim * hidden_dim])

        real_vec0 = [0.0] * (conf.n_heads * head_size)
        real_vec1 = [0.0] * (conf.n_heads * head_size + 1)
        imag_vec0 = [0.0] * (conf.n_heads * head_size)
        imag_vec1 = [0.0] * (conf.n_heads * head_size + 1)

        key_cache = [0.0] * (conf.seq_len * conf.dim + conf.dim)
        value_cache = [0.0] * (conf.seq_len * conf.dim + conf.dim)
        loff = l * conf.seq_len * conf.dim
        key_cache = state.key_cache[loff: loff + conf.seq_len * conf.dim + conf.dim]
        value_cache = state.value_cache[loff: loff + conf.seq_len * conf.dim + conf.dim]

        for h in range(conf.n_heads):
            for i in range(head_size):
                real_vec0[h * head_size + i] = freq_cis_real_row[i // 2]
                real_vec1[h * head_size + i + 1] = freq_cis_real_row[i // 2]
                imag_vec0[h * head_size + i] = freq_cis_imag_row[i // 2]
                imag_vec1[h * head_size + i + 1] = freq_cis_imag_row[i // 2]

        py_script(transformer_func_kernel, locals(), globals(),
                  [x, state.q, state.k,
                   state.v, state.att, state.xb, state.xb2, state.hb, state.hb2,
                   key_cache, value_cache, rms_weights, weights_rms_ffn_weight,
                   weights_wq, weights_wk, weights_wv, weights_wo, weights_w1,
                   weights_w2, weights_w3, real_vec0, real_vec1, imag_vec0,
                   imag_vec1, conf.n_heads, head_size, conf.seq_len, pos, dim])
        x = transformer_func(x, state.q, state.k, state.v, state.att, state.xb,
                             state.xb2, state.hb, state.hb2, key_cache, value_cache,
                             rms_weights, weights_rms_ffn_weight, weights_wq,
                             weights_wk, weights_wv, weights_wo, weights_w1, weights_w2,
                             weights_w3, real_vec0, real_vec1, imag_vec0, imag_vec1,
                             conf.n_heads, head_size, conf.seq_len, pos, dim)

        state.key_cache[loff: loff + conf.seq_len * conf.dim + conf.dim] = key_cache
        state.value_cache[loff: loff + conf.seq_len * conf.dim + conf.dim] = value_cache

    # Final rmsnorm
    x = rmsnorm(x, weights.rms_final_weight)

    # Classifier into logits
    state.logits = matmul(x, weights.wcls)


def argmax(v):
    # return argmax of v
    max_i = 0
    max_p = v[0]
    for i in range(1, len(v)):
        if v[i] > max_p:
            max_i = i
            max_p = v[i]
    return max_i


def init_run_state(state, config):
    state.x = [0.0] * config.dim
    state.xb = [0.0] * config.dim
    state.xb2 = [0.0] * config.dim
    state.hb = [0.0] * config.hidden_dim
    state.hb2 = [0.0] * config.hidden_dim
    state.q = [0.0] * config.dim
    state.k = [0.0] * config.dim
    state.v = [0.0] * config.dim
    state.att = [0.0] * (config.n_heads * config.seq_len)
    state.logits = [0.0] * config.vocab_size
    state.key_cache = [0.0] * (config.n_layers * config.seq_len * config.dim)
    state.value_cache = [0.0] * (config.n_layers * config.seq_len * config.dim)


def run(args):
    checkpoint = args["checkpoint"]
    temperature = float(args["temperature"])
    steps = int(args["steps"])
    prompt = args["prompt"]

    # Read in the model.bin file
    weights = TransformerWeights()

    with open(checkpoint, "rb") as file:
        # Read in the config header
        _config = file.read(struct.calcsize('7i'))
        # Unpacking the data
        dim, hidden_dim, n_layers, n_heads, n_kv_heads, vocab_size, seq_len = struct.unpack(
            '7i', _config)
        # Creating a Config object
        config = Config(dim, hidden_dim, n_layers, n_heads, n_kv_heads, vocab_size, seq_len)

        # negative vocab size is hacky way of signaling unshared weights. bit yikes.
        shared_weights = 1 if config.vocab_size > 0 else 0
        config.vocab_size = abs(config.vocab_size)

        checkpoint_init_weights(weights, config, file, shared_weights)

    # Right now we cannot run for more than config.seq_len steps
    if steps <= 0 or steps > config.seq_len:
        steps = config.seq_len

    # Read in the tokenizer.bin file
    with open("tokenizer.bin", "rb") as file:
        vocab, vocab_scores, max_token_length = tokenizer_init(config, file)

    # Create and initialize the application RunState
    state = RunState()
    init_run_state(state, config)

    # Process the prompt, if any
    prompt_tokens = []
    if prompt:
        prompt_tokens = bpe_encode(prompt, vocab, vocab_scores)

    # Start the main loop
    start = 0  # Used to time our code, only initialized after the first iteration
    next_token = 0  # Will store the next token in the sequence
    # Initialize with token 1 (=BOS), as done in Llama-2 sentencepiece tokenizer
    token = 1
    pos = 0  # Position in the sequence
    # Explicitly print the initial BOS token for stylistic symmetry reasons
    print("<s>")

    while pos < steps:
        # Forward the transformer to get logits for the next token
        transformer(token, pos, config, state, weights)

        if pos < len(prompt_tokens):
            # If we are still processing the input prompt, force the next prompt token
            next_token = prompt_tokens[pos]
        else:
            # Sample the next token
            if temperature == 0.0:
                # Greedy argmax sampling: take the token with the highest probability
                next_token = argmax(state.logits)
            else:
                # Apply the temperature to the logits
                state.logits = [i / temperature for i in state.logits]
                # Apply softmax to the logits to get the probabilities for the next token
                softmax(state.logits, config.vocab_size)
                # Sample from this distribution to get the next token
                next_token = sample(state.logits)

        # Following BOS token (1), sentencepiece decoder strips any leading whitespace
        token_str = (
            vocab[next_token].lstrip()
            if token == 1 and vocab[next_token][0] == ' ' else vocab[next_token]
        )

        print(token_str, end="")
        sys.stdout.flush()

        if next_token == 1:
            break

        # Advance forward
        token = next_token
        pos += 1

    # Report achieved tok/s


if __name__ == "__main__":
    args = {
        "checkpoint": './out/stories15M.bin',
        "temperature": "0.0",
        "steps": "256",
        "prompt": None
    }
    # if len(sys.argv) < 2:
    #     print(
    #         "Usage: python script.py <checkpoint_file> [temperature] [steps] [prompt]")
    #     sys.exit(1)

    if len(sys.argv) >= 2:
        args["checkpoint"] = sys.argv[1]

    if len(sys.argv) >= 3:
        args["temperature"] = sys.argv[2]

    if len(sys.argv) >= 4:
        args["steps"] = sys.argv[3]

    if len(sys.argv) >= 5:
        args["prompt"] = sys.argv[4]

    run(args)
