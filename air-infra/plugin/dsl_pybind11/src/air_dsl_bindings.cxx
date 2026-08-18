// example_bindings.cpp
#include "dsl.hpp"
#include "vector/vector_ops.h"
#include <pybind11/pybind11.h>

namespace py = pybind11;
using namespace air::dsl;

#define ADD_PRIMTYPE_ENUM(name) value(#name, PRIMITIVE_TYPE::name)
#define ADD_SIMPLE_CLASS(name, py_name) py::class_<name>(m, #py_name)
#define ADD_TYPE_CLASS(name, py_name) py::class_<name, TYPE_PTR>(m, #py_name)

// Wrap C++ classes and member functions as Python modules
PYBIND11_MODULE(air_dsl, m) {
  py::class_<GLOB_SCOPE>(m, "GlobScope");

  py::class_<SPOS>(m, "SPOS").def(
      py::init<uint32_t, uint32_t, uint32_t, uint32_t>());

  py::class_<TYPE>(m, "Type");
  py::class_<PRIM_TYPE, TYPE>(m, "PrimType");
  py::class_<ARRAY_TYPE, TYPE>(m, "ArrayType");
  py::class_<POINTER_TYPE, TYPE>(m, "PointerType");

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

  ADD_SIMPLE_CLASS(TYPE_PTR, TypePtr);
  ADD_SIMPLE_CLASS(ARRAY_TYPE_PTR, ArrayTypePtr);
  ADD_SIMPLE_CLASS(POINTER_TYPE_PTR, PointerTypePtr);
  ADD_SIMPLE_CLASS(PRIM_TYPE_PTR, PrimTypePtr);
  ADD_SIMPLE_CLASS(RECORD_TYPE_PTR, RecordTypePtr);

  ADD_SIMPLE_CLASS(NODE_PTR, NodePtr);
  ADD_SIMPLE_CLASS(STMT_PTR, StmtPtr);

  ADD_SIMPLE_CLASS(FUNC_SCOPE, FuncScope)
      .def("Formal", &FUNC_SCOPE::Formal, py::return_value_policy::reference)
      .def("toString", &FUNC_SCOPE::To_str, py::arg("rot") = true,
           py::return_value_policy::reference);

  py::class_<DSL>(m, "DSL")
      .def(py::init<>())
      .def("add", &DSL::add)
      .def("sub", &DSL::sub)
      .def("mul", &DSL::mul)
      .def("BinOp", &DSL::BinOp)
      .def("Assign", &DSL::Assign)
      .def("Constant", &DSL::Constant)
      .def("getGlobalScope", &DSL::getGlobalScope,
           py::return_value_policy::reference)
      .def("getPrimType", &DSL::getPrimType, py::return_value_policy::reference)
      .def("getArrayType", &DSL::getArrayType,
           py::return_value_policy::reference)
      .def("newFunc", &DSL::newFunc, py::arg("name"), py::arg("spos"),
           py::arg("with_scope") = true, py::return_value_policy::reference)
      .def("newFuncScope", &DSL::newFuncScope,
           py::return_value_policy::reference)
      .def("newEntryPoint", &DSL::newEntryPoint,
           py::return_value_policy::reference)
      .def("newSigType", &DSL::newSigType, py::return_value_policy::reference)
      .def("newVar", &DSL::newVar, py::return_value_policy::reference)
      .def("Formal", &DSL::Formal, py::return_value_policy::reference)
      .def("Ld", &DSL::newLd, py::return_value_policy::reference)
      .def("St", &DSL::newSt, py::return_value_policy::reference)
      .def("Retv", &DSL::newRETV, py::return_value_policy::reference)
      .def("addParm", &DSL::addParm, py::return_value_policy::reference)
      .def("addRet", &DSL::addRet, py::return_value_policy::reference)
      .def("getCurFuncScope", &DSL::getCurFuncScope,
           py::return_value_policy::reference)
      .def("setSigComplete", &DSL::setSigComplete);

  py::class_<VECTOR_API>(m, "VectorAPI")
      .def(py::init<DSL &>())
      .def("add", &VECTOR_API::Add, py::return_value_policy::reference);

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
      .ADD_PRIMTYPE_ENUM(END);
}