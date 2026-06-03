//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstdint>
#include <string>

#include "context.h"

#include "common/common.h"
#include "common/error.h"
#include "common/io_api.h"
#include "common/rt_api.h"
#include "common/rtlib_timing.h"


using namespace phantom;
using namespace phantom::arith;
using namespace phantom::util;

#ifdef ENABLE_PERFORMANCE_STATS
#include <unordered_map>

std::unordered_map<std::string, OperationStats> operation_stats;
#endif

PHANTOM_CONTEXT& PHANTOM_CONTEXT::Context() {
    static PHANTOM_CONTEXT instance;
    return instance;
}

PHANTOM_CONTEXT::PHANTOM_CONTEXT() {
    bool with_bootstrap = Need_bts();
    CKKS_PARAMS *prog_param = Get_context_params();
    IS_TRUE(prog_param->_provider == LIB_PHANTOM, "provider is not PHANTOM");

    if (with_bootstrap) {
        _slot_count = prog_param->_poly_degree / 2;
    }

    EncryptionParameters parms(scheme_type::ckks);
    uint32_t degree = prog_param->_poly_degree;
    parms.set_poly_modulus_degree(degree);

    std::vector<int> bits;
    bits.push_back(prog_param->_first_mod_size);

    if (with_bootstrap) {
        uint32_t bts_required_level = 14;
        uint32_t bts_remaining_level = prog_param->_mul_depth - 14;
        for (uint32_t i = 0; i < bts_remaining_level; ++i) {
            bits.push_back(prog_param->_scaling_mod_size);
        }
        for (uint32_t i = 0; i < bts_required_level; ++i) {
            bits.push_back(prog_param->_first_mod_size);
        }
        constexpr size_t special_modulus_size = 4;
        for (size_t i = 0; i < special_modulus_size; i++) {
            bits.push_back(prog_param->_first_mod_size);
        }
        parms.set_secret_key_hamming_weight(192);
        parms.set_special_modulus_size(special_modulus_size);
        _num_prime_parts = bits.size() - special_modulus_size + 1;
    } else {
        for (uint32_t i = 0; i < prog_param->_mul_depth; ++i) {
            bits.push_back(prog_param->_scaling_mod_size);
        }
        bits.push_back(prog_param->_first_mod_size);
        parms.set_secret_key_hamming_weight(prog_param->_hamming_weight);
        _num_prime_parts = bits.size();
    }

    phantom::arith::sec_level_type sec = phantom::arith::sec_level_type::tc128;
    switch (prog_param->_sec_level) {
    case 128: sec = phantom::arith::sec_level_type::tc128; break;
    case 192: sec = phantom::arith::sec_level_type::tc192; break;
    case 256: sec = phantom::arith::sec_level_type::tc256; break;
    default: sec = phantom::arith::sec_level_type::none; break;
    }
    if (degree < 4096 && sec != phantom::arith::sec_level_type::none) {
        DEV_WARN("WARNING: degree %d too small, reset security level to none\n", degree);
        sec = phantom::arith::sec_level_type::none;
    }

    _scaling_mod_size = prog_param->_scaling_mod_size;
    _key_mgr.Init(with_bootstrap);
    _codec._encoder = std::make_unique<PhantomCKKSEncoder>(*_key_mgr.ctx());
    _codec._evaluator = std::make_unique<CKKSEvaluator>(
        _key_mgr.ctx(), _key_mgr.pk(), _key_mgr.sk(),
        _codec._encoder.get(), _key_mgr.rlk(), _key_mgr.rtk(),
        std::pow(2.0, _scaling_mod_size));

    if (with_bootstrap) {
        long boundary_K = 25;
        long deg = 59;
        long scale_factor = 2;
        long inverse_deg = 1;
        long loge = 10;
        int log_slot_count = 15;

        _bootstrap.Set(BTS_SLOTS::POW_15, std::make_unique<Bootstrapper>(
            loge, 15, log_slot_count, prog_param->_mul_depth, std::pow(2.0, _scaling_mod_size),
            boundary_K, deg, scale_factor, inverse_deg, _codec._evaluator.get()));
        _bootstrap.Set(BTS_SLOTS::POW_14, std::make_unique<Bootstrapper>(
            loge, 14, log_slot_count, prog_param->_mul_depth, std::pow(2.0, _scaling_mod_size),
            boundary_K, deg, scale_factor, inverse_deg, _codec._evaluator.get()));
        _bootstrap.Set(BTS_SLOTS::POW_13, std::make_unique<Bootstrapper>(
            loge, 13, log_slot_count, prog_param->_mul_depth, std::pow(2.0, _scaling_mod_size),
            boundary_K, deg, scale_factor, inverse_deg, _codec._evaluator.get()));
        _bootstrap.Set(BTS_SLOTS::POW_12, std::make_unique<Bootstrapper>(
            loge, 12, log_slot_count, prog_param->_mul_depth, std::pow(2.0, _scaling_mod_size),
            boundary_K, deg, scale_factor, inverse_deg, _codec._evaluator.get()));

        _bootstrap.Get(BTS_SLOTS::POW_15)->prepare_mod_polynomial();
        _bootstrap.Get(BTS_SLOTS::POW_14)->prepare_mod_polynomial();
        _bootstrap.Get(BTS_SLOTS::POW_13)->prepare_mod_polynomial();
        _bootstrap.Get(BTS_SLOTS::POW_12)->prepare_mod_polynomial();

        vector<int> gal_steps_vector;
        gal_steps_vector.push_back(0);
        for (int i = 0; i < log_slot_count; i++) {
            gal_steps_vector.push_back((1 << i));
        }

        _bootstrap.Get(BTS_SLOTS::POW_15)->addLeftRotKeys_Linear_to_vector_3(gal_steps_vector);
        _bootstrap.Get(BTS_SLOTS::POW_14)->addLeftRotKeys_Linear_to_vector_3(gal_steps_vector);
        _bootstrap.Get(BTS_SLOTS::POW_13)->addLeftRotKeys_Linear_to_vector_3(gal_steps_vector);
        _bootstrap.Get(BTS_SLOTS::POW_12)->addLeftRotKeys_Linear_to_vector_3(gal_steps_vector);

        _codec._evaluator->decryptor.create_galois_keys_from_steps(
            gal_steps_vector, *(_codec._evaluator.get()->galois_keys));

        _bootstrap.Get(BTS_SLOTS::POW_15)->slot_vec.push_back(15);
        _bootstrap.Get(BTS_SLOTS::POW_14)->slot_vec.push_back(14);
        _bootstrap.Get(BTS_SLOTS::POW_13)->slot_vec.push_back(13);
        _bootstrap.Get(BTS_SLOTS::POW_12)->slot_vec.push_back(12);

        _bootstrap.Get(BTS_SLOTS::POW_15)->generate_LT_coefficient_3();
        _bootstrap.Get(BTS_SLOTS::POW_14)->generate_LT_coefficient_3();
        _bootstrap.Get(BTS_SLOTS::POW_13)->generate_LT_coefficient_3();
        _bootstrap.Get(BTS_SLOTS::POW_12)->generate_LT_coefficient_3();
    } else {
        vector<int> rotation_keys(prog_param->_rot_idxs, prog_param->_rot_idxs + prog_param->_num_rot_idx);
        vector<int> gal_steps_vector;
        for (auto rot : rotation_keys) {
            if (find(gal_steps_vector.begin(), gal_steps_vector.end(), rot) == gal_steps_vector.end())
                gal_steps_vector.push_back(rot);
        }
        _codec._evaluator->decryptor.create_galois_keys_from_steps(
            gal_steps_vector, *(_codec._evaluator.get()->galois_keys));
    }

    printf("ckks_param: _provider = %d, _poly_degree = %d, _sec_level = %ld, "
           "mul_depth = %ld, _first_mod_size = %ld, _scaling_mod_size = %ld, "
           "_num_q_parts = %ld, _num_rot_idx = %ld, _num_prime_parts = %ld\n",
           prog_param->_provider, prog_param->_poly_degree, prog_param->_sec_level,
           prog_param->_mul_depth, prog_param->_first_mod_size,
           prog_param->_scaling_mod_size, prog_param->_num_q_parts,
           prog_param->_num_rot_idx, _num_prime_parts);

    // Initialize pt_mgr with rt data file if available
    RT_DATA_INFO* data_info = Get_rt_data_info();
    if (data_info != nullptr) {
        _pt_mgr.Init(data_info->_file_name);
    }
}

