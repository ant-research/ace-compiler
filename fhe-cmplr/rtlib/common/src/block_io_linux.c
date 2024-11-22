//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief block_io_linux.c.c
//! There are 3 basic asynchronous I/O APIs on Linux:
//!  POSIX AIO. Kernel asynchronous I/O with O_DIRECT, io_uring.
//!
//! Perform asynchronous file I/O with Linux io_uring.
//!  https://www.man7.org/linux/man-pages/man7/io_uring.7.html
//!  https://kernel.dk/io_uring.pdf
//!
//! Linux io_uring may not work before kernel 5.1 (March 2019). In such case,
//! POSIX aio can be utilized to perform asynchronous file I/O.
//!  - aio_read, aio_write
//!  https://www.man7.org/linux/man-pages/man7/aio.7.html
//!
//! Or, Linux asynchronous I/O context since kernel 2.5
//!  - io_setup, io_submit
//!  https://www.man7.org/linux/man-pages/man2/io_setup.2.html
//!  https://www.kernel.org/doc/ols/2003/ols2003-pages-351-366.pdf

// the code below only available for linux
#ifdef __linux__

#include <fcntl.h>
#include <linux/io_uring.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <sys/uio.h>
#include <time.h>
#include <unistd.h>

#include "common/block_io.h"
#include "common/error.h"
#include "common/rtlib_timing.h"

// NOLINTBEGIN (readability-identifier-naming)

// io_uring system call wrappers
static inline int io_uring_setup(unsigned entries, struct io_uring_params* p) {
  return (int)syscall(__NR_io_uring_setup, entries, p);
}

static inline int io_uring_enter(int ring_fd, unsigned int to_submit,
                                 unsigned int min_complete,
                                 unsigned int flags) {
  return (int)syscall(__NR_io_uring_enter, ring_fd, to_submit, min_complete,
                      flags, NULL, 0);
}

static inline int io_uring_register(unsigned int fd, unsigned int opcode,
                                    const void* arg, unsigned int nr_args) {
  return (int)syscall(__NR_io_uring_register, fd, opcode, arg, nr_args);
}

#define read_barrier()  __asm__ __volatile__("" ::: "memory")
#define write_barrier() __asm__ __volatile__("" ::: "memory")

// NOLINTEND (readability-identifier-naming)

// internal data structure
struct IO_URING_CTX {
  int _ring_fd;
  struct SQ_RING {
    unsigned int*        _head;
    unsigned int*        _tail;
    unsigned int*        _ring_mask;
    unsigned int*        _ring_entries;
    unsigned int*        _flags;
    unsigned int*        _array;
    struct io_uring_sqe* _sqes;
    size_t               _sqes_size;
    void*                _sq_ptr;
    size_t               _sq_size;
  } _sq_ring;
  struct CQ_RING {
    unsigned int*        _head;
    unsigned int*        _tail;
    unsigned int*        _ring_mask;
    unsigned int*        _ring_entries;
    struct io_uring_cqe* _cqes;
    void*                _cq_ptr;
    size_t               _cq_size;
  } _cq_ring;
};

// internal settings
#define IO_QUEUE_DEPTH 1

// internal global status
static bool                Lib_init;
static struct IO_URING_CTX Io_ctx;

