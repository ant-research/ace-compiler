// -*-c++-*- ===================================================================
//
// Part of the AVH_Compiler Project, under the Apache License v2.0.
// See http://avhc.org/LICENSE/txt for license information.
//
// =============================================================================

#ifndef COPY_PROP_H
#define COPY_PROP_H
#include <unordered_map>

#include "air/base/analyze_ctx.h"
#include "air/base/container.h"
#include "air/base/handler_retv.h"
#include "air/base/st.h"
#include "air/base/visitor.h"
#include "air/core/default_handler.h"
#include "air/core/handler.h"
#include "air/core/opcode.h"
#include "air/driver/driver_ctx.h"
#include "nn/core/default_handler.h"
#include "nn/core/handler.h"

namespace nn {
namespace opt {
using namespace air::base;

extern GLOB_SCOPE* Opt_perform_copy_propagation(
    GLOB_SCOPE* glob, const air::driver::DRIVER_CTX* driver_ctx);

class CP_CTX {
public:
  explicit CP_CTX(air::base::ANALYZE_CTX* ctx) : _ana_ctx(ctx) {}

  ~CP_CTX() {}

  air::base::ANALYZE_CTX* Ana_ctx() const { return _ana_ctx; }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  CP_CTX(void);
  CP_CTX(const CP_CTX&);
  CP_CTX& operator=(const CP_CTX&);

  air::base::ANALYZE_CTX* _ana_ctx;
  // std::unordered_map<ADDR_DATUM_ID, STMT_ID> cp_cands;
};

class CORE_HANDLER_IMPL : public air::core::DEFAULT_HANDLER {
public:
  explicit CORE_HANDLER_IMPL(CONTAINER* cntr, CP_CTX* ctx) : _ana_ctx(ctx) {}

  ~CORE_HANDLER_IMPL() {}

  template <typename RETV, typename VISITOR>
  RETV Handle_store(VISITOR* visitor, NODE_PTR store_node);

private:
  // REQUIRED UNDEFINED UNWANTED methods
  CORE_HANDLER_IMPL(void);
  CORE_HANDLER_IMPL(const CORE_HANDLER_IMPL&);
  CORE_HANDLER_IMPL& operator=(const CORE_HANDLER_IMPL&);

  CP_CTX* _ana_ctx;
};

class NN_CORE_HANDLER_IMPL : public nn::core::DEFAULT_HANDLER {
public:
  NN_CORE_HANDLER_IMPL(CONTAINER* cntr, CP_CTX* ctx)
      : _mng_ctx(ctx), _cntr(cntr) {}

  ~NN_CORE_HANDLER_IMPL() {}

private:
  // REQUIRED UNDEFINED UNWANTED methods
  NN_CORE_HANDLER_IMPL(void);
  NN_CORE_HANDLER_IMPL(const NN_CORE_HANDLER_IMPL&);
  NN_CORE_HANDLER_IMPL& operator=(const NN_CORE_HANDLER_IMPL&);

  CP_CTX* Get_ana_ctx() const { return _mng_ctx; }

  CP_CTX*    _mng_ctx;
  CONTAINER* _cntr;  // CONTAINER of current function
};

class COPY_PROPAGATION_MANAGER {
public:
  COPY_PROPAGATION_MANAGER(FUNC_SCOPE* func_scope, air::base::ANALYZE_CTX* ctx)
      : _func_scope(func_scope), _mng_ctx(ctx) {}

  ~COPY_PROPAGATION_MANAGER() {}

  void Run() {
    // handle function body
    CORE_HANDLER_IMPL core_handler_impl(&Func_scope()->Container(), &Mng_ctx());
    using CORE_HANDLER = air::core::HANDLER<CORE_HANDLER_IMPL>;
    CORE_HANDLER         core_handler(&core_handler_impl);
    NN_CORE_HANDLER_IMPL nn_core_handler_impl(&Func_scope()->Container(),
                                              &Mng_ctx());
    using NN_CORE_HANDLER = nn::core::HANDLER<NN_CORE_HANDLER_IMPL>;
    NN_CORE_HANDLER        nn_core_handler(&nn_core_handler_impl);
    air::base::ANALYZE_CTX trav_ctx;
    using CP_VISITOR = air::base::VISITOR<air::base::ANALYZE_CTX, CORE_HANDLER,
                                          NN_CORE_HANDLER>;
    CP_VISITOR visitor(trav_ctx, {core_handler, nn_core_handler});
    NODE_PTR   func_body = Func_scope()->Container().Stmt_list().Block_node();
    visitor.template Visit<air::base::HANDLER_RETV>(func_body);
  }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  COPY_PROPAGATION_MANAGER(void);
  COPY_PROPAGATION_MANAGER(const COPY_PROPAGATION_MANAGER&);
  COPY_PROPAGATION_MANAGER& operator=(const COPY_PROPAGATION_MANAGER&);

  FUNC_SCOPE*             Func_scope() const { return _func_scope; }
  CP_CTX&                 Mng_ctx() { return _mng_ctx; }
  air::base::ANALYZE_CTX* Ana_ctx() const { return _mng_ctx.Ana_ctx(); }

  FUNC_SCOPE* _func_scope;  // function scope to fulfill scale constraints
  CP_CTX      _mng_ctx;     // context for scale manage handlers
};
}  // namespace opt
}  // namespace nn
#endif  // COPY_PROP_H
