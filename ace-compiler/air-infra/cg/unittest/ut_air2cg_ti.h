//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_CG_UNITTEST_UT_AIR2CGIR_TI_H
#define AIR_CG_UNITTEST_UT_AIR2CGIR_TI_H

#include "air/base/st.h"
#include "air/cg/cgir_container.h"
#include "air/cg/cgir_enum.h"
#include "air/cg/targ_info.h"

using namespace air::cg;

namespace {

static constexpr uint32_t UT_GPR_ID = 1;
static constexpr uint32_t UT_ISA_ID = 1;
static constexpr uint32_t UT_ISA_2  = 2;
static constexpr uint32_t UT_ISA_3  = 3;
static constexpr uint32_t UT_ISA_4  = 4;
static constexpr uint32_t UT_ISA_5  = 5;
static constexpr uint32_t UT_ISA_6  = 6;
static constexpr uint32_t UT_ABI_ID = 1;

static constexpr uint32_t UT_RC_UNKNOWN = 0;
static constexpr uint32_t UT_GPR_SIZE   = 8;
static constexpr uint32_t UT_GPR_FLAG   = 0;
static constexpr uint32_t UT_ADDR_SIZE  = 8;

#define DECL_REG_GPR_ALL() \
  DECL_REG_GPR(R0)         \
  DECL_REG_GPR(R1)         \
  DECL_REG_GPR(R2)         \
  DECL_REG_GPR(R3)         \
  DECL_REG_GPR(R4)         \
  DECL_REG_GPR(R5)         \
  DECL_REG_GPR(R6)         \
  DECL_REG_GPR(R7)

#define DECL_OPERATOR_ALL() \
  DECL_OPERATOR(UNKNOWN)    \
  DECL_OPERATOR(LD)         \
  DECL_OPERATOR(ST)         \
  DECL_OPERATOR(ADD)        \
  DECL_OPERATOR(LT)         \
  DECL_OPERATOR(JMP)        \
  DECL_OPERATOR(TRUE_BR)    \
  DECL_OPERATOR(FALSE_BR)   \
  DECL_OPERATOR(RETV)

#define DECL_OPND_FLAG_ALL() \
  DECL_OPND_FLAG(LO, lo)     \
  DECL_OPND_FLAG(HI, hi)

enum class OPERATOR : uint8_t {
#define DECL_OPERATOR(oper) oper,
  DECL_OPERATOR_ALL()
#undef DECL_OPERATOR
};

#define UT_MAKE_OPCODE(isa, oper) ((isa << 8) | (uint32_t)OPERATOR::oper)
#define DECL_OPCODE(oper) \
  static constexpr uint32_t ISA_##oper = UT_MAKE_OPCODE(UT_ISA_ID, oper)
#define DECL_OPERATOR(oper) DECL_OPCODE(oper);
DECL_OPERATOR_ALL()
#undef DECL_OPERATOR

enum UT_REG_GPR : uint8_t {
#define DECL_REG_GPR(reg) reg,
  DECL_REG_GPR_ALL()
#undef DECL_REG_GPR
};

enum UT_OPND_FLAG : uint8_t {
#define DECL_OPND_FLAG(id, desc) id,
  DECL_OPND_FLAG_ALL()
#undef DECL_OPND_FLAG
};

static constexpr struct REG_META UT_REG_INFO_GPR[] = {
#define DECL_REG_GPR(reg) {#reg, UT_REG_GPR::reg, UT_GPR_SIZE, UT_GPR_FLAG},
    DECL_REG_GPR_ALL()
#undef DECL_REG_GPR
};

static constexpr struct REG_INFO_META REG_INFO_GPR = {
    "GPR",
    UT_GPR_ID,
    sizeof(UT_REG_INFO_GPR) / sizeof(UT_REG_INFO_GPR[0]),
    UT_GPR_SIZE,
    0,
    UT_REG_INFO_GPR};

static constexpr struct INST_META INST_META_UNKNOWN = {
    "unknown", ISA_UNKNOWN, 0, 0, {}};
static constexpr struct INST_META INST_META_LD = {
    "ld",
    ISA_LD,
    1,
    1,
    0,
    {
      {OPND_KIND::REGISTER, UT_GPR_ID, REG_UNKNOWN, UT_GPR_SIZE},
      {OPND_KIND::SYMBOL, UT_RC_UNKNOWN, REG_UNKNOWN, UT_ADDR_SIZE},
      }
};
static constexpr struct INST_META INST_META_ST = {
    "st",
    ISA_ST,
    0,
    2,
    0,
    {
      {OPND_KIND::REGISTER, UT_GPR_ID, REG_UNKNOWN, UT_GPR_SIZE},
      {OPND_KIND::SYMBOL, UT_RC_UNKNOWN, REG_UNKNOWN, UT_ADDR_SIZE},
      }
};
static constexpr struct INST_META INST_META_ADD = {
    "add",
    ISA_ADD,
    1,
    2,
    INST_COMMUTABLE,
    {
      {OPND_KIND::REGISTER, UT_GPR_ID, REG_UNKNOWN, UT_GPR_SIZE},
      {OPND_KIND::REGISTER, UT_GPR_ID, REG_UNKNOWN, UT_GPR_SIZE},
      {OPND_KIND::REGISTER, UT_GPR_ID, REG_UNKNOWN, UT_GPR_SIZE},
      }
};
static constexpr struct INST_META INST_META_LT = {
    "lt",
    ISA_LT,
    1,
    2,
    INST_CMP,
    {
      {OPND_KIND::REGISTER, UT_GPR_ID, REG_UNKNOWN, UT_GPR_SIZE},
      {OPND_KIND::REGISTER, UT_GPR_ID, REG_UNKNOWN, UT_GPR_SIZE},
      {OPND_KIND::REGISTER, UT_GPR_ID, REG_UNKNOWN, UT_GPR_SIZE},
      }
};
static constexpr struct INST_META INST_META_JMP = {
    "jmp",
    ISA_JMP,
    0,
    1,
    INST_BRANCH,
    {
      {OPND_KIND::BB, UT_RC_UNKNOWN, REG_UNKNOWN, UT_ADDR_SIZE},
      }
};
static constexpr struct INST_META INST_META_TRUE_BR = {
    "true_br",
    ISA_TRUE_BR,
    0,
    2,
    INST_BRANCH,
    {
      {OPND_KIND::REGISTER, UT_GPR_ID, REG_UNKNOWN, UT_GPR_SIZE},
      {OPND_KIND::BB, UT_RC_UNKNOWN, REG_UNKNOWN, UT_ADDR_SIZE},
      }
};
static constexpr struct INST_META INST_META_FALSE_BR = {
    "false_br",
    ISA_FALSE_BR,
    0,
    2,
    INST_BRANCH,
    {
      {OPND_KIND::REGISTER, UT_GPR_ID, REG_UNKNOWN, UT_GPR_SIZE},
      {OPND_KIND::BB, UT_RC_UNKNOWN, REG_UNKNOWN, UT_ADDR_SIZE},
      }
};
static constexpr struct INST_META INST_META_RETV = {
    "retv",
    ISA_RETV,
    0,
    1,
    INST_RETURN,
    {
      {OPND_KIND::REGISTER, UT_GPR_ID, REG_UNKNOWN, UT_GPR_SIZE},
      }
};

static constexpr struct INST_META const* INST_META_ALL[] = {
#define DECL_OPERATOR(oper) &INST_META_##oper,
    DECL_OPERATOR_ALL()
#undef DECL_OPERATOR
};

static constexpr struct OPND_FLAG_META OPND_FLAG[] = {
#define DECL_OPND_FLAG(id, desc) {id, #desc},
    DECL_OPND_FLAG_ALL()
#undef DECL_OPND_FLAG
};

static constexpr struct ISA_INFO_META ISA_INFO = {
    "UT-ISA",
    UT_ISA_ID,
    sizeof(INST_META_ALL) / sizeof(INST_META_ALL[0]),
    sizeof(OPND_FLAG) / sizeof(OPND_FLAG[0]),
    INST_META_ALL,
    OPND_FLAG};

static constexpr struct ISA_INFO_META ISA_INFO_2 = {
    "UT-ISA-2",
    UT_ISA_2,
    sizeof(INST_META_ALL) / sizeof(INST_META_ALL[0]),
    sizeof(OPND_FLAG) / sizeof(OPND_FLAG[0]),
    INST_META_ALL,
    OPND_FLAG};

static constexpr struct ISA_INFO_META ISA_INFO_3 = {
    "UT-ISA-3",
    UT_ISA_3,
    sizeof(INST_META_ALL) / sizeof(INST_META_ALL[0]),
    sizeof(OPND_FLAG) / sizeof(OPND_FLAG[0]),
    INST_META_ALL,
    OPND_FLAG};

static constexpr struct ISA_INFO_META ISA_INFO_4 = {
    "UT-ISA-4",
    UT_ISA_4,
    sizeof(INST_META_ALL) / sizeof(INST_META_ALL[0]),
    sizeof(OPND_FLAG) / sizeof(OPND_FLAG[0]),
    INST_META_ALL,
    OPND_FLAG};

static constexpr struct ISA_INFO_META ISA_INFO_5 = {
    "UT-ISA-5",
    UT_ISA_5,
    sizeof(INST_META_ALL) / sizeof(INST_META_ALL[0]),
    sizeof(OPND_FLAG) / sizeof(OPND_FLAG[0]),
    INST_META_ALL,
    OPND_FLAG};

static constexpr struct ISA_INFO_META ISA_INFO_6 = {
    "UT-ISA-6",
    UT_ISA_6,
    sizeof(INST_META_ALL) / sizeof(INST_META_ALL[0]),
    sizeof(OPND_FLAG) / sizeof(OPND_FLAG[0]),
    INST_META_ALL,
    OPND_FLAG};

static constexpr struct REG_CONV_META REG_CONV_GPR = {
    UT_GPR_ID, REG_UNKNOWN, 2, 1, {R6, R7, R6}
};

static constexpr struct REG_CONV_META const* REG_CONV_ALL[] = {
    &REG_CONV_GPR,
};

static constexpr struct ABI_INFO_META ABI_INFO = {
    "UT-ABI",    UT_ABI_ID,   UT_GPR_ID,
    REG_UNKNOWN, R0,          REG_UNKNOWN,
    REG_UNKNOWN, REG_UNKNOWN, sizeof(REG_CONV_ALL) / sizeof(REG_CONV_ALL[0]),
    REG_CONV_ALL};

static inline void Ut_register_ti() {
  TARG_INFO_MGR::Register_reg_info(&REG_INFO_GPR);
  TARG_INFO_MGR::Register_isa_info(&ISA_INFO);
  TARG_INFO_MGR::Register_abi_info(&ABI_INFO);

  TARG_INFO_MGR::Register_isa_info(&ISA_INFO_2);
  TARG_INFO_MGR::Register_isa_info(&ISA_INFO_3);
  TARG_INFO_MGR::Register_isa_info(&ISA_INFO_4);
  TARG_INFO_MGR::Register_isa_info(&ISA_INFO_5);
  TARG_INFO_MGR::Register_isa_info(&ISA_INFO_6);
}

class UT_TARG_INFO {
public:
  UT_TARG_INFO(CGIR_CONTAINER* cntr) : _cntr(cntr) {}

