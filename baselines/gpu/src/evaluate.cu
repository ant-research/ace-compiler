#include "evaluate.cuh"

#include "rns_bconv.cuh"
#include "scalingvariant.cuh"
#include "util.cuh"
#include <iostream>

using namespace std;
using namespace phantom;
using namespace phantom::util;
using namespace phantom::arith;

// https://github.com/microsoft/SEAL/blob/3a05febe18e8a096668cd82c75190255eda5ca7d/native/src/seal/evaluator.cpp#L24
template <typename T, typename S>
[[nodiscard]] inline bool are_same_scale(const T &value1, const S &value2) noexcept
{
    return are_close<double>(value1.scale(), value2.scale());
}
// negate a ciphertext in RNS form
static void negate_internal(const PhantomContext &context, PhantomCiphertext &encrypted, const cudaStream_t &stream)
{
    // Extract encryption parameters.
    auto &context_data = context.get_context_data(encrypted.chain_index());
    auto &parms = context_data.parms();
    auto &coeff_modulus = parms.coeff_modulus();
    const auto coeff_mod_size = coeff_modulus.size();
    const auto poly_degree = parms.poly_modulus_degree();
    const auto base_rns = context.gpu_rns_tables().modulus();
    const auto rns_coeff_count = poly_degree * coeff_mod_size;

    uint64_t gridDimGlb = rns_coeff_count / blockDimGlb.x;
    for (size_t i = 0; i < encrypted.size(); i++)
    {
        negate_rns_poly<<<gridDimGlb, blockDimGlb, 0, stream>>>(
            encrypted.data() + i * rns_coeff_count, base_rns,
            encrypted.data() + i * rns_coeff_count,
            poly_degree,
            coeff_mod_size);
    }
}

void negate_inplace(const PhantomContext &context, PhantomCiphertext &encrypted,
                    const phantom::util::cuda_stream_wrapper &stream_wrapper)
{
    negate_internal(context, encrypted, stream_wrapper.get_stream());
}

/**
 * Adds two ciphertexts. This function adds together encrypted1 and encrypted2 and stores the result in encrypted1.
 * @param[in] encrypted1 The first ciphertext to add
 * @param[in] encrypted2 The second ciphertext to add
 */
void add_inplace(const PhantomContext &context, PhantomCiphertext &encrypted1, const PhantomCiphertext &encrypted2,
                 const phantom::util::cuda_stream_wrapper &stream_wrapper)
{
    if (encrypted1.chain_index() != encrypted2.chain_index())
    {
        throw std::invalid_argument("encrypted1 and encrypted2 parameter mismatch");
    }
    if (encrypted1.is_ntt_form() != encrypted2.is_ntt_form())
    {
        throw std::invalid_argument("NTT form mismatch");
    }
    // if (!are_same_scale(encrypted1, encrypted2)) {
    //     throw std::invalid_argument("scale mismatch");
    // }
    if (encrypted1.size() != encrypted2.size())
    {
        throw std::invalid_argument("poly number mismatch");
    }

    const auto &s = stream_wrapper.get_stream();

    // Extract encryption parameters.
    auto &context_data = context.get_context_data(encrypted1.chain_index());
    auto &parms = context_data.parms();
    auto &coeff_modulus = parms.coeff_modulus();
    auto &plain_modulus = parms.plain_modulus();
    auto coeff_modulus_size = coeff_modulus.size();
    auto poly_degree = context.gpu_rns_tables().n();
    auto base_rns = context.gpu_rns_tables().modulus();
    auto rns_coeff_count = poly_degree * coeff_modulus_size;
    size_t encrypted1_size = encrypted1.size();
    size_t encrypted2_size = encrypted2.size();
    size_t max_size = max(encrypted1_size, encrypted2_size);
    size_t min_size = min(encrypted1_size, encrypted2_size);

    uint64_t gridDimGlb = rns_coeff_count / blockDimGlb.x;

    // Prepare destination
    encrypted1.resize(context, context_data.chain_index(), max_size, s);
    for (size_t i = 0; i < min_size; i++)
    {
        add_rns_poly<<<gridDimGlb, blockDimGlb, 0, s>>>(
            encrypted1.data() + i * rns_coeff_count, encrypted2.data() + i * rns_coeff_count, base_rns,
            encrypted1.data() + i * rns_coeff_count, poly_degree, coeff_modulus_size);
    }
    if (encrypted1_size < encrypted2_size)
    {
        cudaMemcpyAsync(encrypted1.data() + min_size * rns_coeff_count,
                        encrypted2.data() + min_size * rns_coeff_count,
                        (encrypted2_size - encrypted1_size) * rns_coeff_count * sizeof(uint64_t),
                        cudaMemcpyDeviceToDevice, s);
    }
}

