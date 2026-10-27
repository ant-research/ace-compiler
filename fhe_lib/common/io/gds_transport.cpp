//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief gds_transport.cpp
//! GDS (cuFile) I/O transport implementation.
//!
//! GdsTransport uses cuFile for GPU data I/O and POSIX FILE* for host data.
//! GPU reads use gds_parallel_read() for multi-threaded cuFileRead with an
//! atomic chunk dispenser. GPU writes use single-threaded cuFileWrite with
//! partial-write retry.
//!
//! If GDS errors occur and fallback_on_error is true, GPU I/O falls back
//! to POSIX pread/pwrite + cudaMemcpy (chunked to avoid OOM on large keys).

#include "fhe/io/gds_transport.h"

#include "fhe/io/cuda_check.h"
#include "fhe/io/cufile_api.h"
#include "fhe/io/gds_io.h"
#include "fhe/util/logging.h"

#include <atomic>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <thread>
#include <unistd.h>
#include <vector>

namespace rtio
{

//=============================================================================
// Internal: GDS write implementation (single-threaded)
//=============================================================================

//! Perform a cuFile write with fallback to POSIX + cudaMemcpy.
//! @return true if write succeeded (possibly via fallback)
static bool gds_write_impl(const std::string& path, const void* gpu_ptr, size_t offset, size_t size,
                           bool fallback_on_error, bool silent_fallback, size_t chunk_size, bool& used_gds)
{
    used_gds = false;
    if (size == 0) {
        used_gds = true;
        return true;
    }

    const CuFileApi& api = GdsDriver::instance().api();

    // Try cuFile write if available
    if (GdsDriver::instance().available() && api.Write) {
        // Pre-allocate file to the required size (needed for cuFile write)
        {
            int fd = open(path.c_str(), O_CREAT | O_RDWR, 0644);
            if (fd < 0) {
                if (!silent_fallback)
                    spdlog::warn("[gds] Failed to create file for writing: {} (errno={})", path, errno);
                goto posix_fallback;
            }
            off_t needed_size = static_cast<off_t>(offset + size);
            off_t current_size = lseek(fd, 0, SEEK_END);
            if (current_size < needed_size) {
                if (ftruncate(fd, needed_size) != 0) {
                    if (!silent_fallback)
                        spdlog::warn("[gds] Failed to extend file {} to {} bytes (errno={})", path,
                                     static_cast<long long>(needed_size), errno);
                    close(fd);
                    goto posix_fallback;
                }
            }
            close(fd);
        }

        // Open with O_DIRECT for cuFile
        int fd = open(path.c_str(), O_RDWR | O_DIRECT);
        if (fd < 0) {
            if (!silent_fallback)
                spdlog::warn("[gds] Failed to reopen file with O_DIRECT: {} (errno={})", path, errno);
            goto posix_fallback;
        }

        CUfileHandle_t fh = nullptr;
        CUfileDescr_t desc;
        memset(&desc, 0, sizeof(desc));
        desc.type = CU_FILE_HANDLE_TYPE_OPAQUE_FD;
        desc.handle.fd = fd;

        CUfileError_t err = api.HandleRegister(&fh, &desc);
        if (err != CUfileError_t::SUCCESS) {
            close(fd);
            goto posix_fallback;
        }

        // Register the entire GPU buffer
        err = api.BufRegister(const_cast<void*>(gpu_ptr), size, 0);
        bool buf_registered = (err == CUfileError_t::SUCCESS);

        // Write with retry for partial writes — single-threaded
        ssize_t total_written = 0;
        size_t remaining = size;
        size_t current_offset = offset;
        const char* current_ptr = static_cast<const char*>(gpu_ptr);
        bool write_ok = true;

        while (remaining > 0) {
            ssize_t bytes_written = api.Write(fh, current_ptr, remaining, static_cast<off_t>(current_offset), 0);
            if (bytes_written < 0) {
                write_ok = false;
                break;
            }
            if (bytes_written == 0) {
                write_ok = false;
                break;
            }
            total_written += bytes_written;
            current_offset += static_cast<size_t>(bytes_written);
            current_ptr += bytes_written;
            remaining -= static_cast<size_t>(bytes_written);
        }

        if (buf_registered) {
            api.BufDeregister(const_cast<void*>(gpu_ptr));
        }
        api.HandleDeregister(fh);
        close(fd);

        if (write_ok) {
            used_gds = true;
            return true;
        }

        // cuFile write failed
        if (!fallback_on_error)
            return false;
        if (!silent_fallback)
            spdlog::warn("[gds] cuFile write failed for {}, falling back to host-staged I/O", path);
    }

posix_fallback:
    // POSIX + cudaMemcpy fallback — chunked to avoid OOM on large keys
    {
        int fd = -1;
        if (offset == 0) {
            fd = open(path.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
        }
        else {
            fd = open(path.c_str(), O_CREAT | O_WRONLY, 0644);
        }
        if (fd < 0) {
            spdlog::error("[gds] Fallback: failed to open {} for writing (errno={})", path, errno);
            return false;
        }

        const size_t cs = (chunk_size > 0) ? chunk_size : 512UL * 1024;
        std::vector<uint8_t> host_buf(cs);
        size_t remaining = size;
        size_t pos = 0;
        bool ok = true;

        while (remaining > 0) {
            size_t this_chunk = (remaining > cs) ? cs : remaining;

            FHE_CHECK_CUDA(cudaMemcpy(host_buf.data(), static_cast<const char*>(gpu_ptr) + pos, this_chunk,
                                      cudaMemcpyDeviceToHost));

            ssize_t nwritten = pwrite(fd, host_buf.data(), this_chunk, static_cast<off_t>(offset + pos));
            if (nwritten != static_cast<ssize_t>(this_chunk)) {
                spdlog::error("[gds] Fallback: failed to write {} bytes at offset {} to {}", this_chunk, offset + pos,
                              path);
                ok = false;
                break;
            }

            pos += this_chunk;
            remaining -= this_chunk;
        }

        close(fd);
        return ok;
    }
}

//=============================================================================
// Constructor / Destructor
//=============================================================================

GdsTransport::GdsTransport(const std::string& path, const char* mode, bool fallback_on_error, size_t chunk_size,
                           int num_io_threads)
    : _path(path), _fallback_on_error(fallback_on_error), _chunk_size(chunk_size), _num_io_threads(num_io_threads)
{
    // Ensure GDS driver is initialized (via GdsDriver singleton)
    if (!GdsDriver::instance().available()) {
        throw std::runtime_error("[GdsTransport] GDS driver not available for: " + path);
    }

    const char* fmode = nullptr;
    if (mode[0] == 'w' || mode[0] == 'W') {
        fmode = "w+b";
    }
    else {
        fmode = "r+b";
    }

    _fd = std::fopen(path.c_str(), fmode);
    if (!_fd && mode[0] != 'w' && mode[0] != 'W') {
        _fd = std::fopen(path.c_str(), "rb");
    }
    if (!_fd) {
        throw std::runtime_error(std::string("[GdsTransport] Failed to open file: ") + path +
                                 " (errno=" + std::to_string(errno) + ")");
    }

    // Get file size
    if (std::fseek(_fd, 0, SEEK_END) == 0) {
        _file_size = static_cast<size_t>(std::ftell(_fd));
    }
}

GdsTransport::~GdsTransport()
{
    if (_fd) {
        std::fflush(_fd);
        std::fclose(_fd);
        _fd = nullptr;
    }
}

//=============================================================================
// Host data I/O (POSIX)
//=============================================================================

bool GdsTransport::write(const void* data, size_t offset, size_t size)
{
    if (!_fd || size == 0)
        return size == 0;

    if (std::fseek(_fd, static_cast<long>(offset), SEEK_SET) != 0) {
        return false;
    }

    size_t written = std::fwrite(data, 1, size, _fd);
    if (written != size)
        return false;

    std::fflush(_fd);

    size_t end = offset + size;
    if (end > _file_size)
        _file_size = end;

    return true;
}

bool GdsTransport::read(void* buf, size_t offset, size_t size)
{
    if (!_fd || size == 0)
        return size == 0;

    if (std::fseek(_fd, static_cast<long>(offset), SEEK_SET) != 0) {
        return false;
    }

    size_t nread = std::fread(buf, 1, size, _fd);
    if (nread != size)
        return false;

    return true;
}

//=============================================================================
// GPU data I/O (cuFile)
//=============================================================================

bool GdsTransport::write_gpu(const void* gpu_ptr, size_t offset, size_t size)
{
    if (size == 0)
        return true;

    // Flush FILE* buffer before cuFile writes via its own fd.
    if (_fd)
        std::fflush(_fd);

    bool used_gds = false;
    bool ok = gds_write_impl(_path, gpu_ptr, offset, size, _fallback_on_error,
                             true,  // silent_fallback — GdsTransport logs once
                             _chunk_size, used_gds);
    _used_gds = used_gds;

    if (!used_gds && !_fallback_logged) {
        _fallback_logged = true;
        spdlog::warn("[gds] cuFile write failed for {}, falling back to host-staged I/O", _path);
    }

    if (!ok)
        return false;

    // Update file size
    size_t end = offset + size;
    if (end > _file_size)
        _file_size = end;

    return true;
}

bool GdsTransport::read_gpu(void* gpu_ptr, size_t offset, size_t size)
{
    if (size == 0)
        return true;

    const CuFileApi& api = GdsDriver::instance().api();
    auto t_enter = std::chrono::high_resolution_clock::now();

    // Try cuFile read if available
    if (GdsDriver::instance().available() && api.Read) {
        // Register GPU buffer for cuFile if not already registered
        bool buf_registered_here = false;
        bool already_registered = false;
        for (const auto& rb : _registered_bufs) {
            if (rb.ptr == gpu_ptr) {
                already_registered = true;
                break;
            }
        }
        if (!already_registered && gpu_ptr && size > 0) {
            auto t0 = std::chrono::high_resolution_clock::now();
            CUfileError_t err = api.BufRegister(gpu_ptr, size, 0);
            auto t1 = std::chrono::high_resolution_clock::now();
            buf_registered_here = (err == CUfileError_t::SUCCESS);
            {
                double bufreg_ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
                spdlog::debug("[gds] BufRegister: {:.3f} ms (ok={}, size={:.1f} MiB)", bufreg_ms, buf_registered_here,
                              size / 1048576.0);
            }
            if (!buf_registered_here && !_fallback_logged) {
                spdlog::warn("[gds] BufRegister failed for {} ptr={} size={} (err={}), trying anyway", _path, gpu_ptr,
                             size, static_cast<int>(err));
            }
        }

        // Open the file with O_DIRECT for cuFile
        auto t_open0 = std::chrono::high_resolution_clock::now();
        int fd = open(_path.c_str(), O_RDONLY | O_DIRECT);
        auto t_open1 = std::chrono::high_resolution_clock::now();
        double open_ms = std::chrono::duration<double, std::milli>(t_open1 - t_open0).count();
        if (fd >= 0) {
            // Register cuFile handles for parallel read
            int num_threads = (_num_io_threads > 0) ? _num_io_threads : 1;
            int actual_threads = std::min(num_threads, static_cast<int>((size + _chunk_size - 1) / _chunk_size));
            if (actual_threads < 1)
                actual_threads = 1;

            // Open one fd + cuFileHandle per thread
            struct HandleInfo
            {
                int fd;
                void* fh;  // CUfileHandle_t
            };

            std::vector<HandleInfo> handles(actual_threads);
            bool all_ok = true;
            auto t_handle0 = std::chrono::high_resolution_clock::now();
            for (int t = 0; t < actual_threads; t++) {
                handles[t].fh = nullptr;
                if (t == 0) {
                    handles[t].fd = fd;  // reuse the already-opened fd
                }
                else {
                    handles[t].fd = open(_path.c_str(), O_RDONLY | O_DIRECT);
                    if (handles[t].fd < 0) {
                        all_ok = false;
                        break;
                    }
                }

                CUfileDescr_t desc;
                memset(&desc, 0, sizeof(desc));
                desc.type = CU_FILE_HANDLE_TYPE_OPAQUE_FD;
                desc.handle.fd = handles[t].fd;

                CUfileError_t err = api.HandleRegister(reinterpret_cast<CUfileHandle_t*>(&handles[t].fh), &desc);
                if (err != CUfileError_t::SUCCESS) {
                    handles[t].fh = nullptr;
                    all_ok = false;
                    break;
                }
            }
            auto t_handle1 = std::chrono::high_resolution_clock::now();
            double handle_ms = std::chrono::duration<double, std::milli>(t_handle1 - t_handle0).count();

            bool read_ok = false;
            double read_ms = 0;
            if (all_ok) {
                // Extract handle pointers for gds_parallel_read
                std::vector<void*> handle_ptrs(actual_threads);
                for (int t = 0; t < actual_threads; t++) {
                    handle_ptrs[t] = handles[t].fh;
                }

                auto t_read0 = std::chrono::high_resolution_clock::now();
                read_ok = gds_parallel_read(api, handle_ptrs.data(), actual_threads, gpu_ptr, offset, size,
                                            _chunk_size);
                auto t_read1 = std::chrono::high_resolution_clock::now();
                read_ms = std::chrono::duration<double, std::milli>(t_read1 - t_read0).count();
            }

            // Cleanup: deregister handles and close fds
            auto t_cleanup0 = std::chrono::high_resolution_clock::now();
            for (int t = actual_threads - 1; t >= 0; t--) {
                if (handles[t].fh) {
                    api.HandleDeregister(static_cast<CUfileHandle_t>(handles[t].fh));
                }
                if (handles[t].fd >= 0) {
                    close(handles[t].fd);
                }
            }
            auto t_cleanup1 = std::chrono::high_resolution_clock::now();
            double cleanup_ms = std::chrono::duration<double, std::milli>(t_cleanup1 - t_cleanup0).count();

            double bufdereg_ms = 0;
            if (read_ok) {
                // Deregister buffer if we registered it here
                if (buf_registered_here) {
                    auto t0 = std::chrono::high_resolution_clock::now();
                    api.BufDeregister(gpu_ptr);
                    auto t1 = std::chrono::high_resolution_clock::now();
                    bufdereg_ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
                }
                {
                    auto t_exit = std::chrono::high_resolution_clock::now();
                    double total_ms = std::chrono::duration<double, std::milli>(t_exit - t_enter).count();
                    spdlog::debug("[gds] read_gpu: {:.3f} ms (open={:.1f} handle_reg={:.1f} read={:.1f} cleanup={:.1f} "
                                  "bufdereg={:.1f}) "
                                  "size={:.1f} MiB threads={} bw={:.2f} GiB/s",
                                  total_ms, open_ms, handle_ms, read_ms, cleanup_ms, bufdereg_ms, size / 1048576.0,
                                  actual_threads, (size / 1073741824.0) / (total_ms / 1000.0));
                }
                _used_gds = true;
                return true;
            }

            // cuFile read failed — deregister buffer if we registered it
            if (buf_registered_here) {
                api.BufDeregister(gpu_ptr);
            }

            if (!_fallback_on_error)
                return false;
            if (!_fallback_logged) {
                _fallback_logged = true;
                spdlog::warn("[gds] cuFile read failed for {}, falling back to host-staged I/O", _path);
            }
            // Fall through to POSIX fallback
        }
        else {
            // Could not open file with O_DIRECT — deregister buffer if we registered it
            if (buf_registered_here) {
                api.BufDeregister(gpu_ptr);
            }
            // Could not open file with O_DIRECT
            if (!_fallback_on_error)
                return false;
            if (!_fallback_logged) {
                _fallback_logged = true;
                spdlog::warn("[gds] Failed to open {} with O_DIRECT, falling back to host-staged I/O", _path);
            }
        }
    }

    // POSIX + cudaMemcpy fallback — chunked to avoid OOM on large keys
    {
        int fb_fd = open(_path.c_str(), O_RDONLY);
        if (fb_fd < 0) {
            spdlog::error("[gds] Fallback: failed to open {} for reading (errno={})", _path, errno);
            return false;
        }

        const size_t cs = _chunk_size;
        std::vector<uint8_t> host_buf(cs);
        size_t remaining = size;
        size_t pos = 0;
        bool ok = true;

        while (remaining > 0) {
            size_t this_chunk = (remaining > cs) ? cs : remaining;
            ssize_t nread = pread(fb_fd, host_buf.data(), this_chunk, static_cast<off_t>(offset + pos));
            if (nread != static_cast<ssize_t>(this_chunk)) {
                spdlog::error("[gds] Fallback: failed to read {} bytes at offset {} from {}", this_chunk, offset + pos,
                              _path);
                ok = false;
                break;
            }

            FHE_CHECK_CUDA(
                cudaMemcpy(static_cast<char*>(gpu_ptr) + pos, host_buf.data(), this_chunk, cudaMemcpyHostToDevice));
            pos += this_chunk;
            remaining -= this_chunk;
        }

        close(fb_fd);
        _used_gds = false;
        return ok;
    }
}

//=============================================================================
// CRC32 (not computed for GDS path — hardware ECC protects data)
//=============================================================================

void GdsTransport::reset_crc()
{
    // GDS path: CRC is not computed. Hardware ECC protects the data path.
}

uint32_t GdsTransport::crc32() const
{
    return 0;  // Signal that CRC was not computed
}

//=============================================================================
// Info
//=============================================================================

const char* GdsTransport::transport_name() const
{
    if (!_used_gds)
        return "gds(fallback)";
    if (_num_io_threads > 1)
        return "gds+mt";
    return "gds";
}

//=============================================================================
// GPU buffer registration (cuFile)
//=============================================================================

bool GdsTransport::register_buffer(void* gpu_ptr, size_t size)
{
    if (!gpu_ptr || size == 0)
        return true;
    const auto& api = GdsDriver::instance().api();
    if (!api.loaded || !api.BufRegister)
        return true;
    CUfileError_t err = api.BufRegister(gpu_ptr, size, 0);
    if (err != CUfileError_t::SUCCESS) {
        spdlog::warn("[gds] BufRegister failed for ptr={} size={}", gpu_ptr, size);
        return false;
    }
    _registered_bufs.push_back({gpu_ptr, size});
    return true;
}

void GdsTransport::deregister_all()
{
    const auto& api = GdsDriver::instance().api();
    if (!api.loaded || !api.BufDeregister)
        return;
    for (auto& buf : _registered_bufs) {
        api.BufDeregister(buf.ptr);
    }
    _registered_bufs.clear();
}

}  // namespace rtio
