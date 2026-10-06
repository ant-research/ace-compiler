//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

// Stage 4 unit test: exercise the OPENFHE_CONTEXT bootstrap branch
// (Need_bts()==true) added in Stage 3.
//
// One gtest; no main() (unittest/test_fhert_openfhe.cxx provides main and
// calls RUN_ALL_TESTS). This TU supplies Need_bts() / Get_context_params() so
// the constructor takes the EvalBootstrap branch and builds a bootstrap
// context.
//
// Flow:
//   1. Prepare_context() builds the bootstrap context (EvalBootstrapSetup,
//      KeyGen, EvalMultKeyGen, EvalBootstrapKeyGen, EvalRotateKeyGen).
//   2. Encrypt a small vector v (len 8, values in [-1,1]); the fresh cipher is
//      at the top of the modulus chain (GetLevel 0).
//   3. Deplete it to near the bottom (GetLevel = depth - 2, 2 levels left) with
//      repeated mul_by_plain(1.0) + rescale (each rescale step is scale-matched
//      by re-encoding the scalar-1 plaintext at the cipher's current level).
//   4. Record pre-bootstrap levelsAvailable (= depth - GetLevel).
//   5. Openfhe_bootstrap(res, op, level) with level = depth (target_gl = 1,
//      lands near the chain top after the nsd-reset rescale).
//   6. Decrypt, compute max_abs vs v.
//   7. ASSERT post levelsAvailable > pre (refresh happened) AND max_abs < tol.
//
// Bootstrap consumes a chain of GetBootstrapDepth({4,4}, UNIFORM_TERNARY) (+ R)
// primes; depth (= fresh_cipher tower count - 1) is measured at runtime from the
// cipher itself rather than reconstructed, so the level-refresh assertions stay
// exact even if the env (FHE_BTS_LEVEL_BUDGET / FHE_BTS_RESERVE / FHE_BTS_SF) is
// changed.

#include <openfhe.h>

#include <algorithm>
#include <cmath>
#include <vector>

#include "gtest/gtest.h"
#include "common/rt_api.h"
#include "common/tensor.h"
#include "rt_openfhe/rt_openfhe.h"

// Force the constructor's bootstrap branch on.
bool Need_bts() { return true; }

// Io_init() (called by Prepare_context) reads these weak symbols; define them
// here so the Io subsystem is set up for the single input "input" and single
// output "output" used by this test.
int Get_input_count() { return 1; }
DATA_SCHEME* Get_encode_scheme(int /*idx*/) {
  static MAP_DESC   desc_0[] = {{NORMAL, 0, 0, 0, 0}};
  static DATA_SCHEME scheme_0 = {"input", {0, 0, 0, 0}, 1, desc_0};
  static DATA_SCHEME* scheme[] = {&scheme_0};
  return scheme[0];
}
int Get_output_count() { return 1; }
DATA_SCHEME* Get_decode_scheme(int /*idx*/) {
  static MAP_DESC   desc[] = {{NORMAL, 0, 0, 0, 0}};
  static DATA_SCHEME scheme = {"output", {0, 0, 0, 0}, 1, desc};
  return &scheme;
}

// Toy CKKS bootstrap config: ring 4096, FIXEDMANUAL, full packing. Each
// value is the toy-context minimum: ring 4096 gives ~22 levels for the
// depth-22 EvalBootstrap budget, and FIXEDMANUAL matches the rt_fhert host
// toy (the choice that exercises the real EvalBootstrap hot path).
CKKS_PARAMS* Get_context_params() {
  static CKKS_PARAMS parm = {
      LIB_OPENFHE,  // _provider
      4096,         // _poly_degree   (ring dim; OpenFHE bootstrap minimum ring)
      0,            // _sec_level     -> HEStd_NotSet (avoid ring auto-raise)
      29,           // _mul_depth     (gives depth = 35 >> bts_depth=22: refresh headroom)
      0,            // _input_level
      60,           // _first_mod_size  (canonical simple-ckks-bootstrapping 64-bit branch: 60/59)
      59,           // _scaling_mod_size (FIXEDMANUAL; == FHE_BTS_SF default)
      0,            // _num_q_parts
      0,            // _hamming_weight  -> c_bts_d = 19 in the ctor sizing
      0,            // _num_rot_idx
      {}            // _rot_idxs
  };
  return &parm;
}