// TODO: fixme
void add_many(const PhantomContext &context, const vector<PhantomCiphertext> &encrypteds,
              PhantomCiphertext &destination, const phantom::util::cuda_stream_wrapper &stream_wrapper)
{
    const auto &s = stream_wrapper.get_stream();

    if (encrypteds.empty())
    {
        throw std::invalid_argument("encrypteds cannot be empty");
    }
    for (size_t i = 0; i < encrypteds.size(); i++)
    {
        if (&encrypteds[i] == &destination)
        {
            throw std::invalid_argument("encrypteds must be different from destination");
        }
        if (encrypteds[0].chain_index() != encrypteds[i].chain_index())
        {
            throw invalid_argument("encrypteds parameter mismatch");
        }
        if (encrypteds[0].is_ntt_form() != encrypteds[i].is_ntt_form())
        {
            throw std::invalid_argument("NTT form mismatch");
        }
        // if (!are_same_scale(encrypteds[0], encrypteds[i])) {
        //     throw std::invalid_argument("scale mismatch");
        // }
        if (encrypteds[0].size() != encrypteds[i].size())
        {
            throw std::invalid_argument("poly number mismatch");
        }
    }

    auto &context_data = context.get_context_data(encrypteds[0].chain_index());
    auto &parms = context_data.parms();
    auto &coeff_modulus = parms.coeff_modulus();
    auto coeff_mod_size = coeff_modulus.size();
    auto poly_degree = parms.poly_modulus_degree();
    auto poly_num = encrypteds[0].size();
    auto base_rns = context.gpu_rns_tables().modulus();
    // reduction_threshold = 2 ^ (64 - max modulus bits)
    // max modulus bits = static_cast<uint64_t>(log2(coeff_modulus.front().value())) + 1
    uint64_t reduction_threshold =
        (1 << (bits_per_uint64 - static_cast<uint64_t>(log2(coeff_modulus.front().value())) - 1)) - 1;

    destination.resize(context, encrypteds[0].chain_index(), encrypteds[0].size(), s);
    destination.set_ntt_form(encrypteds[0].is_ntt_form());
    destination.set_scale(encrypteds[0].scale());

    auto enc_device_ptr = make_cuda_auto_ptr<uint64_t *>(encrypteds.size(), s);
    std::vector<uint64_t *> enc_host_ptr(encrypteds.size());
    for (size_t i = 0; i < encrypteds.size(); i++)
    {
        enc_host_ptr[i] = encrypteds[i].data();
    }
    cudaMemcpyAsync(enc_device_ptr.get(), enc_host_ptr.data(), sizeof(uint64_t *) * encrypteds.size(),
                    cudaMemcpyHostToDevice, s);

    uint64_t gridDimGlb = poly_degree * coeff_mod_size / blockDimGlb.x;
    for (size_t i = 0; i < poly_num; i++)
    {
        add_many_rns_poly<<<gridDimGlb, blockDimGlb, 0, s>>>(
            enc_device_ptr.get(), encrypteds.size(), base_rns,
            destination.data(), i, poly_degree, coeff_mod_size,
            reduction_threshold);
    }
}

void sub_inplace(const PhantomContext &context, PhantomCiphertext &encrypted1, const PhantomCiphertext &encrypted2,
                 const bool &negate, const phantom::util::cuda_stream_wrapper &stream_wrapper)
{
    if (encrypted1.parms_id() != encrypted2.parms_id())
    {
        throw invalid_argument("encrypted1 and encrypted2 parameter mismatch");
    }
    if (encrypted1.chain_index() != encrypted2.chain_index())
        throw std::invalid_argument("encrypted1 and encrypted2 parameter mismatch");
    if (encrypted1.is_ntt_form() != encrypted2.is_ntt_form())
        throw std::invalid_argument("NTT form mismatch");
    if (!are_same_scale(encrypted1, encrypted2))
        throw std::invalid_argument("scale mismatch");
    if (encrypted1.size() != encrypted2.size())
        throw std::invalid_argument("poly number mismatch");

    const auto &s = stream_wrapper.get_stream();

    // Extract encryption parameters.
    auto &context_data = context.get_context_data(encrypted1.chain_index());
    auto &parms = context_data.parms();
    auto &coeff_modulus = parms.coeff_modulus();
    auto &plain_modulus = parms.plain_modulus();
    auto coeff_modulus_size = coeff_modulus.size();
    auto poly_degree = parms.poly_modulus_degree();
    auto base_rns = context.gpu_rns_tables().modulus();
    auto rns_coeff_count = poly_degree * coeff_modulus_size;
    size_t encrypted1_size = encrypted1.size();
    size_t encrypted2_size = encrypted2.size();
    size_t max_count = max(encrypted1_size, encrypted2_size);
    size_t min_count = min(encrypted1_size, encrypted2_size);

    uint64_t gridDimGlb = rns_coeff_count / blockDimGlb.x;

    if (negate)
    {
        for (size_t i = 0; i < encrypted1.size(); i++)
        {
            sub_rns_poly<<<gridDimGlb, blockDimGlb, 0, s>>>(
                encrypted2.data() + i * rns_coeff_count, encrypted1.data() + i * rns_coeff_count, base_rns,
                encrypted1.data() + i * rns_coeff_count, poly_degree, coeff_modulus_size);
        }
    }
    else
    {
        for (size_t i = 0; i < encrypted1.size(); i++)
        {
            sub_rns_poly<<<gridDimGlb, blockDimGlb, 0, s>>>(
                encrypted1.data() + i * rns_coeff_count, encrypted2.data() + i * rns_coeff_count, base_rns,
                encrypted1.data() + i * rns_coeff_count, poly_degree, coeff_modulus_size);
        }
    }
}

