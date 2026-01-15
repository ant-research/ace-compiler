//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CKKS_IR2C_CTX_H
#define FHE_CKKS_IR2C_CTX_H

#include "air/base/container_decl.h"
#include "air/base/st_decl.h"
#include "air/util/debug.h"
#include "fhe/core/ir2c_ctx.h"
#include "fhe/core/rt_context.h"
#include "fhe/core/rt_data_writer.h"
#include "fhe/core/rt_encode_api.h"
#include "nn/core/attr.h"
#include "nn/vector/vector_opcode.h"

namespace fhe {

namespace ckks {

//! @brief Context for CKKS IR to C in fhe-cmplr
class IR2C_CTX : public fhe::core::IR2C_CTX {
public:
  //! @brief Construct a new ir2c ctx object
  IR2C_CTX(std::ostream& os, const fhe::core::LOWER_CTX& lower_ctx,
           const fhe::poly::POLY2C_CONFIG& cfg)
      : fhe::core::IR2C_CTX(os, lower_ctx, cfg), _rt_data_writer(nullptr) {
    if (cfg.Emit_data_file()) {
      // create rt_data_writer
      _data_file_uuid  = "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX";
      _data_entry_type = fhe::core::DE_MSG_F32;
      _rt_data_writer =
          new fhe::core::RT_DATA_WRITER(cfg.Data_file(), _data_entry_type,
                                        cfg.Ifile(), _data_file_uuid.c_str());
    }
  }

  //! @brief Destruct ir2c ctx object
  ~IR2C_CTX() {
    if (_rt_data_writer != nullptr) {
      delete _rt_data_writer;
    }
  }

