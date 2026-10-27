#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "phantom.h"

using namespace std;
using namespace phantom;

static vector<double> prep_input(const vector<double> &input,
                                 int raw_in_wid,
                                 int in_wid,
                                 int N,
                                 int norm)
{
    vector<double> out(N, 0.0);
    int batch = N / (in_wid * in_wid);
    int k = 0;

    for (int i = 0; i < in_wid; i++)
    {
        for (int j = 0; j < in_wid; j++)
        {
            for (int b = 0; b < batch / norm; b++)
            {
                if (i < raw_in_wid && j < raw_in_wid)
                {
                    out[i * in_wid * batch + j * batch + b * norm] = input[k];
                    k++;
                }
            }
        }
    }

    return out;
}

static vector<double> read_csv_floats(const string &path)
{
    ifstream in(path);
    if (!in.is_open())
    {
        throw invalid_argument("failed to open file: " + path);
    }

    vector<double> values;
    string line;
    while (getline(in, line))
    {
        if (line.empty())
        {
            continue;
        }
        string token;
        stringstream ss(line);
        while (getline(ss, token, ','))
        {
            if (token.empty())
            {
                continue;
            }
            values.push_back(stod(token));
        }
    }
    return values;
}

static vector<double> post_process(const vector<double> &in_cfs,
                                   int raw_in_wid,
                                   int in_wid)
{
    int batch = static_cast<int>(in_cfs.size()) / (in_wid * in_wid);
    vector<double> out(raw_in_wid * raw_in_wid * batch, 0.0);

    for (int i = 0; i < raw_in_wid; i++)
    {
        for (int j = 0; j < raw_in_wid; j++)
        {
            for (int b = 0; b < batch; b++)
            {
                out[i * raw_in_wid * batch + batch * j + b] =
                    in_cfs[i * in_wid * batch + batch * j + b];
            }
        }
    }

    return out;
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
                              pl_ker[i],1);
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
        encoder.encode_coeffs(context, coeffs, 1.0, idx[i], 3);
        coeffs[1 << i] = 0.0;
    }

    return idx;
}

static PhantomCiphertext pack_ctxts(CKKSEvaluator &ckks_evaluator,
                                    const vector<PhantomCiphertext> &ctxts_in,
                                    int max_cnum,
                                    int real_cnum,
                                    const vector<PhantomPlaintext> &idx,
                                    int logN)
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

    int step = max_cnum / 2;
    int logStep = 0;
    for (int i = step; i > 1; i /= 2)
    {
        logStep++;
    }
    int j = logN - logStep;

    PhantomCiphertext tmp1, tmp2, rot;
    for (; step >= norm; step /= 2)
    {
        for (int i = 0; i < step; i += norm)
        {
            ckks_evaluator.evaluator.multiply_plain(ctxts[i + step], idx[logStep], tmp1);
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
                                        PhantomCKKSEncoder &encoder,
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
            ckks_evaluator.evaluator.multiply_plain(ctxt_in, pl_ker[i], ctxt_out[i]); 
            
            ckks_evaluator.evaluator.set_scale_inplace(
            ctxt_out[i], out_scale / static_cast<double>(max_ob / norm));
            //ctxt_out[i].scale() = out_scale / static_cast<double>(max_ob / norm)

        }
    }

    return pack_ctxts(ckks_evaluator, ctxt_out, max_ob, max_ob / norm, plain_idx, logN);
}