static void multiply_internal(const PhantomContext &context, PhantomCiphertext &encrypted1,
                              const PhantomCiphertext &encrypted2, const cudaStream_t &stream)
{
    if (!(encrypted1.is_ntt_form() && encrypted2.is_ntt_form()))
        throw invalid_argument("encrypted1 and encrypted2 must be in NTT form");

    if (encrypted1.chain_index() != encrypted2.chain_index())
        throw invalid_argument("encrypted1 and encrypted2 parameter mismatch");

    // Extract encryption parameters.
    auto &context_data = context.get_context_data(encrypted1.chain_index());
    auto &parms = context_data.parms();
    auto base_rns = context.gpu_rns_tables().modulus();
    size_t coeff_mod_size = parms.coeff_modulus().size();
    size_t poly_degree = parms.poly_modulus_degree();
    uint32_t encrypted1_size = encrypted1.size();
    uint32_t encrypted2_size = encrypted2.size();

    // Determine destination.size()
    // Default is 3 (c_0, c_1, c_2)
    uint32_t dest_size = encrypted1_size + encrypted2_size - 1;

    // Size check
    // Prepare destination
    encrypted1.resize(context, encrypted1.chain_index(), dest_size, stream);

    uint64_t gridDimGlb = poly_degree * coeff_mod_size / blockDimGlb.x;

    if (dest_size == 3)
    {
        if (&encrypted1 == &encrypted2)
        {
            // square
            tensor_square_2x2_rns_poly<<<gridDimGlb, blockDimGlb, 0, stream>>>(
                encrypted1.data(), base_rns, encrypted1.data(), poly_degree, coeff_mod_size);
        }
        else
        {
            // standard multiply
            tensor_prod_2x2_rns_poly<<<gridDimGlb, blockDimGlb, 0, stream>>>(
                encrypted1.data(), encrypted2.data(), base_rns, encrypted1.data(), poly_degree, coeff_mod_size);
        }
    }
    else
    {
        tensor_prod_mxn_rns_poly<<<gridDimGlb, blockDimGlb, 0, stream>>>(
            encrypted1.data(), encrypted1_size, encrypted2.data(), encrypted2_size, base_rns, encrypted1.data(),
            dest_size, poly_degree, coeff_mod_size);
    }

    encrypted1.set_scale(encrypted1.scale() * encrypted2.scale());
}

