//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "common/common.h"
#include "common/error.h"
#include "common/io_api.h"
#include "common/rt_api.h"
#include "common/rtlib_timing.h"
#include "hyperfhe_api.h"

#include <vector>
#include <frontend/encryptor.h>
#include <frontend/ckks.h>
#include <frontend/decryptor.h>
#include <frontend/keygenerator.h>
#if defined(HYPER_BTS_MACRO) && defined(GPU_BACKEND)
#include "EvaluatorWrapper.h"
#include "NVTXWrapper.h"
#include "bootstrap/EvalBootstrap.h"

#endif

//27+28
static const std::vector<uint64_t> mod_base = {
// 0x7e00001ULL, 0xffa0001ULL   //55
// 0x1ffc0001, 0x1ff60001 //58
0x1ffc0001, 0xffa0001 //57
// 0xffa0001, 0xfd20001 //56
};

//51  //STC、NORMAL
static const std::vector<uint64_t> mod_51 = {
0x6520001, 0x6640001, 0x6a00001, 0x6a20001, 0x6ae0001, 0x61e0001,
0x6be0001, 0x6c00001, 0x6400001, 0x6cc0001, 0x6dc0001, 0x6de0001, 0x6ea0001, 0x7080001, 0x7620001, 0x7300001, 0x72c0001, 0x7140001
};

static const std::vector<uint64_t> mod_53 = {
// 0x6280001, 0x4fc0001, 0x6520001, 0x6640001, 0x61e0001, 0x5140001, 
// 0x5f80001, 0x6100001, 0x5ee0001, 0x51a0001, 0x5e20001, 0x5e60001, 
// 0x5340001, 0x5560001, 0x5aa0001, 0x5b60001, 0x58e0001, 0x5640001, 
// 0x5940001, 0x5980001
0x5340001, 0x5560001, 0x58e0001, 0x5640001, 0x61e0001, 0x5140001, 
0x6520001, 0x6640001, 0x5ee0001, 0x51a0001, 0x5e20001, 0x5e60001, 
0x6280001, 0x4fc0001, 0x5940001, 0x5980001, 0x5f80001, 0x6100001, 
0x5aa0001, 0x5b60001, 0x3ee0001, 0x7e00001
};
//, 0x3ee0001, 0x7e00001

//24+24 //NORMAL
static const std::vector<uint64_t> mod_46 = {
0xd00001ULL, 0xd80001ULL, 
0xe40001ULL, 0xf60001ULL, 
0xfa0001ULL, 0xfc0001ULL
};

// //25+25  //CTS
// static const std::vector<uint64_t> mod_51 = {
// 0x1aa0001, 0x3720001, 0x1b00001, 0x3820001, 0x1b60001, 0x38e0001
// };

//28+28  //evalmod
static const std::vector<uint64_t> mod_56 = {
0xaa80001, 0xaaa0001,
0xb820001, 0xac60001,
0xae60001, 0xaf20001,
0xb680001, 0xb7a0001,
0xb2c0001, 0xb040001,
0xb560001, 0xb100001,
0xb2e0001, 0xb200001,
0xb3e0001, 0xb4c0001
};

//NSP
static const std::vector<uint64_t> mod_nsp = {
// >55 
// 0xfd20001, 0xc060001, 0xfc60001, 0xc300001, 0xfb40001, 0xc360001, 0xfb20001, 0xc420001,
// 0xf9c0001, 0xc4c0001, 0xf960001, 0xc640001, 0xf4e0001, 0xc760001, 0xf480001, 0xc8a0001
// >29+29 
0x1ff60001, 0x1fcc0001, 0x1fba0001, 0x1fb00001, 0x1f960001, 0x1f8c0001, 0x1f7e0001, 0x1f5c0001, 
0x1f560001, 0x1f4a0001, 0x1f480001, 0x1f0e0001, 0x1eee0001, 0x1ee80001, 0x1ed80001, 0x1ed20001, 
0x1e8a0001, 0x1e840001, 0x1e660001, 0x1e520001, 0x1e120001, 0x1de20001, 0x1dd40001, 0x1db80001, 
0x1db60001, 0x1d880001, 0x1d7a0001, 0x1d700001, 0x1d560001, 0x1d4a0001, 0x1d2c0001
// >28+28
// 0xfd20001, 0xfc60001, 0xfb40001, 0xfb20001, 0xf9c0001, 0xf960001, 0xf8a0001, 0xf880001, 0xf6a0001, 0xf600001, 0xf4e0001, 0xf480001, 0xf280001, 0xf1c0001, 0xf160001, 0xee00001, 0xeb00001, 0xea60001, 0xe9e0001, 0xe9a0001, 0xe980001, 0xe6a0001, 0xe580001, 0xe4c0001, 0xe440001, 0xe1c0001, 0xe1a0001, 0xe0e0001, 0xdf80001, 0xdde0001, 0xdda0001
};

static size_t take_from_bucket(const std::vector<uint64_t>& bucket,
                               size_t& idx,
                               size_t need,
                               std::vector<uint64_t>& out)
{
    if (idx >= bucket.size()) return 0;
    size_t available = bucket.size() - idx;
    size_t take = (need < available) ? need : available;
    if (take > 0) {
        out.insert(out.end(), bucket.begin() + idx, bucket.begin() + idx + take);
        idx += take;
    }
    return take;
}

static bool build_mod_vector(int mul_depth, int n_sp,
                             std::vector<uint64_t>& out,
                             std::string& err)
{
    out.clear();
    err.clear();

    // Step 1: 先从 mod_55 取两个
    if (mod_base.size() < 2) {
        err = "not enough mod_base (need 2)";
        return false;
    }
    out.push_back(mod_base[0]);
    out.push_back(mod_base[1]);


    // Step 2: 
    size_t idx51 = 0, idx46 = 0, idx53 = 0;
    size_t need_norm_q = 2 * static_cast<size_t>(mul_depth);
   
   
    // if (need_norm_q > 0) {
    //     size_t taken46 = take_from_bucket(mod_46, idx46, need_norm_q, out);
    //     need_norm_q -= taken46;
    // }

    

    // if (need_norm_q > 0) {
    //     size_t taken50 = take_from_bucket(mod_51, idx50, need_norm_q, out);
    //     need_norm_q -= taken50;
    // }

    // if (need_norm_q > 0) {
    //   size_t taken51 = take_from_bucket(mod_51, idx51, need_norm_q, out);
    //   need_norm_q -= taken51;
    // }

    if (need_norm_q > 0) {
      size_t taken53 = take_from_bucket(mod_53, idx53, need_norm_q, out);
      need_norm_q -= taken53;
    }
   
    if (need_norm_q > 0) {
        err = "not enough moduli to satisfy mul_depth";
        return false;
    }

    // Step 3: 再从 mod_nsp 取n_sp * 2个
    if (mod_nsp.size() < n_sp * 2) {
        err = "not enoughmoduli to satisfy n_sp";
        return false;
    }
    for(int i=0; i< n_sp * 2; i++)
    {
      out.push_back(mod_nsp[i]);
    }

    return true;
}

