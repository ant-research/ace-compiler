//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <openfhe.h>

#include "common/common.h"
#include "common/error.h"
#include "common/io_api.h"
#include "common/rt_api.h"
#include "rt_openfhe/openfhe_api.h"

class OPENFHE_CONTEXT {
  using Plaintext     = lbcrypto::Plaintext;
  using KeyPair       = lbcrypto::KeyPair<lbcrypto::DCRTPoly>;
  using Ciphertext    = lbcrypto::Ciphertext<lbcrypto::DCRTPoly>;
  using CryptoContext = lbcrypto::CryptoContext<lbcrypto::DCRTPoly>;

public:
  static OPENFHE_CONTEXT* Context() {
    IS_TRUE(Instance != nullptr, "instance not initialized");
    return Instance;
  }

  static void Init_context() {
    IS_TRUE(Instance == nullptr, "instance already initialized");
    Instance = new OPENFHE_CONTEXT();
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
    Plaintext           pt = _ctx->MakeCKKSPackedPlaintext(vec);
    Ciphertext*         ct = new Ciphertext;
    *ct                    = _ctx->Encrypt(_kp.publicKey, pt);
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
    _ctx->Decrypt(_kp.secretKey, *data, &pt);
    std::vector<double>* vec = new std::vector<double>;
    *vec                     = pt->GetRealPackedValue();
    return vec->data();
  }

  void Encode_float(Plaintext* pt, float* input, size_t len, uint32_t sc_degree,
                    uint32_t level) {
    std::vector<double> vec(input, input + len);
    *pt = _ctx->MakeCKKSPackedPlaintext(vec, sc_degree, level);
  }

  void Decrypt(Ciphertext* ct, std::vector<double>& vec) {
    Plaintext pt;
    _ctx->Decrypt(_kp.secretKey, *ct, &pt);
    vec = pt->GetRealPackedValue();
  }

  void Decode(Plaintext* pt, std::vector<double>& vec) {
    vec = (*pt)->GetRealPackedValue();
  }

public:
  void Add(Ciphertext* op1, Ciphertext* op2, Ciphertext* res) {
    *res = _ctx->EvalAdd(*op1, *op2);
  }

  void Add(Ciphertext* op1, Plaintext* op2, Ciphertext* res) {
    *res = _ctx->EvalAdd(*op1, *op2);
  }

  void Mul(Ciphertext* op1, Ciphertext* op2, Ciphertext* res) {
    *res = _ctx->EvalMult(*op1, *op2);
  }

  void Mul(Ciphertext* op1, Plaintext* op2, Ciphertext* res) {
    *res = _ctx->EvalMult(*op1, *op2);
  }

  void Rotate(Ciphertext* op1, int step, Ciphertext* res) {
    *res = _ctx->EvalRotate(*op1, step);
  }

private:
  OPENFHE_CONTEXT(const OPENFHE_CONTEXT&)            = delete;
  OPENFHE_CONTEXT& operator=(const OPENFHE_CONTEXT&) = delete;

  OPENFHE_CONTEXT();
  ~OPENFHE_CONTEXT();

  static OPENFHE_CONTEXT* Instance;

private:
  CryptoContext _ctx;
  KeyPair       _kp;
};

OPENFHE_CONTEXT* OPENFHE_CONTEXT::Instance = nullptr;