size_t FindLevelsToDrop(const PhantomContext &context, size_t multiplicativeDepth, double dcrtBits, bool isKeySwitch,
                        bool isAsymmetric)
{
    // Extract encryption parameters.
    auto &context_data = context.get_context_data(0);
    auto &parms = context_data.parms();
    auto n = parms.poly_modulus_degree();

    // handle no relin scenario
    size_t gpu_rns_tool_index = 0;
    if (context.using_keyswitching())
    {
        gpu_rns_tool_index = 1;
    }

    auto &rns_tool = context.get_context_data(gpu_rns_tool_index).gpu_rns_tool(); // BFV does not drop modulus
    auto mul_tech = rns_tool.mul_tech();

    if (mul_tech != mul_tech_type::hps_overq_leveled)
        throw invalid_argument("FindLevelsToDrop is only used in HPS over Q Leveled");

    double sigma = distributionParameter;
    double alpha = assuranceMeasure;

    double p = parms.plain_modulus().value();

    uint32_t k = rns_tool.size_P();
    uint32_t numPartQ = rns_tool.v_base_part_Ql_to_compl_part_QlP_conv().size();
    uint32_t thresholdParties = 1;
    // Bkey set to thresholdParties * 1 for ternary distribution
    const double Bkey = thresholdParties;

    double w = pow(2, dcrtBits);

    // Bound of the Gaussian error polynomial
    double Berr = sigma * sqrt(alpha);

    // expansion factor delta
    auto delta = [](uint32_t n) -> double
    { return (2. * sqrt(n)); };

    // norm of fresh ciphertext polynomial (for EXTENDED the noise is reduced to modulus switching noise)
    auto Vnorm = [&](uint32_t n) -> double
    {
        if (isAsymmetric)
            return (1. + delta(n) * Bkey) / 2.;
        return Berr * (1. + 2. * delta(n) * Bkey);
    };

    auto noiseKS = [&](uint32_t n, double logqPrev, double w) -> double
    {
        return k * (numPartQ * delta(n) * Berr + delta(n) * Bkey + 1.0) / 2;
    };

    // function used in the EvalMult constraint
    auto C1 = [&](uint32_t n) -> double
    { return delta(n) * delta(n) * p * Bkey; };

    // function used in the EvalMult constraint
    auto C2 = [&](uint32_t n, double logqPrev) -> double
    {
        return delta(n) * delta(n) * Bkey * Bkey / 2.0 + noiseKS(n, logqPrev, w);
    };

    // main correctness constraint
    auto logqBFV = [&](uint32_t n, double logqPrev) -> double
    {
        if (multiplicativeDepth > 0)
        {
            return log(4 * p) + (multiplicativeDepth - 1) * log(C1(n)) +
                   log(C1(n) * Vnorm(n) + multiplicativeDepth * C2(n, logqPrev));
        }
        return log(p * (4 * (Vnorm(n))));
    };

    // initial values
    double logqPrev = 6. * log(10);
    double logq = logqBFV(n, logqPrev);

    while (fabs(logq - logqPrev) > log(1.001))
    {
        logqPrev = logq;
        logq = logqBFV(n, logqPrev);
    }

    // get an estimate of the error q / (4t)
    double loge = logq / log(2) - 2 - log2(p);

    double logExtra = isKeySwitch ? log2(noiseKS(n, logq, w)) : log2(delta(n));

    // adding the cushon to the error (see Appendix D of https://eprint.iacr.org/2021/204.pdf for details)
    // adjusted empirical parameter to 16 from 4 for threshold scenarios to work correctly, this might need to
    // be further refined
    int32_t levels = std::floor((loge - 2 * multiplicativeDepth - 16 - logExtra) / dcrtBits);
    auto sizeQ = static_cast<int32_t>(rns_tool.base_Q().size());

    if (levels < 0)
        levels = 0;
    else if (levels > sizeQ - 1)
        levels = sizeQ - 1;

    return levels;
}

// encrypted1 = encrypted1 * encrypted2
void multiply_inplace(const PhantomContext &context, PhantomCiphertext &encrypted1, const PhantomCiphertext &encrypted2,
                      const phantom::util::cuda_stream_wrapper &stream_wrapper)
{
    // Verify parameters.
    if (encrypted1.parms_id() != encrypted2.parms_id())
    {
        throw invalid_argument("encrypted1 and encrypted2 parameter mismatch");
    }
    if (encrypted1.chain_index() != encrypted2.chain_index())
        throw std::invalid_argument("encrypted1 and encrypted2 parameter mismatch");
    if (encrypted1.is_ntt_form() != encrypted2.is_ntt_form())
        throw std::invalid_argument("NTT form mismatch");
    // if (!are_same_scale(encrypted1, encrypted2))
    //     throw std::invalid_argument("scale mismatch");
    if (encrypted1.size() != encrypted2.size())
        throw std::invalid_argument("poly number mismatch");

    const auto &s = stream_wrapper.get_stream();
    auto &context_data = context.get_context_data(encrypted1.chain_index());

    multiply_internal(context, encrypted1, encrypted2, s);
}

// encrypted1 = encrypted1 * encrypted2
// relin(encrypted1)
void multiply_and_relin_inplace(const PhantomContext &context, PhantomCiphertext &encrypted1,
                                const PhantomCiphertext &encrypted2, const PhantomRelinKey &relin_keys,
                                const phantom::util::cuda_stream_wrapper &stream_wrapper)
{
    // Verify parameters.
    if (encrypted1.parms_id() != encrypted2.parms_id())
    {
        throw invalid_argument("encrypted1 and encrypted2 parameter mismatch");
    }
    if (encrypted1.chain_index() != encrypted2.chain_index())
        throw std::invalid_argument("encrypted1 and encrypted2 parameter mismatch");
    if (encrypted1.is_ntt_form() != encrypted2.is_ntt_form())
        throw std::invalid_argument("NTT form mismatch");
    // if (!are_same_scale(encrypted1, encrypted2))
    //     throw std::invalid_argument("scale mismatch");
    if (encrypted1.size() != encrypted2.size())
        throw std::invalid_argument("poly number mismatch");

    // Extract encryption parameters.
    auto &context_data = context.get_context_data(encrypted1.chain_index());
    auto &params = context_data.parms();
    auto scheme = params.scheme();
    auto mul_tech = params.mul_tech();

    const auto &s = stream_wrapper.get_stream();

    multiply_internal(context, encrypted1, encrypted2, s);
    relinearize_inplace(context, encrypted1, relin_keys, stream_wrapper);
}

