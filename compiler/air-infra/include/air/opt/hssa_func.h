//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OPT_HSSA_FUNC_H
#define AIR_OPT_HSSA_FUNC_H

#include "air/base/st.h"
#include "air/driver/driver_ctx.h"
#include "air/opt/cfg.h"
#include "air/opt/cfg_decl.h"
#include "air/opt/hssa_builder.h"
#include "air/opt/hssa_container.h"
#include "air/opt/hssa_decl.h"
#include "air/opt/ssa_build.h"
#include "air/util/mem_pool.h"

namespace air {
namespace opt {
class HSSA_FUNC {
  typedef air::util::MEM_POOL<4096> HSSA_FUNC_MPOOL;

public:
  HSSA_FUNC(air::base::FUNC_SCOPE* fscope) : _ma(&_mpool) {
    _input_fscope  = fscope;
    _output_fscope = nullptr;
    _hssa_cont     = nullptr;
    _cfg           = nullptr;
  }

  CFG&                   Cfg() const { return *_cfg; }
  air::base::FUNC_SCOPE* Input_fscope() const { return _input_fscope; }
  air::base::FUNC_SCOPE* Output_fscope() const { return _output_fscope; }
  void                   Set_output_fscope(air::base::FUNC_SCOPE* fscope) {
    _output_fscope = fscope;
  }
  HCONTAINER* Hssa_cont() const { return _hssa_cont; }

  air::base::CONTAINER* Input_cont() const {
    return &(Input_fscope()->Container());
  }
  air::base::CONTAINER* Output_cont() const {
    return &(Output_fscope()->Container());
  }

  template <typename BUILDER_CTX>
  void Build(BUILDER_CTX& build_ctx);
  void Emit(air::base::GLOB_SCOPE* glob);

private:
  CFG* Cfg_ptr() const { return _cfg; }

  HSSA_FUNC_MPOOL                           _mpool;
  air::util::MEM_ALLOCATOR<HSSA_FUNC_MPOOL> _ma;
  air::base::FUNC_SCOPE*                    _input_fscope;
  air::base::FUNC_SCOPE*                    _output_fscope;
  HCONTAINER*                               _hssa_cont;
  CFG*                                      _cfg;
};

template <typename HSSA_VISITOR>
void HSSA_FUNC::Build(HSSA_VISITOR& visitor) {
  // build ssa
  air::driver::DRIVER_CTX driver_ctx;
  air::base::FUNC_SCOPE*  fscope = Input_fscope();
  air::base::CONTAINER*   cont   = Input_cont();
  air::opt::SSA_CONTAINER ssa_cont(cont);
  air::opt::SSA_BUILDER   ssa_builder(fscope, &ssa_cont, &driver_ctx);
  ssa_builder.Perform();
  // std::cout << "IR after SSA construction:" << std::endl;
  // ssa_cont.Print_tree(cont->Entry_node()->Id());

  // build hssa & cfg
  _hssa_cont = _ma.Allocate<HCONTAINER>(cont, &ssa_cont);
  _cfg       = _ma.Allocate<CFG>(_hssa_cont);

  HSSA_BUILDER hssa_builder(fscope, *_hssa_cont, ssa_cont, *_cfg, &driver_ctx);
  hssa_builder.Run(visitor);
  std::cout.unsetf(std::ios::hex);
  _cfg->Build_dom_info();
}

}  // namespace opt
}  // namespace air
#endif