PHANTOM_CONTEXT::~PHANTOM_CONTEXT() {
}

void KEY_MANAGER::Init(bool with_bootstrap) {
    CKKS_PARAMS *prog_param = Get_context_params();
    EncryptionParameters parms(scheme_type::ckks);
    uint32_t degree = prog_param->_poly_degree;
    parms.set_poly_modulus_degree(degree);

    std::vector<int> bits;
    bits.push_back(prog_param->_first_mod_size);

    if (with_bootstrap) {
        uint32_t bts_required_level = 14;
        uint32_t bts_remaining_level = prog_param->_mul_depth - 14;
        for (uint32_t i = 0; i < bts_remaining_level; ++i) {
            bits.push_back(prog_param->_scaling_mod_size);
        }
        for (uint32_t i = 0; i < bts_required_level; ++i) {
            bits.push_back(prog_param->_first_mod_size);
        }
        constexpr size_t special_modulus_size = 4;
        for (size_t i = 0; i < special_modulus_size; i++) {
            bits.push_back(prog_param->_first_mod_size);
        }
        parms.set_secret_key_hamming_weight(192);
        parms.set_special_modulus_size(special_modulus_size);
    } else {
        for (uint32_t i = 0; i < prog_param->_mul_depth; ++i) {
            bits.push_back(prog_param->_scaling_mod_size);
        }
        bits.push_back(prog_param->_first_mod_size);
        parms.set_secret_key_hamming_weight(prog_param->_hamming_weight);
    }

    parms.set_coeff_modulus(phantom::arith::CoeffModulus::Create(degree, bits));

    _ctx = std::make_unique<PhantomContext>(parms);
    _sk = std::make_unique<PhantomSecretKey>(*_ctx);
    _pk = std::make_unique<PhantomPublicKey>(_sk->gen_publickey(*_ctx));
    _rlk = std::make_unique<PhantomRelinKey>(_sk->gen_relinkey(*_ctx));
    _rtk = std::make_unique<PhantomGaloisKey>();
}

