//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_PHANTOM_PT_MGR_H
#define RTLIB_PHANTOM_PT_MGR_H

// Standard library
#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>

// External libraries
#include "common/rt_data_file.h"

namespace phantom {

struct ENCODE_PARAM {
    uint32_t _index;      // 权重索引
    size_t   _len;        // 数据长度
    uint32_t _scale;      // 缩放因子
    uint32_t _level;      // 密文层级
    size_t   _offset = 0; // 偏移量（默认0）
};

class PT_MGR {
public:
    PT_MGR()  = default;
    ~PT_MGR() { Fini(); }

    // Disable copy, enable move
    PT_MGR(const PT_MGR&) = delete;
    PT_MGR& operator=(const PT_MGR&) = delete;
    PT_MGR(PT_MGR&&) = default;
    PT_MGR& operator=(PT_MGR&&) = default;

    // C++ 风格接口
    bool Init(const std::string& fname);
    void Fini();

    // 从权重文件加载数据并编码为明文
    void* Load_encode(void* pt, const ENCODE_PARAM& params);

    // 从权重文件加载数据，编码为明文，并验证内容
    void Load_encode_validate(void* pt, std::vector<float>& buf,
                              const ENCODE_PARAM& params);

    bool Is_initialized() const { return _file != nullptr; }

private:
    RT_DATA_FILE* _file   = nullptr;
    char*         _msg_buf = nullptr;
    uint64_t      _msg_size = 0;
    bool          _sync_read = true;

    float* Msg_ptr(uint32_t index, uint64_t ofst, size_t len);
    void Dump_msg_preview(const char* tag, const ENCODE_PARAM& params,
                          const float* data);
    size_t Dump_count() const;
};

}  // namespace phantom

#endif  // RTLIB_PHANTOM_PT_MGR_H