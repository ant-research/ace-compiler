#ifndef RTLIB_RT_PHANTOM_RT_PHANTOM_H
#define RTLIB_RT_PHANTOM_RT_PHANTOM_H

//! @brief rt_phantom.cuh
//! Define common API for fhe-cmplr. Dispatch to specific implementations
//! built on top of Phantom libraries.

#include "common/pt_mgr.h"
#include "common/rt_api.h"
#include "phantom_api.h"

#ifdef ENABLE_PERFORMANCE_STATS
#include <chrono>
#include <iostream>
#include <unordered_map>

struct OperationStats {
  size_t count      = 0;
  double total_time = 0.0;
};

extern std::unordered_map<std::string, OperationStats> operation_stats;

#define START_TIMER auto start = std::chrono::high_resolution_clock::now();
#define END_TIMER(op_name)                                \
  {                                                       \
    auto end = std::chrono::high_resolution_clock::now(); \
    std::chrono::duration<double> diff = end - start;     \
    operation_stats[op_name].count++;                     \
    operation_stats[op_name].total_time += diff.count();  \
  }
#else
#define START_TIMER
#define END_TIMER(op_name)
#endif

//! @brief Get polynomial degree
inline uint32_t Degree() { return Get_context_params()->_poly_degree; }

//! @brief Get input cipher by name and index
inline CIPHERTEXT Get_input_data(const char* name, size_t idx) {
  START_TIMER
  auto result = Phantom_get_input_data(name, idx);
  END_TIMER("Get_input_data")
  return result;
}

//! @brief Set output cipher by name and index
inline void Set_output_data(const char* name, size_t idx, CIPHER data) {
  START_TIMER
  Phantom_set_output_data(name, idx, data);
  END_TIMER("Set_output_data")
}

//! @brief Encode float array into plaintext.
//!  The 4th parameter is named with 'sc_degree' to match with other libraries
//!  but it's actually the scale value = pow(2.0, sc_degree).
inline void Encode_float(PLAIN plain, float* input, size_t len,
                         SCALE_T sc_degree, LEVEL_T level) {
  START_TIMER
  Phantom_encode_float(plain, input, len, sc_degree, level);
  END_TIMER("Encode_float")
}

inline void Encode_float_cst_lvl(PLAIN plain, float* input, size_t len,
                                 SCALE_T sc_degree, int level) {
  START_TIMER
  Phantom_encode_float_cst_lvl(plain, input, len, sc_degree, level);
  END_TIMER("Encode_float_cst_lvl")
}

inline void Encode_float_mask(PLAIN plain, float input, size_t len,
                              SCALE_T sc_degree, LEVEL_T level) {
  START_TIMER
  Phantom_encode_float_mask(plain, input, len, sc_degree, level);
  END_TIMER("Encode_float_mask")
}

inline void Encode_float_mask_cst_lvl(PLAIN plain, float input, size_t len,
                                      SCALE_T sc_degree, int level) {
  START_TIMER
  Phantom_encode_float_mask_cst_lvl(plain, input, len, sc_degree, level);
  END_TIMER("Encode_float_mask_cst_lvl")
}

// HE Operations
inline CIPHER Add_scalar(CIPHER res, CIPHER op, double op2) {
  START_TIMER
  Phantom_add_const(res, op, op2);
  END_TIMER("Add_const")
  return res;
}

inline CIPHER Add_ciph(CIPHER res, CIPHER op1, CIPHER op2) {
  START_TIMER
  Phantom_add_ciph(res, op1, op2);
  END_TIMER("Add_ciph")
  return res;
}

inline CIPHER Add_plain(CIPHER res, CIPHER op1, PLAIN op2) {
  START_TIMER
  Phantom_add_plain(res, op1, op2);
  END_TIMER("Add_plain")
  return res;
}

// Mul_ciph has two type of interfaces, one for CIPHER and one for double
inline CIPHER Mul_ciph(CIPHER res, CIPHER op1, CIPHER op2) {
  START_TIMER
  Phantom_mul_ciph(res, op1, op2);
  END_TIMER("Mul_ciph")
  return res;
}

inline CIPHER Mul_scalar(CIPHER res, CIPHER op1, double op2) {
  START_TIMER
  Phantom_mul_ciph_const(res, op1, op2);
  END_TIMER("Mul_ciph_const")
  return res;
}

