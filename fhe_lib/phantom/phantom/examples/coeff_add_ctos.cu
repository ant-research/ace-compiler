#include <algorithm>
#include <cmath>
#include <iostream>
#include <random>
#include <vector>

#include "boot/Bootstrapper.cuh"
#include "phantom.h"

using namespace std;
using namespace phantom;

int main()
{
    long logN = 14;
    long logn = logN - 1;
    long loge = 10;
    long boundary_K = 25;
    long deg = 59;
    long scale_factor = 2;
    long inverse_deg = 127;

    size_t poly_modulus_degree = static_cast<size_t>(1 << logN);
    int logp = 47;
    int logq = 51;
    int log_special_prime = 51;
    int secret_key_hamming_weight = 192;

    int remaining_level = 3;
    int boot_level = 3 + 6 + 2 + 1 + 7;
    int total_level = remaining_level + boot_level;

    vector<int> coeff_bit_vec;
    coeff_bit_vec.push_back(logq);
    for (int i = 0; i < remaining_level; i++)
    {
        coeff_bit_vec.push_back(logp);
    }
    for (int i = 0; i <= boot_level; i++)
    {
        coeff_bit_vec.push_back(logq);
    }
    coeff_bit_vec.push_back(log_special_prime);

    double scale = pow(2.0, logp);

    EncryptionParameters parms(scheme_type::ckks);
    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_coeff_modulus(phantom::arith::CoeffModulus::Create(poly_modulus_degree, coeff_bit_vec));
    parms.set_secret_key_hamming_weight(secret_key_hamming_weight);
    parms.set_sparse_slots(static_cast<size_t>(1 << logn));

    PhantomContext context(parms);
    PhantomCKKSEncoder encoder(context);

    PhantomSecretKey secret_key(context);
    PhantomPublicKey public_key = secret_key.gen_publickey(context);
    PhantomRelinKey relin_keys = secret_key.gen_relinkey(context);
    PhantomGaloisKey galois_keys;

    CKKSEvaluator ckks_evaluator(&context, &public_key, &secret_key,
                                 &encoder, &relin_keys, &galois_keys, scale);

    Bootstrapper bootstrapper(
        loge,
        logn,
        logN - 1,
        total_level,
        scale,
        boundary_K,
        deg,
        scale_factor,
        inverse_deg,
        &ckks_evaluator);

    bootstrapper.prepare_mod_polynomial();

    vector<int> gal_steps_vector;
    gal_steps_vector.push_back(0);
    for (int i = 0; i < logN - 1; i++)
    {
        gal_steps_vector.push_back(1 << i);
    }
    bootstrapper.addLeftRotKeys_Linear_to_vector_3(gal_steps_vector);
    ckks_evaluator.decryptor.create_galois_keys_from_steps(
        gal_steps_vector, *(ckks_evaluator.galois_keys));

    bootstrapper.slot_vec.push_back(logn);
    bootstrapper.generate_LT_coefficient_3();

    vector<double> coeffs(poly_modulus_degree, 0.0);
    random_device rd;
    mt19937_64 rng(rd());
    uniform_real_distribution<double> dist(-1.0, 1.0);
    for (size_t i = 0; i < coeffs.size(); i++)
    {
        coeffs[i] = dist(rng);
    }

    PhantomPlaintext pt;
    encoder.encode_coeffs(context, coeffs, scale, pt);

    PhantomCiphertext ct;
    ckks_evaluator.encryptor.encrypt(pt, ct);

    while (ct.coeff_modulus_size() > 1)
    {
        ckks_evaluator.evaluator.mod_switch_to_next_inplace(ct);
    }
    PhantomCiphertext ct_manual = ct;
    bootstrapper.initial_scale = ct_manual.scale();
    bootstrapper.modraise_inplace(ct_manual);
    const auto &modulus = context.first_context_data().parms().coeff_modulus();
    ct_manual.set_scale(static_cast<double>(modulus[0].value()));

    PhantomCiphertext ct_slot_1, ct_slot_2;
    bootstrapper.coefftoslot_full_3(ct_slot_1, ct_slot_2, ct_manual);
    ckks_evaluator.evaluator.multiply_const_inplace(ct_slot_1, 4 * 0.9);
    ckks_evaluator.evaluator.rescale_to_next_inplace(ct_slot_1);
    ckks_evaluator.evaluator.multiply_const_inplace(ct_slot_2, 4 * 0.9);
    ckks_evaluator.evaluator.rescale_to_next_inplace(ct_slot_2);
    PhantomCiphertext ct_red_1, ct_red_2;
    bootstrapper.mod_reducer->modular_reduction_relu(ct_red_1, ct_slot_1);
    bootstrapper.mod_reducer->modular_reduction_relu(ct_red_2, ct_slot_2);
    cout << "ct_slot_1 scale: " << log2(ct_slot_1.scale())
         << ", chain_index: " << ct_slot_1.chain_index() << endl;
    cout << "ct_slot_2 scale: " << log2(ct_slot_2.scale())
         << ", chain_index: " << ct_slot_2.chain_index() << endl;

    PhantomPlaintext pt_slot_1, pt_slot_2;
    ckks_evaluator.decryptor.decrypt(ct_slot_1, pt_slot_1);
    ckks_evaluator.decryptor.decrypt(ct_slot_2, pt_slot_2);

    vector<complex<double>> slots_1;
    vector<complex<double>> slots_2;
    encoder.decode(context, pt_slot_1, slots_1);
    encoder.decode(context, pt_slot_2, slots_2);

    cout << "CtoS slots_1 (first 8): ";
    for (size_t i = 0; i < 8 && i < slots_1.size(); i++)
    {
        cout << slots_1[i] << (i + 1 == 8 || i + 1 == slots_1.size() ? "\n" : ", ");
    }
    cout << "CtoS slots_2 (first 8): ";
    for (size_t i = 0; i < 8 && i < slots_2.size(); i++)
    {
        cout << slots_2[i] << (i + 1 == 8 || i + 1 == slots_2.size() ? "\n" : ", ");
    }

    PhantomCiphertext ct_out;
    bootstrapper.slottocoeff_full_3(ct_out, ct_red_1, ct_red_2);
    ct_out.set_scale(bootstrapper.final_scale);

    cout << "ct_out scale: " << log2(ct_out.scale())
         << ", chain_index: " << ct_out.chain_index() << endl;

    PhantomPlaintext pt_out;
    ckks_evaluator.decryptor.decrypt(ct_out, pt_out);

    vector<double> decoded;
    encoder.decode_coeffs(context, pt_out, decoded);

    auto relu = [](double x)
    { return x > 0.0 ? x : 0.0; };
    double max_error = 0.0;
    for (size_t i = 0; i < coeffs.size(); i++)
    {
        max_error = max(max_error, fabs(decoded[i] - relu(coeffs[i])));
    }

    cout << "Max error: " << max_error << endl;
    cout << "Decoded coefficients (first 8): ";
    for (size_t i = 0; i < 8; i++)
    {
        cout << decoded[i] << (i + 1 == 8 ? "\n" : ", ");
    }
    cout << "Expected (first 8): ";
    for (size_t i = 0; i < 8; i++)
    {
        cout << relu(coeffs[i]) << (i + 1 == 8 ? "\n" : ", ");
    }

    return 0;
}