static vector<double> cpu_conv_ref(const vector<double> &raw_input,
                                   const vector<double> &ker_in,
                                   const vector<double> &bn_a,
                                   const vector<double> &bn_b,
                                   int raw_in_wid,
                                   int in_wid,
                                   int ker_wid,
                                   int real_ib,
                                   int real_ob)
{
    int k_sz = ker_wid * ker_wid;
    vector<vector<double>> ker_rs = reshape_ker(ker_in, k_sz, real_ob, false);

    vector<double> out(raw_in_wid * raw_in_wid * real_ob, 0.0);
    for (int ob = 0; ob < real_ob; ob++)
    {
        for (int idx = 0; idx < k_sz * real_ib; idx++)
        {
            ker_rs[ob][idx] *= bn_a[ob];
        }
    }

    for (int ob = 0; ob < real_ob; ob++)
    {
        for (int i = 0; i < raw_in_wid; i++)
        {
            for (int j = 0; j < raw_in_wid; j++)
            {
                double acc = 0.0;
                for (int ib = 0; ib < real_ib; ib++)
                {
                    for (int ki = 0; ki < ker_wid; ki++)
                    {
                        for (int kj = 0; kj < ker_wid; kj++)
                        {
                            int ii = i + ki - ker_wid / 2;
                            int jj = j + kj - ker_wid / 2;
                            if (ii < 0 || jj < 0 || ii >= raw_in_wid || jj >= raw_in_wid)
                            {
                                continue;
                            }
                            double in_val = raw_input[ib + real_ib * (jj + raw_in_wid * ii)];
                            double ker_val = ker_rs[ob][ib * k_sz + ki * ker_wid + kj];
                            acc += in_val * ker_val;
                        }
                    }
                }
                acc += bn_b[ob];
                out[ob + real_ob * (j + raw_in_wid * i)] = acc;
            }
        }
    }

    return out;
}

