// fhe_ops.h — SEAL Evaluator → rt_iie.h-aligned interface declarations
//
// This is the L1 interface layer that FHE-MP-CNN's infer_seal.cpp /
// cnn_seal.cpp / Bootstrapper.cpp / ModularReducer.cpp call instead of
// direct seal::Evaluator methods.
//
// Signatures are aligned to rt_iie.h's high-level wrapper API
// (Add_ciph / Sub_ciph / Mul_ciph / Relin / Rescale_ciph / Rotate_ciph /
// Conjugate_ciph / Mul_plain / Add_plain / Modswitch_ciph / Bootstrap_ciph /
// Add_scalar / ...). See plans/fpga-adaptation/{PLAN.md, FPGA-INTEGRATION.md, rt_iie.h}.
//
// Two backends share ONE signature set; types and the backend handle are
// macro-conditional. Param NAMES and ARITY are identical across backends so
// every call site compiles on both with no source change.
//
//   CPU  (verification) : CIPHER = seal::Ciphertext, PLAIN = seal::Plaintext,
//                         EVAL   = seal::Evaluator&, KSW = seal::RelinKeys&,
//                         GAL    = seal::GaloisKeys&, TGT = seal::parms_id_type
//                         implementations forward to seal::Evaluator (fhe_ops.cpp).
//   FPGA  (future)       : CIPHER/PLAIN = FPGA handles (cipher2_api.hh),
//                         EVAL = BackendCtx&, KSW/GAL/TGT = opaque-ignored,
//                         implementations call rt_iie.h wrappers (fhe_ops.cpp FPGA).
//
// Select backend at CMake time via target_compile_definitions(cnn PRIVATE
// FHE_BACKEND_CPU) (default) or FHE_BACKEND_FPGA.
#pragma once

#include <vector>
#include <complex>
#include <seal/seal.h>

