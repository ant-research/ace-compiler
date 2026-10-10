// clib-backed implementation of the FHE-MP-CNN Bootstrapper (see
// Bootstrapper.h). All real work is delegated to the clib SDK backend via the
// SEAL-compatible Evaluator; the setup entry points are intentionally empty.

#include "Bootstrapper.h"

Bootstrapper::Bootstrapper(long _loge, long _logn, long _logNh, long _L, double _final_scale,
                           long _boundary_K, long _sin_cos_deg, long _scale_factor, long _inverse_deg,
                           SEALContext &_context,
                           KeyGenerator &_keygen,
                           CKKSEncoder &_encoder,
                           Encryptor &_encryptor,
                           Decryptor &_decryptor,
                           Evaluator &_evaluator,
                           RelinKeys &_relin_keys,
                           GaloisKeys &_gal_keys)
    : loge(_loge),
      logn(_logn),
      n(1L << _logn),
      logNh(_logNh),
      Nh(1L << _logNh),
      L(_L),
      initial_scale(_final_scale),
      final_scale(_final_scale),
      boundary_K(_boundary_K),
      sin_cos_deg(_sin_cos_deg),
      scale_factor(_scale_factor),
      inverse_deg(_inverse_deg),
      context(_context),
      keygen(_keygen),
      encoder(_encoder),
      encryptor(_encryptor),
      decryptor(_decryptor),
      evaluator(_evaluator),
      relin_keys(_relin_keys),
      gal_keys(_gal_keys) {}

void Bootstrapper::addLeftRotKeys_Linear_to_vector_3(vector<int> &gal_steps_vector) {
    // Keys come from the clib material directory.
    //(void)gal_steps_vector;
}

void Bootstrapper::prepare_mod_polynomial() {
    // The backend owns the modular-reduction polynomial.
}

void Bootstrapper::generate_LT_coefficient_3() {
    // The backend owns the linear-transform coefficients.
}

void Bootstrapper::bootstrap_real_3(Ciphertext &rtncipher, Ciphertext &cipher) {
    // clib::Bootstrap expects the input at the lowest level (2 Q limbs) and
    // refreshes it; drop any remaining levels first. Each mod_switch consumes
    // one level (2 Q limbs), and the chain has an even limb count.
    while (cipher.coeff_modulus_size() > 2) {
        evaluator.mod_switch_to_next_inplace(cipher);
    }
    evaluator.bootstrap(cipher, rtncipher);
    // Ledger refresh, as in the original SEAL bootstrapper: the refreshed
    // ciphertext comes back at the canonical final scale. The FHE-MP-CNN
    // schedule restarts every post-refresh cycle from this scale, and the
    // clib kernels never report theirs (Cipher_info scale is NaN in export
    // mode), so the deterministic bookkeeping must be reset here - without
    // it the ledger decays by ~logp bits per level and underflows to a
    // zero scale within a few stages.
    rtncipher.scale() = final_scale;
}

void Bootstrapper::bootstrap_3(Ciphertext &rtncipher, Ciphertext &cipher) {
    bootstrap_real_3(rtncipher, cipher);
}

void Bootstrapper::bootstrap_inplace_3(Ciphertext &cipher) {
    while (cipher.coeff_modulus_size() > 2) {
        evaluator.mod_switch_to_next_inplace(cipher);
    }
    evaluator.bootstrap_inplace(cipher);
    cipher.scale() = final_scale;
}