OPENFHE_CONTEXT::OPENFHE_CONTEXT() {
  IS_TRUE(Instance == nullptr, "_install already created");

  CKKS_PARAMS* prog_param = Get_context_params();
  IS_TRUE(prog_param->_provider == LIB_OPENFHE, "provider is not OPENFHE");

  printf(
      "ckks_param: _provider = %d, _poly_degree = %d, _sec_level = %ld, "
      "mul_depth = %ld, _first_mod_size = %ld, _scaling_mod_size = %ld, "
      "_num_q_parts = %ld, _num_rot_idx = %ld\n",
      prog_param->_provider, prog_param->_poly_degree, prog_param->_sec_level,
      prog_param->_mul_depth, prog_param->_first_mod_size,
      prog_param->_scaling_mod_size, prog_param->_num_q_parts,
      prog_param->_num_rot_idx);

  uint32_t                degree = prog_param->_poly_degree;
  lbcrypto::SecurityLevel sec    = lbcrypto::SecurityLevel::HEStd_128_classic;
  if (degree < 4096) {
    DEV_WARN("WARNING: degree %d too small, reset security level to none\n",
             degree);
    sec = lbcrypto::SecurityLevel::HEStd_NotSet;
  }
  lbcrypto::CCParams<lbcrypto::CryptoContextCKKSRNS> parms;
  parms.SetRingDim(degree);
  parms.SetMultiplicativeDepth(prog_param->_mul_depth);
  parms.SetScalingModSize(prog_param->_scaling_mod_size);
  parms.SetFirstModSize(prog_param->_first_mod_size);
  parms.SetSecurityLevel(sec);
  _ctx = lbcrypto::GenCryptoContext(parms);
  _ctx->Enable(lbcrypto::PKE);
  _ctx->Enable(lbcrypto::KEYSWITCH);
  _ctx->Enable(lbcrypto::LEVELEDSHE);

  _kp = _ctx->KeyGen();
  _ctx->EvalMultKeyGen(_kp.secretKey);
  std::vector<int32_t> steps(prog_param->_rot_idxs,
                             prog_param->_rot_idxs + prog_param->_num_rot_idx);
  _ctx->EvalRotateKeyGen(_kp.secretKey, steps);
}

OPENFHE_CONTEXT::~OPENFHE_CONTEXT() {}

// Vendor-specific RT API
void Prepare_context() {
  Io_init();
  OPENFHE_CONTEXT::Init_context();
}

void Finalize_context() {
  OPENFHE_CONTEXT::Fini_context();
  Io_fini();
}

void Prepare_input(TENSOR* input, const char* name) {
  OPENFHE_CONTEXT::Context()->Prepare_input(input, name);
}

double* Handle_output(const char* name) {
  return OPENFHE_CONTEXT::Context()->Handle_output(name, 0);
}

// Encode/Decode API
void Openfhe_set_output_data(const char* name, size_t idx, CIPHER data) {
  OPENFHE_CONTEXT::Context()->Set_output_data(name, idx, data);
}

CIPHERTEXT Openfhe_get_input_data(const char* name, size_t idx) {
  return OPENFHE_CONTEXT::Context()->Get_input_data(name, idx);
}

void Openfhe_encode_from_float(PLAIN pt, float* input, size_t len,
                               uint32_t sc_degree, uint32_t level) {
  OPENFHE_CONTEXT::Context()->Encode_float(pt, input, len, sc_degree, level);
}

// Evaluation API
void Openfhe_add_ciph(CIPHER res, CIPHER op1, CIPHER op2) {
  if (op1->get() == nullptr) {
    // special handling for accumulation
    *res = *op2;
    return;
  }
  OPENFHE_CONTEXT::Context()->Add(op1, op2, res);
}

void Openfhe_add_plain(CIPHER res, CIPHER op1, PLAIN op2) {
  OPENFHE_CONTEXT::Context()->Add(op1, op2, res);
}

void Openfhe_mul_ciph(CIPHER res, CIPHER op1, CIPHER op2) {
  OPENFHE_CONTEXT::Context()->Mul(op1, op2, res);
}

void Openfhe_mul_plain(CIPHER res, CIPHER op1, PLAIN op2) {
  OPENFHE_CONTEXT::Context()->Mul(op1, op2, res);
}

void Openfhe_rotate(CIPHER res, CIPHER op, int step) {
  OPENFHE_CONTEXT::Context()->Rotate(op, step, res);
}

void Openfhe_copy(CIPHER res, CIPHER op) { *res = *op; }

void Openfhe_zero(CIPHER res) { res->reset(); }

// Debug API
void Dump_ciph(CIPHER ct, size_t start, size_t len) {
  std::vector<double> vec;
  OPENFHE_CONTEXT::Context()->Decrypt(ct, vec);
  size_t max = std::min(vec.size(), start + len);
  for (size_t i = start; i < max; ++i) {
    std::cout << vec[i] << " ";
  }
  std::cout << std::endl;
}

void Dump_plain(PLAIN pt, size_t start, size_t len) {
  std::vector<double> vec;
  OPENFHE_CONTEXT::Context()->Decode(pt, vec);
  size_t max = std::min(vec.size(), start + len);
  for (size_t i = start; i < max; ++i) {
    std::cout << vec[i] << " ";
  }
  std::cout << std::endl;
}
