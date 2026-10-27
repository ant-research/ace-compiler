#include <random>

#include "boot/Bootstrapper.cuh"
#include "phantom.h"

using namespace std;
using namespace phantom;

void random_real(vector<double> &vec, size_t size)
{
  random_device rn;
  mt19937_64 rnd(rn());
  thread_local std::uniform_real_distribution<double> distribution(-1, 1);

  vec.reserve(size);

  for (size_t i = 0; i < size; i++)
  {
    vec[i] = distribution(rnd);
  }
}

int main()
{
  long boundary_K = 25;
  long deg = 59;
  long scale_factor = 2;
  long inverse_deg = 1;
  bool enable_relu = false;

  // The following parameters have been adjusted to satisfy the memory constraints of an H800 GPU
  long logN = 16; // 16 -> 15
  long loge = 10;

  long logn = 14; // 14 -> 13
  size_t sparse_slot_count = 1 << logn;

  int logp = 47;
  int logq = 51;
  int log_special_prime = 51;
  // Non-hybrid key switching uses one special prime.
  // Set this to >1 to enable hybrid key switching.
  int special_modulus_size = 4;
  int secret_key_hamming_weight = 192;

  int remaining_level = 3+7; // s2c
  int boot_level = 3       // c2s
                   + 6 + 2; // sin & double angle => sin(2*pi*x)
  int total_level = remaining_level + boot_level;

  vector<int> coeff_bit_vec;
  coeff_bit_vec.push_back(logq);
 
  for (int i = 0; i < remaining_level; i++)
  {
    coeff_bit_vec.push_back(logp);
  }
  for (int i = 0; i < boot_level; i++)
  {
    coeff_bit_vec.push_back(logq);
  }
  for (int i = 0; i < special_modulus_size; i++)
  {
    coeff_bit_vec.push_back(log_special_prime);
  }
  std::cout << "Setting Parameters..." << endl;
  phantom::EncryptionParameters parms(scheme_type::ckks);
  size_t poly_modulus_degree = (size_t)(1 << logN);
  double scale = pow(2.0, logp);

  parms.set_poly_modulus_degree(poly_modulus_degree);
  parms.set_coeff_modulus(phantom::arith::CoeffModulus::Create(poly_modulus_degree, coeff_bit_vec));
  parms.set_secret_key_hamming_weight(secret_key_hamming_weight);
  parms.set_special_modulus_size(special_modulus_size);
  PhantomContext context(parms);

  PhantomSecretKey secret_key(context);
  PhantomPublicKey public_key = secret_key.gen_publickey(context);
  PhantomRelinKey relin_keys = secret_key.gen_relinkey(context);
  PhantomGaloisKey galois_keys;
  PhantomCKKSEncoder encoder(context);

  CKKSEvaluator ckks_evaluator(&context, &public_key, &secret_key, &encoder, &relin_keys, &galois_keys, scale);

  Bootstrapper bootstrapper(
      loge,
      logn,
      logN - 1,
      total_level,
      scale,
      boundary_K,
      deg,
      scale_factor,
      inverse_deg,
      &ckks_evaluator,enable_relu);
  //bootstrapper.set_slim_relu(enable_relu);

  std::cout << "Generating Optimal Minimax Polynomials..." << endl;
  bootstrapper.prepare_mod_polynomial();

  std::cout << "Adding Bootstrapping Keys..." << endl;
  vector<int> gal_steps_vector;
  gal_steps_vector.push_back(0);
  for (int i = 0; i < logN - 1; i++)
  {
    gal_steps_vector.push_back((1 << i));
  }
  bootstrapper.addLeftRotKeys_Linear_to_vector_3(gal_steps_vector);
  ckks_evaluator.decryptor.create_galois_keys_from_steps(gal_steps_vector, *(ckks_evaluator.galois_keys));
  bootstrapper.slot_vec.push_back(logn);

  std::cout << "Generating Linear Transformation Coefficients..." << endl;
  bootstrapper.generate_LT_coefficient_3();
  std::cout << "Generating Linear Transformation Coefficients... Done" << endl;

  vector<double> input(1 << (logN - 1));
  vector<double> before(1 << (logN - 1));
  vector<double> after(1 << (logN - 1));

  PhantomPlaintext plain;
  PhantomCiphertext cipher;

  for (int i = 0; i < sparse_slot_count; i++)
  {
    input[i] = -1 + (1. - -1) * i / sparse_slot_count;
  }
  input.resize(1 << (logN - 1));
  for (int i = sparse_slot_count; i < input.size(); i++)
  {
    input[i] = input[i % sparse_slot_count];
  }

  ckks_evaluator.encoder.encode(input, scale, plain);
  ckks_evaluator.encryptor.encrypt(plain, cipher);

  // Mod switch to the lowest level
  for (int i = 0; i < total_level - 3; i++)
  {
    ckks_evaluator.evaluator.mod_switch_to_next_inplace(cipher);
  }
  std::cout<<"the initial scale is "<<log2(cipher.scale())<<std::endl;
  // Decrypt input cipher to obtain the original input
  ckks_evaluator.decryptor.decrypt(cipher, plain);
  ckks_evaluator.encoder.decode(plain, before);

  auto start = system_clock::now();

  PhantomCiphertext rtn;
  bootstrapper.slim_bootstrap(rtn, cipher);

  duration<double> sec = system_clock::now() - start;
  std::cout << "Bootstrapping took: " << sec.count() << "s" << endl;
  std::cout << "Return cipher level: " << rtn.chain_index() << std::endl;

  ckks_evaluator.decryptor.decrypt(rtn, plain);
  ckks_evaluator.encoder.decode(plain, after);

  double max_err = 0;
  double avg_err = 0;
  double ignore_eps = 0.01;
  auto relu = [](auto x)
  { return (x >= 0.0) ? x : 0.0; };
  auto v_shape = [](auto x)
  { return abs(x); };
  auto id = [](auto x)
  { return x; };
  for (size_t i = 0; i < sparse_slot_count; i++)
  {
    auto expected = enable_relu ? relu(input[i]) : id(input[i]);
    auto curr_err = abs(expected - after[i]);

    if (abs((double)i - (double)sparse_slot_count / 2) / ((double)sparse_slot_count / 2) >= ignore_eps)
    {
      max_err = max(max_err, curr_err);
    }
    avg_err += curr_err;
    if (i < 10 || i > sparse_slot_count-10)
    {
      cout << "(" << input[i] << ", " << after[i] << "), ";
    }
  }
  cout << endl;
  cout << "max error: " << max_err << " (2^" << log2(max_err) << ")" << endl;
  avg_err /= sparse_slot_count;
  cout << "avg error: " << avg_err << " (2^" << log2(avg_err) << ")" << endl;

  return 0;
}
