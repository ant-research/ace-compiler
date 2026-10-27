#pragma once

#include "boot.h"
#include "phantom.h"
#include <omp.h>
#include <NTL/RR.h>
#include <fstream>
#include <vector>
#include <chrono>

using namespace std;
using namespace phantom;

class TensorCipher
{
private:
	int k_; // k: gap
	int h_; // w: height
	int w_; // w: width
	int c_; // c: number of channels
	int t_; // t: \lfloor c/k^2 \rfloor
	int p_; // p: 2^log2(nt/k^2hwt)
	int logn_;
	PhantomCiphertext cipher_;

public:
	TensorCipher();
	TensorCipher(int logn, int k, int h, int w, int c, int t, int p, vector<double> data, CKKSEvaluator &evaluator, int logp); // data vector contains hxwxc real numbers.
	TensorCipher(int logn, int k, int h, int w, int c, int t, int p, PhantomCiphertext cipher);
	int k() const;
	int h() const;
	int w() const;
	int c() const;
	int t() const;
	int p() const;
	int logn() const;
	PhantomCiphertext cipher() const;
	void set_ciphertext(PhantomCiphertext cipher);
	void print_parms();
};

void multiplexed_parallel_convolution_print(const TensorCipher &cnn_in, TensorCipher &cnn_out, int co, int st, int fh, int fw, const vector<double> &data, vector<double> running_var, vector<double> constant_weight, double epsilon, CKKSEvaluator &ckksevaluator, vector<PhantomCiphertext > &cipher_pool, ofstream &output, size_t stage, bool end = false);
void multiplexed_parallel_batch_norm_phantom_print(const TensorCipher &cnn_in, TensorCipher &cnn_out, vector<double> bias, vector<double> running_mean, vector<double> running_var, vector<double> weight, double epsilon, CKKSEvaluator &ckksevaluator, double B, ofstream &output, size_t stage, bool end = false);
void multiplexed_parallel_convolution_phantom(const TensorCipher &cnn_in, TensorCipher &cnn_out, int co, int st, int fh, int fw, const vector<double> &data, vector<double> running_var, vector<double> constant_weight, double epsilon, CKKSEvaluator &ckksevaluator, vector<PhantomCiphertext> &cipher_pool, bool end = false);
void multiplexed_parallel_batch_norm_phantom(const TensorCipher &cnn_in, TensorCipher &cnn_out, vector<double> bias, vector<double> running_mean, vector<double> running_var, vector<double> weight, double epsilon, CKKSEvaluator &ckksevaluator, double B, bool end = false);



void approx_ReLU_phantom_print(const TensorCipher &cnn_in, TensorCipher &cnn_out, long comp_no, vector<int> deg, long alpha, vector<Tree> &tree, double scaled_val, long scalingfactor, CKKSEvaluator &ckksevaluator, double B, ofstream &output, size_t stage);
void minimax_ReLU_phantom(long comp_no, vector<int> deg, long alpha, vector<Tree> &tree, double scaled_val, long scalingfactor, CKKSEvaluator &ckksevaluator, PhantomCiphertext &cipher_in, PhantomCiphertext &cipher_res);


void bootstrap_print(const TensorCipher &cnn_in, TensorCipher &cnn_out, Bootstrapper &bootstrapper,CKKSEvaluator &ckksevaluator, ofstream &output, size_t stage);


void multiplexed_parallel_downsampling_phantom_print(const TensorCipher &cnn_in, TensorCipher &cnn_out,CKKSEvaluator &ckksevaluator, ofstream &output);
void multiplexed_parallel_downsampling_phantom(const TensorCipher &cnn_in, TensorCipher &cnn_out, CKKSEvaluator &ckksevaluator);


void cipher_add_phantom_print(const TensorCipher &cnn1, const TensorCipher &cnn2, TensorCipher &destination,  CKKSEvaluator &ckksevaluator, ofstream &output);
void cnn_add_phantom(const TensorCipher &cnn1, const TensorCipher &cnn2, TensorCipher &destination, CKKSEvaluator &ckksevaluator);

void averagepooling_phantom_scale_print(const TensorCipher &cnn_in, TensorCipher &cnn_out, CKKSEvaluator &ckksevaluator, double B, ofstream &output);
void averagepooling_phantom_scale(const TensorCipher &cnn_in, TensorCipher &cnn_out, CKKSEvaluator &ckksevaluator, double B, ofstream &output);

