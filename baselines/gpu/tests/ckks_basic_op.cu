#include <gtest/gtest.h>
#include "phantom.h"
#include <vector>
#include <cmath>
#include <random>
#include <memory>

using namespace phantom;
using namespace phantom::arith;
using namespace phantom::util;

class PhantomCKKSBasicOperationsTest : public ::testing::Test {
protected:
    void SetUp() override {
        parms = std::make_unique<EncryptionParameters>(scheme_type::ckks);
        poly_modulus_degree = 8192;
        parms->set_poly_modulus_degree(poly_modulus_degree);
        parms->set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, {60, 40, 40, 60}));
        scale = pow(2.0, 40);
        
        context = std::make_unique<PhantomContext>(*parms);
        encoder = std::make_unique<PhantomCKKSEncoder>(*context);
        secret_key = std::make_unique<PhantomSecretKey>(*context);
        public_key = std::make_unique<PhantomPublicKey>(secret_key->gen_publickey(*context));
        relin_keys = std::make_unique<PhantomRelinKey>(secret_key->gen_relinkey(*context));
        galois_keys = std::make_unique<PhantomGaloisKey>(secret_key->create_galois_keys(*context));
    }

    std::unique_ptr<EncryptionParameters> parms;
    size_t poly_modulus_degree;
    std::unique_ptr<PhantomContext> context;
    std::unique_ptr<PhantomCKKSEncoder> encoder;
    std::unique_ptr<PhantomSecretKey> secret_key;
    std::unique_ptr<PhantomPublicKey> public_key;
    std::unique_ptr<PhantomRelinKey> relin_keys;
    std::unique_ptr<PhantomGaloisKey> galois_keys;
    double scale;

    const double EPSILON = 0.001;

    std::vector<cuDoubleComplex> generate_random_vector(size_t size) {
        std::vector<cuDoubleComplex> result(size);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(-1.0, 1.0);
        for (size_t i = 0; i < size; ++i) {
            result[i] = make_cuDoubleComplex(dis(gen), dis(gen));
        }
        return result;
    }
};

TEST_F(PhantomCKKSBasicOperationsTest, EncodeDecodeTest) {
    std::vector<cuDoubleComplex> input = generate_random_vector(encoder->slot_count());
    
    PhantomPlaintext plain;
    encoder->encode(*context, input, scale, plain);
    
    std::vector<cuDoubleComplex> output;
    encoder->decode(*context, plain, output);
    
    ASSERT_EQ(input.size(), output.size());
    for (size_t i = 0; i < input.size(); i++) {
        EXPECT_NEAR(input[i].x, output[i].x, EPSILON);
        EXPECT_NEAR(input[i].y, output[i].y, EPSILON);
    }
}

TEST_F(PhantomCKKSBasicOperationsTest, SymmetricEncryptionTest) {
    std::vector<cuDoubleComplex> input = generate_random_vector(encoder->slot_count());
    
    PhantomPlaintext plain, decrypted_plain;
    encoder->encode(*context, input, scale, plain);
    
    PhantomCiphertext cipher;
    secret_key->encrypt_symmetric(*context, plain, cipher);
    
    secret_key->decrypt(*context, cipher, decrypted_plain);
    
    std::vector<cuDoubleComplex> output;
    encoder->decode(*context, decrypted_plain, output);
    
    ASSERT_EQ(input.size(), output.size());
    for (size_t i = 0; i < input.size(); i++) {
        EXPECT_NEAR(input[i].x, output[i].x, EPSILON);
        EXPECT_NEAR(input[i].y, output[i].y, EPSILON);
    }
}

TEST_F(PhantomCKKSBasicOperationsTest, HomomorphicAdditionTest) {
    std::vector<cuDoubleComplex> input1 = generate_random_vector(encoder->slot_count());
    std::vector<cuDoubleComplex> input2 = generate_random_vector(encoder->slot_count());
    
    PhantomPlaintext plain1, plain2, result_plain;
    encoder->encode(*context, input1, scale, plain1);
    encoder->encode(*context, input2, scale, plain2);
    
    PhantomCiphertext cipher1, cipher2;
    public_key->encrypt_asymmetric(*context, plain1, cipher1);
    public_key->encrypt_asymmetric(*context, plain2, cipher2);
    
    add_inplace(*context, cipher1, cipher2);
    
    secret_key->decrypt(*context, cipher1, result_plain);
    
    std::vector<cuDoubleComplex> output;
    encoder->decode(*context, result_plain, output);
    
    ASSERT_EQ(input1.size(), output.size());
    for (size_t i = 0; i < input1.size(); i++) {
        EXPECT_NEAR(input1[i].x + input2[i].x, output[i].x, EPSILON);
        EXPECT_NEAR(input1[i].y + input2[i].y, output[i].y, EPSILON);
    }
}

TEST_F(PhantomCKKSBasicOperationsTest, HomomorphicMultiplicationTest) {
    std::vector<cuDoubleComplex> input1 = generate_random_vector(encoder->slot_count());
    std::vector<cuDoubleComplex> input2 = generate_random_vector(encoder->slot_count());
    
    PhantomPlaintext plain1, plain2, result_plain;
    encoder->encode(*context, input1, scale, plain1);
    encoder->encode(*context, input2, scale, plain2);
    
    PhantomCiphertext cipher1, cipher2;
    public_key->encrypt_asymmetric(*context, plain1, cipher1);
    public_key->encrypt_asymmetric(*context, plain2, cipher2);
    
    PhantomCiphertext cipher_result = multiply(*context, cipher1, cipher2);
    relinearize_inplace(*context, cipher_result, *relin_keys);
    rescale_to_next_inplace(*context, cipher_result);
    
    secret_key->decrypt(*context, cipher_result, result_plain);
    
    std::vector<cuDoubleComplex> output;
    encoder->decode(*context, result_plain, output);
    
    ASSERT_EQ(input1.size(), output.size());
    for (size_t i = 0; i < input1.size(); i++) {
        cuDoubleComplex expected = cuCmul(input1[i], input2[i]);
        EXPECT_NEAR(expected.x, output[i].x, EPSILON * 10);
        EXPECT_NEAR(expected.y, output[i].y, EPSILON * 10);
    }
}

TEST_F(PhantomCKKSBasicOperationsTest, RotationTest) {
    std::vector<cuDoubleComplex> input(encoder->slot_count());
    for (size_t i = 0; i < input.size(); i++) {
        input[i] = make_cuDoubleComplex((double)i, 0.0);
    }
    
    PhantomPlaintext plain, result_plain;
    encoder->encode(*context, input, scale, plain);
    
    PhantomCiphertext cipher;
    public_key->encrypt_asymmetric(*context, plain, cipher);
    
    int rotation_steps = 3;
    rotate_inplace(*context, cipher, rotation_steps, *galois_keys);
    
    secret_key->decrypt(*context, cipher, result_plain);
    
    std::vector<cuDoubleComplex> output;
    encoder->decode(*context, result_plain, output);
    
    ASSERT_EQ(input.size(), output.size());
    for (size_t i = 0; i < input.size(); i++) {
        size_t expected_index = (i + rotation_steps) % input.size();
        EXPECT_NEAR(input[expected_index].x, output[i].x, EPSILON);
        EXPECT_NEAR(input[expected_index].y, output[i].y, EPSILON);
    }
}

