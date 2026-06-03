//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <algorithm>
#include <chrono>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <vector>

#include <omp.h>
#include <spdlog/spdlog.h>

#include "ace/runtime/batch_runner.h"
#include "ace/runtime/config_manager.h"
#include "ace/runtime/fhe_profiling.h"

namespace ace {
namespace runtime {

BATCH_RUNNER::BATCH_RUNNER(std::shared_ptr<KERNEL_RUNNER> runner) : _runner(std::move(runner)) {
    if (!_runner) {
        throw std::runtime_error("BATCH_RUNNER: runner cannot be null");
    }
}

std::vector<long> BATCH_RUNNER::Get_output_shape() const {
    std::vector<long> shape = {1, 1, 1, 1};
    auto config = CONFIG_MANAGER::Instance().Get_config();
    if (config._decode_sch.size() > 0) {
        const auto& ds = config._decode_sch[0];
        shape[0] = static_cast<long>(ds._shape._n);
        shape[1] = static_cast<long>(ds._shape._c);
        shape[2] = static_cast<long>(ds._shape._h);
        shape[3] = static_cast<long>(ds._shape._w);
    }
    return shape;
}

torch::Tensor BATCH_RUNNER::Make_output_tensor(
    double* raw_result,
    const std::vector<long>& shape) const {
    auto options = torch::TensorOptions().dtype(torch::kFloat64);
    torch::Tensor output = torch::from_blob(raw_result, shape, options).clone();
    free(raw_result);
    return output;
}

BATCH_RESULT BATCH_RUNNER::Run_sequential_batch(
    const std::vector<std::vector<torch::Tensor>>& batch_inputs,
    const std::string& output_name,
    bool verbose) {

    FHE_PROFILE_SCOPE("fhe::run_batch_sequential");
    auto batch_start = std::chrono::high_resolution_clock::now();
    const size_t N = batch_inputs.size();

    std::vector<long> output_shape = Get_output_shape();
    BATCH_RESULT result;
    result.outputs.resize(N);
    result.num_success = 0;
    result.num_failure = 0;

    double min_ms = std::numeric_limits<double>::max();
    double max_ms = 0.0;

    for (size_t i = 0; i < N; ++i) {
        auto img_start = std::chrono::high_resolution_clock::now();
        bool ok = false;
        try {
            double* raw = _runner->Run(batch_inputs[i], output_name);
            result.outputs[i] = Make_output_tensor(raw, output_shape);
            result.num_success++;
            ok = true;
        } catch (const std::exception& e) {
            result.outputs[i] = torch::zeros(output_shape, torch::kFloat64);
            result.num_failure++;
            if (verbose) {
                spdlog::info("[Batch {}/{}] FAILED: {}", i + 1, N, e.what());
            }
        }
        auto img_end = std::chrono::high_resolution_clock::now();
        double img_ms = std::chrono::duration<double, std::milli>(img_end - img_start).count();
        min_ms = std::min(min_ms, img_ms);
        max_ms = std::max(max_ms, img_ms);

        if (verbose && ok) {
            // Print prediction: argmax of flattened output
            auto flat = result.outputs[i].flatten();
            int64_t pred = flat.argmax().item<int64_t>();
            spdlog::info("[Batch {}/{}] OK  pred={}  time={:.1f}ms",
                         i + 1, N, pred, img_ms);
        }
    }

    auto batch_end = std::chrono::high_resolution_clock::now();
    result.timing.total_ms = std::chrono::duration<double, std::milli>(batch_end - batch_start).count();
    result.timing.num_images = static_cast<int>(N);
    result.timing.avg_per_image_ms = (N > 0) ? result.timing.total_ms / N : 0.0;
    result.timing.min_image_ms = (result.num_success > 0) ? min_ms : 0.0;
    result.timing.max_image_ms = (result.num_success > 0) ? max_ms : 0.0;

    return result;
}

BATCH_RESULT BATCH_RUNNER::Run_parallel_batch(
    const std::vector<std::vector<torch::Tensor>>& batch_inputs,
    const std::string& output_name,
    int num_threads,
    bool verbose) {

    FHE_PROFILE_SCOPE("fhe::run_batch_parallel");
    if (num_threads <= 0) {
        num_threads = static_cast<int>(std::max(1u, std::thread::hardware_concurrency()));
    }
    num_threads = std::min(num_threads, static_cast<int>(batch_inputs.size()));
    if (num_threads < 1) num_threads = 1;

    auto batch_start = std::chrono::high_resolution_clock::now();
    const size_t N = batch_inputs.size();

    std::vector<long> output_shape = Get_output_shape();

    // Collect raw results and timing in the parallel region.
    // torch::Tensor creation is done sequentially afterward to avoid
    // PyTorch thread-safety issues inside OpenMP.
    std::vector<double*> raw_results(N, nullptr);
    std::vector<bool> success(N, false);
    std::vector<double> img_times_ms(N, 0.0);
    int num_success = 0;
    int num_failure = 0;

    #pragma omp parallel for reduction(+:num_success,num_failure) \
                             num_threads(num_threads) schedule(dynamic)
    for (size_t i = 0; i < N; ++i) {
        auto img_start = std::chrono::high_resolution_clock::now();
        try {
            // Phase 1: Prepare input (serialized)
            #pragma omp critical(batch_io)
            {
                _runner->Prepare_input_only(batch_inputs[i]);
            }

            // Phase 2: Execute FHE computation (parallel)
            _runner->Execute();

            // Phase 3: Get output (serialized)
            #pragma omp critical(batch_io)
            {
                raw_results[i] = _runner->Get_output(output_name);
            }

            success[i] = true;
            num_success++;
        } catch (const std::exception& e) {
            num_failure++;
        }
        auto img_end = std::chrono::high_resolution_clock::now();
        img_times_ms[i] = std::chrono::duration<double, std::milli>(img_end - img_start).count();
    }

    // Create torch::Tensors sequentially from raw results (thread-safe)
    std::vector<torch::Tensor> outputs(N);
    for (size_t i = 0; i < N; ++i) {
        if (success[i] && raw_results[i] != nullptr) {
            outputs[i] = Make_output_tensor(raw_results[i], output_shape);
        } else {
            outputs[i] = torch::zeros(output_shape, torch::kFloat64);
        }
    }

    // Verbose: print per-image results after parallel region
    if (verbose) {
        for (size_t i = 0; i < N; ++i) {
            if (success[i]) {
                auto flat = outputs[i].flatten();
                int64_t pred = flat.argmax().item<int64_t>();
                spdlog::info("[Batch {}/{}] OK  pred={}  time={:.1f}ms",
                             i + 1, N, pred, img_times_ms[i]);
            } else {
                spdlog::info("[Batch {}/{}] FAILED  time={:.1f}ms",
                             i + 1, N, img_times_ms[i]);
            }
        }
    }

    auto batch_end = std::chrono::high_resolution_clock::now();

    BATCH_RESULT result;
    result.outputs = std::move(outputs);
    result.timing.total_ms = std::chrono::duration<double, std::milli>(batch_end - batch_start).count();
    result.timing.num_images = static_cast<int>(N);
    result.timing.avg_per_image_ms = (N > 0) ? result.timing.total_ms / N : 0.0;
    result.timing.min_image_ms = *std::min_element(img_times_ms.begin(), img_times_ms.end());
    result.timing.max_image_ms = *std::max_element(img_times_ms.begin(), img_times_ms.end());
    result.num_success = num_success;
    result.num_failure = num_failure;

    return result;
}

}  // namespace runtime
}  // namespace ace