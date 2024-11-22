//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_COMMON_COMMON_H
#define RTLIB_COMMON_COMMON_H

//! @brief common.h
//! define common data structure for library-independent runtime interface

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

//! @brief method of data scheme for enc/dec
typedef enum {
  NORMAL,
  CONV,
  CHANNEL,
  DIAGONAL,
} MAP_KIND;

//! @brief describe provider of the underlying library
typedef enum {
  LIB_ANT,      //!< Using ANT in-house library
  LIB_SEAL,     //!< Using SEAL library
  LIB_OPENFHE,  //!< Using OpenFHE library
} LIB_PROV;

//! @brief describe data type in seperated weight data file
typedef enum {
  DE_MSG_F32,   //!< Data entry is message with float type
  DE_MSG_F64,   //!< Data entry is message with double type
  DE_PLAINTEXT  //!< Data entry is plaintext after encoding
} DATA_ENTRY_TYPE;

//! @brief describe the detail of enc/dec
typedef struct {
  MAP_KIND _kind;    //!< How elements are mapped
  int      _count;   //!< Number of elements
  int      _start;   //!< Start index, inclusive
  int      _end;     //!< End index, inclusive
  int      _stride;  //!< Stride
} MAP_DESC;

//! @brief shape of tensor (NCHW)
typedef struct {
  size_t _n;
  size_t _c;
  size_t _h;
  size_t _w;
} SHAPE;

//! @brief describe scheme for enc/dec
typedef struct {
  const char* _name;    //!< name of the parameter/return value
  SHAPE       _shape;   //!< shape of original data
  int         _count;   //!< number of ciphertext to enc/dec
  MAP_DESC    _desc[];  //!< how each ciphertext is enc'ed/dec'ed
} DATA_SCHEME;

//! @brief parameters to create CKKS Context
typedef struct {
  LIB_PROV _provider;          //!< underlying library provider
  uint32_t _poly_degree;       //!< polynomial degree
  size_t   _sec_level;         //!< security level of HE
  size_t   _mul_depth;         //!< multiply depth
  size_t   _first_mod_size;    //!< first prime size
  size_t   _scaling_mod_size;  //!< bits of scaling factor
  size_t   _num_q_parts;       //!< number of parts of q primes
  size_t   _hamming_weight;    //!< hamming weight of secret key
  size_t   _num_rot_idx;       //!< number of rotation idx
  int32_t  _rot_idxs[];        //!< array of rotation idxs
} CKKS_PARAMS;

//! @brief seperated weight data file info
typedef struct {
  const char*     _file_name;   //!< Data file name for reference
  const char*     _file_uuid;   //!< Data file uuid for verification
  DATA_ENTRY_TYPE _entry_type;  //!< Entry type in data file
} RT_DATA_INFO;

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_COMMON_COMMON_H
