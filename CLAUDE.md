# CLAUDE.md — Project Context for AI Sessions

## Project Overview

ACE (Ant Cryptographic Engine) FHE Compiler — a homomorphic encryption compiler targeting GPU backends (Phantom). Key feature: GPU Direct Storage (GDS) IO for loading large FHE keys (~22 GiB galois keys for ResNet-20) directly from NVMe to GPU HBM via cuFile, bypassing host memory.

## Test Environment

- **Docker**: `4c228eb24ce5` (image: `reg.docker.alibaba-inc.com/ace/dev-cuda:12.8.0-devel-ubuntu24.04`)
- **GPU**: NVIDIA A100 80GB PCIe (PCIe 4.0 x16)
- **NVMe**: 4× Samsung PM1743 PCIe 5.0 x4 (`/mnt/nvme0`–`/mnt/nvme3`, XFS)
- **cuFile**: nvidia-fs 2.28.2 / cufile 1.17.0.46
- **cuFile Config**: `/etc/cufile.json` — `max_direct_io_size_kb=16384`, `parallel_io=true`, `max_io_threads=4`
- **Driver**: 570.124.06 / CUDA 12.8

## Build & Test Commands

```bash
# Build (in docker)
CI_TOKEN=f8fee7370ee5492eb79244d0a2aca4c4 ./scripts/dev-build.sh

# Quick smoke test
python3 -c 'from ace import frontend, runtime; print("OK")'
```

## Benchmark Commands

### Pre-requisites (ALWAYS before benchmarking)

```bash
# 1. Check GPU is idle (cuFile Bar1 mapping fails if other processes are running)
nvidia-smi --query-compute-apps=pid,used_memory --format=csv,noheader
pkill -9 -f bench_read   # kill zombie processes if needed

# 2. Drop page cache (required for cold-cache benchmarks)
echo 3 > /proc/sys/vm/drop_caches
```

### GDS Read Benchmark

```bash
echo 3 > /proc/sys/vm/drop_caches
python3 benchmark/io/bench_read.py --only-gds           # GDS configs only
python3 benchmark/io/bench_read.py                       # All datapaths (staged/mapped/gds)
python3 benchmark/io/bench_read.py --quick               # 1 GiB, D1, 1 repeat
python3 benchmark/io/bench_read.py --only-gds --chunk-size 8388608  # Custom chunk size
```

### Key Load Profile (KMS-TIMER + /proc/diskstats)

```bash
echo 3 > /proc/sys/vm/drop_caches
python3 .claude/io/key_load_profile.py                                          # D4
python3 .claude/io/key_load_profile.py --dir /mnt/nvme0/kms_demo_keys_resnet20_gds  # D1
```

### GDS Key-Read Benchmark (subprocess-per-config for cold start)

```bash
echo 3 > /proc/sys/vm/drop_caches
python3 benchmark/io/gds_key_read_bench.py /mnt/nvme0/kms_demo_keys_resnet20_4disk \
    --skip-merge --repeat 3
```

### bench_io_read + /proc/diskstats Verification

```bash
echo 3 > /proc/sys/vm/drop_caches
python3 .claude/io/bench_io_verify.py
```

### DD Baseline (raw NVMe bandwidth)

```bash
# Single-disk (single queue, underutilized)
dd if=/mnt/nvme0/kms_4d/model/galois_key.merged.0.bin of=/dev/null bs=1M iflag=direct

# 4-disk parallel (reveals true per-SSD peak ~8.5 GB/s each)
echo 3 > /proc/sys/vm/drop_caches
dd if=/mnt/nvme0/kms_4d/model/galois_key.merged.0.bin of=/dev/null bs=4M iflag=direct &
dd if=/mnt/nvme1/kms_4d/model/galois_key.merged.1.bin of=/dev/null bs=4M iflag=direct &
dd if=/mnt/nvme2/kms_4d/model/galois_key.merged.2.bin of=/dev/null bs=4M iflag=direct &
dd if=/mnt/nvme3/kms_4d/model/galois_key.merged.3.bin of=/dev/null bs=4M iflag=direct &
wait
```

### Debug Timing (ACE_GDS_PERF)

```bash
# Per-file timing from GdsTransport::read_gpu
ACE_GDS_PERF=1 python3 benchmark/io/bench_read.py --only-gds
```

### Data Preparation

```bash
# Split v2 merged files → v3 multi-file format
python3 benchmark/io/data_prep/split_merged_to_v3.py /mnt/nvme0/kms_demo_keys_resnet20_4disk --slots 4
```

## Key Data on Disk

- **D4**: `/mnt/nvme0/kms_demo_keys_resnet20_4disk` — 16 sub-files (4/disk), 21.79 GiB total
- **D1**: `/mnt/nvme0/kms_demo_keys_resnet20_gds` — 4 sub-files on 1 disk, 21.79 GiB total

---

## Key Code Paths

### IO DataPath Modes

| Mode | Path | Transport |
|------|------|-----------|
| `host_staged` | SSD → page cache → user buf → GPU | fread + cudaMemcpy |
| `host_mapped` | SSD → page cache → GPU | mmap + cudaMemcpy |
| `gds` | SSD → GPU | cuFile DMA |

### IO Source Files

