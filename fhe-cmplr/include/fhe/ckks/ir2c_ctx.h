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
#include "fhe/ckks/ckks_opcode.h"
#include "fhe/core/ir2c_ctx.h"
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
      if (cfg.Ct_encode()) {
        // prepare encode context
        const core::CTX_PARAM& param = lower_ctx.Get_ctx_param();
        Prepare_encode_context(
            param.Get_poly_degree(), param.Get_security_level(),
            param.Get_mul_level(), param.Get_first_prime_bit_num(),
            param.Get_scaling_factor_bit_num());
      }

      // create rt_data_writer
      _data_file_uuid = "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX";
      _data_entry_type =
          cfg.Ct_encode() ? fhe::core::DE_PLAINTEXT : fhe::core::DE_MSG_F32;
      _rt_data_writer =
          new fhe::core::RT_DATA_WRITER(cfg.Data_file(), _data_entry_type,
                                        cfg.Ifile(), _data_file_uuid.c_str());
    }
  }

  //! @brief Destruct ir2c ctx object
  ~IR2C_CTX() {
    if (_rt_data_writer != nullptr) {
      delete _rt_data_writer;
      if (Ct_encode()) {
        Finalize_encode_context();
      }
    }
  }

  template <typename RETV, typename VISITOR>
  void Emit_encode(VISITOR* visitor, air::base::NODE_PTR dest,
                   air::base::NODE_PTR node) {
    bool            prec_plain = false;
    const uint32_t* prec       = node->Attr<uint32_t>(
        Lower_ctx().Attr_name(fhe::core::FHE_ATTR_KIND::PRECOMPUTE));
    if (prec != nullptr && *prec != 0) {
      prec_plain = true;
    }
    if (_rt_data_writer != nullptr && !_rt_data_writer->Is_full() &&
        (node->Child(0)->Opcode() == air::core::OPC_LDC ||
         node->Child(0)->Opcode() == air::core::OPC_LDCA) &&
        node->Child(1)->Opcode() == air::core::OPC_INTCONST) {
      air::base::CONSTANT_PTR cst = node->Child(0)->Const();
      const float*            data;
      float                   val;
      uint64_t                count;
      if (cst->Kind() == air::base::CONSTANT_KIND::ARRAY) {
        AIR_ASSERT(cst->Kind() == air::base::CONSTANT_KIND::ARRAY);
        AIR_ASSERT(cst->Type()->Is_array());
        AIR_ASSERT(cst->Type()->Cast_to_arr()->Elem_type()->Is_prim());
        AIR_ASSERT(cst->Type()
                       ->Cast_to_arr()
                       ->Elem_type()
                       ->Cast_to_prim()
                       ->Encoding() == air::base::PRIMITIVE_TYPE::FLOAT_32);
        data  = (const float*)cst->Array_buffer();
        count = cst->Array_byte_len() / sizeof(float);
      } else {
        AIR_ASSERT(cst->Type()->Cast_to_prim()->Encoding() ==
                       air::base::PRIMITIVE_TYPE::FLOAT_32 ||
                   cst->Type()->Cast_to_prim()->Encoding() ==
                       air::base::PRIMITIVE_TYPE::FLOAT_64);
        val   = cst->Float_literal().Val_as_float();
        data  = &val;
        count = 1;
      }
      char name[32];
      snprintf(name, 32, "cst_%d", cst->Id().Value());
      AIR_ASSERT(count >= node->Child(1)->Intconst());

      if (Ct_encode()) {
        air::base::NODE_PTR n_scale = node->Child(2);
        air::base::NODE_PTR n_level = node->Child(3);
        air::base::NODE_PTR n_ext   = node->Child(4);
        if (n_scale->Opcode() == air::core::OPC_INTCONST &&
            n_level->Opcode() == air::core::OPC_INTCONST &&
            n_ext->Opcode() == air::core::OPC_INTCONST) {
          if (Exceed_file_limit(node, 1)) {
            this->template Emit_runtime_encode<RETV, VISITOR>(visitor, dest,
                                                              node);
            _rt_data_writer->Set_is_full(true);
            return;
          }
          PLAINTEXT_BUFFER* buf = Encode_buffer(node, data, count);
          uint64_t idx = _rt_data_writer->Append_pt(name, (const char*)buf,
                                                    Plain_buffer_length(buf));

          if (Ct_prec() && prec_plain) {
            PLAINTEXT_BUFFER* buf_inv = Plain_prec(buf);
            uint64_t          idx2    = _rt_data_writer->Append_pt(
                name, (const char*)buf_inv,
                sizeof(PLAINTEXT_BUFFER) + buf_inv->_size);
            Free_plain_buffer(buf_inv);
          }
          Free_plain_buffer(buf);
          // dest = Pt_get_validate(cst, index, len, scale, level)
          // dest = Pt_get(index, len, scale, level)
          Emit_st_var<RETV, VISITOR>(visitor, dest);
          if (Rt_validate()) {
            _os << " = *(PLAIN)Pt_get_validate(";
            visitor->template Visit<RETV>(node->Child(0));  // buffer address
            _os << ", ";
          } else {
            _os << " = *(PLAIN)Pt_get(";
          }
          _os << idx << " /* " << name << " */";
        } else {
          this->template Emit_runtime_encode<RETV, VISITOR>(visitor, dest,
                                                            node);
          return;
        }
      } else {
        uint64_t idx = _rt_data_writer->Append(name, data, count);
        // Pt_from_msg_validate(&dest, cst, index, len, scale, level)
        // Pt_from_msg(&dest, index, len, scale, level)
        if (Rt_validate()) {
          _os << "Pt_from_msg_validate(&";
          Emit_st_var<RETV, VISITOR>(visitor, dest);
          _os << ", ";
          visitor->template Visit<RETV>(node->Child(0));  // buffer address
        } else {
          _os << "Pt_from_msg(&";
          Emit_st_var<RETV, VISITOR>(visitor, dest);
        }
        _os << ", " << idx << " /* " << name << " */";
      }
    } else if (_rt_data_writer != nullptr && !_rt_data_writer->Is_full() &&
               node->Child(0)->Opcode() == nn::vector::OPC_SLICE &&
               node->Child(1)->Opcode() == air::core::OPC_INTCONST) {
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
      // Pt.get does not support non-continous data read
      // set writer full to tell do not emit anything for precompute emit
      // this is a temp solution, the right way would be setting up the
      // communication of encode, and encode_precompute
      bool is_continuous =
          this->template Continuous_encode<VISITOR>(visitor, node);
      if (Exceed_file_limit(node, loop_cnt) || !is_continuous) {
        this->template Emit_runtime_encode<RETV, VISITOR>(visitor, dest, node);
        _rt_data_writer->Set_is_full(true);
        return;
      } else {
        _rt_data_writer->Set_is_full(false);
      }
      const float* data        = (const float*)cst->Array_buffer();
      uint64_t     total_count = cst->Array_byte_len() / sizeof(float);
      uint64_t     span        = slice->Child(2)->Intconst();
      uint64_t     count       = node->Child(1)->Intconst();

      for (uint64_t i = 0; i < loop_cnt; ++i) {
        AIR_ASSERT(total_count >= i * span + count);
        char name[32];
        snprintf(name, 32, "cst_%d_%d", cst->Id().Value(), (int)i);
        if (Ct_encode()) {
          PLAINTEXT_BUFFER* buf = Encode_buffer(node, data + i * span, count);
          uint64_t          idx = _rt_data_writer->Append_pt(
              name, (const char*)buf, sizeof(PLAINTEXT_BUFFER) + buf->_size);

          if (Ct_prec() && prec_plain) {
            PLAINTEXT_BUFFER* buf_inv = Plain_prec(buf);
            uint64_t          idx2    = _rt_data_writer->Append_pt(
                name, (const char*)buf_inv,
                sizeof(PLAINTEXT_BUFFER) + buf_inv->_size);
            Free_plain_buffer(buf_inv);
          }
          Free_plain_buffer(buf);
          if (i == 0) {
            // dest = Pt_get_validate(cst, index, len, scale, level)
            // dest = Pt_get(index, len, scale, level)
            Emit_st_var<RETV, VISITOR>(visitor, dest);
            if (Rt_validate()) {
              _os << " = *(PLAIN)Pt_get_validate(";
              visitor->template Visit<RETV>(node->Child(0));  // buffer address
              _os << ", ";
            } else {
              _os << " = *(PLAIN)Pt_get(";
            }
            if (Ct_prec() && prec_plain) {
              // when Ct_prec() enabled, two plaintext will be generated
              // loop index should multiply 2
              _os << "(2 * ";
              visitor->template Visit<RETV>(start);
              _os << ") + " << idx << " /* " << name << " */";
            } else {
              visitor->template Visit<RETV>(start);
              _os << " + " << idx << " /* " << name << " */";
            }
          }
        } else {
          uint64_t idx = _rt_data_writer->Append(name, data + i * span, count);
          if (i == 0) {
            // Pt_from_msg(&dest, index, len, scale, level)
            // Pt_from_msg_validate(&dest, cst, index, len, scale, level)
            if (Rt_validate()) {
              _os << "Pt_from_msg_validate(&";
              Emit_st_var<RETV, VISITOR>(visitor, dest);
              _os << ", ";
              visitor->template Visit<RETV>(node->Child(0));  // buffer address
            } else {
              _os << "Pt_from_msg(&";
              Emit_st_var<RETV, VISITOR>(visitor, dest);
            }
            _os << ", ";
            visitor->template Visit<RETV>(start);
            _os << " + " << idx << " /* " << name << " */";
          }
        }
      }
    } else {
      // runtime encoding with internal data embedded in C code
      // Encode_plain_from_float(&dest, cst, len, scale, level, false);
      air::base::NODE_PTR cst      = node->Child(0);
      air::base::TYPE_PTR cst_type = cst->Rtype();
      air::base::TYPE_PTR domain_type;
      const double*       mask_attr = node->Attr<double>(nn::core::ATTR::MASK);
      bool                encoding_mask = (mask_attr != nullptr);
      if (cst_type->Is_ptr()) {
        domain_type = cst_type->Cast_to_ptr()->Domain_type();
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
      switch (domain_type->Cast_to_prim()->Encoding()) {
        case air::base::PRIMITIVE_TYPE::FLOAT_32:
          _os << (encoding_mask ? "Encode_plain_from_float_mask(&"
                                : "Encode_plain_from_float(&");
          break;
        case air::base::PRIMITIVE_TYPE::FLOAT_64:
          _os << (encoding_mask ? "Encode_plain_from_double_mask(&"
                                : "Encode_plain_from_double(&");
          break;
        default:
          AIR_ASSERT_MSG(false, "not supported primitive type");
      }

      Emit_st_var<RETV, VISITOR>(visitor, dest);
      _os << ", ";
      visitor->template Visit<RETV>(cst);  // buffer address

      _os << ", ";
      visitor->template Visit<RETV>(node->Child(1));  // element count
      _os << ", ";
      visitor->template Visit<RETV>(node->Child(2));  // scale
      _os << ", ";
      visitor->template Visit<RETV>(node->Child(3));  // level
      _os << ", ";
      visitor->template Visit<RETV>(node->Child(4));  // is_ext
      _os << ")";
      return;
    }
    _os << ", ";
    visitor->template Visit<RETV>(node->Child(1));  // element count
    _os << ", ";
    visitor->template Visit<RETV>(node->Child(2));  // scale
    _os << ", ";
    visitor->template Visit<RETV>(node->Child(3));  // level
    if (!Ct_encode()) {
      _os << ", ";
      visitor->template Visit<RETV>(node->Child(4));  // is_ext
    }
    _os << ")";
  }

  template <typename RETV, typename VISITOR>
  void Emit_runtime_encode(VISITOR* visitor, air::base::NODE_PTR dest,
                           air::base::NODE_PTR node) {
    // runtime encoding with internal data embedded in C code
    // Encode_plain_from_float(&dest, cst, len, scale, level, false);
    air::base::NODE_PTR cst      = node->Child(0);
    air::base::TYPE_PTR cst_type = cst->Rtype();
    air::base::TYPE_PTR domain_type;
    const double*       mask_attr = node->Attr<double>(nn::core::ATTR::MASK);
    bool                encoding_mask = (mask_attr != nullptr);
    if (cst_type->Is_ptr()) {
      domain_type = cst_type->Cast_to_ptr()->Domain_type();
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
    switch (domain_type->Cast_to_prim()->Encoding()) {
      case air::base::PRIMITIVE_TYPE::FLOAT_32:
        _os << (encoding_mask ? "Encode_plain_from_float_mask(&"
                              : "Encode_plain_from_float(&");
        break;
      case air::base::PRIMITIVE_TYPE::FLOAT_64:
        _os << (encoding_mask ? "Encode_plain_from_double_mask(&"
                              : "Encode_plain_from_double(&");
        break;
      default:
        AIR_ASSERT_MSG(false, "not supported primitive type");
    }

    Emit_st_var<RETV, VISITOR>(visitor, dest);
    _os << ", ";
    visitor->template Visit<RETV>(cst);  // buffer address

    _os << ", ";
    visitor->template Visit<RETV>(node->Child(1));  // element count
    _os << ", ";
    visitor->template Visit<RETV>(node->Child(2));  // scale
    _os << ", ";
    visitor->template Visit<RETV>(node->Child(3));  // level
    _os << ", ";
    visitor->template Visit<RETV>(node->Child(4));  // is_ext
    _os << ")";
  }

  PLAINTEXT_BUFFER* Encode_buffer(air::base::NODE_PTR node, const float* data,
                                  uint64_t count) {
    AIR_ASSERT(node->Opcode() == fhe::ckks::OPC_ENCODE);
    PLAINTEXT_BUFFER*   buf     = nullptr;
    air::base::NODE_PTR n_scale = node->Child(2);
    air::base::NODE_PTR n_level = node->Child(3);
    air::base::NODE_PTR n_ext   = node->Child(4);
    if (n_scale->Opcode() == air::core::OPC_INTCONST &&
        n_level->Opcode() == air::core::OPC_INTCONST &&
        n_ext->Opcode() == air::core::OPC_INTCONST) {
      uint64_t sc     = n_scale->Intconst();
      uint64_t lv     = n_level->Intconst();
      uint64_t is_ext = n_ext->Intconst();
      AIR_ASSERT(is_ext == 0 || is_ext == 1);
      buf = Encode_plain_buffer(data, count, sc, lv, is_ext);
    } else {
      // TODO: fix scale and level
      uint64_t is_ext = n_ext->Intconst();
      buf             = Encode_plain_buffer(data, count, 1, 3, is_ext);
      AIR_ASSERT_MSG(false, "unhandled encode");
    }
    return buf;
  }

  const char* Data_file_uuid() const { return _data_file_uuid.c_str(); }

  core::DATA_ENTRY_TYPE Data_entry_type() const { return _data_entry_type; }

public:
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
      AIR_ASSERT(node->Child(0)->Opcode() == air::core::OPC_LD);
      AIR_ASSERT(node->Child(1)->Opcode() == air::core::OPC_MUL);
      Parse_subscript_expr(node->Child(0), subscript);
      return Parse_subscript_expr(node->Child(1), subscript);
    } else if (node->Opcode() == air::core::OPC_MUL) {
      AIR_ASSERT(node->Child(1)->Opcode() == air::core::OPC_INTCONST);
      AIR_ASSERT(subscript.size() > 0);
      AIR_ASSERT(subscript.back().second == -1);
      subscript.back().second = node->Child(1)->Intconst();
      return Parse_subscript_expr(node->Child(0), subscript);
    }
    return false;
  }

  // check if encoding plaintext will exceed file limit
  bool Exceed_file_limit(air::base::NODE_PTR node, uint32_t loop_cnt) {
    // if no limit return false
    if (Df_limit() == 0) return false;

    air::base::NODE_PTR n_level = node->Child(3);
    air::base::NODE_PTR n_ext   = node->Child(4);

    AIR_ASSERT(n_level->Opcode() == air::core::OPC_INTCONST &&
               n_ext->Opcode() == air::core::OPC_INTCONST);
    uint64_t lv     = n_level->Intconst();
    uint64_t is_ext = n_ext->Intconst();
    size_t   pt_len =
        Get_plaintext_length(lv, is_ext) + sizeof(struct PLAINTEXT_BUFFER);

    // double the size if precompute plaintext is enabled
    if (Ct_prec()) pt_len *= 2;
    if (pt_len * loop_cnt + _rt_data_writer->Cur_ofst() >
        Df_limit() * 1024 * 1024) {
      return true;
    }
    return false;
  }

  template <typename VISITOR>
  bool Continuous_encode(VISITOR* visitor, air::base::NODE_PTR node) {
    uint32_t            i    = 1;
    air::base::NODE_PTR loop = visitor->Parent_loop(i);
    while (loop != air::base::Null_ptr) {
      AIR_ASSERT(loop->Opcode() == air::core::DO_LOOP);
      air::base::NODE_PTR body = loop->Body_blk();
      for (air::base::STMT_PTR stmt       = body->Begin_stmt();
           stmt != body->End_stmt(); stmt = stmt->Next()) {
        air::base::NODE_PTR s_node = stmt->Node();
        for (uint32_t j = 0; j < s_node->Num_child(); ++j) {
          air::base::NODE_PTR n_child = s_node->Child(j);
          AIR_ASSERT(n_child != air::base::Null_ptr);
          if (n_child->Opcode() == fhe::ckks::OPC_ENCODE) {
            return false;
          }
        }
      }
      i++;
      loop = visitor->Parent_loop(i);
    }
    return true;
  }

  fhe::core::RT_DATA_WRITER* _rt_data_writer;
  std::string                _data_file_uuid;
  fhe::core::DATA_ENTRY_TYPE _data_entry_type;
};  // IR2C_CTX

}  // namespace ckks

}  // namespace fhe

#endif  // FHE_CKKS_IR2C_CTX_H
