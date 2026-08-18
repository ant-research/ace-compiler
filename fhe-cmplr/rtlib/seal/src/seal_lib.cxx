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
  using Ciphertext = seal::Ciphertext;
  using Plaintext  = seal::Plaintext;

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
    Plaintext           pt;
    _encoder->encode(vec, std::pow(2.0, _scaling_mod_size), pt);
    Ciphertext* ct = new Ciphertext;
    _encryptor->encrypt(pt, *ct);
    Io_set_input(name, 0, ct);
  }

  void Set_output_data(const char* name, size_t idx, Ciphertext* ct) {
    Io_set_output(name, idx, new Ciphertext(*ct));
  }

  Ciphertext Get_input_data(const char* name, size_t idx) {
    Ciphertext* data = (Ciphertext*)Io_get_input(name, idx);
    IS_TRUE(data != nullptr, "not find data");
    return *data;
  }

  double* Handle_output(const char* name, size_t idx) {
    Ciphertext* data = (Ciphertext*)Io_get_output(name, idx);
    IS_TRUE(data != nullptr, "not find data");
    Plaintext pt;
    _decryptor->decrypt(*data, pt);
    std::vector<double> vec;
    _encoder->decode(pt, vec);
    double* msg = (double*)malloc(sizeof(double) * vec.size());
    memcpy(msg, vec.data(), sizeof(double) * vec.size());
    return msg;
  }

  void Encode_float(Plaintext* pt, float* input, size_t len, SCALE_T scale,
                    LEVEL_T level) {
    std::vector<double> vec(input, input + len);
    _encoder->encode(vec, level, scale, *pt);
  }

  void Decrypt(Ciphertext* ct, std::vector<double>& vec) {
    Plaintext pt;
    _decryptor->decrypt(*ct, pt);
    _encoder->decode(pt, vec);
  }

  void Decode(Plaintext* pt, std::vector<double>& vec) {
    _encoder->decode(*pt, vec);
  }

public:
  void Add(const Ciphertext* op1, const Ciphertext* op2, Ciphertext* res) {
    if (res == op1) {
      _eval->add_inplace(*res, *op2);
    } else if (res == op2) {
      _eval->add_inplace(*res, *op1);
    } else {
      _eval->add(*op1, *op2, *res);
    }
  }

  void Add(const Ciphertext* op1, const Plaintext* op2, Ciphertext* res) {
    if (res == op1) {
      _eval->add_plain_inplace(*res, *op2);
    } else {
      _eval->add_plain(*op1, *op2, *res);
    }
  }

  void Mul(const Ciphertext* op1, const Ciphertext* op2, Ciphertext* res) {
    if (res == op1) {
      _eval->multiply_inplace(*res, *op2);
    } else if (res == op2) {
      _eval->multiply_inplace(*res, *op1);
    } else {
      _eval->multiply(*op1, *op2, *res);
    }
  }

  void Mul(const Ciphertext* op1, const Plaintext* op2, Ciphertext* res) {
    if (res == op1) {
      _eval->multiply_plain_inplace(*res, *op2);
    } else {
      _eval->multiply_plain(*op1, *op2, *res);
    }
  }

  void Rotate(const Ciphertext* op1, int step, Ciphertext* res) {
    if (res == op1) {
      _eval->rotate_vector_inplace(*res, step, *_rtk);
    } else {
      _eval->rotate_vector(*op1, step, *_rtk, *res);
    }
  }

  void Rescale(const Ciphertext* op1, Ciphertext* res) {
    if (res == op1) {
      _eval->rescale_to_next_inplace(*res);
    } else {
      _eval->rescale_to_next(*op1, *res);
    }
  }

  void Mod_switch(const Ciphertext* op1, Ciphertext* res) {
    if (res == op1) {
      _eval->mod_switch_to_next_inplace(*res);
    } else {
      _eval->mod_switch_to_next(*op1, *res);
    }
  }

  void Relin(const Ciphertext* op1, Ciphertext* res) {
    if (res == op1) {
      _eval->relinearize_inplace(*res, *_rlk);
    } else {
      _eval->relinearize(*op1, *_rlk, *res);
    }
  }

  SCALE_T Scale(Ciphertext* op) { return op->scale(); }

  LEVEL_T Level(Ciphertext* op) { return op->parms_id(); }

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
  for (uint32_t i = 1; i < prog_param->_mul_depth; ++i) {
    bits.push_back(prog_param->_scaling_mod_size);
  }
  bits.push_back(prog_param->_first_mod_size);
  parms.set_coeff_modulus(seal::CoeffModulus::Create(degree, bits));

  seal::sec_level_type sec = seal::sec_level_type::tc128;
  switch (prog_param->_sec_level) {
    case 128:
      sec = seal::sec_level_type::tc128;
      break;
    case 192:
      sec = seal::sec_level_type::tc192;
      break;
    case 256:
      sec = seal::sec_level_type::tc256;
      break;
    default:
      sec = seal::sec_level_type::none;
      break;
  }
  if (degree < 4096 && sec != seal::sec_level_type::none) {
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

void Seal_encode_from_float(PLAIN pt, float* input, size_t len, SCALE_T scale,
                            LEVEL_T level) {
  SEAL_CONTEXT::Context()->Encode_float(pt, input, len, scale, level);
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

void Seal_rescale(CIPHER res, CIPHER op) {
  SEAL_CONTEXT::Context()->Rescale(op, res);
}

void Seal_mod_switch(CIPHER res, CIPHER op) {
  SEAL_CONTEXT::Context()->Mod_switch(op, res);
}

void Seal_relin(CIPHER res, CIPHER3 op) {
  SEAL_CONTEXT::Context()->Relin(op, res);
}

void Seal_copy(CIPHER res, CIPHER op) { *res = *op; }

void Seal_zero(CIPHER res) { res->release(); }

SCALE_T Seal_scale(CIPHER res) { return SEAL_CONTEXT::Context()->Scale(res); }

LEVEL_T Seal_level(CIPHER res) { return SEAL_CONTEXT::Context()->Level(res); }

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

void Dump_cipher_msg(const char* name, CIPHER ct, uint32_t len) {
  std::cout << "[" << name << "]: ";
  Dump_ciph(ct, 16, len);
}

void Dump_plain_msg(const char* name, PLAIN pt, uint32_t len) {
  std::cout << "[" << name << "]: ";
  Dump_plain(pt, 16, len);
}

double* Get_msg(CIPHER ct) {
  std::vector<double> vec;
  SEAL_CONTEXT::Context()->Decrypt(ct, vec);
  double* msg = (double*)malloc(sizeof(double) * vec.size());
  memcpy(msg, vec.data(), sizeof(double) * vec.size());
  return msg;
}

double* Get_msg_from_plain(PLAIN pt) {
  std::vector<double> vec;
  SEAL_CONTEXT::Context()->Decode(pt, vec);
  double* msg = (double*)malloc(sizeof(double) * vec.size());
  memcpy(msg, vec.data(), sizeof(double) * vec.size());
  return msg;
}

bool Within_value_range(CIPHER ciph, double* msg, uint32_t len) {
  FMT_ASSERT(false, "TODO: not implemented in seal");
}