void PHANTOM_CONTEXT::Prepare_input(TENSOR *input, const char *name) {
    size_t len = TENSOR_SIZE(input);
    std::vector<double> vec(input->_vals, input->_vals + len);
    CKKS_PARAMS *prog_param = Get_context_params();
    PhantomPlaintext pt;
    int chain_index = _num_prime_parts - prog_param->_input_level;
    double encode_scale = std::pow(2.0, _scaling_mod_size);
    _codec._evaluator->encoder.encode(vec, chain_index, encode_scale, pt);
    PhantomCiphertext *ct = new PhantomCiphertext;
    _codec._evaluator->encryptor.encrypt(pt, *ct);
    Io_set_input(name, 0, ct);
}

void PHANTOM_CONTEXT::Set_output_data(const char *name, size_t idx, PhantomCiphertext &ct) {
    Io_set_output(name, idx, new PhantomCiphertext(std::move(ct)));
}

PhantomCiphertext PHANTOM_CONTEXT::Get_input_data(const char *name, size_t idx) {
    PhantomCiphertext *data = (PhantomCiphertext *)Io_get_input(name, idx);
    IS_TRUE(data != nullptr, "not find data");
    PhantomCiphertext ret = std::move(*data);
    delete data;
    return ret;
}

double *PHANTOM_CONTEXT::Handle_output(const char *name, size_t idx) {
    PhantomCiphertext *data = (PhantomCiphertext *)Io_get_output(name, idx);
    IS_TRUE(data != nullptr, "not find data");
    PhantomPlaintext pt;
    _codec._evaluator->decryptor.decrypt(*data, pt);
    std::vector<double> vec;
    _codec._evaluator->encoder.decode(pt, vec);
    double *msg = (double *)malloc(sizeof(double) * vec.size());
    memcpy(msg, vec.data(), sizeof(double) * vec.size());
    delete data;
    return msg;
}

