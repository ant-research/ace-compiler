//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <cstring>

#include "ace/runtime/rtlib_interface.h"
#include "ace/runtime/config_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

CKKS_PARAMS* get_context_params_c() {
    const auto& config = ace::runtime::CONFIG_MANAGER::Instance().Get_config_ref();
    if (config._ctx_param == 0) {
        return nullptr;
    }
    return reinterpret_cast<CKKS_PARAMS*>(config._ctx_param);
}

RT_DATA_INFO* get_rt_data_info_c() {
    const auto& config = ace::runtime::CONFIG_MANAGER::Instance().Get_config_ref();
    static RT_DATA_INFO cached_info;
    cached_info = config._weight_info;
    return &cached_info;
}

DATA_SCHEME* get_encode_scheme_c(int idx) {
    const auto& config = ace::runtime::CONFIG_MANAGER::Instance().Get_config_ref();
    if (idx >= 0 && idx < static_cast<int>(config._encode_sch.size())) {
        return const_cast<DATA_SCHEME*>(&config._encode_sch[idx]);
    }
    return nullptr;
}

DATA_SCHEME* get_decode_scheme_c(int idx) {
    const auto& config = ace::runtime::CONFIG_MANAGER::Instance().Get_config_ref();
    if (idx >= 0 && idx < static_cast<int>(config._decode_sch.size())) {
        return const_cast<DATA_SCHEME*>(&config._decode_sch[idx]);
    }
    return nullptr;
}

int get_input_cnt() {
    auto config = ace::runtime::CONFIG_MANAGER::Instance().Get_config();
    return config._input_cnt;
}

int get_output_cnt() {
    auto config = ace::runtime::CONFIG_MANAGER::Instance().Get_config();
    return config._output_cnt;
}

#ifdef __cplusplus
}
#endif

void Register_config_func(void) {
#ifdef RTLIB_SUPPORT_DYNAMIC
    // Register callbacks (only once)
    static bool registered = false;
    if (!registered) {
        Register_runtime_callbacks(
            get_context_params_c,
            get_rt_data_info_c,
            get_decode_scheme_c,
            get_encode_scheme_c,
            get_input_cnt,
            get_output_cnt
        );
        registered = true;
    }
#endif
}