//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "ckks/cipher.h"
#include "ckks/ciphertext.h"
#include "ckks/decryptor.h"
#include "ckks/encoder.h"
#include "ckks/encryptor.h"
#include "ckks/plaintext.h"
#include "common/io_api.h"
#include "common/rt_api.h"
#include "context/ckks_context.h"

VALUE_LIST* Pre_encode_scheme_dup(TENSOR* image, const MAP_DESC* desc, uint32_t slot_size) {
  VALUE_LIST* res;
  if (desc->_count == 0) {
    // encode whole image
    size_t len = TENSOR_SIZE(image);
    int sz = 1;
    while(sz<len) sz <<= 1;
    size_t idx = 0;
    res        = Alloc_value_list(DCMPLX_TYPE, slot_size);
    FOR_ALL_TENSOR_ELEM(image, n, c, h, w) {
      DCMPLX_VALUE_AT(res, idx) = TENSOR_ELEM(image, n, c, h, w);
      idx++;
    }
    for(int i=0;i<slot_size;i++) DCMPLX_VALUE_AT(res, i) = DCMPLX_VALUE_AT(res, i%sz);
    IS_TRUE(idx == len, "invalid length of vector");
  } else {
    // encode according to desc
    IS_TRUE(
      desc->_count < TENSOR_SIZE(image) &&
      desc->_start + desc->_count * desc->_stride <= TENSOR_SIZE(image),
    "chunk out of image bound");
    IS_TRUE(desc->_kind == NORMAL, "TODO: other kind");
    IS_TRUE(desc->_stride == 1, "TODO: other stride");
    size_t loc = desc->_start;
    res        = Alloc_value_list(DCMPLX_TYPE, desc->_count + 2 * desc->_pad);
    bool begin_channel = (loc % (TENSOR_H(image) * TENSOR_W(image))) == 0;
    bool end_channel   = ((loc + desc->_count * desc->_stride) %
                            (TENSOR_H(image) * TENSOR_W(image))) == 0;
    // pad before image data
    for (size_t i = 0; i < desc->_pad; ++i) {
      DCMPLX_VALUE_AT(res, i) =
        begin_channel
          ? 0
          : TENSOR_VAL(image, loc - (desc->_pad - i) * desc->_stride);
    }
    for (size_t i = 0; i < desc->_count; ++i) {
      DCMPLX_VALUE_AT(res, i + desc->_pad) = TENSOR_VAL(image, loc);
      loc += desc->_stride;
    }
    // pad after image data
    for (size_t i = 0; i < desc->_pad; ++i) {
      DCMPLX_VALUE_AT(res, i + desc->_pad + desc->_count) =
        end_channel ? 0 : TENSOR_VAL(image, loc + i * desc->_stride);
    }
  }
  return res;
}

VALUE_LIST* Pre_encode_scheme(TENSOR* image, const MAP_DESC* desc) {
  VALUE_LIST* res;
  if (desc->_count == 0) {
    // encode whole image
    size_t len = TENSOR_SIZE(image);
    size_t idx = 0;
    res        = Alloc_value_list(DCMPLX_TYPE, len);
    FOR_ALL_TENSOR_ELEM(image, n, c, h, w) {
      DCMPLX_VALUE_AT(res, idx) = TENSOR_ELEM(image, n, c, h, w);
      idx++;
    }
    IS_TRUE(idx == len, "invalid length of vector");
  } else {
    // encode according to desc
    IS_TRUE(
        desc->_count < TENSOR_SIZE(image) &&
            desc->_start + desc->_count * desc->_stride <= TENSOR_SIZE(image),
        "chunk out of image bound");
    IS_TRUE(desc->_kind == NORMAL, "TODO: other kind");
    IS_TRUE(desc->_stride == 1, "TODO: other stride");
    size_t loc = desc->_start;
    res        = Alloc_value_list(DCMPLX_TYPE, desc->_count + 2 * desc->_pad);
    bool begin_channel = (loc % (TENSOR_H(image) * TENSOR_W(image))) == 0;
    bool end_channel   = ((loc + desc->_count * desc->_stride) %
                        (TENSOR_H(image) * TENSOR_W(image))) == 0;
    // pad before image data
    for (size_t i = 0; i < desc->_pad; ++i) {
      DCMPLX_VALUE_AT(res, i) =
          begin_channel
              ? 0
              : TENSOR_VAL(image, loc - (desc->_pad - i) * desc->_stride);
    }
    for (size_t i = 0; i < desc->_count; ++i) {
      DCMPLX_VALUE_AT(res, i + desc->_pad) = TENSOR_VAL(image, loc);
      loc += desc->_stride;
    }
    // pad after image data
    for (size_t i = 0; i < desc->_pad; ++i) {
      DCMPLX_VALUE_AT(res, i + desc->_pad + desc->_count) =
          end_channel ? 0 : TENSOR_VAL(image, loc + i * desc->_stride);
    }
  }
  return res;
}

void Post_decode_scheme(double* ret, size_t len, VALUE_LIST* vec,
                        const MAP_DESC* desc) {
  // TODO: post decode from DATA_SCHEME
  IS_TRUE(LIST_TYPE(vec) == DCMPLX_TYPE, "invalid type");
  if (desc->_count == 0) {
    FOR_ALL_ELEM(vec, idx) { ret[idx] = creal(Get_dcmplx_value_at(vec, idx)); }
  } else {
    IS_TRUE(desc->_count < len &&
                desc->_start + desc->_count * desc->_stride <= len,
            "chunk out of image bound");
    IS_TRUE(desc->_kind == NORMAL, "TODO: other kind");
    size_t loc = desc->_start;
    for (size_t i = 0; i < desc->_count; ++i) {
      ret[loc] = creal(Get_dcmplx_value_at(vec, i));
      loc += desc->_stride;
    }
  }
}