void PHANTOM_CONTEXT::Encode_float(PhantomPlaintext &pt, const float *input, size_t len, SCALE_T scale,
                      LEVEL_T level) {
    std::vector<double> vec(input, input + len);
    _codec._evaluator->encoder.encode(vec, _num_prime_parts - level, std::pow(2.0, _scaling_mod_size * scale), pt);
}

void PHANTOM_CONTEXT::Encode_float_cst_lvl(PhantomPlaintext &pt, const float *input, size_t len,
                              SCALE_T scale, int level) {
    std::vector<double> vec(input, input + len);
    auto &context_data = _key_mgr.ctx()->get_context_data(level);

    _codec._encoder->encode(*_key_mgr.ctx(), vec,
                     std::pow(2.0, _scaling_mod_size * scale), pt, context_data.chain_index());
}

void PHANTOM_CONTEXT::Encode_float_mask(PhantomPlaintext &pt, float input, size_t len, SCALE_T scale,
                           LEVEL_T level) {
    std::vector<double> vec(len, input);
    _codec._evaluator->encoder.encode(vec, _num_prime_parts - level, std::pow(2.0, _scaling_mod_size * scale), pt);
}

void PHANTOM_CONTEXT::Encode_float_mask_cst_lvl(PhantomPlaintext &pt, float input, size_t len,
                                   SCALE_T scale, int level) {
    std::vector<double> vec(len, input);
    auto &context_data = _key_mgr.ctx()->get_context_data(level);
    _codec._encoder->encode(*_key_mgr.ctx(), vec,
                     std::pow(2.0, _scaling_mod_size * scale), pt, context_data.chain_index());
}

void PHANTOM_CONTEXT::Decrypt(PhantomCiphertext &ct, std::vector<double> &vec) {
    PhantomPlaintext pt;
    _codec._evaluator->decryptor.decrypt(ct, pt);
    _codec._evaluator->encoder.decode(pt, vec);
}

void PHANTOM_CONTEXT::Decode(PhantomPlaintext &pt, std::vector<double> &vec) {
    _codec._evaluator->encoder.decode(pt, vec);
}

void PHANTOM_CONTEXT::Equal_level(CIPHERTEXT &op1, CIPHERTEXT &op2) {
    auto level_1 = op1.chain_index();
    auto level_2 = op2.chain_index();
    if (level_1 != level_2) {
        if (level_1 < level_2) {
            _codec._evaluator->evaluator.mod_switch_to_inplace(op1, level_2);
        }
        else {
            _codec._evaluator->evaluator.mod_switch_to_inplace(op2, level_1);
        }
    }
}

void PHANTOM_CONTEXT::Equal_level(CIPHERTEXT &op1, PLAINTEXT &op2) {
    auto level_1 = op1.chain_index();
    auto level_2 = op2.chain_index();
    if (level_1 != level_2)
    {
        if (level_1 < level_2) {
            _codec._evaluator->evaluator.mod_switch_to_inplace(op1, level_2);
        }
        else {
            _codec._evaluator->evaluator.mod_switch_to_inplace(op2, level_1);
        }
    }
}

void PHANTOM_CONTEXT::Add(PhantomCiphertext &res, const PhantomCiphertext &op1, const PhantomCiphertext &op2) {
    PhantomCiphertext tmp1 = op1;
    PhantomCiphertext tmp2 = op2;
    Equal_level(tmp1, tmp2);
    _codec._evaluator->evaluator.add(tmp1, tmp2, res);
}

void PHANTOM_CONTEXT::Add(PhantomCiphertext &res, const PhantomCiphertext &op1, const PhantomPlaintext &op2) {
    PhantomCiphertext tmp1 = op1;
    PhantomPlaintext tmp2 = op2;
    Equal_level(tmp1, tmp2);
    _codec._evaluator->evaluator.add_plain(tmp1, tmp2, res);
}

void PHANTOM_CONTEXT::Add_const(PhantomCiphertext &res, const PhantomCiphertext &op1, double op2) {
    PhantomPlaintext pt;
    _codec._evaluator->encoder.encode(op2, op1.chain_index(), op1.scale(), pt);
    if (&res == &op1) {
        _codec._evaluator->evaluator.add_plain_inplace(res, pt);
    }
    else {
        _codec._evaluator->evaluator.add_plain(op1, pt, res);
    }
}

