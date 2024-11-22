//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_VECTOR_TENSOR_INSTR_H
#define NN_VECTOR_TENSOR_INSTR_H

#include "air/base/instrument_ctx.h"
#include "air/core/null_handler.h"
#include "nn/vector/config.h"

namespace nn {
namespace vector {

//! @brief Context for instrumentation to dump tensor value
class TENSOR_INSTR_CTX : public air::base::INSTRUMENT_CTX {
public:
  TENSOR_INSTR_CTX(const VECTOR_CONFIG& cfg) : _config(cfg) {}

  template <typename RETV, typename VISITOR>
  RETV Handle_node(VISITOR* visitor, air::base::NODE_PTR node) {
    AIR_ASSERT(node->Opcode() == air::core::OPC_ST ||
               node->Opcode() == air::core::OPC_STP);
    air::base::NODE_PTR val = node->Child(0);
    if (val->Domain() == nn::core::NN) {
      AIR_ASSERT(val->Rtype()->Is_array());
      AIR_ASSERT(node->Access_type()->Is_array());
      air::base::CONTAINER* cntr = node->Container();
      uint64_t len = node->Access_type()->Cast_to_arr()->Elem_count();
      int32_t  epi = Get_op_epi(val->Opcode());

      auto gen_ld = [](air::base::CONTAINER* cntr,
                       air::base::NODE_PTR   ptr) -> air::base::NODE_PTR {
        return (ptr->Opcode() == air::core::OPC_ST)
                   ? cntr->New_ld(ptr->Addr_datum(), ptr->Spos())
                   : cntr->New_ldp(ptr->Preg(), ptr->Spos());
      };

      if (_config.Rt_dump()) {
        // dump result
        const char*         msg = air::base::META_INFO::Op_name(val->Opcode());
        air::base::NODE_PTR ld  = gen_ld(cntr, node);
        Append(cntr->New_dump_var(msg, ld, len, node->Spos()));
      }

      air::base::OPCODE ref_opc;
      if (_config.Ref_validate() &&
          (ref_opc = Ref_opcode(val->Opcode())) != air::core::OPC_INVALID) {
        // whole program validate, store intermediate result into preg
        air::base::NODE_PTR ref_node = Gen_ref_node(cntr, val, ref_opc);
        air::base::PREG_PTR ref_var =
            cntr->Parent_func_scope()->New_preg(ref_node->Rtype());
        Append(cntr->New_stp(ref_node, ref_var, node->Spos()));
        Ref_map(node, ref_var->Id());

        air::base::NODE_PTR ld   = gen_ld(cntr, node);
        air::base::NODE_PTR v_ld = cntr->New_ldp(ref_var, node->Spos());
        Append(Validate_stmt(cntr, ld, v_ld, len, epi));
      }

      air::base::OPCODE rtv_opc;
      if (_config.Rt_validate() &&
          (rtv_opc = Rtv_opcode(val->Opcode())) != air::core::OPC_INVALID) {
        // operator validate, verify result directly
        air::base::NODE_PTR ld   = gen_ld(cntr, node);
        air::base::NODE_PTR v_ld = Gen_rtv_node(cntr, val, rtv_opc);
        Append(Validate_stmt(cntr, ld, v_ld, len, epi));
      }
    }
  }

private:
  int Get_op_epi(air::base::OPCODE opc) {
    switch (opc) {
      case nn::core::OPC_RELU:
        return -4;
      default:
        return -6;
    }
  }

  air::base::OPCODE Rtv_opcode(air::base::OPCODE opc) {
    switch (opc) {
      case nn::core::OPC_AVERAGE_POOL:
        return nn::vector::OPC_AVERAGE_POOL_RTV;
      case nn::core::OPC_CONV:
        return nn::vector::OPC_CONV_RTV;
      case nn::core::OPC_GEMM:
        return nn::vector::OPC_GEMM_RTV;
      case nn::core::OPC_GLOBAL_AVERAGE_POOL:
        return nn::vector::OPC_GLOBAL_AVERAGE_POOL_RTV;
      case nn::core::OPC_MAX_POOL:
        return nn::vector::OPC_MAX_POOL_RTV;
      case nn::core::OPC_RELU:
        return nn::vector::OPC_RELU_RTV;
      default:
        return air::core::OPC_INVALID;
    }
  }

