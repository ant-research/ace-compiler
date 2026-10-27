#pragma once

#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>

#include "boot/RemezArcsin.cuh"
#include "boot/RemezCos.cuh"
#include "boot/Polynomial.cuh"
#include "boot/PolyUpdate.cuh"
using namespace std;

class ModularReducer {
 public:
  long boundary_K;
  double log_width;
  long deg;
  long num_double_formula;

  double inverse_log_width;
  long inverse_deg;

  double scale_inverse_coeff;

  CKKSEvaluator *ckks = nullptr;

  RemezParam rmparm;

  RemezSin* poly_generator;

  RemezSin* sin_generator;
	RemezSin* cos_generator;
  RemezArcsin *inverse_poly_generator;

	boot::Polynomial sin_cos_polynomial;
	boot::Polynomial inverse_sin_polynomial;


  boot::Polynomial sin_polynomial;
	boot::Polynomial cos_polynomial;
	boot::Polynomial inverse_sin_polynomial_v1;
	boot::Polynomial inverse_sin_polynomial_v1_original;

  ModularReducer(long _boundary_K, double _log_width, long _deg, long _num_double_formula, long _inverse_deg, CKKSEvaluator *ckks, bool _use_relu_mode = false);

  void double_angle_formula(PhantomCiphertext &cipher);
  void double_angle_formula_scaled(PhantomCiphertext &cipher, double scale_coeff);

  void generate_sin_cos_polynomial();
  void generate_inverse_sine_polynomial();

  void generate_sin_cos_polynomial_relu();
  void generate_inverse_sine_polynomial_relu();

  void scaling_for_turn_back_q();
  void write_polynomials();
  inline void set_relu_mode(bool enable_relu) { use_relu_mode = enable_relu; }
  inline bool relu_mode() const { return use_relu_mode; }
  void modular_reduction_relu(PhantomCiphertext &rtn, PhantomCiphertext &cipher);
  void modular_reduction(PhantomCiphertext &rtn, PhantomCiphertext &cipher);

	vector<RR> arcsin_decomp_coeff;
	vector<RR> arcsin_decomp_coeff_original;
	minicomp::Tree arcsin_tree;
	double abs_lift = 0.125;
  bool use_relu_mode = false;
};

static double scale_for_eval = 4.0 * 0.9; // tuning
extern double scale_for_boost_relu_range;
extern double dyn_scale_for_turn_back_q;
extern bool after_cnn_first_boot;