  template <typename RETV, typename VISITOR>
  void Emit_encode(VISITOR* visitor, air::base::NODE_PTR dest,
                   air::base::NODE_PTR node) {
    if (_rt_data_writer != nullptr &&
        node->Child(0)->Opcode() == air::core::OPC_LDC &&
        node->Child(1)->Opcode() == air::core::OPC_INTCONST) {
      air::base::CONSTANT_PTR cst = node->Child(0)->Const();
      AIR_ASSERT(cst->Kind() == air::base::CONSTANT_KIND::ARRAY);
      AIR_ASSERT(cst->Type()->Is_array());
      AIR_ASSERT(cst->Type()->Cast_to_arr()->Elem_type()->Is_prim());
      AIR_ASSERT(
          cst->Type()->Cast_to_arr()->Elem_type()->Cast_to_prim()->Encoding() ==
          air::base::PRIMITIVE_TYPE::FLOAT_32);
      char name[32];
      snprintf(name, 32, "cst_%d", cst->Id().Value());
      const float* data  = (const float*)cst->Array_buffer();
      uint64_t     count = cst->Array_byte_len() / sizeof(float);
      AIR_ASSERT(count >= node->Child(1)->Intconst());
      // get level & scale from node
      uint32_t sc  = node->Child(2)->Intconst();
      uint32_t lv  = node->Child(3)->Intconst();
      uint64_t idx = _rt_data_writer->Append(name, data, count, sc, lv);
      // Pt_from_msg_validate(&dest, cst, index, len, scale, level)
      // Pt_from_msg(&dest, index, len, scale, level)
      // TODO: offline encoding support validate?
      if (Rt_validate()) {
        _ir2c_util << "Pt_from_msg_validate(&";
        Emit_st_var<RETV, VISITOR>(visitor, dest);
        _ir2c_util << ", ";
        Emit_buffer_address<RETV, VISITOR>(visitor, node->Child(0));
      } else {
        Emit_st_var<RETV, VISITOR>(visitor, dest);
        _ir2c_util << " = *(PLAIN)Pt_from_msg(&";
        Emit_st_var<RETV, VISITOR>(visitor, dest);
      }
      _ir2c_util << ", " << idx << " /* " << name << " */";
    } else if (_rt_data_writer != nullptr &&
               node->Child(0)->Opcode() == nn::vector::OPC_SLICE &&
               node->Child(1)->Opcode() == air::core::OPC_INTCONST) {
#if 0
      air::base::NODE_PTR slice = node->Child(0);
      AIR_ASSERT(slice->Child(0)->Opcode() == air::core::OPC_LDC);
      air::base::CONSTANT_PTR cst = slice->Child(0)->Const();
      AIR_ASSERT(cst->Kind() == air::base::CONSTANT_KIND::ARRAY);
      AIR_ASSERT(cst->Type()->Is_array());
      AIR_ASSERT(cst->Type()->Cast_to_arr()->Elem_type()->Is_prim());
      AIR_ASSERT(
          cst->Type()->Cast_to_arr()->Elem_type()->Cast_to_prim()->Encoding() ==
          air::base::PRIMITIVE_TYPE::FLOAT_32);
      AIR_ASSERT(slice->Child(2)->Opcode() == air::core::OPC_INTCONST);
      AIR_ASSERT(node->Child(1)->Opcode() == air::core::OPC_INTCONST);
      AIR_ASSERT(slice->Child(2)->Intconst() == node->Child(1)->Intconst());

      char name[32];
      snprintf(name, 32, "cst_%d", cst->Id().Value());
      const float*    data  = (const float*)cst->Array_buffer();
      uint64_t        count = cst->Array_byte_len() / sizeof(float);
      const uint32_t* sc_attr =
          node->Attr<uint32_t>(fhe::core::FHE_ATTR_KIND::SCALE);
      AIR_ASSERT_MSG(sc_attr, "missing scale attribute");
      const uint32_t* lv_attr =
          node->Attr<uint32_t>(fhe::core::FHE_ATTR_KIND::LEVEL);
      AIR_ASSERT_MSG(lv_attr, "missing level attribute");
      uint64_t idx =
          _rt_data_writer->Append(name, data, count, *sc_attr, *lv_attr);
      // Pt_from_msg_ofst_validate(&dest, idx, start, len, scale, level)
      // Pt_from_msg_ofst(&dest, idx, start, len, scale, level)
      if (Rt_validate()) {
        _ir2c_util << "Pt_from_msg_ofst_validate(&";
        Emit_st_var<RETV, VISITOR>(visitor, dest);
        _ir2c_util << ", ";
        Emit_buffer_address<RETV, VISITOR>(visitor, node->Child(0));
      } else {
        Emit_st_var<RETV, VISITOR>(visitor, dest);
        _ir2c_util << " = *(PLAIN)Pt_from_msg_ofst(&";
        Emit_st_var<RETV, VISITOR>(visitor, dest);
      }
      _ir2c_util << ", " << idx << "/* " << name << " */, (";
      visitor->template Visit<RETV>(slice->Child(1));  // row_idx
      _ir2c_util << ") * (";
      visitor->template Visit<RETV>(slice->Child(2));  // col
      _ir2c_util << ")";
#endif
      // the code below doesn't work for complex array subscript because it's
      // very difficult to evaluate complex subscript expression with irregular
      // IV order.
      // disable it for future reference
#if 1
      air::base::NODE_PTR slice = node->Child(0);
      AIR_ASSERT(slice->Child(0)->Opcode() == air::core::OPC_LDC);
      air::base::CONSTANT_PTR cst = slice->Child(0)->Const();
      AIR_ASSERT(cst->Kind() == air::base::CONSTANT_KIND::ARRAY);
      AIR_ASSERT(cst->Type()->Is_array());
      AIR_ASSERT(cst->Type()->Cast_to_arr()->Elem_type()->Is_prim());
      AIR_ASSERT(
          cst->Type()->Cast_to_arr()->Elem_type()->Cast_to_prim()->Encoding() ==
          air::base::PRIMITIVE_TYPE::FLOAT_32);
      AIR_ASSERT(slice->Child(2)->Opcode() == air::core::OPC_INTCONST);
      air::base::NODE_PTR                       start = slice->Child(1);
      std::vector<std::pair<int64_t, int64_t> > subscript;
      bool ret = Parse_subscript_expr(start, subscript);
      AIR_ASSERT(ret == true && subscript.size() > 0);
      uint64_t loop_cnt = 1;
      for (uint64_t i = 0; i < subscript.size(); ++i) {
        air::base::NODE_PTR loop = visitor->Parent_loop(i);
        AIR_ASSERT(loop != air::base::Null_ptr &&
                   loop->Opcode() == air::core::DO_LOOP);
        int64_t lb, ub, stride;
        ret = Parse_do_loop(loop, lb, ub, stride);
        AIR_ASSERT(ret == true && lb == 0 && stride == 1);
        AIR_ASSERT(subscript[i].first == loop->Iv_id().Value());
        AIR_ASSERT(subscript[i].second == -1 || subscript[i].second == ub);
        loop_cnt *= ub;
      }
      const float* data        = (const float*)cst->Array_buffer();
      uint64_t     total_count = cst->Array_byte_len() / sizeof(float);
      uint64_t     span        = slice->Child(2)->Intconst();
      uint64_t     count       = node->Child(1)->Intconst();
      for (uint64_t i = 0; i < loop_cnt; ++i) {
        AIR_ASSERT(total_count >= i * span + count);
        char name[32];
        snprintf(name, 32, "cst_%d_%d", cst->Id().Value(), (int)i);
        // get level & scale from node
        uint32_t sc = node->Child(2)->Intconst();
        uint32_t lv = node->Child(3)->Intconst();
        uint64_t idx =
            _rt_data_writer->Append(name, data + i * span, count, sc, lv);
        if (i == 0) {
          // Pt_from_msg(&dest, index, len, scale, level)
          // Pt_from_msg_validate(&dest, cst, index, len, scale, level)
          // TODO: offline encoding support validate?
          if (Rt_validate()) {
            _ir2c_util << "Pt_from_msg_validate(&";
            Emit_st_var<RETV, VISITOR>(visitor, dest);
            _ir2c_util << ", ";
            Emit_buffer_address<RETV, VISITOR>(visitor, node->Child(0));
          } else {
            Emit_st_var<RETV, VISITOR>(visitor, dest);
            _ir2c_util << " = *(PLAIN)Pt_from_msg(&";
            Emit_st_var<RETV, VISITOR>(visitor, dest);
          }
          _ir2c_util << ", ";
          visitor->template Visit<RETV>(start);
          _ir2c_util << " + " << idx << " /* " << name << " */";
        }
      }
#endif
    } else {
      // runtime encoding with internal data embedded in C code
      // Encode_float(&dest, cst, len, scale, level);
      Emit_runtime_encode<RETV, VISITOR>(visitor, dest, node);
    }
    _ir2c_util << ", ";
    visitor->template Visit<RETV>(node->Child(1));  // element count
    _ir2c_util << ", ";
    visitor->template Visit<RETV>(node->Child(2));  // scale
    _ir2c_util << ", ";
    visitor->template Visit<RETV>(node->Child(3));  // level
    _ir2c_util << ")";
  }

