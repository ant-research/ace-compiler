//
// fhe_cmplr_bindings.cpp - pybind11 bindings for FHE domain opcodes
//
// This module provides Python access to FHE domain opcode constants:
// - fhe::sihe operations (scheme-independent HE)
// - fhe::ckks operations (CKKS-specific)
// - fhe::poly operations (polynomial operations)
//
// Note: For actual FHE IR building, use air_builder's Container which
// integrates with the real ACE compiler.

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

// ═══════════════════════════════════════════════════════════════════════════════
// Python module definition
// ═══════════════════════════════════════════════════════════════════════════════

PYBIND11_MODULE(fhe_cmplr, m) {
    m.doc() = "FHE Compiler - Python bindings for ACE-compiler FHE opcode constants";
    
    // Submodules for opcodes
    auto sihe = m.def_submodule("sihe", "fhe::sihe opcodes (scheme-independent HE)");
    sihe.attr("ENCODE") = "fhe::sihe::ENCODE";
    sihe.attr("ADD") = "fhe::sihe::ADD";
    sihe.attr("SUB") = "fhe::sihe::SUB";
    sihe.attr("MUL") = "fhe::sihe::MUL";
    sihe.attr("NEG") = "fhe::sihe::NEG";
    sihe.attr("ROTATE") = "fhe::sihe::ROTATE";
    sihe.attr("BOOTSTRAP") = "fhe::sihe::BOOTSTRAP";
    // Runtime validation
    sihe.attr("ROTATE_MSG") = "fhe::sihe::ROTATE_MSG";
    sihe.attr("ADD_MSG") = "fhe::sihe::ADD_MSG";
    sihe.attr("MUL_MSG") = "fhe::sihe::MUL_MSG";
    sihe.attr("RELU_MSG") = "fhe::sihe::RELU_MSG";
    sihe.attr("BOOTSTRAP_MSG") = "fhe::sihe::BOOTSTRAP_MSG";
    
    auto ckks = m.def_submodule("ckks", "fhe::ckks opcodes (CKKS-specific)");
    ckks.attr("ENCODE") = "fhe::ckks::ENCODE";
    ckks.attr("ADD") = "fhe::ckks::ADD";
    ckks.attr("SUB") = "fhe::ckks::SUB";
    ckks.attr("MUL") = "fhe::ckks::MUL";
    ckks.attr("NEG") = "fhe::ckks::NEG";
    ckks.attr("ROTATE") = "fhe::ckks::ROTATE";
    ckks.attr("RESCALE") = "fhe::ckks::RESCALE";
    ckks.attr("RELIN") = "fhe::ckks::RELIN";
    ckks.attr("MOD_SWITCH") = "fhe::ckks::MOD_SWITCH";
    ckks.attr("UPSCALE") = "fhe::ckks::UPSCALE";
    ckks.attr("BOOTSTRAP") = "fhe::ckks::BOOTSTRAP";
    ckks.attr("SCALE") = "fhe::ckks::SCALE";
    ckks.attr("LEVEL") = "fhe::ckks::LEVEL";
    ckks.attr("BATCH_SIZE") = "fhe::ckks::BATCH_SIZE";
    
    auto poly = m.def_submodule("poly", "fhe::poly opcodes (polynomial operations)");
    // Memory
    poly.attr("ALLOC") = "fhe::poly::ALLOC";
    poly.attr("FREE") = "fhe::poly::FREE";
    poly.attr("INIT_CIPH_SAME_SCALE") = "fhe::poly::INIT_CIPH_SAME_SCALE";
    poly.attr("INIT_CIPH_UP_SCALE") = "fhe::poly::INIT_CIPH_UP_SCALE";
    poly.attr("INIT_CIPH_DOWN_SCALE") = "fhe::poly::INIT_CIPH_DOWN_SCALE";
    // Arithmetic
    poly.attr("ADD") = "fhe::poly::ADD";
    poly.attr("SUB") = "fhe::poly::SUB";
    poly.attr("MUL") = "fhe::poly::MUL";
    poly.attr("ADD_EXT") = "fhe::poly::ADD_EXT";
    poly.attr("SUB_EXT") = "fhe::poly::SUB_EXT";
    poly.attr("MUL_EXT") = "fhe::poly::MUL_EXT";
    // Transforms
    poly.attr("NTT") = "fhe::poly::NTT";
    poly.attr("INTT") = "fhe::poly::INTT";
    poly.attr("RESCALE") = "fhe::poly::RESCALE";
    poly.attr("ROTATE") = "fhe::poly::ROTATE";
    poly.attr("DECOMP") = "fhe::poly::DECOMP";
    poly.attr("DECOMP_MODUP") = "fhe::poly::DECOMP_MODUP";
    poly.attr("AUTO_ORDER") = "fhe::poly::AUTO_ORDER";
    poly.attr("MOD_UP") = "fhe::poly::MOD_UP";
    poly.attr("MOD_DOWN") = "fhe::poly::MOD_DOWN";
    poly.attr("EXTRACT") = "fhe::poly::EXTRACT";
    // Field access
    poly.attr("COEFFS") = "fhe::poly::COEFFS";
    poly.attr("SET_COEFFS") = "fhe::poly::SET_COEFFS";
    poly.attr("LEVEL") = "fhe::poly::LEVEL";
    poly.attr("SET_LEVEL") = "fhe::poly::SET_LEVEL";
    poly.attr("NUM_P") = "fhe::poly::NUM_P";
    poly.attr("NUM_ALLOC") = "fhe::poly::NUM_ALLOC";
    poly.attr("NUM_DECOMP") = "fhe::poly::NUM_DECOMP";
    // Scheme properties
    poly.attr("DEGREE") = "fhe::poly::DEGREE";
    poly.attr("Q_MODULUS") = "fhe::poly::Q_MODULUS";
    poly.attr("P_MODULUS") = "fhe::poly::P_MODULUS";
    // Key operations
    poly.attr("SWK") = "fhe::poly::SWK";
    poly.attr("PK0_AT") = "fhe::poly::PK0_AT";
    poly.attr("PK1_AT") = "fhe::poly::PK1_AT";
    // Hardware operations
    poly.attr("HW_NTT") = "fhe::poly::HW_NTT";
    poly.attr("HW_INTT") = "fhe::poly::HW_INTT";
    poly.attr("HW_MODADD") = "fhe::poly::HW_MODADD";
    poly.attr("HW_MODSUB") = "fhe::poly::HW_MODSUB";
    poly.attr("HW_MODMUL") = "fhe::poly::HW_MODMUL";
    poly.attr("HW_ROTATE") = "fhe::poly::HW_ROTATE";
    
    m.attr("__version__") = "0.1.0";
    m.attr("__is_mock__") = false;
}
