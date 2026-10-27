// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

#pragma once

#include <cstddef>
#include <map>
#include <memory>
#include <vector>

#include "hestdparms.h"
#include "common.h"
#include "modulus.h"
#include "cuda_wrapper.cuh"

namespace phantom::util::global_variables {
    /**
    Default value for the standard deviation of the noise (error) distribution.
    */
    constexpr std::size_t prng_seed_uint64_count = 8;
    constexpr std::size_t prng_seed_byte_count = prng_seed_uint64_count * arith::bytes_per_uint64;

    constexpr double noise_standard_deviation = distributionParameter;

    constexpr double noise_distribution_width_multiplier = 6;

    constexpr double noise_max_deviation = noise_standard_deviation * noise_distribution_width_multiplier;

    /**
    This data structure is a key-value storage that maps degrees of the polynomial modulus
    to vectors of Modulus elements so that when used with the default value for the
    standard deviation of the noise distribution (noise_standard_deviation), the security
    level is at least 128 bits according to https://HomomorphicEncryption.org. This makes
    it easy for non-expert users to select secure parameters.
    */
    const std::map<std::size_t, std::vector<arith::Modulus>> &GetDefaultCoeffModulus128();

    /**
    This data structure is a key-value storage that maps degrees of the polynomial modulus
    to vectors of Modulus elements so that when used with the default value for the
    standard deviation of the noise distribution (noise_standard_deviation), the security
    level is at least 192 bits according to https://HomomorphicEncryption.org. This makes
    it easy for non-expert users to select secure parameters.
    */
    const std::map<std::size_t, std::vector<arith::Modulus>> &GetDefaultCoeffModulus192();

    /**
    This data structure is a key-value storage that maps degrees of the polynomial modulus
    to vectors of Modulus elements so that when used with the default value for the
    standard deviation of the noise distribution (noise_standard_deviation), the security
    level is at least 256 bits according to https://HomomorphicEncryption.org. This makes
    it easy for non-expert users to select secure parameters.
    */
    const std::map<std::size_t, std::vector<arith::Modulus>> &GetDefaultCoeffModulus256();

    // Global default CUDA stream, implement and init at context.cu
    extern std::unique_ptr<phantom::util::cuda_stream_wrapper> default_stream;

    // Multi-session (Mode B-P) per-thread CUDA stream override.
    // IMPORTANT: tl_current_stream is accessed ONLY inside phantom .so (via the
    // functions below), never cross-DSO. The compiled FHE kernel .so inlines
    // current_stream() which calls get_thread_stream() — a function defined in
    // phantom .so — so the thread_local read stays within phantom .so's TLS.
    // Inlining a direct thread_local read into the kernel .so would resolve to
    // a DIFFERENT TLS instance across DSOs (segfault). The function-call
    // indirection keeps the access confined to one DSO.
    extern thread_local phantom::util::cuda_stream_wrapper *tl_current_stream;

    // Resolve the stream a HE op should use on the calling thread: the
    // per-thread override if set, else default_stream. Defined in phantom .so
    // (context.cu) so the thread_local read is confined there.
    cuda_stream_wrapper &get_thread_stream();

    // Resolve the stream wrapper for the calling thread — usable as an inline
    // default-argument value in evaluate/ckks headers WITHOUT a cross-DSO
    // thread_local read (it forwards to get_thread_stream).
    inline phantom::util::cuda_stream_wrapper &current_stream() {
        return get_thread_stream();
    }

    // Set the calling thread's per-thread stream override (Mode B-P). Called by
    // fhe_lib's Activate_session_for_this_thread on each worker thread. Pass
    // nullptr to revert to the process-global default_stream.
    void set_thread_stream(phantom::util::cuda_stream_wrapper *s);

    // The per-thread PhantomGaloisTool override (Mode B-P) is declared in
    // "galois_tls.h" (global namespace) as get_thread_galois_tool() /
    // set_thread_galois_tool() — see that header. Kept out of globals.h because
    // galois.cuh (PhantomGaloisTool's definition) cannot be included here
    // without breaking the std headers that globals.h precedes.
}


