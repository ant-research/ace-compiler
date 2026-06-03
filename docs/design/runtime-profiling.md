# Runtime Profiling Design

## Overview

The runtime profiling system provides developers and users with FHE inference performance data, enabling them to optimize FHE inference performance through compiler and library improvements. The system provides three capabilities:

- **Timing visibility** — per-phase breakdown of CPU and GPU time, so users can identify bottlenecks.
- **Memory visibility** — GPU and CPU memory snapshots at each phase boundary, so users can track consumption and detect leaks.
- **Trace export** — Chrome Trace / Perfetto compatible output, so users can visualize the full inference timeline interactively.

The system is built on two layers:

```
┌─────────────────────────────────────────────────────────────────┐
│  Python API                                                     │
│  fhe.profiler() context manager, CompiledProgram.profile(),     │
│  ProfileResult with structured events + memory snapshots,       │
│  Chrome Trace export with memory counter tracks                 │
├─────────────────────────────────────────────────────────────────┤
│  C++ Instrumentation                                            │
│  FHE_PROFILE_SCOPE annotations (RECORD_USER_SCOPE + NVTX),     │
│  cudaMemGetInfo / VmRSS memory sampling at phase boundaries,   │
│  fhe::mem::* snapshot events, fhe::phase [N MB] event names    │
└─────────────────────────────────────────────────────────────────┘
```

The Python layer wraps `torch.profiler` with FHE-specific defaults and result parsing. The C++ layer emits the annotations and memory data that the Python layer consumes. Both layers work for both CPU and GPU environments.

## Motivation

FHE inference is orders of magnitude slower than plaintext computation — performance directly determines whether a solution is viable. To optimize FHE inference performance through compiler and library improvements, we need to answer three questions:

1. **Where is time spent?** FHE inference has distinct phases — key generation, encoding, homomorphic execution, decoding — with vastly different costs. Optimization must target the bottleneck phase.
2. **Where is memory consumed?** FHE ciphertext expansion is high; GPU/CPU memory directly limits the model size and batch size that can run. Knowing which phase is the memory peak enables informed capacity planning.
3. **Is an optimization effective?** Compiler or library improvements need quantitative comparison — before/after timing and memory changes are the only way to verify.

Without profiling data, these questions can only be guessed. Runtime profiling provides phase-level CPU/GPU timing and memory snapshots, giving optimization a factual basis.

## Design Decisions

### Decision 1: Profiling Framework — Based on torch.profiler

**Options considered:**

| Approach | Integration | Trace Export | C++ Annotation | GPU Support |
|----------|-------------|-------------|----------------|-------------|
| `torch.profiler` | Native (FHE runtime is a PyTorch C++ extension) | Chrome Trace built-in | `RECORD_USER_SCOPE` | `ProfilerActivity.CUDA` |
| Custom C++ profiler | Full control | Must implement trace format and visualization | Custom | Must implement |
| `cProfile` | Python standard lib | Limited | No C++ visibility | No |
| NVIDIA Nsight Systems (`nsys`) | External tool | Proprietary format | NVTX | Full GPU |

**Chosen: `torch.profiler`.** The FHE runtime is built as a PyTorch C++ extension, so `torch.profiler` integrates with zero friction — `RECORD_USER_SCOPE` in C++ is automatically captured, and Python API wraps it naturally. Chrome Trace export is built-in, enabling interactive visualization in Perfetto. For GPU users who need deeper analysis (kernel-level timeline, SM occupancy), `nsys` serves as a complementary external tool.

### Decision 2: FHE Memory Visibility

`torch.profiler`'s `profile_memory=True` only monitors PyTorch's own memory allocators. FHE runtime allocates memory through different paths that torch.profiler cannot observe:

| Layer | Allocation Method | Usage | Visible to torch.profiler |
|-------|-------------------|-------|---------------------------|
| **CPU** | | | |
| Application | PyTorch `THAllocator` | Wraps malloc with PyTorch tracking | Yes (KB-scale) |
| C runtime | `malloc` / `free` | Most common memory allocation interface | No |
| C runtime | `posix_memalign` / `aligned_alloc` | SIMD-aligned allocation | No |
| C runtime | jemalloc / tcmalloc etc. | Performance-optimized malloc replacements | No |
| System call | `mmap` / `munmap` | Large memory allocation, also used internally by malloc | No |
| **GPU** | | | |
| Application | PyTorch `CUDACachingAllocator` | Caching pool over cudaMalloc, reduces GPU allocation overhead | Yes |
| CUDA Runtime API | `cudaMalloc` / `cudaFree` | Synchronous GPU memory allocation | No |
| CUDA Runtime API | `cudaMallocAsync` / `cudaFreeAsync` | Stream-ordered GPU memory allocation | No |
| CUDA Runtime API | `cudaMallocManaged` | Unified memory, shared address space between CPU/GPU | No |
| CUDA Driver API | `cuMemAlloc` / `cuMemFree` | Driver-level synchronous allocation, underlying Runtime API | No |
| CUDA Driver API | `cuMemAllocManaged` | Driver-level unified memory allocation | No |
| CUDA Driver API | `cuMemCreate` / `cuMemMap` | Virtual memory management, fine-grained GPU physical memory control | No |