void PHANTOM_CONTEXT::Mul(PhantomCiphertext &res, const PhantomCiphertext &op1, const PhantomCiphertext &op2) {
    PhantomCiphertext tmp1 = op1;
    PhantomCiphertext tmp2 = op2;
    Equal_level(tmp1, tmp2);
    _codec._evaluator->evaluator.multiply(tmp1, tmp2, res);
}

void PHANTOM_CONTEXT::Mul(PhantomCiphertext &res, const PhantomCiphertext &op1, const PhantomPlaintext &op2) {
    PhantomCiphertext tmp1 = op1;
    PhantomPlaintext tmp2 = op2;
    Equal_level(tmp1, tmp2);
    _codec._evaluator->evaluator.multiply_plain(tmp1, tmp2, res);
}

void PHANTOM_CONTEXT::Mul_const(PhantomCiphertext &res, const PhantomCiphertext &op1, double op2) {
    PhantomPlaintext pt;
    _codec._evaluator->encoder.encode(op2, op1.chain_index(), op1.scale(), pt);
    if (&res == &op1) {
        _codec._evaluator->evaluator.multiply_plain_inplace(res, pt);
    }
    else {
        _codec._evaluator->evaluator.multiply_plain(op1, pt, res);
    }
}

void PHANTOM_CONTEXT::Rotate(PhantomCiphertext &res, const PhantomCiphertext &op1, int step) {
    if (step < 0) {
        step = _slot_count + step;
    }
    if (&res == &op1) {
        _codec._evaluator->evaluator.rotate_vector_inplace(res, step, _key_mgr.Rotate_key());
    }
    else {
        _codec._evaluator->evaluator.rotate_vector(const_cast<PhantomCiphertext &>(op1), step, _key_mgr.Rotate_key(), res);
    }
}

void PHANTOM_CONTEXT::Rescale(PhantomCiphertext &res, const PhantomCiphertext &op1) {
    if (&res == &op1) {
        _codec._evaluator->evaluator.rescale_to_next_inplace(res);
    }
    else {
        _codec._evaluator->evaluator.rescale_to_next(op1, res);
    }
}

void PHANTOM_CONTEXT::Mod_switch(PhantomCiphertext &res, const PhantomCiphertext &op1) {
    if (&res == &op1) {
        _codec._evaluator->evaluator.mod_switch_to_next_inplace(res);
    }
    else {
        _codec._evaluator->evaluator.mod_switch_to_inplace(res, op1.chain_index());
    }
}

void PHANTOM_CONTEXT::Relin(PhantomCiphertext &res, const PhantomCiphertext &op1) {
    if (&res == &op1) {
        _codec._evaluator->evaluator.relinearize_inplace(res, _key_mgr.Relin_key());
    }
    else {
        _codec._evaluator->evaluator.relinearize(op1, _key_mgr.Relin_key(), res);
    }
}

void PHANTOM_CONTEXT::Bootstrap(PhantomCiphertext &res, PhantomCiphertext &op1, int level, int slot) {
    _codec._evaluator->evaluator.mod_switch_to_inplace(op1, _num_prime_parts - 1);

    int effective_slot = slot;
    if (effective_slot == 0) {
        effective_slot = static_cast<uint32_t>(BTS_SLOTS::POW_15);
    }

    PhantomCiphertext *bootstrap_input = &op1;

    PhantomCiphertext cyclic_input;
    bool cyclic_input_used = false;
    if (effective_slot > 0 && static_cast<uint64_t>(effective_slot) < _slot_count) {
        cyclic_input = op1;
        PhantomCiphertext rotated_input = op1;
        for (uint64_t offset = effective_slot; offset < _slot_count; offset += effective_slot) {
            Rotate(rotated_input, rotated_input, effective_slot);
            Add(cyclic_input, rotated_input, cyclic_input);
        }
        rotated_input.release();
        bootstrap_input = &cyclic_input;
        cyclic_input_used = true;
    }

    PhantomCiphertext in_place_input;
    bool in_place_bootstrap = &res == &op1;
    if (in_place_bootstrap) {
        in_place_input = *bootstrap_input;
        bootstrap_input = &in_place_input;
    }

    res.release();
    BTS_SLOTS slot_enum;
    switch (effective_slot) {
        case 32768: slot_enum = BTS_SLOTS::POW_15; break;
        case 16384: slot_enum = BTS_SLOTS::POW_14; break;
        case 8192:  slot_enum = BTS_SLOTS::POW_13; break;
        case 4096:  slot_enum = BTS_SLOTS::POW_12; break;
        default:    slot_enum = BTS_SLOTS::POW_15; break;
    }
    auto bs = _bootstrap.Get(slot_enum);
    IS_TRUE(bs != nullptr, "Unsupported slot size for bootstrap");
    bs->bootstrap_real_3(res, *bootstrap_input);

    if (in_place_bootstrap) {
        in_place_input.release();
    }

    if (cyclic_input_used) {
        cyclic_input.release();
    }

    int target_level = _num_prime_parts - level;
    if (level != 0 && target_level > res.chain_index()) {
        _codec._evaluator->evaluator.mod_switch_to_inplace(res, target_level);
    }
}

