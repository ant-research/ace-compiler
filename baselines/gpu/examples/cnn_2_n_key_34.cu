#include <iostream>
#include <algorithm>
#include <NTL/RR.h>
#include <fstream>
#include <vector>
#include <chrono>
#include "boot/PolyUpdate.cuh"
#include "cnn_phantom.h"
#include "phantom.h"
#include "boot/Bootstrapper.cuh"

using namespace std;
using namespace phantom;
using namespace phantom::arith;
using namespace phantom::util;
// func dec

void upgrade_oddbaby(long n, Tree &tree);
void upgrade_baby(long n, Tree &tree);

int main(int argc, char **argv)
{
	int layer = atoi(argv[1]);
	int dataset = atoi(argv[2]);
	int start = atoi(argv[3]);
	int end = atoi(argv[4]);

	if (start < 0 || start >= 10000)
		throw std::invalid_argument("start number is not correct");
	if (end < 0 || end >= 10000)
		throw std::invalid_argument("end number is not correct");
	if (start > end)
		throw std::invalid_argument("start number is larger than end number");

	cout << "model: ResNet-" << layer << endl;
	cout << "dataset: CIFAR-" << dataset << endl;
	cout << "start image: " << start << endl;
	cout << "end image: " << end << endl;

	if (dataset == 10)
		ResNet_cifar10_seal_sparse(layer, start, end);
	else if (dataset == 100) ResNet_cifar100_seal_sparse(layer, start, end);
	else throw std::invalid_argument("dataset number is not correct");

	return 0;
}

void ResNet_cifar10_seal_sparse(size_t layer_num, size_t start_image_id, size_t end_image_id)
{
	// approximation boundary setting
	double B = 40.0; // approximation boundary

	// approx ReLU setting
	long alpha = 13;				// precision parameter alpha
	long comp_no = 3;				// number of compositions
	vector<int> deg = {15, 15, 27}; // degrees of component polynomials
	// double eta = pow(2.0,-15);		// margin
	double scaled_val = 1.7; // scaled_val: the last scaled value
	// double max_factor = 16;		// max_factor = 1 for comparison operation. max_factor > 1 for max or ReLU function
	vector<Tree> tree; // structure of polynomial evaluation
	evaltype ev_type = evaltype::oddbaby;
	// RR::SetOutputPrecision(25);

	// generate tree
	for (int i = 0; i < comp_no; i++)
	{
		Tree tr;
		if (ev_type == evaltype::oddbaby)
			upgrade_oddbaby(deg[i], tr);
		else if (ev_type == evaltype::baby)
			upgrade_baby(deg[i], tr);
		else
			std::invalid_argument("evaluation type is not correct");
		tree.emplace_back(tr);
		// tr.print();
	}

	// all threads output files
	ofstream out_share;
	if (layer_num == 20)
		out_share.open("../../result/resnet20_cifar10_label_" + to_string(start_image_id) + "_" + to_string(end_image_id));
	else if (layer_num == 32)
		out_share.open("../../result/resnet32_cifar10_label_" + to_string(start_image_id) + "_" + to_string(end_image_id));
	else if (layer_num == 44)
		out_share.open("../../result/resnet44_cifar10_label_" + to_string(start_image_id) + "_" + to_string(end_image_id));
	else if (layer_num == 56)
		out_share.open("../../result/resnet56_cifar10_label_" + to_string(start_image_id) + "_" + to_string(end_image_id));
	else if (layer_num == 110)
		out_share.open("../../result/resnet110_cifar10_label_" + to_string(start_image_id) + "_" + to_string(end_image_id));
	else
		throw std::invalid_argument("layer_num is not correct");

	// SEAL and bootstrapping setting
	long boundary_K = 25;
	long boot_deg = 59;
	long scale_factor = 2;
	long inverse_deg = 1;
	long logN = 16;
	long loge = 10;
	long logn = 15;	  // full slots
	long logn_1 = 14; // sparse slots
	long logn_2 = 13;
	long logn_3 = 12;
	int logp = 46;
	int logq = 51;
	int log_special_prime = 51;
	// int log_integer_part = logq - logp - loge + 5;
	int remaining_level = 20; // Calculation required
	int boot_level = 14;	  //
	int total_level = remaining_level + boot_level;

	vector<int> coeff_bit_vec;
	coeff_bit_vec.push_back(logq);
	for (int i = 0; i < remaining_level; i++)
		coeff_bit_vec.push_back(logp);
	for (int i = 0; i < boot_level; i++)
		coeff_bit_vec.push_back(logq);
	coeff_bit_vec.push_back(log_special_prime);

	// cout << "Setting Parameters" << endl;
	EncryptionParameters parms(scheme_type::ckks);
	size_t poly_modulus_degree = (size_t)(1 << logN);
	parms.set_poly_modulus_degree(poly_modulus_degree);
	parms.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, coeff_bit_vec));

	// added
	size_t secret_key_hamming_weight = 192;
	parms.set_secret_key_hamming_weight(secret_key_hamming_weight);
	// parms.set_sparse_slots(1 << logn_1);
	double scale = pow(2.0, logp);

	PhantomContext context(parms);
	// KeyGenerator keygen(context, 192);

	PhantomSecretKey secret_key(context);
	PhantomPublicKey public_key = secret_key.gen_publickey(context);
	PhantomRelinKey relin_keys = secret_key.gen_relinkey(context);
	PhantomGaloisKey galois_keys ;

	PhantomCKKSEncoder encoder(context);
	// ScaleInvEvaluator scale_evaluator(context, encoder, relin_keys);
	CKKSEvaluator ckks_evaluator(&context, &public_key, &secret_key, &encoder, &relin_keys, &galois_keys, scale);

	Bootstrapper bootstrapper_1(loge, logn_1, logN - 1, total_level, scale, boundary_K, boot_deg, scale_factor, inverse_deg, &ckks_evaluator);
	Bootstrapper bootstrapper_2(loge, logn_2, logN - 1, total_level, scale, boundary_K, boot_deg, scale_factor, inverse_deg, &ckks_evaluator);
	Bootstrapper bootstrapper_3(loge, logn_3, logN - 1, total_level, scale, boundary_K, boot_deg, scale_factor, inverse_deg, &ckks_evaluator);

	//	additional rotation kinds for CNN
	vector<int> rotation_kinds = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33
								  // ,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55
								  ,
								  56
								  // ,57,58,59,60,61
								  ,
								  62, 63, 64, 66, 84, 124, 128, 132, 256, 512, 959, 960, 990, 991, 1008, 1023, 1024, 1036, 1064, 1092, 1952, 1982, 1983, 2016, 2044, 2047, 2048, 2072, 2078, 2100, 3007, 3024, 3040, 3052, 3070, 3071, 3072, 3080, 3108, 4031, 4032, 4062, 4063, 4095, 4096, 5023, 5024, 5054, 5055, 5087, 5118, 5119, 5120, 6047, 6078, 6079, 6111, 6112, 6142, 6143, 6144, 7071, 7102, 7103, 7135, 7166, 7167, 7168, 8095, 8126, 8127, 8159, 8190, 8191, 8192, 9149, 9183, 9184, 9213, 9215, 9216, 10173, 10207, 10208, 10237, 10239, 10240, 11197, 11231, 11232, 11261, 11263, 11264, 12221, 12255, 12256, 12285, 12287, 12288, 13214, 13216, 13246, 13278, 13279, 13280, 13310, 13311, 13312, 14238, 14240, 14270, 14302, 14303, 14304, 14334, 14335, 15262, 15264, 15294, 15326, 15327, 15328, 15358, 15359, 15360, 16286, 16288, 16318, 16350, 16351, 16352, 16382, 16383, 16384, 17311, 17375, 18335, 18399, 18432, 19359, 19423, 20383, 20447, 20480, 21405, 21406, 21437, 21469, 21470, 21471, 21501, 21504, 22429, 22430, 22461, 22493, 22494, 22495, 22525, 22528, 23453, 23454, 23485, 23517, 23518, 23519, 23549, 24477, 24478, 24509, 24541, 24542, 24543, 24573, 24576, 25501, 25565, 25568, 25600, 26525, 26589, 26592, 26624, 27549, 27613, 27616, 27648, 28573, 28637, 28640, 28672, 29600, 29632, 29664, 29696, 30624, 30656, 30688, 30720, 31648, 31680, 31712, 31743, 31744, 31774, 32636, 32640, 32644, 32672, 32702, 32704, 32706, 32735, 32736, 32737, 32759, 32760, 32761, 32762, 32763, 32764, 32765, 32766, 32767};

	// bootstrapping preprocessing
	// cout << "Generating Optimal Minimax Polynomials..." << endl;
	bootstrapper_1.prepare_mod_polynomial();
	bootstrapper_2.prepare_mod_polynomial();
	bootstrapper_3.prepare_mod_polynomial();

	// cout << "Adding Bootstrapping Keys..." << endl;
	vector<int> gal_steps_vector;
	gal_steps_vector.push_back(0);
	for (int i = 0; i < logN - 1; i++)
	{
		gal_steps_vector.push_back((1 << i));
		gal_steps_vector.push_back(-(1 << i));

	}
	
