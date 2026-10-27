#include "conv_eval.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include "phantom_debug.h"
#include <stdexcept>

using namespace std;

namespace phantom
{

    static void print_first_n(const vector<double> &values, size_t n);
    static void print_first_n(const vector<complex<double>> &values, size_t n);
    static double log2_scale(double scale);

    static void print_first_n(const vector<double> &values, size_t n)
    {
        size_t limit = std::min(n, values.size());
        for (size_t i = 0; i < limit; i++)
        {
            PHANTOM_LOG_RAW(values[i] << (i + 1 == limit ? "\n" : ", "));
        }
    }

    static void print_first_n(const vector<complex<double>> &values, size_t n)
    {
        size_t limit = std::min(n, values.size());
        for (size_t i = 0; i < limit; i++)
        {
            PHANTOM_LOG_RAW(values[i] << (i + 1 == limit ? "\n" : ", "));
        }
    }

    static double log2_scale(double scale)
    {
        return std::log2(scale);
    }

    static vector<vector<double>> reshape_ker(const vector<double> &ker_in,
                                              int k_sz,
                                              int out_batch,
                                              bool trans)
    {
        vector<vector<double>> ker_out(out_batch, vector<double>(k_sz * (static_cast<int>(ker_in.size()) / (k_sz * out_batch)), 0.0));
        int in_batch = static_cast<int>(ker_in.size()) / (k_sz * out_batch);

        for (int i = 0; i < out_batch; i++)
        {
            for (int j = 0; j < in_batch; j++)
            {
                for (int k = 0; k < k_sz; k++)
                {
                    if (trans)
                    {
                        ker_out[i][j * k_sz + (k_sz - 1 - k)] =
                            ker_in[j + i * in_batch + k * out_batch * in_batch];
                    }
                    else
                    {
                        ker_out[i][j * k_sz + k] =
                            ker_in[i + j * out_batch + k * out_batch * in_batch];
                    }
                }
            }
        }

        return ker_out;
    }

    static vector<double> encode_ker_final(const vector<vector<double>> &ker_in,
                                           int pos,
                                           int i,
                                           int in_wid,
                                           int in_batch,
                                           int ker_wid)
    {
        int vec_size = in_wid * in_wid * in_batch;
        vector<double> output(vec_size, 0.0);
        int bias = pos * ker_wid * ker_wid * in_batch;
        int k_sz = ker_wid * ker_wid;

        for (int j = 0; j < in_batch; j++)
        {
            for (int k = 0; k < k_sz; k++)
            {
                output[(in_wid * (k / ker_wid) + k % ker_wid) * in_batch + j] =
                    ker_in[i][(in_batch - 1 - j) * k_sz + (k_sz - 1 - k) + bias];
            }
        }

        int adj = (in_batch - 1) + (in_batch) * (in_wid + 1) * (ker_wid - 1) / 2;
        vector<double> tmp(adj, 0.0);
        for (int idx = 0; idx < adj; idx++)
        {
            tmp[idx] = output[vec_size - adj + idx];
            output[vec_size - adj + idx] = -output[idx];
        }
        for (int idx = 0; idx < vec_size - 2 * adj; idx++)
        {
            output[idx] = output[idx + adj];
        }
        for (int idx = 0; idx < adj; idx++)
        {
            output[idx + vec_size - 2 * adj] = tmp[idx];
        }

        return output;
    }

