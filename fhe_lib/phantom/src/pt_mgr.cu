//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "pt_mgr.h"

#include <cmath>
#include <cstdio>
#include <cstring>

#include "common/error.h"
#include "common/rt_data_file.h"
#include "common/rt_env.h"
#include "rt_phantom/rt_phantom.h"

namespace phantom {

bool PT_MGR::Init(const std::string& fname) {
    IS_TRUE(!fname.empty(), "missing rt data file name");
    IS_TRUE(_file == nullptr, "pt mgr already initialized");

    bool        sync_read = true;
    const char* sr_env    = getenv(ENV_RT_DATA_ASYNC_READ);
    if (sr_env != NULL && atoi(sr_env) == 1) {
        sync_read = false;
    }

    if (Block_io_init(sync_read) == false) {
        return false;
    }

    _sync_read = sync_read;
    _file      = Rt_data_open(fname.c_str(), sync_read);
    if (_file == NULL) {
        Block_io_fini(sync_read);
        _sync_read = false;
        return false;
    }

    IS_TRUE(!Rt_data_is_plaintext(_file),
            "phantom pt mgr does not support plaintext rt data yet");

    _msg_size = Rt_data_size(_file);
    _msg_buf  = (char*)malloc(_msg_size);
    IS_TRUE(_msg_buf != NULL || _msg_size == 0,
            "failed to malloc rt data buffer");
    bool fill_ok = Rt_data_fill(_file, _msg_buf, _msg_size);
    FMT_ASSERT(fill_ok, "failed to fill rt data from file");
    return true;
}

void PT_MGR::Fini() {
    if (_file != NULL) {
        Rt_data_close(_file);
        _file = NULL;
    }
    free(_msg_buf);
    _msg_buf   = NULL;
    _msg_size  = 0;
    Block_io_fini(_sync_read);
    _sync_read = false;
}

float* PT_MGR::Msg_ptr(uint32_t index, uint64_t ofst, size_t len) {
    IS_TRUE(_file != NULL, "pt mgr is not initialized");
    uint64_t file_ofst = Rt_data_entry_offset(_file, index,
                                              (ofst + len) * sizeof(float));
    IS_TRUE(file_ofst + (ofst + len) * sizeof(float) <= _msg_size,
            "entry offset too large");
    return (float*)&_msg_buf[file_ofst + ofst * sizeof(float)];
}

size_t PT_MGR::Dump_count() const {
    static size_t dump_count = (size_t)-1;
    if (dump_count == (size_t)-1) {
        const char* env = getenv(ENV_PT_MSG_DUMP_COUNT);
        dump_count      = (env == NULL) ? 0 : strtoull(env, NULL, 10);
    }
    return dump_count;
}

void PT_MGR::Dump_msg_preview(const char* tag, const ENCODE_PARAM& params,
                               const float* data) {
    size_t dump_count = Dump_count();
    if (dump_count == 0 || data == NULL) {
        return;
    }

    size_t len = params._len;
    size_t ofst = params._offset;
    size_t preview       = (len < dump_count) ? len : dump_count;
    size_t first_nonzero = len;
    size_t last_nonzero  = len;
    size_t nonzero_count = 0;
    float  min_val       = 0.0f;
    float  max_val       = 0.0f;

    if (len > 0) {
        min_val = data[0];
        max_val = data[0];
    }
    for (size_t i = 0; i < len; ++i) {
        float val = data[i];
        if (val < min_val) {
            min_val = val;
        }
        if (val > max_val) {
            max_val = val;
        }
        if (fabs(val) > 0.000001f) {
            if (first_nonzero == len) {
                first_nonzero = i;
            }
            last_nonzero = i;
            ++nonzero_count;
        }
    }

    fprintf(stderr,
            "[pt_mgr] %s index=%u ofst=%zu len=%zu scale=%u level=%u head:",
            tag, params._index, ofst, len, params._scale, params._level);
    for (size_t i = 0; i < preview; ++i) {
        fprintf(stderr, " %g", data[i]);
    }
    if (preview < len) {
        fprintf(stderr, " ...");
    }

    fprintf(stderr, " | nz=%zu", nonzero_count);
    if (len > 0) {
        fprintf(stderr, " min=%g max=%g", min_val, max_val);
    }
    if (first_nonzero < len) {
        size_t nz_preview = ((len - first_nonzero) < dump_count)
                                ? (len - first_nonzero)
                                : dump_count;
        fprintf(stderr, " first_nz=%zu:%g last_nz=%zu:%g nz_head:", first_nonzero,
                data[first_nonzero], last_nonzero, data[last_nonzero]);
        for (size_t i = 0; i < nz_preview; ++i) {
            fprintf(stderr, " %g", data[first_nonzero + i]);
        }
        if (first_nonzero + nz_preview < len) {
            fprintf(stderr, " ...");
        }
    } else {
        fprintf(stderr, " all_zero=yes");
    }
    fprintf(stderr, "\n");
    fflush(stderr);
}

void* PT_MGR::Load_encode(void* pt, const ENCODE_PARAM& params) {
    float* data = Msg_ptr(params._index, params._offset, params._len);
    Dump_msg_preview("Load_encode", params, data);
    Encode_float((PLAIN)pt, data, params._len, params._scale, params._level);
    return pt;
}

void PT_MGR::Load_encode_validate(void* pt, std::vector<float>& buf,
                                   const ENCODE_PARAM& params) {
    float* data = Msg_ptr(params._index, params._offset, params._len);
    Dump_msg_preview("Load_encode_validate", params, data);
    for (uint32_t i = 0; i < params._len; ++i) {
        FMT_ASSERT(fabs(buf[i] - data[i]) < 0.000001,
                   "Load_encode_validate failed. index=%u, i=%u: %f != %f.",
                   params._index, i, buf[i], data[i]);
    }
    Encode_float((PLAIN)pt, data, params._len, params._scale, params._level);
}

}  // namespace phantom