The `[memory]` events in traces are only PyTorch's own small allocations (KB-scale) — completely unrelated to FHE ciphertext memory (GB-scale). Adding `ProfilerActivity.CUDA` captures CUDA API calls as timing events, but does not aggregate them into a memory usage curve. We must sample memory explicitly.

**CPU memory tracking options:**

| Approach | Precision | Overhead | Complexity |
|----------|-----------|----------|------------|
| `/proc/self/status` VmRSS at phase boundaries | Per-phase | Negligible | Low |
| Hook `malloc`/`free` | Per-allocation | Low | High (requires allocator replacement or LD_PRELOAD) |
| `mallinfo()` / `malloc_info()` | Per-phase | Negligible | Low, but only covers glibc allocator |

**GPU memory tracking options:**

| Approach | Precision | Overhead | Complexity |
|----------|-----------|----------|------------|
| `cudaMemGetInfo` at phase boundaries | Per-phase (~11 samples/inference) | ~100ms total | Low |
| Hook CUDA memory allocation APIs | Per-allocation | Near-zero | High (modify low-level libraries) |
| `nsys --cuda-memory-usage` | Continuous | External tool | Zero code change, but requires nsys |
| PyTorch `CUDACachingAllocator` integration | Per-allocation | N/A | Not feasible — FHE libraries use their own allocators |

**Chosen: phase-boundary sampling.** CPU uses `/proc/self/status` VmRSS, GPU uses `cudaMemGetInfo()`. Both read system interfaces at FHE phase boundaries, providing sufficient granularity to identify which phase consumes memory, with minimal code change and negligible overhead. The `sample_memory()` function returns a `MemInfo` struct with a `gpu_available` flag; CPU-only builds skip GPU fields and populate RSS instead. Event names reflect the data source:

```
fhe::mem::<phase>::512MB_rss                            (CPU)
fhe::mem::<phase>::23307MB_gpu_used_74049MB_gpu_free   (GPU)
```

Users who need continuous memory curves can use `nsys` as a complementary tool.

### Decision 3: Dual Annotation (RECORD_USER_SCOPE + NVTX)

`torch.profiler` uses `RECORD_USER_SCOPE` annotations while NVIDIA Nsight Systems (`nsys`) uses NVTX annotations. They are incompatible — each tool only recognizes its own format. Using `RECORD_USER_SCOPE` alone means FHE phases are invisible in `nsys`, making it impossible for GPU users to correlate FHE phases with CUDA kernel execution.

**Options considered:**

| Option | torch.profiler | nsys | Risk |
|--------|---------------|------|------|
| RECORD_USER_SCOPE only | Yes | No | nsys users see nothing |
| NVTX only | No | Yes | torch.profiler users see nothing |
| Both (dual annotation) | Yes | Yes | None — different trace categories |

**Chosen: dual annotation on GPU builds.** Each tool sees its own annotation type (`user_annotation` vs `nvtx`), no duplication within any single tool. CPU builds use `RECORD_USER_SCOPE` only (NVTX unavailable).

### Decision 4: Python API — Context Manager vs Decorator

**Options considered:**

| Approach | Pros | Cons |
|----------|------|------|
| Context manager (`with fhe.profiler()`) | Explicit scope, flexible, matches `torch.profiler` idiom | Slightly more code |
| Decorator (`@fhe.profile`) | Concise | Implies profiling on every call — wrong semantics for a debugging tool |
| `CompiledProgram.profile()` method | One-liner, natural | Less flexible |

**Chosen: context manager as primary API + `CompiledProgram.profile()` as convenience method.** Profiling is a debugging activity, not a permanent feature — a context manager makes the profiling scope explicit. The convenience method covers the common case of quick one-shot profiling.

## Usage

### Context Manager: `fhe.profiler()`

```python
from ace import fhe

with fhe.profiler(device="cuda", trace_dir="./trace") as prof:
    result = program.run_dataset(images, labels)
print(prof.summary())
```

Parameters:

| Parameter        | Default | Description                                                        |
|------------------|---------|--------------------------------------------------------------------|
| `device`         | `"cpu"` | `"cuda"` adds `ProfilerActivity.CUDA`                              |
| `trace_dir`      | `None`  | Auto-export Chrome Trace on exit, with memory counter tracks added |
| `profile_memory` | `True`  | Enable PyTorch memory tracking                                     |
| `record_shapes`  | `False` | Record tensor shapes for each op                                   |
| `with_stack`     | `False` | Record call stacks                                                 |