void add_plain_inplace(const PhantomContext &context, PhantomCiphertext &encrypted, const PhantomPlaintext &plain,
                       const phantom::util::cuda_stream_wrapper &stream_wrapper)
{
    const auto &s = stream_wrapper.get_stream();

    // Extract encryption parameters.
    auto &context_data = context.get_context_data(encrypted.chain_index());
    auto &parms = context_data.parms();
    if (parms.scheme() == scheme_type::ckks && !(encrypted.is_ntt_form()))
    {
        throw std::invalid_argument("CKKS encrypted must be in NTT form");
    }

//    if (!are_same_scale(encrypted, plain))
 //   {
        // TODO: be more precious
   //     throw std::invalid_argument("scale mismatch");
   // }
    if (encrypted.chain_index() != plain.chain_index())
    {
        throw invalid_argument("encrypted and plain parameter mismatch");
    }

    auto &coeff_modulus = parms.coeff_modulus();
    auto coeff_mod_size = coeff_modulus.size();
    auto poly_degree = parms.poly_modulus_degree();
    auto base_rns = context.gpu_rns_tables().modulus();

    uint64_t gridDimGlb = poly_degree * coeff_mod_size / blockDimGlb.x;

    add_rns_poly<<<gridDimGlb, blockDimGlb, 0, s>>>(
        encrypted.data(), plain.data(), base_rns, encrypted.data(),
        poly_degree, coeff_mod_size);
}

void sub_plain_inplace(const PhantomContext &context, PhantomCiphertext &encrypted, const PhantomPlaintext &plain,
                       const phantom::util::cuda_stream_wrapper &stream_wrapper)
{

    const auto &s = stream_wrapper.get_stream();
    // Extract encryption parameters.
    auto &context_data = context.get_context_data(encrypted.chain_index());
    auto &parms = context_data.parms();

    if (parms.scheme() == scheme_type::ckks && !(encrypted.is_ntt_form()))
    {
        throw std::invalid_argument("CKKS encrypted must be in NTT form");
    }
    if (!are_same_scale(encrypted, plain))
    {
        throw std::invalid_argument("scale mismatch");
    }

    auto &coeff_modulus = parms.coeff_modulus();
    auto coeff_mod_size = coeff_modulus.size();
    auto poly_degree = parms.poly_modulus_degree();
    auto base_rns = context.gpu_rns_tables().modulus();
    uint64_t gridDimGlb = poly_degree * coeff_mod_size / blockDimGlb.x;

    sub_rns_poly<<<gridDimGlb, blockDimGlb, 0, s>>>(
        encrypted.data(), plain.data(), base_rns, encrypted.data(),
        poly_degree, coeff_mod_size);
}

static void multiply_plain_ntt(const PhantomContext &context, PhantomCiphertext &encrypted,
                               const PhantomPlaintext &plain, const cudaStream_t &stream)
{
    if (encrypted.chain_index() != plain.chain_index())
    {
        throw std::invalid_argument("encrypted and plain parameter mismatch");
    }
    if (encrypted.parms_id() != plain.parms_id())
    {
        throw invalid_argument("encrypted_ntt and plain_ntt parameter mismatch");
    }

    // Extract encryption parameters.
    auto &context_data = context.get_context_data(encrypted.chain_index());
    auto &parms = context_data.parms();
    auto &coeff_modulus = parms.coeff_modulus();
    auto coeff_mod_size = coeff_modulus.size();
    auto poly_degree = parms.poly_modulus_degree();
    auto base_rns = context.gpu_rns_tables().modulus();
    auto rns_coeff_count = poly_degree * coeff_mod_size;

    double new_scale = encrypted.scale() * plain.scale();

    //(c0 * pt, c1 * pt)
    for (size_t i = 0; i < encrypted.size(); i++)
    {
        uint64_t gridDimGlb = poly_degree * coeff_mod_size / blockDimGlb.x;
        multiply_rns_poly<<<gridDimGlb, blockDimGlb, 0, stream>>>(
            encrypted.data() + i * rns_coeff_count, plain.data(), base_rns,
            encrypted.data() + i * rns_coeff_count, poly_degree, coeff_mod_size);
    }

    encrypted.set_scale(new_scale);
}

