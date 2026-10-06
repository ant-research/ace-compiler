// fhe_ops.cpp — CPU backend implementations (rt_iie.h-aligned names).
//
// Each function is a 1-line forward to the corresponding seal::Evaluator
// method. The "reduced_error" variants are specific to this modified SEAL
// 3.6.6 (lazy-scaling add/sub/multiply). On FPGA these map to rt_iie.h
// wrappers — see plans/fpga-adaptation/FPGA-INTEGRATION.md.

#include "fhe_ops.h"

#ifdef FHE_BACKEND_CPU

namespace fhe {

// --- Cipher × Cipher ---
void Add_ciph(seal::Evaluator& eval, CIPHER& out, const CIPHER& in1, const CIPHER& in2) {
    eval.add(in1, in2, out);
}
void Add_ciph_inplace(seal::Evaluator& eval, CIPHER& out, const CIPHER& in2) {
    eval.add_inplace_reduced_error(out, in2);
}
void Sub_ciph(seal::Evaluator& eval, CIPHER& out, const CIPHER& in1, const CIPHER& in2) {
    eval.sub(in1, in2, out);
}
void Sub_ciph_inplace(seal::Evaluator& eval, CIPHER& out, const CIPHER& in2) {
    eval.sub_inplace_reduced_error(out, in2);
}
// multiply yields a 3-polynomial ciphertext (pre-relin). On CPU the SEAL
// Ciphertext carries the size internally; the caller follows with Relin.
void Mul_ciph(seal::Evaluator& eval, CIPHER& out, const CIPHER& in1, const CIPHER& in2) {
    eval.multiply(in1, in2, out);
}

// --- Cipher × Plain ---
void Mul_plain(seal::Evaluator& eval, CIPHER& out, const CIPHER& in, const PLAIN& p) {
    eval.multiply_plain(in, p, out);
}
void Mul_plain_inplace(seal::Evaluator& eval, CIPHER& out, const PLAIN& p) {
    eval.multiply_plain_inplace(out, p);
}
void Add_plain(seal::Evaluator& eval, CIPHER& out, const CIPHER& in, const PLAIN& p) {
    eval.add_plain(in, p, out);
}
void Add_plain_inplace(seal::Evaluator& eval, CIPHER& out, const PLAIN& p) {
    eval.add_plain_inplace(out, p);
}

// --- Cipher × vector<double> (multiply_vector_reduced_error variants) ---
void Mul_plain(seal::Evaluator& eval, CIPHER& out, CIPHER& in,
               const std::vector<double>& vec) {
    eval.multiply_vector_reduced_error(in, vec, out);
}
void Mul_plain_inplace(seal::Evaluator& eval, CIPHER& out,
                       const std::vector<double>& vec) {
    eval.multiply_vector_inplace_reduced_error(out, vec);
}

// vector<complex<double>> overload (Bootstrapper rotatedcoeff path)
void Mul_plain(seal::Evaluator& eval, CIPHER& out, CIPHER& in,
               const std::vector<std::complex<double>>& vec) {
    eval.multiply_vector_reduced_error(in, vec, out);
}
void Mul_plain_inplace(seal::Evaluator& eval, CIPHER& out,
                       const std::vector<std::complex<double>>& vec) {
    eval.multiply_vector_inplace_reduced_error(out, vec);
}

// --- Rescale / Rotate / Modswitch ---
void Rescale_ciph(seal::Evaluator& eval, CIPHER& out, const CIPHER& in) {
    eval.rescale_to_next(in, out);
}
void Rescale_ciph_inplace(seal::Evaluator& eval, CIPHER& c) {
    eval.rescale_to_next_inplace(c);
}
void Rotate_ciph(seal::Evaluator& eval, CIPHER& out, const CIPHER& in, int step,
                 const seal::GaloisKeys& gal) {
    eval.rotate_vector(in, step, gal, out);
}
void Rotate_ciph_inplace(seal::Evaluator& eval, CIPHER& c, int step,
                         const seal::GaloisKeys& gal) {
    eval.rotate_vector_inplace(c, step, gal);
}
void Modswitch_ciph(seal::Evaluator& eval, CIPHER& out, const CIPHER& in,
                    const seal::parms_id_type& target) {
    eval.mod_switch_to(in, target, out);
}
void Modswitch_ciph_inplace(seal::Evaluator& eval, CIPHER& c,
                            const seal::parms_id_type& target) {
    eval.mod_switch_to_inplace(c, target);
}
// Plaintext modswitch (for Plaintext objects like `scaler`)
void Modswitch_plain_inplace(seal::Evaluator& eval, PLAIN& p,
                             const seal::parms_id_type& target) {
    eval.mod_switch_to_inplace(p, target);
}
void Modswitch_next_inplace(seal::Evaluator& eval, CIPHER& c) {
    eval.mod_switch_to_next_inplace(c);
}

// --- Keyswitch / Relin ---
void Relin(seal::Evaluator& eval, CIPHER& out, const CIPHER3& in,
           const seal::RelinKeys& relin_keys) {
    eval.relinearize(in, relin_keys, out);
}
void Relin_inplace(seal::Evaluator& eval, CIPHER& c,
                    const seal::RelinKeys& relin_keys) {
    eval.relinearize_inplace(c, relin_keys);
}

// --- Boot-only ops (ModularReducer) ---
void Square(seal::Evaluator& eval, CIPHER& out, const CIPHER& in) {
    eval.square(in, out);
}
void Square_inplace(seal::Evaluator& eval, CIPHER& c) {
    eval.square_inplace(c);
}
void Double_ciph(seal::Evaluator& eval, CIPHER& c) {
    eval.double_inplace(c);
}
// 3-arg: SEAL add_const(in, value, out) handles out==in aliasing.
void Add_scalar(seal::Evaluator& eval, CIPHER& out, const CIPHER& in, double value) {
    eval.add_const(in, value, out);
}
void Add_scalar_inplace(seal::Evaluator& eval, CIPHER& c, double value) {
    eval.add_const_inplace(c, value);
}
void Conjugate_ciph(seal::Evaluator& eval, CIPHER& out, const CIPHER& in,
                    const seal::GaloisKeys& gal) {
    eval.complex_conjugate(in, gal, out);
}
void Conjugate_ciph_inplace(seal::Evaluator& eval, CIPHER& c,
                            const seal::GaloisKeys& gal) {
    eval.complex_conjugate_inplace(c, gal);
}

// --- NTT Transform (SEAL bootstrap internal; FPGA bootstrap replacement path) ---
void Transform_to_ntt_inplace(seal::Evaluator& eval, CIPHER& c,
                              const seal::SEALContext& /*ctx*/) {
    eval.transform_to_ntt_inplace(c);
}
void Transform_from_ntt_inplace(seal::Evaluator& eval, CIPHER& c,
                                const seal::SEALContext& /*ctx*/) {
    eval.transform_from_ntt_inplace(c);
}

} // namespace fhe

#elif defined(FHE_BACKEND_FPGA)
// FPGA backend bodies: implemented when the FPGA SDK is wired up.
// See plans/fpga-adaptation/FPGA-INTEGRATION.md for the per-function mapping
// to rt_iie.h wrappers (Add_ciph / Sub_ciph / Mul_ciph / Relin /
// Rescale_ciph / Rotate_ciph / Mul_plain / Add_plain / Modswitch /
// Bootstrap -> eval_bootsrap, etc.).
#error "FHE_BACKEND_FPGA not yet implemented — see FPGA-INTEGRATION.md"
#endif
