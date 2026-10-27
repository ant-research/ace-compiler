//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "ace/runtime/provider_manager.h"

#include "ace/runtime/config_manager.h"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <stdexcept>
#include <torch/torch.h>

namespace ace
{
namespace runtime
{

void PROVIDER_MANAGER::Register_provider(const std::string& name, const std::string& lib_path, bool use_cuda_graph)
{
    auto runner = std::make_shared<KERNEL_RUNNER>(lib_path, use_cuda_graph);
    if (!runner->Load_kernel()) {
        throw std::runtime_error("Failed to load kernel from: " + lib_path);
    }
    std::unique_lock<std::shared_mutex> lock(_runners_mutex);
    _runners[name] = std::move(runner);
}

void PROVIDER_MANAGER::Init(const std::string& provider_name)
{
    std::shared_lock<std::shared_mutex> lock(_runners_mutex);
    auto it = _runners.find(provider_name);
    if (it == _runners.end()) {
        throw std::runtime_error("Provider not registered: " + provider_name);
    }
    it->second->Init();
}

std::shared_ptr<KERNEL_RUNNER> PROVIDER_MANAGER::Get_runner(const std::string& provider_name) const
{
    std::shared_lock<std::shared_mutex> lock(_runners_mutex);
    auto it = _runners.find(provider_name);
    if (it == _runners.end()) {
        throw std::runtime_error("Provider not registered: " + provider_name);
    }
    return it->second;  // Returns a copy of the shared_ptr, keeping the runner alive
}

torch::Tensor PROVIDER_MANAGER::Run(const std::string& provider_name, const std::vector<torch::Tensor>& inputs,
                                    const std::string& output_name)
{
    auto runner = Get_runner(provider_name);

    double* raw_result = runner->Run(inputs, output_name);

    // Get output shape from config (decode_schemes)
    std::vector<long> output_shape_vec = {1, 1, 1, 1};
    auto config = CONFIG_MANAGER::Instance().Get_config();
    if (config._decode_sch.size() > 0) {
        const auto& decode_sch = config._decode_sch[0];
        output_shape_vec[0] = static_cast<long>(decode_sch._shape._n);
        output_shape_vec[1] = static_cast<long>(decode_sch._shape._c);
        output_shape_vec[2] = static_cast<long>(decode_sch._shape._h);
        output_shape_vec[3] = static_cast<long>(decode_sch._shape._w);
    }

    auto options = torch::TensorOptions().dtype(torch::kFloat64);
    torch::Tensor output = torch::from_blob(raw_result, output_shape_vec, options).clone();

    // Free the raw result buffer — data has been cloned into the tensor
    free(raw_result);

    return output;
}

// Mode B-P: Run_session runs prepare+execute+get on the calling thread, with
// the GIL RELEASED so that two Python worker threads calling Run_session(i)/
// Run_session(j) execute concurrently in native code (the concurrency point of
// B-P). Output tensor construction re-acquires the GIL.
torch::Tensor PROVIDER_MANAGER::Run_session(const std::string& provider_name, int session_idx,
                                            const std::vector<torch::Tensor>& inputs, const std::string& output_name)
{
    auto runner = Get_runner(provider_name);

    // Pre-fetch output shape under the GIL before releasing it.
    std::vector<long> output_shape_vec = {1, 1, 1, 1};
    auto config = CONFIG_MANAGER::Instance().Get_config();
    if (config._decode_sch.size() > 0) {
        const auto& decode_sch = config._decode_sch[0];
        output_shape_vec[0] = static_cast<long>(decode_sch._shape._n);
        output_shape_vec[1] = static_cast<long>(decode_sch._shape._c);
        output_shape_vec[2] = static_cast<long>(decode_sch._shape._h);
        output_shape_vec[3] = static_cast<long>(decode_sch._shape._w);
    }

    double* raw_result = nullptr;
    {
        py::gil_scoped_release release;  // allow native concurrency across threads
        raw_result = runner->Run_session(session_idx, inputs, output_name);
    }

    auto options = torch::TensorOptions().dtype(torch::kFloat64);
    torch::Tensor output = torch::from_blob(raw_result, output_shape_vec, options).clone();
    free(raw_result);
    return output;
}

std::string PROVIDER_MANAGER::Run_cipher(const std::string& provider_name, int session_idx,
                                         const std::vector<std::string>& input_bytes, const std::string& output_name)
{
    auto runner = Get_runner(provider_name);
    std::string out;
    {
        py::gil_scoped_release release;  // allow native concurrency across threads
        out = runner->Run_cipher(session_idx, input_bytes, output_name);
    }
    return out;
}

std::vector<double> PROVIDER_MANAGER::LastRunCipherTiming(const std::string& provider_name)
{
    // Read on the calling thread — caller must be on the SAME worker thread that
    // just ran Run_cipher (the backend timing is thread-local). The server uses
    // this to echo the per-stage split to the client.
    auto runner = Get_runner(provider_name);
    return runner->LastRunCipherTiming();
}

BATCH_RESULT PROVIDER_MANAGER::Run_batch(const std::string& provider_name,
                                         const std::vector<std::vector<torch::Tensor>>& batch_inputs,
                                         const std::string& output_name, bool verbose)
{

    auto runner = Get_runner(provider_name);
    BATCH_RUNNER batch_runner(runner);

    return batch_runner.Run_sequential_batch(batch_inputs, output_name, verbose);
}

BATCH_RESULT PROVIDER_MANAGER::Run_batch_parallel(const std::string& provider_name,
                                                  const std::vector<std::vector<torch::Tensor>>& batch_inputs,
                                                  const std::string& output_name, int num_threads, bool verbose)
{

    auto runner = Get_runner(provider_name);
    BATCH_RUNNER batch_runner(runner);

    return batch_runner.Run_parallel_batch(batch_inputs, output_name, num_threads, verbose);
}

void PROVIDER_MANAGER::Finalize(const std::string& provider_name)
{
    std::shared_lock<std::shared_mutex> lock(_runners_mutex);
    auto it = _runners.find(provider_name);
    if (it == _runners.end()) {
        throw std::runtime_error("Provider not registered: " + provider_name);
    }
    it->second->Finalize();
}

bool PROVIDER_MANAGER::Capture_graph(const std::string& provider_name)
{
    std::shared_lock<std::shared_mutex> lock(_runners_mutex);
    auto it = _runners.find(provider_name);
    if (it == _runners.end()) {
        throw std::runtime_error("Provider not registered: " + provider_name);
    }
    return it->second->Capture_graph();
}

// KMS key management — forward to the provider's runner. Requires Init() first
// (active FheContext inside the backend .so).
int PROVIDER_MANAGER::Save_keys(const std::string& provider_name, const std::string& key_dir)
{
    return Get_runner(provider_name)->KmsSaveKeys(key_dir);
}
int PROVIDER_MANAGER::Save_eval_keys(const std::string& provider_name, const std::string& key_dir)
{
    return Get_runner(provider_name)->KmsSaveEvalKeys(key_dir);
}
int PROVIDER_MANAGER::Load_keys(const std::string& provider_name, const std::string& key_dir)
{
    return Get_runner(provider_name)->KmsLoadKeys(key_dir);
}
int PROVIDER_MANAGER::Load_eval_keys(const std::string& provider_name, const std::string& key_dir)
{
    return Get_runner(provider_name)->KmsLoadEvalKeys(key_dir);
}
int PROVIDER_MANAGER::Keys_loaded_from_kms(const std::string& provider_name)
{
    return Get_runner(provider_name)->KmsKeysLoadedFromKms();
}
double PROVIDER_MANAGER::Get_galois_load_time_seconds(const std::string& provider_name)
{
    return Get_runner(provider_name)->KmsGetGaloisLoadTimeSeconds();
}
int PROVIDER_MANAGER::Create_session(const std::string& provider_name, const std::string& key_dir)
{
    return Get_runner(provider_name)->KmsCreateSession(key_dir);
}
int PROVIDER_MANAGER::Switch_session(const std::string& provider_name, int idx)
{
    return Get_runner(provider_name)->KmsSwitchSession(idx);
}

int PROVIDER_MANAGER::Release_session(const std::string& provider_name, int idx)
{
    return Get_runner(provider_name)->KmsReleaseSession(idx);
}

int PROVIDER_MANAGER::Num_sessions(const std::string& provider_name)
{
    return Get_runner(provider_name)->KmsNumSessions();
}

}  // namespace runtime
}  // namespace ace