    static vector<PhantomPlaintext> prep_ker(const PhantomContext &context,
                                             PhantomCKKSEncoder &encoder,
                                             const vector<double> &ker_in,
                                             const vector<double> &bn_a,
                                             int in_wid,
                                             int ker_wid,
                                             int real_ib,
                                             int real_ob,
                                             int norm,
                                             int pos,
                                             bool trans,
                                             double scale)
    {
        int max_bat = static_cast<int>(context.key_context_data().parms().poly_modulus_degree()) / (in_wid * in_wid);
        int ker_size = ker_wid * ker_wid;
        vector<vector<double>> ker_rs = reshape_ker(ker_in, ker_size, real_ob, trans);

        for (int i = 0; i < real_ob; i++)
        {
            for (size_t j = 0; j < ker_rs[i].size(); j++)
            {
                ker_rs[i][j] *= bn_a[i];
            }
        }

        vector<vector<double>> max_ker_rs(max_bat, vector<double>(max_bat * ker_size, 0.0));
        for (int i = 0; i < real_ob; i++)
        {
            for (int j = 0; j < real_ib; j++)
            {
                for (int k = 0; k < ker_size; k++)
                {
                    max_ker_rs[norm * i][norm * j * ker_size + k] =
                        ker_rs[i][j * ker_size + k];
                }
            }
        }

        vector<PhantomPlaintext> pl_ker(max_bat);
        for (int i = 0; i < max_bat; i++)
        {
            encoder.encode_coeffs(context,
                                  encode_ker_final(max_ker_rs, pos, i, in_wid, max_bat, ker_wid),
                                  scale,
                                  pl_ker[i]);
        }

        return pl_ker;
    }

    static vector<PhantomPlaintext> gen_idxNlogs(const PhantomContext &context,
                                                 PhantomCKKSEncoder &encoder)
    {
        size_t N = context.key_context_data().parms().poly_modulus_degree();
        int logN = static_cast<int>(round(log2(static_cast<double>(N))));
        vector<PhantomPlaintext> idx(logN);
        vector<double> coeffs(N, 0.0);

        for (int i = 0; i < logN; i++)
        {
            coeffs[1 << i] = 1.0;
            encoder.encode_coeffs(context, coeffs, 1.0, idx[i]);
            coeffs[1 << i] = 0.0;
        }

        return idx;
    }

    static vector<complex<double>> reshape_input_BL(const vector<double> &input,
                                                    int in_wid)
    {
        vector<complex<double>> out(input.size(), complex<double>(0.0, 0.0));
        int batch = static_cast<int>(input.size()) / (in_wid * in_wid);
        int l = 0;

        for (int i = 0; i < in_wid; i++)
        {
            for (int j = 0; j < in_wid; j++)
            {
                for (int k = 0; k < batch; k++)
                {
                    out[i * in_wid + j + k * in_wid * in_wid] = complex<double>(input[l], 0.0);
                    l++;
                }
            }
        }

        return out;
    }

    static vector<vector<vector<vector<double>>>> reshape_ker_BL(const vector<double> &input,
                                                                 const vector<double> &bn_a,
                                                                 int ker_wid,
                                                                 int inB,
                                                                 int outB,
                                                                 int max_bat,
                                                                 int pos,
                                                                 int norm,
                                                                 bool trans)
    {
        vector<vector<vector<vector<double>>>> ker_rs(ker_wid);
        for (int i = 0; i < ker_wid; i++)
        {
            ker_rs[i].resize(ker_wid);
            for (int j = 0; j < ker_wid; j++)
            {
                ker_rs[i][j].resize(inB);
                for (int ib = 0; ib < inB; ib++)
                {
                    ker_rs[i][j][ib].resize(outB, 0.0);
                    for (int ob = 0; ob < outB; ob++)
                    {
                        if (trans)
                        {
                            if (ib < (inB / 4))
                            {
                                ker_rs[i][j][ib][ob] =
                                    input[(4 * ib + pos) + ob * inB +
                                          (ker_wid - j - 1) * outB * inB +
                                          (ker_wid - i - 1) * outB * inB * ker_wid] *
                                    bn_a[ob];
                            }
                        }
                        else
                        {
                            ker_rs[i][j][ib][ob] =
                                input[ob + ib * outB + j * outB * inB + i * outB * inB * ker_wid] *
                                bn_a[ob];
                        }
                    }
                }
            }
        }

        vector<vector<vector<vector<double>>>> max_ker_rs(ker_wid);
        for (int i = 0; i < ker_wid; i++)
        {
            max_ker_rs[i].resize(ker_wid);
            for (int j = 0; j < ker_wid; j++)
            {
                max_ker_rs[i][j].resize(max_bat);
                for (int ib = 0; ib < max_bat; ib++)
                {
                    max_ker_rs[i][j][ib].resize(max_bat, 0.0);
                }
                for (int ib = 0; ib < inB; ib++)
                {
                    for (int ob = 0; ob < outB; ob++)
                    {
                        max_ker_rs[i][j][norm * ib][norm * ob] = ker_rs[i][j][ib][ob];
                    }
                }
            }
        }

        return max_ker_rs;
    }

