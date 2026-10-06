#include <gtest/gtest.h>
#include "phantom.h"
#include <vector>
#include <cmath>
#include <random>
#include <memory>

using namespace phantom;
using namespace phantom::arith;
using namespace phantom::util;

using namespace std;

class CKKSEvaluatorTest : public ::testing::TestWithParam<int>
{
protected:
    void SetUp() override
    {
        int alpha = GetParam();
        parms = std::make_unique<EncryptionParameters>(scheme_type::ckks);

        size_t poly_modulus_degree = 1 << 15;
        scale = pow(2.0, 40);

        switch (alpha)
        {
        case 1:
            parms->set_poly_modulus_degree(poly_modulus_degree);
            parms->set_coeff_modulus(
                CoeffModulus::Create(poly_modulus_degree, {60, 40, 40, 40, 40, 40, 40, 40, 40, 40,
                                                           40, 40, 40, 40, 40, 40, 40, 40, 40, 60}));
            break;
        case 2:
            parms->set_poly_modulus_degree(poly_modulus_degree);
            parms->set_coeff_modulus(CoeffModulus::Create(
                poly_modulus_degree, {60, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 60, 60}));
            parms->set_special_modulus_size(alpha);
            break;
        case 3:
            parms->set_poly_modulus_degree(poly_modulus_degree);
            parms->set_coeff_modulus(CoeffModulus::Create(
                poly_modulus_degree, {60, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 60, 60, 60}));
            parms->set_special_modulus_size(alpha);
            break;
        case 4:
            parms->set_poly_modulus_degree(poly_modulus_degree);
            parms->set_coeff_modulus(CoeffModulus::Create(
                poly_modulus_degree, {60, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 60, 60, 60, 60}));
            parms->set_special_modulus_size(alpha);
            break;
        case 15:
            poly_modulus_degree = 1 << 16;
            parms->set_poly_modulus_degree(poly_modulus_degree);
            parms->set_coeff_modulus(CoeffModulus::Create(
                poly_modulus_degree,
                {60, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
                 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
                 50, 50, 50, 50, 50, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60}));
            parms->set_special_modulus_size(alpha);
            scale = pow(2.0, 50);
            break;
        default:
            throw std::invalid_argument("unsupported alpha params");
        }

        context = std::make_unique<PhantomContext>(*parms);
        encoder = std::make_unique<PhantomCKKSEncoder>(*context);
        secret_key = std::make_unique<PhantomSecretKey>(*context);
        public_key = std::make_unique<PhantomPublicKey>(secret_key->gen_publickey(*context));
        relin_keys = std::make_unique<PhantomRelinKey>(secret_key->gen_relinkey(*context));
        galois_keys = std::make_unique<PhantomGaloisKey>(secret_key->create_galois_keys(*context));

        evaluator = std::make_unique<CKKSEvaluator>(context.get(), public_key.get(), secret_key.get(),
                                                    encoder.get(), relin_keys.get(), galois_keys.get(),
                                                    scale);
    }

    std::unique_ptr<EncryptionParameters> parms;
    std::unique_ptr<PhantomContext> context;
    std::unique_ptr<PhantomCKKSEncoder> encoder;
    std::unique_ptr<PhantomSecretKey> secret_key;
    std::unique_ptr<PhantomPublicKey> public_key;
    std::unique_ptr<PhantomRelinKey> relin_keys;
    std::unique_ptr<PhantomGaloisKey> galois_keys;
    std::unique_ptr<CKKSEvaluator> evaluator;
    double scale;

    const double EPSILON = 0.01;

    vector<double> generate_random_vector(size_t size)
    {
        vector<double> result(size);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(-1.0, 1.0);
        for (size_t i = 0; i < size; ++i)
        {
            result[i] = dis(gen);
        }
        return result;
    }
};

TEST_P(CKKSEvaluatorTest, EncodeDecodeTest)
{
    vector<double> input = generate_random_vector(evaluator->encoder.slot_count());

    PhantomPlaintext plain;
    evaluator->encoder.encode(input, scale, plain);

    vector<double> output;
    evaluator->encoder.decode(plain, output);

    ASSERT_EQ(input.size(), output.size());
    for (size_t i = 0; i < input.size(); i++)
    {
        EXPECT_NEAR(input[i], output[i], EPSILON);
    }
}

