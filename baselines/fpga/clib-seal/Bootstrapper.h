#pragma once

// clib-backed replacement for FHE-MP-CNN's SEAL-internal CKKS bootstrapper.
//
// The original Bootstrapper reimplements RNS-CKKS bootstrapping directly on
// raw SEAL internals: it walks ciphertext RNS coefficients (seal::util::
// RNSIter), builds FFT/linear-transform plaintexts host-side, relies on a
// fork-only KeyGenerator::create_hoisted_galois_keys, and carries a
// ModularReducer. None of that exists in the clib SDK backend, which owns
// bootstrapping and the key material: clib::Bootstrap refreshes a Q=2
// ciphertext, and rotations/galois keys come from the material directory.
//
// This drop-in keeps exactly the Bootstrapper surface that cnn_ckks drives
// and routes the refresh to seal::Evaluator::bootstrap (clib::Bootstrap). The
// FFT/linear-transform setup steps become no-ops because the backend performs
// them internally.

#include <complex>
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>

#include "seal/seal.h"

using namespace std;
using namespace seal;

class Bootstrapper {
public:
    long loge;
    long logn;
    long n;
    long logNh;
    long Nh;
    long L;

    double initial_scale;
    double final_scale;

    long boundary_K;
    long sin_cos_deg;
    long scale_factor;
    long inverse_deg;

    SEALContext &context;
    KeyGenerator &keygen;
    CKKSEncoder &encoder;
    Encryptor &encryptor;
    Decryptor &decryptor;
    Evaluator &evaluator;
    RelinKeys &relin_keys;
    GaloisKeys &gal_keys;

    vector<long> slot_vec;
    long slot_index = 0;

    Bootstrapper(long _loge, long _logn, long _logNh, long _L, double _final_scale,
                 long _boundary_K, long _sin_cos_deg, long _scale_factor, long _inverse_deg,
                 SEALContext &_context,
                 KeyGenerator &_keygen,
                 CKKSEncoder &_encoder,
                 Encryptor &_encryptor,
                 Decryptor &_decryptor,
                 Evaluator &_evaluator,
                 RelinKeys &_relin_keys,
                 GaloisKeys &_gal_keys);

    // Rotation keys needed by the original linear transforms are supplied by
    // the clib material directory, so this only records nothing.
    void addLeftRotKeys_Linear_to_vector_3(vector<int> &gal_steps_vector);

    // FFT/linear-transform coefficient preparation is internal to the backend.
    void prepare_mod_polynomial();
    void generate_LT_coefficient_3();

    // Refresh a ciphertext through clib::Bootstrap.
    void bootstrap_real_3(Ciphertext &rtncipher, Ciphertext &cipher);
    void bootstrap_3(Ciphertext &rtncipher, Ciphertext &cipher);
    void bootstrap_inplace_3(Ciphertext &cipher);
};
