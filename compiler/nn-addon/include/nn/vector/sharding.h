//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_VECTOR_SHARDING_H
#define NN_VECTOR_SHARDING_H

#include <algorithm>
#include <cmath>
#include <map>
#include <vector>

#include "air/base/container.h"
#include "air/base/container_decl.h"
#include "air/base/st.h"
#include "air/core/opcode.h"
#include "air/util/debug.h"
#include "nn/core/attr.h"
#include "nn/core/opcode.h"
#include "nn/vector/sym_map.h"
#include "nn/vector/vector_utils.h"

namespace nn {
namespace vector {

using namespace air::base;

// 3D axis for computation mesh [x, y, z]
typedef struct {
  int64_t X() const { return _x; }
  int64_t Y() const { return _y; }
  int64_t Z() const { return _z; }
  int64_t Size() const {
    int64_t s = _x * _y * _z;
    AIR_ASSERT(s != 0);
    return s;
  }
  void Print(std::ostream& os) const {
    os << "mesh[" << _x << ", " << _y << ", " << _z << "]" << std::endl;
  }

  int64_t _x;
  int64_t _y;
  int64_t _z;
} DIM3;

// DataSharding = partition+mesh
// which can derive the array distribution in each mesh device.
class ARRAY_SHARDING {
public:
  ARRAY_SHARDING() : _reshard(0), _halosize(0) {}
  ARRAY_SHARDING(DIM3 m) : _mesh(m), _reshard(0), _halosize(0) {}
  ~ARRAY_SHARDING() = default;

  void                 Set_spec(std::vector<int64_t> ss) { _spec = ss; }
  std::vector<int64_t> Spec() const { return _spec; }
  void                 Set_reshard(int t) { _reshard = t; }
  int                  Reshard() { return _reshard; }
  int64_t              Halosize() const { return _halosize; }
  void                 Set_halosize(int64_t h) { _halosize = h; }

  void Print(std::ostream& os) const {
    int dim = _spec.size();
    os << std::dec << "Partition: [";
    for (int i = 0; i < dim - 1; i++) os << _spec[i] << ", ";
    os << _spec[dim - 1] << "] ";
    if (_halosize > 0) os << "halosize=" << _halosize;
    os << " @ ";
    _mesh.Print(os);
  }

  friend std::ostream& operator<<(std::ostream& os, const ARRAY_SHARDING& sh) {
    sh.Print(os);
    return os;
  }

private:
  DIM3                 _mesh;
  std::vector<int64_t> _spec;
  int                  _reshard;
  int64_t              _halosize;
};

// ComputationSharding/OpSharding
// mesh and input/output partition
class OP_SHARDING {
public:
  OP_SHARDING() {}
  OP_SHARDING(DIM3 m) : _mesh(m) {}
  ~OP_SHARDING() = default;

  using IOMAP = std::map<int, ARRAY_SHARDING>;

  // op imap
  void Set_imap(int i, ARRAY_SHARDING sharding) {
    AIR_ASSERT_MSG(_imap.find(i) == _imap.end(), "imap i is set.");
    _imap[i] = sharding;
  }

  ARRAY_SHARDING Imap(int i) {
    IOMAP::iterator iter = _imap.find(i);
    AIR_ASSERT_MSG(iter != _imap.end(), "Cannot find imap");
    return iter->second;
  }

  // op omap
  void Set_omap(int i, ARRAY_SHARDING sharding) {
    AIR_ASSERT_MSG(_omap.find(i) == _omap.end(), "omap i is set.");
    _omap[i] = sharding;
  }

  ARRAY_SHARDING Omap(int i) {
    IOMAP::iterator iter = _omap.find(i);
    AIR_ASSERT_MSG(iter != _omap.end(), "Cannot find omap");
    return iter->second;
  }

  DIM3 Mesh() { return _mesh; }

  void Set_mesh(DIM3 m) { _mesh = m; }

  void Print(std::ostream& os) const {
    os << std::dec << "OP_SHARDING @ ";
    _mesh.Print(os);
    os << "imap:" << std::endl;
    for (IOMAP::const_iterator it = _imap.begin(); it != _imap.end(); ++it) {
      os << "  " << it->first << ": " << it->second;
    }
    os << "omap:" << std::endl;
    for (IOMAP::const_iterator it = _omap.begin(); it != _omap.end(); ++it) {
      os << "  " << it->first << ": " << it->second;
    }
  }

private:
  DIM3 _mesh;
  // How input is sharded
  IOMAP _imap;
  // How output is sharded
  IOMAP _omap;
};

class SHARDING_MAP {
public:
  SHARDING_MAP() {}
  SHARDING_MAP(CONTAINER* cntr)
      : _cntr(cntr),
        _fscope(cntr->Parent_func_scope()),
        _gscope(cntr->Glob_scope()) {}
  ~SHARDING_MAP() = default;

  void Set_cntr(CONTAINER* cntr) {
    _cntr   = cntr;
    _fscope = cntr->Parent_func_scope();
    _gscope = cntr->Glob_scope();
  }