static double MaxAbs(const std::vector<double>& got,
                     const std::vector<double>& want) {
  size_t n   = std::min(got.size(), want.size());
  double m   = 0.0;
  for (size_t i = 0; i < n; ++i) m = std::max(m, std::fabs(got[i] - want[i]));
  return m;
}

TEST(OpenfheBootstrap, Refresh) {
  Prepare_context();

  // small vector v, values in [-1,1]; OpenFHE zero-pads to full packing.
  std::vector<double> v = {0.25, 0.5, 0.75, 1.0, -0.5, -0.25, -0.75, -1.0};
  const size_t       N = v.size();

  // encrypt v (fresh at top, GetLevel 0)
  std::vector<double> in(v);
  TENSOR*             t = Alloc_tensor(1, 1, 2, 4, in.data());
  Prepare_input(t, "input");
  Free_tensor(t);

  CIPHERTEXT c = Get_input_data("input", 0);

  // depth = fresh cipher tower count - 1 == _num_primes - 1
  size_t fresh_towers = c->GetElements()[0].GetNumOfElements();
  size_t depth        = fresh_towers - 1;
  ASSERT_GE(depth, 4u) << "chain too shallow for bootstrap toy test";

  // deplete to GetLevel = depth - 2 (leave 2 levels). Each step multiplies by a
  // full-slots plaintext of all-1.0 (so every slot is preserved, not just slot
  // 0), encoded at the cipher's current level (scale-matched), then rescaled.
  // The value is preserved (x1 in every slot) while one modulus level is
  // consumed.
  size_t           num_slots = Degree() / 2;
  std::vector<float> ones(num_slots, 1.0f);
  while (Level(&c) < depth - 2) {
    uint32_t  lvl = static_cast<uint32_t>(Level(&c));
    PLAINTEXT pt_one;
    Encode_float(&pt_one, ones.data(), num_slots, /*sc_degree=*/1,
                 /*level=*/lvl);
    CIPHERTEXT tmp;
    Mul_plain(&tmp, &c, &pt_one);
    Rescale_ciph(&c, &tmp);
  }

  uint64_t pre_level = Level(&c);
  size_t   pre_avail = (depth)-pre_level;  // levelsAvailable = depth - GetLevel

  // bootstrap to a target near the top: IR level = depth -> target_gl =
  // _num_primes - level = (depth+1) - depth = 1. After EvalBootstrap the cipher
  // lands at the top; the nsd-reset rescale drops one tower (GetLevel 1), and the
  // droplevel (target_gl == post_gl == 1) is a no-op.
  int       target_level = static_cast<int>(depth);
  CIPHERTEXT res;
  Bootstrap(&res, &c, target_level);

  uint64_t post_level = Level(&res);
  size_t   post_avail = (depth)-post_level;

  // decrypt first N slots
  double*             msg = Get_msg(&res);
  std::vector<double> got(msg, msg + N);
  free(msg);

  double max_abs = MaxAbs(got, v);
  long   delta   = static_cast<long>(post_avail) - static_cast<long>(pre_avail);

  printf("[OpenfheBootstrap] depth=%zu (fresh_towers=%zu)\n", depth,
         fresh_towers);
  printf("[OpenfheBootstrap] pre  GetLevel=%lu  levelsAvailable=%zu\n",
         static_cast<unsigned long>(pre_level), pre_avail);
  printf("[OpenfheBootstrap] post GetLevel=%lu  levelsAvailable=%zu\n",
         static_cast<unsigned long>(post_level), post_avail);
  printf("[OpenfheBootstrap] level-refresh delta (post-pre)=%ld\n", delta);
  printf("[OpenfheBootstrap] max_abs=%.3g\n", max_abs);

  EXPECT_GT(post_avail, pre_avail) << "bootstrap did not refresh levels";
  // strict 1e-3; relax to 1e-2 only if toy FIXEDMANUAL precision requires it.
  EXPECT_LT(max_abs, 1e-3) << "bootstrap accuracy too low";

  Finalize_context();
}
