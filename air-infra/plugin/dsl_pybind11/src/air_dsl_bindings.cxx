// example_bindings.cpp
#include <pybind11/pybind11.h>

#include "nn/core/nn_ops.h"
#include "py_airgen.h"
#include "pycontext.h"
#include "vector/vector_ops.h"
// clang-format off
#include "nn/vector/tensor2vector_handler.h"
// clang-format on
#include "fhe/ckks/ckks_ops.h"

namespace py = pybind11;
using namespace air::dsl;

#define ADD_PRIMTYPE_ENUM(name)         value(#name, PRIMITIVE_TYPE::name)
#define ADD_SIMPLE_CLASS(name, py_name) py::class_<name>(m, #py_name)
#define ADD_TYPE_CLASS(name, py_name)   py::class_<name, TYPE_PTR>(m, #py_name)

// Wrap C++ classes and member functions as Python modules
PYBIND11_MODULE(air_dsl, m) {
  py::class_<GLOB_SCOPE>(m, "GlobScope");

  py::class_<SPOS>(m, "SPOS").def(
      py::init<uint32_t, uint32_t, uint32_t, uint32_t>());

  py::class_<TYPE>(m, "Type");
  py::class_<PRIM_TYPE, TYPE>(m, "PrimType");
  py::class_<ARRAY_TYPE, TYPE>(m, "ArrayType");
  py::class_<POINTER_TYPE, TYPE>(m, "PointerType");

  py::class_<TRANSFORM_CTX>(m, "TransformCtx");
  py::class_<nn::vector::TENSOR2VECTOR_CTX, TRANSFORM_CTX>(m, "DslLowerCtx");

  // bind pointer classes
  ADD_SIMPLE_CLASS(ADDR_DATUM_PTR, AddrDatumPtr);
  ADD_SIMPLE_CLASS(BLOCK_PTR, BlockPtr);
  // ADD_SIMPLE_CLASS(COND_PTR, CondPtr);
  ADD_SIMPLE_CLASS(ENTRY_PTR, EntryPtr);
  ADD_SIMPLE_CLASS(FIELD_PTR, FieldPtr);
  ADD_SIMPLE_CLASS(FUNC_PTR, FuncPtr);
  // ADD_SIMPLE_CLASS(LABEL_PTR, LabelPtr);
  ADD_SIMPLE_CLASS(PARAM_PTR, ParamPtr);
  // ADD_SIMPLE_CLASS(REGION_INFO_PTR, RegionInfoPtr);
  ADD_SIMPLE_CLASS(FILE_PTR, FilePtr);
  ADD_SIMPLE_CLASS(STR_PTR, StrPtr);
  ADD_SIMPLE_CLASS(SUBTYPE_PTR, SubTypePtr);
  ADD_SIMPLE_CLASS(SYM_PTR, SymPtr);
  ADD_SIMPLE_CLASS(PREG_PTR, PregPtr);
  ADD_SIMPLE_CLASS(SIGNATURE_TYPE_PTR, SignatureTypePtr);
  ADD_SIMPLE_CLASS(VA_LIST_TYPE_PTR, VaListTypePtr);
  ADD_SIMPLE_CLASS(PACKET_PTR, PacketPtr);
  ADD_SIMPLE_CLASS(ARB_PTR, ArbPtr);
  ADD_SIMPLE_CLASS(ATTR_PTR, AttrPtr);
  ADD_SIMPLE_CLASS(CONSTANT_PTR, ConstantPtr);

  ADD_SIMPLE_CLASS(TYPE_PTR, TypePtr);
  ADD_SIMPLE_CLASS(ARRAY_TYPE_PTR, ArrayTypePtr);
  ADD_SIMPLE_CLASS(POINTER_TYPE_PTR, PointerTypePtr);
  ADD_SIMPLE_CLASS(PRIM_TYPE_PTR, PrimTypePtr);
  ADD_SIMPLE_CLASS(RECORD_TYPE_PTR, RecordTypePtr);
  ADD_SIMPLE_CLASS(CONST_TYPE_PTR, ConstTypePtr);

  ADD_SIMPLE_CLASS(STMT_PTR, StmtPtr);
  ADD_SIMPLE_CLASS(NODE_PTR, NodePtr)
      .def(
          "Spos", [](NODE_PTR& n) { return n->Spos(); },
          py::return_value_policy::copy);

  ADD_SIMPLE_CLASS(FUNC_SCOPE, FuncScope)
      .def("Formal", &FUNC_SCOPE::Formal, py::return_value_policy::reference)
      .def("toString", &FUNC_SCOPE::To_str, py::arg("rot") = true,
           py::return_value_policy::reference);

  py::class_<PY_AIRGEN>(m, "DSL")
      .def(py::init<>())
      .def(py::init<FUNC_SCOPE&, NODE_PTR>())
      .def("get_global_scope", &PY_AIRGEN::Glob_scope,
           py::return_value_policy::reference)
      .def("get_prim_type", &PY_AIRGEN::Get_prim_type,
           py::return_value_policy::reference)
      .def("get_array_type", &PY_AIRGEN::Get_array_type,
           py::return_value_policy::reference)
      .def("new_func", &PY_AIRGEN::New_func, py::arg("name"), py::arg("spos"),
           py::arg("with_scope") = true, py::return_value_policy::reference)
      .def("new_func_scope", &PY_AIRGEN::New_func_scope,
           py::return_value_policy::reference)
      .def("new_entry_point", &PY_AIRGEN::New_entry_point,
           py::return_value_policy::reference)
      .def("new_sig_type", &PY_AIRGEN::New_sig_point,
           py::return_value_policy::reference)
      .def("new_var", &PY_AIRGEN::New_var, py::return_value_policy::reference)
      .def("new_float_array_const", &PY_AIRGEN::New_float_array_const,
           py::return_value_policy::reference)
      .def("new_int_array_const", &PY_AIRGEN::New_int_array_const,
           py::return_value_policy::reference)
      .def("formal", &PY_AIRGEN::Formal, py::return_value_policy::reference)
      .def("ld_var", &PY_AIRGEN::Ld_var, py::return_value_policy::reference)
      .def("call", &PY_AIRGEN::New_call, py::return_value_policy::reference)
      .def("zero", &PY_AIRGEN::New_zero, py::return_value_policy::reference)
      .def("intconst", &PY_AIRGEN::New_intconst,
           py::return_value_policy::reference)
      .def("ld", &PY_AIRGEN::New_ld, py::return_value_policy::reference)
      .def("add", &PY_AIRGEN::New_add, py::return_value_policy::reference)
      .def("mul", &PY_AIRGEN::New_mul, py::return_value_policy::reference)
      .def("ldc", &PY_AIRGEN::New_ldc, py::return_value_policy::reference)
      .def("block", &PY_AIRGEN::New_stmt_block,
           py::return_value_policy::reference)
      .def("st", &PY_AIRGEN::New_st, py::return_value_policy::reference)
      .def("lt", &PY_AIRGEN::New_lt, py::return_value_policy::reference)
      .def("retv", &PY_AIRGEN::New_retv, py::return_value_policy::reference)
      .def("do_loop", &PY_AIRGEN::New_do_loop,
           py::return_value_policy::reference)
      .def("block_append", &PY_AIRGEN::Block_append)
      .def("add_parm", &PY_AIRGEN::Add_parm, py::return_value_policy::reference)
      .def("add_ret", &PY_AIRGEN::Add_ret, py::return_value_policy::reference)
      .def("get_cur_func_scope", &PY_AIRGEN::Get_cur_func_scope,
           py::return_value_policy::reference)
      .def("get_rtype", &PY_AIRGEN::Get_rtype,
           py::return_value_policy::reference)
      .def("set_cur_funcScope", &PY_AIRGEN::Set_cur_func_scope)
      .def("set_sig_complete", &PY_AIRGEN::Set_sig_complete)
      .def("owning_func", &PY_AIRGEN::Owning_func,
           py::return_value_policy::reference)
      .def("append", &PY_AIRGEN::Append)
      .def("set_blk_ctx", &PY_AIRGEN::Set_blk_ctx)
      .def("get_blk_ctx", &PY_AIRGEN::Get_blk_ctx,
           py::return_value_policy::reference)
      .def("print", &PY_AIRGEN::Print, py::return_value_policy::reference)
      .def("print_glob", &PY_AIRGEN::Print_glob,
           py::return_value_policy::reference)
      .def("clone_exp", &PY_AIRGEN::Clone_exp,
           py::return_value_policy::reference)
      .def("new_array",
           py::overload_cast<CONSTANT_PTR, NODE_PTR&, const SPOS&>(
               &PY_AIRGEN::New_array),
           py::return_value_policy::reference)
      .def("new_array",
           py::overload_cast<ADDR_DATUM_PTR, NODE_PTR&, const SPOS&>(
               &PY_AIRGEN::New_array),
           py::return_value_policy::reference)
      .def("append_block", &PY_AIRGEN::Append_block)
      .def("new_ild", &PY_AIRGEN::New_ild, py::return_value_policy::reference)
      .def("new_ist", &PY_AIRGEN::New_ist, py::return_value_policy::reference);

  py::class_<PYCONTEXT>(m, "PyContext")
      .def(py::init<NODE_PTR>())
      .def("get_array_nchw", &PYCONTEXT::Get_array_nchw,
           py::return_value_policy::copy)
      .def("rtype", &PYCONTEXT::Rtype, py::return_value_policy::copy)
      .def("strides", &PYCONTEXT::Strides, py::return_value_policy::copy)
      .def("pads", &PYCONTEXT::Pads, py::return_value_policy::copy)
      .def("group", &PYCONTEXT::Group, py::return_value_policy::copy)
      .def("get_cur_node", &PYCONTEXT::Get_cur_node,
           py::return_value_policy::reference)
      .def("get_float_const_array", &PYCONTEXT::Get_float_const_array,
           py::return_value_policy::copy)
      .def("get_elem_type", &PYCONTEXT::Get_elem_type,
           py::return_value_policy::reference)
      .def("get_elem_const_type", &PYCONTEXT::Get_elem_const_type,
           py::return_value_policy::reference);

  py::class_<VECTOR_API>(m, "VectorAPI")
      .def(py::init<PY_AIRGEN&>())
      .def("add", &VECTOR_API::Add, py::return_value_policy::reference)
      .def("mul", &VECTOR_API::Mul, py::return_value_policy::reference)
      .def("roll", &VECTOR_API::Roll, py::return_value_policy::reference)
      .def("reshape", &VECTOR_API::Reshape, py::return_value_policy::reference)
      .def("slice", &VECTOR_API::Slice, py::return_value_policy::reference);

// import CKKS dsl binding
#include "fhe/ckks/ckks_binding.h"

// import NN dsl binding
#include "nn/core/nn_binding.h"

  py::class_<NODE>(m, "Node").def("spos", &NODE::Spos,
                                  py::return_value_policy::copy);

  py::enum_<PRIMITIVE_TYPE>(m, "PrimTypeEnum")
      .ADD_PRIMTYPE_ENUM(INT_S8)
      .ADD_PRIMTYPE_ENUM(INT_S16)
      .ADD_PRIMTYPE_ENUM(INT_S32)
      .ADD_PRIMTYPE_ENUM(INT_S64)
      .ADD_PRIMTYPE_ENUM(INT_U8)
      .ADD_PRIMTYPE_ENUM(INT_U16)
      .ADD_PRIMTYPE_ENUM(INT_U32)
      .ADD_PRIMTYPE_ENUM(INT_U64)
      .ADD_PRIMTYPE_ENUM(FLOAT_32)
      .ADD_PRIMTYPE_ENUM(FLOAT_64)
      .ADD_PRIMTYPE_ENUM(FLOAT_80)
      .ADD_PRIMTYPE_ENUM(FLOAT_128)
      .ADD_PRIMTYPE_ENUM(COMPLEX_32)
      .ADD_PRIMTYPE_ENUM(COMPLEX_64)
      .ADD_PRIMTYPE_ENUM(COMPLEX_80)
      .ADD_PRIMTYPE_ENUM(COMPLEX_128)
      .ADD_PRIMTYPE_ENUM(VOID)
      .ADD_PRIMTYPE_ENUM(BOOL)
      .ADD_PRIMTYPE_ENUM(END)
      .export_values();
}
