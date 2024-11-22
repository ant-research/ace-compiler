//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_INCLUDE_CIPHER_EVAL_H
#define RTLIB_INCLUDE_CIPHER_EVAL_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "ckks/plain_eval.h"
#include "util/ciphertext.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef CIPHERTEXT*  CIPHER;
typedef CIPHERTEXT3* CIPHER3;

//! @brief cleanup ciphertext
void Free_cipher(CIPHER ciph);

//! @brief Initialize ciphertext from ciph1 & ciph2 with same scale,
//! which is used for add_ciph() & sub_ciph(), and used for
//! mul_integer() & mul_by_monomial() & rotate() when ciph2 == NULL
void Init_ciph_same_scale(CIPHER res, CIPHER ciph1, CIPHER ciph2);

//! @brief Initialize ciphertext from ciph & plain with same scale,
//! which is used for add_plain()
void Init_ciph_same_scale_plain(CIPHER res, CIPHER ciph1, PLAIN plain);

//! @brief Initialize ciphertext from ciph3 with same scale,
//! which is used for relinearize()
void Init_ciph_same_scale_ciph3(CIPHER res, CIPHER3 ciph);

//! @brief Initialize ciphertext3 from ciph1 & ciph2 with same scale,
//! which is used for add_ciph() & sub_ciph() for ciphertext3
void Init_ciph3_same_scale_ciph3(CIPHER3 res, CIPHER3 ciph1, CIPHER3 ciph2);

//! @brief Initialize ciphertext from ciph1 & ciph2 with scale up,
//! which is used for mul_ciph()
void Init_ciph_up_scale(CIPHER res, CIPHER ciph1, CIPHER ciph2);

//! @brief Initialize ciphertext from ciph1 & ciph2 with scale up,
//! which is used for mul_plain()
void Init_ciph_up_scale_plain(CIPHER res, CIPHER ciph1, PLAIN plain);

//! @brief Initialize ciphertext3 from ciph1 & ciph2 with scale up,
//! which is used for mul_ciph()
void Init_ciph3_up_scale(CIPHER3 res, CIPHER ciph1, CIPHER ciph2);

//! @brief Initialize ciphertext from ciph1 & ciph2 with scale down,
//! which is used for rescale()
void Init_ciph_down_scale(CIPHER res, CIPHER ciph);

//! @brief Copy ciphertext
void Copy_ciph(CIPHER res, CIPHER ciph);

//! @brief Get level from ciphertext
size_t Level(CIPHER ciph);

//! @brief Get degree of scaling factor for ciphertext
uint32_t Sc_degree(CIPHER ciph);

//! @brief Get slots from ciphertext
uint32_t Get_slots(CIPHER ciph);

//! @brief Set slots to ciphertext
void Set_slots(CIPHER ciph, uint32_t slots);

//! @brief Get the message(only real part) content obtained by decrypting &
//! decoding ciphertext
double* Get_msg(CIPHER ciph);

//! @brief Get the full message (including imaginary part) content obtained by
//! decrypting & decoding ciphertext
DCMPLX* Get_msg_with_imag(CIPHER ciph);

//! @brief Print message[start, end] obtained by decrypt & decode ciphertext
void Print_cipher_msg(FILE* fp, const char* name, CIPHER ciph, uint32_t len);

//! @brief Print message[start, end] (including imaginary part) obtained by
//! decrypt & decode ciphertext
void Print_cipher_msg_with_imag(FILE* fp, const char* name, CIPHER ciph,
                                uint32_t len);

//! @brief Print ciphertext message range
void Print_cipher_range(FILE* fp, const char* name, CIPHER ciph);

//! @brief Print ciphertext degree/level/slots/scaling factor info
void Print_cipher_info(FILE* fp, const char* name, CIPHER ciph);

//! @brief Print ciphertext polynomial's coefficient
void Print_cipher_poly(FILE* fp, const char* name, CIPHER ciph);

//! @brief Dump ciphertext message
void Dump_cipher_msg(const char* name, CIPHER ciph, uint32_t len);

//! @brief decrypt, do relu, encrypt
CIPHER Real_relu(CIPHER ciph);

//! @brief Add two ciphertexts
CIPHER Add_ciph(CIPHER res, CIPHER ciph1, CIPHER ciph2);

//! @brief Add a ciphertext with a plaintext
CIPHER Add_plain(CIPHER res, CIPHER ciph, PLAIN plain);

//! @brief Subtract two ciphertexts
CIPHER Sub_ciph(CIPHER res, CIPHER ciph1, CIPHER ciph2);

//! @brief Multiply two ciphertexts with relinearization
CIPHER Mul_ciph(CIPHER res, CIPHER ciph1, CIPHER ciph2);

//! @brief Multiply two ciphertexts without relinearization
CIPHER3 Mul_ciph3(CIPHER3 res, CIPHER ciph1, CIPHER ciph2);

//! @brief Multiply a ciphertext with a plaintext
CIPHER Mul_plain(CIPHER res, CIPHER ciph, PLAIN plain);

//! @brief Relinearizes a 3-dimensional ciphertext(ciphertext3)
//! to a 2-dimensional ciphertext.
CIPHER Relin(CIPHER res, CIPHER3 ciph);

//! @brief Rescale a ciphertext to a new scaling factor
CIPHER Rescale_ciph(CIPHER res, CIPHER ciph);

//! @brief Upscale a ciphertext to a given mod_size
CIPHER Upscale_ciph(CIPHER res, CIPHER ciph, uint32_t mod_size);

//! @brief Downscale a ciphertext to given waterline
CIPHER Downscale_ciph(CIPHER res, CIPHER ciph, uint32_t waterline);

//! @brief Modswitch a ciphertext
void Modswitch_ciph(CIPHER ciph);

//! @brief Rotate a ciphertext with given rotation idx
CIPHER Rotate_ciph(CIPHER res, CIPHER ciph, int32_t rotation);

//! @brief Perform bootstrap
//! @param level_after_bts The level avaiable after bootstrap, which is used to
//! set raise level, when the level is set to 0, ciph will be raised to q_cnt
CIPHER Bootstrap(CIPHER res, CIPHER ciph, uint32_t level_after_bts);

//! @brief TO BEREMOVE just for bootstrap example
CIPHER Encrypt(CIPHER res, PLAIN plain);

//! @brief Free poly inside a cipher array
static inline void Free_ciph_poly(CIPHER ciph, uint32_t cnt) {
  for (uint32_t i = 0; i < cnt; ++i) {
    Free_poly_data(&ciph[i]._c0_poly);
    Free_poly_data(&ciph[i]._c1_poly);
  }
}

//! @brief Clear ciph content and free poly
static inline void Zero_ciph(CIPHER ciph) {
  if (Get_c0(ciph)) {
    Free_poly_data(Get_c0(ciph));
  }
  if (Get_c1(ciph)) {
    Free_poly_data(Get_c1(ciph));
  }
  memset(ciph, 0, sizeof(*ciph));
}

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_INCLUDE_CIPHER_EVAL_H
