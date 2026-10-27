#include <cmath>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "phantom.h"
#include "conv_eval.h"
#include "galois.cuh"

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

static void write_csv_floats(const string &path, const vector<double> &values) {
    ofstream out(path);
    if (!out.is_open()) {
        throw invalid_argument("failed to open file: " + path);
    }
    for (size_t i = 0; i < values.size(); i++) {
        out << values[i];
        if (i + 1 < values.size()) {
            out << ",";
        }
    }
    out << "\n";
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

static vector<double> prt_mat_one_norm(const vector<double> &vec,
                                       int batch,
                                       int norm,
                                       int sj,
                                       int sk) {
    int mat_size = static_cast<int>(vec.size()) / batch;
    vector<double> out(batch / norm, 0.0);
    int j = 1;
    int k = 1;
    for (int i = 0; i < static_cast<int>(vec.size()); i += batch) {
        if (j == sj && k == sk) {
            for (int idx = 0; idx < batch / norm; idx++) {
                out[idx] = vec[i + norm * idx];
            }
            return out;
        }
        k++;
        if (k * k > mat_size) {
            k = 1;
            j++;
        }
    }
    return out;
}

int main(int argc, char **argv) {
    if (argc < 6) {
        cout << "Usage: resnet_crop_sparse <ker_wid> <depth> <st> <end> <cf100(0|1)>\n";
        return 1;
    }

    int ker_wid = stoi(argv[1]);
    int depth = stoi(argv[2]);
    int st = stoi(argv[3]);
    int end = stoi(argv[4]);
    bool cf100 = stoi(argv[5]) != 0;

    string ker_name = "ker" + to_string(ker_wid);
    string weight_dir = "data/Resnet_weights/weights_crop_" + ker_name + "_d" + to_string(depth) + "_wid1/";
    string out_dir = "data/Resnet_enc_results/results_crop_" + ker_name + "_d" + to_string(depth) + "_wid1/";
    int fc_out = 10;
    double init_pow = 6.0;
    double mid_pow = 6.0;
    double final_pow = 6.0;
    if (cf100) {
        weight_dir = "data/Resnet_weights/weights_cf100_crop_" + ker_name + "_d" + to_string(depth) + "_wid1/";
        out_dir = "data/Resnet_enc_results/results_cf100_crop_" + ker_name + "_d" + to_string(depth) + "_wid1/";
        fc_out = 100;
        if (ker_wid == 3) {
            final_pow = 7.0;
        } else if (ker_wid == 5) {
            final_pow = 6.0;
        } else {
            final_pow = 5.0;
        }
        init_pow = 5.0;
        mid_pow = 5.0;
    }

    int num_blcs[3] = {0, 0, 0};
    if (depth == 20) {
        num_blcs[0] = 7;
        num_blcs[1] = 5;
        num_blcs[2] = 5;
    } else if (depth == 14) {
        num_blcs[0] = 5;
        num_blcs[1] = 3;
        num_blcs[2] = 3;
    } else if (depth == 8) {
        num_blcs[0] = 3;
        num_blcs[1] = 1;
        num_blcs[2] = 1;
    } else {
        throw invalid_argument("wrong depth (not in 8, 14, 20)");
    }

    vector<int> real_batch = {16, 32, 64};
    vector<int> norm = {4, 8, 16};
    vector<int> step = {1, 1, 1};

    int logN = 16;
    double alpha = 0.0;
    vector<int> in_wids = {32, 16, 8};
    vector<int> raw_in_wids = {32 - ker_wid / 2, 16 - ker_wid / 2, 8 - ker_wid / 2};
    bool fast_pack = true;
    int ker_size = ker_wid * ker_wid;
    vector<int> max_batch(in_wids.size(), 0);
    for (size_t i = 0; i < max_batch.size(); i++) {
        max_batch[i] = (1 << logN) / (in_wids[i] * in_wids[i]);
    }

    size_t poly_modulus_degree = static_cast<size_t>(1 << logN);
    int logp = 47;
    int logq = 51;
    int log_special_prime = 51;
    int secret_key_hamming_weight = 192;
    int remaining_level = 3;
    int boot_level = 3 + 6 + 2 + 1 + 7 + 1 + 4;

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

    vector<int> galois_steps;
    galois_steps.push_back(0);
    for (int i = 0; i < logN - 1; i++) {
        galois_steps.push_back(1 << i);
        galois_steps.push_back(-(1 << i));
    }
    auto step_elts = get_elts_from_steps(galois_steps, poly_modulus_degree);
    std::set<uint32_t> galois_elt_set(step_elts.begin(), step_elts.end());
    for (int i = 1; i <= logN; i++) {
        galois_elt_set.insert((1u << i) + 1);
    }
    vector<uint32_t> galois_elts(galois_elt_set.begin(), galois_elt_set.end());
    ckks_evaluator.decryptor.create_galois_keys_from_elts(galois_elts, *(ckks_evaluator.galois_keys));
    for (int iter = st; iter < end; iter++) {
        cout << "Running " << iter << "-th iter... ker size: " << ker_wid << endl;
        string img_path = "data/Resnet_plain_data/crop_ker" + to_string(ker_wid) + "_d" +
                          to_string(depth) + "_wid1/test_image_" + to_string(iter) + ".csv";
        if (cf100) {
            img_path = "data/Resnet_plain_data/cf100_crop_ker" + to_string(ker_wid) + "_d" +
                       to_string(depth) + "_wid1/test_image_" + to_string(iter) + ".csv";
        }
        vector<double> image = read_csv_floats(img_path);

        vector<double> input(poly_modulus_degree, 0.0);
        int k = 0;
        for (int i = 0; i < in_wids[0]; i++) {
            for (int j = 0; j < in_wids[0]; j++) {
                for (int b = 0; b < 3; b++) {
                    if (i < raw_in_wids[0] && j < raw_in_wids[0]) {
                        input[i * in_wids[0] * max_batch[0] + j * max_batch[0] + b * norm[0]] = image[k];
                    }
                    k++;
                }
            }
        }

        PhantomPlaintext pt_input;
        encoder.encode_coeffs(context, input, scale, pt_input);

        PhantomCiphertext ct_input;
        ckks_evaluator.encryptor.encrypt(pt_input, ct_input);
        ckks_evaluator.evaluator.mod_switch_to_inplace(ct_input, 24);

        double relu_pow = init_pow;
        PhantomCiphertext ct_layer = ct_input;

        for (int i = 1; i <= num_blcs[0]; i++) {
            vector<double> bn_a = read_csv_floats(weight_dir + "w" + to_string(i - 1) + "-a.csv");
            vector<double> bn_b = read_csv_floats(weight_dir + "w" + to_string(i - 1) + "-b.csv");
            int ker_in_batch = (i == 1) ? 3 : real_batch[0];
            vector<double> ker_in = read_csv_floats(weight_dir + "w" + to_string(i - 1) + "-conv.csv");
            ct_layer = evalConv_BNRelu_new(context, ckks_evaluator, encoder, ct_layer,
                                           ker_in, bn_a, bn_b, alpha, relu_pow,
                                           in_wids[0], raw_in_wids[0], ker_wid,
                                           ker_in_batch, real_batch[0], norm[0],
                                           0, step[0], 2, 2, "Conv_sparse", fast_pack, false);
            relu_pow = mid_pow;
            cout << "Block1, Layer " << i << " done!" << endl;
        }
        cout << "Block1 done." << endl;

        vector<double> ker_in12 = read_csv_floats(weight_dir + "w" + to_string(num_blcs[0]) + "-conv.csv");
        vector<double> bn_a12 = read_csv_floats(weight_dir + "w" + to_string(num_blcs[0]) + "-a.csv");
        vector<double> bn_b12 = read_csv_floats(weight_dir + "w" + to_string(num_blcs[0]) + "-b.csv");
            ct_layer = evalConv_BNRelu_new(context, ckks_evaluator, encoder, ct_layer,
                                       ker_in12, bn_a12, bn_b12, alpha, relu_pow,
                                       in_wids[0], raw_in_wids[1], ker_wid,
                                       real_batch[0], real_batch[1], norm[1],
                                       0, step[1], 2, 1, "StrConv_sparse", fast_pack, false);
        cout << "Block1 to 2 done!" << endl;

        for (int i = 1; i <= num_blcs[1]; i++) {
            int idx = num_blcs[0] + i;
            vector<double> bn_a2 = read_csv_floats(weight_dir + "w" + to_string(idx) + "-a.csv");
            vector<double> bn_b2 = read_csv_floats(weight_dir + "w" + to_string(idx) + "-b.csv");
            vector<double> ker_in2 = read_csv_floats(weight_dir + "w" + to_string(idx) + "-conv.csv");
                ct_layer = evalConv_BNRelu_new(context, ckks_evaluator, encoder, ct_layer,
                                           ker_in2, bn_a2, bn_b2, alpha, relu_pow,
                                           in_wids[1], raw_in_wids[1], ker_wid,
                                           real_batch[1], real_batch[1], norm[1],
                                           0, step[1], 2, 3, "Conv_sparse", fast_pack, false);
            cout << "Block2, Layer " << i << " done!" << endl;
        }
        cout << "Block2 done." << endl;

        int idx23 = num_blcs[0] + num_blcs[1] + 1;
        vector<double> ker_in23 = read_csv_floats(weight_dir + "w" + to_string(idx23) + "-conv.csv");
        vector<double> bn_a23 = read_csv_floats(weight_dir + "w" + to_string(idx23) + "-a.csv");
        vector<double> bn_b23 = read_csv_floats(weight_dir + "w" + to_string(idx23) + "-b.csv");
            ct_layer = evalConv_BNRelu_new(context, ckks_evaluator, encoder, ct_layer,
                                       ker_in23, bn_a23, bn_b23, alpha, relu_pow,
                                       in_wids[1], raw_in_wids[2], ker_wid,
                                       real_batch[1], real_batch[2], norm[2],
                                       0, step[2], 2, 2, "StrConv_sparse", fast_pack, false);
        cout << "Block2 to 3 done!" << endl;

        for (int i = 1; i <= num_blcs[2]; i++) {
            int idx = num_blcs[0] + num_blcs[1] + i + 1;
            vector<double> bn_a3 = read_csv_floats(weight_dir + "w" + to_string(idx) + "-a.csv");
            vector<double> bn_b3 = read_csv_floats(weight_dir + "w" + to_string(idx) + "-b.csv");
            vector<double> ker_in3 = read_csv_floats(weight_dir + "w" + to_string(idx) + "-conv.csv");
            if (i == num_blcs[2]) {
                relu_pow = final_pow;
            }
                ct_layer = evalConv_BNRelu_new(context, ckks_evaluator, encoder, ct_layer,
                                           ker_in3, bn_a3, bn_b3, alpha, relu_pow,
                                           in_wids[2], raw_in_wids[2], ker_wid,
                                           real_batch[2], real_batch[2], norm[2],
                                           0, step[2], 2, 4, "Conv_sparse", fast_pack, false);
            cout << "Block3, Layer " << i << " done!" << endl;
        }
        cout << "Block3 done." << endl;

        int ker_inf_wid = raw_in_wids[2];
        if (ker_inf_wid % 2 == 0) {
            ker_inf_wid++;
        }
        vector<double> ker_inf = read_csv_floats(weight_dir + "final-fckernel.csv");

        vector<double> res_out;
        if (cf100) {
            vector<double> ker_inf_1(ker_inf_wid * ker_inf_wid * real_batch[2] * fc_out / 2, 0.0);
            vector<double> ker_inf_2(ker_inf_wid * ker_inf_wid * real_batch[2] * fc_out / 2, 0.0);
            for (int i = 0; i < fc_out / 2; i++) {
                for (int j = 0; j < real_batch[2]; j++) {
                    for (int b = 0; b < ker_inf_wid * ker_inf_wid; b++) {
                        ker_inf_1[j * fc_out / 2 + i + b * real_batch[2] * fc_out / 2] =
                            ker_inf[j * fc_out + i];
                        ker_inf_2[j * fc_out / 2 + i + b * real_batch[2] * fc_out / 2] =
                            ker_inf[j * fc_out + i + fc_out / 2];
                    }
                }
            }
            vector<double> bn_af(fc_out / 2, 1.0 / static_cast<double>(raw_in_wids[2] * raw_in_wids[2]));
            vector<double> bn_bf = read_csv_floats(weight_dir + "final-fcbias.csv");
            vector<double> bn_bf_1(fc_out / 2, 0.0);
            vector<double> bn_bf_2(fc_out / 2, 0.0);
            for (int i = 0; i < fc_out / 2; i++) {
                bn_bf_1[i] = bn_bf[i];
                bn_bf_2[i] = bn_bf[i + fc_out / 2];
            }
            PhantomCiphertext ct_result = evalConv_BN(context, ckks_evaluator, encoder, ct_layer,
                                                      ker_inf_1, bn_af, bn_bf_1,
                                                      in_wids[2], ker_inf_wid, real_batch[2], fc_out / 2,
                                                      norm[2], std::pow(2.0, 30), false);
            PhantomCiphertext ct_result2 = evalConv_BN(context, ckks_evaluator, encoder, ct_layer,
                                                       ker_inf_2, bn_af, bn_bf_2,
                                                       in_wids[2], ker_inf_wid, real_batch[2], fc_out / 2,
                                                       norm[2], std::pow(2.0, 30), false);
            PhantomPlaintext pt_out1;
            PhantomPlaintext pt_out2;
            ckks_evaluator.decryptor.decrypt(ct_result, pt_out1);
            ckks_evaluator.decryptor.decrypt(ct_result2, pt_out2);
            vector<double> res_tmp1;
            vector<double> res_tmp2;
            encoder.decode_coeffs(context, pt_out1, res_tmp1);
            encoder.decode_coeffs(context, pt_out2, res_tmp2);
            vector<double> out1 = prt_mat_one_norm(res_tmp1, max_batch[2], norm[2], ker_inf_wid / 2 + 1, ker_inf_wid / 2 + 1);
            vector<double> out2 = prt_mat_one_norm(res_tmp2, max_batch[2], norm[2], ker_inf_wid / 2 + 1, ker_inf_wid / 2 + 1);
            res_out.reserve(out1.size() + out2.size());
            res_out.insert(res_out.end(), out1.begin(), out1.end());
            res_out.insert(res_out.end(), out2.begin(), out2.end());
        } else {
            vector<double> ker_inf_full(ker_inf_wid * ker_inf_wid * real_batch[2] * fc_out, 0.0);
            for (size_t i = 0; i < ker_inf.size(); i++) {
                for (int b = 0; b < ker_inf_wid * ker_inf_wid; b++) {
                    ker_inf_full[i + b * real_batch[2] * fc_out] = ker_inf[i];
                }
            }
            vector<double> bn_af(fc_out, 1.0 / static_cast<double>(raw_in_wids[2] * raw_in_wids[2]));
            vector<double> bn_bf = read_csv_floats(weight_dir + "final-fcbias.csv");
            PhantomCiphertext ct_result = evalConv_BN(context, ckks_evaluator, encoder, ct_layer,
                                                      ker_inf_full, bn_af, bn_bf,
                                                      in_wids[2], ker_inf_wid, real_batch[2], fc_out,
                                                      norm[2], std::pow(2.0, 30), false);
            PhantomPlaintext pt_out;
            ckks_evaluator.decryptor.decrypt(ct_result, pt_out);
            vector<double> res_tmp;
            encoder.decode_coeffs(context, pt_out, res_tmp);
            res_out = prt_mat_one_norm(res_tmp, max_batch[2], norm[2], ker_inf_wid / 2 + 1, ker_inf_wid / 2 + 1);
            if (res_out.size() > static_cast<size_t>(fc_out)) {
                res_out.resize(fc_out);
            }
        }

        cout << "result: ";
        for (size_t i = 0; i < res_out.size(); i++) {
            cout << res_out[i] << (i + 1 == res_out.size() ? "\n" : ", ");
        }
        write_csv_floats(out_dir + "class_result_" + ker_name + "_" + to_string(iter) + ".csv", res_out);
    }

    return 0;
}
