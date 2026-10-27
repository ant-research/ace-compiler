# ResNet I/O Benchmark

## Test Environment

- **GPU**: NVIDIA A100
- **SSD**: 4× NVMe (`/mnt/nvme0`–`/mnt/nvme3`, ~14 TB each)
- **Key size**: ~22.7 GB (N=65536, L=37, 67 galois elements)
- **Key format**: per-key (67 × 349 MB files) and merged (1 × 21.8 GB file)
- **Backend**: phantom/cuda
- **Date**: 2025-06-26

## Optimization: `prepare_mod_polynomial` Deduplication

4 Bootstrapper instances (POW_15/14/13/12) share identical FHE parameters
(boundary_K=25, deg=59, scale_factor=2, inverse_deg=1, loge=10) but each ran
the RemezArcsin solver independently. RemezArcsin at RR_prec=1000 bits +
log_scan_step_diff=12 is extremely expensive.

**Before**: `prep_mod = 4 × ~4.2s = 16.8s`

**Fix**: Compute once on POW_15, copy polynomial results to the other 3.
`Polynomial::copy()` skips `poly_heap`, so `generate_poly_heap()` is called
after each copy.

**After**: `prep_mod = 1 × ~4.2s + 3 × ~0.07s ≈ 4.2s` (**-75%**)

**Change**: `fhe_lib/phantom/src/vendor_traits.cu` only — no upstream Phantom changes.

## Performance Results

### Time Hierarchy

```
Total (wall-clock)
├── Init (FheContext init)
│   ├── create_context      ~2.5s   (CKKS context, fixed)
│   ├── load_keys           varies  (I/O bound, key differentiator)
│   ├── init_bootstrap      ~5.2s   (includes prep_mod + LT)
│   │   ├── prep_mod        ~4.2s   (CPU: RemezArcsin + poly_heap)
│   │   └── LT              ~0.9s   (fixed)
│   └── pt_mgr              ~0.12s  (fixed)
└── Inference               ~8.3s   (GPU compute, fixed)
```

### load_keys Comparison: Per-Key vs Merged Format

The most important finding is the **key file format** impact on Host Mapped (mmap)
performance. The same ~22.7 GB of galois keys stored as 67 individual files vs.
one contiguous file produces dramatically different mmap results:

<table>
<tr>
  <th rowspan="2">I/O Config</th>
  <th colspan="2"><strong>Per-Key (67 files)</strong></th>
  <th colspan="2"><strong>Merged (1 file)</strong></th>
  <th rowspan="2">Merged speedup</th>
</tr>
<tr>
  <th>load_keys</th><th>Init</th><th>load_keys</th><th>Init</th>
</tr>
<tr><td>Host Staged 1T</td>  <td>3.57s</td><td>11.21s</td><td>3.41s</td><td>11.21s</td><td>1.05×</td></tr>
<tr><td>Host Staged 4T</td>  <td>3.25s</td><td>11.08s</td><td>3.39s</td><td>11.22s</td><td>0.96×</td></tr>
<tr><td><strong>Host Mapped 1T</strong></td>  <td>9.22s</td><td>17.07s</td><td><strong>4.85s</strong></td><td><strong>12.63s</strong></td><td><strong>1.90×</strong></td></tr>
<tr><td><strong>Host Mapped 4T</strong></td>  <td>4.90s</td><td>12.73s</td><td><strong>4.63s</strong></td><td><strong>12.53s</strong></td><td><strong>1.06×</strong></td></tr>
<tr><td>GDS 1T</td>          <td>17.64s</td><td>25.45s</td><td>8.85s</td><td>16.67s</td><td>1.99×</td></tr>
<tr><td>GDS 4T</td>          <td>2.43s</td><td>10.27s</td><td>2.96s</td><td>10.80s</td><td>0.82×</td></tr>
</table>

- **Merged format nearly doubles mmap 1T performance**: 9.22s → 4.85s. Eliminating 67
  per-key `mmap()`/`munmap()` syscalls and getting a single contiguous VMA dramatically
  reduces page fault overhead.
- **Merged format halves GDS 1T overhead**: 17.64s → 8.85s. The persistent `GdsMergedReadSession`
  avoids per-key cuFileHandle registration overhead.
- **Staged and GDS-4T are format-neutral**: fread and multi-threaded cuFile already
  handle sequential/large reads efficiently; merged format provides little or no benefit.

### ResNet-20 CIFAR-10 — Merged Format (N=65536, mul_depth=32)

<table>
<tr>
  <th rowspan="2">I/O Config</th>
  <th colspan="5"><strong>Init (FheContext init)</strong></th>
  <th rowspan="2"><strong>Inference</strong></th>
  <th rowspan="2"><strong>Total</strong></th>
</tr>
<tr>
  <th>create_context</th><th>load_keys</th><th>prep_mod</th><th>LT</th><th>pt_mgr</th>
