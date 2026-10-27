//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief enc_mgr.h
//! Templated encoding manager for FHE runtime backends.
//!
//! EncMgr<VendorTraits> handles loading weight data from files, encoding
//! on the fly (DE_MSG_F32), or loading pre-encoded plaintexts (DE_PLAINTEXT).
//!
//! When EncPolicy::LOAD_ENCODED is set, all DE_MSG_F32 weights are encoded at
//! Init time and cached in _preload_cache. Subsequent Pt_from_msg() calls
//! return the cached Plaintext directly (zero encoding cost at inference).
//!
//! Backend-agnostic methods (Init, Fini, Msg_ptr, Dump_count, Dump_msg_preview,
//! Preload_encode, Get_or_encode) are defined inline in the template base.
//! Backend-specific methods (Load_encode, Load_plaintext, Load_encode_validate)
//! must be explicitly specialized for each VendorTraits.
//!
//! NOTE: This header is installed to the public include directory and used
//! by JIT-compiled kernels. It must NOT depend on spdlog/fmt directly,
//! because the JIT environment does not have spdlog headers/flags available.
//! Use fprintf/std::snprintf for debug output in this header.

#ifndef FHERT_ENC_MGR_H
#define FHERT_ENC_MGR_H

#include "common/block_io.h"
#include "common/error.h"
#include "common/rt_data_file.h"
#include "common/rt_env.h"
#include "fhe/enc/enc_io.h"
#include "fhe/fhe_config.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unistd.h>
#include <vector>

