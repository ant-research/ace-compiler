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
#include "fhe/core/rt_data_writer.h"
#include "fhe/core/rt_encode_api.h"
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
      // prepare encode context
      const core::CTX_PARAM& param = lower_ctx.Get_ctx_param();
      Prepare_encode_context(param.Get_poly_degree(),
                             param.Get_security_level(), param.Get_mul_level(),
                             param.Get_first_prime_bit_num(),
                             param.Get_scaling_factor_bit_num());

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
      Finalize_encode_context();
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
      if (Ct_encode()) {
        // TODO: fix scale and level
        uint64_t          sc  = (_rt_data_writer->Cur_idx() == 1024) ? 2 : 1;
        uint64_t          lv  = (_rt_data_writer->Cur_idx() == 1024) ? 3 : 2;
        PLAINTEXT_BUFFER* buf = Encode_plain_buffer(data, count, sc, lv);
        uint64_t idx = _rt_data_writer->Append_pt(name, (const char*)buf,
                                                  Plain_buffer_length(buf));
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
    } else if (_rt_data_writer != nullptr &&
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
        // Parent(1):block, (2):loop, (3):block, (4):loop, ...
        air::base::NODE_PTR loop = visitor->Parent(i * 2 + 2);
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
        if (Ct_encode()) {
          // TODO: fix scale and level
          PLAINTEXT_BUFFER* buf =
              Encode_plain_buffer(data + i * span, count, 1, 3);
          uint64_t idx = _rt_data_writer->Append_pt(
              name, (const char*)buf, sizeof(PLAINTEXT_BUFFER) + buf->_size);
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
            visitor->template Visit<RETV>(start);
            _os << " + " << idx << " /* " << name << " */";
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
      // Encode_plain_from_float(&dest, cst, len, scale, level);
      air::base::NODE_PTR cst      = node->Child(0);
      air::base::TYPE_PTR cst_type = cst->Rtype();
      air::base::TYPE_PTR domain_type;
      if (cst_type->Is_ptr()) {
        domain_type = cst_type->Cast_to_ptr()->Domain_type();
      } else {
        AIR_ASSERT(cst_type->Is_array());
        domain_type = cst_type->Cast_to_arr()->Elem_type();
      }
      AIR_ASSERT(domain_type->Is_prim());
      switch (domain_type->Cast_to_prim()->Encoding()) {
        case air::base::PRIMITIVE_TYPE::FLOAT_32:
          _os << "Encode_plain_from_float(&";
          break;
        case air::base::PRIMITIVE_TYPE::FLOAT_64:
          _os << "Encode_plain_from_double(&";
          break;
        default:
          AIR_ASSERT_MSG(false, "not supported primitive type");
      }

      Emit_st_var<RETV, VISITOR>(visitor, dest);
      _os << ", ";
      visitor->template Visit<RETV>(cst);  // buffer address
    }
    _os << ", ";
    visitor->template Visit<RETV>(node->Child(1));  // element count
    _os << ", ";
    visitor->template Visit<RETV>(node->Child(2));  // scale
    _os << ", ";
    visitor->template Visit<RETV>(node->Child(3));  // level
    _os << ")";
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

  fhe::core::RT_DATA_WRITER* _rt_data_writer;
  std::string                _data_file_uuid;
  fhe::core::DATA_ENTRY_TYPE _data_entry_type;
};  // IR2C_CTX

}  // namespace ckks

}  // namespace fhe

#endif  // FHE_CKKS_IR2C_CTX_H