void multiply_plain_inplace(const PhantomContext &context, PhantomCiphertext &encrypted, const PhantomPlaintext &plain,
                            const phantom::util::cuda_stream_wrapper &stream_wrapper)
{
    const auto &s = stream_wrapper.get_stream();
    // Extract encryption parameters.
    auto &context_data = context.get_context_data(encrypted.chain_index());
    auto &parms = context_data.parms();
    auto scheme = parms.scheme();

    multiply_plain_ntt(context, encrypted, plain, s);
}

void relinearize_inplace(const PhantomContext &context, PhantomCiphertext &encrypted,
                         const PhantomRelinKey &relin_keys, const phantom::util::cuda_stream_wrapper &stream_wrapper)
{
    // Extract encryption parameters.
    auto &context_data = context.get_context_data(encrypted.chain_index());
    auto &parms = context_data.parms();
    size_t decomp_modulus_size = parms.coeff_modulus().size();
    size_t n = parms.poly_modulus_degree();

    // Verify parameters.
    auto scheme = parms.scheme();
    auto encrypted_size = encrypted.size();
    if (encrypted_size != 3)
    {
        throw invalid_argument("destination_size must be 3");
    }

    if (scheme == scheme_type::ckks && !encrypted.is_ntt_form())
    {
        throw invalid_argument("CKKS encrypted must be in NTT form");
    }

    uint64_t *c2 = encrypted.data() + 2 * decomp_modulus_size * n;

    const auto &s = stream_wrapper.get_stream();

    keyswitch_inplace(context, encrypted, c2, relin_keys, true, s);

    // update the encrypted
    encrypted.resize(2, decomp_modulus_size, n, s);
}

static void mod_switch_scale_to_next(const PhantomContext &context, const PhantomCiphertext &encrypted,
                                     PhantomCiphertext &destination, const cudaStream_t &stream)
{
    // Assuming at this point encrypted is already validated.
    auto &context_data = context.get_context_data(encrypted.chain_index());
    auto &parms = context_data.parms();
    auto &rns_tool = context_data.gpu_rns_tool();

    // Extract encryption parameters.
    size_t coeff_mod_size = parms.coeff_modulus().size();
    size_t poly_degree = parms.poly_modulus_degree();
    size_t encrypted_size = encrypted.size();

    auto next_index_id = context.get_next_index(encrypted.chain_index());
    auto &next_context_data = context.get_context_data(next_index_id);
    auto &next_parms = next_context_data.parms();

    auto encrypted_copy = make_cuda_auto_ptr<uint64_t>(encrypted_size * coeff_mod_size * poly_degree, stream);
    cudaMemcpyAsync(encrypted_copy.get(), encrypted.data(),
                    encrypted_size * coeff_mod_size * poly_degree * sizeof(uint64_t),
                    cudaMemcpyDeviceToDevice, stream);

    // resize and empty the data array
    destination.resize(context, next_index_id, encrypted_size, stream);

    rns_tool.divide_and_round_q_last_ntt(encrypted_copy.get(), encrypted_size, context.gpu_rns_tables(),
                                         destination.data(), stream);

    // Set other attributes
    destination.set_ntt_form(encrypted.is_ntt_form());
    destination.set_scale(encrypted.scale() / static_cast<double>(parms.coeff_modulus().back().value()));
}