void fully_connected_phantom_print(const TensorCipher &cnn_in, TensorCipher &cnn_out, vector<double> matrix, vector<double> bias, int q, int r, CKKSEvaluator &ckksevaluator, ofstream &output);
void matrix_multiplication_phantom(const TensorCipher &cnn_in, TensorCipher &cnn_out, vector<double> matrix, vector<double> bias, int q, int r, CKKSEvaluator &ckksevaluator);



//void bootstrap_phantom(const TensorCipher &cnn_in, TensorCipher &cnn_out, Bootstrapper &bootstrapper, ofstream &output, Decryptor &decryptor, CKKSEncoder &encoder, SEALContext &context, size_t stage);

void ReLU_phantom(const TensorCipher &cnn_in, TensorCipher &cnn_out, long comp_no, vector<int> deg, long alpha, vector<Tree> &tree, double scaled_val, long scalingfactor, CKKSEvaluator &ckksevaluator, double scale);

void memory_save_rotate(const PhantomCiphertext &cipher_in, PhantomCiphertext &cipher_out, int steps, CKKSEvaluator &ckksevaluator);


void MultipleAdd_SEAL(CKKSEvaluator &ckksevaluator, PhantomCiphertext &cipher, PhantomCiphertext &result, long long n);
void test_evaluation(CKKSEvaluator &ckksevaluator, const PhantomCiphertext &cipher_in, PhantomCiphertext &cipher_out);
void geneT0T1(CKKSEvaluator &ckksevaluator, PhantomCiphertext &T0, PhantomCiphertext &T1, PhantomCiphertext &cipher);
void evalT(CKKSEvaluator &ckksevaluator, PhantomCiphertext &Tmplusn, const PhantomCiphertext &Tm, const PhantomCiphertext &Tn, const PhantomCiphertext &Tmminusn);
void eval_polynomial_integrate(CKKSEvaluator &ckksevaluator, PhantomCiphertext &res, PhantomCiphertext &cipher, long deg, const vector<RR> &decomp_coeff, Tree &tree);
long coeff_number(long deg, Tree &tree);
void coeff_change(long comp_no, long deg[], double *coeff[], long type[], vector<Tree> &tree);
long ShowFailure_ReLU(CKKSEvaluator &ckksevaluator, PhantomCiphertext &cipher, vector<double> &x, long precision, long n);

void decrypt_and_print( PhantomCiphertext &cipher, CKKSEvaluator &ckksevaluator, long sparse_slots, size_t front, size_t back);
void decrypt_and_print_part( PhantomCiphertext &cipher, CKKSEvaluator &ckksvaluator, long sparse_slots, size_t start, size_t end);
void decrypt_and_print_txt( PhantomCiphertext &cipher, CKKSEvaluator &ckksevaluator, long sparse_slots, size_t front, size_t back, ofstream &output);


//infer_seal
// import parameters
void import_parameters_cifar10(vector<double> &linear_weight, vector<double> &linear_bias, vector<vector<double>> &conv_weight, vector<vector<double>> &bn_bias, vector<vector<double>> &bn_running_mean, vector<vector<double>> &bn_running_var, vector<vector<double>> &bn_weight, size_t layer_num, size_t end_num);
void import_parameters_cifar100(vector<double> &linear_weight, vector<double> &linear_bias, vector<vector<double>> &conv_weight, vector<vector<double>> &bn_bias, vector<vector<double>> &bn_running_mean, vector<vector<double>> &bn_running_var, vector<vector<double>> &bn_weight, vector<vector<double>> &shortcut_weight, vector<vector<double>> &shortcut_bn_bias, vector<vector<double>> &shortcut_bn_mean, vector<vector<double>> &shortcut_bn_var, vector<vector<double>> &shortcut_bn_weight, size_t layer_num, size_t end_num);

// cifar10, cifar100 integrated
void ResNet_cifar10_seal_sparse(size_t layer_num, size_t start_image_id, size_t end_image_id);
void ResNet_cifar100_seal_sparse(size_t layer_num, size_t start_image_id, size_t end_image_id);

void upgrade_oddbaby(long n, Tree& tree);
void upgrade_baby(long n, Tree& tree);