namespace fhert
{

//=============================================================================
// EncodeParam — weight encoding parameters
//=============================================================================

/// Parameters for on-the-fly weight encoding (DE_MSG_F32 path).
/// Used by EncMgr::Load_encode and the C API shim (Pt_from_msg).
struct EncodeParam
{
    uint32_t _index;     //!< Weight index
    size_t _len;         //!< Data length
    uint32_t _scale;     //!< Scale factor
    uint32_t _level;     //!< Ciphertext level
    size_t _offset = 0;  //!< Offset (default 0)
};

//=============================================================================
// EncMgr<VendorTraits>
//=============================================================================

template <typename Traits> class EncMgr
{
public:
    using Plaintext = typename Traits::Plaintext;
    using ContextT = typename Traits::Context;

    EncMgr() = default;
    ~EncMgr() { Fini(); }

    // Disable copy and move
    EncMgr(const EncMgr&) = delete;
    EncMgr& operator=(const EncMgr&) = delete;
    EncMgr(EncMgr&&) = delete;
    EncMgr& operator=(EncMgr&&) = delete;

    // --- Backend-agnostic methods (defined inline) ---

    bool Init(const std::string& fname, ContextT* ctx, EncPolicy enc_policy)
    {
        IS_TRUE(!fname.empty(), "missing rt data file name");
        IS_TRUE(_file == nullptr, "enc mgr already initialized");
        _ctx = ctx;
        _enc_policy = enc_policy;

        if (enc_policy == EncPolicy::LOAD_ON_DEMAND) {
            // Reserved: not yet implemented. On-demand per-entry file IO
            // (no full read at init) is a future mode.
            fprintf(stderr, "[enc-mgr] EncPolicy::LOAD_ON_DEMAND is not yet "
                            "implemented.\n");
            IS_TRUE(false, "EncPolicy::LOAD_ON_DEMAND not yet implemented");
        }

        bool sync_read = true;
        const char* sr_env = getenv(ENV_RT_DATA_ASYNC_READ);
        if (sr_env != NULL && atoi(sr_env) == 1) {
            sync_read = false;
        }

        if (Block_io_init(sync_read) == false) {
            return false;
        }

        _sync_read = sync_read;
        _file = Rt_data_open(fname.c_str(), sync_read);
        if (_file == NULL) {
            Block_io_fini(_sync_read);
            _sync_read = false;
            return false;
        }

        _entry_count = Rt_data_entry_count(_file);
        _is_plaintext = Rt_data_is_plaintext(_file);
        _fd = Rt_data_fd(_file);  // kept for diagnostics / future on-demand mode

        // LOAD_RAW / LOAD_ENCODED: read the whole file into memory regardless
        // of format. msg (DE_MSG_F32) is read raw; enc (DE_PLAINTEXT) is read
        // as-is and later sliced by (offset, size) in Load_plaintext.
        _msg_size = Rt_data_size(_file);
        _msg_buf = (char*)malloc(_msg_size);
        IS_TRUE(_msg_buf != NULL || _msg_size == 0, "failed to malloc rt data buffer");
        bool fill_ok = Rt_data_fill(_file, _msg_buf, _msg_size);
        FMT_ASSERT(fill_ok, "failed to fill rt data from file");

        // LOAD_ENCODED: if the file is raw (DE_MSG_F32), the caller
        // (Prepare_context in api.cu) calls Preload_encode() after
        // MakeCurrent() to encode all weights into the in-memory cache.
        // Init() only records the policy. For already-encoded (DE_PLAINTEXT)
        // files, LOAD_ENCODED is equivalent to LOAD_RAW.
        return true;
    }

    void Fini()
    {
        _preload_cache.clear();
        if (_file != NULL) {
            Rt_data_close(_file);
            _file = NULL;
        }
        free(_msg_buf);
        _msg_buf = NULL;
        _msg_size = 0;
        _is_plaintext = false;
        _fd = -1;
        _entry_count = 0;
        _enc_policy = EncPolicy::LOAD_RAW;
        Block_io_fini(_sync_read);
        _sync_read = false;
        _ctx = nullptr;
    }

    // --- Backend-specific methods (must be explicitly specialized) ---

    /// Load and encode on the fly (DE_MSG_F32 path)
    void* Load_encode(Plaintext& pt, const EncodeParam& params, ContextT& ctx);

    /// Load pre-encoded plaintext (DE_PLAINTEXT path).
    /// Common implementation — same for all backends. Uses EncIo::deserialize.
    /// Under LOAD_RAW / LOAD_ENCODED the whole file is already in _msg_buf,
    /// so this slices the entry by (offset, size) from memory (no file IO).
    void* Load_plaintext(Plaintext& pt, uint32_t index)
    {
        IS_TRUE(_file != NULL, "enc mgr is not initialized");
        IS_TRUE(_is_plaintext, "Load_plaintext called on non-plaintext data file");
        IS_TRUE(index < (uint32_t)Rt_data_entry_count(_file), "plaintext index out of range");

        uint64_t ent_size = Rt_data_lut_size(_file, index);
        uint64_t ent_ofst = Rt_data_lut_offset(_file, index);
        IS_TRUE(ent_ofst + ent_size <= _msg_size, "plaintext entry range out of buffer");

        using EncIoT = EncIo<Traits>;
        bool ok = EncIoT::deserialize(_msg_buf + ent_ofst, ent_size, pt, _ctx, nullptr);
        IS_TRUE(ok, "failed to deserialize plaintext entry");

        return &pt;
    }

    /// Load and encode with validation (DE_MSG_F32 path)
    void Load_encode_validate(Plaintext& pt, std::vector<float>& buf, const EncodeParam& params, ContextT& ctx);

    /// Preload-encode all DE_MSG_F32 weights into _preload_cache.
    /// Iterates over LUT entries and calls Load_encode() for each.
    /// Must be called AFTER FheContext::MakeCurrent() because
    /// Load_encode() uses FheContext::Current().
    void Preload_encode()
    {
        IS_TRUE(_file != nullptr, "enc mgr not initialized");
        IS_TRUE(!_is_plaintext, "Preload_encode called on plaintext data");
        IS_TRUE(_ctx != nullptr, "context pointer is null");
        IS_TRUE(_preload_cache.empty(), "preload cache already filled");

        _preload_cache.reserve(_entry_count);

        for (uint64_t i = 0; i < _entry_count; ++i) {
            auto idx = static_cast<uint32_t>(i);
            uint32_t scale = Rt_data_lut_scale(_file, idx);
            uint32_t level = Rt_data_lut_level(_file, idx);
            size_t len = Rt_data_lut_size(_file, idx) / sizeof(float);

            EncodeParam params{idx, len, scale, level, 0};
            Plaintext pt;
            Load_encode(pt, params, *_ctx);
            _preload_cache.push_back(std::move(pt));
        }

        fprintf(stderr, "[enc-mgr] PRELOAD: encoded %lu weights into GPU HBM\n", (unsigned long)_entry_count);
        fflush(stderr);
    }

    /// Unified dispatch: return encoded Plaintext for the given index.
    /// Checks preload cache first, then falls back to Load_plaintext
    /// or Load_encode depending on file type.
    void* Get_or_encode(Plaintext& pt, uint32_t index, size_t len, uint32_t scale, uint32_t level, size_t offset,
                        ContextT& ctx)
    {
        if (is_preloaded()) {
            pt = _preload_cache[index];
            return &pt;
        }
        if (_is_plaintext) {
            return Load_plaintext(pt, index);
        }
        EncodeParam params{index, len, scale, level, offset};
        return Load_encode(pt, params, ctx);
    }

    /// Unified dispatch with validation.
    void Get_or_encode_validate(Plaintext& pt, float* buf, uint32_t index, size_t len, uint32_t scale, uint32_t level,
                                size_t offset, ContextT& ctx)
    {
        if (is_preloaded()) {
            pt = _preload_cache[index];
            return;
        }
        if (_is_plaintext) {
            Load_plaintext(pt, index);
            return;
        }
        EncodeParam params{index, len, scale, level, offset};
        std::vector<float> vec(buf, buf + len);
        Load_encode_validate(pt, vec, params, ctx);
    }

    bool Is_initialized() const { return _file != nullptr; }
    bool is_plaintext() const { return _is_plaintext; }
    bool is_preloaded() const { return !_preload_cache.empty(); }

    /// Get a preloaded encoded Plaintext by index (PRELOAD path).
    const Plaintext& preloaded(uint32_t index) const
    {
        IS_TRUE(is_preloaded(), "preload cache is empty");
        IS_TRUE(index < _preload_cache.size(), "preload index out of range");
        return _preload_cache[index];
    }

    uint64_t entry_count() const { return _entry_count; }
    EncPolicy enc_policy() const { return _enc_policy; }

protected:
    RT_DATA_FILE* _file = nullptr;
    char* _msg_buf = nullptr;
    uint64_t _msg_size = 0;
    bool _sync_read = true;
    bool _is_plaintext = false;
    int _fd = -1;              // file descriptor for direct pread (DE_PLAINTEXT)
    ContextT* _ctx = nullptr;  // non-owning, for deserialize
    uint64_t _entry_count = 0;
    EncPolicy _enc_policy = EncPolicy::LOAD_RAW;

    // PRELOAD cache: encoded Plaintext objects, one per LUT entry.
    // Filled by Preload_encode(). Note: PhantomPlaintext's cuda_auto_ptr
    // uses deep-copy semantics (cudaMalloc + cudaMemcpy D2D), so
    // copying a cached entry (e.g. via Get_or_encode) performs a
    // D2D transfer — still much faster than re-encoding (NTT).
    std::vector<Plaintext> _preload_cache;

    float* Msg_ptr(uint32_t index, uint64_t ofst, size_t len)
    {
        IS_TRUE(_file != NULL, "enc mgr is not initialized");
        IS_TRUE(!_is_plaintext, "Msg_ptr called on plaintext data file");
        uint64_t file_ofst = Rt_data_entry_offset(_file, index, (ofst + len) * sizeof(float));
        IS_TRUE(file_ofst + (ofst + len) * sizeof(float) <= _msg_size, "entry offset too large");
        return (float*)&_msg_buf[file_ofst + ofst * sizeof(float)];
    }

    void Dump_msg_preview(const char* tag, const EncodeParam& params, const float* data)
    {
        size_t dump_count_val = Dump_count();
        if (dump_count_val == 0 || data == NULL) {
            return;
        }

        size_t len = params._len;
        size_t preview = (len < dump_count_val) ? len : dump_count_val;

        fprintf(stderr, "[data] %s index=%u ofst=%zu len=%zu scale=%u level=%u head:", tag, params._index,
                params._offset, len, params._scale, params._level);
        for (size_t i = 0; i < preview; ++i) {
            fprintf(stderr, " %g", data[i]);
        }
        if (preview < len) {
            fprintf(stderr, " ...");
        }
        fprintf(stderr, "\n");
        fflush(stderr);
    }

    size_t Dump_count() const
    {
        static size_t dump_count = (size_t)-1;
        if (dump_count == (size_t)-1) {
            const char* env = getenv(ENV_PT_MSG_DUMP_COUNT);
            dump_count = (env == NULL) ? 0 : strtoull(env, NULL, 10);
        }
        return dump_count;
    }
};

}  // namespace fhert

#endif  // FHERT_ENC_MGR_H
