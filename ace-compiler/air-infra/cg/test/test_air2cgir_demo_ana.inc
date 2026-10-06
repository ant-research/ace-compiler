//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/cg/visitor.h"
#include "air/cg/analyze_ctx.h"
#include "test_aircg_demo_isa.h"

namespace air {

namespace cg {

namespace demo {

// demo an ANALYZE_CTX without handlers. This CTX needs to implement Handle_inst()
class DEMO_NO_HANDLER_ANA_CTX : public air::cg::ANALYZE_CTX {
public:
  // for a visit without any handler, CTX must implemement Handle_inst().
  template <typename VISITOR>
  void Handle_inst(VISITOR* visitor, INST_PTR inst) {
    inst->Print();
  }
};

// demo an ANALYZE_CTX with handler. All instructions will be dispatched
// to handler. Nothing to be done in the CTX
class DEMO_WITH_HANDLER_CTX : public air::cg::ANALYZE_CTX {
public:

};

// demo an ANALYZE_CTX with a single handler. All instructions will be dispatched
// to this handler.
class DEMO_MONO_ANA_HANDLER {
public:
  // for a visit with a single handler, HANDLER must implement Handle().
  template<typename VISITOR>
  void Handle(VISITOR* visitor, INST_PTR inst) {
    std::cout << inst->Isa() << " " << inst->To_str();
  }
};

// demo an ANALYZE_CTX with multiple handlers. Instructions will be dispatched to
// different handlers according to their ISA
class DEMO_ISA0_ANA_HANDLER {
public:
  // HANDLER must define a constant "ID" to represent the ISA
  static constexpr uint32_t ID = 0;

  // HANDLER must implement Handle(). All instructions whose ISA matches with ID will
  // be dispatch to this handler
  template<typename VISITOR>
  void Handle(VISITOR* visitor, INST_PTR inst) {
    AIR_ASSERT(inst->Isa() == ID);
  }
};

// demo an ANALYZE_CTX with multiple handlers. Instructions will be dispatched to
// different handlers according to their ISA
class DEMO_ISA1_ANA_HANDLER {
public:
  // HANDLER must define a constant "ID" to represent the ISA
  static constexpr uint32_t ID = 1;

  // HANDLER must implement Handle(). All instructions whose ISA matches with ID will
  // be dispatch to this handler
  template<typename VISITOR>
  void Handle(VISITOR* visitor, INST_PTR inst) {
    AIR_ASSERT(inst->Isa() == ID);
  }
};

// demo how to use ANALYZE_CTX without HANDLERs
static void No_handler_analyze(const air::cg::CGIR_CONTAINER* cg_cont) {
  std::cout << "Test Analyze CGIR using visitor-handler-context without handler\n";
  DEMO_NO_HANDLER_ANA_CTX ctx;
  air::cg::VISITOR<DEMO_NO_HANDLER_ANA_CTX> trav(ctx);
  trav.template Visit<LAYOUT_ITER, INST_ITER>(LAYOUT_ITER(cg_cont));
}

// demo how to use ANALYZE_CTX with a single HANDLER
static void Mono_handler_analyze(const air::cg::CGIR_CONTAINER* cg_cont) {
  std::cout << "Test Analyze CGIR using visitor-handler-context with monolithic handler\n";
  DEMO_WITH_HANDLER_CTX ctx;
  air::cg::VISITOR<DEMO_WITH_HANDLER_CTX,
                   DEMO_MONO_ANA_HANDLER>
                   trav(ctx);
  trav.template Visit<LAYOUT_ITER, INST_ITER>(LAYOUT_ITER(cg_cont));
}

// demo how to use ANALYZE_CTX with multiple HANDLERs
static void Multi_handler_analyze(const air::cg::CGIR_CONTAINER* cg_cont) {
  std::cout << "Test Aanalyze CGIR using visitor-handler-context with multiple handlers\n";
  DEMO_WITH_HANDLER_CTX ctx;
  air::cg::VISITOR<DEMO_WITH_HANDLER_CTX,
                   DEMO_ISA0_ANA_HANDLER, DEMO_ISA1_ANA_HANDLER> trav(ctx);
  trav.template Visit<LAYOUT_ITER, INST_ITER>(LAYOUT_ITER(cg_cont));
}

// main entry for Analyze_cgir() to demo how to use ANALYZE_CTX and HANDLERs
static void Analyze_cgir(const air::cg::CGIR_CONTAINER* cg_cont) {
  No_handler_analyze(cg_cont);
  Mono_handler_analyze(cg_cont);
  Multi_handler_analyze(cg_cont);
}

}  // namespace demo

}  // namespace cg

}  // namespace air
