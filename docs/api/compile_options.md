# Compile Options

Compile options control FHE compilation parameters. Pass them as keyword arguments to `@fhe.compile()` or `@fhe.compute()`.

## Quick Reference

```python
@fhe.compile(
    frontend="torch",
    library="phantom",
    device="cuda",
    ckks={"N": 4096, "q0": 60, "sf": 56},
    vec={"ms": 256},
    p2c={"fp": True},
    encrypt_inputs=["x", "y"],
)
```

## CKKS Parameters (`ckks`)

Controls the CKKS encryption scheme parameters. Maps to `-CKKS:` CLI flags.

| Key | Type | Description |
|-----|------|-------------|
| `N` | `int` | Polynomial modulus degree (power of 2, e.g. 4096, 8192, 65536) |
| `q0` | `int` | First modulus bit size (e.g. 60) |
| `sf` | `int` | Scaling factor bit size (e.g. 40, 56) |
| `hw` | `int` | Hamming weight for secret key (e.g. 192) |
| `num_q_parts` | `int` | Number of Q partitions |

**Note:** GPU backends (`phantom`, `acelib`) default to `N=65536` if not specified.

## Vectorization (`vec`)

Controls vectorization/tiling options. Maps to `-VEC:` CLI flags.

| Key | Type | Description |
|-----|------|-------------|
| `ms` | `int` | Minimum vectorization size |
| `conv_parl` | `bool` | Enable convolution parallelization |

## SIHE Options (`sihe`)

Controls SIHE-level options. Maps to `-SIHE:` CLI flags.

| Key | Type | Description |
|-----|------|-------------|
| `relu_vr_def` | `float` | Default ReLU value range |
| `relu_vr` | `str` | Per-op ReLU value range (semicolon-separated) |

## Poly-to-C Options (`p2c`)

Controls code generation options. Maps to `-P2C:` CLI flags.

| Key | Type | Description |
|-----|------|-------------|
| `fp` | `bool` | Enable floating-point code generation |
| `lib` | `str` | Target library provider (e.g. `"phantom"`, `"hyperfhe"`) |

## Polynomial Options (`poly`)

Controls polynomial-level options. Maps to `-POLY:` CLI flags.

| Key | Type | Description |
|-----|------|-------------|
| `rtt` | `bool` | Enable RTT optimization |
| `ts` | `bool` | Enable term scheduling |

## ReLU Value Range

Three ways to specify ReLU value range (priority from high to low):

1. **`relu_vr_data`** — Explicit dict: `{"relu_0": 4.0, "relu_1": 3.5}`
2. **`relu_vr_file`** — Path to JSON profile file
3. **`profile_relu=True`** — Auto-profile at compile time (less reliable for deep networks)

## Encryption Control

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `encrypt_inputs` | `List[str]` or `List[int]` | `None` | Which inputs to encrypt (by name or index). `None` = all |

## FHE Config

Default encryption configuration (used when no `ckks` dict is provided):

| Field | Default | Description |
|-------|---------|-------------|
| `scheme` | `"CKKS"` | Encryption scheme |
| `poly_modulus_degree` | `8192` | Polynomial modulus degree |
| `multiplication_depth` | `2` | Maximum multiplication depth |
| `backend` | `"CPU"` | Target backend |

## Environment Variable Override

Set `ACE_COMPILE_OPTIONS` as a JSON string to override any compile option (highest priority):

```bash
ACE_COMPILE_OPTIONS='{"ckks": {"N": 4096}}' python script.py
```

## Cache Control

```python
from ace import fhe

fhe.configure_cache(force_rebuild=True)   # Force recompilation
fhe.configure_cache(cache_dir="/path")     # Custom cache directory
```

Cache is keyed on: model identity + input shapes/dtypes + compile options hash + ReLU VR hash.