</tr>
<tr><td>Host Staged 1T</td>      <td>2.52s</td><td>3.41s</td><td>4.23s</td><td>0.91s</td><td>0.11s</td><td>8.34s</td><td>19.54s</td></tr>
<tr><td>Host Staged 4T</td>      <td>2.52s</td><td>3.39s</td><td>4.26s</td><td>0.91s</td><td>0.11s</td><td>8.30s</td><td>19.53s</td></tr>
<tr><td>Host Mapped 1T</td>      <td>2.52s</td><td>4.85s</td><td>4.21s</td><td>0.91s</td><td>0.11s</td><td>8.33s</td><td>20.95s</td></tr>
<tr><td>Host Mapped 4T</td>      <td>2.52s</td><td>4.63s</td><td>4.33s</td><td>0.91s</td><td>0.11s</td><td>8.34s</td><td>20.87s</td></tr>
<tr><td>GDS 1T</td>             <td>2.52s</td><td>8.85s</td><td>4.26s</td><td>0.90s</td><td>0.11s</td><td>8.37s</td><td>25.04s</td></tr>
<tr><td>GDS 4T</td>             <td>2.52s</td><td>2.96s</td><td>4.27s</td><td>0.90s</td><td>0.11s</td><td>8.32s</td><td>19.12s</td></tr>
</table>

### ResNet-20 CIFAR-10 — Per-Key Format (N=65536, mul_depth=32)

<table>
<tr>
  <th rowspan="2">I/O Config</th>
  <th colspan="5"><strong>Init (FheContext init)</strong></th>
  <th rowspan="2"><strong>Inference</strong></th>
  <th rowspan="2"><strong>Total</strong></th>
</tr>
<tr>
  <th>create_context</th><th>load_keys</th><th>prep_mod</th><th>LT</th><th>pt_mgr</th>
</tr>
<tr><td>Host Staged 1T</td>      <td>2.52s</td><td>3.57s</td><td>4.22s</td><td>0.92s</td><td>0.11s</td><td>8.30s</td><td>19.66s</td></tr>
<tr><td>Host Staged 4T</td>      <td>2.52s</td><td>3.25s</td><td>4.25s</td><td>0.91s</td><td>0.11s</td><td>8.30s</td><td>19.38s</td></tr>
<tr><td>Host Mapped 1T</td>      <td>2.52s</td><td>9.22s</td><td>4.23s</td><td>0.91s</td><td>0.14s</td><td>8.30s</td><td>25.36s</td></tr>
<tr><td>Host Mapped 4T</td>      <td>2.52s</td><td>4.90s</td><td>4.26s</td><td>0.92s</td><td>0.11s</td><td>8.32s</td><td>21.05s</td></tr>
<tr><td>Host Mapped 4T 4disk</td><td>2.52s</td><td>4.43s</td><td>4.26s</td><td>0.91s</td><td>0.11s</td><td>8.32s</td><td>20.58s</td></tr>
<tr><td>GDS 1T 1disk</td>       <td>2.52s</td><td>17.64s</td><td>4.24s</td><td>0.91s</td><td>0.11s</td><td>8.26s</td><td>33.71s</td></tr>
<tr><td>GDS 4T 1disk</td>       <td>2.52s</td><td>2.43s</td><td>4.28s</td><td>0.91s</td><td>0.11s</td><td>8.31s</td><td>18.58s</td></tr>
<tr><td>GDS 4T 4disk</td>       <td>2.52s</td><td>1.63s</td><td>4.27s</td><td>0.92s</td><td>0.11s</td><td>8.33s</td><td>17.80s</td></tr>
</table>

### ResNet-32/44/56/110 — load_keys is model-independent

`load_keys` and `prep_mod` are **model-independent**: all ResNet variants share the
same CKKS parameters (N=65536), so key files and bootstrap polynomials are identical.
Only `pt_mgr` (weight encoding) and Inference vary by model depth.

| Model | pt_mgr | Inference |
|-------|--------|-----------|
| ResNet-20  | 0.11s | 8.3s |
| ResNet-32  | 0.18s | 13.8s |
| ResNet-44  | 0.24s | 19.1s |
| ResNet-56  | 0.33s | 24.4s |
| ResNet-110 | 0.58s | 48.5s |

To compute any model's Total for a given I/O config:
`Total = load_keys + 2.52 (create_ctx) + 4.24 (prep_mod) + 0.91 (LT) + pt_mgr + Inference`

### Init sub-stage definitions