//	for (auto rot : rotation_kinds)
//	{
//		if (find(gal_steps_vector.begin(), gal_steps_vector.end(), rot) == gal_steps_vector.end())
//			gal_steps_vector.push_back(rot);
//	}
	//bootstrapper_1.addLeftRotKeys_Linear_to_vector_3(gal_steps_vector);
	//bootstrapper_2.addLeftRotKeys_Linear_to_vector_3(gal_steps_vector);
	//bootstrapper_3.addLeftRotKeys_Linear_to_vector_3(gal_steps_vector);

	ckks_evaluator.decryptor.create_galois_keys_from_steps(gal_steps_vector, *(ckks_evaluator.galois_keys));

	bootstrapper_1.slot_vec.push_back(logn_1);
	bootstrapper_2.slot_vec.push_back(logn_2);
	bootstrapper_3.slot_vec.push_back(logn_3);

	// cout << "Generating Linear Transformation Coefficients..." << endl;
	bootstrapper_1.generate_LT_coefficient_3();
	bootstrapper_2.generate_LT_coefficient_3();
	bootstrapper_3.generate_LT_coefficient_3();

	// time setting
	chrono::high_resolution_clock::time_point all_time_start, all_time_end;
	chrono::microseconds all_time_diff;
	all_time_start = chrono::high_resolution_clock::now();

	// end number
	int end_num = 0;
	if (layer_num == 20)
		end_num = 2; // 0 ~ 2
	else if (layer_num == 32)
		end_num = 4; // 0 ~ 4
	else if (layer_num == 44)
		end_num = 6; // 0 ~ 6
	else if (layer_num == 56)
		end_num = 8; // 0 ~ 8
	else if (layer_num == 110)
		end_num = 17; // 0 ~ 17
	else
		throw std::invalid_argument("layer_num is not correct");

	for (size_t image_id = start_image_id; image_id <= end_image_id; image_id++)
	{
		// each thread output result file
		ofstream output;
		if (layer_num == 20)
			output.open("../../result/resnet20_cifar10_image" + to_string(image_id) + ".txt");
		else if (layer_num == 32)
			output.open("../../result/resnet32_cifar10_image" + to_string(image_id) + ".txt");
		else if (layer_num == 44)
			output.open("../../result/resnet44_cifar10_image" + to_string(image_id) + ".txt");
		else if (layer_num == 56)
			output.open("../../result/resnet56_cifar10_image" + to_string(image_id) + ".txt");
		else if (layer_num == 110)
			output.open("../../result/resnet110_cifar10_image" + to_string(image_id) + ".txt");
		else
			throw std::invalid_argument("layer_num is not correct");
		string dir = "resnet" + to_string(layer_num) + "_new";

		// PhantomCiphertext pool generation
		vector<PhantomCiphertext> cipher_pool(14);

		// time setting
		chrono::high_resolution_clock::time_point time_start, time_end, total_time_start, total_time_end;
		chrono::microseconds time_diff, total_time_diff;

		// variables
		TensorCipher cnn, temp;

		// deep learning parameters and import
		int co = 0, st = 0, fh = 3, fw = 3;
		long init_p = 8, n = 1 << logn;
		int stage = 0;
		double epsilon = 0.00001;
		vector<double> image, linear_weight, linear_bias;
		vector<vector<double>> conv_weight, bn_bias, bn_running_mean, bn_running_var, bn_weight;
		import_parameters_cifar10(linear_weight, linear_bias, conv_weight, bn_bias, bn_running_mean, bn_running_var, bn_weight, layer_num, end_num);
		// pack images compactly
		ifstream in;
		double val;
		in.open("../../testFile/test_values.txt");
		for (long i = 0; i < 1 << logn; i++)
			image.emplace_back(0);
		for (long i = 0; i < 32 * 32 * 3 * image_id; i++)
		{
			in >> val;
		}
		for (long i = 0; i < 32 * 32 * 3; i++)
		{
			in >> val;
			image[i] = val;
		}
		in.close();
		for (long i = n / init_p; i < n; i++)
			image[i] = image[i % (n / init_p)];
		for (long i = 0; i < n; i++)
			image[i] /= B; // for boundary [-1,1]

		ifstream in_label;
		int image_label;
		in_label.open("../../testFile/test_label.txt");
		for (long i = 0; i < image_id; i++)
		{
			in_label >> image_label;
		}
		in_label >> image_label;

		// generate CIFAR-10 image
		cnn = TensorCipher(logn, 1, 32, 32, 3, 3, init_p, image, ckks_evaluator, logq);
		// decrypt_and_print(cnn.cipher(), decryptor, encoder, 1<<logn, 256, 2); cnn.print_parms();
		// cout << "remaining level : " << cnn.cipher().chain_index() << endl;
		// cout << "scale: " << cnn.cipher().scale() << endl;
		total_time_start = chrono::high_resolution_clock::now();

		// modulus down
		PhantomCiphertext ctxt;
		ctxt = cnn.cipher();
		for (int i = 0; i < boot_level - 3; i++)
			ckks_evaluator.evaluator.mod_switch_to_next_inplace(ctxt);
		cnn.set_ciphertext(ctxt);

		// layer 0
		cout << "---------------------------------------------------------" << endl; 
		cout << "layer 0" << endl;
		output << "layer 0" << endl;
		multiplexed_parallel_convolution_print(cnn, cnn, 16, 1, fh, fw, conv_weight[stage], bn_running_var[stage], bn_weight[stage], epsilon, ckks_evaluator, cipher_pool, output, stage);

		// scaling factor ~2^51 -> 2^46
		//const auto &modulus = iter(context.first_context_data()->parms().coeff_modulus());
		const auto modulus = context.first_context_data().total_coeff_modulus();
		
		ctxt = cnn.cipher();
		size_t cur_level = ctxt.coeff_modulus_size();
		auto first_modulus = modulus[cur_level-1];
		PhantomPlaintext scaler;
		double scale_change = pow(2.0, 46) * ((double)first_modulus) / ctxt.scale();
		ckks_evaluator.encoder.encode(1, scale_change, scaler);
		ckks_evaluator.evaluator.mod_switch_to_inplace(scaler, ctxt.chain_index());
		ckks_evaluator.evaluator.multiply_plain_inplace(ctxt, scaler);
		ckks_evaluator.evaluator.rescale_to_next_inplace(ctxt);
		ctxt.scale() = pow(2.0, 46);
		cnn.set_ciphertext(ctxt);

		multiplexed_parallel_batch_norm_phantom_print(cnn, cnn, bn_bias[stage], bn_running_mean[stage], bn_running_var[stage], bn_weight[stage], epsilon, ckks_evaluator, B, output, stage);
		approx_ReLU_phantom_print(cnn, cnn, comp_no, deg, alpha, tree, scaled_val, logp, ckks_evaluator, B, output, stage);

		for (int j = 0; j < 3; j++) // layer 1_x, 2_x, 3_x
		{
			if (j == 0)
				co = 16;
			else if (j == 1)
				co = 32;
			else if (j == 2)
				co = 64;

			// // sparse slot
			// if(j==0) {
			// 	parms.set_sparse_slots(1<<logn_1);
			// 	encoder.set_sparse_slots(1<<logn_1);
			// } else if(j==1) {
			// 	parms.set_sparse_slots(1<<logn_2);
			// 	encoder.set_sparse_slots(1<<logn_2);
			// } else if(j==2) {
			// 	parms.set_sparse_slots(1<<logn_3);
			// 	encoder.set_sparse_slots(1<<logn_3);
			// }

			for (int k = 0; k <= end_num; k++) // 0 ~ 2/4/6/8/17
			{
				stage = 2 * ((end_num + 1) * j + k) + 1;
				cout << "layer " << stage << endl;
				output << "layer " << stage << endl;
				temp = cnn;
				if (j >= 1 && k == 0)
					st = 2;
				else
					st = 1;
				multiplexed_parallel_convolution_print(cnn, cnn, co, st, fh, fw, conv_weight[stage], bn_running_var[stage], bn_weight[stage], epsilon, ckks_evaluator, cipher_pool, output, stage);
				multiplexed_parallel_batch_norm_phantom_print(cnn, cnn, bn_bias[stage], bn_running_mean[stage], bn_running_var[stage], bn_weight[stage], epsilon, ckks_evaluator, B, output, stage);
				if (j == 0)
					bootstrap_print(cnn, cnn, bootstrapper_1, ckks_evaluator, output, stage);
				else if (j == 1)
					bootstrap_print(cnn, cnn, bootstrapper_2, ckks_evaluator, output, stage);
				else if (j == 2)
					bootstrap_print(cnn, cnn, bootstrapper_3, ckks_evaluator, output, stage);
				approx_ReLU_phantom_print(cnn, cnn, comp_no, deg, alpha, tree, scaled_val, logp, ckks_evaluator, B, output, stage);

				stage = 2 * ((end_num + 1) * j + k) + 2;
				cout << "layer " << stage << endl;
				output << "layer " << stage << endl;
				st = 1;

				multiplexed_parallel_convolution_print(cnn, cnn, co, st, fh, fw, conv_weight[stage], bn_running_var[stage], bn_weight[stage], epsilon, ckks_evaluator, cipher_pool, output, stage);
				multiplexed_parallel_batch_norm_phantom_print(cnn, cnn, bn_bias[stage], bn_running_mean[stage], bn_running_var[stage], bn_weight[stage], epsilon, ckks_evaluator, B, output, stage);
				if (j >= 1 && k == 0)
					multiplexed_parallel_downsampling_phantom_print(temp, temp, ckks_evaluator, output);
				cipher_add_phantom_print(temp, cnn, cnn, ckks_evaluator, output);
				if (j == 0)
					bootstrap_print(cnn, cnn, bootstrapper_1, ckks_evaluator, output, stage);
				else if (j == 1)
					bootstrap_print(cnn, cnn, bootstrapper_2, ckks_evaluator, output, stage);
				else if (j == 2)
					bootstrap_print(cnn, cnn, bootstrapper_3, ckks_evaluator, output, stage);
				approx_ReLU_phantom_print(cnn, cnn, comp_no, deg, alpha, tree, scaled_val, logp, ckks_evaluator, B, output, stage);
			}
		}
		cout << "layer " << layer_num - 1 << endl;
		output << "layer " << layer_num - 1 << endl;
		averagepooling_phantom_scale_print(cnn, cnn, ckks_evaluator, B, output);
		fully_connected_phantom_print(cnn, cnn, linear_weight, linear_bias, 10, 64,ckks_evaluator, output);

		total_time_end = chrono::high_resolution_clock::now();
		total_time_diff = chrono::duration_cast<chrono::milliseconds>(total_time_end - total_time_start);

		// final text file print
		PhantomPlaintext plain;
		auto decrypt_temp = cnn.cipher();
		ckks_evaluator.decryptor.decrypt(decrypt_temp, plain);
		vector<complex<double>> rtn_vec;
		// encoder.decode(plain, rtn_vec, 1<<logn);
		ckks_evaluator.encoder.decode(plain, rtn_vec);

		// noprintf final result
		// cout << "( ";
		// output << "( ";
		// for (size_t i = 0; i < 9; i++)
		// {
		// 	cout << rtn_vec[i] << ", ";
		// 	output << rtn_vec[i] << ", ";
		// }
		// cout << rtn_vec[9] << ")" << endl;
		// output << rtn_vec[9] << ")" << endl;
		cout << "\ntotal time : " << total_time_diff.count() / 1000 << " ms" << endl;
		output << "total time : " << total_time_diff.count() / 1000 << " ms" << endl;

		size_t label = 0;
		double max_score = -100.0;
		for (size_t i = 0; i < 10; i++)
		{
			if (max_score < rtn_vec[i].real())
			{
				label = i;
				max_score = rtn_vec[i].real();
			}
		}
		cout << "image label: " << image_label << endl;
		cout << "inferred label: " << label << endl;
		cout << "max score: " << max_score << endl;
		output << "image label: " << image_label << endl;
		output << "inferred label: " << label << endl;
		output << "max score: " << max_score << endl;
		out_share << "image_id: " << image_id << ", " << "image label: " << image_label << ", inferred label: " << label << endl;
	}

	all_time_end = chrono::high_resolution_clock::now();
	all_time_diff = chrono::duration_cast<chrono::milliseconds>(all_time_end - all_time_start);
	cout << "all threads time : " << all_time_diff.count() / 1000 << " ms" << endl;
	out_share << endl
			  << "all threads time : " << all_time_diff.count() / 1000 << " ms" << endl;
}