  // record all data sharding.
  using DATA_SHARDING = std::vector<CONST_ADDR_DATUM_PTR>;
  using SHMAP_NODE    = std::map<FUNC_ID, std::map<NODE_ID, OP_SHARDING>>;
  // TODO: change to unordered<uint64_t, ARRAY_SHARDING>
  using SHMAP_PREG = std::map<FUNC_ID, std::map<PREG_ID, ARRAY_SHARDING>>;
  using SHMAP_DATA = std::map<FUNC_ID, std::map<ADDR_DATUM_ID, ARRAY_SHARDING>>;

  void Print(std::ostream& os) const;

  // op(node) -> OP_SHARDING
  OP_SHARDING Get_op_sharding(NODE_PTR op) const;
  void        Set_op_sharding(NODE_PTR op, const OP_SHARDING& vec);
  bool        Is_op_sharding(NODE_PTR op) const;

  // Just for formal data: data -> ARRAY_SHARDING
  // Why not recording for all data? We record in old-new mapping.
  const ARRAY_SHARDING* Get_data_sharding(FUNC_ID fid, PREG_ID did) const;
  const ARRAY_SHARDING* Get_data_sharding(FUNC_ID fid, ADDR_DATUM_ID did) const;
  void Set_data_sharding(FUNC_ID fid, PREG_ID did, const ARRAY_SHARDING& sh);
  void Set_data_sharding(FUNC_ID fid, ADDR_DATUM_ID did,
                         const ARRAY_SHARDING& sh);

  // All new DataSharding symbols.
  void Add_data_sharding(ADDR_DATUM_PTR d) { _data_shardings.emplace_back(d); }

  bool Is_data_sharding(ADDR_DATUM_PTR d) {
    if (std::find(_data_shardings.begin(), _data_shardings.end(), d) !=
        _data_shardings.end())
      return true;
    else
      return false;
  }

  // datum-datum: datum -> sharding tensor
  void Set_shard_datum(ADDR_DATUM_PTR old_sym, ADDR_DATUM_PTR new_sym) {
    FUNC_SCOPE* fscope = _cntr->Parent_func_scope();
    Set_map_datum(old_sym, new_sym, _datum_map);
  }

  ADDR_DATUM_PTR Get_shard_datum(ADDR_DATUM_PTR old_sym) {
    FUNC_SCOPE*    fscope  = _cntr->Parent_func_scope();
    ADDR_DATUM_PTR new_sym = Get_map_datum(old_sym, fscope, _datum_map);
    return new_sym;
  }

  // No "preg-> sharding_preg" mapping now.
  void Set_shard_preg(PREG_PTR old_sym, PREG_PTR new_sym) {
    FUNC_SCOPE* fscope = _cntr->Parent_func_scope();
    Set_map_preg(old_sym, new_sym, _preg_map);
  }

  PREG_PTR Get_shard_preg(PREG_PTR old_sym) {
    FUNC_SCOPE* fscope  = _cntr->Parent_func_scope();
    PREG_PTR    new_sym = Get_map_preg(old_sym, fscope, _preg_map);
    return new_sym;
  }

  // preg-datum:  preg -> sharding tensor
  bool Is_sharded(PREG_PTR old_sym) {
    return Is_preg_in_map(old_sym, _preg_datum_map);
  }

  void Set_shard_preg_datum(PREG_PTR old_sym, ADDR_DATUM_PTR new_sym) {
    FUNC_SCOPE* fscope = _cntr->Parent_func_scope();
    Set_map_preg_datum(old_sym, new_sym, _preg_datum_map);
  }

  ADDR_DATUM_PTR Get_shard_preg_datum(PREG_PTR old_sym) {
    FUNC_SCOPE*    fscope = _cntr->Parent_func_scope();
    ADDR_DATUM_PTR new_sym =
        Get_map_preg_datum(old_sym, fscope, _preg_datum_map);
    return new_sym;
  }

  TYPE_PTR New_sharding_type(const std::string& ty_name,
                             uint32_t ty_name_suffix, int64_t num,
                             CONST_TYPE_PTR block_type, const SPOS& spos);
  void     Print_sharding_type(TYPE_PTR shard_type);

