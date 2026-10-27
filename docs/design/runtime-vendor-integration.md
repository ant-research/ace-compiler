# ACE Runtime — Vendor Backend Integration Guide

> Status: living document · reflects the codebase as of mid-July 2026
> Scope: how to wire a **new compute backend** (GPU or CPU) into the ACE Runtime, layer by layer
> Companion: [runtime-architecture.md](runtime-architecture.md) — the four-layer "what and why"

This is the **which-file, which-symbol** document. It assumes you have read the architecture overview
and know where a backend sits (L4, consumed by L2's `FheContext<Backend>` template).

---

## 1. Where a Backend Lives in the Four Layers

A single backend touches **three** of the four layers (L1, L2/L4, and the compiler). The pybind
runtime extension is **backend-agnostic** and needs no change.

| Layer | What the backend adds | Key mechanism |
|-------|-----------------------|---------------|
| **L1 — Python** | a `backend/<name>.py` class + registry entry | `register_library(name, device, Cls)` |
| **L2/L4 — C++ library** (`fhe_lib/`) | a `VendorTraits` struct + `FheContext<Backend>` instantiation + one `api.cu` | templates, compile-time concept check |
| **Compiler codegen** (`compiler/fhe-cmplr/`) | one `DEFINE_PROVIDER(...)` line | `ALL_PROVIDERS()` macro |
| **CMake** | `WITH_<NAME>` option + `add_subdirectory` | top-level + `fhe_lib/CMakeLists.txt` |
| **pybind `_C` extension** | **nothing** | dlsym contract is uniform (Section 4) |
| **rtlib** | only for **CPU/legacy** backends | `rt_<name>.h` inline dispatch (Section 6) |

### Two parallel systems (important)

```
fhe_lib/  (new, active)          compiler/fhe-cmplr/rtlib/  (legacy baseline)
─────────────────────────        ──────────────────────────────────────
phantom, ace  (GPU)              ant (CPU default), seal/openfhe (stubbed),
VendorTraits template            phantom/ace (old path, superseded)
FheContext<Backend>              rt_<name>.h → Vendor_* C functions
extern "C" dlsym symbols         DEFINE_PROVIDER enum
```

- A **new GPU backend** integrates into `fhe_lib/`. Do not touch rtlib.
- A **new CPU backend** still needs the rtlib path until CPU support lands in `fhe_lib/`
  (currently the `seal`/`openfhe` entries in `fhe_lib/CMakeLists.txt` are commented out).

---

## 2. Core Mechanism: the `VendorTraits` Template

There is **no factory** and **no virtual interface**. Dispatch is resolved at compile time via C++
templates. A backend is fully described by one struct that satisfies a concept.

### 2.1 The contract

Each backend `<name>` provides, in `fhe_lib/include/fhe/vendor/<name>/vendor_traits.h`:

```cpp
namespace <name> {
struct VendorTraits {
    // — Type aliases (24, all checked by concept.h) —
    using SecretKey   = ...;   using PublicKey  = ...;
    using RelinKey    = ...;   using GaloisKey  = ...;
    using Context     = ...;   using Plaintext  = ...;   using Ciphertext = ...;
    using ScaleType   = ...;   using LevelType  = ...;
    using Evaluator   = ...;   using Crypto     = ...;   using BtsMgr     = ...;
    using SecretKeyAccess  = ...;  using PublicKeyAccess  = ...;
    using RelinKeyAccess   = ...;  using GaloisKeyAccess   = ...;
    using SecretKeyPopulate= ...;  using PublicKeyPopulate= ...;
    using RelinKeyPopulate = ...;  using GaloisKeyPopulate = ...;
    using PtAccess         = ...;  using PtPopulate        = ...;
    using HostCiphertext   = ...;  using HostPlaintext     = ...;

    // — Static methods (~30): key access/populate, HE ops, context creation —
    static SecretKey&  access_secret_key(...);
    static void        populate_secret_key(...);
    static size_t      poly_degree(...);
    static size_t      coeff_mod_size(...);
    static void        Add(...);   static void Mul(...);   static void Rotate(...);
    static void        Relinearize(...);  static void Rescale_next(...);
    static Context*    create_context(...);
    static void        generate_keys(...);
    static void        create_encoder(...);  static void create_evaluator(...);
    static void        init_bootstrap(...);  static void upload_keys_to_evaluator(...);
    // …
};
}  // namespace <name>
```

…validated at compile time:

```cpp
static_assert(is_valid_vendor_traits_v<<name>::VendorTraits>,
              "<name> backend is missing required VendorTraits members");
```

`FheContext<Backend>` is then instantiated with this struct; all HE operations route through
`FheContext<Backend>::Current().<Op>()`.

### 2.2 The codegen dispatch header

`fhe_lib/include/fhe/vendor/<name>/rt_<name>.h` defines inline functions the compiler-generated `.cu`
calls — `Add_ciph()`, `Mul_ciph()`, `Encode_float()`, `Rotate_ciph()`, `Rescale_ciph()`,
`Bootstrap()`, etc. — each one line forwarding to `FheContext<VendorTraits>::Current()`. It also
defines the global typedefs `CIPHERTEXT`/`CIPHER`/`PLAINTEXT`/`PLAIN` used by codegen.

A thin forwarding shim `fhe_lib/include/rt_<name>/rt_<name>.h` (one line) re-includes
`fhe/vendor/<name>/rt_<name>.h` so the installed include tree resolves the
`#include "rt_<name>/rt_<name>.h"` the compiler emits.

### 2.3 The lifecycle + KMS glue (one `api.cu`)

Each backend's `api.cu` is ~two stanzas:

```cpp
// 1. Emit the 5 lifecycle extern "C" symbols (dlsym'd by the runtime)
FHERT_DEFINE_LIFECYCLE_API(<name>::VendorTraits);

// 2. Six thin C wrappers around the templated KMS API
extern "C" int    Kms_save_keys(const char* dir, const uint32_t* elts, size_t n)
{ return fhert::kms::SaveKeys<<name>::VendorTraits>(dir, elts, n); }
extern "C" int    Kms_load_keys(const char* dir)
{ return fhert::kms::LoadKeys<<name>::VendorTraits>(dir); }
// … SaveEvalKeys / LoadEvalKeys / KeysLoadedFromKms / GetGaloisLoadTimeSeconds
```

The lifecycle macro and the KMS templates live in **shared, backend-agnostic headers**
(`fhe/vendor/lifecycle_api.h`, `fhe/kms/kms_api.h`) — this is the recent "dedup backend C API"
refactor. A new backend gets all 11 symbols from two stanzas, not 11 hand-written functions.

---

## 3. Layer-by-Layer Integration Checklist

### 3.1 Python (L1) — 2 files

| File | Action |
|------|--------|
| `fhe_dsl/python/fhe/backend/<name>.py` | New file. `class <Name>LIB(GpuBackend)` (GPU) or `(CpuBackend)` (CPU). Implement: `backend_name() → "<name>"`, `device_name() → "cuda"/"cpu"`, `supported_format_types()`, `compile_to_lib()` (must emit `-P2C:lib=<name>` to `fhe_cmplr`), `build_command()` (link `-lFHErt_<name>`; add any `-D` defines). |
| `fhe_dsl/python/fhe/backend/__init__.py` | Import the class and add `register_library("<name>", "<device>", <Name>LIB)` inside `_register_default_backends()`. |

**No changes** to `driver/registry.py`, `driver/builder.py`, `driver/driver.py`, or
`runtime/runtime.py` — the registry, builder, and runtime are backend-agnostic. The runtime never
needs a backend name at inference time: the compiled `.so` determines the backend through its
`libFHErt_<name>.so` NEEDED dependency.

### 3.2 C++ backend library (L2/L4, `fhe_lib/`) — ~12 new files

| File | Purpose |
|------|---------|
| `fhe_lib/include/fhe/vendor/<name>/vendor_traits.h` | `VendorTraits` struct (24 aliases + ~30 static methods + CIPHER/PLAIN typedefs) |
| `fhe_lib/include/fhe/vendor/<name>/rt_<name>.h` | inline dispatch → `FheContext<VendorTraits>::Current()` |
| `fhe_lib/include/fhe/vendor/<name>/bts_mgr.h` | bootstrap manager (if bootstrap is supported) |
| `fhe_lib/include/rt_<name>/rt_<name>.h` | 1-line forwarding shim to the above |
| `fhe_lib/<name>/CMakeLists.txt` | `add_library(FHErt_<name> SHARED src/api.cu src/context.cu src/vendor_traits.cu src/kms/*.cu src/data/*.cu)`; link `FHErt_common_shared FHErt_io`; set `INSTALL_RPATH "$ORIGIN"`; install to `ace/lib/` |
| `fhe_lib/<name>/src/api.cu` | `FHERT_DEFINE_LIFECYCLE_API(<name>::VendorTraits)` + 6 `Kms_*` wrappers |
| `fhe_lib/<name>/src/{context,vendor_traits}.cu` | explicit template instantiations / TUs |
| `fhe_lib/<name>/src/kms/{key_mgr,key_serializer}.cu` | backend key lifecycle + serialization |
| `fhe_lib/<name>/src/data/{enc_mgr,enc_mgr_api}.cu` | weight encoding |

`fhe_lib/common/` (shared, already built) provides everything cross-backend: `IoTransport`/`GdsTransport`/
`IoBulkReader`/`IoPolicy`, `key_index`/`key_io`/`key_protocol`, `enc_mgr`/`enc_pipeline`/`enc_io`,
`context.h`/`session.h`/`fhe_config.h`, and the shared `concept.h`/`types.h`/`lifecycle_api.h`. Link
`FHErt_io` (and `FHErt_common_shared`) — do not reimplement IO or serialization.

### 3.3 Compiler codegen (1 file)

| File | Action |
|------|--------|
| `compiler/fhe-cmplr/include/fhe/core/lib_provider.h` | Add one line to the `ALL_PROVIDERS()` macro: `DEFINE_PROVIDER(<NAME>, "<name>", "LIB_<NAME>")`. The macro auto-generates: the `PROVIDER::<NAME>` enum value, the `"<name>"` string→id map, the library name, and the codegen header `"rt_<name>/rt_<name>.h"`. |

If the backend needs bespoke codegen, add `if (this->Provider() == core::PROVIDER::<NAME>)` branches
in `compiler/fhe-cmplr/include/fhe/poly/ir2c_ctx.h` (e.g. ACE skips `LEVEL_T`/`SCALE_T` typedefs;
ANT emits `memset` + `Alloc_lpoly_data`). Otherwise nothing else.

### 3.4 CMake (2 files)

| File | Action |
|------|--------|
| Top-level `CMakeLists.txt` | `option(WITH_<NAME> "Build <Name> backend" OFF)` |
| `fhe_lib/CMakeLists.txt` | Under `if(ACE_CUDA_AVAILABLE)`: `if(WITH_<NAME>) add_subdirectory(<name>) endif()` |

### 3.5 pybind `_C` extension — **no change**

The extension (`fhe_dsl/csrc/runtime/`) dlopen's the kernel `.so` and resolves 11 symbols by name
(Section 4). It is fully backend-agnostic; the only contract is that the backend exports those
symbols. This is the cleanest seam in the design — do not add backend-specific bindings here.

### 3.6 rtlib — **only for CPU/legacy**

If and only if the backend is a CPU backend going through the legacy path
(`compiler/fhe-cmplr/rtlib/`), also add:

| File | Purpose |
|------|---------|
| `rtlib/include/rt_<name>/rt_<name>.h` | inline dispatch to `<Vendor>_*` C functions (PascalCase ops) |
| `rtlib/include/rt_<name>/rt_def.h` | `CIPHERTEXT`/`CIPHER`/`PLAINTEXT`/`PLAIN`/`SCALE_T`/`LEVEL_T` typedefs |
| `rtlib/include/rt_<name>/<name>_api.h` | declarations of `<Vendor>_*` C functions |
| `rtlib/<name>/CMakeLists.txt` + `src/<name>_lib.cxx` | `add_subdirectory` (gated by `RTLIB_ENABLE_<NAME>`) + implementations |
| `rtlib/CMakeLists.txt` | `option(RTLIB_ENABLE_<NAME> …)` + `add_subdirectory(<name>)` |

(GPU backends skip this entirely — `fhe_lib/` supersedes the rtlib phantom/ace path.)

> Heads-up: `fhe_lib/include/rt_<name>/rt_<name>.h` and
> `compiler/fhe-cmplr/rtlib/include/rt_<name>/rt_<name>.h` are **different files with the same include
> guard**. The build's include-path order decides which one the compiler picks up. The Python
> `build_command()` include flags place the `fhe_lib/` version first for GPU backends. Be careful
> when adding a backend that exists in both trees.

---

## 4. The dlsym / C-API Contract (11 symbols)

This is the **single contract** between the pybind `_C` extension and any backend. The runtime
(`fhe_dsl/csrc/runtime/src/kernel_runner.cxx`) resolves exactly these 11 `extern "C"` symbols from
the loaded kernel `.so` (which links the backend `.so` as NEEDED, so the symbols resolve there):

### 4.1 Lifecycle (5) — emitted by `FHERT_DEFINE_LIFECYCLE_API`

| Symbol | Role |
|--------|------|
| `Prepare_context()` | create + make-current the `FheContext` (keys, weights) |
| `Finalize_context()` | `cudaDeviceSynchronize()` then destroy context |
| `Prepare_input(inputs)` | encode the user's input tensors |
| `Handle_output()` | decode the output ciphertext |
| `Main_graph()` (weak) | the compiled HE graph |

### 4.2 KMS (6) — wrappers around `kms_api.h` templates

| Symbol | Role |
|--------|------|
| `Kms_save_keys(dir, elts, n)` | save all keys (SK+PK+RLK+GK) |
| `Kms_load_keys(dir)` | load all keys |
| `Kms_save_eval_keys(dir, elts, n)` | save eval keys only (RLK+GK) |
| `Kms_load_eval_keys(dir)` | load eval keys only |
| `Kms_keys_loaded_from_kms()` | `1` if loaded from KMS, `0` if generated |
| `Kms_get_galois_load_time_seconds()` | wall-clock seconds of last galois load (`-1.0` if none) |

> The kernel `.so` is loaded with `RTLD_LOCAL | RTLD_NODELETE` to stop `_cst_N` constant-symbol
> pollution across kernels. All 11 symbols are `extern "C"` with C linkage — no name mangling.

---

## 5. GPU Backend — Full File Template

Concrete file set for a new GPU backend `newgpu` (namespaced `newgpu`):

```
fhe_lib/
  include/fhe/vendor/newgpu/
    vendor_traits.h     # struct newgpu::VendorTraits  (+ static_assert concept)
    rt_newgpu.h         # inline ops → FheContext<newgpu::VendorTraits>::Current()
    bts_mgr.h           # optional
  include/rt_newgpu/
    rt_newgpu.h         # one line: #include "fhe/vendor/newgpu/rt_newgpu.h"
  newgpu/
    CMakeLists.txt
    src/api.cu          # FHERT_DEFINE_LIFECYCLE_API(newgpu::VendorTraits) + 6 Kms_* wrappers
    src/context.cu
    src/vendor_traits.cu
    src/kms/key_mgr.cu
    src/kms/key_serializer.cu
    src/data/enc_mgr.cu
    src/data/enc_mgr_api.cu
```

`fhe_lib/newgpu/CMakeLists.txt` sketch:

```cmake
add_library(FHErt_newgpu SHARED
    src/api.cu src/context.cu src/vendor_traits.cu
    src/kms/key_mgr.cu src/kms/key_serializer.cu
    src/data/enc_mgr.cu src/data/enc_mgr_api.cu)
target_link_libraries(FHErt_newgpu PUBLIC FHErt_common_shared FHErt_io)
set_target_properties(FHErt_newgpu PROPERTIES INSTALL_RPATH "$ORIGIN")
install(TARGETS FHErt_newgpu LIBRARY DESTINATION ace/lib)
```

`fhe_dsl/python/fhe/backend/newgpu.py` sketch:

```python
from .base import GpuBackend

class NewGPULIB(GpuBackend):
    @classmethod
    def backend_name(cls) -> str: return "newgpu"
    @classmethod
    def device_name(cls) -> str:  return "cuda"
    def compile_to_lib(self, ir, output_dir): ...   # must emit -P2C:lib=newgpu
    def build_command(self, ...): ...               # link -lFHErt_newgpu, add -D defines
```

Compiler one-liner in `lib_provider.h`:

```c
#define ALL_PROVIDERS()                              \
  DEFINE_PROVIDER(ANT, "ant", "LIB_ANT")             \
  DEFINE_PROVIDER(SEAL, "seal", "LIB_SEAL")          \
  DEFINE_PROVIDER(OPENFHE, "openfhe", "LIB_OPENFHE") \
  DEFINE_PROVIDER(PHANTOM, "phantom", "LIB_PHANTOM") \
  DEFINE_PROVIDER(ACE, "ace", "LIB_ACE")             \
  DEFINE_PROVIDER(NEWGPU, "newgpu", "LIB_NEWGPU")    /* ← add this line */
```

---

## 6. Backend Comparison Table

| Backend | Device | System | Python class | C++ lib | Implemented? |
|---------|--------|--------|--------------|---------|--------------|
| `phantom` | GPU | `fhe_lib/` | `PhantomLIB` | `libFHErt_phantom.so` | ✅ |
| `ace` / `acelib` | GPU | `fhe_lib/` | `AcelibLIB` | `libFHErt_ace.so` | ✅ |
| `ant` / `antlib` | CPU | rtlib | `AntLIB` | `libFHErt_ant.so` | ✅ (CPU default) |
| `seal` | CPU | rtlib | `SealLIB` | (stub) | ❌ stub (`implemented=False`) |
| `openfhe` | CPU | rtlib | `OpenFHELIB` | (stub) | ❌ stub (`implemented=False`) |

### rtlib dispatch mechanism (legacy, CPU only)

Each `rtlib/include/rt_<name>/rt_<name>.h` defines the same uniform inline PascalCase API
(`Degree()`, `Get_input_data()`, `Encode_float()`, `Add_ciph()`, `Mul_ciph()`, `Rotate_ciph()`,
`Rescale_ciph()`, `Relin()`, `Bootstrap()`, `Copy_ciph()`, `Free_cipher()`, `Sc_degree()`,
`Level()`, …) that forwards to vendor-prefixed C functions (`Ant_*`, `Seal_*`, `Openfhe_*`,
`Phantom_*`, `AceLibrary_*`). The compiler codegen emits calls to these inline functions; the
per-backend header is selected via the `Provider_header(id)` expansion of `ALL_PROVIDERS()`.

---

## 7. Verification — Compile + Inference Smoke Test

After wiring a backend, verify end to end:

1. **Build with the backend enabled.**
   ```bash
   cmake -DWITH_<NAME>=ON ... && make
   ```
   Confirm `ace/lib/libFHErt_<name>.so` exists and that `ldd` on a compiled kernel `.so` shows it
   under NEEDED.

2. **Confirm the 11 dlsym symbols are exported.**
   ```bash
   nm -D --defined-only ace/lib/libFHErt_<name>.so | grep -E 'Prepare_context|Finalize_context|Prepare_input|Handle_output|Main_graph|Kms_'
   ```
   Expect all 11 (with `T` for text). Missing symbols → the `api.cu` stanzas are wrong.

3. **Compile a small model against the backend.**
   In Python, use `Driver(library="<name>")` (or set the backend in the package) and run the
   normal compile flow; check the generated `.cu` includes `"rt_<name>/rt_<name>.h"` and that
   `fhe_cmplr` received `-P2C:lib=<name>`.

4. **Run one ONLINE inference.**
   ```python
   runner = FHERuntime({"model": "Add", "kernel": "<kernel>.so", "key_mode": "online"})
   runner.init(); runner.inference(x)
   ```
   `init()` exercises `Prepare_context` + key generation; `inference()` exercises
   `Prepare_input`/`Main_graph`/`Handle_output`.

5. **Exercise the KMS path.**
   Save then reload keys to hit `Kms_save_keys`/`Kms_load_keys`, and check
   `Kms_keys_loaded_from_kms()` flips to `1`.

6. **concept check at compile time.**
   If `VendorTraits` is incomplete, `static_assert(is_valid_vendor_traits_v<...>)` fails at build
   of `FHErt_<name>.so` with a precise message — fix the missing alias/method and rebuild.

---

## 8. Recent Refactors That Shaped This Guide

| Commit | What it changed (relevant to backend wiring) |
|--------|-----------------------------------------------|
| `5fdfdf5` dedup backend C API | KMS templates lifted to `fhe/kms/kms_api.h`; lifecycle macro → `fhe/vendor/lifecycle_api.h`; dead `Phantom_*`/`FHert_*` shims dropped; `VendorTraits` renamed (no vendor prefix) |
| `674fa66` migrate bts_mgr to ace RNS API | `fhe_lib/ace/src/bts_mgr.cu` rewritten on ace-library's native RNS chain API |
| `fef1242` default EncPolicy ON_DEMAND | avoids init-time OOM on large models |
| `537de72` restore `_sync_gpu` before teardown | `Finalize_context` always `cudaDeviceSynchronize()`s, across all backends |
| `5330bd3` dlopen with `RTLD_LOCAL` | `kernel_runner.cxx` uses `RTLD_LOCAL \| RTLD_NODELETE` to stop `_cst_N` pollution |
| header-layout restructure (`11a3822` etc.) | `runtime/` → top-level, `data/` → `enc/`, stale `lib/` shim dir removed |

If you are extending an older guide that spoke of "many `Phantom_*`/`AceLibrary_*` C functions called
via ctypes", that model is **superseded** — the template + macro mechanism above is the current truth.