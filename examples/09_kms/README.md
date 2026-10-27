# KMS -- Key Management for FHE

FHE key generation is expensive (seconds to minutes depending on CKKS
parameters). The KMS workflow lets you **pre-generate keys once**, save them
to disk, and **load them in subsequent runs** -- skipping key generation entirely.

## Examples

| Script | Description |
|--------|-------------|
| `01_save_keys.py` | Generate and save keys for a simple add model |
| `02_load_keys.py` | Load pre-generated keys, run inference without key gen |
| `03_resnet_save_keys.py` | Generate and save ResNet keys (`--model resnet20\|resnet32\|resnet44\|resnet56\|resnet110`) |
| `04_resnet_load_keys.py` | Load pre-generated ResNet keys |
| `05_inference_modes.py` | Selective key loading — `online` / `key_offline` / `server_only` |

## Quick Start

```bash
# Simple add model
python3 01_save_keys.py
python3 02_load_keys.py ./kms_demo_keys

# ResNet-20 (default)
python3 03_resnet_save_keys.py
python3 04_resnet_load_keys.py ./kms_demo_keys_resnet20

# ResNet-110
python3 03_resnet_save_keys.py --model resnet110
python3 04_resnet_load_keys.py ./kms_demo_keys_resnet110
```

## I/O Transport Options (`--io`)

All KMS scripts support the `--io` flag:

| Mode     | Description |
|----------|-------------|
| `auto`   | **Default.** Use GDS (cuFile) if available, otherwise host-staged I/O |
| `gds`    | Force GDS mode. Fails if cuFile/GDS is not available |
| `staged` | Force host-staged I/O. Never attempts GDS, uses 4 MB chunked streaming |
| `host_mapped` | Host Mapped I/O. mmap + cudaMemcpy, one less copy than staged |

### Python API

```python
from ace.fhe.config import IoConfig, DataPath
from ace import runtime as _C

_C.gds_available()                                  # Check GDS availability
IoConfig(data_path=DataPath.HOST_STAGED).apply()    # Force staged I/O
IoConfig(data_path=DataPath.GDS).apply()            # Force GDS
IoConfig(data_path=DataPath.HOST_MAPPED).apply()    # Host mapped I/O
IoConfig(data_path=DataPath.AUTO).apply()           # Auto-detect (default)
```

## Key Directory Layout

```
KEY_DIR/
├── meta.json              ← metadata for the load step
├── global/
│   ├── secret_key.bin     ← secret key
│   └── public_key.bin     ← public key
└── model/
    ├── relin_key.bin      ← relinearization key
    └── galois_key.bin     ← Galois (rotation) key
```

## Related

- [Offline Weight Encoding](../06_advanced/08_offline_encode.py) — Pre-encode weights (DE_MSG_F32 → DE_PLAINTEXT)
- [GDS I/O Transport Design](../../docs/design/gds-io-transport.md) — Architecture, transport selection, cuFile loading
- [ResNet Offline Loading Benchmark](../../docs/benchmarks/resnet_offline_benchmark.py) — Benchmark script (staged vs GDS)
- [ResNet Offline Loading Results](../../docs/benchmarks/resnet-offline-loading.md) — Benchmark data, GDS vs staged comparison

## Requirements

- CUDA-capable GPU
- `phantom` backend installed (`libFHErt_phantom.so`)
- Compiled FHE kernel (auto-cached by the compile step)