    static vector<PhantomCiphertext> preConv_BL(CKKSEvaluator &ckks_evaluator,
                                                const PhantomCiphertext &ct_in,
                                                int in_wid,
                                                int ker_wid)
    {
        int ker_size = ker_wid * ker_wid;
        vector<PhantomCiphertext> ct_in_rots(ker_size);
        int st = -(ker_wid / 2);
        int end = ker_wid / 2;

        int k = 0;
        for (int i = st; i <= end; i++)
        {
            for (int j = st; j <= end; j++)
            {
                int rot = i * in_wid + j;
                if (rot == 0)
                {
                    ct_in_rots[k] = ct_in;
                }
                else
                {
                    ckks_evaluator.evaluator.rotate_vector(
                        const_cast<PhantomCiphertext &>(ct_in),
                        rot,
                        *(ckks_evaluator.galois_keys),
                        ct_in_rots[k]);
                }
                k++;
            }
        }

        return ct_in_rots;
    }

    static PhantomCiphertext postConv_BL(const PhantomContext &context,
                                         PhantomCKKSEncoder &encoder,
                                         CKKSEvaluator &ckks_evaluator,
                                         const vector<PhantomCiphertext> &ct_in_rots,
                                         int in_wid,
                                         int ker_wid,
                                         int rot,
                                         int pad,
                                         const vector<vector<vector<vector<double>>>> &max_ker_rs)
    {
        size_t slots = context.key_context_data().parms().poly_modulus_degree() / 2;
        int max_batch = static_cast<int>(slots / (in_wid * in_wid));
        vector<complex<double>> postKer(slots, complex<double>(0.0, 0.0));

        PhantomCiphertext ct_out;
        int iter = 0;
        for (int i = 0; i < ker_wid; i++)
        {
            for (int j = 0; j < ker_wid; j++)
            {
                for (int k = 0; k < max_batch; k++)
                {
                    for (int ki = 0; ki < (in_wid - pad); ki++)
                    {
                        for (int kj = 0; kj < (in_wid - pad); kj++)
                        {
                            size_t idx = static_cast<size_t>(k * in_wid * in_wid + ki * in_wid + kj);
                            postKer[idx] = complex<double>(
                                max_ker_rs[i][j][k][(k - rot + max_batch) % max_batch], 0.0);
                            if (((ki + i - (ker_wid / 2)) < 0) ||
                                ((ki + i - (ker_wid / 2)) >= (in_wid - pad)) ||
                                ((kj + j - (ker_wid / 2)) < 0) ||
                                ((kj + j - (ker_wid / 2)) >= (in_wid - pad)))
                            {
                                postKer[idx] = complex<double>(0.0, 0.0);
                            }
                        }
                    }
                }

                PhantomPlaintext pl_tmp;
                encoder.encode(context, postKer, ct_in_rots[0].scale(), pl_tmp, ct_in_rots[0].chain_index());
                PhantomCiphertext ct_tmp;
                ckks_evaluator.evaluator.multiply_plain(ct_in_rots[iter], pl_tmp, ct_tmp);
                if (i == 0 && j == 0)
                {
                    ct_out = ct_tmp;
                }
                else
                {
                    ckks_evaluator.evaluator.add(ct_out, ct_tmp, ct_out);
                }
                iter++;
            }
        }

        return ct_out;
    }