### Convenience Method: `CompiledProgram.profile()`

```python
profile_result = program.profile(images, labels, device="cuda")
print(profile_result)
```

### Manual Trace Export

```python
with fhe.profiler(device="cuda") as prof:
    result = program(x)

prof.export_trace("trace.json")  # auto-adds Perfetto memory counter tracks
```

### Output Example

```
--- FHE Events (sorted by CPU time) ---
Event                                                    CPU total (ms)    Calls    CPU avg (ms)
-----------------------------------------------------------------------------------------------
fhe::init                                                     28585.8        1         28585.8
fhe::run                                                      27390.1        3          9130.0
fhe::finalize                                                     0.1        1             0.1

--- Memory Snapshots ---
Phase                                      GPU Used (MB)   GPU Free (MB)
------------------------------------------------------------------------
execute_after                                     24557           72799
execute_before                                    23339           74017
init_after                                        23307           74049
init_before                                         339           97017
```

### `ProfileResult` Dataclass

```python
@dataclass
class FHEEvent:
    name: str
    cpu_time_total_ms: float
    cpu_time_avg_ms: float
    count: int

@dataclass
class MemSnapshot:
    phase: str
    gpu_used_mb: int
    gpu_free_mb: int

@dataclass
class ProfileResult:
    fhe_events: List[FHEEvent]
    memory_snapshots: List[MemSnapshot]
    dataset_result: object       # from run_dataset, if used
    trace_path: Optional[str]    # set if trace_dir was specified

    def summary(self) -> str     # formatted table
    def export_trace(self, path) # Chrome Trace + memory counter tracks
```

## Trace Hierarchy

FHE inference produces the following nested annotation hierarchy in profiler traces:

```
fhe::inference               (Python FHERuntime)
  fhe::run_batch_sequential  (C++ BATCH_RUNNER)
    fhe::run                 (C++ KERNEL_RUNNER, per image)
      fhe::prepare_input
        fhe::mem::prepare_input_before::...MB_gpu_used_...MB_gpu_free
        fhe::mem::prepare_input_after::...MB_gpu_used_...MB_gpu_free
      fhe::execute
        fhe::mem::execute_before::...MB_gpu_used_...MB_gpu_free
        fhe::mem::execute_after::...MB_gpu_used_...MB_gpu_free
      fhe::get_output
        fhe::mem::get_output_before_free::...
        fhe::mem::get_output_after_free::...
```

## C++ Implementation

All C++ instrumentation lives in `fhe_dsl/csrc/runtime/include/ace/runtime/fhe_profiling.h`.

### Phase Annotations: `FHE_PROFILE_SCOPE`

On GPU builds, emits **both** `RECORD_USER_SCOPE` (for `torch.profiler`) and NVTX range push/pop (for `nsys`). On CPU builds, emits `RECORD_USER_SCOPE` only.

```cpp
// GPU build (USE_CUDA):
#define FHE_PROFILE_SCOPE(name)      \
    RECORD_USER_SCOPE(name);         \
    NvtxRangeGuard _guard(name)      // owns std::string, nvtxRangePushA/Pop

// CPU build:
#define FHE_PROFILE_SCOPE(name)      \
    RECORD_USER_SCOPE(name)
```

`NvtxRangeGuard` owns a `std::string name_` member because `nvtxRangePushA` only stores a pointer — it does not copy the string. This is critical for `FHE_PROFILE_SCOPE_WITH_MEM`, which constructs dynamic names like `"fhe::execute [23307MB gpu]"`.

Annotated phases:

```
fhe::init, fhe::run, fhe::prepare_input, fhe::execute,
fhe::get_output, fhe::finalize, fhe::run_batch_sequential,
fhe::run_batch_parallel, fhe::inference, fhe::run_batch
```

### Memory Sampling

`sample_memory()` reads memory at each phase boundary:

- **GPU** (`USE_CUDA`): `cudaMemGetInfo()` → `used = total - free`
- **CPU**: `/proc/self/status` VmRSS

`record_mem_snapshot()` emits an instant marker (`nvtxMarkA` on GPU, `RECORD_USER_SCOPE` on CPU):

```
fhe::mem::<phase>::<used>MB_gpu_used_<free>MB_gpu_free   (GPU)
fhe::mem::<phase>::<used>MB_rss                            (CPU)
```

`FHE_PROFILE_SCOPE_WITH_MEM` appends memory to the phase name:

```
fhe::execute [23307MB gpu]    (GPU)
fhe::execute [512MB rss]      (CPU)
```

### Sampling Points

