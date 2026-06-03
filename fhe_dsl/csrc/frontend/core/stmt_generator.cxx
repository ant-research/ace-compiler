//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "ace/frontend/core/stmt_generator.h"

#include "ace/frontend/core/air_context.h"
#include "ace/frontend/core/symbol_table.h"

namespace ace {
namespace frontend {

uint32_t STMT_GENERATOR::Generate_pragma_comment(const std::string& op_name,
                                                  air::base::OPCODE opcode,
                                                  air::base::FUNC_SCOPE* func_scope) {
    if (!func_scope) {
        return 0;
    }

    air::base::CONTAINER* cntr = &func_scope->Container();

    // Generate comment with operator name
    std::string op_comment = !_current_op_name.empty() ? _current_op_name : "Op: " + op_name;
    air::base::STMT_PTR comment_stmt = cntr->New_comment(op_comment.c_str(), _ctx->Get_spos());
    cntr->Stmt_list().Append(comment_stmt);

    uint32_t comment_id = comment_stmt->Node()->Comment_id().Value();

    // Generate pragma OP_START if enabled
    if (_enable_pragma) {
        uint32_t op_code = static_cast<uint32_t>(opcode);
        air::base::STMT_PTR pragma_start = cntr->New_pragma(
            nn::core::PRAGMA_OP_START, op_code, comment_id, _ctx->Get_spos());
        cntr->Stmt_list().Append(pragma_start);
    }

    return comment_id;
}

void STMT_GENERATOR::Generate_store_statement(air::base::NODE_PTR op_node,
                                               const std::string& result_name,
                                               bool is_output,
                                               SYMBOL_TABLE* sym_tab,
                                               air::base::FUNC_SCOPE* func_scope,
                                               air::base::ADDR_DATUM_PTR output_var,
                                               air::base::TYPE_PTR rtype) {
    if (!func_scope || op_node == air::base::Null_ptr) {
        return;
    }

    air::base::CONTAINER* cntr = &func_scope->Container();

    if (is_output) {
        // Store directly to output variable
        if (output_var != air::base::Null_ptr) {
            air::base::STMT_PTR store_stmt = cntr->New_st(op_node, output_var, _ctx->Get_spos());
            cntr->Stmt_list().Append(store_stmt);
        }
    } else {
        // Store to PREG for intermediate results
        air::base::PREG_PTR preg = func_scope->New_preg(rtype->Id(), air::base::SYM_ID());
        if (sym_tab) {
            sym_tab->Add_preg(result_name, preg);
        }
        air::base::STMT_PTR store_stmt = cntr->New_stp(op_node, preg, _ctx->Get_spos());
        cntr->Stmt_list().Append(store_stmt);
    }
}

void STMT_GENERATOR::Complete_operation(air::base::OPCODE opcode,
                                         uint32_t comment_id,
                                         air::base::FUNC_SCOPE* func_scope) {
    if (!func_scope || !_enable_pragma) {
        _enable_pragma = false;
        _current_op_name.clear();
        return;
    }

    air::base::CONTAINER* cntr = &func_scope->Container();
    uint32_t op_code = static_cast<uint32_t>(opcode);

    air::base::STMT_PTR pragma_end = cntr->New_pragma(
        nn::core::PRAGMA_OP_END, op_code, comment_id, _ctx->Get_spos());
    cntr->Stmt_list().Append(pragma_end);

    // Reset pragma state
    _enable_pragma = false;
    _current_op_name.clear();
}

}  // namespace frontend
}  // namespace ace