    PhantomCiphertext evalConv_BN_BL_test(const PhantomContext &context,
                                          CKKSEvaluator &ckks_evaluator,
                                          PhantomCKKSEncoder &encoder,
                                          const PhantomCiphertext &ct_input,
                                          const vector<double> &ker_in,
                                          const vector<double> &bn_a,
                                          const vector<double> &bn_b,
                                          int in_wid,
                                          int ker_wid,
                                          int real_ib,
                                          int real_ob,
                                          int pos,
                                          int norm,
                                          int pad,
                                          bool trans,
                                          bool printResult)
    {
        (void)printResult;
        int in_size = in_wid * in_wid;
        int out_size = in_size;
        int max_batch = static_cast<int>(context.key_context_data().parms().poly_modulus_degree() / 2 / in_size);

        auto max_ker_rs = reshape_ker_BL(ker_in, bn_a, ker_wid, real_ib, real_ob, max_batch, pos, norm, trans);
        double scale_exp = ct_input.scale() * ct_input.scale();
        if (trans)
        {
            scale_exp = ct_input.scale() * ct_input.scale() * ct_input.scale();
        }

        vector<complex<double>> bn_b_slots(context.key_context_data().parms().poly_modulus_degree() / 2,
                                           complex<double>(0.0, 0.0));
        for (size_t i = 0; i < bn_b.size(); i++)
        {
            for (int j = 0; j < in_wid - pad; j++)
            {
                for (int k = 0; k < in_wid - pad; k++)
                {
                    size_t idx = static_cast<size_t>(j + k * in_wid + norm * out_size * static_cast<int>(i));
                    if (idx < bn_b_slots.size())
                    {
                        bn_b_slots[idx] = complex<double>(bn_b[i], 0.0);
                    }
                }
            }
        }

        PhantomPlaintext pl_bn_b;
        encoder.encode(context, bn_b_slots, scale_exp, pl_bn_b, ct_input.chain_index());

        auto ct_inputs_rots = preConv_BL(ckks_evaluator, ct_input, in_wid, ker_wid);

        int rot_iters = (norm * real_ob == max_batch) ? real_ob : max_batch;
        PhantomCiphertext ct_res;
        for (int i = 0; i < rot_iters; i++)
        {
            PhantomCiphertext ct_tmp = postConv_BL(context, encoder, ckks_evaluator, ct_inputs_rots,
                                                   in_wid, ker_wid, norm * i, pad, max_ker_rs);
            if (i == 0)
            {
                ct_res = ct_tmp;
            }
            else
            {
                PhantomCiphertext ct_rot;
                if (norm * i * out_size == 0)
                {
                    ct_rot = ct_tmp;
                }
                else
                {
                    ckks_evaluator.evaluator.rotate_vector(ct_tmp, norm * i * out_size,
                                                           *(ckks_evaluator.galois_keys), ct_rot);
                }
                ckks_evaluator.evaluator.add(ct_res, ct_rot, ct_res);
            }
        }

        ckks_evaluator.evaluator.add_plain_inplace(ct_res, pl_bn_b);

        return ct_res;
    }

    static PhantomCiphertext pack_ctxts(CKKSEvaluator &ckks_evaluator,
                                        const vector<PhantomCiphertext> &ctxts_in,
                                        int max_cnum,
                                        int real_cnum,
                                        const vector<PhantomPlaintext> &idx,
                                        int logN,
                                        size_t target_chain_index)
    {
        int norm = max_cnum / real_cnum;
        vector<PhantomCiphertext> ctxts(max_cnum);

        for (int i = 0; i < max_cnum; i++)
        {
            if (i % norm == 0)
            {
                ctxts[i] = ctxts_in[i];
                ctxts[i].set_scale(ctxts[i].scale() * static_cast<double>(real_cnum));
            }
        }

        size_t target_chain_index_local = target_chain_index;
        for (int i = 0; i < max_cnum; i += norm)
        {
            if (ctxts[i].chain_index() != 0)
            {
                target_chain_index_local = ctxts[i].chain_index();
                break;
            }
        }

        int step = max_cnum / 2;
        int logStep = 0;
        for (int i = step; i > 1; i /= 2)
        {
            logStep++;
        }
        int j = logN - logStep;
        vector<PhantomPlaintext> idx_sw = idx;
        for (size_t i = 0; i < idx_sw.size(); i++)
        {
            ckks_evaluator.evaluator.mod_switch_to_inplace(idx_sw[i], target_chain_index_local);
        }
        PhantomCiphertext tmp1, tmp2, rot;
        for (; step >= norm; step /= 2)
        {
            for (int i = 0; i < step; i += norm)
            {
                PHANTOM_LOG("[pack] ctxt chain_index: " << ctxts[i + step].chain_index() << ", idx chain_index: " << idx_sw[logStep].chain_index());
                PHANTOM_LOG("[pack] apply_galois elt: " << ((1u << j) + 1));
                ckks_evaluator.evaluator.multiply_plain(ctxts[i + step], idx_sw[logStep], tmp1);
                ckks_evaluator.evaluator.sub(ctxts[i], tmp1, tmp2);
                ckks_evaluator.evaluator.add(ctxts[i], tmp1, tmp1);
                uint32_t elt = (1u << j) + 1;
                ckks_evaluator.evaluator.apply_galois(tmp2, elt, *(ckks_evaluator.galois_keys), rot);
                ckks_evaluator.evaluator.add(tmp1, rot, ctxts[i]);
            }
            logStep--;
            j++;
        }

        return ctxts[0];
    }