void ResNet_cifar100_seal_sparse(size_t layer_num, size_t start_image_id, size_t end_image_id)
{
	// approximation boundary setting
	double B = 65.0; // approximation boundary

	// approx ReLU setting
	long alpha = 13;				// precision parameter alpha
	long comp_no = 3;				// number of compositions
	vector<int> deg = {15, 15, 27}; // degrees of component polynomials
	// double eta = pow(2.0,-15);		// margin
	double scaled_val = 1.7; // scaled_val: the last scaled value
	// double max_factor = 16;		// max_factor = 1 for comparison operation. max_factor > 1 for max or ReLU function
	vector<Tree> tree; // structure of polynomial evaluation
	evaltype ev_type = evaltype::oddbaby;
	// RR::SetOutputPrecision(25);

	// generate tree
	for (int i = 0; i < comp_no; i++)
	{
		Tree tr;
		if (ev_type == evaltype::oddbaby)
			upgrade_oddbaby(deg[i], tr);
		else if (ev_type == evaltype::baby)
			upgrade_baby(deg[i], tr);
		else
			std::invalid_argument("evaluation type is not correct");
		tree.emplace_back(tr);
		// tr.print();
	}

	// all threads output files
	ofstream out_share;
	if (layer_num == 32)
		out_share.open("../../result/resnet32_cifar100_label_" + to_string(start_image_id) + "_" + to_string(end_image_id));
	else
		throw std::invalid_argument("layer_num is not correct");

	// SEAL and bootstrapping setting
	long boundary_K = 25;
	long boot_deg = 59;
	long scale_factor = 2;
	long inverse_deg = 1;
	long logN = 16;
	long loge = 10;
	long logn = 15;	  // full slots
	long logn_1 = 14; // sparse slots
	long logn_2 = 13;
	long logn_3 = 12;
	int logp = 46;
	int logq = 51;
	int log_special_prime = 51;
	// int log_integer_part = logq - logp - loge + 5;
	int remaining_level = 16; // Calculation required
	int boot_level = 14;	  //
	int total_level = remaining_level + boot_level;

	vector<int> coeff_bit_vec;
	coeff_bit_vec.push_back(logq);
	for (int i = 0; i < remaining_level; i++)
		coeff_bit_vec.push_back(logp);
	for (int i = 0; i < boot_level; i++)
		coeff_bit_vec.push_back(logq);
	coeff_bit_vec.push_back(log_special_prime);

	// cout << "Setting Parameters" << endl;
	EncryptionParameters parms(scheme_type::ckks);
	size_t poly_modulus_degree = (size_t)(1 << logN);
	parms.set_poly_modulus_degree(poly_modulus_degree);
	parms.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, coeff_bit_vec));

	// added
	size_t secret_key_hamming_weight = 192;
	parms.set_secret_key_hamming_weight(secret_key_hamming_weight);
	// parms.set_sparse_slots(1 << logn_1);
	double scale = pow(2.0, logp);

	PhantomContext context(parms);
	// KeyGenerator keygen(context, 192);

	PhantomSecretKey secret_key(context);
	PhantomPublicKey public_key = secret_key.gen_publickey(context);
	PhantomRelinKey relin_keys = secret_key.gen_relinkey(context);
	PhantomGaloisKey galois_keys;

	PhantomCKKSEncoder encoder(context);
	// ScaleInvEvaluator scale_evaluator(context, encoder, relin_keys);
	CKKSEvaluator ckks_evaluator(&context, &public_key, &secret_key, &encoder, &relin_keys, &galois_keys, scale);

	Bootstrapper bootstrapper_1(loge, logn_1, logN - 1, total_level, scale, boundary_K, boot_deg, scale_factor, inverse_deg, &ckks_evaluator);
	Bootstrapper bootstrapper_2(loge, logn_2, logN - 1, total_level, scale, boundary_K, boot_deg, scale_factor, inverse_deg, &ckks_evaluator);
	Bootstrapper bootstrapper_3(loge, logn_3, logN - 1, total_level, scale, boundary_K, boot_deg, scale_factor, inverse_deg, &ckks_evaluator);

	//	additional rotation kinds for CNN
	vector<int> rotation_kinds = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33
								  // ,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55
								  ,
								  56
								  // ,57,58,59,60,61
								  ,
								  62, 63, 64, 66, 84, 124, 128, 132, 256, 512, 959, 960, 990, 991, 1008, 1023, 1024, 1036, 1064, 1092, 1952, 1982, 1983, 2016, 2044, 2047, 2048, 2072, 2078, 2100, 3007, 3024, 3040, 3052, 3070, 3071, 3072, 3080, 3108, 4031, 4032, 4062, 4063, 4095, 4096, 5023, 5024, 5054, 5055, 5087, 5118, 5119, 5120, 6047, 6078, 6079, 6111, 6112, 6142, 6143, 6144, 7071, 7102, 7103, 7135, 7166, 7167, 7168, 8095, 8126, 8127, 8159, 8190, 8191, 8192, 9149, 9183, 9184, 9213, 9215, 9216, 10173, 10207, 10208, 10237, 10239, 10240, 11197, 11231, 11232, 11261, 11263, 11264, 12221, 12255, 12256, 12285, 12287, 12288, 13214, 13216, 13246, 13278, 13279, 13280, 13310, 13311, 13312, 14238, 14240, 14270, 14302, 14303, 14304, 14334, 14335, 15262, 15264, 15294, 15326, 15327, 15328, 15358, 15359, 15360, 16286, 16288, 16318, 16350, 16351, 16352, 16382, 16383, 16384, 17311, 17375, 18335, 18399, 18432, 19359, 19423, 20383, 20447, 20480, 21405, 21406, 21437, 21469, 21470, 21471, 21501, 21504, 22429, 22430, 22461, 22493, 22494, 22495, 22525, 22528, 23453, 23454, 23485, 23517, 23518, 23519, 23549, 24477, 24478, 24509, 24541, 24542, 24543, 24573, 24576, 25501, 25565, 25568, 25600, 26525, 26589, 26592, 26624, 27549, 27613, 27616, 27648, 28573, 28637, 28640, 28672, 29600, 29632, 29664, 29696, 30624, 30656, 30688, 30720, 31648, 31680, 31712, 31743, 31744, 31774, 32636, 32640, 32644, 32672, 32702, 32704, 32706, 32735, 32736, 32737, 32759, 32760, 32761, 32762, 32763, 32764, 32765, 32766, 32767};

	// bootstrapping preprocessing
	// cout << "Generating Optimal Minimax Polynomials..." << endl;
	bootstrapper_1.prepare_mod_polynomial();
	bootstrapper_2.prepare_mod_polynomial();
	bootstrapper_3.prepare_mod_polynomial();

	// cout << "Adding Bootstrapping Keys..." << endl;
	vector<int> gal_steps_vector;
	gal_steps_vector.push_back(0);
	for (int i = 0; i < logN - 1; i++)
		gal_steps_vector.push_back((1 << i));

	bootstrapper_1.addLeftRotKeys_Linear_to_vector_3(gal_steps_vector);
	bootstrapper_2.addLeftRotKeys_Linear_to_vector_3(gal_steps_vector);
	bootstrapper_3.addLeftRotKeys_Linear_to_vector_3(gal_steps_vector);

	ckks_evaluator.decryptor.create_galois_keys_from_steps(gal_steps_vector, *(ckks_evaluator.galois_keys));

	bootstrapper_1.slot_vec.push_back(logn_1);
	bootstrapper_2.slot_vec.push_back(logn_2);
	bootstrapper_3.slot_vec.push_back(logn_3);

	// cout << "Generating Linear Transformation Coefficients..." << endl;
	bootstrapper_1.generate_LT_coefficient_3();
	bootstrapper_2.generate_LT_coefficient_3();
	bootstrapper_3.generate_LT_coefficient_3();

	// time setting
	chrono::high_resolution_clock::time_point all_time_start, all_time_end;
	chrono::microseconds all_time_diff;
	all_time_start = chrono::high_resolution_clock::now();

	// end number
	int end_num = 4;
	
	for (size_t image_id = start_image_id; image_id <= end_image_id; image_id++)
	{
		// each thread output result file
		ofstream output;
		output.open("../../result/resnet32_cifar100_image" + to_string(image_id) + ".txt");
		
		string dir = "resnet32_cifar100";

		// PhantomCiphertext pool generation
		vector<PhantomCiphertext> cipher_pool(14);

		// time setting
		chrono::high_resolution_clock::time_point time_start, time_end, total_time_start, total_time_end;
		chrono::microseconds time_diff, total_time_diff;

		// variables
		TensorCipher cnn, temp;

		// deep learning parameters and import
		int co = 0, st = 0, fh = 3, fw = 3;
		long init_p = 8, n = 1 << logn;
		int stage = 0;
		double epsilon = 0.00001;
		vector<double> image, linear_weight, linear_bias;
		vector<vector<double>> conv_weight, bn_bias, bn_running_mean, bn_running_var, bn_weight, shortcut_weight, shortcut_bn_bias, shortcut_bn_mean, shortcut_bn_var, shortcut_bn_weight;
		import_parameters_cifar100(linear_weight, linear_bias, conv_weight, bn_bias, bn_running_mean, bn_running_var, bn_weight, shortcut_weight, shortcut_bn_bias, shortcut_bn_mean, shortcut_bn_var, shortcut_bn_weight, layer_num, end_num);
// pack images compactly
		ifstream in;
		double val;
		in.open("../../../testFile_cifar100/test_values.txt");
		for (long i = 0; i < 1 << logn; i++)
			image.emplace_back(0);
		for (long i = 0; i < 32 * 32 * 3 * image_id; i++)
		{
			in >> val;
		}
		for (long i = 0; i < 32 * 32 * 3; i++)
		{
			in >> val;
			image[i] = val;
		}
		in.close();
		for (long i = n / init_p; i < n; i++)
			image[i] = image[i % (n / init_p)];
		for (long i = 0; i < n; i++)
			image[i] /= B; // for boundary [-1,1]

		ifstream in_label;
		int image_label;
		in_label.open("../../../testFile_cifar100/test_label.txt");
		for (long i = 0; i < image_id; i++)
		{
			in_label >> image_label;
		}
		in_label >> image_label;

		// generate CIFAR-100 image
		cnn = TensorCipher(logn, 1, 32, 32, 3, 3, init_p, image, ckks_evaluator, logq);
		//decrypt_and_print(cnn.cipher(), decryptor, encoder, 1<<logn, 256, 2); cnn.print_parms();
		// cout << "remaining level : " << cnn.cipher().chain_index() << endl;
		// cout << "scale: " << cnn.cipher().scale() << endl;
		total_time_start = chrono::high_resolution_clock::now();

		// modulus down
		PhantomCiphertext ctxt;
		ctxt = cnn.cipher();
		for (int i = 0; i < boot_level - 3; i++)
			ckks_evaluator.evaluator.mod_switch_to_next_inplace(ctxt);
		cnn.set_ciphertext(ctxt);

		// layer 0
		cout << "---------------------------------------------------------" << endl; 
		cout << "layer 0" << endl;
		output << "layer 0" << endl;
		multiplexed_parallel_convolution_print(cnn, cnn, 16, 1, fh, fw, conv_weight[stage], bn_running_var[stage], bn_weight[stage], epsilon, ckks_evaluator, cipher_pool, output, stage);

		// scaling factor ~2^51 -> 2^46
		//const auto &modulus = iter(context.first_context_data()->parms().coeff_modulus());
		const auto modulus = context.first_context_data().total_coeff_modulus();
		
		ctxt = cnn.cipher();
		size_t cur_level = ctxt.coeff_modulus_size();
		auto first_modulus = modulus[cur_level-1];
		PhantomPlaintext scaler;
		double scale_change = pow(2.0, 46) * ((double)first_modulus) / ctxt.scale();
		ckks_evaluator.encoder.encode(1, scale_change, scaler);
		ckks_evaluator.evaluator.mod_switch_to_inplace(scaler, ctxt.chain_index());
		ckks_evaluator.evaluator.multiply_plain_inplace(ctxt, scaler);
		ckks_evaluator.evaluator.rescale_to_next_inplace(ctxt);
		ctxt.scale() = pow(2.0, 46);
		cnn.set_ciphertext(ctxt);

		multiplexed_parallel_batch_norm_phantom_print(cnn, cnn, bn_bias[stage], bn_running_mean[stage], bn_running_var[stage], bn_weight[stage], epsilon, ckks_evaluator, B, output, stage);
		approx_ReLU_phantom_print(cnn, cnn, comp_no, deg, alpha, tree, scaled_val, logp, ckks_evaluator, B, output, stage);

		for (int j = 0; j < 3; j++) // layer 1_x, 2_x, 3_x
		{
			if (j == 0)
				co = 16;
			else if (j == 1)
				co = 32;
			else if (j == 2)
				co = 64;

			

			for (int k = 0; k <= end_num; k++) // 0 ~ 2/4/6/8/17
			{
				stage = 2 * ((end_num + 1) * j + k) + 1;
				cout << "layer " << stage << endl;
				output << "layer " << stage << endl;
				temp = cnn;
				if (j >= 1 && k == 0)
					st = 2;
				else
					st = 1;
				multiplexed_parallel_convolution_print(cnn, cnn, co, st, fh, fw, conv_weight[stage], bn_running_var[stage], bn_weight[stage], epsilon, ckks_evaluator, cipher_pool, output, stage);
				multiplexed_parallel_batch_norm_phantom_print(cnn, cnn, bn_bias[stage], bn_running_mean[stage], bn_running_var[stage], bn_weight[stage], epsilon, ckks_evaluator, B, output, stage);
				if (j == 0)
					bootstrap_print(cnn, cnn, bootstrapper_1, ckks_evaluator, output, stage);
				else if (j == 1)
					bootstrap_print(cnn, cnn, bootstrapper_2, ckks_evaluator, output, stage);
				else if (j == 2)
					bootstrap_print(cnn, cnn, bootstrapper_3, ckks_evaluator, output, stage);
				approx_ReLU_phantom_print(cnn, cnn, comp_no, deg, alpha, tree, scaled_val, logp, ckks_evaluator, B, output, stage);

				stage = 2 * ((end_num + 1) * j + k) + 2;
				cout << "layer " << stage << endl;
				output << "layer " << stage << endl;
				st = 1;

				multiplexed_parallel_convolution_print(cnn, cnn, co, st, fh, fw, conv_weight[stage], bn_running_var[stage], bn_weight[stage], epsilon, ckks_evaluator, cipher_pool, output, stage);
				multiplexed_parallel_batch_norm_phantom_print(cnn, cnn, bn_bias[stage], bn_running_mean[stage], bn_running_var[stage], bn_weight[stage], epsilon, ckks_evaluator, B, output, stage);
				if (j >= 1 && k == 0)
					multiplexed_parallel_downsampling_phantom_print(temp, temp, ckks_evaluator, output);
				cipher_add_phantom_print(temp, cnn, cnn, ckks_evaluator, output);
				if (j == 0)
					bootstrap_print(cnn, cnn, bootstrapper_1, ckks_evaluator, output, stage);
				else if (j == 1)
					bootstrap_print(cnn, cnn, bootstrapper_2, ckks_evaluator, output, stage);
				else if (j == 2)
					bootstrap_print(cnn, cnn, bootstrapper_3, ckks_evaluator, output, stage);
				approx_ReLU_phantom_print(cnn, cnn, comp_no, deg, alpha, tree, scaled_val, logp, ckks_evaluator, B, output, stage);
			}
		}
		cout << "layer " << layer_num - 1 << endl;
		output << "layer " << layer_num - 1 << endl;
		averagepooling_phantom_scale_print(cnn, cnn, ckks_evaluator, B, output);
		fully_connected_phantom_print(cnn, cnn, linear_weight, linear_bias, 100, 64,ckks_evaluator, output);

		total_time_end = chrono::high_resolution_clock::now();
		total_time_diff = chrono::duration_cast<chrono::milliseconds>(total_time_end - total_time_start);

		// final text file print
		PhantomPlaintext plain;
		auto decrypt_temp = cnn.cipher();
		ckks_evaluator.decryptor.decrypt(decrypt_temp, plain);
		vector<complex<double>> rtn_vec;
		// encoder.decode(plain, rtn_vec, 1<<logn);
		ckks_evaluator.encoder.decode(plain, rtn_vec);
		cout << "( ";
		output << "( ";
		for (size_t i = 0; i < 99; i++)
		{
			cout << rtn_vec[i] << ", ";
			output << rtn_vec[i] << ", ";
		}
		cout << rtn_vec[9] << ")" << endl;
		output << rtn_vec[9] << ")" << endl;
		cout << "\ntotal time : " << total_time_diff.count() / 1000 << " ms" << endl;
		output << "total time : " << total_time_diff.count() / 1000 << " ms" << endl;

		size_t label = 0;
		double max_score = -100.0;
		for (size_t i = 0; i < 10; i++)
		{
			if (max_score < rtn_vec[i].real())
			{
				label = i;
				max_score = rtn_vec[i].real();
			}
		}
		cout << "image label: " << image_label << endl;
		cout << "inferred label: " << label << endl;
		cout << "max score: " << max_score << endl;
		output << "image label: " << image_label << endl;
		output << "inferred label: " << label << endl;
		output << "max score: " << max_score << endl;
		out_share << "image_id: " << image_id << ", " << "image label: " << image_label << ", inferred label: " << label << endl;
	}

	all_time_end = chrono::high_resolution_clock::now();
	all_time_diff = chrono::duration_cast<chrono::milliseconds>(all_time_end - all_time_start);
	cout << "all threads time : " << all_time_diff.count() / 1000 << " ms" << endl;
	out_share << endl
			  << "all threads time : " << all_time_diff.count() / 1000 << " ms" << endl;
}


