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

void run_multi_test(size_t poly_modulus_degree, const vector<int>& coeff_modulus, double scale){
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
    vector<complex<double>> input1_vector = generate_random_vector(slots);
    vector<complex<double>> input2_vector = generate_random_vector(slots);

    PhantomPlaintext plain1, plain2;
    ckks_evaluator.encoder.encode(input1_vector, scale, plain1);
    ckks_evaluator.encoder.encode(input2_vector, scale, plain2);

    PhantomCiphertext cipher1_multiply, cipher2_multiply, dest_multiply;
    ckks_evaluator.encryptor.encrypt(plain1, cipher1_multiply);
    ckks_evaluator.encryptor.encrypt(plain2, cipher2_multiply); 
    ckks_evaluator.evaluator.multiply(cipher1_multiply, cipher2_multiply, dest_multiply);

    PhantomCiphertext dest_rescale;
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start, 0);
    ckks_evaluator.evaluator.rescale_to_next(cipher1_multiply, dest_rescale);
    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    float elapsedTime;
    cudaEventElapsedTime(&elapsedTime, start, stop);
    //duration<double> sec = system_clock::now() - start;
    std::cout << "Rescale to next Kernel execution time: " << elapsedTime << " ms" << std::endl;  
    ckks_evaluator.evaluator.relinearize_inplace(dest_multiply, relin_keys);

    PhantomPlaintext plain_multiply;
    ckks_evaluator.decryptor.decrypt(dest_multiply, plain_multiply);
    vector<complex<double>> output_multiply;
    ckks_evaluator.encoder.decode(plain_multiply, output_multiply);

    ASSERT_EQ(input1_vector.size(), output_multiply.size());
    vector<complex<double>> standard_vector(input1_vector.size()); 
    for(size_t i = 0; i < input1_vector.size(); i++){
        standard_vector[i] = input1_vector[i] * input2_vector[i];
    }
    auto [sumAbsError, maxError, maxError_index] = calculateErrorStats(standard_vector, output_multiply);

    std::cout << "Rescale to next Mean Absolute Error (MAE): " << sumAbsError << std::endl;
    std::cout << "Max Error Index: " << maxError_index << endl << "Rescale to next Max Error: " << maxError << std::endl;
}

namespace phantomtest{
    TEST(PhantomCKKSBasicOperationsTest, MultiOperationTest1) {
        run_multi_test(8192, {60, 40, 40, 60}, pow(2.0, 40));
    }
    TEST(PhantomCKKSBasicOperationsTest, MultiOperationTest2) {
        run_multi_test(8192, {50, 40, 40, 50}, pow(2.0, 40));
    }

    TEST(PhantomCKKSBasicOperationsTest, MultiOperationTest3) {
        run_multi_test(16384, {60, 50, 50, 50, 50, 50, 50, 60}, pow(2.0, 50));
    }

    TEST(PhantomCKKSBasicOperationsTest, MultiOperationTest4) {
        run_multi_test(16384, {60, 45, 45, 45, 45, 45, 45, 45, 60}, pow(2.0, 45));
    }
    TEST(PhantomCKKSBasicOperationsTest, MultiOperationTest5) {
        run_multi_test(16384, {60, 40, 40, 40, 40, 40, 40, 40, 60}, pow(2.0, 40));
    }
    TEST(PhantomCKKSBasicOperationsTest, MultiOperationTest6) {
        run_multi_test(32768, {60, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 60}, pow(2.0, 50));
    }
    TEST(PhantomCKKSBasicOperationsTest, MultiOperationTest7) {
        run_multi_test(32768, {60, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 60}, pow(2.0, 40));
    }

    TEST(PhantomCKKSBasicOperationsTest, MultiOperationTest8) {
        run_multi_test(32768, {60, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 60}, pow(2.0, 60));
    }
    TEST(PhantomCKKSBasicOperationsTest, MultiOperationTest9) {
        run_multi_test(65536, {60, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 60}, pow(2.0, 60));
    }
    TEST(PhantomCKKSBasicOperationsTest, MultiOperationTest10) {
        run_multi_test(65536, {60, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50,50, 60}, pow(2.0, 50));
    }
}

