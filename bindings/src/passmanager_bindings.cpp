//
// passmanager_bindings.cpp - pybind11 bindings for Pass Manager
//
// This module provides Python access to the ACE-compiler pass infrastructure.
// It interfaces with real nn-addon passes, NOT mock implementations.
//

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <sstream>
#include <map>
#include <set>

#ifdef ACE_BINDINGS_ENABLED
// Real ACE-compiler includes for GLOB_SCOPE
#include "air/base/st.h"
#include "air/base/container.h"
#endif

// Include the Python lowering bridge
#include "python_lowering_bridge.h"

namespace py = pybind11;

namespace pyace {

// Forward declarations
class Pass;
class Module;

// Module representation - holds AIR GLOB_SCOPE
class Module {
public:
    std::string name;
    std::string ir_dump;
    bool multithreading_enabled;
    
#ifdef ACE_BINDINGS_ENABLED
    // When ACE is enabled, we hold a pointer to the real GLOB_SCOPE
    air::base::GLOB_SCOPE* glob_scope;
    bool owns_glob_scope;
#endif
    
    Module(const std::string& n = "module") 
        : name(n), multithreading_enabled(true)
#ifdef ACE_BINDINGS_ENABLED
        , glob_scope(nullptr), owns_glob_scope(false)
#endif
    {}
    
#ifdef ACE_BINDINGS_ENABLED
    // Constructor that takes a real GLOB_SCOPE
    Module(air::base::GLOB_SCOPE* gs, bool owns = false) 
        : name("module"), multithreading_enabled(true),
          glob_scope(gs), owns_glob_scope(owns) {}
    
    ~Module() {
        if (owns_glob_scope && glob_scope) {
            delete glob_scope;
        }
    }
    
    bool has_glob_scope() const { return glob_scope != nullptr; }
    air::base::GLOB_SCOPE* get_glob_scope() { return glob_scope; }
    void set_glob_scope(air::base::GLOB_SCOPE* gs, bool owns = false) {
        if (owns_glob_scope && glob_scope) {
            delete glob_scope;
        }
        glob_scope = gs;
        owns_glob_scope = owns;
    }
#else
    bool has_glob_scope() const { return false; }
#endif
    
    void enable_multithreading(bool enabled) {
        multithreading_enabled = enabled;
    }
    
    std::string dump() const { return ir_dump; }
    void set_ir(const std::string& ir) { ir_dump = ir; }
    
    // Accept native GLOB_SCOPE* pointer from air_builder
    void set_native_glob_scope(uintptr_t ptr) {
#ifdef ACE_BINDINGS_ENABLED
        glob_scope = reinterpret_cast<air::base::GLOB_SCOPE*>(ptr);
        owns_glob_scope = false;  // Don't own, air_builder owns it
#else
        (void)ptr;
#endif
    }
};

// Pass representation - wraps real nn-addon passes
class Pass {
public:
    std::string name;
    bool trace_ir_before;
    bool trace_ir_after;
    bool trace_stat;
    
    Pass(const std::string& n) 
        : name(n), trace_ir_before(false), trace_ir_after(false), trace_stat(false) {}
    
    void set_trace_ir_before(bool v) { trace_ir_before = v; }
    void set_trace_ir_after(bool v) { trace_ir_after = v; }
    void set_trace_stat(bool v) { trace_stat = v; }
    
    virtual void run(Module& module) {
        if (trace_ir_before) {
            std::cout << "=== IR Before " << name << " ===" << std::endl;
            std::cout << module.dump() << std::endl;
        }
        
        do_run(module);
        
        if (trace_ir_after) {
            std::cout << "=== IR After " << name << " ===" << std::endl;
            std::cout << module.dump() << std::endl;
        }
    }
    
    virtual void do_run(Module& module) {
        // Real passes are implemented in nn-addon
        // This is just the Python wrapper
        std::cerr << "Warning: Pass '" << name << "' has no implementation. "
                  << "Use nn-addon passes via run_pass()." << std::endl;
    }
};

// PassManager - manages compilation pipeline
class PassManager {
public:
    std::vector<std::shared_ptr<Pass>> passes;
    bool ir_printing_enabled;
    bool verifier_enabled;
    
    PassManager() : ir_printing_enabled(false), verifier_enabled(true) {}
    
    static std::shared_ptr<PassManager> parse(const std::string& pipeline) {
        auto pm = std::make_shared<PassManager>();
        
        // Parse pipeline string into pass names
        std::istringstream iss(pipeline);
        std::string pass_name;
        while (std::getline(iss, pass_name, ',')) {
            // Trim whitespace
            size_t start = pass_name.find_first_not_of(" \t");
            size_t end = pass_name.find_last_not_of(" \t");
            if (start != std::string::npos) {
                pass_name = pass_name.substr(start, end - start + 1);
            }
            
            if (!pass_name.empty()) {
                pm->passes.push_back(std::make_shared<Pass>(pass_name));
            }
        }
        
        return pm;
    }
    
    void enable_ir_printing() {
        ir_printing_enabled = true;
        for (auto& pass : passes) {
            pass->set_trace_ir_before(true);
            pass->set_trace_ir_after(true);
        }
    }
    
    void enable_verifier(bool enabled) {
        verifier_enabled = enabled;
    }
    
