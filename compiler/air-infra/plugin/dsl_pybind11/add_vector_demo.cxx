#include <pybind11/embed.h>
#include <pybind11/pybind11.h>

#include <iomanip>

#include "air/base/container.h"
#include "air/base/meta_info.h"
#include "air/base/opcode.h"
#include "air/base/st.h"
#include "air/core/opcode.h"
#include "nn/vector/vector_opcode.h"

namespace py = pybind11;
using namespace air::base;
using namespace air::core;
using namespace air::util;

int main() {
  // start the interpreter and keep it alive

  META_INFO::Remove_all();
  air::core::Register_core();
  nn::vector::Register_vector_domain();
  std::cout << "Domain number" << META_INFO::Num_domain() << std::endl;
  AIR_ASSERT(META_INFO::Valid_domain(0));
  GLOB_SCOPE* glob = new GLOB_SCOPE(0, true);
  SPOS        spos(0, 1, 1, 0);

  STR_PTR  str_add  = glob->New_str("add");
  FUNC_PTR func_add = glob->New_func(str_add, spos);
  func_add->Set_parent(glob->Comp_env_id());
  FUNC_SCOPE* fs = &glob->New_func_scope(func_add);

  py::scoped_interpreter guard{};
  py::module             m_instantiator = py::module::import("instantiator");
  py::module             m_dsl          = py::module::import("air_dsl");
  py::object             py_fs          = py::cast(fs);
  auto                   add_vector     = m_instantiator.attr("add_vector");
  add_vector(py_fs);
  fs->Print();
  py::print("Hello, World!");
}
