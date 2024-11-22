//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <seal/seal.h>

#include "common/common.h"
#include "common/error.h"
#include "common/io_api.h"
#include "common/rt_api.h"
#include "rt_seal/seal_api.h"

class SEAL_CONTEXT {
public:
  const seal::SecretKey&  Secret_key() const { return _kgen->secret_key(); }
  const seal::PublicKey&  Public_key() const { return *_pk; }
  const seal::RelinKeys&  Relin_key() const { return *_rlk; }
  const seal::GaloisKeys& Rotate_key() const { return *_rtk; }

  static SEAL_CONTEXT* Context() {
    IS_TRUE(Instance != nullptr, "instance not initialized");
    return Instance;
  }

  static void Init_context() {
    IS_TRUE(Instance == nullptr, "instance already initialized");
    Instance = new SEAL_CONTEXT();
  }

  static void Fini_context() {
    IS_TRUE(Instance != nullptr, "instance not initialized");
    delete Instance;
    Instance = nullptr;
  }

public:
  void Prepare_input(TENSOR* input, const char* name) {
    size_t              len = TENSOR_SIZE(input);
    std::vector<double> vec(input->_vals, input->_vals + len);
    seal::Plaintext     pt;
    _encoder->encode(vec, std::pow(2.0, _scaling_mod_size), pt);
    seal::Ciphertext* ct = new seal::Ciphertext;
    _encryptor->encrypt(pt, *ct);
    Io_set_input(name, 0, ct);
  }

  void Set_output_data(const char* name, size_t idx, seal::Ciphertext* ct) {
    Io_set_output(name, idx, new seal::Ciphertext(*ct));
  }

  seal::Ciphertext Get_input_data(const char* name, size_t idx) {
    seal::Ciphertext* data = (seal::Ciphertext*)Io_get_input(name, idx);
    IS_TRUE(data != nullptr, "not find data");
    return *data;
  }

  double* Handle_output(const char* name, size_t idx) {
    seal::Ciphertext* data = (seal::Ciphertext*)Io_get_output(name, idx);
    IS_TRUE(data != nullptr, "not find data");
    seal::Plaintext pt;
    _decryptor->decrypt(*data, pt);
    std::vector<double>* vec = new std::vector<double>;
    _encoder->decode(pt, *vec);
    return vec->data();
  }

  void Encode_float(seal::Plaintext* pt, float* input, size_t len,
                    uint32_t sc_degree, uint32_t level) {
    std::vector<double> vec(input, input + len);
    // TODO: find parms_id from level and pass to encode
    _encoder->encode(vec, std::pow(2, sc_degree * _scaling_mod_size), *pt);
  }

  void Decrypt(seal::Ciphertext* ct, std::vector<double>& vec) {
    seal::Plaintext pt;
    _decryptor->decrypt(*ct, pt);
    _encoder->decode(pt, vec);
  }

  void Decode(seal::Plaintext* pt, std::vector<double>& vec) {
    _encoder->decode(*pt, vec);
  }

public:
  void Add(seal::Ciphertext* op1, seal::Ciphertext* op2,
           seal::Ciphertext* res) {
    _eval->add(*op1, *op2, *res);
  }

  void Add(seal::Ciphertext* op1, seal::Plaintext* op2, seal::Ciphertext* res) {
    _eval->add_plain(*op1, *op2, *res);
  }

  void Mul(seal::Ciphertext* op1, seal::Ciphertext* op2,
           seal::Ciphertext* res) {
    _eval->multiply(*op1, *op2, *res);
  }

  void Mul(seal::Ciphertext* op1, seal::Plaintext* op2, seal::Ciphertext* res) {
    _eval->multiply_plain(*op1, *op2, *res);
  }

  void Rotate(seal::Ciphertext* op1, int step, seal::Ciphertext* res) {
    _eval->rotate_vector(*op1, step, *_rtk, *res);
  }

  uint64_t Sc_degree(seal::Ciphertext* op) {
    return (uint64_t)std::log2(op->scale()) / _scaling_mod_size;
  }

  uint64_t Level(seal::Ciphertext* op) {
    auto ctx_data = _ctx->get_context_data(op->parms_id());
    return ctx_data->chain_index();
  }

private:
  SEAL_CONTEXT(const SEAL_CONTEXT&)            = delete;
  SEAL_CONTEXT& operator=(const SEAL_CONTEXT&) = delete;

  SEAL_CONTEXT();
  ~SEAL_CONTEXT();

  static SEAL_CONTEXT* Instance;

private:
  seal::SEALContext*  _ctx;
  seal::KeyGenerator* _kgen;

  const seal::SecretKey* _sk;
  seal::PublicKey*       _pk;
  seal::RelinKeys*       _rlk;
  seal::GaloisKeys*      _rtk;

  seal::Evaluator*   _eval;
  seal::CKKSEncoder* _encoder;
  seal::Encryptor*   _encryptor;
  seal::Decryptor*   _decryptor;

  uint64_t _scaling_mod_size;
};

SEAL_CONTEXT* SEAL_CONTEXT::Instance = nullptr;