void PHANTOM_CONTEXT::Free_cipher(PhantomCiphertext &ct) {
    if (ct.size() > 0) {
        ct.release();
    }
}

void PHANTOM_CONTEXT::Free_plain(PhantomPlaintext &pt) {
    pt.release();
}

void PHANTOM_CONTEXT::Free_ciph_poly(PhantomCiphertext *ct, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        ct[i].release();
    }
}

SCALE_T PHANTOM_CONTEXT::Scale(const PhantomCiphertext &op) {
    return (uint64_t)std::log2(op.scale()) / _scaling_mod_size;
}

LEVEL_T PHANTOM_CONTEXT::Level(const PhantomCiphertext &op) { return op.coeff_modulus_size(); }

// ============================================================================
// Low-level C API for direct GPU kernel access
// ============================================================================

extern "C" {

void* Phantom_get_context() {
    auto& ctx = PHANTOM_CONTEXT::Context();
    return ctx.phantom_ctx();
}

void* Phantom_get_ntt_tables() {
    auto& ctx = PHANTOM_CONTEXT::Context();
    auto* pctx = ctx.phantom_ctx();
    return pctx ? const_cast<DNTTTable*>(&pctx->gpu_rns_tables()) : nullptr;
}

void* Phantom_get_galois_tool() {
    auto& ctx = PHANTOM_CONTEXT::Context();
    auto* pctx = ctx.phantom_ctx();
    return pctx ? pctx->key_galois_tool_.get() : nullptr;
}

void* Phantom_get_galois_key() {
    auto& ctx = PHANTOM_CONTEXT::Context();
    return const_cast<PhantomGaloisKey*>(&ctx.galois_key());
}

void* Phantom_get_relin_key() {
    auto& ctx = PHANTOM_CONTEXT::Context();
    return const_cast<PhantomRelinKey*>(&ctx.relin_key());
}

void* Phantom_get_rns_tool(size_t chain_index) {
    auto& ctx = PHANTOM_CONTEXT::Context();
    auto* pctx = ctx.phantom_ctx();
    if (!pctx || chain_index >= pctx->context_data_.size()) return nullptr;
    auto& rns_tool = pctx->context_data_[chain_index].gpu_rns_tool();
    return &rns_tool;
}

void* Phantom_get_modulus_tables() {
    auto& ctx = PHANTOM_CONTEXT::Context();
    auto* pctx = ctx.phantom_ctx();
    return pctx ? pctx->gpu_rns_tables().modulus() : nullptr;
}

uint64_t Phantom_get_poly_degree() {
    auto& ctx = PHANTOM_CONTEXT::Context();
    auto* pctx = ctx.phantom_ctx();
    return pctx ? pctx->poly_degree_ : 0;
}

uint64_t Phantom_get_slot_count() {
    auto& ctx = PHANTOM_CONTEXT::Context();
    return ctx.slot_count();
}

uint64_t Phantom_get_coeff_mod_size() {
    auto& ctx = PHANTOM_CONTEXT::Context();
    auto* pctx = ctx.phantom_ctx();
    return pctx ? pctx->coeff_mod_size_ : 0;
}

uint64_t Phantom_get_scaling_mod_size() {
    auto& ctx = PHANTOM_CONTEXT::Context();
    return ctx.scaling_mod_size();
}

} // extern "C"