  template <typename RETV, typename VISITOR>
  void Emit_runtime_encode(VISITOR* visitor, air::base::NODE_PTR dest,
                           air::base::NODE_PTR node) {
    air::base::NODE_PTR cst      = node->Child(0);
    air::base::TYPE_PTR cst_type = cst->Rtype();
    air::base::TYPE_PTR domain_type;
    const double*       mask_attr = node->Attr<double>(nn::core::ATTR::MASK);
    bool                encoding_mask = (mask_attr != nullptr);
    if (cst_type->Is_ptr()) {
      domain_type = cst_type->Cast_to_ptr()->Domain_type();
    } else if (cst_type->Is_prim()) {
      // Handle primitive types (e.g., INTCONST from Python DSL)
      domain_type = cst_type;
    } else {
      // in case of encoding mask, the constant is a single float value.
      if (encoding_mask) {
        domain_type = cst_type;
      } else {
        AIR_ASSERT(cst_type->Is_array());
        domain_type = cst_type->Cast_to_arr()->Elem_type();
      }
    }
    AIR_ASSERT(domain_type->Is_prim());
    air::base::NODE_PTR level_child = node->Child(3);
    switch (domain_type->Cast_to_prim()->Encoding()) {
      case air::base::PRIMITIVE_TYPE::FLOAT_32:
        if (this->Provider() == core::PROVIDER::SEAL &&
            level_child->Opcode() == air::core::OPC_INTCONST) {
          _ir2c_util << (encoding_mask ? "Encode_float_mask_cst_lvl(&"
                                       : "Encode_float_cst_lvl(&");
        } else {
          _ir2c_util << (encoding_mask ? "Encode_float_mask(&"
                                       : "Encode_float(&");
        }
        break;
      case air::base::PRIMITIVE_TYPE::FLOAT_64:
        if (this->Provider() == core::PROVIDER::SEAL &&
            level_child->Opcode() == air::core::OPC_INTCONST) {
          _ir2c_util << (encoding_mask ? "Encode_double_mask_cst_lvl(&"
                                       : "Encode_double_cst_lvl(&");
        } else {
          _ir2c_util << (encoding_mask ? "Encode_double_mask(&"
                                       : "Encode_double(&");
        }
        break;
      // Handle integer types from Python DSL - treat as double encoding
      case air::base::PRIMITIVE_TYPE::INT_S32:
      case air::base::PRIMITIVE_TYPE::INT_U32:
      case air::base::PRIMITIVE_TYPE::INT_S64:
      case air::base::PRIMITIVE_TYPE::INT_U64:
        if (this->Provider() == core::PROVIDER::SEAL &&
            level_child->Opcode() == air::core::OPC_INTCONST) {
          _ir2c_util << "Encode_double_cst_lvl(&";
        } else {
          _ir2c_util << "Encode_double(&";
        }
        break;
      // Handle complex types from Python DSL bootstrap (weight encodings)
      case air::base::PRIMITIVE_TYPE::COMPLEX_32:
      case air::base::PRIMITIVE_TYPE::COMPLEX_64:
        if (this->Provider() == core::PROVIDER::SEAL &&
            level_child->Opcode() == air::core::OPC_INTCONST) {
          _ir2c_util << "Encode_dcmplx_cst_lvl(&";
        } else {
          _ir2c_util << "Encode_dcmplx(&";
        }
        break;
      default:
        AIR_ASSERT_MSG(false, "not supported primitive type");
    }

    Emit_st_var<RETV, VISITOR>(visitor, dest);
    _ir2c_util << ", ";
    Emit_buffer_address<RETV, VISITOR>(visitor, cst);
  }