    static PhantomCiphertext conv_then_pack(const PhantomContext &context,
                                            CKKSEvaluator &ckks_evaluator,
                                            const PhantomCiphertext &ctxt_in,
                                            const vector<PhantomPlaintext> &pl_ker,
                                            const vector<PhantomPlaintext> &plain_idx,
                                            int max_ob,
                                            int norm,
                                            double out_scale,
                                            int logN)
    {
        vector<PhantomCiphertext> ctxt_out(max_ob);

        for (int i = 0; i < max_ob; i++)
        {
            if (i % norm == 0)
            {
                PHANTOM_LOG("[conv] ctxt_in chain_index: " << ctxt_in.chain_index() << ", pl_ker chain_index: " << pl_ker[i].chain_index());
                ckks_evaluator.evaluator.multiply_plain(ctxt_in, pl_ker[i], ctxt_out[i]);
                // ctxt_out[i].set_scale(out_scale / static_cast<double>(max_ob / norm));
                ckks_evaluator.evaluator.set_scale_inplace(
                    ctxt_out[i], out_scale / static_cast<double>(max_ob / norm));
            }
        }

        return pack_ctxts(ckks_evaluator, ctxt_out, max_ob, max_ob / norm, plain_idx, logN,
                          ctxt_in.chain_index());
    }

    PhantomCiphertext evalConv_BN(const PhantomContext &context,
                                  CKKSEvaluator &ckks_evaluator,
                                  PhantomCKKSEncoder &encoder,
                                  const PhantomCiphertext &ct_input,
                                  const vector<double> &ker_in,
                                  const vector<double> &bn_a,
                                  const vector<double> &bn_b,
                                  int in_wid,
                                  int ker_wid,
                                  int real_ib,
                                  int real_ob,
                                  int norm,
                                  double out_scale,
                                  bool trans)
    {
        int max_batch = static_cast<int>(context.key_context_data().parms().poly_modulus_degree()) / (in_wid * in_wid);
        vector<PhantomPlaintext> pl_ker = prep_ker(context, encoder, ker_in, bn_a,
                                                   in_wid, ker_wid, real_ib, real_ob,
                                                   norm, 0, trans, ct_input.scale());
        vector<PhantomPlaintext> plain_idx = gen_idxNlogs(context, encoder);
        int logN = static_cast<int>(round(log2(static_cast<double>(context.key_context_data().parms().poly_modulus_degree()))));

        size_t target_chain_index = ct_input.chain_index();
        for (int i = 0; i < static_cast<int>(pl_ker.size()); i++)
        {
            ckks_evaluator.evaluator.mod_switch_to_inplace(pl_ker[i], target_chain_index);
        }
        for (int i = 0; i < static_cast<int>(plain_idx.size()); i++)
        {
            ckks_evaluator.evaluator.mod_switch_to_inplace(plain_idx[i], target_chain_index);
        }
        PhantomCiphertext ct_conv = conv_then_pack(context, ckks_evaluator, ct_input, pl_ker,
                                                   plain_idx, max_batch, norm, out_scale, logN);

        vector<double> b_coeffs(context.key_context_data().parms().poly_modulus_degree(), 0.0);
        for (int i = 0; i < real_ib; i++)
        {
            for (int j = 0; j < in_wid * in_wid; j++)
            {
                b_coeffs[norm * i + j * max_batch] = bn_b[i];
            }
        }
        PhantomPlaintext pt_bn_b;
        encoder.encode_coeffs(context, b_coeffs, ct_conv.scale(), pt_bn_b, ct_conv.chain_index());
        PHANTOM_LOG("[bn] ct_conv chain_index: " << ct_conv.chain_index() << ", pt_bn_b chain_index: " << pt_bn_b.chain_index());
        ckks_evaluator.evaluator.add_plain_inplace(ct_conv, pt_bn_b);

        return ct_conv;
    }