- **create_context**: Create CKKS context (poly modulus, coefficient modulus, etc.). Fixed ~2.5s, not I/O related.
- **load_keys**: Read FHE keys (secret/public/relin/galois) from disk into GPU memory. I/O bound.
- **prep_mod**: Compute approximate modular reduction polynomials (RemezArcsin solver, CPU-bound, NTL RR_prec=1000). Optimized: compute once, share across 4 Bootstrapper instances.
- **LT**: Generate linear transform coefficients for bootstrap (coefftoslot/slottocoeff FFT). Fixed.
- **pt_mgr**: Encode model weights into CKKS plaintexts. Scales with model depth.

### Key Observations

1. **Merged format is the right default for mmap**: `Mapped 1T` drops from 9.22s (per-key)
   to 4.85s (merged) — a **1.9× improvement**. The 67 separate `mmap()`/`munmap()` calls
   on per-key files are the main bottleneck; a single contiguous VMA amortizes this.
2. **mmap is still ~42% slower than staged even with merged format**:
   `Mapped 1T` 4.85s vs `Staged 1T` 3.41s. Page fault latency on cold reads exceeds
   fread's kernel readahead benefit. The "skip one copy" advantage is small vs the
   PCIe H2D transfer cost (~25 GB/s).
3. **GDS remains fastest**: `GDS 4T` 2.43–2.96s is the best `load_keys` on single-disk.
   `GDS 4T 4disk` (per-key) at 1.63s is the overall champion.
4. **Merged format also helps GDS 1T**: 17.64s → 8.85s. The `GdsMergedReadSession`
   keeps a persistent cuFileHandle, avoiding per-key handle registration overhead.
5. **GDS 4T performs better with per-key format** (2.43s) than merged (2.96s): per-key
   allows 4 threads to independently register cuFileHandles on different files,
   increasing parallelism; merged shares a single file handle across threads.
6. **load_keys and prep_mod are model-independent**: all ResNet variants share the same CKKS parameters.
7. **pt_mgr scales with model depth**: 0.11s (ResNet-20) → 0.58s (ResNet-110).
8. **Inference dominates for deeper models**: ResNet-20 inference 8.3s (42% of total),
   ResNet-110 inference 48.5s (77–83% of total). I/O optimization matters most for
   shallow models.

### Host Mapped (mmap) I/O Analysis

MappedTransport uses `mmap(MAP_PRIVATE, PROT_READ)` + chunked `cudaMemcpy(HostToDevice)`
instead of StagedTransport's `fread()` + chunked `cudaMemcpy`. The key trade-off:

| Aspect | Staged (fread+memcpy) | Mapped (mmap+memcpy) |
|--------|----------------------|---------------------|
| Read path | fread → page cache → user buf → cudaMemcpy H2D | mmap → page cache → cudaMemcpy H2D (no extra copy) |
| Kernel readahead | ✅ kernel prefetches sequentially | ❌ page faults on demand |
| Syscall overhead (per-key) | 1× fopen/fread/fclose per file | 1× mmap + 1× munmap per file |
| Syscall overhead (merged) | 1× fopen/fread/fclose total | 1× mmap + 1× munmap total |
| Host memory | ~4 MB chunk buffer | 0 (reads from mapped pages) |
| Cold-read latency | Low (readahead) | Higher (page fault per page) |

**Why mmap is slower than staged (even with merged)**:
- Page fault latency on cold reads: 22.7 GB at 4 KB/page = ~5.9M page faults
- Kernel readahead for fread is highly effective for sequential large reads
- The "skip one copy" benefit (~4 GB/s host memory bandwidth saved) is small
  when PCIe H2D (~25 GB/s) dominates the transfer time

**Why mmap per-key is much worse than mmap merged**:
- 67 separate files → 67 × (mmap + munmap) = 134 syscalls + 67 VMA entries
- Each mmap starts with cold TLB; no cross-file readahead
- Kernel must validate page alignment per mmap call

**Where mmap helps**: warm-cache repeated reads (page faults = 0), environments
where GDS is unavailable, or where fread performance is poor (e.g. network filesystems
with high per-read latency but good bandwidth).

### prep_mod Optimization Impact

| Metric | Before (4× RemezArcsin) | After (1× + 3× copy) | Savings |
|--------|------------------------|---------------------|---------|
| prep_mod | 16.8s | 4.24s | **-75%** (12.6s) |
| FheContext init (GDS 4T 1disk) | ~23.2s | 9.55s | **-59%** (13.7s) |
| Total (GDS 4T 1disk) | ~31s | 17.91s | **-42%** (13s) |

### Legacy Data (pre-per-key, pre-dedup, single-file galois_key.bin)

| Scenario | Init(s) | Avg/img(s) | Total(s) | Notes |
|----------|---------|-----------|----------|-------|
| online (baseline) | 20.56 | 8.30 | 28.86 | key generation ~20s |
| offline-keys-staged | 82.19 | 9.63 | 91.82 | single-file, ~277 MB/s |
| offline-keys-gds | 31.96 | 8.95 | 40.90 | single-file, ~710 MB/s |