bool Block_io_init(bool sync_read) {
  IS_TRUE(Lib_init == false, "library already initialized");
  Lib_init = true;

  if (sync_read) {
    // no need to set up io_uring for sync read
    return true;
  }

  struct io_uring_params io_param;
  memset(&io_param, 0, sizeof(io_param));
  Io_ctx._ring_fd = io_uring_setup(IO_QUEUE_DEPTH, &io_param);
  if (Io_ctx._ring_fd < 0) {
    perror("Fatal: io_uring_setup()");
    return false;
  }
  int sq_size = io_param.sq_off.array + io_param.sq_entries * sizeof(unsigned);
  int cq_size =
      io_param.cq_off.cqes + io_param.cq_entries * sizeof(struct io_uring_cqe);
  // map submission and completion buffers with a sinle map() if allowed
  if (io_param.features & IORING_FEAT_SINGLE_MMAP) {
    if (cq_size > sq_size) {
      sq_size = cq_size;
    }
    cq_size = sq_size;
  }
  void* sq_ptr =
      mmap(0, sq_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE,
           Io_ctx._ring_fd, IORING_OFF_SQ_RING);
  if (sq_ptr == MAP_FAILED) {
    perror("Fatal: sq_ring mmap()");
    close(Io_ctx._ring_fd);
    return false;
  }
  Io_ctx._sq_ring._sq_ptr  = sq_ptr;
  Io_ctx._sq_ring._sq_size = sq_size;
  void* cq_ptr;
  if (io_param.features & IORING_FEAT_SINGLE_MMAP) {
    cq_ptr                  = sq_ptr;
    Io_ctx._cq_ring._cq_ptr = MAP_FAILED;
  } else {
    // map cq_ptr with second mmap() call
    cq_ptr = mmap(0, cq_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE,
                  Io_ctx._ring_fd, IORING_OFF_CQ_RING);
    if (cq_ptr == MAP_FAILED) {
      perror("Fatal: cq_ring mmap()");
      munmap(sq_ptr, sq_size);
      close(Io_ctx._ring_fd);
      return false;
    }
    Io_ctx._cq_ring._cq_ptr  = cq_ptr;
    Io_ctx._cq_ring._cq_size = cq_size;
  }

  // map submision queue entry
  size_t sqes_size = io_param.sq_entries * sizeof(struct io_uring_sqe);
  void*  sqes =
      mmap(0, sqes_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE,
           Io_ctx._ring_fd, IORING_OFF_SQES);
  if (sqes == MAP_FAILED) {
    perror("Fatal: sqes mmap()");
    if (cq_ptr != MAP_FAILED) {
      munmap(cq_ptr, cq_size);
    }
    munmap(sq_ptr, sq_size);
    close(Io_ctx._ring_fd);
    return false;
  }
  Io_ctx._sq_ring._sqes      = sqes;
  Io_ctx._sq_ring._sqes_size = sqes_size;

  // save fields into sq_ring
  Io_ctx._sq_ring._head         = sq_ptr + io_param.sq_off.head;
  Io_ctx._sq_ring._tail         = sq_ptr + io_param.sq_off.tail;
  Io_ctx._sq_ring._ring_mask    = sq_ptr + io_param.sq_off.ring_mask;
  Io_ctx._sq_ring._ring_entries = sq_ptr + io_param.sq_off.ring_entries;
  Io_ctx._sq_ring._flags        = sq_ptr + io_param.sq_off.flags;
  Io_ctx._sq_ring._array        = sq_ptr + io_param.sq_off.array;

  // save fields into cring
  Io_ctx._cq_ring._head         = cq_ptr + io_param.cq_off.head;
  Io_ctx._cq_ring._tail         = cq_ptr + io_param.cq_off.tail;
  Io_ctx._cq_ring._ring_mask    = cq_ptr + io_param.cq_off.ring_mask;
  Io_ctx._cq_ring._ring_entries = cq_ptr + io_param.cq_off.ring_entries;
  Io_ctx._cq_ring._cqes         = cq_ptr + io_param.cq_off.cqes;

  return true;
}

void Block_io_fini(bool sync_read) {
  IS_TRUE(Lib_init == true, "library not initialized");
  Lib_init = false;

  if (sync_read) {
    return;
  }

  // munmap sqe array
  if (Io_ctx._sq_ring._sqes != MAP_FAILED) {
    munmap(Io_ctx._sq_ring._sqes, Io_ctx._sq_ring._sqes_size);
    Io_ctx._sq_ring._sqes = MAP_FAILED;
  }
  // munmap sq_ptr
  if (Io_ctx._sq_ring._sq_ptr != MAP_FAILED) {
    munmap(Io_ctx._sq_ring._sq_ptr, Io_ctx._sq_ring._sq_size);
    Io_ctx._sq_ring._sq_ptr = MAP_FAILED;
  }
  // munmap cq_ptr
  if (Io_ctx._cq_ring._cq_ptr != MAP_FAILED) {
    munmap(Io_ctx._cq_ring._cq_ptr, Io_ctx._cq_ring._cq_size);
    Io_ctx._cq_ring._cq_ptr = MAP_FAILED;
  }
  if (Io_ctx._ring_fd != -1) {
    io_uring_register(Io_ctx._ring_fd, IORING_UNREGISTER_FILES, NULL, 0);
    io_uring_register(Io_ctx._ring_fd, IORING_UNREGISTER_BUFFERS, NULL, 0);
    close(Io_ctx._ring_fd);
    Io_ctx._ring_fd = -1;
  }
}

