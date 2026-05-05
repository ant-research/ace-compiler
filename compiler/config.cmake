#=============================================================================
#
# Compiler dependency URLs
# These dependencies will eventually be sourced directly (not fetched).
#
#=============================================================================

# Dependency names
set(DEP_AIR_NAME    "air-infra")
set(DEP_NN_NAME     "nn-addon")
set(DEP_FHE_NAME    "fhe-cmplr")
set(DEP_RISCV_NAME  "riscv-cg")
set(DEP_HPU_NAME    "hpu-cg")

# Dependency git URLs and refs
set(DEP_AIR_URL     "https://git:$ENV{CI_TOKEN}@code.alipay.com/air-infra/air-infra.git")
set(DEP_AIR_REF     "master")
set(DEP_NN_URL      "https://git:$ENV{CI_TOKEN}@code.alipay.com/air-infra/nn-addon.git")
set(DEP_NN_REF      "master")
set(DEP_FHE_URL     "https://git:$ENV{CI_TOKEN}@code.alipay.com/fhe-cmplr/fhe-cmplr.git")
set(DEP_FHE_REF     "master")
set(DEP_RISCV_URL   "https://git:$ENV{CI_TOKEN}@code.alipay.com/air-infra/riscv-cg.git")
set(DEP_RISCV_REF   "master")
set(DEP_HPU_URL     "https://git:$ENV{CI_TOKEN}@code.alipay.com/riscv-hpu/hpu-cg.git")
set(DEP_HPU_REF     "master")