| Sampling Point                    | Location                      | Meaning                                      |
|-----------------------------------|-------------------------------|----------------------------------------------|
| `init_before` / `init_after`      | `KERNEL_RUNNER::Init()`       | Context initialization / key generation      |
| `run_start`                       | `KERNEL_RUNNER::Run()` entry  | Baseline before inference                    |
| `run_before_execute`              | Before `_run_kernel()`        | Memory after input encoding                  |
| `run_after_execute`               | After `_run_kernel()`         | Memory after computation (peak)             |
| `run_end`                         | `Run()` return                | Memory after inference                       |
| `prepare_input_before` / `after`  | `Prepare_input_only()`        | Input encoding phase                         |
| `execute_before` / `after`        | `Execute()`                   | Core computation phase                       |
| `get_output_before_free` / `after_free` | `Get_output()`          | Output decode and tensor free               |
| `finalize_before` / `after`       | `Finalize()`                  | Context teardown memory reclamation         |

### Conditional Compilation

All CUDA/NVTX code is guarded by `#ifdef USE_CUDA`. CPU-only builds have zero overhead.

```cmake
if(CMAKE_CUDA_COMPILER OR CUDAToolkit_FOUND)
    target_compile_definitions(runtime PRIVATE USE_CUDA)
    target_include_directories(runtime PRIVATE ${CUDAToolkit_INCLUDE_DIRS})
    target_link_libraries(runtime PRIVATE CUDA::cudart CUDA::nvToolsExt)
endif()
```

### Performance Impact

- `cudaMemGetInfo()`: ~9–11ms per call, ~100ms total per inference (negligible vs. 9s inference time)
- NVTX push/pop: ~1μs per scope
- `RECORD_USER_SCOPE`: ~1μs per scope

## External Tool: NVIDIA Nsight Systems

`nsys` is a complementary external tool (not integrated into ACE). It provides capabilities that `torch.profiler` cannot:

- Continuous GPU memory curves (vs. per-phase sampling)
- GPU kernel execution timeline
- SM occupancy and utilization metrics
- CUDA API timing

The NVTX annotations in `FHE_PROFILE_SCOPE` make FHE phases visible in `nsys` traces:

```bash
nsys profile -t cuda,nvtx,osrt \
            --gpu-metrics-device=all \
            --cuda-memory-usage=true \
            -o fhe_inference \
            python 01_quick_profile.py --library phantom --device cuda

# Export to Chrome Trace format
nsys export -t chrome -o fhe_inference.json fhe_inference.nsys-rep
```

## File Index

### Python (Profiling API)

| File | Description |
|------|-------------|
| `fhe_dsl/python/fhe/profiler.py` | `FHEProfiler`, `ProfileResult`, `FHEEvent`, `MemSnapshot`, `fhe.profiler()` |
| `fhe_dsl/python/fhe/__init__.py` | Exports: `profiler`, `FHEProfiler`, `ProfileResult` |
| `fhe_dsl/python/fhe/runtime/program.py` | `CompiledProgram.profile()` convenience method |

### C++ (Runtime Instrumentation)

| File | Description |
|------|-------------|
| `fhe_dsl/csrc/runtime/include/ace/runtime/fhe_profiling.h` | `FHE_PROFILE_SCOPE`, `FHE_PROFILE_SCOPE_WITH_MEM`, `record_mem_snapshot`, `sample_memory` |
| `fhe_dsl/csrc/runtime/src/kernel_runner.cxx` | Sampling points + `FHE_PROFILE_SCOPE_WITH_MEM` on all FHE phases |
| `fhe_dsl/csrc/runtime/src/batch_runner.cxx` | `FHE_PROFILE_SCOPE` on batch phases (`fhe::run_batch_sequential`, `fhe::run_batch_parallel`) |
| `fhe_dsl/csrc/runtime/CMakeLists.txt` | `USE_CUDA` conditional, `CUDA::cudart` + `CUDA::nvToolsExt` linking |

### Examples

| File | Description |
|------|-------------|
| `examples/07_profiling/01_quick_profile.py` | Small LinearModel example showing all 3 profiling patterns |
| `examples/07_profiling/02_resnet20_profile.py` | Full ResNet-20 profiling with dataset accuracy |

## Future Work

| Direction | Description | Complexity |
|-----------|-------------|------------|
| CPU RSS tracking | Read `/proc/self/status` VmRSS in `sample_memory()` for CPU environments | Low |
| `cuda_auto_ptr` counters | Add atomic counters to phantom's `make_cuda_auto_ptr`/`reset()` for per-allocation tracking | Medium |
| `cudaMallocAsync` event aggregation | Parse `cudaMallocAsync`/`cudaFreeAsync` events from trace to reconstruct allocation timeline | Medium |
| Automatic trace analysis | Script to parse trace and identify top memory consumers per FHE phase | Medium |