  INST_PTR Gen_st(air::base::ADDR_DATUM_ID var, int64_t val) {
    OPND_PTR op0  = _cntr->New_opnd(var);
    OPND_PTR op1  = _cntr->New_opnd(val);
    INST_PTR inst = _cntr->New_inst(air::base::SPOS(), UT_ISA_ID,
                                    (uint8_t)OPERATOR::ST, 0, 2);
    inst->Set_opnd(0, op0->Id());
    inst->Set_opnd(1, op1->Id());
    return inst;
  }

  INST_PTR Gen_jmp(air::cg::BB_ID target) {
    OPND_PTR op0  = _cntr->New_opnd(target);
    INST_PTR inst = _cntr->New_inst(air::base::SPOS(), UT_ISA_ID,
                                    (uint8_t)OPERATOR::JMP, 0, 1);
    inst->Set_opnd(0, op0->Id());
    return inst;
  }

  template <bool TRUE_BR>
  INST_PTR Gen_cond_jmp(air::base::ADDR_DATUM_ID var, int64_t val,
                        air::cg::BB_ID target) {
    OPND_PTR op0  = _cntr->New_opnd(var);
    OPND_PTR op1  = _cntr->New_opnd(val);
    OPND_PTR op2  = _cntr->New_opnd(target);
    INST_PTR inst = _cntr->New_inst(
        air::base::SPOS(), UT_ISA_ID,
        TRUE_BR ? (uint8_t)OPERATOR::TRUE_BR : (uint8_t)OPERATOR::FALSE_BR, 0,
        3);
    inst->Set_opnd(0, op0->Id());
    inst->Set_opnd(1, op1->Id());
    inst->Set_opnd(2, op2->Id());
    return inst;
  }

  INST_PTR Gen_retv(int64_t val) {
    OPND_PTR op0  = _cntr->New_opnd(val);
    INST_PTR inst = _cntr->New_inst(air::base::SPOS(), UT_ISA_ID,
                                    (uint8_t)OPERATOR::RETV, 0, 1);
    inst->Set_opnd(0, op0->Id());
    return inst;
  }

private:
  CGIR_CONTAINER* _cntr;
};

}  // namespace

#endif  // AIR_CG_UNITTEST_UT_AIR2CGIR_TI_H