void upgrade_oddbaby(long n, Tree &tree) // n should be odd
{
	long d = ceil_to_int(log(static_cast<double>(n)) / log(2.0)); // required minimum depth
	long m, l;
	long min, total_min = 10000, min_m = 0, min_l = 0;
	Tree min_tree, total_min_tree;

	for (l = 1; pow2(l) - 1 <= n; l++)
	{
		for (m = 1; pow2(m - 1) < n; m++)
		{
			// initialization
			vector<vector<int>> f(n + 1, vector<int>(d + 1, 0));
			vector<vector<Tree>> G(n + 1, vector<Tree>(d + 1, Tree(evaltype::oddbaby)));
			f[1][1] = 0;
			for (int i = 3; i <= n; i += 2)
				f[i][1] = 10000;

			// recursion
			for (int j = 2; j <= d; j++)
			{
				for (int i = 1; i <= n; i += 2)
				{
					if (i <= pow2(l) - 1 && i <= pow2(j - 1))
						f[i][j] = 0;
					else
					{
						min = 10000;
						min_tree.clear();
						for (int k = 1; k <= m - 1 && pow2(k) < i && k < j; k++) // g = 2^k
						{
							long g = pow2(k);
							if (f[i - g][j - 1] + f[g - 1][j] + 1 < min)
							{
								min = f[i - g][j - 1] + f[g - 1][j] + 1;
								min_tree.merge(G[g - 1][j], G[i - g][j - 1], g);
							}
						}
						f[i][j] = min;
						G[i][j] = min_tree;
					}
				}
			}
			if (f[n][d] + pow2(l - 1) + m - 2 < total_min)
			{
				total_min = f[n][d] + pow2(l - 1) + m - 2;
				total_min_tree = G[n][d];
				min_m = m;
				min_l = l;
			}
		}
	}

	// cout << "deg " << n << ": " << total_min << endl;
	// cout << "m: " << min_m << ", l: " << min_l << endl;
	tree = total_min_tree;
	tree.m = min_m;
	tree.l = min_l;
}
void upgrade_baby(long n, Tree &tree)
{
	long d = ceil_to_int(log(static_cast<double>(n + 1)) / log(2.0)); // required minimum depth
	long m, b;
	long min, total_min = 10000, min_m = 0, min_b = 0;
	Tree min_tree, total_min_tree;
	evaltype type = evaltype::baby;

	// cout << "minimum depth: " << d << endl;

	// n==1
	if (n == 1)
	{
		total_min = 0;
		total_min_tree = Tree(type);
		min_m = 1;
		min_b = 1;
	}

	for (b = 1; b <= n; b++)
	{
		for (m = 1; pow2(m - 1) * b <= n; m++)
		{
			//	cout << "Stage b,m: " << b << " " << m << endl;

			// initialization
			vector<vector<int>> f(n + 1, vector<int>(d + 1, 0));
			vector<vector<Tree>> G(n + 1, vector<Tree>(d + 1, Tree(type)));

			// recursion
			for (int j = 1; j <= d; j++)
			{
				for (int i = 1; i <= n; i++)
				{
					// int k;
					if (i + 1 > pow2(j))
					{
						f[i][j] = 10000;
						G[i][j] = Tree(type);
					}
					else if (b == 1 && m >= 2 && i <= 2 && i <= pow2(j - 1))
					{
						f[i][j] = 0;
						G[i][j] = Tree(type);
					}
					else if (i <= b && i <= pow2(j - 1))
					{
						f[i][j] = 0;
						G[i][j] = Tree(type);
					}
					else
					{
						min = 10000;
						min_tree.clear();
						for (int k = 2; k <= b; k++) // g = k
						{
							long g = k;
							if (g <= pow2(j - 1) && 2 <= g && g < i && f[i - g][j - 1] + f[g - 1][j] + 1 < min)
							{
								min = f[i - g][j - 1] + f[g - 1][j] + 1;
								min_tree.merge(G[g - 1][j], G[i - g][j - 1], g);
							}
						}
						for (int k = 0; k <= m - 1; k++) // g = 2^k b
						{
							long g = pow2(k) * b;
							if (g <= pow2(j - 1) && 2 <= g && g < i && f[i - g][j - 1] + f[g - 1][j] + 1 < min)
							{
								min = f[i - g][j - 1] + f[g - 1][j] + 1;
								min_tree.merge(G[g - 1][j], G[i - g][j - 1], g);
							}
						}
						f[i][j] = min;
						G[i][j] = min_tree;
						if (min == 10000)
						{
							//		cout << "no g found " << b << " " << m << " " << j << " " << i << endl;
							//		throw std::runtime_error("this case should not occur!");
						}
					}
				}
			}
			if (f[n][d] + m + b - 2 < total_min)
			{
				total_min = f[n][d] + m + b - 2;
				total_min_tree = G[n][d];
				min_m = m;
				min_b = b;
			}
		}
	}

	// cout << "deg " << n << ": " << total_min << endl;
	// cout << "m: " << min_m << ", b: " << min_b << endl;
	tree = total_min_tree;
	tree.m = min_m;
	tree.b = min_b;
}
