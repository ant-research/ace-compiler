//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef ACE_RUNTIME_BATCH_RUNNER_H
#define ACE_RUNTIME_BATCH_RUNNER_H

#include <torch/extension.h>

#include <chrono>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "ace/runtime/kernel_runner.h"

namespace ace {
namespace runtime {

//! Timing statistics for a batch inference run.
struct BATCH_TIMING {
    double total_ms        = 0.0;  //!< Total wall-clock time in milliseconds
    double avg_per_image_ms = 0.0;  //!< Average time per image in milliseconds
    double min_image_ms    = 0.0;  //!< Fastest single image in milliseconds
    double max_image_ms    = 0.0;  //!< Slowest single image in milliseconds
    int    num_images      = 0;    //!< Number of images processed
};

//! Result of a batch inference run.
struct BATCH_RESULT {
    std::vector<torch::Tensor> outputs;  //!< Output tensors, one per input
    BATCH_TIMING timing;                  //!< Timing statistics
    int num_success = 0;                 //!< Number of successful inferences
    int num_failure = 0;                 //!< Number of failed inferences
};

/**
 * @brief Batch inference runner with sequential and OpenMP parallel modes.
 *
 * Uses KERNEL_RUNNER's phase-split methods (Prepare_input_only, Execute,
 * Get_output) to enable the OpenMP critical-section pattern:
 *   - Prepare_input: serialized (critical section)
 *   - Execute (Main_graph): parallel
 *   - Get_output: serialized (critical section)
 */
class BATCH_RUNNER {
public:
    /**
     * @brief Construct a batch runner for the given kernel runner.
     * @param runner Shared pointer to an initialized KERNEL_RUNNER
     */
    explicit BATCH_RUNNER(std::shared_ptr<KERNEL_RUNNER> runner);

    ~BATCH_RUNNER() = default;

    // Non-copyable, non-movable
    BATCH_RUNNER(const BATCH_RUNNER&) = delete;
    BATCH_RUNNER& operator=(const BATCH_RUNNER&) = delete;

    /**
     * @brief Run batch inference sequentially (safe for all backends).
     *
     * Processes images one at a time using KERNEL_RUNNER::Run().
     *
     * @param batch_inputs Vector of input tensor lists (one per image)
     * @param output_name Name of the output to retrieve
     * @param verbose If true, print per-image progress and results
     * @return BATCH_RESULT with outputs, timing, and success/failure counts
     */
    BATCH_RESULT Run_sequential_batch(
        const std::vector<std::vector<torch::Tensor>>& batch_inputs,
        const std::string& output_name = "output",
        bool verbose = false);

    /**
     * @brief Run batch inference with OpenMP parallelism (CPU backends).
     *
     * Uses the critical-section pattern: Prepare_input and Get_output are
     * serialized via #pragma omp critical, while Execute (Main_graph) runs
     * in parallel across threads.
     *
     * @param batch_inputs Vector of input tensor lists (one per image)
     * @param output_name Name of the output to retrieve
     * @param num_threads Number of OpenMP threads (0 = hardware concurrency)
     * @param verbose If true, print per-image progress and results
     * @return BATCH_RESULT with outputs, timing, and success/failure counts
     */
    BATCH_RESULT Run_parallel_batch(
        const std::vector<std::vector<torch::Tensor>>& batch_inputs,
        const std::string& output_name = "output",
        int num_threads = 0,
        bool verbose = false);

private:
    //! Get the output shape from the current configuration.
    std::vector<long> Get_output_shape() const;

    //! Create a torch::Tensor from raw result data, cloning it.
    torch::Tensor Make_output_tensor(double* raw_result,
                                     const std::vector<long>& shape) const;

    std::shared_ptr<KERNEL_RUNNER> _runner;  //!< Shared ownership of kernel runner
};

}  // namespace runtime
}  // namespace ace

#endif  // ACE_RUNTIME_BATCH_RUNNER_H