| File | Purpose |
|------|---------|
| `fhe_lib/common/io/io_policy.cpp` | IoPolicy → IoConfig, `open_reader()`/`open_writer()` factories |
| `fhe_lib/common/io/gds_transport.cpp` | `GdsTransport` — per-call cuFile, auto BufRegister, parallel chunked read |
| `fhe_lib/common/io/gds_io.cu` | `gds_parallel_read()` — multi-handle atomic chunk-steal read engine |
| `fhe_lib/common/io/io_bulk_reader.cpp` | `IoBulkReader` — multi-file parallel read (group by disk, per-disk thread) |
| `fhe_lib/common/kms/key_index.h/cpp` | `GaloisKeyIndex` — JSON index for v3 galois key files |
| `fhe_lib/phantom/src/kms/key_serializer.cu` | `load_galois_key_merged` — core key IO with KMS-TIMER |
| `fhe_dsl/csrc/runtime/pybind_extension.cxx` | `bench_io_read`/`bench_io_write_prep` — Python benchmark bindings |
| `fhe_dsl/python/fhe/config/io_config.py` | `IoConfig`/`IoPolicySettings` Python API |

### Key Load Flow

1. Python: `IoPolicySettings(policy="high_throughput", disk_paths=DISKS).apply()` + `IoConfig(DataPath.GDS, threads=4, disk_paths=DISKS).apply()`
2. Python: `_C.set_key_path(key_dir)` → `FHERuntime(package)`
3. C++ `KeyManager::Init()` → `KeySerializer::Load_keys()`
4. `Load_keys()` → `load_galois_key_per_key()` → detects version≥2 → `load_galois_key_merged()`
5. `load_galois_key_merged()`: IoBulkReader groups by disk, 4 disk threads, per-file read+scatter
6. **Important**: `FHERuntime.__init__` does NOT auto-load keys — must call `runner.init()`

### Critical Constraints

- `cuda_auto_ptr` destructor calls `cudaFreeAsync(ptr_)` → each pointer from independent `cudaMalloc`. Cannot use sub-ranges of contiguous buffer → D2D scatter is unavoidable (~42ms for 22 GiB).
- `FHERuntime` is process-wide singleton — benchmarks use subprocess-per-config for cold-start.
- **GPU must be idle** before benchmarking — cuFile Bar1 mapping fails if other GPU processes exist.
- **Any reported GDS bandwidth > 26 GiB/s is impossible** on this hardware (PCIe 4.0 x16 limit).

### IO Policy System

- `IoPolicySettings` (Python) → `set_global_io_policy()` → global policy
- `IoConfig` (Python, expert) → `set_global_io_config()` → expert overrides
- `resolve_io_config(policy, loc)` merges policy defaults with global overrides
- Default chunk_size: **8 MiB** (optimal for GDS DMA throughput)
- Default num_io_threads: **4** (T2 is marginally better for D4 but T4 works for both)

---

## Quick Reference: Current Performance (8 MiB chunk, cold cache)

### bench_io_read (10 GiB/disk)

| Config | Median GiB/s | PCIe Util |
|--------|-------------|-----------|
| 4T1D single | 9.80 | ~NVMe limit |
| 4T1D multi | 9.10 | ~NVMe limit |
| **4T4D single** | **22.2** | **86%** |
| **4T4D multi** | **20.6** | **79%** |

### Key Load (21.79 GiB galois keys)

| Config | Pure IO | Total (incl D2D scatter) | Effective |
|--------|---------|-------------------------|-----------|
| D4 | 17.9 GiB/s (1.22s) | 1.26s | 17.3 GiB/s |
| D1 | 9.7 GiB/s (2.25s) | 2.29s | 9.5 GiB/s |

> Previous (512K chunk): D4 pure IO = 14.5 GiB/s. **8M chunk = +23% improvement.**

---

## Known Issues & Next Steps

1. **`bench_io_roundtrip` still has `total_gib` bug** — same `data_size * n_disks` calculation needs fixing
2. **Save_keys dispatch to merged format** — `KeySerializer::Save_keys` hardcodes `save_galois_key_per_key`. Need dispatch based on `IoConfig::galois_merged_format`.
3. **Remaining PCIe gap (~6 GiB/s, 79→100%)** — cuFile DMA per-chunk overhead, multi-file serial within disk, PCIe link-level framing. Low priority; diminishing returns.

---

## `.claude/io/` Documentation Index

Detailed technical docs and verification scripts for GDS IO work.

| File | Description |
|------|-------------|
| `hardware-profile.md` | Server hardware: SSDs, GPU, topology, cuFile config, bandwidth budget |
| `measured-results.md` | All cold-cache measurements: dd, bench, key load, chunk/thread sweeps, per-file timing |
| `bandwidth-verification.md` | How to verify bandwidth: dd, /proc/diskstats, KMS-TIMER, ACE_GDS_PERF, cold-cache discipline |
| `gds-read-analysis.md` | Code path analysis: GdsTransport flow, bench vs key load, overhead breakdown, optimization opportunities |
| `bench-bug-report.md` | `bench_io_read` `total_gib` inflation bug: root cause, fix, historical impact |
| `bench-tools.md` | Key data layout on disk, v3 index format, benchmark data files |
| `bench_io_verify.py` | `bench_io_read` + `/proc/diskstats` calibration script |
| `key_load_profile.py` | Key load profiling: KMS-TIMER + diskstats + PCIe utilization, D1/D4 |
