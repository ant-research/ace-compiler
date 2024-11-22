//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "common/pt_mgr.h"

#include <math.h>
#include <stdlib.h>

#include "common/error.h"
#include "common/rt_api.h"
#include "common/rt_data_file.h"
#include "common/rt_env.h"
#include "common/rtlib_timing.h"
#include "fhe/core/rt_data_def.h"
#include "fhe/core/rt_encode_api.h"

typedef struct PT_MGR {
  struct RT_DATA_FILE* _file;
  char*                _pt_buf;
  BLOCK_INFO*          _pt_entry;
  uint64_t             _pt_size;
  uint32_t             _ent_invalid;
  uint32_t             _ent_count;
  uint32_t             _prefetch_count;
  bool                 _sync_read;
} PT_MGR;

static PT_MGR Pt_mgr;

bool Pt_mgr_init(const char* fname) {
  // Check environment
  const char* pt_env = getenv(ENV_PT_ENTRY_COUNT);
  uint32_t    pt_count;
  if (pt_env == NULL || (pt_count = atoi(pt_env)) == 0) {
    pt_count = 8;
  }
  const char* pf_env = getenv(ENV_PT_PREFETCH_COUNT);
  uint32_t    pf_count;
  if (pf_env == NULL || (pf_count = atoi(pf_env)) < 0) {
    pf_count = 2;
  }
  bool        sync_read = false;
  const char* sr_env    = getenv(ENV_RT_DATA_ASYNC_READ);
  if (sr_env == NULL || atoi(sr_env) != 1) {
    sync_read = true;
  }
  // Initialize block io
  if (Block_io_init(sync_read) == false) {
    return false;
  }

  Pt_mgr._sync_read = sync_read;
  Pt_mgr._file      = Rt_data_open(fname, sync_read);
  if (Pt_mgr._file == NULL) {
    return false;
  }

  if (Rt_data_is_plaintext(Pt_mgr._file)) {
    // setup a fixed length buffer and recycle it for plaintext
    // TODO: use mmap to allocate memory on large page
    uint64_t pt_size = Max_plain_buffer_length();
    // align to DATA_FILE_PAGE_SIZE
    pt_size =
        (pt_size + DATA_FILE_PAGE_SIZE - 1) & (~(DATA_FILE_PAGE_SIZE - 1));
    Pt_mgr._pt_buf = (char*)malloc(pt_size * pt_count);
    IS_TRUE(Pt_mgr._pt_buf != NULL, "failed to malloc PT_BUF");

    Pt_mgr._pt_entry = (BLOCK_INFO*)malloc(sizeof(BLOCK_INFO) * pt_count);
    IS_TRUE(Pt_mgr._pt_entry != NULL, "failed to malloc BLOCK_INFO");
    for (uint32_t i = 0; i < pt_count; ++i) {
      Pt_mgr._pt_entry[i]._blk_idx        = (uint32_t)-1;
      Pt_mgr._pt_entry[i]._blk_sts        = BLK_INVALID;
      Pt_mgr._pt_entry[i]._mem_next       = i + 1;
      Pt_mgr._pt_entry[i]._iovec.iov_base = Pt_mgr._pt_buf + i * pt_size;
      Pt_mgr._pt_entry[i]._iovec.iov_len  = pt_size;
    }

    Pt_mgr._pt_size     = pt_size;
    Pt_mgr._ent_invalid = 0;
    Pt_mgr._ent_count   = pt_count;

    // do some prefetch
    Pt_mgr._prefetch_count = pf_count;
    for (uint32_t i = 0; i < pf_count; ++i) {
      Pt_prefetch(i);
    }
  } else {
    // create a large buffer to contain all message
    uint64_t msg_sz = Rt_data_size(Pt_mgr._file);
    Pt_mgr._pt_buf  = (char*)malloc(msg_sz);
    Pt_mgr._pt_size = msg_sz;
    Rt_data_fill(Pt_mgr._file, Pt_mgr._pt_buf, msg_sz);
  }

  return true;
}

void Pt_mgr_fini() {
  Rt_data_close(Pt_mgr._file);
  free(Pt_mgr._pt_buf);
  if (Pt_mgr._pt_entry) {
    free(Pt_mgr._pt_entry);
  }
  Block_io_fini(Pt_mgr._sync_read);
}

static inline uint32_t Get_slot(uint32_t pt_idx) {
  // so far, using direct mapping
  return pt_idx % Pt_mgr._ent_count;
}

void Pt_prefetch(uint32_t pt_idx) {
  uint32_t slot                   = Get_slot(pt_idx);
  Pt_mgr._pt_entry[slot]._blk_sts = BLK_INVALID;
  IS_TRUE(Pt_mgr._pt_entry[slot]._blk_sts == BLK_INVALID,
          "BLOCK_INFO state is not invalid");
  Pt_mgr._pt_entry[slot]._blk_idx       = pt_idx;
  Pt_mgr._pt_entry[slot]._iovec.iov_len = Pt_mgr._pt_size;
  Rt_data_prefetch(Pt_mgr._file, pt_idx, &Pt_mgr._pt_entry[slot],
                   Pt_mgr._sync_read);
}