  air::base::OPCODE Ref_opcode(air::base::OPCODE opc) {
    switch (opc) {
      case nn::core::OPC_ADD:
        return nn::vector::OPC_ADD_REF;
      case nn::core::OPC_AVERAGE_POOL:
        return nn::vector::OPC_AVERAGE_POOL_REF;
      case nn::core::OPC_CONV:
        return nn::vector::OPC_CONV_REF;
      case nn::core::OPC_FLATTEN:
        return nn::vector::OPC_FLATTEN_REF;
      case nn::core::OPC_GEMM:
        return nn::vector::OPC_GEMM_REF;
      case nn::core::OPC_GLOBAL_AVERAGE_POOL:
        return nn::vector::OPC_GLOBAL_AVERAGE_POOL_REF;
      case nn::core::OPC_MAX_POOL:
        return nn::vector::OPC_MAX_POOL_REF;
      case nn::core::OPC_RELU:
        return nn::vector::OPC_RELU_REF;
      case nn::core::OPC_RESHAPE:
        return nn::vector::OPC_RESHAPE_REF;
      default:
        return air::core::OPC_INVALID;
    }
  }

  air::base::NODE_PTR Gen_rtv_node(air::base::CONTAINER* cntr,
                                   air::base::NODE_PTR   node,
                                   air::base::OPCODE     rtv_opc) {
    air::base::NODE_PTR rtv_node = cntr->New_validate_node(node, rtv_opc);
    Set_attr(node, rtv_node);
    return rtv_node;
  }

  air::base::NODE_PTR Gen_ref_node(air::base::CONTAINER* cntr,
                                   air::base::NODE_PTR   node,
                                   air::base::OPCODE     ref_opc) {
    air::base::NODE_PTR ref_node = cntr->New_validate_node(node, ref_opc);
    air::base::TYPE_PTR f64_ptr  = cntr->Glob_scope()->New_ptr_type(
        cntr->Glob_scope()->Prim_type(air::base::PRIMITIVE_TYPE::FLOAT_64),
        POINTER_KIND::FLAT32, "f64*");
    ref_node->Set_rtype(f64_ptr);
    air::base::NODE_PTR op0 = node->Child(0);
    AIR_ASSERT(op0->Opcode() == air::core::OPC_LD ||
               op0->Opcode() == air::core::OPC_LDP);
    if (op0->Opcode() == air::core::OPC_LDP ||
        !op0->Addr_datum()->Is_formal()) {
      PREG_PTR parm = cntr->Parent_func_scope()->Preg(Ref_find(op0));
      ref_node->Set_child(0, cntr->New_ldp(parm, node->Spos()));
    }
    if (ref_opc == nn::vector::OPC_ADD_REF) {
      air::base::NODE_PTR op1  = node->Child(1);
      PREG_PTR            parm = cntr->Parent_func_scope()->Preg(Ref_find(op1));
      ref_node->Set_child(1, cntr->New_ldp(parm, node->Spos()));
    }

    Set_attr(node, ref_node);
    return ref_node;
  }

  void Ref_map(NODE_PTR node, PREG_ID val) {
    AIR_ASSERT(node->Opcode() == air::core::OPC_ST ||
               node->Opcode() == air::core::OPC_STP);
    if (node->Opcode() == air::core::OPC_ST) {
      uint32_t key = node->Addr_datum_id().Value();
      AIR_ASSERT(_ref_var_map.find(key) == _ref_var_map.end());
      _ref_var_map[key] = val.Value();
    } else if (node->Opcode() == air::core::OPC_STP) {
      uint32_t key = node->Preg_id().Value();
      AIR_ASSERT(_ref_preg_map.find(key) == _ref_preg_map.end());
      _ref_preg_map[key] = val.Value();
    }
  }

