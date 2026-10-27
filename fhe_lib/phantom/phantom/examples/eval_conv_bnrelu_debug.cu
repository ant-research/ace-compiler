#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "phantom.h"
#include "conv_eval.h"
using namespace std;
using namespace phantom;

static vector<double> read_csv_floats(const string &path) {
    ifstream in(path);
    if (!in.is_open()) {
        throw invalid_argument("failed to open file: " + path);
    }

    vector<double> values;
    string line;
    while (getline(in, line)) {
        if (line.empty()) {
            continue;
        }
        string token;
        stringstream ss(line);
        while (getline(ss, token, ',')) {
            if (token.empty()) {
                continue;
            }
            values.push_back(stod(token));
        }
    }
    return values;
}

static vector<double> prep_input(const vector<double> &input,
                                 int raw_in_wid,
                                 int in_wid,
                                 int N,
                                 int norm) {
    vector<double> out(N, 0.0);
    int batch = N / (in_wid * in_wid);
    int k = 0;

    for (int i = 0; i < in_wid; i++) {
        for (int j = 0; j < in_wid; j++) {
            for (int b = 0; b < batch / norm; b++) {
                if (i < raw_in_wid && j < raw_in_wid) {
                    out[i * in_wid * batch + j * batch + b * norm] = input[k];
                    k++;
                }
            }
        }
    }

    return out;
}