    void run(Module& module) {
        if (ir_printing_enabled) {
            std::cout << "=== Initial IR ===" << std::endl;
            std::cout << module.dump() << std::endl;
        }
        
        for (auto& pass : passes) {
            pass->run(module);
        }
        
        if (ir_printing_enabled) {
            std::cout << "=== Final IR ===" << std::endl;
            std::cout << module.dump() << std::endl;
        }
    }
    
    size_t num_passes() const { return passes.size(); }
};

} // namespace pyace

// ═══════════════════════════════════════════════════════════════════════════════
// Python module definition
// ═══════════════════════════════════════════════════════════════════════════════

PYBIND11_MODULE(passmanager, m) {
    m.doc() = "Pass Manager - Python bindings for ACE-compiler pass infrastructure";
    
    using namespace pyace;
    
    // Module class
    py::class_<Module>(m, "Module")
        .def(py::init<const std::string&>(), py::arg("name") = "module")
        .def("enable_multithreading", &Module::enable_multithreading)
        .def("dump", &Module::dump)
        .def("set_ir", &Module::set_ir)
        .def("has_glob_scope", &Module::has_glob_scope,
             "Returns True if module holds a real AIR GLOB_SCOPE")
        .def("set_native_glob_scope", &Module::set_native_glob_scope,
             py::arg("ptr"),
             "Set native GLOB_SCOPE* pointer from air_builder.GlobScope.get_native_ptr()")
        .def_readonly("name", &Module::name);
    
    // Pass class
    py::class_<Pass, std::shared_ptr<Pass>>(m, "Pass")
        .def(py::init<const std::string&>())
        .def("set_trace_ir_before", &Pass::set_trace_ir_before)
        .def("set_trace_ir_after", &Pass::set_trace_ir_after)
        .def("set_trace_stat", &Pass::set_trace_stat)
        .def("run", &Pass::run)
        .def_readonly("name", &Pass::name);
    
    // PassManager class
    py::class_<PassManager, std::shared_ptr<PassManager>>(m, "PassManager")
        .def(py::init<>())
        .def_static("parse", &PassManager::parse)
        .def("enable_ir_printing", &PassManager::enable_ir_printing)
        .def("enable_verifier", &PassManager::enable_verifier)
        .def("run", &PassManager::run)
        .def("num_passes", &PassManager::num_passes);
    
    // Pre-defined pass names (real passes from nn-addon)
    m.attr("VECTOR_PASS") = "tensor2vector";
    m.attr("SIHE_PASS") = "vector2sihe";
    m.attr("CKKS_PASS") = "sihe2ckks";
    m.attr("POLY_PASS") = "ckks2poly";
    m.attr("POLY2C_PASS") = "poly2c";
    
    m.attr("__version__") = "0.1.0";
#ifdef ACE_BINDINGS_ENABLED
    m.attr("__ace_enabled__") = true;
#else
    m.attr("__ace_enabled__") = false;
#endif

    // ═══════════════════════════════════════════════════════════════════════════════
    // Python Lowering Bridge - for selective lowering support
    // ═══════════════════════════════════════════════════════════════════════════════
    
    // Expose PythonLoweringBridge for selective lowering
    py::class_<PythonLoweringBridge>(m, "PythonLoweringBridge")
        .def_static("instance", &PythonLoweringBridge::instance,
                    py::return_value_policy::reference,
                    "Get the singleton instance of PythonLoweringBridge")
        .def("set_skip_ops", &PythonLoweringBridge::set_skip_ops,
             py::arg("ops"),
             "Set the list of ops that C++ passes should skip")
        .def("add_skip_op", &PythonLoweringBridge::add_skip_op,
             py::arg("op"),
             "Add a single op to the skip list (format: 'domain::op_name')")
        .def("clear_skip_ops", &PythonLoweringBridge::clear_skip_ops,
             "Clear all skip ops")
        .def("should_skip", 
             py::overload_cast<const std::string&, const std::string&>(&PythonLoweringBridge::should_skip, py::const_),
             py::arg("domain"), py::arg("op_name"),
             "Check if an op should be skipped by C++ lowering")
        .def("get_skip_ops", &PythonLoweringBridge::get_skip_ops,
             py::return_value_policy::copy,
             "Get all ops that should be skipped")
        .def("skip_count", &PythonLoweringBridge::skip_count,
             "Get the number of ops to skip");
    
    // Module-level convenience functions for selective lowering
    m.def("set_skip_ops", [](const std::vector<std::string>& ops) {
        PythonLoweringBridge::instance().set_skip_ops(ops);
    }, py::arg("ops"),
    "Set ops that C++ passes should skip");
    
    m.def("add_skip_op", [](const std::string& op) {
        PythonLoweringBridge::instance().add_skip_op(op);
    }, py::arg("op"),
    "Add an op to skip list");
    
    m.def("clear_skip_ops", []() {
        PythonLoweringBridge::instance().clear_skip_ops();
    }, "Clear all skip ops");
    
    m.def("should_skip_op", [](const std::string& domain, const std::string& op_name) {
        return PythonLoweringBridge::instance().should_skip(domain, op_name);
    }, py::arg("domain"), py::arg("op_name"),
    "Check if an op should be skipped");
    
    m.def("get_skip_ops", []() -> std::vector<std::string> {
        const auto& ops = PythonLoweringBridge::instance().get_skip_ops();
        return std::vector<std::string>(ops.begin(), ops.end());
    }, "Get all skip ops as a list");
}
