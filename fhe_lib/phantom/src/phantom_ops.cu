//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "rt_phantom/rt_phantom.h"
#include "common/rt_api.h"
#include "context.h"

#include <cstdlib>
#include <cstring>
#include <iostream>

// Encode/Decode API
void Phantom_set_output_data(const char *name, size_t idx, CIPHER data) {
    PHANTOM_CONTEXT::Context().Set_output_data(name, idx, *data);
}

CIPHERTEXT Phantom_get_input_data(const char *name, size_t idx) {
    return PHANTOM_CONTEXT::Context().Get_input_data(name, idx);
}

void Phantom_encode_float(PLAIN pt, float *input, size_t len, SCALE_T scale,
                          LEVEL_T level) {
    PHANTOM_CONTEXT::Context().Encode_float(*pt, input, len, scale, level);
}

void Phantom_decode_float(PLAIN pt, std::vector<double> &output) {
    PHANTOM_CONTEXT::Context().Decode(*pt, output);
}

void Phantom_encode_float_cst_lvl(PLAIN pt, float *input, size_t len,
                                  SCALE_T scale, int level) {
    PHANTOM_CONTEXT::Context().Encode_float_cst_lvl(*pt, input, len, scale, level);
}

void Phantom_encode_float_mask(PLAIN pt, float input, size_t len, SCALE_T scale,
                               LEVEL_T level) {
    PHANTOM_CONTEXT::Context().Encode_float_mask(*pt, input, len, scale, level);
}

void Phantom_encode_float_mask_cst_lvl(PLAIN pt, float input, size_t len,
                                       SCALE_T scale, int level) {
    PHANTOM_CONTEXT::Context().Encode_float_mask_cst_lvl(*pt, input, len, scale,
                                                          level);
}

// Evaluation API
void Phantom_add_ciph(CIPHER res, CIPHER op1, CIPHER op2) {
    if (op1->size() == 0) {
        *res = std::move(*op2);
        return;
    }

    PHANTOM_CONTEXT::Context().Add(*res, *op1, *op2);
}

void Phantom_add_plain(CIPHER res, CIPHER op1, PLAIN op2) {
    PHANTOM_CONTEXT::Context().Add(*res, *op1, *op2);
}

void Phantom_add_const(CIPHER res, CIPHER op1, double op2) {
    PHANTOM_CONTEXT::Context().Add_const(*res, *op1, op2);
}

void Phantom_mul_ciph(CIPHER res, CIPHER op1, CIPHER op2) {
    PHANTOM_CONTEXT::Context().Mul(*res, *op1, *op2);
}

void Phantom_mul_ciph_const(CIPHER res, CIPHER op1, double op2) {
    PHANTOM_CONTEXT::Context().Mul_const(*res, *op1, op2);
}

void Phantom_mul_plain(CIPHER res, CIPHER op1, PLAIN op2) {
    PHANTOM_CONTEXT::Context().Mul(*res, *op1, *op2);
}

void Phantom_rotate(CIPHER res, CIPHER op, int step) {
    PHANTOM_CONTEXT::Context().Rotate(*res, *op, step);
}

void Phantom_rescale(CIPHER res, CIPHER op) {
    PHANTOM_CONTEXT::Context().Rescale(*res, *op);
}

void Phantom_mod_switch(CIPHER res, CIPHER op) {
    PHANTOM_CONTEXT::Context().Mod_switch(*res, *op);
}

void Phantom_relin(CIPHER res, CIPHER3 op) {
    PHANTOM_CONTEXT::Context().Relin(*res, *op);
}

void Phantom_bootstrap(CIPHER res, CIPHER op, int level, int slot) {
    PHANTOM_CONTEXT::Context().Bootstrap(*res, *op, level, slot);
}

void Phantom_free_cipher(CIPHER ct) {
    PHANTOM_CONTEXT::Context().Free_cipher(*ct);
}

void Phantom_free_plain(PLAIN pt) {
    PHANTOM_CONTEXT::Context().Free_plain(*pt);
}

void Phantom_free_ciph_poly(CIPHER ct, size_t size) {
    PHANTOM_CONTEXT::Context().Free_ciph_poly(ct, size);
}

void Phantom_copy(CIPHER res, CIPHER op) { *res = *op; }

void Phantom_zero(CIPHER res) { res->zero_ciph(); }

SCALE_T Phantom_scale(CIPHER res) { return PHANTOM_CONTEXT::Context().Scale(*res); }

LEVEL_T Phantom_level(CIPHER res) { return PHANTOM_CONTEXT::Context().Level(*res); }

// Debug API
void Dump_ciph(CIPHER ct, size_t start, size_t len) {
    std::vector<double> vec;
    PHANTOM_CONTEXT::Context().Decrypt(*ct, vec);
    size_t max = std::min(vec.size(), start + len);
    for (size_t i = start; i < max; ++i)
    {
        std::cout << vec[i] << " ";
    }
    std::cout << std::endl;
}

void Dump_plain(PLAIN pt, size_t start, size_t len) {
    std::vector<double> vec;
    PHANTOM_CONTEXT::Context().Decode(*pt, vec);
    size_t max = std::min(vec.size(), start + len);
    for (size_t i = start; i < max; ++i)
    {
        std::cout << vec[i] << " ";
    }
    std::cout << std::endl;
}

void Dump_cipher_msg(const char *name, CIPHER ct, uint32_t len) {
    std::cout << "[" << name << "]: ";
    Dump_ciph(ct, 16, len);
}

void Dump_plain_msg(const char *name, PLAIN pt, uint32_t len) {
    std::cout << "[" << name << "]: ";
    Dump_plain(pt, 16, len);
}

double *Get_msg(CIPHER ct) {
    std::vector<double> vec;
    PHANTOM_CONTEXT::Context().Decrypt(*ct, vec);
    double *msg = (double *)malloc(sizeof(double) * vec.size());
    memcpy(msg, vec.data(), sizeof(double) * vec.size());
    return msg;
}

double *Get_msg_from_plain(PLAIN pt) {
    std::vector<double> vec;
    PHANTOM_CONTEXT::Context().Decode(*pt, vec);
    double *msg = (double *)malloc(sizeof(double) * vec.size());
    memcpy(msg, vec.data(), sizeof(double) * vec.size());
    return msg;
}