    static PhantomCiphertext relu_coeff(const PhantomContext &context,
                                        CKKSEvaluator &ckks_evaluator,
                                        PhantomCKKSEncoder &encoder,
                                        Bootstrapper &bootstrapper,
                                        const PhantomCiphertext &ct_in,
                                        bool debug)
    {
        PhantomCiphertext ct_manual = ct_in;

        if (debug)
        {
            PHANTOM_LOG("[conv] ct_manual scale(log2): " << log2_scale(ct_manual.scale()) << ", chain_index: " << ct_manual.chain_index());
            PhantomPlaintext pt_conv;
            ckks_evaluator.decryptor.decrypt(ct_manual, pt_conv);
            vector<double> coeffs_conv;
            encoder.decode_coeffs(context, pt_conv, coeffs_conv);
            PHANTOM_LOG_RAW("[conv] ct_manual coeffs (first 8): ");
            print_first_n(coeffs_conv, 8);
        }

        bootstrapper.initial_scale = ct_manual.scale();
        while (ct_manual.coeff_modulus_size() > 1)
        {
            ckks_evaluator.evaluator.mod_switch_to_next_inplace(ct_manual);
        }
        bootstrapper.modraise_inplace(ct_manual);

        const auto &modulus = context.first_context_data().parms().coeff_modulus();
        ct_manual.set_scale(static_cast<double>(modulus[0].value()));

        PhantomCiphertext ct_slot_1, ct_slot_2;
        bootstrapper.coefftoslot_full_3(ct_slot_1, ct_slot_2, ct_manual);
        ckks_evaluator.evaluator.multiply_const_inplace(ct_slot_1, 4 * 0.9);
        ckks_evaluator.evaluator.rescale_to_next_inplace(ct_slot_1);
        ckks_evaluator.evaluator.multiply_const_inplace(ct_slot_2, 4 * 0.9);
        ckks_evaluator.evaluator.rescale_to_next_inplace(ct_slot_2);
        PhantomCiphertext ct_red_1, ct_red_2;
        bootstrapper.mod_reducer->modular_reduction_relu(ct_red_1, ct_slot_1);
        bootstrapper.mod_reducer->modular_reduction_relu(ct_red_2, ct_slot_2);

        PhantomCiphertext ct_out;
        bootstrapper.slottocoeff_full_3(ct_out, ct_red_1, ct_red_2);
        ct_out.set_scale(bootstrapper.final_scale);
        if (debug)
        {
            PHANTOM_LOG("[relu] ct_manual scale(log2): " << log2_scale(ct_manual.scale()) << ", chain_index: " << ct_manual.chain_index());
            PHANTOM_LOG("[relu] ct_slot_1 scale(log2): " << log2_scale(ct_slot_1.scale()) << ", chain_index: " << ct_slot_1.chain_index());
            PHANTOM_LOG("[relu] ct_slot_2 scale(log2): " << log2_scale(ct_slot_2.scale()) << ", chain_index: " << ct_slot_2.chain_index());
            PHANTOM_LOG("[relu] ct_red_1 scale(log2): " << log2_scale(ct_red_1.scale()) << ", chain_index: " << ct_red_1.chain_index());
            PHANTOM_LOG("[relu] ct_red_2 scale(log2): " << log2_scale(ct_red_2.scale()) << ", chain_index: " << ct_red_2.chain_index());
            PHANTOM_LOG("[relu] ct_out scale(log2): " << log2_scale(ct_out.scale()) << ", chain_index: " << ct_out.chain_index());
        }
        return ct_out;
    }

