//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_LLAMA_LLAMA_H
#define NN_LLAMA_LLAMA_H

#include <vector>

#include "air/base/container.h"
#include "air/base/st.h"
#include "air/driver/driver_ctx.h"
#include "config.h"

using namespace air::base;

namespace nn {
namespace llama {

#define DECL_CONFIG_FIELD(type)                                              \
  DEFINE_FIELD(type, _dim, "transformer dimension")                          \
  DEFINE_FIELD(type, _hidden_dim, "for ffn layers")                          \
  DEFINE_FIELD(type, _n_layers, "number of layers")                          \
  DEFINE_FIELD(type, _n_heads, "number of query heads")                      \
  DEFINE_FIELD(                                                              \
      type, _n_kv_heads,                                                     \
      "number of key value heads(can be query heads because of multiquery)") \
  DEFINE_FIELD(type, _vocab_size,                                            \
               "vocabulary size, usually 256(byte - level)")                 \
  DEFINE_FIELD(type, _seq_len, "max sequence length")

typedef struct {
#define DEFINE_FIELD(type, name, comment) type name; /* comment */
  DECL_CONFIG_FIELD(int)
#undef DEFINE_FIELD
} CONFIG;

#define DECL_WEIGHT_FIELD(type)                                       \
  DEFINE_FIELD(type, _token_embedding_table,                          \
               "token embedding table (vocab_size, dim)")             \
  DEFINE_FIELD(type, _rms_att_weight, "rmsnorm weights (layer, dim)") \
  DEFINE_FIELD(type, _rms_ffn_weight, "ffn weights (layer, dim)")     \
  DEFINE_FIELD(type, _wq, "(layer, dim, dim)")                        \
  DEFINE_FIELD(type, _wk, "(layer, dim, dim)")                        \
  DEFINE_FIELD(type, _wv, "(layer, dim, dim)")                        \
  DEFINE_FIELD(type, _wo, "(layer, dim, dim)")                        \
  DEFINE_FIELD(type, _w1, "(layer, hidden_dim, dim)")                 \
  DEFINE_FIELD(type, _w2, "(layer, dim, hidden_dim)")                 \
  DEFINE_FIELD(type, _w3, "(layer, hidden_dim, dim)")                 \
  DEFINE_FIELD(type, _rms_final_weight, "(dim)")                      \
  DEFINE_FIELD(type, _freq_cis_real, "(seq_len, dim/2)")              \
  DEFINE_FIELD(type, _freq_cis_imag, "(seq_len, dim/2)")              \
  DEFINE_FIELD(type, _real_vec0, "(dim)")                             \
  DEFINE_FIELD(type, _real_vec1, "(dim)")                             \
  DEFINE_FIELD(type, _imag_vec0, "(dim)")                             \
  DEFINE_FIELD(type, _imag_vec1, "(dim)")                             \
  DEFINE_FIELD(type, _wcls,                                           \
               "classifier weights for the logits, on the last layer")

typedef float* FLOAT_PTR;
typedef struct {
#define DEFINE_FIELD(type, name, comment) type name; /* comment */
  DECL_WEIGHT_FIELD(FLOAT_PTR)
#undef DEFINE_FIELD
} TRANSFORMER_WEIGHTS;

typedef struct {
#define DEFINE_FIELD(type, name, comment) type name; /* comment */
  DECL_WEIGHT_FIELD(NODE_PTR)

#undef DEFINE_FIELD
} TRANSFORMER_WEIGHTS_CST;

typedef struct {
  CONFIG
  _config;  // the hyperparameters of the architecture (the blueprint)
  TRANSFORMER_WEIGHTS     _weights;      // the weights of the model
  TRANSFORMER_WEIGHTS_CST _weights_cst;  // the weights const of the model
  // some more state needed to properly clean up the memory mapping (sigh)
  int     _fd;         // file descriptor for memory mapping
  float*  _data;       // memory mapped data pointer
  ssize_t _file_size;  // size of the checkpoint file in bytes
} TRANSFORMER;

class LLAMA {
public:
  LLAMA(GLOB_SCOPE* glob) : _glob(glob) {
    _spos = _glob->Unknown_simple_spos();
  }
  ~LLAMA() {}
  void Create_entry_func();
#if 0
  ADDR_DATUM_PTR Create_rmsnorm(std::string output_name);
  ADDR_DATUM_PTR Create_matmul(ADDR_DATUM_PTR input, std::string weight_input,
                               std::string output_name);
  ADDR_DATUM_PTR Create_matmul(ADDR_DATUM_PTR input_var_1,
                               ADDR_DATUM_PTR input_var_2,
                               std::string    output_name);
  ADDR_DATUM_PTR Create_matmul(NODE_PTR input_1, NODE_PTR input_2,
                               TYPE_PTR array_type, std::string output_name);
  std::pair<ADDR_DATUM_PTR, ADDR_DATUM_PTR> Create_rope_rotary(
      ADDR_DATUM_PTR input_1, ADDR_DATUM_PTR input_2, std::string weight_input,
      std::string output);
  std::pair<ADDR_DATUM_PTR, ADDR_DATUM_PTR> Create_kv_cache(
      ADDR_DATUM_PTR input_1, ADDR_DATUM_PTR input_2, ADDR_DATUM_PTR input_3,
      std::string start_pos, std::string output);
  FIELD_PTR    Get_fld(ADDR_DATUM_PTR var, uint32_t fld_id);
  CONSTANT_PTR Read_value_from_file(std::string file_name);
  std::pair<ADDR_DATUM_PTR, ADDR_DATUM_PTR> Create_record_return(
      NODE_PTR nn_node, TYPE_PTR array_type, std::string output_name);
  ADDR_DATUM_PTR Create_repeat_kv(ADDR_DATUM_PTR, int32_t times,
                                  std::string output_name);
  ADDR_DATUM_PTR Create_transpose(ADDR_DATUM_PTR input, int32_t val_1,
                                  int32_t val_2, std::string result);
  ADDR_DATUM_PTR Create_sqrt(std::string weight_input, std::string result);
  ADDR_DATUM_PTR Create_divide(ADDR_DATUM_PTR input_1, ADDR_DATUM_PTR input_2,
                               std::string result);
  ADDR_DATUM_PTR Create_softmax(ADDR_DATUM_PTR input, std::string result);
#endif
  void     Create_return(ADDR_DATUM_PTR input);
  void     Memory_map_weights(TRANSFORMER_WEIGHTS* w, CONFIG* p, float* ptr,
                              int shared_weights);
  void     Read_checkpoint(const char* checkpoint);
  void     Create_weights_const();
  NODE_PTR Create_weight_cst(std::vector<int64_t> shape, float* data);
  void     Create_pydsl_kernel();

private:
  LLAMA(void);                          // REQUIRED UNDEFINED UNWANTED methods
  LLAMA(const LLAMA&);                  // REQUIRED UNDEFINED UNWANTED methods
  LLAMA&      operator=(const LLAMA&);  // REQUIRED UNDEFINED UNWANTED methods
  GLOB_SCOPE* _glob;
  FUNC_SCOPE* _func_scope;
  SPOS        _spos;
  TRANSFORMER _transformer;
  FILE_PTR    _fbin;
  float*      _orig_data;
};

GLOB_SCOPE* Llama_driver(GLOB_SCOPE*                    glob,
                         const air::driver::DRIVER_CTX* driver_ctx,
                         const nn::llama::LLAMA_CONFIG& cfg, const char* ifile);

}  // namespace llama
}  // namespace nn

#endif  // NN_LLAMA_LLAMA_H