namespace fhe {

// ============================================================
// Backend-conditional typedefs (types only differ; arity identical)
// ============================================================
#ifdef FHE_BACKEND_CPU

using CIPHER  = seal::Ciphertext;
using PLAIN   = seal::Plaintext;
using CIPHER3 = seal::Ciphertext;   // SEAL multiply yields a 3-poly ciphertext
using EVAL    = seal::Evaluator&;   // first-arg backend handle
using KSW     = const seal::RelinKeys&;
using GAL     = const seal::GaloisKeys&;
using TGT     = const seal::parms_id_type&;
using CTX     = const seal::SEALContext&;

#elif defined(FHE_BACKEND_FPGA)

// Forward-declared FPGA types (resolved when the FPGA SDK is wired up).
struct BackendCtx;            // thin wrapper over the global FPGA `context`
struct RelinKeysIgnored {};   // FPGA uses global context.relin_evk
struct GaloisKeyIgnored {};   // FPGA uses global automorphism key
struct ModswitchTarget {};    // resolved by fhe_backend to (ms_level, sf_degree)

// `CIPHER`/`PLAIN`/`CIPHER3` are FPGA opaque handles from cipher2_api.hh /
// cipher_op.hh. Concrete when fhe_ops.cpp FPGA section is implemented.
// using CIPHER  = ::CIPHER;
// using PLAIN   = ::PLAIN;
// using CIPHER3 = ::CIPHER3;
using EVAL    = BackendCtx&;
using KSW     = const RelinKeysIgnored&;
using GAL     = const GaloisKeyIgnored&;
using TGT     = const ModswitchTarget&;
using CTX     = BackendCtx&;

#else
#error "Define FHE_BACKEND_CPU or FHE_BACKEND_FPGA when building fhe_ops."
#endif

// ============================================================
// Cipher × Cipher
// ============================================================
void Add_ciph          (EVAL eval, CIPHER& out, const CIPHER& in1, const CIPHER& in2);
void Add_ciph_inplace  (EVAL eval, CIPHER& out, const CIPHER& in2);
void Sub_ciph          (EVAL eval, CIPHER& out, const CIPHER& in1, const CIPHER& in2);
void Sub_ciph_inplace  (EVAL eval, CIPHER& out, const CIPHER& in2);
// Mul_ciph outputs a 3-component ciphertext (pre-relin); caller must Relin.
void Mul_ciph          (EVAL eval, CIPHER& out, const CIPHER& in1, const CIPHER& in2);

// ============================================================
// Cipher × Plain
// ============================================================
void Mul_plain         (EVAL eval, CIPHER& out, const CIPHER& in, const PLAIN& p);
void Mul_plain_inplace (EVAL eval, CIPHER& out, const PLAIN& p);
void Add_plain         (EVAL eval, CIPHER& out, const CIPHER& in, const PLAIN& p);
void Add_plain_inplace (EVAL eval, CIPHER& out, const PLAIN& p);

// ============================================================
// Cipher × vector<double> (SEAL multiply_vector_reduced_error variants)
// CPU-only bridge: encode the vector lazily. On FPGA these compile out
// under FHE_BACKEND_CPU (bootstrap is wholesale-replaced on FPGA, or the
// vector is encoded to a PLAIN first by fhe_backend — see FPGA-INTEGRATION.md).
// ============================================================
void Mul_plain        (EVAL eval, CIPHER& out, CIPHER& in,
                       const std::vector<double>& vec);
void Mul_plain_inplace(EVAL eval, CIPHER& out,
                       const std::vector<double>& vec);
void Mul_plain        (EVAL eval, CIPHER& out, CIPHER& in,
                       const std::vector<std::complex<double>>& vec);
void Mul_plain_inplace(EVAL eval, CIPHER& out,
                       const std::vector<std::complex<double>>& vec);

// ============================================================
// Cipher_ops: rescale / rotate / modswitch
// ============================================================
void Rescale_ciph           (EVAL eval, CIPHER& out, const CIPHER& in);
void Rescale_ciph_inplace   (EVAL eval, CIPHER& c);
void Rotate_ciph            (EVAL eval, CIPHER& out, const CIPHER& in, int step, GAL gal);
void Rotate_ciph_inplace    (EVAL eval, CIPHER& c, int step, GAL gal);
void Modswitch_ciph         (EVAL eval, CIPHER& out, const CIPHER& in, TGT target);
void Modswitch_ciph_inplace (EVAL eval, CIPHER& c, TGT target);
// Plaintext modswitch (for Plaintext objects like `scaler`). FPGA: re-encode
// at the level encoded in target (TBD; see FPGA-INTEGRATION.md §9).
void Modswitch_plain_inplace(EVAL eval, PLAIN& p, TGT target);
void Modswitch_next_inplace (EVAL eval, CIPHER& c);

// ============================================================
// Keyswitch / Relin
// rt_iie.h has no RelinKeys object — Relin uses the global context.relin_evk.
// The KSW arg is kept in the signature so call sites compile unchanged;
// it is ignored on FPGA.
// ============================================================
void Relin        (EVAL eval, CIPHER& out, const CIPHER3& in, KSW keys);
void Relin_inplace(EVAL eval, CIPHER& c, KSW keys);

// ============================================================
// Boot-only ops (ModularReducer)
// rt_iie.h has no square/double/ntt primitive — these are bootstrap-internal
// SEAL lowerings. On the FPGA path the whole Bootstrapper is replaced by
// fhe::Bootstrap_ciph -> eval_bootsrap, so these compile out (FHE_BACKEND_CPU
// only) OR map to composed FPGA primitives when the SDK lands.
// ============================================================
void Square        (EVAL eval, CIPHER& out, const CIPHER& in);
void Square_inplace(EVAL eval, CIPHER& c);
void Double_ciph   (EVAL eval, CIPHER& c);
// Add_scalar = add a constant (rt_iie.h Add_scalar / Add_const).
// 3-arg form: Add_scalar(out, in, value); SEAL handles out==in aliasing.
void Add_scalar        (EVAL eval, CIPHER& out, const CIPHER& in, double value);
void Add_scalar_inplace(EVAL eval, CIPHER& c, double value);
void Conjugate_ciph        (EVAL eval, CIPHER& out, const CIPHER& in, GAL gal);
void Conjugate_ciph_inplace(EVAL eval, CIPHER& c, GAL gal);

// NTT Transform (SEAL bootstrap internal; FPGA bootstrap replacement path).
#ifdef FHE_BACKEND_CPU
void Transform_to_ntt_inplace   (EVAL eval, CIPHER& c, CTX ctx);
void Transform_from_ntt_inplace (EVAL eval, CIPHER& c, CTX ctx);
#endif

// ============================================================
// Bootstrap (FPGA-stub for now).
// CPU keeps the existing Bootstrapper.cpp lowerings (180 call sites) —
// Bootstrap_ciph has NO CPU body and NO call site this round. On FPGA it
// bridges to rt_iie.h's Bootstrap/eval_bootsrap, wholesale replacing the
// SEAL bootstrap lowerings (see FPGA-INTEGRATION.md §5).
// ============================================================
#ifdef FHE_BACKEND_FPGA
void Bootstrap_ciph(EVAL eval, CIPHER& out, const CIPHER& in, int level, int slots);
#endif

} // namespace fhe