    PhantomCiphertext evalConv_BNRelu_new(
        const PhantomContext &context,
        CKKSEvaluator &ckks_evaluator,
        PhantomCKKSEncoder &encoder,
        const PhantomCiphertext &ct_input,
        const vector<double> &ker_in,
        const vector<double> &bn_a,
        const vector<double> &bn_b,
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
        const string &kind,
        bool fast_pack,
        bool debug)
    {
        (void)alpha;
        (void)kp_wid;
        (void)pack_pos;
        (void)step;
        (void)iter;
        (void)fast_pack;
        (void)debug;

        bool trans = false;
        if (kind != "Conv" && kind != "Conv_sparse" && kind != "StrConv_sparse")
        {
            throw invalid_argument("evalConv_BNRelu_new: unsupported kind");
        }

        double out_scale = ct_input.scale();
        PhantomCiphertext ct_conv = evalConv_BN(context, ckks_evaluator, encoder, ct_input,
                                                ker_in, bn_a, bn_b, in_wid, ker_wid,
                                                real_ib, real_ob, norm, out_scale, trans);
        if (debug)
        {
            PHANTOM_LOG("[conv] ct_conv scale(log2): " << log2_scale(ct_conv.scale()) << ", chain_index: " << ct_conv.chain_index());
            PhantomPlaintext pt_conv;
            ckks_evaluator.decryptor.decrypt(ct_conv, pt_conv);
            vector<double> coeffs_conv;
            encoder.decode_coeffs(context, pt_conv, coeffs_conv);
            PHANTOM_LOG_RAW("[conv] ct_conv coeffs (first 8): ");
            print_first_n(coeffs_conv, 8);
        }

        long logN = static_cast<long>(round(log2(static_cast<double>(context.key_context_data().parms().poly_modulus_degree()))));
        long logn = logN - 1;
        if (log_sparse > 0)
        {
            logn = std::max(0L, logn - log_sparse);
        }
        long loge = 10;
        long boundary_K = 25;
        long deg = 59;
        long scale_factor = 2;
        long inverse_deg = 127;
        long total_level = static_cast<long>(context.key_context_data().parms().coeff_modulus().size()) - 1;

        Bootstrapper bootstrapper(loge, logn, logN - 1, total_level, ct_conv.scale(),
                                  boundary_K, deg, scale_factor, inverse_deg, &ckks_evaluator);
        bootstrapper.prepare_mod_polynomial();
        vector<int> gal_steps_vector;
        gal_steps_vector.push_back(0);
        for (int i = 0; i < logN - 1; i++)
        {
            gal_steps_vector.push_back(1 << i);
        }
        bootstrapper.addLeftRotKeys_Linear_to_vector_3(gal_steps_vector);
        ckks_evaluator.decryptor.create_galois_keys_from_steps(gal_steps_vector, *(ckks_evaluator.galois_keys));
        bootstrapper.slot_vec.push_back(logn);
        bootstrapper.generate_LT_coefficient_3();

        return relu_coeff(context, ckks_evaluator, encoder, bootstrapper, ct_conv, debug);
    }

    PhantomCiphertext evalConv_BNRelu_new_cached(
        const PhantomContext &context,
        CKKSEvaluator &ckks_evaluator,
        PhantomCKKSEncoder &encoder,
        Bootstrapper &bootstrapper,
        const PhantomCiphertext &ct_input,
        const vector<double> &ker_in,
        const vector<double> &bn_a,
        const vector<double> &bn_b,
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
        const string &kind,
        bool fast_pack,
        bool debug)
    {
        (void)alpha;
        (void)kp_wid;
        (void)pack_pos;
        (void)step;
        (void)iter;
        (void)fast_pack;
        (void)debug;

        bool trans = false;
        if (kind != "Conv" && kind != "Conv_sparse" && kind != "StrConv_sparse")
        {
            throw invalid_argument("evalConv_BNRelu_new_cached: unsupported kind");
        }

        double out_scale = ct_input.scale();
        PhantomCiphertext ct_conv = evalConv_BN(context, ckks_evaluator, encoder, ct_input,
                                                ker_in, bn_a, bn_b, in_wid, ker_wid,
                                                real_ib, real_ob, norm, out_scale, trans);

        bootstrapper.final_scale = ct_conv.scale();

        return relu_coeff(context, ckks_evaluator, encoder, bootstrapper, ct_conv, debug);
    }

} // namespace phantom