int Block_io_open(const char* fname, bool sync_read) {
  // check if library is initialized
  if (Lib_init <= 0) {
    FMT_ASSERT(false, "Fatal: io not initialized.\n");
    return -1;
  }
  IS_TRUE(Io_ctx._ring_fd != -1, "io_uring fd invalid.\n");

  // open the file for read
  int fd = open(fname, O_RDONLY | __O_NOATIME);
  if (fd == -1) {
    perror("Fatal: data open()");
    return -1;
  }

  if (sync_read) {
    return fd;
  }

  // register fd to ring_fd
  int ret = io_uring_register(Io_ctx._ring_fd, IORING_REGISTER_FILES, &fd, 1);
  if (ret < 0) {
    perror("Fatal: file io_uring_register()");
    close(fd);
    return -1;
  }
  return fd;
}

void Block_io_close(int fd) { close(fd); }

bool Block_io_prefetch(int fd, uint64_t ofst, BLOCK_INFO* blk) {
  RTLIB_TM_START(RTM_IO_SUBMIT, rtm);
  IS_TRUE(blk->_blk_sts == BLK_INVALID, "block state is not invalid");
  // find a valid slot in submission queue
  unsigned head         = *Io_ctx._sq_ring._head;
  unsigned tail         = *Io_ctx._sq_ring._tail;
  unsigned ring_entries = *Io_ctx._sq_ring._ring_entries;
  unsigned next_tail    = tail + 1;
  read_barrier();
  if (next_tail - head > ring_entries) {
    IS_TRUE(false, "Fixme: no available slot in sq");
    return false;
  }
  unsigned index                = tail & *Io_ctx._sq_ring._ring_mask;
  Io_ctx._sq_ring._array[index] = index;
  tail                          = next_tail;
  if (*Io_ctx._sq_ring._tail != tail) {
    *Io_ctx._sq_ring._tail = tail;
    write_barrier();
  }

  // fill submission queue entry and submit
  struct io_uring_sqe* sqe = &Io_ctx._sq_ring._sqes[index];
  sqe->opcode              = IORING_OP_READV;
  sqe->fd                  = fd;
  sqe->flags               = 0;
  sqe->off                 = ofst;
  sqe->addr                = (unsigned long)&blk->_iovec;
  sqe->len                 = 1;
  sqe->buf_index           = 0;
  sqe->user_data           = (unsigned long)blk;
  int ret = io_uring_enter(Io_ctx._ring_fd, 1, 1, IORING_ENTER_GETEVENTS);
  if (ret < 0) {
    perror("Fatal: io_uring_enter()");
    return false;
  }
  blk->_blk_sts = BLK_PREFETCHING;
  RTLIB_TM_END(RTM_IO_SUBMIT, rtm);
  return true;
}

bool Block_io_read(int fd, uint64_t ofst, BLOCK_INFO* blk) {
  RTLIB_TM_START(RTM_IO_COMPLETE, rtm);
  // check if complete queue is empty
  unsigned             head = *Io_ctx._cq_ring._head;
  struct io_uring_cqe* cqe;
  do {
    read_barrier();
    if (head == *Io_ctx._cq_ring._tail) {
      IS_TRUE(false, "Fixme: nothing to read");
      return false;
    }

    // process complete queue entry
    unsigned index = head & *Io_ctx._cq_ring._ring_mask;
    cqe            = &Io_ctx._cq_ring._cqes[index];
    if (cqe->res < 0) {
      fprintf(stderr, "Fatal: cqes %s\n", strerror(-cqe->res));
      IS_TRUE(false, "Fixme: async read error");
      return false;
    }
    BLOCK_INFO* cq_blk = (BLOCK_INFO*)cqe->user_data;
    IS_TRUE(cq_blk->_blk_sts == BLK_PREFETCHING, "not in prefetching state");
    cq_blk->_blk_sts = BLK_READY;
    head++;
  } while (cqe->user_data != (unsigned long)blk);

  *Io_ctx._cq_ring._head = head;
  write_barrier();
  RTLIB_TM_END(RTM_IO_COMPLETE, rtm);
  return true;
}

#endif  // __linux__