static void mod_switch_drop_to_next(const PhantomContext &context, const PhantomCiphertext &encrypted,
                                    PhantomCiphertext &destination, const cudaStream_t &stream)
{
    // Assuming at this point encrypted is already validated.
    auto &context_data = context.get_context_data(encrypted.chain_index());
    auto &parms = context_data.parms();
    auto coeff_modulus_size = parms.coeff_modulus().size();
    size_t N = parms.poly_modulus_degree();

    // Extract encryption parameters.
    auto next_chain_index = encrypted.chain_index() + 1;
    auto &next_context_data = context.get_context_data(next_chain_index);
    auto &next_parms = next_context_data.parms();

    // q_1,...,q_{k-1}
    size_t encrypted_size = encrypted.size();
    size_t next_coeff_modulus_size = next_parms.coeff_modulus().size();

    if (&encrypted == &destination)
    {
        auto temp = std::move(destination.data_ptr());
        destination.data_ptr() = make_cuda_auto_ptr<uint64_t>(encrypted_size * next_coeff_modulus_size * N, stream);
        for (size_t i{0}; i < encrypted_size; i++)
        {
            auto temp_iter = temp.get() + i * coeff_modulus_size * N;
            auto encrypted_iter = encrypted.data() + i * next_coeff_modulus_size * N;
            cudaMemcpyAsync(encrypted_iter, temp_iter, next_coeff_modulus_size * N * sizeof(uint64_t),
                            cudaMemcpyDeviceToDevice, stream);
        }
        // Set other attributes
        destination.set_chain_index(next_chain_index);
        destination.set_coeff_modulus_size(next_coeff_modulus_size);
    }
    else
    {
        // Resize destination before writing
        destination.resize(context, next_chain_index, encrypted_size, stream);
        // Copy data over to destination; only copy the RNS components relevant after modulus drop
        for (size_t i = 0; i < encrypted_size; i++)
        {
            auto destination_iter = destination.data() + i * next_coeff_modulus_size * N;
            auto encrypted_iter = encrypted.data() + i * coeff_modulus_size * N;
            cudaMemcpyAsync(destination_iter, encrypted_iter, next_coeff_modulus_size * N * sizeof(uint64_t),
                            cudaMemcpyDeviceToDevice, stream);
        }
        // Set other attributes
        destination.set_scale(encrypted.scale());
        destination.set_ntt_form(encrypted.is_ntt_form());
    }
}

void mod_switch_to_next_inplace(const PhantomContext &context, PhantomPlaintext &plain,
                                const phantom::util::cuda_stream_wrapper &stream_wrapper)
{
    const auto &s = stream_wrapper.get_stream();

    auto &context_data = context.get_context_data(context.get_first_index());
    auto &parms = context_data.parms();
    auto coeff_modulus_size = parms.coeff_modulus().size();

    auto max_chain_index = coeff_modulus_size;
    if (plain.chain_index() == max_chain_index)
    {
        throw invalid_argument("end of modulus switching chain reached");
    }

    auto next_chain_index = plain.chain_index() + 1;
    auto &next_context_data = context.get_context_data(next_chain_index);
    auto &next_parms = next_context_data.parms();

    // q_1,...,q_{k-1}
    auto &next_coeff_modulus = next_parms.coeff_modulus();
    size_t next_coeff_modulus_size = next_coeff_modulus.size();
    size_t coeff_count = next_parms.poly_modulus_degree();

    // Compute destination size first for exception safety
    auto dest_size = next_coeff_modulus_size * coeff_count;

    auto data_copy = std::move(plain.data_ptr());
    plain.data_ptr() = make_cuda_auto_ptr<uint64_t>(dest_size, s);
    cudaMemcpyAsync(plain.data(), data_copy.get(), dest_size * sizeof(uint64_t), cudaMemcpyDeviceToDevice, s);

    plain.set_chain_index(next_chain_index);
}

PhantomCiphertext mod_switch_to_next(const PhantomContext &context, const PhantomCiphertext &encrypted,
                                     const phantom::util::cuda_stream_wrapper &stream_wrapper)
{
    const auto &s = stream_wrapper.get_stream();

    auto &context_data = context.get_context_data(context.get_first_index());
    auto &parms = context_data.parms();
    auto coeff_modulus_size = parms.coeff_modulus().size();
    auto scheme = parms.scheme();

    auto max_chain_index = coeff_modulus_size;
    if (encrypted.chain_index() == max_chain_index)
    {
        throw invalid_argument("end of modulus switching chain reached");
    }

    // Modulus switching with scaling
    PhantomCiphertext destination;
    mod_switch_drop_to_next(context, encrypted, destination, s);
    return destination;
}

PhantomCiphertext rescale_to_next(const PhantomContext &context, const PhantomCiphertext &encrypted,
                                  const phantom::util::cuda_stream_wrapper &stream_wrapper)
{
    const auto &s = stream_wrapper.get_stream();

    auto &context_data = context.get_context_data(context.get_first_index());
    auto &parms = context_data.parms();
    auto max_chain_index = parms.coeff_modulus().size();
    auto scheme = parms.scheme();

    // Verify parameters.
    if (encrypted.chain_index() == max_chain_index)
    {
        throw invalid_argument("end of modulus switching chain reached");
    }

    // Modulus switching with scaling
    PhantomCiphertext destination;
    mod_switch_scale_to_next(context, encrypted, destination, s);
    return destination;
}

