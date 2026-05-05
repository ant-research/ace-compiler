//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "frontend/core/ir_builder.h"

#include "frontend/core/attr_converter.h"
#include "frontend/layers/tensor_level.h"
#include "frontend/layers/tensor_level_handler.h"

// AIR binary writer
#include "air/util/binary/air2elf.h"

#include <iostream>
#include <set>

namespace ace {
namespace frontend {

// ========================================================================
// IR_BUILDER Implementation
// ========================================================================

IR_BUILDER::IR_BUILDER()
    : _ctx(std::make_unique<AIR_CONTEXT>())
    , _type_factory(std::make_unique<TYPE_FACTORY>(_ctx.get()))
    , _stmt_gen(std::make_unique<STMT_GENERATOR>(_ctx.get()))
    , _func_builder(std::make_unique<FUNC_BUILDER>(_ctx.get())) {
    Init_default_level();
}

IR_BUILDER::~IR_BUILDER() = default;

IR_BUILDER& IR_BUILDER::Instance() {
    static IR_BUILDER instance;
    return instance;
}

void IR_BUILDER::Init_default_level() {
    // Register Tensor level
    _level[LEVEL_TYPE::TENSOR] = std::make_unique<TENSOR_LEVEL>();

    // Set default level
    _current_level = LEVEL_TYPE::TENSOR;
}

void IR_BUILDER::Set_level(LEVEL_TYPE level) {
    if (_level.find(level) != _level.end()) {
        _current_level = level;
        std::cout << "[IR_BUILDER] Set level to: " << Level_type_to_string(level) << std::endl;
    } else {
        std::cerr << "[IR_BUILDER] Warning: Level not registered: " << static_cast<int>(level) << std::endl;
    }
}

void IR_BUILDER::Set_level(const std::string& level_name) {
    Set_level(String_to_level_type(level_name));
}

LEVEL_TYPE IR_BUILDER::Get_current_level() const {
    return _current_level;
}

std::string IR_BUILDER::Get_current_level_name() const {
    return Level_type_to_string(_current_level);
}

bool IR_BUILDER::Has_level(LEVEL_TYPE level) const {
    return _level.find(level) != _level.end();
}

void IR_BUILDER::Register_level(LEVEL_TYPE level, LEVEL_PTR handler) {
    _level[level] = std::move(handler);
}

void IR_BUILDER::Begin_func(const std::string& func_name) {
    // Clear symbol table (includes tensor names)
    _symbol_table.Clear();

    // Begin function in FUNC_BUILDER
    _func_builder->Begin_func(func_name);
}

void IR_BUILDER::New_param(const std::string& name, const std::vector<int64_t>& shape) {
    _func_builder->New_param({name, shape});
}

void IR_BUILDER::New_const(const std::string& name, const std::vector<int64_t>& shape,
                              const void* data, size_t /*byte_len*/) {
    CONST_ENTRY entry{name, shape, data};
    air::base::CONSTANT_PTR cst = _func_builder->New_const(entry);
    if (cst != air::base::Null_ptr) {
        _symbol_table.Add_const(name, cst);
    }
}

void IR_BUILDER::New_const_int64(const std::string& name, const std::vector<int64_t>& shape,
                                   const void* data, size_t /*byte_len*/) {
    CONST_ENTRY entry{name, shape, data, air::base::PRIMITIVE_TYPE::INT_S64};
    air::base::CONSTANT_PTR cst = _func_builder->New_const(entry);
    if (cst != air::base::Null_ptr) {
        _symbol_table.Add_const(name, cst);
    }
}

void IR_BUILDER::End_func(const std::vector<int64_t>& output_shape) {
    VAR_ENTRY output{"output", output_shape};
    _func_builder->End_func(output, &_symbol_table);
}

void IR_BUILDER::Complete_func() {
    _func_builder->Complete_func();
}

std::string IR_BUILDER::Add_operation(const std::string& op_name,
                                      const std::vector<std::string>& input_names,
                                      const std::map<std::string, py::object>& attrs,
                                      const std::map<std::string, std::string>& metadata,
                                      const std::vector<int64_t>& output_shape) {
    // Handle is_output flag from metadata
    bool is_output = metadata.count("is_output") && metadata.at("is_output") == "True";

    // Strip "tensor." prefix from op_name if present
    std::string opcode_name = op_name;
    if (opcode_name.rfind("tensor.", 0) == 0) {
        opcode_name = opcode_name.substr(7);
    }

    // Get registered handler
    static TENSOR_LEVEL_HANDLER tensor_handler;
    if (!tensor_handler.Has_op(opcode_name)) {
        std::cerr << "[IR_BUILDER] Operation not supported: " << op_name << std::endl;
        return "";
    }

    try {
        // Setup pragma from metadata
        if (metadata.count("onnx_name")) {
            _stmt_gen->Set_op_name(metadata.at("onnx_name"));
            _stmt_gen->Enable_pragma(true);
        }

        // Convert attrs using ATTR_CONVERTER utility
        std::map<std::string, std::any> attrs_any = ATTR_CONVERTER::Convert(attrs);

        // Process operation with TENSOR_LEVEL_HANDLER
        air::base::CONTAINER* cntr = _func_builder->Get_container();
        if (!cntr) {
            return "";
        }

        air::base::NODE_PTR op_node = tensor_handler.Process_op(
            opcode_name, input_names, attrs_any, metadata, cntr,
            _ctx->Get_spos(), &_symbol_table, _type_factory.get(), output_shape);

        if (op_node == air::base::Null_ptr) {
            return "";
        }

        // Get opcode for pragma generation
        air::base::OPCODE opcode = tensor_handler.Get_opcode(opcode_name);

        // Generate pragma and comment
        uint32_t comment_id = _stmt_gen->Generate_pragma_comment(opcode_name, opcode, _func_builder->Get_func_scope());

        // Generate result name and store statement
        std::string result_name = "_v" + std::to_string(_var_id_counter++);
        _stmt_gen->Generate_store_statement(
            op_node, result_name, is_output,
            &_symbol_table, _func_builder->Get_func_scope(),
            _func_builder->Get_output()._var, op_node->Rtype());

        // Complete operation (pragma_end)
        _stmt_gen->Complete_operation(opcode, comment_id, _func_builder->Get_func_scope());

        return result_name;

    } catch (const std::exception& e) {
        std::cerr << "[IR_BUILDER] TENSOR_LEVEL_HANDLER failed: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "[IR_BUILDER] TENSOR_LEVEL_HANDLER failed with unknown error" << std::endl;
    }

    return "";
}

std::string IR_BUILDER::Add_operation_cpp(const std::string& op_name,
                                          const std::vector<std::string>& input_names,
                                          const std::map<std::string, std::any>& attrs,
                                          const std::map<std::string, std::string>& metadata,
                                          const std::vector<int64_t>& output_shape) {
    // Handle is_output flag from metadata
    bool is_output = metadata.count("is_output") && metadata.at("is_output") == "True";

    // Strip "tensor." prefix from op_name if present
    std::string opcode_name = op_name;
    if (opcode_name.rfind("tensor.", 0) == 0) {
        opcode_name = opcode_name.substr(7);
    }

    // Get registered handler
    static TENSOR_LEVEL_HANDLER tensor_handler;
    if (!tensor_handler.Has_op(opcode_name)) {
        std::cerr << "[IR_BUILDER] Operation not supported: " << op_name << std::endl;
        return "";
    }

    try {
        // Setup pragma from metadata
        if (metadata.count("onnx_name")) {
            _stmt_gen->Set_op_name(metadata.at("onnx_name"));
            _stmt_gen->Enable_pragma(true);
        }

        // Process operation with TENSOR_LEVEL_HANDLER (attrs already in std::any format)
        air::base::CONTAINER* cntr = _func_builder->Get_container();
        if (!cntr) {
            return "";
        }

        air::base::NODE_PTR op_node = tensor_handler.Process_op(
            opcode_name, input_names, attrs, metadata, cntr,
            _ctx->Get_spos(), &_symbol_table, _type_factory.get(), output_shape);

        if (op_node == air::base::Null_ptr) {
            return "";
        }

        // Get opcode for pragma generation
        air::base::OPCODE opcode = tensor_handler.Get_opcode(opcode_name);

        // Generate pragma and comment
        uint32_t comment_id = _stmt_gen->Generate_pragma_comment(opcode_name, opcode, _func_builder->Get_func_scope());

        // Generate result name and store statement
        std::string result_name = "_v" + std::to_string(_var_id_counter++);
        _stmt_gen->Generate_store_statement(
            op_node, result_name, is_output,
            &_symbol_table, _func_builder->Get_func_scope(),
            _func_builder->Get_output()._var, op_node->Rtype());

        // Complete operation (pragma_end)
        _stmt_gen->Complete_operation(opcode, comment_id, _func_builder->Get_func_scope());

        return result_name;

    } catch (const std::exception& e) {
        std::cerr << "[IR_BUILDER] TENSOR_LEVEL_HANDLER failed: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "[IR_BUILDER] TENSOR_LEVEL_HANDLER failed with unknown error" << std::endl;
    }

    return "";
}

std::string IR_BUILDER::Add_operation_with_opcode(const std::string& op_name,
                                                 const std::vector<std::string>& input_names,
                                                 air::base::OPCODE opcode) {
    air::base::CONTAINER* cntr = _func_builder->Get_container();
    if (!cntr) {
        return "";
    }

    const air::base::SPOS& spos = _ctx->Get_spos();

    // Get input nodes from symbol table
    std::vector<air::base::NODE_PTR> input_nodes;
    for (const auto& name : input_names) {
        // First check if it's a constant
        air::base::CONSTANT_PTR cst = _symbol_table.Get_const(name);
        if (cst != air::base::Null_ptr) {
            input_nodes.push_back(cntr->New_ldc(cst, spos));
        } else {
            // Check if it's an intermediate result (stored in PREG)
            air::base::PREG_PTR preg = _symbol_table.Get_preg(name);
            if (preg != air::base::Null_ptr) {
                input_nodes.push_back(cntr->New_ldp(preg, spos));
            } else {
                // Load from FML (input variable)
                air::base::ADDR_DATUM_PTR var = _symbol_table.Get_var(name);
                if (var != air::base::Null_ptr) {
                    input_nodes.push_back(cntr->New_ld(var, spos));
                }
            }
        }
    }

    // Determine result type
    air::base::TYPE_PTR rtype;
    if (!input_nodes.empty()) {
        rtype = input_nodes[0]->Access_type();
    } else {
        rtype = _func_builder->Get_output()._var->Type();
    }

    // Create operation node based on input count
    air::base::NODE_PTR op_node;
    if (input_nodes.size() == 3) {
        op_node = cntr->New_tern_arith(opcode, rtype, input_nodes[0], input_nodes[1], input_nodes[2], spos);
    } else if (input_nodes.size() >= 2) {
        op_node = cntr->New_bin_arith(opcode, rtype, input_nodes[0], input_nodes[1], spos);
    } else if (input_nodes.size() >= 1) {
        op_node = cntr->New_una_arith(opcode, rtype, input_nodes[0], spos);
    }

    if (op_node == air::base::Null_ptr) {
        return "";
    }

    // Generate result name
    std::string result_name = "_v" + std::to_string(_var_id_counter++);

    // Generate pragma and comment
    uint32_t comment_id = _stmt_gen->Generate_pragma_comment(op_name, opcode, _func_builder->Get_func_scope());

    // Store result
    _stmt_gen->Generate_store_statement(op_node, result_name, false, &_symbol_table,
                                        _func_builder->Get_func_scope(),
                                        _func_builder->Get_output()._var, rtype);

    // Complete operation
    _stmt_gen->Complete_operation(opcode, comment_id, _func_builder->Get_func_scope());

    return result_name;
}

void IR_BUILDER::Print_ir() {
    if (_func_builder->Get_func_scope()) {
        std::cout << "\n=== Generated AIR IR ===" << std::endl;
        _func_builder->Get_func_scope()->Print();
    }
}

void IR_BUILDER::Write_ir(const std::string& filename, const std::string& phase) {
    air::base::GLOB_SCOPE* glob = _ctx->Get_glob();
    if (!glob || filename.empty()) {
        return;
    }
    air::util::AIR2ELF ir2elf(filename, std::cout);
    ir2elf.Run(glob, phase);
}

void IR_BUILDER::Set_op_name(const std::string& op_name) {
    _stmt_gen->Set_op_name(op_name);
}

void IR_BUILDER::Enable_pragma(bool enable) {
    _stmt_gen->Enable_pragma(enable);
}

bool IR_BUILDER::Is_building() const {
    return _func_builder->Is_building();
}

air::base::FUNC_SCOPE* IR_BUILDER::Get_func_scope() {
    return _func_builder->Get_func_scope();
}

air::base::GLOB_SCOPE* IR_BUILDER::Get_glob() {
    return _ctx->Get_glob();
}

air::base::CONTAINER* IR_BUILDER::Get_container() {
    return _func_builder->Get_container();
}

const air::base::SPOS& IR_BUILDER::Get_spos() const {
    return _ctx->Get_spos();
}

const VAR_ENTRY& IR_BUILDER::Get_output() const {
    return _func_builder->Get_output();
}

at::Tensor IR_BUILDER::Build_op(const std::string& op_name,
                                const std::vector<at::Tensor>& inputs) {
    auto* level = Get_current_level_handler();
    if (!level) {
        std::cerr << "[IR_BUILDER] No current level handler" << std::endl;
        return at::Tensor();
    }
    return level->Build_op(op_name, inputs);
}

void IR_BUILDER::Register_all_level(pybind11::module& m) {
    // Register each level's ops
    for (auto& p : _level) {
        p.second->Register_py_ops(m);
    }
}

LEVEL_BASE* IR_BUILDER::Get_current_level_handler() {
    auto it = _level.find(_current_level);
    if (it != _level.end()) {
        return it->second.get();
    }
    return nullptr;
}

}  // namespace frontend
}  // namespace ace