#include <iomanip>
bool calculate_scales_per_level_with_logging(
    uint64_t _scaling_mod_size, const std::vector<uint64_t>& modulus,
    int mul_depth, int n_sp, std::vector<double>& scales_out, std::string& err) 
{
    // 设置 double 输出精度以便观察
    std::cout << std::fixed << std::setprecision(10);

    std::cout << "[INFO] Starting scale calculation..." << std::endl;
    std::cout << "------------------------------------" << std::endl;
    
    // 打印输入参数
    std::cout << "[INPUT] _scaling_mod_size: " << _scaling_mod_size << std::endl;
    std::cout << "[INPUT] Total modulus count: " << modulus.size() << std::endl;
    std::cout << "[INPUT] mul_depth: " << mul_depth << std::endl;
    std::cout << "[INPUT] n_sp: " << n_sp << std::endl;
    std::cout << "------------------------------------" << std::endl;

    scales_out.clear();
    err.clear();

    // 1. 输入验证和初始计算
    size_t special_mod_count = static_cast<size_t>(n_sp * 2);
    size_t total_mod_count = modulus.size();
    
    if (total_mod_count < special_mod_count) {
        err = "Total modulus count is less than special modulus count.";
        std::cerr << "[ERROR] " << err << std::endl;
        return false;
    }
    size_t normal_mod_count = total_mod_count - special_mod_count;
    
    if (mul_depth < 0) {
        err = "Multiplication depth cannot be negative.";
        std::cerr << "[ERROR] " << err << std::endl;
        return false;
    }

    // 我们需要 2 * mul_depth 个普通模数来进行 mul_depth 次重缩放
    // 再加上至少2个基础模数
    if (normal_mod_count < static_cast<size_t>(2 * mul_depth + 2)) {
        err = "Not enough normal moduli to support the given multiplication depth. Need at least 2*mul_depth + 2.";
        std::cerr << "[ERROR] " << err << std::endl;
        return false;
    }
    int base_prime_num = normal_mod_count - 2 * mul_depth;

    std::cout << "[CALC] Special modulus count: " << special_mod_count << std::endl;
    std::cout << "[CALC] Normal modulus count: " << normal_mod_count << std::endl;
    std::cout << "[CALC] Base prime count (at lowest level): " << base_prime_num << std::endl;
    std::cout << "------------------------------------" << std::endl;


    // 2. 按自然顺序计算 scales
    std::vector<double> temp_scales;
    
    // 计算最高层的 scale (scale_0)
    std::cout << "[DEBUG] Calculating initial scale (Level 0)..." << std::endl;
    double current_scale = std::pow(2.0, static_cast<double>(_scaling_mod_size));
    std::cout << "[DEBUG] Initial scale = 2^" << _scaling_mod_size << " = " << current_scale << std::endl;
    
    // 最高层（Level 0）有两个模数，它们的scale相同
    temp_scales.push_back(current_scale);
    temp_scales.push_back(current_scale);
    std::cout << "[DEBUG] Added 2 scales for Level 0." << std::endl;
    
    // 循环计算后续的 mul_depth 个 scale
    for (int i = 0; i < mul_depth; ++i) {
        std::cout << "\n[DEBUG] Calculating scale for Level " << i + 1 << " (Rescaling " << i << ")..." << std::endl;
        
        // 确定当前层要使用的模数对。我们从普通模数的最后两个开始，向前取。
        size_t mod_index_1 = normal_mod_count - 2 * (i + 1);
        size_t mod_index_2 = mod_index_1 + 1;
        
        uint64_t mod1 = modulus[mod_index_1];
        uint64_t mod2 = modulus[mod_index_2];

        std::cout << "[DEBUG]   Using modulus at indices: " << mod_index_1 << " and " << mod_index_2 << std::endl;
        std::cout << "[DEBUG]   mod1 = " << mod1 << ", mod2 = " << mod2 << std::endl;

        if (mod1 == 0 || mod2 == 0) {
            err = "Modulus is zero, cannot perform division.";
            std::cerr << "[ERROR] " << err << std::endl;
            return false;
        }

        double mod_product = static_cast<double>(mod1) * static_cast<double>(mod2);
        std::cout << "[DEBUG]   mod1 * mod2 = " << mod_product << std::endl;
        
        double old_scale = current_scale;
        // 计算新 scale: scale_new = (scale_old)^2 / (mod1 * mod2)
        current_scale = (current_scale * current_scale) / mod_product;
        
        std::cout << "[DEBUG]   Old scale was: " << old_scale << std::endl;
        std::cout << "[DEBUG]   New scale = (" << old_scale << ")^2 / " << mod_product << " = " << current_scale << std::endl;

        temp_scales.push_back(current_scale);
        temp_scales.push_back(current_scale);
        std::cout << "[DEBUG]   Added 2 scales for Level " << i + 1 << "." << std::endl;
    }
    
    // 填充剩余的最低层级的 scale
    int remaining_scales_to_add = base_prime_num - 2;
    if (remaining_scales_to_add > 0) {
        std::cout << "\n[DEBUG] Filling " << remaining_scales_to_add << " remaining lowest-level scales with value: " << current_scale << std::endl;
        for (int i = 0; i < remaining_scales_to_add; ++i) {
            temp_scales.push_back(current_scale);
        }
    }


    std::cout << "------------------------------------" << std::endl;
    // 打印中间结果
    std::cout << "[VERIFY] Scales calculated in natural order (before reversal):" << std::endl;
    for (size_t i = 0; i < temp_scales.size(); ++i) {
        std::cout << "  temp_scales[" << i << "] = " << temp_scales[i] << std::endl;
    }
    
    if (temp_scales.size() != normal_mod_count) {
        err = "Internal logic error: temp_scales.size() != normal_mod_count.";
        std::cerr << "[ERROR] " << err << " ( " << temp_scales.size() << " != " << normal_mod_count << " )" << std::endl;
        return false;
    }
    std::cout << "[VERIFY] Size check passed: temp_scales.size() (" << temp_scales.size() << ") == normal_mod_count (" << normal_mod_count << ")." << std::endl;

    // 3. 反转 vector 以满足“后生成的在前”的要求
    std::cout << "[INFO] Reversing the scales vector..." << std::endl;
    std::reverse(temp_scales.begin(), temp_scales.end());
    scales_out = temp_scales;
    
    std::cout << "------------------------------------" << std::endl;
    // 打印最终结果
    std::cout << "[FINAL] Final output scales_out (after reversal):" << std::endl;
    for (size_t i = 0; i < scales_out.size(); ++i) {
        std::cout << "  scales_out[" << i << "] = " << scales_out[i] << std::endl;
    }
    std::cout << "------------------------------------" << std::endl;

    std::cout << "[INFO] Scale calculation successful." << std::endl;
    return true;
}


bool calculate_scales_per_level(uint64_t _scaling_mod_size, const std::vector<uint64_t>& modulus,
    int mul_depth, int n_sp, std::vector<double>& scales_out, std::string& err) 
{ 
    scales_out.clear();
    err.clear();

    // 1. 输入验证
    size_t special_mod_count = static_cast<size_t>(n_sp * 2);
    size_t total_mod_count = modulus.size();
    
    if (total_mod_count < special_mod_count) {
        err = "Total modulus count is less than special modulus count.";
        return false;
    }
    size_t normal_mod_count = total_mod_count - special_mod_count;
    
    // 我们需要 2 * mul_depth 个普通模数来进行 mul_depth 次重缩放
    if (mul_depth < 0) {
        err = "Multiplication depth cannot be negative.";
        return false;
    }
    if (normal_mod_count <= static_cast<size_t>(2 * mul_depth)) {
        err = "Not enough normal moduli to support the given multiplication depth.";
        return false;
    }
    int base_prime_num = normal_mod_count - 2 * mul_depth;


    // 2. 按自然顺序计算 scales
    std::vector<double> temp_scales;
    
    // 计算最高层的 scale (scale_0)
    // 您的描述是 pow(2.0, _scaling_mod_size * scale)，这可能是笔误。
    // 标准CKKS实现是 pow(2.0, _scaling_mod_size)。我将按标准实现。
    double current_scale = std::pow(2.0, static_cast<double>(_scaling_mod_size));
    temp_scales.push_back(current_scale);
    temp_scales.push_back(current_scale);

    // 循环计算后续的 mul_depth 个 scale
    for (int i = 0; i < mul_depth; ++i) {
        // 确定当前层要使用的模数对。我们从普通模数的最后两个开始，向前取。
        size_t mod_index_1 = normal_mod_count - 2 * (i + 1);
        size_t mod_index_2 = mod_index_1 + 1;
        
        uint64_t mod1 = modulus[mod_index_1];
        uint64_t mod2 = modulus[mod_index_2];

        if (mod1 == 0 || mod2 == 0) {
            err = "Modulus is zero, cannot perform division.";
            return false;
        }

        // 计算模数乘积。使用double来避免溢出。
        double mod_product = static_cast<double>(mod1) * static_cast<double>(mod2);

        // 计算新 scale: scale_new = (scale_old)^2 / (mod1 * mod2)
        current_scale = (current_scale * current_scale) / mod_product;
        temp_scales.push_back(current_scale);
        temp_scales.push_back(current_scale);
    }

    for (int i = 0; i < normal_mod_count - mul_depth * 2 - 2; ++i) {
      temp_scales.push_back(current_scale);
    }

    if (temp_scales.size() != normal_mod_count) {
        err = "temp_scales.size() != normal_mod_count.";
        return false;
    }
    
    // 3. 反转 vector 以满足“后生成的在前”的要求
    std::reverse(temp_scales.begin(), temp_scales.end());
    scales_out = temp_scales;

    return true;
}


inline void push_bits(std::vector<int> &coeff_bit_vec, std::vector<int> &scale_flag_vec, int bits, int n_primes, bool ds=false) {
    if(ds && bits < 30 ){
        std::cout << "not support scale bits < 30 when using double rescale" << std::endl;
        exit(1);
    }

    for (int i = 0; i < n_primes; i++){
        if(ds && bits >= 30){
            int half_bits = bits / 2;
            coeff_bit_vec.push_back(half_bits);
            scale_flag_vec.push_back(0);
            coeff_bit_vec.push_back(bits - half_bits);
            scale_flag_vec.push_back(2);
        }else{
            coeff_bit_vec.push_back(bits);
            scale_flag_vec.push_back(1);
        }
    } 
    
}

class HYPER_CONTEXT {
#ifdef GPU_BACKEND
  using Ciphertext = cuckks::DeviceCipher;
  using Plaintext  = cuckks::DevicePlain;
  using HostCiphertext = frontend::Ciphertext;
  using HostPlaintext  = frontend::Plaintext;
#else
  using Ciphertext = frontend::Ciphertext;
  using Plaintext  = frontend::Plaintext;
#endif
public:
  const frontend::SecretKey&  Secret_key() const { return _kgen->secret_key(); }
  const frontend::PublicKey&  Public_key() const { return *_pk; }
  const frontend::RelinKeys&  Relin_key() const { return *_rlk; }
  const frontend::GaloisKeys& Rotate_key() const { return *_rtk; }

  static HYPER_CONTEXT* Context() {
    IS_TRUE(Instance != nullptr, "instance not initialized");
    return Instance;
  }

  static void Init_context() {
    IS_TRUE(Instance == nullptr, "instance already initialized");
    Instance = new HYPER_CONTEXT();
  }

