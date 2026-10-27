//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

// F3.1b gap-filler: standalone OpenFHE 1.5.1 EvalBootstrap proof of concept.
//
// Goal: prove the OpenFHE 1.5.1 bootstrap mechanism (EvalBootstrapSetup /
// EvalBootstrapKeyGen / EvalBootstrap / Rescale-while-NoiseScaleDeg>1 /
// LevelReduceInPlace) works on a toy CKKS context BEFORE any backend
// header/impl lands. Mirrors the call sequence the F3.3 backend will use, per
// doc/rtlib-c-bootstrap/openfhe-bootstrap-integration-plan.md section 5.3 and
// the feat backend at rtlib/openfhe/src/openfhe_lib.cxx (OPENFHE_CONTEXT
// constructor + OPENFHE_CONTEXT::Bootstrap).
//
// The source is path-free: only #include <openfhe.h> and standard includes;
// the -I/-L/-l flags live in the manual build command, not in this file. Do
// not wire this into CMake or ctest -- F3.1b's gate is a manual g++ run; the
// maintained unittest/ (F3.7) and example/ (F3.8) dirs land later.

#include <openfhe.h>

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <vector>

using namespace lbcrypto;

// Mirror the F3.3 / feat backend parameters: FIXEDMANUAL scaling,
// UNIFORM_TERNARY secret key, HYBRID keyswitching, bootstrap level_budget
// [4,4] (s2c, c2s). Ring dimension N = 4096 -> slots = 2048 (full packing).
// Scale factor sf = 59 matches FHE_BTS_SF=59 from the feat (resnet20); the
// simple-ckks-bootstrapping example also uses dcrtBits=59 at N=4096 on
// NATIVEINT=64, so this is a known-good precision point for the bootstrap
// approximation. firstMod=60 is the OpenFHE-recommended top tower.
int main() {
  const uint32_t ringDim                       = 4096;
  const uint32_t firstMod                      = 60;
  const uint32_t sf                            = 59;
  const SecretKeyDist secretKeyDist            = UNIFORM_TERNARY;
  const std::vector<uint32_t> levelBudget      = {4, 4};
  const uint32_t levelsAvailableAfterBootstrap = 10;

  const uint32_t btsDepth =
      FHECKKSRNS::GetBootstrapDepth(levelBudget, secretKeyDist);
  const uint32_t depth = levelsAvailableAfterBootstrap + btsDepth;
  const uint32_t numSlots = ringDim / 2;

  std::cout << "F3.1b EvalBootstrap POC\n";
  std::cout << "  ringDim          = " << ringDim << "\n";
  std::cout << "  numSlots         = " << numSlots << "\n";
  std::cout << "  firstMod / sf    = " << firstMod << " / " << sf << "\n";
  std::cout << "  scalingTechnique = FIXEDMANUAL\n";
  std::cout << "  secretKeyDist    = UNIFORM_TERNARY\n";
  std::cout << "  keyswitchTechnique = HYBRID\n";
  std::cout << "  levelBudget      = {4,4}\n";
  std::cout << "  btsDepth         = " << btsDepth << "\n";
  std::cout << "  mulDepth         = " << depth
            << " (levelsAvailableAfter=" << levelsAvailableAfterBootstrap
            << " + btsDepth=" << btsDepth << ")\n";

  CCParams<CryptoContextCKKSRNS> parameters;
  parameters.SetRingDim(ringDim);
  parameters.SetFirstModSize(firstMod);
  parameters.SetScalingModSize(sf);
  parameters.SetScalingTechnique(FIXEDMANUAL);
  parameters.SetKeySwitchTechnique(HYBRID);
  parameters.SetSecretKeyDist(secretKeyDist);
  // HEStd_NotSet matches the rtlib-py toy and the simple-ckks-bootstrapping
  // example; N=4096 is too small for the 128-bit ring-dim table anyway, so
  // NotSet keeps the example self-contained. The real resnet20 backend uses
  // HEStd_128_classic at N=65536.
  parameters.SetSecurityLevel(HEStd_NotSet);
  parameters.SetMultiplicativeDepth(depth);

  CryptoContext<DCRTPoly> cc = GenCryptoContext(parameters);
  cc->Enable(PKE);
  cc->Enable(KEYSWITCH);
  cc->Enable(LEVELEDSHE);
  cc->Enable(ADVANCEDSHE);
  cc->Enable(FHE);

  // dim1 = {0,0} lets OpenFHE pick the baby-step/giant-step split; the third
  // arg is the slot count for full packing. Matches
  // simple-ckks-bootstrapping.cpp.
  cc->EvalBootstrapSetup(levelBudget, {0, 0}, numSlots);

  auto kp = cc->KeyGen();
  cc->EvalMultKeyGen(kp.secretKey);
  cc->EvalBootstrapKeyGen(kp.secretKey, numSlots);

  // Encode a known real-valued message in the first 16 slots; the bootstrap
  // approximation error is roughly proportional to the message magnitude, so
  // keep |x[i]| small (here <= 1.5) -- comfortably within the bootstrap
  // convergence range.
  const size_t encodedLen = 16;
  std::vector<double> x(encodedLen);
  for (size_t i = 0; i < encodedLen; ++i) {
    x[i] = 0.1 * static_cast<double>(i);
  }

  // Encrypt at the top of the modulus chain (depth - 1), matching the feat
  // Prepare_input toy convention and simple-ckks-bootstrapping. The
  // noiseScaleDeg=1 baked in here is the canonical initial-encryption nsd.
  Plaintext ptxt = cc->MakeCKKSPackedPlaintext(x, 1, depth - 1);
  Ciphertext<DCRTPoly> ciph = cc->Encrypt(kp.publicKey, ptxt);

  std::cout << "\npre-bootstrap:  GetLevel=" << ciph->GetLevel()
            << " NoiseScaleDeg=" << ciph->GetNoiseScaleDeg()
            << " (chain depth=" << depth << ")\n";

  // Bootstrap: refresh near the top of the modulus chain. OpenFHE 1.5.1's
  // EvalBootstrap takes (ciphertext, numIterations=1, precision=0); the
  // 1.5.1 signature has no per-call target level.
  Ciphertext<DCRTPoly> out = cc->EvalBootstrap(ciph);

  std::cout << "post-bootstrap:  GetLevel=" << out->GetLevel()
            << " NoiseScaleDeg=" << out->GetNoiseScaleDeg() << "\n";

  // ant-faithful nsd reset: EvalBootstrap on FIXEDMANUAL leaves the real
  // NoiseScaleDeg == 2; force it back to 1 so the emitted
  // rescale-after-every-mul schedule the F3.3 backend ports cannot drift up.
  // Mirrors OPENFHE_CONTEXT::Bootstrap and rtlib-py bootstrap_ciph
  // (while GetNoiseScaleDeg() > 1: Rescale). Under FIXEDMANUAL this is a
  // real tower-dropping Rescale (divides the scale by the next prime), not a
  // FLEXIBLEAUTO auto-renorm.
  while (out->GetNoiseScaleDeg() > 1) {
    out = cc->Rescale(out);
  }

  std::cout << "post-nsd-reset:  GetLevel=" << out->GetLevel()
            << " NoiseScaleDeg=" << out->GetNoiseScaleDeg() << "\n";

  // droplevel to a target level (scale-preserving tower drop). OpenFHE 1.5.1
  // LevelReduceInPlace(ct, nullptr, levels) drops `levels` towers, increasing
  // GetLevel by `levels` and leaving the scale untouched. nullptr evalKey is
  // the no-key-switch overload (matches the feat Mod_switch / Bootstrap
  // droplevel). For the POC, drop 2 towers to clearly exercise the path; the
  // feat's real Bootstrap drops (ir_capacity - level) - post_gl towers to
  // land on the IR target.
  const size_t dropTowers = 2;
  const size_t postGl     = out->GetLevel();
  cc->LevelReduceInPlace(out, nullptr, dropTowers);
  std::cout << "post-LevelReduce(" << dropTowers << "): GetLevel=" << out->GetLevel()
            << " (was " << postGl << ")\n";

  // Decrypt and recover the message.
  Plaintext result;
  cc->Decrypt(kp.secretKey, out, &result);
  result->SetLength(encodedLen);
  const std::vector<double> rec = result->GetRealPackedValue();

  // max_abs over the encoded slots.
  double maxAbs = 0.0;
  for (size_t i = 0; i < encodedLen; ++i) {
    const double d = std::fabs(rec[i] - x[i]);
    if (d > maxAbs) maxAbs = d;
  }

  std::cout << "\nencoded (first 8):  ";
  for (size_t i = 0; i < 8 && i < encodedLen; ++i) std::cout << x[i] << " ";
  std::cout << "...\nrecovered (first 8): ";
  for (size_t i = 0; i < 8 && i < encodedLen; ++i) std::cout << rec[i] << " ";
  std::cout << "...\n";
  std::cout << "max_abs = " << maxAbs << "\n";

  const double tol = 1e-3;
  if (maxAbs < tol) {
    std::cout << "PASS: max_abs " << maxAbs << " < " << tol << "\n";
    return 0;
  }
  std::cout << "FAIL: max_abs " << maxAbs << " >= " << tol << "\n";
  return 1;
}
