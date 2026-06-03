# Runtime API

## CompiledProgram

Returned by `fhe.compile(...).compile(inputs)`. The primary interface for FHE inference.

```python
program = compiled.compile([example_x, example_y])

# High-level: call like a function
result = program(x, y)

# Validate against plaintext
program.validate()

# Run on a dataset
result = program.run_dataset(images, labels, top_k=1)
```

### Methods

| Method | Description |
|--------|-------------|
| `__call__(*args)` | Run FHE inference (lazy-creates runtime) |
| `validate()` | Validate FHE result vs plaintext using compile-time example inputs |
| `runtime()` | Get/create the underlying `FHERuntime` |
| `run_dataset(images, labels, ...)` | Batch inference on labeled dataset |
| `profile(images, labels, ...)` | Profile FHE execution with torch.profiler |

---

## FHERuntime

Low-level runtime for FHE inference. Only one instance should be active at a time.

```python
from ace.fhe.runtime.runtime import FHERuntime

rt = FHERuntime({"model": "add", "kernel": "kernel.so"})
rt.init()
result = rt.inference(x, y)
rt.finalize()
```

### Methods

| Method | Description |
|--------|-------------|
| `init()` | Initialize FHE context (key generation). Auto-called by `inference()` |
| `finalize()` | Release FHE context resources |
| `inference(*inputs)` | Single FHE inference. Inputs auto-converted to 4D |
| `run_batch(batch_inputs, parallel, num_threads, verbose)` | Batch inference |
| `run_dataset(images, labels, top_k, ...)` | Dataset inference with accuracy metrics |
| `validate(result, expected)` | Compare FHE result against expected tensor |

### `run_dataset` Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `images` | `Tensor` | required | Shape `(N, C, H, W)` |
| `labels` | `List[int]` | required | Ground truth class labels |
| `top_k` | `int` | `1` | Top-K accuracy computation |
| `parallel` | `bool` | `False` | Use OpenMP parallelism |
| `num_threads` | `int` | `0` | Thread count (0 = auto) |
| `verbose` | `bool` | `True` | Print per-image progress |

---

## KernelExecutor

Manages a single FHE kernel (.so). Wraps the C++ `ProviderManager`.

```python
from ace.fhe.runtime.executor import KernelExecutor

executor = KernelExecutor("model", "kernel.so")
executor.init()
result = executor.execute(x, y)
executor.finalize()
```

### Constructor

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `name` | `str` | required | Kernel identifier |
| `lib_path` | `str` | required | Path to compiled FHE .so |
| `use_cuda_graph` | `bool` | `False` | Enable CUDA Graph for Execute() replay |

### Methods

| Method | Description |
|--------|-------------|
| `init()` | Initialize FHE context. Auto-called by `execute()` |
| `finalize()` | Release context. Must re-init before next `execute()` |
| `execute(*inputs)` | Single FHE inference |
| `execute_batch(batch_inputs, parallel, num_threads, verbose)` | Batch inference |
| `validate(result, expected)` | Compare FHE vs expected tensor |
| `capture_graph()` | Attempt CUDA Graph capture (see below) |

### CUDA Graph

CUDA Graph reduces kernel launch overhead by capturing and replaying the GPU execution graph. Only works with the phase-split API (`Prepare_input_only -> Execute -> Get_output`).

```python
executor = KernelExecutor("model", "kernel.so", use_cuda_graph=True)
executor.init()
success = executor.capture_graph()  # Must call after init()
# Subsequent execute_batch calls replay the captured graph
```

**WARNING:** Not compatible with Phantom backend. Phantom's `Main_graph()` calls `cudaMallocAsync` internally, which is illegal during stream capture. Only use with ACE's own kernels that pre-allocate all GPU memory.

---

## Result Types

### BatchResult

```python
@dataclass
class BatchResult:
    outputs: List[torch.Tensor]  # One tensor per input
    timing: BatchTiming
    num_success: int
    num_failure: int
```

### BatchTiming

```python
@dataclass
class BatchTiming:
    total_ms: float
    avg_per_image_ms: float
    min_image_ms: float
    max_image_ms: float
    num_images: int
```

### DatasetResult

```python
@dataclass
class DatasetResult:
    predictions: List[int]
    labels: List[int]
    top1_accuracy: float
    top5_accuracy: Optional[float]
    timing: BatchTiming
    num_correct_top1: int
    num_correct_top5: int
    total: int
```