SEAL_CONTEXT::SEAL_CONTEXT() {
  IS_TRUE(Instance == nullptr, "_install already created");

  CKKS_PARAMS* prog_param = Get_context_params();
  IS_TRUE(prog_param->_provider == LIB_SEAL, "provider is not SEAL");
  seal::EncryptionParameters parms(seal::scheme_type::ckks);
  uint32_t                   degree = prog_param->_poly_degree;
  parms.set_poly_modulus_degree(degree);
  std::vector<int> bits;
  bits.push_back(prog_param->_first_mod_size);
  for (uint32_t i = 0; i < prog_param->_mul_depth; ++i) {
    bits.push_back(prog_param->_scaling_mod_size);
  }
  bits.push_back(prog_param->_first_mod_size);
  parms.set_coeff_modulus(seal::CoeffModulus::Create(degree, bits));

  seal::sec_level_type sec = seal::sec_level_type::tc128;
  if (degree < 4096) {
    DEV_WARN("WARNING: degree %d too small, reset security level to none\n",
             degree);
    sec = seal::sec_level_type::none;
  }
  _ctx  = new seal::SEALContext(parms, true, sec);
  _kgen = new seal::KeyGenerator(*_ctx);
  _sk   = &_kgen->secret_key();
  _pk   = new seal::PublicKey;
  _kgen->create_public_key(*_pk);
  _rlk = new seal::RelinKeys;
  _kgen->create_relin_keys(*_rlk);
  _rtk = new seal::GaloisKeys;
  _kgen->create_galois_keys(*_rtk);

  _eval      = new seal::Evaluator(*_ctx);
  _encoder   = new seal::CKKSEncoder(*_ctx);
  _encryptor = new seal::Encryptor(*_ctx, *_pk, *_sk);
  _decryptor = new seal::Decryptor(*_ctx, *_sk);

  _scaling_mod_size = prog_param->_scaling_mod_size;

  printf(
      "ckks_param: _provider = %d, _poly_degree = %d, _sec_level = %ld, "
      "mul_depth = %ld, _first_mod_size = %ld, _scaling_mod_size = %ld, "
      "_num_q_parts = %ld, _num_rot_idx = %ld\n",
      prog_param->_provider, prog_param->_poly_degree, prog_param->_sec_level,
      prog_param->_mul_depth, prog_param->_first_mod_size,
      prog_param->_scaling_mod_size, prog_param->_num_q_parts,
      prog_param->_num_rot_idx);
}

SEAL_CONTEXT::~SEAL_CONTEXT() {
  delete _decryptor;
  delete _encryptor;
  delete _encoder;
  delete _eval;

  delete _rtk;
  delete _rlk;
  delete _pk;
  // delete _sk;

  delete _kgen;
  delete _ctx;
}

// Vendor-specific RT API
void Prepare_context() {
  Io_init();
  SEAL_CONTEXT::Init_context();
}

void Finalize_context() {
  SEAL_CONTEXT::Fini_context();
  Io_fini();
}

void Prepare_input(TENSOR* input, const char* name) {
  SEAL_CONTEXT::Context()->Prepare_input(input, name);
}

double* Handle_output(const char* name) {
  return SEAL_CONTEXT::Context()->Handle_output(name, 0);
}

// Encode/Decode API
void Seal_set_output_data(const char* name, size_t idx, CIPHER data) {
  SEAL_CONTEXT::Context()->Set_output_data(name, idx, data);
}

CIPHERTEXT Seal_get_input_data(const char* name, size_t idx) {
  return SEAL_CONTEXT::Context()->Get_input_data(name, idx);
}

void Seal_encode_from_float(PLAIN pt, float* input, size_t len,
                            uint32_t sc_degree, uint32_t level) {
  SEAL_CONTEXT::Context()->Encode_float(pt, input, len, sc_degree, level);
}

// Evaluation API
void Seal_add_ciph(CIPHER res, CIPHER op1, CIPHER op2) {
  if (op1->size() == 0) {
    // special handling for accumulation
    *res = *op2;
    return;
  }
  SEAL_CONTEXT::Context()->Add(op1, op2, res);
}

void Seal_add_plain(CIPHER res, CIPHER op1, PLAIN op2) {
  SEAL_CONTEXT::Context()->Add(op1, op2, res);
}

void Seal_mul_ciph(CIPHER res, CIPHER op1, CIPHER op2) {
  SEAL_CONTEXT::Context()->Mul(op1, op2, res);
}

void Seal_mul_plain(CIPHER res, CIPHER op1, PLAIN op2) {
  SEAL_CONTEXT::Context()->Mul(op1, op2, res);
}

void Seal_rotate(CIPHER res, CIPHER op, int step) {
  SEAL_CONTEXT::Context()->Rotate(op, step, res);
}

void Seal_copy(CIPHER res, CIPHER op) { *res = *op; }

void Seal_zero(CIPHER res) { res->release(); }

uint64_t Seal_sc_degree(CIPHER res) {
  return SEAL_CONTEXT::Context()->Sc_degree(res);
}

uint64_t Seal_level(CIPHER res) { return SEAL_CONTEXT::Context()->Level(res); }

// Debug API
void Dump_ciph(CIPHER ct, size_t start, size_t len) {
  std::vector<double> vec;
  SEAL_CONTEXT::Context()->Decrypt(ct, vec);
  size_t max = std::min(vec.size(), start + len);
  for (size_t i = start; i < max; ++i) {
    std::cout << vec[i] << " ";
  }
  std::cout << std::endl;
}

void Dump_plain(PLAIN pt, size_t start, size_t len) {
  std::vector<double> vec;
  SEAL_CONTEXT::Context()->Decode(pt, vec);
  size_t max = std::min(vec.size(), start + len);
  for (size_t i = start; i < max; ++i) {
    std::cout << vec[i] << " ";
  }
  std::cout << std::endl;
}