inline CIPHER Mul_plain(CIPHER res, CIPHER op1, PLAIN op2) {
  START_TIMER
  Phantom_mul_plain(res, op1, op2);
  END_TIMER("Mul_plain")
  return res;
}

inline CIPHER Rotate_ciph(CIPHER res, CIPHER op, int step) {
  START_TIMER
  Phantom_rotate(res, op, step);
  END_TIMER("Rotate_ciph")
  return res;
}

inline CIPHER Rescale_ciph(CIPHER res, CIPHER op) {
  START_TIMER
  Phantom_rescale(res, op);
  END_TIMER("Rescale_ciph")
  return res;
}

inline CIPHER Mod_switch(CIPHER res, CIPHER op) {
  START_TIMER
  Phantom_mod_switch(res, op);
  END_TIMER("Mod_switch")
  return res;
}

inline CIPHER Relin(CIPHER res, CIPHER3 ciph) {
  START_TIMER
  Phantom_relin(res, ciph);
  END_TIMER("Relin")
  return res;
}

inline CIPHER Bootstrap(CIPHER res, CIPHER op, int level, int slot) {
  START_TIMER
  Phantom_bootstrap(res, op, level, slot);
  END_TIMER("Bootstrap")
  return res;
}

inline void Copy_ciph(CIPHER res, CIPHER op) {
  if (res != op) {
    START_TIMER
    Phantom_copy(res, op);
    END_TIMER("Copy_ciph")
  }
}

inline void Zero_ciph(CIPHER res) {
  START_TIMER
  Phantom_zero(res);
  END_TIMER("Zero_ciph")
}

inline SCALE_T Sc_degree(CIPHER ct) {
  START_TIMER
  auto result = Phantom_scale(ct);
  END_TIMER("Sc_degree")
  return result;
}

inline LEVEL_T Level(CIPHER ct) {
  START_TIMER
  auto result = Phantom_level(ct);
  END_TIMER("Level")
  return result;
}

inline void Free_ciph(CIPHER res) {
  START_TIMER
  Phantom_free_ciph(res);
  END_TIMER("Free_ciph")
}
inline void Free_plain(PLAIN res) {
  START_TIMER
  Phantom_free_plain(res);
  END_TIMER("Free_plain")
}
inline void Free_ciph_array(CIPHER3 res, size_t size) {
  START_TIMER
  Phantom_free_ciph_array(res, size);
  END_TIMER("Free_ciph_array")
}

// Dump utilities
void Dump_ciph(CIPHER ct, size_t start, size_t len);

void Dump_plain(PLAIN pt, size_t start, size_t len);

void Dump_cipher_msg(const char* name, CIPHER ct, uint32_t len);

void Dump_plain_msg(const char* name, PLAIN pt, uint32_t len);

double* Get_msg(CIPHER ct);

double* Get_msg_from_plain(PLAIN pt);

inline uint32_t Get_ciph_slots(CIPHER ct) { return Degree() / 2; }

inline uint32_t Get_plain_slots(PLAIN pt) { return Degree() / 2; }

bool Within_value_range(CIPHER ciph, double* msg, uint32_t len);

#ifdef ENABLE_PERFORMANCE_STATS
inline void Print_performance_stats() {
  std::cout << "Performance Statistics:" << std::endl;
  double all_operations_total_time_ms = 0.0;
  for (const auto& pair : operation_stats) {
    double total_time_ms = pair.second.total_time * 1000.0;  // 转换为毫秒
    double avg_time_ms   = total_time_ms / pair.second.count;
    all_operations_total_time_ms += total_time_ms;
    std::cout << pair.first << ": "
              << "Count: " << pair.second.count << ", "
              << "Total Time: " << std::fixed << std::setprecision(6)
              << total_time_ms << " ms, "
              << "Average Time: " << std::fixed << std::setprecision(6)
              << avg_time_ms << " ms" << std::endl;
  }
  std::cout << "\n=== Summary ===" << std::endl;
  std::cout << "Total Execution Time (all operations): " << std::fixed
            << std::setprecision(6) << all_operations_total_time_ms << " ms"
            << std::endl;
}
#else
inline void Print_performance_stats() {
  std::cout << "Performance statistics are not enabled." << std::endl;
}
#endif

// for validate APIs
#include "common/rt_validate.h"

#endif  // RTLIB_RT_PHANTOM_RT_PHANTOM_H