void* Pt_get(uint32_t pt_idx, size_t len, uint32_t scale, uint32_t level) {
  RTLIB_TM_START(RTM_PT_GET, rtm);
  uint32_t slot = Get_slot(pt_idx);
  if (Pt_mgr._prefetch_count == 0) {
    Pt_mgr._pt_entry[slot]._blk_idx       = pt_idx;
    Pt_mgr._pt_entry[slot]._blk_sts       = BLK_INVALID;
    Pt_mgr._pt_entry[slot]._iovec.iov_len = Pt_mgr._pt_size;
  }
  IS_TRUE(Pt_mgr._pt_entry[slot]._blk_idx == (uint32_t)-1 ||
              Pt_mgr._pt_entry[slot]._blk_idx == pt_idx,
          "BLOCK_INFO pt_idx mismatch");
  if (Pt_mgr._pt_entry[slot]._blk_sts == BLK_INVALID) {
    Pt_mgr._pt_entry[slot]._blk_idx = pt_idx;
    bool ret = Rt_data_prefetch(Pt_mgr._file, pt_idx, &Pt_mgr._pt_entry[slot],
                                Pt_mgr._sync_read);
    IS_TRUE(ret == true, "prefetch error");
  }
  if (Pt_mgr._pt_entry[slot]._blk_sts == BLK_PREFETCHING) {
    bool ret = Rt_data_read(Pt_mgr._file, pt_idx, &Pt_mgr._pt_entry[slot],
                            Pt_mgr._sync_read);
    IS_TRUE(ret == true, "prefetch error");
  }
  IS_TRUE(Pt_mgr._pt_entry[slot]._blk_sts == BLK_READY,
          "block state is not ready");
  if (Pt_mgr._prefetch_count > 0) {
    Pt_prefetch(pt_idx + Pt_mgr._prefetch_count);
  }
  void* pt =
      (void*)Cast_buffer_to_plain(Pt_mgr._pt_entry[slot]._iovec.iov_base);
  RTLIB_TM_END(RTM_PT_GET, rtm);
  return pt;
}

void* Pt_get_validate(float* buf, uint32_t index, size_t len, uint32_t scale,
                      uint32_t level) {
  IS_TRUE(false, "TODO: not implemented");
}

void Pt_free(uint32_t pt_idx) {
  uint32_t slot = Get_slot(pt_idx);
  IS_TRUE(Pt_mgr._pt_entry[slot]._blk_sts == BLK_READY,
          "BLOCK_INFO state is not ready");
  Pt_mgr._pt_entry[slot]._blk_idx  = (uint32_t)-1;
  Pt_mgr._pt_entry[slot]._blk_sts  = BLK_INVALID;
  Pt_mgr._pt_entry[slot]._mem_next = Pt_mgr._ent_invalid;
  Pt_mgr._ent_invalid              = slot;
  if (Pt_mgr._prefetch_count > 0) {
    Pt_prefetch(pt_idx + Pt_mgr._prefetch_count);
  }
}

extern void Encode_plain_from_float(void* plain, float* input, size_t len,
                                    uint32_t sc_degree, uint32_t level);

void Pt_from_msg(void* pt, uint32_t index, size_t len, uint32_t scale,
                 uint32_t level) {
  IS_TRUE(!Rt_data_is_plaintext(Pt_mgr._file), "bad entry type");
  uint64_t ofst =
      Rt_data_entry_offset(Pt_mgr._file, index, len * sizeof(float));
  IS_TRUE(ofst + len * sizeof(float) <= Pt_mgr._pt_size,
          "entry offset too large");
  float* data = (float*)&Pt_mgr._pt_buf[ofst];
  Encode_plain_from_float(pt, data, len, scale, level);
}

void Pt_from_msg_validate(void* pt, float* buf, uint32_t index, size_t len,
                          uint32_t scale, uint32_t level) {
  IS_TRUE(!Rt_data_is_plaintext(Pt_mgr._file), "bad entry type");
  uint64_t ofst =
      Rt_data_entry_offset(Pt_mgr._file, index, len * sizeof(float));
  float* data = (float*)&Pt_mgr._pt_buf[ofst];
  for (uint32_t i = 0; i < len; ++i) {
    FMT_ASSERT(fabs(buf[i] - data[i]) < 0.000001,
               "Pt_from_msg_validate failed. index=%d, i=%d: %f != %f.", index,
               i, buf[i], data[i]);
  }
  Encode_plain_from_float(pt, data, len, scale, level);
}
