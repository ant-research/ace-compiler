#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

#include "phantom.h"

using namespace std;
using namespace phantom;

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

static vector<double> encode_ker_final(const vector<vector<double>> &ker_in,
                                       int pos,
                                       int i,
                                       int in_wid,
                                       int in_batch,
                                       int ker_wid) {
    int vec_size = in_wid * in_wid * in_batch;
    vector<double> output(vec_size, 0.0);
    int bias = pos * ker_wid * ker_wid * in_batch;
    int k_sz = ker_wid * ker_wid;

    for (int j = 0; j < in_batch; j++) {
        for (int k = 0; k < k_sz; k++) {
            output[(in_wid * (k / ker_wid) + k % ker_wid) * in_batch + j] =
                ker_in[i][(in_batch - 1 - j) * k_sz + (k_sz - 1 - k) + bias];
        }
    }

    int adj = (in_batch - 1) + (in_batch) * (in_wid + 1) * (ker_wid - 1) / 2;
    vector<double> tmp(adj, 0.0);
    for (int idx = 0; idx < adj; idx++) {
        tmp[idx] = output[vec_size - adj + idx];
        output[vec_size - adj + idx] = -output[idx];
    }
    for (int idx = 0; idx < vec_size - 2 * adj; idx++) {
        output[idx] = output[idx + adj];
    }
    for (int idx = 0; idx < adj; idx++) {
        output[idx + vec_size - 2 * adj] = tmp[idx];
    }

    return output;
}

static vector<double> negacyclic_convolution(const vector<double> &a,
                                             const vector<double> &b) {
    size_t N = a.size();
    vector<double> out(N, 0.0);

    for (size_t i = 0; i < N; i++) {
        for (size_t j = 0; j < N; j++) {
            size_t k = i + j;
            if (k < N) {
                out[k] += a[i] * b[j];
            } else {
                out[k - N] -= a[i] * b[j];
            }
        }
    }

    return out;
}

int main() {
    EncryptionParameters parms(scheme_type::ckks);
    size_t poly_modulus_degree = 8192;
    double input_scale = pow(2.0, 40);
    double kernel_scale = input_scale;

    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_coeff_modulus(phantom::arith::CoeffModulus::Create(
        poly_modulus_degree, {60, 40, 40, 60}));

    PhantomContext context(parms);
    PhantomCKKSEncoder encoder(context);
    PhantomSecretKey secret_key(context);
    PhantomPublicKey public_key = secret_key.gen_publickey(context);
    PhantomRelinKey relin_keys = secret_key.gen_relinkey(context);
    PhantomGaloisKey galois_keys = secret_key.create_galois_keys(context);

    CKKSEvaluator ckks_evaluator(&context, &public_key, &secret_key,
                                 &encoder, &relin_keys, &galois_keys,
                                 input_scale);

    int in_wid = 32;
    int ker_wid = 3;
    int raw_in_wid = in_wid - ker_wid / 2;
    int norm = 1;
    int in_batch = static_cast<int>(poly_modulus_degree / (in_wid * in_wid));

    vector<double> raw_input(raw_in_wid * raw_in_wid * in_batch, 0.0);
    for (size_t i = 0; i < raw_input.size(); i++) {
        raw_input[i] = 0.01 * static_cast<double>(i + 1);
    }

    vector<double> kernel_vals(ker_wid * ker_wid, 0.0);
    for (size_t i = 0; i < kernel_vals.size(); i++) {
        kernel_vals[i] = 0.001 * static_cast<double>(i + 1);
    }

    vector<double> input_coeffs = prep_input(raw_input, raw_in_wid, in_wid,
                                             static_cast<int>(poly_modulus_degree), norm);

    vector<vector<double>> ker_mat(1, vector<double>(in_batch * ker_wid * ker_wid, 0.0));
    for (int ib = 0; ib < in_batch; ib++) {
        for (size_t k = 0; k < kernel_vals.size(); k++) {
            ker_mat[0][ib * kernel_vals.size() + k] = kernel_vals[k];
        }
    }
    vector<double> kernel_coeffs = encode_ker_final(ker_mat, 0, 0, in_wid,
                                                    in_batch, ker_wid);

    PhantomPlaintext pt_input;
    encoder.encode_coeffs(context, input_coeffs, input_scale, pt_input);
    PhantomCiphertext ct_input;
    ckks_evaluator.encryptor.encrypt(pt_input, ct_input);

    PhantomPlaintext pt_kernel;
    encoder.encode_coeffs(context, kernel_coeffs, kernel_scale, pt_kernel);

    ckks_evaluator.evaluator.multiply_plain_inplace(ct_input, pt_kernel);

    PhantomPlaintext pt_out;
    ckks_evaluator.decryptor.decrypt(ct_input, pt_out);

    vector<double> decoded;
    encoder.decode_coeffs(context, pt_out, decoded);

    vector<double> expected = negacyclic_convolution(input_coeffs, kernel_coeffs);

    double max_error = 0.0;
    for (size_t i = 0; i < expected.size(); i++) {
        max_error = max(max_error, fabs(decoded[i] - expected[i]));
    }

    cout << "Max error: " << max_error << endl;
    cout << "First 8 coeffs (decoded): ";
    for (size_t i = 0; i < 8; i++) {
        cout << decoded[i] << (i + 1 == 8 ? "\n" : ", ");
    }
    cout << "First 8 coeffs (expected): ";
    for (size_t i = 0; i < 8; i++) {
        cout << expected[i] << (i + 1 == 8 ? "\n" : ", ");
    }

    return 0;
}