TEST_P(CKKSEvaluatorTest, EncryptDecryptTest)
{
    vector<double> input = generate_random_vector(evaluator->encoder.slot_count());

    PhantomPlaintext plain, decrypted_plain;
    evaluator->encoder.encode(input, scale, plain);

    PhantomCiphertext cipher;
    evaluator->encryptor.encrypt(plain, cipher);

    evaluator->decryptor.decrypt(cipher, decrypted_plain);

    vector<double> output;
    evaluator->encoder.decode(decrypted_plain, output);

    ASSERT_EQ(input.size(), output.size());
    for (size_t i = 0; i < input.size(); i++)
    {
        EXPECT_NEAR(input[i], output[i], EPSILON);
    }
}

TEST_P(CKKSEvaluatorTest, HomomorphicAdditionTest)
{
    vector<double> input1 = generate_random_vector(evaluator->encoder.slot_count());
    vector<double> input2 = generate_random_vector(evaluator->encoder.slot_count());

    PhantomPlaintext plain1, plain2, result_plain;
    evaluator->encoder.encode(input1, scale, plain1);
    evaluator->encoder.encode(input2, scale, plain2);

    PhantomCiphertext cipher1, cipher2;
    evaluator->encryptor.encrypt(plain1, cipher1);
    evaluator->encryptor.encrypt(plain2, cipher2);

    evaluator->evaluator.add_inplace(cipher1, cipher2);

    evaluator->decryptor.decrypt(cipher1, result_plain);

    vector<double> output;
    evaluator->encoder.decode(result_plain, output);

    ASSERT_EQ(input1.size(), output.size());
    for (size_t i = 0; i < input1.size(); i++)
    {
        EXPECT_NEAR(input1[i] + input2[i], output[i], EPSILON);
    }
}

TEST_P(CKKSEvaluatorTest, HomomorphicMultiplicationTest)
{
    vector<double> input1 = generate_random_vector(evaluator->encoder.slot_count());
    vector<double> input2 = generate_random_vector(evaluator->encoder.slot_count());

    PhantomPlaintext plain1, plain2, result_plain;
    evaluator->encoder.encode(input1, scale, plain1);
    evaluator->encoder.encode(input2, scale, plain2);

    PhantomCiphertext cipher1, cipher2, cipher_result;
    evaluator->encryptor.encrypt(plain1, cipher1);
    evaluator->encryptor.encrypt(plain2, cipher2);

    evaluator->evaluator.multiply(cipher1, cipher2, cipher_result);
    evaluator->evaluator.relinearize_inplace(cipher_result, *relin_keys);
    evaluator->evaluator.rescale_to_next_inplace(cipher_result);

    evaluator->decryptor.decrypt(cipher_result, result_plain);

    vector<double> output;
    evaluator->encoder.decode(result_plain, output);

    ASSERT_EQ(input1.size(), output.size());
    for (size_t i = 0; i < input1.size(); i++)
    {
        EXPECT_NEAR(input1[i] * input2[i], output[i], EPSILON * 10);
    }
}

TEST_P(CKKSEvaluatorTest, RotationTest)
{
    vector<double> input(evaluator->encoder.slot_count());
    for (size_t i = 0; i < input.size(); i++)
    {
        input[i] = static_cast<double>(i);
    }

    PhantomPlaintext plain, result_plain;
    evaluator->encoder.encode(input, scale, plain);

    PhantomCiphertext cipher;
    evaluator->encryptor.encrypt(plain, cipher);

    int rotation_steps = 3;
    evaluator->evaluator.rotate_vector_inplace(cipher, rotation_steps, *galois_keys);

    evaluator->decryptor.decrypt(cipher, result_plain);

    vector<double> output;
    evaluator->encoder.decode(result_plain, output);

    ASSERT_EQ(input.size(), output.size());
    for (size_t i = 0; i < input.size(); i++)
    {
        size_t expected_index = (i + rotation_steps) % input.size();
        EXPECT_NEAR(input[expected_index], output[i], EPSILON);
    }
}





INSTANTIATE_TEST_CASE_P(
    CKKSEvaluatorTests,
    CKKSEvaluatorTest,
    ::testing::Values(1, 2, 3, 4, 15),
    [](const testing::TestParamInfo<CKKSEvaluatorTest::ParamType> &info)
    {
        return "Alpha_" + std::to_string(info.param);
    });
