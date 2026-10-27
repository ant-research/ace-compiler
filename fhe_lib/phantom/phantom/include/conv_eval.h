#pragma once

#include <string>
#include <vector>

#include "boot/ckks_evaluator.cuh"
#include "boot/Bootstrapper.cuh"
namespace phantom {

PhantomCiphertext evalConv_BNRelu_new(
    const ::PhantomContext &context,
    CKKSEvaluator &ckks_evaluator,
    PhantomCKKSEncoder &encoder,
    const ::PhantomCiphertext &ct_input,
    const std::vector<double> &ker_in,
    const std::vector<double> &bn_a,
    const std::vector<double> &bn_b,
    double alpha,
    double pow,
    int in_wid,
    int kp_wid,
    int ker_wid,
    int real_ib,
    int real_ob,
    int norm,
    int pack_pos,
    int step,
    int iter,
    int log_sparse,
    const std::string &kind,
    bool fast_pack,
    bool debug);

PhantomCiphertext evalConv_BNRelu_new_cached(
    const ::PhantomContext &context,
    CKKSEvaluator &ckks_evaluator,
    PhantomCKKSEncoder &encoder,
    ::Bootstrapper &bootstrapper,
    const ::PhantomCiphertext &ct_input,
    const std::vector<double> &ker_in,
    const std::vector<double> &bn_a,
    const std::vector<double> &bn_b,
    double alpha,
    double pow,
    int in_wid,
    int kp_wid,
    int ker_wid,
    int real_ib,
    int real_ob,
    int norm,
    int pack_pos,
    int step,
    int iter,
    int log_sparse,
    const std::string &kind,
    bool fast_pack,
    bool debug);

PhantomCiphertext evalConv_BN(
    const ::PhantomContext &context,
    CKKSEvaluator &ckks_evaluator,
    PhantomCKKSEncoder &encoder,
    const ::PhantomCiphertext &ct_input,
    const std::vector<double> &ker_in,
    const std::vector<double> &bn_a,
    const std::vector<double> &bn_b,
    int in_wid,
    int ker_wid,
    int real_ib,
    int real_ob,
    int norm,
    double out_scale,
    bool trans);

PhantomCiphertext evalConv_BN_BL_test(
    const ::PhantomContext &context,
    CKKSEvaluator &ckks_evaluator,
    PhantomCKKSEncoder &encoder,
    const ::PhantomCiphertext &ct_input,
    const std::vector<double> &ker_in,
    const std::vector<double> &bn_a,
    const std::vector<double> &bn_b,
    int in_wid,
    int ker_wid,
    int real_ib,
    int real_ob,
    int pos,
    int norm,
    int pad,
    bool trans,
    bool printResult);

} // namespace phantom
