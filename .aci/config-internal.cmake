#=============================================================================
#
# Internal dependency URLs for ACE (code.alipay.com)
# This file only exists in the internal repo — not published to GitHub.
#
#=============================================================================

# FHE library dependencies (internal)
set(DEP_PHANTOM_URL   "https://git:$ENV{CI_TOKEN}@code.alipay.com/ace-fhe/phantom-fhe.git")
set(DEP_PHANTOM_REF   "master")
set(DEP_HYPERFHE_URL  "https://git:$ENV{CI_TOKEN}@code.alipay.com/cy459642/ckks-infra.git")
set(DEP_HYPERFHE_REF  "master")