  static void Fini_context() {
    IS_TRUE(Instance != nullptr, "instance not initialized");
    delete Instance;
    Instance = nullptr;
  }

public:
  void Prepare_input(TENSOR* input, const char* name) {
    size_t              len = TENSOR_SIZE(input);
    std::vector<double> vec(input->_vals, input->_vals + len);
  #ifdef GPU_BACKEND
    auto parm_id = _ctx->get_parms_id_from_level(_input_level-_base_mod_num);
    HostPlaintext           pt;
    _encoder->encode(vec, parm_id, std::pow(2.0, _scaling_mod_size), pt);
    HostCiphertext* ct = new HostCiphertext;
    _encryptor->encrypt(pt, *ct);
    // cuckks::DeviceCipher* gpu_ct = new cuckks::DeviceCipher(ct->to(_gpu_eval->context(), _data_layout));
    cuckks::DeviceCipher* gpu_ct = new cuckks::DeviceCipher(cuckks::DeviceCipher::from(*ct, _gpu_eval->context(), _data_layout));
    // std::cout << name <<" data_layout:" << (int)(gpu_ct->data_layout())<< std::endl;
    cudaDeviceSynchronize();
    Io_set_input(name, 0, gpu_ct);
  #else 
    auto context_data = _ctx->first_context_data();
    for (int i = context_data->chain_index(); i > _input_level; --i) {
      context_data = context_data->next_context_data();
    }
    Plaintext           pt;
    _encoder->encode(vec, context_data->parms_id(), std::pow(2.0, _scaling_mod_size), pt);
    Ciphertext* ct = new Ciphertext;
    _encryptor->encrypt(pt, *ct);
    Io_set_input(name, 0, ct);
  #endif
  }

  void Set_output_data(const char* name, size_t idx, Ciphertext* ct) {
    Io_set_output(name, idx, new Ciphertext(*ct));
  }

  //fixme: why return Ciphertext will cause rmm::out_of_memory
  Ciphertext Get_input_data(const char* name, size_t idx) {
    // printf("Get_input_data\n");
    Ciphertext* data = (Ciphertext*)Io_get_input(name, idx);
    IS_TRUE(data != nullptr, "not find data");
    // std:: cout << " Get_input_data:" << name <<" data_layout:" << (int)(data->data_layout())<< std::endl;
    // printf("Got_input_data\n");
    return *(data);
  }

  double* Handle_output(const char* name, size_t idx) {
    Ciphertext* data = (Ciphertext*)Io_get_output(name, idx);
    IS_TRUE(data != nullptr, "not find data");
    #ifdef GPU_BACKEND
    HostCiphertext host_data;
    HostPlaintext pt;
    // host_data.from(*data);
    host_data = (*data).to(*_cpu_ctx);
    _decryptor->decrypt(host_data, pt);
    #else
    Plaintext pt;
    _decryptor->decrypt(*data, pt);
    #endif
    std::vector<double> vec;
    std::cout << "Handle_output: scale: " << pt.scale() << std::endl;
    _encoder->decode(pt, vec);
    double* msg = (double*)malloc(sizeof(double) * vec.size());
    memcpy(msg, vec.data(), sizeof(double) * vec.size());
    // 打印 msg 的内容
    printf("Decoded msg (size %zu): [", vec.size());
    for (size_t i = 0; i < 4; ++i) {
        printf("%.6f", msg[i]);  // 打印6位小数
        if (i < vec.size() - 1) printf(", ");
    }
    printf("]\n");

    return msg;
  }

  void Encode_float(Plaintext* pt, float* input, size_t len, SCALE_T scale,
                    LEVEL_T level) {
    std::vector<double> vec(input, input + len);
    #ifdef GPU_BACKEND
    HostPlaintext host_pt;
    _encoder->encode(vec, level, std::pow(2.0, _scaling_mod_size * scale), host_pt);
    // *pt = (host_pt.to(_gpu_eval->context()));  // todo： 消除深拷贝
    *pt = cuckks::DevicePlain::from(host_pt, _gpu_eval->context());
    #else
    _encoder->encode(vec, level, std::pow(2.0, _scaling_mod_size * scale), *pt);
    #endif
  }

  void Encode_float(Plaintext* pt, float* input, size_t len, SCALE_T scale,
                    int level) {
    std::vector<double> vec(input, input + len);
    // std::cout << "---- Encode_float: level-" << level <<" ----"<<std::endl;
    #ifdef GPU_BACKEND
    auto parm_id = _ctx->get_parms_id_from_level(level-_base_mod_num);
    HostPlaintext host_pt;
    _encoder->encode(vec, parm_id, std::pow(2.0, _scaling_mod_size * scale), host_pt);
    // *pt = (host_pt.to(_gpu_eval->context()));  // todo： 消除深拷贝
    *pt = cuckks::DevicePlain::from(host_pt, _gpu_eval->context());
    #else
    throw std::runtime_error("Encode_float: not support int level for cpu backend!");
    // _encoder->encode(vec, parm_id, std::pow(2.0, _scaling_mod_size * scale), *pt);
    #endif
  }

  // void Encode_double(Plaintext* pt, double* input, size_t len, SCALE_T scale,
  //                    LEVEL_T level) {
  //   std::vector<double> vec(input, input + len);
  //   _encoder->encode(vec, level, std::pow(2.0, _scaling_mod_size * scale), *pt);
  // }

  void Encode_float_cst_lvl(Plaintext* pt, float* input, size_t len,
                            SCALE_T scale, int level) {
    std::vector<double> vec(input, input + len);
    #ifdef GPU_BACKEND
    auto parm_id = _ctx->get_parms_id_from_level(level-_base_mod_num);
    HostPlaintext host_pt;
    _encoder->encode(vec, parm_id, std::pow(2.0, _scaling_mod_size * scale), host_pt);
    // *pt = (host_pt.to(_gpu_eval->context()));  // todo： 消除深拷贝
    *pt = cuckks::DevicePlain::from(host_pt, _gpu_eval->context());
    #else
    auto context_data = _ctx->first_context_data();
    for (int i = context_data->chain_index(); i > level; --i) {
      context_data = context_data->next_context_data();
    }
    _encoder->encode(vec, context_data->parms_id(), std::pow(2.0, _scaling_mod_size * scale), *pt);
    #endif
  }

  // void Encode_double_cst_lvl(Plaintext* pt, double* input, size_t len,
  //                            SCALE_T scale, int level) {
  //   std::vector<double> vec(input, input + len);
  //   auto                context_data = _ctx->first_context_data();
  //   for (int i = context_data->chain_index(); i > level; --i) {
  //     context_data = context_data->next_context_data();
  //   }
  //   _encoder->encode(vec, context_data->parms_id(),
  //                    std::pow(2.0, _scaling_mod_size * scale), *pt);
  // }

  void Encode_float_mask(Plaintext* pt, float input, size_t len, SCALE_T scale,
                         LEVEL_T level) {
    std::vector<double> vec(len, input);
    #ifdef GPU_BACKEND 
    HostPlaintext host_pt;
    double scale_this_level;
    #ifdef HYPERFHE_32
    auto data_context_ptr = _ctx->get_context_data(level);
    if(!data_context_ptr){
      throw std::runtime_error("Encode_float_mask Error: data_context_ptr null!");
    }
    scale_this_level = data_context_ptr->scale_per_level();
    // std::cout<< "scale_this_level = " << scale_this_level << " , scale = " << std::pow(2.0, _scaling_mod_size) * scale <<std::endl;
    #else
    scale_this_level = std::pow(2.0, _scaling_mod_size) * scale;
    #endif
    _encoder->encode(vec, level, scale_this_level, host_pt);
    // *pt = (host_pt.to(_gpu_eval->context()));  //深拷贝
    *pt = cuckks::DevicePlain::from(host_pt, _gpu_eval->context());
    #else
    _encoder->encode(vec, level, std::pow(2.0, _scaling_mod_size)  * scale, *pt);
    #endif
  }

  // void Encode_double_mask(Plaintext* pt, double input, size_t len,
  //                         SCALE_T scale, LEVEL_T level) {
  //   std::vector<double> vec(len, input);
  //   _encoder->encode(vec, level, std::pow(2.0, _scaling_mod_size * scale), *pt);
  // }

  // void Encode_float_mask_cst_lvl(Plaintext* pt, float input, size_t len,
  //                                SCALE_T scale, int level) {
  //   std::vector<double> vec(len, input);
  //   auto                context_data = _ctx->first_context_data();
  //   for (int i = context_data->chain_index(); i > level; --i) {
  //     context_data = context_data->next_context_data();
  //   }
  //   _encoder->encode(vec, context_data->parms_id(),
  //                    std::pow(2.0, _scaling_mod_size * scale), *pt);
  // }

  // void Encode_double_mask_cst_lvl(Plaintext* pt, double input, size_t len,
  //                                 SCALE_T scale, int level) {
  //   std::vector<double> vec(len, input);
  //   auto                context_data = _ctx->first_context_data();
  //   for (int i = context_data->chain_index(); i > level; --i) {
  //     context_data = context_data->next_context_data();
  //   }
  //   _encoder->encode(vec, context_data->parms_id(),
  //                    std::pow(2.0, _scaling_mod_size * scale), *pt);
  // }

void Decrypt(Ciphertext* ct, std::vector<double>& vec) {
    #ifdef GPU_BACKEND
    HostCiphertext host_ct;
    HostPlaintext pt;
    // host_ct.from(*ct);
    host_ct = (*ct).to(*_cpu_ctx);
    _decryptor->decrypt(host_ct, pt);
    #else
    Plaintext pt;
    _decryptor->decrypt(*ct, pt);
    #endif
    _encoder->decode(pt, vec);
  }

