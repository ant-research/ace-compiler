//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <stdlib.h>
#include <string.h>
#include <stdatomic.h>

#ifdef _OPENMP
#include <omp.h>
#endif

#include "common/error.h"
#include "common/io_api.h"
#include "common/rt_api.h"

typedef struct {
    const char* _name;
    int         _count;
    void*       _ct[];
} IO_DATA;

// Per-slot init states:
enum {
    SLOT_UNINIT = 0,    // uninit
    SLOT_BUSY   = 1,      // in-progress
    SLOT_DONE   = 2       // done
};

// Per-slot input/output stores (NULL-terminated arrays of IO_DATA pointers).
static IO_DATA** Input_data[IO_MAX_SLOTS];
static IO_DATA** Output_data[IO_MAX_SLOTS];

// Per-slot init flags: prevents double-alloc race via atomic CAS.
static atomic_int Slot_initialized[IO_MAX_SLOTS];

// Calling thread's slot base.  0 = legacy (no bind, OMP fallback).
static __thread int Slot_idx = 0;

static IO_DATA* Io_find_data(IO_DATA** slot, const char* name, size_t idx) {
    while (*slot != NULL) {
        if (strcmp((*slot)->_name, name) == 0) {
            FMT_ASSERT(idx < (*slot)->_count, "index out of bounds\n");
            return *slot;
        }
        ++slot;
    }
    FMT_ASSERT(false, "fail to find %s.\n", name);
    return NULL;
}

static void* Io_create_data(const char* name, size_t count) {
    size_t sz = sizeof(IO_DATA) + count * sizeof(void*);
    IO_DATA* data = (IO_DATA*)malloc(sz);
    data->_name = name;
    data->_count = count;
    memset(data->_ct, 0, count * sizeof(void*));
    return data;
}

static inline int Io_active_slot(void) {
    int s = Slot_idx;
#ifdef _OPENMP
    if (omp_in_parallel())
        s += omp_get_thread_num();
#endif
    FMT_ASSERT(s >= 0 && s < IO_MAX_SLOTS, "IO slot %d out of range [0, IO_MAX_SLOTS=%d).", s, IO_MAX_SLOTS);
    return s;
}

static void Io_ensure_slot(int s) {
    int expected = SLOT_UNINIT;
    if (!atomic_compare_exchange_strong(&Slot_initialized[s], &expected, SLOT_BUSY)) {
        while (atomic_load(&Slot_initialized[s]) != SLOT_DONE) {}
        return;
    }

    int isize = Get_input_count();
    int osize = Get_output_count();

    if (isize > 0) {
        Input_data[s] = (IO_DATA**)malloc((isize + 1) * sizeof(IO_DATA*));
        for (int i = 0; i < isize; ++i) {
            DATA_SCHEME* scheme = Get_encode_scheme(i);
            Input_data[s][i] = Io_create_data(scheme->_name, scheme->_count);
        }
        Input_data[s][isize] = NULL;
    }

    if (osize > 0) {
        Output_data[s] = (IO_DATA**)malloc((osize + 1) * sizeof(IO_DATA*));
        for (int i = 0; i < osize; ++i) {
            DATA_SCHEME* scheme = Get_decode_scheme(i);
            Output_data[s][i] = Io_create_data(scheme->_name, scheme->_count);
        }
        Output_data[s][osize] = NULL;
    }

    atomic_store(&Slot_initialized[s], SLOT_DONE);
}

void Io_init() {
    Io_ensure_slot(0);
}

void Io_bind(int slot_id) {
    Slot_idx = slot_id;
}

void Io_fini() {
    int isize = Get_input_count();
    int osize = Get_output_count();
    for (int s = 0; s < IO_MAX_SLOTS; ++s) {
        if (Input_data[s] != NULL) {
            for (int i = 0; i < isize; ++i) {
                free(Input_data[s][i]);
            }
            free(Input_data[s]);
            Input_data[s] = NULL;
        }

        if (Output_data[s] != NULL) {
            for (int i = 0; i < osize; ++i) {
                free(Output_data[s][i]);
            }
            free(Output_data[s]);
            Output_data[s] = NULL;
        }
        atomic_store(&Slot_initialized[s], SLOT_UNINIT);
    }
}

void Io_set_input(const char* name, size_t idx, void* ct) {
    int s = Io_active_slot();
    if (Input_data[s] == NULL) {
        Io_ensure_slot(s);
    }
    Io_find_data(Input_data[s], name, idx)->_ct[idx] = ct;
}

void* Io_get_input(const char* name, size_t idx) {
    int s = Io_active_slot();
    if (Input_data[s] == NULL) {
        Io_ensure_slot(s);
    }
    return Io_find_data(Input_data[s], name, idx)->_ct[idx];
}

void Io_set_output(const char* name, size_t idx, void* ct) {
    int s = Io_active_slot();
    if (Output_data[s] == NULL) {
        Io_ensure_slot(s);
    }
    Io_find_data(Output_data[s], name, idx)->_ct[idx] = ct;
}

void* Io_get_output(const char* name, size_t idx) {
    int s = Io_active_slot();
    if (Output_data[s] == NULL) {
        Io_ensure_slot(s);
    }
    return Io_find_data(Output_data[s], name, idx)->_ct[idx];
}