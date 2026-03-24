#ifndef RTLIB_RT_PHANTOM_PHANTOM_API_H
#define RTLIB_RT_PHANTOM_PHANTOM_API_H

//! @brief phantom_api.h
//! Define phantom API can be used by rtlib

#include "common/tensor.h"
#include "phantom.h"
#include "rt_def.h"

typedef double SCALE_T;
typedef int    LEVEL_T;

//! @brief Phantom API for context management

void       Phantom_prepare_input(TENSOR* input, const char* name);
void       Phantom_set_output_data(const char* name, size_t idx, CIPHER data);
CIPHERTEXT Phantom_get_input_data(const char* name, size_t idx);
void Phantom_encode_float(PLAIN plain, float* input, size_t len, SCALE_T scale,
                          LEVEL_T level);

void Phantom_encode_float_cst_lvl(PLAIN plain, float* input, size_t len,
                                  SCALE_T scale, int level);
void Phantom_encode_float_mask(PLAIN plain, float input, size_t len,
                               SCALE_T scale, LEVEL_T level);
void Phantom_encode_float_mask_cst_lvl(PLAIN plain, float input, size_t len,
                                       SCALE_T scale, int level);

double* Seal_handle_output(const char* name);

//! @brief Phantom API for evaluation
void Phantom_add_ciph(CIPHER res, CIPHER op1, CIPHER op2);
void Phantom_add_const(CIPHER res, CIPHER op1, double op2);
void Phantom_add_plain(CIPHER res, CIPHER op1, PLAIN op2);
void Phantom_mul_ciph(CIPHER res, CIPHER op1, CIPHER op2);
void Phantom_mul_ciph_const(CIPHER res, CIPHER op1, double op2);
void Phantom_mul_plain(CIPHER res, CIPHER op1, PLAIN op2);
void Phantom_rotate(CIPHER res, CIPHER op, int step);
void Phantom_rescale(CIPHER res, CIPHER op);
void Phantom_mod_switch(CIPHER res, CIPHER op);
void Phantom_relin(CIPHER res, CIPHER3 op);
void Phantom_bootstrap(CIPHER res, CIPHER op, int level, int slot);
void Phantom_copy(CIPHER res, CIPHER op);
void Phantom_zero(CIPHER res);
void Phantom_free_ciph(CIPHER res);
void Phantom_free_ciph_array(CIPHER3 res, size_t size);
void Phantom_free_plain(PLAIN res);

bool Need_bts();

SCALE_T Phantom_scale(CIPHER res);
LEVEL_T Phantom_level(CIPHER res);

#endif
