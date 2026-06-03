//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "ace/frontend/core/func_builder.h"
#include "ace/frontend/core/air_context.h"
#include "ace/frontend/core/type_factory.h"
#include "ace/frontend/core/symbol_table.h"
#include "nn/core/attr.h"

namespace ace {
namespace frontend {

FUNC_BUILDER::FUNC_BUILDER(AIR_CONTEXT* ctx)
    : _ctx(ctx)
    , _type_factory(std::make_unique<TYPE_FACTORY>(ctx)) {
}

void FUNC_BUILDER::Begin_func(const std::string& func_name) {
    // Clear previous state
    _input.clear();
    _output = VAR_ENTRY{};
    _func_scope = nullptr;
    _func_name = air::base::Null_ptr;
    _func = air::base::Null_ptr;
    _sig = air::base::Null_ptr;

    // Initialize GLOB_SCOPE through AIR_CONTEXT
    _ctx->Initialize();

    // Get global scope
    air::base::GLOB_SCOPE* glob = _ctx->Get_glob();
    if (!glob) {
        return;
    }

    // Create function name string
    _func_name = glob->New_str(func_name.c_str());

    // Create function
    _func = glob->New_func(_func_name, _ctx->Get_spos());
    _func->Set_parent(glob->Comp_env_id());
}

air::base::CONSTANT_PTR FUNC_BUILDER::New_const(const CONST_ENTRY& entry) {
    air::base::GLOB_SCOPE* glob = _ctx->Get_glob();
    if (!glob) {
        return air::base::Null_ptr;
    }

    // Create array type
    air::base::TYPE_PTR const_type = _type_factory->New_array_type(entry._elem_type, entry._shape);

    // Calculate byte_len: numel * sizeof(elem_type)
    size_t numel = 1;
    for (int64_t dim : entry._shape) {
        numel *= static_cast<size_t>(dim);
    }
    size_t elem_size = 0;
    switch (entry._elem_type) {
        case air::base::PRIMITIVE_TYPE::FLOAT_32: elem_size = 4; break;
        case air::base::PRIMITIVE_TYPE::FLOAT_64: elem_size = 8; break;
        case air::base::PRIMITIVE_TYPE::INT_S64: elem_size = 8; break;
        case air::base::PRIMITIVE_TYPE::INT_S32: elem_size = 4; break;
        case air::base::PRIMITIVE_TYPE::INT_S16: elem_size = 2; break;
        case air::base::PRIMITIVE_TYPE::INT_S8:  elem_size = 1; break;
        case air::base::PRIMITIVE_TYPE::INT_U64: elem_size = 8; break;
        case air::base::PRIMITIVE_TYPE::INT_U32: elem_size = 4; break;
        case air::base::PRIMITIVE_TYPE::INT_U16: elem_size = 2; break;
        case air::base::PRIMITIVE_TYPE::INT_U8:  elem_size = 1; break;
        default: elem_size = 4; break;
    }
    size_t byte_len = numel * elem_size;

    return glob->New_const(air::base::CONSTANT_KIND::ARRAY, const_type, const_cast<void*>(entry._data), byte_len);
}

void FUNC_BUILDER::End_func(const VAR_ENTRY& output, SYMBOL_TABLE* sym_tab) {
    air::base::GLOB_SCOPE* glob = _ctx->Get_glob();
    if (!glob) {
        return;
    }

    // Create all input tensor types first (to match onnx2air order)
    std::vector<air::base::TYPE_PTR> input_types;
    std::vector<air::base::STR_PTR> input_strs;
    for (const auto& input : _input) {
        input_types.push_back(_type_factory->New_tensor_type(input._shape));
        input_strs.push_back(glob->New_str(input._name.c_str()));
    }

    // Create output type
    _output = output;
    air::base::TYPE_PTR ret_type = _type_factory->New_tensor_type(output._shape);

    // Create signature AFTER all tensor types
    _sig = glob->New_sig_type();

    // Add params to signature
    for (size_t i = 0; i < _input.size(); ++i) {
        glob->New_param(input_strs[i], input_types[i], _sig, _ctx->Get_spos());
    }

    // Set signature complete
    _sig->Set_complete();

    // Create entry point
    glob->New_global_entry_point(_sig, _func, _func_name, _ctx->Get_spos());

    // Create function scope
    _func_scope = &glob->New_func_scope(_func);

    // Create entry statement
    air::base::CONTAINER* cntr = &_func_scope->Container();
    cntr->New_func_entry(_ctx->Get_spos());

    // Map formal parameters to symbol table
    for (size_t i = 0; i < _input.size(); ++i) {
        air::base::ADDR_DATUM_PTR formal = _func_scope->Formal(i);
        if (sym_tab) {
            sym_tab->Add_var(_input[i]._name, formal);
        }
        // Set SHAPE attribute on formal parameter IDNAME node for Emit_data_shape
        air::base::NODE_PTR entry_node = cntr->Entry_node();
        if (entry_node != air::base::Null_ptr && entry_node->Num_child() > i) {
            air::base::NODE_PTR formal_node = entry_node->Child(i);
            formal_node->Set_attr(nn::core::ATTR::SHAPE, _input[i]._shape.data(), _input[i]._shape.size());
        }
    }

    // Create output variable
    _output._name = "output";
    air::base::STR_PTR ret_str = glob->New_str(_output._name.c_str());
    _output._var = _func_scope->New_var(ret_type, ret_str, _ctx->Get_spos());

    // Add return parameter
    glob->New_ret_param(ret_type, _sig);

    // Set as program entry
    _func->Entry_point()->Set_program_entry();
}

void FUNC_BUILDER::Complete_func() {
    if (!_func_scope || _output._var == air::base::Null_ptr) {
        return;
    }

    air::base::CONTAINER* cntr = &_func_scope->Container();

    // Create return statement
    air::base::NODE_PTR ret_val = cntr->New_ld(_output._var, _ctx->Get_spos());
    air::base::STMT_PTR ret_stmt = cntr->New_retv(ret_val, _ctx->Get_spos());
    cntr->Stmt_list().Append(ret_stmt);

    // Set SHAPE attribute on RETV node so that Emit_data_shape can read it
    air::base::NODE_PTR retv_node = ret_stmt->Node();
    retv_node->Set_attr(nn::core::ATTR::SHAPE, _output._shape.data(), _output._shape.size());
}

}  // namespace frontend
}  // namespace ace