  void Decode(Plaintext* pt, std::vector<double>& vec) {
    #ifdef GPU_BACKEND
    HostPlaintext host_pt;
    // host_pt.from(*pt);
    host_pt = (*pt).to(*_cpu_ctx);
    _encoder->decode(host_pt, vec);
    #else
    _encoder->decode(*pt, vec);
    #endif
  }

public:

  void Add(const Ciphertext* op1, const Plaintext* op2, Ciphertext* res) {
    if (res == op1) {
      _eval->add_plain_inplace(*res, *op2);
    } else {
      _eval->add_plain(*op1, *op2, *res);
    }
  }

  void Mul(const Ciphertext* op1, const Plaintext* op2, Ciphertext* res) {
    // printf("--- multiply_plain: op1.scale: %f, op2.scale: %f\n", op1->scale(), op2->scale());
    if (res == op1) {
      _eval->multiply_plain_inplace(*res, *op2);
      //  printf("multiply_plain: c.scale: %f, p.scale: %f\n", op1->scale(), op2->scale());
    } else {
      _eval->multiply_plain(*op1, *op2, *res);
      //  printf("multiply_plain: c.scale: %f, p.scale: %f\n", op1->scale(), op2->scale());
    }
    // printf("---After multiply_plain: res.scale: %f\n", res->scale());
  }

  #ifdef HYPERFHE_32 
  #define ADJUST_LEVLE_REDUCE_ERROR
  #endif
    
  // void Adjust_level(Ciphertext& op1, Ciphertext& op2, uint64_t level_1,
  //                   uint64_t level_2) {
  inline void Adjust_level(Ciphertext& op1, Ciphertext& op2) {
  #ifdef GPU_BACKEND
    std::size_t level_1 = op1.coeff_modulus_size();
    std::size_t level_2 = op2.coeff_modulus_size();
  #else
    uint64_t   level_1 = _cpu_ctx->get_context_data(op1.parms_id())->chain_index();
    uint64_t   level_2 = _cpu_ctx->get_context_data(op2.parms_id())->chain_index();
  #endif
  if(level_1 == level_2) return;

  #ifdef GPU_BACKEND
    const auto &ct_lo = level_1 < level_2 ? op1 : op2;
    auto &ct_hi = level_1 < level_2 ? op2 : op1;
    // auto target_parms_id = _gpu_eval->context().get_context_data(ct_lo.parms_id())->parms_id();
    auto target_parms_id = ct_lo.parms_id();
    #ifdef ADJUST_LEVLE_REDUCE_ERROR
    // Adjust: scale_adjust = delta_lo * q_hi / (delta_hi)^2
    const auto &context_hi = _gpu_eval->context().get_context_data(ct_hi.parms_id());
    double q_hi = static_cast<double>(context_hi->back_coeff_modulus());
    #ifdef HYPERFHE_32   //#ifndef __DATA_WORD_SIZE_64__
    // FIXME: check scale flag
    q_hi *= context_hi->next_context_data()->back_coeff_modulus();
    #endif // __DATA_WORD_SIZE_64__
    double fix_scale = 1.0;
    double scale_adjust =
        (ct_lo.scale() / ct_hi.scale()) * (q_hi / ct_hi.scale()) * fix_scale;

    _gpu_eval->multiply_scalar_inplace(ct_hi, scale_adjust);
    ct_hi.scale() = ct_lo.scale() * q_hi;
    // NOTE: MUST modify the scale manually here!!
    //       Because we want `scale_adjust` to correct scale, not data in
    //       plaintext slots; That is to say, SEAL believes that the scale of
    //       the result is only ct_hi.scale()^2
    // Now: ct_hi.scale = delta_lo * q_hi, ct_hi.level = l_hi
    _gpu_eval->rescale_to_next_inplace(ct_hi);
    #endif
    _gpu_eval->mod_switch_to_inplace(ct_hi, target_parms_id);
  #else
    if (level_1 > level_2) {
      while (level_1 > level_2) {
        _eval->mod_switch_to_next_inplace(op1);
        --level_1;
      }
    } else if (level_1 < level_2) {
      while (level_1 < level_2) {
        _eval->mod_switch_to_next_inplace(op2);
        --level_2;
      }
    }
  #endif
  }

  
  void Add(const Ciphertext* op1, const Ciphertext* op2, Ciphertext* res) {
    // std::cout << "op1" <<" data_layout:" << (int)(op1->data_layout())<< std::endl;
    // std::cout << "op2" <<" data_layout:" << (int)(op2->data_layout())<< std::endl;
    Ciphertext final_op1 = *op1;
    Ciphertext final_op2 = *op2;
    // std::cout << "final_op1" <<" data_layout:" << (int)(final_op1.data_layout())<< std::endl;
    // std::cout << "final_op2" <<" data_layout:" << (int)(final_op2.data_layout())<< std::endl;
   
    std::size_t level_1 = op1->coeff_modulus_size();
    std::size_t level_2 = op2->coeff_modulus_size();
    // if (level_1 != level_2) {
    //   printf("++++++Adjust_level-ADD: level1 = %ld, level2 = %ld\n", level_1, level_2);
    //   printf("++++++Adjust_level-ADD: scale1 = %f, scale2 = %f\n", op1->scale(), op2->scale());
    // }
    Adjust_level(final_op1, final_op2);
      // printf("final_op1-level1 = %ld, final_op2-level2 = %ld\n",final_op1.coeff_modulus_size(), final_op2.coeff_modulus_size());
    // }
    // if (level_1 != level_2) {
    //   printf("++++++ After Adjust_level: level1 = %ld, level2 = %ld\n", final_op1.coeff_modulus_size(), final_op2.coeff_modulus_size());
    //   printf("++++++After Adjust_level: scale1 = %f, scale2 = %f\n", final_op1.scale(), final_op2.scale());
    // }

    // #ifndef GPU_BACKEND   
    // final_op2.scale() = final_op1.scale();  //?
    // #endif 

    _eval->add(final_op1, final_op2, *res);
  }


  void Mul(const Ciphertext* op1, const Ciphertext* op2, Ciphertext* res) {
    Ciphertext final_op1 = *op1;
    Ciphertext final_op2 = *op2;
    // if (level_1 != level_2) {
      // printf("******Adjust_level-MUL: level1 = %ld, level2 = %ld\n",level_1, level_2);
      Adjust_level(final_op1, final_op2);
    // }
    _eval->multiply(final_op1, final_op2, *res);
    // printf("After multiply_ciph: res.scale: %f\n", res->scale());

  }
 

  // void Add(Ciphertext* op1, Ciphertext* op2, Ciphertext* res) {
  //   Adjust_level(*op1, *op2);
  //   _eval->add(*op1, *op2, *res);
  // }

  // void Mul(Ciphertext* op1, Ciphertext* op2, Ciphertext* res) {
  //   Adjust_level(*op1, *op2);
  //   _eval->multiply(*op1, *op2, *res);
  // }

  void Add(const Ciphertext* op1, const double op2, Ciphertext* res) {
    #ifdef GPU_BACKEND
    HostPlaintext host_pt;
    Plaintext plain;
    _encoder->encode(op2, op1->parms_id(), op1->scale(), host_pt);
    // plain = (host_pt.to(_gpu_eval->context()));
   plain = cuckks::DevicePlain::from(host_pt, _gpu_eval->context());
    #else
    Plaintext plain;
    _encoder->encode(op2, op1->parms_id(), op1->scale(), plain);
    #endif
    if (res == op1) {
      _eval->add_plain_inplace(*res, plain);
    } else {
      _eval->add_plain(*op1, plain, *res);
    }
  }

  void Mul(const Ciphertext* op1, const double op2, Ciphertext* res) {
    #ifdef GPU_BACKEND
    HostPlaintext host_pt;
    Plaintext plain;
    _encoder->encode(op2, op1->parms_id(), op1->scale(), host_pt);
    // plain = (host_pt.to(_gpu_eval->context()));
    plain = cuckks::DevicePlain::from(host_pt, _gpu_eval->context());
    #else
    Plaintext plain;
    _encoder->encode(op2, op1->parms_id(), op1->scale(), plain);
    #endif
    if (res == op1) {
      _eval->multiply_plain_inplace(*res, plain);
      // printf("multiply_plain_double: c.scale: %f, p.scale: %f\n", op1->scale(), plain.scale());
    } else {
      _eval->multiply_plain(*op1, plain, *res);
      // printf("multiply_plain_double: c.scale: %f, p.scale: %f\n", op1->scale(), plain.scale());
    }
    // printf("After multiply_scalar: res.scale: %f\n", res->scale());

  }