  PREG_ID Ref_find(NODE_PTR node) {
    AIR_ASSERT(node->Opcode() == air::core::OPC_LD ||
               node->Opcode() == air::core::OPC_LDP);
    if (node->Opcode() == air::core::OPC_LD) {
      uint32_t key = node->Addr_datum_id().Value();
      AIR_ASSERT(_ref_var_map.find(key) != _ref_var_map.end());
      return PREG_ID(_ref_var_map[key]);
    } else if (node->Opcode() == air::core::OPC_LDP) {
      uint32_t key = node->Preg_id().Value();
      AIR_ASSERT(_ref_preg_map.find(key) != _ref_preg_map.end());
      return PREG_ID(_ref_preg_map[key]);
    }
    AIR_ASSERT(false);
    return PREG_ID();
  }

  air::base::STMT_PTR Validate_stmt(air::base::CONTAINER* cntr,
                                    air::base::NODE_PTR   val,
                                    air::base::NODE_PTR v_val, uint64_t len,
                                    int32_t epi) {
    air::base::TYPE_PTR s32_type =
        cntr->Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_S32);
    air::base::NODE_PTR len_node =
        cntr->New_intconst(s32_type, len, val->Spos());
    air::base::NODE_PTR epi_node =
        cntr->New_intconst(s32_type, epi, val->Spos());
    return cntr->New_validate_stmt(val, v_val, len_node, epi_node, val->Spos());
  }

  void Set_attr(air::base::NODE_PTR node, air::base::NODE_PTR v_node) {
    auto set_attr = [](air::base::NODE_PTR v_ptr, const char* name,
                       air::base::TYPE_PTR type) {
      AIR_ASSERT(type->Is_array());
      std::vector<int64_t> shape = type->Cast_to_arr()->Shape();
      v_ptr->Set_attr(name, shape.data(), shape.size());
    };

    set_attr(v_node, "x_shape", node->Child(0)->Rtype());
    set_attr(v_node, "o_shape", node->Rtype());
    if (node->Opcode() == nn::core::OPC_CONV ||
        node->Opcode() == nn::core::OPC_GEMM) {
      set_attr(v_node, "w_shape", node->Child(1)->Rtype());
      set_attr(v_node, "b_shape", node->Child(2)->Rtype());
    }
  }

  const VECTOR_CONFIG&                   _config;
  std::unordered_map<uint32_t, uint32_t> _ref_var_map;
  std::unordered_map<uint32_t, uint32_t> _ref_preg_map;
};  // TENSOR_INSTR_CTX

//! @brief CORE handler for instrumentation to dump tensor value
class TENSOR_INSTR_CORE_HANDLER : public air::core::NULL_HANDLER {
public:
  template <typename RETV, typename VISITOR>
  RETV Handle_st(VISITOR* visitor, air::base::NODE_PTR node) {
    return visitor->Context().template Handle_node<RETV>(visitor, node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_stp(VISITOR* visitor, air::base::NODE_PTR node) {
    return visitor->Context().template Handle_node<RETV>(visitor, node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_block(VISITOR* visitor, air::base::NODE_PTR node) {
    return visitor->Context().template Handle_block<RETV>(visitor, node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_do_loop(VISITOR* visitor, air::base::NODE_PTR node) {
    return visitor->template Visit<RETV>(node->Body_blk());
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_func_entry(VISITOR* visitor, air::base::NODE_PTR node) {
    return visitor->template Visit<RETV>(node->Body_blk());
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_if(VISITOR* visitor, air::base::NODE_PTR node) {
    visitor->template Visit<RETV>(node->Then_blk());
    visitor->template Visit<RETV>(node->Else_blk());
    return RETV();
  }
};  // TENSOR_INSTR_CORE_HANDLER

}  // namespace vector
}  // namespace nn

#endif  // NN_VECTOR_TENSOR_INSTR_H
