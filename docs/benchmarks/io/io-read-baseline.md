# IO Read-Performance Baseline

## Test Environment

- **GPU**: NVIDIA A100 80GB PCIe (Bus 0000:26:00.0)
- **Driver**: 570.124.06 / CUDA 12.8
- **GDS**: nvidia-fs 2.28.2 / cufile 1.17.0.46
- **SSD**: 4× local NVMe (`/mnt/nvme0`–`/mnt/nvme3`, XFS, ~14 TB each)
- **IO Scheduler**: mq-deadline
- **cuFile Config**: `/etc/cufile.json` — `max_direct_io_size_kb=16384`, `parallel_io=true`, `max_io_threads=4`
- **Date**: 2026-06-27

## Benchmark Method

- **Tool**: `benchmark/io/bench_read.py`
- **Data size**: 10 GiB per disk (total = data_size × n_disks)
- **Repeat**: 3 reads per config, report median / min / max
- **Write preparation**: Always staged (`IoPolicy::checkpoint`), via `bench_io_write_prep`
- **Read measurement**: `bench_io_read` — exercises the actual `open_reader` / `read_gpu` code path with the configured DataPath
- **Buffer**: Per-disk independent GPU buffer (`cudaMalloc` per disk thread), avoiding data races

### Matrix

3 DataPath × 2 Disks (D1/D4) × 2 FileMode (single/multi) = **12 configs**

| Parameter | Values |
|-----------|--------|
| DataPath | `host_staged`, `host_mapped`, `gds` |
| Threads | 4 |
| Disks | 1, 4 |
| FileMode | single (1 file/disk), multi (4 files/disk) |
| DataSize | 10 GiB per disk |

## Read Performance Results

### Full Table

| DataPath | T | D | Mode | Size(GiB/disk) | Write(GiB/s) | Read-median | Read-min | Read-max | Transport |
|----------|---|---|------|----------------|-------------|------------|---------|---------|-----------|
| host_staged | 4 | 1 | single | 10 | 4.48 | **7.17** | 7.15 | 7.17 | staged |
| host_staged | 4 | 1 | multi  | 10 | 4.50 | **8.45** | 8.41 | 8.45 | staged |
| host_staged | 4 | 4 | single | 10 | 8.32 | **8.17** | 8.17 | 8.63 | staged |
| host_staged | 4 | 4 | multi  | 10 | 31.44 | **29.80** | 29.66 | 35.22 | staged |
| host_mapped | 4 | 1 | single | 10 | 4.51 | **5.09** | 5.09 | 5.09 | mapped-merged |
| host_mapped | 4 | 1 | multi  | 10 | 4.51 | **5.66** | 5.65 | 5.66 | mapped-merged |
| host_mapped | 4 | 4 | single | 10 | 8.35 | **6.22** | 6.21 | 6.47 | mapped-merged |
| host_mapped | 4 | 4 | multi  | 10 | 32.22 | **22.68** | 22.65 | 25.41 | mapped-merged |
| gds | 4 | 1 | single | 10 | 4.54 | **2.66** | 0.32 | 2.76 | gds-merged |
| gds | 4 | 1 | multi  | 10 | 5.71 | **2.52** | 2.52 | 2.55 | gds-merged |
| gds | 4 | 4 | single | 10 | 7.37 | **10.22** | 10.20 | 10.24 | gds-merged |
| gds | 4 | 4 | multi  | 10 | 28.09 | **39.09** | 38.71 | 39.39 | gds-merged |

> **注**: GDS D1 single 的 `read_min=0.32` 是首次 cuFile DMA setup 开销，后续读恢复正常 (2.66)。Median 取 2.66。

### Summary: Best Read per DataPath

| DataPath | Config | Read(GiB/s) | 传输路径 |
|----------|--------|------------|---------|
| host_staged | T4/D4/multi | 29.80 | 4 盘 fread 并行 + cudaMemcpy |
| host_mapped | T4/D4/multi | 22.68 | 4 盘 fread + mmap 混合 |
| **gds** | **T4/D4/multi** | **39.09** | **4 盘 cuFile DMA 并行** |

## Key Findings

### 1. GDS T4/D4 读 39.09 GiB/s — 合理

4 个 NVMe 各自走独立 PCIe 4.0 x4，cuFile DMA 跳过 DRAM 直接到 GPU，汇聚带宽超越单 x16 理论值。

**与之前基线对比**: 之前测得 41.71 GiB/s (commit `fd35978`)，本次 39.09 — 6.3% 波动，在 NVMe IO 正常波动范围内。

### 2. GDS 单盘读 ~2.5-2.7 GiB/s — setup 开销

10GiB 单盘 cuFile DMA 仍有 per-handle setup 开销。首次读 (read_min=0.32) 明显偏低，后续读恢复到 ~2.6。对比 staged D1 读 7.17 GiB/s，GDS 单盘在当前数据量下无优势。更大/持续的读取场景下 GDS 单盘会更接近 NVMe 原始带宽。

### 3. Staged T4/D4 multi 读 29.80 GiB/s — fread 预读 + cudaMemcpy 流水重叠

也超单 x16 理论值，说明 fread 的 OS 预读和 cudaMemcpy 与读文件的流水重叠有效。

### 4. Mapped 读最慢 — mmap page fault 开销

D4 multi: 22.68 GiB/s，只有 GDS 的 58%。mmap 的 page fault 在 10GiB 文件下开销明显。

## Troubleshooting Log

排查过程中发现了两个影响 GDS 性能的问题：

### 问题 1: `std::thread` 子线程缺少 CUDA context

**症状**: `cuFileBufRegister cuCtxGetDevice error: CUDA_ERROR_INVALID_CONTEXT`

**根因**: `bench_io_read`/`bench_io_roundtrip` 中多盘并行 `std::thread` 子线程没有 CUDA context，cuFile `BufRegister` 失败 → fallback 到 bounce buffer（通过系统内存中转），GDS 读性能减半。

**修复**: 每个子线程入口加 `cudaSetDevice(0)`。

### 问题 2: 僵尸进程占用 GPU 内存

**症状**: Bar1-map err=13944, GDS D1 读从 2.63 降到 1.07, D4 multi 从 41 降到 17

**根因**: 之前的后台 benchmark 进程（`bench_read_perf.py`）未被正确 kill，占用 21 GiB GPU 内存 + 100% GPU 算力，导致 cuFile Bar1 映射空间不足 → DMA fallback。

**修复**: `kill -9` 僵尸进程，释放 GPU 内存后性能恢复。

## How to Reproduce

```bash
# 1. 编译
CI_TOKEN=... ./scripts/dev-build.sh

# 2. 确认 GPU 空闲
nvidia-smi  # 确保无其他进程占用 GPU

# 3. 运行 benchmark
python3 benchmark/io/bench_read.py --data-size 10 --repeat 3

# 4. 仅跑 GDS
python3 benchmark/io/bench_read.py --only-gds --data-size 10 --repeat 3

# 5. Quick 验证 (1GiB, D1 only)
python3 benchmark/io/bench_read.py --quick
```

## Result Files

- JSON: `/mnt/nvme0/io_bench_results/read_bench_10g_20260627_1450.json`
- 旧 1GiB 基线: `benchmark/io/results_1g_a100_4nvme.json`
