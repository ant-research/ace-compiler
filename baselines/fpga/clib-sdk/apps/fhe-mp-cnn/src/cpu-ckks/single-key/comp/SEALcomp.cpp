#include "SEALcomp.h"

void minimax_ReLU_seal(long comp_no, vector<int> deg, long alpha, vector<Tree> &tree, double scaled_val, long scalingfactor, Encryptor &encryptor, Evaluator &evaluator, Decryptor &decryptor, CKKSEncoder &encoder, PublicKey &public_key, SecretKey &secret_key, RelinKeys &relin_keys, Ciphertext& cipher_in, Ciphertext& cipher_res)
{
	// variables
	vector<vector<double>> decomp_coeff(comp_no, vector<double>(0));
	vector<double> scale_val(comp_no, 0.0);
	Plaintext plain_half;
	Ciphertext cipher_temp, cipher_half, cipher_x;

	// ifstream and scale
	string str;
	// The application carries its original minimax coefficients.
	string addr = CNN_DATA_ROOT "/result";
	str = addr + "/d" + to_string(alpha) + ".txt";
	ifstream in(str);
	if(!in.is_open()) throw std::runtime_error("ReLU composition coefficient file is not open: " + str);

	// scaled value setting	
	scale_val[0] = 1.0;	
	for(int i=1; i<comp_no; i++) scale_val[i] = 2.0;
	scale_val[comp_no-1] = scaled_val;

	// print degrees and coefficients of the component polynomials of minimax composite polynomial
	// for(int i=0; i<comp_no; i++) cout << deg[i] << " ";
	// cout << endl;
	for (int i = 0; i < comp_no; i++) {
		for(int j=0; j<coeff_number(deg[i], tree[i]); j++) {
			double temp;
			if (!(in >> temp) || !std::isfinite(temp)) throw std::runtime_error("invalid ReLU coefficients: " + str);
			decomp_coeff[i].emplace_back(temp);
			// cout << decomp_coeff[i][j] << " ";
		}
		// cout << endl;
	}

	// scale coefficients properly so that unnecessary level consumptions do not occur
	for(int i=0; i<comp_no-1; i++) for(int j=0; j<coeff_number(deg[i], tree[i]); j++) decomp_coeff[i][j] /= scale_val[i+1];
	for(int j=0; j<coeff_number(deg[comp_no-1], tree[comp_no-1]); j++) decomp_coeff[comp_no-1][j] *= 0.5;		// scale 

	// generation of half ciphertext
	long n = cipher_in.poly_modulus_degree()/2;
	vector<double> m_half(n);
	for(int i=0; i<n; i++) m_half[i] = 0.5;
	cipher_x = cipher_in;

	const bool refresh_middle = cipher_in.coeff_modulus_size() <= 28;
	// evaluating pk ... p1(x) / 2
	for (int i = 0; i < comp_no; ++i) {
		// cout << "*******************************************" << endl;
		// cout << "               No: " << i << endl;
		eval_polynomial_integrate(encryptor, evaluator, decryptor, encoder, public_key, secret_key, relin_keys, cipher_x, cipher_x, deg[i], decomp_coeff[i], tree[i]); 
        if (refresh_middle && i == 1) {
            // The sign-polynomial intermediate is much larger than a
            // convolution/B value. Keep EvalMod in its small-message range;
            // restore with an integer multiply, which consumes no level.
            constexpr int refresh_divisor = 256;
            evaluator.multiply_const_inplace(cipher_x, 1.0 / refresh_divisor);
            evaluator.rescale_to_next_inplace(cipher_x);
            while (cipher_x.coeff_modulus_size() > 2)
                evaluator.mod_switch_to_next_inplace(cipher_x);
            evaluator.bootstrap_inplace(cipher_x);
            evaluator.multiply_const_inplace(cipher_x, refresh_divisor);
            cipher_x.scale() = std::ldexp(1.0, scalingfactor);
        }
		decrypt_and_print_part(cipher_x, decryptor, encoder, n, 0, 5);
	}

	// x(1+sgn(x))/2 from sgn(x)/2
	encoder.encode(m_half, cipher_x.scale(), plain_half);
	encryptor.encrypt(plain_half, cipher_half);	
	evaluator.add_reduced_error(cipher_x, cipher_half, cipher_temp);
	evaluator.multiply_reduced_error(cipher_temp, cipher_in, relin_keys, cipher_res);
	evaluator.rescale_to_next_inplace(cipher_res);

}
