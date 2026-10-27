#include <algorithm>
#include <cmath>
#include <chrono>
#include <complex>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "conv_eval.h"
#include "galois.cuh"
#include "phantom.h"

using namespace std;
using namespace phantom;

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

static vector<double> prep_input_coeff(const vector<double> &input,
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

static vector<complex<double>> reshape_input_bl(const vector<double> &input,
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

static vector<double> post_trim_bl(const vector<complex<double>> &in_vals,
                                   int raw_in_wid,
                                   int in_wid)
{
    int batch = static_cast<int>(in_vals.size()) / (in_wid * in_wid);
    vector<double> out(raw_in_wid * raw_in_wid * batch, 0.0);

    for (int b = 0; b < batch; b++)
    {
        for (int i = 0; i < raw_in_wid; i++)
        {
            for (int j = 0; j < raw_in_wid; j++)
            {
                out[b * raw_in_wid * raw_in_wid + i * raw_in_wid + j] =
                    in_vals[b * in_wid * in_wid + i * in_wid + j].real();
            }
        }
    }

    return out;
}

static vector<double> post_process_bl(const vector<double> &in_vals,
                                      int raw_in_wid)
{
    int batch = static_cast<int>(in_vals.size()) / (raw_in_wid * raw_in_wid);
    vector<double> out(raw_in_wid * raw_in_wid * batch, 0.0);

    for (int i = 0; i < raw_in_wid; i++)
    {
        for (int j = 0; j < raw_in_wid; j++)
        {
            for (int b = 0; b < batch; b++)
            {
                out[i * raw_in_wid * batch + j * batch + b] =
                    in_vals[b * raw_in_wid * raw_in_wid + i * raw_in_wid + j];
            }
        }
    }

    return out;
}

static vector<double> post_process_coeff(const vector<double> &in_cfs,
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

static void print_first_n(const vector<double> &values, size_t n)
{
    size_t limit = std::min(n, values.size());
    for (size_t i = 0; i < limit; i++)
    {
        cout << values[i] << (i + 1 == limit ? "\n" : ", ");
    }
}

static double max_abs_diff(const vector<double> &a, const vector<double> &b)
{
    if (a.size() != b.size())
    {
        throw invalid_argument("size mismatch in max_abs_diff");
    }
    double max_err = 0.0;
    for (size_t i = 0; i < a.size(); i++)
    {
        max_err = max(max_err, fabs(a[i] - b[i]));
    }
    return max_err;
}

int main(int argc, char **argv)
{
    int ker_wid = 3;
    int real_batch = 16;
    int test_iter = 0;
    if (argc >= 2)
    {
        ker_wid = stoi(argv[1]);
    }
    if (argc >= 3)
    {
        real_batch = stoi(argv[2]);
    }
    if (argc >= 4)
    {
        test_iter = stoi(argv[3]);
    }

    const string data_dir = "data/test_conv_data";
    string prefix = data_dir + "/test_conv" + to_string(ker_wid) +
                    "_batch_" + to_string(real_batch) + "_";
    vector<double> raw_input = read_csv_floats(prefix + "in_" + to_string(test_iter) + ".csv");
    vector<double> ker_in = read_csv_floats(prefix + "ker_" + to_string(test_iter) + ".csv");
    vector<double> bn_a = read_csv_floats(prefix + "bna_" + to_string(test_iter) + ".csv");
    vector<double> bn_b = read_csv_floats(prefix + "bnb_" + to_string(test_iter) + ".csv");
    vector<double> real_out = read_csv_floats(prefix + "out_" + to_string(test_iter) + ".csv");

    int raw_in_wid = static_cast<int>(round(sqrt(static_cast<double>(raw_input.size() / real_batch))));
    if (raw_in_wid * raw_in_wid * real_batch != static_cast<int>(raw_input.size()))
    {
        throw invalid_argument("raw_input size mismatch");
    }
    int pad = ker_wid / 2;
    int in_wid = raw_in_wid + pad;
    int in_size = in_wid * in_wid;
    int ker_size = ker_wid * ker_wid;
    int raw_out_batch = static_cast<int>(ker_in.size()) / (ker_size * real_batch);
    if (ker_size * real_batch * raw_out_batch != static_cast<int>(ker_in.size()))
    {
        throw invalid_argument("ker_in size mismatch");
    }
    if (static_cast<int>(bn_a.size()) != raw_out_batch || static_cast<int>(bn_b.size()) != raw_out_batch)
    {
        throw invalid_argument("bn size mismatch");
    }
    if (static_cast<int>(real_out.size()) != raw_in_wid * raw_in_wid * real_batch)
    {
        throw invalid_argument("real_out size mismatch");
    }

    size_t poly_modulus_degree = static_cast<size_t>(in_size * real_batch);
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
    std::set<uint32_t> elt_set;
    for (int i = 0; i < logN; i++)
    {
        elt_set.insert((1u << (i + 1)) + 1);
    }

    int norm_bl = 1;
    size_t slots = poly_modulus_degree / 2;
    int max_batch_bl = static_cast<int>(slots) / in_size;
    int real_ob_bl = real_batch / 2;
    int rot_iters_bl = (norm_bl * real_ob_bl == max_batch_bl) ? real_ob_bl : max_batch_bl;

    vector<int> steps;
    for (int i = -pad; i <= pad; i++)
    {
        for (int j = -pad; j <= pad; j++)
        {
            int rot = i * in_wid + j;
            if (rot != 0)
            {
                steps.push_back(rot);
            }
        }
    }
    for (int i = 1; i < rot_iters_bl; i++)
    {
        int rot = i * in_size;
        if (rot != 0)
        {
            steps.push_back(rot);
        }
    }
    auto step_elts = get_elts_from_steps(steps, poly_modulus_degree);
    for (auto elt : step_elts)
    {
        elt_set.insert(elt);
    }

    vector<uint32_t> elts(elt_set.begin(), elt_set.end());
    ckks_evaluator.decryptor.create_galois_keys_from_elts(elts, *(ckks_evaluator.galois_keys));

    int max_batch = static_cast<int>(poly_modulus_degree) / (in_size);
    int norm = max_batch / real_batch;

    vector<double> pad_input1(in_wid * in_wid * real_batch / 2, 0.0);
    vector<double> pad_input2(in_wid * in_wid * real_batch / 2, 0.0);
    for (int i = 0; i < raw_in_wid; i++)
    {
        for (int j = 0; j < raw_in_wid; j++)
        {
            for (int b = 0; b < real_batch / 2; b++)
            {
                pad_input1[b + j * real_batch / 2 + i * real_batch / 2 * in_wid] =
                    raw_input[b + j * real_batch + i * real_batch * raw_in_wid];
                pad_input2[b + j * real_batch / 2 + i * real_batch / 2 * in_wid] =
                    raw_input[b + real_batch / 2 + j * real_batch + i * real_batch * raw_in_wid];
            }
        }
    }

    vector<vector<double>> bn_a_sep(2, vector<double>(real_batch / 2, 0.0));
    vector<vector<double>> bn_b_sep(2, vector<double>(real_batch / 2, 0.0));
    for (int out = 0; out < 2; out++)
    {
        for (int i = 0; i < real_batch / 2; i++)
        {
            bn_a_sep[out][i] = bn_a[i + out * real_batch / 2];
            bn_b_sep[out][i] = bn_b[i + out * real_batch / 2];
        }
    }
    vector<double> zeros(real_batch / 2, 0.0);

    vector<vector<vector<double>>> ker_in_sep(2, vector<vector<double>>(2));
    for (int out = 0; out < 2; out++)
    {
        for (int in = 0; in < 2; in++)
        {
            ker_in_sep[out][in].assign(ker_in.size() / 4, 0.0);
            for (int k = 0; k < ker_size; k++)
            {
                for (int i = 0; i < real_batch / 2; i++)
                {
                    for (int j = 0; j < real_batch / 2; j++)
                    {
                        size_t dst = static_cast<size_t>(k * real_batch * real_batch / 4 +
                                                         i * real_batch / 2 + j);
                        size_t src = static_cast<size_t>(k * real_batch * real_batch +
                                                         (i + in * real_batch / 2) * real_batch +
                                                         out * real_batch / 2 + j);
                        ker_in_sep[out][in][dst] = ker_in[src];
                    }
                }
            }
        }
    }

    vector<complex<double>> input1_rs = reshape_input_bl(pad_input1, in_wid);
    vector<complex<double>> input2_rs = reshape_input_bl(pad_input2, in_wid);

    PhantomPlaintext pt_input1;
    PhantomPlaintext pt_input2;
    encoder.encode(context, input1_rs, scale, pt_input1);
    encoder.encode(context, input2_rs, scale, pt_input2);

    PhantomCiphertext ct_input1;
    PhantomCiphertext ct_input2;
    ckks_evaluator.encryptor.encrypt(pt_input1, ct_input1);
    ckks_evaluator.encryptor.encrypt(pt_input2, ct_input2);

    auto bl_start = chrono::steady_clock::now();
    vector<PhantomCiphertext> ct_res_bl(2);
    for (int pos = 0; pos < 2; pos++)
    {
        PhantomCiphertext ct_left = evalConv_BN_BL_test(context, ckks_evaluator, encoder, ct_input1,
                                                        ker_in_sep[pos][0], bn_a_sep[pos], bn_b_sep[pos],
                                                        in_wid, ker_wid, real_batch / 2, real_batch / 2,
                                                        0, 1, pad, false, false);
        PhantomCiphertext ct_right = evalConv_BN_BL_test(context, ckks_evaluator, encoder, ct_input2,
                                                         ker_in_sep[pos][1], bn_a_sep[pos], zeros,
                                                         in_wid, ker_wid, real_batch / 2, real_batch / 2,
                                                         0, 1, pad, false, false);
        ckks_evaluator.evaluator.add(ct_left, ct_right, ct_res_bl[pos]);
    }

    auto bl_end = chrono::steady_clock::now();
    double bl_seconds = chrono::duration<double>(bl_end - bl_start).count();

    vector<double> bl_out;
    for (int pos = 0; pos < 2; pos++)
    {
        PhantomPlaintext pt_out;
        ckks_evaluator.decryptor.decrypt(ct_res_bl[pos], pt_out);
        vector<complex<double>> vals_tmp;
        encoder.decode(context, pt_out, vals_tmp);
        vector<double> tmp = post_trim_bl(vals_tmp, raw_in_wid, in_wid);
        if (pos == 0)
        {
            bl_out = tmp;
        }
        else
        {
            bl_out.insert(bl_out.end(), tmp.begin(), tmp.end());
        }
    }
    bl_out = post_process_bl(bl_out, raw_in_wid);

    vector<double> input_coeffs = prep_input_coeff(raw_input, raw_in_wid, in_wid,
                                                   static_cast<int>(poly_modulus_degree), norm);
    PhantomPlaintext pt_input_coeff;
    encoder.encode_coeffs(context, input_coeffs, scale, pt_input_coeff);
    PhantomCiphertext ct_input_coeff;
    ckks_evaluator.encryptor.encrypt(pt_input_coeff, ct_input_coeff);

    auto coeff_start = chrono::steady_clock::now();
    PhantomCiphertext ct_coeff = evalConv_BN(context, ckks_evaluator, encoder, ct_input_coeff,
                                             ker_in, bn_a, bn_b, in_wid, ker_wid,
                                             real_batch, raw_out_batch, norm, out_scale, false);
    auto coeff_end = chrono::steady_clock::now();
    double coeff_seconds = chrono::duration<double>(coeff_end - coeff_start).count();
    PhantomPlaintext pt_coeff;
    ckks_evaluator.decryptor.decrypt(ct_coeff, pt_coeff);
    vector<double> coeff_vals;
    encoder.decode_coeffs(context, pt_coeff, coeff_vals);
    vector<double> coeff_out = post_process_coeff(coeff_vals, raw_in_wid, in_wid);

    cout << "Max error (BL vs real_out): " << max_abs_diff(bl_out, real_out) << endl;
    cout << "Max error (coeff vs real_out): " << max_abs_diff(coeff_out, real_out) << endl;
    cout << "Max error (BL vs coeff): " << max_abs_diff(bl_out, coeff_out) << endl;
    cout << "Baseline (BL) eval time: " << bl_seconds << " s" << endl;
    cout << "Coeff eval time: " << coeff_seconds << " s" << endl;

    cout << "First 8 coeffs (BL): ";
    print_first_n(bl_out, 8);
    cout << "First 8 coeffs (coeff): ";
    print_first_n(coeff_out, 8);
    cout << "First 8 coeffs (real_out): ";
    print_first_n(real_out, 8);

    return 0;
}