void Prepare_input_dup(TENSOR* input, const char* name) {
  DATA_SCHEME* scheme = NULL;
  for (int i = 0; i < Get_input_count(); ++i) {
    DATA_SCHEME* ptr = Get_encode_scheme(i);
    IS_TRUE(ptr != NULL, "invalid encode scheme");
    if (strcmp(ptr->_name, name) == 0) {
      scheme = ptr;
      break;
    }
  }
  IS_TRUE(scheme != NULL, "not find the input");
  IS_TRUE(scheme->_count > 0, "no desc found in scheme");
  for (int i = 0; i < scheme->_count; ++i) {
    VALUE_LIST* input_vec = Pre_encode_scheme_dup(input, &scheme->_desc[i], ((CKKS_ENCODER*)Context)->_params->_poly_degree/2);
    IS_TRUE(input_vec != NULL, "invalid input vector");

    // encode & encrypt
    PLAINTEXT* plain     = Alloc_plaintext();
    size_t     input_lev = ((CKKS_ENCODER*)Context)->_params->_input_level;
    ENCODE_AT_LEVEL(plain, (CKKS_ENCODER*)Context->_encoder, input_vec,
                        input_lev);
    CIPHER ciph = Alloc_ciphertext();
    Encrypt_msg(ciph, (CKKS_ENCRYPTOR*)Context->_encryptor, plain);

    Io_set_input(name, i, ciph);
    Free_value_list(input_vec);
    Free_plaintext(plain);
  }
}

void Prepare_input(TENSOR* input, const char* name) {
  DATA_SCHEME* scheme = NULL;
  for (int i = 0; i < Get_input_count(); ++i) {
    DATA_SCHEME* ptr = Get_encode_scheme(i);
    IS_TRUE(ptr != NULL, "invalid encode scheme");
    if (strcmp(ptr->_name, name) == 0) {
      scheme = ptr;
      break;
    }
  }
  IS_TRUE(scheme != NULL, "not find the input");
  IS_TRUE(scheme->_count > 0, "no desc found in scheme");
  for (int i = 0; i < scheme->_count; ++i) {
    VALUE_LIST* input_vec = Pre_encode_scheme(input, &scheme->_desc[i]);
    IS_TRUE(input_vec != NULL, "invalid input vector");

    // encode & encrypt
    PLAINTEXT* plain     = Alloc_plaintext();
    size_t     input_lev = ((CKKS_ENCODER*)Context)->_params->_input_level;
    ENCODE_AT_LEVEL(plain, (CKKS_ENCODER*)Context->_encoder, input_vec,
                    input_lev);
    CIPHER ciph = Alloc_ciphertext();
    Encrypt_msg(ciph, (CKKS_ENCRYPTOR*)Context->_encryptor, plain);

    Io_set_input(name, i, ciph);
    Free_value_list(input_vec);
    Free_plaintext(plain);
  }
}

double* Handle_output(const char* name) {
  DATA_SCHEME* scheme = NULL;
  for (int i = 0; i < Get_output_count(); ++i) {
    DATA_SCHEME* ptr = Get_decode_scheme(i);
    IS_TRUE(ptr != NULL, "invalid encode scheme");
    if (strcmp(ptr->_name, name) == 0) {
      scheme = ptr;
      break;
    }
  }
  IS_TRUE(scheme != NULL, "not find the output");
  IS_TRUE(scheme->_count > 0, "no desc found in scheme");
  size_t len = TENSOR_SIZE(scheme);
  if (len == 0) {
    len = Degree() / 2;
  }
  double* ret = (double*)malloc(len * sizeof(double));
  memset(ret, 0, len * sizeof(double));

  for (int i = 0; i < scheme->_count; ++i) {
    // decrypt & decode
    CIPHER ciph = (CIPHER)Io_get_output(name, i);
    IS_TRUE(ciph != NULL, "not find data");
    PLAINTEXT* plain = Alloc_plaintext();
    Decrypt(plain, (CKKS_DECRYPTOR*)Context->_decryptor, ciph, NULL);
    VALUE_LIST* res = Alloc_value_list(DCMPLX_TYPE, plain->_slots);
    Decode(res, (CKKS_ENCODER*)Context->_encoder, plain);

    Post_decode_scheme(ret, len, res, &scheme->_desc[i]);

    Free_ciphertext(ciph);
    Free_plaintext(plain);
    Free_value_list(res);
  }
  return ret;
}

CIPHERTEXT Get_input_data(const char* name, size_t idx) {
  CIPHERTEXT* data = (CIPHERTEXT*)Io_get_input(name, idx);
  IS_TRUE(data != NULL, "not find data");
  CIPHERTEXT ret = *data;
  free(data);  // only free data itself, won't free poly
  return ret;
}

void Set_output_data(const char* name, size_t idx, CIPHER data) {
  CIPHER output = Alloc_ciphertext();
  Copy_ciph(output, data);
  Free_ciph_poly(data, 1);
  Io_set_output(name, idx, output);
}