  void Rotate(const Ciphertext* op1, int step, Ciphertext* res) {
    
    if (res == op1) {
      #ifdef GPU_BACKEND
      _gpu_eval->rotate_vector_inplace(*res, step);
      #else
      _eval->rotate_vector_inplace(*res, step, *_rtk);
      #endif
    } else {
      #ifdef GPU_BACKEND
      _gpu_eval->rotate_vector(*op1, step, *res);
      #else
      _eval->rotate_vector(*op1, step, *_rtk, *res);
      #endif
    }
  }

  void Rescale(const Ciphertext* op1, Ciphertext* res) {
    // printf("Rescale:coeff_modulus_size(): %ld\n", op1->coeff_modulus_size());
    // printf("Rescale:scale(): %f\n", op1->scale());
    if (res == op1) {
      _eval->rescale_to_next_inplace(*res);
    } else {
      _eval->rescale_to_next(*op1, *res);
    }
    // printf("After Rescale:scale(): %f\n", res->scale());
    // printf("After Rescale:coeff_modulus_size(): %ld\n", res->coeff_modulus_size());
  }

  // void Mod_switch(const Ciphertext* op1, Ciphertext* res) {
  //   if (res == op1) {
  //     _eval->mod_switch_to_next_inplace(*res);
  //   } else {
  //     _eval->mod_switch_to_next(*op1, *res);
  //   }
  // }

  void Relin(const Ciphertext* op1, Ciphertext* res) {
    if (res == op1) {
      #ifdef GPU_BACKEND
      _gpu_eval->relinearize_inplace(*res);
      #else
      _eval->relinearize_inplace(*res, *_rlk);
      #endif
    } else {
      #ifdef GPU_BACKEND
      _gpu_eval->relinearize(*op1, *res);
      #else
      _eval->relinearize(*op1, *_rlk, *res);
      #endif
    }
  }

  void Bootstrap(Ciphertext* op1, Ciphertext* res, int level) {
#if defined(HYPER_BTS_MACRO) && defined(GPU_BACKEND)
    bool inplace = false;
    Ciphertext out;
    // if (res == op1) {
    //   res = &out;
    //   inplace = true;
    // }
    // _boot->set_cache(true);
    // Ciphertext rtn_warmup;
    // _boot->bootstrap(rtn_warmup, *op1); // 预热
    // cudaDeviceSynchronize();
    // _boot->set_online_encoding(false);
    if(!_boot_cached)
    {
      _boot->set_cache(true);
      _boot->bootstrap(out, *op1); // 预热
      _boot->set_online_encoding(false);
      _boot_cached = true;
    }
    else
    {
      _boot->bootstrap(out, *op1);
    }
    // if(inplace){
    //   *op1 = std::move(out);
    //   res = op1;
    // }
    // printf("---boot :coeff_modulus_size(): %ld, scale(): %f\n", out.coeff_modulus_size(), out.scale());
    // std::cout << "---- Bootstrap: level-" << level <<" ----"<<std::endl;
    auto parm_id = _ctx->get_parms_id_from_level(level-_base_mod_num);
    _dev_eval->mod_switch_to(out, parm_id, *res);
    // printf("---boot :coeff_modulus_size(): %ld, scale(): %f\n", res->coeff_modulus_size(), res->scale());
#else
    FMT_ASSERT(false, "Bootstrap: HYPER_BTS_MACRO/GPU_BACKEND Not Defined.");
#endif
  }


  SCALE_T Scale_degree(Ciphertext* op) {
    return (uint64_t)std::log2(op->scale()) / _scaling_mod_size;
  }

  SCALE_T Normalized_Scale(Ciphertext* op) {
    return op->scale() / std::pow(2.0, _scaling_mod_size);  
  }

  SCALE_T Scale(Ciphertext* op) {
    return op->scale();  
  }

  LEVEL_T Level(Ciphertext* op) {
    auto data_context_ptr = _ctx->get_context_data(op->parms_id());
    if(!data_context_ptr) 
      std::cout<< "Level : data_context_ptr null!"  <<std::endl;
    return op->parms_id(); 
    }

private:
  std::vector<int> _gal_steps_vector;
  void             Set_gal_steps() {
    CKKS_PARAMS* prog_param = Get_context_params();
    if (Need_bts()) {
      for (int i = 0; i < prog_param->_num_rot_idx; ++i) {
        int rot = prog_param->_rot_idxs[i];
        if (find(_gal_steps_vector.begin(), _gal_steps_vector.end(), rot) ==
            _gal_steps_vector.end())
          _gal_steps_vector.push_back(rot);
      }
    } else {
      for (int i = 0; i < prog_param->_num_rot_idx; ++i) {
        int rot = prog_param->_rot_idxs[i];
        _gal_steps_vector.push_back(rot);
      }
    }
  }

#if defined(HYPER_BTS_MACRO) && defined(GPU_BACKEND)
// void Init_bootstrapper(CKKS_PARAMS* prog_param) {
    // long   boundary_K        = 25;
    // long   boot_deg          = 59;
    // long   scale_factor      = 2;
    // long   inverse_deg       = 1;
    // long   logN              = log2(prog_param->_poly_degree);
    // long   loge              = 10;
    // long   full_logn         = logN - 1;  // full slots
    // long   sparse_logn       = logN - 2;  // sparse slots
    // int    logp              = prog_param->_scaling_mod_size;
    // int    logq              = prog_param->_first_mod_size;
    // int    log_special_prime = prog_param->_first_mod_size;
    // int    total_level       = _bts_remaining_level + _bts_required_level;
    // double scale             = pow(2.0, logp);

    // // _boot = new Bootstrapper(loge, sparse_logn, logN - 1, total_level, scale,
    // //                          boundary_K, boot_deg, scale_factor, inverse_deg,
    // //                          *_ctx, *_kgen, *_encoder, *_encryptor, *_decryptor,
    // //                          *_eval, *_rlk, *_rtk); //FHE_MP_CNN
    // _boot = new Bootstrapper(loge, sparse_logn, logN - 1, total_level, scale,
    //                          boundary_K, boot_deg, scale_factor, inverse_deg,
    //                          *_ctx, *_kgen, *_encoder,
    //                          *_eval, *_rlk, *_rtk, *_hoisted_rtk);
    // _boot->prepare_mod_polynomial();