int main()
{
    const string data_dir = "data/test_conv_data";
    int ker_wid = 3;
    int raw_in_batch = 16;
    int test_iter = 0;

    string prefix = data_dir + "/test_conv" + to_string(ker_wid) +
                    "_batch_" + to_string(raw_in_batch) + "_";
    vector<double> raw_input = read_csv_floats(prefix + "in_" + to_string(test_iter) + ".csv");
    vector<double> ker_in = read_csv_floats(prefix + "ker_" + to_string(test_iter) + ".csv");
    vector<double> bn_a = read_csv_floats(prefix + "bna_" + to_string(test_iter) + ".csv");
    vector<double> bn_b = read_csv_floats(prefix + "bnb_" + to_string(test_iter) + ".csv");
    vector<double> real_out = read_csv_floats(prefix + "out_" + to_string(test_iter) + ".csv");

    int raw_in_wid = static_cast<int>(round(sqrt(static_cast<double>(raw_input.size() / raw_in_batch))));
    if (raw_in_wid * raw_in_wid * raw_in_batch != static_cast<int>(raw_input.size()))
    {
        throw invalid_argument("raw_input size mismatch");
    }
    int in_wid = raw_in_wid + ker_wid / 2;
    int ker_size = ker_wid * ker_wid;
    int raw_out_batch = static_cast<int>(ker_in.size()) / (ker_size * raw_in_batch);
    if (ker_size * raw_in_batch * raw_out_batch != static_cast<int>(ker_in.size()))
    {
        throw invalid_argument("ker_in size mismatch");
    }
    if (static_cast<int>(bn_a.size()) != raw_out_batch)
    {
        throw invalid_argument("bn_a size mismatch");
    }
    if (static_cast<int>(bn_b.size()) != raw_out_batch)
    {
        throw invalid_argument("bn_b size mismatch");
    }
    if (static_cast<int>(real_out.size()) != raw_in_wid * raw_in_wid * raw_in_batch)
    {
        throw invalid_argument("real_out size mismatch");
    }

    size_t poly_modulus_degree = static_cast<size_t>(in_wid * in_wid * raw_in_batch);
    std::cout<<"the poly degree is "<<poly_modulus_degree<<std::endl;
    double scale = pow(2.0, 40);

    double out_scale = scale;
    EncryptionParameters parms(scheme_type::ckks);
    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_coeff_modulus(phantom::arith::CoeffModulus::Create(
        poly_modulus_degree, {60, 40, 40, 40, 40, 60}));

    PhantomContext context(parms);
    PhantomCKKSEncoder encoder(context);
    PhantomSecretKey secret_key(context);
    PhantomPublicKey public_key = secret_key.gen_publickey(context);
    PhantomRelinKey relin_keys = secret_key.gen_relinkey(context);
    PhantomGaloisKey galois_keys;

    CKKSEvaluator ckks_evaluator(&context, &public_key, &secret_key,
                                 &encoder, &relin_keys, &galois_keys, scale);

    int logN = static_cast<int>(round(log2(static_cast<double>(poly_modulus_degree))));
    vector<uint32_t> elts;
    for (int i = 0; i < logN; i++)
    {
        elts.push_back((1u << (i + 1)) + 1);
    }
    ckks_evaluator.decryptor.create_galois_keys_from_elts(elts, *(ckks_evaluator.galois_keys));

    int max_batch = static_cast<int>(poly_modulus_degree) / (in_wid * in_wid);
    int real_batch = raw_in_batch;
    int norm = max_batch / real_batch;

    vector<double> input_coeffs = prep_input(raw_input, raw_in_wid, in_wid,
                                             static_cast<int>(poly_modulus_degree), norm);

    PhantomPlaintext pt_input;
    encoder.encode_coeffs(context, input_coeffs, scale, pt_input);

    PhantomCiphertext ct_input;
    ckks_evaluator.encryptor.encrypt(pt_input, ct_input);

    vector<PhantomPlaintext> pl_ker = prep_ker(context, encoder, ker_in, bn_a,
                                               in_wid, ker_wid, real_batch,
                                               raw_out_batch, norm, 0, false, scale);

    vector<PhantomPlaintext> plain_idx = gen_idxNlogs(context, encoder);
    PhantomCiphertext ct_conv = conv_then_pack(context, ckks_evaluator, encoder, ct_input,
                                               pl_ker, plain_idx, max_batch,
                                               norm, out_scale, logN);
    std::cout << "the scale of ct_conv is " << log2(ct_conv.scale()) << std::endl;
    //ckks_evaluator.evaluator.rescale_to_next_inplace(ct_conv);
    //ct_conv.scale() = std::pow(2, 80);
    vector<double> b_coeffs(poly_modulus_degree, 0.0);
    for (int i = 0; i < real_batch; i++)
    {
        for (int j = 0; j < in_wid * in_wid; j++)
        {
            b_coeffs[norm * i + j * max_batch] = bn_b[i];
        }
    }
    PhantomPlaintext pt_bn_b;
    encoder.encode_coeffs(context, b_coeffs, ct_conv.scale(), pt_bn_b, ct_conv.chain_index());
    ckks_evaluator.evaluator.add_plain_inplace(ct_conv, pt_bn_b);

    PhantomPlaintext pt_out;
    ckks_evaluator.decryptor.decrypt(ct_conv, pt_out);

    vector<double> decoded;
    encoder.decode_coeffs(context, pt_out, decoded);

    vector<double> test_out = post_process(decoded, raw_in_wid, in_wid);
    vector<double> expected = cpu_conv_ref(raw_input, ker_in, bn_a, bn_b,
                                           raw_in_wid, in_wid, ker_wid,
                                           real_batch, raw_out_batch);

    double max_error = 0.0;
    for (size_t i = 0; i < expected.size(); i++)
    {
        max_error = max(max_error, fabs(test_out[i] - expected[i]));
    }
    double max_error_real = 0.0;
    for (size_t i = 0; i < real_out.size(); i++)
    {
        max_error_real = max(max_error_real, fabs(test_out[i] - real_out[i]));
    }

    size_t split = min<size_t>(1024, test_out.size());
    double max_error_first = 0.0;
    double max_error_rest = 0.0;
    for (size_t i = 0; i < test_out.size(); i++)
    {
        double err = fabs(test_out[i] - expected[i]);
        if (i < split)
        {
            max_error_first = max(max_error_first, err);
        }
        else
        {
            max_error_rest = max(max_error_rest, err);
        }
    }

    cout << "Max error: " << max_error << endl;
    cout << "Max error vs real_out: " << max_error_real << endl;
    cout << "Max error (cpu, first " << split << "): " << max_error_first << endl;
    cout << "Max error (cpu, rest): " << max_error_rest << endl;
    cout << "First 8 coeffs (decoded): ";
    for (size_t i = 0; i < 8; i++)
    {
        cout << test_out[i] << (i + 1 == 8 ? "\n" : ", ");
    }
    cout << "First 8 coeffs (expected): ";
    for (size_t i = 0; i < 8; i++)
    {
        cout << expected[i] << (i + 1 == 8 ? "\n" : ", ");
    }
    cout << "First 8 coeffs (real_out): ";
    for (size_t i = 0; i < 8; i++)
    {
        cout << real_out[i] << (i + 1 == 8 ? "\n" : ", ");
    }

    size_t report_n = min<size_t>(2000, test_out.size());
    cout << "Index, computed, real_out, cpu_expected" << endl;
    for (size_t i = 0; i < report_n; i++)
    {
        cout << i << ", " << test_out[i] << ", " << real_out[i] << ", " << expected[i] << endl;
    }

    return 0;
}
