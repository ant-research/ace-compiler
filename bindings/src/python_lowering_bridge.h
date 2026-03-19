/**
 * Python Lowering Bridge
 * 
 * Provides C++ integration for Python-defined lowerings.
 * 
 * Usage:
 *   1. Python registers ops to skip via set_skip_ops()
 *   2. C++ passes check should_skip() before lowering
 *   3. Skipped ops are left for Python pass to handle
 */

#ifndef PYACE_PYTHON_LOWERING_BRIDGE_H
#define PYACE_PYTHON_LOWERING_BRIDGE_H

#include <string>
#include <set>
#include <vector>

namespace pyace {

/**
 * Global registry of ops that should be skipped by C++ passes.
 * These ops have Python-defined lowerings that will be applied
 * after C++ passes complete.
 */
class PythonLoweringBridge {
public:
    static PythonLoweringBridge& instance() {
        static PythonLoweringBridge inst;
        return inst;
    }
    
    /**
     * Set ops to skip (called from Python before running C++ passes)
     */
    void set_skip_ops(const std::vector<std::string>& ops) {
        skip_ops_.clear();
        skip_ops_.insert(ops.begin(), ops.end());
    }
    
    /**
     * Add a single op to skip
     */
    void add_skip_op(const std::string& op) {
        skip_ops_.insert(op);
    }
    
    /**
     * Clear all skip ops
     */
    void clear_skip_ops() {
        skip_ops_.clear();
    }
    
    /**
     * Check if an op should be skipped by C++ lowering
     * 
     * @param domain e.g., "nn::core"
     * @param op_name e.g., "conv"
     * @return true if Python has a registered lowering for this op
     */
    bool should_skip(const std::string& domain, const std::string& op_name) const {
        std::string full_name = domain + "::" + op_name;
        return skip_ops_.count(full_name) > 0;
    }
    
    /**
     * Check by full op name
     */
    bool should_skip(const std::string& full_op_name) const {
        return skip_ops_.count(full_op_name) > 0;
    }
    
    /**
     * Get all ops to skip
     */
    const std::set<std::string>& get_skip_ops() const {
        return skip_ops_;
    }
    
    /**
     * Get count of ops to skip
     */
    size_t skip_count() const {
        return skip_ops_.size();
    }

private:
    PythonLoweringBridge() = default;
    std::set<std::string> skip_ops_;
};

// Convenience functions
inline bool should_skip_op(const std::string& domain, const std::string& op_name) {
    return PythonLoweringBridge::instance().should_skip(domain, op_name);
}

inline bool should_skip_op(const std::string& full_op_name) {
    return PythonLoweringBridge::instance().should_skip(full_op_name);
}

}  // namespace pyace

#endif  // PYACE_PYTHON_LOWERING_BRIDGE_H