    // _boot->addLeftRotKeys_Linear_to_vector_3(_gal_steps_vector);
    // _boot->slot_vec.push_back(sparse_logn);
    // _boot->generate_LT_coefficient_3();
  // }

void make_bts_parms(CKKS_PARAMS* prog_param, frontend::HostParameter &parms_out,
                         EvalBootstrap::BootstrapContext &bts_parms_out,
                         EvalBootstrap::HoistingMode hoisting = EvalBootstrap::HoistingMode::DOUBLE_HOIST)
{
    long   logN              = log2(prog_param->_poly_degree);
    long   full_logn         = logN - 1;  // full slots
    long   sparse_logn       = logN - 2;  // sparse slots
    int    hw                = 32; //prog_param->_hamming_weight;
    int    logq              = prog_param->_scaling_mod_size;
    int    logp              = prog_param->_first_mod_size;
    int    log_special_prime = prog_param->_first_mod_size;
    int    num_p_parts       = prog_param->_num_q_parts;
    int    total_level       = prog_param->_mul_depth;
    // int    stc_depth         = 3;
    // int    cts_depth         = 4;
    int    evalmod_depth     = 8;
    // int    remain_level      = total_level - (stc_depth + cts_depth + evalmod_depth);
    
    auto config = bootstrap::BootstrapConfig();
    config.crypto_params.logN = logN;
    config.crypto_params.logSlots = full_logn;
    config.crypto_params.hamming_weight = hw;
    if (config.crypto_params.hamming_weight == 192)
    {
        config.evalmod_params.n_double_angle = 2;
        config.evalmod_params.degree = 59;
    }
    else if (config.crypto_params.hamming_weight == 32)
    {
        config.evalmod_params.n_double_angle = 3;
        config.evalmod_params.degree = 22;
    }
    else
    {
        throw std::invalid_argument("invalid hamming weight");
    }
    config.evalmod_params.inverse_degree = 1;
    config.SetEvalModLogE(10);
    config.SetHoistingMode(hoisting);

#ifndef HYPERFHE_32
    // Set crypto parameters
    int    stc_depth         = 3;
    int    cts_depth         = 4;
    int    remain_level      = total_level - (stc_depth + cts_depth + evalmod_depth);
    config.crypto_params.logQ0 = logp;
    config.crypto_params.logQi = { logq };
    config.crypto_params.level = remain_level;
    config.crypto_params.logP = { log_special_prime };
    config.crypto_params.n_special_prime = 8;

    // Set linear transformation parameters
    config.cts_params.depth = cts_depth;
    config.cts_params.logQ = logp;
    config.stc_params.depth = stc_depth;
    config.stc_params.logQ = logp;
    if (config.cts_params.depth == 4)
    {
        config.cts_params.groups = { 4, 4, 4, 3 };
        config.stc_params.groups = { 5, 5, 5 };
    }

    // Set modular reduction parameters
    config.evalmod_params.logQ = logp;
    config.evalmod_params.logE = 10;
    config.EnableHoistedRelin();
    config.EnableHoistedRescale(); 
    config.EnableScaleAlignment();
    config.EnableEvalModMergeMDRS();

    // Set linear transformation strategy
    config.EnableFusedBabySteps();
    config.EnableBatchPMAC();
    config.EnableFusedReduceMAC(); // fail
    config.EnableLTMergeMDRS();  // add_scalar: scaled value is too large

    // Set scale parameters
    config.msg_scale = pow(2.0, logq);   // Scale for encoding messages
    config.final_scale = pow(2.0, logq); // Scale after bootstrapping

    // Set bootstrap mode
    config.slim_bootstrap = false;
    config.real_input = true;

    bool enable_score;
    if(config.slim_bootstrap)
      enable_score = false;
    else
      enable_score = false; //can be enabled, but it may impact precision.

    // Enable SCORE optimization
    config.EnableSCORE(enable_score);

    auto bts_parms = EvalBootstrap::generate_bts_context(config);

    const int N = 1 << bts_parms.logN_;
    std::cout << "Init Context..." << std::endl;
    frontend::HostParameter parms(frontend::scheme_type::ckks);
    parms.set_poly_modulus_degree(N);

    parms.set_coeff_modulus(frontend::CoeffModulus::Create(N, bts_parms.modchain_.logQs_));
#else
//     enum class PARMS_SET
// {
//     BOOT_DEMO = 0,
//     SET_C,
//     SET_D,
//     SET_E,
//     SET_NUM
// };
// #include <string>
// static std::array<std::string, size_t(PARMS_SET::SET_NUM)> PARMS_SET_NAME{ "BBOT_DEMO", "SET_C", "SET_D", "SET_E" };

//     PARMS_SET parms_sets = PARMS_SET::SET_E;
//     std::cout << "Parameter Set: " << PARMS_SET_NAME[size_t(parms_sets)] << std::endl;

    // Common parameters for all sets
    int    stc_depth         = 3;
    int    cts_depth         = 3;
    int    remain_level      = total_level - (stc_depth + cts_depth + evalmod_depth);
    config
        .SetCtSDepth(cts_depth).SetCtSLogQ(55)
        .SetStCDepth(stc_depth).SetStCLogQ(55)
        .SetEvalModLogQ(55)
        .SetSlimBootstrap(true)
        .SetRealInput(false)
        .EnableEvalModMergeMDRS(true)
        .EnableHoistedRescale(true) 
        .EnableHoistedRelin(false) //set false in wordsize-32 mode
        .EnableScaleAlignment(false); //set false in wordsize-32 mode

    // Set specific parameters based on parameter set
    logq = 46
    config.crypto_params.logQ0 = 55;
    config.crypto_params.logQi = { logq };
    config.crypto_params.level = remain_level;
    config.crypto_params.n_special_prime = 16;
    config.crypto_params.logP = { 55 };
    config.evalmod_params.logQ = 55;
    config.final_scale = pow(2.0, logq);


    auto bts_parms = EvalBootstrap::generate_bts_context(config);

    const int N = 1 << bts_parms.logN_;
    std::cout << "Init Context..." << std::endl;
    frontend::HostParameter parms(frontend::scheme_type::ckks);
    parms.set_poly_modulus_degree(N);

    /*
    bit size: 27
    # primes: 60
    0x7e00001, 0x7cc0001, 0x79e0001, 0x79c0001, 0x78c0001, 0x7860001, 0x77e0001,
    0x7740001, 0x7620001, 0x7300001, 0x72c0001, 0x7140001, 0x7080001, 0x6ea0001,
    0x6de0001, 0x6dc0001, 0x6cc0001, 0x6c00001, 0x6be0001, 0x6ae0001, 0x6a20001,
    0x6a00001, 0x6640001, 0x6520001, 0x6400001, 0x6280001, 0x61e0001, 0x6100001,
    0x5f80001, 0x5ee0001, 0x5e60001, 0x5e20001, 0x5b60001, 0x5aa0001, 0x5980001,
    0x5940001, 0x58e0001, 0x5640001, 0x5560001, 0x5340001, 0x51a0001, 0x5140001,
    0x4fc0001, 0x4f80001, 0x4ea0001, 0x4e00001, 0x4c60001, 0x4ae0001, 0x4980001,
    0x4920001, 0x47a0001, 0x4740001, 0x4560001, 0x4540001, 0x4420001, 0x4300001,
    0x4200001, 0x4180001, 0x4060001, 0x4020001
    */
    if (bts_parms.evalmod_handler_.parms().skip_)
    {
        parms.set_coeff_modulus(
            frontend::CoeffModulus::Create(
                { /* Q0 */
                  0x1320001, 0x3ee0001,
                  /* StC */
                  0xd00001, 0xd80001, 0xe40001, 0xf60001, 0xfa0001, 0xfc0001,
                  /* CtS */
                  0x1aa0001, 0x3720001, 0x1b00001, 0x3820001, 0x1b60001, 0x38e0001,
                  /* NSP */
                  0x1c80001, 0x39a0001, 0x1d20001, 0x3ae0001, 0x1de0001, 0x3cc0001, 0x1f60001, 0x3dc0001, 0x1fc0001,
                  0x2680001 }));
    }
    else
    {
        switch (parms_sets)
        {
        case PARMS_SET::BOOT_DEMO:
            parms.set_coeff_modulus(
                frontend::CoeffModulus::Create(
                    { /* Q0 */
                      0x7e00001, 0xffa0001,
                      /* StC */
                      0x6520001, 0x6640001, 0x6a00001, 0x6a20001, 0x6ae0001, 0x61e0001,
                      /* EvalMod */
                      0xaa80001, 0xaaa0001, 0xb820001, 0xac60001, 0xae60001, 0xaf20001, 0xb680001, 0xb7a0001, 0xb2c0001,
                      0xb040001, 0xb560001, 0xb100001, 0xb2e0001, 0xb200001, 0xb3e0001, 0xb4c0001,
                      /* CtS */
                      0x1aa0001, 0x3720001, 0x1b00001, 0x3820001, 0x1b60001, 0x38e0001,
                      /* NSP */
                      0xfd20001, 0xc060001, 0xfc60001, 0xc300001, 0xfb40001, 0xc360001, 0xfb20001, 0xc420001, 0xf9c0001,
                      0xc4c0001 }));
            break;
        case PARMS_SET::SET_C:
            parms.set_coeff_modulus(
                frontend::CoeffModulus::Create(
                    { /* Q0 */
                      0x7e00001, 0xffa0001,
                      /* StC */
                      0x6520001, 0x6640001, 0x6a00001, 0x6a20001, 0x6ae0001, 0x61e0001,
                      /* Normal */
                      0xd00001, 0xd80001, 0xe40001, 0xf60001, 0xfa0001, 0xfc0001,
                      /* EvalMod */
                      0xaa80001, 0xaaa0001, 0xb820001, 0xac60001, 0xae60001, 0xaf20001, 0xb680001, 0xb7a0001, 0xb2c0001,
                      0xb040001, 0xb560001, 0xb100001, 0xb2e0001, 0xb200001, 0xb3e0001, 0xb4c0001,
                      /* CtS */
                      0x1aa0001, 0x3720001, 0x1b00001, 0x3820001, 0x1b60001, 0x38e0001,
                      /* NSP */
                      0xfd20001, 0xc060001, 0xfc60001, 0xc300001, 0xfb40001, 0xc360001, 0xfb20001, 0xc420001, 0xf9c0001,
                      0xc4c0001, 0xf960001, 0xc640001 }));
            break;
        case PARMS_SET::SET_E:
            parms.set_coeff_modulus(
                frontend::CoeffModulus::Create(
                    { /* Q0 */
                      0x7e00001, 0xffa0001,
                      /* StC */
                      0x6520001, 0x6640001, 0x6a00001, 0x6a20001, 0x6ae0001, 0x61e0001,
                      /* Normal */
                      0xd00001, 0xd80001, 0xe40001, 0xf60001, 0xfa0001, 0xfc0001, 0x6be0001, 0x6c00001, 0x6400001,
                      0x6cc0001, 0x6dc0001, 0x6de0001, 0x6ea0001, 0x7080001, 0x7620001, 0x7300001, 0x72c0001, 0x7140001,
                      /* EvalMod */
                      0xaa80001, 0xaaa0001, 0xb820001, 0xac60001, 0xae60001, 0xaf20001, 0xb680001, 0xb7a0001, 0xb2c0001,
                      0xb040001, 0xb560001, 0xb100001, 0xb2e0001, 0xb200001, 0xb3e0001, 0xb4c0001,
                      /* CtS */
                      0x1aa0001, 0x3720001, 0x1b00001, 0x3820001, 0x1b60001, 0x38e0001,
                      /* NSP */
                      0xfd20001, 0xc060001, 0xfc60001, 0xc300001, 0xfb40001, 0xc360001, 0xfb20001, 0xc420001, 0xf9c0001,
                      0xc4c0001, 0xf960001, 0xc640001, 0xf4e0001, 0xc760001, 0xf480001, 0xc8a0001 }));
            break;
        default:
            throw std::invalid_argument("invalid parms-set id");
        }
    }
#endif
    // modified SEAL
    parms.set_secret_key_hamming_weight(config.crypto_params.hamming_weight);
    const long sparse_slots = (1 << bts_parms.logSlots_);
    if (sparse_slots != N / 2)
    {
        parms.set_sparse_slots(sparse_slots);
    }
    parms.set_n_special_primes(config.crypto_params.n_special_prime);
    parms.set_scale_flag(bts_parms.modchain_.scale_flags_);

    
    // 通过引用参数输出
    parms_out     = parms;
    bts_parms_out = std::move(bts_parms);

    // HostContext context(parms, true, sec_level_type::none);
}

#endif

private:
  HYPER_CONTEXT(const HYPER_CONTEXT&)            = delete;
  HYPER_CONTEXT& operator=(const HYPER_CONTEXT&) = delete;

