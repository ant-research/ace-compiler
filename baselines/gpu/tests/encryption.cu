#include <gtest/gtest.h>
#include <cuda_runtime.h>
#include "phantom.h"
#include "boot/Bootstrapper.cuh"
#include <vector>
#include <cmath>
#include <random>
#include <memory>
#include <tuple>

using namespace phantom;
using namespace phantom::arith;
using namespace phantom::util;
using namespace std;

std::vector<complex<double>> generate_random_vector(size_t size) {
    std::vector<complex<double>> result(size);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-1.0, 1.0);
    for (size_t i = 0; i < size; ++i) {
        result[i] = complex<double>(dis(gen), dis(gen));
    }
    return result;
}
std::tuple<double, double, size_t> calculateErrorStats(vector<complex<double>> output_vector, vector<complex<double>> standard_vector) {
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

void run_encrypt_test(size_t poly_modulus_degree, const vector<int>& coeff_modulus, double scale){
    EncryptionParameters parms(scheme_type::ckks);
    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_coeff_modulus(phantom::arith::CoeffModulus::Create(poly_modulus_degree, coeff_modulus));

    PhantomContext context(parms);
    PhantomCKKSEncoder encoder(context);
    PhantomSecretKey secret_key(context);
    PhantomPublicKey public_key = secret_key.gen_publickey(context);
    PhantomRelinKey relin_keys = secret_key.gen_relinkey(context);
    PhantomGaloisKey galois_keys = secret_key.create_galois_keys(context);

    CKKSEvaluator ckks_evaluator(&context, &public_key, &secret_key, &encoder, &relin_keys, &galois_keys, scale);

    int slots = ckks_evaluator.encoder.slot_count();
    vector<complex<double>> input_vector = generate_random_vector(slots);

    PhantomPlaintext plain_encryption, result_encryption;
    ckks_evaluator.encoder.encode(input_vector, scale, plain_encryption);

    PhantomCiphertext cipher_encryption;

    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start, 0);
    ckks_evaluator.encryptor.encrypt(plain_encryption, cipher_encryption);
    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    float elapsedTime;
    cudaEventElapsedTime(&elapsedTime, start, stop);
    //duration<double> sec = system_clock::now() - start;
    std::cout << "Encryption Kernel execution time: " << elapsedTime << " ms" << std::endl;  

    ckks_evaluator.decryptor.decrypt(cipher_encryption, result_encryption);

    vector<complex<double>> output_encryption;
    ckks_evaluator.encoder.decode(result_encryption, output_encryption);

    ASSERT_EQ(input_vector.size(), output_encryption.size());
    auto [sumAbsError, maxError, maxError_index] = calculateErrorStats(input_vector, output_encryption);

    std::cout << "Encryption Mean Absolute Error (MAE): " << sumAbsError << std::endl;
    std::cout << "Max Error Index: " << maxError_index << endl << "Encryption Max Error: " << maxError << std::endl;
}

namespace phantomtest{
    TEST(PhantomCKKSBasicOperationsTest, EncryptionOperationTest1) {
        run_encrypt_test(8192, {60, 40, 40, 60}, pow(2.0, 40));
    }
    TEST(PhantomCKKSBasicOperationsTest, EncryptionOperationTest2) {
        run_encrypt_test(8192, {50, 40, 40, 50}, pow(2.0, 40));
    }

    TEST(PhantomCKKSBasicOperationsTest, EncryptionOperationTest3) {
        run_encrypt_test(16384, {60, 50, 50, 50, 50, 50, 50, 60}, pow(2.0, 50));
    }

    TEST(PhantomCKKSBasicOperationsTest, EncryptionOperationTest4) {
        run_encrypt_test(16384, {60, 45, 45, 45, 45, 45, 45, 45, 60}, pow(2.0, 45));
    }
    TEST(PhantomCKKSBasicOperationsTest, EncryptionOperationTest5) {
        run_encrypt_test(16384, {60, 40, 40, 40, 40, 40, 40, 40, 60}, pow(2.0, 40));
    }
    TEST(PhantomCKKSBasicOperationsTest, EncryptionOperationTest6) {
        run_encrypt_test(32768, {60, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 60}, pow(2.0, 50));
    }
    TEST(PhantomCKKSBasicOperationsTest, EncryptionOperationTest7) {
        run_encrypt_test(32768, {60, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 60}, pow(2.0, 40));
    }

    TEST(PhantomCKKSBasicOperationsTest, EncryptionOperationTest8) {
        run_encrypt_test(32768, {60, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 60}, pow(2.0, 60));
    }
    TEST(PhantomCKKSBasicOperationsTest, EncryptionOperationTest9) {
        run_encrypt_test(65536, {60, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 60}, pow(2.0, 60));
    }
    TEST(PhantomCKKSBasicOperationsTest, EncryptionOperationTest10) {
        run_encrypt_test(65536, {60, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50,50, 60}, pow(2.0, 50));
    }
}
