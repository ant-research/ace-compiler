//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "bm_rtlib_ckks.h"

#define REGISTER_BENCHMARK(name) \
  RegisterBenchmark(#name, name)->Unit(::kMicrosecond)

static void Rotate(State& state) {
  CKKS_BENCH&     env  = CKKS_BENCH::Instance();
  CKKS_EVALUATOR* eval = env.Evaluator();

  size_t rot_idx = 1;
  Insert_rot_map(env.Key_gen(), rot_idx);

  CIPHERTEXT* ciph     = env.Ciph1();
  CIPHERTEXT* rot_ciph = Alloc_ciphertext();

  for (auto _ : state) {
    Eval_fast_rotate(rot_ciph, ciph, rot_idx, eval);
  }

  Free_ciphertext(rot_ciph);
}

static void Relin(State& state) {
  CKKS_BENCH& env = CKKS_BENCH::Instance();

  CIPHERTEXT3*    ciph3 = Alloc_ciphertext3();
  CIPHERTEXT*     res   = Alloc_ciphertext();
  CKKS_EVALUATOR* eval  = env.Evaluator();

  // ciph3 = ciph1 * ciph2
  Mul_ciphertext3(ciph3, env.Ciph1(), env.Ciph2(), eval);

  POLYNOMIAL* p0     = Get_ciph3_c0(ciph3);
  POLYNOMIAL* p1     = Get_ciph3_c1(ciph3);
  POLYNOMIAL* p2     = Get_ciph3_c2(ciph3);
  size_t      sf     = Get_ciph3_sfactor(ciph3);
  size_t      sf_deg = Get_ciph3_sf_degree(ciph3);
  size_t      slots  = Get_ciph3_slots(ciph3);

  for (auto _ : state) {
    Relinearize_ciph3(res, p0, p1, p2, sf, sf_deg, slots, eval);
  }

  Free_ciphertext3(ciph3);
  Free_ciphertext(res);
}

static void Rescale(State& state) {
  CKKS_BENCH& env = CKKS_BENCH::Instance();

  CIPHERTEXT*     ciph_mul = Alloc_ciphertext();
  CIPHERTEXT*     res      = Alloc_ciphertext();
  CKKS_EVALUATOR* eval     = env.Evaluator();

  // ciph_mul = ciph1 * ciph2
  Mul_ciphertext(ciph_mul, env.Ciph1(), env.Ciph2(), eval);

  for (auto _ : state) {
    Rescale_ciphertext(res, ciph_mul, eval);
  }

  Free_ciphertext(ciph_mul);
  Free_ciphertext(res);
}

static void Bootstrap_full(State& state) {
  size_t level_after_bts = state.range(0);

  CKKS_BENCH&   env     = CKKS_BENCH::Instance();
  CKKS_BTS_CTX* bts_ctx = env.Bts_ctx();
  CIPHERTEXT*   ciph    = env.Ciph_l1();
  CIPHERTEXT*   res     = Alloc_ciphertext();

  size_t num_slots = env.Degree() / 2;
  env.Eval_bts_precom(num_slots);

  for (auto _ : state) {
    Eval_bootstrap(res, ciph, level_after_bts, bts_ctx);
  }

  Free_ciphertext(res);
}

static void Bootstrap_sparse(State& state) {
  size_t level_after_bts = state.range(0);
  size_t sparse          = state.range(1);
  FMT_ASSERT(sparse > 2, "sparse should larger than 2");

  CKKS_BENCH&   env     = CKKS_BENCH::Instance();
  CKKS_BTS_CTX* bts_ctx = env.Bts_ctx();
  CIPHERTEXT*   ciph    = env.Ciph_l1();
  CIPHERTEXT*   res     = Alloc_ciphertext();

  size_t num_slots = env.Degree() / sparse;
  env.Eval_bts_precom(num_slots);

  for (auto _ : state) {
    Eval_bootstrap(res, ciph, level_after_bts, bts_ctx);
  }

  Free_ciphertext(res);
}

ENV_INFO BM_GLOB_ENV::Env_info[ENV_LAST];

int main(int argc, char** argv) {
  Initialize(&argc, argv);
  BM_GLOB_ENV::Initialize();
  BM_GLOB_ENV::Print_env();
  CKKS_BENCH::Instance().Setup();

  REGISTER_BENCHMARK(Rotate);
  REGISTER_BENCHMARK(Relin);
  REGISTER_BENCHMARK(Rescale);
  REGISTER_BENCHMARK(Bootstrap_full)->ArgName("ret_level")->Arg(0);
  REGISTER_BENCHMARK(Bootstrap_full)->ArgName("ret_level")->Arg(2);
  REGISTER_BENCHMARK(Bootstrap_sparse)
      ->ArgNames({"ret_level", "sparse"})
      ->Args({0, 4});
  REGISTER_BENCHMARK(Bootstrap_sparse)
      ->ArgNames({"ret_level", "sparse"})
      ->Args({2, 4});

  RunSpecifiedBenchmarks();
  CKKS_BENCH::Instance().Teardown();
  return 0;
}