  template <typename RETV, typename VISITOR>
  void Emit_buffer_address(VISITOR* visitor, air::base::NODE_PTR node) {
    if (node->Opcode() == air::core::LDC && node->Rtype()->Is_array() &&
        visitor->Parent(0)->Opcode() == OPC_ENCODE) {
      air::base::CONSTANT_PTR cst = node->Const();
      AIR_ASSERT(cst->Kind() == air::base::CONSTANT_KIND::ARRAY);
      AIR_ASSERT(cst->Type()->Is_array());
      air::base::CONST_ARRAY_TYPE_PTR arr_ty = cst->Type()->Cast_to_arr();
      AIR_ASSERT(arr_ty->Elem_type()->Is_prim());
      uint32_t dims = arr_ty->Dim();
      if (dims == 1) {
        visitor->template Visit<RETV>(node);
      } else {
        _ir2c_util << "&";
        visitor->template Visit<RETV>(node);
        for (size_t i = 0; i < dims; i++) {
          _ir2c_util << "[0]";
        }
      }
    } else {
      visitor->template Visit<RETV>(node);
    }
  }

  const char* Data_file_uuid() const { return _data_file_uuid.c_str(); }

  core::DATA_ENTRY_TYPE Data_entry_type() const { return _data_entry_type; }

public:
  // return total stride from subscript array
  int64_t Stride(const std::vector<std::pair<int64_t, int64_t> >& subscript) {
    AIR_ASSERT(subscript.size() > 0);
    AIR_ASSERT(subscript.back().second == -1);
    int64_t stride = 1;
    for (int i = 0; i < subscript.size() - 1; ++i) {
      stride *= subscript[i].second;
    }
    AIR_ASSERT(stride >= 1);
    return stride;
  }

  // Parse do_loop children to get constant lb/ub/stride
  bool Parse_do_loop(air::base::NODE_PTR node, int64_t& lb, int64_t& ub,
                     int64_t& stride) {
    if (node->Opcode() != air::core::OPC_DO_LOOP) {
      return false;
    }
    air::base::ADDR_DATUM_ID iv = node->Iv_id();
    if (node->Loop_init()->Opcode() != air::core::OPC_INTCONST) {
      return false;
    }
    lb = node->Loop_init()->Intconst();
    if (node->Compare()->Opcode() != air::core::OPC_LT ||
        node->Compare()->Child(0)->Opcode() != air::core::OPC_LD ||
        node->Compare()->Child(1)->Opcode() != air::core::OPC_INTCONST) {
      return false;
    }
    AIR_ASSERT(node->Compare()->Child(0)->Addr_datum_id() == iv);
    ub = node->Compare()->Child(1)->Intconst();
    if (node->Loop_incr()->Opcode() != air::core::OPC_ADD ||
        node->Loop_incr()->Child(0)->Opcode() != air::core::OPC_LD ||
        node->Loop_incr()->Child(1)->Opcode() != air::core::OPC_INTCONST) {
      return false;
    }
    AIR_ASSERT(node->Loop_incr()->Child(0)->Addr_datum_id() == iv);
    stride = node->Loop_incr()->Child(1)->Intconst();
    return true;
  }