void apply_galois_inplace(const PhantomContext &context, PhantomCiphertext &encrypted, uint32_t galois_elt,
                          const PhantomGaloisKey &galois_keys, const phantom::util::cuda_stream_wrapper &stream_wrapper)
{
    auto &context_data = context.get_context_data(encrypted.chain_index());
    auto &parms = context_data.parms();
    auto &coeff_modulus = parms.coeff_modulus();
    size_t N = parms.poly_modulus_degree();
    size_t coeff_modulus_size = coeff_modulus.size();
    size_t encrypted_size = encrypted.size();
    if (encrypted_size > 2)
    {
        throw invalid_argument("encrypted size must be 2");
    }

    // Use key_context_data where permutation tables exist since previous runs.
    auto &key_galois_tool = context.key_galois_tool_;
    auto &galois_elts = key_galois_tool->galois_elts();

    auto iter = find(galois_elts.begin(), galois_elts.end(), galois_elt);
    if (iter == galois_elts.end())
    {
        throw std::invalid_argument("Galois elt not present");
    }
    auto galois_elt_index = std::distance(galois_elts.begin(), iter);

    const auto &s = stream_wrapper.get_stream();

    auto temp = make_cuda_auto_ptr<uint64_t>(coeff_modulus_size * N, s);

    auto c0 = encrypted.data();
    auto c1 = encrypted.data() + encrypted.coeff_modulus_size() * encrypted.poly_modulus_degree();

    // !!! DO NOT CHANGE EXECUTION ORDER!!
    // First transform c0
    key_galois_tool->apply_galois_ntt(c0, coeff_modulus_size, galois_elt_index, temp.get(), s);
    // Copy result to c0
    cudaMemcpyAsync(c0, temp.get(), coeff_modulus_size * N * sizeof(uint64_t), cudaMemcpyDeviceToDevice, s);
    // Next transform c1
    key_galois_tool->apply_galois_ntt(c1, coeff_modulus_size, galois_elt_index, temp.get(), s);

    // Wipe c1
    cudaMemsetAsync(c1, 0, coeff_modulus_size * N * sizeof(uint64_t), s);

    // END: Apply Galois for each ciphertext
    // REORDERING IS SAFE NOW
    // Calculate (temp * galois_key[0], temp * galois_key[1]) + (c0, 0)
    keyswitch_inplace(context, encrypted, temp.get(), galois_keys.get_relin_keys(galois_elt_index), false, s);
}

// TODO: remove recursive chain
static void rotate_internal(const PhantomContext &context, PhantomCiphertext &encrypted, int step,
                            const PhantomGaloisKey &galois_key,
                            const phantom::util::cuda_stream_wrapper &stream_wrapper)
{
    auto &context_data = context.get_context_data(encrypted.chain_index());

    // Is there anything to do?
    if (step == 0)
    {
        return;
    }

    size_t coeff_count = context_data.parms().poly_modulus_degree();
    auto &key_galois_tool = context.key_galois_tool_;
    auto &galois_elts = key_galois_tool->galois_elts();
    auto step_galois_elt = key_galois_tool->get_elt_from_step(step);

    auto iter = find(galois_elts.begin(), galois_elts.end(), step_galois_elt);
    if (iter != galois_elts.end())
    {
        auto galois_elt_index = iter - galois_elts.begin();
        // Perform rotation and key switching
        apply_galois_inplace(context, encrypted, galois_elts[galois_elt_index], galois_key, stream_wrapper);
    }
    else
    {
        // Convert the steps to NAF: guarantees using smallest HW
        vector<int> naf_step = naf(step);

        // If naf_steps contains only one element, then this is a power-of-two
        // rotation and we would have expected not to get to this part of the
        // if-statement.
        if (naf_step.size() == 1)
        {
            throw invalid_argument("Galois key not present");
        }
        for (auto temp_step : naf_step)
        {
            if (static_cast<size_t>(abs(temp_step)) != (coeff_count >> 1))
            {
                rotate_internal(context, encrypted, temp_step, galois_key, stream_wrapper);
            }
        }
    }
}

void rotate_inplace(const PhantomContext &context, PhantomCiphertext &encrypted, int step,
                    const PhantomGaloisKey &galois_key,
                    const phantom::util::cuda_stream_wrapper &stream_wrapper)
{
    if (context.key_context_data().parms().scheme() != phantom::scheme_type::ckks)
    {
        throw std::logic_error("unsupported scheme");
    }
    rotate_internal(context, encrypted, step, galois_key, stream_wrapper);
}


void complex_conjugate_inplace(const PhantomContext &context, PhantomCiphertext &encrypted,
                               const PhantomGaloisKey &galois_key,
                               const phantom::util::cuda_stream_wrapper &stream_wrapper)
{
    if (context.key_context_data().parms().scheme() != phantom::scheme_type::ckks)
    {
        throw std::logic_error("unsupported scheme");
    }
    auto &key_galois_tool = context.key_galois_tool_;
    apply_galois_inplace(context, encrypted, key_galois_tool->get_elt_from_step(0), galois_key, stream_wrapper);
}
