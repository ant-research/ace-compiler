//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <openfhe.h>

#include <cstdlib>

#include "common/common.h"
#include "common/error.h"
#include "common/io_api.h"
#include "common/rt_api.h"
#include "common/rtlib_timing.h"
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
    CKKS_PARAMS* pp = Get_context_params();
    // Encrypt the input at the IR-mapped OpenFHE GetLevel
    // (_bts_ir_capacity - input_level), matching the rt_fhert/rt_openfhe
    // input-seam convention (Cipher([values], input_level, 1, False) encrypts at
    // _of_level(input_level)). The codegen emits plains and Bootstrap args as
    // IR-REMAINING literals; the consuming cipher must START the first segment
    // at the IR-mapped level so the first bootstrap is invoked at IR 1 (the
    // deepest point, like rt_openfhe -- which reaches acc=1.0 on resnet20 here)
    // rather than at IR 19 (the GL0-start consequence). Guarded so the
    // unaffected cases
    // keep the chain-top (GL 0) behavior: the 3a plain branch
    // (_bts_ir_capacity == 0, set only in the bootstrap branch) and the
    // bootstrap toy (input_level == 0) stay at GL 0 -- the toy depletes from the
    // chain top and its depth-2 underflow guard would break at the chain bottom.
    // of_level is computed once here and REUSED by every shard below so all
    // shards share the same seam-correct level; the sharding itself (below)
    // does not recompute the seam.
    uint32_t of_level = 0;
    if (_bts_ir_capacity > 0 && (size_t)pp->_input_level > 0 &&
        _bts_ir_capacity > (size_t)pp->_input_level) {
      of_level = (uint32_t)(_bts_ir_capacity - (size_t)pp->_input_level);
    }

    // Mirror ant Prepare_input (rtlib/ant/ckks/src/rtlib.c): find the
    // compiler-generated DATA_SCHEME named `name`, then encode ONE ciphertext
    // per scheme shard per its MAP_DESC. The single-shard cases carry one
    // {NORMAL,0,0,0,0} desc covering the whole image, so the loop degenerates
    // to one shard of the full tensor (identical to the prior one-ciphertext
    // path -- NO regression). The sharded cases (e.g. resnet18-conv2d,
    // k=4 shards whose image-data count is 3136 each, plus per-shard channel-pad
    // slots) carry one desc per shard; building each
    // shard separately keeps every ciphertext within the slot count
    // (ringDim/2), where encoding the WHOLE 12544-value tensor in one
    // ciphertext exceeds it (MakeCKKSPackedPlaintextInternal size > ringDim/2).
    DATA_SCHEME* scheme = nullptr;
    for (int i = 0; i < Get_input_count(); ++i) {
      DATA_SCHEME* ptr = Get_encode_scheme(i);
      IS_TRUE(ptr != nullptr, "invalid encode scheme");
      if (strcmp(ptr->_name, name) == 0) {
        scheme = ptr;
        break;
      }
    }
    IS_TRUE(scheme != nullptr, "not find the input");
    IS_TRUE(scheme->_count > 0, "no desc found in scheme");

    size_t hw = TENSOR_H(input) * TENSOR_W(input);
    for (int j = 0; j < scheme->_count; ++j) {
      const MAP_DESC*     desc = &scheme->_desc[j];
      std::vector<double> shard_vec;
      if (desc->_count == 0) {
        // NORMAL window: encode the whole image (ant Pre_encode_scheme
        // count==0 branch). Fill in NCHW order; the linear index matches
        // input->_vals layout exactly, so this equals the prior
        // one-ciphertext vec(input->_vals, input->_vals + len).
        size_t len = TENSOR_SIZE(input);
        shard_vec.resize(len);
        size_t idx = 0;
        FOR_ALL_TENSOR_ELEM(input, n, c, h, w) {
          shard_vec[idx++] = TENSOR_ELEM(input, n, c, h, w);
        }
      } else {
        // NORMAL window with start/count/stride/pad: build the shard
        // [pad | count stride-1 values | pad], mirroring ant
        // Pre_encode_scheme exactly (start..start+count*stride, channel-padded
        // at begin/end so a shard that starts/ends mid-channel repeats the
        // neighbor pixel; a shard at a channel boundary pads with 0).
        IS_TRUE(desc->_kind == NORMAL, "TODO: other kind");
        IS_TRUE(desc->_stride == 1, "TODO: other stride");
        IS_TRUE((size_t)desc->_count < TENSOR_SIZE(input) &&
                    (size_t)desc->_start +
                            (size_t)desc->_count * (size_t)desc->_stride <=
                        TENSOR_SIZE(input),
                "chunk out of image bound");
        size_t loc          = (size_t)desc->_start;
        size_t shard_len    = (size_t)desc->_count + 2 * (size_t)desc->_pad;
        bool   begin_channel = (loc % hw) == 0;
        bool   end_channel =
            ((loc + (size_t)desc->_count * (size_t)desc->_stride) % hw) == 0;
        shard_vec.resize(shard_len);
        // pad before image data
        for (int i = 0; i < desc->_pad; ++i) {
          shard_vec[i] = begin_channel
                             ? 0.0
                             : TENSOR_VAL(input, loc -
                                                  (size_t)(desc->_pad - i) *
                                                      (size_t)desc->_stride);
        }
        // image data
        for (int i = 0; i < desc->_count; ++i) {
          shard_vec[i + desc->_pad] = TENSOR_VAL(input, loc);
          loc += (size_t)desc->_stride;
        }
        // pad after image data
        for (int i = 0; i < desc->_pad; ++i) {
          shard_vec[i + desc->_pad + desc->_count] =
              end_channel ? 0.0
                          : TENSOR_VAL(input, loc +
                                                  (size_t)i *
                                                      (size_t)desc->_stride);
        }
      }
      Plaintext   pt = _ctx->MakeCKKSPackedPlaintext(shard_vec, 1, of_level);
      Ciphertext* ct = new Ciphertext;
      *ct            = _ctx->Encrypt(_kp.publicKey, pt);
      Io_set_input(name, j, ct);
    }
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
    std::vector<double> vec;
    vec         = pt->GetRealPackedValue();
    double* msg = (double*)malloc(sizeof(double) * vec.size());
    memcpy(msg, vec.data(), sizeof(double) * vec.size());
    return msg;
  }

  void Encode_float(Plaintext* pt, float* input, size_t len, uint32_t sc_degree,
                    uint32_t level) {
    std::vector<double> vec(input, input + len);
    *pt = _ctx->MakeCKKSPackedPlaintext(vec, sc_degree, level);
  }

  // Encode a windowed "mask" plaintext: the first `len` slots hold `cst`, the
  // rest 0, at `level` with noiseScaleDeg = sc_degree. Mirrors ant
  // Encode_float_mask (mask[len]=cst; Encode_float(plain, mask, len, sc_degree,
  // level)). The SIHE Chebyshev-relu value-range control uses it as a windowed
  // constant -- the graph calls
  //   Encode_float_mask(&plain, cst, mask_len, sc_degree, Level(&ciph)).
  // MakeCKKSPackedPlaintext zero-pads `vec` to the ring dimension, so slots
  // [len, ringSlots) are 0 (the mask window), matching ant exactly.
  void Encode_float_mask(Plaintext* pt, float cst, size_t len, uint32_t sc_degree,
                         uint32_t level) {
    std::vector<double> vec(len, (double)cst);
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
  void Add(const Ciphertext* op1, const Ciphertext* op2, Ciphertext* res) {
    if (res == op1) {
      _ctx->EvalAddInPlace(*res, *op2);
    } else if (res == op2) {
      _ctx->EvalAddInPlace(*res, *op1);
    } else {
      *res = _ctx->EvalAdd(*op1, *op2);
    }
  }

  void Add(const Ciphertext* op1, const Plaintext* op2, Ciphertext* res) {
    Plaintext op2_copy = _plain_at_cipher_level(*op2, (*op1)->GetLevel());
    if (res == op1) {
      _ctx->EvalAddInPlace(op2_copy, *res);
    } else {
      *res = _ctx->EvalAdd(*op1, op2_copy);
    }
  }

  void Mul(const Ciphertext* op1, const Ciphertext* op2, Ciphertext* res) {
    *res = _ctx->EvalMultNoRelin(*op1, *op2);
  }

  void Mul(const Ciphertext* op1, const Plaintext* op2, Ciphertext* res) {
    *res = _ctx->EvalMult(*op1, _plain_at_cipher_level(*op2, (*op1)->GetLevel()));
  }

  // Re-encode `plain` at the consuming cipher's OpenFHE GetLevel, preserving
  // the plain's noiseScaleDeg (sc_degree from Encode_float). Mirrors
  // rt_fhert/rt_openfhe `_plain_pt` (rt_fhert/__init__.py:851-858) /
  // `_plain_for_op` (:861-870): the codegen emits the Encode_float level as an
  // IR-REMAINING literal (descending 7,6,5,... from input_level), which the
  // raw MakeCKKSPackedPlaintext(vec, sc_degree, level) interprets as the
  // OpenFHE consumed GetLevel -- so every literal-level plain (resnet20's 80
  // lowered-conv/BN Encode_float sites) lands at the wrong tower and
  // scale-drifts Mul_plain/Add_plain. The Python path re-encodes at the
  // cipher's runtime GetLevel (_of_level(cipher.level)); this helper does the
  // same: decode the plain's values (GetRealPackedValue -- no secret key, a
  // plaintext is not encrypted) and re-encode at the cipher's current
  // GetLevel with the plain's original noiseScaleDeg. The codegen's baked
  // sc_degree already carries the right nsd for the op (1 for mul_plain, the
  // cipher's nsd for add_plain), so only the LEVEL needs fixing. This also
  // covers Encode_float_mask plains (the SIHE Chebyshev-relu windowed
  // constants), whose values round-trip through GetRealPackedValue exactly.
  Plaintext _plain_at_cipher_level(const Plaintext& plain,
                                   size_t cipher_getlevel) {
    std::vector<double> vals = plain->GetRealPackedValue();
    return _ctx->MakeCKKSPackedPlaintext(
        vals, plain->GetNoiseScaleDeg(), (uint32_t)cipher_getlevel);
  }

  void Mul(const Ciphertext* op1, double scalar, Ciphertext* res) {
    // Plain scalar multiply, no rescale (graph's explicit Rescale_ciph follows).
    *res = _ctx->EvalMult(*op1, scalar);
  }

  void Add(const Ciphertext* op1, double scalar, Ciphertext* res) {
    // Plain scalar add, no rescale (mirrors the Mul scalar above). EvalAdd(
    // cipher, double) encodes the scalar at the cipher's current scale/level and
    // adds it to every slot; res = op1 + scalar. The SIHE Chebyshev-relu emits
    // Add_scalar(&res, &op, -1) for the low-bound shift.
    if (res == op1) {
      _ctx->EvalAddInPlace(*res, scalar);
    } else {
      *res = _ctx->EvalAdd(*op1, scalar);
    }
  }

  void Rotate(const Ciphertext* op1, int step, Ciphertext* res) {
    *res = _ctx->EvalRotate(*op1, step);
  }

  void Rescale(const Ciphertext* op1, Ciphertext* res) {
    if (op1 == res) {
      _ctx->RescaleInPlace(*res);
    } else {
      *res = _ctx->Rescale(*op1);
    }
  }

  void Mod_switch(const Ciphertext* op1, Ciphertext* res) {
    if (op1 == res) {
      _ctx->LevelReduceInPlace(*res, nullptr);
    } else {
      *res = _ctx->LevelReduce(*op1, nullptr);
    }
  }

  void Relin(const Ciphertext* op1, Ciphertext* res) {
    if (op1 == res) {
      _ctx->RelinearizeInPlace(*res);
    } else {
      *res = _ctx->Relinearize(*op1);
    }
  }

  //! @brief Bootstrap a cipher (port of rtlib-py bootstrap_ciph): refresh near
  //! the top of the modulus chain via EvalBootstrap, run the ant-faithful nsd
  //! reset (rescale until noiseScaleDeg == 1), then droplevel to the IR target
  //! level (scale-preserving tower drop) when the target is at or below the
  //! post-reset position. `level` is the IR remaining level the bootstrapped
  //! cipher must land at; `target_gl = _bts_ir_capacity - level` is the openfhe
  //! consumed GetLevel to reach, leaving the R reserve towers below the
  //! landing for EvalBootstrap down-headroom (the rtlib-py landing contract).
  void Bootstrap(const Ciphertext* op, Ciphertext* res, int level) {
    IS_TRUE(_bootstrap_ready, "Openfhe_bootstrap: no bootstrap context");

    Ciphertext out = _ctx->EvalBootstrap(*op);

    // ant-faithful nsd reset: EvalBootstrap leaves real noiseScaleDeg == 2;
    // force it back to 1 so the emitted rescale-after-every-mul schedule cannot
    // drift up (mirrors rtlib-py's while GetNoiseScaleDeg() > 1: Rescale).
    while (out->GetNoiseScaleDeg() > 1) {
      out = _ctx->Rescale(out);
    }

    // droplevel to the IR target (scale-preserving tower drop). OpenFHE 1.5.1
    // LevelReduceInPlace(ct, nullptr, levels) drops `levels` towers; for
    // levels == 1 this matches the existing Mod_switch call exactly. The target
    // uses _bts_ir_capacity (the chain WITHOUT the R reserve), so
    // target_gl = ir_capacity - level leaves the R reserve towers below the
    // landing. That gives EvalBootstrap R towers of down-headroom for a precise
    // refresh -- the next bootstrap is invoked at remaining R+1, not at the
    // chain bottom, so it refreshes without starvation (no DropLastElement
    // crash). This mirrors the rtlib-py/rt_openfhe landing contract: the +R
    // reserve sits below IR 0. Guard level >= ir_capacity (the Stage-4 toy
    // bootstraps at level = depth = ir_capacity + R - 1 > ir_capacity): clamp
    // target_gl to 0 (no droplevel), which is correct for the toy (it only
    // asserts refresh + max_abs, both of which hold with the post-bootstrap
    // position intact).
    size_t post_gl = out->GetLevel();
    size_t target_gl =
        (_bts_ir_capacity > (size_t)level) ? (_bts_ir_capacity - (size_t)level) : 0;
    if (target_gl >= post_gl) {
      size_t drop = target_gl - post_gl;
      _ctx->LevelReduceInPlace(out, nullptr, drop);
    }
    *res = out;
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
  bool          _bootstrap_ready = false;
  size_t        _num_primes      = 0;
  // Bootstrap reserve + IR capacity (the rtpy landing contract). ir_capacity
  // is the modulus chain WITHOUT the R reserve (max(input_level,
  // max_target + bts_depth + 1)); num_primes = ir_capacity + R. The R towers
  // sit below the IR-0 working zone to give EvalBootstrap down-headroom -- the
  // bootstrap is invoked at remaining R+1 (not the chain bottom), so it never
  // DropLastElements and refreshes with R towers of precision. The droplevel
  // target uses ir_capacity (not num_primes) so the landing leaves the R
  // towers below it.
  size_t        _bts_ir_capacity = 0;
  size_t        _bts_R           = 0;
};

OPENFHE_CONTEXT* OPENFHE_CONTEXT::Instance = nullptr;

OPENFHE_CONTEXT::OPENFHE_CONTEXT() {
  IS_TRUE(Instance == nullptr, "_install already created");

  CKKS_PARAMS* prog_param = Get_context_params();
  IS_TRUE(prog_param->_provider == LIB_OPENFHE, "provider is not OPENFHE");

  printf(
      "ckks_param: _provider = %d, _poly_degree = %d, _sec_level = %ld, "
      "mul_depth = %ld, input_level = %ld, _first_mod_size = %ld, "
      "_scaling_mod_size = %ld, _num_q_parts = %ld, _num_rot_idx = %ld\n",
      prog_param->_provider, prog_param->_poly_degree, prog_param->_sec_level,
      prog_param->_mul_depth, prog_param->_input_level,
      prog_param->_first_mod_size, prog_param->_scaling_mod_size,
      prog_param->_num_q_parts, prog_param->_num_rot_idx);

  uint32_t                degree = prog_param->_poly_degree;
  lbcrypto::SecurityLevel sec    = lbcrypto::SecurityLevel::HEStd_128_classic;
  switch (prog_param->_sec_level) {
    case 128:
      sec = lbcrypto::SecurityLevel::HEStd_128_classic;
      break;
    case 192:
      sec = lbcrypto::SecurityLevel::HEStd_192_classic;
      break;
    case 256:
      sec = lbcrypto::SecurityLevel::HEStd_256_classic;
      break;
    default:
      sec = lbcrypto::SecurityLevel::HEStd_NotSet;
      break;
  }
  if (degree < 4096 && sec != lbcrypto::SecurityLevel::HEStd_NotSet) {
    DEV_WARN("WARNING: degree %d too small, reset security level to none\n",
             degree);
    sec = lbcrypto::SecurityLevel::HEStd_NotSet;
  }
  std::vector<int32_t> steps(prog_param->_rot_idxs,
                             prog_param->_rot_idxs + prog_param->_num_rot_idx);

  lbcrypto::CCParams<lbcrypto::CryptoContextCKKSRNS> parms;
  parms.SetRingDim(degree);
  parms.SetFirstModSize(prog_param->_first_mod_size);
  parms.SetSecurityLevel(sec);

  if (!Need_bts()) {
    // 3a plain levelled-SHE context. fhe-cmplr emits input_level = mul_depth+1
    // (the top-of-chain level the input ciphertext / encoded plaintexts sit at),
    // and a plain graph can rescale a cipher downward up to _mul_depth times
    // below that level, so the deepest cipher level the runtime reaches is at
    // most input_level + _mul_depth. ant's encode path tolerates an operating
    // level above its chain depth; OpenFHE's strict SetLevel rejects it
    // (ciphertext-impl.cpp:50 "multiplicative depth insufficient" -- the
    // plain-branch depth mismatch the plan anticipated: e.g. i64_o10 needs
    // depth 3, the lenet/resnet pool/conv cases need depth 7, both <=
    // _mul_depth + _input_level). Size the chain to that upper bound. This only
    // adds towers vs _mul_depth (never removes); for elementwise graphs
    // (_input_level==0) it is exactly _mul_depth (no regression), and the
    // hand-written plain examples (input_level == mul_depth+1) only gain
    // headroom and still print SUCCESS.
    // Scaling technique: set FIXEDMANUAL explicitly (mirrors the bootstrap
    // branch below and rtlib-py/rt_openfhe decision #2). constants-defs.h
    // defines ScalingTechnique{FIXEDMANUAL=0, FIXEDAUTO, FLEXIBLEAUTO,
    // FLEXIBLEAUTOEXT}, so the platform default is already FIXEDMANUAL -- this
    // call is a defensive no-op today that pins the manual-rescale semantics
    // the generated graph expects (Mul_plain -> explicit Rescale_ciph) and
    // guards against a future default change.
    parms.SetScalingTechnique(lbcrypto::FIXEDMANUAL);
    parms.SetScalingModSize(prog_param->_scaling_mod_size);
    size_t plain_depth =
        (size_t)prog_param->_mul_depth + (size_t)prog_param->_input_level;
    parms.SetMultiplicativeDepth(plain_depth);
    _ctx = lbcrypto::GenCryptoContext(parms);
    _ctx->Enable(lbcrypto::PKE);
    _ctx->Enable(lbcrypto::KEYSWITCH);
    _ctx->Enable(lbcrypto::LEVELEDSHE);

    _kp = _ctx->KeyGen();
    _ctx->EvalMultKeyGen(_kp.secretKey);
    _ctx->EvalRotateKeyGen(_kp.secretKey, steps);
    _bootstrap_ready = false;
    // num_primes = built multiplicative depth + 1. The plain branch is
    // FIXEDMANUAL (the defensive SetScalingTechnique above), and plain_depth
    // (which covers the emitted input level) is the built chain depth, so track
    // it for the level/droplevel bookkeeping shared with the bts branch.
    _num_primes = plain_depth + 1;
    return;
  }

  // 3b bootstrap context (port of rtlib-py _build_bootstrap_context).
  lbcrypto::SecretKeyDist secret_key_dist = lbcrypto::UNIFORM_TERNARY;
  parms.SetSecretKeyDist(secret_key_dist);
  // FIXEDMANUAL scaling (mirrors the rtlib-py/rt_openfhe base the C backend
  // ports from; both branches here are FIXEDMANUAL). The codegen's explicit
  // Rescale_ciph drives tower drops, so FIXEDMANUAL scale tracking
  // (Mod_switch preserves scale; Rescale divides) matches the emitted
  // rescale-after-every-mul schedule. FHE_BTS_RESERVE (default 2) sets the R
  // down-headroom reserve below IR 0 that EvalBootstrap refreshes into; a
  // larger R is wasteful (rt_fhert/rt_openfhe default R=2; the non-StCFirst
  // EvalBootstrap path only needs >=2 towers below IR 0). resnet20 should run
  // with FHE_BTS_RESERVE UNSET (R=2), not R=8.
  // FIXEDMANUAL is correct for resnet20 (rt_fhert host passes under all three
  // scaling techniques). The prior C-path garbage under FIXEDMANUAL was the
  // encode/plain level convention (Mul_plain/Add_plain used the codegen's
  // IR-remaining literal as the OpenFHE GetLevel instead of re-encoding at the
  // consuming cipher's runtime GetLevel), fixed in _plain_at_cipher_level.
  // No scaling-technique change is required.
  parms.SetScalingTechnique(lbcrypto::FIXEDMANUAL);
  parms.SetKeySwitchTechnique(lbcrypto::HYBRID);

  // scaling-modulus bit size; FHE_BTS_SF overrides (resnet20 sets 59 to lift
  // FIXEDMANUAL bootstrap refresh precision). Default = the emitted value.
  uint32_t sf = (uint32_t)prog_param->_scaling_mod_size;
  if (const char* e = std::getenv("FHE_BTS_SF")) {
    int v = std::atoi(e);
    if (v > 0) sf = (uint32_t)v;
  }
  parms.SetScalingModSize(sf);

  // level_budget [s2c, c2s]; FHE_BTS_LEVEL_BUDGET overrides (default "4,4").
  std::vector<uint32_t> level_budget = {4, 4};
  if (const char* lb = std::getenv("FHE_BTS_LEVEL_BUDGET")) {
    unsigned a = 4, b = 4;
    if (std::sscanf(lb, "%u,%u", &a, &b) == 2) level_budget = {a, b};
  }

  uint32_t bts_depth =
      lbcrypto::FHECKKSRNS::GetBootstrapDepth(level_budget, secret_key_dist);

  // minimal modulus-chain sizing (not levels_after = mul_depth): the chain only
  // needs to fit the input (IR input_level) and the deepest bootstrap *target*
  // reachable from EvalBootstrap's post-output landing by a downward droplevel.
  uint32_t c_bts_d =
      (prog_param->_hamming_weight > 0 && prog_param->_hamming_weight <= 192)
          ? 15
          : 19;
  uint32_t max_target = (uint32_t)std::max<long>(
      0L, (long)prog_param->_mul_depth + 1 - (long)c_bts_d);
  uint32_t in_lev     = (uint32_t)prog_param->_input_level;
  uint32_t ir_capacity = std::max(in_lev, max_target + bts_depth + 1);
  uint32_t levels_after = ir_capacity - bts_depth - 1;

  // R = down-headroom reserve below IR 0 (2-tower floor for the non-StCFirst
  // bootstrap path); FHE_BTS_RESERVE overrides (default 2).
  uint32_t R = 2;
  if (const char* e = std::getenv("FHE_BTS_RESERVE")) {
    int v = std::atoi(e);
    if (v > 0) R = (uint32_t)v;
  }
  R = std::max<uint32_t>(R, 2u);

  uint32_t depth = levels_after + bts_depth + R;
  parms.SetMultiplicativeDepth(depth);
  _ctx = lbcrypto::GenCryptoContext(parms);
  _ctx->Enable(lbcrypto::PKE);
  _ctx->Enable(lbcrypto::KEYSWITCH);
  _ctx->Enable(lbcrypto::LEVELEDSHE);
  _ctx->Enable(lbcrypto::ADVANCEDSHE);
  _ctx->Enable(lbcrypto::FHE);

  size_t num_slots = _ctx->GetRingDimension() / 2;
  _ctx->EvalBootstrapSetup(level_budget);

  _kp = _ctx->KeyGen();
  _ctx->EvalMultKeyGen(_kp.secretKey);
  _ctx->EvalBootstrapKeyGen(_kp.secretKey, (uint32_t)num_slots);
  _ctx->EvalRotateKeyGen(_kp.secretKey, steps);
  _bootstrap_ready = true;
  // num_primes = built multiplicative depth + 1. For FIXEDMANUAL the built
  // chain depth equals the set depth exactly (no augmentation), so track the
  // set value (== GetCryptoParameters()->GetMultiplicativeDepth() after cast).
  _num_primes = (size_t)depth + 1;
  // Record the rtpy landing contract (see _bts_ir_capacity/_bts_R members):
  // num_primes = ir_capacity + R, and the droplevel target uses ir_capacity so
  // the R reserve towers stay below the landing for EvalBootstrap headroom.
  _bts_ir_capacity = (size_t)ir_capacity;
  _bts_R           = (size_t)R;
}

OPENFHE_CONTEXT::~OPENFHE_CONTEXT() {}

// Vendor-specific RT API
void Prepare_context() {
  Init_rtlib_timing();
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

void Prepare_input_dup(TENSOR* input, const char* name) {
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

void Openfhe_encode_float(PLAIN pt, float* input, size_t len,
                          uint32_t sc_degree, uint32_t level) {
  OPENFHE_CONTEXT::Context()->Encode_float(pt, input, len, sc_degree, level);
}

void Openfhe_encode_float_mask(PLAIN pt, float cst, size_t len, uint32_t sc_degree,
                               uint32_t level) {
  OPENFHE_CONTEXT::Context()->Encode_float_mask(pt, cst, len, sc_degree, level);
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

void Openfhe_mul_scalar(CIPHER res, CIPHER op1, double scalar) {
  // Multiply every slot by a plain double scalar WITHOUT rescaling (the graph
  // emits Rescale_ciph afterward; matches ant/seal/ace/phantom Mul_scalar and
  // GlobalAveragePool's 1/N factor). OpenFHE EvalMult(cipher, double) multiplies
  // each slot by the scalar; under FIXEDMANUAL it leaves the noiseScaleDeg step
  // for the caller's explicit Rescale_ciph.
  OPENFHE_CONTEXT::Context()->Mul(op1, scalar, res);
}

void Openfhe_add_scalar(CIPHER res, CIPHER op1, double scalar) {
  OPENFHE_CONTEXT::Context()->Add(op1, scalar, res);
}

void Openfhe_rotate(CIPHER res, CIPHER op, int step) {
  OPENFHE_CONTEXT::Context()->Rotate(op, step, res);
}

void Openfhe_rescale(CIPHER res, CIPHER op) {
  OPENFHE_CONTEXT::Context()->Rescale(op, res);
}

void Openfhe_mod_switch(CIPHER res, CIPHER op) {
  OPENFHE_CONTEXT::Context()->Mod_switch(op, res);
}

void Openfhe_relin(CIPHER res, CIPHER3 op) {
  OPENFHE_CONTEXT::Context()->Relin(op, res);
}

void Openfhe_bootstrap(CIPHER res, CIPHER op, int level) {
  OPENFHE_CONTEXT::Context()->Bootstrap(op, res, level);
}

// Openfhe_copy is a DEEP copy (Clone), not a shared_ptr copy. The generated
// graph's reduce/accumulate pattern
//   Copy_ciph(&dst, &src); Add_ciph(&dst, &dst, &other);   // dst res+op1
// drives Add -> EvalAddInPlace(*dst, *other), which mutates *dst IN PLACE. A
// shallow `*res = *op` would alias dst with src (both share one CiphertextImpl),
// so the in-place add would silently corrupt src while src is still read by
// later rotations -- the op/gemm BSGS reduce (Copy_ciph(&reduce, &gemm); sum 17
// rotate(gemm) into reduce) returned 11x absolute error this way; ant's Copy is
// deep and was correct. Clone() allocates an independent CiphertextImpl, so dst
// and src decouple and the in-place add touches only dst. (rtlib-py/rt_openfhe
// likewise deep-copies via _ct.Clone().)
void Openfhe_copy(CIPHER res, CIPHER op) {
  if (op->get() == nullptr) { res->reset(); return; }
  *res = (*op)->Clone();
}

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

void Dump_cipher_msg(const char* name, CIPHER ct, uint32_t len) {
  std::cout << "[" << name << "]: ";
  Dump_ciph(ct, 0, len);
}

void Dump_plain_msg(const char* name, PLAIN pt, uint32_t len) {
  std::cout << "[" << name << "]: ";
  Dump_plain(pt, 0, len);
}

double* Get_msg(CIPHER ct) {
  std::vector<double> vec;
  OPENFHE_CONTEXT::Context()->Decrypt(ct, vec);
  double* msg = (double*)malloc(sizeof(double) * vec.size());
  memcpy(msg, vec.data(), sizeof(double) * vec.size());
  return msg;
}

double* Get_msg_from_plain(PLAIN pt) {
  std::vector<double> vec;
  OPENFHE_CONTEXT::Context()->Decode(pt, vec);
  double* msg = (double*)malloc(sizeof(double) * vec.size());
  memcpy(msg, vec.data(), sizeof(double) * vec.size());
  return msg;
}

bool Within_value_range(CIPHER ciph, double* msg, uint32_t len) {
  FMT_ASSERT(false, "TODO: not implemented in openfhe");
}
