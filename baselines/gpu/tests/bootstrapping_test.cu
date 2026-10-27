#include <gtest/gtest.h>
#include <cuda_runtime.h>
#include "boot/Bootstrapper.cuh"
#include "phantom.h"
#include <vector>
#include <cmath>
#include <random>
#include <memory>
#include <tuple>

using namespace phantom;
using namespace std;

void random_real(vector<double> &vec, size_t size) {
  random_device rn;
  mt19937_64 rnd(rn());
  thread_local std::uniform_real_distribution<double> distribution(-1, 1);

  vec.reserve(size);

  for (size_t i = 0; i < size; i++) {
    vec[i] = distribution(rnd);
  }
}
std::tuple<double, double, size_t> calculateErrorStats(vector<double> output_vector, vector<double> standard_vector) {
    if(output_vector.size() != standard_vector.size()){
        throw std::invalid_argument("Input vectors must have the same size");
    }
    if(output_vector.empty()){
        return std::make_tuple(0.0, 0.0, 0);
    }
    double sumAbsError = 0.0;
    double maxError = 0.0;
    size_t maxError_index = 0;
    for(size_t i = 0; i < output_vector.size(); i++){
        double error = abs(output_vector[i] - standard_vector[i]);
        sumAbsError += error;
        if (maxError < error){
            maxError = error;
            maxError_index = i;
        }
    }
    sumAbsError /= output_vector.size();
    return std::make_tuple(sumAbsError, maxError, maxError_index);
}

void run_bootstrapping_test(long logN, long logn, int logp, int logq, int remaining_level, int boot_level){
    long boundary_K = 25;
    long deg = 59;
    long scale_factor = 2;
    long inverse_deg = 1;
    long loge = 10;

    long sparse_slots = (1 << logn);
    int log_special_prime = 51;
    int secret_key_hamming_weight =192;
    
    int total_level = remaining_level + boot_level;   
    vector<int> coeff_bit_vec;
    coeff_bit_vec.push_back(logq);
    for(int i = 0; i < remaining_level; i++){
        coeff_bit_vec.push_back(logp);
    }
    for(int i = 0; i < boot_level; i++){
        coeff_bit_vec.push_back(logq);
    }
    coeff_bit_vec.push_back(log_special_prime);

    //std::cout << "Setting Parameters..." << endl;
    phantom::EncryptionParameters parms(scheme_type::ckks);
    size_t poly_modulus_degree = (size_t)(1 << logN);
    double scale = pow(2.0, logp);

    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_coeff_modulus(phantom::arith::CoeffModulus::Create(poly_modulus_degree, coeff_bit_vec));
    parms.set_secret_key_hamming_weight(secret_key_hamming_weight);
    parms.set_sparse_slots(sparse_slots);

    PhantomContext context(parms);

    PhantomSecretKey secret_key(context);
    PhantomPublicKey public_key = secret_key.gen_publickey(context);
    PhantomRelinKey  relin_keys = secret_key.gen_relinkey(context);
    PhantomGaloisKey galois_keys;

    PhantomCKKSEncoder encoder(context);

    CKKSEvaluator ckks_evaluator(&context, &public_key, &secret_key, &encoder, &relin_keys, &galois_keys, scale);

    size_t slot_count = encoder.slot_count();

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
    
    //std::cout << "Generating Optimal Minimax Polynimials..." << endl;
    bootstrapper.prepare_mod_polynomial();

    //std::cout << "Adding Bootstrapping Keys..."  <<endl;
    vector<int> gal_steps_vector;
    gal_steps_vector.push_back(0);
    for(int i = 0; i < logN - 1; i++){
        gal_steps_vector.push_back((1 << i));                     
    }
    bootstrapper.addLeftRotKeys_Linear_to_vector_3(gal_steps_vector);

    ckks_evaluator.decryptor.create_galois_keys_from_steps(gal_steps_vector, *(ckks_evaluator.galois_keys));
    //std::cout << "Galois key generated from steps vector." << endl;

    bootstrapper.slot_vec.push_back(logn);

    //std::cout << "Generating Linear Transformation Coefficients..." << endl;
    bootstrapper.generate_LT_coefficient_3();

    vector<double> sparse(sparse_slots, 0.0);
    vector<double> input(slot_count, 0.0);
    vector<double> before(slot_count, 0.0);
    vector<double> after(slot_count, 0.0); 

    random_real(sparse, sparse_slots);

    PhantomPlaintext plain;
    PhantomCiphertext cipher;

    for(size_t i = 0; i < slot_count; i++){
        input[i] = sparse[i % sparse_slots];
    }

    ckks_evaluator.encoder.encode(input, scale, plain);
    ckks_evaluator.encryptor.encrypt(plain, cipher);

    for(int i = 0; i < total_level; i++){
        ckks_evaluator.evaluator.mod_switch_to_next_inplace(cipher);
    }

    ckks_evaluator.decryptor.decrypt(cipher, plain);
    ckks_evaluator.encoder.decode(plain, before);

    PhantomCiphertext rtn;
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start, 0);
    bootstrapper.bootstrap_3(rtn, cipher);
    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    float elapsedTime;
    cudaEventElapsedTime(&elapsedTime, start, stop);
    //duration<double> sec = system_clock::now() - start;
    std::cout << "Bootstrapping Kernel execution time: " << elapsedTime << " ms" << std::endl;  

    ckks_evaluator.decryptor.decrypt(rtn, plain);
    ckks_evaluator.encoder.decode(plain, after);

    ASSERT_EQ(before.size(), after.size());

    auto [sumAbsError, maxError, maxError_index] = calculateErrorStats(after, before);

    std::cout << "Bootstrapping Mean Absolute Error (MAE): " << sumAbsError << std::endl;
    std::cout << "Max Error Index: " << maxError_index << endl << "Bootstrapping Max Error: " << maxError << std::endl;
}

namespace phantomtest{
    TEST(PhantomCKKSBasicOperationsTest, BootstrappingOperationTest1){
        run_bootstrapping_test(16, 15, 48, 54, 16, 14);
    }
    TEST(PhantomCKKSBasicOperationsTest, BootstrappingOperationTest2){
        run_bootstrapping_test(16, 15, 46, 51, 16, 14);
    }
}