  CONSTANT_PTR Broadcast_bias(CONST_CONSTANT_PTR cst, int64_t channel_out,
                              int64_t height, int64_t width, const SPOS& spos);
  CONSTANT_PTR Split_array_const(CONST_CONSTANT_PTR cst, int num_chunks,
                                 std::vector<int64_t> shape,
                                 CONST_TYPE_PTR etype, const SPOS& spos);
  std::vector<int64_t> Analyze_conv(int64_t maxsize, int64_t channel_out,
                                    int64_t channel_in, int64_t height,
                                    int64_t width);

private:
  MAP_DATUM      _datum_map;
  MAP_PREG       _preg_map;
  MAP_PREG_DATUM _preg_datum_map;
  // <FUNC_ID, NODE_ID> -> strategy for each operation
  SHMAP_NODE    _smap_node;
  SHMAP_PREG    _smap_preg;
  SHMAP_DATA    _smap_data;
  CONTAINER*    _cntr;
  FUNC_SCOPE*   _fscope;
  GLOB_SCOPE*   _gscope;
  DATA_SHARDING _data_shardings;
};

// Simple decision by H dim. Never handle H+W
void Compute_conv_halo(ARRAY_SHARDING& sh, int64_t kh, int64_t kw);

ADDR_DATUM_PTR New_sharding_var(CONTAINER* cntr, const std::string& name,
                                uint32_t suffix, int64_t num_blocks,
                                std::vector<int64_t> block_shape,
                                CONST_TYPE_PTR etype, const SPOS& spos);

// Ops for sharding
NODE_PTR New_conv_node(CONTAINER* cntr, NODE_PTR input, NODE_PTR weight,
                       NODE_PTR bias, TYPE_PTR rtype, const SPOS& spos);
NODE_PTR New_gather_node(CONTAINER* cntr, NODE_PTR input0, NODE_PTR input1,
                         TYPE_PTR rtype, int dim, const SPOS& spos);
NODE_PTR New_halo_node(CONTAINER* cntr, NODE_PTR op0,
                       std::vector<int64_t> start_indices,
                       std::vector<int64_t> slice_size,
                       std::vector<int64_t> stride_size, int channel,
                       const SPOS& spos);

// Loader and storer for sharding
NODE_PTR New_sharding_loader(CONTAINER* cntr, CONST_ADDR_DATUM_PTR svar,
                             NODE_PTR i, const SPOS& spos);
NODE_PTR New_sharding_loader(CONTAINER* cntr, CONST_CONSTANT_PTR cvar,
                             NODE_PTR i, const SPOS& spos);
STMT_PTR New_sharding_store(CONTAINER* cntr, CONST_ADDR_DATUM_PTR svar,
                            NODE_PTR i, NODE_PTR rhs, const SPOS& spos);
STMT_PTR New_sharding_update_store(CONTAINER* cntr, CONST_ADDR_DATUM_PTR svar,
                                   NODE_PTR i, NODE_PTR adder,
                                   const SPOS& spos);

STMT_LIST Get_loop_sl(STMT_PTR loop);
STMT_PTR  New_conv_sharding_body(
     CONTAINER* cntr, NODE_PTR node, NODE_PTR new_input,
     CONSTANT_PTR weight_sharding_const, CONSTANT_PTR bias_sharding_const,
     ADDR_DATUM_PTR output_sharding_var, NODE_PTR xiv, NODE_PTR yiv,
     std::vector<int64_t> mesh, TYPE_PTR output_block_type, const SPOS& spos);

STMT_PTR New_conv_sharding_body_spatial(
    CONTAINER* cntr, NODE_PTR node, NODE_PTR new_input,
    CONSTANT_PTR weight_sharding_const, CONSTANT_PTR bias_sharding_const,
    ADDR_DATUM_PTR output_sharding_var, NODE_PTR xiv, NODE_PTR yiv,
    NODE_PTR ziv, std::vector<int64_t> mesh, TYPE_PTR output_block_type,
    int64_t halosize, const SPOS& spos);

STMT_PTR New_add_sharding_body(CONTAINER* cntr, NODE_PTR node,
                               ADDR_DATUM_PTR input0_sharding_var,
                               ADDR_DATUM_PTR input1_sharding_var,
                               ADDR_DATUM_PTR output_sharding_var, NODE_PTR xiv,
                               NODE_PTR ziv, std::vector<int64_t> mesh,
                               TYPE_PTR output_block_type, const SPOS& spos);
STMT_PTR New_concat_sharding_body(CONTAINER* cntr, uint64_t ofst, NODE_PTR iv,
                                  ADDR_DATUM_PTR input_sharding_var,
                                  ADDR_DATUM_PTR output_sharding_var,
                                  const SPOS&    spos);
STMT_PTR New_average_pool_sharding_body(CONTAINER* cntr, NODE_PTR node,
                                        ADDR_DATUM_PTR input_sharding_var,
                                        ADDR_DATUM_PTR output_sharding_var,
                                        NODE_PTR yiv, NODE_PTR ziv,
                                        std::vector<int64_t> mesh,
                                        TYPE_PTR             output_block_type,
                                        int64_t halosize, const SPOS& spos);
STMT_PTR New_global_avgpool_sharding_body(
    CONTAINER* cntr, NODE_PTR node, ADDR_DATUM_PTR input_sharding_var,
    ADDR_DATUM_PTR output_sharding_var, NODE_PTR yiv, NODE_PTR ziv,
    std::vector<int64_t> mesh, TYPE_PTR output_block_type, const SPOS& spos);
STMT_PTR New_relu_sharding_body(CONTAINER* cntr, NODE_PTR node,
                                ADDR_DATUM_PTR input_sharding_var,
                                ADDR_DATUM_PTR output_sharding_var,
                                NODE_PTR xiv, NODE_PTR ziv,
                                std::vector<int64_t> mesh,
                                TYPE_PTR output_block_type, const SPOS& spos);

// a*b+c
NODE_PTR New_muladd_s32(CONTAINER* cntr, NODE_PTR a, NODE_PTR b, NODE_PTR c,
                        const SPOS& spos);

}  // namespace vector
}  // namespace nn

#endif