  HYPER_CONTEXT();
  ~HYPER_CONTEXT();

  static HYPER_CONTEXT* Instance;

private:
  frontend::HostContext*  _cpu_ctx;
  frontend::KeyGenerator* _kgen;

  const frontend::SecretKey* _sk;
  frontend::PublicKey*       _pk;
  frontend::RelinKeys*       _rlk;
  frontend::GaloisKeys*      _rtk;

  // frontend::Evaluator*   _cpu_eval;
  frontend::CKKSEncoder* _encoder;
  frontend::Encryptor*   _encryptor;
  frontend::Decryptor*   _decryptor;
// #ifdef HYPER_BTS_MACRO
//   frontend::GaloisKeys*      _hoisted_rtk;
//   Bootstrapper* _boot                = nullptr;
//   // uint32_t      _bts_remaining_level = 16;
//   // uint32_t      _bts_required_level  = 14;
// #endif

#ifdef GPU_BACKEND
  cuckks::MEMORY_LAYOUT _data_layout;
  cuckks::ContextPro* _gpu_ctx;
  cuckks::Evaluator* _gpu_eval;
  cuckks::ContextPro* _ctx;
  cuckks::Evaluator* _eval;
  frontend::HostContext* _host_context;
#ifdef HYPER_BTS_MACRO
  EvalBootstrap::BootstrapContext _bts_parms; 
  EvalBootstrap* _boot; 
  DeviceEvaluator* _dev_eval;               
#endif
#else
  frontend::HostContext* _ctx;
  frontend::Evaluator*  _eval;
#endif

  uint64_t _scaling_mod_size;
  uint64_t _input_level;
  uint64_t _base_mod_num;
  bool _boot_cached;
};

HYPER_CONTEXT* HYPER_CONTEXT::Instance = nullptr;

HYPER_CONTEXT::HYPER_CONTEXT() {
  IS_TRUE(Instance == nullptr, "_install already created");

  CKKS_PARAMS* prog_param = Get_context_params();
  IS_TRUE(prog_param->_provider == LIB_HYPERFHE, "provider is not HyperFHE");

  frontend::HostParameter parms(frontend::scheme_type::ckks);
  uint32_t                   degree = prog_param->_poly_degree;
  parms.set_poly_modulus_degree(degree);
  int n_special_primes;
  if(prog_param->_num_q_parts){
    n_special_primes = prog_param->_num_q_parts;
  }else{
    int dnum = 3;
    n_special_primes = (prog_param->_mul_depth)/dnum + 1;
  }

  if(!Need_bts())
  {
#ifdef HYPERFHE_32
    std::vector<uint64_t> modulus;
    std::vector<double> scales;
    std::string mod_err;
    if (!build_mod_vector(prog_param->_mul_depth, n_special_primes, modulus, mod_err)) {
      throw std::runtime_error("HyperContext Init Failed: " + mod_err);
    }
    if (!calculate_scales_per_level(prog_param->_scaling_mod_size, modulus, prog_param->_mul_depth, n_special_primes, scales, mod_err)) {
      throw std::runtime_error("HyperContext Init Failed: " + mod_err);
    }

    parms.set_scale_per_level(scales);
    parms.set_n_special_primes(n_special_primes * 2);
    std::vector<int> scale_flag_vec;
    for (int i = 0; i < 1 + prog_param->_mul_depth; i++){scale_flag_vec.push_back(0); scale_flag_vec.push_back(2);}
    parms.set_scale_flag(scale_flag_vec);
    parms.set_coeff_modulus(frontend::CoeffModulus::Create(modulus));
    // #endif
#else   // #ifdef HYPERFHE_32
    std::vector<int> bits;
    bits.push_back(prog_param->_first_mod_size);

    for (uint32_t i = 0; i < prog_param->_mul_depth; ++i) {
      bits.push_back(prog_param->_scaling_mod_size);
    }
    for (uint32_t i = 0; i < n_special_primes; ++i) {
      bits.push_back(prog_param->_first_mod_size);
    }
    if(n_special_primes > 1)
      parms.set_n_special_primes(n_special_primes);
    parms.set_coeff_modulus(frontend::CoeffModulus::Create(degree, bits));
#endif
  }

  frontend::sec_level_type sec = frontend::sec_level_type::tc128;
  switch (prog_param->_sec_level) {
    case 128:
      sec = frontend::sec_level_type::tc128;
      break;
    case 192:
      sec = frontend::sec_level_type::tc192;
      break;
    case 256:
      sec = frontend::sec_level_type::tc256;
      break;
    default:
      sec = frontend::sec_level_type::none;
      break;
  }
  if (degree < 4096 && sec != frontend::sec_level_type::none) {
    DEV_WARN("WARNING: degree %d too small, reset security level to none\n",
             degree);
    sec = frontend::sec_level_type::none;
  }
  
  

#ifdef GPU_BACKEND
  bool hoisting = false;
  bool transpose = false;
  _data_layout = transpose ? cuckks::MEMORY_LAYOUT::COLUMN_MAJOR : cuckks::MEMORY_LAYOUT::ROW_MAJOR;
  if(Need_bts())
  {
#ifdef HYPER_BTS_MACRO
  frontend::HostParameter host_bts_parms(frontend::scheme_type::ckks);
  make_bts_parms(prog_param, host_bts_parms, _bts_parms);
  _host_context = new frontend::HostContext(host_bts_parms, true, frontend::sec_level_type::none);

  // Enable imaginary-removing bootstrapping
  _bts_parms.set_remove_imag(true);

  _cpu_ctx  = _host_context;
#else // #ifdef HYPER_BTS_MACRO
  FMT_ASSERT(false, "HYPER_CONTEXT: HYPER_BTS_MACRO Not Defined.");
#endif 
  }
  else
  {
    _cpu_ctx  = new frontend::HostContext(parms, true, sec);
  }  
  

  _kgen = new frontend::KeyGenerator(*_cpu_ctx);
  _sk   = &_kgen->secret_key();
  _pk   = new frontend::PublicKey;
  _kgen->create_public_key(*_pk);
  _rlk = new frontend::RelinKeys;
  _kgen->create_relin_keys(*_rlk);
  _rtk = new frontend::GaloisKeys;

  _encryptor = new frontend::Encryptor(*_cpu_ctx, *_pk, *_sk);
  _decryptor = new frontend::Decryptor(*_cpu_ctx, *_sk);
  _encoder   = new frontend::CKKSEncoder(*_cpu_ctx);
  _scaling_mod_size = prog_param->_scaling_mod_size;
  if(prog_param->_input_level > prog_param->_mul_depth)
    std::cout << "prog_param->_input_level > prog_param->_mul_depth!" <<std::endl;
  _input_level = prog_param->_input_level;
#ifdef HYPERFHE_32
  _base_mod_num = 2;
#else
  _base_mod_num = 1;
#endif
  _boot_cached = false;

  if(Need_bts())
  {
#ifdef HYPER_BTS_MACRO

    // frontend::CKKSEncoder encoder(*_host_context);
    // EvalWrapper gpu_evaluator(encoder, ContextPro::get_device_context(context), true);
    _dev_eval = new EvalWrapper(*_encoder, cuckks::ContextPro::get_device_context(*_host_context), true);
    
    // Create a copy of bts_parms since we need to move it
    auto bts_parms_copy = _bts_parms;
    // EvalBootstrap bootstrapper(std::move(bts_parms_copy), *_gpu_eval);
    _boot = new EvalBootstrap(std::move(bts_parms_copy), *_dev_eval);
    hoisting = _boot->is_hoisted();
#ifdef USE_NVTX
    _boot->profile = true;
#endif
    _boot->get_bootstrap_aux(_gal_steps_vector);
    Set_gal_steps();
    _kgen->create_galois_keys(_gal_steps_vector, *_rtk, hoisting);
    _dev_eval->enable_memory_pool(0.8);
    _dev_eval->get_publickey_from(*_pk);
    _dev_eval->get_secretkey_from(*_sk);
    _dev_eval->get_evk_from(*_rlk, _data_layout);
    _dev_eval->get_evk_from(*_rtk, _data_layout);
    _gpu_eval = _dev_eval;
#else  //#ifdef HYPER_BTS_MACRO
    FMT_ASSERT(false, "HYPER_CONTEXT: HYPER_BTS_MACRO Not Defined.");
#endif
  }
  else
  {
    Set_gal_steps();
    std::cout << std::endl;
    _kgen->create_galois_keys(_gal_steps_vector, *_rtk);
    _gpu_eval = new cuckks::Evaluator(cuckks::ContextPro::get_device_context(*_cpu_ctx), true);
    _gpu_eval->enable_memory_pool(0.4);
    _gpu_eval->get_publickey_from(*_pk);
    _gpu_eval->get_secretkey_from(*_sk);
    _gpu_eval->get_evk_from(*_rlk, _data_layout);
    _gpu_eval->get_evk_from(*_rtk, _data_layout);
  }
  
  _ctx = const_cast<cuckks::ContextPro*>(&(_gpu_eval->context()));
  _gpu_ctx = const_cast<cuckks::ContextPro*>(&(_gpu_eval->context()));
  _eval = _gpu_eval;

#endif// #ifdef GPU_BACKEND 

  printf(
      "ckks_param: _provider = %d, _poly_degree = %d, _sec_level = %ld, "
      "mul_depth = %ld, _first_mod_size = %ld, _scaling_mod_size = %ld, "
      "_num_q_parts = %ld, _num_rot_idx = %ld\n",
      prog_param->_provider, prog_param->_poly_degree, prog_param->_sec_level,
      prog_param->_mul_depth, prog_param->_first_mod_size,
      prog_param->_scaling_mod_size, prog_param->_num_q_parts,
      prog_param->_num_rot_idx);
}

