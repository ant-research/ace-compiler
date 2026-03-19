/**
 * Skipable Handler Wrapper
 * 
 * Wraps nn::vector::TENSOR2VECTOR_HANDLER to add skip checking.
 * When an op is in the skip list (has Python lowering), the original
 * node is preserved instead of being lowered to vector IR.
 */

#ifndef PYACE_SKIPABLE_HANDLER_H
#define PYACE_SKIPABLE_HANDLER_H

#include "python_lowering_bridge.h"
#include "nn/vector/tensor2vector_handler.h"

namespace pyace {

/**
 * Handler that wraps TENSOR2VECTOR_HANDLER with skip checking.
 * 
 * For each op type, checks PythonLoweringBridge before lowering.
 * If the op should be skipped, clones the original node.
 */
class SKIPABLE_TENSOR2VECTOR_HANDLER : public nn::vector::TENSOR2VECTOR_HANDLER {
public:
    using BASE = nn::vector::TENSOR2VECTOR_HANDLER;
    
    SKIPABLE_TENSOR2VECTOR_HANDLER() : BASE() {}
    
    // Handle add - check skip before lowering
    template <typename RETV, typename VISITOR>
    RETV Handle_add(VISITOR* visitor, air::base::NODE_PTR node) {
        if (should_skip_op("nn::core", "add")) {
            // Clone original node - Python will lower it
            return clone_with_visited_children<RETV>(visitor, node);
        }
        return BASE::Handle_add<RETV>(visitor, node);
    }
    
    // Handle mul - check skip before lowering
    template <typename RETV, typename VISITOR>
    RETV Handle_mul(VISITOR* visitor, air::base::NODE_PTR node) {
        if (should_skip_op("nn::core", "mul")) {
            return clone_with_visited_children<RETV>(visitor, node);
        }
        return BASE::Handle_mul<RETV>(visitor, node);
    }
    
    // Handle conv - check skip before lowering
    template <typename RETV, typename VISITOR>
    RETV Handle_conv(VISITOR* visitor, air::base::NODE_PTR node) {
        if (should_skip_op("nn::core", "conv")) {
            return clone_with_visited_children<RETV>(visitor, node);
        }
        return BASE::Handle_conv<RETV>(visitor, node);
    }
    
    // Handle relu - check skip before lowering
    template <typename RETV, typename VISITOR>
    RETV Handle_relu(VISITOR* visitor, air::base::NODE_PTR node) {
        if (should_skip_op("nn::core", "relu")) {
            return clone_with_visited_children<RETV>(visitor, node);
        }
        return BASE::Handle_relu<RETV>(visitor, node);
    }
    
    // Handle matmul - check skip before lowering
    template <typename RETV, typename VISITOR>
    RETV Handle_matmul(VISITOR* visitor, air::base::NODE_PTR node) {
        if (should_skip_op("nn::core", "matmul")) {
            return clone_with_visited_children<RETV>(visitor, node);
        }
        // matmul uses gemm handler in base
        return BASE::Handle_gemm<RETV>(visitor, node);
    }
    
    // Handle average_pool - check skip before lowering
    template <typename RETV, typename VISITOR>
    RETV Handle_average_pool(VISITOR* visitor, air::base::NODE_PTR node) {
        if (should_skip_op("nn::core", "average_pool")) {
            return clone_with_visited_children<RETV>(visitor, node);
        }
        return BASE::Handle_average_pool<RETV>(visitor, node);
    }
    
    // Handle max_pool - check skip before lowering
    template <typename RETV, typename VISITOR>
    RETV Handle_max_pool(VISITOR* visitor, air::base::NODE_PTR node) {
        if (should_skip_op("nn::core", "max_pool")) {
            return clone_with_visited_children<RETV>(visitor, node);
        }
        return BASE::Handle_max_pool<RETV>(visitor, node);
    }
    
    // Handle flatten - check skip before lowering
    template <typename RETV, typename VISITOR>
    RETV Handle_flatten(VISITOR* visitor, air::base::NODE_PTR node) {
        if (should_skip_op("nn::core", "flatten")) {
            return clone_with_visited_children<RETV>(visitor, node);
        }
        return BASE::Handle_flatten<RETV>(visitor, node);
    }
    
    // Handle reshape - check skip before lowering
    template <typename RETV, typename VISITOR>
    RETV Handle_reshape(VISITOR* visitor, air::base::NODE_PTR node) {
        if (should_skip_op("nn::core", "reshape")) {
            return clone_with_visited_children<RETV>(visitor, node);
        }
        return BASE::Handle_reshape<RETV>(visitor, node);
    }

private:
    /**
     * Clone node with visited children.
     * 
     * This preserves the original op but visits children to allow
     * lowering of nested ops that aren't skipped.
     */
    template <typename RETV, typename VISITOR>
    RETV clone_with_visited_children(VISITOR* visitor, air::base::NODE_PTR node) {
        air::base::CONTAINER* cntr = visitor->Context().Container();
        air::base::NODE_PTR new_node = cntr->Clone_node(node);
        
        // Visit children to allow lowering of non-skipped nested ops
        for (uint32_t i = 0; i < node->Num_child(); ++i) {
            air::base::NODE_PTR child = node->Child(i);
            air::base::NODE_PTR new_child = visitor->template Visit<RETV>(child);
            new_node->Set_child(i, new_child->Id());
        }
        
        return new_node;
    }
};

}  // namespace pyace

#endif  // PYACE_SKIPABLE_HANDLER_H

