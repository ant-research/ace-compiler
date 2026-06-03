//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "common/pt_mgr.h"
#include "context.h"
#include "pt_mgr.h"

#include <vector>

//=============================================================================
// C interface wrappers for pt_mgr (provide to common/pt_mgr.h)
//=============================================================================
extern "C" {

bool Pt_mgr_init(const char* fname) {
    return PHANTOM_CONTEXT::Context().pt_mgr().Init(std::string(fname));
}

void Pt_mgr_fini() {
    PHANTOM_CONTEXT::Context().pt_mgr().Fini();
}

void Pt_prefetch(uint32_t index) {
    (void)index;  // Not implemented for phantom
}

void* Pt_get(uint32_t index, size_t len, uint32_t scale, uint32_t level) {
    (void)index;
    (void)len;
    (void)scale;
    (void)level;
    IS_TRUE(false, "phantom pt mgr does not support plaintext rt data");
    return nullptr;
}

void* Pt_get_validate(float* buf, uint32_t index, size_t len, uint32_t scale,
                      uint32_t level) {
    (void)buf;
    (void)index;
    (void)len;
    (void)scale;
    (void)level;
    IS_TRUE(false, "phantom pt mgr does not support plaintext rt data");
    return nullptr;
}

void Pt_free(uint32_t index) {
    (void)index;
}

void Free_data(void* poly) {
    (void)poly;
}

void* Pt_from_msg(void* pt, uint32_t index, size_t len, uint32_t scale,
                  uint32_t level) {
    using namespace phantom;
    ENCODE_PARAM params{index, len, scale, level, 0};
    return PHANTOM_CONTEXT::Context().pt_mgr().Load_encode(pt, params);
}

void* Pt_from_msg_ofst(void* pt, uint32_t index, size_t ofst, size_t len,
                       uint32_t scale, uint32_t level) {
    using namespace phantom;
    ENCODE_PARAM params{index, len, scale, level, ofst};
    return PHANTOM_CONTEXT::Context().pt_mgr().Load_encode(pt, params);
}

void Pt_from_msg_validate(void* pt, float* buf, uint32_t index, size_t len,
                          uint32_t scale, uint32_t level) {
    using namespace phantom;
    ENCODE_PARAM params{index, len, scale, level, 0};
    std::vector<float> vec(buf, buf + len);
    PHANTOM_CONTEXT::Context().pt_mgr().Load_encode_validate(pt, vec, params);
}

void Pt_from_msg_ofst_validate(void* pt, float* buf, uint32_t index,
                               size_t offset, size_t len, uint32_t scale,
                               uint32_t level) {
    using namespace phantom;
    ENCODE_PARAM params{index, len, scale, level, offset};
    std::vector<float> vec(buf, buf + len);
    PHANTOM_CONTEXT::Context().pt_mgr().Load_encode_validate(pt, vec, params);
}

}  // extern "C"