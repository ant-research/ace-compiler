#include <random>

#include "boot/Bootstrapper.cuh"
#include "phantom.h"

using namespace std;
using namespace phantom;

void random_real(vector<complex<double>> &vec, size_t size)
{
    random_device rn;
    mt19937_64 rnd(rn());
    thread_local std::uniform_real_distribution<double> distribution(-1, 1);

    vec.reserve(size);

    for (size_t i = 0; i < size; i++)
    {
        vec[i] = complex<double>(distribution(rnd), 0.0);
    }
}

void random_complex(vector<complex<double>> &vec, size_t size)
{
    random_device rn;
    mt19937_64 rnd(rn());
    thread_local std::uniform_real_distribution<double> distribution(-1, 1);

    vec.reserve(size);

    for (size_t i = 0; i < size; i++)
    {
        vec[i] = complex<double>(distribution(rnd), distribution(rnd));
    }
}

double recover_vefore(double before, long boundary_K)
{
    double scaled = boundary_K * before;
    return scaled - round(scaled);
}

int main()
{

    long boundary_K = 25;
    long deg = 59;
    long scale_factor = 2;
    long inverse_deg = 1;

    // The following parameters have been adjusted to satisfy the memory constraints of an H800 GPU
    long logN = 16; // 16 -> 15
    long loge = 10;

    long logn = 15;   // 14 -> 13
    long logn_1 = 14; // 14 -> 13
    long logn_2 = 13; // 14 -> 13
    long logn_3 = 12; // 14 -> 13
    long sparse_slots = (1 << logn_3);

    int logp = 56;
    int logq = 60;
    int log_special_prime = 51;

    int secret_key_hamming_weight = 192;

    int remaining_level = 20;
    int boot_level = 14; // >= subsum 1 + coefftoslot 2 + ModReduction 9 + slottocoeff 2
    int total_level = remaining_level + boot_level;

    vector<int> coeff_bit_vec;
    coeff_bit_vec.push_back(logp);
    for (int i = 0; i < total_level; i++)
    {
        coeff_bit_vec.push_back(logp);
    }

    coeff_bit_vec.push_back(logq);

    std::cout << "Setting Parameters..." << endl;
    phantom::EncryptionParameters parms(scheme_type::ckks);
    size_t poly_modulus_degree = (size_t)(1 << logN);
    double scale = pow(2.0, logp);

    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_coeff_modulus(phantom::arith::CoeffModulus::Create(poly_modulus_degree, coeff_bit_vec));
    parms.set_secret_key_hamming_weight(secret_key_hamming_weight);
    // parms.set_sparse_slots(16384);
    PhantomContext context(parms);

    PhantomSecretKey secret_key(context);
    PhantomPublicKey public_key = secret_key.gen_publickey(context);
    PhantomRelinKey relin_keys = secret_key.gen_relinkey(context);
    PhantomGaloisKey galois_keys;

    PhantomCKKSEncoder encoder(context);

    CKKSEvaluator ckks_evaluator(&context, &public_key, &secret_key, &encoder, &relin_keys, &galois_keys, scale);

    size_t slot_count = encoder.slot_count();

    Bootstrapper bootstrapper(loge, logn, logN - 1, total_level, scale, boundary_K, deg, scale_factor, inverse_deg, &ckks_evaluator);
    Bootstrapper bootstrapper_1(loge, logn_1, logN - 1, total_level, scale, boundary_K, deg, scale_factor, inverse_deg, &ckks_evaluator);
    Bootstrapper bootstrapper_2(loge, logn_2, logN - 1, total_level, scale, boundary_K, deg, scale_factor, inverse_deg, &ckks_evaluator);
    Bootstrapper bootstrapper_3(loge, logn_3, logN - 1, total_level, scale, boundary_K, deg, scale_factor, inverse_deg, &ckks_evaluator);
    cout << "Generating Optimal Minimax Polynomials..." << endl;

    bootstrapper.prepare_mod_polynomial();
    bootstrapper_1.prepare_mod_polynomial();
    bootstrapper_2.prepare_mod_polynomial();
    bootstrapper_3.prepare_mod_polynomial();
    cout << "Adding Bootstrapping Keys..." << endl;

    vector<int> gal_steps_vector;
    gal_steps_vector.push_back(0);
    for (int i = 0; i < logN - 1; i++)
    {
        gal_steps_vector.push_back((1 << i));
    }
    bootstrapper_1.addLeftRotKeys_Linear_to_vector_3(gal_steps_vector);
    bootstrapper_2.addLeftRotKeys_Linear_to_vector_3(gal_steps_vector);
    bootstrapper_3.addLeftRotKeys_Linear_to_vector_3(gal_steps_vector);

    ckks_evaluator.decryptor.create_galois_keys_from_steps(gal_steps_vector, *(ckks_evaluator.galois_keys));

    bootstrapper.slot_vec.push_back(logn);
    bootstrapper_1.slot_vec.push_back(logn_1);
    bootstrapper_2.slot_vec.push_back(logn_2);
    bootstrapper_3.slot_vec.push_back(logn_3);

    cout << "Generating Linear Transformation Coefficients..." << endl;
    bootstrapper.generate_LT_coefficient_3();
    bootstrapper_1.generate_LT_coefficient_3();
    bootstrapper_2.generate_LT_coefficient_3();
    bootstrapper_3.generate_LT_coefficient_3();

    double tot_err = 0, mean_err;

    vector<double> input(2048, 1);
    input.resize(4096, 2);

    vector<double> sparse(slot_count, 0.0);

    for (size_t i = 0; i < slot_count; i++)
    {
        sparse[i] = input[i % sparse_slots];
    }

    PhantomPlaintext plain;
    PhantomCiphertext cipher;

    cout << "Encrypting..." << endl;
    ckks_evaluator.encoder.encode(sparse, scale, plain);
    // encoder.encode(context,sparse,scale,plain);
    PhantomCiphertext cipher_0, cipher_1, cipher_2, cipher_3;
    ckks_evaluator.encryptor.encrypt(plain, cipher_0);
    ckks_evaluator.encryptor.encrypt(plain, cipher_1);
    ckks_evaluator.encryptor.encrypt(plain, cipher_2);
    ckks_evaluator.encryptor.encrypt(plain, cipher_3);

    PhantomPlaintext plain_0_1, plain_1_2, plain_2_3, plain_3_3;
    ckks_evaluator.decryptor.decrypt(cipher_0, plain_0_1);
    ckks_evaluator.decryptor.decrypt(cipher_1, plain_1_2);
    ckks_evaluator.decryptor.decrypt(cipher_2, plain_2_3);
    ckks_evaluator.decryptor.decrypt(cipher_3, plain_3_3);

    std::vector<double> before_0, before_1, before_2, before_3;
    // encoder.decode(context,plain_0_1, before_0);
    ckks_evaluator.encoder.decode(plain_0_1, before_0);
    for (int i = 16374; i < 16474; i = i + 1)
    {
        cout << before_0[i] << " ";
    }
    cout << "\n=============================================" << endl;

    // encoder.decode(context,plain_1_2, before_1);
    ckks_evaluator.encoder.decode(plain_1_2, before_1);
    for (int i = 16374; i < 16474; i = i + 1)
    {
        cout << before_1[i] << " ";
    }
    cout << "\n=============================================" << endl;

    // encoder.decode(context,plain_2_3, before_2);
    ckks_evaluator.encoder.decode(plain_2_3, before_2);
    for (int i = 16374; i < 16474; i = i + 1)
    {
        cout << before_2[i] << " ";
    }
    cout << "\n=============================================" << endl;

    // encoder.decode(context,plain_3_3, before_3);
    ckks_evaluator.encoder.decode(plain_3_3, before_3);
    for (int i = 16374; i < 16474; i = i + 1)
    {
        cout << before_3[i] << " ";
    }
    cout << "\n=============================================" << endl;

    cout << "\n=============================================" << endl;

    for (int i = 0; i < total_level; i++)
    {
        ckks_evaluator.evaluator.mod_switch_to_next_inplace(cipher_0);
        ckks_evaluator.evaluator.mod_switch_to_next_inplace(cipher_1);
        ckks_evaluator.evaluator.mod_switch_to_next_inplace(cipher_2);
        ckks_evaluator.evaluator.mod_switch_to_next_inplace(cipher_3);
    }

    cout << "Bootstrapping..." << endl;
    PhantomCiphertext cipher_0_out, cipher_1_out, cipher_2_out, cipher_3_out;
    bootstrapper.bootstrap_3(cipher_0_out, cipher_0);
    bootstrapper_1.bootstrap_3(cipher_1_out, cipher_1);
    bootstrapper_2.bootstrap_3(cipher_2_out, cipher_2);
    bootstrapper_3.bootstrap_3(cipher_3_out, cipher_3);

    cout << "Decrypting..." << endl;
    PhantomPlaintext plain_0, plain_1, plain_2, plain_3;
    ckks_evaluator.decryptor.decrypt(cipher_0_out, plain_0);
    ckks_evaluator.decryptor.decrypt(cipher_1_out, plain_1);
    ckks_evaluator.decryptor.decrypt(cipher_2_out, plain_2);
    ckks_evaluator.decryptor.decrypt(cipher_3_out, plain_3);
    cout << "\n=============================================" << endl;

    vector<double> after_0;
    ckks_evaluator.encoder.decode(plain_0, after_0);
    for (int i = 0; i < after_0.size(); i = i + 1)
    {
        cout << after_0[i] << " ";
        if (i % 1024 == 0)
        {
            cout << "\n";
        }
    }
    cout << "\n=============================================" << endl;
    vector<double> after;
    ckks_evaluator.encoder.decode(plain_1, after);
    for (int i = 0; i < after.size(); i = i + 1)
    {
        cout << after[i] << " ";
        if (i % 1024 == 0)
        {
            cout << "\n";
        }
    }
    cout << "\n=============================================" << endl;

    std::vector<double> after_1, after_2;
    ckks_evaluator.encoder.decode(plain_2, after_1);
    for (int i = 0; i < after_1.size(); i = i + 1)
    {
        cout << after_1[i] << " ";
        if (i % 1024 == 0)
        {
            cout << "\n";
        }
    }
    cout << "\n=============================================" << endl;

    ckks_evaluator.encoder.decode(plain_3, after_2);
    for (int i = 0; i < after_2.size(); i = i + 1)
    {
        cout << after_2[i] << " ";
        if (i % 1024 == 0)
        {
            cout << "\n";
        }
    }
    cout << "\n=============================================" << endl;

    return 0;
}