> Legacy data uses single-file galois_key.bin format, no multi-threaded I/O,
> no prep_mod dedup. Current data uses per-key/merged format + 4-thread I/O + prep_mod dedup.

## Test Commands

### Prerequisites: Save Keys

```bash
# Per-key format (default)
python3 examples/09_kms/03_resnet_save_keys.py \
    --model resnet20 /mnt/nvme0/kms_demo_keys_resnet20

# Merged format (single file, recommended for mmap)
python3 examples/09_kms/03_resnet_save_keys.py \
    --model resnet20 /mnt/nvme0/kms_demo_keys_resnet20_merged --merged

# Per-key with 4-disk distribution (for gds-4t-4disk)
python3 examples/09_kms/03_resnet_save_keys.py \
    --model resnet20 /mnt/nvme0/kms_demo_keys_resnet20_4disk \
    --disk-paths /mnt/nvme0/kms_4d /mnt/nvme1/kms_4d /mnt/nvme2/kms_4d /mnt/nvme3/kms_4d
```

### Run Benchmarks

```bash
# --- Merged format (recommended for Host Mapped) ---
KEY_DIR=/mnt/nvme0/kms_demo_keys_resnet20_merged

# Staged 1T / 4T
python3 examples/09_kms/04_resnet_load_keys.py $KEY_DIR \
    --io staged --num-io-threads 1
python3 examples/09_kms/04_resnet_load_keys.py $KEY_DIR \
    --io staged --num-io-threads 4

# Host Mapped 1T / 4T
python3 examples/09_kms/04_resnet_load_keys.py $KEY_DIR \
    --io host_mapped --num-io-threads 1
python3 examples/09_kms/04_resnet_load_keys.py $KEY_DIR \
    --io host_mapped --num-io-threads 4

# GDS 1T / 4T
python3 examples/09_kms/04_resnet_load_keys.py $KEY_DIR \
    --io gds --num-io-threads 1
python3 examples/09_kms/04_resnet_load_keys.py $KEY_DIR \
    --io gds --num-io-threads 4

# --- Per-key format (for 4-disk GDS) ---
KEY_DIR=/mnt/nvme0/kms_demo_keys_resnet20_gds

# GDS 4T 4disk
python3 examples/09_kms/04_resnet_load_keys.py /mnt/nvme0/kms_demo_keys_resnet20_4disk \
    --io gds --num-io-threads 4 \
    --disk-paths /mnt/nvme0/kms_4d /mnt/nvme1/kms_4d /mnt/nvme2/kms_4d /mnt/nvme3/kms_4d
```

### I/O Transport Configuration (Python API)

```python
from ace.fhe.config import IoConfig, DataPath
from ace import runtime as _C

_C.gds_available()                                    # Check GDS availability
IoConfig(data_path=DataPath.HOST_STAGED).apply()     # Force staged I/O
IoConfig(data_path=DataPath.AUTO).apply()             # Auto-detect GDS
IoConfig(data_path=DataPath.GDS).apply()              # Force GDS
IoConfig(data_path=DataPath.HOST_MAPPED).apply()      # Host mapped I/O (mmap+cudaMemcpy)
```

The `ACE_GDS_ENABLED` environment variable provides a process-level override
(`0`=force off, `1`=force on) for debugging or containerized environments.

## Known Issues

1. **Keys Persist in GPU HBM** — After `FHERuntime.finalize()`, keys are not
   released. Run different I/O scenarios in **separate processes**.

2. **4-disk key distribution**: When using `--disk-paths`, the C++ runtime looks
   for `galois_key.index.json` at `disk_paths[0]/model/`. Ensure the index file
   is present on the first disk path.

3. **cuFile and symlinks**: cuFile does not support symlinked key files. Use
   real files (cp) or hard links on the same filesystem.

4. **GDS single thread is extremely slow**: cuFile serial per-key reads have
   very high overhead. Always use `--num-io-threads 4` or more with GDS.
   Merged format reduces this (17.6s → 8.9s) but 4T is still recommended.

5. **Host Mapped per-key is slow**: 67 separate mmap/munmap calls add significant
   overhead. Use merged format when using `DataPath.HOST_MAPPED`.

6. **prep_mod still costs ~4.2s**: Single RemezArcsin is CPU-bound (NTL RR_prec=1000).
   Can be eliminated via future file-based caching (offline precomputation).

7. **MappedTransport per-key mmap/munmap**: Each `read_gpu()` call does a fresh
   `mmap()` + `munmap()`. A future `MappedMergedReadSession` that mmaps the
   entire file once and reads at offsets could further reduce mmap overhead.

## References

- [GDS I/O Transport Design](../design/gds-io-transport.md)
- [KMS Example README](../../examples/09_kms/README.md)
- [Offline Weight Encoding](../../examples/06_advanced/08_offline_encode.py)
