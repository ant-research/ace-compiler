//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_DRIVER_DRIVER_H
#define AIR_DRIVER_DRIVER_H

#include "air/base/container.h"
#include "air/base/st.h"
#include "air/driver/driver_ctx.h"
#include "air/driver/pass.h"
#include "air/driver/pass_manager.h"
#include "air/util/binary/air2elf.h"
#include "air/util/binary/elf2air.h"

namespace air {
namespace driver {

//! @brief An empty driver as common base for all drivers
class DRIVER {
public:
  //! @brief Construct a new DRIVER object. If it's a standalone driver,
  //! create a DRIVER_CTX object.
  DRIVER(bool standalone) : _standalone(standalone) {
    if (_standalone) {
      _ctx = new DRIVER_CTX();
    } else {
      _ctx = nullptr;
    }
  }

  //! @brief Destroy the DRIVER object. If it's a standalone driver, destroy
  //!  the DRIVER_CTX object
  ~DRIVER() {
    if (_standalone) {
      AIR_ASSERT(_ctx != nullptr);
      delete _ctx;
    }
  }

  //! @brief Init a sub driver with pointer to up-level driver
  //! @tparam UP_DRV Type of up-level driver
  //! @param drv Pointer to up-level driver
  //! @return R_CODE during init process
  template <typename UP_DRV>
  R_CODE Init(UP_DRV* drv) {
    AIR_ASSERT(_standalone == false);
    _ctx = drv->Context();
    AIR_ASSERT(_ctx != nullptr);
    return R_CODE::NORMAL;
  }

  //! @brief Init a standalone driver with command line options
  //!  @param argc Number of command line options
  //! @param argv Command line options
  //! @return R_CODE during init process
  R_CODE Init(int argc, char** argv) {
    AIR_ASSERT(_standalone == true);
    AIR_ASSERT(_ctx != nullptr);
    return _ctx->Parse_options(argc, argv);
  }

  //! @brief Pre run all passes. do nothing here. A real driver should
  //! pre-run all passes in this function.
  //! @return R_CODE during running passes
  R_CODE Pre_run() { return R_CODE::NORMAL; }

  //! @brief Run all passes. do nothing here. A real driver should
  //! run all passes in this function.
  //! @return R_CODE during running passes
  R_CODE Run() { return R_CODE::NORMAL; }

  //! @brief Post run all passes. do nothing here. A real driver should
  //! post run all passes in this function
  void Post_run() {}

  //! @brief Finalization phase. do nothing here. A real driver should
  //! finalize all objects and passes in this function
  void Fini() {}

public:
  //!  @brief Get global scope from driver context
  air::base::GLOB_SCOPE* Glob_scope() { return _ctx->Glob_scope(); }

  //! @brief Update global scope
  void Update_glob_scope(air::base::GLOB_SCOPE* glob) {
    _ctx->Update_glob_scope(glob);
  }

  //! @brief Get trace file object
  air::util::TFILE& Tfile() { return _ctx->Tfile(); }

  //! @brief Get ostream for trace
  std::ostream& Tstream() { return _ctx->Tstream(); }

  //! @brief Get driver context
  DRIVER_CTX* Context() { return _ctx; }

public:
  //! @brief Print IR to trace file
  void Trace_ir() { Glob_scope()->Print_ir(Tstream()); }

  //! @brief Print SymTab to trace file
  void Trace_st() { Glob_scope()->Print_st(Tstream()); }

  //! @brief Print mempool info to trace file
  void Trace_mp_info() { Glob_scope()->Print_mp_info(Tstream()); }

  //! @brief Write IR to ELF file
  void Write_ir(const std::string& ofile) {
    if (ofile.empty())
      CMPLR_USR_MSG(U_CODE::Incorrect_Option, "use option : :ir2b=file");

    air::util::AIR2ELF ir2b(ofile, _ctx->Tstream());
    ir2b.Run(_ctx->Glob_scope());
  }

  //! @brief Read IR to ARENA
  void Read_ir(const std::string& ifile) {
    if (ifile.empty())
      CMPLR_USR_MSG(U_CODE::Incorrect_Option, "use option : :b2ir=file");

    air::util::ELF2AIR     b2ir(ifile, _ctx->Tstream());
    air::base::GLOB_SCOPE* scope = b2ir.Run();
    _ctx->Update_glob_scope(scope);
  }

  bool Verify_ir() { return Glob_scope()->Verify_ir(); }

public:
  // pointer to unique driver context
  DRIVER_CTX* _ctx;

  // is this driver standalone or controlled by another driver
  bool _standalone;

public:
  //! @brief get executable program name
  const char* Exe_name() const { return _ctx->Exe_name(); }

  //! @brief Reset measure position
  void Perf_start() { _ctx->Perf_start(); }

  //! @brief Call measure position
  void Perf_taken(std::string driver, std::string phase, std::string pass) {
    _ctx->Perf_taken(driver, phase, pass);
  }

  //! @brief Get Input file object
  const char* Ifile() { return _ctx->Ifile(); }

  //! @brief Access global config items
  DECLARE_GLOBAL_CONFIG_ACCESS_API((*_ctx))

};  // DRIVER

}  // namespace driver

}  // namespace air

#endif  // AIR_DRIVER_DRIVER_H
