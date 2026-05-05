# ANT-ACE: A Unified FHE Framework for Privacy-Preserving Computation

ANT-ACE is a developer-friendly, high-performance framework that enables you to run arbitrary Python programs on encrypted data—without ever decrypting it. Built on battle-tested FHE libraries (Antlib, hyperFHE, Openfhe, Seal), ANT-ACE provides a seamless path from Python code to optimized homomorphic execution.

## 🌟 Key Features

✨ Write Python, Run Encrypted

```
import torch
from ace_tool import fhe

# user input
@fhe.compute({"x": "encrypted", "y": "encrypted"})
def add(x, y):
    return x + y

# example input
input0 = torch.ones(1, 1, 3, 3) 
input1 = torch.ones(1, 1, 3, 3) 
inputset = [input0, input1]

# Auto run with FHE
result = add(inputset)
print(f"Inference Result :: {result}")
```

## 🧩 Three-Layer Architecture

|Layer | Purpose     |Inspired by      |
|-------|-----------|----------|
|Application  |No-code APIs, SaaS endpoints|Fairmath FHE Computer|
|Developer SDK  |@fhe.compute,@fhe.compile, @fhe.operator|ACE Compiler|
|Core Runtime    |CPU/GPU-accelerated Antlib + hyperfhe|Antlib, hyperfhe, OpenFHE, SEAL|

🚀 Production-Ready
GPU acceleration: 10–15× speedup with CUDA (via OpenFHE)
Client-server separation: Keys never leave the client
Chrome Trace profiling: Pinpoint bottlenecks in Python & C++
Multi-scheme support: CKKS (for ML), TFHE (for Boolean circuits) (TODO)

## 📦 Installation

From PyPI (CPU-only)
```
pip install ant-ace
```

With GPU Support
```
# Requires NVIDIA driver ≥ 530.xx
pip install ant-ace[gpu]
```

From Source
```
git clone https://github.com/ant-research/ant-fhe.git
cd ant-fhe
pip install -e ".[dev,gpu]"
```

💡 Note: GPU support requires CUDA 11.8+ and compatible NVIDIA drivers.


## 🚀 Quick Start

1. Define your private function
```
# scoring.py
from fhe import fhe
import numpy as np

@fhe.compute
def encrypted_model(input_data: np.ndarray) -> int:
    # Supports NumPy operations
    hidden = np.dot(input_data, weights) + bias
    return int(np.argmax(hidden))
```

2. Run locally (simulation mode)

```
from fhe import set_simulation_mode
set_simulation_mode(True)  # No real encryption

result = encrypted_model(np.array([1.0, 2.0, 3.0]))
print(result)  # → 2 (plaintext result)
```

3. Deploy to production

```
# Client side
client = FHEClient("https://fhe-api.yourcompany.com")
encrypted_input = client.encrypt([1.0, 2.0, 3.0])
job = client.submit("encrypted_model", encrypted_input)
result = client.decrypt(job.result())  # → 2
```

### 🏗️ Architecture Overview

```
User Python Code
       ↓
[FHE Decorators] → @fhe.compile / @fhe.compute / @fhe.operator
       ↓
[Python IR]      → AST-based computation graph
       ↓
[AIR]            → Abstract Intermediate Representation (scheme-agnostic)
       ↓
[Code Generator] → Optimized C++ (OpenFHE) / Rust (TFHE-rs)
       ↓
[Runtime]        → CPU/GPU execution with client-side key management
```

Unlike pure research libraries, FHE Stack:

Hides cryptographic complexity behind intuitive decorators
Optimizes automatically (noise budgeting, relinearization)
Scales to production with client-server deployment patterns

## 📊 Performance

|Operation | CPU (Intel i9)     |GPU (A100)   | Speedup |
|-------|-----------|----------|----------|
|CKKS Vector Multiply  |120 ms|8 ms|15×|
|TFHE Boolean Circuit |45 ms|N/A| - |
|Full ML Inference   |2.1 s|0.15 s| 14x|

Benchmarks run on ResNet-20 inference with encrypted inputs.
See benchmarks/ for details.

## 🤝 Comparison with Alternatives


|Feature | ANT-ACE     |Zama Concrete  | Microsoft SEAL |
|-------|-----------|----------|----------|
|Python decorators |✅	|✅	| ❌ |
|GPU acceleration |✅(hyperfhe)| ❌ | ❌ |
|Client-server model  |	✅	|	⚠️ Limited |❌ |
|Multi-scheme  |	✅ (CKKS/TFHE)	|	✅ (TFHE) | ✅ (BGV/CKKS) |
|Chrome Trace profiling  |	✅ |	❌ | ❌ |
|Production deployment |	⚠️ Research-focused	 |	⚠️ Research-focused	| ⚠️ Library-only |

## 📚 Documentation & Resources
- Getting Started Guide
- API Reference
- Tutorials:
  - Private Machine Learning
  - Encrypted Database Queries
  - Secure Multi-Party Computation
- Examples Repository

## 🛠️ Development
We welcome contributions! Please see our Contributing Guide.

### Local Setup
```
git clone https://github.com/ant-research/ant-fhe.git
cd ant-fhe
pip install -e ".[dev]"
pre-commit install  # Enforces code style
```

### Run Tests
```
pytest tests/  # Unit tests
pytest integration/  # End-to-end tests
```

## 📜 License
This project is licensed under the Apache License 2.0 – see the LICENSE file for details.

Note: FHE Stack bundles cryptographic libraries (OpenFHE, TFHE-rs) with their own licenses (BSD-3-Clause, Apache 2.0).

## 🙏 Acknowledgements
Inspired by Zama's Concrete and Microsoft SEAL
Built on OpenFHE for high-performance FHE
Uses PyTorch Profiler for performance analysis
FHE Stack: Because privacy shouldn’t require a PhD in cryptography.
Star us on GitHub ⭐ if you find this useful!