  // Parse compound expression with add/mul to get subscript info
  bool Parse_subscript_expr(
      air::base::NODE_PTR                        node,
      std::vector<std::pair<int64_t, int64_t> >& subscript) {
    if (node->Opcode() == air::core::OPC_LD) {
      subscript.emplace_back(std::make_pair(node->Addr_datum_id().Value(), -1));
      return true;
    } else if (node->Opcode() == air::core::OPC_ADD) {
      if (node->Child(0)->Opcode() == air::core::OPC_MUL) {
        if (node->Child(1)->Opcode() == air::core::OPC_LD) {
          // iv_0 * stride_1 + iv_1
          Parse_subscript_expr(node->Child(1), subscript);
          return Parse_subscript_expr(node->Child(0), subscript);
        }
        // (iv_0 * stride_1 + iv_1) * stride_2 + iv2
        //   --> iv_0 * stride_1 * stride_2 + iv_1 * stride_2 + iv2
        AIR_ASSERT(node->Child(1)->Opcode() == air::core::OPC_ADD);
        AIR_ASSERT(node->Child(0)->Child(1)->Opcode() ==
                   air::core::OPC_INTCONST);

        if (Parse_subscript_expr(node->Child(1), subscript) == false) {
          return false;
        }

        int64_t inner_stride = Stride(subscript);
        AIR_ASSERT(inner_stride != -1 &&
                   node->Child(0)->Child(1)->Intconst() % inner_stride == 0);
        subscript.back().second =
            node->Child(0)->Child(1)->Intconst() / inner_stride;

        if (Parse_subscript_expr(node->Child(0)->Child(0), subscript) ==
            false) {
          return false;
        }

        for (int i = 0; i < subscript.size(); ++i) {
          const air::base::FUNC_SCOPE*    fscope = node->Func_scope();
          const air::base::ADDR_DATUM_PTR datum =
              fscope->Addr_datum(air::base::ADDR_DATUM_ID(subscript[i].first));
          printf("FULL: %d: %s -> %ld\n", i, datum->Name()->Char_str(),
                 subscript[i].second);
        }

#if 0
        if (Parse_subscript_expr(node->Child(0)->Child(0), subscript) == false) {
          return false;
        }
        AIR_ASSERT(subscript.back().second == -1);

        for (int i = 0; i < subscript.size(); ++i) {
          const air::base::FUNC_SCOPE* fscope = node->Func_scope();
          const air::base::ADDR_DATUM_PTR datum = fscope->Addr_datum(air::base::ADDR_DATUM_ID(subscript[i].first));
          printf("OUTER: %d: %s -> %ld\n", i, datum->Name()->Char_str(), subscript[i].second);
        }

        std::vector<std::pair<int64_t, int64_t> > inner;
        if (Parse_subscript_expr(node->Child(1), inner) == false) {
          return false;
        }
        int64_t inner_stride = Stride(inner);
        AIR_ASSERT(inner_stride != -1 &&
                   node->Child(0)->Child(1)->Intconst() % inner_stride == 0);

        for (int i = 0; i < subscript.size(); ++i) {
          printf("INNER: %d: %ld -> %ld\n", i, subscript[i].first, subscript[i].second);
        }

        subscript.back().second = node->Child(0)->Child(1)->Intconst() / inner_stride;
        subscript.insert(subscript.begin(), inner.begin(), inner.end());

        for (int i = 0; i < subscript.size(); ++i) {
          const air::base::FUNC_SCOPE* fscope = node->Func_scope();
          const air::base::ADDR_DATUM_PTR datum = fscope->Addr_datum(air::base::ADDR_DATUM_ID(subscript[i].first));
          printf("FULL: %d: %s -> %ld\n", i, datum->Name()->Char_str(), subscript[i].second);
        }
#endif
        return true;

        // AIR_ASSERT(subscript.back().second != -1 &&
        //            node->Child(0)->Child(1)->Intconst() %
        //            subscript.back().second == 0);
      } else {
        // iv_1 + iv_0 * stride_1
        AIR_ASSERT(node->Child(0)->Opcode() == air::core::OPC_LD);
        AIR_ASSERT(node->Child(1)->Opcode() == air::core::OPC_MUL);
        Parse_subscript_expr(node->Child(0), subscript);
        return Parse_subscript_expr(node->Child(1), subscript);
      }
    } else if (node->Opcode() == air::core::OPC_MUL) {
      // iv * stride
      AIR_ASSERT(node->Child(1)->Opcode() == air::core::OPC_INTCONST);
      AIR_ASSERT(subscript.size() > 0);
      AIR_ASSERT(subscript.back().second == -1);
      subscript.back().second = node->Child(1)->Intconst();
      return Parse_subscript_expr(node->Child(0), subscript);
    }
    return false;
  }

  fhe::core::RT_DATA_WRITER* _rt_data_writer;
  std::string                _data_file_uuid;
  fhe::core::DATA_ENTRY_TYPE _data_entry_type;
  bool                       _need_bts = false;
};  // IR2C_CTX

}  // namespace ckks

}  // namespace fhe

#endif  // FHE_CKKS_IR2C_CTX_H