static vector<double> post_process(const vector<double> &in_cfs,
                                   int raw_in_wid,
                                   int in_wid) {
    int batch = static_cast<int>(in_cfs.size()) / (in_wid * in_wid);
    vector<double> out(raw_in_wid * raw_in_wid * batch, 0.0);

    for (int i = 0; i < raw_in_wid; i++) {
        for (int j = 0; j < raw_in_wid; j++) {
            for (int b = 0; b < batch; b++) {
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
                                          bool trans) {
    vector<vector<double>> ker_out(out_batch, vector<double>(k_sz * (static_cast<int>(ker_in.size()) / (k_sz * out_batch)), 0.0));
    int in_batch = static_cast<int>(ker_in.size()) / (k_sz * out_batch);

    for (int i = 0; i < out_batch; i++) {
        for (int j = 0; j < in_batch; j++) {
            for (int k = 0; k < k_sz; k++) {
                if (trans) {
                    ker_out[i][j * k_sz + (k_sz - 1 - k)] =
                        ker_in[j + i * in_batch + k * out_batch * in_batch];
                } else {
                    ker_out[i][j * k_sz + k] =
                        ker_in[i + j * out_batch + k * out_batch * in_batch];
                }
            }
        }
    }

    return ker_out;
}

static vector<double> cpu_conv_ref(const vector<double> &raw_input,
                                   const vector<double> &ker_in,
                                   const vector<double> &bn_a,
                                   const vector<double> &bn_b,
                                   int raw_in_wid,
                                   int in_wid,
                                   int ker_wid,
                                   int real_ib,
                                   int real_ob) {
    int k_sz = ker_wid * ker_wid;
    vector<vector<double>> ker_rs = reshape_ker(ker_in, k_sz, real_ob, false);

    vector<double> out(raw_in_wid * raw_in_wid * real_ob, 0.0);
    for (int ob = 0; ob < real_ob; ob++) {
        for (int idx = 0; idx < k_sz * real_ib; idx++) {
            ker_rs[ob][idx] *= bn_a[ob];
        }
    }

    for (int ob = 0; ob < real_ob; ob++) {
        for (int i = 0; i < raw_in_wid; i++) {
            for (int j = 0; j < raw_in_wid; j++) {
                double acc = 0.0;
                for (int ib = 0; ib < real_ib; ib++) {
                    for (int ki = 0; ki < ker_wid; ki++) {
                        for (int kj = 0; kj < ker_wid; kj++) {
                            int ii = i + ki - ker_wid / 2;
                            int jj = j + kj - ker_wid / 2;
                            if (ii < 0 || jj < 0 || ii >= raw_in_wid || jj >= raw_in_wid) {
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

int main() {
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
    vector<double> real_out = read_csv_floats(prefix + "reluout_" + to_string(test_iter) + ".csv");

    int raw_in_wid = static_cast<int>(round(sqrt(static_cast<double>(raw_input.size() / raw_in_batch))));
    if (raw_in_wid * raw_in_wid * raw_in_batch != static_cast<int>(raw_input.size())) {
        throw invalid_argument("raw_input size mismatch");
    }
    int in_wid = raw_in_wid + ker_wid / 2;
    int ker_size = ker_wid * ker_wid;
    int raw_out_batch = static_cast<int>(ker_in.size()) / (ker_size * raw_in_batch);
    if (ker_size * raw_in_batch * raw_out_batch != static_cast<int>(ker_in.size())) {
        throw invalid_argument("ker_in size mismatch");
    }
    if (static_cast<int>(bn_a.size()) != raw_out_batch) {
        throw invalid_argument("bn_a size mismatch");
    }
    if (static_cast<int>(bn_b.size()) != raw_out_batch) {
        throw invalid_argument("bn_b size mismatch");
    }
    if (static_cast<int>(real_out.size()) != raw_in_wid * raw_in_wid * raw_in_batch) {
        throw invalid_argument("real_out size mismatch");
    }

    size_t poly_modulus_degree = 65536;
    int logN = 16;

    int logp = 47;
    int logq = 51;
    int log_special_prime = 51;
    int secret_key_hamming_weight = 192;
    int remaining_level = 3;
    int boot_level = 3 + 6 + 2 + 1 + 7+1;
    int total_level = remaining_level + boot_level;

    vector<int> coeff_bit_vec;
    coeff_bit_vec.push_back(logq);
    for (int i = 0; i < remaining_level; i++) {
        coeff_bit_vec.push_back(logp);
    }
    for (int i = 0; i < boot_level; i++) {
        coeff_bit_vec.push_back(logq);
    }
    coeff_bit_vec.push_back(log_special_prime);

    double scale = pow(2.0, logp);

    EncryptionParameters parms(scheme_type::ckks);
    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_coeff_modulus(phantom::arith::CoeffModulus::Create(poly_modulus_degree, coeff_bit_vec));
    parms.set_secret_key_hamming_weight(secret_key_hamming_weight);

    PhantomContext context(parms);
    PhantomCKKSEncoder encoder(context);
    PhantomSecretKey secret_key(context);
    PhantomPublicKey public_key = secret_key.gen_publickey(context);
    PhantomRelinKey relin_keys = secret_key.gen_relinkey(context);
    PhantomGaloisKey galois_keys;

    CKKSEvaluator ckks_evaluator(&context, &public_key, &secret_key,
                                 &encoder, &relin_keys, &galois_keys, scale);

    vector<uint32_t> elts;
    for (int i = 0; i < logN; i++) {
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
    ckks_evaluator.evaluator.mod_switch_to_inplace(ct_input,20);
    cout << "ct_input scale(log2): " << log2(ct_input.scale())
         << ", chain_index: " << ct_input.chain_index() << endl;
    {
        PhantomPlaintext pt_input_debug;
        ckks_evaluator.decryptor.decrypt(ct_input, pt_input_debug);
        vector<double> input_decoded;
        encoder.decode_coeffs(context, pt_input_debug, input_decoded);
        cout << "ct_input coeffs (first 8): ";
        for (size_t i = 0; i < 8; i++) {
            cout << input_decoded[i] << (i + 1 == 8 ? "\n" : ", ");
        }
    }

    PhantomCiphertext ct_out = evalConv_BNRelu_new(
        context,
        ckks_evaluator,
        encoder,
        ct_input,
        ker_in,
        bn_a,
        bn_b,
        0.0,
        4.0,
        in_wid,
        in_wid,
        ker_wid,
        raw_in_batch,
        raw_out_batch,
        norm,
        0,
        0,
        2,
        0,
        "Conv",
        false,
        false);

    PhantomPlaintext pt_out;
    ckks_evaluator.decryptor.decrypt(ct_out, pt_out);
    cout << "ct_out scale(log2): " << log2(ct_out.scale())
         << ", chain_index: " << ct_out.chain_index() << endl;

    vector<double> decoded;
    encoder.decode_coeffs(context, pt_out, decoded);
    vector<double> test_out = post_process(decoded, raw_in_wid, in_wid);
    vector<double> expected = real_out;
    vector<double> cpu_expected = cpu_conv_ref(raw_input, ker_in, bn_a, bn_b,
                                               raw_in_wid, in_wid, ker_wid,
                                               raw_in_batch, raw_out_batch);
    auto expected_mm = minmax_element(expected.begin(), expected.end());
    auto output_mm = minmax_element(test_out.begin(), test_out.end());
    cout << "expected min/max: " << *expected_mm.first
         << ", " << *expected_mm.second << endl;
    cout << "output min/max: " << *output_mm.first
         << ", " << *output_mm.second << endl;

    double max_error = 0.0;
    for (size_t i = 0; i < expected.size(); i++) {
        max_error = max(max_error, fabs(test_out[i] - expected[i]));
    }

    cout << "Max error: " << max_error << endl;
    cout << "Decoded coefficients (first 8): ";
    for (size_t i = 0; i < 8; i++) {
        cout << test_out[i] << (i + 1 == 8 ? "\n" : ", ");
    }
    cout << "Expected (first 8): ";
    for (size_t i = 0; i < 8; i++) {
        cout << expected[i] << (i + 1 == 8 ? "\n" : ", ");
    }

    size_t report_n = min<size_t>(2000, test_out.size());
    cout << "Index, computed, real_out, cpu_expected" << endl;
    for (size_t i = 0; i < report_n; i++) {
        cout << i << ", " << test_out[i] << ", " << expected[i] << ", " << cpu_expected[i] << endl;
    }

    return 0;
}