HYPER_CONTEXT::~HYPER_CONTEXT() {
  delete _decryptor;
  delete _encryptor;
  delete _encoder;
  // delete _cpu_eval;

  delete _rtk;
  delete _rlk;
  delete _pk;
  // delete _sk;

  delete _kgen;
  delete _cpu_ctx;

#ifdef GPU_BACKEND
#ifdef HYPER_BTS_MACRO 
 delete _boot;
#endif
  // delete _gpu_ctx;
  delete _gpu_eval;

#endif
}

// Vendor-specific RT API
void Prepare_context() {
  Init_rtlib_timing();
  Io_init();
  HYPER_CONTEXT::Init_context();
}

void Finalize_context() {
  HYPER_CONTEXT::Fini_context();
  Io_fini();
}

void Prepare_input(TENSOR* input, const char* name) {
  HYPER_CONTEXT::Context()->Prepare_input(input, name);
}

double* Handle_output(const char* name) {
  return HYPER_CONTEXT::Context()->Handle_output(name, 0);
}

// Encode/Decode API
void Hyperfhe_set_output_data(const char* name, size_t idx, CIPHER data) {
  HYPER_CONTEXT::Context()->Set_output_data(name, idx, data);
}

CIPHERTEXT Hyperfhe_get_input_data(const char* name, size_t idx) {
  return HYPER_CONTEXT::Context()->Get_input_data(name, idx);
}

void Hyperfhe_encode_float(PLAIN pt, float* input, size_t len, SCALE_T scale,
                       LEVEL_T level) {
  HYPER_CONTEXT::Context()->Encode_float(pt, input, len, scale, level);
}

// void Hyperfhe_encode_double(PLAIN pt, double* input, size_t len, SCALE_T scale,
//                         LEVEL_T level) {
//   HYPER_CONTEXT::Context()->Encode_double(pt, input, len, scale, level);
// }

void Hyperfhe_encode_float_cst_lvl(PLAIN pt, float* input, size_t len,
                               SCALE_T scale, int level) {
  HYPER_CONTEXT::Context()->Encode_float_cst_lvl(pt, input, len, scale, level);
}

// void Hyperfhe_encode_double_cst_lvl(PLAIN pt, double* input, size_t len,
//                                 SCALE_T scale, int level) {
//   HYPER_CONTEXT::Context()->Encode_double_cst_lvl(pt, input, len, scale, level);
// }

void Hyperfhe_encode_float_mask(PLAIN pt, float input, size_t len, SCALE_T scale,
                            LEVEL_T level) {
  HYPER_CONTEXT::Context()->Encode_float_mask(pt, input, len, scale, level);
}

// void Hyperfhe_encode_double_mask(PLAIN pt, double input, size_t len, SCALE_T scale,
//                              LEVEL_T level) {
//   HYPER_CONTEXT::Context()->Encode_double_mask(pt, input, len, scale, level);
// }

// void Hyperfhe_encode_float_mask_cst_lvl(PLAIN pt, float input, size_t len,
//                                     SCALE_T scale, int level) {
//   HYPER_CONTEXT::Context()->Encode_float_mask_cst_lvl(pt, input, len, scale,
//                                                      level);
// }

// void Hyperfhe_encode_double_mask_cst_lvl(PLAIN pt, double input, size_t len,
//                                      SCALE_T scale, int level) {
//   HYPER_CONTEXT::Context()->Encode_double_mask_cst_lvl(pt, input, len, scale,
//                                                       level);
// }

// Evaluation API
void Hyperfhe_add_ciph(CIPHER res, CIPHER op1, CIPHER op2) {
  if (op1->size() == 0) {
    // special handling for accumulation
    *res = *op2;
    return;
  }
  // std::cout << "Hyperfhe_add_ciph: op1" <<" data_layout:" << (int)(op1->data_layout())<< std::endl;
  // std::cout << "Hyperfhe_add_ciph: op2" <<" data_layout:" << (int)(op2->data_layout())<< std::endl;
  // std::cout << "op1" <<" address:" << (op1)<< std::endl;
  // std::cout << "op2" <<" address:" << op2 << std::endl;
  HYPER_CONTEXT::Context()->Add(op1, op2, res);
}

void Hyperfhe_add_plain(CIPHER res, CIPHER op1, PLAIN op2) {
  HYPER_CONTEXT::Context()->Add(op1, op2, res);
}

void Hyperfhe_add_scalar(CIPHER res, CIPHER op1, double op2) {
  HYPER_CONTEXT::Context()->Add(op1, op2, res);
}

void Hyperfhe_mul_ciph(CIPHER res, CIPHER op1, CIPHER op2) {
  HYPER_CONTEXT::Context()->Mul(op1, op2, res);
}

void Hyperfhe_mul_plain(CIPHER res, CIPHER op1, PLAIN op2) {
  HYPER_CONTEXT::Context()->Mul(op1, op2, res);
}

void Hyperfhe_mul_scalar(CIPHER res, CIPHER op1, double op2) {
  HYPER_CONTEXT::Context()->Mul(op1, op2, res);
}

void Hyperfhe_rotate(CIPHER res, CIPHER op, int step) {
  HYPER_CONTEXT::Context()->Rotate(op, step, res);
}

void Hyperfhe_rescale(CIPHER res, CIPHER op) {
  HYPER_CONTEXT::Context()->Rescale(op, res);
}

// void Hyperfhe_mod_switch(CIPHER res, CIPHER op) {
//   HYPER_CONTEXT::Context()->Mod_switch(op, res);
// }

void Hyperfhe_relin(CIPHER res, CIPHER3 op) {
  HYPER_CONTEXT::Context()->Relin(op, res);
}

void Hyperfhe_bootstrap(CIPHER res, CIPHER op, int level) {
  HYPER_CONTEXT::Context()->Bootstrap(op, res, level);
}

void Hyperfhe_copy(CIPHER res, CIPHER op) { *res = *op; }

void Hyperfhe_zero(CIPHER res) { if(res) *res = CIPHERTEXT(); }

SCALE_T Hyperfhe_scale_degree(CIPHER res) { return HYPER_CONTEXT::Context()->Scale_degree(res); }

SCALE_T Hyperfhe_scale(CIPHER res) { return HYPER_CONTEXT::Context()->Normalized_Scale(res); }

LEVEL_T Hyperfhe_level(CIPHER res) { return HYPER_CONTEXT::Context()->Level(res); }

// Debug API
void Dump_ciph(CIPHER ct, size_t start, size_t len) {
  std::vector<double> vec;
  HYPER_CONTEXT::Context()->Decrypt(ct, vec);
  size_t max = std::min(vec.size(), start + len);
  for (size_t i = start; i < max; ++i) {
    std::cout << vec[i] << " ";
  }
  std::cout << std::endl;
}

// void Dump_plain(PLAIN pt, size_t start, size_t len) {
//   std::vector<double> vec;
//   HYPER_CONTEXT::Context()->Decode(pt, vec);
//   size_t max = std::min(vec.size(), start + len);
//   for (size_t i = start; i < max; ++i) {
//     std::cout << vec[i] << " ";
//   }
//   std::cout << std::endl;
// }

// void Dump_cipher_msg(const char* name, CIPHER ct, uint32_t len) {
//   std::cout << "[" << name << "]: ";
//   Dump_ciph(ct, 16, len);
// }

// void Dump_plain_msg(const char* name, PLAIN pt, uint32_t len) {
//   std::cout << "[" << name << "]: ";
//   Dump_plain(pt, 16, len);
// }

// double* Get_msg(CIPHER ct) {
//   std::vector<double> vec;
//   HYPER_CONTEXT::Context()->Decrypt(ct, vec);
//   double* msg = (double*)malloc(sizeof(double) * vec.size());
//   memcpy(msg, vec.data(), sizeof(double) * vec.size());
//   return msg;
// }

// double* Get_msg_from_plain(PLAIN pt) {
//   std::vector<double> vec;
//   HYPER_CONTEXT::Context()->Decode(pt, vec);
//   double* msg = (double*)malloc(sizeof(double) * vec.size());
//   memcpy(msg, vec.data(), sizeof(double) * vec.size());
//   return msg;
// }

bool Within_value_range(CIPHER ciph, double* msg, uint32_t len) {
  FMT_ASSERT(false, "TODO: not implemented.");
}

bool Need_bts() { return false; }
